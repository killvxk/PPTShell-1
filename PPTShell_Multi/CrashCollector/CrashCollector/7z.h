#ifndef ND_TOOL_PATCH_7Z_H
#define ND_TOOL_PATCH_7Z_H

#include "afx.h"

enum SZIPRES
{
	SZIP_OK,
	SZIP_FAIL,
	SZIP_PARAM_ERROR,
	SZIP_DLL_INTERFACE_ERROR,
	SZIP_READ_SRCFILE_FAIL,
	SZIP_WRITE_DESFILE_FAIL
};


// 进度回调通知
class ISevenZipCallback
{
public:
	virtual ~ISevenZipCallback(){};
	virtual void CompressTotal(UINT64 ullSize)=0;
	virtual void CompressComplete(const UINT64* pullCompleteValue)=0;
};

/**
 * 功能描述：压缩文件或目录
 * @param [IN]pcszDestFile 目标文件名 如：*.7z
 * @param [IN]pcszBasePath 基路径
 * @param [IN]pcszSrcFileOrDirectory 源文件(夹）数组指针，基于pcszBasePath的相对路径
 * @param [IN]iCount 源文件（夹）数组个数
 * @param [IN]cb 进度回调通知
 * @return SZIPRES
 */

SZIPRES SevenZipCompressItems(const char* pcszDestFile,
							  const char* pcszBasePath,
							  char* pcszSrcFileOrDirectory[],
							  int iCount,
							  ISevenZipCallback* cb
							  );
/**
 * 功能描述：解压文件
 * @param [IN]pcszDestDirectory 目标文件夹 解压的文件将放在此目录下，如果为空，则解压到当前目录下
 * @param [IN]pcszCompressedFile 被解压的文件 如：*.7z
 * @param [IN]cb 进度回调
 * @return SZIPRES
 */
SZIPRES SevenZipDecompress(const char* pcszDestDirectory,
						   const char* pcszCompressedFile,
						   ISevenZipCallback* cb
						   );
/**
 * 功能描述：获取被解压文件解压后的大小
 * @param [IN] pcszCompressedFile 被解压文件名 如*.7z
 * @param [OUT]pullSize 大小
 */
SZIPRES SevenZipGetDecompressSize(const char* pcszCompressedFile,
								  UINT64* pullSize
								  );

#endif