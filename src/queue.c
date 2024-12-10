#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
        if (q == NULL) return 1;
	return (q->size == 0);
}
void enqueue(struct queue_t *q, struct pcb_t *proc) {
   	if (q == NULL || proc == NULL) return; 

    	if (q->size >= MAX_QUEUE_SIZE) {
        	return;
    	}
    	q->proc[q->size] = proc;
    	q->size++;
}
struct pcb_t * dequeue(struct queue_t * q) {
        /* TODO: return a pcb whose priority is the highest
         * in the queue [q] and remember to remove it from q
         * */
	int sizeOfQueue = q->size;
	if( sizeOfQueue ==1 ){
		struct pcb_t* return_proc = q->proc[0];
		q->proc[0]=NULL;
		q->size =0;
		return return_proc;
	}
	else if( sizeOfQueue >1 && sizeOfQueue <= MAX_QUEUE_SIZE){
		struct pcb_t* return_proc= q->proc[0];// process will be returned
		uint32_t highest_prior = q->proc[0]->priority;
		int position =0;
		int i;
		for(i=0 ; i < sizeOfQueue ;i++){
			if( highest_prior < q->proc[i]->priority ){
				highest_prior =q->proc[i]->priority;
				position = i;
			}	
		}
		return_proc = q->proc[position];
		if(position == (sizeOfQueue -1 )){
			q->proc[position] =NULL;
			q->size = sizeOfQueue -1;
			return return_proc;
		}
		else {
			for(i = position+1;i< sizeOfQueue; i++){
				q->proc[i-1]=q->proc[i];
			}
			q->proc[sizeOfQueue -1]=NULL;
			q->size = sizeOfQueue -1;
			return return_proc;
		}	
	}
	return NULL;
}
