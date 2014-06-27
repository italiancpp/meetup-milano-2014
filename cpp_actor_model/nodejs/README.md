Node examples
====
I you installed node by [nvm](https://github.com/creationix/nvm) activate node with `node use`

* read_fromfile.js
Read the file /etc/passwod in async way

* server.js
A simple http server

* server_block.js
A simple http server that can be blocked calling `http://127.0.0.1:1337/?block=true` from one browser e `http://127.0.0.1:1337/` from another. The first request blocks the later.
You can also set a delay calling `http://127.0.0.1:1337/?sleepFor=5` to wait 5 seconds before the request returns.
