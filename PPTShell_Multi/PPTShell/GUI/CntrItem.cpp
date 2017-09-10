// 这段 MFC 示例源代码演示如何使用 MFC Microsoft Office Fluent 用户界面 
// ("Fluent UI")，该示例仅作为参考资料提供， 
// 用以补充《Microsoft 基础类参考》和 
// MFC C++ 库软件随附的相关电子文档。
// 复制、使用或分发 Fluent UI 的许可条款是单独提供的。
// 若要了解有关 Fluent UI 许可计划的详细信息，请访问  
// http://msdn.microsoft.com/officeui。
//
// 版权所有 (C) Microsoft Corporation
// 保留所有权利。

// CntrItem.cpp : CPPTShellCntrItem 类的实现
//

#include "stdafx.h"
//#include "PPTShell.h"

//#include "PPTShellDoc.h"
//#include "PPTShellView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPPTShellCntrItem 的实现

IMPLEMENT_SERIAL(CPPTShellCntrItem, COleDocObjectItem, 0)

CPPTShellCntrItem::CPPTShellCntrItem(COleDocument* pContainer)
	: COleDocObjectItem(pContainer)
{
	// TODO: 在此添加一次性构造代码
}

CPPTShellCntrItem::~CPPTShellCntrItem()
{
	// TODO: 在此处添加清理代码
}

void CPPTShellCntrItem::OnChange(OLE_NOTIFICATION nCode, DWORD dwParam)
{
	ASSERT_VALID(this);

	COleDocObjectItem::OnChange(nCode, dwParam);

	// 在编辑某项时(就地编辑或完全打开进行编辑)，
	//  该项将就其自身状态的更改
	//  或其内容的可视外观状态的更改发送 OnChange 通知。

	// TODO: 通过调用 UpdateAllViews 使该项无效
	//  (包括适合您的应用程序的提示)

	GetDocument()->UpdateAllViews(NULL);
		// 现在只更新所有视图/无提示
}

BOOL CPPTShellCntrItem::OnChangeItemPosition(const CRect& rectPos)
{
	ASSERT_VALID(this);

	// 就地激活期间，服务器调用 CPPTShellCntrItem::OnChangeItemPosition
	//  以更改就地窗口的位置。
	//  这通常是由于服务器
	//  文档中的数据更改而导致范围改变或是
	//  就地调整大小所引起的。
	//
	// 此处的默认设置调用基类，该基类将调用
	//  COleDocObjectItem::SetItemRects 以将该项
	//  移动到新的位置。

	if (!COleDocObjectItem::OnChangeItemPosition(rectPos))
		return FALSE;

	// TODO: 更新该项的矩形/范围可能存在的任何缓存

	return TRUE;
}


void CPPTShellCntrItem::OnActivate()
{
}

void CPPTShellCntrItem::OnDeactivateUI(BOOL bUndoable)
{
	COleDocObjectItem::OnDeactivateUI(bUndoable);

    DWORD dwMisc = 0;
    m_lpObject->GetMiscStatus(GetDrawAspect(), &dwMisc);
    if (dwMisc & OLEMISC_INSIDEOUT)
        DoVerb(OLEIVERB_HIDE, NULL);
}

void CPPTShellCntrItem::Serialize(CArchive& ar)
{
	ASSERT_VALID(this);

	// 首先调用基类以读取 COleDocObjectItem 数据。
	// 因为这样会设置从 CPPTShellCntrItem::GetDocument 返回的 m_pDocument 指针，
	//  所以最好先调用
	//  基类 Serialize。
	COleDocObjectItem::Serialize(ar);

	// 现在存储/检索特定于 CPPTShellCntrItem 的数据
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}


// CPPTShellCntrItem 诊断

#ifdef _DEBUG
void CPPTShellCntrItem::AssertValid() const
{
	COleDocObjectItem::AssertValid();
}

void CPPTShellCntrItem::Dump(CDumpContext& dc) const
{
	COleDocObjectItem::Dump(dc);
}
#endif

