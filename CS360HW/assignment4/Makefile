CFLAGS = -Wall -Wextra -O0 -g -fsanitize=address

ARGS = 

assignment4 : assignment4.o
	cc $(CFLAGS) $< -o $@

%.o : %.c
	cc $(CFLAGS) -c $< -o $@

run: assignment4 
	./assignment4 $(ARGS)

clean:
	rm *.o assignment4






