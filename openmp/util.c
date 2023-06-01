#include <limits.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include "pi.h"

int parse_int(char *arg, int opt) {
    int value = atoi(arg);
    //TODO check for values out of range (values out of range have undefined behaviour)
    if(value == 0) {
        // here 0 can represent conversion failure or a genuine value: in both cases, it is considered an error            
        printf("invalid argument [%s] in option -%c \n", optarg, opt);
        exit(EXIT_FAILURE);
    }
    return value;
}

struct pi parse_args(int argc, char const *argv[]) {
    size_t num_steps = DEFAULT_NUM_STEPS;
    int requested_num_threads = DEFAULT_NUM_THREADS, num_repetitions = NUM_REPETITIONS;

    int opt;
    while((opt = getopt(argc, (char *const *)argv, "r:s:t:")) != -1) {
        switch(opt) {
            case 'r':
                num_repetitions = parse_int(optarg, opt);
                break;
            case 't':
                requested_num_threads = parse_int(optarg, opt);
                break;
            case 's':
                num_steps = strtoul(optarg, NULL, 0);
                if(num_steps == ULONG_MAX && errno == ERANGE) {

                    printf("number of steps [%s] out of range\n", optarg);
                    exit(EXIT_FAILURE);

                } else if(num_steps == 0) { // 0 can represent conversion failure or a genuine value: in both cases, it is considered an error            
                    printf("invalid number of steps [%s]\n", optarg);
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                fprintf(stderr, "Usage: %s [-r num_repetitions] [-s num_steps] [-t num_threads]\n", argv[0]);
                exit(EXIT_FAILURE);
        }

    }

    printf("NUM_REPETITIONS=%d\n", num_repetitions);
    printf("NUM_STEPS=%ld\n", num_steps);
    printf("NUM_THREADS=%d\n", requested_num_threads);
    struct pi args = { .num_repetitions = num_repetitions, .num_steps = num_steps, .requested_num_threads = requested_num_threads, .step_size = 1.0 / num_steps };
    return args;
}





