# Compiler settings
CXX := clang++
CXXFLAGS := -std=c++17
CXXFLAGS += -I$(INCLUDE_DIR)
CXXFLAGS += -I$(YAML_PATH)/include
CXXFLAGS += -I$(MIDI_PATH)
CXXFLAGS += -I$(DAISY_PATH)/Source  # Ensure DaisySP include path is correct

# ncurses Paths
LDFLAGS += -L/opt/homebrew/opt/ncurses/lib -lncurses
CPPFLAGS += -I/opt/homebrew/opt/ncurses/include



# DaisySP Paths
DAISY_PATH := lib/DaisySP
CXXFLAGS += -I$(DAISY_PATH)/Source
LDFLAGS_DAISY := -L$(DAISY_PATH)/build -ldaisysp

# YAML Paths
YAML_PATH := lib/yaml-cpp
LDFLAGS_YAML := -L$(YAML_PATH)/build -lyaml-cpp

# RtMidi Paths
MIDI_PATH := lib/rtmidi
CXXFLAGS += -I$(MIDI_PATH)
LDFLAGS_MIDI := $(MIDI_PATH)/build/librtmidi.a

# macOS-specific frameworks
LDFLAGS_MAC := -framework CoreMIDI -framework CoreAudio -framework CoreFoundation

# PortAudio & PortMIDI Paths
CXXFLAGS += -I/usr/local/include -I/opt/homebrew/include
LDFLAGS_AUDIO := -L/usr/local/lib -L/opt/homebrew/lib -lportaudio -lportmidi -lpthread

# Files
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
TARGET := $(BIN_DIR)/my_synth
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))

# Combined LDFLAGS 
LDFLAGS := $(LDFLAGS_DAISY) $(LDFLAGS_YAML) $(LDFLAGS_MIDI) $(LDFLAGS_AUDIO) $(LDFLAGS_MAC) -lncurses

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

# Build RtMidi separately
.PHONY: build-midi
build-midi:
	mkdir -p $(MIDI_PATH)/build
	cd $(MIDI_PATH) && cmake -B build && cmake --build build

# Build DaisySP separately
.PHONY: build-daisy
build-daisy:
	mkdir -p $(DAISY_PATH)/build
	cd $(DAISY_PATH) && cmake -B build && cmake --build build

