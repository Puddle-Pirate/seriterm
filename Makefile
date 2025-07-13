# Compiler and flags
CXX := g++
CXXFLAGS := -std=c++20 -Wall -Wextra

# Include paths
INCLUDES := -Isrc -Isrc/serial -Iexternal/cxxopts/include

# Source and object files
SRCDIR := src
OBJDIR := build
BINDIR := bin
TARGET := $(BINDIR)/seriterm

SOURCES := $(wildcard $(SRCDIR)/*.cpp)
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -Og -c -o $@ $<

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)

compile_commands.json:
	bear -- make clean all

.PHONY: all clean compile_commands.json
