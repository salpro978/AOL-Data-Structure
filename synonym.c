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
    printf("\nTekan Enter untuk melanjutkan...");
    while (getchar() != '\n');
}

void trim(char *word)
{
    if(word == NULL || *word == '\0') return;

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
    if(word == NULL || word[0] == '\0' || root == NULL) return;

    TrieNode *curr = root;
    char *wordCopy = NULL;

    wordCopy = strdup(word);
    if(wordCopy == NULL)
    {
        perror("Failed to allocate memory for word copy in insertToTrie");
        return;
    }

    trim_and_tolower(wordCopy);

    const char *currentChar = wordCopy;
    while(*currentChar)
    {
        int index = *currentChar - 'a';
        
        if(index < 0 || index >= ALPHABET_SIZE)
        {
            fprintf(stderr, "Warning: Character '%c' out of 'a'-'z' range in insertToTrie after tolower. Skipping.\n", *currentChar);
            currentChar++;
            continue;
        }
        if(curr->children[index] == NULL)
        {
            curr->children[index] = createTrieNode();
            if(!curr->children[index])
            {
                fprintf(stderr, "Failed to create TrieNode in insertToTrie.\n");
                free(wordCopy); // Clean up allocated copy
                return;
            }
        }
        curr = curr->children[index];
        currentChar++;
    }
    curr->isEndOfWord = 1;

    free(wordCopy);
    wordCopy = NULL;
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

    insertToTrie(rootTrie, word);

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

void deleteWordEntry(const char *word)
{
    if (word == NULL || word[0] == '\0') {
        printf("Error: Cannot delete an empty or NULL word.\n");
        return;
    }

    unsigned int index = hash(word);
    WordEntry *current = hashTable[index];
    WordEntry *prev = NULL;

    while(current != NULL && strcmp(current->word, word) != 0)
    {
        prev = current;
        current = current->next;
    }

    if(current == NULL)
    {
        printf("Word entry '%s' not found for deletion.\n", word);
        return;
    }

    if(prev == NULL)
    {
        hashTable[index] = current->next;
    } 
    else
    {
        prev->next = current->next;
    } 

    SynonymNode *syn = current->synonyms;
    while(syn != NULL)
    {
        SynonymNode *synTemp = syn;
        syn = syn->next;

        free(synTemp->word);
        synTemp->word = NULL;
        free(synTemp);
        synTemp = NULL;
    }
    current->synonyms = NULL;

    free(current->word);
    current->word = NULL;

    free(current);
    current = NULL;

    int len = snprintf(NULL, 0, "Delete %s from word entry", word);
    char *action = malloc(len + 1);
    if(action)
    {
        snprintf(action, len + 1, "Delete %s from word entry", word);
        pushHistory(action);
        free(action);
    }
}

void deleteSynonym(const char *word, const char *synonym)
{
    if (word == NULL || word[0] == '\0' || synonym == NULL || synonym[0] == '\0') {
        printf("Error: Word or synonym to delete cannot be empty or NULL.\n");
        return;
    }

    WordEntry *entry = findWord(word);
    if(!entry)
    {
        printf("Word entry '%s' not found.\n", word);
        return;
    }

    SynonymNode *currentSyn = entry->synonyms;
    SynonymNode *prevSyn = NULL;

    while (currentSyn != NULL && strcmp(currentSyn->word, synonym) != 0)
    {
        prevSyn = currentSyn;
        currentSyn = currentSyn->next;
    }
    
    if(currentSyn == NULL)
    {
        printf("Synonym '%s' not found for word '%s'.\n", synonym, word);
        return;
    }

    if(prevSyn == NULL) 
    {
        entry->synonyms = currentSyn->next;
    }
    else 
    {
        prevSyn->next = currentSyn->next;    
    }

    free(currentSyn->word);
    currentSyn->word = NULL;
    free(currentSyn);
    currentSyn = NULL;

    printf("Synonym '%s' deleted successfully from word '%s'.\n", synonym, word);

    int len = snprintf(NULL, 0, "Delete %s from %s", synonym, word);
    char *action = malloc(len + 1);
    if (action)
    {
        snprintf(action, len + 1, "Delete %s from %s", synonym, word);
        pushHistory(action);
        free(action);
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
            trim_and_tocapital(word);
            addWord(word, condition);

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
        printf("6. Delete entry word\n");
        printf("7. Delete synonym from entry words\n");
        printf("8. View history\n");
        printf("9. Save and exit\n");
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
            printf("Enter entry word: ");
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            trim_and_tocapital(word);
            printSynonyms(word);
            break;
        case 4:
            printf("Enter new entry: ");
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            trim_and_tocapital(word);
            addWord(word, condition);
            break;
        case 5:
            printf("Enter entry word: ");
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            trim_and_tocapital(word);

            printf("Enter new synoynm word: ");
            fgets(synonym, sizeof(synonym), stdin);
            synonym[strcspn(synonym, "\n")] = 0;
            trim_and_tocapital(synonym);
            addSynonym(word, synonym, condition);
            break;
        case 6:
            printf("Enter entry word for delete: ");
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            trim_and_tocapital(word);
            deleteWordEntry(word);
            break;
        case 7:
            printf("Enter entry word: ");
            fgets(word, sizeof(word) ,stdin);
            word[strcspn(word, "\n")] = 0;
            trim_and_tocapital(word); 

            printf("Enter synonym word to delete: ");
            fgets(synonym, sizeof(synonym), stdin);
            synonym[strcspn(synonym, "\n")] = 0;
            trim_and_tocapital(synonym);

            deleteSynonym(word, synonym);
            break;
        case 8:
            printHistory();
            pauseProgram();
            break;
        case 9:
            saveToFileSynonym(SYNONYM_FILE);
            saveToFileHistory(HISTORY_FILE);
            printf("Data disimpan.\n");
            break;
        default:
            printf("Pilihan tidak valid.\n");
        }
        sleep(1);
        clearScreen();
    } while (choice != 9);
}