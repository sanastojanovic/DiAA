CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude 

SRC = main.cpp ./src/complex.cpp
OBJ = $(SRC:.cpp=.o)

TARGET = test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $(TARGET) 
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)