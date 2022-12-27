#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include <mem.h>
#include <conio.h>
#include <assert.h>

char *read_file_to_string(const char *filename)
{
	FILE *handle = fopen(filename, "r");
	fseek(handle, 0, SEEK_END);
	long size = ftell(handle);
	fseek(handle, 0, SEEK_SET);
	char *mem = (char*)malloc(size+1);
	size_t n = fread(mem, 1, size, handle);
	fclose(handle);
	mem[n] = 0;
	return mem;
}

unsigned char far *map;

#define SIZE 1023

#define GET(x,y) (map[((unsigned int)y)&SIZE]  & (1 << (x)))
#define SET(x,y) (map[((unsigned int)y)&SIZE] |= (1 << (x)))
#define CLR(x,y) (map[((unsigned int)y)&SIZE] &= ~(1 << (x)))

int test0(int x, int y) // ----
{
	if (x < 0 || x > 3) return 0;
	return !GET(x, y) && !GET(x+1, y) && !GET(x+2, y) && !GET(x+3, y);
}

void draw0(int x, int y) // ----
{
	SET(x,y);
	SET(x+1, y);
	SET(x+2, y);
	SET(x+3, y);
}

void clear0(int x, int y) // ----
{
	CLR(x,y);
	CLR(x+1, y);
	CLR(x+2, y);
	CLR(x+3, y);
}
//   -
int test1(int x, int y) //  ---
{                       //   -
	if (x < 0 || x > 4) return 0;
	return !GET(x+1, y)   && !GET(  x, y+1) && !GET(x+1, y+1)
			&& !GET(x+2, y+1) && !GET(x+1, y+2);
}

void draw1(int x, int y)
{
	SET(x+1,y);
	SET(x, y+1);
	SET(x+1, y+1);
	SET(x+2, y+1);
	SET(x+1, y+2);
}

void clear1(int x, int y)
{
	CLR(x+1,y);
	CLR(x, y+1);
	CLR(x+1, y+1);
	CLR(x+2, y+1);
	CLR(x+1, y+2);
}

//    -
int test2(int x, int y) //    -
{                       //  ---
	if (x < 0 || x > 4) return 0;
	return !GET(  x, y)   && !GET(x+1, y) && !GET(x+2, y)
			&& !GET(x+2, y+1) && !GET(x+2, y+2);
}

void draw2(int x, int y)
{
	SET(x,y);
	SET(x+1, y);
	SET(x+2, y);
	SET(x+2, y+1);
	SET(x+2, y+2);
}

void clear2(int x, int y)
{
	CLR(x,y);
	CLR(x+1, y);
	CLR(x+2, y);
	CLR(x+2, y+1);
	CLR(x+2, y+2);
}



												//  -
												//  -
int test3(int x, int y) //  -
{                       //  -
	if (x < 0 || x > 6) return 0;
	return !GET(  x, y) && !GET(x, y+1) && !GET(x, y+2) && !GET(x, y+3);
}

void draw3(int x, int y)
{
	SET(x, y);
	SET(x, y+1);
	SET(x, y+2);
	SET(x, y+3);
}

void clear3(int x, int y)
{
	CLR(x, y);
	CLR(x, y+1);
	CLR(x, y+2);
	CLR(x, y+3);
}


int test4(int x, int y) //  --
{                       //  --
	if (x < 0 || x > 5) return 0;
	return !GET(x, y) && !GET(x+1, y) && !GET(x, y+1) && !GET(x+1, y+1);
}

void draw4(int x, int y)
{
	SET(x,   y);
	SET(x+1, y);
	SET(x,   y+1);
	SET(x+1, y+1);
}

void clear4(int x, int y)
{
	CLR(x,   y);
	CLR(x+1, y);
	CLR(x,   y+1);
	CLR(x+1, y+1);
}

int main(int argc, char **argv)
{
	char *input = read_file_to_string(argc > 1 ? argv[1] : "input.txt");
	char *jet = input;

	map = (unsigned char far *)farmalloc(SIZE+1);
	assert(map);
	_fmemset(map, 0x80, SIZE+1);
	map[0] = 0xFF;

	struct
	{
		int (*test)(int x, int y);
		void (*draw)(int x, int y);
		void (*clear)(int x, int y);
		int height;
	} shape[5] =
	{
		{ test0, draw0, clear0, 1 },
		{ test1, draw1, clear1, 3 },
		{ test2, draw2, clear2, 3 },
		{ test3, draw3, clear3, 4 },
		{ test4, draw4, clear4, 2 }
	}, *curShape = shape;

	unsigned long totalLaps = 0;

	unsigned int height = 4;
//	for(unsigned long I = 0; I < 1000000; ++I)
//	for(unsigned long J = 0; J < 1000000; ++J)
	for(int i = 0; i < 2022; ++i)
	{
		int x = 2;

		unsigned int y = height;
		for(unsigned int k = 0; k < curShape->height; k = (k+1)&SIZE)
			map[(y+k)&SIZE] = 0x80;

		for(;;)
		{
			// x movement
			int newX = x;
			if (*jet++ == '>') ++newX; // apply jet
			else --newX;
			if (*jet <= 32) jet = input;
			if (curShape->test(newX, y)) x = newX;
			// y movement
			unsigned int newY = (y + SIZE) & SIZE;
			if (!curShape->test(x, newY))
			{
				curShape->draw(x, y);
				unsigned int newHt = y + curShape->height + 3;
				if (height < newHt)
				{
					if (height > SIZE)
					{
						height -= SIZE+1;
						++totalLaps;
					}
					height = newHt;
				}
				break;
			}
			y = newY;
		}
		if (++curShape == shape + 5) curShape = shape;
	}
	printf("Rock height: %d\n", height);

	return 0;
}
