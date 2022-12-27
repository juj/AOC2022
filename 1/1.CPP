#include <stdio.h>
#include "../tick.h"

char *read_file_to_string(const char *filename)
{
	FILE *handle = fopen(filename, "r");
	fseek(handle, 0, SEEK_END);
	long size = ftell(handle);
	fseek(handle, 0, SEEK_SET);
	char *mem = (char*)malloc(size+1);
	fread(mem, size, 1, handle);
	mem[size] = 0;
	return mem;
}

long line_to_int(char *&s)
{
	char *q = s;
	while(*q > ' ') ++q;
	long val = (q != s) ? atol(s) : 0;
	s = (*q != 0) ? q+1 : q;
	*q = 0;
	return val;
}

long max[3] = { 0 };

void insert(unsigned long cal)
{
	if (cal > max[0])      max[2] = max[1], max[1] = max[0], max[0] = cal;
	else if (cal > max[1]) max[2] = max[1], max[1] = cal;
	else if (cal > max[2]) max[2] = cal;
}

void main()
{
	long t0 = tick();
	char *input = read_file_to_string("input.txt");
	long cur_calories = 0, num;
	while(*input)
	{
		cur_calories += (num = line_to_int(input));
		if (!num)
		{
			insert(cur_calories);
			cur_calories = 0;
		}
	}
	insert(cur_calories);
	printf("Most calories: %lu. Elapsed: %ld msecs.\n", max[0], tick()-t0);
	printf("Top three calories: %lu\n", max[0] + max[1] + max[2]);
}
