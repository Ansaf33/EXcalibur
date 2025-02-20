#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CAPACITY 5 // Maximum cache size
#define HASHMAP_SIZE 50

typedef struct LRUNode
{
  char *expression; // Expression as key
  int register_id;  // Register ID as value
  struct LRUNode *prev;
  struct LRUNode *next;
} LRUNode;

typedef struct
{
  LRUNode *head; // Most recently used (MRU)
  LRUNode *tail; // Least recently used (LRU)
  int size;
  LRUNode *hashmap[HASHMAP_SIZE]; // Hashmap for fast lookups
} LRUCache;

// Hash function (basic sum-based hashing)
unsigned int hash_func(const char *expression)
{
  unsigned int hash = 0;
  while (*expression)
  {
    hash = (hash * 31) + (*expression++); // Simple polynomial rolling hash
  }
  return hash % HASHMAP_SIZE;
}

// Create a new LRU Cache
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

// Move a node to the front (most recently used)
void move_to_front(LRUCache *cache, LRUNode *node)
{
  if (cache->head == node)
    return; // Already at front

  // Remove node from current position
  if (node->prev)
    node->prev->next = node->next;
  if (node->next)
    node->next->prev = node->prev;

  // If removing tail, update tail
  if (cache->tail == node)
    cache->tail = node->prev;

  // Move node to front
  node->next = cache->head;
  node->prev = NULL;
  if (cache->head)
    cache->head->prev = node;
  cache->head = node;
}

// Get a value from the cache
int getValue(LRUCache *cache, const char *expression)
{
  unsigned int hash = hash_func(expression);
  LRUNode *node = cache->hashmap[hash];

  // Traverse the hashmap bucket to handle collisions
  while (node)
  {
    if (strcmp(node->expression, expression) == 0)
    {
      move_to_front(cache, node);
      return node->register_id; // Return stored register
    }
    node = node->next;
  }

  return -1; // Not found
}

// Put a value into the cache
void put(LRUCache *cache, const char *expression, int reg_id)
{
  if (!expression)
    return; // Handle null input

  unsigned int hash = hash_func(expression);
  LRUNode *node = cache->hashmap[hash];

  // Traverse the hashmap bucket to handle collisions
  while (node)
  {
    if (strcmp(node->expression, expression) == 0)
    {
      node->register_id = reg_id;
      move_to_front(cache, node);
      return;
    }
    node = node->next;
  }

  // Evict least recently used (LRU) if full
  if (cache->size == CAPACITY)
  {
    LRUNode *lru = cache->tail;
    if (lru)
    {
      unsigned int old_hash = hash_func(lru->expression);
      LRUNode *curr = cache->hashmap[old_hash];
      LRUNode *prev = NULL;

      // Remove from hashmap bucket
      while (curr)
      {
        if (curr == lru)
        {
          if (prev)
            prev->next = curr->next;
          else
            cache->hashmap[old_hash] = curr->next;
          break;
        }
        prev = curr;
        curr = curr->next;
      }

      // Remove from linked list
      if (lru->prev)
        lru->prev->next = NULL;
      cache->tail = lru->prev;

      free(lru->expression);
      free(lru);
      cache->size--;
    }
  }

  // Create new node
  LRUNode *new_node = (LRUNode *)malloc(sizeof(LRUNode));
  if (!new_node)
    return; // Handle malloc failure

  new_node->expression = strdup(expression);
  if (!new_node->expression)
  {
    free(new_node);
    return; // Handle strdup failure
  }

  new_node->register_id = reg_id;
  new_node->prev = NULL;
  new_node->next = cache->head;

  // Add to front
  if (cache->head)
    cache->head->prev = new_node;
  cache->head = new_node;
  if (!cache->tail)
    cache->tail = new_node;

  // Add to hashmap
  new_node->next = cache->hashmap[hash];
  cache->hashmap[hash] = new_node;

  cache->size++;
}

// Print cache (for debugging)
void print_cache(LRUCache *cache)
{
  LRUNode *current = cache->head;
  printf("Cache State (MRU -> LRU):\n");
  while (current)
  {
    printf("[%s: R%d] -> ", current->expression, current->register_id);
    current = current->next;
  }
  printf("NULL\n");
}

// Free memory
void free_cache(LRUCache *cache)
{
  LRUNode *current = cache->head;
  while (current)
  {
    LRUNode *temp = current;
    current = current->next;
    free(temp->expression);
    free(temp);
  }
  free(cache);
}
