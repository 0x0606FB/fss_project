CC = gcc
CFLAGS = -Wall -g
INCLUDES = -Isrc/include
BIN_DIR = bin
SRC_DIR = src

MANAGER_OBJS = $(SRC_DIR)/fss_manager.c $(SRC_DIR)/sync_manager.c $(SRC_DIR)/command_processor.c $(SRC_DIR)/utils.c
CONSOLE_OBJS = $(SRC_DIR)/fss_console.c $(SRC_DIR)/utils.c

all: $(BIN_DIR)/fss_manager $(BIN_DIR)/fss_console

$(BIN_DIR)/fss_manager: $(MANAGER_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

$(BIN_DIR)/fss_console: $(CONSOLE_OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

clean:
	rm -f $(BIN_DIR)/fss_manager $(BIN_DIR)/fss_console