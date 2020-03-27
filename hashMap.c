/*
 * CS 261 Data Structures
 * Assignment 5
 * Name: Joshua McKerracher
 * Date: 2/28/2020
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key) {
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key) {
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++) {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next) {
    struct HashLink * link = (struct HashLink *) malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link) {
    free(link->key);
    free(link);
}


/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity) {
    assert(capacity > 0);
    assert(map != 0);

    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    assert(map->table != 0);

    // intializes each bucket to null.
    for (int i = 0; i < capacity; i++) {
        map->table[i] = 0;
    }
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity) {
    assert(capacity > 0);

    HashMap* map = malloc(sizeof(HashMap));
    assert(map != 0);

    hashMapInit(map, capacity);

    return map;
}


/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map) {
    assert(map != 0);

    struct HashLink * curr, * temp;
    for (int i = 0; i < map->capacity; i++) {
        curr = map->table[i];
        while(curr != 0) {
            temp = curr;
            curr = curr->next;
            hashLinkDelete(temp);

        }
    }
}


/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map) {
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 *
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key) {
    int * tmp = NULL;

    // Compute index with hash function.
    int index = HASH_FUNCTION(key) % map->capacity;

    // Sets curr to the first link at index.
    struct HashLink * curr = map->table[index];

    // Checks for the key in the list at index.
    while (curr != 0) {
        if(strcmp(key, curr->key) == 0) {
            tmp = &curr->value;
            return tmp;
        } else {
            curr = curr->next;
        }
    }

    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given
 * capacity (double of the old capacity). After allocating the new table,
 * all of the links need to rehashed into it because the capacity has changed.
 *
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 *
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity) {
    assert(map != 0);
    assert(capacity > map->capacity);

    struct HashMap * old, * new;
    int oldCapacity = map->capacity;
    old = map;

    struct HashLink * curr;

    new = hashMapNew(capacity);


    // Copying all elements to the new map.
    for (int i = 0; i < oldCapacity; i++) {

        curr = old->table[i];

        while (curr != 0) {
            hashMapPut(new, curr->key, curr->value);
            curr = curr->next;
        }
    }

    map->capacity = capacity;
    map->size = new->size;
    map->table = new->table;
    new->table = 0;
    free(new);
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 *
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value) {
    assert(map != 0);
    assert(key != 0);

    int index = HASH_FUNCTION(key) % map->capacity;

    struct HashLink * newLink;

    // Checks that the index is positive.
    if (index < 0) {
        index += map->size;
    }

    // Check and replaces duplicate key.
    if(hashMapContainsKey(map, key)) {
        hashMapRemove(map, key);
    }

    // creates and initializes the new link.
    newLink = hashLinkNew(key, value, map->table[index]);
    assert(newLink != 0);

    // inserts the new link to the table.
    map->table[index] = newLink;

    map->size++;

    // Resize if load factor is too high.
    if (hashMapTableLoad(map) > MAX_TABLE_LOAD) {
        resizeTable(map, (map->capacity * 2));
    }
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key) {
    assert(map != 0);

    if(hashMapContainsKey(map, key)) {

        int idx = HASH_FUNCTION(key) % map->capacity;

        HashLink * curr = map->table[idx];
        HashLink * prev = map->table[idx];
        HashLink * tmp;
        int loopCount = 0;

        while(curr != 0) {

            if(strcmp(key, curr->key) == 0) {

                if(loopCount == 0) {
                    tmp = curr;
                    map->table[idx] = curr->next;
                    curr = curr->next;
                    prev = prev->next;
                } else {
                    tmp = curr;
                    prev->next = curr->next;
                    curr = curr->next;
                }

                hashLinkDelete(tmp);
                tmp = 0;
                loopCount++;
                map->size--;

                // if match not found, advance hashLinks.
            } else {
                // if this is first loop, only advance curr.
                if (loopCount == 0) {
                    curr = curr->next;
                    loopCount++;
                } else {
                    // otherwise advance both.
                    prev = prev->next;
                    curr = curr ->next;
                    loopCount++;
                }
            }
        }
    }
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 *
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 *
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key) {
    assert(map != 0);

    int index = HASH_FUNCTION(key) % map->capacity;

    // Sets curr equal to the first link at index.
    struct HashLink * curr = map->table[index];


    // Return 0 if curr is assigned to an empty index.
    if(curr == 0) {
        return 0;
    }

    // Linear search through the linked list.
    while(curr != 0) {
        if(strcmp(key, curr->key) == 0) {
            return 1;
        } else {
            curr = curr->next;
        }

    }

    // Returns 0 if the key isn't found.
    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map) {
    assert(map != 0);
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map) {
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map) {
    assert(map != 0);

    int counter = 0;

    for(int i = 0; i < map->capacity; i++) {
        if(map->table[i] == 0) {
            counter++;
        }
    }

    return counter;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map) {
    assert(map != 0);

    float load = 0.0;

    load = ((float)(map->size) / (float)(map->capacity));

    return load;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map) {
    assert(map != 0);

    struct HashLink * curr;

    for(int i = 0; i < map->capacity; i++) {
        int loopCount = 0;

        curr = map->table[i];

        if(curr == 0) {
            printf("Slot %d = empty\n", i);
        }

        while (curr != 0) {

            if(loopCount == 0) {
                printf("\nSlot %d = ", i);
                loopCount++;
            } else {
                printf("Slot %d = ", i);
                loopCount++;
            }

            printf("Key: %s = ", curr->key);

            printf("Value = %d\n", curr->value);

            curr = curr->next;
        }

    }

}


