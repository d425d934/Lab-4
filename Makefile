CC     = gcc
CFLAGS = -Wall -g

all: task1 task2

task1: task1.o
	$(CC) $^ -o $@

task2: task2.o
	$(CC) $^ -o $@ -lm

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o task1 task2

.PHONY: all clean
