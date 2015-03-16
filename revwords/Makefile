CC=gcc
FLAGS=-std=c99 -L../lib -I../lib
TARGET=revwords

all: $(TARGET)

$(TARGET): revwords.c
	$(CC) $(FLAGS) revwords.c -lhelpers -o $@

clean:
	rm -f $(TARGET) 
