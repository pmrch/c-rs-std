# Makefile - Fixed compile_commands.json generation
CC = gcc
CFLAGS = -std=gnu23 -O3 -march=native -flto -I./include \
	-Wall -Wextra -Wpedantic -Werror -Wuninitialized -Wmaybe-uninitialized \
	-Wconversion -Wsign-conversion -Wcast-align -Wcast-qual -Wstrict-aliasing=2 \
	-Wpointer-arith -Warray-bounds -Wnull-dereference -Wmissing-prototypes \
	-Wstrict-prototypes -Wold-style-definition -Wredundant-decls -Wshadow \
	-Wformat=2 -Wformat-security -Wwrite-strings -Wvla -Wdouble-promotion \
	-Wfloat-equal -Wundef -Wswitch-enum -Wswitch-default -Wunused \
	-Wunused-function -Wunused-variable -Wunused-parameter -Wno-padded \
	-Wno-declaration-after-statement
LDFLAGS = -flto

SRC_DIR = src
BUILD_DIR = build
INC_DIR = include

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TARGET = replicate

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	@$(CC) $(LDFLAGS) -o $@ $^
	@echo "✅ Built $(TARGET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

# FIX: Generate compile_commands.json properly
compile_commands.json:
	@echo "Generating compile_commands.json..."
	@rm -f compile_commands.json
	@$(MAKE) clean
	@bear --output compile_commands.json -- $(MAKE) all
	@echo "✅ compile_commands.json generated at project root"

clean:
	@rm -rf $(BUILD_DIR)

distclean: clean
	@rm -f compile_commands.json

run: $(TARGET)
	@./$(TARGET)

.PHONY: all clean distclean run compile_commands.json