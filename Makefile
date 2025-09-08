CC=gcc
CCOPTS=--std=gnu99 -Wall -g 
AR = ar
OBJS=DIR_Entry.o  Errors.o  FAT_info.o  FS_info.o  FileHandle.o  linked_list.o ListHandle.o DirHandle.o shell.o run.o #test2.o test.o
HEADERS=FileHandle.h FAT_info.h DIR_Entry.h DirHandle.h FS_info.h Errors.h linked_list.h ListHandle.h DirHandle.h shell.h Extern_fs.h Colors.h
LIBS=libtest.a 
BINS = run shell #test test2
.phony: clean all
all: $(BINS) $(LIBS)
%.o: %.c $(HEADERS)
	$(CC) $(CCOPTS) -c -o $@ $<
libtest.a:	$(OBJS)
	$(AR) -rcs $@ $^
#test: test.c $(LIBS)
#	$(CC) $(CCOPTS) -o $@ $^
#test2: test2.c $(LIBS)
#	$(CC) $(CCOPTS) -o $@ $^
shell: shell.c $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^
run: run.c $(LIBS)
	$(CC) $(CCOPTS) -o $@ $^
clean:
	@echo "rimuovo i .o"
		rm -rf test test2 *.o *.fs *~ $(LIBS) $(BINS)