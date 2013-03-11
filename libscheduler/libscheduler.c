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
  return ( ((job_t*)a)->priority - ((job_t*)b)->priority );
}

//The comparison function for SJF.
int compare1(const void * a, const void * b)
{
  return ( ((job_t*)a)->running_time - ((job_t*)b)->running_time );
}

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

  ugh->total_time = ugh->num_jobs = ugh->secret = 0;

  /**
   * 0 = FCFS
   * 1 = SJF
   * 2 = PSJF
   * 3 = PRI
   * 4 = PPRI
   * 5 = RR
   */
  //Different schemes have different notions of priority.
  switch(scheme) {
    case 0: priqueue_init(ugh->thing, compare0);
            break;
    case 1: priqueue_init(ugh->thing, compare1);
            break;
    case 2: priqueue_init(ugh->thing, compare1);
            break;
    case 3: priqueue_init(ugh->thing, compare3);
            break;
    case 4: priqueue_init(ugh->thing, compare3);
            break;
    case 5: 
    default: break;
  }

  int i;
  for(i = 0; i < cores; i++)
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

  priqueue_offer(ugh->thing, job); 

  //Look for an idle core
  int i;
  for(i=0; i<ugh->num_cores; i++)
    if(!ugh->corelist[i]) {
      ugh->corelist[i] = 1; //The core is now in use
      job->is_running = 1; //is being performed
      job->core = i;
      return i; //The id of the core to which job has been assigned.
    }

//All cores are busy. Return, if nonpreemptive.
if(ugh->sch%3 + ugh->sch/3 < 2) //i.e. if scheme is 0, 1, or 3
    return -1; 

//If there are no idle cores, use the power of preemption.
/*SOMETHING*/
  

	return -1;
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
  
  ugh->corelist[core_id] = 0; //The core is now idle

  int index;
  for(index = 0; index < priqueue_size(ugh->thing); index++) 
    if( (done = (job_t *) priqueue_at(ugh->thing, index))->job_number == job_number)
      break;
            
  priqueue_remove_at(ugh->thing, index);
  
  ugh->total_time += done->time; //total time updated only when a job is done
  
  free(done);
            
  for(index = 0; index < priqueue_size(ugh->thing); index++)
    if( !(next = (job_t *) priqueue_at(ugh->thing, index))->is_running)
      break;
          
  if(next) 
    return next->job_number;
   
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
	return ugh->total_time/ ( (float) ugh->num_jobs);
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

}
