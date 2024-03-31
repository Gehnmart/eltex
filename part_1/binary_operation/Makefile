all: exc1 exc2 exc3 exc4 

exc1: exc_1.o general_io.o
	gcc exc_1.o general_io.o -o exc1
	./exc1

exc2: exc_2.o general_io.o
	gcc exc_2.o general_io.o -o exc2
	./exc2

exc3: exc_3.o general_io.o
	gcc exc_3.o general_io.o -o exc3
	./exc3

exc4: exc_4.o general_io.o
	gcc exc_4.o general_io.o -o exc4
	./exc4

general_io.o: general_io.c
	gcc general_io.c -c

clean:
	rm -f exc1 exc2 exc3 exc4
	rm -f *.o