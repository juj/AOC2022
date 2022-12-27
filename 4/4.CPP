#include <stdio.h>

void main()
{
	FILE *f = fopen("input.txt", "r");
	int contained = 0, overlap = 0, as, ae, bs, be;
	while(fscanf(f, "%d-%d,%d-%d", &as, &ae, &bs, &be) == 4)
	{
		contained += (as >= bs && ae <= be) || (bs >= as && be <= ae);
		overlap += (as <= be && bs <= ae);
	}
	printf("Ranges contained: %d, overlapping: %d\n", contained, overlap);
}
