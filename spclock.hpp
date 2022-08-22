#ifndef SPCLOCK_HPP
#define SPCLOCK_HPP

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

#include <array>

#define up(x)     printf("\e[%dA", (x))
#define down(x)   printf("\e[%dB", (x))
#define right(x)  printf("\e[%dC", (x))
#define left(x)   printf("\e[%dD", (x))
#define move(x,y) printf("\e[%d;%dH", (y+1), (x+1))
#define clear()   printf("\e[H\e[2J")
#define curoff()  printf("\e[?25l")
#define curon()   printf("\e[?25h");
#define color(x)  printf("\e[%d;1m", (x+30))

#define WIN_W 51
#define WIN_H 7
#define OFFSET_DATE_X 6
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

class SimpleClock {
private:
	std::array<int, 3> t, tp; //{h, m, s}
	int term_w, term_h;
	int term_w_prev, term_h_prev;
	int x, y;
	bool running, force_draw;
	char datestr[20];
	struct tm *tm;
public:
	SimpleClock();
	void draw_number(int n, int x = 0, int y = 0);
	void draw_colon(bool draw, int x = 0, int y = 0);
	void draw_clock();
	void update_time();
	void update_date();
	void update_terminfo();
	void update_history();
	void update();
	void draw();
	void key_event();
	void run();
};

#endif
