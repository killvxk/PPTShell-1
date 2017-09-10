/// @file       UILabelEx.h
/// @brief      支持自动换行的LabelUI, 
/// 修改了 DuiLib::CLabelUI::PaintText 为虚函数, LabelUIEx继承自CLabelUI
/// 重载了 virtual void PaintText(HDC hDC);


#ifndef __UI_LABEL_EX_H__
#define __UI_LABEL_EX_H__

#pragma once

namespace DuiLib
{
	class UILIB_API CLabelUIEx :
		public DuiLib::CLabelUI
	{
	public:
		CLabelUIEx(void);
		virtual ~CLabelUIEx(void);


		virtual void PaintText(HDC hDC);
		virtual void SetPos(RECT rc);


	private:
		Color _MakeRGB(int a, Color cl);
		Color _MakeRGB(int r, int g, int b);
	};
}


#endif

