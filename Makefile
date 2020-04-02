EXEC     = paperboy
CC       = gcc
CFLAGS   = -std=c11 -lpthread -O3 -Wall -Wextra -Wpedantic -Wstrict-aliasing
CFLAGS  += $(shell pkg-config --cflags gio-2.0 yaml-0.1 libcurl)
LDFLAGS  = -pthread
LDFLAGS += $(shell pkg-config --libs gio-2.0 yaml-0.1 libcurl)

SRC      = $(wildcard src/*.c)
OBJ      = $(SRC:.c=.o)

${EXEC}: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: all
all: $(EXEC)

.PHONY: install
install: $(EXEC)
	@install -m 755 $(EXEC) $(HOME)/.local/bin/

.PHONY: clean
clean:
	@rm -rf src/*.o $(EXEC)