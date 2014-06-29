/***
* ==++==
*
* Copyright (c) Microsoft Corporation. All rights reserved. 
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* ==--==
* =+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
*
* searchfile.cpp - Simple cmd line application that uses a variety of streams features to search a file,
*      store the results, and write results back to a new file.
*
* =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-
****/

#include "cpprest/filestream.h"
#include "cpprest/containerstream.h"
#include "cpprest/producerconsumerstream.h"

#pragma warning (disable : 4127)
#pragma warning (disable : 4457)
#pragma warning (disable : 4389)
#include <pplawait.h>

using namespace utility;
using namespace concurrency::streams;

/// <summary>
/// A convenient helper function to loop asychronously until a condition is met.
/// </summary>
pplx::task<bool> _do_while_iteration(std::function<pplx::task<bool>(void)> func)
{
    pplx::task_completion_event<bool> ev;
    func().then([=](bool guard)
    {
        ev.set(guard);
    });
    return pplx::create_task(ev);
}
pplx::task<bool> _do_while_impl(std::function<pplx::task<bool>(void)> func)
{
    return _do_while_iteration(func).then([=](bool guard) -> pplx::task<bool>
    {
        if(guard)
        {
            return ::_do_while_impl(func);
        }
        else
        {
            return pplx::task_from_result(false);
        }
    });
}
pplx::task<void> do_while(std::function<pplx::task<bool>(void)> func)
{
    return _do_while_impl(func).then([](bool){});
}

/// <summary>
/// Structure used to store individual line results.
/// </summary>
typedef std::vector<std::string> matched_lines;
namespace Concurrency { namespace streams {
/// <summary>
/// Parser implementation for 'matched_lines' type.
/// </summary>

template <typename CharType>
class type_parser<CharType, matched_lines>
{
public:
    static pplx::task<matched_lines> parse(streambuf<CharType> buffer)
    {
        basic_istream<CharType> in(buffer);
        auto lines = std::make_shared<matched_lines>();
        return do_while([=]()
        {
            container_buffer<std::string> line;
            return in.read_line(line).then([=](const size_t bytesRead)
            {
                if(bytesRead == 0 && in.is_eof())
                {
                    return false;
                }
                else
                {
                    lines->push_back(std::move(line.collection()));
                    return true;
                }
            });
        }).then([=]()
        {
            return matched_lines(std::move(*lines));
        });
    }
};

}}
/// <summary>
/// Function to create in data from a file and search for a given string writing all lines containing the string to memory_buffer.
/// </summary>
static pplx::task<void> find_matches_in_file(const string_t fileName, const std::string searchString, basic_ostream<char> results) __resumable
{
    basic_istream<char> inFile = __await file_stream<char>::open_istream(fileName);
 
    int lineNumber = 1;
    size_t bytesRead = 0;
    do {
        container_buffer<std::string> inLine;
        bytesRead = __await inFile.read_line(inLine);

        if (inLine.collection().find(searchString) != std::string::npos)
        {
            __await results.print("line ");
            __await results.print(lineNumber++);
            __await results.print(":");

            container_buffer<std::string> outLine(std::move(inLine.collection()));
            __await results.write(outLine, outLine.collection().size());
            __await results.print("\r\n");
        }
        else
        {
            lineNumber++;
        }
    } while (bytesRead > 0 || !inFile.is_eof());

    // Close the file and results stream.
    __await inFile.close();
    __await results.close();
}

/// <summary>
/// Function to write out results from matched_lines type to file
/// </summary>
static pplx::task<void> write_matches_to_file(const string_t fileName, matched_lines results) __resumable
{
    // Create a shared pointer to the matched_lines structure to copying repeatedly.
    auto sharedResults = std::make_shared<matched_lines>(std::move(results));

    basic_ostream<char> outFile = __await file_stream<char>::open_ostream(fileName, std::ios::trunc);

    auto currentIndex = std::make_shared<size_t>(0);
    while (true)
    {
        if (*currentIndex >= sharedResults->size()) {
            break;
        }

        container_buffer<std::string> lineData((*sharedResults)[(*currentIndex)++]);
        __await outFile.write(lineData, lineData.collection().size());
        __await outFile.print("\r\n");
    }
    
    __await outFile.close();
}

pplx::task<void> run(const string_t inFileName, const std::string searchString, const string_t outFileName) __resumable
{
    producer_consumer_buffer<char> lineResultsBuffer;

    // Find all matches in file.
    basic_ostream<char> outLineResults(lineResultsBuffer);
    __await find_matches_in_file(inFileName, searchString, outLineResults);

    // Write matches into custom data structure.
    basic_istream<char> inLineResults(lineResultsBuffer);
    matched_lines lines = __await inLineResults.extract<matched_lines>();

    // Write out stored match data to a new file.
    __await write_matches_to_file(outFileName, std::move(lines));

    // Wait for everything to complete.
}

#ifdef _MS_WINDOWS
int wmain(int argc, wchar_t *args[])
#else
int main(int argc, char *args[])
#endif
{
    if(argc != 4)
    {
        printf("Usage: SearchFile.exe input_file search_string output_file\n");
        return -1;
    }
    const string_t inFileName = args[1];
    const std::string searchString = utility::conversions::to_utf8string(args[2]);
    const string_t outFileName = args[3];
    run(inFileName, searchString, outFileName).wait();

    return 0;
}