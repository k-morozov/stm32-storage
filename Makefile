CC = gcc
CFLAGS = -g -Wall -o0 -Wextra -w
BIN = ./bin/
DIR = ./src/
BUILD = ./build/
OBJS = main.o init.o database.o test.o

main.out: $(OBJS)
	@echo "CC " $<
	$(CC) $(CFLAGS) $(addprefix $(BIN), $(OBJS))  -o $(addprefix $(BUILD),$@)

%.o: $(DIR)%.c
	$(CC) $(CFLAGS) -o $(BIN)$@ -c $<

.PHONY: clean

clean:
	rm $(OBJS)