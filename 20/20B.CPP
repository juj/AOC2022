#include <stdio.h>
#include <stdlib.h>
#include "../tick.h"
#include "../int64.h"

void main(int argc, char **argv)
{
	tick_t t0 = tick();
	int i, n, val[3], last = -1;
	struct item { int n; item *prev, *next; } *zero,
		*list = (item*)malloc(5000 * sizeof(item)); // input is 5000 items

	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	while(fscanf(handle, "%d\n", &n) == 1)
	{
		list[++last].n = n;
		if (n == 0) zero = &list[last];
		list[last].prev = &list[last-1];
		list[last].next = &list[last+1];
	}
	list[0].prev = &list[last]; // Tie the ends of the doubly linked list
	list[last].next = &list[0];
	const long mul = 811589153ul % last; // Reduce mul to avoid 64bit numbers

	for(int rounds = 0; rounds < 10; ++rounds)
		for(item *cur = &list[0]; cur <= &list[last]; ++cur)
		{
			cur->prev->next = cur->next; // Remove cur from the linked list
			cur->next->prev = cur->prev;
			n = (cur->n * mul) % last; // Calculate how much to shift cur node
			if      (n >  (last>>1)) n -= last; // Take the shorter round trip to
			else if (n < -(last>>1)) n += last; // destination to optimize
			item *parent = cur->prev;  // Find the new predecessor to insert to
#define UNROLL 38
			if (n > 0)
			{
				while (n >= UNROLL) { n -= UNROLL; parent = parent->
															next->
													 next->next->        /* Hyv„„ joulua! */
		/* God Jul! */      next->next->next->
										 next->next->next->next->
												next->next->next->          /* Merry Christmas! */
										 next->next->next->next->
 /* Gl‘edelig Jul! */   next->next->next->
										 next->next->next->next->   /* ­Feliz Navidad! */
									next->next->next->next->next->
									next->next->next->next->next->
	 /* Gledileg J¢l! */	   next->next->                /* Joyeux No‰l! */
													 next->next;
				}
				while(n--) parent = parent->next;
			}
			else if (n < 0)
			{
				while (n <= -UNROLL) { n += UNROLL; parent = parent->
															prev->
														prev->prev->      /* Frohe Weihnachten! */
	/* Wesolych Swiat! */   prev->prev->prev->
											 prev->prev->prev->prev->
											 prev->prev->prev->prev->        /* Buon Natale! */
											 prev->prev->prev->prev->
	/* Vesel‚ V noce! */ prev->prev->prev->prev->
												 prev->prev->prev->      /* Prettige Kerstdagen! */
														prev->prev->
															prev->prev->prev->
	/* Vrolijk Kerstfeest! */		prev->prev->prev->          /* Feliz Natal! */
																			prev->prev->
																					prev->prev->
																									prev;
				}
				while(n++) parent = parent->prev;
			}
			cur->prev = parent;       // Insert current node in between
			cur->next = parent->next; // parent ---> cur ---> parent->next
			cur->prev->next = cur->next->prev = cur;
		}
	for(i = 0; i < 1000; ++i) zero = zero->next; val[0] = zero->n;
	for(i = 0; i < 1000; ++i) zero = zero->next; val[1] = zero->n;
	for(i = 0; i < 1000; ++i) zero = zero->next; val[2] = zero->n;
	int64 sum = int64(811589153ul) * (val[0] + val[1] + val[2]);
	printf("Sum: %s. Elapsed: %f msecs.\n", sum.to_str(), msecs(t0, tick()));
}
