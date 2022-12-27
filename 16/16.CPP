#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if 0
#define NODES 7ul
const unsigned long flows[7] = { 0,13,2,20,3,22,21 };
const unsigned long graph[7*7] = {
	0,1,2,1,2,5,2,
	1,0,1,2,3,6,3,
	2,1,0,1,2,5,4,
	1,2,1,0,1,4,3,
	2,3,2,1,0,3,4,
	5,6,5,4,3,0,7,
	2,3,4,3,4,7,0,
};

#else

#define NODES 16ul
const char flows[16] = { 0,19,8,9,17,24,10,25,12,13,22,14,18,5,21,6, };
const char graph[16*16] = {
	0, 4, 2, 2, 4, 7, 2, 7, 6, 9, 11, 6, 5, 2, 4, 3,
	4, 0, 2, 6, 5, 11, 6, 8, 10, 13, 15, 10, 9, 4, 8, 7,
	2, 2, 0, 4, 3, 9, 4, 6, 8, 11, 13, 8, 7, 2, 6, 5,
	2, 6, 4, 0, 2, 5, 4, 5, 4, 7, 9, 4, 7, 2, 2, 3,
	4, 5, 3, 2, 0, 6, 4, 3, 6, 9, 11, 6, 7, 4, 4, 2,
	7, 11, 9, 5, 6, 0, 9, 3, 2, 5, 7, 4, 12, 7, 3, 8,
	2, 6, 4, 4, 4, 9, 0, 7, 8, 11, 13, 8, 3, 3, 6, 2,
	7, 8, 6, 5, 3, 3, 7, 0, 5, 8, 10, 7, 10, 7, 6, 5,
	6, 10, 8, 4, 6, 2, 8, 5, 0, 3, 5, 2, 11, 6, 2, 7,
	9, 13, 11, 7, 9, 5, 11, 8, 3, 0, 2, 5, 14, 9, 5, 10,
	11, 15, 13, 9, 11, 7, 13, 10, 5, 2, 0, 7, 16, 11, 7, 12,
	6, 10, 8, 4, 6, 4, 8, 7, 2, 5, 7, 0, 11, 6, 2, 7,
	5, 9, 7, 7, 7, 12, 3, 10, 11, 14, 16, 11, 0, 6, 9, 5,
	2, 4, 2, 2, 4, 7, 3, 7, 6, 9, 11, 6, 6, 0, 4, 3,
	4, 8, 6, 2, 4, 3, 6, 6, 2, 5, 7, 2, 9, 4, 0, 5,
	3, 7, 5, 3, 2, 8, 2, 5, 7, 10, 12, 7, 5, 3, 5, 0,
};
#endif

#define DIST(a,b) (graph[(a)*NODES+(b)])

/*
int is_suboptimal(int start, int p1, int p2, int end)
{
	// How much time it takes to follow order 1?
	int t_s1 = DIST(start, p1) + 1;
	int t_12 = DIST(p1, 2) + 1;
	int t_2e = DIST(p2, end);
	int time_order1 = t_s1 + t_12 + t_2e;

	// How much time it takes to follow order 2?
	int t_s2 = DIST(start, p2) + 1;
	int t_21 = DIST(p2, p1) + 1;
	int t_1e = DIST(p2, end);
	int time_order2 = t_s2 + t_21 + t_1e;

	// How much flow do we gain to follow orders 1 vs 2?
	int flow_order1 = (t_12 + t_2e) * flows[p1] + t_2e * flows[p2];
	int flow_order2 = (t_21 + t_1e) * flows[p2] + t_1e * flows[p1];

	return time_order2 < time_order1 && flow_order2 >= flow_order1;
}
*/

class state
{
public:
	unsigned int locationsLeft, scoreSoFar, flowLeft;
	unsigned char pos, timeLeft;
};
/*
int findLowestBitSet(int val)
{
	for (uint8_t i = 1; i < NODES; ++i)
		if ((val & (1 << i)) != 0)
			return i;
	return -1;
}
*/
void main()
{
	state stackBottom[128], *stack = stackBottom;
	stack->locationsLeft = ((1ul << NODES) - 1ul) & ~1ul;
	stack->pos = 0;
	stack->timeLeft = 30;
	stack->scoreSoFar = 0;
	stack->flowLeft = 0;
	for (int i = 0; i < NODES; ++i) stack->flowLeft += flows[i];

	unsigned int bestScore = 0;

	unsigned long states = 0;
	unsigned long maxQueueSize = 0;
	while(stack >= stackBottom)
	{
		if (stack - stackBottom > maxQueueSize) maxQueueSize = stack - stackBottom;
		++states;

		const state cur = *stack;
//		printf("curloc:%lu, score:%lu, flowleft:%lu, pos: %lu, timeleft:%lu\n",
 //			cur.locationsLeft, cur.scoreSoFar, cur.flowLeft, cur.pos, cur.timeLeft);
		--stack;

		if (cur.scoreSoFar > bestScore)
		{
			bestScore = cur.scoreSoFar;
			printf("Best score: %u\n", bestScore);
		}

		// Estimate the absolute max amount of score that can still be gained.
		if (cur.scoreSoFar + cur.flowLeft * cur.timeLeft <= bestScore)
			continue;

		for(int i = 1; i < NODES; ++i)
		{
			unsigned int mask = 1ul << i;
 //			printf("cur.locsleft: %lu, i=%lu, locsLeft: %lu\n",
	//			cur.locationsLeft, i, locsLeft);
			if ((cur.locationsLeft & mask) != 0)
			{
				unsigned char dist = DIST(cur.pos, i) + 1ul;
				if (dist < cur.timeLeft)
				{
					++stack;
					stack->locationsLeft = cur.locationsLeft & ~mask;
					stack->pos = i;
					stack->timeLeft = cur.timeLeft - dist;
					stack->scoreSoFar = cur.scoreSoFar + stack->timeLeft * flows[i];
//			printf("flows[i]=%lu\n", flows[i]);
 //					assert(cur.flowLeft >= flows[i]);
					stack->flowLeft = cur.flowLeft - flows[i];
				}
			}
		}
	}
	printf("%lu states searched. maxqueueusize: %lu\n", states, maxQueueSize);
}
