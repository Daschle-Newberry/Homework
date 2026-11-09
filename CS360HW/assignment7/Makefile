CFLAGS = -Wall -Wextra -O0 -g -fsanitize=address -lpthread -lm

ARGS = 

assignment7: main.o assignment7.o
	cc $(CFLAGS) $^ -o $@

%.o : %.c
	cc $(CFLAGS) -c $< -o $@

run: assignment7
	./assignment7 $(ARGS)

clean:
	rm *.o assignment7






