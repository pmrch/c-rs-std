# Makefile - Fixed compile_commands.json generation
LOG_LEVEL ?= 2

CC = gcc
CFLAGS = -std=gnu2x -O3 -march=native -flto -fsanitize=address -I./include \
	-Wall -Wextra -Wpedantic -Werror -Wuninitialized -Wmaybe-uninitialized \
	-Wconversion -Wsign-conversion -Wcast-align -Wcast-qual -Wstrict-aliasing=2 \
	-Wpointer-arith -Warray-bounds -Wnull-dereference -Wmissing-prototypes \
	-Wstrict-prototypes -Wold-style-definition -Wredundant-decls -Wshadow \
	-Wformat=2 -Wformat-security -Wwrite-strings -Wvla -Wdouble-promotion \
	-Wfloat-equal -Wundef -Wswitch-enum -Wswitch-default -Wunused \
	-Wunused-function -Wunused-variable -Wunused-parameter -Wno-padded \
	-Wno-declaration-after-statement -DLOG_LEVEL=$(LOG_LEVEL)
LDFLAGS = -flto

SRC_DIR = src
BUILD_DIR = build

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
TARGET = replicate

TEST_DIR = tests
TEST_SRCS = $(wildcard $(TEST_DIR)/*.c)

# Filter out the main application file object
APP_MAIN_OBJ = $(BUILD_DIR)/main.o
TEST_OBJS = $(filter-out $(APP_MAIN_OBJ), $(OBJS))
TEST_BINS = $(TEST_SRCS:$(TEST_DIR)/%.c=$(BUILD_DIR)/test_%)

all: $(BUILD_DIR) $(TARGET)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJS)
	@$(CC) $(LDFLAGS) -o $@ $^
	@echo "✅ Built $(TARGET)"

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_%: $(TEST_DIR)/%.c $(TEST_OBJS)
	@$(CC) $(CFLAGS) -I$(TEST_DIR)  $< $(TEST_OBJS) -o $@ $(LDFLAGS)
	@echo "🛠️  Compiled test: $@"

# FIX: Generate compile_commands.json properly
compile_commands.json:
	@echo "Generating compile_commands.json..."
	@rm -f compile_commands.json
	@$(MAKE) clean
	@bear --output compile_commands.json -- $(MAKE) all
	@echo "✅ compile_commands.json generated at project root"

test: $(BUILD_DIR) $(TEST_BINS)
	@echo "🚀 Running tests..."
	@for test in $(TEST_BINS); do ./$$test || exit 1; done

clean:
	@rm -rf $(BUILD_DIR)
	@rm -rf $(TARGET)

distclean: clean
	@rm -f compile_commands.json

run: $(TARGET)
	@./$(TARGET)

.PHONY: all clean distclean run compile_commands.json