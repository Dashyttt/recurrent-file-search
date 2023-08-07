CC = gcc
CFLAGS = -O3 -Wall -Wextra -Werror
OBJS = lab11dabN32511.o

.PHONY: all clean

all: lab11dabN32511

lab1: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

lab1.o: lab11dabN32511.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) lab1dabN32511


