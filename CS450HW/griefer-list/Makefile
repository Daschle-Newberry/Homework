CFLAGS = -std=c11 -O2 -fno-omit-frame-pointer -Wall -Wextra -fsanitize=address -g -lm
CC = gcc

ARGS =

griefer-list: griefer-list.o
	$(CC) $(CFLAGS) $< -o griefer-list

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: griefer-list
	./griefer-list $(ARGS)

.PHONY:
clean:
	rm griefer-list griefer-list.o

