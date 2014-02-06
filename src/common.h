#ifndef COMMON_H
#define COMMON_H


typedef struct binary_tree_node btnode;


typedef int (*comp_fun)(const void *ldata, const void *rdata);
typedef void (*visitor_t)(btnode *n);


int lt(const void *l, const void *r);


int max(int a, int b);


/* oj przydałoby się dziedziczenie TT */
int bin_tree_height(btnode *node);


void bin_tree_print(btnode *node);



#endif	/* COMMON_H */
