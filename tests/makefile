CC=gcc

SRC_PATH=.
OBJ_PATH=.obj
SRC=$(wildcard $(SRC_PATH)/*.c)
OBJ=$(patsubst $(SRC_PATH)/%,$(OBJ_PATH)/%,$(SRC:.c=.o))

INC= -lm -lcheck
LDFLAGS = -lm -lcheck
CFLAGS = -Wall -Wextra -std=c99

ALIB_SRC_PATH=../src
ALIB_SRC=$(wildcard $(ALIB_SRC_PATH)/*.c)
ALIB_OBJ=$(patsubst $(ALIB_SRC_PATH)/%,$(OBJ_PATH)/%,$(ALIB_SRC:.c=.o))

all: testing

$(OBJ_PATH)/%.o: $(ALIB_SRC_PATH)/%.c
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) 

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) 

alib: $(ALIB_OBJ)

testing: $(OBJ) $(ALIB_OBJ)
	$(CC) -o testing $^ $(LDFLAGS) $(CFLAGS)

.PHONY: clean
clean:
	rm -rf $(OBJ)
	rm -f testing
