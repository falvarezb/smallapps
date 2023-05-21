#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include "pi.h"


struct pi parse_args(int argc, char const *argv[]) {
    size_t num_steps = DEFAULT_NUM_STEPS;
    int requested_num_threads = DEFAULT_NUM_THREADS;
    if(argc > 1) {

        // PARSING NUM STEPS
        num_steps = strtoul(argv[1], NULL, 0);
        if(num_steps == ULONG_MAX) {
            if(errno == ERANGE) {
                printf("number of steps [%s] out of range\n", argv[1]);
                exit(EXIT_FAILURE);
            }
        } else if(num_steps == 0) { // 0 can represent conversion failure or a genuine value: in both cases, it is considered an error            
            printf("invalid number of steps [%s]\n", argv[1]);
            exit(EXIT_FAILURE);
        }

        // PARSING NUM THREADS
        if(argc > 2) {
            //TODO check for values out of range
            if((requested_num_threads = atoi(argv[2])) == 0) { // 0 can represent conversion failure or a genuine value: in both cases, it is considered an error            
                printf("invalid number of threads [%s]\n", argv[2]);
                exit(EXIT_FAILURE);
            }
        }
    }

    printf("NUM_STEPS=%ld\n", num_steps);
    printf("NUM_THREADS=%d\n", requested_num_threads);
    struct pi args = { .num_steps = num_steps, .requested_num_threads = requested_num_threads, .step_size = 1.0 / num_steps };
    return args;
}

void timeit(void (*func)(struct pi *), struct pi *args, int repeat) {
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

    printf("avg_time=%0.3f sec\n", time_sum / repeat);
}
