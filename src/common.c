#include <stdio.h>
#include <stdlib.h>

#include "../include/common.h"
#include "../include/btree.h"
#include "../include/list.h"


int lt(const void *l, const void *r)
{
	if ((long)l < (long)r)
		return 1;
	if ((long)l == (long)r)
		return 0;
	return -1;
}


int max(int a, int b)
{
	return (a > b) ? a : b;
}


int bin_tree_height(btnode *n)
{
	if (n == NULL)
		return 0;
	return 1 + max(bin_tree_height(n->left),
	               bin_tree_height(n->right));
}


void free_node(btnode *n, int destroy_data)
{
	if (destroy_data)
		free(n->key);
	free(n);
}


void free_node_only(btnode *n)
{
	free_node(n, 0);
}


void free_node_destroy(btnode *n)
{
	free_node(n, 1);
}



struct pair
{
	long value;
	long order;
	int left;
	btnode *node;
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
		pp->value = (long)n->key;
		pp->order = (*cnt)++;
		pp->left = l;
		pp->node = n;
		lpush_back(&levels[cur], pp);
		/* printf("[%d] cnt:%ld  val:%ld\n", cur, pp->order, pp->value); */
		do_walk(n->right, cur + 1, max, levels, cnt, 1);
	}
}
/* renderer to funkcja przyjmująca btnode* i zapisująca do char **
 * to co powinno być wypisane w miejscu noda na diagramie drzewa */
void bin_tree_print(btnode *n, void (*renderer)(btnode *, char *))
{
	int height = bin_tree_height(n);
	if (height == 0)
		return;

	list *levels = malloc(sizeof(list) * height);
	if (levels == NULL)
		return;

	int l;
	for (l = 0; l < height; ++l)
		linit(&levels[l]);
	l = 0;
	do_walk(n, 0, height, levels, &l, 0);

	const int BUF_SIZE = 30;
	char buf[BUF_SIZE];

	for (l = 0; l < height; ++l) {
		lnode *n = levels[l].head;
		long t = 0;
		while (n) {
			long order = ((struct pair *)n->data)->order;
			order -= t;
//			int left = ((struct pair *)n->data)->left;
			while (++t && order--) 
				printf("         ");
			if (renderer) {
				renderer((btnode *)(((struct pair *)n->data)->node), buf);
				printf("%s", buf);
			} else
				printf("%2ld", ((struct pair *)n->data)->value);
			/* if (left == -1) { */
			/* 	printf("/ "); */
			/* 	++t; */
			/* } */
			n = n->next;
		}
		printf("\n");
	}
	for (l = 0; l < height; ++l)
		ldestroy(&levels[l]);
	free(levels);
}
