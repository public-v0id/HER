#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <ncurses.h>

int max2pow(int val) {
	int cur = 1;
	for (int i = 0; i < val; ++i) {
		if (cur >= val) {
			return i;
		}
		cur <<= 1;
	}
	return -1;
}

int max2powcols(int cols, int powfs) {
	int cur = 1;
	for (int i = 0; i < cols; ++i) {
		if (powfs+3+3*cur > cols) {
			return i-1;
		}
		cur <<= 1;
	}
	return -1;
}

int main(int argc, char **argv) {
	char* filename = NULL;
	for (int i = 1; i < argc; ++i) {
		if (argv[i][0] != '-') {
			filename = argv[i];
		}
	}
	if (filename == NULL) {
		fprintf(stderr, "Error! Filename not provided!\n");
		return 0;
	}
	int fd;
	struct stat statbuf;
	if ((fd = open(filename, O_RDWR)) < 0) {
		fprintf(stderr, "Error! Unable to open file!\n");
		return 0;
	}
	if (fstat(fd, &statbuf) < 0) {
		fprintf(stderr, "Error! Can't find file ending!\n");
		close(fd);
		return 0;
	}
	bool active = true;
	char* file;
	if ((file = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0)) == NULL) {
		fprintf(stderr, "Error! Can't map file to memory!\n");
		close(fd);
		return 0;
	}
	int maxpowfs = max2pow(statbuf.st_size) >> 2;
	int rows, cols;
	char hex[] = "0123456789ABCDEF";
	initscr();
	while (active) {	
		getmaxyx(stdscr, rows, cols);
		int maxpow = max2powcols(cols, maxpowfs);
		if (maxpow < 2) {
			continue;
		}
		int bytes_count = (1 << maxpow)-1;
		size_t totalrows = statbuf.st_size >> maxpow;
		int colsinfinal = statbuf.st_size & bytes_count;
		for (int currow = 0; currow < rows && currow <= totalrows; ++currow) {
			int staddr = currow << maxpow;
			for (int curcol = 0; curcol < maxpowfs; ++curcol) {
				mvaddch(currow, maxpowfs-1-curcol, hex[staddr & 0xF]);
				staddr >>= 4;
			}
			for (int curcol = 0; curcol <= bytes_count && (curcol < colsinfinal || currow < totalrows); ++curcol) {
				char symb = file[(currow << maxpow) + curcol];
				mvaddch(currow, maxpowfs+1+(curcol << 1), hex[symb>>4]);
				mvaddch(currow, maxpowfs+2+(curcol << 1), hex[symb&0xF]);
				mvaddch(currow, (bytes_count << 1)+maxpowfs+4+curcol, symb);
			}
		}
		move(rows-1, 0);
		refresh();
		int ch = getch();
		keypad(stdscr, TRUE);
		noecho();
		if (ch == 'q') {
			active = false;
		}
	}
	endwin();
	munmap(file, statbuf.st_size);
	close(fd);
}
