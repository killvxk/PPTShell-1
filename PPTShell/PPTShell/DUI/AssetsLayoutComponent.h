#pragma once
#include "DUI/GifAnimUI.h"

class CAssetsLayoutComponent
{
protected:
	CGifAnimUI*					m_pLoadingGif;							//获取当前gif

	CTileLayoutUI*				m_pContentLayout;			//数据列表
	CVerticalLayoutUI*			m_pLoadingLayout;			//loading界面
	CVerticalLayoutUI*			m_pNetlessLayout;			//网络失败界面
	CContainerUI*				m_pContainerLayout;

};