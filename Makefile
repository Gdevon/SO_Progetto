CC=gcc
CCOPTS=--std=gnu99 -Wall -g 
AR = ar
OBJS=test.o DIR_Entry.o  Errors.o  FAT_info.o  FS_info.o  FileHandle.o  linked_list.o ListHandle.o DirHandle.o
HEADERS=FileHandle.h FAT_info.h DIR_Entry.h DirHandle.h FS_info.h Errors.h linked_list.h ListHandle.h DirHandle.h
LIBS=libtest.a 
BINS = test 
.phony: clean all
all: $(BINS) $(LIBS)
%.o: %.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@ $<
libtest.a:	$(OBJS)
	$(AR) -rcs $@ $^
test: test.c $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^
clean:
	@echo "rimuovo i .o"
		rm -rf *.o *.fs *~ $(LIBS) $(BINS)