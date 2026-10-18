CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20

# list of source files
SRCS = main.cpp parser.cpp cfg.cpp

# object files corresponding to sources
OBJS = $(SRCS:.cpp=.o)

TARGET = a.out

all: $(TARGET)

# link objects into the final executable
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# compile each cpp file into an object
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
