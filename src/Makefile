MAKEFLAGS += --no-builtin-rules --no-builtin-variables

DIAG = -Wall -Wextra -Wpedantic
OFLAGS = -Ofast

CC := gcc
INCS := -Indjin
LIBS := -lm
CFLAGS := $(OFLAGS) $(DIAG) $(INCS) -MD -g
LDFLAGS := $(LDFLAGS) $(LIBS)

OBJS = \
	ndjin/types.o \
	ndjin/fen.o \
	ndjin/bb.o

GUI_OBJS = \
	gui/ui.o \
	gui/main.o

TEST = test

GUI = chess

.PHONY: all build clean demo

all: clean build demo

clean:
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d
	rm -f $(TEST) $(GUI)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(TEST): $(OBJS)
	$(CC) $(CFLAGS) -o $(TEST) $(OBJS) $(LDFLAGS)

$(GUI): $(OBJS) $(GUI_OBJS)
	$(CC) $(CFLAGS) -o $(GUI) $(OBJS) $(GUI_OBJS) $(LDFLAGS) -lraylib

build: $(OBJS) $(GUI_OBJS) $(GUI)

demo: clean build
	./$(GUI)
	# ./$(TEST)
