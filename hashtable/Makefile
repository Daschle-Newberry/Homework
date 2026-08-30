CCFLAGS = -fsanitize=address -fanalyzer -Wall -g -O0

main.o: main.c hash_table.h
	cc ${CCFLAGS} $< -c
	
hash_table.o: hash_table.c hash_table.h
	cc ${CCFLAGS}  $< -c

linkedlist.0: linkedlist.c linkedlist.h
	cc ${CCFLAGS} $< -c

crc64.o: crc64.c crc64.h
	cc ${CCFLAGS} $< -c

getWord.o: getWord.c getWord.h
	cc ${CCFLAGS} $< -c

test.o: test.c linkedlist.o hash_table.o
	cc ${CCFLAGS} $< -c

test: test.o linkedlist.o 
	cc ${CCFLAGS} -o $@ $^

run-test: test
	./test

pairsofwords: main.o hash_table.o crc64.o getWord.o
	cc ${CCFLAGS} -o $@ $^

.PHONY: clean
clean:
	rm pairsofwords *.o
