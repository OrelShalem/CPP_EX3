# קובץ Makefile למשחק coup

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
VALGRIND_FLAGS = --tool=memcheck --leak-check=full --show-possibly-lost=yes --show-reachable=yes --num-callers=20 --track-origins=yes

# תיקיות
SRC_DIR = src
TEST_DIR = test
OBJ_DIR = obj
BIN_DIR = bin

# יצירת התיקיות אם הן לא קיימות
$(shell mkdir -p $(OBJ_DIR) $(BIN_DIR))

# קבצי מקור
MAIN_SRC = $(SRC_DIR)/main.cpp
# הקובץ עם ה-DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN חייב להופיע ראשון
TEST_SRC = $(TEST_DIR)/EdgeCaseTest.cpp $(TEST_DIR)/GameTest.cpp $(TEST_DIR)/PlayerTest.cpp $(TEST_DIR)/RolesTest.cpp

# קבצי אובייקט
SRC_OBJ = $(OBJ_DIR)/Player.o $(OBJ_DIR)/Game.o
ROLE_OBJ = $(OBJ_DIR)/Baron.o $(OBJ_DIR)/General.o $(OBJ_DIR)/Governor.o $(OBJ_DIR)/Judge.o $(OBJ_DIR)/Merchant.o $(OBJ_DIR)/Spy.o
MAIN_OBJ = $(OBJ_DIR)/main.o
# EdgeCaseTest.o חייב להופיע ראשון כי הוא מכיל את ה-main
TEST_OBJ = $(OBJ_DIR)/EdgeCaseTest.o $(OBJ_DIR)/GameTest.o $(OBJ_DIR)/PlayerTest.o $(OBJ_DIR)/RolesTest.o

# יעדים
all: Main test

# הרצת קובץ ההדגמה
Main: $(BIN_DIR)/Main
	$(BIN_DIR)/Main

# הרצת בדיקות היחידה
test: $(BIN_DIR)/test
	$(BIN_DIR)/test

# בדיקת זליגת זיכרון - בודק גם Main וגם טסטים
valgrind: $(BIN_DIR)/Main $(BIN_DIR)/test
	@echo "=== Valgrind check for Main ==="
	valgrind $(VALGRIND_FLAGS) $(BIN_DIR)/Main
	@echo ""
	@echo "=== Valgrind check for tests ==="
	valgrind $(VALGRIND_FLAGS) $(BIN_DIR)/test

# קימפול תוכנית ראשית
$(BIN_DIR)/Main: $(SRC_OBJ) $(ROLE_OBJ) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# קימפול בדיקות
$(BIN_DIR)/test: $(SRC_OBJ) $(ROLE_OBJ) $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@

# כללי קימפול כלליים
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/Roles/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(TEST_DIR) -c $< -o $@

# ניקוי קבצים לא רלוונטיים
clean:
	rm -rf $(OBJ_DIR)/* $(BIN_DIR)/*

# הגדרות תלויות
$(OBJ_DIR)/main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/Game.hpp $(SRC_DIR)/Player.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/Game.o: $(SRC_DIR)/Game.cpp $(SRC_DIR)/Game.hpp $(SRC_DIR)/Player.hpp $(SRC_DIR)/GameExceptions.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR)/Player.o: $(SRC_DIR)/Player.cpp $(SRC_DIR)/Player.hpp $(SRC_DIR)/Game.hpp $(SRC_DIR)/GameExceptions.hpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: all Main test valgrind clean 