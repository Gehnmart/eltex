.PHONY: all run_all clean

BUILD_DIR := ./build

all: exc1 exc2 exc3 exc4

exc1: exc_1.o general_io.o general_utils.o
	gcc exc_1.o general_io.o general_utils.o -o exc1

exc2: exc_2.o general_io.o general_utils.o
	gcc exc_2.o general_io.o general_utils.o -o exc2

exc3: exc_3.o general_io.o general_utils.o
	gcc exc_3.o general_io.o general_utils.o -o exc3

exc4: exc_4.o general_io.o general_utils.o
	gcc exc_4.o general_io.o general_utils.o -o exc4

%.o: %.c
	gcc -Wall -Werror -Wextra -c $<

run_all: exc1 exc2 exc3 exc4
	./exc1
	./exc2
	./exc3
	./exc4

clean:
	rm -f exc1 exc2 exc3 exc4
	rm -f *.o