#ifndef H_BFINTERPRETER
#define H_BFINTERPRETER

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MEMORY_SIZE 30000

typedef struct String
{
    char* string;
    int length;
} String;


enum Code
{
    ADD = 0,
    MV = 1, 
    COND_BEGIN = 2,
    COND_END = 3,
    UNKNOW = 4,
    PRINT = 5,
    ASK = 6,
    ZERO = 7,
    CELL_ADD = 8,
    CELL_CPY = 9
};

typedef struct Order
{
    char code;
    int argument;
} Order;

typedef struct Prgm
{
    struct Order* prgm;
    int size;
} Prgm;

void initPrgm(Prgm* prgm, String* source);
void destroyPrgm(Prgm* prgm);

typedef struct StackElement
{
    struct StackElement* next;
    int address;
} StackElement;

typedef struct Stack
{
    struct StackElement* first;
} Stack;

void addElement(Stack* stack, int address);
int getElement(Stack* stack);
void destroyStack(Stack* stack);

void BFInterpreter(String* code);
void interpreter(Prgm* prgm);
int motifDetector(const String motif, String* source, int indice);
void removeUnuseds(String* source);

#endif