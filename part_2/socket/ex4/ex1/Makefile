TARGET_1 = client
CC = gcc
CFLAGS = -g
LIBS = 

all: $(TARGET_1)

$(TARGET_1): $(TARGET_1).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(TARGET_1)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET_1)
	rm -f *.o
	rm -f *.txt