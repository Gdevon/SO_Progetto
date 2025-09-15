CC=gcc
CCOPTS=--std=gnu99 -Wall -g 
OBJS=DIR_Entry.o  Errors.o  FAT_info.o  FS_info.o  FileHandle.o  linked_list.o ListHandle.o DirHandle.o shell.o run.o 
HEADERS=FileHandle.h FAT_info.h DIR_Entry.h DirHandle.h FS_info.h Errors.h linked_list.h ListHandle.h DirHandle.h shell.h Extern_fs.h Colors.h
BINS = run
.phony: clean all
all: $(BINS)
%.o: %.c $(HEADERS)
	@$(CC) $(CCOPTS) -c -o $@ $<
$(BINS): $(OBJS)
	@$(CC) $(CCOPTS) -o $@ $^
	@echo "Eseguibile creato: $@"
clean:
	@rm -f run *.o *.fs *.FS *~ $(TARGET)
	@echo "Pulizia completata"
