#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#define PART_1

char *extract_line(char *str)
{
	while(*str)
	{
		if (*str == '\n')
		{
			*str = 0;
			return str+1;
		}
		++str;
	}
	return str;
}

void main(int argc, char **argv)
{
	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	char *input = (char*)malloc(32768);
	long size = fread(input, 1, 32768, handle);
	input[size]=0;

	char *stackLines[9], *end = 0;
	int numStackLines = 0;

	while(!end)
	{
		char *nl = extract_line(input);
		if (!(end = strstr(input, "1"))) stackLines[numStackLines++] = input;
		input = nl;
	}

	int numStacks = (strlen(stackLines[numStackLines-1]) + 3) / 4;
	char **stack = (char**)malloc(sizeof(char*)*numStacks);
	int *stackLen = (int*)malloc(sizeof(int)*numStacks);

	for(int i = 0; i < numStacks; ++i)
	{
		stack[i] = (char*)malloc(sizeof(char)*numStacks*numStackLines);
		stackLen[i] = numStackLines;
		for(int j = 0; j < numStackLines; ++j)
		{
			char ch = stackLines[numStackLines-1-j][1 + 4*i];
			if (ch < 'A' || ch > 'Z')
			{
				stackLen[i] = j;
				break;
			}
			stack[i][j] = ch - 'A';
		}
	}

	while(*input)
	{
		char *nl = extract_line(input);
		int count, from, to;
		if (sscanf(input, "move %d from %d to %d", &count, &from, &to) == 3)
		{
			--from;
			--to;
			char *f = stack[from], *t = stack[to];
#ifdef PART_1
			for(int i = 0; i < count; ++i) t[stackLen[to]++] = f[--stackLen[from]];
#else
			for(int i = 0; i < count; ++i) t[stackLen[to]++] = f[stackLen[from]-count+i];
			stackLen[from] -= count;
#endif
		}
		input = nl;
	}
	for(i = 0; i < numStacks; ++i) printf("%c", 'A' + stack[i][stackLen[i]-1]);
	printf("\n");
}
