#ifndef FAB_PI
#define FAB_PI

struct pi {
    int requested_num_threads;
    double pi;
};

void timeit(void (*func)(struct pi*), struct pi* args, int repeat);

#endif
