#include "pi.h"

#define NUM_STEPS_THRESHOLD 100000 //threshold to create new tasks

double task(size_t start, size_t end, double step, size_t steps_threshold) {

    double sum = 0;
    size_t num_steps = end - start;
    if(num_steps < steps_threshold) {
        for(size_t i = start; i < end; i++) {
            double x = (i + 0.5) * step;
            sum += 4.0 / (1.0 + x * x);
        }
    } else {
        double left_segment_sum, right_segment_sum;
        size_t half_step = (start + end) / 2;
#pragma omp task shared(left_segment_sum)
        left_segment_sum = task(start, half_step, step, steps_threshold);
#pragma omp task shared(right_segment_sum)
        right_segment_sum = task(half_step, end, step, steps_threshold);
#pragma omp taskwait
        sum = left_segment_sum + right_segment_sum;
    }
    return sum;
}

void compute_pi(struct pi *args) {
    double sum;
#pragma omp parallel
    {
#pragma omp single
        sum = task(0, args->num_steps, args->step_size, NUM_STEPS_THRESHOLD);
    }

    args->pi = args->step_size * sum;
}

int main(int argc, char const *argv[]) {
    struct pi args = parse_args(argc, argv);
    timeit(compute_pi, &args, args.num_repetitions);
    printf("pi=%0.20f\n", args.pi);
}
