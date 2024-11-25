CC = gcc
CFLAGS = -Wall -Werror -g
LDFLAGS = 

SRCS = cache.c main.c
OBJS = $(SRCS:.c=.o)
TARGET = cache_test

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
