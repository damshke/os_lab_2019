# Makefile

TARGETS=memory parallel_min_max psum
CC=gcc
CFLAGS=-I.

.PHONY: all clean

all: $(TARGETS)

psum: utils.o utils.h sum.o sum.h
	$(CC) -pthread -o psum utils.o sum.o parallel_sum.c  $(CFLAGS)

parallel_min_max: utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

sum.o : sum.h
	$(CC) -o sum.o -c sum.c $(CFLAGS)

memory:
	$(CC) -o memory process_memory.c $(CFLAGS)

libSUM4.a: sum.o
	ar rc libSUM4.a sum.o
	ranlib libSUM4.a
	
find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

clean :
	rm memory calculate_sum.o utils.o