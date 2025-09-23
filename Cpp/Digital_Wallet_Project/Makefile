# ===========================
# Project Makefile
# ===========================

# Compiler settings
CXX      = g++
CXXFLAGS = -std=c++20 -Wall -I include -g -O0 -MMD -MP
LDFLAGS  =

# Directories
SRCDIR   = src
BUILDDIR = build
INCDIR   = include

# Sources and objects
SRCS = $(wildcard $(SRCDIR)/*.cpp)
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(BUILDDIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)   # dependency files

# Final executable
TARGET = main

# Phony targets
.PHONY: all build run clean

# Default target
all: build

# Build executable
build: $(TARGET)
	@echo "<=============== MAKEFILE BUILD ===============>"

# Link step
$(TARGET): $(OBJS)
	$(CXX) $^ -o $@ $(LDFLAGS)
	
# Compile step: .cpp -> .o
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp | $(BUILDDIR)
	$(CXX) $< -c -o $@ $(CXXFLAGS)

# Ensure build dir exists
$(BUILDDIR):
	mkdir -p $@

# Run program
run: $(TARGET)
	@echo "<=============== MAKEFILE RUN ===============>"
	./$(TARGET)

# Clean build artifacts
clean:
	rm -f $(OBJS) $(TARGET) $(DEPS)

# Include dependency files if they exist
-include $(DEPS)
