#ifndef AVLTREE_H
#define AVLTREE_H

#include <stdlib.h>
#include "common.h"

typedef struct avl_tree_node avlnode;
typedef struct avl_tree      avltree;

struct avl_tree_node {
	avlnode *left, *right;
	void *data;
	avlnode *parent;
	int height;
	short balance;
};


struct avl_tree {
	avlnode *root;	
	comp_fun comp;
	size_t size;
	visitor_t visitor;
};


void avlrenderer(btnode *node, char *buf);

void avlinit(avltree *t, comp_fun comp);


avlnode *avlinsert(avltree *t, void *data);


void *avlerase(avltree *t, void *data);


avlnode *avlfind(avltree *t, void *data);


void avlprint(const avltree *t);


void avlclear(avltree *t);


void avldestroy(avltree *t);


static inline size_t avlsize(avltree *t)
{
	return t->size;
}


static inline int avlempty(avltree *t)
{
	return t->root == NULL;
}


avlnode *avlrotater(avlnode **n);

avlnode *avlrotatel(avlnode **n);

#endif /* BTREE_H */
