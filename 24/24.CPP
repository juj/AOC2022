#include <stdio.h>
#include "../tick.h"
#define OPEN_SZ 2047
#define CLOSED_SZ 131071
#define COST(s) ((int)(heuristic0 + (s)->t - (s)->x - (s)->y))
#define HASH(s) ((s->t*50159ul) ^ (s->x*35969ul) ^ (s->y * 54083ul))
int w1, h1, w2, h2, w2mod, h2mod, width = 0, height, heuristic0, numOpen;

struct blizzard
{
	int pos;
	blizzard *next;
} *up[152]={0}, *down[152]={0}, *left[24]={0}, *right[24]={0}, *blizzards;

struct state { unsigned x : 8, y : 5, t : 11; } *openNodes,  *closed;

int no_blizzard(blizzard *b, int pos, int time, int mod)
{
	for(; b; b = b->next) if ((b->pos + time) % mod == pos) return 0;
	return 1;
}

int is_blizzard_free(state *s)
{
	int bx = s->x-1, by = s->y-1;
	return (s->y >= height ||
			 (no_blizzard(right[s->y], bx,       s->t, w2)
		 && no_blizzard( left[s->y], bx, w2mod-s->t, w2)))
		 && no_blizzard(   up[s->x], by, h2mod-s->t, h2)
		 && no_blizzard( down[s->x], by,       s->t, h2);
}

int min_heapify(int i)
{
	int changed = 0, c0, c1, child;
	for(;;)
	{
		if ((child = (i<<1)+1) >= numOpen) return changed;
		int first = i;
		if ((c1 = COST(openNodes+child)) < (c0 = COST(openNodes+i)))
			first = child, c0 = c1;
		if (++child < numOpen && COST(openNodes+child) < c0) first = child;
		if (first == i) return changed;
		state tmp = openNodes[i];
		openNodes[i] = openNodes[first];
		openNodes[first] = tmp;
		changed = 32767;
		i = first;
	}
}

int get_or_insert_closed(state *s)
{
	state  *c = &closed[HASH(s) & CLOSED_SZ];
	if (c->x == s->x && c->y == s->y && c->t == s->t) return 1;
	*c = *s;
	return 0;
}

void insert_open(unsigned char x, unsigned char y, unsigned int t)
{
	state s = { x, y, t };
	if (!is_blizzard_free(&s) || get_or_insert_closed(&s)) return;
	openNodes[numOpen++] = s;
	int i = numOpen-1;
	while(i != 0)
	{
		i = (i-1)>>1;
		i &= min_heapify(i);
	}
}

int search(int sx, int sy, int dx, int dy, int st)
{
	numOpen = 0;
	for(long i = 0; i <= CLOSED_SZ; ++i) closed[i].x = 0;
	insert_open(sx, sy, st);
	while(numOpen > 0)
	{
		state s = openNodes[0]; // pop first element in open min queue.
		openNodes[0] = openNodes[--numOpen];
		min_heapify(0);
		if (s.x == dx && s.y == dy) return s.t;
		int nt = s.t+1;
		insert_open(s.x, s.y, nt); // wait
		if (s.x > 1  &&  s.y > 0  && s.y < h1)   insert_open(s.x-1, s.y, nt); // left
		if (s.x < w2 &&  s.y > 0  && s.y < h1)   insert_open(s.x+1, s.y, nt); // right
		if (s.y > 1  || (s.y > 0  && s.x == 1))  insert_open(s.x, s.y-1, nt); // up
		if (s.y < h2 || (s.y < h1 && s.x == w2)) insert_open(s.x, s.y+1, nt); // down
	}
} // unreachable

void main(int argc, char **argv)
{
	tick_t t0 = tick();
	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	int size = 0, x = 0, y = 1, numBlizzards = 0, ch;
	while((ch = fgetc(handle)) != '\n') ++width, ++size;
	blizzards = (blizzard*)calloc(2700, sizeof(blizzard));
	while((ch = fgetc(handle)) != EOF)
	{
		blizzard *b = &blizzards[numBlizzards++];
		if (ch == '>' || ch == '<') b->pos = x - 1;
		else b->pos = y - 1;
		++size;
		if      (ch == '>') { b->next = right[y]; right[y] = b; }
		else if (ch == '<') { b->next =  left[y];  left[y] = b; }
		else if (ch == '^') { b->next =    up[x];    up[x] = b; }
		else if (ch == 'v') { b->next =  down[x];  down[x] = b; }
		else --numBlizzards;
		++x;
		if (ch == '\n') x = 0, ++y;
	}
	height = size/width;
	heuristic0 = width+height-3;
	w1 = width-1;
	h1 = height-1;
	w2 = width-2;
	h2 = height-2;
	w2mod = 2048 - 2048%w2;
	h2mod = 2048 - 2048%h2;
	openNodes = (state*)calloc(OPEN_SZ+1, sizeof(state));
	closed =  (state *)calloc(CLOSED_SZ+1, sizeof(state));
	int t = search(1, 0, w2, h1, 0);
	printf("From start to end in %d turns. %f msecs.\n", t, msecs(t0, tick()));
	t = search(w2, h1, 1, 0, t);
	printf("Back to get snacks at turn %d. Total: %f msecs.\n", t, msecs(t0, tick()));
	t = search(1, 0, w2, h1, t);
	printf("At end again after %d turns. Total: %f msecs.\n", t, msecs(t0, tick()));
}
