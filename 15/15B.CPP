#include <stdio.h>
#include <stdlib.h>
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MAPX(x, y) ((x) + (y)) // Maps to 45 rotated coordinate system
#define MAPY(x, y) (-(x) + (y))
#define UNMAPX(x, y) (((x) - (y)) >> 1) // Unmaps back to original coords
#define UNMAPY(x, y) (((x) + (y)) >> 1)

struct Reading  { long x, y, r; }; // Input data
struct Interval { long min, max; }; // Ranges seen by sensors

int sortCmpReading(const void *a, const void *b)
{
	long d = ((Reading*)a)->y - ((Reading*)b)->y;
	return (d > 0) - (d < 0); // Sort input data by ascending y-visibility
}

int sortCmpInterval(const void *a, const void *b) // sorts intervals by min
{
	long d = ((Interval*)a)->min - ((Interval*)b)->min;
	return (d > 0) - (d < 0);
}

void report(long x, long y, long MAX_SZ)
{
	long X = UNMAPX(x, y), Y = UNMAPY(x, y);
	if (X >= 0 && Y >= 0 && X <= MAX_SZ && Y <= MAX_SZ)
		printf("Solution: %ldx%ld.\n", X, Y);
}

void main(int argc, char **argv)
{
	FILE *f = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	Reading readings[64], *firstReading = readings;
	long numReadings = 0, skipY, sx, sy, bx, by, jump;
	long MAX_SZ = argc > 2 ? atol(argv[2]) : 4000000;
	while(fscanf(f,"Sensor at x=%ld, y=%ld: closest beacon is at x=%ld, y=%ld\n",
							 &sx, &sy, &bx, &by) == 4)
	{
		long radius = ABS(sx - bx) + ABS(sy - by);
		readings[numReadings].x = MAPX(sx, sy - radius);
		readings[numReadings].y = MAPY(sx, sy - radius);
		readings[numReadings++].r = radius << 1;
	}

	qsort(readings, numReadings, sizeof(Reading), sortCmpReading);
	Interval intervals[64];
	for(long targetY=MAPY(MAX_SZ,0); targetY<=MAPY(0,MAX_SZ); targetY+=skipY+1)
	{
		int numIntervals = 0;
		skipY = 0x7FFFFFFF;
		for(Reading *r = firstReading; r != readings + numReadings; ++r)
		{
			if (r->y > targetY)
				if (r == firstReading) { ++firstReading; continue; }
				else break;
			if (r->y + r->r < targetY) continue;
			if (skipY > (jump = r->y + r->r - targetY)) skipY = jump;
			intervals[numIntervals].min = r->x;
			intervals[numIntervals++].max = r->x + r->r;
		}

		qsort(intervals, numIntervals, sizeof(Interval), sortCmpInterval);
		if (intervals[0].min > MAPX(0, targetY))
			report(intervals[0].min - 1, targetY, MAX_SZ);
		long maxEnd = intervals[0].max;
		for (Interval *i = intervals + 1; i != intervals + numIntervals; ++i)
			if (maxEnd < i->max)
			{
				if (maxEnd < i->min) report(maxEnd + 1, targetY, MAX_SZ);
				maxEnd = i->max;
			}
		if (maxEnd < MAPX(MAX_SZ, targetY)) report(maxEnd+1, targetY, MAX_SZ);
	}
}
