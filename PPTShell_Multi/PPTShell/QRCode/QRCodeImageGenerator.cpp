//===========================================================================
// FileName:				QRCodeImageGenerator.cpp
// 
// Desc:				
//============================================================================
#include "stdafx.h"
#include "QRCodeImageGenerator.h"
#include "Util/Util.h"
#include "BlueTooth/BlueTooth.h"
#include "PPTShell.h"

CQRCodeImageGenerator::CQRCodeImageGenerator()
{

}

CQRCodeImageGenerator::~CQRCodeImageGenerator()
{

}

void* CQRCodeImageGenerator::GenQRCodeImage()
{
	// http://p.101.com/down/?&PPTID&BlueMac&IP1&IP2&IP3

	// get first local radio address
	tstring strBthAddress = BlueToothMonitor::GetInstance()->GetFirstBthDeviceAddress();
	CPPTShellApp* pApp = (CPPTShellApp*)AfxGetApp();
	DWORD dwPPTID = pApp->GetPPTID();
	
	vector<pair<string, string>> vecIpMacs;
	GetLocalIpMacs(vecIpMacs);

	// ÄÚÍøµØÖ·ÊÇhttp://p.101.com/down/?&%u&%s
	char szInfo[MAX_PATH];
	sprintf_s(szInfo, "http://ppt.101.com/?&%u&%s", dwPPTID, strBthAddress.c_str());

	for(int i = 0; i < (int)vecIpMacs.size(); i++)
	{
		pair<string, string> p = vecIpMacs[i];

		strcat(szInfo, "&");
		strcat(szInfo, p.first.c_str());
	}

	void* pImage = GenQRCodeImage(szInfo, 10);
	return pImage;

}

void CQRCodeImageGenerator::GenQRCodeImageToFile(tstring strFilePath)
{
	Gdiplus::Bitmap* pImage = (Gdiplus::Bitmap*)GenQRCodeImage();
	if( pImage == NULL )
		return;

	wstring wstr = Str2Unicode(strFilePath);

	GUID png = {0x557CF406, 0x1A04, 0x11D3, 0x9A, 0x73, 0x00, 0x00, 0xF8, 0x1E, 0xF3, 0x2E};
	pImage->Save(wstr.c_str(), &png);

	delete pImage;
}

//
// generate from string
//
void* CQRCodeImageGenerator::GenQRCodeImage(char* szInfo, unsigned int OUT_FILE_PIXEL_PRESCALER)
{
	unsigned int	unWidth, x, y, l, n, unWidthAdjusted, unDataBytes;
	unsigned char*  pRGBData = NULL, *pSourceData, *pDestData;
	QRcode*			pQRC;

	pQRC = QRcode_encodeString(szInfo, 0, QR_ECLEVEL_H, QR_MODE_8, 1);
	if( pQRC == NULL )
		return NULL;

	unWidth = pQRC->width;
	unWidthAdjusted = unWidth * OUT_FILE_PIXEL_PRESCALER * 3;
	if (unWidthAdjusted % 4)
		unWidthAdjusted = (unWidthAdjusted / 4 + 1) * 4;

	unDataBytes = unWidthAdjusted * unWidth * OUT_FILE_PIXEL_PRESCALER;

	// Allocate pixels buffer
	if( !(pRGBData = (unsigned char*)malloc(unDataBytes)) )
		return NULL;

	unsigned int unBMPBytesLen = unDataBytes+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER);
	memset(pRGBData, 0xff, unDataBytes);

	HGLOBAL hMem = GlobalAlloc(GMEM_FIXED,unBMPBytesLen);
	BYTE* pmem = (BYTE*)GlobalLock(hMem);

	// Prepare bmp headers
	BITMAPFILEHEADER kFileHeader;

	kFileHeader.bfType		= 0x4d42;  // "BM"
	kFileHeader.bfSize		= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + unDataBytes;
	kFileHeader.bfReserved1 = 0;
	kFileHeader.bfReserved2 = 0;
	kFileHeader.bfOffBits	= sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	memcpy(pmem,&kFileHeader,sizeof(BITMAPFILEHEADER));

	BITMAPINFOHEADER kInfoHeader;

	kInfoHeader.biSize			= sizeof(BITMAPINFOHEADER);
	kInfoHeader.biWidth			= unWidth * OUT_FILE_PIXEL_PRESCALER;
	kInfoHeader.biHeight		= -(int)(unWidth * OUT_FILE_PIXEL_PRESCALER);
	kInfoHeader.biPlanes		= 1;
	kInfoHeader.biBitCount		= 24;
	kInfoHeader.biCompression	= BI_RGB;
	kInfoHeader.biSizeImage		= 0;
	kInfoHeader.biXPelsPerMeter = 0;
	kInfoHeader.biYPelsPerMeter = 0;
	kInfoHeader.biClrUsed		= 0;
	kInfoHeader.biClrImportant	= 0;

	memcpy(pmem+sizeof(BITMAPFILEHEADER),&kInfoHeader,sizeof(BITMAPINFOHEADER));

	// Convert QrCode bits to bmp pixels
	pSourceData = pQRC->data;
	for(y = 0; y < unWidth; y++)
	{
		pDestData = pRGBData + unWidthAdjusted * y * OUT_FILE_PIXEL_PRESCALER;
		for(x = 0; x < unWidth; x++)
		{
			if (*pSourceData & 1)
			{
				for(l = 0; l < OUT_FILE_PIXEL_PRESCALER; l++)
				{
					for(n = 0; n < OUT_FILE_PIXEL_PRESCALER; n++)
					{
						*(pDestData +		n * 3 + unWidthAdjusted * l) =	0;
						*(pDestData + 1 +	n * 3 + unWidthAdjusted * l) =	0;
						*(pDestData + 2 +	n * 3 + unWidthAdjusted * l) =	0;
					}
				}
			}

			pDestData += 3 * OUT_FILE_PIXEL_PRESCALER;
			pSourceData++;
		}
	}

	memcpy(pmem+sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER), pRGBData, unDataBytes);

	IStream* pIStream = NULL;
	CreateStreamOnHGlobal(hMem, TRUE, &pIStream);

	Gdiplus::Bitmap *pImage = Gdiplus::Bitmap::FromStream(pIStream, TRUE);

	if(pIStream)
	{
		pIStream->Release();
		pIStream = NULL;
	}

	GlobalUnlock(hMem);
	//GlobalFree(hMem);

	if(pRGBData)
	{
		free(pRGBData);
		pRGBData = NULL;
	}

	if(pQRC)
		QRcode_free(pQRC);

	return pImage;
}