# Project paths
DAISY_PATH := lib/DaisySP
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
INCLUDE_DIR := include

# Compiler settings
CXX := clang++
CXXFLAGS := -std=c++17 -I$(DAISY_PATH)/Source -I$(INCLUDE_DIR)
LDFLAGS := -L$(DAISY_PATH)/build -ldaisysp

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

.PHONY: update-submodules
update-submodules:
	git submodule update --init --recursive

.PHONY: update
update:
	git submodule update --init --recursive
	git pull --recurse-submodules
