//
//  PipeManager.h
//  myShell
//
//  Created by Abhineet on 25/01/13.

#ifndef myShell_PipeManager_h
#define myShell_PipeManager_h

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void printFd(int *arr,int tp);
void createPipes(int numberofPipes, int *pcon);
void closePipe(const int pipeIndex, int *pcontainer);
void closeReadEndOfPipe(const int pipeIndex, int *pcontainer);
void closeWriteEndOfPipe(const int pipeIndex, int *pcontainer);
int getReadEndOfPipeAtIndex(const int pipeIndex, int *pcontainer);
int getWriteEndOfPipeAtIndex(const int pipeIndex, int *pcontainer);
void printPipesContainer(const int kPipes, int *kPcontainer);
void closeAllPipes(int numberOfPipes, int *pcon);
void creatPipeAtIndex(int index, int *pcon);
void closeAllPrevPipes(int index, int *pcon);


#endif
