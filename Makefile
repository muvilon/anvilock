# Set default build directory
BUILD_DIR = build
EXECUTABLE_NAME = anvilock

# Compiler and Flags
CMAKE = cmake
CMAKE_FLAGS = -DCMAKE_BUILD_TYPE=Release
MAKE = make

# Sanitized Builds
ASAN_BUILD_DIR = build-asan
TSAN_BUILD_DIR = build-tsan

# Targets
all: release

debug:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=Debug .. && $(MAKE)

release:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && $(CMAKE) $(CMAKE_FLAGS) .. && $(MAKE)

asan:
	@mkdir -p $(ASAN_BUILD_DIR)
	@cd $(ASAN_BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=Debug-ASan .. && $(MAKE)

tsan:
	@mkdir -p $(TSAN_BUILD_DIR)
	@cd $(TSAN_BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=Debug-TSan .. && $(MAKE)

format:
	@find src include -name "*.c" -o -name "*.h" | xargs clang-format -i

tidy:
	@find src include -name "*.c" -o -name "*.h" | xargs clang-tidy 

clean:
	@rm -rf $(BUILD_DIR) $(ASAN_BUILD_DIR) $(TSAN_BUILD_DIR)

install:
	@cd $(BUILD_DIR) && $(MAKE) install

uninstall:
	@xargs rm -f < install_manifest.txt

.PHONY: all debug release asan tsan format clean install uninstall
