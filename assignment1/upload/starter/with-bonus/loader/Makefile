#Create lib_simpleloader.so from loader.c

# Compiler settings
CC = gcc
CFLAGS = -Wall -Werror -shared -fPIC -m32

# Source file
SRC = loader.c

# Target library
TARGET = lib_simpleloader.so

# Directory for the library and binary
BIN_DIR = ../bin

all: $(TARGET)
    mkdir -p $(BIN_DIR)
    mv $(TARGET) $(BIN_DIR)

$(TARGET): $(SRC) loader.h
    $(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
    rm -f $(TARGET)
