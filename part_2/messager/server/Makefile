TARGET = server
CC = gcc
CFLAGS = -g
LIBS =  -lpthread -lrt

SRCS := $(shell find . -name "*.c")
OBJS := $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(TARGET)

*.o: *.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)