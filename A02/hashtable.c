#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 10

typedef struct {
  char* key;
  int value;
} Entry;

typedef struct {
  Entry* entries;
} HashTable;

unsigned long hash(char* key) {
  unsigned long hash = 5381;
  int c;
  while ((c = *key++)) {
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  }
  return hash % TABLE_SIZE;
}

void put(HashTable* ht, char* key, int value) {
  unsigned long index = hash(key);
  ht->entries[index].key = key;
  ht->entries[index].value = value;
}

int get(HashTable* ht, char* key) {
  unsigned long index = hash(key);
  return ht->entries[index].value;
}

void resize(HashTable* ht, int new_size) {
  HashTable new_ht;
  new_ht.entries = malloc(new_size * sizeof(Entry));

  // Rehash all of the entries from the old table into the new one
  for (int i = 0; i < TABLE_SIZE; i++) {
    Entry* entry = &ht->entries[i];
    if (entry->key != NULL) {
      unsigned long index = hash(entry->key);
      new_ht.entries[index] = *entry;
    }
  }

  // Free the old table and update the table size
  free(ht->entries);
  ht->entries = new_ht.entries;
  TABLE_SIZE = new_size;
}

int main() {
  HashTable ht;
  ht.entries = malloc(TABLE_SIZE * sizeof(Entry));
  put(&ht, "key1", 1);
  put(&ht, "key2", 2);
  put(&ht, "key3", 3);
  printf("%d\n", get(&ht, "key1"));
  printf("%d\n", get(&ht, "key2"));
  printf("%d\n", get(&ht, "key3"));
  return 0;
}