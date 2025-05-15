CC=gcc
CFLAGS=-Wall -Wextra -g -Iglad/include $(shell sdl2-config --clfags)
LIB=$(shell sdl2-config --cflags --libs) -lGL -ldl -lm
SRC=main.c \
	input/input.c \
	utils/utils.c \
	glad/src/glad.c

OBJ=$(SRC:.c=.o)
EXEC=main.out
all: $(OBJ)
	$(CC) -o $(EXEC) $+ $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean:
	rm $(EXEC) $(OBJ)
