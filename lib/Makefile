CC=gcc

all: libhelpers.so libbufio.so

libhelpers.so: helpers.c	
	$(CC) -Wall -std=c11 -shared -o $@ -fPIC $^
libbufio.so: bufio.c
	$(CC) -shared -o $@ -fPIC $^
clean:
	rm *.so

