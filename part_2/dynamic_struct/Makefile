all: abonent_note

abonent_note: additional.o abonent_note_ui.o main.o abonent_note.o
	gcc additional.o abonent_note.o abonent_note_ui.o main.o -o abonent_note

%.o: %.c
	gcc -c $< -g

clean:
	rm -f abonent_note
	rm -f *.o