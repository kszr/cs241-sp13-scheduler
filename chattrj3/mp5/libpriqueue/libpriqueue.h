/** @file libpriqueue.h
 */

#ifndef LIBPRIQUEUE_H_
#define LIBPRIQUEUE_H_


/**
 * A single entry in this priority queue,
 * which is implemented as a linked list.
 */
typedef struct _priqueue_entry_t {
    struct _priqueue_entry_t *next;
    void *data;
	int index;
} entry_t;

/**
  Priqueue Data Structure
*/
typedef struct _priqueue_t
{
	entry_t *head;
	int size;
	int (*comparitor)(const void *, const void *);
} priqueue_t;

void   priqueue_init     (priqueue_t *q, int(*comparer)(const void *, const void *));

int    priqueue_offer    (priqueue_t *q, void *ptr);
void * priqueue_peek     (priqueue_t *q);
void * priqueue_poll     (priqueue_t *q);
void * priqueue_at       (priqueue_t *q, int index);
int    priqueue_remove   (priqueue_t *q, void *ptr);
void * priqueue_remove_at(priqueue_t *q, int index);
int    priqueue_size     (priqueue_t *q);

void   priqueue_destroy  (priqueue_t *q);

#endif /* LIBPQUEUE_H_ */
