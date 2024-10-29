CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g $(shell pkg-config --cflags freetype2) -Itoml
LIBS = -lwayland-client -lwayland-server -lpam -lxkbcommon $(shell pkg-config --libs freetype2)
TARGET = anvilock
SRC = main.c toml/toml.c
CONFIG_FILE = config.toml

all: $(TARGET) $(CONFIG_FILE)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(TARGET)

$(CONFIG_FILE):
	@if [ ! -f $(CONFIG_FILE) ]; then \
		echo "[font]" > $(CONFIG_FILE); \
		echo "name = \"# your font name goes here\"" >> $(CONFIG_FILE); \
		echo "path = \"# your font path goes here\"" >> $(CONFIG_FILE); \
		echo "Created config.toml with placeholder values."; \
	fi

clean:
	rm -f $(TARGET)

.PHONY: all clean
