#include <stdio.h>
#include <string.h>
int char_to_score(char ch) { return ch >= 'a' ? ch - 'a' : ch - 'A' + 26; }
int find_dup(char *input1, char *input2) {
	unsigned char a[52] = { 0 };
	while(input1 != input2) a[char_to_score(*input1++)]  = 1;
	while(*input2) {
		int score = char_to_score(*input2++);
		if (a[score]) return score + 1;
	}
} // unreachable

void main() {
	FILE *f = fopen("input.txt", "r");
	char sack[64];
	int score = 0;
	while(fgets(sack, 64, f))
		score += find_dup(sack, sack + (strlen(sack)>>1));
	printf("Score: %d\n", score);
}
