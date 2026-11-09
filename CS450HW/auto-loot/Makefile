CFLAGS = -std=c11 -O2 -fno-omit-frame-pointer -Wall -Wextra -fsanitize=address -g -lm
CC = gcc

ARGS =

auto-loot: auto-loot.o
	$(CC) $(CFLAGS) $< -o auto-loot

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: auto-loot
	./auto-loot $(ARGS)

.PHONY:
clean:
	rm auto-loot auto-loot.o

