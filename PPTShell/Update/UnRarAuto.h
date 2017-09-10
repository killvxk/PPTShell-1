#ifndef _UNRARAUTO_H_
#define _UNRARAUTO_H_

#include <windows.h>
#include <stdio.h>
#include <ctype.h>
#include "unrar/dll.hpp"

enum { EXTRACT, TEST, PRINT };

extern "C" __declspec(dllexport) void ExtractArchive(char *ArcName,int Mode,char *Password);
extern "C" __declspec(dllexport) int ListArchive(char *ArcName);
void ShowComment(char *CmtBuf);
extern "C" __declspec(dllexport)void OutHelp(void);
void OutOpenArchiveError(int Error,char *ArcName);
void ShowArcInfo(unsigned int Flags,char *ArcName);
void OutProcessFileError(int Error);
int CALLBACK CallbackProc(UINT msg,LONG UserData,LONG P1,LONG P2);

#endif //_UNRARAUTO_H_
