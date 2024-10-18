CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g $(shell pkg-config --cflags freetype2)
LIBS = -lwayland-client -lwayland-server -lpam -lxkbcommon $(shell pkg-config --libs freetype2)
TARGET = anvilock
SRC = main.c

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: all clean
