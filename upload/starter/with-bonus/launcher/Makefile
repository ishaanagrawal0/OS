#Compile the launch.c by linking it with the lib_simpleloader.so
TARGET = launch
SRC = launch.c
OBJ = $(SRC:.c=.o)

LIB_PATH = ../loader
LIB_NAME = lib_simpleloader.so

CFLAGS = -I$(LIB_PATH)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ -L$(LIB_PATH) -l_simpleloader

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)

#Provide the command for cleanup
