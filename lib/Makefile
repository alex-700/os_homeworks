CC=gcc
CFLAGS=-c -Wall -fPIC
LDFLAGS=-shared -o
SOURCE=helpers.c
OBJECT=helpers.o
LIBRARY=libhelpers.so

all: $(LIBRARY)

$(LIBRARY): $(OBJECT)
	$(CC) $(LDFLAGS) $(LIBRARY) $(OBJECT)

.c.o:
	$(CC) $(CFLAGS) $(SOURCE) 

clean:
	rm -rf *.o $(LIBRARY)
