CC=clang++
CFLAGS=-I/usr/include/python2.7 -I/usr/include/x86_64-linux-gnu/python2.7 -std=c++14 -g -c -fPIC -Wall -Wextra -O3
LDFLAGS=`icu-config --ldflags` -licuio -lpython2.7 
SOURCES=regexpparser.cpp transducer.cpp main.cpp intervalunion.cpp io.cpp extern.cpp icu.cpp transduceroptimizer.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=editdistance

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
	make dll

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)

dll: $(OBJECTS)
	$(CC) -shared -Wl,-soname,libtrans.so -o libtrans.so $(OBJECTS) $(LDFLAGS)
