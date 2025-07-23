CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp
TARGET = render_engine

# Include directories
INCLUDES = -I$(SRCDIR)

# Default target
all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET)

clean:
	rm -f $(TARGET) output.ppm

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
