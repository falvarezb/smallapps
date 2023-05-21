#include "pi.h"

void compute_pi(struct pi *args) {
    double sum = 0;
#pragma omp parallel for reduction (+:sum)   
    for(size_t i = 0; i < args->num_steps; i++) {
        double x = (i + 0.5) * args->step_size;
        sum += 4.0 / (1.0 + x * x);
    }

    args->pi = args->step_size * sum;
}

int main(__unused int argc, __unused char const *argv[]) {
    struct pi args = parse_args(argc, argv);
    timeit(compute_pi, &args, args.num_repetitions);
    printf("pi=%0.20f\n", args.pi);
}


