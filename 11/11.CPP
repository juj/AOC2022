#include <stdio.h>
struct monkey {
	unsigned long *items;
	long numItems, opMul, opAdd, testDiv, throwTrue, throwFalse, examined;
};
#define NUM_MONKEYS 8
#define MOD 9699690ul
unsigned long items[8][64] = { { 54, 53 }, { 95, 88, 75, 81, 91, 67, 65, 84 },
	{ 76, 81, 50, 93, 96, 81, 83 }, { 83, 85, 85, 63 }, { 85, 52, 64 }, { 57 },
	{ 60, 95, 76, 66, 91 }, { 65, 84, 76, 72, 79, 65 } };
monkey monkeys[NUM_MONKEYS] = {
	{ items[0], 2, 3,  0,  2, 2, 6 }, { items[1], 8, 11, 0,  7, 3, 4 },
	{ items[2], 7, 1,  6,  3, 5, 1 }, { items[3], 4, 1,  4, 11, 7, 4 },
	{ items[4], 3, 1,  8, 17, 0, 7 }, { items[5], 1, 1,  2,  5, 1, 3 },
	{ items[6], 5, 0,  0, 13, 2, 5 }, { items[7], 6, 1,  5, 19, 6, 0 }
};

void main()
{
	unsigned long worry, hi12, lo12, b1 = 0, b2 = 0;
	for(int turn = 0; turn < 10000; ++turn)
		for(int i = 0; i < NUM_MONKEYS; ++i)
		{
			monkey &m = monkeys[i];
			for(int j = 0; j < m.numItems; ++j)
			{
				if ((m.opMul | m.opAdd) == 0)
				{
					hi12 = m.items[j] >> 12;
					lo12 = m.items[j] & 0x000FFFUL;
					worry =  lo12*lo12 // no 64-bit integers in Turbo C++ :(
						+    ((hi12*lo12 << 7ul) % MOD << 6ul) % MOD
						+   (((hi12*hi12 << 8ul) % MOD << 8ul) % MOD << 8ul) % MOD;
				}
				else worry = m.items[j] * m.opMul + m.opAdd;
				monkey &target = monkeys[worry%m.testDiv ?m.throwFalse:m.throwTrue];
				target.items[target.numItems++] = worry % MOD;
			}
			m.examined += m.numItems;
			m.numItems = 0;
		}
	for(i = 0; i < NUM_MONKEYS; ++i)
		if      (monkeys[i].examined > b1) b2 = b1, b1 = monkeys[i].examined;
		else if (monkeys[i].examined > b2) b2 = monkeys[i].examined;
	printf("Monkeys with most inspections: %llu, %llu\n", b1, b2);
}