//==============================================================================
// FileName:			common.h
//
// Desc:		 
//==============================================================================
#ifndef _COMMON_H_
#define _COMMON_H_
#include <sstream>

template<class Type>
std::string toString(const Type &t)
{
	std::string  strTemp;
	std::ostringstream temp;
	temp << t;
	strTemp = temp.str();
	return  strTemp;
}

#ifdef _DEBUG
#define DEBUG_PRINT//(format, ...) printf(format, __VA_ARGS__)
#else
#define DEBUG_PRINT
#endif

#define SET_CONSOLE_RED SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),FOREGROUND_INTENSITY | FOREGROUND_RED)
#define RESET_CONSOLE SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE)

//
// Logs
//
void logMsg(char* filename, int logtype, int logfolder, char *format, ...);

#ifdef _DEBUG
#define WRITE_LOG_EX(logtype, logfolder, format, ...)	{ printf(format, __VA_ARGS__);printf("\r\n"); logMsg(NULL, logtype, logfolder, format, __VA_ARGS__); }
#else
#define WRITE_LOG_EX(logtype, logfolder, format, ...)	{ logMsg(NULL, logtype, logfolder, format, __VA_ARGS__); }
#endif

#ifdef _DEBUG
#define WRITE_LOG_TO_FILE(filename, format, ...)		{ printf(format, __VA_ARGS__);printf("\r\n"); logMsg(filename, -1, -1, format, __VA_ARGS__); }
#else
#define WRITE_LOG_TO_FILE(filename, format, ...)		{ logMsg(filename, -1, -1, format, __VA_ARGS__); }
#endif

#define WRITE_LOG_SERVER(format, ...)					WRITE_LOG_EX(AT_ONCE, LOG_FOLDER_LOG_SERVER, format, __VA_ARGS__)
#define WRITE_LOG_TEMP(format, ...)						WRITE_LOG_EX(AT_ONCE, LOG_FOLDER_LOG_TEMP, format, __VA_ARGS__)

#define  WRITE_LOG_GLOBAL(format, ...) {printf(format, __VA_ARGS__);printf("\r\n"); logMsg(NULL, AT_ONCE, LOG_FOLDER_LOG_SERVER, format, __VA_ARGS__);}

#endif


