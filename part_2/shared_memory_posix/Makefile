TARGET_1 = client
TARGET_2 = server
CC = gcc
CFLAGS = -g
LIBS = -lrt -lpthread

all: $(TARGET_1) $(TARGET_2)

$(TARGET_1): $(TARGET_1).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(TARGET_1)

$(TARGET_2): $(TARGET_2).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(TARGET_2)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET_1) $(TARGET_2)
	rm -f *.o
	rm -f *.txt