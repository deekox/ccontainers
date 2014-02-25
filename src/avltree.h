#ifndef AVLTREE_H
#define AVLTREE_H

#include <stdlib.h>
#include "common.h"

typedef struct avl_tree_node     avlnode;
typedef struct avl_tree_iterator avliterator;
typedef struct avl_tree          avltree;

struct avl_tree_node {
	avlnode *left, *right;
	void *data;
	avlnode *parent;
	int height;
	short balance;
};

struct avl_tree_iterator {
	avlnode *node;
	enum last_move {
		AVL_MOV_NONE,
		AVL_MOV_LEFT_EDGE,
		AVL_MOV_RIGHT,
		AVL_MOV_BACK_DIRECT,
		AVL_MOV_BACK_SKIP
	} move;
	enum direction {
		AVL_DIR_FORWARD,
		AVL_DIR_BACKWARD
	} dir;
};

struct avl_tree {
	avlnode *root;	
	comp_fun comp;
	size_t size;
	visitor_t visitor;
};


void avlrenderer(btnode *node, char *buf);

void avlinit(avltree *t, comp_fun comp);


avliterator *avlbegin(avltree *t, avliterator *it);


avliterator *avlitnext(avliterator *it);

void avlitdesc(avliterator *it);

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

#endif /* BTREE_H */
