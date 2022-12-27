#include <stdio.h>
#include <string.h>

int from_snafu(char ch)
{
	if (ch == '=') return -2;
	if (ch == '-') return -1;
	return ch ? ch - '0' : 0;
}

char to_snafu(int i)
{
	if (i == -2) return '=';
	if (i == -1) return '-';
	return '0' + i;
}

void main(int argc, char **argv)
{
	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	char n[64] = { 0 }, m[64] = { 0 };
	while(fgets(m+1, sizeof(m)-1, handle))
	{
		char *i = n+62, *j = m + strlen(m+1);
		if (*j == '\n') --j;
		int carry = 0;
		while(*i || *j || carry)
		{
			int sum = from_snafu(*i) + from_snafu(*j) + carry;
			if      (sum < -2) { carry = -1; sum += 5; }
			else if (sum >  2) { carry =  1; sum -= 5; }
			else carry = 0;
			*i-- = to_snafu(sum);
			if (*j) --j;
		}
	}
	char *c = n; while(!*c) ++c;
	printf("%s\n", c);
}
