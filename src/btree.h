#ifndef BTREE_H
#define BTREE_H

#include <stdlib.h>

#include "common.h"

typedef struct binary_tree_node btnode;
typedef struct binary_tree      btree;


struct binary_tree_node {
	btnode *left, *right;
	void *data;
};


struct binary_tree {
	btnode *root;	
	comp_fun comp;
	size_t size;
	visitor_t visitor;
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


btnode *btrotater(btnode **n);

btnode *btrotatel(btnode **n);

void tree_to_vine(btree *t);

void balance_Day(btree *t);

void balance_DSW(btree *t);

size_t v2t_DSW_FullSize(size_t size);

#endif /* BTREE_H */
