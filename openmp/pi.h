#ifndef FAB_PI
#define FAB_PI

#include <stdio.h>
#include <string.h>
#include <sys/time.h>

// this value can be overridden with the option -s
#define DEFAULT_NUM_STEPS 1e9 

// this value can be overridden with the option -t
#define DEFAULT_NUM_THREADS 16 

// this value can be overridden with the option -r
#define NUM_REPETITIONS 1


struct pi {
    int num_repetitions;
    size_t num_steps;
    int requested_num_threads;
    double step_size;
    double pi;
};

struct pi parse_args(int argc, char const *argv[]);

//void timeit(void (*func)(struct pi *), struct pi *args, int repeat);

#define timeit(func, args, repeat) \
    do {\
        double time_sum = 0;\
        for(int i = 0; i < repeat; i++) {\
            struct timeval start_tv, end_tv;\
            gettimeofday(&start_tv, NULL);\
            func(args);\
            gettimeofday(&end_tv, NULL);\
            double runtime = end_tv.tv_sec - start_tv.tv_sec + (end_tv.tv_usec - start_tv.tv_usec) * 1e-6;\
            printf("time=%0.3f sec\n", runtime);\
            time_sum += runtime;\
        }\
        printf("avg_time=%0.3f sec\n", time_sum / repeat);\
    } while(0)\

#endif
