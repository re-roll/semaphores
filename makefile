CC=gcc
CFLAGS=-std=c99 -Wall -Wextra -Werror -pedantic
TARGET=proj2

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	rm $(TARGET)
	rm $(TARGET).out
