#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// For CPU affinity
#include <sched.h>

// For allocation
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

// The size of the array, it depends on the memory we have
// If our memory is too small, while this hyperparameter is too large, it can
// cause segmentation fault
#define MAX_SIZE 99999999

// Number of trials to access the memory
#define TRIALS 100000

// Total number of integer we get in a block
#define BLOCKS PAGE_SIZE / sizeof(int)

// Create storage
int arr[MAX_SIZE];

void fill_array() {
  int i = 0;
  for(; i < MAX_SIZE; i++) {
    arr[i] = 0;
  }
}

void generate_cache_performance(unsigned number_of_pages) {
  struct timespec start_t, end_t;

  // Floating value to avoid optimization from compiler
  // We also use this to calculate the average time to this experiment
  double n = 0;
  double total_time = 0;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_t);

  // TODO: Do the experiment here
  int i = 0;
  for(; i < TRIALS; i++) {
    int j = 0;
    for(; j < number_of_pages * BLOCKS; j += BLOCKS) {
      arr[j]++;
      n += 1.0;
    }
  }

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_t);
  unsigned long start = 1000000000*start_t.tv_sec + start_t.tv_nsec;
  unsigned long end = 1000000000*end_t.tv_sec + end_t.tv_nsec;
  total_time += ((double) (end - start));
  printf("%u, %lf\n", number_of_pages, total_time / n);

}

int main() {
  // For multi processor architecture, we want this process to be run in a processor only
  cpu_set_t set;
  CPU_ZERO(&set);

  // Add cpu 0 to set
  CPU_SET(0, &set);

  // If fail to se affinity
  if (sched_setaffinity(getpid(), sizeof(set), &set) == -1) {
    exit(1);
  }
  
  // Initiate array
  fill_array();

  printf("Number of trials: %d\n", TRIALS);
  printf("Generating cache performance data...\n");
  
  int i = 2;
  while (i <= 4096) {
    generate_cache_performance(i);
    i = i * 2;
  }

  return 0;
}
