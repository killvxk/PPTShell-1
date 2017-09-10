// CUnrarDLL.cpp : implementation file
//

#include "stdafx.h"
#include "CUnrarDLL.h"

/////////////////////////////////////////////////////////////////////////////
// CUnrarDLL class



CUnrarDLL::CUnrarDLL()
{
	// Load the DLL
	//LoadDLL();
}



CUnrarDLL::~CUnrarDLL()
{
	// Load the DLL
	//UnloadDLL();
}



// Open the DLL
//
bool CUnrarDLL::LoadDLL()
{
	// Attempt to load the DLL
	unrarDLL = LoadLibrary(_T("unrar.dll"));

	// It failed, update the error and return false
	if (unrarDLL == NULL)
	{
		lastError = "Load Error: unrar.dll could not be loaded";

		return false;
	}

	// Otherwise true
	return true;
}



// Unload the DLL
void CUnrarDLL::UnloadDLL()
{
	FreeLibrary(unrarDLL);
}


// Open a RAR file
bool CUnrarDLL::OpenRAR(char* rarTo)
{
	// Is the DLL loaded?
	//if (unrarDLL == NULL) {	return false; }

	// Set up our RAR archive data
	RAROpenArchiveDataEx archiveData;

	ZeroMemory(&archiveData, sizeof(archiveData));
	archiveData.ArcName	= rarTo;
	archiveData.CmtBuf = NULL;
	archiveData.OpenMode = 0;

	// Open the archive into a handle
	HANDLE archiveHandle = RAROpenArchiveEx(&archiveData);

	// Did it fail? Update the error and return false
	if (archiveData.OpenResult != 0)
	{
		lastError = "Open Error: Failed to open RAR file";

		return false;
	}

	char emptyBuf[16384] = {0};

	// Declare our file struct
	CUnrarFile curFile;

	// Set up the header data
	RARHeaderDataEx headerData;

	headerData.CmtBuf = emptyBuf;
	headerData.CmtBufSize = sizeof(emptyBuf);

	// Declare variables
	int readHeaderCode, processFileCode;

	// Empty the vector
	fileList.clear();

	// While we get a valid response to reading the header, continue
	while ((readHeaderCode = RARReadHeaderEx(archiveHandle, &headerData)) == 0)
	{
		// Get the file size
		__int64 unpackSize = headerData.UnpSize+(((__int64)headerData.UnpSizeHigh) <<32);
		__int64 packSize = headerData.PackSize+(((__int64)headerData.PackSizeHigh) <<32);

		// Get the filename from the header data
		curFile.fileName = headerData.FileName;

		// Set pack/unpacked sizes
		curFile.packSize = packSize;
		curFile.unpackSize = unpackSize;

		// Push back this file into the vector
		fileList.push_back(curFile);

		// Invalid file? Break, we're done
		if ((processFileCode = RARProcessFile(archiveHandle, 0, NULL, NULL)) != 0)
		{
			break;
		}
	}

	int totalNum = fileList.size();

	// Close the archive
	RARCloseArchive(archiveHandle);

	return true;
}

// List all files in a RAR
void CUnrarDLL::ListFileNames(CStringArray* outList)
{
	// Clear the array
	outList->RemoveAll();

	// Loop through the vector and get the filenames
	CUnrarFile curFile;

	for (size_t i = 0; i < fileList.size(); i++)
	{
		curFile = fileList.at(i);

		outList->Add(curFile.fileName);
	}
}



// UnRAR the archive to the specified location
bool CUnrarDLL::UnRARArchive()
{
	// Is the DLL loaded?
	//if (unrarDLL == NULL) {	return false; }

	// Set up our RAR archive data
	RAROpenArchiveDataEx archiveData;

	ZeroMemory(&archiveData, sizeof(archiveData));
	archiveData.ArcName	= currentRar.GetBuffer(currentRar.GetLength());
	archiveData.CmtBuf = NULL;
	archiveData.OpenMode = 1;

	// Open the archive into a handle
	HANDLE archiveHandle = RAROpenArchiveEx(&archiveData);

	// Did it fail? Update the error and return false
	if (archiveData.OpenResult != 0)
	{
		lastError = "Open Error: Failed to open RAR file";

		return false;
	}

	// Simple header data struct
	RARHeaderData headerData;

	headerData.CmtBuf = NULL;

	// Set variables
	int readHeaderCode, processFileCode;

	// While the header data is valid..
	while ((readHeaderCode = RARReadHeader(archiveHandle, &headerData)) == 0)
	{
		unrarDir.AnsiToOem();

		// UnRAR this file
		processFileCode = RARProcessFile(archiveHandle, 2, unrarDir.GetBuffer(unrarDir.GetLength()), NULL);

		// Did it go okay? If not, we're done
		if (processFileCode != 0)
		{
			break;
		}
	}

	// Close the archive
	RARCloseArchive(archiveHandle);

	return true;
}