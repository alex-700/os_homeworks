CC=gcc
FLAGS=-g -Wall -std=c11 -L../lib -I../lib -Wl,-rpath=../lib
TARGET1=forking
TARGET2=polling

all: $(TARGET1) $(TARGET2)

$(TARGET1): $(TARGET1).c
	$(CC) $(FLAGS) $^ -lbufio -o $@

$(TARGET2): $(TARGET2).c
	$(CC) $(FLAGS) $^ -lbufio -lhelpers -o $@
clean:
	rm -f $(TARGET1) $(TARGET2)
