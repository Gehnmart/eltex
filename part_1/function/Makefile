all: run_crack

exc2: exc_2.c
	gcc exc_2.c -fno-stack-protector -no-pie -o exc2

crack: crack.o
	gcc crack.o -o crack

create_crack: crack
	./crack > offset.txt

run_crack: exc2 create_crack
	./exc2 < offset.txt

%.o: %.c
	gcc -fno-stack-protector -no-pie -c $<

clean:
	rm -f exc2 crack offset.txt
	rm -f *.o