all: calc

libcalc.a: add.o sub.o mul.o div.o
	ar rc libcalc.a add.o sub.o mul.o div.o

calc: main.o libcalc.a
	gcc main.o -L. -lcalc -o calc

%.o: %.c
	gcc -c $< -g

clean:
	rm -f libcalc.a calc
	rm -f *.o