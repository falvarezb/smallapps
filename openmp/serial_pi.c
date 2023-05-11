/**
 * program to do a numerical integration to calculate the value of Pi
 */

#include <stdio.h>
#include <sys/time.h>

#define NUM_STEPS 5000000000L

int main(__attribute__ ((unused)) int argc, __attribute__ ((unused)) char const *argv[]) {
    printf("NUM_STEPS=%ld\n", NUM_STEPS);
    double sum = 0;
    double step = 1.0 / (double)NUM_STEPS;

    struct timeval start_tv, end_tv;
    gettimeofday(&start_tv, NULL);
    
    for(size_t i = 0; i < NUM_STEPS; i++) {
        double x = (i + 0.5) * step;
        sum += 4.0 / (1.0 + x * x);
    }
    double pi = step * sum;

    gettimeofday(&end_tv, NULL);    
    printf("time=%0.3f sec\n", end_tv.tv_sec - start_tv.tv_sec + (end_tv.tv_usec - start_tv.tv_usec)*1e-6);
    printf("pi=%0.20f\n", pi);
}


