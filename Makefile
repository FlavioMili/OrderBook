# Compiler and Flags
CXX = g++
# Basic flags for compilation
CXXFLAGS = -std=c++20 -Wall -O3 -DNDEBUG
# Include paths for the project and external libraries
INCLUDES = -I./include -I./extern/benchmark/include
# Linker flags
LDFLAGS = -lpthread

# Automatic parallel builds
NPROC := $(shell nproc)
MAKEFLAGS += -j$(NPROC)

# Directories
SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BENCHMARK_DIR = benchmark
TOOLS_DIR = tools
EXTERN_DIR = extern

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.cpp)
BENCHMARK_FILES = $(wildcard $(BENCHMARK_DIR)/*.cpp)
GENERATE_DATA_FILES = $(wildcard $(TOOLS_DIR)/*.cpp)

# Object files
OBJ_FILES = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC_FILES))
BENCHMARK_OBJ = $(patsubst $(BENCHMARK_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(BENCHMARK_FILES))
GENERATE_DATA_OBJ = $(patsubst $(TOOLS_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(GENERATE_DATA_FILES))

# Executables
BENCHMARK_EXEC = $(BUILD_DIR)/benchmark_runner
GENERATE_DATA_EXEC = $(BUILD_DIR)/generate_data

# External Libraries
BENCHMARK_LIB = $(EXTERN_DIR)/benchmark/build/src/libbenchmark.a

.PHONY: all benchmark generate-data clean

all: benchmark

# --- Build Targets ---

# Build and run the benchmark
benchmark: $(BENCHMARK_EXEC)
	@echo "Running benchmark..."
	@./$(BENCHMARK_EXEC)

# Build the benchmark executable
$(BENCHMARK_EXEC): $(BENCHMARK_OBJ) $(OBJ_FILES) $(BENCHMARK_LIB)
	@echo "Linking benchmark executable..."
	@$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Build the data generator
generate-data: $(GENERATE_DATA_EXEC)
	@echo "Running data generator..."
	@./$(GENERATE_DATA_EXEC)

$(GENERATE_DATA_EXEC): $(GENERATE_DATA_OBJ)
	@echo "Linking data generator..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

# --- Compilation Rules ---

# Rule for compiling source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Rule for compiling benchmark files
$(OBJ_DIR)/%.o: $(BENCHMARK_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Rule for compiling tools files
$(OBJ_DIR)/%.o: $(TOOLS_DIR)/%.cpp | $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# --- Dependencies and Setup ---

# Build Google Benchmark library
$(BENCHMARK_LIB):
	@echo "Configuring and building Google Benchmark library..."
	@cmake -S $(EXTERN_DIR)/benchmark -B $(EXTERN_DIR)/benchmark/build -DCMAKE_BUILD_TYPE=Release -DBENCHMARK_ENABLE_TESTING=OFF > /dev/null
	@cmake --build $(EXTERN_DIR)/benchmark/build > /dev/null

# Create build directories
$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

# --- Housekeeping ---

clean:
	@echo "Cleaning up build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(EXTERN_DIR)/benchmark/build
