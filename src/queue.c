#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t *q)
{
        if (q == NULL)
                return 1;
        return (q->size == 0);
}

void enqueue(struct queue_t *q, struct pcb_t *proc)
{
        /* TODO: put a new process to queue [q] */
        /*
         * Increase the number of Processe(s) in queue by 1
         * Put in the queue the new process
         */
        if (q->size < MAX_QUEUE_SIZE)
        {
                q->proc[q->size] = proc;
                q->size++;
        }
        else
        {
                printf("Try to add a proces to a full queue_t\n");
                exit(1);
        }
}

struct pcb_t *dequeue(struct queue_t *q)
{
        /* TODO: return a pcb whose priority is the highest
         * in the queue [q] and remember to remove it from q
         * */

        // Can only process if only it is not empty
        if (empty(q))
                return NULL;

        // res hold the pcb with the highest priority
        struct pcb_t *res = q->proc[0];
        /*int idx = 0;
        for (int i = 0; i < q->size - 1; i++)
        {
                if(q->proc[i]->priority < res->priority) 
                {
                   res = q->proc[i];
                   idx = i;
                }
        }*/

        /* Remove the process from the queue
         *  shift the right to left 1 time by removing a pcb took place */
        for (int i = 0; i < q->size - 1; i++)
        {
                q->proc[i] = q->proc[i + 1];
        }

        // Set NULL to last element
        q->proc[q->size - 1] = NULL;

        // Size change by 1 cause we take 1 pcb
        q->size--;

        // Decrease the number of proces in this queue type by one, 
        // the usable slot after this call is slot--
        q->slot--;

        return (res);
}
