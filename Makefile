CC = gcc
CCOPTS = --std=gnu99 -Wall -g

SRC_DIR = src
OBJ_DIR = objs
BIN_DIR = bins

OBJS = $(OBJ_DIR)/DIR_Entry.o $(OBJ_DIR)/Errors.o $(OBJ_DIR)/FAT_info.o $(OBJ_DIR)/FS_info.o $(OBJ_DIR)/FileHandle.o $(OBJ_DIR)/linked_list.o $(OBJ_DIR)/ListHandle.o $(OBJ_DIR)/DirHandle.o $(OBJ_DIR)/shell.o $(OBJ_DIR)/run.o

HEADERS =  $(SRC_DIR)/FileHandle.h  $(SRC_DIR)/FAT_info.h  $(SRC_DIR)/DIR_Entry.h  $(SRC_DIR)/DirHandle.h  $(SRC_DIR)/FS_info.h  $(SRC_DIR)/Errors.h  $(SRC_DIR)/linked_list.h $(SRC_DIR)/ListHandle.h  $(SRC_DIR)/shell.h  $(SRC_DIR)/Extern.h  $(SRC_DIR)/Colors.h

BINS = $(BIN_DIR)/run
.phony: all clean

all: $(BINS)

$(BINS): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@$(CC) $(CCOPTS) -o $@ $^
	@echo "Eseguibile creato: $@"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(HEADERS)
	@mkdir -p $(OBJ_DIR)
	@$(CC) $(CCOPTS) -c -o $@ $<
	@echo "Compilato: $< -> $@"


clean:
	@rm -rf $(OBJ_DIR) $(BIN_DIR) *.fs *.FS *~
	@echo "Pulizia completata"
