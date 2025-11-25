CC      = gcc
CFLAGS  = -Wall -Wextra -O2

SRC     = fuzzer.c
OBJ     = $(SRC:.c=.o)
BIN     = fuzzer

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)

.PHONY: all clean
