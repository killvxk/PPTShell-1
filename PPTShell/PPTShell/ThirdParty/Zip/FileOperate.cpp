/*
 */

#include "stdafx.h"
#include "FileOperate.h"

#include <assert.h>
#include <string>
#include "ZipWrapTools.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define PATH_SEPARATOR_CHAR '\\'
#define PATH_SEPARATOR_STR "\\"

bool CFileOperate::GetModuleFilePath(char* modulefilepath, int length, HMODULE hModule)
{
	assert(modulefilepath != NULL);
	if ( NULL == modulefilepath || length <= 0 )
	{
		return false;
	}
	memset( modulefilepath, '\0', length );
	DWORD modulefileallpath_length = 0;
	char modulefileallpath[MAX_PATH + 1] = {0};

	modulefileallpath_length = ::GetModuleFileNameA(hModule, modulefileallpath, sizeof(modulefileallpath) - 1);
	if (0 == modulefileallpath_length)
	{
		return false;
	}
	return ParseFilePath(modulefileallpath, modulefilepath, length);
}
bool CFileOperate::GetModuleFileName(char* modulefilename, int length, HMODULE hModule)
{
	assert(modulefilename != NULL);
	if ( NULL == modulefilename || length <= 0 )
	{
		return false;
	}
	DWORD modulefileallpath_length = 0;
	char modulefileallpath[MAX_PATH + 1] = {0};

	modulefileallpath_length = ::GetModuleFileNameA(hModule, modulefileallpath, sizeof(modulefileallpath) - 1);
	if (0 == modulefileallpath_length)
	{
		return false;
	}
	return ParseFileName(modulefileallpath, modulefilename, length);
}
bool CFileOperate::ParseFilePath(const char* fileallpath, char* filepath, int length)
{
	assert(fileallpath != NULL);
	assert(filepath != NULL);

	if (length <= 0 || NULL == fileallpath || NULL == filepath)
	{
		return false;
	}
	char buffer[MAX_PATH+1] = {0};
	_snprintf(buffer, sizeof(buffer) - 1, "%s", fileallpath);
	CheckFilePathVoid(buffer);
	
	memset( filepath, '\0', length );

	int lastbackslashindex = -1;
	//解析出文件目录路径
	for (int i = 0; buffer[i] != '\0'; ++i)
	{
		if (buffer[i] == '\\' || buffer[i] == '/')
		{
			lastbackslashindex = i;
		}		
	}	
	if (-1 == lastbackslashindex)
	{
		return true;
	}
	if (lastbackslashindex + 1 > length) // filepath char count include null char
	{
		return false;
	}
	memcpy(filepath, buffer, lastbackslashindex);
	return true;
}
bool CFileOperate::ParseFileName(const char* filepath, char* filename, int length)
{
	assert(filepath != NULL);
	assert(filename != NULL);
	if ( length <= 0 || NULL == filepath || NULL == filename )
	{
		return false;
	}
	char buffer[MAX_PATH+1] = {0};
	_snprintf(buffer, sizeof(buffer) - 1, "%s", filepath);
	CheckFilePathVoid(buffer);

	int last_backslash_index = -1;
	int filepath_length = 0;
	memset( filename, '\0', length );
	for (int i = 0; buffer[i] != '\0'; ++i)
	{
		if ( '\\' == buffer[i] || '/' == buffer[i])
		{
			last_backslash_index = i;
		}
		++filepath_length;
	}
	if (-1 == last_backslash_index)
	{
		if (filepath_length + 1 > length)
		{
			return false;
		}
		memcpy(filename, buffer, filepath_length);
		return true;
	}
	if (filepath_length - last_backslash_index > length) // filename char count include null character  
	{
		return false;
	}
	memcpy(filename, &buffer[last_backslash_index + 1], filepath_length - last_backslash_index - 1);
	return true;
}

bool CFileOperate::ParsePostfix( const char* filepath, char* postfix, int length )
{
	assert(filepath != NULL);
	assert(postfix != NULL);
	if ( length <= 0 || NULL == filepath || NULL == postfix)
	{
		return false;
	}
	memset( postfix, '\0', length );
	char filename[500] = {0};
	if ( !ParseFileName(filepath, filename, sizeof(filename)) )
	{
		return false;
	}
	int lastindex = -1;
	int filename_length = 0;
	for (int i = 0; filename[i] != '\0'; ++i)
	{
		if (filename[i] == '.')
		{
			lastindex = i;
		}
		++filename_length;
	}
	if (-1 == lastindex)
	{
		return true;
	}
	if (filename_length - lastindex > length)
	{
		return false;
	}
	memcpy(postfix, &filename[lastindex + 1], filename_length - lastindex - 1);
	return true;
}
bool CFileOperate::ParseFileNameNoPostfix(const char* filepath, char* filenamenopostfix, int length)
{
	assert(filepath != NULL);
	assert(filenamenopostfix != NULL);

	if (length <= 0 || NULL == filepath || NULL == filenamenopostfix)
	{
		return false;
	}
	memset(filenamenopostfix, '\0', length);
	char filename[500] = {0};
	if ( !ParseFileName(filepath, filename, sizeof(filename)) )
	{
		return false;
	}
	int lastindex = -1;
	int filename_length = 0;

	for (int i = 0; filename[i] != '\0'; ++i)
	{
		if (filename[i] == '.')
		{
			lastindex = i;
		}
		++filename_length;
	}
	if (-1 == lastindex)
	{
		if (length <= filename_length)
		{
			return false;
		}
		memcpy(filenamenopostfix, filename, filename_length);
		return true;
	}
	if (length <= lastindex + 1)
	{
		return false;
	}
	memcpy( filenamenopostfix, filename, lastindex );
	return true;
}

bool CFileOperate::CheckFileIsExist(const char* filePath)
{
	if (NULL == filePath)
	{
		return false;
	}
	DWORD dwfileFlag = 0;
	dwfileFlag = ::GetFileAttributesA(filePath);
	if (0xFFFFFFFF == dwfileFlag)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CFileOperate::CreateMultiFolder(const char *pDirPath)
{
	if (NULL == pDirPath) return false;
	std::string strDirPath, parentDirPath;
	strDirPath = CheckFilePathReturnString(pDirPath);

	E_FILE_TYPE fileType = E_FILE_TYPE_NOT_EXIST;
	fileType = CheckFileAttribute(pDirPath);
	if ( E_FILE_TYPE_NOT_EXIST == fileType )//目录不存在创建目录
	{
		std::string::size_type Pos = std::string::npos;
		Pos = strDirPath.rfind(PATH_SEPARATOR_CHAR, std::string::npos);
		if ( std::string::npos == Pos )
		{
			if (TRUE == ::CreateDirectoryA(strDirPath.c_str(), NULL))
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			parentDirPath = strDirPath.substr(0, Pos);
			if (true == CreateMultiFolder(parentDirPath.c_str()))
			{
				if (TRUE == ::CreateDirectoryA(strDirPath.c_str(), NULL))
				{
					return true;
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
	}
	else if (E_FILE_TYPE_DIRECTORY == fileType)//目录存在
	{
		return true;
	}
	else{
		return false; // 该文件存在，且不是目录
	}
}

E_FILE_TYPE CFileOperate::CheckFileAttribute(const char *pFilePath)
{
	assert( pFilePath != NULL );
	if (NULL == pFilePath)
	{
		return E_FILE_TYPE_NOT_EXIST;
	}
	DWORD dwfileFlag = 0;
	dwfileFlag = ::GetFileAttributesA(pFilePath);
	if (0xFFFFFFFF == dwfileFlag)
	{
		return E_FILE_TYPE_NOT_EXIST;
	}
	else
	{
		if (dwfileFlag & FILE_ATTRIBUTE_DIRECTORY) 
		{
			return E_FILE_TYPE_DIRECTORY;
		}
		else
		{
			return E_FILE_TYPE_NORMAL_FILE; 
		}
	}
}

bool CFileOperate::DeleteFolder(const char *pFolder)
{
	assert( pFolder != NULL );
	if ( NULL == pFolder ){ return false; }
	if ( !CleanFolder(pFolder) ){ return false; }
	if (FALSE == ::RemoveDirectoryA(pFolder) ){ return false; }
	return true;
}

unsigned long CFileOperate::GetFileSize(const char *pFile, unsigned long *High)
{
	if ( NULL == pFile )
	{
		return 0;
	}
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	hFind = ::FindFirstFileA(pFile, &FindFileData);
	if ( INVALID_HANDLE_VALUE == hFind )
	{
		return 0;
	}
	::FindClose(hFind);
	if ( NULL != High )
	{
		*High = FindFileData.nFileSizeHigh;
	}
	return FindFileData.nFileSizeLow;
}

bool CFileOperate::GetAboslutePath(const char* filepath, char *aboslutepath, int lenght)
{
	if( lenght <= 0 || NULL == filepath || NULL == aboslutepath) return false;
	bool flag = (NULL == strstr(filepath, ":")) ? false : true;
	if ( !flag )
	{
		DWORD dw = ::GetFullPathNameA(filepath, lenght - 1, aboslutepath, NULL);
		if ( dw > DWORD(lenght - 1) )
		{
			return false;
		}
	}
	else
	{
		_snprintf(aboslutepath, lenght, "%s", filepath);
	}
	return true;
}

std::string CFileOperate::CheckFilePathReturnString(const char *filepath)
{
	std::string student_filepath;
	student_filepath = filepath;

	StringTrimLeft(student_filepath, "\\");
	StringTrimLeft(student_filepath, "\\");
	StringTrimRight(student_filepath, "/");
	StringTrimRight(student_filepath, "/");

	std::string::iterator src = student_filepath.begin();
	std::string::iterator des = student_filepath.begin();

	for ( ; src != student_filepath.end(); )
	{
		if ( '\\' == *src  || '/' == *src )
		{
			*des++ = PATH_SEPARATOR_CHAR;
			src++;
			for ( ; src != student_filepath.end(); )
			{
				if ( '\\' == *src  || '/' == *src )
				{
					src++;
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			*des++ = *src++;
		}
	}
	if (student_filepath.at(student_filepath.length() - 1) == '\\')
	{
		student_filepath.erase(--student_filepath.end());
	}
	return student_filepath;
}

void CFileOperate::CheckFilePathVoid( char *filepath )
{
	if(NULL == filepath) return;
	const char* src = filepath;
	char *dst = filepath;
	while(*src)
	{
		if( '\\' == *src  || '/' == *src )
		{
			*dst++ = PATH_SEPARATOR_CHAR;
			src++;
			while(*src)
			{
				if ( '\\' == *src  || '/' == *src )
				{
					src++;
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			*dst++ = *src++;
		}
	}
	size_t path_size = strlen(filepath);
	if ( path_size > 0 )
	{
		if ( filepath[path_size - 1] == PATH_SEPARATOR_CHAR )
		{
			filepath[path_size - 1] = '\0';
			path_size = path_size - 1;
		}
	}
	if ( filepath[0] == PATH_SEPARATOR_CHAR )
	{
		memmove(filepath, filepath + 1, path_size);
	}
}

bool CFileOperate::CleanFolder(const char *pFolder)
{
	assert( pFolder != NULL );
	if ( NULL == pFolder ){ return false; }

	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char DirSpec[MAX_PATH] = {0};  // directory specification

	DWORD dwError = 0;
	_snprintf(DirSpec, sizeof(DirSpec) - 1, "%s\\*", pFolder );

	hFind = FindFirstFileA(DirSpec, &FindFileData);

	if ( INVALID_HANDLE_VALUE == hFind ) 
	{
		return false;
	}
	bool bFlag = true;
	do
	{
		if (0 != strcmp(".", FindFileData.cFileName) && 0 != strcmp("..", FindFileData.cFileName))
		{

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char szNextDir[MAX_PATH] = {0};
				_snprintf(szNextDir, sizeof(szNextDir) - 1, "%s\\%s", pFolder, FindFileData.cFileName);
				if (false == CleanFolder(szNextDir))
				{ 
					bFlag = false; break; 
				}
				else
				{
					if (FALSE == ::RemoveDirectoryA(szNextDir))
					{ 
						bFlag = false; break;
					}
				}
			}
			else
			{
				char szFile[MAX_PATH] = {0};
				_snprintf(szFile, sizeof(szFile) - 1, "%s\\%s", pFolder, FindFileData.cFileName);
				if (FALSE == ::DeleteFileA(szFile))
				{ bFlag = false; break; }
			}
		}
	}
	while ( FindNextFileA(hFind, &FindFileData) != 0 );
	dwError = ::GetLastError();
	FindClose(hFind);
	if ( false == bFlag ) { return false; }
	if ( dwError != ERROR_NO_MORE_FILES) { return false; }
	return true;
}

bool CFileOperate::GetFirstFileName(const char *pFolder, HANDLE &handle, WIN32_FIND_DATAA& FindFileData)
{
	if ( NULL == pFolder ) return false;

	char DirSpec[MAX_PATH] = {0};  // directory specification
	_snprintf(DirSpec, sizeof(DirSpec) - 1, "%s\\*", pFolder );

	handle = ::FindFirstFileA(DirSpec, &FindFileData);

	if ( INVALID_HANDLE_VALUE == handle )
	{
		return false;
	}
	return true;
}
bool CFileOperate::GetNextFileName(HANDLE &findHandle, WIN32_FIND_DATA& FindFileData)
{
	if ( findHandle != INVALID_HANDLE_VALUE )
	{
		if ( ::FindNextFile(findHandle, &FindFileData) )
		{
			return true;
		}
	}
	return false;
}

void CFileOperate::CloseFindHandle(HANDLE &findHandle)
{
	::FindClose(findHandle);
}