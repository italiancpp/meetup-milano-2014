Erlang Example
==============
You need an [erlang](http://www.erlang.org/) VM, I used R16B02.

To execute the example, run the console with erl command in example directory.

In shell execute:

    c(worker).

    Pid = worker:start().
    Pid ! {self(), "lavora", 5000}.

    Pid ! "work!!!!".

Now the actor Pid doesn't respond again.

Change the code, add work() to "_" pattern matching and now it responds also after wrong command!!!

Use 

    f(). 

to clean the console variables and repeat all commands from the beginning.

References:
[http://learnyousomeerlang.com/the-hitchhikers-guide-to-concurrency](http://learnyousomeerlang.com/the-hitchhikers-guide-to-concurrency)
