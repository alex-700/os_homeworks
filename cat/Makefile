CC=gcc
FLAGS=-L../lib -I../lib -Wl,-rpath=../lib
TARGET=cat

all: $(TARGET)

$(TARGET):
	$(CC) $(FLAGS) cat.c -lhelpers -o $@

clean:
	rm -f $(TARGET) 
