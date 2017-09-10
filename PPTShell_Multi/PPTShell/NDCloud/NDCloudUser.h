//-----------------------------------------------------------------------
// FileName:				NDCloudUser.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _ND_CLOUD_USER_H_
#define _ND_CLOUD_USER_H_

#include "Util/Singleton.h"
#include "Util/Stream.h"
#include "Util/RSA.h"
#include "Http/HttpDelegate.h"
#include "Http/HttpDownload.h"
#include "Http/HttpDownloadManager.h"
#include "json/forwards.h"
#include "json/value.h"

enum
{
	UCSTEP_NONE					= 0,
	UCSTEP_LOGIN,	
	UCSTEP_USERINFO,
	UCSTEP_CLASSROOM,
	UCSTEP_STUDENTINFO,
	UCSTEP_GROUPINFO,
	UCSTEP_DEVICEINFO,
};

enum
{
	BEARER_TOKEN				= 0,
	CHECK_BEARERTOKEN
};

typedef struct _GetBackPwdParam
{
	DWORD dwErrorCode;
	tstring strCode;
	tstring strAccount;
	tstring strEmail;
	tstring strPhone;
	tstring strMessage;
	tstring strTempPwd;
}GetBackPwdParam, *pGetBackPwdParam;

class CNDCloudUser
{
public:
	CNDCloudUser();
	~CNDCloudUser();

public:
	vector<Json::Value> GetGradeList();
	BOOL			Login(tstring strUserName, tstring strPassword, tstring strOrganize, CHttpDelegateBase& delegate);
	void				LogOut();//退出登录 2015.11.20 cws
	BOOL			MobileLogin(CStream *loginStream, CHttpDelegateBase& delegate);
	BOOL			ThirdLogin(tstring strUserName, tstring strPassword, tstring strOrganize, CHttpDelegateBase& delegate);
	BOOL			TokenLogin(tstring strHost, tstring strUrl, tstring strMethod, tstring strMac, tstring strNonce, tstring strToken, CHttpDelegateBase& delegate);
	void				UndoMobileLogin();

	BOOL			Register(tstring strUserName, tstring strPassword, tstring strCheckCode, CHttpDelegateBase& delegate);
	BOOL			ChangePassword(tstring strOldPassword, tstring strNewPassword, CHttpDelegateBase& delegate);
	BOOL			QueryLoginNameExist(tstring strUserName, CHttpDelegateBase& delegate);
	BOOL			ObtainCheckCodeImage(CHttpDelegateBase& delegate);
	BOOL			SendRegisterShortMessage(tstring strPhoneNumber, CHttpDelegateBase& delegate);
	BOOL			SendActiveEmail(tstring strEmail, CHttpDelegateBase& delegate);
	string			MD5EncryptString(string content);
	BOOL			IsLogin();		//判断用户是否已登录

	// bearer_tokens_login
	BOOL			BearerTokensLogin(tstring strSessionId,CHttpDelegateBase& delegate);
	BOOL			GetBearerTokens(int nType);
	tstring			GetBearerToken();
	BOOL			CheckBearerTokens(int nType);

	// avatar
	BOOL			GetUserAvatar(DWORD dwSize, CHttpDelegateBase& delegate);
	BOOL			SetUserAvatar(tstring strFilePath, CHttpDelegateBase& delegate);
	BOOL			SetUserAvatarWithSession();
	BOOL			GetUserAvatarSession();

	// getback password
	BOOL			GetBackPwdConfirmAccount( tstring strUserName, tstring strCheckCode, CHttpDelegateBase& delegate );
	BOOL			GetBackPwdCheckEmail( tstring strEmail, tstring strEmailCode, CHttpDelegateBase& delegate );
	BOOL			GetBackPwdCheckPhone( tstring strPhoneNumber, tstring strPhoneCode, CHttpDelegateBase& delegate );
	BOOL			GetBackPwdResetPassword( tstring strAccount, tstring strPassword, tstring strTempPassword, CHttpDelegateBase& delegate );
	BOOL			SendGetBackPwdSMS( tstring strPhoneNumber, CHttpDelegateBase& delegate );
	BOOL			SendGetBackPwdEmail( tstring strEmail, CHttpDelegateBase& delegate );

	// cancel
	void			CancelLogin();
	void			CancelThirdLogin();
	void			CancelRegister();
	void			CancelQueryLoginNameExist();
	void			CancelObtainCheckCodeImage();
	void			CancelSendRegisterShortMessage();
	void			CancelSendActiveEmail();
	void			CancelOperaUserAvatar();

	// get authorization
	string			GetAuthorizationHeader(string strHost, string strUrl, string strMethod);

	int				GetCurStep();
	BOOL			IsSuccess();
	BOOL			IsLoginNameExist();
	DWORD			GetUserId();
	tstring			GetUserName();
	tstring			GetPassword();
	tstring			GetOriganize();
	tstring			GetErrorCode();
	tstring			GetErrorMessage();
	tstring			GetNickName();
	tstring			GetRealName();
	tstring			GetSex();
	tstring			GetSubjects();
	tstring			GetOrgNodeName();
	tstring			GetFullName();//包含学校和班级等信息  示例"学校节点|福州教育学院附属第三小学|年级|小五|7班"	
	tstring			GetTeacherGrade(tstring sFullName);//老师的班级信息 
	void			SetFreeMode(int nFreeMode);
	int				GetIsFreeMode() { return m_nIsFreeMode;}//是否是自由授课模式
	void			SetCurGradeIndex(int nIndex) { m_nGradeIndex = nIndex;}
	int				GetCurGradeIndex() { return m_nGradeIndex;}
	BOOL			GetIsLoadAllTeacherInfo();// 是否已经加载完全老师的信息 2016.02.17 cws
	tstring			GetLoadALLTeacherTip();//是否加载完全信息提示
	void			ReLoadTeachRoomInfo(tstring sClassID);//根据班级ID信息重新加载教室信息
	void			ReLoadTeachRoomInfo();//根据班级索引信息重新加载教室信息
	bool			CheckItemsSize(tstring strValue, tstring strItemName);//检测json串是否有子串

	BOOL			IcrOnStartInit(CStream* pStream);//进行数据初始化 2016.02.23 cws
	BOOL			IcrStart();
	CStream*		GetIcrStartInfo() {return m_pFullScreenStream;}

	
	bool			GetLoginComplete() {return m_bLoginComplete;}//登陆完成
	void			SetLoginComplete(bool bComplete) {m_bLoginComplete = bComplete;}
	void			SetFullName(tstring sNewFullName) { m_strFullName = sNewFullName;}
	CStream*		GetClassRooms();
	void			SetClassGuid(tstring strClassGuid);
	tstring			GetClassGuid();
	tstring			GetLoginResponse();

	tstring			GetStudentInfo();
	tstring			GetGroupInfo();
	tstring			GetDeviceInfo();
	tstring			GetTeacherPhotoPath(){ return m_strTeacherPhoto;}//老师头像
	void			GetTeacherPhoto();//老师头像回调

	tstring			GetAccessToken();
	tstring			GetMacKey();

	int GetLoginType();
	void SetLoginType(int loginType);

	void SetMobileLoginRqstTime(time_t tmNow) { m_tLastMobileRqst = tmNow; }
	time_t GetLastMobileLoginRqstTime() { return m_tLastMobileRqst; }


	DECLARE_SINGLETON_CLASS(CNDCloudUser);

protected:
	bool			OnUserLoginResponse(void* param);
	bool			OnUserBearerLoginResponse(void* param);
	bool			OnUserInfoObtained(void* param);
	bool			OnClassRoomObtained(void* param);
	bool			OnStudentInfoObtained(void* param);
	bool			OnGroupInfoObtained(void* param);
	bool			OnDeviceInfoObtained(void* param);

	bool			OnCheckCodeImageObtained(void* param);
	bool			OnUserRegisterResponse(void* param);
	bool			OnUserChangePassResponse(void* param);
	bool			OnRegisterShortMessageSent(void* param);
	bool			OnLoginExistQueried(void* param);
	bool			OnActiveEmailSent(void* param);
	bool			OnGetBackPwdConfirmAccountResponse(void* param);
	bool			OnGetBackPwdResetResponse(void* param);
	bool			OnResetPasswordResponse(void* param);
	bool			OnMotifyResponse(void* param);
	
	bool			OnBearerTokensLoginResponse(void* param);
	bool			OnGetBearerTokensResponse(void* param);
	bool			OnCheckBearerTokensResponse(void* param);

	//avatar
	bool			OnGetUserAvatarSessionResponse(void* param);
	bool			OnSetUserAvatarResponse(void* param);
	//
	string			GenerateRandomString();
	BOOL			LoadHistoryData();
	BOOL			SaveHistoryData();

	bool			OnCreateUserPhoto(void * pParam);//传图片到服务端
	bool			OnGetUserPhoto(void * pParam);//从服务端获取头像
	bool			GetUserAvatarStart( void* pParam );
	void			setLoadTeacherInfoState(BOOL bSuccess);//设置获取老师信息的完成状态
	void			LoadTeachRoomInfo(tstring sOrgID, tstring sClassID);
protected:
	int							m_nCurStep;
	BOOL						m_bSuccess; 
	BOOL						m_bUserInfoSuccess;//获取老师信息
	BOOL						m_bClassRoomInfoSuccess;//获取老师班级所有信息
	BOOL						m_bStudentInfoSuccess;//获取学生信息
	BOOL						m_bDeviceInfoSuccess;//获取设备信息 
	BOOL						m_bGroupInfoSuccess;//获取学生分组信息 
	BOOL						m_bLoginNameExist;

	CHttpEventSource			m_OnCompleteLogin;
	CHttpEventSource			m_OnCompleteRegister;
	CHttpEventSource			m_OnCompleteChangePassword;
	CHttpEventSource			m_OnCompleteGetBackPwd;
	CHttpEventSource			m_OnCompleteCheckCode;
	CHttpEventSource			m_OnCompleteSendSms;
	CHttpEventSource			m_OnCompleteQueryLoginExist;
	CHttpEventSource			m_OnCompleteSendActiveEmail;
	CHttpEventSource			m_OnCompleteGetAvatar;
	CHttpEventSource			m_OnCompleteSetAvatar;

	tstring						m_strUserName;
	tstring						m_strPassword;
	tstring						m_strOriganize;

	// Login Response
	DWORD						m_dwUserId;
	tstring						m_strMacKey;
	tstring						m_strAccessToken;
	tstring						m_strLoginResponse;

	tstring						m_strErrorCode;
	tstring						m_strErrorMessage;

	// User Information
	tstring						m_strNickName;
	tstring						m_strRealName;
	tstring						m_strSex;
	tstring						m_strSubjects;
	tstring						m_strOrgNodeName;
	tstring						m_strFullName;//学校的全称
	tstring						m_strTeacherGrade;//老师的班级信息
	int							m_nIsFreeMode;//自由授课模式 2016.02.16
	
	tstring						m_strTeacherPhoto;//老师头像2016.01.28 cws
	int							m_nGradeIndex;//个人中心窗口是每次销毁，先老师班级的索引值在这里 2016.02.19 cws
	UINT64						m_ulOrgId;
	bool						m_bLoginComplete;//所有登陆操作完成2016.01.27
	INT64						m_dwServerTime;
	INT64						m_dwTickCout;

	// Classroom Information
	CStream*					m_pClassroomStream;
	tstring						m_strClassGuid;

	// Student Information
	CStream*					m_pStudentInfoStream;
	CStream*					m_pGroupInfoStream;
	CStream*					m_pDeviceInfoStream;

	CStream*					m_pFullScreenStream;

	// RSA
	CRSA						m_RSA;

	// Cookie
	tstring						m_CheckCodeSession;

	// Bearer Token
	tstring						m_strBearerSessionId;
	tstring						m_strBearerToken;

	// backup user data to file
	map<DWORD, CStream*>		m_mapUserHistoryData;

	// avatar
	tstring						m_strAvatarSession;
	tstring						m_strAvatarFilePath;
	tstring						m_strAvatarServerPath;
	//

	int m_nLoginType;

	time_t m_tLastMobileRqst;


};

typedef Singleton<CNDCloudUser>		NDCloudUser;

#endif