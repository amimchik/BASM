# Project name = current folder name
PROJECT := $(notdir $(CURDIR))

# Compiler and flags
CC := gcc
INC_DIR := include
CFLAGS := -Wall -Wextra -g -std=c99 -I$(INC_DIR)

# Directories
SRC_DIR := src
OBJ_DIR := build/obj
BIN_DIR := build/bin

# Find all .c files in src and subdirs
SRCS := $(shell find $(SRC_DIR) -name '*.c')
# Convert src/... to obj/... with .o extension
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

# Final binary
TARGET := $(BIN_DIR)/$(PROJECT)

# Phony targets
.PHONY: all clean build rebuild run

# Default
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) -o $@ $^

# Compiling .c -> .o (mirroring directory structure)
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -MMD -c $< -o $@

# Include dependency files
-include $(DEPS)

# Convenience targets
build: all
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
rebuild: clean build
run: all
	@./$(TARGET)

