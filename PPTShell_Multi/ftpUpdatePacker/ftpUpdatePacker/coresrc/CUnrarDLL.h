// CUnrarDLL.h : header file
//

#ifndef _C_UNRARDLL_H
#define _C_UNRARDLL_H
//#include "dll.hpp"
//#include <cstring>
using namespace std;
#include "dll.hpp"
#pragma comment(lib, "NEWRAR.lib")



//struct RARHeaderData
//{
//  char         ArcName[260];
//  char         FileName[260];
//  unsigned int Flags;
//  unsigned int PackSize;
//  unsigned int UnpSize;
//  unsigned int HostOS;
//  unsigned int FileCRC;
//  unsigned int FileTime;
//  unsigned int UnpVer;
//  unsigned int Method;
//  unsigned int FileAttr;
//  char         *CmtBuf;
//  unsigned int CmtBufSize;
//  unsigned int CmtSize;
//  unsigned int CmtState;
//};
//
//struct RARHeaderDataEx
//{
//  char         ArcName[1024];
//  wchar_t      ArcNameW[1024];
//  char         FileName[1024];
//  wchar_t      FileNameW[1024];
//  unsigned int Flags;
//  unsigned int PackSize;
//  unsigned int PackSizeHigh;
//  unsigned int UnpSize;
//  unsigned int UnpSizeHigh;
//  unsigned int HostOS;
//  unsigned int FileCRC;
//  unsigned int FileTime;
//  unsigned int UnpVer;
//  unsigned int Method;
//  unsigned int FileAttr;
//  char         *CmtBuf;
//  unsigned int CmtBufSize;
//  unsigned int CmtSize;
//  unsigned int CmtState;
//  unsigned int Reserved[1024];
//};
//
//struct RAROpenArchiveData
//{
//  char         *ArcName;
//  unsigned int OpenMode;
//  unsigned int OpenResult;
//  char         *CmtBuf;
//  unsigned int CmtBufSize;
//  unsigned int CmtSize;
//  unsigned int CmtState;
//};
//
//struct RAROpenArchiveDataEx
//{
//  char         *ArcName;
//  wchar_t      *ArcNameW;
//  unsigned int OpenMode;
//  unsigned int OpenResult;
//  char         *CmtBuf;
//  unsigned int CmtBufSize;
//  unsigned int CmtSize;
//  unsigned int CmtState;
//  unsigned int Flags;
//  unsigned int Reserved[32];
//};

/////////////////////////////////////////////////////////////////////////////
// CUnrarDLL definitions

#include <vector>
using namespace std;

struct CUnrarFile {
	CString fileName;
	__int64 packSize;
	__int64 unpackSize;
};

class CUnrarDLL
{
// Construction
public:
	CUnrarDLL();   // Constructor
	~CUnrarDLL();  // Deconstructor

	// Load the DLL, automatically called in the constructor
	bool LoadDLL();

	// Unload the DLL, automatically called in the destructor
	void UnloadDLL();

	// Set the unrar directory
	void SetOutputDirectory(LPCSTR outDir) { unrarDir = outDir; }

	// Get the unrar directory
	CString GetOutputDirectory() { return unrarDir; }

	// Open a RAR file into the class
	bool OpenRARFile(LPCSTR rarPath)
	{ 
		currentRar = rarPath; 

		return OpenRAR(currentRar.GetBuffer(currentRar.GetLength()));
	}

	// Get the opened RAR path
	CString GetCurrentRAR() { return currentRar; }

	// Get a list of all the files in the current RAR in a CStringArray
	void ListFileNames(CStringArray* outList);

	// Get a file at a specified location
	CUnrarFile GetFileAt(int filePos) { return fileList.at(filePos); }

	// Get the number of files
	int GetNumberOfFiles() { return fileList.size(); }

	// UnRAR the archive
	bool UnRARArchive();

	// Current error
	CString lastError;

// Implementation
protected:
	// Current information
	CString unrarDir;
	CString currentRar;

	// Open RAR
	bool OpenRAR(char* rarTo);

	// The DLL
	HINSTANCE unrarDLL;

	// Files
	vector<CUnrarFile> fileList;

	// Function pointers for accessing RAR information
	//HANDLE (WINAPI *RAROpenArchiveEx)(RAROpenArchiveDataEx *pArchiveData);
	//int    (WINAPI *CloseArchive)(HANDLE hArcData);
	//int    (WINAPI *ReadRARHeader)(HANDLE hArcData, RARHeaderData *pHeaderData);
	//int    (WINAPI *ProcessRARFile)(HANDLE hArcData, int iOperation, char* strDestFolder, char* strDestName);
	//int    (WINAPI *ReadRARHeaderEx)(HANDLE hArcData,struct RARHeaderDataEx *HeaderData);

	//HANDLE	RAROpenArchiveEx(RAROpenArchiveDataEx *pArchiveData);
	//int		RARCloseArchive(HANDLE hArcData);
	//int		ReadRARHeader(HANDLE hArcData, RARHeaderData *pHeaderData);
	//int		RARProcessFile(HANDLE hArcData, int iOperation, char* strDestFolder, char* strDestName);
	//int		RARReadHeaderEx(HANDLE hArcData,struct RARHeaderDataEx *HeaderData);
};

#endif
