#include <stdio.h>
#include <stdlib.h>

long scenic_score(char *map, int w, int h, int s, int x, int y)
{
	char cur = map[y*s+x];
	for(int T = y-1; T > 0   && map[T*s+x] < cur; --T) /*nop*/;
	for(int B = y+1; B < h-1 && map[B*s+x] < cur; ++B) /*nop*/;
	for(int L = x-1; L > 0   && map[y*s+L] < cur; --L) /*nop*/;
	for(int R = x+1; R < w-1 && map[y*s+R] < cur; ++R) /*nop*/;
	return (long)(y-T) * (B-y) * (x-L) * (R-x);
}

void main(int argc, char **argv)
{
	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	char *input = (char*)malloc(16384);
	long size = fread(input, 1, 16384, handle), best = 0, score;
	for(char *s = input; *s != '\n'; ++s) /*nop*/;
	int width = s-input, stride = width+1, height = (size+stride-1) / stride;
	for(int y = 1; y < height-1; ++y)
		for(int x = 1; x < width-1; ++x)
			if (best < (score = scenic_score(input, width, height, stride, x, y)))
				best = score;
	printf("Best visibility amount: %ld\n", best);
}
