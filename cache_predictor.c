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
#define TRIALS 1000000

// Create storage
int arr[MAX_SIZE];

void fill_array(int size) {
  int i = 0;
  for(; i < size; i++) {
    arr[i] = 0;
  }
}

void generate_cache_performance(int number_of_pages) {
  clock_t start, end;

  int blocks = PAGE_SIZE / sizeof(int);
  // Floating value to avoid optimization from compiler
  // We also use this to calculate the average time to this experiment
  double n = 0.0;
  double total_time = 0;

  start = clock();

  // Do the experiment here
  int i = 0;
  for(; i < TRIALS; i++) {
    int j = 0;
    for(; j < number_of_pages * blocks; j += blocks) {
      arr[j] += 1;
      n += 1.0;
    }
  }

  end = clock();
  unsigned long delta = ((double) (end - start));
  total_time += delta;
  printf("%u, %lf\n", number_of_pages, total_time / n);
}

double get_element_access_time(int index) {
  struct timespec start_t, end_t;
  double start, end, delta;

  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start_t);
  arr[index] += 1;
  clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end_t);

  start = 1000000000 * start_t.tv_sec + start_t.tv_nsec;
  end = 1000000000 * end_t.tv_sec + end_t.tv_nsec;
  delta = end - start;

  printf("%d, %lf\n", index, delta);

  return delta;
}

void run_cache_size_test() {
  // Initiate array
  fill_array(10485760);

  printf("Number of trials: %d\n", TRIALS);
  printf("Generating cache performance data...\n");

  int i = 2;
  while (i <= 4096) {
    generate_cache_performance(i);
    i = i * 2;
  }
} 

void run_cache_line_test() {
  int i  = 2;
  while (i <= 10000) {
    get_element_access_time(i);

    if (i == 1024 || i == 2048 || i == 4096 || i == 8192) {
      int j = 1;
      while (j <= 10) {
        get_element_access_time(i + j);
        j++;
      } 
    }
    i = i * 2;
  }
}

int main(int argc, char *argv[]) {
  int mode;
  char *p;
  if (argc < 2) {
    printf("Usage: ./cache_predictor operation_number\n" \
        "1: Generate cache size data\n" \
        "2: Generate cache line data\n");
    return 0;
  }
  else {
    long conv = strtol(argv[1], &p, 10);
    mode = conv;
  }

  // For multi processor architecture, we want this process to be run in a processor only
  cpu_set_t set;
  CPU_ZERO(&set);

  // Add cpu 0 to set
  CPU_SET(0, &set);

  // If fail to se affinity
  if (sched_setaffinity(getpid(), sizeof(set), &set) == -1) {
    exit(1);
  }

  switch(mode) {
    case 1:
      // Cache size test
      printf("------------------------------------\n");
      run_cache_size_test();
      break;
    case 2:
      // Cache line test
      printf("------------------------------------\n");
      run_cache_line_test();
      break;
  }


  

  return 0;
}
