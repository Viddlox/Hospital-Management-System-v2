# Program name
NAME = Hospital_Management_System

# Compiler and flags
CC = g++
CFLAGS = -Wall -std=c++17 -Iincludes -mavx2

# Source files and object files
SRCS = $(wildcard src/*.cpp)
OBJ_DIR = obj
OBJ = $(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Clean command
RM = rm -rf

# Rules
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME)

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
