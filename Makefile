EXEC     = paperboy
CC       = gcc
CFLAGS   = -std=gnu11 -O3 -Wall -Wextra -Wpedantic -Wstrict-aliasing
CFLAGS  += $(shell pkg-config --cflags gio-2.0)
LDFLAGS  = $(shell pkg-config --libs   gio-2.0)

SRC      = $(wildcard src/*.c)
OBJ      = $(SRC:.c=.o)

${EXEC}: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: all
all: $(EXEC)

.PHONY: clean
clean:
	@rm -rf src/*.o $(EXEC)