#Create lib_simpleloader.so from loader.c
TARGET = lib_simpleloader.so
SRC = loader.c
OBJ = $(SRC:.c=.o)

CFLAGS = -m32 -fPIC
LDFLAGS = -shared

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)

#Provide the command for cleanup
