#include <stdio.h>
#include <sys/time.h>
#include "pi.h"

void timeit(void (*func)(struct pi*), struct pi* args, int repeat) {        
    double time_sum = 0;

    for(int i = 0; i < repeat; i++) {
        struct timeval start_tv, end_tv;
        gettimeofday(&start_tv, NULL);
        func(args);
        gettimeofday(&end_tv, NULL);
        double runtime = end_tv.tv_sec - start_tv.tv_sec + (end_tv.tv_usec - start_tv.tv_usec) * 1e-6;
        printf("time=%0.3f sec\n", runtime);
        time_sum += runtime;
    }    
    
    printf("avg_time=%0.3f sec\n", time_sum/repeat);    
}
