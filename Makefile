C=gcc
CFLAGS=-Wall -Werror
INCLUDE=-lncursesw

all: her

her: src/main.c
	$(C) $(CFLAGS) $(INCLUDE) src/*.c -o her

install: her
	sudo cp her /bin/

clean:
	rm -rf *.o her
