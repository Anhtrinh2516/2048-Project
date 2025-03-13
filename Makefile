CXX = g++
CXXFLAGS = -std=c++11 -Wall
LDFLAGS = -lSDL2 -lSDL2_ttf -lSDL2_mixer

SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TARGET = 2048

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: run
run: $(TARGET)
	./$(TARGET) 