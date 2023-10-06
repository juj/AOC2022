#include <stdio.h>
#include "../tick.h"
#define LINEAR_ADDRESS(x, y, z) (((long)(x)<<12)|((long)(y)<<6)|(long)(z))
#define IS_SOLID(pos)     (map[(pos)>>3]      & (1 << ((pos)&7)))
#define MARK_SOLID(pos)   (map[(pos)>>3]     |= (1 << ((pos)&7)))
#define IS_VISITED(pos)   (visited[(pos)>>3]  & (1 << ((pos)&7)))
#define MARK_VISITED(pos) (visited[(pos)>>3] |= (1 << ((pos)&7)))
void main(int argc, char **argv)
{
	tick_t t0 = tick();
	FILE *f = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	int x, y, z, maxX = 0, maxY = 0, maxZ = 0, sides = 0;
	unsigned char  *map = (unsigned char *)calloc(32768, 1);
	unsigned char  *visited = (unsigned char *)calloc(32768, 1);
	struct pt { int x, y, z; }  *stack = (pt *)calloc(5000,sizeof(pt));
	pt  *stop = stack+1;

	while(fscanf(f, "%d,%d,%d", &x, &y, &z) == 3)
	{
		if (++x > maxX) maxX = x;
		if (++y > maxY) maxY = y;
		if (++z > maxZ) maxZ = z;
		MARK_SOLID(LINEAR_ADDRESS(x,y,z));
	}
#define VISIT(X, Y, Z) { \
	long lin = LINEAR_ADDRESS((X), (Y), (Z)); \
	if (IS_SOLID(lin)) ++sides; \
	else if (!IS_VISITED(lin)) { \
		MARK_VISITED(lin); \
		stop->x = (X); stop->y = (Y); stop->z = (Z); ++stop; \
	} }
	MARK_VISITED(0);
	while(stop != stack)
	{
		pt p = *--stop;
		if (p.x > 0)     VISIT(p.x-1, p.y, p.z);
		if (p.x <= maxX) VISIT(p.x+1, p.y, p.z);
		if (p.y > 0)     VISIT(p.x, p.y-1, p.z);
		if (p.y <= maxY) VISIT(p.x, p.y+1, p.z);
		if (p.z > 0)     VISIT(p.x, p.y, p.z-1);
		if (p.z <= maxZ) VISIT(p.x, p.y, p.z+1);
	}
	printf("Surface area: %d. Elapsed: %f msecs.\n", sides, msecs(t0, tick()));
}
