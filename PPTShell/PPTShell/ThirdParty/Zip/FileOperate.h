/*
 */

#ifndef _FILEOPERATE_H_
#define _FILEOPERATE_H_

#include <windows.h>
#include <string>

typedef enum _E_FILE_TYPE_
{
	E_FILE_TYPE_NOT_EXIST		= 0, //文件不存在
	E_FILE_TYPE_NORMAL_FILE		= 1, //文件为普通文件
	E_FILE_TYPE_DIRECTORY		= 2  //文件为目录
}E_FILE_TYPE;

class CFileOperate
{
public:
	//获取模块文件路径
	static bool GetModuleFilePath(char* modulefilepath, int length, HMODULE hModule = NULL);
	//获取文件名称
	static bool GetModuleFileName(char* modulefilename, int length, HMODULE hModule = NULL);
	//解析文件路径
	static bool ParseFilePath(const char* fileallpath, char* filepath, int length);
	//解析文件名
	static bool ParseFileName(const char* filepath, char* filename, int length );
	//解析后缀名
	static bool ParsePostfix(const char* filepath, char* postfix, int length);
	//解析文件名，不带后缀
	static bool ParseFileNameNoPostfix(const char* filepath, char* filenamenopostfix, int length);
	//检测文件是否存在
	static bool CheckFileIsExist(const char* filePath);
	//检测文件属性
	static E_FILE_TYPE CheckFileAttribute(const char *pFilePath);
	//创建目录
	static bool CreateMultiFolder(const char *pDirPath);
	//删除目录
	static bool DeleteFolder(const char *pFolder);
	//清空目录
	static bool CleanFolder(const char *pFolder);

	static unsigned long GetFileSize(const char *pFile, unsigned long *High = NULL);

	static bool GetAboslutePath(const char* filepath, char *aboslutepath, int lenght);

	static std::string CheckFilePathReturnString(const char *filepath );
	static void CheckFilePathVoid( char *filepath );

	//查找目录下的文件
	static bool GetFirstFileName( const char *pFolder, HANDLE &handle, WIN32_FIND_DATAA& FindFileData );
	static bool GetNextFileName( HANDLE &findHandle, WIN32_FIND_DATA& FindFileData );
	static void CloseFindHandle(HANDLE &findHandle);
};

#endif // _FILEPATH_H_
