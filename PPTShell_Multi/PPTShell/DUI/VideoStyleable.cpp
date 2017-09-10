#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/VideoStyleable.h"
#include "DUI/IButtonTag.h"


CVideoStyleable::CVideoStyleable()
{

}

CVideoStyleable::~CVideoStyleable()
{

}

ImplementStyleable(CVideoStyleable)


LPCTSTR CVideoStyleable::GetDescription()
{
	return _T("视频");
}

LPCTSTR CVideoStyleable::GetDefaultBkIamge()
{
	return _T("Item\\item_bg_video.png");
}

LPCTSTR CVideoStyleable::GetIcon()
{
	return _T("Item\\bg_tit_video.png");
}
