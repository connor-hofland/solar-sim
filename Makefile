# Compiler
CXX = g++

# Source files
SRC = src/main.cpp src/glad.c

# Include directories
INCLUDE_DIRS = -Iinclude

# Library directories
LIB_DIRS = -Llib

# Libraries
LIBS = -lglfw -lGL -lm -ldl -lpthread
ifeq ($(OS),Windows_NT)
    RM = del
    OUT = main.exe
else
    RM = rm -f
    OUT = main
endif

# (rest of Makefile stays the same, just replace 'rm' with '$(RM)' and 'main' with '$(OUT)')


# Compiler and linker flags
CXXFLAGS = -std=c++11 $(INCLUDE_DIRS)
LDFLAGS = $(LIB_DIRS) $(LIBS)

# Build target
all:
	$(CXX) $(SRC) $(CXXFLAGS) $(LDFLAGS) -o $(OUT)

# Clean target
clean:
	$(RM) -f *.o $(OUT)
