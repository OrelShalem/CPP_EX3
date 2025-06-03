# Makefile for the Coup card game
# This Makefile contains build configurations for compiling and testing the Coup game

# Compiler and flags definitions
CXX = g++                  # Using g++ as the C++ compiler
CXXFLAGS = -std=c++17 -Wall -g  # Compiler flags: C++17 standard, show all warnings, include debug info
VALGRIND_FLAGS = --tool=memcheck --leak-check=full --show-possibly-lost=yes --show-reachable=yes --num-callers=20 --track-origins=yes  # Memory checking configuration
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio  # SFML libraries for the graphical interface

# Directory structure
SRC_DIR = src              # Source code directory
TEST_DIR = test            # Test files directory
OBJ_DIR = obj              # Compiled object files directory
BIN_DIR = bin              # Binary executables directory
ASSETS_DIR = assets        # Game assets directory

# Create directories if they don't exist
# The shell command creates the necessary directories at the beginning of the build process
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR) $(ASSETS_DIR)/fonts)

# Source files definitions
MAIN_SRC = $(SRC_DIR)/main.cpp  # Main application source file
# The file with DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN must appear first in the test source list
TEST_SRC = $(TEST_DIR)/EdgeCaseTest.cpp $(TEST_DIR)/GameTest.cpp $(TEST_DIR)/PlayerTest.cpp $(TEST_DIR)/RolesTest.cpp  # Test source files
GUI_SRC = $(SRC_DIR)/CoupGUI.cpp  # GUI-related source file

# Object files definitions
SRC_OBJ = $(OBJ_DIR)/Player.o $(OBJ_DIR)/Game.o  # Core game objects
ROLE_OBJ = $(OBJ_DIR)/Baron.o $(OBJ_DIR)/General.o $(OBJ_DIR)/Governor.o $(OBJ_DIR)/Judge.o $(OBJ_DIR)/Merchant.o $(OBJ_DIR)/Spy.o  # Role-specific objects
MAIN_OBJ = $(OBJ_DIR)/main.o  # Main program object
GUI_OBJ = $(OBJ_DIR)/CoupGUI.o  # GUI object
# EdgeCaseTest.o must be first because it contains the main function for the test suite
TEST_OBJ = $(OBJ_DIR)/EdgeCaseTest.o $(OBJ_DIR)/GameTest.o $(OBJ_DIR)/PlayerTest.o $(OBJ_DIR)/RolesTest.o  # Test objects

# Build targets
all: Main test  # Default target builds and runs both the main program and tests

# Run the demo file
Main: $(BIN_DIR)/Main  # Build the main program
	$(BIN_DIR)/Main    # Run the main program

# Run the unit tests
test: $(BIN_DIR)/test  # Build the test program
	$(BIN_DIR)/test    # Run the test program

# Memory leak checks - tests both Main and test executables
valgrind: $(BIN_DIR)/Main $(BIN_DIR)/test
	@echo "=== Valgrind check for Main ==="  # Message indicating start of main program check
	valgrind $(VALGRIND_FLAGS) $(BIN_DIR)/Main  # Check main program for memory leaks
	@echo ""
	@echo "=== Valgrind check for tests ==="  # Message indicating start of test program check
	valgrind $(VALGRIND_FLAGS) $(BIN_DIR)/test  # Check test program for memory leaks

# Compile main program
$(BIN_DIR)/Main: $(SRC_OBJ) $(ROLE_OBJ) $(MAIN_OBJ) $(GUI_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIBS)  # Link all objects into the main executable with SFML libraries

# Compile test program
$(BIN_DIR)/test: $(SRC_OBJ) $(ROLE_OBJ) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@  # Link all test objects into the test executable

# General compilation rules for source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@  # Compile source files to object files

# Compilation rule for role-specific source files in the Roles subdirectory
$(OBJ_DIR)/%.o: $(SRC_DIR)/Roles/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@  # Compile role files to object files

# Compilation rule for test source files
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(TEST_DIR) -c $< -o $@  # Compile test files to object files with test directory in include path

# Clean target to remove all generated files
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*  # Remove all object and binary files

# Dependency specifications for specific files
# These ensure that files are recompiled when their dependencies change
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/Game.hpp $(SRC_DIR)/Player.hpp $(SRC_DIR)/CoupGUI.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@  # Compile main.cpp with its dependencies

$(OBJ_DIR)/Game.o: $(SRC_DIR)/Game.cpp $(SRC_DIR)/Game.hpp $(SRC_DIR)/Player.hpp $(SRC_DIR)/GameExceptions.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@  # Compile Game.cpp with its dependencies

$(OBJ_DIR)/Player.o: $(SRC_DIR)/Player.cpp $(SRC_DIR)/Player.hpp $(SRC_DIR)/Game.hpp $(SRC_DIR)/GameExceptions.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@  # Compile Player.cpp with its dependencies

$(OBJ_DIR)/CoupGUI.o: $(SRC_DIR)/CoupGUI.cpp $(SRC_DIR)/CoupGUI.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@  # Compile CoupGUI.cpp with its dependencies

.PHONY: all Main test valgrind clean  # Declare targets that don't produce files with the target name