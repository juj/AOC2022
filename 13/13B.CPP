#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

int check(char *l, char *r)
{
	int l_is_array_of_one = 0, r_is_array_of_one = 0;
	while(*l && *r)
	{
		assert(l_is_array_of_one == 0 || r_is_array_of_one == 0);
		if (isdigit(*l) && isdigit(*r))
		{
			int a = atoi(l);
			int b = atoi(r);
			if (a < b) return 1;
			if (a > b) return 0;
			while(isdigit(*l)) ++l;
			while(isdigit(*r)) ++r;
		}
		else if (*l == ']' && r_is_array_of_one)
		{
			if (isdigit(*r)) return 1;
			++l;
			--r_is_array_of_one;
		}
		else if (*r == ']' && l_is_array_of_one)
		{
			if (isdigit(*l)) return 0;
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
		else if (*l == ']') return 1;
		else if (*r == ']') return 0;
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
	FILE *f = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	char l[256], r[256], null[4];
	int sum = 0, idx2 = 1, idx6 = 2;

	for(int i=1; fgets(l, 256, f) && fgets(r, 256, f); ++i, fgets(null, 4, f))
	{
		if (check(l, r)) sum += i;

		if      (check(l, "[[2]]")) ++idx2, ++idx6;
		else if (check(l, "[[6]]")) ++idx6;

		if      (check(r, "[[2]]")) ++idx2, ++idx6;
		else if (check(r, "[[6]]")) ++idx6;
	}
	printf("Sum: %d, idx2: %d, idx6: %d, mul: %d\n", sum, idx2, idx6, idx2*idx6);
}
