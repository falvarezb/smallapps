/**
 * parallel version of the program to calculate value of pi by solving integral
 */

#include <stdio.h>
#include "omp.h"

#define NUM_STEPS 5000000000L
#define NUM_THREADS 256

void thread_body(double *sum, double step) {
    int num_threads = omp_get_num_threads();
    int id = omp_get_thread_num();
    // printf(" thread(%d)\n", id);
    size_t init_num_steps = NUM_STEPS / num_threads * id;
    size_t end_num_steps = NUM_STEPS / num_threads * (id + 1);
    // in case NUM_THREADS does not divide NUM_STEPS, last thread takes the extra work
    if(id == NUM_THREADS - 1 && end_num_steps < NUM_STEPS) {
        end_num_steps = NUM_STEPS;
    }

    // printf(" thread(%d) - init_NUM_STEPS(%ld)\n", id, init_num_steps);
    // printf(" thread(%d) - end_NUM_STEPS(%ld)\n", id, end_num_steps);
    for(size_t i = init_num_steps; i < end_num_steps; i++) {
        double x = (i + 0.5) * step;
        sum[id] += 4.0 / (1.0 + x * x);
    }
}

int main(int argc, char const *argv[]) {
    double step = 1.0 / NUM_STEPS;
    omp_set_num_threads(NUM_THREADS);
    double sum[NUM_THREADS] = { 0 };
    printf("NUM_STEPS=%ld\n", NUM_STEPS);
    printf("NUM_THREADS=%d\n", NUM_THREADS);
    double start_time = omp_get_wtime();

#pragma omp parallel
    {
        thread_body(sum, step);
    }

    double total_sum = 0;
    for(int i = 0; i < NUM_THREADS; i++) {
        total_sum += sum[i];
    }

    double pi = step * total_sum;
    printf("time=%0.3f\n", omp_get_wtime() - start_time);
    printf("pi=%0.20f\n", pi);
}


