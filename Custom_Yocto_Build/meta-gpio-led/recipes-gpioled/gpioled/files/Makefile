# Compiler and flags
CXX      ?= g++
CXXFLAGS ?= -Wall -O2
LDFLAGS  ?=

# Target binary
TARGET   = gpioled

# Sources and objects
SRCS = main.cpp led.cpp
OBJS = $(SRCS:.cpp=.o)

# Default rule
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp led.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET)
