#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "btree.h"
#include "list.h"


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


void bin_tree_print(btnode *n)
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
