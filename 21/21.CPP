#include <stdio.h>
#include <stdlib.h>
#include <alloc.h>
#include "../uint64.h"
#include "../tick.h"
#define HASH_SZ 4095
#define NAME(name) ((long)name[0]      | ((long)name[1]<<8) \
								 | ((long)name[2]<<16) | ((long)name[3]<<24))
#define HASH(name) ((name&0xFFF) ^ ((name>>11)&0xFFF) ^ (name>>22))

struct node
{
	uint64 num;
	unsigned long name, leftName, rightName;
	node huge *l, huge *r, huge *parent;
	char op, hasX;
};

node huge **nodeSet = (node huge**)farcalloc(HASH_SZ + 1, sizeof(node*));

node huge *hash_find(unsigned long name)
{
	for(unsigned int hash = HASH(name); nodeSet[hash]; hash=(hash+1)&HASH_SZ)
		if (nodeSet[hash]->name == name)
			return nodeSet[hash];
} // unreachable

uint64 solve_x(node huge *n, uint64 target)
{
	while(n->name != NAME("humn"))
	{
		node huge *x = n->l->hasX ? n->l      : n->r; // Which subtree has X?
		uint64 k     = n->l->hasX ? n->r->num : n->l->num; // k = known constant
		if      (n->op == '+') target =              target - k;
		else if (n->op == '-') target = n->l->hasX ? target + k : k - target;
		else if (n->op == '*') target =              target / k;
		else /* (op == '/') */ target = n->l->hasX ? target * k : k / target;
		n = x;
	}
	return target;
}

void main(int argc, char **argv)
{
	long t0 = tick(), numNodes = 0;
	node huge *nodes = (node huge*)farcalloc(2048, sizeof(node));
	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	char line[32], name[8], left[8], right[8], op;
	while(fgets(line, sizeof(line), handle))
	{
		int num = 0;
		if (sscanf(line, "%s %s %c %s", name, left, &op, right) != 4
		 && sscanf(line, "%s %d", name, &num) != 2)
			break;

		node huge *n = &nodes[numNodes++];
		unsigned long uname = *(unsigned long*)name;
		for(int hash = HASH(uname); nodeSet[hash]; hash = (hash + 1) & HASH_SZ)
			; // nop, linear probe a free slot
		nodeSet[hash] = n;
		n->name = uname;
		n->num  = num;
		if (!num)
		{
			n->op = op;
			n->leftName  = *(unsigned long*)left;
			n->rightName = *(unsigned long*)right;
		}
	}
	hash_find(NAME("humn"))->hasX = 1; // human node contains unknown X
	for(int i = 0; i <= HASH_SZ; ++i)
	{
		node huge *n = nodeSet[i];
		if (!n || n->num != 0) continue;
		(n->l = hash_find(n->leftName ))->parent = n;
		(n->r = hash_find(n->rightName))->parent = n;
	}
	node huge *root = hash_find(NAME("root"));
	node huge **head = &nodeSet[0]; // Find leaves for BFS search. Use hash set
	for(i = 0; i < HASH_SZ+1; ++i) // mem as work area, destroys the hash set.
		if (nodeSet[i] && nodeSet[i]->num != 0) *head++ = nodeSet[i];

	for(node huge **tail = &nodeSet[0]; tail != head; ++tail)
	{
		node huge *n = *tail;
		if      (n->op == '+') n->num = n->l->num + n->r->num;
		else if (n->op == '-') n->num = n->l->num - n->r->num;
		else if (n->op == '*') n->num = n->l->num * n->r->num;
		else if (n->op == '/') n->num = n->l->num / n->r->num;
		if (!n->parent) continue;
		n->parent->hasX |= n->hasX; // Propagate location of X up
		if (n->parent->l->num != 0 && n->parent->r->num != 0)
			*head++ = n->parent; // Queue parent if its children were evaled
	}
	printf("Root number: %s\n", root->num.to_str());
	uint64 knownValue = root->l->hasX ? root->r->num : root->l->num;
	root              = root->l->hasX ? root->l      : root->r;
	printf("Human value: %s\n", solve_x(root, knownValue).to_str());
	printf("Elapsed: %ld msecs.\n", tick()-t0);
}
