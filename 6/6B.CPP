#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// O(n) search of first all chars unique substr of length len, or -1 if none.
int find_first_unique_substr_index(const char *str, int len) {
	int last[256], next_try_idx = len - 1, skip_to;
	memset(last, -1, sizeof(last));
	for(int i = 0; str[i]; ++i) {
		if ((skip_to = last[str[i]]+len) > next_try_idx) next_try_idx = skip_to;
		else if (next_try_idx <= i) return i+1-len;
		last[str[i]] = i;
	}
	return -1;
}
void main(int argc, char **argv) {
	FILE *f = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	char *input = (char*)malloc(8192);
	fgets(input, 8192, f);
	printf("Marker pos: %d\n", find_first_unique_substr_index(input, 14) + 14);
}
