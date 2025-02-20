#define HASH_SIZE 100 // Number of buckets in the hashmap

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../AST.h"

typedef struct HashNode
{
  char *key;
  struct TreeNode *value;
  struct HashNode *next; // Linked list for collisions
} HashNode;

typedef struct HashMap
{
  HashNode *buckets[HASH_SIZE]; // Array of pointers to HashNodes
} HashMap;

unsigned int hashFunction(const char *key)
{
  unsigned long hash = 5381;
  int c;
  while ((c = *key++))
  {
    hash = ((hash << 5) + hash) + c; // hash * 33 + c
  }
  return hash % HASH_SIZE;
}

HashMap *createHashMap()
{
  HashMap *map = (HashMap *)malloc(sizeof(HashMap));
  for (int i = 0; i < HASH_SIZE; i++)
  {
    map->buckets[i] = NULL; // Initialize all buckets to NULL
  }
  return map;
}

void insert(HashMap *map, const char *key, struct TreeNode *value)
{
  unsigned int index = hashFunction(key);
  HashNode *newNode = (HashNode *)malloc(sizeof(HashNode));
  newNode->key = strdup(key); // Copy the key
  newNode->value = value;
  newNode->next = map->buckets[index]; // Insert at head (separate chaining)
  map->buckets[index] = newNode;
}

struct TreeNode *get(HashMap *map, const char *key)
{
  unsigned int index = hashFunction(key);
  HashNode *current = map->buckets[index];

  while (current)
  {
    if (strcmp(current->key, key) == 0)
    {
      return current->value; // Found
    }
    current = current->next; // Move to next node (handle collisions)
  }

  return NULL; // Not found
}
