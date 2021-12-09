
NAME = SameBinary
BOOST = -lboost_system -lboost_filesystem
GCC = clang++ -std=c++17
FLAGS = -Wall -pedantic -Wextra
SOURCE = main.cpp
.PHONY: all
all: $(NAME)

$(NAME) : $(SOURCE)
	@$(GCC) $(BOOST) $(FLAGS) $(SOURCE) -o $(NAME) 

clean:
	@ rm -rf $(NAME)
	
fclean: clean
	@rm -rf $(NAME)
	
re: fclean all
