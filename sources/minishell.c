#include <ctype.h>

/**ajout des bibliotheques pour l'historique*/
#include <readline/readline.h>
#include <readline/history.h>

#include "minishell.h"

#define TRUE 1

/* Strip whitespace from the start and end of STRING.  Return a pointer
   into STRING. */
char* trimspace(char *str) {

    while( isspace(*(str++)) ); // Trim leading space

    if( (*str) == '\0' ){
        return str;
    }

    int len = strlen(str) - 1;

    while ( isspace(*(str +len) )){
        len--;
    }

    *(str + len + 1) = '\0';

    return (str-1);
}



int main(int argc, char* argv[]) {

    char *input, *cleanInput, userInput[MAX];
    cmdLine* command = (cmdLine*)malloc(sizeof(cmdLine));
    variable* var = (variable*) malloc(sizeof(variable));
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    sa.sa_flags = SA_RESTART;  /* Restart function incase it's interrupted by handler */
    welcomeScreen();
    while(TRUE)
    {
        snprintf(userInput, sizeof(userInput), "%s:%s$ -> ",getenv("USER"), getcwd(NULL, MAX) );


        input = readline(userInput);
        if (!input)
            break;

        //autocomplete path when tabulation is hit
        rl_bind_key('\t', rl_complete);

        /** Remove leading and trailing whitespace from the line. */
        cleanInput = trimspace(input);

        /**Add it to the history list. */
        if (*input || *cleanInput)
            //adding the input into history
            add_history(cleanInput);

        command = parseCmd(cleanInput);

        if(command != NULL){
            //runPipedCommands(command, cleanInput);
            if(!check_syntax(cleanInput)){
                executeVariables(cleanInput, var);
            }

            else{
                if(strchr(cleanInput, '|') != NULL)
                    runPipedCommands(command, cleanInput);

                else{
                    expandArguments(command);
                    expandVariables(command, var);
                    executeCommands(command, cleanInput, var);
                }

                freeCmdLines(command);

            }
        }

    }

    puts("");
    goodbye();

    return 0;
}



//void runPipedCommands(cmdLine* command, char* userInput) {
//    int numPipes = 2 * countPipes(userInput);
//    int status;
//    int i = 0, j = 0;
//
//    //printf("number of pipes = %d\n",numPipes/2);
//    int pipefds[numPipes];
//
//    pipe(pipefds);
//    //pipe(pipefds + (numPipes/2));
//    for(i = 2; i < (numPipes); i += 2)
//        pipe(pipefds + i);
//
//    while(command != NULL) {
//        switch(fork()) {
//
//            case -1: perror("fork");
//                     exit(EXIT_FAILURE);
//                     break;
//            case 0:
//                if(j != 0){
//                    if(dup2(pipefds[j - 2], 0) != -1) puts(" dup");///j-2 0 j+1 1
//                    //printf("j != 0  dup(pipefd[%d], 0])\n", j-2);
//                }
//
//                if(command->next != NULL){
//                    //printf("il ya un next: dup(pipefd[%d], 1])\n", j+1);
//                    if(dup2(pipefds[j + 1], 1) != -1) puts(" dup");
//                }
//
//
//                //close all unused pipes
//                for(i = 0; i < (numPipes); i++){
//                    close(pipefds[i]);
//                    //printf("close pipe in fils[%d]\n", i);
//                    //puts("all pipes closed in child");
//                }
//                           // puts("doing command next------------------------");
//                //showCommand(command);
//                if( execvp(*command->arguments, command->arguments) < 0 ){
//                    perror(*command->arguments);
//                    exit(EXIT_FAILURE);
//                }
//                //command = command->next;
//
//                break;
//
//                default:
//
//                    if(command != NULL){
//                        command = command->next;
//                        puts("did command next");}
//
//                    j += 2;
//                    //printf("j = %d\n",j);
//
//                    for(i = 0; i < (numPipes ); i++){
//                        close(pipefds[i]);
//                        //puts("all pipes closed in parent");
//                        //printf("parent: close pipe[%d]\n", i);
//                    }
//
//                    while(waitpid(0,0,0) <= 0);
//        }
//    }
//
//}
