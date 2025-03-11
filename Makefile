# Makefile for Anvilock (CMake Wrapper)

# Set default build directory
BUILD_DIR = build
EXECUTABLE_NAME = anvilock

# Compiler and Flags
CMAKE = cmake
CMAKE_FLAGS = -DCMAKE_BUILD_TYPE=Release
MAKE = make

FLAGS ?=

STB_URL = "https://raw.githubusercontent.com/nothings/stb/master/stb_image.h"
STB_PATH= "stb_image.h"

# Paths to protocol files
EXT_PROTOCOL_PATH ?= /usr/share/wayland-protocols/staging/ext-session-lock/ext-session-lock-v1.xml
XDG_PROTOCOL_PATH ?= /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml

# Output directories
PROTOCOLS_DIR := protocols
PROTOCOLS_SRC_DIR := $(PROTOCOLS_DIR)/src

WAYLAND_SCANNER := wayland-scanner

# Ensure output directories exist
$(shell mkdir -p $(PROTOCOLS_SRC_DIR))

# Rules for EXT_SESSION_LOCK_V1 protocol
$(PROTOCOLS_DIR)/ext-session-lock-client-protocol.h $(PROTOCOLS_SRC_DIR)/ext-session-lock-client-protocol.c: $(EXT_PROTOCOL_PATH)
	@if [ -f "$<" ]; then \
		echo "Generating headers and sources for EXT_SESSION_LOCK_V1 protocol..."; \
		$(WAYLAND_SCANNER) client-header "$<" $(PROTOCOLS_DIR)/ext-session-lock-client-protocol.h; \
		$(WAYLAND_SCANNER) private-code "$<" $(PROTOCOLS_SRC_DIR)/ext-session-lock-client-protocol.c; \
		echo "EXT_SESSION_LOCK_V1 protocol generated successfully."; \
	else \
		echo "Error: EXT_SESSION_LOCK_V1 protocol file not found." >&2; \
		exit 1; \
	fi

# Rules for XDG_SHELL protocol
$(PROTOCOLS_DIR)/xdg-shell-client-protocol.h $(PROTOCOLS_SRC_DIR)/xdg-shell-client-protocol.c: $(XDG_PROTOCOL_PATH)
	@if [ -f "$<" ]; then \
		echo "Generating headers and sources for XDG_SHELL protocol..."; \
		$(WAYLAND_SCANNER) client-header "$<" $(PROTOCOLS_DIR)/xdg-shell-client-protocol.h; \
		$(WAYLAND_SCANNER) private-code "$<" $(PROTOCOLS_SRC_DIR)/xdg-shell-client-protocol.c; \
		echo "XDG_SHELL protocol generated successfully."; \
	else \
		echo "Error: XDG_SHELL protocol file not found." >&2; \
		exit 1; \
	fi


# Sanitized Builds
ASAN_BUILD_DIR = build-asan
TSAN_BUILD_DIR = build-tsan

# Targets
all: release

debug:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=Debug .. && $(MAKE)

protocols: 
	$(PROTOCOLS_DIR)/ext-session-lock-client-protocol.h $(PROTOCOLS_SRC_DIR)/ext-session-lock-client-protocol.c \
  $(PROTOCOLS_DIR)/xdg-shell-client-protocol.h $(PROTOCOLS_SRC_DIR)/xdg-shell-client-protocol.c

init:
	@if [ ! -f "$(STB_PATH)" ]; then \
		wget -q -O "$(STB_PATH)" "$(STB_URL)"; \
	else \
		echo ">> $(STB_PATH) already exists. Skipping download."; \
	fi
	$(MAKE) protocols

release:
	$(MAKE) init
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) $(CMAKE_FLAGS) $(FLAGS) .. && $(MAKE)

asan:
	@mkdir -p $(ASAN_BUILD_DIR)
	@cd $(ASAN_BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=Debug-ASan $(FLAGS) .. && $(MAKE)

tsan:
	@mkdir -p $(TSAN_BUILD_DIR)
	@cd $(TSAN_BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=Debug-TSan $(FLAGS) .. && $(MAKE)

format:
	@find src include \( -name "*.c" -o -name "*.h" \) -exec clang-format -i {} +

tidy:
	@find src include -name "*.c" -o -name "*.h" | xargs clang-tidy 

clean:
	@rm -rf $(BUILD_DIR) $(ASAN_BUILD_DIR) $(TSAN_BUILD_DIR)

install:
	@cd $(BUILD_DIR) && $(MAKE) install

uninstall:
	@xargs rm -f < install_manifest.txt

run:
	./$(BUILD_DIR)/$(EXECUTABLE_NAME)

.PHONY: all debug release asan tsan format clean install uninstall
