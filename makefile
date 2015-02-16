CC=clang++
CFLAGS=-std=c++11 -g -c -fPIC -Wall -Wextra
LDFLAGS=
SOURCES=regexpparser.cpp transducer.cpp main.cpp intervalunion.cpp io.cpp extern.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=editdistance

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

dll: $(OBJECTS)
	$(CC) -shared -Wl,-soname,libtrans.so -o libtrans.so $(OBJECTS) extern.o
