CC=gcc
FLAGS=-std=c99 -L../lib -I../lib -Wl,-rpath=../lib
TARGET=filter

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(FLAGS) $^ -lhelpers -o $@

clean:
	rm -f $(TARGET) 
