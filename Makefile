# variables
CC=gcc
CFLAGS=-std=c11 -Wall #-Werror
LDLIBS=-lm -lcurses
OUTPUT=hangman

# targets
all: $(OUTPUT)

$(OUTPUT): program.c
	cppcheck --enable=performance --error-exitcode=1 *.c
	$(CC) $(CFLAGS) program.c $(LDLIBS) -o program


