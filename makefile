CC=gcc

SRC_PATH=src
OBJ_PATH=.obj
SRC=$(wildcard $(SRC_PATH)/*.c)
OBJ=$(patsubst $(SRC_PATH)/%,$(OBJ_PATH)/%,$(SRC:.c=.o))

INC= -lm

LDFLAGS = -lm
CFLAGS = -ggdb3 -Og -Wall -Wextra -pedantic-errors -fstrict-aliasing -std=c99

all: debug

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) 

debug: $(OBJ)
	$(CC) -o testing $^ $(LDFLAGS) $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJ)
	rm -f testing
