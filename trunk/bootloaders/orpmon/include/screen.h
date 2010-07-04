#ifndef SCREEN_H
#define SCREEN_H

#if CRT_ENABLED
#define RESX 640
#define RESY 480
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 12
#define COLOR_BLACK 0x00
#define COLOR_WHITE 0xFF

#define CHARSX (RESX/CHAR_WIDTH)
#define CHARSY (RESY/CHAR_HEIGHT)

#define CRT_REG        (CRT_BASE_ADDR + 0)
#define CRT_PALLETE    (CRT_BASE_ADDR + 0x400)
#define CRT_BUFFER_REG (CRT_BASE_ADDR + 4)
#define PUT_PIXEL(x, y, color) (*(((unsigned char *)FB_BASE_ADDR) + (y) * RESX + (x)) = (color))
#define SET_PALLETE(i, r, g, b) (*(((unsigned long *)CRT_PALLETE) + (i)) = (((r) >> 3) << 11) | (((g) >> 2) << 5) | (((b) >> 3) << 0))

void put_char_xy (int x, int y, char c);
void put_char (char c);
void put_string (char *s);
void screen_clear (void);
void screen_init (void);
void screen_putc (char); 

extern unsigned long fg_color;
extern unsigned long bg_color;
extern int cx;
extern int cy;

#endif /* CRT_ENABLED */
#endif

