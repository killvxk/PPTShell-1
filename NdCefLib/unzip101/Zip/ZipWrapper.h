#ifndef _ZIP_WRAPPER_H_
#define _ZIP_WRAPPER_H_

#if defined(ZIPWRAP_EXPORTS)
#define ZIPWRAP_EXP __declspec(dllexport)
#elif defined(_MSC_VER)&&(_MSC_VER<1200)
#define ZIPWRAP_EXP __declspec(dllimport)
#else
#define ZIPWRAP_EXP
#endif

/*
 */
class ZIPWRAP_EXP CZipper
{
public:
	CZipper();
	virtual ~CZipper();

	// simple interface
	static bool ZipFile(const char* szFilePath, const char* szZipPath); // saves as same name with .zip
	static bool ZipFolder(const char* szFilePath, bool ignoreself = false); // saves as same name with .zip

	bool AddFolderToZipFile(const char*foldername, const char* rootfolder);
	bool AddFileToZipFile(const char*filename, const char*relfolder = NULL, const char* comment = NULL);
	bool AddFolderOnlyPathToFile(const char* foldername, const char* comment = NULL);

	bool OpenZipFile(const char* zipfilename, bool append = false);
	bool CloseZipFile(const char* global_comment = NULL);
private:
	void* zipfile_;/* = NULL */
};

/*
 */
#define MAX_COMMENT (255)

/* tm_unz contain date/time info */
typedef struct UZ_s
{
	unsigned int tm_sec;            /* seconds after the minute - [0,59] */
	unsigned int tm_min;            /* minutes after the hour - [0,59] */
	unsigned int tm_hour;           /* hours since midnight - [0,23] */
	unsigned int tm_mday;           /* day of the month - [1,31] */
	unsigned int tm_mon;            /* months since January - [0,11] */
	unsigned int tm_year;           /* years - [1980..2044] */
} UZ_s;

// create our own fileinfo struct to hide the underlying implementation
struct UZ_FileInfo
{
	char szFileName[260 + 1];
	char szComment[255 + 1];

	unsigned long dwVersion;  
	unsigned long dwVersionNeeded;
	unsigned long dwFlags;	 
	unsigned long dwCompressionMethod; 
	unsigned long dwDosDate;	
	unsigned long dwCRC;   
	unsigned long dwCompressedSize; 
	unsigned long dwUncompressedSize;
	unsigned long dwInternalAttrib; 
	unsigned long dwExternalAttrib; 
	bool bFolder;

	UZ_s tmu_date;
};

class ZIPWRAP_EXP CUnZipper
{
public:
	CUnZipper();
	virtual ~CUnZipper();

	// simple interface
	static bool UnZip( const char* filename, const char* dstfolder, bool ingorepath = false, const char* password = NULL);

	bool OpenUnZipFile(const char* filename);
	bool CloseUnZipFile();
	bool UnZipTo( const char* dstfolder, bool ingorepath = false, const char* password = NULL);


	int GetFileCount();
	
	bool GotoFirstFile();
	bool GotoNextFile();
	bool GotoZipFile(int index);
	bool GotoZipFile(const char* zipfilename);

	bool GetCurrentFileInfo(UZ_FileInfo&fileinfo);
	bool UnCurrentZipFile(const char* dstfolder, bool ingorepath = false, const char* password = NULL);
	bool UnOneZipFile(const char* filename, const char* dstfolder, bool ingorepath = false, const char* password = NULL);
	bool UnOneZipFile(int index, const char* dstfolder, bool ingorepath = false, const char* password = NULL);

private:
	void* unzipfile_;
};

#endif
