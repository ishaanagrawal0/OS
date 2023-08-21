#Compile the launch.c by linking it with the lib_simpleloader.so

#Provide the command for cleanup

# Compiler settings
CC = gcc
CFLAGS = -Wall -Werror
LDFLAGS = -L../bin -lsimpleloader

# Source file
SRC = launch.c

# Target executable
TARGET = launch

all: $(TARGET)

$(TARGET): $(SRC) $(CC) $(CFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean: rm -f $(TARGET)