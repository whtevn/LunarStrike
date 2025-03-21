# Directories
BUILD_DIR := build
BIN_DIR := bin

# Executable name
TARGET := lunar-strike

# Tools
CMAKE ?= cmake
MAKE ?= make

# Default target
.PHONY: all
all: build

# Set up git submodules (if needed)
.PHONY: setup
setup:
	git submodule update --init --recursive

# Configure and build the project
.PHONY: build
build:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) .. && $(MAKE) -j

# Debug build
.PHONY: debug
debug:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=Debug .. && $(MAKE) -j

# Release build
.PHONY: release
release:
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) -DCMAKE_BUILD_TYPE=Release .. && $(MAKE) -j

# Run the compiled binary
.PHONY: run
run:
	$(BIN_DIR)/$(TARGET)

# Clean build artifacts
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

# Reconfigure
.PHONY: reconfigure
reconfigure:
	rm -rf $(BUILD_DIR)
	mkdir -p $(BUILD_DIR)
	cd $(BUILD_DIR) && $(CMAKE) ..

# Full clean: project + submodules
.PHONY: clean-all
clean-all: clean
	rm -rf lib/*/build \
		$(BUILD_DIR)/CMakeCache.txt \
		$(BUILD_DIR)/CMakeFiles \
		$(BUILD_DIR)/cmake_install.cmake \
		$(BUILD_DIR)/Makefile

# Configure and build the project
.PHONY: docker
docker:
	docker compose run --rm $(TARGET)


.PHONY: docker-fresh
docker-fresh:
	make clean-all
	make setup
	docker compose build --no-cache
	make docker

