#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

const char packet1[] = "[[2]]";
const char packet2[] = "[[6]]";

int check(char *l, char *r)
{
	int l_is_array_of_one = 0, r_is_array_of_one = 0;
	unsignedl long
	while(*l && *r)
	{
		assert(l_is_array_of_one == 0 || r_is_array_of_one == 0);
//		printf("%d,%d\n%s\n%s\n", l_is_array_of_one, r_is_array_of_one, l, r);
		printf("%c | %c, l: %d, r: %d\n", *l, *r, l_is_array_of_one, r_is_array_of_one);
		if (isdigit(*l) && isdigit(*r))
		{
			int a = atoi(l);
			int b = atoi(r);
			if (a < b) { printf("a<b, 1\n"); return 1; }
			if (a > b) { printf("b<a, 0\n"); return 0; }
			while(isdigit(*l)) ++l;
			while(isdigit(*r)) ++r;
 //			if (*l == ',' && r_is_array_of_one) { printf("0: right ran out\n"); return 0; }
	//		if (*r == ',' && l_is_array_of_one) { printf("1: left ran out\n"); return 1; }
 //			if (*l == ',') ++l;
	//		if (*r == ',') ++r;
//			if (l_is_array_of_one) --l_is_array_of_one;
 //			if (r_is_array_of_one) --r_is_array_of_one;
		}
		else if (*l == ']' && r_is_array_of_one)
		{
			if (isdigit(*r)) { printf("r still has elems\n"); return 1; }
			++l;
			--r_is_array_of_one;
		}
		else if (*r == ']' && l_is_array_of_one)
		{
			if (isdigit(*l)) { printf("l still has elems\n"); return 0; }
			++r;
			--l_is_array_of_one;
		}
		else if (*l == *r && (*l == '['))
		{
			assert(l_is_array_of_one == 0);
			assert(r_is_array_of_one == 0);
			++l; ++r;
		}
		else if (*l == *r && *l == ',')
		{
			if (l_is_array_of_one) return 1;
			if (r_is_array_of_one) return 0;
			++l; ++r;
		}
		else if (*l == *r && *l == ']')
		{
			if (l_is_array_of_one) --l_is_array_of_one;
			else ++l;
			if (r_is_array_of_one) --r_is_array_of_one;
			else ++r;
		}
		else if (*l == '[' && isdigit(*r))
		{
			++r_is_array_of_one;
			++l;
		}
		else if (*r == '[' && isdigit(*l))
		{
			++l_is_array_of_one;
			++r;
		}
//		else if (*l == ',')
//			++l;
//		else if (*r == ',')
//			++r;
		else if (*l == ']')
		{
			printf("1: left array ended\n");
			return 1;
		}
		else if (*r == ']')
		{
			printf("0: right array ended\n");
			return 0;
		}
		else
		{
			printf("unknown: l: %s, r: %s\n", l, r);
			return -1;
		}
	}
	printf("Equal?\n");
	return 0;
}

void main(int argc, char **argv)
{
	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	char left[256], right[256];
	unsigned long index = 1;
	unsigned long sum = 0;
	while(fgets(left, 256, handle) && fgets(right, 256, handle))
	{
		printf("origl: %s, origr: %s\n", left, right);
		int ret = check(left, right);
		if (ret == -1) printf("%s vs %s\n", left, right);
		if (ret)
		{
			sum += index;
	 //		printf("In right order.\n");
		}
		fgets(left, 256, handle);
		++index;
	}
	printf("Sum: %lu\n", sum);
}
