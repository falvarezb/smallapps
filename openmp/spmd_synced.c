#include "omp.h"
#include "pi.h"

static double pi = 0;

void thread_body(size_t num_steps, double step_size) {
    int num_threads = omp_get_num_threads();
    int id = omp_get_thread_num();

    double sum = 0;
    for(size_t i = id; i < num_steps; i += num_threads) {
        double x = (i + 0.5) * step_size;
        sum += 4.0 / (double)(1.0 + x * x);
    }
#pragma omp critical
    pi += step_size * sum;
}

void compute_pi(struct pi *args) {
    pi = 0;

#pragma omp parallel
    {
        thread_body(args->num_steps, args->step_size);
    }
    args->pi = pi;
}

int main(int argc, char const *argv[]) {
    struct pi args = parse_args(argc, argv);
    timeit(compute_pi, &args, args.num_repetitions);
    printf("pi=%0.20f\n", args.pi);
}
