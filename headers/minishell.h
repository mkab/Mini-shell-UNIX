#include "vars_manager.h"
#define MAX 2048

void printline()
{
    int i = 0;
    for(i = 0; i < 80; i++) {
        printf("*");
    }
    printf("\n");
}


void welcomeScreen(void)
{
        printline();
        printf("\n");
        printf("\tWelcome to MINI-SHELL \n");
        puts("\tAuthors: ABDULSALAM Mohammad Kabir & SADIK Sofiane");
        printf("\n");
        printline();
        printf("\n\n");
}


void goodbye()
{
    puts("Quitting...");
    fflush(stdout);
    puts("Goodbye!!");
    exit(EXIT_SUCCESS);
}

void runPipedCommands(cmdLine* command, char* userInput) {
    int numPipes = countPipes(userInput);


    int status;
    int i = 0;
    pid_t pid;

    int pipefds[2*numPipes];

    for(i = 0; i < (numPipes); i++){
        if(pipe(pipefds + i*2) < 0) {
            perror("couldn't pipe");
            exit(EXIT_FAILURE);
        }
    }


    int j = 0;
    while(command) {
        pid = fork();
        if(pid == 0) {

            //if not last command
            if(command->next){
                if(dup2(pipefds[j + 1], 1) < 0){
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            //if not first command
            if(j != 0 ){
                if(dup2(pipefds[j-2], 0) < 0){
                    perror(" dup2");///j-2 0 j+1 1
                    exit(EXIT_FAILURE);

                }
            }

            //close pipes in child
            for(i = 0; i < 2*numPipes; i++){
                    close(pipefds[i]);
            }

            //execute commands
            if( execvp(*command->arguments, command->arguments) < 0 ){
                    perror(*command->arguments);
                    exit(EXIT_FAILURE);
            }
        } else if(pid < 0){
            perror("error");
            exit(EXIT_FAILURE);
        }

        command = command->next;//go to the next command in the linked list
        j+=2;//increment j
    }

    /**Parent closes the pipes and waits for children*/

    for(i = 0; i < 2 * numPipes; i++){
        close(pipefds[i]);
    }

    for(i = 0; i < numPipes + 1; i++)
        wait(&status);
}

/**Function to execute redirections.
Checks which type of redirection the user inputted to determine appropriate flags*/
void executeRedirection(cmdLine* command, char* userInput) {

    int files[2];
    pid_t pid;

    if((pid = fork()) == 0){
        // open files

        files[1]  = open(command->inputRedirect, O_RDONLY, 0666);

        if( (strstr(userInput, ">>")) ) {
            files[2] = open(command->outputRedirect, O_WRONLY | O_CREAT | O_APPEND, 0666);
        }

        else
            files[2] = open(command->outputRedirect, O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR, 0666);

        // replace stdin with file[1]
        dup2(files[1], 0);

        // replace stdout with file[2]
        dup2(files[2], 1);

       //execute commands
        if( execvp(*command->arguments, command->arguments) < 0 ){
                perror(*command->arguments);
                exit(EXIT_FAILURE);
        }
    }
    else{
        // close unused file descriptors
        close(files[1]);
        close(files[2]);

        while(waitpid(pid,0,0) <= 0);

    }
}


/**Function to handle Ctrl-C Signal*/
void handle_signal(int signum)
{
    printf("\n%s:%s$ -> ",getenv("USER"), getcwd(NULL, MAX) );
    //printf("\n");
	fflush(stdout);
}

/**Changes Directory*/
void changeDirectory(char* argv[]){

    const char* home = getenv("HOME");
    if (argv[1] == NULL){
        chdir(home); /**change directrory to home dorectory if no arguments*/
    }

    else if (chdir(argv[1]) == -1){
        printf(" %s: no such directory\n", argv[1]);
    }
}
////char* getCommandPath(char* command)
////{
////    FILE *fd;
////    char *path;
////    char cmdpath[MAX];
////    char* temppath = (char *) calloc(MAX, sizeof(char));
////
////    path = (char *) getenv("PATH");
////
////    strcpy(cmdpath, path);
////
////    path = strtok(cmdpath, ":");///find the firdt path
////    while(path != NULL)
////    {
////        strcpy(temppath, path);///save a copy of the path
////        strcat(temppath, "/");/// form a path with the filename
////        strcat(temppath, command);
////        if( (fd = fopen(temppath, "r")) == NULL){
////            temppath[0] = '\0';
////            path  = strtok('\0', ":"); //try next path
////        }
////        else
////            break;
////
////        }
////
////        if(fd == NULL)
////            return NULL;
////        else
////            return temppath;
////}
//
//char *copy_str(const char *s)
//{
//    char *copy = malloc(strlen(s) + 1);
//
//        if (copy != NULL)
//            strcpy(copy, s);
//
//        return copy;
//}
//
//
//char **split_str(const char *s, const char *delim)
//{
//
//    char **dst = malloc(sizeof *dst);
//    char *src = copy_str(s);
//    char *tok = strtok(src, delim);
//    size_t n = 0;
//
//    while (tok != NULL) {
//        //dst = realloc(dst, ++n * sizeof *dst);
//        dst = realloc(dst, ++n * sizeof *dst);
//        dst[n - 1] = copy_str(tok);
//        tok = strtok(NULL, delim);
//    }
//
//    dst[n] = NULL;
//    free(src);
//    return dst;
//}


/**Execute builtin and external commands*/
int executeCommands(cmdLine* command, char* userInput, variable* var){

    pid_t pid;

    char *cd, *quit, *unset;
    cd    = strndup(userInput, 2);
    quit  = strndup(userInput, 4);
    unset = strndup(userInput, 5);

    expandArguments(command);


    if(strcmp("exit", quit) == 0)/*if the user typed exit*/
    {
        free(cd);
        free(quit);
        free(unset);
        goodbye();
    }

    else if(strcmp("cd", cd) == 0) { /*if the user typed cd*/
        changeDirectory(command->arguments);
        free(cd);
        free(quit);
        free(unset);
    }

    else if(strcmp("unset", unset) == 0) { /*if the user typed unset*/
        int i = 0;
        while(command){
            for(i = 1; command->arguments[i] != NULL; i++){
                var = removeVariable(command->arguments[i],var);
            }
            command  = command->next;
        }
        free(cd);
        free(quit);
        free(unset);
    }

    else  if( strchr(userInput, '>') || strchr(userInput, '<') || strpbrk(userInput, ">>")){
        executeRedirection(command, userInput);
    }

    else{
        switch(pid = fork() ){
            case -1:
                    perror("fork");
                    exit(EXIT_FAILURE);
                    break;
            case 0: /**child process*/

                    if( execvp(*command->arguments, command->arguments) < 0 ){
                            perror(*command->arguments);
                            exit(EXIT_FAILURE);
                    }

                    break;
            default: /**parent process*/

                    if(command->background == 1) {
                        while(waitpid(pid,0,0) <= 0); //wait for the child process to terminate
                        command->background = 0;
                    }

                    break;
        }
    }

    return 0;
}


