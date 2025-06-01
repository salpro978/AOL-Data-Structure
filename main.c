#include "dictionary.h"

int main()
{
    loadFromFile("daftarkata.txt");
    menu();
    freeMemory();
    return 0;
}