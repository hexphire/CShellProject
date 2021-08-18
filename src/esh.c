
//EggShell: A basic interactive shell

#include "../include/esh.h"




//Signal Handlers

//Sigchild handler, handles any signals coming from terminating or finished children and reaps them.

void sigChild_handler(int signal){
    pid_t cPid;
    int cStatus;
    while ((cPid = waitpid(-1, &cStatus, WNOHANG)) > 0 ){
        //printf("child Reaped");
    }
    return;
}

//function to use ANSI escape characters to clear the screen more POSIX  than using system calls like system(clear);
void clearPrompt(){                         
    const char* clr_ANSI = " \033[2J\033[1;1H";
    write(STDOUT_FILENO, clr_ANSI, 12);
}
void eshPrompt(){
    static bool initial = true;             //flag for signaling this is the inital prompt
    char *prompt = "esh:";
    
    if(initial){
        //clearPrompt();                    //clear screen
        initial = false;
    }

    
    printf("%s", prompt);                   //provide shell prompt
    fflush(stdout);
}
//eshGetInput: This function gets the users input (hopefully a command and arguements) and parses it
void eshGetInput(struct cmd *curCommand){
    char *inputLine = NULL;                 //buffer for getline
    char *inputTok = NULL;                  //buffer for strtok
    char *inputArray[MAX_PATH];                  //max 5 arguements and 1 command, 6 total 7 is null terminated.
    size_t inputLength = 0;
    ssize_t lineSize;
    int argCount = 0, i = 0;
    //read stdin
    lineSize = getline(&inputLine, &inputLength, stdin);
    if(lineSize < 0){
        return;
    }
    //printf("%s", inputLine);              TEST: test print for results of getline on STDIN.
    inputTok = strtok(inputLine, " \n");
    while ( inputTok != NULL){              //while we are still getting tokens
        inputArray[i++] = strdup(inputTok); //add the token to our array of input tokens.
        inputTok = strtok (NULL, " \n");    //grab the next token.
    }
    
    strcpy(curCommand->command, inputArray[0]);         //first token should be the command
    for(int j = 0; j < i; j++){             //the rest of the tokens are paramaters
        argCount++;
        curCommand->args[j] = inputArray[j];      //process command and arguments
    }
    if(strcmp(curCommand->args[i-1], "&") == 0){  //check for & and update flag.
        curCommand->backgPro = true;
        curCommand->args[i-1] = NULL;
        //printf("bg detected \n");         //TEST: declare if we found a & or not

    }else{
        curCommand->backgPro = false;
    }
    curCommand->args[i] = NULL;
    
    free(inputLine);
    free(inputTok);
}
//eshFindCmd: This function checks the command parsed by eshGetInput and checks it against what is stored in the enviroment PATH variable.
void eshFindCmd(struct cmd *curCommand){
    char *path = getenv("PATH");
    struct stat *sb;
    sb = malloc(sizeof(struct stat));
    char *pathToken = NULL;
    char *pathBuf;
    bool cmdExists = false;

    pathBuf = strdup(path);
    pathToken = strtok(pathBuf,":");
    while(pathToken != NULL){
        char tempPath[MAX_PATH];
        strcpy(tempPath, pathToken);
        strcat(pathToken, "\0");
        strcat(tempPath, "/"); 
        strcat(tempPath, curCommand->command);
        if(stat(tempPath, sb) != 0){       //check directories on PATH for command using stat
            //printf("command not found in %s \n", tempPath);// TEST: print path if cmd is not found
            cmdExists = false;
            pathToken = strtok(NULL, ":");
            
            continue;            
        }

        if(sb->st_mode & S_IXUSR){
            //printf("command exists! \n");// TEST: report that the command was found and is executable.
            cmdExists = true;              //if it exists update the flag
            strcpy(curCommand->fullCommand, tempPath);
            
            break;
        }
        
    }
    if(cmdExists == false){
            strcpy(curCommand->fullCommand, "./\0");
            strcat(curCommand->fullCommand, curCommand->command);
        }
    free(pathBuf);
    free(sb);
           
  return;  
}

int main(){                        
    pid_t child;
                //get PATH
    
    signal(SIGINT, SIG_IGN); //we exit the shell using "exit" so ignore ctrl-c
    signal(SIGCHLD, sigChild_handler); //Use our handler for sig child to handle when the child terminates 
    

    while(1){
        struct cmd *curCommand = (struct cmd *)malloc(sizeof(struct cmd));
        //printf("%s", cmd);                TEST: print cmd to see if it's correct after execution.
        
        eshPrompt();                        //clear screen and provide user with shell prompt.
        
        eshGetInput(curCommand);    //process command and arguement input.

        //printf("%s \n", cmd);             TEST: print for results of eshGetInput
        
        eshFindCmd(curCommand);    //check PATH for command.
      /*printf("%s\n", curCommand->command);          TEST: test prints verifying eshGetInput and eshFindCmd worked as intended.
        printf("%s\n", curCommand->fullCommand);   
        printf("%s\n", curCommand->args[0]); */        
        if(strcmp(curCommand->command,"exit") == 0){        //if "exit" close shell.
            break;
        }
        if(strcmp(curCommand->command,"") == 0){
            printf("Command not recognized, please try again\n");
        }
        //fflush(stdout);
        child = fork();
        if (child < 0){                     //attempt to fork and execute command.
            perror("error in fork");
        }else if (child == 0){
            signal(SIGINT, SIG_DFL); //we want the child to react to ctrl-C
            execv(curCommand->fullCommand, curCommand->args);
            exit(0);          //pass full command string and envp to execve.
        }else{
            if(curCommand->backgPro){                     //something weird is happening, child runs in background but parent gets funky.
                //printf("bg is true\n");
                //printf("child running in background. \n"); //TEST: we are running a background
                //printf("%d\n", child);
            }else{
                //printf("waiting for child \n");       TEST: print that fork worked and the parent is waiting for the child to terminate.
                wait(&child);
                
            }
        }
        
        //report back to user.
        
        free(curCommand);
    }
    return 0;
}