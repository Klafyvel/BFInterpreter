#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "BFInterpreter.h"

int main(int argc, char const *argv[])
{
    if(argc < 2)
        return 1;
    FILE* fichier = NULL;
    fichier = fopen(argv[1], "r");
    if(!fichier)
        return 1;
    fseek (fichier, 0, SEEK_END);
    int size=ftell (fichier);
    rewind(fichier);
    String code;
    code.length = size+1;
    code.string = NULL;
    code.string = malloc(sizeof(char)*code.length);
    if(code.string == NULL)
        return 1;
    char c=' ';
    int i=0;
    do
    {
        c = fgetc(fichier);
        code.string[i] = c;
        i++;
    }while (c != EOF && i < code.length);
    code.string[i-1] = '\0';
    BFInterpreter(&code);
    free(code.string);
    fclose(fichier);
    return 0;
}