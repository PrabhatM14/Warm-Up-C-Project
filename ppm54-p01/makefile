CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -D_GNU_SOURCE

TARGET = p01

SRC = p01.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

run: $(TARGET)
	./$(TARGET) test.txt substring1 substring2

debug: CFLAGS += -g
debug: clean $(TARGET)

# make run
