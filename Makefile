CC = gcc
CFLAGS = -Wall -Wextra -Wpedantic -g $(shell pkg-config --cflags freetype2) -Itoml
LIBS = -lwayland-client -lwayland-server -lwayland-egl -lEGL -lGLESv2 -lpam -lxkbcommon $(shell pkg-config --libs freetype2)
TARGET = anvilock
SRC = main.c toml/toml.c
CONFIG_DIR = $(HOME)/.config/anvilock
CONFIG_FILE = $(CONFIG_DIR)/config.toml

all: $(TARGET) $(CONFIG_FILE)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LIBS) -o $(TARGET)

$(CONFIG_FILE):
	@if [ ! -d $(CONFIG_DIR) ]; then \
		mkdir -p $(CONFIG_DIR); \
	fi
	@if [ ! -f $(CONFIG_FILE) ]; then \
		echo "[font]" > $(CONFIG_FILE); \
		echo "name = \"# your font name goes here\"" >> $(CONFIG_FILE); \
		echo "path = \"# your font path goes here\"" >> $(CONFIG_FILE); \
		echo "Created $(CONFIG_FILE) with placeholder values."; \
	else \
		echo "$(CONFIG_FILE) already exists, skipping creation."; \
	fi

clean:
	rm -f $(TARGET)

.PHONY: all clean
