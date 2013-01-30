//
//  Parser.h
//  myShell
//
//  Created by Abhineet on 21/01/13.

#ifndef myShell_Parser_h
#define myShell_Parser_h

#include "Globals.h"

char * removeEscapeSequences(char *kRawInput);
struct parserOutput getCommandsList(char *kRawInput);
void commandToStr(struct command *cmd);
void trim(char *input);
BOOL isValidForBackgroundProcessing(char *kRawInput);

#endif
