#include "common.h"
#include "support.h"
#include "screen.h"

#if CRT_ENABLED
unsigned long fg_color = COLOR_WHITE;
unsigned long bg_color = COLOR_BLACK;
int cx = 0;
int cy = 0;

extern unsigned char font[256][12];
static char screen[CHARSY][CHARSX];

void put_char_xy(int x, int y, char c)
{
	int i, j;
	screen[y][x] = c;
	x *= CHAR_WIDTH;
	y *= CHAR_HEIGHT;
	for (i = 0; i < CHAR_HEIGHT; i++) {
		int t = font[(unsigned char)c][i];
		for (j = 0; j < CHAR_WIDTH; j++) {
			PUT_PIXEL(x + j, y + i, (t & 1) ? fg_color : bg_color);
			t >>= 1;
		}
	}
}

static void scroll(void)
{
	int x, y;
#if 1
	for (y = 1; y < CHARSY; y++)
		for (x = 0; x < CHARSX; x++)
			put_char_xy(x, y - 1, screen[y][x]);
#else
	memcpy((unsigned char *)FB_BASE_ADDR,
	       ((unsigned char *)FB_BASE_ADDR) + RESX * CHAR_HEIGHT,
	       (RESY - CHAR_HEIGHT) * RESX);
	memcpy(&screen[0][0], &screen[1][0], (CHARSY - 1) * CHARSX);
#endif
	for (x = 0; x < CHARSX; x++)
		put_char_xy(x, CHARSY - 1, ' ');
	cy--;
}

void screen_putc(char c)
{
	int t;
	switch (c) {
	case '\n':
		cy++;
		cx = 0;
		if (cy >= CHARSY)
			scroll();
		break;
	case '\r':
		cx = 0;
		break;
	case '\t':
		for (t = 0; t < 8 - (cx & 7); t++)
			screen_putc(' ');
		break;
	case '\b':
		if (cx > 0)
			cx--;
		put_char_xy(cx, cy, ' ');
		break;
	default:
		put_char_xy(cx, cy, c);
		cx++;
		if (cx >= CHARSX)
			screen_putc('\n');
		break;
	}
}

void screen_clear()
{
	memset((unsigned char *)FB_BASE_ADDR, bg_color, RESX * RESY);
	memset(&screen[0][0], ' ', CHARSX * CHARSY);
	cx = cy = 0;
}

void screen_puts(char *s)
{
	while (*s) {
		screen_putc(*s);
		s++;
	}
}

void screen_init()
{
	screen_clear();
	SET_PALLETE(COLOR_BLACK, 0, 0, 0);
	SET_PALLETE(COLOR_WHITE, 127, 127, 127);

	/* Set screen offset */
	*((unsigned long *)CRT_BUFFER_REG) = FB_BASE_ADDR;

	/* Turn screen on */
	*((unsigned long *)CRT_REG) = 0x00000001;
}

#endif /* CRT_ENABLED */
