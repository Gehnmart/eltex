TARGET = shop_sim
CC = gcc
CFLAGS = -g
PREPROCESSOR = 
LIBS = -lpthread

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< $(PREPROCESSOR)

clean:
	rm -f $(TARGET)
	rm -f *.o
	rm -f *.txt