#pragma once
#include "StdAfx.h"
#include "DUI/IComponent.h"
#include "DUI/ResourceStyleable.h"
#include "DUI/QuestionStyleable.h"
#include "DUI/IButtonTag.h"



CQuestionStyleable::CQuestionStyleable()
{

}

CQuestionStyleable::~CQuestionStyleable()
{

}
ImplementStyleable(CQuestionStyleable)

LPCTSTR CQuestionStyleable::GetDescription()
{
	return _T("œ∞Ã‚");
}

LPCTSTR CQuestionStyleable::GetDefaultBkIamge()
{
	return _T("Item\\item_bg_course.png");
}

LPCTSTR CQuestionStyleable::GetIcon()
{
	return _T("Item\\bg_tit_class.png");
}
