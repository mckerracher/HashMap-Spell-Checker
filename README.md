# HashMap Spell Checker
Assignment 5 (Portfolio Assignment):  
Implementation of hash map and spell checker 
 
Portfolio Assignment 
The purpose of the Portfolio Assignment is to showcase your programming skills to potential employers and colleagues. This is the only assignment in this course that is allowed to be publicly posted online (e.g. GitHub, personal website, etc. …). While this is a great opportunity to publicize your work, it is not required that you post the assignment online.  
 
Prerequisites 
There are two parts to this assignment. In the first part, you will complete the implementation of a hash map. In the second part, you will implement a spell checker. 
 
Part 1: Hash Map 
First complete the hash map implementation in hashMap.c. This hash map uses a table of buckets, each containing a linked list of hash links. Each hash link stores the key-value pair (string and integer in this case) and a pointer to the next link in the list. You must implement each function in hashMap.c with the // FIXME: implement comment. 
 
hashMap.h is the header file which defines structs and public functions for your hash table. At the top of hashMap.h you should see two macros: HASH_FUNCTION and MAX_TABLE_LOAD. 
 
Make sure everywhere in your implementation to use HASH_FUNCTION(key) instead of directly calling a hash function. MAX_TABLE_LOAD is the table load threshold 
(>=MAX_TABLE_LOAD) on which you should trigger resizing the table (double the size of the current hash table capacity). 
 
A number of tests for the hash map are included in tests.c. Each one of these test cases use several or all of the hash map functions, so don’t expect tests to pass until you implement all of them. Each test case is slightly more thorough than the one before it and there is a lot of redundancy to better ensure correctness. Use these tests to help you debug your hash map implementation. They will also help your TA grade your submission. You can build the tests with make tests or make and run them with ./tests. 
 
Part 2: Spell Checker 
There are a lot of uses for a hash map, and one of them is implementing a caseinsensitive spell checker. All you need to get started is a dictionary, which is provided in dictionary.txt.
 
The function nextWord() must take a FILE*, allocate memory for the next word in the file, and returns the word. If the end of the file is reached, nextWord() will return NULL. 
 
The spellchecker program should flow as follows: 
 
1.	The user types in a word [only one word (consists of uppercase and lowercase letters only) at a time should be allowed] 
2.	If the spelling is correct, the following message should be outputted: 
"The inputted word .... is spelled correctly" 
3.	If the spelling is incorrect, the following message should be outputted:  
" The inputted word .... is spelled incorrectly". Also, 5 potential matches should be outputted like "Did you mean...?" (5 choices) 
4.	Continue to prompt user for a word until they type "Quit"
