/**********************************************
Parses input gotten from the command line into
separate tokens
**********************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#ifndef NULL
    #define NULL 0
#endif

#define FREE(X) if(X) free((void*)X)
#define MAX_ARGUMENTS 256

typedef struct noeud
{

    /** command line arguments */
    char*  arguments[MAX_ARGUMENTS];
    /** number of arguments */
    int argCount;
    /** input redirection path. NULL if not redirected */
    char const* inputRedirect;
    /** output redirection path. NULL if not redirected */
    char const* outputRedirect;
    /** boolean fo background indication: 0 if ampersand has been typed, else 1*/
    int background;
    /** next cmdLine in chain */
    struct noeud* next;

}cmdLine;


char* copyWord(char* str)
{
    char* begin = NULL;
    char* end = NULL;
    char* word;

    while (!end) {
        switch (*str) {
            case '>':
            case '<':
            case'=':
            case 0:
                end = str - 1;
                break;
            case ' ':
                if (begin)
                    end = str - 1;
                break;
            default:
                if (!begin)
                    begin = str;
                break;
        }
        str++;
    }

    if (begin == NULL)
        return NULL;

    word = (char*) malloc(end-begin+2);
    strncpy(word, begin, ((int)(end-begin)+1)) ;
    word[ (int)((end-begin)+1)] = 0;

    return word;
}

void extractRedirections(char* strLine, cmdLine *command)
{
    char* str = strLine;

    while ( (str = strpbrk(str,"<>")) ) {
        if (*str == '<') {
            FREE(command->inputRedirect);
            command->inputRedirect = copyWord(str+1);
        }
        else {
            FREE(command->outputRedirect);
            command->outputRedirect = copyWord(str+1);
        }

        *(str++) = 0;
    }
}

char* strClone(const char* source)
{
    char* clone = (char*)malloc(strlen(source) + 1);
    strcpy(clone, source);
    return clone;
}

int isEmpty(const char* str)
{
    if (!str)
    return 1;

    while (*str)
    if (!isspace(*(str++)))
      return 0;

    return 1;
}

cmdLine* parseSingleCmdLine(const char* strLine)
{
    char* delimiter = " ";
    char *line, *result;

    if (isEmpty(strLine))
      return NULL;

    cmdLine* command = (cmdLine*)malloc( sizeof(cmdLine) ) ;
    memset(command, 0, sizeof(cmdLine));

    line = strClone(strLine);

    extractRedirections(line, command);

    result = strtok( line, delimiter);
    while( result && command->argCount < (MAX_ARGUMENTS - 1) ) {
        ((char**)command->arguments)[command->argCount] = strClone(result);
        command->argCount++;
        result = strtok ( NULL, delimiter);
    }

    FREE(line);
    return command;
}

cmdLine* parseCmdLines(char* line)
{
    char *nextStrCmd;
    cmdLine *command;
    char pipeDelimiter = '|';

    if (isEmpty(line))
      return NULL;

    nextStrCmd = strchr(line , pipeDelimiter);
    if (nextStrCmd)
      *nextStrCmd = 0;

    command = parseSingleCmdLine(line);
    if (!command)
      return NULL;

    if (nextStrCmd)
      command->next = parseCmdLines(nextStrCmd+1);

    return command;
}

cmdLine* parseCmd(const char* strLine)
{
    char* line, *ampersand;
    cmdLine *head, *last;

    if (isEmpty(strLine))
      return NULL;

    line = strClone(strLine);
    if (line[strlen(line)-1] == '\n')
      line[strlen(line)-1] = 0;

    ampersand = strchr( line,  '&');
    if (ampersand)
      *(ampersand) = 0;

    if ( (last = head = parseCmdLines(line)) )
    {
      while (last->next)
        last = last->next;
        last->background = ampersand? 0:1;
    }

    FREE(line);
    return head;
}


void freeCmdLines(cmdLine* command)
{
    int i;
    if (!command)
    return;

    FREE(command->inputRedirect);
    FREE(command->outputRedirect);
    for (i = 0; i > command->argCount; i++)
      FREE(command->arguments[i]);

    if (command->next)
      freeCmdLines(command->next);

    FREE(command);
}

void showCommand(cmdLine* command) {
    int i = 0;
    while(command != NULL) {
        while(command->arguments[i])
            printf("command = %s", command->arguments[i++]);

       command = command->next;
    }
    puts("");
}
