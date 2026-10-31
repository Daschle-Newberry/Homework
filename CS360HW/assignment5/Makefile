CFLAGS = -Wall -Wextra -O0 -g -fsanitize=address -lm

ARGS = 

assignment5 : assignment5.o
	cc $(CFLAGS) $< -o $@

%.o : %.c
	cc $(CFLAGS) -c $< -o $@

run: assignment5 
	./assignment5 $(ARGS)

clean:
	rm *.o assignment5






