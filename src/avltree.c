#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "btree.h"
#include "list.h"


static btnode *alloc_node(void *data)
{
	btnode * n = malloc(sizeof(btnode));
	if (!n) return NULL;
	n->left = n->right = n->parent = NULL;
	n->data = data;
	return n;
}


static void free_node(btnode *n, int destroy_data)
{
	if (destroy_data)
		free(n->data);
	free(n);
}


static void free_node_only(btnode *n)
{
	free_node(n, 0);
}


static void free_node_destroy(btnode *n)
{
	free_node(n, 1);
}


void btinit(btree *t, comp_fun cmp)
{
	t->root = NULL;
	if (cmp == NULL)
		cmp = lt;
	t->comp = cmp;
	t->size = 0;
	t->visitor = NULL;
}


void do_lrv(btree *t, btnode *n)
{
	if (n) {
		do_lrv(t, n->left);
		do_lrv(t, n->right);
		t->visitor(n);
	}
}


static void do_clear(btree *t)
{
	do_lrv(t, t->root);
	t->root = NULL;
	t->visitor = NULL;
	t->size = 0;
}


void btclear(btree *t)
{
	t->visitor = free_node_only;
	do_clear(t);
}


void btdestroy(btree *t)
{
	t->visitor = free_node_destroy;
	do_clear(t);
}


static btnode **btsrch_node(btree *t, void *data)
{
	btnode **cur = &t->root;
	int r;
	while (*cur) {
		r = t->comp(data, (*cur)->data);
		if (r == 0) 
			break;
		cur = (r > 0) ? &((*cur)->left) : &((*cur)->right);
	}
	return cur;
}


btnode *btinsert(btree *t, void *data)
{
	btnode **place = btsrch_node(t, data);
	if (*place == NULL) {
		btnode *n = alloc_node(data);
		if (n == NULL)
			return NULL;
		*place = n;
		++t->size;
		errno = 0;
	} else {  /* próba wstawienia istniejącej wartości */
		errno = EEXIST;
	}
	return *place;
}


void *bterase(btree *t, void *data)
{
	btnode **place = btsrch_node(t, data);
	if (*place != NULL) {
		btnode *tmp = *place;
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
			btnode *n = (*place)->left, *prev = *place;
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
		free_node_only(tmp);			
		--t->size;
		return data;
	}
	return NULL;
}




btnode *btfind(btree *t, void *data)
{
	return *btsrch_node(t, data);
}



btnode *btrotater(btnode **n)
{
	btnode *newRoot = NULL;
	if ((*n) != NULL && (*n)->left != NULL) {
		newRoot = (*n)->left;
		(*n)->left = newRoot->right;
		newRoot->right = *n;
		*n = newRoot;
	}
	return newRoot;
}


btnode *btrotatel(btnode **n)
{
	btnode *newRoot = NULL;
	if ((*n) != NULL && (*n)->right != NULL) {
		newRoot = (*n)->right;
		(*n)->right = newRoot->left;
		newRoot->left = *n;
		*n = newRoot;
	}
	return newRoot;
}
