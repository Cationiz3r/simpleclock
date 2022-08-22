#include "spclock.hpp"

SimpleClock::SimpleClock() {
	running = true;
	force_draw = true;
}

void SimpleClock::draw_number(int n, int x, int y) {
	for (int i = 0; i < 15; ++i) {
		if (i % 3 == 0)	move(x, y + i/3);
		printf(number[n][i]? "██" : "  ");
	}
};
void SimpleClock::draw_colon(bool draw, int x, int y) {
	move(x, y + 1); printf(draw? "██": "  ");
	move(x, y + 3); printf(draw? "██": "  ");
}
void SimpleClock::draw_clock() {
	// Draw big numbers
	bool colon = !(t[2] % 2);
	draw_colon(colon, x + 15, y);
	draw_colon(colon, x + 34, y);
	draw_number(t[0] / 10, x,      y);
	draw_number(t[0] % 10, x + 7,  y);
	draw_number(t[1] / 10, x + 19, y);
	draw_number(t[1] % 10, x + 26, y);
	draw_number(t[2] / 10, x + 38, y);
	draw_number(t[2] % 10, x + 45, y);

	// Draw date
	int len = sizeof(datestr) / sizeof(datestr[0]);
	move(x + (WIN_W - len) / 2 + OFFSET_DATE_X, y + 6 + OFFSET_DATE_Y);
	printf("%s", datestr);

	// Flush buffer
	fflush(stdout);
}

void SimpleClock::update_time() {
	// Aquire time info
	time_t lt = time(NULL);
	tm = localtime(&(lt));
	t = {tm->tm_hour, tm->tm_min, tm->tm_sec};
}
void SimpleClock::update_date() {
	memset(datestr, 0, sizeof(datestr));
	char tmpstr[16];
	strftime(tmpstr, sizeof(tmpstr), "%Y/%m/%d", tm);
	sprintf(datestr, "%s", tmpstr);
}
void SimpleClock::update_terminfo() {
	struct winsize w;

	// Aquire terminal info
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	term_w = w.ws_col; term_h = w.ws_row;
	if (term_w < WIN_W || term_h < WIN_H) {
		// Terminal is too small to draw
		running = false;
	}
	if ( term_w != term_w_prev
		|| term_h != term_h_prev
		|| force_draw) {
		// Terminal size changes or initializtion
		clear();
		x = (term_w - WIN_W) / 2;
		y = (term_h - WIN_H) / 2;
		force_draw = true;
	}
}
void SimpleClock::update_history() {
	// Save previous info
	tp          = t;
	term_w_prev = term_w;
	term_h_prev = term_h;
}

void SimpleClock::update() {
	update_time();
	update_terminfo();
	if (!force_draw && t == tp) return;
	update_date();
	update_history();
}
void SimpleClock::draw() {
	draw_clock();
	force_draw = false;
}

void SimpleClock::key_event() {
	struct termios settings_old, settings;

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

void SimpleClock::run() {
	curoff();
	color(6);
	while (running) {
		update();
		draw();
		key_event();
	}
	curon();
	clear();
}
