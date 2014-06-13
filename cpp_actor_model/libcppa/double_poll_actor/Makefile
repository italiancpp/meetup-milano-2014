CC=g++
CFLAGS=-c -Wall -std=c++11 -ggdb -o0
LDFLAGS=-lcppa
SOURCES=double_poll_actor.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=double_poll_actor

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(EXECUTABLE)

run: $(EXECUTABLE)
	./$(EXECUTABLE)