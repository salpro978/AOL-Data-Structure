#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#define HASH_SIZE 503
#define ALPHABET_SIZE 26
#define SYNONYM_FILE "synonymList.txt"
#define HISTORY_FILE "history.txt"

#include <stddef.h>

/*
    copy to terminal to create .exe program : 
    gcc history.c synonym.c main.c -o program
*/

// declaration the data struture
typedef struct SynonymNode 
{
    char* word;
    struct SynonymNode* next;
} SynonymNode;

// Struktur node untuk kata utama di Hash table
typedef struct WordEntry {
    char* word;
    SynonymNode* synonyms;  // linked list sinonim
    struct WordEntry* next; // untuk collision chaining
} WordEntry;

// Trie node untuk sesuai urutan
typedef struct TrieNode {
    struct TrieNode *children[ALPHABET_SIZE];
    int isEndOfWord; // 1 jika akhir kata
} TrieNode;

typedef struct HistoryNode
{ 
    char* action;
    char* time;
    struct HistoryNode* next;
} HistoryNode;

// declare global variable
extern WordEntry* hashTable[HASH_SIZE];
extern HistoryNode *historyTop;
extern TrieNode *rootTrie;

// declaration the functions here
// global
static inline void clearScreen(void);
static inline void pauseProgram(void);
void trim_and_tolower(char *word);
void trim_and_tocapital(char *word);
void trim(char *word);

// create node and hash
SynonymNode *createSynonymNode(const char *word);
WordEntry *createWordEntry(const char *word);
WordEntry *findWord(const char *word);
HistoryNode *createHistoryNode(const char *action);
TrieNode *createTrieNode();
unsigned int hash(const char *str);

// synonym.c
void addSynonym(const char *word, const char *synonym, const int condition);
void addWord(const char* str, const int condition);
void printSynonyms(const char* str);
void saveToFileSynonym(const char* filename);
void loadFromFileSynonym(const char* filename);
void printAllWords(void);
void freeMemorySynonym(void);
void freeTrieNode(TrieNode *root);
void menu(void);

// history.c
void pushLoadedHistory(const char *time, const char *action);
void saveToFileHistory(const char *filename);
void loadFromFileHistory(const char *filename);
void getCurrentTimestamp(char *buffer, size_t size);
void pushHistory(const char *action);
void printHistory(void);
void freeHistory(void);

#endif