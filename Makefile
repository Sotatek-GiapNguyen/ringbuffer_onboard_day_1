CC = gcc
CXX = g++
AR = ar
CFLAGS = -Wall -Wextra -fPIC -g -O2 -pthread
CXXFLAGS = -Wall -Wextra -g -O2 -pthread
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
LDFLAGS = -pthread
GTEST_LIBS = -lgtest -lgtest_main

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
OBJ_DIR = obj
LIB_DIR = lib
COV_DIR = coverage

# Source files
SRC = $(wildcard $(SRC_DIR)/*.c)
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
TEST_SRC = $(wildcard $(TEST_DIR)/*.cpp)
TEST_OBJ = $(TEST_SRC:$(TEST_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Targets
STATIC_LIB = $(LIB_DIR)/libringbuffer.a
SHARED_LIB = $(LIB_DIR)/libringbuffer.so
TEST_BIN = ringbuffer_test

.PHONY: all clean test coverage

all: dirs $(STATIC_LIB) $(SHARED_LIB) test

dirs:
	@mkdir -p $(OBJ_DIR) $(LIB_DIR) $(COV_DIR)

# Build object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Build static library
$(STATIC_LIB): $(OBJ)
	$(AR) rcs $@ $^

# Build shared library
$(SHARED_LIB): $(OBJ)
	$(CC) -shared $(LDFLAGS) -o $@ $^

# Build test objects
$(OBJ_DIR)/%.o: $(TEST_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

# Build and run tests
test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_OBJ) $(STATIC_LIB)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(GTEST_LIBS) $(LDFLAGS)

# Coverage build
coverage: CFLAGS += $(COVERAGE_FLAGS)
coverage: CXXFLAGS += $(COVERAGE_FLAGS)
coverage: clean all
	./$(TEST_BIN)
	lcov --capture --directory . --output-file $(COV_DIR)/coverage.info
	lcov --remove $(COV_DIR)/coverage.info '/usr/*' --output-file $(COV_DIR)/coverage.info
	genhtml $(COV_DIR)/coverage.info --output-directory $(COV_DIR)
	@echo "Coverage report generated in $(COV_DIR)/index.html"

clean:
	rm -rf $(OBJ_DIR) $(LIB_DIR) $(TEST_BIN) $(COV_DIR) *.gcda *.gcno 