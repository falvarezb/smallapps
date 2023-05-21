#ifndef FAB_PI
#define FAB_PI

double timeit(double (*func)(void), int repeat);
double timeit2(double (*func)(int), int requested_num_threads, int repeat);
void timeit3(void (*func)(void), int repeat);

#endif
