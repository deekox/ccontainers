CC = gcc
CFLAGS = -Wall
DEBUGFLAGS =  -g3 -O0 -gdwarf-2
CFLAGS += $(DEBUGFLAGS)
SDIR = src
ODIR = obj
TARGET = test
_OBJS = btree.o test.o list.o common.o #avltree.o common.o
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))


.PHONY : clean rebuild directories

all : directories $(TARGET)


$(TARGET) : $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(ODIR)/btree.o : $(SDIR)/btree.c $(SDIR)/btree.h
	$(CC) $(CFLAGS) -c -o $@ $<

# $(ODIR)/avltree.o : $(SDIR)/avltree.c $(SDIR)/avltree.h
# 	$(CC) $(CFLAGS) -c -o $@ $<

$(ODIR)/list.o : $(SDIR)/list.c $(SDIR)/list.h
	$(CC) $(CFLAGS) -c -o $@ $<	


clean :
	rm -rf $(OBJS) $(TARGET)

rebuild: clean all

directories:
	mkdir -p $(ODIR)
