#include <stdio.h>
#include <sys/time.h>
#include "pi.h"

double timeit(double (*func)(void), int repeat) {        
    double retval, time_sum = 0;

    for(int i = 0; i < repeat; i++) {
        struct timeval start_tv, end_tv;
        gettimeofday(&start_tv, NULL);
        retval = func();
        gettimeofday(&end_tv, NULL);
        double runtime = end_tv.tv_sec - start_tv.tv_sec + (end_tv.tv_usec - start_tv.tv_usec) * 1e-6;
        printf("time=%0.3f sec\n", runtime);
        time_sum += runtime;
    }    
    
    printf("avg_time=%0.3f sec\n", time_sum/repeat);
    return retval;
}

double timeit2(double (*func)(int), int requested_num_threads, int repeat) {
    double retval, time_sum = 0;

    for(int i = 0; i < repeat; i++) {
        struct timeval start_tv, end_tv;
        gettimeofday(&start_tv, NULL);
        retval = func(requested_num_threads);
        gettimeofday(&end_tv, NULL);
        double runtime = end_tv.tv_sec - start_tv.tv_sec + (end_tv.tv_usec - start_tv.tv_usec) * 1e-6;
        printf("time=%0.3f sec\n", runtime);
        time_sum += runtime;
    }    
    
    printf("avg_time=%0.3f sec\n", time_sum/repeat);
    return retval;
}

void timeit3(void (*func)(void), int repeat) {
    double time_sum = 0;

    for(int i = 0; i < repeat; i++) {
        struct timeval start_tv, end_tv;
        gettimeofday(&start_tv, NULL);
        func();
        gettimeofday(&end_tv, NULL);
        double runtime = end_tv.tv_sec - start_tv.tv_sec + (end_tv.tv_usec - start_tv.tv_usec) * 1e-6;
        printf("time=%0.3f sec\n", runtime);
        time_sum += runtime;
    }    
    
    printf("avg_time=%0.3f sec\n", time_sum/repeat);    
}
