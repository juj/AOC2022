#include <stdio.h>

class inode
{
public:
	inode *parent, *child, *sibling;
	long size;
	inode():parent(0), child(0), sibling(0), size(0) {}
};

inode *add_child(inode *parent)
{
	inode *i = new inode();
	if (parent->child)
	{
		inode *c = parent->child;
		while(c->sibling)
			c = c->sibling;
		c->sibling = i;
		i->parent = c->parent;
	}
	else
	{
		parent->child = i;
		i->parent = parent;
	}
	return i;
}

long count_100k_dirs(inode *node, long &total)
{
	long child_sizes = node->size;
	for(inode *i = node->child; i; i = i->sibling)
		child_sizes += count_100k_dirs(i, total);
	if (child_sizes <= 100000) total += child_sizes;
	return child_sizes;
}

long find_smallest_dir_size_to_delete(inode *node, long minsz, long *bestsz)
{
	long subtree_size = node->size;
	for(inode *i = node->child; i; i = i->sibling)
		subtree_size += find_smallest_dir_size_to_delete(i, minsz, bestsz);
	if (subtree_size >= minsz && subtree_size < *bestsz)
		*bestsz = subtree_size;
	return subtree_size;
}

void main(int argc, char **argv)
{
	FILE *handle = fopen(argc > 1 ? argv[1] : "input.txt", "r");
	char input[256], name[64];
	inode *root = new inode(), *cur_dir = root;
	long free_space = 70000000, total = 0, size, best_del = 0x7FFFFFFF;

	while(fgets(input, sizeof(input), handle))
		if (sscanf(input, "$ cd %s", name) == 1)
		{
			if (name[0] == '.') cur_dir = cur_dir->parent;
			else if (name[0] != '/') cur_dir = add_child(cur_dir);
		}
		else if (sscanf(input, "%ld", &size) == 1)
		{
			cur_dir->size += size;
			free_space -= size;
		}
	count_100k_dirs(root, total);
	printf("<= 100k dirs: %ld\n", total);
	find_smallest_dir_size_to_delete(root, 30000000 - free_space, &best_del);
	printf("Size of best dir to delete: %ld\n", best_del);
}
