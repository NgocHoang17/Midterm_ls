CC = gcc
CFLAGS = -Wall -Iinclude
SRC = src/main.c src/list_dir.c src/file_info.c src/options.c src/utils.c
OBJ = $(SRC:.c=.o)
TARGET = my_ls

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

clean:
	rm -f $(OBJ) $(TARGET)
