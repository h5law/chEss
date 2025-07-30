MAKEFLAGS += --no-builtin-rules --no-builtin-variables

DIAG = -Wall -Wextra -Wpedantic
OFLAGS = -O2

CC := gcc
INCS := -I/usr/local/include
LIBS := -L/usr/local/lib64 -lraylib -lm
CFLAGS := $(OFLAGS) $(DIAG) $(INCS) -MD -g
LDFLAGS := $(LDFLAGS) $(LIBS)

SRCDIR := src

OBJS = \
	$(SRCDIR)/main.o \
	$(SRCDIR)/dsa.o \
	$(SRCDIR)/board.o \
	$(SRCDIR)/ui.o \
	$(SRCDIR)/players.o \
	$(SRCDIR)/moves.o

TARGET = chEss

.PHONY: all build clean demo

all: clean build demo

clean:
	rm -f $(OBJS) *.o */*.o */*/*.o
	rm -f $(OBJS:.o=.d) *.d */*.d */*/*.d

%.o: %.S
	$(CC) $(CFLAGS) -o $@ -c $<

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

build: $(TARGET)

demo: clean build
	./$(TARGET)
