#include <stdio.h>
#include <stdlib.h>
#include "libpriqueue/libpriqueue.h"
int compare1(const void * a, const void * b)
{
    return ( *(int*)a - *(int*)b );
}
int compare2(const void * a, const void * b)
{
    return ( *(int*)b - *(int*)a );
}
int main() {
    priqueue_t q; 
    priqueue_init(&q, compare1);
    int *values = malloc(100 * sizeof(int));
    int i;
    for (i = 0; i < 100; i++)
        values[i] = i;
    for(i = 30; i <35; i++)
        priqueue_offer(&q, &values[i]);
    for(i=0; i<10; i++)
        priqueue_offer(&q, &values[i]);
    printf("queue size = %d; should be 15\n", priqueue_size(&q));

    printf("\n");
    printf("Printing queue, should be in order from 0-9 and 30-34\n");

    for (i = 0; i < priqueue_size(&q); i++)
        printf("%d ", *((int *)priqueue_at(&q, i)) );

    printf("\n");

    printf("\n");
    void* temp = priqueue_remove_at(&q, 17);
    if(temp==NULL)
        printf("Properly handled out-of-bounds remove_at\n");
    else
        printf("Did NOT properly handle out-of-bounds remove_at\n");

    printf("\n");
    printf("\n");

    int vals_removed = priqueue_remove(&q, &values[99]);
    if(vals_removed == 0)
        printf("properly ignored values that aren't in queue\n");
    else
        printf("removed values that shouldn't have been removed!\n");

    printf("\n");
    printf("\n");
    vals_removed= priqueue_remove(&q, &values[0]);
    if(vals_removed == 1)
        printf("properly removed values that are at the front of the queue\n");
    else
        printf("removed values that shouldn't have been removed! from the front of the queue\n");

    printf("\n");
    printf("\n");



    int j = priqueue_size(&q);
    for(i = 0; i<j; i++)
    {
        priqueue_remove_at(&q,0);

    }
    if(priqueue_size(&q) ==0)
        printf("properly deleted all elements from the queue using remove_at\n");
    else 
        printf("didn't get rid of all values from queue using remove_at(0) ( did you decrement size?) \n");

    printf("\n");

    priqueue_offer(&q, &values[1]);
    priqueue_offer(&q, &values[2]);
    priqueue_offer(&q, &values[1]);
    priqueue_offer(&q, &values[1]);
    priqueue_offer(&q, &values[1]);
    priqueue_offer(&q, &values[3]);
    priqueue_offer(&q, &values[1]);

    vals_removed= priqueue_remove(&q, &values[3]);
    if(vals_removed == 1)
        printf("properly removed values that are within the  queue\n");
    else
        printf("did not propely remove values within queue\n");

    printf("\n");
    printf("printing queue, should be 1 1 1 1 1 2\n");
    for (i = 0; i < priqueue_size(&q); i++)
        printf("%d ", *((int *)priqueue_at(&q, i)) );

    printf("\n");
    printf("\n");

    vals_removed= priqueue_remove(&q, &values[1]);
    if(vals_removed == 5)
        printf("properly removed values that are at the front, end, and within the queue\n");
    else
        printf("did not propely remove values within queue\n");

    printf("\n");
    printf("\n");
    printf("printing queue, should be 2\n");
    for (i = 0; i < priqueue_size(&q); i++)
        printf("%d ", *((int *)priqueue_at(&q, i)) );


    printf("\n");
    priqueue_offer(&q, &values[1]);
    priqueue_offer(&q, &values[2]);
    priqueue_offer(&q, &values[3]);
    priqueue_offer(&q, &values[4]);
    priqueue_offer(&q, &values[5]);

    printf("size of queue is currently: %d (Should be 6)\n", priqueue_size(&q));
    printf("\n");

    printf("removing the last element from the queue using remove_at\n");
    printf("\n");
    void * tester = priqueue_remove_at(&q, priqueue_size(&q)-1);
    if(tester !=NULL) printf("properly got the last element in the queue and that value is %d,( it should be 5)\n",*((int*)tester) );
    else printf("failed to get the last element in queue\n");

    j = priqueue_size(&q);
    for(i = 0; i<j; i++)
    {
        priqueue_remove_at(&q,0);

    }
    printf("\n");
    printf("size is now:%d (should be zero)\n",priqueue_size(&q));

    priqueue_offer(&q, &values[11]);
    priqueue_offer(&q, &values[11]);
    priqueue_offer(&q, &values[11]);
    priqueue_offer(&q, &values[11]);
    priqueue_offer(&q, &values[11]);
    int removeAll = priqueue_remove(&q, &values[11]);
    if(removeAll == 5)
        printf("removed all values in queue when they're all the same\n");
    else printf("didn't remove all values in queue when they're all the same\n");

   
    printf("printing queue, should be empty\n");
    for (i = 0; i < priqueue_size(&q); i++)
        printf("%d ", *((int *)priqueue_at(&q, i)) );

    printf("\n");

    printf("printing size: %d, (should be zero)\n", priqueue_size(&q));

    priqueue_offer(&q, &values[89]);

    int removeOne =  priqueue_remove(&q, &values[89]);
    if(removeOne == 1)
        printf("properly removed a single value from the queue\n");
    else printf("did not properly remove value when it's the only one in the queue\n");
     priqueue_destroy(&q);
    free(values);
    return 0;



}
