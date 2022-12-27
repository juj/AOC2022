#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#define VIEW_OFFSET 180
#define IS_FILLED(pos) (screen[(pos) >> 3] & (0x80 >> ((pos) & 7)))
#define FILL(pos) (screen[(pos) >> 3] |= 0x80 >> ((pos) & 7))

unsigned char far *screen = (unsigned char far *)MK_FP(0xA000, 0);
struct stack { long pos, step; } s[4096] = { 500 - VIEW_OFFSET, 0 }, *sp = s;
const int DIR_OFFSET[4] = { 0, 640, 639, 641 };

void set_video_mode(int mode)
{
	REGS r;
	r.x.ax = mode;
	int86(0x10, &r, &r);
}

void main(int argc, char **argv)
{
	FILE *f = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	char str[512];
	unsigned long x0, y0, x1, y1, maxy = 0, min, max, pos, grains = 0;
	set_video_mode(0x12);
	while(fgets(str, 512, f))
	{
		char *line = str;
		if (sscanf(line, "%lu,%lu", &x0, &y0) != 2) break;
		x0 -= VIEW_OFFSET;
#define NEXT(s) while(*s && *s != '>') ++s; if (*s == '>') s += 2;
		NEXT(line);
		while(sscanf(line, "%lu,%lu", &x1, &y1) == 2)
		{
			x1 -= VIEW_OFFSET;
			if (x0 == x1)
			{
				if (y0 < y1) { min = y0; max = y1; } else { min = y1; max = y0; }
				if (max > maxy) maxy = max;
				for(pos = min*640 + x0; min++ <= max; pos += 640) FILL(pos);
			}
			else
			{
				if (x0 < x1) { min = x0; max = x1; } else { min = x1; max = x0; }
				if (y1 > maxy) maxy = y1;
				for(pos = y0*640 + min; min++ <= max; ++pos) FILL(pos);
			}
			NEXT(line);
			x0 = x1;
			y0 = y1;
		}
	}
	for(pos = (maxy+2)*80; pos < (maxy+3)*80; ++pos) screen[pos] = 0xFF;

	while(sp >= s)
	{
		if (sp->step++ >= 3)
		{
			++grains;
			FILL(sp--->pos);
//			--sp;
			continue;
		}
		pos = sp->pos + DIR_OFFSET[sp->step];
		if (!IS_FILLED(pos))
		{
			(++sp)->pos = pos;
			sp->step = 0;
		}
	}
	set_video_mode(0x03);
	printf("Deposited %lu grains of sand.\n", grains);
}
