CC = gcc
CCFLAGS = -std=c99 -pedantic -Wall -fsanitize=address -lm
BUILDDIR = build
SRCDIR = src
EXEC = tree_exe

OBJS = sgtree.o main.o

${EXEC}: ${OBJS}
	${CC} ${CCFLAGS} -o ${BUILDDIR}/${EXEC} $(addprefix ${BUILDDIR}/,${OBJS})

sgtree.o: 
	${CC} ${CCFLAGS} -include ${SRCDIR}/sgtree.h -c ${SRCDIR}/sgtree.c -o ${BUILDDIR}/sgtree.o

main.o:
	${CC} ${CCFLAGS} -include ${SRCDIR}/sgtree.h -c ${SRCDIR}/main.c -o ${BUILDDIR}/main.o
clean:
	rm -f ${BUILDDIR}/${EXEC} ${BUILDDIR}/${OBJS}

run: ${EXEC}
	./${BUILDDIR}/${EXEC}

