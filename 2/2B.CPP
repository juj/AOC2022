#include <stdio.h>

void main()
{
	FILE *handle = fopen("input.txt", "r");
	int score = 0;
	char a, b;
	while(fscanf(handle, "%c %c\n", &a, &b) == 2)
		score += 1 + (b-'X')*3 + ((2 + (a-'A') + (b-'X')) % 3);
	printf("Total score: %d\n", score);
}
