#include <time.h>
#include <stdio.h>
#include "avltest.h"
#include "avltree.h"
#include "list.h"



void testAVL(long size)
{	
	avltree atree, *t;
	t = &atree;
	avlinit(t, NULL);

	long tab[] = { 3, 5, 1,9, 0, 7,8,6};
//	long tab[] = { 3, 5, 10};
	const int TAB_SIZE = sizeof tab / sizeof(long);
	long i;
	for (i = 0; i < TAB_SIZE; ++i) {
		avlinsert(t, (void *)tab[i]);
		avlprint(t);
		printf("\n");
		printf("\n");		
	}


	printf("\n\nIteration\n");
	avliterator it;
	if (avlbegin(t, &it)) {
		avlitdesc(&it);
		while (avlitnext(&it))
			avlitdesc(&it);
	}
	printf("\n\n");
	       
	
	avlclear(t);
	

}

int insert_unique_random(avltree *t, long size, list *insertorder)
{
	long *tab = malloc(size * sizeof(long));

	long i;
	for (i = 0; i < size; ++i)
		tab[i] = i;
	while (size) {
		long r = ((long)rand() << 32) | rand();
		r %= size;
		/* printf("%ld ", tab[r]); */
		
		if (insertorder)
			lpush_back(insertorder, (void *)tab[r]);
		if (avlinsert(t, (void *)tab[r]) == NULL)
			return 0;
		tab[r] = tab[size - 1];
		--size;
	}
	/* putchar('\n'); */
		
	free(tab);
	return 1;
}

int avlittest(avltree *t, long size, long repeat, list *insertorder)
{
	long i;
	for (i = 0; i < repeat; ++i) {
		insert_unique_random(t, size, insertorder);
		avliterator it;
	
		long expected = 0;
		if (avlbegin(t, &it)) {
			if ((long)it.node->data != expected) {
				fprintf(stderr, "%ld %ld\n",
				        expected, (long)it.node->data);
				return 0;
			}
			/* avlitdesc(&it); */
			while (avlitnext(&it)) {
				++expected;
				if ((long)it.node->data != expected) {
					fprintf(stderr, "%ld %ld\n",
					        expected, (long)it.node->data);
					return 0;
				}
			}
		}
	}
	return 1;
}

void avlwalk(avltree *t)
{
	/* chciał bym  by iteratory działały tak: */
	/* for (iter i = begin(); i != end(); it.next()) */
	/* 	; */
	
	avlprint(t);
	avliterator it;
	if (avlbegin(t, &it)) {
		avlitdesc(&it);
		while (avlitnext(&it))
			avlitdesc(&it);
	}
}

void avlinsertarray(avltree *t, long * ar, size_t size)
{
	int i;
	for (i = 0; i < size; ++i)
		avlinsert(t, (void *)ar[i]);
}







/* void testTree() */
/* { */
/* 	btree bt, *t; */
/* 	t = &bt; */
/* 	btinit(t, NULL); */

/* 	printf("sizeof empty tree: %ld\n", (long)btsize(t)); */


/* 	long tab[] = { 16, 8, 24, 12, 36, 44, 50, */
/* 	               30, 20, 27, 21, 22, 4, 2, 1, 5 }; */
/* 	const int BTSIZE = sizeof tab / sizeof(long); */
/* 	long i; */
/* 	for (i = 0; i < BTSIZE; ++i) { */
/* 		printf("inserting %ld\n", tab[i]); */
/* 		btinsert(t, (void *)tab[i]); */
/* 	} */
/* 	printf("size after %d inserts: %ld\n", BTSIZE, (long)btsize(t)); */
/* 	bin_tree_print(t->root); */

/* 	printf("searching keys [0 to 50]\n"); */
/* 	for (i = 0; i < 51; ++i) { */
/* 		btnode *f; */
/* 		if ((f = btfind(t, (void*)i)))  */
/* 			printf("%ld ", (long)f->data); */
/* 	} */
/* 	balance_Day(t); */
/* 	printf("\nafter Day's balance\n"); */
/* 	bin_tree_print(t->root); */
/* 	printf("\n\n\n"); */

/* 	i = 10; */
/* 	int dsw = 0; */
/* 	for (dsw = 0; dsw < 2; ++dsw) { */
/* 		btclear(t); */
/* 		long n; */
/* 		for (n = 0; n < i; ++n) */
/* 			btinsert(t, (void *)n); */
/* 		printf("tree #%ld(size:%ld):\n", i, (long)btsize(t)); */
/* 		bin_tree_print(t->root); */
/* 		if (dsw) { */
/* 			printf("DSW balancing\n"); */
/* 			balance_DSW(t); */
/* 			printf("after DSW balance:\n"); */
/* 		} else { */
/* 			printf("Day's balancing\n"); */
/* 			balance_Day(t); */
/* 			printf("after Day's balance:\n");	 */
/* 		} */
/* 		bin_tree_print(t->root); */
/* 	} */

	
/* 	/\* btprint(t); *\/ */
/* 	/\* printf("bt_Day_balance:\n"); *\/ */
/* 	/\* balance_Day(t); *\/ */
/* 	/\* printf("after\n"); *\/ */
/* 	/\* btprint(t); *\/ */

/* 	/\* printf("bt_DSW_balance:\n"); *\/ */
/* 	/\* balance_DSW(t); *\/ */
/* 	/\* printf("after\n"); *\/ */
/* 	/\* btprint(t); *\/ */

/* 	/\* printf("removig children:\n"); *\/ */
/* 	/\* long chld[] = {27, 33, 40, 50, 12, 20}; *\/ */
/* 	/\* for (i = 0; i < sizeof(chld)/sizeof(long); ++i) { *\/ */
/* 	/\* 	printf("erase: %ld\n", chld[i]); *\/ */
/* 	/\* 	bterase(t, (void *)chld[i]); *\/ */
/* 	/\* 	btprint(t); *\/ */
/* 	/\* } *\/ */


/* 	/\* printf("removig inner nodes with two childred:\n"); *\/ */
/* 	/\* long inner_one[] = {36, 24}; *\/ */
/* 	/\* for (i = 0; i < sizeof(inner_one)/sizeof(long); ++i) { *\/ */
/* 	/\* 	printf("erase: %ld\n", inner_one[i]); *\/ */
/* 	/\* 	bterase(t, (void *)inner_one[i]); *\/ */
/* 	/\* 	btprint(t); *\/ */
/* 	/\* } *\/ */


/* 	/\* printf("removig inner nodes with one child:\n"); *\/ */
/* 	/\* long inner_one[] = {8, 44, 30}; *\/ */
/* 	/\* for (i = 0; i < sizeof(inner_one)/sizeof(long); ++i) { *\/ */
/* 	/\* 	printf("erase: %ld\n", inner_one[i]); *\/ */
/* 	/\* 	bterase(t, (void *)inner_one[i]); *\/ */
/* 	/\* 	btprint(t); *\/ */
/* 	/\* } *\/ */

/* 	/\* printf("removig all nodes:\n"); *\/ */
/* 	/\* for (i = 0; i < BTSIZE; ++i) { *\/ */
/* 	/\* 	printf("erase: %ld\n", tab[i]); *\/ */
/* 	/\* 	bterase(t, (void *)tab[i]); *\/ */
/* 	/\* 	btprint(t); *\/ */
/* 	/\* } *\/ */


/* 	/\* printf("inserting keys [0 to 50]\n"); *\/ */
/* 	/\* for (i = 0; i < 51; ++i)  *\/ */
/* 	/\* 	if (btinsert(t, (void *)i) != NULL) *\/ */
/* 	/\* 		if (errno == EEXIST) *\/ */
/* 	/\* 			fprintf(stderr, "%ld already in set\n", i); *\/ */
/* 	/\* printf("size after isertion: %ld\n", (long)btsize(t)); *\/ */
/* 	/\* btprint(t); *\/ */

/* 	btclear(t); */

/* 	for (i = 0; i < 20; ++i) */
/* 		printf("%ld: %ld\n", i, (long)v2t_DSW_FullSize(i)); */
/* } */

int avltest_removing()
{
	return 0;
}


int avltest()
{
	srand(time(NULL));

	avltree avlt, *t;
	list order;
	linit(&order);

	t = &avlt;
	avlinit(t, NULL);

	/* te 3 linie spokojnie można wyjebać
	 * (to się nazywa dokumentowanie kodu ;) */
	insert_unique_random(t, 10, NULL);
	avlprint(t);
	exit(0);
	
		
	
	int rv;
	while ((rv = insert_unique_random(t, 1000, &order))) {
		avlclear(t);
		lclear(&order);
	}

	lnode *pn = order.head;
	printf("order:\n");
	while (pn) {
		printf("%ld ", (long)pn->data);
		pn = pn->next;
	}
	putchar('\n');
	exit(1);
	
	/* avlcheckbalance(t); */
	/* long tab[] = { 8, 3, 9, 0, 5, 2, 1, 4, 7, 6 }; */
	/* avlinsertarray(t, tab, sizeof tab / sizeof(long)); */
	/* avlwalk(t); */

	list insertorder;
	linit(&insertorder);
	
	if (!avlittest(t, 10, 1, &insertorder))
		fprintf(stderr, "avlittest failed\n");

	//	testList();
    //	testTree();
    //	testAVL(1000);


}
