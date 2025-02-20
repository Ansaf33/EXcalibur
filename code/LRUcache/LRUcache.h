#ifndef LRU_CACHE_H
#define LRU_CACHE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Constants
#define CAPACITY 5      // Maximum cache size
#define MAP_SIZE 50 // Size of the hashmap for fast lookups

// Node structure for the doubly linked list
typedef struct LRUNode
{
  char* dependencies[10];
  char *expression;     // Expression as key
  int register_id;      // Register ID as value
  struct LRUNode *prev; // Pointer to the previous node
  struct LRUNode *next; // Pointer to the next node
} LRUNode;

// LRU Cache structure
typedef struct
{
  LRUNode *head;                  // Most recently used (MRU)
  LRUNode *tail;                  // Least recently used (LRU)
  int size;                       // Current size of the cache
  LRUNode *hashmap[MAP_SIZE]; // Hashmap for fast lookups
} LRUCache;

// Function prototypes

// Create a new LRU cache
LRUCache *create_cache();

// Get a value from the cache
int getValue(LRUCache *cache, const char *expression);

// Put a value into the cache
void put(LRUCache *cache, const char *expression, int reg_id,char** dependencies);

// Print the cache state (for debugging)
void print_cache(LRUCache *cache);

// Free the cache and release memory
void free_cache(LRUCache *cache);

// removes LRUcache entries if invalidated during assignment statement
void invalidate_dependencies(LRUCache* cache,char* expr);

#endif // LRU_CACHE_H
