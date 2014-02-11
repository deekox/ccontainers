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


void goup(avlnode *node)
{
	while (node) {
		int lh = -1, rh = -1;
		if (node->left)
			lh = node->left->height;
		if (node->right)
			rh = node->right->height;
		node->height = max(lh, rh) + 1;
		node->balance = rh - lh;
		/* printf("data: %ld\n", (long)node->data); */
		node = node->parent;
	}
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
		goup(n);
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


/* btnode *btrotater(btnode **n) */
/* { */
/* 	btnode *newRoot = NULL; */
/* 	if ((*n) != NULL && (*n)->left != NULL) { */
/* 		newRoot = (*n)->left; */
/* 		(*n)->left = newRoot->right; */
/* 		newRoot->right = *n; */
/* 		*n = newRoot; */
/* 	} */
/* 	return newRoot; */
/* } */


/* btnode *btrotatel(btnode **n) */
/* { */
/* 	btnode *newRoot = NULL; */
/* 	if ((*n) != NULL && (*n)->right != NULL) { */
/* 		newRoot = (*n)->right; */
/* 		(*n)->right = newRoot->left; */
/* 		newRoot->left = *n; */
/* 		*n = newRoot; */
/* 	} */
/* 	return newRoot; */
/* } */

