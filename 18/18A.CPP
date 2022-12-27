#include <stdio.h>
#include <stdlib.h>
#include "../tick.h"
#define LINEAR_ADDRESS(x, y, z) (((unsigned long)(x) << 12) \
															 | ((unsigned long)(y) <<  6) \
															 |  (unsigned long)(z))
unsigned char *map = (unsigned char*)calloc(32768, 1);
int  get(unsigned long addr) { return (map[addr>>3]&(1 << (addr&7))) ? -1:1;}
void set(unsigned long addr) {        map[addr>>3] |= 1 << (addr&7); }

void main(int argc, char **argv)
{
	long t0 = tick();
	FILE *f = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	int sides = 0, x, y, z;
	while(fscanf(f, "%d,%d,%d", &x, &y, &z) == 3)
	{
		++x; ++y; ++z;
		set(LINEAR_ADDRESS(x,y,z));
		sides += get(LINEAR_ADDRESS(x-1,y,z));
		sides += get(LINEAR_ADDRESS(x+1,y,z));
		sides += get(LINEAR_ADDRESS(x,y-1,z));
		sides += get(LINEAR_ADDRESS(x,y+1,z));
		sides += get(LINEAR_ADDRESS(x,y,z-1));
		sides += get(LINEAR_ADDRESS(x,y,z+1));
	}
	printf("Droplet area: %d. Elapsed: %ld msecs.\n", sides, tick()-t0);
}
