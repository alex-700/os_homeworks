CC=gcc
FLAGS=-std=c99 -L../lib -I../lib -Wl,-rpath=../lib
TARGET=revwords

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(FLAGS) revwords.c -lhelpers -o $@

clean:
	rm -f $(TARGET) 
