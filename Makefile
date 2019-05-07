# Name of your program:
NAME=driver

# List of all .cpp source code files included in your program (separated by spaces):
SRCPATH=./
SRC=$(wildcard *.cpp)

OBJ=$(addprefix $(SRCPATH), $(SRC:.cpp=.o))

RM=rm -f
INCPATH=../includes
CPPFLAGS= -I $(INCPATH) -std=c++11 -g


all: $(OBJ)
	g++ $(OBJ) -o $(NAME) $(CPPFLAGS)

clean:
	-$(RM) *~
	-$(RM) *#*
	-$(RM) *swp
	-$(RM) *.core
	-$(RM) *.stackdump
	-$(RM) $(SRCPATH)*.o
	-$(RM) $(SRCPATH)*.obj
	-$(RM) $(SRCPATH)*~
	-$(RM) $(SRCPATH)*#*
	-$(RM) $(SRCPATH)*swp
	-$(RM) $(SRCPATH)*.core
	-$(RM) $(SRCPATH)*.stackdump

fclean: clean
	-$(RM) $(NAME)

re: fclean all

