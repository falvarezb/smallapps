/**
 * parallel version of the program to calculate value of pi by solving integral
 */

#include <stdio.h>
#include "omp.h"

#define NUM_STEPS 5000000000L
#define NUM_THREADS 1000
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
    double step = 1.0 / NUM_STEPS;

    //CAUTION: the environment can choose to create fewer threads than requested
    omp_set_num_threads(NUM_THREADS);
    int actual_num_threads;
    double sum[NUM_THREADS][PAD] = { {0} };
    printf("NUM_STEPS=%ld\n", NUM_STEPS);
    printf("NUM_THREADS=%d\n", NUM_THREADS);
    printf("PAD=%d\n", PAD);
    double start_time = omp_get_wtime();

#pragma omp parallel
    {
        thread_body(sum, step, &actual_num_threads);
    }

    double total_sum = 0;
    for(int i = 0; i < actual_num_threads; i++) {
        total_sum += sum[i][0];
    }

    double pi = step * total_sum;
    printf("actual num threads=%d\n", actual_num_threads);
    printf("time=%0.3f\n", omp_get_wtime() - start_time);
    printf("pi=%0.20f\n", pi);
}
