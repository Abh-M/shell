//
//  Parser.c
//  myShell
//
//  Created by Abhineet on 21/01/13.

#include "Parser.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "Globals.h"
#include <string.h>

typedef enum{
    kPipe,
    kTTY,
    kFile,
}KFILE;



// check if '&' in the user input is valid.

BOOL isValidForBackgroundProcessing(char *kRawInput)
{
    BOOL isValid = true;

    
    char *next = kRawInput;
    while ((next = strstr(next, "&"))!=NULL) {
        
        //printf("\n %c",*next);
        //check if it is at the end
        if(*(next+1)=='\0')
        {
            
            if(*(next-2)=='|' || *(next-2)=='>' || *(next-2)=='<')
                return false;
            
            return true;
            
        }
        else
        {
            // not at end
            return false;
        }
        next = next + 1;
        
    }
    
    

    return isValid;
}

//function to display details of struct command
void commandToStr(struct command *cmd)
{
    //printf("\n Commnad Name : %s",cmd->cName);
    
    //printf("\n Arguments :");
    
//    int i=0;
//    for(i=0;i<cmd->numberOfArgs;i++)
//        printf(" %s ",cmd->args[i]);
    if(cmd->numberOfArgs>0)
    {
        
        char **temp = cmd->args;
        while (*temp!=NULL) {
            printf(" [%s] ",*temp);
            temp=temp+1;
        }
        
        
    }
    
        
        
        switch (cmd->inputSource) {
            case ktty:
                printf("\n INPUT : TERMINAL");
                break;
                
            case kfile:
                printf("\n INPUT FILENAME");
//                for(i=0;i<cmd->numberOfInputFiles;i++)
//                    printf(" [%s] ",cmd->inputFiles[i]);
                char **temp = cmd->inputFiles;
                while (*temp!=NULL) {
                    printf(" [%s] ",*temp);
                    temp=temp+1;
                }
                
                    
                    break;
                
                
            case kpipe:
                printf("\n INPUT FROM PIPE");
            default:
                break;
        }
    
    switch (cmd->outputDestination) {
        case ktty:
            printf("\n OUTPUT : TERMINAL");
            break;
            
        case kfile:
            printf("\n OUTPUT FILENAME");
//            for(i=0;i<cmd->numberOfOutputFiles;i++)
//                printf(" [%s] ",cmd->outputFiles[i]);
            
            char **temp = cmd->outputFiles;
            while (*temp!=NULL) {
                printf(" [%s] ",*temp);
                temp=temp+1;
            }
            
                
                break;
            
        case kpipe:
            printf("\n OUTUT TO PIPE");
        default:
            break;
    }
    
}




void shiftLeft(char *input)
{
    
    //char *cpy = strdup(input);
    char *n = input;
    while (*n!='\0') {
        *n = *(n+1);
        n++;
    }
}


//remove extra spaces from the input
void trim(char *input)
{
    //char *cpy = input;
    while (*input==' ') {
        shiftLeft(input);
    }
    while (*input!= '\0') {
        if((*input == ' ' && *(input+1)==' ') || (*input == ' ' && *(input+1)=='\0'))
            shiftLeft(input);
        else
            input++;
    }
    //printf("\n [%s]",cpy);
    
    
}



// remove '\n' and '\t' from input
char * removeEscapeSequences( char *kRawInput)
{
    
    int nextline = '\n';
//    int tab = '\t';
    char *pos = kRawInput;
    while ((pos=strstr(pos, "\\"))!=NULL) {
        if(*(pos+1)=='\n')
        {
            *pos= ' ';
        }
        pos = pos+1;
    }
    
    
    pos =kRawInput;
    while ((pos = strchr(pos, nextline))!=NULL)
    {
        pos[0] = ' ';
        pos = pos+1;
        
    }
    
    pos = kRawInput;
	while( (pos = strchr(pos,'\t'))!=NULL)
   	{
        *pos = ' ';
        pos = pos + 1;
    }
    return kRawInput;
    
}


//creat struct command from the user input.
//extract input/output files and set the structure variables

struct command * getCommand(char *kCommand,KFILE input, KFILE output, int kIndex)
{
    
    
    
    
    struct command *newCommand = (    struct command *) malloc(sizeof(    struct command ));
    
    
    
    //printf("\n iiii : %s",kCommand);
    int i=0;
    
    const char outputDelimiter[] =">";
    const char inputDelimiter[] ="<";
    const char paramsDelimiter[] =" ";
    const char appendDelimiter[] = ">>";
    
    
    char *commandCopy = strdup(kCommand);
    char **outputCheckResult = (char **)malloc(sizeof(char*));
    char *token = NULL;
    
    
    //
    //
    //    check for output '>'
    //
    
    
    //check for append delimiters
    char *location = strstr(commandCopy,appendDelimiter);
    if(location)
    {
        
        newCommand->isAppendMode = TRUE;
        //printf("\n found");
        //remove extra '>' sign
        while (*location!='\0') {
            //printf("\n %c",*location);
            *location = *(location+1);
            location = location+1;
        }
    }
    else
    {
        newCommand->isAppendMode = FALSE;
    }
    
    int outputFiles = 0;
    char *opResultCommand = NULL;
    while ((token = strsep(&commandCopy, outputDelimiter)) != NULL) {
        
        if(outputFiles == 0)
        {
            //firt one is command
            opResultCommand = token;
        }
        else
        {
            outputCheckResult = (char**)realloc(outputCheckResult, sizeof(char*)*outputFiles);
            trim(token);
            outputCheckResult[outputFiles-1]=token;
        }
        outputFiles++;
        
        
        
    }
    
    outputFiles--;
    
    if(outputFiles>0)
    {
        newCommand->outputDestination = kfile;
        newCommand->outputFiles = (char **)malloc(sizeof(char*)*(outputFiles+1));
        memset(newCommand->outputFiles,0,sizeof(char*)*(outputFiles+1));
        newCommand->numberOfOutputFiles = outputFiles;
        for(i=0;i<outputFiles;i++)
        {
            
            
            //printf("\n  COMMAND : %s | OUTPUT FILE: %s",opResultCommand,outputCheckResult[i]);
            *(newCommand->outputFiles+i) = outputCheckResult[i];
        }
        
        
        // set output files for the command
    }
    else if(output == kPipe)
    {
        //set to pipe
        newCommand->outputDestination = kpipe;
    }
    else if(output == kTTY)
    {
        //set output to console
        newCommand->outputDestination = ktty;
    }
    
    
    
    
    
    
    //
    //
    //    check for input '<'
    //
    char *commandCopyForInput = (opResultCommand==NULL)?strdup(kCommand):opResultCommand;
    char **inputCheckResult = (char **)malloc(sizeof(char*));
    token = NULL;
    
    int inputFiles = 0;
    char *ipResultCommand = NULL;
    while ((token = strsep(&commandCopyForInput, inputDelimiter)) != NULL) {
        
        if(inputFiles == 0)
        {
            //first one is command
            ipResultCommand = token;
        }
        else
        {
            inputCheckResult = (char**)realloc(inputCheckResult, sizeof(char*)*inputFiles);
            trim(token);
            inputCheckResult[inputFiles-1]=token;
            
            
        }
        inputFiles++;
        
        
        
    }
    
    inputFiles--;
    
    if(inputFiles>0)
    {
        
        //no input files in input for this command
        //        for(i=0;i<inputFiles;i++)
        //            printf("\n  COMMAND : %s | IN: %s",ipResultCommand,inputCheckResult[i]);
        
        
        newCommand->inputSource = kfile;
        newCommand->inputFiles = (char **)malloc(sizeof(char*)*(inputFiles+1));
        memset(newCommand->inputFiles, 0,sizeof(char*)*(inputFiles+1));
        newCommand->numberOfInputFiles = inputFiles;
        for(i=0;i<inputFiles;i++)
        {
            //printf("\n  COMMAND : %s | OUTPUT FILE: %s",ipResultCommand,inputCheckResult[i]);
            *(newCommand->inputFiles+i) = inputCheckResult[i];
        }
        
        
    }
    else if(input==kPipe)
    {
        //set input as pipe
        newCommand->inputSource = kpipe;
        
    }
    else if(input == kTTY)
    {
        //set input as console
        newCommand->inputSource = ktty;
        
    }
    
    
    
    
    
    
    
    
    //extract arguments for the commnad
    
    char *commandCopyForParams = (ipResultCommand==NULL)?strdup(opResultCommand):ipResultCommand;
    char **paramsCheckResult = (char **)malloc(sizeof(char*));
    token = NULL;
    
    int params = 0;
    char *paramsResultCommand = NULL;
    trim(commandCopyForParams);
    while ((token = strsep(&commandCopyForParams, paramsDelimiter)) != NULL) {
        
        if(params == 0)
        {
            //firt one is command
            paramsResultCommand = token;
        }
        params++;
        paramsCheckResult = (char**)realloc(paramsCheckResult, sizeof(char*)*params);
        trim(token);
        paramsCheckResult[params-1]=token;
        
        
    }
    
    //params--;
    
    if(params>0)
    {
        newCommand->args = (char**)malloc(sizeof(char*)*(params+1));
        memset(newCommand->args, 0, sizeof(char*)*(params+1));
        *(newCommand->args) = paramsResultCommand;
        newCommand->numberOfArgs = params;
        newCommand->cName = paramsResultCommand;
        for(i=0;i<params;i++)
        {
            //printf("  COMMAND : %s | PARAMS: %s",paramsResultCommand,paramsCheckResult[i]);
            *(newCommand->args+i)=paramsCheckResult[i];
        }
//        newCommand->args[i]=NULL;
        
        
    }
    else
    {
        
        newCommand->args = (char**)malloc(sizeof(char*)*(params+1));
        memset(newCommand->args,0, sizeof(char*)*(params+1));
        
        memcpy(newCommand->args, ipResultCommand, strlen(ipResultCommand));
        newCommand->numberOfArgs = 0;
        newCommand->cName = (char*)malloc(strlen(ipResultCommand));
        memset(newCommand->cName, 0, strlen(ipResultCommand));
        memcpy(newCommand->cName, (const char*)ipResultCommand, strlen(ipResultCommand));
        
    }
    
    
    
    
    
    
    //printf("%s",newCommand->cName);
    //printf("\n NEW COMMAND\n");
    //commandToStr(newCommand);
    newCommand->jobIndex = kIndex;
    
    return newCommand;
}


//get raw input and determine number of pipes required.

struct parserOutput getCommandsList(char *kRawInput)
{
    struct parserOutput result;

    const char *rawInput = (const char *)kRawInput;
    const char pipeDelimiter[] = "|";
    int totalPipesRequired = 0;
    char **allCommands = NULL;
    allCommands = (char **)malloc(sizeof(char*));
    int totalCommands = 0;
    char *copyOfInput = strdup(rawInput);
    char *token = NULL;
    while ((token = strsep(&copyOfInput, pipeDelimiter))!=NULL) {
        //printf("\n TOKEN PIPE : %s",token);
        totalCommands++;
        allCommands = realloc(allCommands,totalCommands*(sizeof(char*)));
        allCommands[totalCommands-1]=token;
        totalPipesRequired++;
    }
    totalPipesRequired--;
    
    //printf("\n TOTAL PIPES ; %d", totalPipesRequired);
    
    int i=0;
    struct command **allCommandsList = NULL;
    if(totalPipesRequired>0)
    {
        allCommandsList = (struct command**)malloc(sizeof(struct command*)*(totalCommands+1));
        memset(allCommandsList,0,sizeof(struct command*)*(totalCommands+1));
        for(i=0;i<totalCommands;i++)
        {
            
            //printf("\nCC: %s\n",allCommands[i]);
            trim(allCommands[i]);
            if(i==0)
                allCommandsList[i]=getCommand(allCommands[i], kTTY, kPipe,i);
            else if(i==totalCommands-1)
                allCommandsList[i]=getCommand(allCommands[i], kPipe, kTTY,i);
            else
                allCommandsList[i]=getCommand(allCommands[i], kPipe, kPipe,i);
            
        }

        //allCommandsList[i]=NULL;
        
        
    }
    else
    {
        totalCommands=1;
        trim(kRawInput);
        allCommandsList = (struct command**)malloc(sizeof(struct command*)*(totalCommands+1));
        memset(allCommandsList, 0,sizeof(struct command*)*(totalCommands+1));
        allCommandsList[0] =getCommand(kRawInput, kTTY, kTTY,0);
        //allCommandsList[1]=NULL;
    }
    
    
    
//print all collected commands
//    for(i=0;i<totalCommands;i++)
//    {
//        printf("\n>>> %s\n",allCommandsList[i]->cName);
//        commandToStr(allCommandsList[i]);
//    }
    
    
    result.allCommands = allCommandsList;
    result.totalPipesRequired = totalPipesRequired;
    
    return result;
    //return allCommandsList;
    
}
