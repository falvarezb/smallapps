/**
 * program to calculate value of pi by solving integral
 */

#include <stdio.h>

#define NUM_STEPS 3000000000L

int main(int argc, char const *argv[]) {
    printf("NUM_STEPS=%ld\n", NUM_STEPS);
    double sum = 0;
    double step = 1.0 / (double)NUM_STEPS;

    for(size_t i = 0; i < NUM_STEPS; i++) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }
    double pi = step * sum;
    printf("pi=%0.20f\n", pi);
}


