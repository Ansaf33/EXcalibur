#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "LRUcache.h"
#include "../reghandling.h"

#define MAX_DEPENDENCIES 10

unsigned int hash_func(const char *expression)
{
  unsigned int hash = 0;
  while (*expression)
  {
    hash = (hash * 31) + (*expression++);
  }
  return hash % MAP_SIZE;
}

LRUCache *create_cache()
{
  LRUCache *cache = (LRUCache *)malloc(sizeof(LRUCache));
  if (!cache)
    return NULL;

  cache->head = NULL;
  cache->tail = NULL;
  cache->size = 0;
  memset(cache->hashmap, 0, sizeof(cache->hashmap));
  return cache;
}

void print_cache(LRUCache *cache)
{
  LRUNode *current = cache->head;
  printf("Cache State (MRU -> LRU):\n");
  while (current)
  {
    printf("[%s: %d ", current->expression, current->register_id);
    if (current->dependencies[0] != NULL)
    { // Fixed dependency check
      printf("( ");
      int i = 0;
      while (current->dependencies[i] != NULL)
      {
        printf("%s, ", current->dependencies[i]);
        i++;
      }
      printf(") ");
    }
    printf("] -> ");
    current = current->next;
  }
  printf("NULL\n");
}

void move_to_front(LRUCache *cache, LRUNode *node)
{
  if (cache->head == node)
    return;

  if (node->prev)
    node->prev->next = node->next;
  if (node->next)
    node->next->prev = node->prev;
  if (cache->tail == node)
    cache->tail = node->prev;

  node->next = cache->head;
  node->prev = NULL;
  if (cache->head)
    cache->head->prev = node;
  cache->head = node;
  if (!cache->tail)
    cache->tail = node;
}

int getValue(LRUCache *cache, const char *expression)
{
  unsigned int hash = hash_func(expression);
  LRUNode *node = cache->hashmap[hash];

  while (node)
  {
    if (strcmp(node->expression, expression) == 0)
    {
      move_to_front(cache, node);
      return node->register_id;
    }
    node = node->hash_next;
  }
  return -1;
}

void put(LRUCache *cache, const char *expression, int reg_id, char **dependencies)
{
  if (!expression)
    return;

  unsigned int hash = hash_func(expression);
  LRUNode *node = cache->hashmap[hash];

  while (node)
  {
    if (strcmp(node->expression, expression) == 0)
    {
      node->register_id = reg_id;
      move_to_front(cache, node);
      return;
    }
    node = node->hash_next;
  }

  if (cache->size == CAPACITY)
  {
    LRUNode *lru = cache->tail;
    if (lru)
    {
      unsigned int old_hash = hash_func(lru->expression);
      LRUNode **curr = &(cache->hashmap[old_hash]);
      while (*curr)
      {
        if (*curr == lru)
        {
          *curr = lru->hash_next;
          break;
        }
        curr = &((*curr)->hash_next);
      }
      if (lru->prev)
        lru->prev->next = NULL;
      else
        cache->head = NULL;
      cache->tail = lru->prev;

      free(lru->expression);

      // Free dependencies
      int i = 0;
      while (lru->dependencies[i] != NULL)
      {
        free(lru->dependencies[i]);
        i++;
      }

      free(lru);
      cache->size--;
    }
  }

  LRUNode *new_node = (LRUNode *)malloc(sizeof(LRUNode));
  if (!new_node)
    return;

  new_node->expression = strdup(expression);
  new_node->register_id = reg_id;

  // Copy dependencies properly and ensure NULL termination
  memset(new_node->dependencies, 0, sizeof(new_node->dependencies));
  if (dependencies)
  {
    int i = 0;
    while (dependencies[i] != NULL && i < MAX_DEPENDENCIES - 1)
    {
      new_node->dependencies[i] = strdup(dependencies[i]);
      i++;
    }
    new_node->dependencies[i] = NULL; // Null-terminate
  }

  new_node->prev = NULL;
  new_node->next = cache->head;
  new_node->hash_next = cache->hashmap[hash];
  if (cache->head)
    cache->head->prev = new_node;
  cache->head = new_node;
  if (!cache->tail)
    cache->tail = new_node;

  cache->hashmap[hash] = new_node;
  cache->size++;
}

 void invalidate_dependencies(LRUCache *cache, const char *variable) {
  printf("Invalidating %s\n", variable);
  LRUNode *current = cache->head;
  while (current)
  {
    LRUNode *next = current->next;

    if (current->dependencies)
    {
      int i = 0;
      while (current->dependencies[i] != NULL)
      {
        if (strcmp(current->dependencies[i], variable) == 0)
        {
          if( current->register_id >= 15 ){
            freeCacheReg(current->register_id);
          }
          // Just mark it invalid, don't remove dependencies
          put(cache, current->expression, 0, current->dependencies);
          break;
        }
        i++;
      }
    }
    current = next;
  }
  print_cache(cache);
}

void free_cache(LRUCache *cache)
{
  LRUNode *current = cache->head;
  while (current)
  {
    LRUNode *temp = current;
    current = current->next;

    free(temp->expression);

    int i = 0;
    while (temp->dependencies[i] != NULL)
    {
      free(temp->dependencies[i]);
      i++;
    }
    memset(temp->dependencies, 0, sizeof(temp->dependencies));

    free(temp);
  }
  free(cache);
}
