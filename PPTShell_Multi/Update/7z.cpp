#include "7z.h"
#include "DllExports.h"

#include "Common/MyString.h"
#include "Common/StringConvert.h"
#include "Common/MyCom.h"
#include "Common/IntToString.h"

#include "7zip/Archive/IArchive.h"
#include "7zip/Common/FileStreams.h"
#include "7zip/IPassword.h"

#include "Windows/DLL.h"
#include "Windows/FileDir.h"
#include "Windows/FileFind.h"
#include "Windows/FileName.h"
#include "Windows/PropVariant.h"
#include "Windows/PropVariantConversions.h"

#include <stack>

DEFINE_GUID(CLSID_CFormat7z,
			0x23170F69,0x40C1,0x278A,0x10,0x00,0x00,0x01,0x10,0x07,0x00,0x00);
DEFINE_GUID(CLSID_CFormatRar,
			0x23170F69,0x40C1,0x278A,0x10,0x00,0x00,0x01,0x10,0x03,0x00,0x00);
DEFINE_GUID(CLSID_CFormatZip,
			0x23170F69,0x40C1,0x278A,0x10,0x00,0x00,0x01,0x10,0x02,0x00,0x00);


// 输出调式信息
void PrintString(const UString &s)
{
#ifdef _DEBUG
	OutputDebugString((LPCSTR)GetOemString(s));
#endif
}

void PrintString(const AString &s)
{
#ifdef _DEBUG
	OutputDebugString((LPCSTR)s);
#endif
}

void PrintNewLine()
{
	PrintString("\n");
}

void PrintStringLn(const AString &s)
{
	PrintString(s);
	PrintNewLine();
}

void PrintError(const AString &s)
{
	PrintNewLine();
	PrintString(s);
	PrintNewLine();
}

// 判断某个归档文件属于何种类型
static HRESULT IsArchiveItemProp(IInArchive *archive, UInt32 index, PROPID propID, bool &result)
{
	NWindows::NCOM::CPropVariant prop;
	RINOK(archive->GetProperty(index, propID, &prop));
	if (prop.vt == VT_BOOL)
		result = VARIANT_BOOLToBool(prop.boolVal);
	else if (prop.vt == VT_EMPTY)
		result = false;
	else
		return E_FAIL;
	return S_OK;
}

// 判断该归档文件是否是文件夹
static HRESULT IsArchiveItemFolder(IInArchive *archive, UInt32 index, bool &result)
{
	return IsArchiveItemProp(archive, index, kpidIsDir, result);
}

/**
 * 打开压缩文件回调类
 */

class CArchiveOpenCallback:
	public IArchiveOpenCallback,
	public ICryptoGetTextPassword,
	public CMyUnknownImp
{
public:
	MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

	STDMETHOD(SetTotal)(const UInt64 *files, const UInt64 *bytes);
	STDMETHOD(SetCompleted)(const UInt64 *files, const UInt64 *bytes);

	STDMETHOD(CryptoGetTextPassword)(BSTR *password);

	bool PasswordIsDefined;
	UString Password;

	CArchiveOpenCallback() : PasswordIsDefined(false) {}

};

STDMETHODIMP CArchiveOpenCallback::SetTotal(const UInt64 * /* files */, const UInt64 * /* bytes */)
{
	return S_OK;
}

STDMETHODIMP CArchiveOpenCallback::SetCompleted(const UInt64 * /* files */, const UInt64 * /* bytes */)
{
	return S_OK;
}

STDMETHODIMP CArchiveOpenCallback::CryptoGetTextPassword(BSTR *password)
{
	if (!PasswordIsDefined)
	{
		// You can ask real password here from user
		// Password = GetPassword(OutStream);
		// PasswordIsDefined = true;
		PrintError("Password is not defined");
		return E_ABORT;
	}
	return StringToBstr(Password, password);
}

/**
 * 解压压缩文件回调类
 */
static const wchar_t *kEmptyFileAlias = L"[Content]";
static const wchar_t *kCantDeleteOutputFile = L"ERROR: Can not delete output file ";

static const char *kTestingString    =  "Testing     ";
static const char *kExtractingString =  "Extracting  ";
static const char *kSkippingString   =  "Skipping    ";

static const char *kUnsupportedMethod = "Unsupported Method";
static const char *kCRCFailed = "CRC Failed";
static const char *kDataError = "Data Error";
static const char *kUnknownError = "Unknown Error";

class CArchiveExtractCallback:
	public IArchiveExtractCallback,
	public ICryptoGetTextPassword,
	public CMyUnknownImp
{
public:
	MY_UNKNOWN_IMP1(ICryptoGetTextPassword)

	// IProgress
	STDMETHOD(SetTotal)(UInt64 size);
	STDMETHOD(SetCompleted)(const UInt64 *completeValue);

	// IArchiveExtractCallback
	STDMETHOD(GetStream)(UInt32 index, ISequentialOutStream **outStream, Int32 askExtractMode);
	STDMETHOD(PrepareOperation)(Int32 askExtractMode);
	STDMETHOD(SetOperationResult)(Int32 resultEOperationResult);

	// ICryptoGetTextPassword
	STDMETHOD(CryptoGetTextPassword)(BSTR *aPassword);

private:
	CMyComPtr<IInArchive> _archiveHandler;
	UString _directoryPath;  // Output directory
	UString _filePath;       // name inside arcvhive
	UString _diskFilePath;   // full path to file on disk
	bool _extractMode;
	struct CProcessedFileInfo
	{
		FILETIME MTime;
		UInt32 Attrib;
		bool isDir;
		bool AttribDefined;
		bool MTimeDefined;
	} _processedFileInfo;

	COutFileStream *_outFileStreamSpec;
	CMyComPtr<ISequentialOutStream> _outFileStream;

public:
	void Init(IInArchive *archiveHandler, const UString &directoryPath);

	UInt64 NumErrors;
	bool PasswordIsDefined;
	UString Password;

	ISevenZipCallback* m_cb;

	CArchiveExtractCallback() : PasswordIsDefined(false) {m_cb=NULL;}
};

void CArchiveExtractCallback::Init(IInArchive *archiveHandler, const UString &directoryPath)
{
	NumErrors = 0;
	_archiveHandler = archiveHandler;
	_directoryPath = directoryPath;
	NWindows::NFile::NName::NormalizeDirPathPrefix(_directoryPath);
}

STDMETHODIMP CArchiveExtractCallback::SetTotal(UInt64  size )
{
	if(m_cb)
	{
		m_cb->CompressTotal(size);
	}
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::SetCompleted(const UInt64 *  completeValue )
{
	if(m_cb)
	{
		m_cb->CompressComplete(completeValue);
	}
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::GetStream(UInt32 index,ISequentialOutStream **outStream, Int32 askExtractMode)
{
	*outStream = 0;
	_outFileStream.Release();
	
	// Get Name
	{	
		NWindows::NCOM::CPropVariant prop;
		RINOK(_archiveHandler->GetProperty(index, kpidPath, &prop));

		UString fullPath;
		if (prop.vt == VT_EMPTY)
		{
			fullPath = kEmptyFileAlias;
		}
		else
		{
			if (prop.vt != VT_BSTR)
			{
				return E_FAIL;
			}
			fullPath = prop.bstrVal;
		}
		_filePath = fullPath;
		

		if (askExtractMode != NArchive::NExtract::NAskMode::kExtract)
		{
			return S_OK;
		}
	}
	
	// Get Attrib
	{	
		NWindows::NCOM::CPropVariant prop;
		RINOK(_archiveHandler->GetProperty(index, kpidAttrib, &prop));
		if (prop.vt == VT_EMPTY)
		{
			_processedFileInfo.Attrib = 0;
			_processedFileInfo.AttribDefined = false;
		}
		else
		{
			if (prop.vt != VT_UI4)
			{
				return E_FAIL;
			}
			_processedFileInfo.Attrib = prop.ulVal;
			_processedFileInfo.AttribDefined = true;
		}	

		RINOK(IsArchiveItemFolder(_archiveHandler, index, _processedFileInfo.isDir));
	}
	
	// Get Modified Time
	{	
		NWindows::NCOM::CPropVariant prop;
		RINOK(_archiveHandler->GetProperty(index, kpidMTime, &prop));
		_processedFileInfo.MTimeDefined = false;
		switch(prop.vt)
		{
		case VT_EMPTY:
			// _processedFileInfo.MTime = _utcMTimeDefault;
			break;
		case VT_FILETIME:
			_processedFileInfo.MTime = prop.filetime;
			_processedFileInfo.MTimeDefined = true;
			break;
		default:
			return E_FAIL;
		}
	}
	
	// Get Size
	{
		NWindows::NCOM::CPropVariant prop;
		RINOK(_archiveHandler->GetProperty(index, kpidSize, &prop));
		bool newFileSizeDefined = (prop.vt != VT_EMPTY);
		UInt64 newFileSize;
		if (newFileSizeDefined)
		{
			newFileSize = ConvertPropVariantToUInt64(prop);
		}
	}


	// Create folders for file
	int slashPos = _filePath.ReverseFind(WCHAR_PATH_SEPARATOR);
	if (slashPos >= 0)
	{
		NWindows::NFile::NDirectory::CreateComplexDirectory(_directoryPath + _filePath.Left(slashPos));
	}	

	UString fullProcessedPath = _directoryPath + _filePath;
	_diskFilePath = fullProcessedPath;

	if (_processedFileInfo.isDir)
	{
		NWindows::NFile::NDirectory::CreateComplexDirectory(fullProcessedPath);
	}
	else
	{
		NWindows::NFile::NFind::CFileInfoW fi;
		if (NWindows::NFile::NFind::FindFile(fullProcessedPath, fi))
		{
			if (!NWindows::NFile::NDirectory::DeleteFileAlways(fullProcessedPath))
			{
				PrintString(UString(kCantDeleteOutputFile) + fullProcessedPath);
				return E_ABORT;
			}
		}

		_outFileStreamSpec = new COutFileStream;
		CMyComPtr<ISequentialOutStream> outStreamLoc(_outFileStreamSpec);
		if (!_outFileStreamSpec->Open(fullProcessedPath, CREATE_ALWAYS))
		{
			PrintString((UString)L"can not open output file " + fullProcessedPath);
			return E_ABORT;
		}
		_outFileStream = outStreamLoc;
		*outStream = outStreamLoc.Detach();
	}
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::PrepareOperation(Int32 askExtractMode)
{
	_extractMode = false;
	switch (askExtractMode)
	{
	case NArchive::NExtract::NAskMode::kExtract:  _extractMode = true; break;
	};
	switch (askExtractMode)
	{
	case NArchive::NExtract::NAskMode::kExtract:  PrintString(kExtractingString); break;
	case NArchive::NExtract::NAskMode::kTest:  PrintString(kTestingString); break;
	case NArchive::NExtract::NAskMode::kSkip:  PrintString(kSkippingString); break;
	};
	PrintString(_filePath);
	return S_OK;
}

STDMETHODIMP CArchiveExtractCallback::SetOperationResult(Int32 operationResult)
{
	switch(operationResult)
	{
	case NArchive::NExtract::NOperationResult::kOK:
		break;
	default:
		{
			NumErrors++;
			PrintString("     ");
			switch(operationResult)
			{
			case NArchive::NExtract::NOperationResult::kUnSupportedMethod:
				PrintString(kUnsupportedMethod);
				break;
			case NArchive::NExtract::NOperationResult::kCRCError:
				PrintString(kCRCFailed);
				break;
			case NArchive::NExtract::NOperationResult::kDataError:
				PrintString(kDataError);
				break;
			default:
				PrintString(kUnknownError);
			}
		}
	}

	if (_outFileStream != NULL)
	{
		if (_processedFileInfo.MTimeDefined)
			_outFileStreamSpec->SetMTime(&_processedFileInfo.MTime);
		RINOK(_outFileStreamSpec->Close());
	}
	_outFileStream.Release();
	if (_extractMode && _processedFileInfo.AttribDefined)
	{
		NWindows::NFile::NDirectory::MySetFileAttributes(_diskFilePath, _processedFileInfo.Attrib);
	}
	PrintNewLine();
	return S_OK;
}


STDMETHODIMP CArchiveExtractCallback::CryptoGetTextPassword(BSTR *password)
{
	if (!PasswordIsDefined)
	{
		// You can ask real password here from user
		// Password = GetPassword(OutStream);
		// PasswordIsDefined = true;
		PrintError("Password is not defined");
		return E_ABORT;
	}
	return StringToBstr(Password, password);
}

/**
 * 创建压缩文件回调类
 */
struct CDirItem
{
	UInt64 Size;
	FILETIME CTime;
	FILETIME ATime;
	FILETIME MTime;
	UString Name;
	UString FullPath;
	UInt32 Attrib;

	bool isDir() const { return (Attrib & FILE_ATTRIBUTE_DIRECTORY) != 0 ; }
};

class CArchiveUpdateCallback:
	public IArchiveUpdateCallback2,
	public ICryptoGetTextPassword2,
	public CMyUnknownImp
{
public:
	MY_UNKNOWN_IMP2(IArchiveUpdateCallback2, ICryptoGetTextPassword2)

	// IProgress
	STDMETHOD(SetTotal)(UInt64 size);
	STDMETHOD(SetCompleted)(const UInt64 *completeValue);

	// IUpdateCallback2
	STDMETHOD(EnumProperties)(IEnumSTATPROPSTG **enumerator);
	STDMETHOD(GetUpdateItemInfo)(UInt32 index,
		Int32 *newData, Int32 *newProperties, UInt32 *indexInArchive);
	STDMETHOD(GetProperty)(UInt32 index, PROPID propID, PROPVARIANT *value);
	STDMETHOD(GetStream)(UInt32 index, ISequentialInStream **inStream);
	STDMETHOD(SetOperationResult)(Int32 operationResult);
	STDMETHOD(GetVolumeSize)(UInt32 index, UInt64 *size);
	STDMETHOD(GetVolumeStream)(UInt32 index, ISequentialOutStream **volumeStream);

	STDMETHOD(CryptoGetTextPassword2)(Int32 *passwordIsDefined, BSTR *password);

public:
	CRecordVector<UInt64> VolumesSizes;
	UString VolName;
	UString VolExt;

	UString DirPrefix;
	const CObjectVector<CDirItem> *DirItems;

	bool PasswordIsDefined;
	UString Password;
	bool AskPassword;

	bool m_NeedBeClosed;

	UStringVector FailedFiles;
	CRecordVector<HRESULT> FailedCodes;

	ISevenZipCallback* m_cb;

	CArchiveUpdateCallback(): PasswordIsDefined(false), AskPassword(false), DirItems(0) {m_cb=NULL;};

	~CArchiveUpdateCallback() { Finilize(); }
	HRESULT Finilize();

	void Init(const CObjectVector<CDirItem> *dirItems)
	{
		DirItems = dirItems;
		m_NeedBeClosed = false;
		FailedFiles.Clear();
		FailedCodes.Clear();
	}
};

STDMETHODIMP CArchiveUpdateCallback::SetTotal(UInt64  size )
{
	if(m_cb)
	{
		m_cb->CompressTotal(size);
	}
	return S_OK;
}

STDMETHODIMP CArchiveUpdateCallback::SetCompleted(const UInt64 *  completeValue )
{
	if(m_cb)
	{
		m_cb->CompressComplete(completeValue);
	}
	return S_OK;
}


STDMETHODIMP CArchiveUpdateCallback::EnumProperties(IEnumSTATPROPSTG ** /* enumerator */)
{
	return E_NOTIMPL;
}

STDMETHODIMP CArchiveUpdateCallback::GetUpdateItemInfo(UInt32 /* index */,
													   Int32 *newData, Int32 *newProperties, UInt32 *indexInArchive)
{
	if (newData != NULL)
		*newData = BoolToInt(true);
	if (newProperties != NULL)
		*newProperties = BoolToInt(true);
	if (indexInArchive != NULL)
		*indexInArchive = (UInt32)-1;
	return S_OK;
}

STDMETHODIMP CArchiveUpdateCallback::GetProperty(UInt32 index, PROPID propID, PROPVARIANT *value)
{
	NWindows::NCOM::CPropVariant prop;

	if (propID == kpidIsAnti)
	{
		prop = false;
		prop.Detach(value);
		return S_OK;
	}

	{
		const CDirItem &dirItem = (*DirItems)[index];
		switch(propID)
		{
		case kpidPath:  prop = dirItem.Name; break;
		case kpidIsDir:  prop = dirItem.isDir(); break;
		case kpidSize:  prop = dirItem.Size; break;
		case kpidAttrib:  prop = dirItem.Attrib; break;
		case kpidCTime:  prop = dirItem.CTime; break;
		case kpidATime:  prop = dirItem.ATime; break;
		case kpidMTime:  prop = dirItem.MTime; break;
		}
	}
	prop.Detach(value);
	return S_OK;
}

HRESULT CArchiveUpdateCallback::Finilize()
{
	if (m_NeedBeClosed)
	{
		PrintNewLine();
		m_NeedBeClosed = false;
	}
	return S_OK;
}

static void GetStream2(const wchar_t *name)
{
	PrintString("Compressing  ");
	if (name[0] == 0)
		name = kEmptyFileAlias;
	PrintString(name);
}

STDMETHODIMP CArchiveUpdateCallback::GetStream(UInt32 index, ISequentialInStream **inStream)
{
	RINOK(Finilize());

	const CDirItem &dirItem = (*DirItems)[index];
	GetStream2(dirItem.Name);

	if (dirItem.isDir())
		return S_OK;

	{
		CInFileStream *inStreamSpec = new CInFileStream;
		CMyComPtr<ISequentialInStream> inStreamLoc(inStreamSpec);
		UString path = DirPrefix + dirItem.FullPath;
		if (!inStreamSpec->Open(path))
		{
			DWORD sysError = ::GetLastError();
			FailedCodes.Add(sysError);
			FailedFiles.Add(path);
			// if (systemError == ERROR_SHARING_VIOLATION)
			{
				PrintNewLine();
				PrintError("WARNING: can't open file");
				// PrintString(NError::MyFormatMessageW(systemError));
				return S_FALSE;
			}
			// return sysError;
		}
		*inStream = inStreamLoc.Detach();
	}
	return S_OK;
}

STDMETHODIMP CArchiveUpdateCallback::SetOperationResult(Int32 /* operationResult */)
{
	m_NeedBeClosed = true;
	return S_OK;
}

STDMETHODIMP CArchiveUpdateCallback::GetVolumeSize(UInt32 index, UInt64 *size)
{
	if (VolumesSizes.Size() == 0)
		return S_FALSE;
	if (index >= (UInt32)VolumesSizes.Size())
		index = VolumesSizes.Size() - 1;
	*size = VolumesSizes[index];
	return S_OK;
}

STDMETHODIMP CArchiveUpdateCallback::GetVolumeStream(UInt32 index, ISequentialOutStream **volumeStream)
{
	wchar_t temp[32];
	ConvertUInt64ToString(index + 1, temp);
	UString res = temp;
	while (res.Length() < 2)
		res = UString(L'0') + res;
	UString fileName = VolName;
	fileName += L'.';
	fileName += res;
	fileName += VolExt;
	COutFileStream *streamSpec = new COutFileStream;
	CMyComPtr<ISequentialOutStream> streamLoc(streamSpec);
	if (!streamSpec->Create(fileName, false))
		return ::GetLastError();
	*volumeStream = streamLoc.Detach();
	return S_OK;
}

STDMETHODIMP CArchiveUpdateCallback::CryptoGetTextPassword2(Int32 *passwordIsDefined, BSTR *password)
{
	if (!PasswordIsDefined)
	{
		if (AskPassword)
		{
			// You can ask real password here from user
			// Password = GetPassword(OutStream);
			// PasswordIsDefined = true;
			PrintError("Password is not defined");
			return E_ABORT;
		}
	}
	*passwordIsDefined = BoolToInt(PasswordIsDefined);
	return StringToBstr(Password, password);
}

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
							  const char* pcszSrcFileOrDirectory[],
							  int iCount,
							  ISevenZipCallback* cb
							  )
{
	if(pcszDestFile==NULL||pcszBasePath==NULL||pcszSrcFileOrDirectory==NULL||iCount<1)
	{
		return SZIP_PARAM_ERROR;
	}

#ifdef _WIN32
#ifndef _UNICODE
	g_IsNT = IsItWindowsNT();
#endif
#endif

	UString basePath = GetUnicodeString(pcszBasePath,CP_OEMCP);
	unsigned int uiBasePathLen = basePath.Length();
	if((basePath[uiBasePathLen-1]!=L'\\') && (basePath[uiBasePathLen-1]!=L'/'))
	{
		basePath += L'\\';
	}
	uiBasePathLen = basePath.Length();

	UString archiveName;
	archiveName = GetUnicodeString(pcszDestFile,CP_OEMCP);

	CObjectVector<CDirItem> dirItems;
	std::stack<UString> stkPath;
	UString strFullFileName;
	int i;

	for(i = 0; i < iCount; i++)
	{
		CDirItem di;
		NWindows::NFile::NFind::CFileInfoW fi;
		NWindows::NFile::NFind::CFindFile finder;
		strFullFileName = basePath;
		strFullFileName += GetUnicodeString(pcszSrcFileOrDirectory[i],CP_OEMCP);
		if(!finder.FindFirst(strFullFileName,fi))
		{
			return SZIP_READ_SRCFILE_FAIL;
		}

		if(fi.IsDir())
		{
			stkPath.push(strFullFileName);  // 将目录保存在栈里
		}

		di.Attrib = fi.Attrib;
		di.Size = fi.Size;
		di.CTime = fi.CTime;
		di.ATime = fi.ATime;
		di.MTime = fi.MTime;
		di.Name = strFullFileName.Mid(uiBasePathLen);
		di.FullPath = strFullFileName;
		dirItems.Add(di);
	}

	// 非递归扫描目录下的文件和子目录

	UString strLastPath;
	while(!stkPath.empty())
	{
		CDirItem di;
		NWindows::NFile::NFind::CFileInfoW fi;
		NWindows::NFile::NFind::CFindFile finder;
		strLastPath = stkPath.top();
		stkPath.pop();
		
		strFullFileName = strLastPath;
		strFullFileName += L"\\*.*";

		NWindows::NFile::NName::NormalizeDirPathPrefix(strLastPath);
		if(!finder.FindFirst(strFullFileName,fi))
		{
			continue; // 目录里没有文件
		}

		do
		{
			if(fi.IsDots())
			{
				continue; // . or ..
			}
			else if(fi.IsDir())
			{
				stkPath.push(strLastPath+fi.Name); // directory
			}

			di.Attrib = fi.Attrib;
			di.Size = fi.Size;
			di.CTime = fi.CTime;
			di.ATime = fi.ATime;
			di.MTime = fi.MTime;			
			di.FullPath = strLastPath+fi.Name;
			di.Name = di.FullPath.Mid(uiBasePathLen);
			dirItems.Add(di);

		}while(finder.FindNext(fi));

	}

	COutFileStream* outFileStreamSpec = new COutFileStream;
	CMyComPtr<IOutStream> outFileStream = outFileStreamSpec;

	if(!outFileStreamSpec->Create(archiveName,true))
	{
		return SZIP_WRITE_DESFILE_FAIL;
	}

	CMyComPtr<IOutArchive> outArchive;
	if(CreateObject(&CLSID_CFormat7z,&IID_IOutArchive,(void**)&outArchive)!=S_OK)
	{
		return SZIP_DLL_INTERFACE_ERROR;
	}

	CArchiveUpdateCallback* updateCallbackSpec = new CArchiveUpdateCallback;
	CMyComPtr<IArchiveUpdateCallback2> updateCallback(updateCallbackSpec);

    updateCallbackSpec->Init(&dirItems);
	updateCallbackSpec->m_cb = cb;

	HRESULT hr = outArchive->UpdateItems(outFileStream,dirItems.Size(),updateCallback);
	updateCallbackSpec->Finilize();

	if(hr != S_OK)
	{
		return SZIP_FAIL;
	}

	if(updateCallbackSpec->FailedFiles.Size()!=0)
	{
		return SZIP_READ_SRCFILE_FAIL;
	}
	return SZIP_OK;
}

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
						   )
{
	if(pcszCompressedFile == NULL)
	{
		return SZIP_PARAM_ERROR;
	}

#ifdef _WIN32
#ifndef _UNICODE
	g_IsNT = IsItWindowsNT();
#endif
#endif

	UString archiveName = GetUnicodeString(pcszCompressedFile,CP_OEMCP);
	CMyComPtr<IInArchive> archive;
	CMyComPtr<IInArchive> archiveRar;
	CMyComPtr<IInArchive> archive7z;
	CMyComPtr<IInArchive> archiveZip;
	HRESULT hr = S_OK;
	hr = CreateObject(&CLSID_CFormat7z,&IID_IInArchive,(void**)&archive7z);
	if(hr != S_OK)
	{

		return SZIP_DLL_INTERFACE_ERROR;
	}
// 	hr = CreateObject(&CLSID_CFormatRar,&IID_IInArchive,(void**)&archiveRar);
// 	if(hr != S_OK)
// 	{
// 
// 		return SZIP_DLL_INTERFACE_ERROR;
// 	}
// 	hr = CreateObject(&CLSID_CFormatZip,&IID_IInArchive,(void**)&archiveZip);
// 	if(hr != S_OK)
// 	{
// 
// 		return SZIP_DLL_INTERFACE_ERROR;
// 	}

	

	CInFileStream* fileSpec = new CInFileStream;
	CMyComPtr<IInStream> file = fileSpec;
	if(!fileSpec->Open(archiveName))
	{
		return SZIP_READ_SRCFILE_FAIL;
	}

	CArchiveOpenCallback* openCallbackSpec = new CArchiveOpenCallback;
	CMyComPtr<IArchiveOpenCallback> openCallback(openCallbackSpec);
	openCallbackSpec->PasswordIsDefined = false;

	archive = archive7z;
	if(archive->Open(file,0,openCallback)!=S_OK)
	{
		archive->Close();
		archive=archiveRar;
		UInt64 pos = 0;
		file->Seek(0,SEEK_SET,&pos);
		if(archive->Open(file,0,openCallback)!=S_OK)
		{
			archive->Close();
			archive=archiveZip;
			pos = 0;
			file->Seek(0,SEEK_SET,&pos);
			if(archive->Open(file,0,openCallback)!=S_OK)
			{
				archive->Close();
				return SZIP_READ_SRCFILE_FAIL;
			}
		}

	}

	// Extract Command
	CArchiveExtractCallback* extractCallbackSpec = new CArchiveExtractCallback;
	CMyComPtr<IArchiveExtractCallback> extractCallback(extractCallbackSpec);
	extractCallbackSpec->m_cb = cb;

	extractCallbackSpec->Init(archive,(NULL==pcszDestDirectory)?L"":GetUnicodeString(pcszDestDirectory));
	extractCallbackSpec->PasswordIsDefined = false;

	hr = archive->Extract(NULL,(UInt32)(Int32)(-1),false,extractCallback);

	if(hr != S_OK)
	{
		return SZIP_FAIL;
	}

	return SZIP_OK;
}
/**
 * 功能描述：获取被解压文件解压后的大小
 * @param [IN] pcszCompressedFile 被解压文件名 如*.7z
 * @param [OUT]pullSize 大小
 */
SZIPRES SevenZipGetDecompressSize(const char* pcszCompressedFile,
								  UINT64* pullSize
								  )
{
	if(pcszCompressedFile==NULL || pullSize==NULL)
	{
		return SZIP_PARAM_ERROR;
	}

#ifdef _WIN32
#ifndef _UNICODE
	g_IsNT = IsItWindowsNT();
#endif
#endif

	UString archiveName = GetUnicodeString(pcszCompressedFile,CP_OEMCP);
	HRESULT hr = S_OK;
    bool bIsOK = false;

	CMyComPtr<IInArchive> archive;
	hr = CreateObject(&CLSID_CFormat7z,&IID_IInArchive,(void**)&archive);
	if(hr != S_OK)
	{
		return SZIP_DLL_INTERFACE_ERROR;
	}

	CInFileStream* fileSpec = new CInFileStream;
	CMyComPtr<IInStream> file = fileSpec;
	bIsOK = fileSpec->Open(archiveName);
	if(!bIsOK)
	{
		return SZIP_READ_SRCFILE_FAIL;
	}

	//CArchiveOpenCallback* openCallbackSpec = new CArchiveOpenCallback;
	//CMyComPtr<IArchiveOpenCallback> openCallback(openCallbackSpec);
	//openCallbackSpec->PasswordIsDefined = false;

	if(archive->Open(file,0,0)!=S_OK)
	{
		return SZIP_READ_SRCFILE_FAIL;
	}

	NWindows::NCOM::CPropVariant prop;
	UINT uiItems;
	*pullSize = 0;

	hr = archive->GetNumberOfItems(&uiItems);
	for(UINT i=0;i<uiItems;i++)
	{
		archive->GetProperty(i,kpidSize,&prop);
		if(prop.vt == VT_EMPTY)
		{
			return SZIP_FAIL;
		}

		*pullSize += ConvertPropVariantToUInt64(prop);
	}

	return SZIP_OK;
}