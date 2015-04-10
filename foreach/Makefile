CC=gcc
FLAGS=-std=c99 -L../lib -I../lib -Wl,-rpath=../lib
TARGET=foreach

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(FLAGS) $^ -lhelpers -lbufio -o $@

clean:
	rm -f $(TARGET) 
