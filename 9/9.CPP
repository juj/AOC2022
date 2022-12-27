#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>

void main(int argc, char **argv)
{
	long num_visited = 0;
	unsigned char huge *is_visited = (unsigned char huge*)farcalloc(131072, 1);
#define KNOTS 10
	int x[KNOTS], y[KNOTS], amount;
	char dir;

	for(int i = 0; i < KNOTS; ++i)
		x[i] = y[i] = 512;

	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	while(fscanf(handle, "%c %d\n", &dir, &amount) == 2)
	{
		while(amount--)
		{
			if      (dir == 'U') ++y[0];
			else if (dir == 'D') --y[0];
			else if (dir == 'L') --x[0];
			else /* dir == 'R'*/ ++x[0];

			for(int i = 1; i < KNOTS; ++i)
			{
#define ABS(x) ((x) >= 0 ? (x) : -(x))
#define SIGN(x) (((x) > 0) - ((x) < 0))
				int dx = x[i-1]-x[i];
				int dy = y[i-1]-y[i];
				if ((ABS(dx) | ABS(dy)) < 2)
					break;
				x[i] += SIGN(dx);
				y[i] += SIGN(dy);
			}
			long linear_idx = ((long)y[KNOTS-1] << 10) + x[KNOTS-1];
			long byte_idx = linear_idx >> 3;
			int mask = 1 << (linear_idx & 7);
			if (!(is_visited[byte_idx] & mask))
			{
				++num_visited;
				is_visited[byte_idx] |= mask;
			}
		}
	}
	printf("Num visited: %ld.\n", num_visited);
}
