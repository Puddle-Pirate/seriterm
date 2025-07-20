# Directories
SRCDIR := src
EXTDIR := external
OBJDIR := obj
BINDIR := bin
TARGET := $(BINDIR)/seriterm

# Compilers
CXX := clang++
CC  := clang

SDKROOT := $(shell xcrun --show-sdk-path) # I hate that I have to do this right now. Curse you apple.

# Flags
CXXFLAGS := -std=c++20 -Wall -Wextra -Og -g -isysroot $(SDKROOT)
CFLAGS   := -Wall -Wextra -Og -g -isysroot $(SDKROOT)

# Include paths
INCLUDES := -I$(SRCDIR) \
				-I$(SRCDIR)/serial \
				-I$(EXTDIR)/cxxopts/include \
				-I$(EXTDIR)/linenoise \

# Source discovery
CPP_SOURCES := $(shell find $(SRCDIR) -name '*.cpp')
C_SOURCES   := $(EXTDIR)/linenoise/linenoise.c

CPP_OBJECTS := $(patsubst %.cpp,$(OBJDIR)/%.o,$(CPP_SOURCES))
C_OBJECTS   := $(patsubst %.c,$(OBJDIR)/%.o,$(C_SOURCES))

OBJECTS := $(CPP_OBJECTS) $(C_OBJECTS)

# Default target
all: $(TARGET)

# Link final binary using C++ compiler
$(TARGET): $(OBJECTS)
	@mkdir -p $(BINDIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile C++ sources
$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

# Compile C sources
$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -c -o $@ $<

# Clean
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# compile_commands.json for clangd
compile_commands.json:
	bear -- make clean all

.PHONY: all clean compile_commands.json
