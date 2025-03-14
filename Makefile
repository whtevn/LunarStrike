# Project paths
DAISY_PATH := lib/DaisySP
YAML_PATH := lib/yaml-cpp
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
INCLUDE_DIR := include

# Compiler settings
CXX := clang++
CXXFLAGS := -std=c++17 -I$(DAISY_PATH)/Source -I$(INCLUDE_DIR) -I$(YAML_PATH)/include
LDFLAGS := -L$(DAISY_PATH)/build -L$(YAML_PATH)/build -lyaml-cpp -ldaisysp

# Files
TARGET := $(BIN_DIR)/my_synth
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))

# Build rules
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)/my_synth

# Update submodules (DaisySP & yaml-cpp)
.PHONY: update-submodules
update-submodules:
	git submodule update --init --recursive

# Update submodules and pull latest changes
.PHONY: update
update:
	git submodule update --init --recursive
	git pull --recurse-submodules

# Build yaml-cpp separately
.PHONY: build-yaml
build-yaml:
	mkdir -p $(YAML_PATH)/build
	cd $(YAML_PATH) && cmake -B build && cmake --build build

