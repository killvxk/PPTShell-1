//===========================================================================
// FileName:				QRCodeImageGenerator.h
// 
// Desc:				
//============================================================================
#ifndef _QRCODE_IMAGE_GENERATOR_H_
#define _QRCODE_IMAGE_GENERATOR_H_

#include "ThirdParty/QRCode/qrencode.h"
#include "ThirdParty/json/json.h"

class CQRCodeImageGenerator
{
public:
	CQRCodeImageGenerator();
	~CQRCodeImageGenerator();
 
	void* GenQRCodeImage();
	void* GenQRCodeImage(char* szInfo, unsigned int OUT_FILE_PIXEL_PRESCALER);
	void  GenQRCodeImageToFile(tstring strFilePath);
};



#endif