.SUFFIXES: .c .o

CC = gcc
CCFLAGS = -std=c99 -Wall -pedantic
EXEC = main
OBJS = main.o func.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS} -lm

.c.o:
	${CC} ${CCFLAGS} -c $<

run: ${EXEC}
	./${EXEC}

clean:
	rm -f ${EXEC} ${OBJS}

test: ${EXEC}
	./${EXEC} < test_input


freq.o: freq.c func.h
func.o: func.c func.h