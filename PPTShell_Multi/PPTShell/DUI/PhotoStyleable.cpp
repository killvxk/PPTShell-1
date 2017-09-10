#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/PhotoStyleable.h"


CPhotoStyleable::CPhotoStyleable()
{

}

CPhotoStyleable::~CPhotoStyleable()
{

}

ImplementStyleable(CPhotoStyleable)


LPCTSTR CPhotoStyleable::GetDescription()
{
	return _T("图片");
}

LPCTSTR CPhotoStyleable::GetDefaultBkIamge()
{
	return _T("Item\\item_bg_image.png");
}

LPCTSTR CPhotoStyleable::GetIcon()
{
	return _T("Item\\bg_tit_pic.png");
}


