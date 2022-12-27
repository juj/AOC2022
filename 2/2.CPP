#include <stdio.h>
#include "../tick.h"

void main()
{
	long t0 = tick();
	FILE *handle = fopen("input.txt", "r");
	int score1 = 0, score2 = 0;
	char a, b;
	while(fscanf(handle, "%c %c\n", &a, &b) == 2)
	{
		a -= 'A'; b -= 'X';
		score1 += 1 + b   + ((4 + b - a) % 3)*3;
		score2 += 1 + b*3 + ((2 + b + a) % 3);
	}
	printf("Part 1 score: %d, part 2 score: %d. Elapsed: %ld msecs.\n", score1, score2, tick()-t0);
}
