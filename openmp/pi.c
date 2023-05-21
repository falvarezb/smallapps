#include <stdio.h>
#include "pi.h"

#define NUM_STEPS 5000000000L

double compute_pi(void) {
    double sum = 0;
    const double step = 1.0 / (double)NUM_STEPS;    
#pragma omp parallel for reduction (+:sum)   
    for(size_t i = 0; i < NUM_STEPS; i++) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }

    double pi = step * sum;
    return pi;
}

int main(__unused int argc, __unused char const *argv[]) {
    printf("NUM_STEPS=%ld\n", NUM_STEPS);
    double pi = timeit(compute_pi, 2);
    printf("pi=%0.20f\n", pi);
}


