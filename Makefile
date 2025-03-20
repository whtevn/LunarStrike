
SRC_DIR := src
BUILD_DIR := build
BIN_DIR := bin
BIN_FILE := lunar-strike
DAISY_PATH := lib/DaisySP
YAML_PATH := lib/yaml-cpp
MIDI_PATH := lib/rtmidi

# Detect OS
UNAME_S := $(shell uname -s)

# Compiler settings
CXX := clang++
CXXFLAGS := -std=c++17
CXXFLAGS += -I$(INCLUDE_DIR)
CXXFLAGS += -I$(YAML_PATH)/include
CXXFLAGS += -I$(MIDI_PATH)
CXXFLAGS += -I$(DAISY_PATH)/Source

# OS-Specific Flags
ifeq ($(UNAME_S), Darwin)  # macOS
  LDFLAGS += -framework CoreMIDI -framework CoreAudio -framework CoreFoundation
  LDFLAGS += -lc++
  LDFLAGS += -L/usr/local/lib -L/opt/homebrew/lib -lportaudio -lportmidi
  LDFLAGS += -L/opt/homebrew/opt/ncurses/lib -lncurses

  CXXFLAGS += -I/usr/local/include -I/opt/homebrew/include
  CPPFLAGS += -I/opt/homebrew/opt/ncurses/include
  CXXFLAGS += -DMACOS
else  # Linux (Alpine)
  LDFLAGS += -lpthread
  LDFLAGS += -lstdc++
  LDFLAGS += -lncurses
  LDFLAGS += -ltinfo
  LDFLAGS += -lportaudio
  LDFLAGS += -lm
  LDFLAGS += $(MIDI_PATH)/build/librtmidi.a  # ✅ Link RtMidi first
  LDFLAGS += -lasound  # ✅ Ensure ALSA is linked **last**
endif

# DaisySP Paths
CXXFLAGS += -I$(DAISY_PATH)/Source
LDFLAGS += $(DAISY_PATH)/build/libDaisySP.a

# YAML Paths
CXXFLAGS += -I$(YAML_PATH)/include
LDFLAGS += -L$(YAML_PATH)/build -lyaml-cpp

# RtMidi Paths
CXXFLAGS += -I$(MIDI_PATH)
LDFLAGS += $(MIDI_PATH)/build/librtmidi.a


# Files
TARGET := $(BIN_DIR)/$(BIN_FILE)
SRC := $(wildcard $(SRC_DIR)/*.cpp)
OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(BUILD_DIR)/%.o, $(SRC))

# Build rules
all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p $(BIN_DIR) $(BUILD_DIR)  

	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)/$(BIN_FILE)

clean-all:
	rm -rf $(BUILD_DIR) $(BIN_DIR)/$(BIN_FILE) $(YAML_PATH)/build $(MIDI_PATH)/build $(DAISY_PATH)/build
	
update:
	git submodule update --init --recursive
	git pull --recurse-submodules origin main || true

build-daisy:
	@if [ -d "$(DAISY_PATH)" ]; then \
		mkdir -p $(DAISY_PATH)/build && \
		cd $(DAISY_PATH) && cmake -DCMAKE_CXX_STANDARD=17 -B build && cmake --build build; \
	else \
		echo "DAISY_PATH does not exist. Run 'make update' first."; \
	fi

build-yaml:
	@if [ -d "$(YAML_PATH)" ]; then \
		mkdir -p $(YAML_PATH)/build && \
		cd $(YAML_PATH) && cmake -DCMAKE_CXX_STANDARD=17 -B build && cmake --build build; \
	else \
		echo "YAML_PATH does not exist. Run 'make update' first."; \
	fi


build-midi:
	@if [ -d "$(MIDI_PATH)" ]; then \
		mkdir -p $(MIDI_PATH)/build && \
		cd $(MIDI_PATH) && cmake -B build -DBUILD_SHARED_LIBS=OFF -DCMAKE_CXX_STANDARD=17 && cmake --build build; \
	else \
		echo "MIDI_PATH does not exist. Run 'make update' first."; \
	fi

build:
	make -j4 build-yaml build-midi build-daisy
