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
    pi += step * sum;
}

void compute_pi(struct pi* args) {
    pi = 0;
    double step = 1.0 / NUM_STEPS;

#pragma omp parallel //fork-join construct
    {
        thread_body(step);
    }
    args->pi = pi;
}

int main(int argc, char const *argv[]) {
    printf("NUM_STEPS=%ld\n", NUM_STEPS);
    int requested_num_threads = NUM_THREADS;
    if(argc > 1) {
        requested_num_threads = atoi(argv[1]);
    }
    printf("requested_num_threads=%d\n", requested_num_threads);
    struct pi args;
    timeit(compute_pi,&args,2);    
    printf("pi=%0.20f\n", args.pi);
}
