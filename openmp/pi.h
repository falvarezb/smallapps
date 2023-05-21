#ifndef FAB_PI
#define FAB_PI

#include <stdio.h>
#include <string.h>

// this value can be overridden by passing an argument to the program
#define DEFAULT_NUM_STEPS 5000000000UL 

// = number of logical processors in my machine
// this value can be overridden by passing an argument to the program
#define DEFAULT_NUM_THREADS 16 

// this value can be overridden by passing an argument to the program
#define NUM_REPETITIONS 1


struct pi {
    int num_repetitions;
    size_t num_steps;
    int requested_num_threads;
    double step_size;
    double pi;
};

struct pi parse_args(int argc, char const *argv[]);

void timeit(void (*func)(struct pi *), struct pi *args, int repeat);

#endif
