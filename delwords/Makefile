CC=gcc
FLAGS=-std=c99 -L../lib -I../lib -Wl,-rpath=../lib
TARGET=delwords

all: $(TARGET)

$(TARGET): delwords.c
	$(CC) $(FLAGS) delwords.c -lhelpers -o $@

clean:
	rm -f $(TARGET) 
