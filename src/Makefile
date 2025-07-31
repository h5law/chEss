MAKEFLAGS += --no-builtin-rules --no-builtin-variables

DIAG = -Wall -Wextra -Wpedantic
OFLAGS = -Ofast

CC := gcc
INCS := -I. -Iengine
LIBS := -lm
CFLAGS := $(OFLAGS) $(DIAG) $(INCS) -MD -g
LDFLAGS := $(LDFLAGS) $(LIBS)

OBJS = \
	engine/test.o \
	engine/bb.o \
	fen.o \
	types.o

TEST = engine/engine_test

.PHONY: all build clean demo

all: clean build demo

clean:
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d
	rm -f $(TEST)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(TEST): $(OBJS)
	$(CC) $(CFLAGS) -o $(TEST) $(OBJS) $(LDFLAGS)

build: $(OBJS) $(TEST)

demo: clean build
	./$(TEST)
