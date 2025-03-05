C=gcc
CFLAGS=-Wall -Werror
INCLUDE=-lncursesw

all: editor

editor: src/main.c
	$(C) $(CFLAGS) $(INCLUDE) src/*.c -o her

clean:
	rm -rf *.o her
