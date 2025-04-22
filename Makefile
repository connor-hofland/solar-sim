# Makefile for building with MSVC cl
# Requires nmake

# Compiler and linker
CC = cl
LINK = link

# Source files
SRC = src/main.cpp src/glad.c

# Include directories
INCLUDE_DIRS = /Iinclude

# Library directory
LIB_DIR = lib

# Libraries
LIBS = glfw3.lib opengl32.lib user32.lib gdi32.lib shell32.lib

# Output executable
OUT = main.exe

# Compiler and linker flags
CFLAGS = /MD $(INCLUDE_DIRS)
LFLAGS = /link /LIBPATH:$(LIB_DIR) $(LIBS)

# Build target
all:
	$(CC) $(SRC) $(CFLAGS) $(LFLAGS) /Fe$(OUT)

# Clean target
clean:
	del *.obj
	del $(OUT)