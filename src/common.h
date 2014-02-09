#ifndef COMMON_H
#define COMMON_H


typedef struct binary_tree_node btnode;


typedef int (*comp_fun)(const void *ldata, const void *rdata);
typedef void (*visitor_t)(btnode *n);


int lt(const void *l, const void *r);


int max(int a, int b);


int bin_tree_height(btnode *node);


/* renderer = NULL lub adres funkcji przyjmującej btnode* i
 * zapisująca do char ** interpretacje danych zawartą w nodzie
 */
void bin_tree_print(btnode *n, void (*renderer)(btnode *, char *));
	
void free_node(btnode *n, int destroy_data);

void free_node_only(btnode *n);

void free_node_destroy(btnode *n);


#endif	/* COMMON_H */
