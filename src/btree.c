#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "btree.h"
#include "list.h"

static int lt(const void *l, const void *r)
{
	if ((long)l < (long)r)
		return 1;
	if ((long)l == (long)r)
		return 0;
	return -1;
}


static int max(int a, int b)
{
	return (a > b) ? a : b;
}


static btnode *alloc_node(void *data)
{
	btnode * n = malloc(sizeof(btnode));
	if (!n) return NULL;
	n->left = n->right = NULL;
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
	t->visit = NULL;
}


void do_lrv(btree *t, btnode *n)
{
	if (n) {
		do_lrv(t, n->left);
		do_lrv(t, n->right);
		t->visit(n);
	}
}


static void do_clear(btree *t)
{
	do_lrv(t, t->root);
	t->root = NULL;
	t->visit = NULL;
	t->size = 0;
}


void btclear(btree *t)
{
	t->visit = free_node_only;
	do_clear(t);
}


void btdestroy(btree *t)
{
	t->visit = free_node_destroy;
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

int do_height(const btnode *n)
{
	if (n == NULL)
		return 0;
	return 1 + max(do_height(n->left),
	               do_height(n->right));
}


/* height 0 - emty tree
 * height 1 - only root
 * etc
 */
int bt_height(const btree *t)
{
	return do_height(t->root);
}


struct pair
{
	long value;
	long order;
	int left;
};

/* n - aktualnie odwiedzany węzeł
 * cur - aktualny poziom drzewa
 * max - maksymalny poziom drzewa
 * l - czy aktualnie odwiedzany węzeł to jest lewe dziecko
 */
void do_walk(btnode *n, int cur, int max, list *levels, int *cnt, int l)
{
	if (cur < max && n != NULL) {
		do_walk(n->left, cur + 1, max, levels, cnt, -1);

		struct pair *pp = malloc(sizeof(struct pair));
		pp->value = (long)n->data;
		pp->order = (*cnt)++;
		pp->left = l;
		lpush_back(&levels[cur], pp);
		/* printf("[%d] cnt:%ld  val:%ld\n", cur, pp->order, pp->value); */
		do_walk(n->right, cur + 1, max, levels, cnt, 1);
	}
}


void btprint(const btree *t)
{
	int height = bt_height(t);
	if (height == 0)
		return;

	list *levels = malloc(sizeof(list) * height);
	if (levels == NULL)
		return;

	int l;
	for (l = 0; l < height; ++l)
		linit(&levels[l]);
	l = 0;
	do_walk(t->root, 0, height, levels, &l, 0);

	for (l = 0; l < height; ++l) {
		lnode *n = levels[l].head;
		long t = 0;
		while (n) {
			long order = ((struct pair *)n->data)->order;
			order -= t;
			int left = ((struct pair *)n->data)->left;
			while (++t && order--) 
				printf("  ");
			
			printf("%2ld", ((struct pair *)n->data)->value);
			if (left == -1) {
				printf("/ ");
				++t;
			}
			n = n->next;
		}
		printf("\n");
	}
	for (l = 0; l < height; ++l)
		ldestroy(&levels[l]);
	free(levels);
}
