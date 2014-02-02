#ifndef BTREE_H
#define BTREE_H

#include <stdlib.h>


typedef struct binary_tree_node btnode;
typedef struct binary_tree      btree;
typedef int (*comp_fun)(const void *ldata, const void *rdata);
typedef void (*visitor)(btnode *n);


struct binary_tree_node {
	btnode *left, *right;
	void *data;
};


struct binary_tree {
	btnode *root;	
	comp_fun comp;
	size_t size;
	visitor visit;
};


void btinit(btree *t, comp_fun comp);


btnode *btinsert(btree *t, void *data);


void *bterase(btree *t, void *data);


btnode *btfind(btree *t, void *data);


void btprint(const btree *t);


void btclear(btree *t);


void btdestroy(btree *t);


static inline size_t btsize(btree *t)
{
	return t->size;
}


static inline int btempty(btree *t)
{
	return t->root == NULL;
}
	
#endif /* BTREE_H */
