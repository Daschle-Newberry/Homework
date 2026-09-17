CCFLAGS = -Wall -Wextra -g -fsanitize=address -fanalyzer

run-test: test
	./test

test: test.o lineNum.o
	cc $(CCFLAGS) $^ -o $@
	
%.o : %.c
	cc $(CCFLAGS) -c $< -o $@

.PHONY:
clean:
	rm *.o test

