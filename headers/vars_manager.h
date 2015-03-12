#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#include "parser.h"
#include "utility.h"

#define MAX_NAME 512
#define MAX_VALUE 2048

typedef struct node
{
  char name[MAX_NAME];
  char value[MAX_VALUE];
  struct node *next;
} variable;

variable* getVariable(const char* name, variable* var)
{
    if (name == NULL)
        return NULL;

    while (var != NULL) {
        if (strcmp(name, var->name) == 0)
          break;

        var = var->next;
    }

    return var;
}
/** Finds a variable by its name. Returns NULL if not found */
int findVariable(const char* name, variable* var)
{
    if (name == NULL)
        return 0;

    while (var != NULL) {
        if (strcmp(name, var->name) == 0)
          return 1;

        var = var->next;
    }

    return 0;
}

/** Removes variables indicated by name */
variable* removeVariable(const char* name, variable* var)
{
    variable* tmp;

    if (var == NULL)
        return NULL;

    if (!name || (*name == '\0') )
        return var;

    if (strcmp(var->name,name) == 0) {
        tmp = var->next;
        free(var);
        return tmp;
    }

    var->next = removeVariable(name, var->next);
    return var;
}

/** Adds a new variable with given name and value */
variable* addVariable(const char* name, const char* value, variable* var)
{
    if (name == NULL || value == NULL)
        return var;

    if (var == NULL)
    {
        variable* newVariable = (variable*) malloc( sizeof(variable) );
        strcpy(newVariable->name, name);
        strcpy(newVariable->value, value);
        newVariable->next = NULL;
        var = newVariable;
    }
    else
        var->next = addVariable(name, value, var->next);

  return var;
}


/** Releases all memory that was allocated for all the variables in the structure */
void freeEnvironment(variable* var)
{
    if (var == NULL)
        return;
    freeEnvironment(var->next);
    free(var);
}


/**Checks the userInput to determine if the user wants to create a variable*/
int check_syntax(char* userInput) {
    if(strstr(userInput, "=") != NULL && strstr(userInput, " = ") == NULL){
        return 0;
    }
    return 1;
}


void expandVariables(cmdLine* command, variable *var){
    int i = 0;

    while(command){
        for(i = 0; command->arguments[i] != NULL; i++){
            char *str = command->arguments[i];

            if(*str == '"' || *str == '\'' || *str == '$'){
                if(*str == '"'){
                    int j = 0;
                    for(j = 1; *(str+j) != '"'; j++){
                        *(str+j-1) = *(str+j);
                    }
                    str[j-1] = '\0';
                }

                else if(*str == '\''){
                    int j = 0;
                    for(j = 1; *(str+j) != '\''; j++){
                        *(str+j-1) = *(str+j);
                    }
                    str[j-1] = '\0';
                }


                ///check if the string starts with a $ sign so as to expand the variable
                if(*str == '$'){
                    //check if it starts with a $ and make sure the result isn't NULL
                    char *tmp = getenv(str + 1);
                    if (tmp != NULL) {
                        command->arguments[i] = getenv(str + 1); // save off the argument
                    }
                    else{
                        char* string = command->arguments[i];
                        variable* temp = getVariable(string + 1, var);
                        if(temp){
                            command->arguments[i] = temp->value;
                        }
                        else
                            command->arguments[i]="";
                    }
                }
            }
        }
        command = command->next;
    }
}


/** Replaces some shell expansions with their equivalueent */
void expandArguments(cmdLine *command) {
    //puts("expanding varibles");
    int i = 0;
    if(command->arguments == '\0')
        return;

    for(i = 0; command->arguments[i] != NULL; i++){
        if( strcmp(command->arguments[i], "~") == 0 && strcmp(command->arguments[i], "~+") != 0 && strcmp(command->arguments[i], "~-") != 0 )
            command->arguments[i] = replaceCharacter(command->arguments[i], "~", getenv("HOME"));
        else if( strcmp(command->arguments[i], "~+") == 0 && strcmp(command->arguments[i], "~") != 0 && strcmp(command->arguments[i], "~-") != 0 )
            command->arguments[i] = replaceCharacter(command->arguments[i], "~+", getenv("PWD"));
//        else if( strcmp(command->arguments[i], "~") != 0 && strcmp(command->arguments[i], "~-") != 0 && strcmp(command->arguments[i], "~-") == 0 )
//            command->arguments[i] = replaceCharacter(command->arguments[i], "~+", getenv("OLDPWD"));
    }

}


void executeVariables(char* userInput, variable* var){

    char name[MAX_NAME];
    char val[MAX_VALUE];


    sscanf(userInput, "%[^=]%s", name, val);
    int find = findVariable(name, var);
    if(getenv(name) == NULL){
        if(find == 0){
        var = addVariable(name, val+1, var);
        }

        else{
            while (var != NULL) {
                if (strcmp(name, var->name) == 0){
                    strcpy(var->value, (val + 1));
                    break;
                }
                var = var->next;
            }
        }
    }

}
