CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude

SRC = src/main.cpp \
	  src/helper.cpp \
	  src/tokenizer.cpp

# Object files
OBJ = $(SRC:.cpp=.o)

# Output binary name
TARGET = shell

# === Build target ===
all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJ)

# Compile .cpp → .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# === Utilities ===
clean:
	rm -f $(OBJ) $(TARGET)

rebuild: clean all

