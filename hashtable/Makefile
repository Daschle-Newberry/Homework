CCFLAGS = -fsanitize=address -fanalyzer -Wall -g -pg -O0
#CCFLAGS = -g -p

BUILDDIR = build
SRCDIR = src

SRCS = $(SRCDIR)/hash_table.c $(SRCDIR)/linkedlist.c $(SRCDIR)/crc64.c $(SRCDIR)/getWord.c
MAIN_SRCS = $(SRCDIR)/main.c
TEST_SRCS = $(SRCDIR)/test.c

#https://stackoverflow.com/questions/28559338/makefile-to-convert-all-c-to-o
OBJS = $(SRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
MAIN_OBJS = $(MAIN_SRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
TEST_OBJS = $(TEST_SRCS:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)

ARGS =

pairsofwords: $(MAIN_OBJS) $(OBJS)
	cc $(CCFLAGS) -o $(BUILDDIR)/$@ $^

test: $(TEST_OBJS) $(OBJS)
	cc $(CCFLAGS) -o $(BUILDDIR)/$@ $^

#https://stackoverflow.com/questions/34621364/makefile-compile-o-from-c-files
$(MAIN_OBJS) $(TEST_OBJS) $(OBJS): $(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIR)
	cc $(CCFLAGS) -c $< -o $@

run-test: test
	./$(BUILDDIR)/test

run-comparison: pairsofwords
	cp build/pairsofwords tests/executables
	python3 tests/test.py

run: pairsofwords
	./$(BUILDDIR)/pairsofwords $(ARGS)

zip: 
	cp src/* autolab
	zip -r autolab.zip autolab
.PHONY: clean
clean:
	rm -r $(BUILDDIR)
