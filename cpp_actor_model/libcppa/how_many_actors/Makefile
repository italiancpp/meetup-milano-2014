CC=g++
CFLAGS=-c -Wall -std=c++11 -ggdb -o0
LDFLAGS=-lcppa -pthread
SOURCES=how_many_actors.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=how_many_actors

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(EXECUTABLE) how_many_actors_task

how_many_actors_task: how_many_actors_task.o
	$(CC) -o $@ how_many_actors_task.o $(LDFLAGS)
