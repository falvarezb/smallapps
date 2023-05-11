/**
 * parallel version of the program to calculate value of pi by solving integral
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "omp.h"

#define NUM_STEPS 5000000000L
#define NUM_THREADS 16 //default value (number of virtual cores on my machine)
#define PAD 8 // 8 doubles  = 64 bytes (assuming 64-byte L1 cache line size)

void thread_body(double sum[][PAD], double step, int *actual_num_threads) {
    int num_threads = omp_get_num_threads();
    int id = omp_get_thread_num();

    //we choose a random thread to update actual_num_threads
    if(id == 0) {
        *actual_num_threads = num_threads;
    }

    // printf(" thread(%d)\n", id);

    //round robin distribution of the thread's job
    for(size_t i = id; i < NUM_STEPS; i += num_threads) {
        double x = (i + 0.5) * step;
        sum[id][0] += 4.0 / (double)(1.0 + x * x);
    }
}

int main(int argc, char const *argv[]) {
    int requested_num_threads = NUM_THREADS;
    if(argc > 1) {
        requested_num_threads = atoi(argv[1]);
    }
    
    double step = 1.0 / NUM_STEPS;

    //CAUTION: the environment can choose to create fewer threads than requested
    omp_set_num_threads(requested_num_threads);
    int actual_num_threads;

    // Padded arrays so elements are on distinct cache lines 
    // (as long as padding is bigger than cache line size)    
    double sum[requested_num_threads][PAD];
    memset(sum, 0, requested_num_threads * PAD * sizeof(double));
    printf("NUM_STEPS=%ld\n", NUM_STEPS);
    printf("requested_num_threads=%d\n", requested_num_threads);
    printf("PAD=%d\n", PAD);
    double start_time = omp_get_wtime();

#pragma omp parallel //fork-join construct
    {
        thread_body(sum, step, &actual_num_threads);
    }

    double total_sum = 0;
    for(int i = 0; i < actual_num_threads; i++) {
        total_sum += sum[i][0];
    }

    double pi = step * total_sum;
    printf("actual num threads=%d\n", actual_num_threads);
    printf("time=%0.3f sec\n", omp_get_wtime() - start_time);
    printf("pi=%0.20f\n", pi);
}
