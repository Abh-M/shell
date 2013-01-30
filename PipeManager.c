//
//  PipeManager.c
//  myShell
//
//  Created by Abhineet on 24/01/13.



//Helper methods for managing pipes


#include "PipeManager.h"

void printFd(int *arr,int tp)
{
	int i=0;
	for(i=0;i<tp*2;i++)
	{
     //printf("\n %d",arr[i]);   
    }
}

void createPipes(int numberofPipes, int *pcon)
{
    
	//close end
	int i=0;
	for(i=0;i<numberofPipes;i++)
	{
		pcon[i*3]=i;
        int res=0;
		res = pipe(&pcon[i*3+1]);
        if(res==-1)
            perror("Error in pipe creation");
	}
}


void creatPipeAtIndex(int index, int *pcon)
{
    pcon[index*3]=index;
    int res = 0;
    res = pipe(&pcon[index*3+1]);
    if(res==-1)
        perror("Error in pipe creation");
    
}

void closeAllPipes(int numberOfPipes, int *pcon)
{
    int arrayIndex = 0;
    for (arrayIndex=0; arrayIndex<numberOfPipes; arrayIndex++) {
        int pipeIndex = arrayIndex*3;
        close(pcon[pipeIndex+1]);
        close(pcon[pipeIndex+2]);
    }
}

void closePipe(const int pipeIndex, int *pcontainer)
{
    
	//close read end of pipe
	int arrayIndex = pipeIndex * 3;
	close(pcontainer[arrayIndex+1]);
	close(pcontainer[arrayIndex+2]);
    
}

void closeReadEndOfPipe(const int pipeIndex, int *pcontainer)
{
	//close write end of particular pipe
	int arrayIndex = pipeIndex * 3;
	close(pcontainer[arrayIndex+1]);
}

void closeWriteEndOfPipe(const int pipeIndex, int *pcontainer)
{
    
	int arrayIndex = pipeIndex * 3;
	close(pcontainer[arrayIndex+2]);
}


int getReadEndOfPipeAtIndex(const int pipeIndex, int *pcontainer)
{
	int arrayIndex = pipeIndex * 3;
	return (pcontainer[arrayIndex+1]);
    
    
}

int getWriteEndOfPipeAtIndex(const int pipeIndex, int *pcontainer)
{
	int arrayIndex = pipeIndex * 3;
	return (pcontainer[arrayIndex+2]);
    
    
}

void printPipesContainer(const int kPipes, int *kPcontainer)
{
	int index = 0;
    
	for(index=0; index < kPipes*3; index++)
	{
		printf("\n %d  ",kPcontainer[index]);
	}
}

void closeAllPrevPipes(int index, int *pcon)
{
    
    //printf("\n CLOSING 2 %d\n",index);
    
    int arrayIndex = 0;
    for (arrayIndex=0; arrayIndex<=index; arrayIndex++)
    {
        int pipeIndex = arrayIndex*3;
        //printf("\n CLosing : %d ",arrayIndex);
        close(pcon[pipeIndex+2]);
    }
    
}

