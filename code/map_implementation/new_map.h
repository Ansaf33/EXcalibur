#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a struct for storing key-value pairs
typedef struct HashNode
{
  char *key;
  struct TreeNode *value;
  struct HashNode *next; // Handle collisions using chaining
} HashNode;

#define HASHMAP_SIZE 100 // Adjust as needed

typedef struct HashMap
{
  HashNode *buckets[HASHMAP_SIZE];
} HashMap;

// Function prototypes
HashMap *createHashMap();
void insert(HashMap *map, const char *key, struct TreeNode *value);
struct TreeNode *get(HashMap *map, const char *key);
void freeHashMap(HashMap *map);

#endif // HASHMAP_H
