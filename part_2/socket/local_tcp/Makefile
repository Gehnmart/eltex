TARGET_1 = local_tcp_client
TARGET_2 = local_tcp_server
CC = gcc
CFLAGS = -g
LIBS = 

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