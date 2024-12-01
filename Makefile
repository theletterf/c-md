CC = gcc
CFLAGS = -std=c2x -Wall -Wextra -pedantic
LDFLAGS = -lncurses -lm

SOURCES = mdview.c md_parser.c viewer.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = mdview

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET) 