TARGET = client
CC = gcc
CFLAGS = -g 
LIBS = -lrt -lncurses -lpthread

SRCS := $(shell find . -name "*.c")
OBJS := $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ $(LIBS) -o $(TARGET)

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)