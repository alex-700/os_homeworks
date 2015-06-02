CC=gcc
FLAGS=-Wall -std=c11 -L../lib -I../lib -Wl,-rpath=../lib
TARGET=forking

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(FLAGS) $^ -lbufio -o $@

clean:
	rm -f $(TARGET) 
