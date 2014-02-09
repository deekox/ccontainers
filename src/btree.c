#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "btree.h"
/* #include "list.h"				 */



btnode *alloc_node(void *data)
{
	btnode * n = malloc(sizeof(btnode));
	if (!n) return NULL;
	n->left = n->right = NULL;
	n->data = data;
	return n;
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


void DSW_compression(btnode *root, int cnt)
{
	btnode *sc = root;			
  	int j;
	for (j = 0; j < cnt; ++j) {
		btnode *chld = sc->right;
		sc->right = chld->right;
		sc = sc->right;
		chld->right = sc->left;
		sc->left = chld;
		printf("\n%d/%d:\n", j + 1, cnt);
		bin_tree_print(root, NULL);
	}
}


void vine_to_tree_Day(btree *t)
{
	btnode pseudo_root;
	pseudo_root.data = 0;
	pseudo_root.left = NULL;
	pseudo_root.right = t->root;

	int NBack = t->size - 1;
	int M;
	int rot = 0;
	for (M = NBack /2; M > 0; M = NBack / 2) {
		DSW_compression(&pseudo_root, M);
		NBack = NBack - M - 1;
		rot += M;
	}
	t->root = pseudo_root.right;
	printf("TOTAL ROTATIONS: %d\n\n", rot);
}


size_t v2t_DSW_FullSize(size_t size)
{
	size_t Rtn = 1;
	while (Rtn <= size)
		Rtn = Rtn + Rtn + 1;
	return Rtn / 2;
}


void vine_to_tree_DSW(btree *t)
{
	btnode pseudo_root;
	pseudo_root.data = 0;
	pseudo_root.left = NULL;
	pseudo_root.right = t->root;

	size_t size = t->size;
	size_t full_count = v2t_DSW_FullSize(size);
	DSW_compression(&pseudo_root, size - full_count);
	int rot = size - full_count;
	for (size = full_count; size > 1; size /= 2) {
		DSW_compression(&pseudo_root, size / 2);
		rot += size / 2;
	}
	printf("TOTAL ROTATIONS: %d\n\n", rot);
	t->root = pseudo_root.right;
}


void tree_to_vine(btree *t)
{
	btnode **n = &t->root;
	while (*n) {
		if ((*n)->left)
			while (btrotater(n));
		n = &(*n)->right;
	}
}


void balance_Day(btree *t)
{
	tree_to_vine(t);
	vine_to_tree_Day(t);
}


void balance_DSW(btree *t)
{
	tree_to_vine(t);
	vine_to_tree_DSW(t);
}

