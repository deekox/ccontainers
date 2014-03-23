#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <errno.h>
#include <assert.h>

#include "avltree.h"
#include "list.h"

enum operation {
	INSERT,
	ERASE
};


static inline int isleftchild(const avlnode *n)
{
	assert(n);
	assert(n->parent);
	return (n->parent->left == n);
}

static inline int isaleaf(const avlnode *n)
{
	assert(n);
	return !(n->left || n->right);
}

static avlnode *sibling(const avlnode *n)
{
	assert(n);
	if (n->parent)
		return isleftchild(n) ? n->parent->right : n->parent->left;
	return NULL;
}


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
	/* long pv = -1; */
	/* if (n->parent) */
	/* 	pv = (long)n->parent->data; */
	snprintf(buf, 11, "(%2ld __ %2ld)",
	         (long)n->data, (long)n->balance);
	buf[10] = '\0';

	/* if (n->parent) */
	/* 	snprintf(buf, 10, "%2ld_%2ld_%2ld", */
	/* 	         (long)n->data, (long)n->height, (long)n->parent->data); */
	/* else */
	/* 	snprintf(buf, 10, "%2ld_%2ld_  ", (long)n->data, (long)n->height); */
}

int avltreeheight(avlnode *node)
{
	if (node) 
		return 1 + max(avltreeheight(node->left),
		               avltreeheight(node->right));
	return 0;
}

int avlchecknodebalance(avlnode *n)
{
	int d = avltreeheight(n->right) - avltreeheight(n->left);
	if (d != n->balance)
	{
		fprintf(stderr, "BAD BALANCE:[%ld] ^%d =%d\n",
		        (long)n->data, d, n->balance);
		return 0;
	}
	return 1;
	/* int lh = avltreeheight(n->left); */
	/* int rh = avltreeheight(n->right); */
	/* int d = rh - lh; */
	/* printf("[%ld] %d %d ^%d =%d\n", */
	/*        (long)n->data, lh, rh, d, n->balance); */
	/* return 0; */
}

int avlcheckbalance(avltree *t)
{
	avliterator it;
	if (avlbegin(t, &it)) {
		if (!avlchecknodebalance(it.node))
			return 0;
		while (avlitnext(&it))
			if (!avlchecknodebalance(it.node))
				return 0;
	}
	return 1;
}


int avlcheckheight(avlnode *n)
{
	/* if (n) { */
	/* 	int left = avlcheckheight(n->left); */
	/* 	int right = avlcheckheight(n->left); */
	/* 	return 1 +  */
	/* } */
	return 0;
}

int avlparenttest(avlnode *n)
{
	if (n) {
		if (n->left && n->left->parent != n) {
			long cpd = -1;
			if (n->left->parent)
				cpd = (long)n->left->parent->data;
			fprintf(stderr, "BAD PARENT L: pd: %ld   cpd: %ld\n",
			        (long)n->data, cpd);
			return 0;
		}
		if (n->right && n->right->parent != n) {
			long cpd = -1;
			if (n->right->parent)
				cpd = (long)n->right->parent->data;
			fprintf(stderr, "BAD PARENT R: pd: %ld   cpd: %ld\n",
			        (long)n->data, cpd);
			return 0;
		}
		return avlparenttest(n->left) && avlparenttest(n->right);
	}
	return 1;
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

/* try to go to the left edge */
avlnode *avl_try_LE(avlnode *n)
{
	while (n->left)
		n = n->left;
	return n;
}

/* try to go to right node and then go to left edge */
avlnode *avl_try_RLE(avlnode *n)
{
	if (n->right) {
		if (n->right->left)
			return avl_try_LE(n->right);
	}
	return n;
}

/* try to go to right node */
avlnode *avl_try_R(avlnode *n)
{
	return (n->right) ? n->right : n;
}

/* try to go to the direct parent */
avlnode *avl_try_back_direct(avlnode *n)
{
	return (n->parent) ? n->parent : n;
}



/* search for the first ancestor such that node n is its left descendant,
 * but skip first parent
 */
avlnode *avl_try_back_skip(avlnode *n)
{
	avlnode *org = n;

	if (n->parent) {
		n = n->parent;			/* skipping first ancestor */
		while (n->parent) {
			if (isleftchild(n))
				return n->parent;
			n = n->parent;
		}
	}
	return org;
}


avliterator *avlbegin(avltree *t, avliterator *it)
{
	it->dir = AVL_DIR_FORWARD;
	it->node = t->root;
	if (it->node == NULL) {
		it->move = AVL_MOV_NONE;
		return NULL;
	}

	avlnode *tmp = avl_try_LE(it->node);
	if (it->node != tmp) {
		it->node = tmp;
		it->move = AVL_MOV_LEFT_EDGE;
	} else
		it->move = AVL_MOV_BACK_SKIP;
	/* if we can't go left we should return root
	 * which is the current it*/
	return it;
}


avliterator *avlitnext(avliterator *it)
{
	if (it->node == NULL)
		return NULL;

	avlnode *tmp = avl_try_RLE(it->node);
	if (tmp != it->node) {
		it->node = tmp;
		it->move = AVL_MOV_LEFT_EDGE;
		return it;
	}

	tmp = avl_try_R(it->node);
	if (tmp != it->node) {
		it->node = tmp;
		it->move = AVL_MOV_RIGHT;
		return it;
	}


	/* we have to go back
	 * if current node is left node then back_direct
	 * else back_skip */

	if (it->node->parent) {
		if (isleftchild(it->node)) {
			tmp = avl_try_back_direct(it->node);
			if (tmp != it->node) {
				it->node = tmp;
				it->move = AVL_MOV_BACK_DIRECT;
				return it;
			}
			/* else */
			/* 	fprintf(stderr, "CANT DIRECT\n"); */
		} else {
			tmp = avl_try_back_skip(it->node);
			if (tmp != it->node) {
				it->node = tmp;
				it->move = AVL_MOV_BACK_SKIP;
				return it;
			}
			/* else */
			/* 	fprintf(stderr, "CANT SKIP\n"); */
		}
	}
	/* else  */
	/* 	fprintf(stderr, "NO PARENT\n"); */
			

	it->node = NULL;
	it->move = AVL_MOV_NONE;
	return NULL;
}


void avlitdesc(avliterator *it)
{
	if (it == NULL) 
		printf("it == NULL\n");
	else {
		printf("->%ld ", (long)it->node->data);
		switch (it->move) {
		case AVL_MOV_NONE:
			printf("NONE"); break;
		case AVL_MOV_LEFT_EDGE:
			printf("LE"); break;
		case AVL_MOV_RIGHT:
			printf("RIGHT"); break;
		case AVL_MOV_BACK_DIRECT:
			printf("BACK_DIRECT"); break;
		case AVL_MOV_BACK_SKIP:
			printf("BACK_SKIP"); break;
		default:
			printf("unknown"); break;
		}
		printf("\n");
	}
}



/* iterating the tree in Left Vist Right order */
avlnode *avl_lvr(avliterator *prev)
{
	return NULL;
}


/* checks if abs(height(left) - height(right)) < 2
 * i.e. if this subtree is AVL tree
 */
int checkavltree(avlnode *n)
{
	
	return 0;
}


static void reparent(avlnode *old_root, avlnode *new_root, avltree *t)
{
	if (old_root->parent) {
		if (isleftchild(old_root))
			old_root->parent->left = new_root;
		else
			old_root->parent->right = new_root;
	} else
		t->root = new_root;
	new_root->parent = old_root->parent;
	old_root->parent = new_root;
}

avlnode *avlrotater(avlnode *old_root, avltree *t)
{
	avlnode *new_root = old_root->left;
	if (new_root != NULL) {
		old_root->left = new_root->right;
		if (new_root->right)
			new_root->right->parent = old_root;
		new_root->right = old_root;
		reparent(old_root, new_root, t);
	}
	return new_root;
}


avlnode *avlrotatel(avlnode *old_root, avltree *t)
{
	avlnode *new_root = old_root->right;
	if (new_root != NULL) {
		old_root->right = new_root->left;
		if (new_root->left)
			new_root->left->parent = old_root;
		new_root->left = old_root;
		reparent(old_root, new_root, t);
	}
	return new_root;
}


void rebalance(avlnode *node, avltree *t)
{
	avlnode *child = (node->balance > 0) ? node->right : node->left;
	int same_sign = node->balance * child->balance;
	/* printf("[%2ld] imba, ", (long)node->data); */
	if (same_sign > 0) {				   /* one rotation only */
		/* printf("\n"); */
		/* avlprint(t); */
		/* printf("same signs\n"); */
		if (node->balance > 0)
			avlrotatel(node, t);
		else
			avlrotater(node, t);
		node->balance = child->balance = 0;
	} else {
		/* printf("diff signs\n"); */
		avlnode *grand_child = (child->balance > 0) ?
			child->right : child->left;
		assert(grand_child);
		if (node->balance > 0) {
			avlrotater(child, t);
			avlrotatel(node, t);
		} else {
			avlrotatel(child, t);
			avlrotater(node, t);
		}

		if (grand_child->balance) {
			if (grand_child->balance * node->balance < 0)
				node->balance = 0;
			else
				node->balance /= -2;
		} else {
			/* node->balance /= -2; */
			node->balance = 0;
		}
		
		
		if (grand_child->balance) {
			if (grand_child->balance * child->balance > 0)
				child->balance *= -1;
			else
				child->balance = 0;
		} else {
			child->balance = 0;
		}
		
		/* if (!!child->left  ^ !!child->right)  */
		/* 	child->balance = child->left ? -1 : 1; */
		/* else */
		/* 	child->balance = 0; */
		
		/* if (abs(node->balance) > 1) */
		/* 	fprintf(stderr, "[%ld]BAL: %d\n", (long)node->data, node->balance); */
		grand_child->balance = 0;
	}
	
	/* if (n->balance > 0) */

}


avlnode *compute_balance(avlnode *leaf, enum operation op)
{
	avlnode *node = leaf;
	node->balance = 0;
	if (node->parent != NULL) {
		if (node->parent->left == NULL || node->parent->right == NULL) {
			/* First (and only) child */
			do {
				if (node->parent->left == node)
					--node->parent->balance;
				else
					++node->parent->balance;
				if (node->parent->balance == 0)
					break;
				if (abs(node->parent->balance) == 2) 
					return node->parent;
				node = node->parent;
			} while (node->parent != NULL);
		} else {
			/* got one sibling. Assuming there cannot be -2/2 balace
			 * i can safely set parent balance to 0
			 * (because it can only be -1/1)
			 */
			node->parent->balance = 0;
		}
	}
	return NULL;
}


/* f. returns address of poiter pointig to node containig data
 * or (when node with data doesn't exist)
 * address of poiter that should point to node with data if we were
 * adding this node
 * if cmp != NULL */
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
	avlnode *n = NULL;
	if (*place == NULL) {
		n = alloc_node(data);
		if (n == NULL)
			return NULL;
		/* we return n rather than *place because
		 * rebalance can change *place to NULL
		 */
		*place = n;
		if (cmp > 0)  			/* place wskazuje na &parent->left */
			n->parent = (avlnode *)place;
		else if (cmp < 0) 	/* place = &parent->right */
			n->parent = (avlnode *)((void *)place - offsetof(avlnode, right));
		else
			n->parent = NULL; 		/* root */

		/* first node on path from n to root with abs(balance) == 2 */
		avlnode *imba = compute_balance(n, INSERT);
		if (imba) 
			rebalance(imba, t);
		++t->size;
		errno = 0;

#ifdef TEST
		compute_height(n);
		if (!avlcheckbalance(t) || !avlparenttest(t->root)) {
			if (!avlcheckbalance(t)) 
				fprintf(stderr, "\n!avlceckbalance()\n");
			if (!avlparenttest(t->root)) 
				fprintf(stderr, "\n!avlparenttest()\n");
			avlprint(t);
			return NULL;
		}
#endif
	} else {  /* próba wstawienia istniejącej wartości */
		errno = EEXIST;
	}
	
	return n;
	
}


/* removing by copying
 * To prevent invalidation of iterator pointing to previous (in order) node
 * (which will be "copied" to place where erased node was) we have to
 * do extra work (coping only key is too little). */
/* TODO: removing alternately from left/right subtree */
void *avlerase(avltree *t, void *data)
{
	avlnode **holder = avlsrch_node(t, data, NULL);
	if (*holder != NULL) {
		avlnode *erased = *holder, *dirty = NULL;
		void *data = (*holder)->data;
		
		
		if (erased->right == NULL) {
			*holder = erased->left;
			erased->left->parent = erased->parent;
		} else if (erased->left == NULL) {
			*holder = erased->right;
			erased->right->parent = erased->parent;
		} else {					
			avlnode *prev = erased->left; /* perv in sense of in order walking */
			while (prev->right)
				prev = prev->right;

			prev->right = erased->right;
			prev->right->parent = prev;
			if (prev->parent != erased) { /* prev is not erased's direct child */
				prev->parent->right = prev->left;
				if (prev->left)
					prev->left->parent = prev->parent;
			}
			prev->left = erased->left;
			prev->left->parent = prev;

			prev->parent = erased->parent;
			*holder = prev;
		}

		free_node_only((btnode *)erased);

		avlnode *imba = compute_balance(dirty, ERASE);
		if (imba) {
			/* rebalance(t, imba); */
		}


		/* en - erased node
		 * if en is a leaf:
		 *     if en is only leaf of its parent:
		 *         parent's subtree --height
		 *     else
		 *         parent's subtree height isn't changed
		 *         only parent's balance changes +-
		 */

		

		--t->size;
		return data;
	}
	return NULL;
}


avlnode *avlfind(avltree *t, void *data)
{
	return *avlsrch_node(t, data, NULL);
}

