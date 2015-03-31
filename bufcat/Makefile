CC=gcc
FLAGS=-L../lib -I../lib -Wl,-rpath=../lib
TARGET=bufcat

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(FLAGS) bufcat.c -lbufio -o $@

clean:
	rm -f $(TARGET) 
