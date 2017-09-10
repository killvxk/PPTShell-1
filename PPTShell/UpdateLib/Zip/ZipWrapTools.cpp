#include "StdAfx.h"
#include "ZipWrapTools.h"
#include "zlib.h"

std::string StringTrimLeft(const std::string& str, const std::string separator)
{
	std::string strResult = str;
	if ( 0 == separator.compare("") )
	{
		return strResult;
	}
	/* if find separator in strResult and substr begin at 0 
	   truncate strResult begin at separator.size() */
	while( 0 == strResult.find(separator) )
	{
		strResult = strResult.substr( separator.size() );
	}
	return strResult;
}

std::string StringTrimRight(const std::string& str, const std::string separator)
{
	std::string strResult = str;
	std::string::size_type pos = std::string::npos;
	if ( 0 == separator.compare("") )
	{
		return strResult;
	}
	pos = strResult.rfind(separator);
	while ( (std::string::npos != pos)  
		&& (strResult.size() - pos == separator.size()) )
	{
		strResult = strResult.substr( 0, pos );
		pos = strResult.rfind(separator);
	}
	
	return strResult;
}

uLong GetZipFileTime(const char *f,tm_zip * tmzip,uLong * dt)
// char *f;                /* name of file to get info on */
// tm_zip *tmzip;             /* return value: access, modific. and creation times */
// uLong *dt;             /* dostime */
{
	int ret = 0;
	{
		FILETIME ftLocal;
		HANDLE hFind;
		WIN32_FIND_DATAA ff32;

		hFind = FindFirstFileA(f,&ff32);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			FileTimeToLocalFileTime(&(ff32.ftLastWriteTime),&ftLocal);
			FileTimeToDosDateTime(&ftLocal,((LPWORD)dt)+1,((LPWORD)dt)+0);
			FindClose(hFind);
			ret = 1;
		}
	}
	return ret;
}

int IsLargeFile(const char* filename)
{
	int largeFile = 0;
	ZPOS64_T pos = 0;
	FILE* pFile = fopen64(filename, "rb");

	if(pFile != NULL)
	{
		int n = fseeko64(pFile, 0, SEEK_END);

		pos = ftello64(pFile);

		printf("File : %s is %lld bytes\n", filename, pos);

		if(pos >= 0xffffffff)
			largeFile = 1;

		fclose(pFile);
	}

	return largeFile;
}

/* calculate the CRC32 of a file,
   because to encrypt a file, we need known the CRC32 of the file before */
int GetFileCrc(const char* filenameinzip,void*buf,unsigned long size_buf,unsigned long* result_crc)
{
   unsigned long calculate_crc=0;
   int err=ZIP_OK;
   FILE * fin = fopen64(filenameinzip,"rb");
   unsigned long size_read = 0;
   unsigned long total_read = 0;
   if (fin==NULL)
   {
       err = ZIP_ERRNO;
   }

    if (err == ZIP_OK)
        do
        {
            err = ZIP_OK;
            size_read = (int)fread(buf,1,size_buf,fin);
            if (size_read < size_buf)
                if (feof(fin)==0)
            {
                err = ZIP_ERRNO;
            }

            if (size_read>0)
                calculate_crc = crc32(calculate_crc,(const Bytef*) buf,size_read);
            total_read += size_read;

        } while ((err == ZIP_OK) && (size_read>0));

    if (fin)
        fclose(fin);

    *result_crc=calculate_crc;
    return err;
}

