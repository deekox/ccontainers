# TODO: mkdir $(ODIR) $(OTEST_DIR)

CC = clang
CFLAGS = -Wall -Wno-unused-function
DEBUGFLAGS =  -g3 -O0 -gdwarf-2
#CFLAGS += $(DEBUGFLAGS)
SDIR = src
IDIR = include
ODIR = obj
TEST_DIR_NAME = tests
TEST_TARGET = test
STEST_DIR = $(SDIR)/$(TEST_DIR_NAME)
OTEST_DIR = $(ODIR)/$(TEST_DIR_NAME)
TARGET = libcont.a

_TEST_OBJS = avltest.o test.o
_OBJS = common.o btree.o list.o avltree.o 
OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))
TEST_OBJS = $(patsubst %,$(OTEST_DIR)/%,$(_TEST_OBJS))

.PHONY : clean rebuild dirtree

all : dirtree $(TARGET) $(TEST_TARGET) 


$(TARGET) : $(OBJS) 
	ar rcs $(TARGET) $(OBJS)

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -I $(IDIR) -c -o $@ $<

$(ODIR)/btree.o : $(SDIR)/btree.c $(IDIR)/btree.h 
	$(CC) $(CFLAGS) -c -o $@ $< -I $(IDIR)

$(ODIR)/avltree.o : $(SDIR)/avltree.c $(IDIR)/avltree.h
	$(CC) $(CFLAGS) -c -o $@ $< -I $(IDIR)

$(ODIR)/list.o : $(SDIR)/list.c $(IDIR)/list.h 
	$(CC) $(CFLAGS) -c -o $@ $<	 -I $(IDIR)


$(TEST_TARGET) : $(TARGET) $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $(TEST_OBJS) $(TARGET)

$(OTEST_DIR)/%.o : $(STEST_DIR)/%.c
	$(CC) $(CFLAGS) -I $(IDIR) -c -o $@ $<

clean :
	rm -rf $(OBJS) $(TARGET) $(TEST_OBJS) $(TEST_TARGET) $(ODIR) $(OTEST_DIR)

rebuild: clean all

dirtree :  
	@mkdir -p $(ODIR) $(OTEST_DIR)
