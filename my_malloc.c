#include "my_malloc.h"

#define MALLOC(sz) ff_malloc(sz)
#define FREE(p)    ff_free(p)

void* request_more(size_t size){
  size_t more = size + msize;
  struct Node* new_block = sbrk(more);
  if (new_block == (void*)-1){
    return NULL;
  }
  allsize += more;
  tail = new_block; //everytime append, update tail, next=NULL
  new_block->state = 1;
  new_block->size = size;
  new_block->next = NULL;
  return new_block;
}

void split(struct Node* prev, struct Node* block, size_t size){
  struct Node* new_block = (struct Node*)((char*)block + size + msize);
  new_block->state = 0;
  new_block->size = block->size - size - msize;
  new_block->next = block->next;
  block->state = 1;
  block->size = size;
  block->next = NULL;
  if (block == head){
    head = new_block;
  }
  else{
    prev->next = new_block; //update the freelist
  }
  if (block == tail){
    tail = new_block; // if split last block, update tail
  }
  return;
}

struct Node* check_insert_ff(size_t size){
  struct Node* prev = NULL;
  struct Node* cur = head;
  while (cur){
    if (cur->size >= size){
      break;
    }
    else{
      prev = cur;
      cur = cur->next;
    }
  }
  
  if (!cur){
    return request_more(size);
  }
  else{
    if (cur->size > size + msize){
      split(prev, cur,size);
    }
    else{
      cur->state = 1;
      if (cur == head){
        head = head->next;
      }
      else{
        prev->next = cur->next;
        cur->state = 1;
        cur->next = NULL;
      }
    }
    return cur;
  }
}

struct Node* check_insert_bf(size_t size){
  struct Node* prev = NULL;
  struct Node* cur = head;
  struct Node* tprev = NULL;
  struct Node* fit = NULL;
  while (cur){
    if (cur->size >= size){
      if (!fit){
        fit = cur;
        prev = tprev;
      }
      else{
        if (cur->size < fit->size){
          fit = cur;
          prev = tprev;
        }
      }
      if (cur->size == size){
        break;
      }
    } 
    tprev = cur;
    cur = cur->next;
  }
  
  if (!fit){
    return request_more(size);
  }
  else{
    if (fit->size > size + msize){
      split(prev,fit,size);
    }
    else{
      fit->state = 1;
      if (fit == head){
        head = head->next;
      }
      else{
        prev->next = fit->next;
        fit->state = 1;
        fit->next = NULL;
      }
    }
    return fit;
  }
}

void* bf_malloc(size_t size){
  if (size <= 0){
    return NULL;
  }
  struct Node* res = NULL;
  if (tail == NULL){
    res = request_more(size);
  }
  else{
    res = check_insert_bf(size);
    }
  if (res == NULL){
    return res;
  }
  else{
    return res+1;
  }
}

void* ff_malloc(size_t size){
  if (size <= 0){
    return NULL;
  }
  struct Node* res = NULL;
  if (tail == NULL){
    res = request_more(size);
  }
  else{
    res = check_insert_ff(size);
  }
  if (res == NULL){
    return res;
  }
  else{
    return res+1;
  }
}

void merge(struct Node* prev, struct Node* cur, struct Node* next){
  int mp = 0;
  int mn = 0;
  if (prev){
    if ((struct Node*)((char*)prev + prev->size + msize) == cur){
      mp = 1;
    }
  }
  if (next){
    if ((struct Node*)((char*)cur + cur->size + msize) == next){
      mn = 1;
    }
  }
  if (mp && mn){
    prev->next = next->next;
    prev->size += cur->size + msize + next->size + msize;
    cur->next = NULL;
    next->next = NULL;
  }
  if (!mp && mn){
    cur->next = next->next;
    cur->size += next->size + msize;
    next->next = NULL;
  }
  if (mp && !mn){
    prev->next = cur->next;
    prev->size += cur->size + msize;
    cur->next = NULL;
  }
}

void addfreelist(struct Node* toAdd){
  if (head == NULL){
    head = toAdd;
  }
  else{
    if (toAdd < head){
      toAdd->next = head;
      head = toAdd;
      merge(NULL,toAdd,head);
    }
    else{
      struct Node* prev = head;
      struct Node* next = head->next;
      while (next && prev){
        if (next > toAdd && prev < toAdd){
          break;
        }
        prev = prev->next;
        next = next->next;
      }
      prev->next = toAdd;
      toAdd->next = next;
      merge(prev,toAdd,next);
    }
  }
}

void all_free(void* ptr){
  if (ptr != NULL){
    struct Node* cur = ptr;
    cur -= 1;
    if (cur->state == 1){
      cur->state = 0;
      addfreelist(cur);
    }
  }
}

void ff_free(void* ptr){
  all_free(ptr);
}

void bf_free(void* ptr){
  all_free(ptr);
}

void printlist(){
  struct Node* cur = head;
  while (cur){
    printf("[%d, %zu] ",cur->state, cur->size);
    cur = cur->next;
  }
}

unsigned long get_data_segment_size(){
  //return (unsigned long)((char*)tail-(char*)all_head + tail->size + msize);
  return allsize;
}

unsigned long get_data_segment_free_space_size(){
  unsigned long size = 0;
  struct Node* cur = head;
  while (cur){
    size += cur->size + msize;
    cur = cur->next;
  }
  return size;
}

int main(int argc, char *argv[])
{
  const unsigned NUM_ITEMS = 10;
  int i;
  int size;
  int sum = 0;
  int expected_sum = 0;
  int *array[NUM_ITEMS];

  size = 4;
  expected_sum += size * size;
  array[0] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[0][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[0][i];
  } //for i

  size = 16;
  expected_sum += size * size;
  array[1] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[1][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[1][i];
  } //for i

  size = 8;
  expected_sum += size * size;
  array[2] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[2][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[2][i];
  } //for i

  size = 32;
  expected_sum += size * size;
  array[3] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[3][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[3][i];
  } //for i

  FREE(array[0]);
  FREE(array[2]);

  size = 7;
  expected_sum += size * size;
  array[4] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[4][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[4][i];
  } //for i

  size = 256;
  expected_sum += size * size;
  array[5] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[5][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[5][i];
  } //for i

  FREE(array[5]);
  FREE(array[1]);
  FREE(array[3]);

  size = 23;
  expected_sum += size * size;
  array[6] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[6][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[6][i];
  } //for i

  size = 4;
  expected_sum += size * size;
  array[7] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[7][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[7][i];
  } //for i

  FREE(array[4]);

  size = 10;
  expected_sum += size * size;
  array[8] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[8][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[8][i];
  } //for i

  size = 32;
  expected_sum += size * size;
  array[9] = (int *)MALLOC(size * sizeof(int));
  for (i=0; i < size; i++) {
    array[9][i] = size;
  } //for i
  for (i=0; i < size; i++) {
    sum += array[9][i];
  } //for i

  FREE(array[6]);
  FREE(array[7]);
  FREE(array[8]);
  FREE(array[9]);

  if (sum == expected_sum) {
    printf("Calculated expected value of %d\n", sum);
    printf("Test passed\n");
  } else {
    printf("Expected sum=%d but calculated %d\n", expected_sum, sum);
    printf("Test failed\n");
  } //else

  return 0;
}
