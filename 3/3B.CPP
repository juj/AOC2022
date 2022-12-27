#include <stdio.h>
int char_to_score(char ch) { return ch >= 'a' ? ch - 'a' : ch - 'A' + 26; }
int find_dup(char *input1, char *input2, char *input3) {
	unsigned char a[52] = { 0 };
	while(*input1) a[char_to_score(*input1++)]  = 1;
	while(*input2) a[char_to_score(*input2++)] |= 2;
	while(*input3) {
		int score = char_to_score(*input3++);
		if (a[score] == 3) return score + 1;
	}
} // unreachable

void main() {
	FILE *f = fopen("input.txt", "r");
	char sack1[64], sack2[64], sack3[64];
	int score = 0;
	while(fgets(sack1, 64, f) && fgets(sack2, 64, f) && fgets(sack3, 64, f))
		score += find_dup(sack1, sack2, sack3);
	printf("Score: %d\n", score);
}
