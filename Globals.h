//
//  Globals.h
//  myShell
//
//  Created by Abhineet on 21/01/13.

#ifndef myShell_Globals_h
#define myShell_Globals_h


#define BOOL int
#define TRUE 1
#define FALSE 0
#define YES FALSE
#define NO FALSE


#define  ktty 0
#define  kpipe 1
#define  kfile 2




#define NOT_USED  -9999
#define true TRUE
#define false FALSE



#define QUIT "exit"
#define DEFAULT_PROMT "myshell"

struct  command{
    
    
    int jobIndex;
	
    char *cName;
	char **args;
    int numberOfArgs;
    
	int outputDestination;
    int inputSource;
	
    
    int numberOfOutputFiles;
    char **outputFiles;
    
    int numberOfInputFiles;
    char **inputFiles;
    
    
    
	char *inputFile;
	char *outputFile;
    
    
    int isAppendMode;
    int isDeamon;
};

struct parserOutput
{
    struct command **allCommands;
    int totalPipesRequired;
};


#endif
