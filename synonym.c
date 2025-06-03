#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include "dictionary.h"

WordEntry* hashTable[HASH_SIZE] = {0};
TrieNode *rootTrie = NULL;

static inline void clearScreen(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static inline void pauseProgram(void) {
    printf("Tekan Enter untuk melanjutkan...");
    while (getchar() != '\n');
}

void trim(char *word)
{
    char *start = word;
    while(isspace((unsigned char)*start)) start++;

    char *end = start + strlen(start) - 1;
    while(end > start && isspace((unsigned char)*end)) end--;
    
    *(end + 1) = 0;

    if(start != word) memmove(word, start, end - start + 2);
}

void trim_and_tolower(char *word) 
{
    trim(word);

    for(int i = 0; word[i]; i++) word[i] = tolower((unsigned char)word[i]);
}

void trim_and_tocapital(char *word)
{
    trim(word);

    word[0] = toupper((unsigned char)word[0]);
    for(int i = 1; word[i]; i++) word[i] = tolower((unsigned char)word[i]);
}

// Fungsi hash 'sederhana'
unsigned int hash(const char *str)
{
    unsigned int hash = 5381;
    int c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % HASH_SIZE;
}

// Buat node sinonim
SynonymNode *createSynonymNode(const char *word)
{
    SynonymNode *node = (SynonymNode *)malloc(sizeof(SynonymNode));
    node->word = strdup(word);
    node->next = NULL;
    return node;
}

// Buat entry kata utama
WordEntry *createWordEntry(const char *word)
{
    WordEntry *entry = (WordEntry *)malloc(sizeof(WordEntry));
    entry->word = strdup(word);
    entry->synonyms = NULL;
    entry->next = NULL;
    return entry;
}

// Cari kata di hash table
WordEntry *findWord(const char *word)
{
    unsigned int index = hash(word);
    WordEntry *entry = hashTable[index];
    while (entry != NULL)
    {
        if (strcmp(entry->word, word) == 0)
            return entry;
        entry = entry->next;
    }
    return NULL;
}

TrieNode *createTrieNode()
{
    TrieNode *newNode = (TrieNode*)malloc(sizeof(TrieNode));
    if(newNode)
    {
        newNode->isEndOfWord = 0;
        for(int i = 0; i < ALPHABET_SIZE; i++)
            newNode->children[i] = NULL;
    }
}

void insertToTrie(TrieNode *root, const char *word)
{
    TrieNode *curr = root;
    while(*word)
    {
        int index = *word - 'a';
        if(curr->children[index] == NULL)
            curr->children[index] = createTrieNode();
        curr = curr->children[index];
        word++;
    }
    curr->isEndOfWord = 1;
}

// Tambah kata baru ke hash table
void addWord(const char *word, const int condition)
{
    if (findWord(word)){
        printf("Entry word '%s' already exists.\n", word);
        sleep(0.7);
        return; // return if already exists
    }

    unsigned int index = hash(word);
    WordEntry *newEntry = createWordEntry(word);

    newEntry->next = hashTable[index];
    hashTable[index] = newEntry;

    if(condition)
    {
        int len = snprintf(NULL, 0, "create a new entry '%s' into the dictionary", newEntry->word);
        char *action = malloc(len + 1);
        if (action) {
            snprintf(action, len + 1, "create a new entry '%s' into the dictionary", newEntry->word);
            pushHistory(action);
            free(action);
        }
    }
}

// Tambah sinonim untuk sebuah kata
void addSynonym(const char *word, const char *synonym, const int condition)
{
    WordEntry *entry = findWord(word);
    if (!entry)
    {
        printf("Word Entry '%s' does not exists.\n", word);
        return;
    }

    SynonymNode *syn = entry->synonyms;
    while(syn)
    {
        if(strcmp(syn->word, synonym) == 0) return;
        syn = syn->next;
    }

    SynonymNode *newSyn = createSynonymNode(synonym);
    newSyn->next = entry->synonyms;
    entry->synonyms = newSyn;

    if(condition)
    {
        int len = snprintf(NULL, 0, "add synonym %s to %s", newSyn->word, entry->word);
        char *action = malloc(len + 1);
        if (action) {
            snprintf(action, len + 1, "add synonym %s to %s", newSyn->word, entry->word);
            pushHistory(action);
            free(action); // Jangan lupa membebaskan memori
        }
    }
}

// Tampilkan sinonim dari kata tertentu
void printSynonyms(const char *word)
{
    WordEntry *entry = findWord(word);
    if (!entry)
    {
        printf("Kata '%s' tidak ditemukan.\n", word);
        return;
    }

    printf("Sinonim dari '%s': ", word);
    SynonymNode *temp = entry->synonyms;
    if(!temp) {printf("No synonyms recorded.\n");}
    else 
    {
        printf("%s", temp->word);
        temp = temp->next;
        while (temp)
        {
            printf(", %s", temp->word);
            temp = temp->next;
        }
        printf("\n");
        pauseProgram();
    }
}

// Tampilkan semua kata dan sinonim
void printAllWords(void)
{
    // int order = 1;
    for (int i = 0; i < HASH_SIZE; i++)
    {
        WordEntry *entry = hashTable[i];
        while (entry)
        {
            printf("%s: ", entry->word);
            SynonymNode *syn = entry->synonyms;
            if(!syn)
            {
                printf("Synonym not found.\n");
            }
            else
            {
                printf("%s", syn->word);
                syn = syn->next;
                while (syn)
                {
                    printf(", %s", syn->word);
                    syn = syn->next;
                }
            }
            printf("\n");
            entry = entry->next;
        }
        // if(entry) printf("%d. %s\n", order++, entry->word);
    }
}

void printAllWordsTrieUtil(TrieNode *root, char *buffer, int depth)
{
    if(root->isEndOfWord)
    {
        buffer[depth] = '\0';
        printf("%s\n", buffer);
    }

    for(int i = 0; i < ALPHABET_SIZE; i++)
    {
        if(root->children[i]) 
        {
            buffer[depth] = 'a' + i;
            printAllWordsTrieUtil(root->children[i], buffer, depth + 1);
        }
    }
}

void printAllWordsTrie(TrieNode *root)
{
    if(!root) return;
    
    char buffer[31];

    for (int i = 0; i < ALPHABET_SIZE; i++)
    {
        if(root->children[i])
        {
            printf("\n---%c---\n", 'A' + i);
            buffer[0] = 'A' + i;
            printAllWordsTrieUtil(root->children[i], buffer, 1);    
        }
    }
}

// Simpan data ke file
void saveToFileSynonym(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("Failed to open file to save synonym.\n");
        return;
    }

    for (int i = 0; i < HASH_SIZE; i++)
    {
        WordEntry *entry = hashTable[i];
        while (entry)
        {
            fprintf(file, "%s :", entry->word);
            SynonymNode *syn = entry->synonyms;
            while (syn)
            {
                fprintf(file, " %s", syn->word);
                if (syn->next)
                    fprintf(file, ",");
                syn = syn->next;
            }
            fprintf(file, "\n");
            entry = entry->next;
        }
    }
    fclose(file);
}

// Free memory semua node
void freeMemorySynonym(void)
{
    for (int i = 0; i < HASH_SIZE; i++)
    {
        WordEntry *entry = hashTable[i];
        while (entry)
        {
            WordEntry *temp = entry;
            SynonymNode *syn = entry->synonyms;
            while (syn)
            {
                SynonymNode *tempSyn = syn;
                syn = syn->next;
                free(tempSyn->word);
                free(tempSyn);
            }
            entry = entry->next;
            free(temp->word);
            free(temp);
        }
    }
}

void freeTrieNode(TrieNode *root)
{
    if(root == NULL) return;
    for(int i = 0; i < ALPHABET_SIZE; i++)
        freeTrieNode(root->children[i]);
    free(root);
}

// Fungsi untuk load data dari file
void loadFromFileSynonym(const char *filename)
{
    const int condition = 0;
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("Failed to open file %s for synonym.\n", filename);
        return;
    }

    char line[1024];
    while (fgets(line, sizeof(line), file))
    {
        char *token;
        char *word = strtok(line, ":\n");
        if (word)
        {
            trim_and_tolower(word);
            addWord(word, condition);
            insertToTrie(rootTrie, word);

            token = strtok(NULL, "\n");
            if (token)
            {
                char *synonym = strtok(token, ",");
                while (synonym != NULL)
                {
                    while (*synonym == ' ')
                        synonym++;
                    addSynonym(word, synonym, condition);
                    synonym = strtok(NULL, ",");
                }
            }
        }
    }
    fclose(file);
}

// Main menu program
void menu(void)
{
    int choice;
    char word[100], synonym[100];
    const int condition = 1;

    do
    {
        printf("Menu:\n");
        printf("1. View all words and synonyms\n");
        printf("2. View all words entry\n");
        printf("3. Find synonyms words\n");
        printf("4. Add new entry word\n");
        printf("5. Add synonyms to entry words\n");
        printf("6. View history\n");
        printf("7. Save and exit\n");
        printf("Pilihan: ");
        scanf("%d", &choice);
        getchar(); // consume newline

        switch (choice)
        {
        case 1:
            printAllWords();
            pauseProgram();
            break;
        case 2:
            printAllWordsTrie(rootTrie);
            pauseProgram();
            break;
        case 3:
            printf("Masukkan kata: ");
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            trim_and_tolower(word);
            printSynonyms(word);
            break;
        case 4:
            printf("Masukkan kata baru: ");
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            trim_and_tolower(word);
            addWord(word, condition);
            break;
        case 5:
            printf("Masukkan kata: ");
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            trim_and_tolower(word);

            printf("Masukkan sinonim: ");
            fgets(synonym, sizeof(synonym), stdin);
            synonym[strcspn(synonym, "\n")] = 0;
            trim_and_tocapital(synonym);
            addSynonym(word, synonym, condition);
            break;
        case 6:
            printHistory();
            break;
        case 7:
            saveToFileSynonym(SYNONYM_FILE);
            saveToFileHistory(HISTORY_FILE);
            printf("Data disimpan.\n");
            break;
        default:
            printf("Pilihan tidak valid.\n");
        }
        sleep(1);
        clearScreen();
    } while (choice != 7);
}