#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

/* Single Linked List */
struct Node{
  int state;
  size_t size;
  struct Node* next;
};

//#define msize sizeof(struct Node)
unsigned long allsize = 0;
size_t msize = 24;
struct Node* all_head = NULL; // keep track of all head
struct Node* head = NULL;
struct Node* tail = NULL;

void* request_more(size_t size);

struct Node* check_insert_bf(size_t size);

struct Node* check_insert_ff(size_t size);

void split(struct Node* prev, struct Node* block, size_t size);

void printlist();

void* ff_malloc(size_t size);

void ff_free(void* ptr);

void* bf_malloc(size_t size);

void bf_free(void* ptr);

void all_free(void* ptr);

unsigned long get_data_segment_size();

unsigned long get_data_segment_free_space_size();
