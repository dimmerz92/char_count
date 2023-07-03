COMPILER = gcc
CFLAGS = -g -Wall -pedantic

EXES = char_count

all: ${EXES}

char_count: char_count.c
		${COMPILER} ${CFLAGS} char_count.c -o char_count

clean:
		rm -f *~ *.0 ${EXES}
