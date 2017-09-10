#pragma once

#include "DUICommon.h"
#include "DUI/GifAnimUI.h"
#include "DUI/WndShadow.h"

#define Feedback_host		_T("p.101.com")
#define Feedback_UploadPicUrl	_T("/101ppt/feedback/post_pic.php?file=")
#define Feedback_PostFeedbackUrl	_T("101ppt/feedback/post_data.php")

class CFeedbackDialogUI : public WindowImplBase
{
public:
	CFeedbackDialogUI();
	~CFeedbackDialogUI();

	UIBEGIN_MSG_MAP
	EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnClose"),OnBtnCloseClick);
	EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnAddPic"),OnBtnAddPicClick);
	EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("editDescription"),OnEditDescriptionTextChanged);
	EVENT_ID_HANDLER(DUI_MSGTYPE_MOUSEENTER,_T("layoutDelete1"),OnLayoutDelete1MouseEnter);
	EVENT_ID_HANDLER(DUI_MSGTYPE_MOUSELEAVE,_T("layoutDelete1"),OnLayoutDelete1MouseLeave);
	EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,_T("btnDeletePic1"),OnBtnDeletePic1Click);
	EVENT_ID_HANDLER(DUI_MSGTYPE_MOUSEENTER,_T("layoutDelete2"),OnLayoutDelete2MouseEnter);
	EVENT_ID_HANDLER(DUI_MSGTYPE_MOUSELEAVE,_T("layoutDelete2"),OnLayoutDelete2MouseLeave);
	EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,_T("btnDeletePic2"),OnBtnDeletePic2Click);
	EVENT_ID_HANDLER(DUI_MSGTYPE_MOUSEENTER,_T("layoutDelete3"),OnLayoutDelete3MouseEnter);
	EVENT_ID_HANDLER(DUI_MSGTYPE_MOUSELEAVE,_T("layoutDelete3"),OnLayoutDelete3MouseLeave);
	EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,_T("btnDeletePic3"),OnBtnDeletePic3Click);
	EVENT_ID_HANDLER(DUI_MSGTYPE_MOUSEENTER,_T("layoutDelete4"),OnLayoutDelete4MouseEnter);
	EVENT_ID_HANDLER(DUI_MSGTYPE_MOUSELEAVE,_T("layoutDelete4"),OnLayoutDelete4MouseLeave);
	EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,_T("btnDeletePic4"),OnBtnDeletePic4Click);
	EVENT_ID_HANDLER(DUI_MSGTYPE_MOUSEENTER,_T("layoutDelete5"),OnLayoutDelete5MouseEnter);
	EVENT_ID_HANDLER(DUI_MSGTYPE_MOUSELEAVE,_T("layoutDelete5"),OnLayoutDelete5MouseLeave);
	EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,_T("btnDeletePic5"),OnBtnDeletePic5Click);
	EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,_T("btnBackFeedback"),OnBtnBackFeedbackClick);
	EVENT_ID_HANDLER(DUI_MSGTYPE_SELECTCHANGED,_T("chkResource"),OnCheckBoxSelectChanged);
	EVENT_ID_HANDLER(DUI_MSGTYPE_SELECTCHANGED,_T("chkOperation"),OnCheckBoxSelectChanged);
	EVENT_ID_HANDLER(DUI_MSGTYPE_SELECTCHANGED,_T("chkStyle"),OnCheckBoxSelectChanged);
	EVENT_ID_HANDLER(DUI_MSGTYPE_SELECTCHANGED,_T("chkFunction"),OnCheckBoxSelectChanged);
	EVENT_ID_HANDLER(DUI_MSGTYPE_SELECTCHANGED,_T("chkFluency"),OnCheckBoxSelectChanged);
	EVENT_ID_HANDLER(DUI_MSGTYPE_SELECTCHANGED,_T("chkOther"),OnCheckBoxSelectChanged);
	EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnSubmit"),OnBtnSubmitClick);
	EVENT_ID_HANDLER(DUI_MSGTYPE_CLICK,	_T("btnCloseUserContactMode"),OnBtnCloseUserContactModeClick);
	EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("editContactMode"),OnEditContactModeTextChanged);
	EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("editOperation"),OnEditOperationTextChanged);
	EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("editStyle"),OnEditStyleTextChanged);
	EVENT_ID_HANDLER(DUI_MSGTYPE_TEXTCHANGED,_T("editFunction"),OnEditFunctionTextChanged);
	UIEND_MSG_MAP

public:
	void Init(CRect rect);
private:
	virtual LPCTSTR GetWindowClassName() const;	
	virtual void InitWindow();
	virtual CDuiString GetSkinFile();
	virtual CDuiString GetSkinFolder();
	virtual CControlUI* CreateControl(LPCTSTR pstrClass);

	void OnBtnCloseClick(TNotifyUI& msg);
	void OnEditDescriptionTextChanged(TNotifyUI& msg);
	void OnBtnAddPicClick(TNotifyUI& msg);
	void AddPictures(vector<tstring> files);
	void OnLayoutDelete1MouseEnter(TNotifyUI& msg);
	void OnLayoutDelete1MouseLeave(TNotifyUI& msg);
	void OnBtnDeletePic1Click(TNotifyUI& msg);
	void OnLayoutDelete2MouseEnter(TNotifyUI& msg);
	void OnLayoutDelete2MouseLeave(TNotifyUI& msg);
	void OnBtnDeletePic2Click(TNotifyUI& msg);
	void OnLayoutDelete3MouseEnter(TNotifyUI& msg);
	void OnLayoutDelete3MouseLeave(TNotifyUI& msg);
	void OnBtnDeletePic3Click(TNotifyUI& msg);
	void OnLayoutDelete4MouseEnter(TNotifyUI& msg);
	void OnLayoutDelete4MouseLeave(TNotifyUI& msg);
	void OnBtnDeletePic4Click(TNotifyUI& msg);
	void OnLayoutDelete5MouseEnter(TNotifyUI& msg);
	void OnLayoutDelete5MouseLeave(TNotifyUI& msg);
	void OnBtnDeletePic5Click(TNotifyUI& msg);
	void OnBtnBackFeedbackClick(TNotifyUI& msg);
	void OnCheckBoxSelectChanged(TNotifyUI& msg);
	void OnBtnSubmitClick(TNotifyUI& msg);
	void OnBtnCloseUserContactModeClick(TNotifyUI& msg);
	void OnEditContactModeTextChanged(TNotifyUI& msg);
	void OnEditOperationTextChanged(TNotifyUI& msg);
	void OnEditStyleTextChanged(TNotifyUI& msg);
	void OnEditFunctionTextChanged(TNotifyUI& msg);
	void ShowLoading();
	void HideLoading();
	void ShowSubmitContent();
	void HideSubmitContent();
	void ShowFeedbackSuccess();
	void HideFeedbackSuccess();
	void ShowFeedbackFail();
	void HideFeedbackFail();
	void RefreshPictureList();
	//反馈提交相关方法
	DWORD m_dwUploadPic1TaskId;
	DWORD m_dwUploadPic2TaskId;
	DWORD m_dwUploadPic3TaskId;
	DWORD m_dwUploadPic4TaskId;
	DWORD m_dwUploadPic5TaskId;
	DWORD m_dwPostFeedbackTaskId;
	vector<tstring> m_vRemotePictures;
	void StartUploadPic1Task();
	void StopUploadPic1Task();
	bool OnUploadPic1Compeleted(void* param);
	void StartUploadPic2Task();
	void StopUploadPic2Task();
	bool OnUploadPic2Compeleted(void* param);
	void StartUploadPic3Task();
	void StopUploadPic3Task();
	bool OnUploadPic3Compeleted(void* param);
	void StartUploadPic4Task();
	void StopUploadPic4Task();
	bool OnUploadPic4Compeleted(void* param);
	void StartUploadPic5Task();
	void StopUploadPic5Task();
	bool OnUploadPic5Compeleted(void* param);
	void StartPostFeedbackTask();
	void StopPostFeedbackTask();
	bool OnPostFeedbackCompeleted(void* param);
	bool OnLoginComplete(void* pObj);
	bool OnLogoutComplete(void* pObj);
	void ShowUserContactMode(tstring strUserContact);
	void ShowContactMode();
	bool IsMailString(tstring strMail);
	bool IsPhoneString(tstring strPhone);

	tstring m_strPic1Path;
	tstring m_strPic2Path;
	tstring m_strPic3Path;
	tstring m_strPic4Path;
	tstring m_strPic5Path;
	int m_nLoginType;
	bool isContactModeModify;//表示是否需要判断联系方式
	bool isDescriptionModify;
	bool isOperationModify;
	bool isStyleModify;
	bool isFunctionModify;

	CContainerUI* m_pContainerSubmit;
	CCheckBoxUI* m_pChkResource;
	CCheckBoxUI* m_pChkOperation;
	CRichEditUI* m_pEditOperation;
	CCheckBoxUI* m_pChkStyle;
	CRichEditUI* m_pEditStyle;
	CCheckBoxUI* m_pChkFunction;
	CRichEditUI* m_pEditFunction;
	CCheckBoxUI* m_pChkFluency;
	CCheckBoxUI* m_pChkOther;
	CLabelUI* m_pLblInputLeftCount;
	CRichEditUI* m_pEditDescription;
	CButtonUI* m_pBtnAddPic;
	CContainerUI* m_pContainerPic1;
	CControlUI* m_pCtrPic1;
	CControlUI* m_pCtrDeleteFrame1;
	CButtonUI* m_pBtnDeletePic1;
	CContainerUI* m_pContainerPic2;
	CControlUI* m_pCtrPic2;
	CControlUI* m_pCtrDeleteFrame2;
	CButtonUI* m_pBtnDeletePic2;
	CContainerUI* m_pContainerPic3;
	CControlUI* m_pCtrPic3;
	CControlUI* m_pCtrDeleteFrame3;
	CButtonUI* m_pBtnDeletePic3;
	CContainerUI* m_pContainerPic4;
	CControlUI* m_pCtrPic4;
	CControlUI* m_pCtrDeleteFrame4;
	CButtonUI* m_pBtnDeletePic4;
	CContainerUI* m_pContainerPic5;
	CControlUI* m_pCtrPic5;
	CControlUI* m_pCtrDeleteFrame5;
	CButtonUI* m_pBtnDeletePic5;
	CRichEditUI* m_pEditContactMode;
	CHorizontalLayoutUI* m_pLayoutUserContactMode;
	CLabelUI* m_pLblUserContactMode;
	CButtonUI* m_pBtnSubmit;
	CVerticalLayoutUI* m_pLayoutLoading;
	CGifAnimUI* m_pGifLoading;
	CVerticalLayoutUI* m_pLayoutSuccess;
	CVerticalLayoutUI* m_pLayoutFail;
	CButtonUI* m_pBtnBackFeedback;
};