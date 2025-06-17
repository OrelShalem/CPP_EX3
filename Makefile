# Makefile for the Coup card game

# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g
VALGRIND_FLAGS = --tool=memcheck --leak-check=full --show-possibly-lost=yes --show-reachable=yes --num-callers=20 --track-origins=yes
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio

# Directories
SRC_DIR = src
TEST_DIR = test
OBJ_DIR = obj
BIN_DIR = bin

# Create directories
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

# Source files
MAIN_SRC = $(SRC_DIR)/main.cpp
SRC_FILES = $(SRC_DIR)/Player.cpp $(SRC_DIR)/Game.cpp
GUI_FILES = $(SRC_DIR)/CoupGUI.cpp
ROLE_FILES = $(SRC_DIR)/Roles/Baron.cpp $(SRC_DIR)/Roles/General.cpp $(SRC_DIR)/Roles/Governor.cpp $(SRC_DIR)/Roles/Judge.cpp $(SRC_DIR)/Roles/Merchant.cpp $(SRC_DIR)/Roles/Spy.cpp
TEST_FILES = $(TEST_DIR)/EdgeCaseTest.cpp $(TEST_DIR)/GameTest.cpp $(TEST_DIR)/PlayerTest.cpp $(TEST_DIR)/RolesTest.cpp

# Object files
MAIN_OBJ = $(OBJ_DIR)/main.o
SRC_OBJ = $(SRC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
GUI_OBJ = $(GUI_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
ROLE_OBJ = $(ROLE_FILES:$(SRC_DIR)/Roles/%.cpp=$(OBJ_DIR)/%.o)
TEST_OBJ = $(TEST_FILES:$(TEST_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Targets
.PHONY: all Main test valgrind clean

all: Main test

Main: $(BIN_DIR)/Main
	$(BIN_DIR)/Main

test: $(BIN_DIR)/test
	$(BIN_DIR)/test

# Memory leak checks
valgrind: $(BIN_DIR)/Main $(BIN_DIR)/test
	@echo "=== Valgrind check for Main ==="
	valgrind $(VALGRIND_FLAGS) $(BIN_DIR)/Main
	@echo ""
	@echo "=== Valgrind check for tests ==="
	valgrind $(VALGRIND_FLAGS) $(BIN_DIR)/test

$(BIN_DIR)/Main: $(MAIN_OBJ) $(SRC_OBJ) $(GUI_OBJ) $(ROLE_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(SFML_LIBS)

$(BIN_DIR)/test: $(SRC_OBJ) $(ROLE_OBJ) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# Compilation rules
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/Roles/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(TEST_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*