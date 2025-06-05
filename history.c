#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "dictionary.h"

HistoryNode *historyTop = NULL;

void getCurrentTimestamp(char *buffer, size_t size) 
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(buffer, size, "%Y-%m-%d %H:%M:%S", t);
}

HistoryNode *createHistoryNode(const char *action)
{
    HistoryNode *newNode = (HistoryNode *)malloc(sizeof(HistoryNode));
    if(!newNode) return NULL;

    newNode->action = strdup(action);
    if(!newNode->action) {
        free(newNode);
        return NULL;
    }

    char timeBuffer[30];
    getCurrentTimestamp(timeBuffer, sizeof(timeBuffer));

    newNode->time = strdup(timeBuffer);
    if(!newNode->time)
    {
        free(newNode->action);
        free(newNode);
        return NULL;
    }

    newNode->next = NULL;
    return  newNode;
}

void pushHistory(const char *action)
{
    HistoryNode *newNode = createHistoryNode(action);
    if(!newNode) return;

    newNode->next = historyTop;
    historyTop = newNode;
}

void pushLoadedHistory(const char *time, const char *action)
{
    HistoryNode *newNode = (HistoryNode *)malloc(sizeof(HistoryNode));
    if(!newNode) 
    {
        perror("Memory allocation failed for HistoryNode (load).\n");
        return;
    }

    newNode->time = strdup(time);
    if(!newNode->time)
    {
        perror("Memory allocation failed for time (load)\n");
        return;
    }

    newNode->action = strdup(action);
    if(!newNode->action)
    {
        perror("Memory allocation failed for action (load)\n");
        return;
    }

    newNode->next = historyTop;
    historyTop = newNode;
}

void printHistory(void) 
{
    printf("\n==================================== Action History ====================================\n\n");
    HistoryNode *current = historyTop;

    if(!current) printf("Belum ada riwayat.\n");

    while(current)
    {
        printf("%-30s %s\n", current->time, current->action);
        current = current->next;
    }
}

void freeHistory(void)
{
    HistoryNode *current = historyTop;
    while(current)
    {
        HistoryNode *next = current->next;
        free(current->action);
        free(current->time);
        free(current);
        current = next;
    }
    historyTop = NULL;
}

void saveToFileHistory(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if(!file)
    {
        printf("Failed to open file to save history.\n");
        return;
    }

    HistoryNode *current = historyTop;
    while(current)
    {
        fprintf(file, "%s,%s\n", current->time, current->action);
        current = current->next;
    }

    fclose(file);
}

void loadFromFileHistory(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if(!file)
    {
        printf("Failed to open file %s for history.\n", filename);
        return;
    }

    char line[1024];
    while(fgets(line, sizeof(line), file))
    {
        char *token;
        char *historyTime = strtok(line, ",\n");
        char *historyAction = strtok(NULL, "\n");

        if(historyTime) 
            trim(historyTime);

        if(historyAction)
            trim(historyAction);

        if (historyTime && historyAction && strlen(historyAction) > 0 && strlen(historyTime) > 0) 
            pushLoadedHistory(historyTime, historyAction);
    }

    fclose(file);
}