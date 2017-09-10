#pragma once
#include "StdAfx.h"

#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/FlashStyleable.h"
#include "DUI/IButtonTag.h"


CFlashStyleable::CFlashStyleable()
{

}

CFlashStyleable::~CFlashStyleable()
{

}
ImplementStyleable(CFlashStyleable)

LPCTSTR CFlashStyleable::GetDescription()
{
	return _T("¶¯»­");
}

LPCTSTR CFlashStyleable::GetDefaultBkIamge()
{
	return _T("Item\\item_bg_flash.png");
}

LPCTSTR CFlashStyleable::GetIcon()
{
	return _T("Item\\bg_tit_flash.png");
}
