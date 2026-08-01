CC = gcc

# Libraries to include
LIBS = -lgdi32

# Prevent terminal window also opening
CFLAGS = -Wl,-subsystem,windows -mwindows -Wall -Wextra -Wpedantic

# Main source file
SRC = src/windowBase.c src/app.c src/demos.c

BUILD_DIR = build
BIN_DIR = bin

WINDRES = windres

OBJ = $(SRC:src/%.c=$(BUILD_DIR)/%.o)

# Output executable
BIN = $(BIN_DIR)/windowBase.exe

# Default target
all: $(BIN)

build/LaGuiEnC.res: resources/LaGuiEnC.rc resources/resource.h resources/LaGuiEnC.ico
	$(WINDRES) resources/LaGuiEnC.rc -O coff -o build/LaGuiEnC.res

RESOURCES = build/LaGuiEnC.res

# Link object files into the exe
$(BIN): $(OBJ) $(RESOURCES)
	@if not exist $(BIN_DIR) mkdir $(BIN_DIR)
	$(CC) $(OBJ) $(RESOURCES) -o $@ $(CFLAGS)

# Compile .c file into .o files
$(BUILD_DIR)/%.o: src/%.c
	@if not exist $(BUILD_DIR) mkdir $(BUILD_DIR)
	$(CC) -c $< -o $@ $(CFLAGS)

# Clean up build artifacts
clean:
	-if exist $(BUILD_DIR) rmdir /s /q $(BUILD_DIR)
	-if exist $(BIN_DIR) rmdir /s /q $(BIN_DIR)
