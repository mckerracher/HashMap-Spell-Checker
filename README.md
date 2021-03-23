# HashMap Spell Checker
## Oregon State University Data Structures course portfolio assignment: an implementation of a hash map and a spell checker.

Part 1: Hash Map
The hash map implementation is in hashMap.c. This hash map uses a table of buckets, each containing a linked list of hash links. Each hash link stores the key-value pair (string and integer in this case) and a pointer to the next link in the list. hashMap.h is the header file which defines structs and public functions for the hash table. MAX_TABLE_LOAD is the table load threshold (>=MAX_TABLE_LOAD), which is used to trigger resizing the table (doubling the size of the current hash table capacity).
 
A number of tests for the hash map are included in tests.c. Each one of these test cases use several or all of the hash map functions. Each test case is slightly more thorough than the one before it and there is a lot of redundancy to better ensure correctness. These tests can be run with ./tests. 
 
Part 2: Spell Checker
There are a lot of uses for a hash map, and one of them is implementing a case insensitive spell checker. dictionary.txt is used the dictionary. The function nextWord() takes a FILE*, allocates memory for the next word in the file, and returns the word. If the end of the file is reached, nextWord() will return NULL. The spellchecker program flows as follows:
 
1.	The user types in a word [only one word (consists of uppercase and lowercase letters only) at a time is allowed] 
2.	If the spelling is correct, the following message is be outputted: 
"The inputted word .... is spelled correctly" 
3.	If the spelling is incorrect, the following message is outputted:  
" The inputted word .... is spelled incorrectly". Also, 5 potential matches are outputted like "Did you mean...?" (5 choices) 
4.	Continues to prompt user for a word until they type "Quit".
