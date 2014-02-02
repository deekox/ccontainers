#include <stdlib.h>
#include "list.h"


static lnode *alloc_list_node(void *data)
{
	lnode *n = malloc(sizeof(lnode));
	if (n == NULL)
		return NULL;
	n->data = data;
	return n;
}


static inline void free_node(lnode *n)
{
	free(n);
}


void linit(list *l)
{
	l->head = NULL;
	l->tail = NULL;
	l->size = 0;
}


void lfree(list *l, int free_data)
{
	lnode *n;
	while ((n = l->head)) {
		l->head = n->next;
		if (free_data)
			free(n->data);
		free(n);
	}
	l->head = l->tail = NULL;
	l->size = 0;
}


lnode *lpush_front(list *l, void *data)
{
	lnode *n = alloc_list_node(data);
	if (n != NULL) {
		n->prev = NULL;
		if (l->head == NULL) {
			l->head = l->tail = n;
			n->next = NULL;
		} else {
			l->head->prev = n;
			n->next = l->head;
			l->head = n;
		}
		++l->size;
	}
	return n;
}


lnode *lpush_back(list *l, void *data)
{
	lnode *n = alloc_list_node(data);
	if (n != NULL) {
		n->next = NULL;
		if (l->head == NULL) {
			n->prev = NULL;
			l->head = l->tail = n;
		} else {
			n->prev = l->tail;
			l->tail->next = n;
			l->tail = n;
		}
		++l->size;
	}
	return n;
}


lnode *linsert(list *l, lnode *before, void *data)
{
	if (l->head == NULL || before ==  NULL)
		return lpush_back(l, data);

	if (before == l->head) 		/* wstawiamy na pierwszą pozycję */
		return lpush_front(l, data);

	/* wstawiamy między dwa poprawne nody */
	lnode *n = alloc_list_node(data);
	if (n != NULL) {
		n->prev = before->prev;
		n->prev->next = n;
		n->next = before;
		before->prev = n;
		++l->size;
	}
	return n;
}


void *lpop_front(list *l)
{
	if (l->head == NULL)
		return NULL;

	void *data = l->head->data;
	lnode *newHead = l->head->next;
	free_node(l->head);

	l->head = newHead;
	if (newHead == NULL)
		l->tail = NULL;
	else
		newHead->prev = NULL;

	--l->size;
	return data;
}


void *lpop_back(list *l)
{
	if (l->head == NULL)
		return NULL;

	void *data = l->tail->data;
	lnode *newTail = l->tail->prev;
	free_node(l->tail);

	l->tail = newTail;
	if (newTail == NULL)
		l->head = NULL;
	else
		newTail->next = NULL;

	--l->size;
	return data;
}


void *lerase(list *l, lnode *n)
{
	if (n == NULL)
		return NULL;

	void *data = n->data;
	if (n->prev == NULL)		/* pierwszy na liście */
		return lpop_front(l);
	if (n->next == NULL) 		/* ostatni na liście */
		return lpop_back(l);

	/* usuwamy wewnętrzny node */
	n->prev->next = n->next;
	n->next->prev = n->prev;

	free_node(n);
	--l->size;
	return data;
}
