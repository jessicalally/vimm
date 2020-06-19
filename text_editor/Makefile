CC      = gcc
CFLAGS  = -Wall -g -D_DEFAULT_SOURCE -std=c99 -Werror -pedantic
PROGS   = editor test
LDFLAGS	= -lncurses

.SUFFIXES: .c .o

.PHONY: all clean

all: editor test

editor:          editor.o trie.o queue.o common.o auto_complete.o vector.o parser.o
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)
vector_test:     vector_test.o vector.o
vector.o:        vector.h
editor.o:        editor.h vector.h
parser.o:        parser.h trie.h vector.h
test:            test.o trie.o queue.o auto_complete.o common.o parser.o vector.o
test.o:          test.h trie.h queue.h auto_complete.h common.h parser.h vector.h
auto_complete.o: auto_complete.h queue.h trie.h common.h
queue.o:         queue.h trie.h common.h
trie.o:          trie.h common.h
common.o:        common.h

clean:
		rm -f $(wildcard *.o) $(PROGS) *~ gmon.out callgrind.out.*
