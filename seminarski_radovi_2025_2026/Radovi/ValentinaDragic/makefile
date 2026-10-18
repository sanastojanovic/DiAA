CXX      = g++
CXXFLAGS = -std=c++2a -Wall -Wextra

SRCS   = main.cpp parser.cpp graph.cpp algoritmi.cpp komande.cpp petlja.cpp
TARGET = dep-analyzer

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)
