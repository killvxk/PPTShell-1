#include "NdCefThumbSave.h"
#include "string.h"
#include "tchar.h"
#include <map>

#pragma comment(lib,"gdiplus")

ULONG_PTR m_gdiplusToken;
GdiplusStartupInput StartupInput;  

void CNdCefThumbSave::Initialize()
{	
	GdiplusStartup(&m_gdiplusToken,&StartupInput,NULL); 
}

void CNdCefThumbSave::UnInitialize()
{	
	GdiplusShutdown(m_gdiplusToken); 
}
int WStrReplace(std::wstring& strContent, std::wstring& strReplace, std::wstring & strDest)
{

	while (true)
	{
		size_t pos = strContent.find(strReplace);
		if (pos != std::wstring::npos)
		{
			WCHAR pBuf[1]={L'\0'};
			strContent.replace(pos, strReplace.length(), pBuf, 0);
			strContent.insert(pos, strDest);
		}
		else
		{
			break;
		}
	}

	return 0;
}
BOOL CNdCefThumbSave::SaveBmpToJpeg(HBITMAP hBmp, LPCTSTR lpszJpegFileName, ULONG quality /*= 100*/)
{  
	TCHAR sEncoderClsName[50] = {0};
	if(!GetEncoderClsName(lpszJpegFileName, sEncoderClsName, 50))
		return FALSE;

	CLSID jpgClsid;  
	if ( !GetEncoderClsid(L"image/jpeg", &jpgClsid) ) {  
		return FALSE;  
	}  

	EncoderParameters encoderParameters;  
	encoderParameters.Count = 1;  
	encoderParameters.Parameter[0].Guid = EncoderQuality;  
	encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;  
	encoderParameters.Parameter[0].NumberOfValues = 1;  
	encoderParameters.Parameter[0].Value = &quality;  

	Gdiplus::Bitmap bmp(hBmp, NULL);

	std::wstring nPath(lpszJpegFileName);
	std::wstring nMainName = _T("main.xml.jpg");
	std::wstring nMainBigName = _T("mainbig.xml.jpg");

	WStrReplace(nPath,nMainName,nMainBigName);
	Image *pImage1 = FixedSize(&bmp, 170, 110);
	Status status = pImage1->Save(lpszJpegFileName, &jpgClsid, &encoderParameters);
	Image *pImage2 = FixedSize(&bmp, 960, 720);
	Status status1 = pImage2->Save(nPath.c_str(), &jpgClsid, &encoderParameters);

	if(status==Ok)
		return TRUE;

	return FALSE;
}


bool CNdCefThumbSave::GetEncoderClsid(const WCHAR* pszFormat, CLSID* pClsid)  
{  
	UINT  unNum = 0;          // number of image encoders  
	UINT  unSize = 0;         // size of the image encoder array in bytes  

	ImageCodecInfo* pImageCodecInfo = NULL;  

	// How many encoders are there?  
	// How big (in bytes) is the array of all ImageCodecInfo objects?  
	GetImageEncodersSize( &unNum, &unSize );  
	if ( 0 == unSize ) {  
		return false;  // Failure  
	}  

	// Create a buffer large enough to hold the array of ImageCodecInfo  
	// objects that will be returned by GetImageEncoders.  
	pImageCodecInfo = (ImageCodecInfo*)( malloc(unSize) );  
	if ( !pImageCodecInfo ) {  
		return false;  // Failure  
	}  

	// GetImageEncoders creates an array of ImageCodecInfo objects  
	// and copies that array into a previously allocated buffer.   
	// The third argument, imageCodecInfos, is a pointer to that buffer.   
	GetImageEncoders( unNum, unSize, pImageCodecInfo );  

	for ( UINT j = 0; j < unNum; ++j ) {  
		if ( wcscmp( pImageCodecInfo[j].MimeType, pszFormat ) == 0 ) {  
			*pClsid = pImageCodecInfo[j].Clsid;  
			free(pImageCodecInfo);  
			pImageCodecInfo = NULL;  
			return true;  // Success  
		}      
	}  

	free( pImageCodecInfo );  
	pImageCodecInfo = NULL;  
	return false;  // Failure  
}  


BOOL CNdCefThumbSave::GetEncoderClsName(LPCTSTR lpszFileName, LPTSTR lpszBuf, DWORD dwBufSize)
{
	TCHAR* pFileExt =_tcsrchr((TCHAR *)lpszFileName, '.');
	if(NULL == pFileExt)
		return FALSE;

	_stprintf_s(lpszBuf, dwBufSize, L"image/%s", pFileExt + 1);
	return TRUE;
}


Image* CNdCefThumbSave::FixedSize(Image *imgSrc, int Width, int Height)
{
	int w,h;
	w = imgSrc->GetWidth ();
	h = imgSrc->GetHeight ();

	if (w<h) //图片是竖着的 交换Width和Height
	{
		Width = Width + Height;
		Height = Width - Height;
		Width = Width - Height;
	}
	Bitmap *bmPhoto = new Bitmap(Width, Height);
	bmPhoto->SetResolution(imgSrc->GetHorizontalResolution(),imgSrc->GetVerticalResolution());
	Graphics grPhoto(bmPhoto);
	grPhoto.Clear((ARGB)Color::White);
	grPhoto.SetInterpolationMode(InterpolationModeHighQualityBicubic);

	RectF destRect;
	destRect.X = 0;
	destRect.Y = 0;
	destRect.Width = REAL(Width);
	destRect.Height = REAL(Height);
	grPhoto.DrawImage(imgSrc,destRect,
		0,
		0,
		REAL(w),
		REAL(h),
		UnitPixel);

	return bmPhoto;
}