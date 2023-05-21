#include "omp.h"
#include "pi.h"


void thread_body(double *sum, size_t num_steps, double step_size, int *actual_num_threads) {
    int num_threads = omp_get_num_threads();
    int id = omp_get_thread_num();

    //we choose master thread to update actual_num_threads
#pragma omp master    
    *actual_num_threads = num_threads;

    for(size_t i = id; i < num_steps; i += num_threads) {
        double x = (i + 0.5) * step_size;
        sum[id] += 4.0 / (1.0 + x * x);
    }
}

void compute_pi(struct pi *args) {
    
    int actual_num_threads;

    // WARNING: the array elements are contiguous in memory and hence share cache lines
    double sum[args->requested_num_threads];
    memset(sum, 0, args->requested_num_threads * sizeof(double));

#pragma omp parallel
    {
        thread_body(sum, args->num_steps, args->step_size, &actual_num_threads);
    }

    printf("actual num threads=%d\n", actual_num_threads);
    double total_sum = 0;
    for(int i = 0; i < actual_num_threads; i++) {
        total_sum += sum[i];
    }

    args->pi = args->step_size * total_sum;
}

int main(int argc, char const *argv[]) {
    struct pi args = parse_args(argc, argv);
    
    //CAUTION: the environment can choose to create fewer threads than requested
    // actual num threads to be checked in the parallel region    
    omp_set_num_threads(args.requested_num_threads);
    
    timeit(compute_pi, &args, args.num_repetitions);
    printf("pi=%0.20f\n", args.pi);
}
