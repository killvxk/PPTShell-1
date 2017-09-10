#include "StdAfx.h"
#include "DUI/IItemHandler.h"
#include "DUI/INotifyHandler.h"
#include "NDCloud/NDCloudAPI.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "Util/Stream.h"
#include "DUI/IStreamReader.h"
#include "DUI/IVisitor.h"
#include "DUI/ItemHandler.h"
#include "DUI/CloudResourceHandler.h"
#include "DUI/IThumbnailListener.h"
#include "DUI/CloudPhotoHandler.h"
#include "DUI/CloudThumbnailHandler.h"

#include "Util/Util.h"


ImplementHandlerId(CCloudThumbnailHandler);
CCloudThumbnailHandler::CCloudThumbnailHandler()
{

}


CCloudThumbnailHandler::~CCloudThumbnailHandler()
{

}

void CCloudThumbnailHandler::DoClick( TNotifyUI* pNotify )
{
	CItemHandler::DoClick(pNotify);
}

void CCloudThumbnailHandler::ReadFrom( CStream* pStream )
{
	m_strGuid		= pStream->ReadString();
	m_strTitle		= pStream->ReadString();
	m_strUrl		= pStream->ReadString();
	InitHandlerId();
}
