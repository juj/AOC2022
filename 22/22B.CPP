#include <stdio.h>
#include <stdlib.h>
#include "../tick.h"
#define AT(x, y) input[y_table[(y)] + (x)]
#define S 50 // cube side length
char *input  = (char*)malloc(32768);
int *y_table = (int*)malloc(256*sizeof(int));
int *width   = (int*)malloc(256*sizeof(int)), height = 0;
struct pos { int x, y, h; };
enum { RIGHT, DOWN, LEFT, UP };

pos inc_x(int x, int y)
{
	if (x+1 < width[y] && AT(x+1, y) != ' ')
							 { pos p = {             x+1,       y, RIGHT }; return p; }
	if (y <   S) { pos p = {           2*S-1, 3*S-1-y,  LEFT }; return p; }
	if (y < 2*S) { pos p = { 3*S-1-(2*S-1-y),     S-1,    UP }; return p; }
	if (y < 3*S) { pos p = {           3*S-1, 3*S-1-y,  LEFT }; return p; }
							 { pos p = { 2*S-1-(4*S-1-y),   3*S-1,    UP }; return p; }
}
pos inc_y(int x, int y)
{
	if (y+1 < height && x < width[y+1] && AT(x, y+1) != ' ')
							 { pos p = {     x,             y+1, DOWN }; return p; }
	if (x <   S) { pos p = { 2*S+x,               0, DOWN }; return p; }
	if (x < 2*S) { pos p = {   S-1, 4*S-1-(2*S-1-x), LEFT }; return p; }
							 { pos p = { 2*S-1, 2*S-1-(3*S-1-x), LEFT }; return p; }
}
pos dec_x(int x, int y)
{
	if (x > 0 && AT(x-1, y) != ' ')
							 { pos p = {     x-1,       y,  LEFT }; return p; }
	if (y <   S) { pos p = {       0, 3*S-1-y, RIGHT }; return p; }
	if (y < 2*S) { pos p = {     y-S,     2*S,  DOWN }; return p; }
	if (y < 3*S) { pos p = {       S, 3*S-1-y, RIGHT }; return p; }
							 { pos p = { S+y-3*S,       0,  DOWN }; return p; }
}
pos dec_y(int x, int y)
{
	if (y > 0 && x < width[y-1] && AT(x, y-1) != ' ')
							 { pos p = {     x,     y-1,    UP }; return p; }
	if (x <   S) { pos p = {     S,     S+x, RIGHT }; return p; }
	if (x < 2*S) { pos p = {     0, 3*S+x-S, RIGHT }; return p; }
							 { pos p = { x-2*S,   4*S-1,    UP }; return p; }
}
typedef pos(*movefunc)(int x, int y);
const movefunc move[4] = { inc_x, inc_y, dec_x, dec_y };

void main()
{
	tick_t t0 = tick();
	FILE *handle = fopen("input.txt", "r");
	long size = fread(input, 1, 32768, handle);
	for(char *instr = input + size - 2; instr[-1] != '\n'; --instr) /*nop*/;

	for(char *s = input; s < instr; ++s, ++height)
	{
		char *start = s;
		y_table[height] = s - input; // compute start idx of each line
		while(*s != '\n') ++s;
		width[height] = s - start; // and width
	}
	--height; // account for newline between map and instructions

	pos cur = { 0, 0, RIGHT };
	for(s = input; *s != '.'; ++s) ++cur.x; // find starting x pos

	for(; instr < input + size; ++instr) // follow moves
	{
		int length = atoi(instr);
		while(length--)
		{
			pos next = move[cur.h](cur.x, cur.y);
			if (AT(next.x, next.y) == '#') break;
			cur = next;
		}
		while (*instr >= '0' && *instr <= '9') ++instr; // skip parsed digit
		if      (*instr == 'L') cur.h = (cur.h + 3) % 4; // turn left
		else if (*instr == 'R') cur.h = (cur.h + 1) % 4; // or right
	}
	printf("Final password: %lu. Elapsed: %f msecs.\n",
		1000ul * (cur.y+1) + 4*(cur.x+1) + cur.h, msecs(t0, tick()));
}
