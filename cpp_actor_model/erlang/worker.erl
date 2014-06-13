-module(worker).
-compile(export_all).

start() ->
	spawn(?MODULE, work, []).

work() ->
    receive
        {From, Name, Milliseconds} ->
            io:format("Starting work for ~p - for ~p ms.~n", [Name, Milliseconds]),	
            timer:sleep(Milliseconds),
            io:format("Work done, send result.~n"),	
            From ! "Work Done",
            work();
        _ ->
            io:format("Whatt??????.~n")
    end.