#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>

char *input = (char*)malloc(32768);
int *y_table = (int*)malloc(256*sizeof(int));
int *width = (int*)malloc(256*sizeof(int));
int height;
#define AT(x, y) input[y_table[(y)]+(x)]

int prev_free_x(int x, int y)
{
	if (x > 0)
	{
		if (AT(x-1,y) == '.') return x-1;
		if (AT(x-1,y) == '#') return x;
		assert(AT(x-1, y) == ' ');
	}
	int new_x = width[y]-1;
	while(AT(new_x, y) == ' ') --new_x;
	if (AT(new_x,y) == '.') return new_x;
	if (AT(new_x,y) == '#') return x;
	assert(0);
	return x;
}

int next_free_x(int x, int y)
{
	if (x+1 < width[y])
	{
		if (AT(x+1,y) == '.') return x+1;
		if (AT(x+1, y) == '#') return x;
	}
	int new_x = 0;
	while(AT(new_x, y) == ' ') ++new_x;
	if (AT(new_x, y) == '.') return new_x;
	else return x;
}

int prev_free_y(int x, int y)
{
	if (y > 0)
	{
		if (x < width[y-1] && AT(x,y-1) == '.') return y-1;
		if (x < width[y-1] && AT(x,y-1) == '#') return y;
		assert(x >= width[y-1] || AT(x, y-1) == ' ');
	}
	int new_y = height-1;
	while(x >= width[new_y] || AT(x, new_y) == ' ') --new_y;
	if (AT(x,new_y) == '.') return new_y;
	if (AT(x,new_y) == '#') return y;
	printf("Char at %dx%d='%c'(%d)\n",
		x, new_y, (char)AT(x, new_y), (int)AT(x, new_y));
	assert(0);
	return y;
}

int next_free_y(int x, int y)
{
	if (y+1 < height)
	{
		if (x < width[y+1] && AT(x,y+1) == '.') return y+1;
		if (x < width[y+1] && AT(x, y+1) == '#') return y;
	}
	int new_y = 0;
	while(x >= width[new_y] || AT(x, new_y) == ' ') ++new_y;
	if (AT(x, new_y) == '.') return new_y;
	else
	{
		assert(AT(x, new_y) == '#');
		return y;
	}
}

void main(int argc, char **argv) {
	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	assert(input);
	long size = fread(input, 1, 32768, handle);
	input[size]=0;

	for(char *instr = input+size-2; *instr != '\n'; --instr) /*nop*/;
	++instr;
//	printf("Instructions: %s\n", instr);

	char *s = input;
	int y = 0;
	while(s < instr)
	{
		char *start = s;
		y_table[y] = s-input;
		while(*s != '\n') ++s;
		width[y] = s - start;
//		printf("y: %d, width: %d\n", y, width[y]);
		++s;
		++y;
	}
	assert(y < 256);
	height = y-1;
/*
	printf("Map height: %d\n", height);
	for(int i = 0; i < height; ++i)
		printf("y_table[%d] = %d, width: %d\n",
			i, y_table[i], width[i]);
*/
	for(y = 0; y < height; ++y)
	{
		for(int x = 0; x < width[y]; ++x)
			assert(AT(x,y) == ' ' || AT(x,y) == '.' || AT(x,y) == '#');
	}


	int x = 0;
	y = 0;
	int direction = 0; // 0-right
	for(s = input; *s != '.'; ++s) ++x;
	printf("Starting position: %dx%d\n", x, y);

#define IN_BOUNDS(x,y) (x >= 0 && y >= 0 && y < height && x < width[y])
	while(*instr > ' ')
	{
		int length = atoi(instr);
		while(isdigit(*instr)) ++instr;
		char facing = *instr++;
		printf("%dx%d, direction: %d, length: %d\n", x, y, direction, length);
		assert(IN_BOUNDS(x, y));
		switch(direction)
		{
		case 0: for(int i = 0; i < length; ++i)
		{
			int new_x = next_free_x(x, y);
			if (new_x == x) break;
			x = new_x;
		assert(IN_BOUNDS(x, y));
		}
		break;
		case 1: for(i = 0; i < length; ++i)
		{
			int new_y = next_free_y(x, y);
			if (new_y == y) break;
			y = new_y;
		assert(IN_BOUNDS(x, y));
		}
		break;
		case 2: for(i = 0; i < length; ++i)
		{
			int new_x = prev_free_x(x, y);
			if (new_x == x) break;
			x = new_x;
		assert(IN_BOUNDS(x, y));
		}
		break;
		case 3: for(i = 0; i < length; ++i)
		{
			int new_y = prev_free_y(x, y);
			if (new_y == y) break;
			y = new_y;
		assert(IN_BOUNDS(x, y));
		}
		break;
		}
		if (facing == 'L') direction = (direction + 3) % 4;
		else if (facing == 'R') direction = (direction + 1) % 4;
	}
	printf("Final password: %lu\n", 1000ul*(y+1)+4*(x+1)+direction);
}
