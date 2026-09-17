CFLAGS = -Wall -Wextra -fanalyzer -fsanitize=address -g -lm
CC = gcc

ARGS =

determinant-calculator: determinant-calculator.o
	$(CC) $(CCFLAGS) $< -o determinant-calculator

%.o: %.c
	$(CC) $(CCFLAGS) -c $< -o $@

run: determinant-calculator
	./determinant-calculator $(ARGS)

.PHONY:
clean:
	rm determinant-calculator determinant-calculator.o
	
