CC = gcc
CCFLAGS = -std=c99 -pedantic -Wall
EXEC = freq

OBJS = freq.o func.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS}

freq.o: freq.c func.h
	${CC} ${CCFLAGS} -c freq.c

func.o: func.c func.h
	${CC} ${CCFLAGS} -c func.c

clean:
	rm -f ${EXEC} ${OBJS}

run: ${EXEC}
	./${EXEC}
