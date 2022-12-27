#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include "../tick.h"
#include "../uint64.h"

void main(int argc, char **argv)
{
//	long t0 = tick();
	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	int n, used = 0;
	struct item { int n; item far *prev, far *next; } far *list =
		(item far*)farmalloc(5002*sizeof(item));
	item far *zero;
	while(fscanf(handle, "%d\n", &n) == 1)
	{
		if (n == 0) zero = &list[used];
		list[used].prev = &list[used-1];
		list[used].next = &list[used+1];
		list[used++].n = n;
	}
	list[0].prev = &list[used-1];
	list[used-1].next = &list[0];

	for(int j = 0; j < 10; ++j)
	for(int i = 0; i < used; ++i)
	{
		if ((i & 255) == 0) printf("%d\n", i);

		item far *cur = &list[i];
//		printf("%d moves.\n", cur->n);
		item far *to = cur->prev;
		cur->prev->next = cur->next;
		cur->next->prev = cur->prev;

		uint64 n64 = cur->n;
		n64 *= 811589153ul;
		n64 %= used-1;
		unsigned int n = n64.lo;
		if (n > 0) while(n--) to = to->next;
		else if (n < 0) while(n++) to = to->prev;

		item far *newPrev = cur->prev = to;
		item far *newNext = cur->next = to->next;
		newPrev->next = cur;
		newNext->prev = cur;
		/*
		for(item far *head = &list[0]; ; head = head->next)
		{
			printf("%d, ", head->n);
			if (head->next == &list[0]) break;
		}
		printf("\n\n");
		*/
	}
	/*
		printf("i=%d: ", i);
		for(int m = 0; m < used; ++m)
			printf("%d, ", list[m].n);
		printf("\n\n");
		*/
	uint64 val[3];
	for(i = 0; i < 1000; ++i) zero = zero->next;
	val[0] = uint64(zero->n) * 811589153ul;
	for(i = 0; i < 1000; ++i) zero = zero->next;
	val[1] = zero->n;
	for(i = 0; i < 1000; ++i) zero = zero->next;
	val[2] = zero->n;

	printf("Grove coordinates: %ld+%ld+%ld=%ld\n", val[0], val[1], val[2], val[0] + val[1] + val[2]);
}
