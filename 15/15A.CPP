#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct interval
{
	long min, max;
};

int cmp(const void *a, const void *b)
{
	interval *A = (interval*)a;
	interval *B = (interval*)b;
//	printf("cmp: %ldx%ld vs %ldx%ld\n",
//		A->min, A->max, B->min, B->max);
	if (A->min < B->min) return -1;
	if (A->min > B->min) return 1;
	if (A->max < B->max) return -1;
	if (A->max > B->max) return 1;
	return 0;
}

interval intervals[256];

void main(int argc, char **argv)
{
	FILE *f = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	long sx, sy, bx, by, n = 0;
	const long targetY = argc > 2 ? atoi(argv[2]) : 2000000;
	long numBeaconsAtTargetY = 0;
	interval beacons[64];
	int numBeacons = 0;

	while(fscanf(f, "Sensor at x=%lu, y=%lu: closest beacon is at x=%lu, y=%lu\n",
		&sx, &sy, &bx, &by) == 4)
	{
		if (by == targetY)
		{
			beacons[numBeacons].min = bx;
			beacons[numBeacons].max = by;
			++numBeacons;
		}
		long dx = sx - bx, dy = sy - by;
		if (dx < 0) dx = -dx;
		if (dy < 0) dy = -dy;
		long dist = dx + dy;
		printf("s: %ld,%ld, b: %ld,%ld, dist: %ld\n", sx, sy, bx, by, dist);
		long distToTargetY = sy - targetY;
		if (distToTargetY < 0) distToTargetY = -distToTargetY;
		assert(dist >= 0);
		long widthAtTargetY = dist - distToTargetY;
		if (widthAtTargetY >= 0)
		{
			long minX = sx - widthAtTargetY;
			long maxX = sx + widthAtTargetY;
			printf("Width at target Y: %ld, %ld\n", minX, maxX);
			intervals[n].min = minX;
			intervals[n].max = maxX;
			++n;
			assert(n < 256);
		}
		else
		{
//			printf("Outside targetY\n");
		}
	}
	qsort(beacons, numBeacons, sizeof(interval), cmp);
	if (numBeacons > 0) numBeaconsAtTargetY = 1;
	for(int b = 1; b < numBeacons; ++b)
	{
		if (beacons[b].min != beacons[b-1].min)
			++numBeaconsAtTargetY;
	}
//	printf("Qsorting:\n");
	qsort(intervals, n, sizeof(interval), cmp);
//	printf("done\n");
	long visiblePositions = -numBeaconsAtTargetY;
	interval *prev = &intervals[0];
	for(int i = 1; i < n; ++i)
	{
//	printf("%d\n", i);
		interval *cur = &intervals[i];
		if (prev->max < cur->min)
		{
			printf("Visible: %ld -> %ld\n", prev->min, prev->max);
			visiblePositions += prev->max + 1 - prev->min;
			prev = cur;
		}
		else
		{
			if (cur->max > prev->max)
				prev->max = cur->max;
		}
	}
	printf("Visible: %ld -> %ld\n", prev->min, prev->max);
	visiblePositions += prev->max + 1 - prev->min;
	printf("Visible positions: %ld\n", visiblePositions);
}
