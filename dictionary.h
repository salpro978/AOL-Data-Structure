#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#define HASH_SIZE 503

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

// Struktur Stack untuk Undo
typedef struct UndoAction {
    char* word;
    char* synonym;
    struct UndoAction* next;
} UndoAction;

typedef struct HistoryNode
{ 
    char* action;
    char* time;
    struct HistoryNode* next;
} HistoryNode;

// declare global variable
extern HistoryNode *historyTop;

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
unsigned int hash(const char *str);

// synonym.c
void addSynonym(const char *word, const char *synonym);
void addWord(const char* str);
void printSynonyms(const char* str);
void saveToFile(const char* filename);
void loadFromFile(const char* filename);
void printAllWords(void);
void freeMemory(void);
void menu(void);

// history.c


#endif