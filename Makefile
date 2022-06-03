CC := gcc
CFLAGS := -g -O2 -Wall -Werror -pthread -std=c99 -D_DEFAULT_SOURCE
LDFLAGS := -pthread

# You can add additional tests here to have them be run by make test.
# Each test should have a corresponding .c file in tests/.
TESTS := binary_semaphore counting_semaphore single_producer
TESTS += multiple_producers destroy_pcq_retrieve

all: src/csesem.o src/pcq.o

test: $(patsubst %,test_%, $(TESTS))
	@for test in $^; do            \
	    printf "%-40s: " "$$test"; \
	    ./$$test;                  \
	done;                          \
	exit 0

clean:
	rm -f *~ src/*~ src/*.o tests/*~ tests/*.o
	rm -f $(patsubst %,test_%,$(TESTS)) fifo.tar

submission: fifo.tar

fifo.tar: Makefile src/csesem.c src/pcq.c
	tar cf fifo.tar $^

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

test_%: tests/%.o src/csesem.o src/pcq.o
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: all clean test
