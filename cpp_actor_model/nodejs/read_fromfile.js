var fs = require("fs");

console.log("Start reading");

fs.readFile('/etc/passwd', function (err, data) {
  if (err) throw err;
  console.log("-> " + data + " <-");
});

console.log("----------------------------------------------------------------");

console.log("start open");

var stream = fs.createReadStream('/etc/passwd', { 
    flags: 'r', 
    encoding: null, 
    fd: null,  
    mode: 0666,
    autoClose: true 
});

stream.on('open', function(fd) {
  console.log(">> Stream open!!! <<");
});

stream.on('data', function(chunk) {
  console.log('>> got %d bytes of data', chunk.length);
  console.log(chunk.toString() + " <<");
})
stream.on('end', function() {
  console.log('>> there will be no more data. <<');
});


console.log("BEFORE END!!!");


