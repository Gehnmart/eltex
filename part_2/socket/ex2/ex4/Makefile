TARGET_1 = server
TARGET_2 = clientudp
TARGET_3 = clienttcp
CC = gcc
CFLAGS = -g -Wall -Werror -Wextra
LIBS = 

all: $(TARGET_1) $(TARGET_2) $(TARGET_3)

$(TARGET_1): $(TARGET_1).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(TARGET_1)

$(TARGET_2): $(TARGET_2).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(TARGET_2)

$(TARGET_3): $(TARGET_3).o
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(TARGET_3)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET_1) $(TARGET_2) $(TARGET_3)
	rm -f *.o
	rm -f *.txt