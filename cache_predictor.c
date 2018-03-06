#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

// For CPU affinity
#include <sched.h>

// For allocation
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

// The size of the array, it depends on the memory we have
// If our memory is too small, while this hyperparameter is too large, it can
// cause segmentation fault
#define MAX_SIZE 99999999

// Number of trials to access the memory
#define TRIALS 1000000

// Create storage
int arr[MAX_SIZE];

// Operation mode
int mode;

void
fill_array(int size)
{
  int i = 0;
  for (; i < size; i++) {
    arr[i] = 0;
  }
}

void
generate_cache_performance(int number_of_pages)
{
  struct timespec start_t, end_t;
  double start, end, delta;

  int blocks = PAGE_SIZE / sizeof(int);
  // Floating value to avoid optimization from compiler
  // We also use this to calculate the average time to this experiment
  double n = 0.0;
  double total_time = 0;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_t);

  // Do the experiment here
  int i = 0;

  // We repeat many many times because we want to avoid the test being over-
  // shadowed by the clock function call. If we make the loops larger, then
  // the noise by the clock function call, become less significant
  for (; i < TRIALS; i++) {
    int j = 0;
    for (; j < number_of_pages * blocks; j += blocks) {
      arr[j] += 1;
      n += 1.0;
    }
  }

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_t);
  start = 1000000000 * start_t.tv_sec + start_t.tv_nsec;
  end = 1000000000 * end_t.tv_sec + end_t.tv_nsec;
  delta = end - start;
  total_time += delta;

  if (mode == 1 || mode == 3) {
    printf("%u, %lf\n", number_of_pages, total_time / n);
  }
}

double
get_element_access_time(int index)
{
  struct timespec start_t, end_t;
  double start, end, delta;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_t);
  arr[index] += 1;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_t);

  start = 1000000000 * start_t.tv_sec + start_t.tv_nsec;
  end = 1000000000 * end_t.tv_sec + end_t.tv_nsec;
  delta = end - start;

  if (mode == 2 || mode == 3) {
    printf("%d, %lf\n", index, delta);
  }

  return delta;
}

void
run_cache_size_test()
{
  // Initiate array
  fill_array(10485760);

  int i = 2;
  while (i <= 4096) {
    generate_cache_performance(i);
    i = i * 2;
  }
}

// Cache line for L1
void
run_cache_line_test()
{
  int i = 2;
  while (i <= 10000) {
    get_element_access_time(i);

    // Added here just to proof the cache line
    if (i == 1024) {
      int j = 1;
      while (j <= 17) {
        get_element_access_time(i + j);
        j++;
      }
    }
    i = i * 2;
  }
}

void
run_associativity_test()
{
  int j = 0;
  while (j < 20) {
    int i = 2;
    while (i <= 256) {
      generate_cache_performance(i);
      i = i * 2;
    }
    j++;
  }
}

int
main(int argc, char* argv[])
{
  char* p;

  // Mode so we don't have to run all
  if (argc < 2) {
    printf("Usage: ./cache_predictor operation_number\n"
           "1: Generate cache size data\n"
           "2: Generate cache line data\n"
           "3: Generate associativity data\n");
    return 0;
  } else {
    long conv = strtol(argv[1], &p, 10);
    mode = conv;
  }

  // ONLY FOR LINUX, DOES NOT WORK ON MAC OR WINDOWS
  // For multi processor architecture, we want this process to be run in a
  // processor only
  cpu_set_t set;
  CPU_ZERO(&set);

  // Add cpu 0 to set
  CPU_SET(0, &set);

  // If fail to se affinity
  if (sched_setaffinity(getpid(), sizeof(set), &set) == -1) {
    exit(1);
  }

  switch (mode) {
    case 1:
      // Cache size test
      printf("page,cpu_time\n");
      run_cache_size_test();
      break;
    case 2:
      // Cache line test
      run_cache_size_test();
      printf("element,time\n");
      run_cache_line_test();
      break;
    case 3:
      // Associativity test
      run_cache_size_test();
      printf("element,time\n");
      run_associativity_test();
      break;
  }

  return 0;
}
