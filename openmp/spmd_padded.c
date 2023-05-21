#include "omp.h"
#include "pi.h"

#define PAD 8 // 8 doubles  = 64 bytes (assuming 64-byte L1 cache line size)

void thread_body(double sum[][PAD], size_t num_steps, double step, int *actual_num_threads) {
    int num_threads = omp_get_num_threads();
    int id = omp_get_thread_num();

    //we choose master thread to update actual_num_threads
#pragma omp master    
    *actual_num_threads = num_threads;    
    
    for(size_t i = id; i < num_steps; i += num_threads) {
        double x = (i + 0.5) * step;
        sum[id][0] += 4.0 / (double)(1.0 + x * x);
    }
}

void compute_pi(struct pi* args) {    
    int actual_num_threads;

    // Padded arrays so elements are on distinct cache lines 
    // (as long as padding is bigger than cache line size)    
    double sum[args->requested_num_threads][PAD];
    memset(sum, 0, args->requested_num_threads * PAD * sizeof(double));            

#pragma omp parallel
    {
        thread_body(sum, args->num_steps, args->step_size, &actual_num_threads);
    }

    printf("actual num threads=%d\n", actual_num_threads);
    double total_sum = 0;
    for(int i = 0; i < actual_num_threads; i++) {
        total_sum += sum[i][0];
    }

    args->pi = args->step_size * total_sum;    
}

int main(int argc, char const *argv[]) {
    struct pi args = parse_args(argc, argv);
    printf("PAD=%d\n", PAD);
    
    //CAUTION: the environment can choose to create fewer threads than requested
    // actual num threads to be checked in the parallel region    
    omp_set_num_threads(args.requested_num_threads);
    
    timeit(compute_pi,&args,2);    
    printf("pi=%0.20f\n", args.pi);
}
