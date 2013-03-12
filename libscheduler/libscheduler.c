/** @file libscheduler.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libscheduler.h"
#include "../libpriqueue/libpriqueue.h"


/**
  Stores information making up a job to be scheduled including any statistics.

  You may need to define some global variables or a struct to store your job queue elements. 
*/
typedef struct _job_t
{
    int job_number;
    int priority;
    int running_time;
    int is_running;
    int time;
    int core;
    int pre_jobn;
    int response_time; //when it first got some time in a core
} job_t;

/**
 * Just a collection of variables that would otherwise
 * have to be declared global individually.
 */
typedef struct _details_t {
  int *corelist; //1 = corelist[index] is in use; 0 = not in use
  scheme_t sch;
  priqueue_t *thing;
  int num_cores;
  int total_time; //stats
  int num_jobs; //stats
} details_t;

static details_t *ugh = NULL;

//The comparison function for FCFS.
int compare0(const void * a, const void * b)
{
  return ( ((job_t*)a)->time - ((job_t*)b)->time );
}

//The comparison function for PRI.
int compare3(const void * a, const void * b)
{
  int pridiff = ( ((job_t*)a)->priority - ((job_t*)b)->priority );

  return pridiff == 0 ? compare0(a, b) : pridiff;
}

//The comparison function for SJF.
int compare1(const void * a, const void * b)
{
  return ( ((job_t*)a)->running_time - ((job_t*)b)->running_time );
}

/**
 * Finds the entry with the maximum value of an attribute 
 * determined by the comparer function passed into it.
 */ /*
ministruct_t * max(priqueue_t *q, const void *a, const void *b, 
        int(*comparer)(const void *, const void *) {
    
    job_t *sofar = priqueue_peek(q); //the head

    int index;

    for(index = 0; index < priqueue_size(q); index++)
        ;
    for(index = 0; index < priqueue_size(q); index++)
        if( ((job_t *) priqueue_at(index))->is_running)
            if(comparer((void *) sofar, (void *) priqueue_at(index)) < 0)
                sofar = priqueue_at(thindex = index);
    
    ministruct_t roofus;
    roofus->maxest = sofar;
    roofus->thindex = thindex;

    return &roofus;           
}
*/
/** 
  Initalizes the scheduler.
 
  Assumptions:
    - You may assume this will be the first scheduler function called.
    - You may assume this function will be called only once.
    - You may assume that cores is a positive, non-zero number.
    - You may assume that scheme is a valid scheduling scheme.

  @param cores the number of cores that is available by the scheduler. 
  These cores will be known as core(id=0), core(id=1), ..., core(id=cores-1).
  @param scheme  the scheduling scheme that should be used. This value will be
   one of the six enum values of scheme_t
*/
void scheduler_start_up(int cores, scheme_t scheme)
{
  ugh = (details_t *) malloc(sizeof(details_t));
  ugh->thing = (priqueue_t *) malloc(sizeof(priqueue_t));
  ugh->corelist = (int *) malloc(sizeof(int) * (ugh->num_cores = cores));

  ugh->total_time = ugh->num_jobs = 0;

  /**
   * 0 = FCFS
   * 1 = SJF
   * 2 = PSJF
   * 3 = PRI
   * 4 = PPRI
   * 5 = RR
   */
  /**
   * Different schemes have different notions of priority. The
   * preemptive versions of schemes are in principle the same.
   */
  switch(scheme) {
    case FCFS: priqueue_init(ugh->thing, compare0);
            break;
    case SJF:
    case PSJF: priqueue_init(ugh->thing, compare1);
            break;
    case PRI:
    case PPRI: priqueue_init(ugh->thing, compare3);
            break;
    case RR: 
    default: break;
  }

  int i;
  for(i = 0; i < ugh->num_cores; i++)
    ugh->corelist[i] = 0; //Every core is initially idle.

  ugh->sch = scheme;
}


/**
  Called when a new job arrives.
 
  If multiple cores are idle, the job should be assigned to the core with 
  the lowest id.
  If the job arriving should be scheduled to run during the next
  time cycle, return the zero-based index of the core the job should be
  scheduled on. If another job is already running on the core specified,
  this will preempt the currently running job.
  Assumptions:
    - You may assume that every job wil have a unique arrival time.

  @param job_number a globally unique identification number of the job 
  arriving.
  @param time the current time of the simulator.
  @param running_time the total number of time units this job will run 
        before it will be finished.
  @param priority the priority of the job. (The lower the value, the higher 
        the priority.)
  @return index of core job should be scheduled on
  @return -1 if no scheduling changes should be made. 
 
 */

int scheduler_new_job(int job_number, int time, int running_time, int priority)
{
  ugh->num_jobs++;

  job_t *job = (job_t *) malloc(sizeof(job_t));
  job->job_number = job_number;
  job->priority = priority;
  job->running_time = running_time;
  job->time = time; 
  job->is_running = 0; //not being performed by default
  //job->pre_jobn = -1; //it hasn't preempted anything yet

  priqueue_offer(ugh->thing, job); 

  //Look for an idle core
  int i;
  for(i=0; i<ugh->num_cores; i++)
    if(!ugh->corelist[i]) {
      ugh->corelist[i] = 1; //The core is now in use
      job->is_running = 1; //is being performed
      return job->core = i; //The id of the core to which job has been assigned.
    }

    if(ugh->sch/3 + ugh->sch%3 < 2) return -1; //returns if nonpreemptive
  
	/**
     * If there be no idle cores, use the power of preemption.
     * (Nonpreemptive jobs return -1 at the end of this function.)
     */

    //rt = REMAINING time, not running time; pt = priority
    int index, rt, lrt = -1, thindex = -1, pt, mpt = -1;
    job_t *curr;

    /**
     * PREEMPTIVE SHORTEST JOB FIRST:
     * This will preempt the job that is running with the largest
     * remaining time, if that time be greteater than the running
     * time for this job. It will additionally reschedule the 
     * preempted job with updated running time.
     * 
     * One must remember that these schemes are like the nonpreemptive
     * ones insomuch as there be no conflicts with jobs that are running.
     */
    if(ugh->sch == PSJF) {
        /**
         * Finding the largest remaining time of a running job and the index of the job
         * to which it belongs.
         */
	    for(index = 0; index < priqueue_size(ugh->thing); index++)
            if( ((job_t *) priqueue_at(ugh->thing, index))->is_running) {
                curr = (job_t *) priqueue_at(ugh->thing, index);
			
				if(lrt < (rt = curr->running_time - time + curr->time)) {
                    lrt = rt;
                    thindex = index;
                }
		    }
    }
    else if(ugh->sch == PPRI) {
        /**
         * Finding the largest priority number or lowest priority of a running job 
         * and the index of the job to which it belongs.
         */
        for(index = 0; index < priqueue_size(ugh->thing); index++)
            if( ((job_t *) priqueue_at(ugh->thing, index))->is_running) {
                curr = (job_t *) priqueue_at(ugh->thing, index);
            
                if(mpt < (pt = curr->priority)) {
                    lrt = curr->running_time - time + curr->time;
                    mpt = pt;
                    thindex = index;
                }
            }
    }
        

    /**
     * thindex is the index of the job with the largest remaining time/priority number
     * that is currently running. It is this job that the new job must
     * attempt to preempt.
     */

    curr = priqueue_at(ugh->thing, thindex);

    if( (ugh->sch == PSJF && job->running_time < lrt) ||
        (ugh->sch == PPRI && job->priority < mpt) ) {
        job->core = curr->core; //assign job to run on the preempted job's core
        //job->pre_jobn = curr->job_number; //remember which job the new job preempted
        priqueue_remove_at(ugh->thing, thindex); //remove curr from the queue in order to fix its stats
        curr->running_time = lrt; //change its running time to be the remaining time
        curr->is_running = 0; //remember that it is no longer running
        job->is_running = 1;
        curr->core = -1; //it is not running on any cores
        priqueue_offer(ugh->thing, curr); //put it back into the priority queue
        return job->core; //return the core on which job is to be run
    }
    
    return -1; //job needs to wait in line like everyone else

}


/**
  Called when a job has completed execution.
 
  The core_id, job_number and time parameters are provided for convenience. 
  You may be able to calculate the values with your own data structure.
  If any job should be scheduled to run on the core free'd up by the
  finished job, return the job_number of the job that should be scheduled to
  run on core core_id.
 
  @param core_id the zero-based index of the core where the job was located.
  @param job_number a globally unique identification number of the job.
  @param time the current time of the simulator.
  @return job_number of the job that should be scheduled to run on core core_id
  @return -1 if core should remain idle.
 */
int scheduler_job_finished(int core_id, int job_number, int time)
{
  /**
   * The current assumption is that this function works similarly for
   * every scheme. Preemption occurs when a new job arrives.
   */

  job_t *done; //the finished job
  job_t *next = NULL;
  
  //The core is now idle
  ugh->corelist[core_id] = 0;
  int index;
  for(index = 0; index < priqueue_size(ugh->thing); index++) 
    if( ((job_t *) priqueue_at(ugh->thing, index))->job_number == job_number) {
    	done = (job_t *) priqueue_at(ugh->thing, index);
		break;
	}
            
  priqueue_remove_at(ugh->thing, index);
  
  ugh->total_time += done->time; //total time updated only when a job is done
 
  free(done);
            
  for(index = 0; index < priqueue_size(ugh->thing); index++)
    if( !((job_t *) priqueue_at(ugh->thing, index))->is_running) {
        next = (job_t *) priqueue_at(ugh->thing, index);
        break;
    }
          
    if(next) {
	   next->core = core_id;
	   ugh->corelist[core_id] = 1;
	   next->is_running = 1;
       return next->job_number;
    }
   
    //The core should remain idle, such as for instance when the queue is empty.
	
	return -1;
}


/**
  When the scheme is set to RR, called when the quantum timer has expired
  on a core.
 
  If any job should be scheduled to run on the core free'd up by
  the quantum expiration, return the job_number of the job that should be
  scheduled to run on core core_id.

  @param core_id the zero-based index of the core where the quantum has expired.
  @param time the current time of the simulator. 
  @return job_number of the job that should be scheduled on core cord_id
  @return -1 if core should remain idle
 */
int scheduler_quantum_expired(int core_id, int time)
{
	return -1;
}


/**
  Returns the average waiting time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average waiting time of all jobs scheduled.
 */
float scheduler_average_waiting_time()
{
	return ugh->total_time / ( (float) ugh->num_jobs);
}


/**
  Returns the average turnaround time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average turnaround time of all jobs scheduled.
 */
float scheduler_average_turnaround_time()
{
	return 0.0;
}


/**
  Returns the average response time of all jobs scheduled by your scheduler.

  Assumptions:
    - This function will only be called after all scheduling is complete (all jobs that have arrived will have finished and no new jobs will arrive).
  @return the average response time of all jobs scheduled.
 */
float scheduler_average_response_time()
{
	return 0.0;
}


/**
  Free any memory associated with your scheduler.
 
  Assumptions:
    - This function will be the last function called in your library.
*/
void scheduler_clean_up()
{

}


/**
  This function may print out any debugging information you choose. This
  function will be called by the simulator after every call the simulator
  makes to your scheduler.
  In our provided output, we have implemented this function to list the jobs in the order they are to be scheduled. Furthermore, we have also listed the current state of the job (either running on a given core or idle). For example, if we have a non-preemptive algorithm and job(id=4) has began running, job(id=2) arrives with a higher priority, and job(id=1) arrives with a lower priority, the output in our sample output will be:

    2(-1) 4(0) 1(-1)  
  
  This function is not required and will not be graded. You may leave it
  blank if you do not find it useful.
 */
void scheduler_show_queue()
{
	int i;
	for(i=0; i<priqueue_size(ugh->thing); i++)
		printf("%d(%d) ", ( (job_t *) priqueue_at(ugh->thing, i))->job_number, ( (job_t *) priqueue_at(ugh->thing, i))->core);

	printf("\n");
}
