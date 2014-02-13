#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <assert.h>

#include "avltree.h"
#include "list.h"



static avlnode *alloc_node(void *data)
{
	avlnode *n = malloc(sizeof(avlnode));
	if (!n) return NULL;
	n->left = n->right = n->parent = NULL;
	n->data = data;
	n->height = -1;
	n->balance = 0;
 	return n;
}

void avlrenderer(btnode *node, char *buf)
{
	avlnode *n = (avlnode *)node;
	int i;
	for (i = 0; i < 10; ++i)
		buf[i] = ' ';
	snprintf(buf, 11, "(%2ld %2ld %2ld)",
	         (long)n->data, (long)n->height, (long)n->balance);
	buf[10] = '\0';

	/* if (n->parent) */
	/* 	snprintf(buf, 10, "%2ld_%2ld_%2ld", */
	/* 	         (long)n->data, (long)n->height, (long)n->parent->data); */
	/* else */
	/* 	snprintf(buf, 10, "%2ld_%2ld_  ", (long)n->data, (long)n->height); */
}


void avlinit(avltree *t, comp_fun cmp)
{
	t->root = NULL;
	if (cmp == NULL)
		cmp = lt;
	t->comp = cmp;
	t->size = 0;
	t->visitor = NULL;
}


static void do_lrv(avltree *t, avlnode *n)
{
	if (n) {
		do_lrv(t, n->left);
		do_lrv(t, n->right);
		t->visitor((btnode *)n);
	}
}


static void do_clear(avltree *t)
{
	do_lrv(t, t->root);
	t->root = NULL;
	t->visitor = NULL;
	t->size = 0;
}


void avlclear(avltree *t)
{
	t->visitor = free_node_only;
	do_clear(t);
}


void avldestroy(avltree *t)
{
	t->visitor = free_node_destroy;
	do_clear(t);
}

/* try to go to the left edge */
avlnode *avl_try_LE(avlnode *n)
{
	while (n->left)
		n = n->left;
	return n;
}

/* try to go to right node and then go to left edge */
avlnode *avl_try_RLE(avlnode *n)
{
	if (n->right) {
		if (n->right->left)
			return avl_try_LE(n->right);
	}
	return n;
}

/* try to go to right node */
avlnode *avl_try_R(avlnode *n)
{
	return (n->right) ? n->right : n;
}

/* try to go to the direct parent */
avlnode *avl_try_back_direct(avlnode *n)
{
	return (n->parent) ? n->parent : n;
}


static inline int isleftchild(avlnode *n)
{
	return (n->parent->left == n);
}

/* search for the first ancestor such that node n is its left descendant,
 * but skip first parent
 */
avlnode *avl_try_back_skip(avlnode *n)
{
	avlnode *org = n;

	if (n->parent) {
		n = n->parent;			/* skipping first ancestor */
		while (n->parent) {
			if (isleftchild(n))
				return n->parent;
			n = n->parent;
		}
	}
	return org;
}


avliterator *avlbegin(avltree *t, avliterator *it)
{
	it->dir = AVL_DIR_FORWARD;
	it->node = t->root;
	if (it->node == NULL)
		return NULL;

	avlnode *tmp = gotoleftedge(it->node);
	if (it->node != tmp) {
		it->node = tmp;
		it->move = AVL_MOV_LEFT_EDGE;
	} else
		it->move = AVL_MOV_BACK_DIRECT;
	return it;
}


avliterator *avlitnext(avliterator *it)
{
	if (it->node == NULL)
		return NULL;
	switch (it->move) {
	/* case AVL_MOVE_LEFT: */
	/* } */
	return NULL;
}


int avltreeheight(avlnode *node)
{
	if (node) 
		return 1 + max(avltreeheight(node->left),
		               avltreeheight(node->right));
	return 0;
}


/* iterating the tree in Left Vist Right order */
avlnode *avl_lvr(avliterator *prev)
{
	return NULL;
}


/* checks if abs(height(left) - height(right)) < 2
 * i.e. if this subtree is AVL tree
 */
int checkavltree(avlnode *n)
{
	
	return 0;
}


static void reparent(avlnode *old_root, avlnode *new_root, avltree *t)
{
	if (old_root->parent) {
		if (isleftchild(old_root))
			old_root->parent->left = new_root;
		else
			old_root->parent->right = new_root;
	} else
		t->root = new_root;
	new_root->parent = old_root->parent;
	old_root->parent = new_root;
}

avlnode *avlrotater(avlnode *old_root, avltree *t)
{
	avlnode *new_root = old_root->left;
	if (new_root != NULL) {
		old_root->left = new_root->right;
		if (new_root->right)
			new_root->parent = old_root;
		new_root->right = old_root;
		reparent(old_root, new_root, t);
	}
	return new_root;
}


avlnode *avlrotatel(avlnode *old_root, avltree *t)
{
	avlnode *new_root = old_root->right;
	if (new_root != NULL) {
		old_root->right = new_root->left;
		if (new_root->left)
			new_root->parent = old_root;
		new_root->left = old_root;
		reparent(old_root, new_root, t);
	}
	return new_root;
}


void rebalance(avlnode *node, avltree *t)
{
	avlnode *child = (node->balance > 0) ? node->right : node->left;
	int same_sign = node->balance * child->balance;
	printf("[%2ld] imba, ", (long)node->data);
	if (same_sign > 0) {				   /* one rotation only */
		printf("same signs\n");
		if (node->balance > 0)
			avlrotatel(node, t);
		else
			avlrotater(node, t);
		node->balance = child->balance = 0;
	} else {
		printf("diff signs\n");
		avlnode *grand_child = (child->balance > 0) ?
			child->right : child->left;
		if (node->balance > 0) {
			avlrotater(child, t);
			avlrotatel(node, t);
		} else {
			avlrotatel(child, t);
			avlrotater(node, t);
		}
		/* TODO: te balanse mogą być błędne: przetestować */
		node->balance = 0;
		child->balance = 0;
		grand_child->balance = 0;
	}
	
	/* if (n->balance > 0) */

}


avlnode *compute_balance(avlnode *added)
{
	avlnode *node = added;
	node->balance = 0;
	if (node->parent != NULL) {
		if (node->parent->left == NULL || node->parent->right == NULL) {
			/* First (and only) child */
			do {
				if (node->parent->left == node)
					--node->parent->balance;
				else
					++node->parent->balance;
				if (node->parent->balance == 0)
					break;
				if (abs(node->parent->balance) == 2) 
					return node->parent;
				node = node->parent;
			} while (node->parent != NULL);
		} else {
			/* got one sibling. Assuming there cannot be -2/2 balace
			 * i can safely set parent balance to 0
			 * (because it can only be -1/1)
			 */
			node->parent->balance = 0;
		}
	}
	return NULL;
}

/* cmp zwraca wynik ostatniego użycia funkcji comp
 * funkcja lower powinna zwracać 1 gdy dana jest mniejsza
 * cmp ==  1 zwracany jest: &parent->left
 * cmp == -1 &parent->right
 * cmp ==  0 oznacza, że dany klucz już istnieje
 */
static avlnode **avlsrch_node(avltree *t, void *data, int *cmp)
{
	avlnode **cur = &t->root;
	if (cmp) *cmp = 0;
	int r;
	while (*cur) {
		r = t->comp(data, (*cur)->data);
		if (cmp)
			*cmp = r;
		if (r == 0) 
			break;
		cur = (r > 0) ? &(*cur)->left : &(*cur)->right;
	}
	return cur;
}


avlnode *avlinsert(avltree *t, void *data)
{
	int cmp;
	avlnode **place = avlsrch_node(t, data, &cmp);
	if (*place == NULL) {
		avlnode *n = alloc_node(data);
		if (n == NULL)
			return NULL;
		*place = n;
		avlnode *parent;
		if (cmp > 0)  			/* place wskazuje na &parent->left */
			parent = (avlnode *)place;
		else if (cmp < 0) 	/* place = &parent->right */
			parent = (avlnode *)((void *)place - offsetof(avlnode, right));
		else
			parent = NULL; 		/* root */
		n->parent = parent;
		avlnode *imba = compute_balance(n);
		if (imba) {
			printf("inserting: %2ld  ", (long)data);
			rebalance(imba, t);
		}
		/* compute_height(n); */
		++t->size;
		errno = 0;
	} else {  /* próba wstawienia istniejącej wartości */
		errno = EEXIST;
	}
	return *place;
}


void *avlerase(avltree *t, void *data)
{
	int cmp; 		
	avlnode **place = avlsrch_node(t, data, &cmp);
	if (*place != NULL) {
		avlnode *tmp = *place;
		void *data = (*place)->data;

		/* no children or one child */
		if ((*place)->right == NULL)
			(*place) = (*place)->left;
		else if ((*place)->left == NULL)
			(*place) = (*place)->right;
		/* two children */
		else {
			/* usuwanie przez łączenie */
			/* btnode *n = (*place)->left; */
			/* while (n->right != NULL)  */
			/* 	n = n->right; */
			/* n->right = (*place)->right; */
			/* *place = (*place)->left; */

			
			/* usuwanie przez kopiowanie: w przeciwieństwie do delete by merge
			 * nie powoduje wzrostu wysokości drzewa
			 * kopiujemy klucz poprzednika do klucza usuwanego noda i
			 * modyfikujemy rodzica poprzednika tak by pokazywał na dziecko
			 * poprzednika. Samego poprzednika usuwamy */
			avlnode *n = (*place)->left, *prev = *place;
			while (n->right != NULL) {
				prev = n;
				n = n->right;
			}

			/* 1 - skopiuj klucz poprzednika do klucza usuwanego noda */
			(*place)->data = n->data;
			/* 2 - rodzic poprzednika powinien wskazywać na jedyne (ewentualne)
			 *     lewe dziecko poprzednika. Z założenia poprzednik jest prawym
			 *     dzieckiem.
			 */
			
			tmp = n; 			/* to n będzie usuwany */
			/* Nieco inne zachowanie, gdy się okaże, że poprzednikiem jest
			 * bezpośrednie dziecko usuwanego noda. Wówczas poprzednik jest
			 * lewym dzieckiem.
			 */
			if (prev == *place)
				(*place)->left = n->left;
			else
				prev->right = n->left;
		}
		free_node_only((btnode *)tmp);			
		--t->size;
		return data;
	}
	return NULL;
}


avlnode *avlfind(avltree *t, void *data)
{
	return *avlsrch_node(t, data, NULL);
}


