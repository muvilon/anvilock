# Makefile for muvilock

CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g
LIBS = -lwayland-client -lwayland-server -lpam -lxkbcommon
TARGET = muvilock
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
