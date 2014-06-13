
var http = require('http');
var url = require('url');

http.createServer(function (req, res) {
  var url_parts = url.parse(req.url, true);
  var query = url_parts.query;
  // console.log(url_parts);
  // console.log(query);

  if (query.block){
    while(true){
    }
  }
  if (query.sleepFor){
    var start = process.hrtime()[0];
    while((process.hrtime()[0] - start) < parseInt(query.sleepFor)){
    }
  }

  res.writeHead(200, {'Content-Type': 'text/plain'});
  res.end('Hello ++It!!\n');
}).listen(1337, "127.0.0.1");
console.log('Server running at http://127.0.0.1:1337/');
