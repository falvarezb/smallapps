/**
 * program to calculate value of pi by solving integral
 */

#include <stdio.h>
#include "omp.h"

#define NUM_STEPS 5000000000L

int main(int argc, char const *argv[]) {
    printf("NUM_STEPS=%ld\n", NUM_STEPS);
    double sum = 0;
    double step = 1.0 / (double)NUM_STEPS;
    double start_time = omp_get_wtime();
    omp_set_num_threads(4);

#pragma omp parallel for reduction (+:sum) schedule(guided)   
    for(size_t i = 0; i < NUM_STEPS; i++) {
        if(i == 0) {
            int num_threads = omp_get_num_threads();
            printf("num threads=%d\n", num_threads);
        }
        if(i % 1000000000 == 0) {                        
            omp_sched_t sched_kind;
            int chunk_size;
            omp_get_schedule(&sched_kind, &chunk_size);
            printf("Thread %d: schedule=%d, chunk_size=%d\n", omp_get_thread_num(), sched_kind, chunk_size);            
        }

        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    double pi = step * sum;
    printf("time=%0.3f\n", omp_get_wtime() - start_time);
    printf("pi=%0.20f\n", pi);
}


