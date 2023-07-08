COMPILER = gcc
CFLAGS = -g0 -Wall -pedantic

EXES = char_count

all: ${EXES}

char_count: char_count.c
		${COMPILER} ${CFLAGS} char_count.c -o char_count

run :
		./char_count 5 text_files

clean:
		rm -f *~ *.0 ${EXES}
