# Compiler and flags
SDKROOT := $(shell xcrun --show-sdk-path) # I hate that I have to do this right now. Curse you apple.
CXX := clang++
CXXFLAGS := -std=c++23 -Wall -Wextra -Og -g -isysroot $(SDKROOT)
# Include paths
INCLUDES := -Isrc -Isrc/serial -Iexternal/cxxopts/include

# Source and object files
SRCDIR := src
OBJDIR := obj
BINDIR := bin
TARGET := $(BINDIR)/seriterm

SOURCES := $(shell find $(SRCDIR) -name '*.cpp')
OBJECTS := $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)

compile_commands.json:
	bear -- make clean all

.PHONY: all clean compile_commands.json
