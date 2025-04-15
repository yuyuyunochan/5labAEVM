CC = gcc
CFLAGS = -Wall -std=c11 -Iinclude
MYLIB_DIR = mySimpleComputer
BIGCHARS_DIR = myBigChars
READKEY_DIR = myReadkey
TERM_DIR = myTerm
CONSOLE_DIR = console
OBJ_DIR = obj
LIB_DIR = lib
LIB_NAME = libmySimpleComputer.a
BIGCHARS_LIB_NAME = libmyBigChars.a
READKEY_LIB_NAME = libmyReadkey.a
TARGET = $(CONSOLE_DIR)/console

SRC = $(CONSOLE_DIR)/console.c
LIB_SRC = $(wildcard $(MYLIB_DIR)/*.c)
TERM_SRC = $(wildcard myTerm/*.c)
BIGCHARS_SRC = $(wildcard $(BIGCHARS_DIR)/*.c)
READKEY_SRC = $(wildcard $(READKEY_DIR)/*.c)

HEADER = include/mySimpleComputer.h include/myTerm.h include/myBigChars.h include/myReadkey.h

LIB_OBJ = $(patsubst $(MYLIB_DIR)/%.c, $(OBJ_DIR)/%.o, $(LIB_SRC))
TERM_OBJ = $(patsubst myTerm/%.c, $(OBJ_DIR)/%.o, $(TERM_SRC))
BIGCHARS_OBJ = $(patsubst $(BIGCHARS_DIR)/%.c, $(OBJ_DIR)/%.o, $(BIGCHARS_SRC))
READKEY_OBJ = $(patsubst $(READKEY_DIR)/%.c, $(OBJ_DIR)/%.o, $(READKEY_SRC))
OBJ = $(OBJ_DIR)/console.o

all: $(TARGET)

$(TARGET): $(OBJ) $(LIB_DIR)/$(LIB_NAME) $(LIB_DIR)/$(BIGCHARS_LIB_NAME) $(LIB_DIR)/$(READKEY_LIB_NAME)
	$(CC) $(CFLAGS) -o $@ $(OBJ) -L$(LIB_DIR) -lmySimpleComputer -lmyBigChars -lmyReadkey

$(LIB_DIR)/$(LIB_NAME): $(LIB_OBJ) $(TERM_OBJ)
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(OBJ_DIR)
	ar rcs $@ $^
	ranlib $@

$(LIB_DIR)/$(BIGCHARS_LIB_NAME): $(BIGCHARS_OBJ)
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(OBJ_DIR)
	ar rcs $@ $^
	ranlib $@

$(LIB_DIR)/$(READKEY_LIB_NAME): $(READKEY_OBJ)
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(OBJ_DIR)
	ar rcs $@ $^
	ranlib $@

$(OBJ_DIR)/%.o: $(MYLIB_DIR)/%.c $(HEADER)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: myTerm/%.c $(HEADER)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(BIGCHARS_DIR)/%.c $(HEADER)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(READKEY_DIR)/%.c $(HEADER)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/console.o: $(CONSOLE_DIR)/console.c $(HEADER)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

run:
	./$(TARGET)

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(TARGET)
	rm -f $(CONSOLE_DIR)/*.o  $(BIGCHARS_DIR)/*.o  $(READKEY_DIR)/*.o $(MYLIB_DIR)/*.o  $(TERM_DIR)/*.o