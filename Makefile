# Program name
NAME = Hospital_Management_System.exe

# Compiler and flags (f-sanitize commented for memory trace debugging)
CC = g++
CFLAGS = -Wall -std=c++17 -Iincludes -Iincludes/external -Iincludes/classes -mavx2 -m64 #-fsanitize=address -g

# ncurses linking flags (f-sanitize commented for memory trace debugging)
LDFLAGS = -lncursesw -pthread -lformw -lmenuw -pedantic #-fsanitize=address

# Source files and object files
SRCS = $(wildcard src/*.cpp)
OBJ_DIR = obj
OBJ = $(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(SRCS))

# Clean command
RM = rm -rf

# Rules
all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $(NAME) $(LDFLAGS)

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ_DIR)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re
