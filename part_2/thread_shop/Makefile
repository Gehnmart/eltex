TARGET = shop_sim
CC = gcc
CFLAGS = -g 
LIBS = -lpthread

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET)
	rm -f *.o
	rm -f *.txt