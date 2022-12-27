#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <conio.h>

int far *screen = (int far*)MK_FP(0xB800, 0);
long x = 1, cycle = 0, signal_sum = 0;

void tick()
{
	if ((++cycle + 20) % 40 == 0 && cycle <= 220)
	{
		signal_sum += cycle * x;
		gotoxy(1, 8);
		printf("Signal strength: %ld.", signal_sum);
	}

#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define DRAW(coord, ch) screen[((coord-1) / 40) * 80 + ((coord-1) % 40)] = 0x0700 + ch
	if (ABS(((cycle-1)%40) - x) <= 1) DRAW(cycle, '#');
	else DRAW(cycle, '.');
	gotoxy((cycle-1) % 40 + 2, (cycle-1)/40+1);
	delay(30);
}

void main(int argc, char **argv)
{
	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	char str[32];
	clrscr();
	gotoxy(1, 8);
	printf("Signal strength: %ld.", signal_sum);
	while(fgets(str, sizeof(str), handle))
	{
		tick(); // noop
		if (str[0] == 'a') // addx
		{
			tick();
			x += atoi(str + 5);
		}
	}
	gotoxy(1,9);
}
