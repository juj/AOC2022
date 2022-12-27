#include <stdio.h>
#include <stdlib.h>
void main(int argc, char **argv) {
	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	char *input = (char*)malloc(16384), *visible = (char*)calloc(16384, 1);
	long size = fread(input, 1, 16384, handle), pos;
	for(char *s = input; *s != '\n'; ++s) /*nop*/;
	int width = s - input, stride = width + 1,
			height = (size + stride - 1) / stride, num_visible = 0;
#define MARK(x, y) { pos = y*stride+x; if (input[pos] > prev) { \
		prev = input[pos]; \
		if (!visible[pos]) { ++num_visible; visible[pos] = 1; } \
	} }
	for(int y = 0; y < height; ++y) {
		for(int x = 0,   prev = 0; x < width; ++x) MARK(x, y);
		for(x = width-1, prev = 0; x >= 0;    --x) MARK(x, y);
	}
	for(int x = 0; x < width; ++x) {
		for(int y = 0,    prev = 0; y < height; ++y) MARK(x, y);
		for(y = height-1, prev = 0; y >= 0;     --y) MARK(x, y);
	}
	printf("Num visible trees: %d\n", num_visible);
}
