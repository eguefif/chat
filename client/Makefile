NAME = client
CC = g++
RM = rm -f

_SRC = main.cpp client.cpp connexion.cpp display.cpp utils.cpp message.cpp command.cpp
_OBJ = $(_SRC:.cpp=.o)
_INC = client.h
SDIR = ./src/
ODIR = ./obj/
IDIR = -I ./src/
OBJ = $(addprefix $(ODIR), $(_OBJ))
SRC = $(addprefix $(SDIR), $(_SRC))
INC = $(addprefix $(IDIR), $(_INC))

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

$(ODIR)%.o: $(SDIR)%.cpp src/client.h
	$(CC) $(CFLAGS) $(IDIR) -c $< -o $@

.PHONY: clean fclean re
clean:
	$(RM) $(ODIR)*.o

fclean:
	$(RM) $(NAME)

re: fclean clean all
