CC=gcc

all: libhelpers.so libbufio.so

libhelpers.so: helpers.c	
	$(CC) -shared -o $@ -fPIC $^
libbufio.so: bufio.c
	$(CC) -shared -o $@ -fPIC $^
clean:
	rm *.so

