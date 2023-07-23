COMPILER = gcc
CFLAGS = -Wall -pedantic
COBJS = char_count.o parse_args.o get_files.o parallel.o counting.o print_hist.o
CEXES = char_count

all: ${CEXES}

%.o: %.c headers.h makefile
	${COMPILER} ${CFLAGS} -c $< -o $@

${CEXES}: ${COBJS}
	${COMPILER} ${CFLAGS} ${COBJS} -o ${CEXES}

run :
		./char_count 5 text_files

clean:
		rm -f *.o *~ ${CEXES}
