CXX = g++
CXXFLAGS = -O2 -std=c++17 -Iinclude

SRC = main.cpp \
      source/image.cpp \
      source/segmentation.cpp

OUT = seg

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(SRC) $(CXXFLAGS) -o $(OUT)

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
	rm -rf results/*

.PHONY: all run clean

