#include <Windows.h>
#include "ZipWrapper.h"

#include "zip.h"
#include "unzip.h"
#include "crypt.h"
#include "ioapi.h"
#include "zlib.h"
#include "ZipWrapTools.h"
#include "FileOperate.h"
#include <string>


#define WRITEBUFFERSIZE (16384)

CZipper::CZipper()
{
	zipfile_ = NULL;
}
CZipper::~CZipper()
{
	CloseZipFile(NULL);
}

bool CZipper::AddFolderToZipFile(const char*foldername, const char* rootfolder)
{
	if(NULL == zipfile_) return false;
	if(NULL == foldername) return false;
	if(NULL == rootfolder) return false;
	if(E_FILE_TYPE_DIRECTORY != CFileOperate::CheckFileAttribute(foldername)) return false;

	char aboslutepath[MAX_PATH + 1] = {0};
	char savefoldernameinzipfile[MAX_PATH + 1] = {0};
	_snprintf(aboslutepath, sizeof(aboslutepath) - 1, "%s", foldername);
	/*CFileOperate::GetAboslutePath(foldername, aboslutepath, sizeof(aboslutepath));*/

	bool flag = true;

	/* add folder to zip file*/
	// check the root can be found
	if (0 != _strnicmp(aboslutepath, rootfolder, lstrlenA(rootfolder)))
	{
		return false;
	}
	else{
		// else
		lstrcpyA(savefoldernameinzipfile, aboslutepath + lstrlenA(rootfolder));
	}
	if ( 0 != _stricmp("", savefoldernameinzipfile) )
	{
		if ( !AddFolderOnlyPathToFile(savefoldernameinzipfile, NULL) )
		{
			return false;
		}
	}
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFind = INVALID_HANDLE_VALUE;
	char DirSpec[MAX_PATH] = {0};  // directory specification
	DWORD dwError = 0;
	_snprintf(DirSpec,sizeof(DirSpec) - 1, "%s\\*", aboslutepath);

	hFind = ::FindFirstFileA(DirSpec, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) 
	{
		return false;
	}
	do 
	{		
		if (0 != strcmp(".", FindFileData.cFileName) && 0 != strcmp("..", FindFileData.cFileName))
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				char szNextDir[MAX_PATH] = {0};
				_snprintf(szNextDir,sizeof(DirSpec) - 1, "%s\\%s", aboslutepath, FindFileData.cFileName);
				flag = AddFolderToZipFile(szNextDir, rootfolder);
			}
			else
			{
				char szFilePath[MAX_PATH] = {0};
				_snprintf(szFilePath,sizeof(DirSpec) - 1, "%s\\%s", aboslutepath, FindFileData.cFileName);
				
				flag = AddFileToZipFile(szFilePath, savefoldernameinzipfile);
			}
		}
	} while (::FindNextFileA(hFind, &FindFileData) != 0 && flag);
	dwError = ::GetLastError();
	::FindClose(hFind);
	if (dwError != ERROR_NO_MORE_FILES ) 
	{
		flag = false;
	}
	return flag;
}

bool CZipper::AddFileToZipFile(const char*filename, const char*relfolder, const char* comment)
{
	if( NULL == zipfile_ ) return false;
	if ( NULL == filename ) return false;

	if( E_FILE_TYPE_NORMAL_FILE != CFileOperate::CheckFileAttribute(filename) ) return false;
 
    FILE * fin		= NULL;
    int size_read	= 0;
	std::string savefilenameinzip;
    zip_fileinfo zi;
    unsigned long crcFile=0;
    int zip64 = 0;
	int err = ZIP_OK;

	int size_buf=0;
	void* buf=NULL;
//	const char* password=NULL;

	/* create read buffer */
	size_buf = WRITEBUFFERSIZE;
	buf = (void*)malloc(size_buf);
	if (buf==NULL)
	{
		return false;
	}

    zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
    zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
    zi.dosDate = 0;
    zi.internal_fa = 0;
    zi.external_fa = GetFileAttributesA(filename);
    GetZipFileTime(filename,&zi.tmz_date,&zi.dosDate);

//  if ((password != NULL) && (err==ZIP_OK))
//		err = getFileCrc(filenameinzip,buf,size_buf,&crcFile);

	zip64 = IsLargeFile(filename);

	/* The path name saved, should not include a leading slash. */
	/*if it did, windows/xp and dynazip couldn't read the zip file. */

	char savenamebuffer[MAX_PATH] = {0};
	CFileOperate::ParseFileName(filename, savenamebuffer, sizeof(savenamebuffer));
	if ( NULL != relfolder )
	{
		savefilenameinzip = relfolder;
	}
	if ( !savefilenameinzip.empty() )
	{
		savefilenameinzip = StringTrimLeft(savefilenameinzip, "\\");
		savefilenameinzip = StringTrimLeft(savefilenameinzip, "/");
		savefilenameinzip = StringTrimRight(savefilenameinzip, "\\");
		savefilenameinzip = StringTrimRight(savefilenameinzip, "/");
		savefilenameinzip += "\\";
	}
	savefilenameinzip += savenamebuffer;

     /**/
    err = zipOpenNewFileInZip3_64(zipfile_,savefilenameinzip.c_str(),&zi,
                     NULL,0,NULL,0,comment /* comment*/,
                     Z_DEFLATED,
                     Z_DEFLATED,0,
                     /* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
                     -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                     NULL,crcFile, zip64);
	if( ZIP_OK == err )
	{
		fin = fopen64(filename,"rb");
		if (fin==NULL)
		{
			err=ZIP_ERRNO;
		}
	}

	if (err == ZIP_OK)
	{
		do
		{
			err = ZIP_OK;
			size_read = (int)fread(buf,1,size_buf,fin);
			if (size_read < size_buf)
			{
				if (feof(fin)==0)
				{
					err = ZIP_ERRNO;
				}
			}
			if (size_read>0)
			{
				err = zipWriteInFileInZip (zipfile_,buf,size_read);
			}
		} while ((err == ZIP_OK) && (size_read>0));
	}

    if (fin)
	{
        fclose(fin);
	}
	if ( buf )
	{
		free(buf);
	}
	
    if(ZIP_OK == err)
    {
        err = zipCloseFileInZip(zipfile_);
    }
	return (ZIP_OK == err);
}

bool CZipper::AddFolderOnlyPathToFile(const char* foldername, const char* comment)
{
	if ( NULL == foldername ) return false;
	if ( NULL == zipfile_ ) return false;
	
	std::string savefilenameinzip = foldername;
	zip_fileinfo zi;
	int err = ZIP_OK;

	zi.tmz_date.tm_sec = zi.tmz_date.tm_min = zi.tmz_date.tm_hour =
	zi.tmz_date.tm_mday = zi.tmz_date.tm_mon = zi.tmz_date.tm_year = 0;
	zi.dosDate = 0;
	zi.internal_fa = 0;
	zi.external_fa = 0;

	/* The path name saved, should not include a leading slash. */
	/*if it did, windows/xp and dynazip couldn't read the zip file. */
	savefilenameinzip = StringTrimLeft(savefilenameinzip, "\\");
	savefilenameinzip = StringTrimLeft(savefilenameinzip, "/");
	savefilenameinzip = StringTrimRight(savefilenameinzip, "\\");
	savefilenameinzip = StringTrimRight(savefilenameinzip, "/");

	savefilenameinzip += "/";

     /**/
    err = zipOpenNewFileInZip3_64(zipfile_,savefilenameinzip.c_str(),&zi,
                     NULL,0,NULL,0,NULL /* comment*/,
                     Z_DEFLATED,
                     Z_DEFLATED,0,
                     /* -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY, */
                     -MAX_WBITS, DEF_MEM_LEVEL, Z_DEFAULT_STRATEGY,
                     NULL, 0, 0);
	if ( ZIP_OK == err )
	{
		err = zipCloseFileInZip(zipfile_);
	}

	return (ZIP_OK == err);
}

bool CZipper::OpenZipFile(const char* zipfilename, bool append)
{
	CloseZipFile(NULL);
	if(NULL == zipfilename) return false;
	zipfile_ = zipOpen64(zipfilename,(append) ? APPEND_STATUS_ADDINZIP : APPEND_STATUS_CREATE);
	return (NULL != zipfile_);
}

bool CZipper::CloseZipFile(const char* global_comment)
{
	int ret = ZIP_OK;
	if ( NULL != zipfile_ )
	{
		ret = zipClose(zipfile_, global_comment);
		zipfile_ = NULL;
	}
	return (ZIP_OK == ret);
}

// simple interface
// saves as same name with .zip
bool CZipper::ZipFile(const char* szFilePath, const char* szZipPath)
{
	if ( NULL == szFilePath ) return false;
	if( E_FILE_TYPE_NORMAL_FILE != CFileOperate::CheckFileAttribute(szFilePath) ) return false;
	
	// make zip path
	//char szName[_MAX_FNAME];	
	//_splitpath(szFilePath, NULL, NULL, szName, NULL);

	//char szTempZipPath[MAX_PATH];

	//_makepath(szTempZipPath, NULL, szZipPath, szName, "temp");

	CZipper zip;
	bool flag = true;
	flag = zip.OpenZipFile(szZipPath, false);

	if ( flag )
	{
		flag = zip.AddFileToZipFile(szFilePath, NULL, NULL);
		zip.CloseZipFile(NULL);
		if ( !flag )
		{
			::DeleteFileA(szZipPath);
		}
	}
	return flag;
}

// saves as same name with .zip
bool CZipper::ZipFolder(const char* szFilePath, bool ignoreself)
{
	if ( NULL == szFilePath ) return false;
	if( E_FILE_TYPE_DIRECTORY != CFileOperate::CheckFileAttribute(szFilePath) ) return false;

	// make zip path
	char szDrive[_MAX_DRIVE] = {0}, szFolder[MAX_PATH] = {0}, szName[_MAX_FNAME] = {0};
	_splitpath(szFilePath, szDrive, szFolder, szName, NULL);

	char szZipPath[MAX_PATH] = {0};
	_makepath(szZipPath, szDrive, szFolder, szName, "zip");

	char szRootFolder[MAX_PATH] = {0};
	if ( ignoreself )
	{
		_snprintf(szRootFolder, sizeof(szRootFolder) - 1, "%s", szFilePath);
	}
	else
	{
		CFileOperate::ParseFilePath(szFilePath, szRootFolder, sizeof(szRootFolder) - 1);
	}

	CZipper zip;
	bool flag = true;

	flag = zip.OpenZipFile(szZipPath, false);
	if ( flag )
	{
		flag = zip.AddFolderToZipFile(szFilePath, szRootFolder);
		zip.CloseZipFile(NULL);
		if ( !flag )
		{
			::DeleteFileA(szZipPath);
		}
	}
	return flag;
}

/*
 */

void change_file_date(const char *filename,uLong dosdate,UZ_s tmu_date)
{
	HANDLE hFile = INVALID_HANDLE_VALUE;
	FILETIME ftm,ftLocal,ftCreate,ftLastAcc,ftLastWrite;

	hFile = CreateFileA(filename,GENERIC_READ | GENERIC_WRITE,
		0,NULL,OPEN_EXISTING,0,NULL);
	if ( INVALID_HANDLE_VALUE != hFile )
	{
		GetFileTime(hFile,&ftCreate,&ftLastAcc,&ftLastWrite);
		DosDateTimeToFileTime((WORD)(dosdate>>16),(WORD)dosdate,&ftLocal);
		LocalFileTimeToFileTime(&ftLocal,&ftm);
		SetFileTime(hFile,&ftm,&ftLastAcc,&ftm);
		CloseHandle(hFile);
	}
}

#define CASESENSITIVITY (0)

CUnZipper::CUnZipper()
{
	unzipfile_ = NULL;
}
CUnZipper::~CUnZipper()
{
	CloseUnZipFile();
}

// simple interface
bool CUnZipper::UnZip( const char* filename, const char* dstfolder, bool ingorepath, const char* password)
{
	if(NULL == filename) return false;
	if(NULL == dstfolder) return false;

	CUnZipper UnZipper;
	bool flag = true;
	flag = UnZipper.OpenUnZipFile(filename);
	if ( flag )
	{
		flag = UnZipper.UnZipTo(dstfolder, ingorepath, password);
	}
	UnZipper.CloseUnZipFile();
	return flag;
}

bool CUnZipper::OpenUnZipFile(const char* filename)
{
	if(NULL == filename) return false;
	unzipfile_ = unzOpen64(filename);
	return (NULL != unzipfile_);
}

bool CUnZipper::UnZipTo( const char* dstfolder, bool ingorepath, const char* password)
{
	if(NULL == unzipfile_) return false;
	if(NULL == dstfolder) return false;
	
	bool flag = true;
	if ( !GotoFirstFile() )
	{
		return false;
	}
	do 
	{
		flag = UnCurrentZipFile(dstfolder, ingorepath, password);
		if ( !flag ) return false;
	} while ( GotoNextFile() );

	return true;
}


bool CUnZipper::CloseUnZipFile()
{
	int ret = UNZ_OK;
	if ( NULL != unzipfile_ )
	{
		ret = unzClose(unzipfile_);
		unzipfile_ = NULL;
	}
	return (UNZ_OK == ret);
}

int CUnZipper::GetFileCount()
{
	unz_global_info64 gi;
	int err;
	if ( NULL == unzipfile_ )
	{
		return 0;
	}
	err = unzGetGlobalInfo64(unzipfile_, &gi);
	if ( UNZ_OK == err )
	{
		return ( int)gi.number_entry;
	}
	return 0;
}

bool CUnZipper::GotoFirstFile()
{
	if( NULL == unzipfile_ ) return false;
	return (unzGoToFirstFile(unzipfile_) == UNZ_OK);
}
bool CUnZipper::GotoNextFile()
{
	if( NULL == unzipfile_ ) return false;
	return (unzGoToNextFile(unzipfile_) == UNZ_OK);
}

bool CUnZipper::GotoZipFile(int index)
{
	if( NULL == unzipfile_ ) return false;
	if ( index > GetFileCount() || index < 1 ) return false;
	bool flag = true;
	flag = GotoFirstFile();
	if ( flag )
	{
		for ( int i = 1; (i <= GetFileCount()) && flag ; i++ )
		{
			if( i == index ) break;
			flag = GotoNextFile();
		}
	}
	return flag;
}

bool CUnZipper::GotoZipFile(const char* zipfilename)
{
	if( NULL == zipfilename ) return false;
	if( NULL == unzipfile_ ) return false;

	if (UNZ_OK != unzLocateFile(unzipfile_,zipfilename,CASESENSITIVITY))
	{
		return false;
	}
	return true;
}

bool CUnZipper::GetCurrentFileInfo(UZ_FileInfo&info)
{
	if (! unzipfile_)return false;
	
	unz_file_info uzfi;
	memset(&uzfi, 0, sizeof(uzfi));
	memset(&info, 0, sizeof(info));

	if (UNZ_OK != unzGetCurrentFileInfo(unzipfile_, &uzfi, 
		info.szFileName, sizeof(info.szFileName) - 1, 
		NULL, 0, 
		info.szComment, sizeof(info.szComment) - 1))
	{
		return false;
	}

	// copy across
	info.dwVersion = uzfi.version;	
	info.dwVersionNeeded = uzfi.version_needed;
	info.dwFlags = uzfi.flag;	
	info.dwCompressionMethod = uzfi.compression_method; 
	info.dwDosDate = uzfi.dosDate;  
	info.dwCRC = uzfi.crc;	 
	info.dwCompressedSize = uzfi.compressed_size; 
	info.dwUncompressedSize = uzfi.uncompressed_size;
	info.dwInternalAttrib = uzfi.internal_fa; 
	info.dwExternalAttrib = uzfi.external_fa;
	info.tmu_date.tm_year = uzfi.tmu_date.tm_year;
	info.tmu_date.tm_mon  = uzfi.tmu_date.tm_mon;
	info.tmu_date.tm_mday = uzfi.tmu_date.tm_mday;
	info.tmu_date.tm_hour = uzfi.tmu_date.tm_hour;
	info.tmu_date.tm_min = uzfi.tmu_date.tm_min;
	info.tmu_date.tm_sec = uzfi.tmu_date.tm_sec;
	// replace filename forward slashes with backslashes
	int nLen = lstrlenA(info.szFileName);
	if ( nLen > 0 )
	{
		if ( info.szFileName[nLen-1] == '\\' || info.szFileName[nLen-1] == '/' )
			info.bFolder = true;
		else
			info.bFolder = false;
	}
// 	// is it a folder?
// 	info.bFolder = ((info.dwExternalAttrib & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
	return true;
}

bool CUnZipper::UnOneZipFile(const char* filename, const char* dstfolder,
							 bool ingorepath, const char* password)
{
	if( NULL == unzipfile_ ) return false;
	if( NULL == filename) return false;
	if( NULL == dstfolder) return false;

	if (UNZ_OK != unzLocateFile(unzipfile_,filename,CASESENSITIVITY))
	{
		return false;
	}
	return UnCurrentZipFile( dstfolder, ingorepath, password);
}

bool CUnZipper::UnOneZipFile(int index, const char* dstfolder, bool ingorepath, const char* password)
{
	if( NULL == unzipfile_ ) return false;
	if ( index > GetFileCount() || index < 1 ) return false;
	if(NULL == dstfolder) return false;
	bool flag = true;
	flag = GotoFirstFile();
	if ( flag )
	{
		for ( int i = 1; (i <= GetFileCount()) && flag ; i++ )
		{
			if( i == index ) break;
			flag = GotoNextFile();
		}
	}
	if ( flag )
	{
		return UnCurrentZipFile( dstfolder, ingorepath, password);
	}
	return flag;
}

bool CUnZipper::UnCurrentZipFile(const char* dstfolder, bool ingorepath , const char* password )
{
	if( NULL == unzipfile_ ) return false;
	if( NULL == dstfolder ) return false;

	int err = UNZ_OK;
	void* buf	= NULL;
	uInt size_buf = WRITEBUFFERSIZE;
	UZ_FileInfo fileinfo;
	FILE* fout = NULL;
	char fullpath[MAX_PATH * 2+1] = {0};

	if ( !CFileOperate::CreateMultiFolder(dstfolder) )
	{
		return false;
	}
	if ( !GetCurrentFileInfo(fileinfo) )
	{
		return false;
	}
 	if ( fileinfo.bFolder && !ingorepath )
	{
		sprintf_s(fullpath, "%s\\%s", dstfolder, fileinfo.szFileName );
		return CFileOperate::CreateMultiFolder(fullpath);
	}
	
	buf = (void*)malloc(size_buf);
	if (buf==NULL)
	{
		return false;
	}
	err = unzOpenCurrentFilePassword(unzipfile_,password);
	if ( UNZ_OK == err )
	{
		if ( !ingorepath )
		{
			_snprintf(fullpath, sizeof(fullpath) - 1, "%s\\%s", dstfolder, fileinfo.szFileName );
		}
		else
		{
			char filename[MAX_PATH * 2] = {0};
			CFileOperate::ParseFileName(fileinfo.szFileName, filename, sizeof(filename) - 1 );
			_snprintf(fullpath, sizeof(fullpath) - 1, "%s\\%s", dstfolder, filename );
		}
		CFileOperate::CheckFilePathVoid(fullpath);
		char parentpath[MAX_PATH+1] = {0};
		CFileOperate::ParseFilePath(fullpath, parentpath, sizeof(parentpath));
		CFileOperate::CreateMultiFolder(parentpath);
		fout =fopen64(fullpath,"wb");
		if ( NULL == fout ) err = UNZ_ERRNO;
	}
	if ( NULL != fout )
	{
		do
		{
			err = unzReadCurrentFile(unzipfile_, buf, size_buf);
			if (err<0)
			{
				break;
			}
			if (err>0)
			{
				if (fwrite(buf,err,1,fout)!=1)
				{
					err=UNZ_ERRNO;
					break;
				}
			}
		}
		while (err>0);
		if (fout)
		{
			fclose(fout);
		}
		if (UNZ_OK == err)
		{
			change_file_date(fullpath,fileinfo.dwDosDate,
				fileinfo.tmu_date);
		}
	}

	if ( buf )
	{
		free(buf); buf = NULL;
	}
	return (UNZ_OK == err);
}