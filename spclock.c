#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <time.h>

#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>

#define up(x)     printf("\e[%dA", (x))
#define down(x)   printf("\e[%dB", (x))
#define right(x)  printf("\e[%dC", (x))
#define left(x)   printf("\e[%dD", (x))
#define move(x,y) printf("\e[%d;%dH", (y+1), (x+1))
#define clear()   printf("\e[H\e[2J\e[3J")
#define curoff()  printf("\e[?25l")
#define curon()   printf("\e[?25h");
#define color(x)  printf("\e[%d;1m", (x+30))

#define WIN_W 51
#define WIN_H 7
#define OFFSET_X 0
#define OFFSET_Y 0
#define OFFSET_DATE_X 1
#define OFFSET_DATE_Y 0

const bool number[][15] = {
	{1,1,1,1,0,1,1,0,1,1,0,1,1,1,1}, /* 0 */
	{0,0,1,0,0,1,0,0,1,0,0,1,0,0,1}, /* 1 */
	{1,1,1,0,0,1,1,1,1,1,0,0,1,1,1}, /* 2 */
	{1,1,1,0,0,1,1,1,1,0,0,1,1,1,1}, /* 3 */
	{1,0,1,1,0,1,1,1,1,0,0,1,0,0,1}, /* 4 */
	{1,1,1,1,0,0,1,1,1,0,0,1,1,1,1}, /* 5 */
	{1,1,1,1,0,0,1,1,1,1,0,1,1,1,1}, /* 6 */
	{1,1,1,0,0,1,0,0,1,0,0,1,0,0,1}, /* 7 */
	{1,1,1,1,0,1,1,1,1,1,0,1,1,1,1}, /* 8 */
	{1,1,1,1,0,1,1,1,1,0,0,1,1,1,1}, /* 9 */
};

void draw_number(int n, int x, int y) {
	for (int i = 0; i < 15; ++i) {
		if (i % 3 == 0)	move(x, y + i/3);
		if (number[n][i]) printf("██");
		else printf("  ");
	}
};
void draw_colon(bool draw, int x, int y) {
	move(x, y + 1);
	if (draw) printf("██"); else printf("  ");
	move(x, y + 3);
	if (draw) printf("██"); else printf("  ");
}

struct winsize w;
struct sp_time {
	int h, m, s;
} time_prev;
bool time_equal(sp_time a, sp_time b) {
	return a.s == b.s;
}
int term_w_prev = -1, term_h_prev;
int x, y;
bool running = 1;

void draw_clock(sp_time t, char *date, int x, int y) {
	// Offset

	x += OFFSET_X;
	y += OFFSET_Y;

	// Draw big numbers
	bool colon = t.s % 2 == 0;
	draw_colon(colon, x + 15, y);
	draw_colon(colon, x + 34, y);
	draw_number(t.h / 10, x,     y);
	draw_number(t.h % 10, x + 7, y);
	draw_number(t.m / 10, x + 19, y);
	draw_number(t.m % 10, x + 26, y);
	draw_number(t.s / 10, x + 38, y);
	draw_number(t.s % 10, x + 45, y);

	// Draw date
	int len = strlen(date);
	move(x + (WIN_W - len) / 2 + OFFSET_DATE_X, y + 6 + OFFSET_DATE_Y);
	printf("%s", date);

	// Flush buffer
	fflush(stdout);
}

void display_clock() {
	// Aquire time info
	time_t lt = time(NULL);
	struct tm *tm;
	tm = localtime(&(lt));
	sp_time time;
	time.h  = tm->tm_hour;
	time.m  = tm->tm_min;
	time.s  = tm->tm_sec;

	// Aquire terminal info
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	int term_w = w.ws_col, term_h = w.ws_row;
	if (term_w < WIN_W || term_h < WIN_H) {
		// Terminal is too small to draw
		exit(1);
	}
	if (term_w != term_w_prev || term_h != term_h_prev || term_w_prev < 0) {
		// Terminal size changes or initializtion
		clear();
		x = (term_w - WIN_W) / 2;
		y = (term_h - WIN_H) / 2;
	} else if (time_equal(time, time_prev)) {
		return;
	}

	char datestr[12];
	strftime(datestr, sizeof(datestr), "%Y/%m/%d", tm);
	draw_clock(time, datestr, x, y);

	// Save previous info
	time_prev = time;
	term_w_prev = term_w;
	term_h_prev = term_h;
}

struct termios settings_old, settings;
void key_event() {
	tcgetattr(fileno( stdin ), &settings_old);
	settings = settings_old;
	settings.c_lflag &= (~ICANON & ~ECHO);
	tcsetattr(fileno(stdin), TCSANOW, &settings);

	fd_set set;
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 200000;

	FD_ZERO(&set);
	FD_SET(fileno(stdin), &set);
	int res = select(fileno( stdin )+1, &set, NULL, NULL, &tv);
	char c;
	if (res > 0) {
		read(fileno( stdin ), &c, 1);
	} else if (res < 0) fprintf(stderr, "Input error\n");
	tcsetattr(fileno(stdin), TCSANOW, &settings_old);

	switch (c) {
	case 'q':
	case 'Q':
		running = false;
		break;
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
		color(c - '1');
		break;
	}
}

int main() {
	curoff();
	color(6);
	while (running) {
		display_clock();
		key_event();
	}
	curon();
	clear();
}
