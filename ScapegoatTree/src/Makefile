CC = gcc
CCFLAGS = -std=c99 -pedantic -Wall
EXEC = tree_exe

OBJS = sgtree.o main.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${EXEC} ${OBJS} -lm

sgtree.o: 
	${CC} ${CCFLAGS} -include sgtree.h -c sgtree.c -o sgtree.o -lm

main.o:
	${CC} ${CCFLAGS} -include sgtree.h -c main.c -o main.o -lm
clean:
	rm -f ${EXEC} ${OBJS}

run: ${EXEC}
	./${EXEC}

