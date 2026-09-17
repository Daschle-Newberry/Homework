CC = gcc
CCFLAGS = -std=c99 -pedantic -Wall -g -fsanitize=address
CCDEBUGFLAGS = -g -fsanitize=address

EXEC = gol

ARGS = 1 dot-3x5 0 0

OBJS = main.o gameOfLife.o gameOfLifeOutput.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS}

main.o: main.c gameOfLife.o gameOfLifeOutput.o
	${CC} ${CCFLAGS} -c main.c -o main.o

gameOfLife.o: gameOfLife.c gameOfLifeOutput.o
	${CC} ${CCFLAGS} -c gameOfLife.c -o gameOfLife.o

gameOfLifeOutput.o: gameOfLifeOutput.c
	${CC} ${CCFLAGS} -c gameOfLifeOutput.c -o gameOfLifeOutput.o

clean:
	rm -f ${EXEC} ${OBJS}

run: ${EXEC}
	./${EXEC}

test: ${EXEC}
	./${EXEC} ${ARGS}
