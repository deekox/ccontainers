#include "../../include/list.h"


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
