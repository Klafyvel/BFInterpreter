#include "BFInterpreter.h"

void addElement(Stack* stack, int address)
{
    StackElement* new_element = malloc(sizeof(StackElement));
    new_element->next = stack->first;
    new_element->address = address;
    stack->first = new_element;
}
void destroyStack(Stack* stack)
{
    struct StackElement* ptr = stack->first;
    while(ptr != NULL)
    {
        struct StackElement* next = ptr->next;
        free(ptr);
        ptr = next;
    }

}
int getElement(Stack* stack)
{
    if (stack->first ==NULL)
        return -1;
    StackElement* second = stack->first->next;
    int address = stack->first->address;
    free(stack->first);
    stack->first = second;
    return address;
}
void remove_unuseds(String* source)
{
    String* tmp = malloc(sizeof(String));
    tmp->string = malloc(sizeof(char) * source->length);
    tmp->length = 0;
    int i;
    for(i=0; i<source->length; i++)
    {
        if(source->string[i] == '+' || source->string[i] == '-' \
           || source->string[i] == '>' || source->string[i] == '<' \
           || source->string[i] == '[' || source->string[i] == ']' \
           || source->string[i] == '.' || source->string[i] == ',')
        {
            tmp->length ++;
            tmp->string[tmp->length-1] = source->string[i];
        }
    }
    for (i = tmp->length; i < source->length; ++i)
        tmp->string[i] = 0;

    free(source->string);
    source->length = tmp->length;
    source->string = malloc(sizeof(char)*tmp->length);
    for(i=0; i<tmp->length; i++)
        source->string[i]=tmp->string[i];
    free(tmp->string);
    free(tmp);
}
void destroyPrgm(Prgm* prgm)
{
    free(prgm->prgm);
}
int motif_detector(const String motif, String* source, int indice)
{
    int i;
    if(motif.length + indice > source->length)
        return 0;
    for(i=indice; i<motif.length+indice; i++)
    {
        if(motif.string[i-indice]!=source->string[i])
            return 0;
    }
    return 1;
}
void initPrgm(Prgm* prgm, String* source)
{

    const String MOTIF_CELL_ADD_1 = {"[->+<]", 6};
    const String MOTIF_CELL_ADD_2 = {"[>+<-]", 6};
    const String MOTIF_CELL_CPY = {">[-]<[->+<]", 11};
    const String MOTIF_ZERO = {"[-]", 3};
    const String MOTIF_CELL_DUPL = {"[->+>+<<]", 9};

    int i;
    String source_without_comments;
    source_without_comments.string = malloc(sizeof(char)*source->length);
    strcpy(source_without_comments.string, source->string);
    source_without_comments.length = source->length;
    remove_unuseds(&source_without_comments);

    prgm->size = source_without_comments.length;
    prgm->prgm = malloc(sizeof(Order)*prgm->size);
    for (i = 0; i < prgm->size; ++i)
    {
        prgm->prgm[i].argument = 0;
        prgm->prgm[i].code = UNKNOW;
    }

    Stack stack;
    stack.first=NULL;

    int prgm_cursor = 0;
    char prev = UNKNOW;
    int jump_next = 0;
    for(i=0; i<prgm->size; i++)
    {
        if(jump_next)
        {
            jump_next --;
        }
        else
        {
            char cmd = 0;
            int argument = 1;
            if(motif_detector(MOTIF_ZERO, &source_without_comments, i))
            {
                cmd = ZERO;jump_next = MOTIF_ZERO.length-1;
            }
            else if(motif_detector(MOTIF_CELL_CPY, &source_without_comments, i))
            {
                cmd = CELL_CPY;
                jump_next = MOTIF_CELL_CPY.length-1;
            }
            else if(motif_detector(MOTIF_CELL_ADD_1, &source_without_comments, i) \
                    || motif_detector(MOTIF_CELL_ADD_2, &source_without_comments, i))
            {
                cmd = CELL_ADD;
                jump_next = MOTIF_CELL_ADD_1.length-1;
            }
            else if(motif_detector(MOTIF_CELL_DUPL, &source_without_comments, i))
            {
                cmd = CELL_DUPL;
                jump_next = MOTIF_CELL_DUPL.length-1;
            }
            else if(source_without_comments.string[i] == '>')
            {
                cmd = MV;
                if(prev==MV){prgm_cursor--;}
            }
            else if(source_without_comments.string[i] == '<')
            {
                cmd = MV;
                argument=-1;
                if(prev==MV){prgm_cursor--;}
            }
            else if(source_without_comments.string[i] == '+')
            {
                cmd = ADD;
                if(prev==ADD){prgm_cursor--;}
            }
            else if(source_without_comments.string[i] == '-')
            {
                cmd = ADD;argument=-1;
                if(prev==ADD){prgm_cursor--;}
            }
            else if(source_without_comments.string[i] == '[')
            {
                cmd = COND_BEGIN;
                addElement(&stack, prgm_cursor);
            }
            else if(source_without_comments.string[i] == ']')
            {
                int begin = getElement(&stack);
                if (begin == -1)
                {
                    printf("UNBALLANCED ] on char %d",i);
                    exit(1);
                }
                prgm->prgm[begin].argument = prgm_cursor;
                argument = begin;
                cmd = COND_END;
            }
            else if(source_without_comments.string[i] == '.')
            {
                cmd = PRINT;
                if(prev==PRINT) {prgm_cursor--;}
            }
            else if(source_without_comments.string[i] == ',')
            {
                cmd = ASK;
                if(prev==ASK) {prgm_cursor--;}
            }
            else
            {
                cmd = UNKNOW;
                if(prev==UNKNOW) {prgm_cursor--;}
            }
            prgm->prgm[prgm_cursor].code = cmd;
            prgm->prgm[prgm_cursor].argument += argument;
            prev = cmd;

            prgm_cursor ++;
        }
    }
    Order* tmp = malloc(sizeof(Order) * (prgm_cursor+1));
    for (i = 0; i < prgm_cursor+1; ++i)
        tmp[i] = prgm->prgm[i];
    free(prgm->prgm);
    prgm->prgm = malloc(sizeof(Order) * (prgm_cursor+1));
    for (i = 0; i < prgm_cursor+1; ++i)
        prgm->prgm[i] = tmp[i];
    prgm->size = prgm_cursor;
    free(tmp);
    free(source_without_comments.string);
}
void BFInterpreter(String *code)
{
    Prgm prgm;
    initPrgm(&prgm, code);
    interpreter(&prgm);
    destroyPrgm(&prgm);
}
void interpreter(Prgm *prgm)
{
    printPrgm(prgm);
    unsigned int memory_cursor = 0;
    int prgm_cursor = -1;
    unsigned char memory[MEMORY_SIZE] = {0};
    int size = prgm->size;
    int i=0;
    do
    {
        prgm_cursor ++;
        Order o = prgm->prgm[prgm_cursor];
#ifdef DEBUG
		printf("i = %d", memory_cursor);
		printOrder(&o);
#endif
        if (o.code == MV) {
            memory_cursor = (memory_cursor + o.argument)%MEMORY_SIZE;
			printMemory(memory, MEMORY_SIZE);
        }
        else if(o.code == ADD) {
            memory[memory_cursor] = (memory[memory_cursor] + o.argument) % 255;
			printMemory(memory, MEMORY_SIZE);
        }
        else if (o.code == COND_BEGIN && memory[memory_cursor]==0) {
            prgm_cursor = o.argument;
        }
        else if (o.code == COND_END && memory[memory_cursor]!=0) {
            prgm_cursor = o.argument;
        }
        else if (o.code == ZERO) {
            memory[memory_cursor]=0;
			printMemory(memory, MEMORY_SIZE);
        }
        else if (o.code == CELL_CPY)
        {
            memory[(memory_cursor+1)%MEMORY_SIZE] = memory[memory_cursor];
            memory[memory_cursor] = 0;
			printMemory(memory, MEMORY_SIZE);
        }
        else if (o.code == CELL_ADD)
        {
            memory[(memory_cursor+1)%MEMORY_SIZE] = (memory[memory_cursor] \
                                + memory[(1+memory_cursor)%MEMORY_SIZE])%255;
            memory[memory_cursor] = 0;
			printMemory(memory, MEMORY_SIZE);
        }
        else if (o.code == PRINT)
        {
            for(i=0; i<o.argument; i++)
                printf("%c", memory[memory_cursor]);
        }
        else if (o.code == ASK)
        {
            for(i=0; i<o.argument; i++)
                memory[memory_cursor]=getchar();
			printMemory(memory, MEMORY_SIZE);
        }
        else if (o.code == CELL_DUPL)
        {
            memory[(memory_cursor+1)%MEMORY_SIZE] += memory[memory_cursor];
            memory[(memory_cursor+2)%MEMORY_SIZE] += memory[memory_cursor];
            memory[memory_cursor] = 0;
			printMemory(memory, MEMORY_SIZE);
        }
    }while (prgm_cursor<(size-1));
    printf("\n");
}

void printPrgm(Prgm* prgm)
{
	#ifndef DEBUG
	return;
	#endif
    int i;
    for(i=0;i<prgm->size;i++)
    {
        printf("%d :\t", i);
		printOrder(& (prgm->prgm[i]));
    }
}

void printOrder(Order* order)
{
        switch(order->code)
        {
            case ADD: printf("ADD "); break;
            case MV: printf("MV "); break;
            case COND_BEGIN: printf("COND_BEGIN "); break;
            case COND_END: printf("COND_END "); break;
            case UNKNOW: printf("UNKNOW "); break;
            case PRINT: printf("PRINT "); break;
            case ASK: printf("ASK "); break;
            case ZERO: printf("ZERO "); break;
            case CELL_ADD: printf("CELL_ADD "); break;
            case CELL_CPY: printf("CELL_CPY "); break;
            case CELL_DUPL: printf("CELL_DUPL"); break;
            default:break;
        }
        printf("\t\t%d\n", order->argument);
}

void printMemory(unsigned char *memory, int max_value)
{
	#ifndef DEBUG
	return;
	#endif
    for(int i=0; i<max_value; i++)
        printf("|%d\t", memory[i]);
    printf("\n");
}
