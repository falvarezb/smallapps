#ifndef FAB_PI
#define FAB_PI

#include <stdio.h>
#include <string.h>

// this value can be overwritten by passing an argument to the program
#define DEFAULT_NUM_STEPS 5000000000UL 

// = number of logical processors in my machine
// this value can be overwritten by passing an argument to the program
#define DEFAULT_NUM_THREADS 16  


struct pi {
    size_t num_steps;
    int requested_num_threads;
    double step_size;
    double pi;
};

struct pi parse_args(int argc, char const *argv[]);

void timeit(void (*func)(struct pi *), struct pi *args, int repeat);

#endif
