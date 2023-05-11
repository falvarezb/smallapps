/**
 * parallel version of the program to calculate value of pi by solving integral
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omp.h"

#define NUM_STEPS 5000000000L
#define NUM_THREADS 16 //default value (number of virtual cores on my machine)
static double pi = 0;

void thread_body(double step) {
    int num_threads = omp_get_num_threads();
    int id = omp_get_thread_num();   

    //round robin distribution of the thread's job
    double sum = 0;
    for(size_t i = id; i < NUM_STEPS; i += num_threads) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (double)(1.0 + x * x);
    }
#pragma omp critical
    pi += step*sum;
}

int main(int argc, char const *argv[]) {
    int requested_num_threads = NUM_THREADS;
    if(argc > 1) {
        requested_num_threads = atoi(argv[1]);
    }

    double step = 1.0 / NUM_STEPS;

    //CAUTION: the environment can choose to create fewer threads than requested
    omp_set_num_threads(requested_num_threads);    

    printf("NUM_STEPS=%ld\n", NUM_STEPS);
    printf("requested_num_threads=%d\n", requested_num_threads);    
    double start_time = omp_get_wtime();

#pragma omp parallel //fork-join construct
    {
        thread_body(step);
    }
    
    printf("time=%0.3f sec\n", omp_get_wtime() - start_time);
    printf("pi=%0.20f\n", pi);
}
