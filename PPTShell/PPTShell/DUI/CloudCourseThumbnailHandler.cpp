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
#include "DUI/CloudCourseHandler.h"

#include "DUI/CloudCourseThumbnailHandler.h"

#include "DUI/BaseParamer.h"
#include "DUI/InsertParamer.h"


#include "GUI/MainFrm.h"
#include "PPTControl/PPTController.h"
#include "PPTControl/PPTControllerManager.h"
#include "Util/Util.h"
#include <Windef.h>
#include "DUI/PreviewDialogUI.h"
#include "Statistics/Statistics.h"

#include "DUI/ITransfer.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "DUI/IDownloadListener.h"
#include "DUI/ResourceDownloader.h"
#include "DUI/AssetDownloader.h"
#include "DUI/CourseDownloader.h"
#include "DUI/IButtonTag.h"
#include "DUI/ItemExplorer.h"


CCloudCourseThumbnailHandler::CCloudCourseThumbnailHandler()
{

}


CCloudCourseThumbnailHandler::~CCloudCourseThumbnailHandler()
{

}


void CCloudCourseThumbnailHandler::InitHandlerId()
{
	m_strHandlerId= Ansi2Str(typeid(this).name());
	m_strHandlerId += GetPreviewUrl();
}

void CCloudCourseThumbnailHandler::ReadFrom( CStream* pStream )
{
	m_strTitle		= pStream->ReadString();
	m_strGuid		= pStream->ReadString();
	m_strUrl		= pStream->ReadString();
	m_strPreviewUrl	= pStream->ReadString();
	InitHandlerId();
}

void CCloudCourseThumbnailHandler::WriteTo( CStream* pStream )
{

}

void CCloudCourseThumbnailHandler::DoClick( TNotifyUI* pNotify )
{
	CItemHandler::DoClick(pNotify);

}


LPCTSTR CCloudCourseThumbnailHandler::GetPreviewUrl()
{
	return m_strPreviewUrl.c_str();
}
