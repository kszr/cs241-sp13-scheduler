/* code goes here */
/** @file queuetest.c
 */

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
        priqueue_t q; //q2;

        priqueue_init(&q, compare1);
        //priqueue_init(&q2, compare2);

        /* Pupulate some data... */
        int *values = malloc(100 * sizeof(int));

        int i;
        for (i = 0; i < 100; i++)
                values[i] = i;

        // Test 1: Remove function
        printf("Putting five 10s on the queue\n");
        priqueue_offer(&q, &values[10]);
        priqueue_offer(&q, &values[10]);
        priqueue_offer(&q, &values[10]);
        priqueue_offer(&q, &values[10]);
        priqueue_offer(&q, &values[10]);        
        printqueue(&q);

        printf("Removing five 10s from the queue\n");
        int vals_removed = priqueue_remove(&q, &values[10]);
        printqueue(&q);
        printf("Removed %d values from the queue\n", vals_removed);


        // Test 2: Interleaved remove function
        printf("Putting five 10s, two 20s, one 30, two 1s on the queue\n");
        priqueue_offer(&q, &values[10]);
        priqueue_offer(&q, &values[1]);
        priqueue_offer(&q, &values[30]);
        priqueue_offer(&q, &values[10]);
        priqueue_offer(&q, &values[20]);
        priqueue_offer(&q, &values[10]);
        priqueue_offer(&q, &values[20]);
        priqueue_offer(&q, &values[10]);
        priqueue_offer(&q, &values[10]);
        priqueue_offer(&q, &values[1]);
        printqueue(&q);

        printf("Removing five 10s from the queue\n");
        vals_removed = priqueue_remove(&q, &values[10]);
        printqueue(&q);
        printf("Removed %d values from the queue\n", vals_removed);

        printf("Removing two 1s from the queue\n");
        vals_removed = priqueue_remove(&q, &values[1]);
        printqueue(&q);
        printf("Removed %d values from the queue\n", vals_removed);



        // Test 3: Basics (20, 20, 30)
        printf("Polling (removing) top element\n");
        void * temp = priqueue_poll(&q);
        printf("Top element is %d\n", *((int *) temp));
        printqueue(&q);
        
        // (20, 30)
        printf("Peeking top element\n");
        temp = priqueue_peek(&q);
        printf("Top element is %d\n", *((int *) temp));
        printqueue(&q);

        printf("Querying nonexistant element\n");
        temp = priqueue_at(&q, 2);
        if(temp != NULL) printf("Warning, priqueue_at doesn't work");
        else printf("Cool, determined that index DNE\n");



        // Test 4: Remove functions
        // Check whether last element is NULL
        temp = priqueue_remove_at(&q, 2);
        if(temp != NULL) printf("Warning, priqueue_remove_at doesn't work if index DNE\n");
        else printf("Cool, determined that index DNE\n");
        printqueue(&q);

        priqueue_offer(&q, &values[40]);
        priqueue_offer(&q, &values[50]);
        temp = priqueue_remove_at(&q, 2);
        printf("Removed element %d\n", *((int *) temp));
        printqueue(&q);

        printf("Emptying queue\n");
        priqueue_destroy(&q);
        printqueue(&q);


        /* Add 5 values, 3 unique. */
/*
        printf("12 is in place %d\n", priqueue_offer(&q, &values[12]));
        printf("Printing 12\n");
        printqueue(&q);

        printf("\nPutting 13 and 14\n");
        printf("13 is in place %d\n", priqueue_offer(&q, &values[13]));
        printf("14 is in place %d\n", priqueue_offer(&q, &values[14]));
        printqueue(&q);
        
        printf("\nPrinting the whole list\n");
        priqueue_offer(&q, &values[12]);
        priqueue_offer(&q, &values[12]);
        printqueue(&q);

        printf("Total elements: \t%d (expected 5).\n", priqueue_size(&q));

        int val = *((int *)priqueue_poll(&q));
        printf("Top element: \t\t%d (expected 12).\n", val);
        printf("Total elements: \t%d (expected 4).\n", priqueue_size(&q));
        printf("Printing the whole queue:\n");
        printqueue(&q);
        printf("\n");

        int vals_removed = priqueue_remove(&q, &values[12]);
        printf("Elements removed: \t%d (expected 2).\n", vals_removed);
        printf("Total elements: \t%d (expected 2).\n", priqueue_size(&q));
        printf("Printing the whole queue:\n");
        printqueue(&q);
        printf("\n");



        printf("Putting 10, 30, and 20 on queue\n");
        printf("10 is in place %d\n", priqueue_offer(&q, &values[10]));
        printf("30 is in place %d\n", priqueue_offer(&q, &values[30]));
        printf("Printing the whole queue:\n");
        printqueue(&q);
        printf("\n");



        printf("20 is in place %d\n", priqueue_offer(&q, &values[20]));
        printf("Printing the whole queue:\n");
        printqueue(&q);
        printf("\n");

        priqueue_offer(&q2, &values[10]);
        priqueue_offer(&q2, &values[30]);
        priqueue_offer(&q2, &values[20]);


        printf("Elements in order queue (expected 10 13 14 20 30): ");
        for (i = 0; i < priqueue_size(&q); i++)
                printf("%d ", *((int *)priqueue_at(&q, i)) );
        printf("\n");

        printf("Elements in reverse order queue (expected 30 20 10): ");
        for (i = 0; i < priqueue_size(&q2); i++)
                printf("%d ", *((int *)priqueue_at(&q2, i)) );
        printf("\n");
*/

        


//        priqueue_destroy(&q2);
        priqueue_destroy(&q);

        free(values);

        return 0;


}
