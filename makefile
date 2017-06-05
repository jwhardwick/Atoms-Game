CC = clang

CFLAGS  = -g -std=c11 -Wall -Werror -fsanitize=address
DEPS = atomgame.h

TARGET = atomgame

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) $(TARGET).c -o $(TARGET)
