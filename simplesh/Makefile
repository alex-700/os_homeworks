CC=gcc
FLAGS=-std=c11 -L../lib -I../lib -Wl,-rpath=../lib
TARGET=simplesh

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(FLAGS) $^ -lbufio -lhelpers -o $@

clean:
	rm -f $(TARGET) 
