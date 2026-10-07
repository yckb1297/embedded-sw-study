CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = copy
SRCS = main.c
OBJS = $(SRCS:.c=.o)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -f $(TARGET) $(OBJS)
	@echo "cleaned!"

.PHONY: clean

