CC = gcc
CFLAGS = -Wall -Wextra -g
INCLUDE = -Icli-lib/include

SRC_DIR = cli-lib/src
SRCS = $(SRC_DIR)/keyboard.c $(SRC_DIR)/screen.c $(SRC_DIR)/timer.c snake_game.c

all: snake_game

snake_game: $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDE) -o snake_game $(SRCS)

clean:
	rm -f snake_game
