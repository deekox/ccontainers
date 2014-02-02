CFLAGS = -Wall
DEBUGFLAGS =  -g3 -O0 -gdwarf-2
CFLAGS += $(DEBUGFLAGS)
TARGET = test
OBJECTS = btree.o test.o list.o
CC = gcc

.PHONY : clean rebuild

all : $(TARGET)

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

test.o : test.c
	$(CC) $(CFLAGS) -c -o $@ $<

btree.o : btree.c btree.h
	$(CC) $(CFLAGS) -c -o $@ $<

list.o : list.c list.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean :
	rm -rf $(OBJECTS) $(TARGET)

rebuild: clean all

