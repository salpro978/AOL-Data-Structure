#include "dictionary.h"

int main()
{
    loadFromFileSynonym(SYNONYM_FILE);
    loadFromFileHistory(HISTORY_FILE);
    menu();
    freeMemorySynonym();
    return 0;
}