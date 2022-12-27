#include <stdio.h>
#include <stdlib.h>

void main(int argc, char **argv)
{
	FILE *f = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	char *str = (char*)malloc(8192), *s = str;
	fgets(str, 8192, f);
	for(;;)
		if      (s[3] == s[2]) s += 3; // skip 3? (..xx)
		else if (s[2] == s[1] || s[3] == s[1]) s += 2; // skip 2? (.x.x or .xx)
		else if (s[1] == s[0] || s[2] == s[0] || s[3] == s[0]) ++s; // skip 1?
		else break; // s[0]...s[3] are unique
	printf("Start-of-marker position: %d\n", s + 4 - str);
}
