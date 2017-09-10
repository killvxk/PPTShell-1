#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/InteractQuestionStyleable.h"
#include "DUI/IButtonTag.h"


CInteractQuestionStyleable::CInteractQuestionStyleable()
{

}

CInteractQuestionStyleable::~CInteractQuestionStyleable()
{

}

int CInteractQuestionStyleable::GetButtonCount()
{
	return 1;
}

LPCTSTR CInteractQuestionStyleable::GetButtonText( int nButton )
{
	if (nButton == 0)
	{
		return _T("应用");
	}
	return _T("");
}

LPCTSTR CInteractQuestionStyleable::GetDescription()
{
	return _T("互动习题");
}

LPCTSTR CInteractQuestionStyleable::GetDefaultBkIamge()
{
	return _T("Item\\item_bg_course.png");
}

LPCTSTR CInteractQuestionStyleable::GetIcon()
{
	return _T("Item\\bg_tit_class.png");
}
