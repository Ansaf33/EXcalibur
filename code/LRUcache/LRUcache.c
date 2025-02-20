#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 5 // Maximum cache size
#define HASHMAP_SIZE 50
#define MAX_DEPENDENCIES 10

typedef struct LRUNode
{
  char *expression;
  int register_id;
  char *dependencies[MAX_DEPENDENCIES];
  struct LRUNode *prev;
  struct LRUNode *next;
  struct LRUNode *hash_next; // For handling collisions in the hashmap
} LRUNode;

typedef struct
{
  LRUNode *head;
  LRUNode *tail;
  int size;
  LRUNode *hashmap[HASHMAP_SIZE];
} LRUCache;

unsigned int hash_func(const char *expression)
{
  unsigned int hash = 0;
  while (*expression)
  {
    hash = (hash * 31) + (*expression++);
  }
  return hash % HASHMAP_SIZE;
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

      if( lru->dependencies ){
        int i = 0;
        while( lru->dependencies[i] != NULL ){
          free(lru->dependencies[i]);
          i++;
        }
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

  if( dependencies ){
    int i = 0;
    while( dependencies[i] != NULL ){
      new_node->dependencies[i] = (char*)malloc(sizeof(char)*10);
      strcpy(new_node->dependencies[i],dependencies[i]);
      i++;
    }
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

void invalidate_dependencies(LRUCache *cache, const char *variable)
{
  LRUNode *current = cache->head;
  while (current)
  {
    LRUNode *next = current->next;

    if( current->dependencies ){
      int i = 0;
      while( current->dependencies[i] != NULL ){
        if( strcmp(current->dependencies[i],variable) == 0 ){
          put(cache,current->expression,-1,NULL);
          break;
        }
        i++;
      }
    }
    current = next;
  }
}

void print_cache(LRUCache *cache)
{
  LRUNode *current = cache->head;
  printf("Cache State (MRU -> LRU):\n");
  while (current)
  {
    printf("[%s: R%d] -> ", current->expression, current->register_id);
    if( current->dependencies ){
      int i = 0;
      printf("( ");
      while( current->dependencies[i] != NULL ){
        printf("%s, ",current->dependencies[i]);
        i++;
      }
      printf(") ]\n");
    }
    current = current->next;
  }
  printf("NULL\n");
}

void free_cache(LRUCache *cache)
{
  LRUNode *current = cache->head;
  while (current)
  {
    LRUNode *temp = current;
    current = current->next;
    free(temp->expression);
    if( temp->dependencies ){
      int i = 0;
      while(temp->dependencies[i] != NULL ){
        free(temp->dependencies[i]);
        i++;
      }
    }
    free(temp);
  }
  free(cache);
}
