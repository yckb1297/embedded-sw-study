CC = gcc
CFLAGS = -Wall -Wextra -g

TARGET = app
SRCS = main.c
OBJS = main.o

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean: 
	rm -f $(TARGET) $(OBJS)
	@echo "Cleaned!!!"

run:
	@./$(TARGET)

.PHONY: clean run
