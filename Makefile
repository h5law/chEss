MAKEFLAGS += --no-builtin-rules --no-builtin-variables

DIAG = -Wall -Wextra -Wpedantic
OFLAGS = -Ofast

CC := gcc
INCS := -Isrc/ndjin
LIBS := -lm
CFLAGS := $(OFLAGS) $(DIAG) $(INCS) -MD -g
LDFLAGS := $(LDFLAGS) $(LIBS)

OBJS = \
    src/ndjin/bb.o \
	src/ndjin/fen.o \
	src/ndjin/perft.o \
	src/ndjin/types.o

GUI_OBJS = \
	src/gui/game.o \
	src/gui/main.o \
	src/gui/pre.o

NET_OBJS = \
	src/net/network.o \
	src/net/shared.o

BB = bb_test

GUI = chess

PERFT = perft

FEN = fen_test

NET = net_test

.PHONY: all build clean demo

all: clean build demo

clean:
	rm -f $(OBJS) $(GUI_OBJS) $(NET_OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) $(GUI_OBJS:.o=.d) $(NET_OBJS.o=.d) *.d */*.d */*/*.d
	rm -f $(GUI) $(PERFT) $(FEN) $(BB) $(NET)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(TEST): $(OBJS)
	$(CC) $(CFLAGS) -Isrc/ndjin -o $(TEST) $(OBJS) $(LDFLAGS)

$(NET): $(NET_TEST)
	$(CC) $(CFLAGS) -Isrc/net -o $(NET) $(NET_OBJS) $(LDFLAGS)

$(GUI): $(OBJS) $(GUI_OBJS)
	$(CC) $(CFLAGS) -Isrc/gui -Isrc/ndjin -Isrc/net -o $(GUI) $(OBJS) $(GUI_OBJS) $(NET_OBJS) $(LDFLAGS) -lraylib

$(PERFT):
	$(CC) -Ofast -Isrc/ndjin -D_PERFT_TEST -DNO_DEBUG=1 -o $(PERFT) $(wildcard src/ndjin/*.c) -lm
	./$(PERFT)

$(BB):
	$(CC) $(CFLAGS) -D_BB_TEST -o $(BB) $(wildcard src/ndjin/*.c) -lm
	./$(BB)

$(FEN):
	$(CC) $(CFLAGS) -D_FEN_TEST -o $(FEN) $(wildcard src/ndjin/*.c) -lm
	./$(FEN)

build: $(OBJS) $(GUI_OBJS) $(GUI)

demo: clean build
	./$(GUI)
