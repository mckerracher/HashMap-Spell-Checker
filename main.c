#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

# define TYPE const char *

// This is used to store the 5 lowest Levenshtein distances.
struct array {
    TYPE data [10];
    int count;
};

void initArray(struct array * a) {
    a->count = 0;
}

void addToArray(struct array * a, TYPE c) {
    a->data[a->count] = c;
    a->count++;
}


/**
 * This function is used to check if a word already exists in the list of lowest Levenshtein distances.
 * @param a array to be checked.
 * @param c string to be checked for.
 * @return 1 if the array has c, 0 otherwise.
 */
int contains(struct array * a, TYPE c) {
    assert(a != 0);

    for (int i = 0; i < a->count; i++) {
        if(strcmp(c, a->data[i]) == 0) {
            return 1;
        }
    }

    return 0;
}

/**
 * This function calculates the Levenshtein edit distance between two words.
 * @param wordOne
 * @param lengthOne
 * @param wordTwo
 * @param lengthTwo
 * @return
 */
int LevenshteinDistance(const char * wordOne, int lengthOne, const char * wordTwo, int lengthTwo) {

    int wordLengths[lengthOne + 1][lengthTwo + 1];

    int del;
    int insert;
    int sub;
    int min;
    int i;

    for (i = 0; i <= lengthOne; i++) {
        wordLengths[i][0] = i;
    }

    for (i = 0; i <= lengthTwo; i++) {
        wordLengths[0][i] = i;
    }

    for (i = 1; i <= lengthOne; i++) {
        int j;

        char charrOne;

        charrOne = wordOne[i - 1];

        for (j = 1; j <= lengthTwo; j++) {

            char charrTwo;

            charrTwo = wordTwo[j - 1];

            if (charrOne == charrTwo) {
                wordLengths[i][j] = wordLengths[i - 1][j - 1];
            }

            else {

                del = wordLengths[i - 1][j] + 1;
                insert = wordLengths[i][j - 1] + 1;
                sub = wordLengths[i - 1][j - 1] + 1;
                min = del;

                if (insert < min) {
                    min = insert;
                }
                if (sub < min) {
                    min = sub;
                }

                wordLengths[i][j] = min;
            }
        }
    }
    return wordLengths[lengthOne][lengthTwo];
}

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file) {
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map) {
    assert(map != 0);

    char * word = nextWord(file);

    while (word) {
        hashMapPut(map, word, 1);
        free(word);
        word = nextWord(file);
    }

    free(word);
}

/**
 * Loads the contents of the file into a hash map and sets the value of each element to the Levenshtein Distance
 * from the entered word.
 * @param file
 * @param map
 * @param word - everything in the dictionary is compared to this word for Levenshtein Distance.
 */
void loadDictionaryWithLevenshteinDistance(FILE* file, HashMap* map, const char * word) {
    assert(map != 0);

    const char * newWord = nextWord(file);
    int length1 = strlen(newWord);
    int length2 = strlen(word);
    int distance = LevenshteinDistance(newWord, length1, word, length2);

    while (newWord) {
        hashMapPut(map, newWord, distance);

        // Reset variable
        distance = 0;
        length1 = 0;
        length2 = 0;
        //free(newWord);


        // Re-assign variable values
        newWord = nextWord(file);
        if(newWord != NULL) {
            length1 += strlen(newWord);
            length2 += strlen(word);
            distance = LevenshteinDistance(newWord, length1, word, length2);
        }
    }
}


/**
 * Checks the spelling of the word provided by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provided word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv) {

    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("/Users/josh/CLionProjects/CS261Project5_2/dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;

    while (!quit) {

        printf("\nEnter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);

        if (strcmp(inputBuffer, "quit") == 0) {
            quit = 1;
            break;
        }

        if(hashMapContainsKey(map, inputBuffer)) {
            printf("The inputted word %s is spelled correctly\n", inputBuffer);

        } else {
            FILE* newFile = fopen("/Users/josh/CLionProjects/CS261Project5_2/dictionary.txt", "r");

            // New HashMap with values equal to Levenshtein Distance from entered word.
            HashMap * levMap = hashMapNew(1000); // freed
            loadDictionaryWithLevenshteinDistance(newFile, levMap, inputBuffer);
            fclose(newFile);

            // Array used to hold the 5 lowest Levenshtein distance words.
            struct array * levArray = (struct array *) malloc(sizeof(struct array) * 5); // freed
            initArray(levArray);

            // Word & int 1
            HashLink * tmp;
            tmp = levMap->table[levMap->capacity - 1];

            // Gets a HashLink with a word with a Lev distance > 1.
            int x = levMap->capacity - 1;
            while((tmp == NULL) && (x > 0)) {
                tmp = levMap->table[x];
                x--;
            }

            // This Levenshtein distance is compared to every distance in the map, searching for a lower one.
            int tempNum = tmp->value;


            // Once the lowest Levenshtein distance is found, this word is added to an array of words with the 5 lowest
            // distances.
            const char * lowestLevenshteinDistanceWord = tmp->key; // first test word

            HashLink * curr;
            for (int i = 0; i < levMap->capacity; i++) {
                curr = levMap->table[i];
                while(curr != 0) {
                    if (curr->value < tempNum) {
                        tempNum = curr->value;
                        lowestLevenshteinDistanceWord = curr->key;

                        // Restart loop to make sure this is the lowest Lev Distance.
                        i = -1;
                        break;
                    } else {
                        curr = curr->next;
                    }
                }
            }

            // 1.
            // Adds the lowest Levenshtein distance word to the array.
            addToArray(levArray, lowestLevenshteinDistanceWord);

            // To find the next word with the next lowest Levenshtein distance, it's distance must be equal to or
            // greater than the first one.
            int baseValue = tempNum;

            // Sets tmp to the last word in the map.
            tmp = levMap->table[levMap->capacity - 1];
            x = levMap->capacity - 1;
            while((tmp == NULL) && (x > 0)) {
                tmp = levMap->table[x];
                x--;
            }

            tempNum = tmp->value;
            lowestLevenshteinDistanceWord = tmp->key;


            for (int i = 0; i < levMap->capacity; i++) {
                curr = levMap->table[i];
                while(curr != 0) {

                    // If curr's value is lower than tempNum and >= baseValue and curr's key isn't already in the array
                    if ((curr->value < tempNum) && (curr->value >= baseValue) && (!contains(levArray,curr->key))) {
                        tempNum = curr->value;
                        lowestLevenshteinDistanceWord = curr->key;

                        // Restart loop to make sure this is the lowest Lev Distance.
                        i = -1;
                        break;
                    } else {
                        curr = curr->next;
                    }
                }
            }

            // 2.
            // Adds the next lowest Levenshtein distance word to the array.
            addToArray(levArray, lowestLevenshteinDistanceWord);

            // To find the next word with the next lowest Levenshtein distance, it's distance must be equal to or
            // greater than the first one.
            baseValue = tempNum;

            // Sets tmp to the last word in the map.
            tmp = levMap->table[levMap->capacity - 1];
            x = levMap->capacity - 1;
            while((tmp == NULL) && (x > 0)) {
                tmp = levMap->table[x];
                x--;
            }

            tempNum = tmp->value;
            lowestLevenshteinDistanceWord = tmp->key;


            for (int i = 0; i < levMap->capacity; i++) {
                curr = levMap->table[i];
                while(curr != 0) {

                    // If curr's value is lower than tempNum and >= baseValue and curr's key isn't already in the array
                    if ((curr->value < tempNum) && (curr->value >= baseValue) && (!contains(levArray,curr->key))) {
                        tempNum = curr->value;
                        lowestLevenshteinDistanceWord = curr->key;

                        // Restart loop to make sure this is the lowest Lev Distance.
                        i = -1;
                        break;
                    } else {
                        curr = curr->next;
                    }
                }
            }

            // 3.
            // Adds the next lowest Levenshtein distance word to the array.
            addToArray(levArray, lowestLevenshteinDistanceWord);

            // To find the next word with the next lowest Levenshtein distance, it's distance must be equal to or
            // greater than the first one.
            baseValue = tempNum;

            // Sets tmp to the last word in the map.
            tmp = levMap->table[levMap->capacity - 1];
            x = levMap->capacity - 1;
            while((tmp == NULL) && (x > 0)) {
                tmp = levMap->table[x];
                x--;
            }

            tempNum = tmp->value;
            lowestLevenshteinDistanceWord = tmp->key;

            for (int i = 0; i < levMap->capacity; i++) {
                curr = levMap->table[i];
                while(curr != 0) {

                    // If curr's value is lower than tempNum and >= baseValue and curr's key isn't already in the array
                    if ((curr->value < tempNum) && (curr->value >= baseValue) && (!contains(levArray,curr->key))) {
                        tempNum = curr->value;
                        lowestLevenshteinDistanceWord = curr->key;

                        // Restart loop to make sure this is the lowest Lev Distance.
                        i = -1;
                        break;
                    } else {
                        curr = curr->next;
                    }
                }
            }

            // 4.
            // Adds the next lowest Levenshtein distance word to the array.
            addToArray(levArray, lowestLevenshteinDistanceWord);


            // To find the next word with the next lowest Levenshtein distance, it's distance must be equal to or
            // greater than the first one.
            baseValue = tempNum;

            // Sets tmp to the last word in the map.
            tmp = levMap->table[levMap->capacity - 1];
            x = levMap->capacity - 1;
            while((tmp == NULL) && (x > 0)) {
                tmp = levMap->table[x];
                x--;
            }

            tempNum = tmp->value;
            lowestLevenshteinDistanceWord = tmp->key;


            for (int i = 0; i < levMap->capacity; i++) {
                curr = levMap->table[i];
                while(curr != 0) {

                    // If curr's value is lower than tempNum and >= baseValue and curr's key isn't already in the array
                    if ((curr->value < tempNum) && (curr->value >= baseValue) && (!contains(levArray,curr->key))) {
                        tempNum = curr->value;
                        lowestLevenshteinDistanceWord = curr->key;

                        // Restart loop to make sure this is the lowest Lev Distance.
                        i = -1;
                        break;
                    } else {
                        curr = curr->next;
                    }
                }
            }

            // 5.
            // Adds the next lowest Levenshtein distance word to the array.
            addToArray(levArray, lowestLevenshteinDistanceWord);


            printf("The inputted word %s is spelled incorrectly. Did you mean:\n", inputBuffer);
            for (int i = 0; i < 5; i++) {
                printf("%s\n", levArray->data[i]);
            }

            hashMapDelete(levMap);
            free(levArray);
        }
    }
    hashMapDelete(map);
    return 0;
}