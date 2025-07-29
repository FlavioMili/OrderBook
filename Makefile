# Compiler and Flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -I./include -O3 -march=native -flto -DNDEBUG -ffast-math -pipe

# Directories
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

# Files
SRC_FILES = $(SRC_DIR)/OrderBook.cpp $(SRC_DIR)/TestOrderBook.cpp
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
EXEC = $(BUILD_DIR)/OrderBook

# Targets
all: $(EXEC)

# Create the build directory if it doesn't exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Create the obj directory if it doesn't exist
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Build the final executable from object files
$(EXEC): $(OBJ_FILES) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all clean
