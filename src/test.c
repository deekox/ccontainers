#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "btree.h"
#include "list.h"

void printli(const list *l)
{
	lnode *n = l->head;
	printf("[%ld]: ", (long)lsize(l));
	while (n) {
		printf("%ld ", (long)n->data);
		n = n->next;
	}
	printf("  |  ");
	n = l->tail;
	while (n) {
		printf("%ld ", (long)n->data);
		n = n->prev;
	}
	putchar('\n');
}

void list_init(list *l, int size)
{
	lclear(l);
	int i;
	for (i = 0; i < size; ++i)
		lpush_back(l, (void*)(long)i);
}

int testList()
{
	const int LSIZE = 15;
	list li, *l;
	lnode *n, *tn;
	l = &li;

	linit(l);
	printli(l);

	list_init(l, 15);
	printli(l);

	lclear(l);
	printli(l);

	list_init(l, 15);
	printli(l);

	int i = 40;
	printf("rotating left:\n");
	while (--i >= 0) {
		lpush_back(l, lpop_front(l));
		printli(l);
	}
		
	i = 40;
	printf("rotating right:\n");
	while (--i >= 0) {
		lpush_front(l, lpop_back(l));
		printli(l);
	}
	
	printf("poping front\n");
	printli(l);
	while (!lempty(l)) {
		printf("%ld\n", (long)lpop_front(l));
		printli(l);
	}
	
	list_init(l, LSIZE);
	printf("poping back\n");
	printli(l);
	while (!lempty(l)) {
		printf("%ld\n", (long)lpop_back(l));
		printli(l);
	}
	
	lclear(l);
	printf("inserting to empty at NULL/end():\n");
	for (i = 0; i < LSIZE; ++i) {
		linsert(l, NULL, (void*)(long)i);
		printli(l);
	}

	lclear(l);
	printf("inserting to empty at head\n");
	for (i = 0; i < LSIZE; ++i) {
		linsert(l, l->head, (void*)(long)i);
		printli(l);
	}

	lclear(l);
	list_init(l, LSIZE);
	printf("inserting to:\n");
	printli(l);
	printf("at even (at == before)\n");
	n = l->head;
	while (n) {
		if (!((long)n->data % 2)) {
			linsert(l, n, (void*)(long)(LSIZE * 2));
			printli(l);
		}
		n = n->next;
	}

	lclear(l);
	list_init(l, LSIZE);
	printf("inserting to:\n");
	printli(l);
	printf("after even (at n->next)\n");
	n = l->head;
	while (n) {
		if (!((long)n->data % 2)) {
			printf("n=%ld\n", (long)n->data);
			n = n->next;
			linsert(l, n, (void *)(long)(LSIZE * 2));
			printli(l);
		}
		if (n == NULL) break;
		n = n->next;
	}

	lclear(l);
	list_init(l, LSIZE);
	printf("erasing from head:\n");
	printli(l);
	while (lsize(l)) {
		lerase(l, l->head);
		printli(l);
	}

	lclear(l);
	list_init(l, LSIZE);
	printf("erasing every odd:\n");
	printli(l);
	n = l->head;
	while (n) {
		tn = n->next;
		if ((long)n->data % 2) {
			lerase(l, n);
			printli(l);
		}
		n = tn;
	}

	lclear(l);
	list_init(l, LSIZE);
	printf("erasing every even:\n");
	printli(l);
	n = l->head;
	while (n) {
		tn = n->next;
		if (!((long)n->data % 2)) {
			lerase(l, n);
			printli(l);
		}
		n = tn;
	}

	lclear(l);
	list_init(l, LSIZE);
	printf("erasing every even backward:\n");
	printli(l);
	n = l->tail;
	while (n) {
		tn = n->prev;
		if (!((long)n->data % 2)) {
			lerase(l, n);
			printli(l);
		}
		n = tn;
	}


	lclear(l);
	return 0;
}


void testTree()
{
	btree bt, *t;
	t = &bt;
	btinit(t, NULL);

	printf("sizeof empty tree: %ld\n", (long)btsize(t));


	long tab[] = { 16, 8, 24, 12, 36, 44, 50,
	               30, 20, 27, 21, 22, 4, 2, 1, 5 };
	const int BTSIZE = sizeof tab / sizeof(long);
	long i;
	for (i = 0; i < BTSIZE; ++i) {
		printf("inserting %ld\n", tab[i]);
		btinsert(t, (void *)tab[i]);
	}
	printf("size after %d inserts: %ld\n", BTSIZE, (long)btsize(t));
	bin_tree_print(t->root);

	printf("searching keys [0 to 50]\n");
	for (i = 0; i < 51; ++i) {
		btnode *f;
		if ((f = btfind(t, (void*)i))) 
			printf("%ld ", (long)f->data);
	}
	balance_Day(t);
	printf("\nafter Day's balance\n");
	bin_tree_print(t->root);
	printf("\n\n\n");

	i = 10;
	int dsw = 0;
	for (dsw = 0; dsw < 2; ++dsw) {
		btclear(t);
		long n;
		for (n = 0; n < i; ++n)
			btinsert(t, (void *)n);
		printf("tree #%ld(size:%ld):\n", i, (long)btsize(t));
		bin_tree_print(t->root);
		if (dsw) {
			printf("DSW balancing\n");
			balance_DSW(t);
			printf("after DSW balance:\n");
		} else {
			printf("Day's balancing\n");
			balance_Day(t);
			printf("after Day's balance:\n");	
		}
		bin_tree_print(t->root);
	}

	
	/* btprint(t); */
	/* printf("bt_Day_balance:\n"); */
	/* balance_Day(t); */
	/* printf("after\n"); */
	/* btprint(t); */

	/* printf("bt_DSW_balance:\n"); */
	/* balance_DSW(t); */
	/* printf("after\n"); */
	/* btprint(t); */

	/* printf("removig children:\n"); */
	/* long chld[] = {27, 33, 40, 50, 12, 20}; */
	/* for (i = 0; i < sizeof(chld)/sizeof(long); ++i) { */
	/* 	printf("erase: %ld\n", chld[i]); */
	/* 	bterase(t, (void *)chld[i]); */
	/* 	btprint(t); */
	/* } */


	/* printf("removig inner nodes with two childred:\n"); */
	/* long inner_one[] = {36, 24}; */
	/* for (i = 0; i < sizeof(inner_one)/sizeof(long); ++i) { */
	/* 	printf("erase: %ld\n", inner_one[i]); */
	/* 	bterase(t, (void *)inner_one[i]); */
	/* 	btprint(t); */
	/* } */


	/* printf("removig inner nodes with one child:\n"); */
	/* long inner_one[] = {8, 44, 30}; */
	/* for (i = 0; i < sizeof(inner_one)/sizeof(long); ++i) { */
	/* 	printf("erase: %ld\n", inner_one[i]); */
	/* 	bterase(t, (void *)inner_one[i]); */
	/* 	btprint(t); */
	/* } */

	/* printf("removig all nodes:\n"); */
	/* for (i = 0; i < BTSIZE; ++i) { */
	/* 	printf("erase: %ld\n", tab[i]); */
	/* 	bterase(t, (void *)tab[i]); */
	/* 	btprint(t); */
	/* } */


	/* printf("inserting keys [0 to 50]\n"); */
	/* for (i = 0; i < 51; ++i)  */
	/* 	if (btinsert(t, (void *)i) != NULL) */
	/* 		if (errno == EEXIST) */
	/* 			fprintf(stderr, "%ld already in set\n", i); */
	/* printf("size after isertion: %ld\n", (long)btsize(t)); */
	/* btprint(t); */

	btclear(t);

	for (i = 0; i < 20; ++i)
		printf("%ld: %ld\n", i, (long)v2t_DSW_FullSize(i));
}

int main(int argc, char *argv[])
{
//	testList();
	testTree();
	exit(EXIT_SUCCESS);
}
