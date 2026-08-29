pairsofwords: main.o hash_table.o crc64.o getWord.o
	cc -o $@ $^

main.o: main.c hash_table.h
	cc $< -c

hash_table.o: hash_table.c hash_table.h
	cc $< -c

crc64.o: crc64.c crc64.h
	cc $< -c

getWord.o: getWord.c getWord.h
	cc $< -c

.PHONY: clean
clean:
	rm pairsofwords *.o