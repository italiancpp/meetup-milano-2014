#include "stdafx.h"
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <future>
#include <thread>
#include <pplawait.h>
using namespace std;

////////////////////////////////////////////////////////////////////////////////

// Copies a file with normal, blocking I/O.

vector<char> readFile(const string& inPath)
{
    ifstream file(inPath, ios::binary | ios::ate);
    size_t length = (size_t)file.tellg();
    vector<char> buffer(length);
    file.seekg(0, std::ios::beg);
    file.read(&buffer[0], length);
    return buffer;
}

size_t writeFile(const vector<char>& buffer, const string& outPath)
{
    ofstream file(outPath, ios::binary);
    file.write(&buffer[0], buffer.size());
    return (size_t) file.tellp();
}

size_t sync_copyFile(const string& inFile, const string& outFile)
{
    return writeFile(readFile(inFile), outFile);
}

////////////////////////////////////////////////////////////////////////////////

// Copies a file in a separate thread.

std::thread thread_copyFile(const string& inFile, const string& outFile)
{
    std::thread t([=]{
        writeFile(readFile(inFile), outFile);
    });
    return t;
}

////////////////////////////////////////////////////////////////////////////////

// Copies a file using two futures and promises and spawning two threads.

size_t future_copyFile(const string& inFile, const string& outFile)
{
    std::promise<vector<char>> prom1;
    std::future<vector<char>> fut1 = prom1.get_future();
    std::thread th1([&prom1, inFile](){
        prom1.set_value(readFile(inFile));
    });

    std::promise<int> prom2;
    std::future<int> fut2 = prom2.get_future();
    std::thread th2([&fut1, &prom2, outFile](){
        prom2.set_value(writeFile(fut1.get(), outFile));
    });

    size_t result = fut2.get();
    th1.join();
    th2.join();
    return result;
}

////////////////////////////////////////////////////////////////////////////////

// Copies a file using two packaged tasks and spawning two threads.

size_t packagedtask_copyFile(const string& inFile, const string& outFile)
{
    using Task_Type_Read = vector<char>(const string&);
    using Task_Type_Write = size_t(const string&);

    packaged_task<Task_Type_Read> pt1(readFile);
    future<vector<char>> fut1{ pt1.get_future() };
    thread th1{ move(pt1), inFile };

    packaged_task<Task_Type_Write> pt2([&fut1](const string& path){
        return writeFile(fut1.get(), path);
    });
    future<size_t> fut2{ pt2.get_future() };
    thread th2{ move(pt2), outFile };

    size_t result = fut2.get();
    th1.join();
    th2.join();
    return result;
}

////////////////////////////////////////////////////////////////////////////////

// Copies a file using std::async.

size_t async_copyFile(const string& inFile, const string& outFile)
{
    auto fut1 = async(readFile, inFile);
    auto fut2 = async([&fut1](const string& path){
        return writeFile(fut1.get(), path);
    },
        outFile);
    return fut2.get();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Copies a file using the PPL (but blocking on read and write).

size_t ppl_copyFile(const string& inFile, const string& outFile)
{
    Concurrency::task<vector<char>> fut1 = Concurrency::create_task([inFile]() {
        return readFile(inFile);
    });
    Concurrency::task<size_t> fut2 = Concurrency::create_task([&fut1, outFile]() {
        return writeFile(fut1.get(), outFile);
    });
    return fut2.get();
}

////////////////////////////////////////////////////////////////////////////////

// Copies a file using the PPL and task continuations.

size_t ppl_then_copyFile(const string& inFile, const string& outFile)
{
    Concurrency::task<size_t> result =
        Concurrency::create_task([inFile]() {
        return readFile(inFile);
    }).then([outFile](const vector<char>& buffer) {
        return writeFile(buffer, outFile);
    });

    return result.get();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Copies a file using VS CTP resumable functions.

Concurrency::task<void> r_readFile(shared_ptr<vector<char>> buffer, const string inPath) __resumable
{
    ifstream file(inPath, ios::binary | ios::ate);
    size_t length = (size_t) file.tellg();
    buffer->resize(length);
    file.seekg(0, std::ios::beg);
    file.read(&buffer->at(0), length);
}

Concurrency::task<size_t> r_writeFile(shared_ptr<vector<char>> buffer, const string outPath) __resumable
{
    ofstream file(outPath, ios::binary);
    file.write(&buffer->at(0), buffer->size());
    return (size_t) file.tellp();
}

Concurrency::task<size_t> resumable_copyFile(const string inFile, const string outFile) __resumable
{
    shared_ptr<vector<char>> buffer = make_shared<vector<char>>();
    __await r_readFile(buffer, inFile);
    size_t result = __await r_writeFile(buffer, outFile);
    return result;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Copies a file chunk by chunk using PPL tasks in a loop, but blocking for
// each read and write

Concurrency::task<string> readFileChunk(ifstream& file, int chunkLength)
{
    return Concurrency::create_task([&file, chunkLength](){
        vector<char> buffer(chunkLength);
        file.read(&buffer[0], chunkLength);
        return string(&buffer[0], (unsigned int) file.gcount());
    });
}

Concurrency::task<void> writeFileChunk(ofstream& file, const string& chunk)
{
    return Concurrency::create_task([&file, chunk](){
        file.write(chunk.c_str(), chunk.length());
    });
}

void copyFile_ppl_loop_blocking(const string& inFilePath, const string& outFilePath)
{
    ifstream inFile(inFilePath, ios::binary | ios::ate);
    inFile.seekg(0, inFile.beg);
    ofstream outFile(outFilePath, ios::binary);

    string chunk;
    while (chunk = readFileChunk(inFile, 4096).get(), !chunk.empty()) {
        writeFileChunk(outFile, chunk).get();
    }
}

////////////////////////////////////////////////////////////////////////////////

// Copies a file chunk by chunk using PPL tasks in a loop, with continuations.

Concurrency::task<shared_ptr<string>> readFileChunk(shared_ptr<ifstream> file, int chunkLength)
{
    return Concurrency::create_task([file, chunkLength](){
        vector<char> buffer(chunkLength);
        file->read(&buffer[0], chunkLength);
        return make_shared<string>(&buffer[0], (unsigned int) file->gcount());
    });
}

Concurrency::task<bool> writeFileChunk(shared_ptr<ofstream> file, shared_ptr<string> chunk)
{
    return Concurrency::create_task([file, chunk]() {
        file->write(chunk->c_str(), chunk->length());
        return chunk->length() == 0;
    });
}

Concurrency::task<void> copyFile_repeat(shared_ptr<ifstream> inFile, shared_ptr<ofstream> outFile)
{
    return readFileChunk(inFile, 4096)
        .then([=](shared_ptr<string> chunk) {
            return writeFileChunk(outFile, chunk);
        })
        .then([=](bool eof) {
            if (!eof) {
                return copyFile_repeat(inFile, outFile);
            }
            else {
                return Concurrency::task_from_result();
            }
        });
}

Concurrency::task<void> copyFile_ppl_loop_then(const string& inFilePath, const string& outFilePath)
{
    auto inFile = make_shared<ifstream>(inFilePath, ios::binary | ios::ate);
    inFile->seekg(0, inFile->beg);
    auto outFile = make_shared<ofstream>(outFilePath, ios::binary);

    return copyFile_repeat(inFile, outFile);
}

////////////////////////////////////////////////////////////////////////////////

// Copies a file chunk by chunk using a resumable function.

Concurrency::task<string> _readFileChunk(shared_ptr<ifstream> file, int chunkLength)
{
    return Concurrency::create_task([file, chunkLength](){
        vector<char> buffer(chunkLength);
        file->read(&buffer[0], chunkLength);
        return string(&buffer[0], (unsigned int) file->gcount());
    });
}

Concurrency::task<bool> _writeFileChunk(shared_ptr<ofstream> file, string chunk)
{
    return Concurrency::create_task([file, chunk]() {
        file->write(chunk.c_str(), chunk.length());
        return chunk.length() == 0;
    });
}

Concurrency::task<void> copyFile_resumable(const string inFilePath, const string outFilePath) __resumable
{
    auto inFile = make_shared<ifstream>(inFilePath, ios::binary | ios::ate);
    inFile->seekg(0, inFile->beg);
    auto outFile = make_shared<ofstream>(outFilePath, ios::binary);

    while (true)
    {
        string s = __await _readFileChunk(inFile, 4096);
        if (s.empty()) {
            break;
        }
        __await _writeFileChunk(outFile, s);
    }
}

////////////////////////////////////////////////////////////////////////////////

int _tmain(int argc, _TCHAR* argv[])
{
    if (argc != 2) {
        cout << "usage: TestCopyfile <filePath>" << endl;
        return -1;
    }
    string inFile = argv[1];
    
    sync_copyFile(inFile, inFile + ".copy.0");

    thread_copyFile(inFile, inFile + ".copy.1").join();

    future_copyFile(inFile, inFile + ".copy.2");

    packagedtask_copyFile(inFile, inFile + ".copy.3");

    async_copyFile(inFile, inFile + ".copy.4");

    ppl_copyFile(inFile, inFile + ".copy.5");

    ppl_then_copyFile(inFile, inFile + ".copy.6");

    resumable_copyFile(inFile, inFile + ".copy.7").get();

    copyFile_ppl_loop_blocking(inFile, inFile + ".copy.8");

    copyFile_ppl_loop_then(inFile, inFile + ".copy.9").get();
    
    copyFile_resumable(inFile, inFile + ".copy.r").get();

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
