#variables
CC = gcc

#Creation de l'executable
all: minish
	
minish: minishell.c minishell.h vars_manager.h parser.h utility.h
	$(CC) minishell.c -g -o minish -lreadline

install:
	cp minish /usr/bin
	apt-get install libreadline6 libreadline6-dev


uninstall:
	rm -f /usr/bin/minish

#clean file objects
clean:
	rm -f *.o
	rm -f *.h.gch
	rm minish
