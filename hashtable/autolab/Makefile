
CCFLAGS = 

SRCS = hash_table.c linkedlist.c crc64.c getWord.c
MAIN_SRCS = main.c

#https://stackoverflow.com/questions/28559338/makefile-to-convert-all-c-to-o
OBJS = $(SRCS:%.c=%.o)
MAIN_OBJS = $(MAIN_SRCS:%.c=%.o)
TEST_OBJS = $(TEST_SRCS:%.c=%.o)

pairsofwords: $(MAIN_OBJS) $(OBJS)
	cc $(CCFLAGS) -o $@ $^

#https://stackoverflow.com/questions/34621364/makefile-compile-o-from-c-files
$(MAIN_OBJS) $(TEST_OBJS) $(OBJS): %.o: %.c
	cc $(CCFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -r *.o pairsofwords
