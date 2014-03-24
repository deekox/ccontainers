#include <time.h>
#include <stdio.h>
#include <assert.h>
#include "avltest.h"
#include "../../include/avltree.h"
#include "../../include/list.h"

struct AVLError {
	enum EKind {
		ESuccess,
		EParentChildMismatch, 
		EParentChildSelf, /* left/right points to istelf */
		ESize,
		ERoot,  /* tree->root->parent != NULL */
		EKey  /* wrong data returned */
	} kind;
	avlnode *node;
	void *key;
};

void initAVLError(struct AVLError *e)
{
	e->kind = ESuccess;
	e->node = NULL;
	e->key = NULL;
}


static void printAVLError(struct AVLError *e)
{
#define PR_AVLE(er) case er: printf(#er); fflush(stdout); break;
	switch (e->kind) {
		PR_AVLE(ESuccess);
	case EParentChildMismatch:
		printf("EParentChildMismatch: ");
		fflush(stdout);
		printf("P(%ld)[%p]_PL[%p]_RC[%p] ",
		       (long)e->key, e->node, e->node->left, e->node->right);
		if (e->node->left && e->node->left != e->node->left->parent)
			printf("CL(%ld)_PAR[%p] ", (long)e->key, e->node->left->parent);
		if (e->node->right && e->node->right != e->node->right->parent)
			printf("CR(%ld)_PAR[%p] ", (long)e->key, e->node->right->parent);
		break;
		PR_AVLE(EParentChildSelf);
		PR_AVLE(ESize);
		PR_AVLE(ERoot);
		PR_AVLE(EKey);
	}
#undef PR_AVLE
}

static void printlist(list *l)
{
	lnode *it = l->head;
	while (it) {
		printf("%ld ", (long)it->data);
		it = it->next;
	}
	printf("\n");
}


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



void avlWalk(avltree *t)
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

void avlInsertArray(avltree *t, long * ar, size_t size)
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


/* TODO: check if in order walk gives strictly increasing monotonic sequence */
static int avlConsistencyCheck2(avlnode *n, int *ncount, struct AVLError *er)
{
	if (n) {
		++*ncount;
		if (n->left) 
			if (n->left->parent != n) {
				er->kind = EParentChildMismatch;
				return 0;
			}
		if (n->right)
			if (n->right->parent != n) {
				er->kind = EParentChildMismatch;
				return 0;
			}
		if (n->right == n || n->left == n) {
			er->kind = EParentChildSelf;
			return 0;
		}
		if (!avlConsistencyCheck2(n->left, ncount, er) ||
		    !avlConsistencyCheck2(n->right, ncount, er))
		    return 0;
	}
	return 1;
}

static int avlConsistencyCheck(avltree *t, struct AVLError *er)
{
	avlnode *root = t->root;
	if (!root) {
		if (avlsize(t) != 0) {
			er->kind = ESize;
			return 0;
		}
		return 1;
	} else {
		if (root->parent) {
			er->kind = ERoot;
			return 0;
		}
	}
	int ncount = 0;
	if (avlConsistencyCheck2(root, &ncount, er)) {
		if (avlsize(t) != ncount) {
			er->kind = ESize;
			return 0;
		}
	}
	return 1;
}


static int insertUniqueRandom(avltree *t, long size, list *insertorder)
{
	long *tab = malloc(size * sizeof(long));

	long i;
	for (i = 0; i < size; ++i)
		tab[i] = i;
	while (size) {
		long r = ((long)rand() << 32) | rand();
		r %= size;
		if (insertorder)
			lpush_back(insertorder, (void *)tab[r]);
		if (avlinsert(t, (void *)tab[r]) == NULL)
			return 0;
		tab[r] = tab[size - 1];
		--size;
	}
		
	free(tab);
	return 1;
}


static long selectRandom(avltree *t)
{
	size_t n = rand() % avlsize(t), i = 0;
	avliterator it;
	
	if (avlbegin(t, &it)) {
		if (i == n) 
			return (long)it.node->key;
		else {
			while (avlitnext(&it)) {
				++i;
				if (i == n)
					return (long)it.node->key;
			}
		}
	}
	assert(1 == 0);
	return 0;
}

static int removeRandom(avltree *t, list *removeOrder, struct AVLError *er)
{
	while (!avlempty(t)) {
		long rkey = selectRandom(t);
		lpush_back(removeOrder, (void *)rkey);
		void *key = avlerase(t, (void *)rkey);
		
		avlConsistencyCheck(t, er);
		if (er->kind != ESuccess)
			return 0;
		if (rkey != (long)key) {
			er->kind = EKey;
			er->key = key;
			return 0;
		}
	}
	return 1;
}


int avlTestRemoving(int size, int loops)
{
	avltree avlt, *t;
	t = &avlt;
	avlinit(t, NULL);
	list insertOrder, removeOrder;
	linit(&insertOrder);
	linit(&removeOrder);
	int l = 0;
	for (; l < loops; ++l) {
		insertUniqueRandom(t, size, &insertOrder);
		struct AVLError er;
		initAVLError(&er);
		if (!removeRandom(t, &removeOrder, &er)) {
			printf("[%d/%d] AVLError: ", l + 1, loops);
			printAVLError(&er);
			printf("\n");
			printf("insertion order:\n");
			printlist(&insertOrder);
			printf("\nremoval order:\n");
			printlist(&removeOrder);
			printf("\n");
			
			return 0;
		}
		lclear(&insertOrder);
		lclear(&removeOrder);
	}
	return 1;
}


int avlIteratorTest(avltree *t, long size, long repeat, list *insertorder)
{
	long i;
	for (i = 0; i < repeat; ++i) {
		insertUniqueRandom(t, size, insertorder);
		avliterator it;
	
		long expected = 0;
		if (avlbegin(t, &it)) {
			if ((long)it.node->key != expected) {
				fprintf(stderr, "%ld %ld\n",
				        expected, (long)it.node->key);
				return 0;
			}
			/* avlitdesc(&it); */
			while (avlitnext(&it)) {
				++expected;
				if ((long)it.node->key != expected) {
					fprintf(stderr, "%ld %ld\n",
					        expected, (long)it.node->key);
					return 0;
				}
			}
		}
	}
	return 1;
}


int avlTest()
{

	return avlTestRemoving(200, 1000);
	
	avltree avlt, *t;
	list order;
	linit(&order);

	t = &avlt;
	avlinit(t, NULL);

	/* te 3 linie spokojnie można wyjebać
	 * (to się nazywa dokumentowanie kodu ;) */
	insertUniqueRandom(t, 10, NULL);
	avlprint(t);
	exit(0);
	
		
	
	int rv;
	while ((rv = insertUniqueRandom(t, 1000, &order))) {
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
	
	if (!avlIteratorTest(t, 10, 1, &insertorder))
		fprintf(stderr, "avlittest failed\n");

	//	testList();
    //	testTree();
    //	testAVL(1000);


}
