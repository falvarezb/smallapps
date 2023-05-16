/**
 * parallel version of the program to calculate value of pi by solving integral
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "omp.h"

#define NUM_STEPS 5000000000L
#define NUM_STEPS_THRESHOLD 100000 //threshold to create new tasks

double task(size_t start, size_t end, double step) {

    double sum = 0;
    size_t num_steps = end - start;
    if(num_steps < NUM_STEPS_THRESHOLD) {        
        for(size_t i = start; i < end; i++) {
            double x = (i + 0.5) * step;
            sum += 4.0 / (1.0 + x * x);
        }        
    }
    else {
        double left_segment_sum, right_segment_sum;
        size_t half_step = (start + end)/2;
        #pragma omp task shared(left_segment_sum)
        left_segment_sum = task(start, half_step, step);
        #pragma omp task shared(right_segment_sum)
        right_segment_sum = task(half_step, end, step);
        #pragma omp taskwait
        sum = left_segment_sum + right_segment_sum;
    }
    return sum;
}

int main(int argc, char const *argv[]) {
    const double step = 1.0 / NUM_STEPS;

    printf("NUM_STEPS=%ld\n", NUM_STEPS);    
    struct timeval start_tv, end_tv;
    gettimeofday(&start_tv, NULL);

    double sum;
#pragma omp parallel shared(sum,step)
    {
        #pragma omp single
        sum = task(0, NUM_STEPS, step);
    }
    
    double pi = step * sum;
    gettimeofday(&end_tv, NULL);    
    printf("time=%0.3f sec\n", end_tv.tv_sec - start_tv.tv_sec + (end_tv.tv_usec - start_tv.tv_usec)*1e-6);
    printf("pi=%0.20f\n", pi);
}
