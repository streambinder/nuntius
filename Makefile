EXEC     = paperboy
CC       = gcc
CFLAGS   = -O3 -Wall
CFLAGS  += $(shell pkg-config --cflags gio-2.0 libnotify yaml-0.1 libcurl)
LDFLAGS  = -pthread
LDFLAGS += $(shell pkg-config --libs gio-2.0 libnotify yaml-0.1 libcurl)

SRC      = $(wildcard src/gtk/*.c) $(wildcard src/*.c)
OBJ      = $(SRC:.c=.o)

${EXEC}: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: all
all: $(EXEC)

.PHONY: install
ifndef DESTDIR
install: $(EXEC)
	@mkdir -p $(HOME)/.local/bin
	install -m 755 $(EXEC) $(HOME)/.local/bin
else
install: $(EXEC)
	install -m 755 $(EXEC) $(DESTDIR)/usr/sbin
endif

.PHONY: clean
clean:
	@rm -rf src/*.o src/gtk/*.o $(EXEC)