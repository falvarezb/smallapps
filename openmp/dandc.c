/**
 * parallel version of the program to calculate value of pi by solving integral
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pi.h"

#define NUM_STEPS 5000000000L
#define NUM_STEPS_THRESHOLD 100000 //threshold to create new tasks

double task(size_t start, size_t end, double step, size_t steps_threshold) {

    double sum = 0;
    size_t num_steps = end - start;
    if(num_steps < steps_threshold) {        
        for(size_t i = start; i < end; i++) {
            double x = (i + 0.5) * step;
            sum += 4.0 / (1.0 + x * x);
        }        
    }
    else {
        double left_segment_sum, right_segment_sum;
        size_t half_step = (start + end)/2;
        #pragma omp task shared(left_segment_sum)
        left_segment_sum = task(start, half_step, step, steps_threshold);
        #pragma omp task shared(right_segment_sum)
        right_segment_sum = task(half_step, end, step, steps_threshold);
        #pragma omp taskwait
        sum = left_segment_sum + right_segment_sum;
    }
    return sum;
}

double compute_pi(void) {
    const double step = 1.0 / NUM_STEPS;
    double sum;
#pragma omp parallel shared(sum,step)
    {
        #pragma omp single
        sum = task(0, NUM_STEPS, step, NUM_STEPS_THRESHOLD);
    }
    
    double pi = step * sum;
    return pi;
}

int main(int argc, char const *argv[]) {
    printf("NUM_STEPS=%ld\n", NUM_STEPS);    
    double pi = timeit(compute_pi,2);
    printf("pi=%0.20f\n", pi);
}
