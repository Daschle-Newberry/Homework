CC = gcc
CCFLAGS = -std=c99 -pedantic -Wall
BUILDDIR = build
SRCDIR = src
EXEC = tree_exe

OBJS = tree.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${BUILDDIR}/${EXEC} ${BUILDDIR}/${OBJS}

tree.o: 
	${CC} ${CCFLAGS} -c ${SRCDIR}/tree.c -o ${BUILDDIR}/tree.o

clean:
	rm -f ${BUILDDIR}/${EXEC} ${BUILDDIR}/${OBJS}

run: ${EXEC}
	./${BUILDDIR}/${EXEC}

