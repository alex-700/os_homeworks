CC=gcc
FLAGS=-L../lib -I../lib
TARGET=cat

all: $(TARGET)

$(TARGET):
	$(CC) $(FLAGS) cat.c -lhelpers -o $@

clean:
	rm -f $(TARGET) 
