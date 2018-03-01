#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// For CPU affinity
#define _GNU_SOURCE
#include <sched.h>

// For allocation
#define PAGE_SIZE sysconf(_SC_PAGESIZE)

#define MAX_SIZE 99999999

// Create storage
int arr[MAX_SIZE];

void fill_array() {
  int i = 0;
  for(; i < MAX_SIZE; i++) {
    arr[i] = 0;
  }
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

  return 0;
}
