.PHONY: all run clean libs
CC = gcc
FLAGS = -g

TARGET_SRC = main.c io.c lib_controller.c
TARGET_OBJ = $(TARGET_SRC:.c=.o)
TARGET = calc

LIB_DIR = ./lib
LIB_SRC = $(wildcard $(LIB_DIR)/*.c)
LIB_OBJ = $(LIB_SRC:.c=.o)
LIB_EXEC = $(LIB_OBJ:.o=.so)

all: run

$(TARGET): $(TARGET_OBJ)
	gcc -o $(TARGET) $(TARGET_OBJ) -ldl -g -Wl,-rpath=$(PWD)/$(LIB_DIR)

run: $(LIB_EXEC) $(TARGET)
	LD_LIBRARY_PATH=$(PWD)/$(LIB_DIR):$(LD_LIBRARY_PATH) ./$(TARGET)

%.o: %.c
	$(CC) -c -o $@ $< $(FLAGS)

%.so: %.o
	gcc -shared -fPIC -o $@ $<

clean:
	rm -f $(TARGET) $(TARGET_OBJ) *.so
	rm -f $(LIB_OBJ) $(LIB_EXEC)
