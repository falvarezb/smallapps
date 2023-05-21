/**
 * parallel version of the program to calculate value of pi by solving integral
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omp.h"
#include "pi.h"

#define NUM_STEPS 5000000000L
#define NUM_THREADS 16 //default value (number of virtual cores on my machine)

void thread_body(double *sum, double step, int *actual_num_threads) {
    int num_threads = omp_get_num_threads();
    int id = omp_get_thread_num();

    //we choose master thread to update actual_num_threads
#pragma omp master    
    *actual_num_threads = num_threads;

    for(size_t i = id; i < NUM_STEPS; i += num_threads) {
        double x = (i + 0.5) * step;
        sum[id] += 4.0 / (1.0 + x * x);
    }
}

void compute_pi(struct pi* args) {
    double step = 1.0 / NUM_STEPS;    
    int actual_num_threads;

    // If you promote scalars to an array to support creation of an SPMD program, 
    // the array elements are contiguous in memory and hence share cache lines
    double sum[args->requested_num_threads];
    memset(sum, 0, args->requested_num_threads * sizeof(double));        

#pragma omp parallel //fork-join construct
    {
        thread_body(sum, step, &actual_num_threads);
    }

    printf("actual num threads=%d\n", actual_num_threads);
    double total_sum = 0;
    for(int i = 0; i < actual_num_threads; i++) {
        total_sum += sum[i];
    }

    args->pi = step * total_sum;    
}

int main(int argc, char const *argv[]) {

    printf("NUM_STEPS=%ld\n", NUM_STEPS);
    int requested_num_threads = NUM_THREADS;
    if(argc > 1) {
        requested_num_threads = atoi(argv[1]);
    }  
    printf("requested_num_threads=%d\n", requested_num_threads);
    //CAUTION: the environment can choose to create fewer threads than requested
    omp_set_num_threads(requested_num_threads);
     
    struct pi args = {.requested_num_threads = requested_num_threads};
    timeit(compute_pi,&args,2);    
    printf("pi=%0.20f\n", args.pi);
}
