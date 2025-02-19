# Program name - The output executable file
NAME = Hospital_Management_System.exe

# Compiler and flags
CC = g++  # The C++ compiler being used

# Compiler flags:
# -Wall: Enables all warnings
# -std=c++17: Specifies C++17 standard
# -Iincludes -Iincludes/external -Iincludes/classes: Includes directories for header files
# -mavx2 -m64: Enables AVX2 (Advanced Vector Extensions) and compiles for 64-bit architecture
# -fsanitize=address -g: (Commented) Used for memory debugging; enables address sanitizer and debugging symbols
CFLAGS = -Wall -std=c++17 -Iincludes -Iincludes/external -Iincludes/classes -mavx2 -m64 -fsanitize=address -g

# Linker flags:
# -lncursesw: Links the ncurses library (wide-character version)
# -pthread: Enables multi-threading support
# -lformw -lmenuw: Links the ncurses form and menu libraries (wide-character version)
# -pedantic: Enforces strict compliance with the standard
# -fsanitize=address: (Commented) Used for memory debugging
LDFLAGS = -lncursesw -pthread -lformw -lmenuw -pedantic -fsanitize=address

# Source files - Finds all C++ source files inside the 'src' directory
SRCS = $(wildcard src/*.cpp)

# Object files directory
OBJ_DIR = obj

# Converts source file names into corresponding object file names
# - Example: src/main.cpp -> obj/main.o
OBJ = $(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Clean command - Used to remove files and directories
RM = rm -rf

# Default target: Builds the executable
all: $(NAME)

# Rule for linking the final executable
# - Takes all object files and links them into the final executable
$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

# Rule for compiling source files into object files
# - Creates the object file directory if it doesn't exist
# - Compiles each .cpp file into a corresponding .o file
$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(@D)  # Ensures the directory exists
	$(CC) $(CFLAGS) -c $< -o $@  # Compiles source file into object file

# Rule to remove all object files and clean the build
clean:
	$(RM) $(OBJ_DIR)

# Rule to remove all compiled files, including the final executable
fclean: clean
	$(RM) $(NAME)

# Rule to fully clean and rebuild the project
re: fclean all

# Marks these targets as 'phony' (not actual files)
.PHONY: all clean fclean re
