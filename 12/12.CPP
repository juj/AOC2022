#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <alloc.h>

char *read_file_to_string(const char *filename)
{
	FILE *handle = fopen(filename, "r");
	fseek(handle, 0, SEEK_END);
	long size = ftell(handle);
	fseek(handle, 0, SEEK_SET);
	char *mem = (char*)malloc(size+1);
	size_t n = fread(mem, 1, size, handle);
	fclose(handle);
	mem[n] = 0;
	return mem;
}

#define QUEUE_LEN 4095
struct pt { unsigned char x, y; } queue[QUEUE_LEN+1];

void main(int argc, char **argv)
{
	char *map = read_file_to_string(argc > 1 ? argv[1] : "input.txt");
	for(int width = 0; map[width] != '\n'; ++width) /*nop*/;
	for(int size = 0; map[size]; ++size) /*nop*/;
	int stride = width + 1, height = (size + stride - 1) / stride;

//	printf("size: %d, width: %d, height: %d\n", size, width, height);
	*strstr(map, "S") = 'a';
	char *end = strstr(map, "E");
	*end = 'z';

	unsigned int *dist = (unsigned int*)malloc(size*sizeof(int));
	memset(dist, 0xFF, size*sizeof(int));

//#define QUEUE_LEN 4095

//	struct pt { unsigned char x, y; } *queue = (pt *)malloc((QUEUE_LEN+1)*sizeof(pt));

	int head = 0, tail = 0;

	for(int i = 0; i < size; ++i)
		if (map[i] == 'a')
		{
			dist[i] = 0;
			queue[head].x = i % stride;
			queue[head].y = i / stride;
			++head;
		}

	while(head != tail)
	{
		pt cur = queue[tail++];
		tail &= QUEUE_LEN;
		int curpos = cur.y*stride+cur.x;
		unsigned int dcur = dist[curpos];

		if (cur.x > 0 && map[curpos]+1 >= map[curpos-1] && dist[curpos-1] > dcur+1)
		{
			dist[curpos-1] = map[curpos-1] == 'a' ? 0 : dcur+1;
			queue[head].x = cur.x-1;
			queue[head++].y = cur.y;
			head &= QUEUE_LEN;
		}
		if (cur.y > 0 && map[curpos]+1 >= map[curpos-stride] && dist[curpos-stride] > dcur+1)
		{
			dist[curpos-stride] = map[curpos-stride] == 'a' ? 0 : dcur+1;
			queue[head].x = cur.x;
			queue[head++].y = cur.y-1;
			head &= QUEUE_LEN;
		}
		if (cur.x < width-1 && map[curpos]+1 >= map[curpos+1] && dist[curpos+1] > dcur+1)
		{
			dist[curpos+1] = map[curpos+1] == 'a' ? 0 : dcur+1;
			queue[head].x = cur.x+1;
			queue[head++].y = cur.y;
			head &= QUEUE_LEN;
		}
		if (cur.y < height-1 && map[curpos]+1 >= map[curpos+stride] && dist[curpos+stride] > dcur+1)
		{
			dist[curpos+stride] = map[curpos+stride] == 'a' ? 0 : dcur+1;
			queue[head].x = cur.x;
			queue[head++].y = cur.y+1;
			head &= QUEUE_LEN;
		}
	}
	printf("Distance: %u\n", dist[end-map]);
}
