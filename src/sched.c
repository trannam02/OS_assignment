
#include "queue.h"
#include "sched.h"
#include <pthread.h>

#include <stdlib.h>
#include <stdio.h>
static struct queue_t ready_queue;
static struct queue_t run_queue;
static pthread_mutex_t queue_lock;

#ifdef MLQ_SCHED
static struct queue_t mlq_ready_queue[MAX_PRIO];
#endif

int queue_empty(void)
{
#ifdef MLQ_SCHED
	unsigned long prio;
	for (prio = 0; prio < MAX_PRIO; prio++)
		if (!empty(&mlq_ready_queue[prio]))
			return -1;
#endif
	return (empty(&ready_queue) && empty(&run_queue));
}

void init_scheduler(void)
{
#ifdef MLQ_SCHED
	int i;

	for (i = 0; i < MAX_PRIO; i++)
	{
		mlq_ready_queue[i].size = 0;
		mlq_ready_queue[i].slot = 0;
	}
#endif
	ready_queue.size = 0;
	run_queue.size = 0;
	pthread_mutex_init(&queue_lock, NULL);
}

#ifdef MLQ_SCHED
/*
 *  Stateful design for routine calling
 *  based on the priority and our MLQ policy
 *  We implement stateful here using transition technique
 *  State representation   prio = 0 .. MAX_PRIO, curr_slot = 0..(MAX_PRIO - prio)
 */
struct pcb_t *get_mlq_proc(void)
{
	struct pcb_t *proc = NULL;
	/*TODO: get a process from PRIORITY [ready_queue].
	 * Remember to use lock to protect the queue.
	 */

	// New 2 mutex lock and unlock
	pthread_mutex_lock(&queue_lock);
	int cur_prio;
	for (cur_prio = 0; cur_prio < MAX_PRIO; ++cur_prio)
	{
		if (!empty(&mlq_ready_queue[cur_prio]) && mlq_ready_queue[cur_prio].slot > 0)
		{
			proc = dequeue(&mlq_ready_queue[cur_prio]);
			break;
		}
	}
	// This might run into a problem, thus the above code --slot but
	// There might be a process somewhere in the mlq but we have use all slot
	// Due to that reason, we need to "refill" slot for all queues
	// This section only occurs few time, after CPU use once in <slot(slot - 1)/2> times
	if (cur_prio == MAX_PRIO)
	{
		// The mlq has run a cycle (all queue for its related slot)
		// Therefore, set the slot for all queues again
		for (int i = 0; i < MAX_PRIO; i++)
		{
			mlq_ready_queue[i].slot = MAX_PRIO - i;
		}

		// Take out the process
		for (cur_prio = 0; cur_prio < MAX_PRIO; ++cur_prio)
		{
			// in this loop we noo need to check the slot, cause we just pre giveslot for all queue
			// This is only true in this part cause the number of slot run from [1 -> MAX_PRIO - 1]
			// but for clear code, we would also check it
			if (!empty(&mlq_ready_queue[cur_prio]) && mlq_ready_queue[cur_prio].slot > 0)
			{
				proc = dequeue(&mlq_ready_queue[cur_prio]);
				break;
			}
		}
	}
	pthread_mutex_unlock(&queue_lock);
	return proc;
}

void put_mlq_proc(struct pcb_t *proc)
{
	pthread_mutex_lock(&queue_lock);
	enqueue(&mlq_ready_queue[proc->prio], proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_mlq_proc(struct pcb_t *proc)
{
	pthread_mutex_lock(&queue_lock);
	enqueue(&mlq_ready_queue[proc->prio], proc);
	pthread_mutex_unlock(&queue_lock);
}

struct pcb_t *get_proc(void)
{
	return get_mlq_proc();
}

void put_proc(struct pcb_t *proc)
{
	return put_mlq_proc(proc);
}

void add_proc(struct pcb_t *proc)
{
	return add_mlq_proc(proc);
}
#else
struct pcb_t *get_proc(void)
{
	struct pcb_t *proc = NULL;
	/*TODO: get a process from [ready_queue].
	 * Remember to use lock to protect the queue.
	 * */

	pthread_mutex_lock(&queue_lock);
	proc = dequeue(&ready_queue);
	pthread_mutex_unlock(&queue_lock);

	return proc;
}

void put_proc(struct pcb_t *proc)
{
	pthread_mutex_lock(&queue_lock);
	enqueue(&run_queue, proc);
	pthread_mutex_unlock(&queue_lock);
}

void add_proc(struct pcb_t *proc)
{
	pthread_mutex_lock(&queue_lock);
	enqueue(&ready_queue, proc);
	pthread_mutex_unlock(&queue_lock);
}
#endif
