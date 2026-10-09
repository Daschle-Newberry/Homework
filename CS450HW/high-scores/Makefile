CFLAGS = -std=c11 -O2 -fno-omit-frame-pointer -Wall -Wextra -g -lm
CC = gcc

ARGS =

high-scores: high-scores.o
	$(CC) $(CFLAGS) $< -o high-scores

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: high-scores
	./high-scores $(ARGS)

.PHONY:
clean:
	rm high-scores high-scores.o

