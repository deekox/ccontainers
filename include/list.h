#ifndef LIST_H
#define LIST_H

#include <stdlib.h>

typedef struct double_list_node lnode;
typedef struct double_list      list;

struct double_list_node {
	lnode *next;
	lnode *prev;
	void *data;
};


struct double_list {
	lnode *head;
	lnode *tail;
	size_t size;
};

void linit(list *l);
/* free_data != 0 makes free on lnode->data */
void lfree(list *l, int free_data);
static inline void lclear(list *l)
{
	lfree(l, 0);
}
static inline void ldestroy(list *l)
{
	lfree(l, 1);
}

lnode *linsert(list *l, lnode *before, void *data);
lnode *lpush_front(list *l, void *data);
lnode *lpush_back(list *l, void *data);


void *lerase(list *l, lnode *n);
void *lpop_front(list *l);
void *lpop_back(list *l);


static inline int lempty(const list *l)
{
	return (l->head == NULL);
}

static inline size_t lsize(const list *l)
{
	return l->size;
}

#endif	/* LIST_H */
