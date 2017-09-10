#pragma once

using namespace Gdiplus;

class CGdiPlusPng
{
public:
	Gdiplus::Image* m_pImage;

public:
	CGdiPlusPng()
	{
		m_pImage = NULL;
	}

	CGdiPlusPng(LPCWSTR pFile)
	{
		m_pImage = NULL;
		Load(pFile);
	}

	virtual ~CGdiPlusPng()
	{
		Empty();
	}

	void Empty()
	{
		delete m_pImage;
		m_pImage = NULL;
	}

	bool Load(LPCWSTR pFile)
	{
		Empty();
		m_pImage = Gdiplus::Bitmap::FromFile(pFile);
		return m_pImage->GetLastStatus() == Gdiplus::Ok;
	}

	operator Gdiplus::Image * () const
	{
		return m_pImage;
	}

	Gdiplus::Image *GetPngImage() const
	{
		return m_pImage;
	}
};


class CGdiPlusPngFile : public CGdiPlusPng
{
protected:
	HGLOBAL m_hBuffer;

public:
	CGdiPlusPngFile()
	{
		m_hBuffer = NULL;
	}

	virtual ~CGdiPlusPngFile()
	{
		Empty();
	}

	void Empty();

	bool Load(LPCWSTR lpszFilePath);
};

inline void CGdiPlusPngFile::Empty()
{
	CGdiPlusPng::Empty();
	if (m_hBuffer)
	{
		::GlobalUnlock(m_hBuffer);
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	} 
}


inline bool CGdiPlusPngFile::Load(LPCWSTR lpszFilePath)
{
	Empty();

	HANDLE hFile = CreateFileW(lpszFilePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if(hFile == NULL)
		return false;

	// 得到文件大小
	DWORD dwFileSize = GetFileSize(hFile, NULL);

	HGLOBAL m_hBuffer  = ::GlobalAlloc(GMEM_MOVEABLE, dwFileSize);

	if(m_hBuffer != NULL)
	{
		void* pBuffer = ::GlobalLock(m_hBuffer);
		if(pBuffer != NULL)
		{
			//CopyMemory(pBuffer, pResourceData, imageSize);
			DWORD dwBytesRead = 0;
			// 读文件到内存
			BOOL bRead = ReadFile(hFile, pBuffer, dwFileSize, &dwBytesRead, NULL);

			if(bRead)
			{
				IStream* pStream = NULL;
				if(::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK)
				{
					m_pImage = Gdiplus::Image::FromStream(pStream);
					pStream->Release();
					if(m_pImage != NULL)
					{ 
						if(m_pImage->GetLastStatus() == Gdiplus::Ok)
						{
							CloseHandle(hFile);
							return true;
						}

						delete m_pImage;
						m_pImage = NULL;
					}
				}
			}
			::GlobalUnlock(m_hBuffer);
		}
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}
	CloseHandle(hFile);
	return false;
}

class CGdiPlusPngResource : public CGdiPlusPng
{
protected:
	HGLOBAL m_hBuffer;

public:
	CGdiPlusPngResource()
	{
		m_hBuffer = NULL;
	}

	CGdiPlusPngResource(LPCTSTR pName, LPCTSTR pType = RT_RCDATA, HMODULE hInst = NULL)
	{
		m_hBuffer = NULL;
		Load(pName, pType, hInst);
	}

	CGdiPlusPngResource(UINT id, LPCTSTR pType = RT_RCDATA, HMODULE hInst = NULL)
	{
		m_hBuffer = NULL;
		Load(id, pType, hInst);
	}
	CGdiPlusPngResource(UINT id, UINT type, HMODULE hInst = NULL)
	{
		m_hBuffer = NULL;
		Load(id, type, hInst);
	}

	virtual ~CGdiPlusPngResource()
	{
		Empty();
	}

	void Empty();

	bool Load(LPCTSTR pName, LPCTSTR pType = RT_RCDATA, HMODULE hInst = NULL);

	bool Load(UINT id, LPCTSTR pType = RT_RCDATA, HMODULE hInst = NULL)
	{
		return Load(MAKEINTRESOURCE(id), pType, hInst);
	}

	bool Load(UINT id, UINT type, HMODULE hInst = NULL)
	{
		return Load(MAKEINTRESOURCE(id), MAKEINTRESOURCE(type), hInst);
	}
};

inline void CGdiPlusPngResource::Empty()
{
	CGdiPlusPng::Empty();
	if (m_hBuffer)
	{
		::GlobalUnlock(m_hBuffer);
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	} 
}

inline bool CGdiPlusPngResource::Load(LPCTSTR pName, LPCTSTR pType, HMODULE hInst)
{
	Empty();

	HRSRC hResource = ::FindResource(hInst, pName, pType);
	if(hResource == NULL)
	{
		return false;
	}

	DWORD imageSize = ::SizeofResource(hInst, hResource);
	if(imageSize == 0)
	{
		return false;
	}

	const void* pResourceData = ::LockResource(::LoadResource(hInst, hResource));
	if(pResourceData == NULL)
	{
		return false;
	}

	m_hBuffer  = ::GlobalAlloc(GMEM_MOVEABLE, imageSize);
	if(m_hBuffer != NULL)
	{
		void* pBuffer = ::GlobalLock(m_hBuffer);
		if(pBuffer != NULL)
		{
			CopyMemory(pBuffer, pResourceData, imageSize);

			IStream* pStream = NULL;
			if(::CreateStreamOnHGlobal(m_hBuffer, FALSE, &pStream) == S_OK)
			{
				m_pImage = Gdiplus::Image::FromStream(pStream);
				pStream->Release();
				if(m_pImage != NULL)
				{ 
					if(m_pImage->GetLastStatus() == Gdiplus::Ok)
					{
						return true;
					}

					delete m_pImage;
					m_pImage = NULL;
				}
			}
			::GlobalUnlock(m_hBuffer);
		}
		::GlobalFree(m_hBuffer);
		m_hBuffer = NULL;
	}
	return false;
}