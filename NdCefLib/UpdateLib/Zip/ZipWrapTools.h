#ifndef _ZIPWRAPTOOLS_H_
#define _ZIPWRAPTOOLS_H_

#include <string>
#include "zip.h"
#include "unzip.h"

uLong GetZipFileTime(const char *f,tm_zip * tmzip,uLong * dt);

int IsLargeFile(const char* filename);
int GetFileCrc(const char* filenameinzip,void*buf,unsigned long size_buf,unsigned long* result_crc);
std::string StringTrimLeft(const std::string& str, const std::string separator);
std::string StringTrimRight(const std::string& str, const std::string separator);

#endif
