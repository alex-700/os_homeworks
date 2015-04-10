CC=gcc
FLAGS=-std=c99 -L../lib -I../lib -Wl,-rpath=../lib
TARGET=foreach

all: $(TARGET)

$(TARGET): $(TARGET).c	../lib/libbufio.so ../lib/libhelpers.so
	$(CC) $(FLAGS) $^ -lhelpers -lbufio -o $@

clean:
	rm -f $(TARGET) 
