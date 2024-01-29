CC=gcc
CFLAGS=-Wall -Wextra -g

SRC_DIR=src
BUILD_DIR=build

all: executable

#
# Remove all build artifacts
#
clean: 
	rm -rf $(BUILD_DIR)

#
# Ensures that the build directory exists
#
build_dir: $(BUILD_DIR)
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

#
# Build executable
#
executable: $(BUILD_DIR)/acc
$(BUILD_DIR)/acc: build_dir
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/acc $(SRC_DIR)/main.c $(SRC_DIR)/lexer.c $(SRC_DIR)/string.c $(SRC_DIR)/util.c
