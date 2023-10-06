#include <stdio.h>

#include "../tick.h"
#define SZ 131072 // 17 bits + 3 bits = 20 bits -> 10 bits for x & y
#define SIDE 1024
#define GET(map, pos) (map[(pos)>>3]  & (1<<((pos)&7)))
#define SET(map, pos) (map[(pos)>>3] |= (1<<((pos)&7)))
#define LIN(x, y) (((y)<<10)|(x))
char  *map =    (char  *)calloc(SZ, 1);
char  *newmap = (char  *)calloc(SZ, 1);
char  *to =     (char  *)calloc(SZ, 1);
char  *to2 =    (char  *)calloc(SZ, 1);

int check_n(long x, long y) // returns 0 if can't move
{
	long idx = LIN(x-1,y-1); if (GET(map, idx)) return 0;
			 idx += 2;           if (GET(map, idx)) return 0;
			 --idx;              if (GET(map, idx)) return 0;
	if  (GET(to, idx)) SET(to2, idx);
	else SET(to, idx);
	return 1;
}
int check_s(long x, long y)
{
	long idx = LIN(x-1,y+1); if (GET(map, idx)) return 0;
			 idx += 2;           if (GET(map, idx)) return 0;
			 --idx;              if (GET(map, idx)) return 0;
	if  (GET(to, idx)) SET(to2, idx);
	else SET(to, idx);
	return 1;
}
int check_e(long x, long y)
{
	long idx = LIN(x+1,y-1); if (GET(map, idx)) return 0;
			 idx += (SIDE<<1);   if (GET(map, idx)) return 0;
			 idx -= SIDE;        if (GET(map, idx)) return 0;
	if  (GET(to, idx)) SET(to2, idx);
	else SET(to, idx);
	return 1;
}
int check_w(long x, long y)
{
	long idx = LIN(x-1,y-1); if (GET(map, idx)) return 0;
			 idx += (SIDE<<1);   if (GET(map, idx)) return 0;
			 idx -= SIDE;        if (GET(map, idx)) return 0;
	if  (GET(to, idx)) SET(to2, idx);
	else SET(to, idx);
	return 1;
}
typedef int (*checkfunc)(long x, long y);
const checkfunc checks[4] = { check_n, check_s, check_w, check_e };

int move_n(long x, long y) // return 0 if this wasn't our advertised direction
{
	long idx = LIN(x,y-1);
	if (!GET( to, idx)) return 0;
	if (!GET(to2, idx)) SET(newmap, idx);
	else { idx += SIDE; SET(newmap, idx); }
	return 1;
}
int move_s(long x, long y)
{
	long idx = LIN(x,y+1);
	if (!GET( to, idx)) return 0;
	if (!GET(to2, idx)) SET(newmap, idx);
	else { idx -= SIDE; SET(newmap, idx); }
	return 1;
}
int move_w(long x, long y)
{
	long idx = LIN(x-1,y);
	if (!GET( to, idx)) return 0;
	if (!GET(to2, idx)) SET(newmap, idx);
	else { ++idx;       SET(newmap, idx); }
	return 1;
}
int move_e(long x, long y)
{
	long idx = LIN(x+1,y);
	if (!GET( to, idx)) return 0;
	if (!GET(to2, idx)) SET(newmap, idx);
	else { --idx;       SET(newmap, idx); }
	return 1;
}
typedef int (*movefunc)(long x, long y);
const movefunc moves[4] = { move_n, move_s, move_w, move_e };

int has_elf(long x, long y)
{
	long idx = LIN(x,y); return GET(map, idx);
}

int being_bugged(long x, long y)
{
	long idx = LIN(x-1,y-1); if (GET(map, idx)) return 1;
			 ++idx;              if (GET(map, idx)) return 1;
			 ++idx;              if (GET(map, idx)) return 1;
			 idx += SIDE;        if (GET(map, idx)) return 1;
			 idx -= 2;           if (GET(map, idx)) return 1;
			 idx += SIDE;        if (GET(map, idx)) return 1;
			 ++idx;              if (GET(map, idx)) return 1;
			 ++idx;              return GET(map, idx);
}

void main(int argc, char **argv) {
	tick_t t0 = tick();
	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	char ch;
	long x = 512-36, y = 512-36;
	int minx = 2000, miny = 2000, maxx=0, maxy=0, cur_check = 0, num_elves = 0;
	while((ch = fgetc(handle)) != EOF)
	{
		if (ch == '#')
		{
			++num_elves;
			long pos = LIN(x, y);
			SET(map, pos);
			if (minx > x) minx = x;
			if (maxx < x) maxx = x;
			if (miny > y) miny = y;
			if (maxy < y) maxy = y;
		}
		++x;
		if (ch == '\n')
		{
			x = 512-36;
			++y;
		}
	}

	for(int turn = 0; /*turn < 10*/; ++turn)
	{
		for(long i = 0; i < 4; ++i)
		{
			memset(newmap + 32768*i, 0, 32768);
			memset(to + 32768*i, 0, 32768);
			memset(to2 + 32768*i, 0, 32768);
		}
		for(y = miny; y <= maxy; ++y)
			for(x = minx; x <= maxx; ++x)
				if (has_elf(x, y) && being_bugged(x, y))
					for(int i = 0; i < 4; ++i)
						if (checks[(i + cur_check)%4](x, y))
							break;

		int idle_elves = 0;
		for(y = miny; y <= maxy; ++y)
		{
			for(x = minx; x <= maxx; ++x)
			{
				if (!has_elf(x, y)) continue;
				if (!being_bugged(x, y))
				{
					++idle_elves;
					long idx = LIN(x, y);
					SET(newmap, idx);
					continue;
				}
				for(int i = 0; i < 4; ++i)
				{
					int h = (i + cur_check)%4;
					if (moves[h](x, y))
					{
						switch(h)
						{
							case 0: if (y == miny) --miny; break;
							case 1: if (y == maxy) ++maxy; break;
							case 2: if (x == minx) --minx; break;
							case 3: if (x == maxx) ++maxx; break;
						}
						break;
					}
					if (i == 3)
					{
						long idx = LIN(x, y);
						SET(newmap, idx);
					}
				}
			}
		}
		char  *tmp = newmap;
		newmap = map;
		map = tmp;
		++cur_check;
//		printf("Turn %d: num elves: %d, idle elves: %d, BB: %dx%d - %dx%d\n",
//			turn, num_elves, idle_elves, minx,miny, maxx, maxy);
		if (num_elves == idle_elves)
			break;
	}
	int newminx = 2000, newminy = 2000, newmaxx=0, newmaxy=0;
	for(y = miny; y <= maxy; ++y)
		for(x = minx; x <= maxx; ++x)
		{
			long pos = LIN(x, y);
			if (GET(map, pos))
			{
				if (newminx > x) newminx = x;
				if (newmaxx < x) newmaxx = x;
				if (newminy > y) newminy = y;
				if (newmaxy < y) newmaxy = y;
			}
		}
	printf("empty tiles:%d, rounds: %d\n",
		(newmaxx-newminx+1)*(newmaxy-newminy+1)-num_elves, turn+1);
	printf("Elapsed: %fmsec\n", msecs(t0, tick()));
}
