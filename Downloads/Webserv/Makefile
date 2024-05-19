CXX = g++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -Iinclude
SRC_DIR = src
OBJ_DIR = obj
SRC_FILES = $(SRC_DIR)/*.cpp
OBJ_FILES = $(OBJ_DIR)/file1.o $(OBJ_DIR)/file2.o
TARGET = webserv

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
    mkdir -p $(OBJ_DIR)
    $(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

$(TARGET): $(OBJ_FILES)
    $(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^

clean:
    rm -rf $(OBJ_DIR) $(TARGET)