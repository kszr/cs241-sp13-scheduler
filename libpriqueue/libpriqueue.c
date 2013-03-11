/** @file libpriqueue.c
 */

#include <stdlib.h>
#include <stdio.h>

#include "libpriqueue.h"


/**
  Initializes the priqueue_t data structure.
  
  Assumtions
    - You may assume this function will only be called once per instance of priqueue_t
    - You may assume this function will be the first function called using an instance of priqueue_t.
  @param q a pointer to an instance of the priqueue_t data structure
  @param comparer a function pointer that compares two elements.
  See also @ref comparer-page
 */
void priqueue_init(priqueue_t *q, int(*comparer)(const void *, const void *))
{
	q->head = NULL;
	q->size = 0;
	q->comparitor = comparer;
}


/**
  Inserts the specified element into this priority queue.

  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr a pointer to the data to be inserted into the priority queue
  @return The zero-based index where ptr is stored in the priority queue, where 0 indicates that ptr was stored at the front of the priority queue.
 */
int priqueue_offer(priqueue_t *q, void *ptr)
{
	//The latest entry, named ironically after the oldest man in the Bible.
	entry_t *methuselah = (entry_t *) malloc(sizeof(entry_t));
	methuselah->data = ptr;

	q->size++;

	//If the queue is empty;
	if(!q->head) {
		q->head = methuselah;
		methuselah->next = NULL;
		return methuselah->index = 0;
	}

	entry_t *curr = q->head;
	entry_t *prev = NULL;
	
	int hathchanged = 0;

	while(curr) {
		if(!hathchanged && (q->comparitor)(curr->data, methuselah->data) > 0) {
			//If curr == q->head
			if(!prev) {
				q->head = methuselah;
				methuselah->index = 0;	
			}
			else {
				prev->next = methuselah;
				methuselah->index = prev->index + 1;
			}
			methuselah->next = curr;
			hathchanged = 1;
		}
		if(hathchanged)
			curr->index++;

		prev = curr;
		curr = curr->next;
	}
	
	if(!hathchanged) {
		prev->next = methuselah;
		methuselah->next = NULL;
		methuselah->index = prev->index + 1;
	}

	return methuselah->index;

}


/**
  Retrieves, but does not remove, the head of this queue, returning NULL if
  this queue is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return pointer to element at the head of the queue
  @return NULL if the queue is empty
 */
void *priqueue_peek(priqueue_t *q)
{
	if(!q->head)
		return NULL;
	
	return q->head->data;
}


/**
  Retrieves and removes the head of this queue, or NULL if this queue
  is empty.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the head of this queue
  @return NULL if this queue is empty
 */
void *priqueue_poll(priqueue_t *q)
{
	if(!q->head)
		return NULL;
	
	void *data = q->head->data;

	entry_t *curr  = q->head->next;

	while(curr) {
		curr->index--;
		curr = curr->next;
	}

	curr = q->head;
	q->head = q->head->next ? q->head->next : NULL; //I don't know if this check was necessary.
	
	free(curr);
	
	q->size--;
	return data;
}


/**
  Returns the element at the specified position in this list, or NULL if
  the queue does not contain an index'th element.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of retrieved element
  @return the index'th element in the queue
  @return NULL if the queue does not contain the index'th element
 */
void *priqueue_at(priqueue_t *q, int index)
{
	entry_t *curr = q->head;

	while(curr) {
		if(curr->index == index)
			return curr->data;
	
		curr = curr->next;
	}
	
	return NULL;
}


/**
  Removes all instances of ptr from the queue. 
  
  This function should not use the comparer function, but check if the data contained in each element of the queue is equal (==) to ptr.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param ptr address of element to be removed
  @return the number of entries removed
 */
int priqueue_remove(priqueue_t *q, void *ptr)
{	
	entry_t *curr = q->head;
	entry_t *prev = NULL;
	int removed = 0;

	if(!q->head) return removed;

	int i;
	while(curr) {
		if(ptr == curr->data) {
			removed++;
			q->size--;
			if(!prev) {
				q->head = curr->next;
				entry_t *temp = curr;
				curr = curr->next;
				free(temp);
				continue;
			}
			prev->next = curr->next;
			entry_t *temp = curr;
			curr = prev;
			free(temp);
		}
		prev = curr;
		curr = curr->next;
	}
	//Fixing the indices
	int index = 0;
	curr = q->head;
	while(curr) {
		curr->index = index++;
		curr = curr->next;
	}

	return removed;
}


/**
  Removes the specified index from the queue, moving later elements up
  a spot in the queue to fill the gap.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @param index position of element to be removed
  @return the element removed from the queue
  @return NULL if the specified index does not exist
 */
void *priqueue_remove_at(priqueue_t *q, int index)
{
	entry_t *curr = q->head;
    entry_t *prev = NULL;
    int found = 0;

    while(curr && !found) {
        if(curr->index == index) {
            if(!prev)
                q->head = curr->next;
			else prev->next = curr->next;
            
			found = 1;
        }

        prev = curr;
        curr = curr->next;
    }

    if(found) {
        void *data = prev->data;
        free(prev);
        while(curr) {
            curr->index--;
            curr = curr->next;
        }
		
		q->size--;
        return data;
    }
    return NULL;
}


/**
  Returns the number of elements in the queue.
 
  @param q a pointer to an instance of the priqueue_t data structure
  @return the number of elements in the queue
 */
int priqueue_size(priqueue_t *q)
{
	return q->size;
}


/**
  Destroys and frees all the memory associated with q.
  
  @param q a pointer to an instance of the priqueue_t data structure
 */
void priqueue_destroy(priqueue_t *q)
{
	entry_t *curr = q->head;
	entry_t *prev = NULL;

	while(curr) {
		if(prev)
			free(prev);

		prev = curr;
		curr = curr->next;
	}

	free(prev);
}
