#include<sys/types.h>
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<fcntl.h>
#include<signal.h>

#include "Parser.h"
#include "Globals.h"
#include "PipeManager.h"


const int exitval = 999;
const char *ccd = "cd";
const char *backgroundFlag = "&";
const char *homeenv = "HOME";
const char *homepath = "~";


int  execute(int totatPipes,int *pipesContainer,struct command **nextCommand, BOOL isBackground);


int execute(int totatPipes,int *pipesContainer,struct command **nextCommand, BOOL isBackground)
{
    
    
    int index =0;
    while (nextCommand[index]!=NULL)
    {
        //printf("\nWorking\n");
        //commandToStr(*nextCommand);
        struct command *ncommand = nextCommand[index];
        //printf("\n%s\n ",ncommand->cName);
        
        // if command is 'cd' no need to fork
        if(strcmp(ncommand->cName,ccd)==0)
        {
            char *arg = NULL;
            if(ncommand->numberOfArgs==1)
            {
                arg = getenv(homeenv);
                
            }
            else if( (ncommand->numberOfArgs==2) && (strcmp(homepath, ncommand->args[1])==0))
            {
                arg = getenv(homeenv);
            }
            else if( ncommand->numberOfArgs==2 && (strlen(ncommand->args[1])>0))
            {
                arg = ncommand->args[1];
            }
            if((chdir(arg))==-1)
                perror("Error chdir() : ");
            
            
        }
        else
        {
            fflush(stdout);
            
            
            int pid =fork();
            if(pid==-1)
            {
                perror("Error in fork");
            }
            if(pid==0)
            {
                
                
                //printf("\n inside child  1\n");
                
                int pipeForRead = NOT_USED;
                int pipeForWrite = NOT_USED;
                
                if(ncommand->inputSource == kpipe)
                {
                    //get pipe that will be used for reading
                    if(ncommand->jobIndex>0)
                        pipeForRead = ncommand->jobIndex-1;
                    
                }
                
                if(ncommand->outputDestination == kpipe)
                {
                    //get pipe that will be used for writing
                    pipeForWrite = ncommand->jobIndex;
                    
                }
                
                
                
                switch (ncommand->inputSource)
                {
                    case ktty:
                        
                        break;
                        
                    case kpipe:
                    {
                        int fd;
                        //dup read end of pipe to be used for reading
                        fd = getReadEndOfPipeAtIndex(pipeForRead, pipesContainer);
                        int res = dup2(fd,0);
                        if(res==-1)
                            perror("Error in settting stdin to read end of the pipe");
                        
                    }
                        break;
                        
                        
                    case kfile:
                    {
                        //open the file in read mode and dup
                        int infile = open(ncommand->inputFiles[0],O_RDONLY|O_CLOEXEC,0644);
                        if(infile>0)
                        {
                            int res = dup2(infile,0);
                            if(res==-1)
                                perror("Error in settting stdin to read end of the pipe");
                            
                        }
                        else
                        {
                            perror("Error opening file");
                        }
                        
                    }
                        break;
                        
                        
                    default:
                        break;
                }
                
                
                switch (ncommand->outputDestination)
                {
                    case ktty:
                        
                        break;
                        
                    case kpipe:
                    {
                        //dup write end of pipe used for writing
                        int fd = getWriteEndOfPipeAtIndex(pipeForWrite, pipesContainer);
                        int res = dup2(fd,1);
                        if(res==-1)
                            perror("Error redirecting stdout to the pipe");
                        
                    }
                        break;
                    case kfile:
                    {
                        
                        int flag = O_CLOEXEC | O_RDWR;
                        if(!ncommand->isAppendMode)
                            flag = flag | O_TRUNC | O_CREAT;
                        else
                            flag = flag | O_CREAT  | O_APPEND;
                        
                        int outfile = open(ncommand->outputFiles[0], flag, 0644);
                        if(outfile==-1)
                        {
                            perror("\n Error opening file\n");
                        }
                        else
                        {
                            int res = dup2(outfile,1);
                            if(res==-1)
                                perror("Error redirecting stdout to the file");
                        }
                        
                    }
                        break;
                        
                    default:
                        break;
                }
                
                
                execvp(ncommand->cName,ncommand->args);
                perror("Error ");
                exit(0);
                
            }
            //check if other commands are remaining or close the pipes
            else if(pid>0)
            {
                
                
                int status;
                int res=0;
                
                if(!isBackground)
                    waitpid(pid,&status,0);
                else
                    wait4(pid,&status,WNOHANG,NULL);
                
                
                
                //printf("\n %d ERROR NO",status);
                int pipeForRead = NOT_USED;
                int pipeForWrite = NOT_USED;
                
                if(ncommand->inputSource == kpipe)
                {
                    //get pipe that will be used for reading
                    if(ncommand->jobIndex>0)
                        pipeForRead = ncommand->jobIndex-1;
                    
                }
                
                if(ncommand->outputDestination == kpipe)
                {
                    //get pipe that will be used for writing
                    pipeForWrite = ncommand->jobIndex;
                    
                }
                
                
                
                if(ncommand->inputSource == kpipe && ncommand->outputDestination==kpipe)
                {
                    //close read end of pipe used for reading
                    int readEnd = getReadEndOfPipeAtIndex(pipeForRead, pipesContainer);
                    res = close(readEnd);
                    
                    int writeEnd = getWriteEndOfPipeAtIndex(pipeForWrite, pipesContainer);
                    res = close(writeEnd);
                    
                    
                }
                else if(ncommand->inputSource == ktty && ncommand->outputDestination==kpipe)
                {
                    //close write end of pipe used for writing
                    int writeEndOfPipeUsedForWriting = getWriteEndOfPipeAtIndex(pipeForWrite, pipesContainer);
                    res = close(writeEndOfPipeUsedForWriting);
                }
                else if(ncommand->inputSource == kpipe && ncommand->outputDestination == ktty)
                {
                    //close read end of pipe used for reading
                    int readEnd = getReadEndOfPipeAtIndex(pipeForRead, pipesContainer);
                    res = close(readEnd);
                    
                    
                    int writeEnd = getWriteEndOfPipeAtIndex(pipeForRead, pipesContainer);
                    res = close(writeEnd);
                    
                    
                }
                else if(ncommand->inputSource == kpipe && ncommand->outputDestination == kfile)
                {
                    
                    int readEnd = getReadEndOfPipeAtIndex(pipeForRead, pipesContainer);
                    res = close(readEnd);
                    
                    
                    int writeEnd = getWriteEndOfPipeAtIndex(pipeForRead, pipesContainer);
                    res = close(writeEnd);
                    
                    
                    
                }
                
                else if(ncommand->inputSource == kfile && ncommand->outputDestination == kpipe)
                {
                    int writeEndOfPipeUsedForWriting = getWriteEndOfPipeAtIndex(pipeForWrite, pipesContainer);
                    res = close(writeEndOfPipeUsedForWriting);
                    
                    
                }
                
                
                if(ncommand->jobIndex < totatPipes)
                    closeAllPrevPipes(ncommand->jobIndex, pipesContainer);
                
            }
            
            
            
        }
        index++;
        
    }
    
    //printf("\n returning from execute()  \n");
    return 0;
    
}


int main(int argc, char *argv[])
{
    
    
    char *prompt =(argc>1)?((strlen(argv[1])>0)?argv[1]:DEFAULT_PROMT):DEFAULT_PROMT;
    size_t inputSize =1024;
    size_t bytesRead;
    char *input = NULL;
    char *kCommand =NULL;
    struct command **allCommands = NULL;
    char *commandSeperator = ";";
    
    
    input = (char *)malloc(inputSize*sizeof(char)+1);
    while (1)
    {
        printf("\n%s:",prompt);
        bytesRead = getline(&input, &inputSize, stdin);
        
        
        if((feof(stdin)))
            exit(0);
        
        if(bytesRead>0 && strlen(input)>1)
        {
            
            
            int len = (int)strlen(input);
            
            
            kCommand = (char *)malloc(len*sizeof(char));
            memset(kCommand, 0, len);
            strcat(kCommand, input);
            
            // handle multiple lines of output
            if(input[len-2]==92)
            {
                while (1) {
                    printf("\n>");
                    bytesRead = getline(&input, &inputSize, stdin);
                    if(bytesRead>0)
                    {
                        
                        kCommand = realloc(kCommand, strlen(input)*sizeof(char));
                        strcat(kCommand, input);
                    }
                    if(input[strlen(input)-2]==92)
                        continue;
                    else
                        break;
                    
                }
                
            }
            
            // remove  '\n' and '\t' from the input
            kCommand= removeEscapeSequences(kCommand);
            
            // remove extraa whitespace
            trim(kCommand);
            
            
            //check for '&' correctness
            {
                char *p = NULL;
                if((p=strstr(kCommand, backgroundFlag))!=NULL)
                {
                    BOOL isValidBackgroundFlag = isValidForBackgroundProcessing(kCommand);
                    if(!isValidBackgroundFlag)
                    {
                        printf("\n Syntax error\n");
                        continue;
                    }
                }
            }
            
            //seperate by  ;
            char *token  = NULL;
            while ((token = strsep(&kCommand, commandSeperator))!=NULL)
            {
                
                
                
                token = removeEscapeSequences(token);
                trim(token);
                
                //if command is blank then continue
                if(strlen(token)<1) continue;
                
                
                // check if user input contains "quit" command
                if(strcmp(token, QUIT)==0)
                {
                    exit(0);
                }
                
                
                
                //check for background flag
                char *pos =strstr(token, backgroundFlag);
                int background = false;
                if(pos!=NULL)
                {
                    //printf("\n Background");
                    background = true;
                    *pos='\0';
                    
                }
                
                
                // parse the input and get array of structure pointer
                struct parserOutput poutput = getCommandsList(token);
                allCommands = poutput.allCommands;
                struct command **nextCommand = allCommands;
                
                int totatPipes = poutput.totalPipesRequired;
                int *pipesContainer = NULL;
                
                
                //create total pipes that will be required.
                if(totatPipes>0)
                {
                    pipesContainer = (int*)malloc(sizeof(int)*totatPipes*3);
                    createPipes(totatPipes,pipesContainer);
                    //printPipesContainer(totatPipes,pipesContainer);
                }
                
                //execute the command
                int ret = execute(totatPipes, pipesContainer, nextCommand,background);
                if(ret){}
                
                
                //for safety close all the pipes that were created.
                closeAllPipes(totatPipes, pipesContainer);
                //                wait4(-1,&status,WNOHANG,NULL);
                //                int status = 0;
                
                
                
            }
            
        }
        
        //check for zombies.
        int status;	
        wait4(-1,&status,WNOHANG,NULL);
        
        
    }
    
    
    return 0;
}

