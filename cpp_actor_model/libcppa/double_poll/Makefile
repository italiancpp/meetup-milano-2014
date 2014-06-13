CC=g++
CFLAGS=-c -Wall -std=c++11 -ggdb -o0
LDFLAGS=-pthread
SOURCES=double_poll.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=double_poll

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o $(EXECUTABLE)