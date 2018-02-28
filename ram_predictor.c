#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "ram_predictor.h"


#define PAGE_SIZE sysconf(_SC_PAGESIZE)
#define BLOCK_SIZE PAGE_SIZE * 1024
#define DATA_SIZE BLOCK_SIZE - sizeof(address_info_t) 

// If the time we get is greater than PF_INDICATOR_FACTOR * avg time, then 
// it means it is page fault
#define PF_INDICATOR_FACTOR 2

uint64_t total_memory = 0;
address_info_t *head = NULL;
address_info_t *tail = NULL;
unsigned total_element = 0;

// Measure the memset performance and return the time
double mem_benchmark(address_info_t *addr, size_t size) {
  clock_t start, end;
  char byte = 'c';
  double delta_time;

  start = clock();
  // Memset here
  memset(addr, byte, size);
  end = clock();
  delta_time = ((double) (end - start));
  return delta_time;
}

// Push to linked list
void push(address_info_t* node) {
  total_element += 1;
  // If list is empty, add the new node as head
  if (head == NULL) {
    head = node;
    tail = node;
    return;
  }
  tail->next = node;
  tail = node;
}

// Count the stats

// Check if page fault

int main() {

  while(1) {
    address_info_t *addr;

    if ((addr = malloc(BLOCK_SIZE)) == NULL)
      return -1;

    // Store the time in the struct
    addr->time = mem_benchmark(addr + sizeof(address_info_t), DATA_SIZE);
    addr->next = NULL;

    // Push the new node to the list
    push(addr);

    total_memory += BLOCK_SIZE;

    printf("Total memory requested: %lu\n", total_memory);
  }

  printf("Predicted memory: %lu\n", total_memory);
  return 0;
}


