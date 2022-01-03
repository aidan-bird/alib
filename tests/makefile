CC=gcc

SRC_PATH=.
OBJ_PATH=.obj
SRC=$(wildcard $(SRC_PATH)/*.c)
EXE=$(patsubst %.c,%,$(SRC))
OBJ=$(patsubst $(SRC_PATH)/%,$(OBJ_PATH)/%,$(SRC:.c=.o))

INC= -lm -lcheck
LDFLAGS = -lm -lcheck
CFLAGS = -Wall -Wextra -std=c99 -ggdb3 -g3

ALIB_SRC_PATH=../src
ALIB_SRC=$(wildcard $(ALIB_SRC_PATH)/*.c)
ALIB_OBJ=$(patsubst $(ALIB_SRC_PATH)/%,$(OBJ_PATH)/%,$(ALIB_SRC:.c=.o))

all: $(EXE)

$(OBJ_PATH)/%.o: $(ALIB_SRC_PATH)/%.c
	$(CC) -DALIB_TESTING -c $(INC) -o $@ $< $(CFLAGS) 

$(OBJ_PATH)/%.o: $(SRC_PATH)/%.c
	$(CC) -c $(INC) -o $@ $< $(CFLAGS) 

alib: $(ALIB_OBJ)

$(EXE): %: $(OBJ_PATH)/%.o $(ALIB_OBJ)
	@echo $@
	@echo $^
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(OBJ) $(ALIB_OBJ) $(EXE)
