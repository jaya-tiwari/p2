#include "fcfs.h"
#include "common.h"

#include <stdlib.h>
#include <stdio.h>

// takes a sorted process array and a quanta and returns the next valid proc to run or NULL if none found
process * next_proc_fcfs(process* ptr, int Q, process* c){
    int i =0;
    if (c != NULL && c->remaining_runtime > 0) return c; // non-preemptive-ness
    for (i=0; i< NUMBER_OF_PROCS; i++){
        if (Q >= ptr[i].arrival_time && ptr[i].remaining_runtime > 0) return &ptr[i];
    }
    return NULL;
}

void fcfs(process *ptr)
{
	int i=0;
    int done_procs = 0;
    float idle_time = 0;
    float q_idle_time = 0;
    float response_time =0;
    float turnaround_time =0;
    float waiting_time = 0;
    process* c_proc = NULL;

    // sort on expected runtime
    qsort(ptr, NUMBER_OF_PROCS, sizeof(process), compare_arrival_times);

    printf("expected order:\n");
    print_procs(ptr);

    printf("\n Simulation starting...\n\n");
    while (1)
    {
    	q_idle_time = 0;    // var to measure idle time in this quanta

        if (done_procs == NUMBER_OF_PROCS) break; // if all procs are finished we break this loop

        c_proc = next_proc_fcfs(ptr, i, c_proc);

        // If c_proc is NULL there are no available procs to run so print QUANTA header with idle time and continue.
        if (c_proc == NULL){
            printf("| QUANTA %02d:  I D L E  |",i);
            idle_time += 1.0;
        }
        else
        {
            // First time this proc gets the cpu, we get response time from this
            if (c_proc->expected_runtime == c_proc->remaining_runtime){
                response_time += i - c_proc->arrival_time;
            }
	    // Break loop when we get a process that may have started before 100 and 
	    // has completed now
            if(c_proc->expected_runtime == c_proc->remaining_runtime && i>=QUANTA-1){
                done_procs++;
                break;
            }
            printf("| QUANTA %02d: ", i);  // QUANTA header

            // Now we found a process to run and we process by subtracting 1 from the remaining time
            c_proc->remaining_runtime -= 1;

            // Check to see if this process finished, if so, we record the cpu idle time in this quanta
            if (c_proc->remaining_runtime <= 0){
                done_procs++;
                q_idle_time = (-1) * c_proc->remaining_runtime;
                c_proc->remaining_runtime = 0;
                turnaround_time += i - c_proc->arrival_time;
		int current_turnaround = i - c_proc->arrival_time;
		waiting_time += current_turnaround - c_proc->expected_runtime;
            }

            // Print the process id and its remaining runtime
            printf("[p%02d](%2.1f)|", c_proc->pid, c_proc->remaining_runtime);

            // If the cpu was idle in this quanta print that and add it to the total idle time
            if (q_idle_time){
                printf("<< CPU IDLE for %2.1f QUANTA |", q_idle_time);
                idle_time += q_idle_time;
            }
        }
        i++;        
        printf("\n");
    }

        // Finish the rest of the QUANTA
    for (i=i; i< QUANTA; i++){
        printf("| QUANTA %02d: IDLE  |\n",i);
        idle_time += 1.0;
    }

    int proc_cnt = 0;
    int total_quanta = i;
    // Get the number of completed processes
    for (i=0; i< NUMBER_OF_PROCS; i++){
        if (ptr[i].remaining_runtime > 0){
            proc_cnt ++;
            printf("pid:%d remaining time: %f\n", ptr[i].pid, ptr[i].remaining_runtime);
        }
    }
    // Print un finished processes
    printf("\n%d processes left un-finished after %d QUANTA\n", proc_cnt, total_quanta);
    if (proc_cnt == 0){
        printf("All procs finished after %d QUANTA\n", total_quanta);
    }

    printf("\nAverage turn around time in this run: %3.1f quanta\n", turnaround_time / (NUMBER_OF_PROCS-proc_cnt));
    printf("Average response time in this run: %3.1f quanta\n", response_time / (NUMBER_OF_PROCS-proc_cnt));
    printf("Average waiting time in this run: %3.1f quanta\n", waiting_time / (NUMBER_OF_PROCS-proc_cnt));
    printf("Throughput: %3.3f\n", (float)(NUMBER_OF_PROCS-proc_cnt)/(float)total_quanta);

    printf("\ncpu was idle %3.1f Quanta \n", idle_time);

}
