# Makefile
CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -Iinclude

SRCS=src/main.c src/bmp.c src/stego.c
OBJS=$(SRCS:.c=.o)
TARGET=hw-01_bmp

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)