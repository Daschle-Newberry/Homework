CFLAGS = -Wall -Wextra -O0 -g -fsanitize=address -lpthread -lm

ARGS = 

assignment6: assignment6.o
	cc $(CFLAGS) $< -o $@

%.o : %.c
	cc $(CFLAGS) -c $< -o $@

run: assignment6
	./assignment6 $(ARGS)

clean:
	rm *.o assignment6






