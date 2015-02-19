CC=clang++
CFLAGS=-I/usr/include/python2.7 -I/usr/include/x86_64-linux-gnu/python2.7 -std=c++11 -g -c -fPIC -Wall -Wextra
LDFLAGS=`icu-config --ldflags` -licuio -lpython2.7 
SOURCES=regexpparser.cpp transducer.cpp main.cpp intervalunion.cpp io.cpp extern.cpp icu.cpp
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
	$(CC) -shared -Wl,-soname,libtrans.so -o libtrans.so $(OBJECTS) $(LDFLAGS)
