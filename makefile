CXX = g++
CXXFLAGS = -g -Wall -std=gnu++11
SHELL = bash

TARGET = worddice

all: $(TARGET)

$(TARGET): worddice.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) worddice.cpp

.PHONY: test test-output test-memory clean

clean:
	rm -f $(TARGET)
