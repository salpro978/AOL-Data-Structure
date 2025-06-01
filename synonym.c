#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include "dictionary.h"

UndoAction *undoStack = NULL;
HistoryNode *historyFront = NULL;
HistoryNode *historyRear = NULL;

WordEntry* hashTable[HASH_SIZE] = {0};

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

// Tambah kata baru ke hash table
void addWord(const char *word)
{
    if (findWord(word)){
        printf("Entry word '%s' already exists.\n", word);
        sleep(0.7);
        return; // Jangan tambah kalau sudah ada
    }

    unsigned int index = hash(word);
    WordEntry *newEntry = createWordEntry(word);

    newEntry->next = hashTable[index];
    hashTable[index] = newEntry;
}

// Tambah sinonim untuk sebuah kata
void addSynonym(const char *word, const char *synonym)
{
    WordEntry *entry = findWord(word);
    if (!entry)
    {
        addWord(word);
        entry = findWord(word);
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

// Tampilkan semua kata dan sinonim
void printAllWords(void)
{
    int order = 1;
    for (int i = 0; i < HASH_SIZE; i++)
    {
        WordEntry *entry = hashTable[i];
        while (entry)
        {
            printf("%s: ", entry->word);
            SynonymNode *syn = entry->synonyms;
            printf("%s", syn->word);
            syn = syn->next;
            while (syn)
            {
                printf(", %s", syn->word);
                syn = syn->next;
            }
            printf("\n");

            entry = entry->next;
        }
        // if(entry) printf("%d. %s\n", order++, entry->word);
    }
}

// Simpan data ke file
void saveToFile(const char *filename)
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
void freeMemory(void)
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

// Fungsi untuk load data dari file
void loadFromFile(const char *filename)
{
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
            addWord(word);
            token = strtok(NULL, "\n");
            if (token)
            {
                char *synonym = strtok(token, ",");
                while (synonym != NULL)
                {
                    while (*synonym == ' ')
                        synonym++;
                    addSynonym(word, synonym);
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
    do
    {
        printf("Menu:\n");
        printf("1. Lihat semua kata dan sinonim\n");
        // printf("1. Lihat semua daftar sinonim\n");
        printf("2. Cari sinonim kata\n");
        printf("3. Tambah kata baru\n");
        printf("4. Tambah sinonim ke kata\n");
        printf("5. Simpan dan keluar\n");
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
            printf("Masukkan kata: ");
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            trim_and_tolower(word);
            printf("%u\n", hash(word));
            printSynonyms(word);
            break;
        case 3:
            printf("Masukkan kata baru: ");
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            trim_and_tolower(word);
            addWord(word);
            break;
        case 4:
            printf("Masukkan kata: ");
            fgets(word, sizeof(word), stdin);
            word[strcspn(word, "\n")] = 0;
            trim_and_tolower(word);

            printf("Masukkan sinonim: ");
            fgets(synonym, sizeof(synonym), stdin);
            synonym[strcspn(synonym, "\n")] = 0;
            trim_and_tocapital(synonym);
            addSynonym(word, synonym);
            break;
        case 5:
            saveToFile("daftarkata.txt");
            printf("Data disimpan.\n");
            break;
        default:
            printf("Pilihan tidak valid.\n");
        }
        sleep(1);
        clearScreen();
    } while (choice != 5);
}