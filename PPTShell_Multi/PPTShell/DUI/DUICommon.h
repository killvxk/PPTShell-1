#pragma once

#include "..\DuiLib\UIlib.h"

#ifndef NO_USING_DUILIB_NAMESPACE
using namespace DuiLib;
using namespace std;
#endif

#ifdef _DEBUG
#   ifdef _UNICODE
#       pragma comment(lib, "..\\Lib\\DuiLib_ud.lib")
#   else
#       pragma comment(lib, "..\\Lib\\DuiLib_d.lib")
#   endif
#else
#   ifdef _UNICODE
#       pragma comment(lib, "..\\Lib\\DuiLib_u.lib")
#   else
#       pragma comment(lib, "..\\Lib\\DuiLib.lib")
#   endif
#endif


#define UIBEGIN_MSG_MAP virtual void Notify(TNotifyUI& msg) { 
#define SUPER_HANDLER_MSG __super::Notify(msg);

//#define EVENT_HANDLER(event, func) if(msg) \
//{ \
//	func(msg);\
//	OutputDebugString(msg.sType + _T("\t\t") + msg.pSender->GetName() + _T("\r\n"));\
//}

#define EVENT_HANDLER(event, func) if( msg.sType == event && msg.pSender) \
{ \
	OutputDebugString(msg.sType + _T("\t\t") + msg.pSender->GetName() + _T("\r\n"));\
	func(msg);\
}

#define EVENT_ID_HANDLER(event, id, func) if(msg.sType == event && msg.pSender && _tcsicmp(msg.pSender->GetName(), id) == 0 ) \
{ \
	OutputDebugString(msg.sType + _T("\t\t") + msg.pSender->GetName() + _T("\r\n"));\
	func(msg);\
}

#define EVENT_ITEM_HANDLER_WITH_LIST_ID(event, id, func) if(msg.sType == event && msg.pSender && _tcsicmp(msg.pSender->GetParent()->GetParent()->GetName(), id) == 0 ) \
{ \
	OutputDebugString(msg.sType + _T("\t\t") + msg.pSender->GetParent()->GetParent()->GetName() + _T("\r\n"));\
	func(msg);\
}

#ifdef  _DEBUG
#define EVENT_PRINTER { \
	if(msg.pSender){\
		OutputDebugString(msg.sType + _T("\t\t") + msg.pSender->GetName() + _T("\r\n"));\
	}\
}
#else
#define EVENT_PRINTER
#endif



#define EVENT_BUTTON_CLICK(id,	func)		EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,			id, func)
#define EVENT_ITEM_CLICK(id,	func)		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMCLICK,		id, func)
#define EVENT_ITEM_RCLICK(id,	func)		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMRCLICK,	id, func)
#define EVENT_ITEM_LDBCLICK(id, func)		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMACTIVATE,	id, func)
#define EVENT_ITEM_SELECT(id,	func)		EVENT_ID_HANDLER(DUI_MSGTYPE_ITEMSELECT,	id, func)

#define UIEND_MSG_MAP return;} 


#define VERIFY_LPSZ(lpszVal, ret)  do { if ( (NULL == (lpszVal)) || (NULL == lpszVal[0]) ) return ret; } while ( 0 );
#define VERIFY_LPSZ_NOT_RET(lpszVal)  do { if ( (NULL == (lpszVal)) || (NULL == lpszVal[0]) ) return; } while ( 0 );
#define VERIFY_PTR(lpszVal, ret)  do { if ( (NULL == (lpszVal)) ) return ret; } while ( 0 );
#define VERIFY_PTR_NOT_RET(lpszVal)  do { if ( (NULL == (lpszVal)) ) return; } while ( 0 );


typedef enum
{
	UIFONT_GDI = 0,    /**< GDI */
	UIFONT_GDIPLUS,    /**< GDI+ */
	UIFONT_FREETYPE,   /**< FreeType */ 
	UIFONT_DEFAULT,    
} UITYPE_FONT;

#define DT_CALC_SINGLELINE		 DT_SINGLELINE| DT_LEFT_EX|DT_NOPREFIX  //计算单行
#define DT_CALC_MULTILINE		 DT_WORDBREAK | DT_EDITCONTROL | DT_LEFT_EX | DT_NOPREFIX // 计算多行

enum
{
	RES_TYPE_COURSE = 50001,
	RES_TYPE_VIDEO,
	RES_TYPE_PIC,
	RES_TYPE_FLASH,
	RES_TYPE_VOLUME,
	
	RES_TYP_TOTAL
};

#define	TOAST_TIMER_ID		10001

#define _STR_NODEJS_NO_PREPARE					_T("习题模块正在更新中，请稍候...")
#define _STR_PREVIEW_DIALOG_NOSUPPORT			_T("操作失败，该资源缺失")
#define _STR_PREVIEW_DIALOG_INSERT_OK			_T("插入成功")
#define _STR_PREVIEW_DIALOG_INSERT_FAIL			_T("插入失败，该资源缺失")
#define _STR_PREVIEW_DIALOG_UNDO_OK				_T("撤销成功")
#define _STR_PREVIEW_DIALOG_RESOURCE_NOT_FOUND	_T("该资源缺失")
#define _STR_PREVIEW_DIALOG_NOUSER				_T("请先登录用户")
#define _STR_PREVIEW_DIALOG_DOING				_T("正在操作中，请稍后...")

#define _STR_RESOURCE				_T("资源")
#define _STR_GROUPEXPLORER_NONE		_T("暂时没有")

#define _STR_RIGHTBAR_ITEM_COURSE	_T("课件")
#define _STR_RIGHTBAR_ITEM_VEDIO	_T("视频")
#define _STR_RIGHTBAR_ITEM_IMAGE	_T("图片")
#define _STR_RIGHTBAR_ITEM_FLASH	_T("动画")
#define _STR_RIGHTBAR_ITEM_VOLUME	_T("音频")
#define _STR_RIGHTBAR_ITEM_QUESTION		_T("习题")
#define _STR_RIGHTBAR_ITEM_INTERACTION	_T("互动习题")
#define _STR_RIGHTBAR_ITEM_PPTMODEL		_T("PPT模板")


#define _STR_RIGHTBAR_LOCAL_ITEM_SELECT_FOLDER				_T("请先选中或者新建一个本地文件夹")
#define _STR_RIGHTBAR_QUESTION_ITEM_PAGE_SAVE_FAILED		_T("保存page.xml操作失败")
#define _STR_RIGHTBAR_QUESTION_ITEM_MAIN_SAVE_FAILED		_T("保存main.xml操作失败")


#define _STR_LOCAL_ITEM_TIP_FILE_NO_EXIST			_T("文件已经不存在")

#define _STR_PPTCTRLMGR_TIP_PPT_INIT_FAILED			_T("初始化失败，请安装或修复PowerPoint")
#define _STR_PPTCTRLMGR_TIP_PPT_NOT_OPEN			_T("请先打开或者新建一个PPT演示文稿")
#define _STR_PPTCTRLMGR_TIP_INSERT_FAILED			_T("插入失败")
#define _STR_PPTCTRLMGR_TIP_INSERT_SUCESS			_T("插入成功")
#define _STR_PPTCTRLMGR_TIP_FINAL_PPT				_T("当前文档为最终状态文档")
#define _STR_PPTCTRLMGR_TIP_MODIFY_PPT_EDITING		_T("请先切换到PPT的编辑状态")
#define _STR_PPTCTRLMGR_TIP_OPEN_PPT_FAILED			_T("PPT演示文稿打开失败")
#define _STR_PPTCTRLMGR_TIP_SLIDE_NO_SELECT			_T("请先选中一页幻灯片")

#define _STR_PPTCTRL_TIP_APP_EXCEPTION				_T("CPPTController异常，请重视")

#define _STR_PPT_DOC_IS_EMPTY						_T("PPT演示文稿内容为空")
#define _STR_PACK_PPT_DOC_IS_EMPTY					_T("抱歉，当前没有PPT文稿数据可以保存")
#define	_STR_POWER_POINT_NOT_ACTIVE					_T("对话框打开时，PowerPoint 无法执行该操作。请先关闭对话框，然后继续")
#define	_STR_FILE_OPENED							_T("当前文档已经打开")
#define	_STR_FILE_OPENEING							_T("正在打开文档，请稍侯...")
#define	_STR_FILE_OPEN_FAST							_T("打开太过频繁，请稍侯...")
#define	_STR_FILE_OPEN_FAILED						_T("文档路径太长，打开失败")
#define	_STR_FILE_DWONLOAD_FAILED					_T("文件下载失败")
#define _STR_FILE_EXPORT_IMAGES						_T("正在生成3D演示文稿，请稍侯...")

#define _STR_SELECT_FILE_BUFFER_SMALL				_T("选择文件过多, 请重新选择")
#define _STRT_QUESTION_EXIST						_T("抱歉，习题页不能插入资源")
#define _STR_QUESTION_INSERTING						_T("习题插入中...")
#define _STR_QUESTION_SAVING						_T("习题保存中...")
#define _STR_MAGICPEN_FIRSTTIP						_T("长按并拖动选择放大区域")
#define _STR_QUESTION_TYPE_EMPTY					_T("当前习题不支持编辑")
#define _STR_VR_PACKAGE_NOT_FIND					_T("没有对应的移动端资源")

#define _STR_NO_PERMISSION							_T("当前权限无法使用")