# Define the build directory
BUILD_DIR = build
BUILD_TYPE ?= Debug  # Default to 'Debug' if BUILD_TYPE is not defined
# Default target Linux
all: configure build test

# Check if the build directory exists, if not create it
configure-windows:
	# @mkdir $(BUILD_DIR)> NUL
	@cmake build . -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=${BUILD_TYPE}

configure:
	@mkdir -p $(BUILD_DIR)
	@cmake build . -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=${BUILD_TYPE}

configure-web:
	@mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && ../emsdk/upstream/emscripten/emcmake cmake .. -DPLATFORM=Web -DCMAKE_BUILD_TYPE=Release -DCMAKE_EXECUTABLE_SUFFIX=".html"

# Build the project
build:
	@cd $(BUILD_DIR) && $(MAKE)

build-web:
	@cd $(BUILD_DIR) && emmake make 

# Run tests
test:
	@ctest --test-dir $(BUILD_DIR) -DTESTING_ENABLED=1 -DTESTING=1

clean-all:
	rm -rf $(BUILD_DIR)

# Clean the build directory
clean:
	find $(BUILD_DIR) -maxdepth 1 -type f -delete
	rm -rf $(BUILD_DIR)/bin
	rm -rf $(BUILD_DIR)/CMakeFiles
	rm -rf $(BUILD_DIR)/space-pixel-game
	rm -rf $(BUILD_DIR)/src
	rm -rf $(BUILD_DIR)/Testing
	rm -rf $(BUILD_DIR)/tests
	rm -rf $(BUILD_DIR)/lib
run:
	./$(BUILD_DIR)/space-pixel-game/space-pixel-game
.PHONY: all configure build test clean