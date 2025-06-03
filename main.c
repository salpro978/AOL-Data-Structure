#include "dictionary.h"
#include <stdio.h>

int main()
{
    rootTrie = createTrieNode();
    if(!rootTrie)
    {
        printf("Failed to allocate memory for root Trie.\n");
        // Consider exiting or handling this critical failure appropriately
        return 1;
    }

    loadFromFileSynonym(SYNONYM_FILE);
    loadFromFileHistory(HISTORY_FILE);
    menu();
    freeMemorySynonym();
    freeHistory();
    freeTrieNode(rootTrie);
    return 0;
}