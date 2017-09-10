//-----------------------------------------------------------------------
// FileName:				NDCloudUser.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "NDCloudUser.h"
#include "Util/Util.h"
#include "Util/md5.h"
#include "Util/SHA.h"
#include "Util/Base64.h"
#include "ThirdParty/json/json.h"
#include "Http/HttpUploadManager.h"
#include "NDCloudAPI.h"
#include "Plugins/Icr/IcrPlayer.h"

vector<Json::Value>	g_vecsFullNameList;//学校全称的列表2016.02.15



// user login
#define UserCenterHost				_T("aqapi.101.com")
#define UserCenterLoginUrl			_T("/v0.93/tokens")
#define UserCenterThirdLoginUrl		_T("/v0.9/third_tokens")
#define UserCenterUserInfo			_T("/v0.93/users")
#define UserCenterCheckTokenUrl		_T("/v0.93/tokens/%s/actions/valid")

#define ClassroomHost				_T("admin-new.edu.web.sdp.101.com")
#define ClassroomClassInfo			_T("/v06/classes/teaching/%d")
#define ClassroomStudentInfo		_T("/classes/%s/%s/students")
#define ClassroomGroupInfo			_T("/v06/groups/%s")
#define ClassroomDeviceInfo			_T("/v06/devices/classes/%s")

// user register
#define UserEspHost					_T("esp-user.edu.web.sdp.101.com")
#define UserEspCheckCodeUrl			_T("/check_code")
#define UserEspRegisterUrl			_T("/v0.1/users")
#define UserEspSendRegSMS			_T("/v0.1/users/actions/send_reg_sms")
#define UserEspQueryUserExist		_T("/v0.1/users/actions/loginname_exists?login_name=")
#define UserEspSendActiveEmail		_T("/v0.1/users/actions/send_active_email")

// user changepassword
#define UserCenterChangePassUrl		_T("/v0.93/users/%d/password/actions/modify")

// user resetpassword
#define UserEspConfirmAccountUrl	_T("/v0.1/users/actions/confirm_account")
#define UserEspResetSMSUrl			_T("/v0.1/users/actions/send_reset_password_sms")
#define UserEspResetEmailUrl		_T("/v0.1/users/actions/send_reset_password_email")
#define UserEspResetPasswordUrl		_T("/v0.1/users/actions/reset_password")
#define UserEspModifyUrl			_T("/v0.1/users/actions/modify_password_bytemppwd")

// user avatar
#define EduPlatformHostRes			_T("cs.101.com")
#define UserAvatarSessionHost		_T("cscommon.web.sdp.101.com")
#define UserSessionAvatar			_T("/v0.1/csession/avatar")
#define UserGetAvatar				_T("/v0.1/static/cscommon/avatar/%d/%d.jpg?size=%d")
#define UserSetAvatar				_T("/v0.1/upload?session=%s")


#define RsaPublicKey				"-----BEGIN PUBLIC KEY-----\nMFwwDQYJKoZIhvcNAQEBBQADSwAwSAJBALuL/F15axlaMHuG6UkBBbjtS0citT51\nM15fkxnmBS4C/YGW81TnLndhKO8zxMO+KCWQTpzsHYuZ1xizpoPywKUCAwEAAQ==\n-----END PUBLIC KEY-----\n"

#define BrarerLoginHost				_T("esp-sso.edu.web.sdp.101.com")
#define BearerTokensUrl				_T("/v0.93/bearer_tokens")
#define CheckBearerTokenUrl			_T("/v0.9/bearer_tokens/%s/actions/valid")
#define BearerLogin					_T("/v0.1/get_token_info?session_id=%s")
#define BearerTokensName			_T("pptshell")
#define BearerTokensPass			_T("80fba977d063a6f7262a8a9c95f61140")


CNDCloudUser::CNDCloudUser()
{
	m_nCurStep					= UCSTEP_NONE;
	m_bSuccess					= FALSE;
	m_pClassroomStream			= new CStream(1024);
	m_pStudentInfoStream		= new CStream(1024);
	m_pGroupInfoStream			= new CStream(1024);
	m_pDeviceInfoStream			= new CStream(1024);
	m_pFullScreenStream			= new CStream(1024);
	m_dwUserId					= 0;
	m_strClassGuid				= _T("");
	m_strBearerToken			= _T("");
	m_nLoginType = -1;//未登陆，类型设置-1；0为正常登陆，1为新浪微博登陆，2为QQ登陆，3为99通行证登陆，4为手机自动登录
	m_tLastMobileRqst = 0;
	m_bLoginComplete= false;
	m_RSA.LoadPublicKey(RsaPublicKey);

	m_strFullName	 			= _T("");
	m_strTeacherGrade			= _T("");
	m_strTeacherPhoto 			= _T("");
	m_nIsFreeMode				= 1;
	m_nGradeIndex				= 0;
	m_dwTickCout				= 0;
	setLoadTeacherInfoState(FALSE);
	// load user history data

}

CNDCloudUser::~CNDCloudUser()
{
	 if( m_pClassroomStream != NULL )
	 {
		 delete m_pClassroomStream;
		 m_pClassroomStream = NULL;
	 }

	 if( m_pStudentInfoStream != NULL )
	 {
		 delete m_pStudentInfoStream;
		 m_pStudentInfoStream = NULL;
	 }

	 if( m_pGroupInfoStream != NULL )
	 {
		 delete m_pGroupInfoStream;
		 m_pGroupInfoStream = NULL;
	 }

	 if( m_pDeviceInfoStream != NULL )
	 {
		 delete m_pDeviceInfoStream;
		 m_pDeviceInfoStream = NULL;
	 }
	 if (m_pFullScreenStream != NULL)
	 {
		 delete m_pFullScreenStream;
		 m_pFullScreenStream = NULL;
	 }
}

void CNDCloudUser::setLoadTeacherInfoState(BOOL bSuccess)
{ 
	m_bUserInfoSuccess = bSuccess;//获取老师所有信息
	m_bClassRoomInfoSuccess = bSuccess;//获取老师班级所有信息
	m_bStudentInfoSuccess = bSuccess;//获取学生信息
	m_bDeviceInfoSuccess = bSuccess;//获取设备信息
	m_bGroupInfoSuccess = bSuccess;//获取学生分组信息 
}
//-----------------------------------------------------------

BOOL CNDCloudUser::IsLogin()
{
	if (m_dwUserId>0)
	{
		return TRUE;
	}
	return FALSE;
}

BOOL CNDCloudUser::MobileLogin(CStream *loginStream, CHttpDelegateBase& delegate)
{
	if (IsLogin())
	{
		WRITE_LOG_LOCAL(_T("CNDCloudUser::MobileLogin, user login already!"));
		return TRUE;
	}

	if (loginStream == NULL)
	{
		WRITE_LOG_LOCAL(_T("CNDCloudUser::MobileLogin, in param valid!"));
		return FALSE;
	}

	if( delegate.GetFn() != NULL )
		m_OnCompleteLogin += delegate;

	tstring strUserName = loginStream->ReadString();
//	string strPassword = loginStream->ReadString();
	DWORD dwUserID = loginStream->ReadDWORD();
	if (dwUserID <= 0)
	{
		WRITE_LOG_LOCAL(_T("CNDCloudUser::MobileLogin, userid is error:%d"), dwUserID);
		return FALSE;
	}
	tstring strMacKey		= loginStream->ReadString();
	tstring strAccessToken	= loginStream->ReadString();
	tstring strRealName		= loginStream->ReadString();
	tstring strSex			= loginStream->ReadString();
	tstring strSubject		= loginStream->ReadString();
	tstring strOrgNodeName	= loginStream->ReadString();
	tstring strOrgID		= loginStream->ReadString();
	UINT64 ulOrgID			= 0;
	try
	{
		_stscanf_s(strOrgID.c_str(), _T("%I64u"), &ulOrgID);
	}
	catch (CException* e)
	{
		ulOrgID = 0;
		WRITE_LOG_LOCAL(_T("org id have invalid value:%s"), strOrgID);
		return FALSE;
	}
	
	WRITE_LOG_LOCAL(_T("strUserName:%s, userId:%d, mackey:%s, accesstoken:%s"), strUserName.c_str(), dwUserID, strMacKey.c_str(), strAccessToken.c_str());
	WRITE_LOG_LOCAL(_T("realname:%s, sex:%s, orgname;%s, orgid:%s"), strRealName.c_str(), strSex.c_str(), strOrgNodeName.c_str(), strOrgID.c_str());

	if (strAccessToken.empty()  || strMacKey.empty() || strOrgID.empty())
	{
		return FALSE;
	}

	m_strUserName= strUserName;
//	m_strPassword = "wff1234567";
	m_dwUserId = dwUserID;
	m_strRealName = strRealName;
	m_strSex = strSex;
	m_strSubjects = strSubject;
	m_strOrgNodeName = strOrgNodeName;
	m_ulOrgId = ulOrgID;
	m_strMacKey = strMacKey;
	m_strAccessToken = strAccessToken;
	m_strOriganize = _T("org_esp_app_prod");

// 	m_strUserName="260823@nd";
// 	//	m_strPassword = "wff1234567";
// 	m_dwUserId = 260823;
// 	m_strRealName = "王芳芳";
// 	m_strSex = "";
// 	m_strSubjects = "";
// 	m_strOrgNodeName = "程序中心游戏工具开发部游戏安全组";
// 	m_ulOrgId = 481036337156;
// 	m_strMacKey = "SguTqMnV5a";
// 	m_strAccessToken = "3AE286D8CD3B0999E110FA51C5A173EA490513A128E07DA26501606A17A4A5DD";
// 	m_strOriganize = "org_esp_app_prod";

	m_bSuccess = TRUE;
	m_nCurStep = UCSTEP_LOGIN;
	THttpNotify notify;
	notify.dwErrorCode = 0;
	m_OnCompleteLogin((void*)&notify);

	// obtain classrooms of this teacher
	CHttpDownloadManager* pHttpManagerTest = HttpDownloadManager::GetInstance();
	if( pHttpManagerTest == NULL )
		return false;

	TCHAR szUrl[MAX_URL_LEN];
	_stprintf_s(szUrl, ClassroomClassInfo, m_dwUserId);

	m_nCurStep = UCSTEP_CLASSROOM;

	pHttpManagerTest->AddTask(ClassroomHost, 
		szUrl, 
		_T(""),
		_T("GET"), 
		"",
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnClassRoomObtained), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	CheckBearerTokens(CHECK_BEARERTOKEN);
// 	// check bearer token whether it is exist
// 	if( m_strBearerToken == _T("") )
// 	{
// 		tstring strPath = GetLocalPath();
// 		strPath += _T("\\Setting\\Config.ini");
// 
// 		TCHAR szBearerTokens[MAX_PATH] = {0};
// 
// 		GetPrivateProfileString(_T("config"), _T("BearerTokens"), _T(""), szBearerTokens, MAX_PATH, strPath.c_str());
// 		m_strBearerToken = szBearerTokens;
// 
// 		if( m_strBearerToken == _T("") )
// 			GetBearerTokens();
// 	}

	m_tLastMobileRqst = time(NULL);
	m_nLoginType = 4;
	return TRUE;
}

void	 CNDCloudUser::UndoMobileLogin()
{
	if (m_nLoginType == 4)
	{
		LogOut();
		CancelLogin();
	}
}

// User Login
//
BOOL CNDCloudUser::Login(tstring strUserName, tstring strPassword, tstring strOrganize, CHttpDelegateBase& delegate)
{

	// clear
	m_bSuccess				= FALSE;
	m_strErrorCode			= _T("");
	m_strErrorMessage		= _T("");
	m_strUserName			= strUserName;
	m_strPassword			= strPassword;
	m_strOriganize			= strOrganize;	
	m_strFullName			= _T("");//班级信息先清空2016.01.19 cws避免登陆切换时造成信息不准确
	m_nGradeIndex			= 0;//默认选择的班级索引，由于没有对账号和班级信息保存到本地，只要切换账号就默认班级选择第一个（临时处理方案） 2016.02.19 cws

	g_vecsFullNameList.clear();	
	setLoadTeacherInfoState(FALSE);
	//////////////////////////////////////////////////////////////////////////
	//登陆前先把原先有的数据清除2016.02.23 cws 
	m_strClassGuid			= _T(""); 
	if( m_pStudentInfoStream != NULL )
	{
		char* pBuffer = m_pStudentInfoStream->GetBuffer();
		int nBufferSize = m_pStudentInfoStream->GetBufferSize();

		if( pBuffer != NULL && nBufferSize != 0 )
			memset(pBuffer, 0, nBufferSize);
	}

	if( m_pGroupInfoStream != NULL )
	{
		char* pBuffer = m_pGroupInfoStream->GetBuffer();
		int nBufferSize = m_pGroupInfoStream->GetBufferSize();

		if( pBuffer != NULL && nBufferSize != 0 )
			memset(pBuffer, 0, nBufferSize);
	}

	if( m_pDeviceInfoStream != NULL )
	{
		char* pBuffer = m_pDeviceInfoStream->GetBuffer();
		int nBufferSize = m_pDeviceInfoStream->GetBufferSize();

		if( pBuffer != NULL && nBufferSize != 0 )
			memset(pBuffer, 0, nBufferSize);
	}
	//////////////////////////////////////////////////////////////////////////
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return FALSE;

	m_OnCompleteLogin.clear();

	if( delegate.GetFn() != NULL )
		m_OnCompleteLogin += delegate;

	// json
	Json::Value root;
	root["login_name"]	= Str2Utf8(strUserName);
	root["org_name"]	= Str2Utf8(strOrganize);
	root["password"]	= MD5EncryptString(Str2Ansi(strPassword));
	

	Json::FastWriter writter;
	string str = writter.write(root);

	m_nCurStep = UCSTEP_LOGIN;

	DWORD dwTaskId = pHttpManager->AddTask(UserCenterHost, 
										  UserCenterLoginUrl, 
										  _T("Content-Type: application/json"), 
										  _T("POST"), 
										  str.c_str(), 
										  INTERNET_DEFAULT_HTTPS_PORT, 
										  MakeHttpDelegate(this, &CNDCloudUser::OnUserLoginResponse), 
										  MakeHttpDelegate(NULL),
										  MakeHttpDelegate(NULL) );

	if( dwTaskId == 0 )
		return FALSE;

	// check bearer token whether it is exist
	CheckBearerTokens(CHECK_BEARERTOKEN);
// 	if( m_strBearerToken == _T("") )
// 	{
// 		tstring strPath = GetLocalPath();
// 		strPath += _T("\\Setting\\Config.ini");
// 
// 		TCHAR szBearerTokens[MAX_PATH] = {0};
// 
// 		GetPrivateProfileString(_T("config"), _T("BearerTokens"), _T(""), szBearerTokens, MAX_PATH, strPath.c_str());
// 		m_strBearerToken = szBearerTokens;
// 
// 		if( m_strBearerToken == _T("") )
// 			GetBearerTokens();
// 	}

	return TRUE;
						
}

//
// Login by token
//
BOOL CNDCloudUser::TokenLogin(tstring strHost, tstring strUrl, tstring strMethod, tstring strMac, tstring strNonce, tstring strToken, CHttpDelegateBase& delegate)
{
	TCHAR szUrl[1024];
	_stprintf_s(szUrl, UserCenterCheckTokenUrl, strToken.c_str());

	m_OnCompleteLogin.clear();

	if( delegate.GetFn() != NULL )
		m_OnCompleteLogin += delegate;

	m_strUserName = _T("");

	Json::Value root;
	root["mac"]			= Str2Utf8(strMac);
	root["nonce"]		= Str2Utf8(strNonce);
	root["http_method"]	= Str2Utf8(strMethod);
	root["request_uri"]	= Str2Utf8(strUrl);
	root["host"]		= Str2Utf8(strHost);

	Json::FastWriter writter;
	string str = writter.write(root);

	m_nCurStep = UCSTEP_LOGIN;

	DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(UserCenterHost, 
																	szUrl, 
																	_T("Content-Type: application/json"),
																	_T("POST"), 
																	str.c_str(), 
																	INTERNET_DEFAULT_HTTPS_PORT,
																	MakeHttpDelegate(this, &CNDCloudUser::OnUserLoginResponse),
																	MakeHttpDelegate(NULL),
																	MakeHttpDelegate(NULL));



	return TRUE;
}


//
// Teacher logined
//
bool CNDCloudUser::OnUserLoginResponse(void *param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	//
	string str = pNotify->pData;
	m_strLoginResponse = Utf82Str(str);

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
	{
		m_bSuccess			= FALSE;
		m_strErrorMessage = _T("网络不通");
		//m_strErrorMessage += Ansi2Str(str);

		m_OnCompleteLogin(param); 
		return false;
	}


	if( root["user_id"].isNull() )
	{
		// error
		m_bSuccess			= FALSE;
		m_strErrorCode		= Utf82Str(root["code"].asCString());
		m_strErrorMessage	= Utf82Str(root["message"].asCString());
 
		m_OnCompleteLogin(param);
		return false;
	}
	 
	// save informations
	m_dwUserId				= root["user_id"].asUInt();
	m_strMacKey				= Utf82Str(root["mac_key"].asCString());
	m_strAccessToken		= Utf82Str(root["access_token"].asCString());
	m_bSuccess				= TRUE;
	tstring	strServerTime	= Utf82Str(root["server_time"].asCString());

	int nYear, nMonth, nDate, nHour, nMin, nSec, nMilli; 
	TCHAR szUTC[128];
	TCHAR szUTCNum[128];
	TCHAR szUTCSymbol[128];
	if(_stscanf_s(strServerTime.c_str(), _T("%d-%d-%d%[^0-9]%d:%d:%d.%d%[+-]%s"), &nYear, &nMonth, &nDate, szUTC, _countof(szUTC) - 1, &nHour, &nMin, &nSec, &nMilli, szUTCSymbol, _countof(szUTCSymbol) - 1, szUTCNum, _countof(szUTCNum) - 1) == 10)
	{
		CTime time(nYear, nMonth, nDate, nHour, nMin, nSec);
		m_dwServerTime = time.GetTime();
		if( _tcsicmp(szUTCSymbol , _T("+")) == 0)
		{
			m_dwServerTime += (8 - _ttoi(szUTCNum)/100) * 60* 60;
		}
		else if( _tcsicmp(szUTCSymbol , _T("-")) == 0)
		{
			m_dwServerTime += (8 + _ttoi(szUTCNum)/100) * 60* 60;
		}
		m_dwTickCout = GetTickCount();
	}
	else
	{
		m_dwTickCout = 0;
	}

	// Authorization header
	TCHAR szUrl[1024];
	_stprintf_s(szUrl, _T("%s/%u"), UserCenterUserInfo, m_dwUserId);
	string strAuthorization = GetAuthorizationHeader(Str2Ansi(UserCenterHost), Str2Ansi(szUrl), "GET");

	tstring strHeader = _T("Content-Type: application/json");
	strHeader += _T("\r\n");
	strHeader += Ansi2Str(strAuthorization);

	// notify caller
	m_OnCompleteLogin(param);


	// obtain teacher information
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return false;

	m_nCurStep = UCSTEP_USERINFO;

	pHttpManager->AddTask(UserCenterHost, 
							szUrl, 
							strHeader.c_str(), 
							_T("GET"), 
							"",
							INTERNET_DEFAULT_HTTPS_PORT, 
							MakeHttpDelegate(this, &CNDCloudUser::OnUserInfoObtained), 
							MakeHttpDelegate(NULL),
							MakeHttpDelegate(NULL) );

	return true;

}

//
// Teacher information obtained
//
bool CNDCloudUser::OnUserInfoObtained(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = pNotify->pData;

	Json::Reader reader;
	Json::Value root;
	Json::Value NodeItmes(Json::arrayValue);//加载老师的班级信息
	Json::Value SubItem;

	bool res = reader.parse(str, root);
	if( !res )
	{
		m_bUserInfoSuccess = FALSE;
		m_bSuccess		  = FALSE;
		m_strErrorMessage = _T("网络不通");
		//m_strErrorMessage += Ansi2Str(str);

		m_OnCompleteLogin(param);
		return false;
	}

	if( root["user_id"].isNull() )
	{
		m_bUserInfoSuccess = FALSE;
		m_bSuccess		  = FALSE;
		m_strErrorMessage = _T("网络不通");
		//m_strErrorMessage += Ansi2Str(str);

		m_OnCompleteLogin(param);
		return false;
	}

	m_strNickName = Utf82Str(root["nick_name"].asCString());

	if( !root["org_exinfo"].isNull() )
	{
		if( !root["org_exinfo"]["real_name"].isNull() )
			m_strRealName = Utf82Str(root["org_exinfo"]["real_name"].asCString());

		if( !root["org_exinfo"]["sex"].isNull() )
			m_strSex = Utf82Str(root["org_exinfo"]["sex"].asCString());

		if( !root["org_exinfo"]["subjects"].isNull() )
			m_strSubjects = Utf82Str(root["org_exinfo"]["subjects"].asCString());

		if( !root["org_exinfo"]["node_name"].isNull() )
			m_strOrgNodeName = Utf82Str(root["org_exinfo"]["node_name"].asCString());

		if( !root["org_exinfo"]["org_id"].isNull() )
			m_ulOrgId = root["org_exinfo"]["org_id"].asDouble();	
		if( !root["org_exinfo"]["node_items"].isNull() )
		{
			NodeItmes = root["org_exinfo"]["node_items"];
			///清除上次登录的数据2016.02.15 
			m_strFullName = _T("");
			m_strTeacherGrade = _T("");
			if (NodeItmes.size() > 0)
			{
				bool bDefaultGrade= false;
				for (int i=0; i< NodeItmes.size(); i++)
				{
					SubItem = NodeItmes[i];
					if( !SubItem["node_full_name"].isNull() )
					{ 
						CString strGradeClassNO = _T("");  
						CString strFullName =Utf82Str(SubItem["node_full_name"].asCString()).c_str();  
						AfxExtractSubString(strGradeClassNO, strFullName, 4, _T('|')); 
						if (!strGradeClassNO.IsEmpty())
						{
							if (! bDefaultGrade)
							{
								m_strFullName =  Utf82Str(SubItem["node_full_name"].asCString());
								m_strTeacherGrade = GetTeacherGrade(m_strFullName);
								bDefaultGrade = true;
							} 
						}

					}
				}
			} 

		}	
	}


	// set user name
	if( m_strUserName == _T("") && !root["user_name"].isNull() )
		m_strUserName = Utf82Str(root["user_name"].asCString());
	
	if( m_strUserName == _T("") && !root["org_exinfo"].isNull() && !root["org_exinfo"]["reg_mobile"].isNull() )
		m_strUserName = Utf82Str(root["org_exinfo"]["reg_mobile"].asCString());

	if( m_strUserName == _T("") && !root["org_exinfo"].isNull() && !root["org_exinfo"]["reg_email"].isNull() )
		m_strUserName = Utf82Str(root["org_exinfo"]["reg_email"].asCString());


	// notify caller
	m_OnCompleteLogin(param);


	// obtain classrooms of this teacher
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return false;

	TCHAR szUrl[MAX_URL_LEN];
	_stprintf_s(szUrl, ClassroomClassInfo, m_dwUserId);

	m_nCurStep = UCSTEP_CLASSROOM;

	pHttpManager->AddTask(ClassroomHost, 
							szUrl, 
							_T(""),
							_T("GET"), 
							"",
							80, 
							MakeHttpDelegate(this, &CNDCloudUser::OnClassRoomObtained), 
							MakeHttpDelegate(NULL),
							MakeHttpDelegate(NULL) );
	m_bSuccess = TRUE;
	m_bUserInfoSuccess = TRUE;

	return true;
}

//
// Classroom obtained
//
bool CNDCloudUser::OnClassRoomObtained(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	// clear data
	m_pClassroomStream->ResetCursor();

	string str = pNotify->pData;

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
	{
		m_bClassRoomInfoSuccess = FALSE;
		m_bSuccess		  = FALSE;
		m_strErrorMessage = _T("网络不通");
		//m_strErrorMessage += Ansi2Str(str);

		m_OnCompleteLogin(param);
		UndoMobileLogin();
		return false;
	}

	if( root["items"].isNull() )
	{
		m_bClassRoomInfoSuccess = FALSE;
		m_bSuccess		  = FALSE;
		m_strErrorMessage = _T("网络不通");
		//m_strErrorMessage += Ansi2Str(str);

		m_OnCompleteLogin(param);
		return false;
	}


	Json::Value& items = root["items"];
	int count = items.size();

	m_pClassroomStream->WriteDWORD(count);
	
	UINT64 ulFirstClassId = 0L;
	Json::Value lGradeInfo;
	Json::Value lItem;
	g_vecsFullNameList.clear();
	for(int i = 0; i < count; i++)
	{
		Json::Value& item = items[i];

		UINT64 ulClassId = item["class_id"].asDouble();
		tstring strClassName = Utf82Str(item["class_name"].asCString());

		tstring strGradeName = Utf82Str(item["grade_name"].asCString());

		m_pClassroomStream->Write((char*)&ulClassId, sizeof(UINT64));
		m_pClassroomStream->WriteString(strClassName);

		TCHAR szOrgId[MAX_PATH];
		_stprintf_s(szOrgId, _T("%llu"), m_ulOrgId);

		TCHAR szClassId[MAX_PATH];
		_stprintf_s(szClassId, _T("%llu"), ulClassId);

		lItem["orgid"] = szOrgId;
		lItem["classid"] = szClassId;
		lItem["gradename"] = strGradeName.c_str();
		lItem["classname"] = strClassName.c_str();
		lGradeInfo["gradeinfo"] = lItem; 
		g_vecsFullNameList.push_back(lGradeInfo);

		if( i == 0 )
			ulFirstClassId = ulClassId;
	}
	
	m_pClassroomStream->ResetCursor();

	// notify caller
	m_bSuccess = TRUE;
	m_OnCompleteLogin(param);

	//
	// obtain student information of first class
	//
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return false;

	TCHAR szOrgId[MAX_PATH];
	_stprintf_s(szOrgId, _T("%llu"), m_ulOrgId);

	TCHAR szClassId[MAX_PATH];
	_stprintf_s(szClassId, _T("%llu"), ulFirstClassId);

	m_strClassGuid = szClassId;

	TCHAR szUrl[MAX_URL_LEN];
	_stprintf_s(szUrl, ClassroomStudentInfo, szOrgId, szClassId);

	m_nCurStep = UCSTEP_STUDENTINFO;

	// authorization
	string strAuthorization = GetAuthorizationHeader(Str2Ansi(ClassroomHost), Str2Ansi(szUrl), "GET");

	tstring strHeader = _T("Content-Type: application/json");
	strHeader += _T("\r\n");
	strHeader += Utf82Str(strAuthorization);


	pHttpManager->AddTask(ClassroomHost, 
		szUrl, 
		strHeader.c_str(),
		_T("GET"), 
		"",
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnStudentInfoObtained), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	//
	// obtain group information of first class
	//
	_stprintf_s(szUrl, ClassroomGroupInfo, szClassId);

	m_nCurStep = UCSTEP_GROUPINFO;

	strAuthorization = GetAuthorizationHeader(Str2Ansi(ClassroomHost), Str2Ansi(szUrl), "GET");

	strHeader = _T("Content-Type: application/json");
	strHeader += _T("\r\n");
	strHeader += Utf82Str(strAuthorization);


	pHttpManager->AddTask(ClassroomHost, 
		szUrl, 
		strHeader.c_str(),
		_T("GET"), 
		"",
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnGroupInfoObtained), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	//
	// obtain device information of first class
	//
	_stprintf_s(szUrl, ClassroomDeviceInfo, szClassId);

	m_nCurStep = UCSTEP_DEVICEINFO;

	pHttpManager->AddTask(ClassroomHost, 
		szUrl, 
		_T(""),
		_T("GET"), 
		"",
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnDeviceInfoObtained), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	m_bClassRoomInfoSuccess = TRUE;

	return true;
}


//
// Student information obtained
//
bool CNDCloudUser::OnStudentInfoObtained(void *param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	// clear data
	m_pStudentInfoStream->ResetCursor();

	string str = pNotify->pData;
	
	Json::Reader reader;
	Json::Value root;

	reader.parse(str, root);
	if( !root["code"].isNull() )
		str = "";

	m_pStudentInfoStream->WriteString(Utf82Str(str));

	m_pStudentInfoStream->ResetCursor();

	// notify caller
	m_bSuccess = TRUE;
	m_OnCompleteLogin(param);

	m_bStudentInfoSuccess = TRUE;
	return true;
}


//
// Group information obtained
// 
bool CNDCloudUser::OnGroupInfoObtained(void *param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	// clear data
	m_pGroupInfoStream->ResetCursor();

	tstring str = Utf82Str(pNotify->pData);
	m_pGroupInfoStream->WriteString(str);

	m_pGroupInfoStream->ResetCursor();

	// notify caller
	m_bSuccess = TRUE;
	m_OnCompleteLogin(param);

	m_bGroupInfoSuccess = TRUE;


	return true;
}

//
// Device information obtained
//
bool CNDCloudUser::OnDeviceInfoObtained(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	// clear data
	m_pDeviceInfoStream->ResetCursor();

	tstring str = Utf82Str(pNotify->pData);
	m_pDeviceInfoStream->WriteString(str);

	m_pDeviceInfoStream->ResetCursor();

	// notify caller
	m_bSuccess = TRUE;
	m_OnCompleteLogin(param);
	m_bDeviceInfoSuccess = TRUE;


	return true;
}


// bearer_tokens
BOOL CNDCloudUser::BearerTokensLogin(tstring strSessionId, CHttpDelegateBase& delegate)
{
	m_strBearerSessionId = strSessionId;

	if( delegate.GetFn() != NULL )
	{
		m_OnCompleteLogin.clear();
		m_OnCompleteLogin += delegate;
	}

	CheckBearerTokens(BEARER_TOKEN);

	return TRUE;
}

bool CNDCloudUser::OnUserBearerLoginResponse( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	//
	string str = pNotify->pData;
	m_strLoginResponse = Utf82Str(str);

	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( !res )
	{
		m_bSuccess			= FALSE;
		m_strErrorMessage = _T("网络不通");
		//m_strErrorMessage += Ansi2Str(str);

		m_OnCompleteLogin(param); 
		return false;
	}

	if( !root["token"].isNull() )
	{
		if( root["token"]["user_id"].isNull() )
		{
			// error
			m_bSuccess			= FALSE;
			m_strErrorCode		= Utf82Str(root["token"]["code"].asCString());
			m_strErrorMessage	= Utf82Str(root["token"]["message"].asCString());

			m_OnCompleteLogin(param);
			return false;
		}

		// save informations
		m_dwUserId				= atoi(root["token"]["user_id"].asCString());
		m_strMacKey				= Utf82Str(root["token"]["mac_key"].asCString());
		m_strAccessToken		= Utf82Str(root["token"]["access_token"].asCString());
		if( !root["info"].isNull() )
		{
			if( !root["info"]["nick_name"].isNull() )
				m_strUserName			= Utf82Str(root["info"]["nick_name"].asCString());
		}

		tstring	strServerTime	= Utf82Str(root["token"]["server_time"].asCString());
		int nYear, nMonth, nDate, nHour, nMin, nSec, nMilli; 
		TCHAR szUTC[128];
		TCHAR szUTCSymbol[128];
		TCHAR szUTCNum[128];
		if(_stscanf_s(strServerTime.c_str(), _T("%d-%d-%d%[^0-9]%d:%d:%d.%d%[+-]%s"), &nYear, &nMonth, &nDate, szUTC, _countof(szUTC) - 1, &nHour, &nMin, &nSec, &nMilli, szUTCSymbol, _countof(szUTCSymbol) - 1, szUTCNum, _countof(szUTCNum) - 1) == 10)
		{
			CTime time(nYear, nMonth, nDate, nHour, nMin, nSec);
			m_dwServerTime = time.GetTime();
			if( _tcsicmp(szUTCSymbol , _T("+")) == 0)
			{
				m_dwServerTime += (8 - _ttoi(szUTCNum)/100) * 60* 60;
			}
			else if( _tcsicmp(szUTCSymbol , _T("-")) == 0)
			{
				m_dwServerTime += (8 + _ttoi(szUTCNum)/100) * 60* 60;
			}
			m_dwTickCout = GetTickCount();
		}
		else
		{
			m_dwTickCout = 0;
		}
	
	
		m_bSuccess				= TRUE;

		// Authorization header
		TCHAR szUrl[1024];
		_stprintf_s(szUrl, _T("%s/%u"), UserCenterUserInfo, m_dwUserId);
		string strAuthorization = GetAuthorizationHeader(Str2Ansi(UserCenterHost), Str2Ansi(szUrl), "GET");

		tstring strHeader = _T("Content-Type: application/json");
		strHeader += _T("\r\n");
		strHeader += Ansi2Str(strAuthorization);

		// notify caller
		m_OnCompleteLogin(param);


		// obtain teacher information
		CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
		if( pHttpManager == NULL )
			return false;

		m_nCurStep = UCSTEP_USERINFO;

		pHttpManager->AddTask(UserCenterHost, 
			szUrl, 
			strHeader.c_str(), 
			_T("GET"), 
			"",
			INTERNET_DEFAULT_HTTPS_PORT, 
			MakeHttpDelegate(this, &CNDCloudUser::OnUserInfoObtained), 
			MakeHttpDelegate(NULL),
			MakeHttpDelegate(NULL) );
	}
	

	return true;
}


BOOL CNDCloudUser::GetBearerTokens(int nType)
{
	// json
	Json::Value root;
	root["login_name"]	= Str2Utf8(BearerTokensName);
	root["password"]	= Str2Utf8(BearerTokensPass);


	Json::FastWriter writter;
	string str = writter.write(root);

	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return false;

	int * pType = new int;
	*pType = nType;

	DWORD dwTaskId = pHttpManager->AddTask(UserCenterHost, 
		BearerTokensUrl, 
		_T("Content-Type: application/json"), 
		_T("POST"), 
		str.c_str(), 
		INTERNET_DEFAULT_HTTPS_PORT, 
		MakeHttpDelegate(this, &CNDCloudUser::OnGetBearerTokensResponse), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL), TRUE, FALSE, 0,  pType);

	if( dwTaskId == 0 )
		return FALSE;

	return TRUE;
}

bool CNDCloudUser::OnGetBearerTokensResponse( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = pNotify->pData;

	int nType = *(int *)(pNotify->pUserData);
	delete pNotify->pUserData;
	// register failed
	Json::Reader reader;
	Json::Value root;
	bool res = reader.parse(str, root);
	if( !res )
	{
		m_OnCompleteLogin(param);
		return false;
	}
	if( str != "" )
	{
		if( !root["code"].isNull() && !root["message"].isNull() )
		{
			m_strErrorCode = Utf82Str(root["code"].asCString());
			m_strErrorMessage = Utf82Str(root["message"].asCString());

			m_bSuccess = FALSE;

			m_OnCompleteLogin(param);
		}
		else
		{
			// register success
			tstring strBearerTokens = Utf82Str(root["access_token"].asCString());
			m_strBearerToken = strBearerTokens;

			tstring strPath = GetLocalPath();
			strPath += _T("\\Setting\\Config.ini");
			WritePrivateProfileString(_T("config"),  _T("BearerTokens"),  strBearerTokens.c_str(),   strPath.c_str());

			
			if(nType == BEARER_TOKEN)
			{
				CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
				if( pHttpManager == NULL )
					return false;

				TCHAR szPost[1024];
				_stprintf_s(szPost, BearerLogin ,m_strBearerSessionId.c_str());

				tstring strHeader = _T("Authorization: Bearer \"");
				strHeader += strBearerTokens;
				strHeader += _T("\"");
				DWORD dwTaskId = pHttpManager->AddTask(BrarerLoginHost, 
					szPost, 
					strHeader.c_str(),
					_T("GET"), 
					"", 
					80, 
					MakeHttpDelegate(this, &CNDCloudUser::OnUserBearerLoginResponse), 
					MakeHttpDelegate(NULL),
					MakeHttpDelegate(NULL) );

				if( dwTaskId == 0 )
					return FALSE;
			}

			m_bSuccess = TRUE;
		}	
	}
	
		
	
	return true;
}


BOOL CNDCloudUser::CheckBearerTokens( int nType )
{
	tstring strPath = GetLocalPath();
	strPath += _T("\\Setting\\Config.ini");
	TCHAR szBearerTokens[1024]={0};
	GetPrivateProfileString(_T("config"),  _T("BearerTokens"), _T(""),  szBearerTokens, _countof(szBearerTokens),  strPath.c_str());
	
	if(_tcslen(szBearerTokens) == 0)
		GetBearerTokens(nType);
	else
	{
		m_strBearerToken = szBearerTokens;

		TCHAR szUrl[1024];
		_stprintf_s(szUrl, CheckBearerTokenUrl, szBearerTokens);
		CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
		if( pHttpManager == NULL )
			return false;

		int * pType = new int;
		*pType = nType;

		DWORD dwTaskId = pHttpManager->AddTask(UserCenterHost, 
			szUrl, 
			_T("Content-Type: application/json"), 
			_T("GET"), 
			"", 
			INTERNET_DEFAULT_HTTPS_PORT, 
			MakeHttpDelegate(this, &CNDCloudUser::OnCheckBearerTokensResponse), 
			MakeHttpDelegate(NULL),
			MakeHttpDelegate(NULL), TRUE, FALSE, 0,  pType);

		if( dwTaskId == 0 )
			return FALSE;
	}
	return TRUE;
}


bool CNDCloudUser::OnCheckBearerTokensResponse( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = pNotify->pData;

	int nType = *(int *)(pNotify->pUserData);
	delete pNotify->pUserData;
	// register failed
	Json::Reader reader;
	Json::Value root;
	bool res = reader.parse(str, root);
	if( !res )
	{
		m_OnCompleteLogin(param);
		return false;
	}
	if( str != "" )
	{
		if( !root["code"].isNull() && !root["message"].isNull() )
		{
			m_strErrorCode = Utf82Str(root["code"].asCString());
			m_strErrorMessage = Utf82Str(root["message"].asCString());

			if(m_strErrorCode == _T("UC/AUTH_TOKEN_EXPIRED"))
				GetBearerTokens(nType);

			//m_OnCompleteLogin(param);
		}
		else
		{
			// register success
			if(nType == BEARER_TOKEN)
			{
				CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
				if( pHttpManager == NULL )
					return false;

				TCHAR szPost[1024];
				_stprintf_s(szPost, BearerLogin ,m_strBearerSessionId.c_str());

				tstring strHeader = _T("Authorization: Bearer \"");
				strHeader += m_strBearerToken.c_str();
				strHeader += _T("\"");
				DWORD dwTaskId = pHttpManager->AddTask(BrarerLoginHost, 
					szPost, 
					strHeader.c_str(),
					_T("GET"), 
					"", 
					80, 
					MakeHttpDelegate(this, &CNDCloudUser::OnUserBearerLoginResponse), 
					MakeHttpDelegate(NULL),
					MakeHttpDelegate(NULL) );

				if( dwTaskId == 0 )
					return FALSE;
			}
		}	
	}

	return TRUE;
}


//-----------------------------------------------------------
// User register
//
BOOL CNDCloudUser::Register(tstring strUserName, tstring strPassword, tstring strCheckCode, CHttpDelegateBase& delegate)
{
	m_bSuccess				= FALSE;
	m_strErrorCode			= _T("");
	m_strErrorMessage		= _T("");

	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return FALSE;

	m_OnCompleteRegister.clear();

	if( delegate.GetFn() != NULL )
		m_OnCompleteRegister += delegate;

	string strEncryptedPass = m_RSA.EncryptString(Str2Ansi(strPassword));

	// json
	Json::Value root;
	root["login_name"]	= Str2Utf8(strUserName);
	root["check_code"]	= Str2Utf8(strCheckCode);
	root["password"]	= AnsiToUtf8(strEncryptedPass);


	Json::FastWriter writter;
	string str = writter.write(root);

	// set cookie
	tstring strUrl = _T("http://");
	strUrl += UserEspHost;

	InternetSetCookie(strUrl.c_str(), _T("JSESSIONID"), m_CheckCodeSession.c_str());

	DWORD dwTaskId = pHttpManager->AddTask(UserEspHost, 
											UserEspRegisterUrl, 
											_T("Content-Type: application/json"), 
											_T("POST"), 
											str.c_str(), 
											80, 
											MakeHttpDelegate(this, &CNDCloudUser::OnUserRegisterResponse), 
											MakeHttpDelegate(NULL),
											MakeHttpDelegate(NULL) );

	if( dwTaskId == 0 )
		return FALSE;

	return TRUE;
}


bool CNDCloudUser::OnUserRegisterResponse(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = pNotify->pData;
	if( str != "" )
	{
		// register failed
		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(str, root);
		if( !res )
			return false;

		if( root["code"].isNull() )
			return false;

		// cause 
		if( !root["cause"].isNull() )
		{
			if( !root["cause"]["code"].isNull() )
				m_strErrorCode = Utf82Str(root["cause"]["code"].asCString());

			if( !root["cause"]["message"].isNull() )
				m_strErrorMessage = Utf82Str(root["cause"]["message"].asCString());
		}
		else
		{
			m_strErrorCode = Utf82Str(root["code"].asCString());
			m_strErrorMessage = Utf82Str(root["message"].asCString());
		}

		m_bSuccess = FALSE;

	}
	else
	{
		// register success
		m_bSuccess = TRUE;
	}


	if( m_OnCompleteRegister )
		m_OnCompleteRegister(param);

	return true;
}

BOOL CNDCloudUser::ChangePassword( tstring strOldPassword, tstring strNewPassword, CHttpDelegateBase& delegate )
{
	m_bSuccess				= FALSE;
	m_strErrorCode			= _T("");
	m_strErrorMessage		= _T("");

	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return FALSE;

	m_OnCompleteChangePassword.clear();

	if( delegate.GetFn() != NULL )
		m_OnCompleteChangePassword += delegate;

	string strEncryptedOldPass = MD5EncryptString(Str2Ansi(strOldPassword));
	string strEncryptedNewPass = MD5EncryptString(Str2Ansi(strNewPassword));

	// json
	Json::Value root;
	root["old_password"]	= AnsiToUtf8(strEncryptedOldPass);
	root["new_password"]	= AnsiToUtf8(strEncryptedNewPass);

	Json::FastWriter writter;
	string str = writter.write(root);

	TCHAR szPutBuff[1024];
	_stprintf_s(szPutBuff, UserCenterChangePassUrl, GetUserId());
	string strAuthorization = GetAuthorizationHeader(Str2Ansi(UserCenterHost), Str2Ansi(szPutBuff), "PUT");

	tstring strHeader = _T("Content-Type: application/json");
	strHeader += _T("\r\n");
	strHeader += Ansi2Str(strAuthorization);


	DWORD dwTaskId = pHttpManager->AddTask(UserCenterHost, 
		szPutBuff,
		strHeader.c_str(),
		_T("PUT"),  
		str.c_str(), 
		INTERNET_DEFAULT_HTTPS_PORT, 
		MakeHttpDelegate(this, &CNDCloudUser::OnUserChangePassResponse), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	if( dwTaskId == 0 )
		return FALSE;

	return TRUE;
}

bool CNDCloudUser::OnUserChangePassResponse( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = pNotify->pData;
	if( str != "" )
	{
		// register failed
		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(str, root);
		if( !res )
			return false;

		if( !root["code"].isNull() && !root["message"].isNull() )
		{
			m_strErrorCode = Utf82Str(root["code"].asCString());
			m_strErrorMessage = Utf82Str(root["message"].asCString());
		}
		else
			m_bSuccess = TRUE;
	}
	else
	{
		m_bSuccess = TRUE;
	}

	if( m_OnCompleteChangePassword )
		m_OnCompleteChangePassword(param);

	return true;
}

BOOL CNDCloudUser::GetBackPwdConfirmAccount( tstring strUserName, tstring strCheckCode, CHttpDelegateBase& delegate )
{
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return FALSE;

	m_OnCompleteGetBackPwd.clear();

	if( delegate.GetFn() != NULL )
		m_OnCompleteGetBackPwd += delegate;

	// json
	Json::Value root;
	root["login_name"]	= Str2Utf8(strUserName);
	root["check_code"]	= Str2Utf8(strCheckCode);

	Json::FastWriter writter;
	string str = writter.write(root);

	tstring strHeader = _T("Content-Type: application/json");

	// set cookie
	tstring strUrl = _T("http://");
	strUrl += UserEspHost;

	InternetSetCookie(strUrl.c_str(), _T("JSESSIONID"), m_CheckCodeSession.c_str());

	DWORD dwTaskId = pHttpManager->AddTask(UserEspHost, 
		UserEspConfirmAccountUrl,
		strHeader.c_str(),
		_T("POST"),  
		str.c_str(), 
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnGetBackPwdConfirmAccountResponse), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	if( dwTaskId == 0 )
		return FALSE;

	return true;
}

bool CNDCloudUser::OnGetBackPwdConfirmAccountResponse(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = pNotify->pData;

	GetBackPwdParam p;
	p.dwErrorCode = pNotify->dwErrorCode;
	p.strEmail = _T("");
	p.strPhone = _T("");

	if( str != "" )
	{
		// register failed
		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(str, root);
		if( !res )
		{
			p.dwErrorCode = 100;
			m_OnCompleteGetBackPwd(&p);
			return false;
		}

		if( !root["code"].isNull() && !root["message"].isNull() )
		{
			p.strCode = Utf82Str(root["code"].asCString()).c_str();
			p.strMessage = Utf82Str(root["message"].asCString());
		}
		else
		{
			if( !root["login_name"].isNull() )
			{
				p.strAccount = Utf82Str(root["login_name"].asCString());
				if( !root["email"].isNull() )
				{
					p.strEmail = Utf82Str(root["email"].asCString());
				}
				if( !root["mobile"].isNull() )
				{
					p.strPhone = Utf82Str(root["mobile"].asCString());
				}
			}
		}
		
	}
	
	if( m_OnCompleteGetBackPwd )
		m_OnCompleteGetBackPwd(&p);

	return true;
}

BOOL CNDCloudUser::GetBackPwdCheckEmail( tstring strEmail, tstring strEmailCode, CHttpDelegateBase& delegate )
{
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return FALSE;

	m_OnCompleteGetBackPwd.clear();

	if( delegate.GetFn() != NULL )
		m_OnCompleteGetBackPwd += delegate;

	// json
	tstring strMethod = _T("email");
	Json::Value root;
	root["login_name"]	= Str2Utf8(strEmail);
	root["check_code_type"]	= Str2Utf8(strMethod);
	root["check_code"]	= Str2Utf8(strEmailCode);

	Json::FastWriter writter;
	string str = writter.write(root);

	tstring strHeader = _T("Content-Type: application/json");


	DWORD dwTaskId = pHttpManager->AddTask(UserEspHost, 
		UserEspResetPasswordUrl,
		strHeader.c_str(),
		_T("PUT"),  
		str.c_str(), 
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnResetPasswordResponse), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	if( dwTaskId == 0 )
		return FALSE;

	return true;
}

BOOL CNDCloudUser::GetBackPwdCheckPhone( tstring strPhoneNumber, tstring strPhoneCode, CHttpDelegateBase& delegate )
{
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return FALSE;

	m_OnCompleteGetBackPwd.clear();

	if( delegate.GetFn() != NULL )
		m_OnCompleteGetBackPwd += delegate;

	// json
	tstring strMethod = _T("mobile");
	Json::Value root;
	root["login_name"]	= Str2Utf8(strPhoneNumber);
	root["check_code_type"]	= Str2Utf8(strMethod);
	root["check_code"]	= Str2Utf8(strPhoneCode);

	Json::FastWriter writter;
	string str = writter.write(root);

	tstring strHeader = _T("Content-Type: application/json");


	DWORD dwTaskId = pHttpManager->AddTask(UserEspHost, 
		UserEspResetPasswordUrl,
		strHeader.c_str(),
		_T("PUT"),  
		str.c_str(), 
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnResetPasswordResponse), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	if( dwTaskId == 0 )
		return FALSE;

	return true;
}

bool CNDCloudUser::OnResetPasswordResponse( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = pNotify->pData;

	GetBackPwdParam p;
	p.dwErrorCode = pNotify->dwErrorCode;
	p.strEmail = _T("");
	p.strPhone = _T("");

	if( str != "" )
	{
		// register failed
		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(str, root);
		if( !res )
		{
			p.dwErrorCode = 100;
			m_OnCompleteGetBackPwd(&p);
			return false;
		}

		if( !root["cause"].isNull() )
		{
			if( !root["cause"]["code"].isNull() )
				p.strCode = Utf82Str(root["cause"]["code"].asCString());

			if( !root["cause"]["message"].isNull() )
				p.strMessage = Utf82Str(root["cause"]["message"].asCString());
		}
		else if( !root["code"].isNull() && !root["message"].isNull() )
		{
			p.strCode = Utf82Str(root["code"].asCString()).c_str();
			p.strMessage = Utf82Str(root["message"].asCString());
		}
		else
		{
			if( !root["temppwd"].isNull() )
				p.strTempPwd = Utf82Str(root["temppwd"].asCString());
		}

	}

	if( m_OnCompleteGetBackPwd )
		m_OnCompleteGetBackPwd(&p);

	return true;
}


BOOL CNDCloudUser::GetBackPwdResetPassword( tstring strAccount, tstring strPassword, tstring strTempPassword, CHttpDelegateBase& delegate )
{
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return FALSE;

	m_OnCompleteGetBackPwd.clear();

	if( delegate.GetFn() != NULL )
		m_OnCompleteGetBackPwd += delegate;

	string strEncryptedPass = m_RSA.EncryptString(Str2Ansi(strPassword));
//	string strEncryptedTempPass = MD5EncryptString(Str2Ansi(strTempPassword));
	// json
	Json::Value root;
	root["login_name"]	= Str2Utf8(strAccount);
	root["new_password"]	= AnsiToUtf8(strEncryptedPass);
	root["temppwd"]	= Str2Utf8(strTempPassword);

	Json::FastWriter writter;
	string str = writter.write(root);

	tstring strHeader = _T("Content-Type: application/json");


	DWORD dwTaskId = pHttpManager->AddTask(UserEspHost, 
		UserEspModifyUrl,
		strHeader.c_str(),
		_T("PUT"),  
		str.c_str(), 
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnMotifyResponse), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	if( dwTaskId == 0 )
		return FALSE;

	return true;
}

bool CNDCloudUser::OnMotifyResponse( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = pNotify->pData;

	GetBackPwdParam p;
	p.dwErrorCode = pNotify->dwErrorCode;
	p.strEmail = _T("");
	p.strPhone = _T("");

	if( str != "" )
	{
		// register failed
		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(str, root);
		if( !res )
		{
			p.dwErrorCode = 100;
			m_OnCompleteGetBackPwd(&p);
			return false;
		}

		if( !root["code"].isNull() && !root["message"].isNull() )
		{
			p.strCode = Utf82Str(root["code"].asCString()).c_str();
			p.strMessage = Utf82Str(root["message"].asCString());
		}

	}

	if( m_OnCompleteGetBackPwd )
		m_OnCompleteGetBackPwd(&p);

	return true;
}


BOOL CNDCloudUser::SendGetBackPwdSMS( tstring strPhoneNumber , CHttpDelegateBase& delegate)
{
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return FALSE;

	m_OnCompleteGetBackPwd.clear();

	if( delegate.GetFn() != NULL )
		m_OnCompleteGetBackPwd += delegate;

	// json
	Json::Value root;
	root["mobile"]	= Str2Utf8(strPhoneNumber);

	Json::FastWriter writter;
	string str = writter.write(root);

	tstring strHeader = _T("Content-Type: application/json");


	DWORD dwTaskId = pHttpManager->AddTask(UserEspHost, 
		UserEspResetSMSUrl,
		strHeader.c_str(),
		_T("POST"),  
		str.c_str(), 
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnGetBackPwdResetResponse), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	if( dwTaskId == 0 )
		return FALSE;

	return true;
}

BOOL CNDCloudUser::SendGetBackPwdEmail( tstring strEmail , CHttpDelegateBase& delegate)
{
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return FALSE;

	m_OnCompleteGetBackPwd.clear();

	if( delegate.GetFn() != NULL )
		m_OnCompleteGetBackPwd += delegate;

	// json
	Json::Value root;
	root["email"]	= Str2Utf8(strEmail);

	Json::FastWriter writter;
	string str = writter.write(root);

	tstring strHeader = _T("Content-Type: application/json");


	DWORD dwTaskId = pHttpManager->AddTask(UserEspHost, 
		UserEspResetEmailUrl,
		strHeader.c_str(),
		_T("POST"),  
		str.c_str(), 
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnGetBackPwdResetResponse), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	if( dwTaskId == 0 )
		return FALSE;

	return true;
}

bool CNDCloudUser::OnGetBackPwdResetResponse( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = pNotify->pData;

	GetBackPwdParam p;
	p.dwErrorCode = pNotify->dwErrorCode;

	if( str != "" )
	{
		// register failed
		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(str, root);
		if( !res )
			return false;

		if( !root["code"].isNull() && !root["message"].isNull() )
		{
			p.strCode = Utf82Str(root["code"].asCString()).c_str();
			p.strMessage = Utf82Str(root["message"].asCString());
		}
	}

	if( m_OnCompleteGetBackPwd )
		m_OnCompleteGetBackPwd(&p);

	return true;
}


BOOL CNDCloudUser::GetUserAvatar( DWORD dwSize, CHttpDelegateBase& delegate )
{
	BOOL bRet = TRUE;

	CHttpDownloadManager* pHttpDownloadManager = HttpDownloadManager::GetInstance();

	TCHAR szPost[1024];
	_stprintf_s(szPost, UserGetAvatar ,NDCloudUser::GetInstance()->GetUserId(), NDCloudUser::GetInstance()->GetUserId(), dwSize);
	if(pHttpDownloadManager)
	{
		tstring lUrl = szPost;
		int dwRes = NDCloudDownloadFile(lUrl, _T(""), _T(""), CloudFileImageEx, 0, delegate ,MakeHttpDelegate(NULL));
		
		//int nDownloadId = NDCloudDownloadFile(GetResource(), m_strGuid, GetTitle(), CloudFileImage, 240, MakeHttpDelegate(this, delegate));
		//pHttpDownloadManager->AddTask(EduPlatformHostRes, szPost, _T(""), _T("GET"), _T(""), 80, delegate ,MakeHttpDelegate(NULL), MakeHttpDelegate(this, &CNDCloudUser::GetUserAvatarStart) , TRUE, TRUE);
	}

	return bRet;
}

bool CNDCloudUser::GetUserAvatarStart( void* pParam ) 
{
	THttpNotify* pHttpNotify = (THttpNotify*)pParam;
	m_strTeacherPhoto = pHttpNotify->strFilePath.c_str();
	if (pHttpNotify->strFilePath == _T(""))
	{ 
		return false;
	} 
	return true;
}

BOOL CNDCloudUser::SetUserAvatar( tstring strFilePath, CHttpDelegateBase& delegate )
{
	BOOL bRet = FALSE;

	m_strAvatarFilePath = strFilePath;

	m_OnCompleteSetAvatar.clear();

 	if( delegate.GetFn() != NULL )
 		m_OnCompleteSetAvatar += delegate;

	bRet = GetUserAvatarSession();

	return bRet;
}

BOOL CNDCloudUser::SetUserAvatarWithSession()
{
	BOOL bRet = FALSE;
	
	TCHAR szUrl[1024];
	_stprintf_s(szUrl, UserSetAvatar, m_strAvatarSession.c_str());

	TCHAR szPost[1024];
	_stprintf_s(szPost, _T("%s/%d.jpg"), m_strAvatarServerPath.c_str(), GetUserId() );
	DWORD dwTaskId = HttpUploadManager::GetInstance()->AddUploadTask(EduPlatformHostRes, szUrl, 
		m_strAvatarFilePath.c_str(), szPost , _T("POST"), 80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnSetUserAvatarResponse), MakeHttpDelegate(NULL), TRUE, TRUE);

	return bRet;
}

bool CNDCloudUser::OnSetUserAvatarResponse( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;


	if (pNotify->dwErrorCode==0)
	{
		pNotify->pData[pNotify->nDataSize] = '\0';
		string str = pNotify->pData;
		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(str, root);
		if(!res||root.get("dentry_id", Json::Value()).isNull())
		{
			pNotify->dwErrorCode=5;
		}
	}

	if( m_OnCompleteSetAvatar )
		m_OnCompleteSetAvatar(pNotify);

	return TRUE;
}


BOOL CNDCloudUser::GetUserAvatarSession()
{
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return FALSE;
	
	string strAuthorization = GetAuthorizationHeader(Str2Ansi(UserAvatarSessionHost), Str2Ansi(UserSessionAvatar), "GET");

	tstring strHeader = _T("Content-Type: application/json");
	strHeader += _T("\r\n");
	strHeader += Ansi2Str(strAuthorization);

	DWORD dwTaskId = pHttpManager->AddTask(UserAvatarSessionHost, 
		UserSessionAvatar,
		strHeader.c_str(),
		_T("GET"),  
		"", 
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnGetUserAvatarSessionResponse), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	return TRUE;
}

bool CNDCloudUser::OnGetUserAvatarSessionResponse( void* param )
{
	THttpNotify* pNotify = (THttpNotify*)param;

	if(pNotify->dwErrorCode != 0)
	{
		if( m_OnCompleteSetAvatar )
			m_OnCompleteSetAvatar(pNotify);
		return false;
	}

	pNotify->pData[pNotify->nDataSize] = '\0';
	string str = pNotify->pData;

	if( str != "" )
	{
		// register failed
		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(str, root);
		if( !res )
			return false;

		if( !root["session"].isNull() && !root["path"].isNull() )
		{
			m_strAvatarSession = Utf82Str(root["session"].asCString());
			m_strAvatarServerPath = Utf82Str(root["path"].asCString());
			SetUserAvatarWithSession();
			return true;
		}
	}

	if( m_OnCompleteSetAvatar )
		m_OnCompleteSetAvatar(pNotify);


	return true;
}

//-----------------------------------------------------------
// obtain check code image
//
BOOL CNDCloudUser::ObtainCheckCodeImage(CHttpDelegateBase& delegate)
{
	m_bSuccess				= FALSE;
	m_strErrorCode			= _T("");
	m_strErrorMessage		= _T("");

	m_OnCompleteCheckCode.clear();
	if( delegate.GetFn() != NULL )
		m_OnCompleteCheckCode += delegate;

	// generate guid
	m_CheckCodeSession = Ansi2Str(NewGuid(false));

	// set cookie
	tstring strUrl = _T("http://");
	strUrl += UserEspHost;

	InternetSetCookie(strUrl.c_str(), _T("JSESSIONID"), m_CheckCodeSession.c_str());


	DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(UserEspHost, UserEspCheckCodeUrl, _T(""), _T("GET"),  "", 80,
		MakeHttpDelegate(this, &CNDCloudUser::OnCheckCodeImageObtained),
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL));
										

	return TRUE;
}

bool CNDCloudUser::OnCheckCodeImageObtained(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	if( pNotify->dwErrorCode != 0 )
	{
		if( m_OnCompleteCheckCode )
			m_OnCompleteCheckCode(param);

		return false;
	}

	// save to temp path
	TCHAR szTempPath[MAX_PATH];
	GetTempPath(MAX_PATH, szTempPath);

	TCHAR szFilePath[MAX_PATH];
	_stprintf_s(szFilePath, _T("%scheckcode_%08lX.png"), szTempPath, GetTickCount());

	FILE* fp = NULL;
	errno_t err = _tfopen_s(&fp, szFilePath, _T("wb"));
	if( err == 0 )
	{
		fwrite(pNotify->pData, pNotify->nDataSize, 1, fp);
		fclose(fp);

		pNotify->strFilePath = szFilePath;
		m_bSuccess = TRUE;

		if( m_OnCompleteCheckCode )
			m_OnCompleteCheckCode(param);
	}
	else
	{
		pNotify->strFilePath = _T("");
		if( m_OnCompleteCheckCode )
			m_OnCompleteCheckCode(param);
	}

	return true;
}

//-----------------------------------------------------------
// send register short message
//
BOOL CNDCloudUser::SendRegisterShortMessage(tstring strPhoneNumber, CHttpDelegateBase& delegate)
{
	m_bSuccess				= FALSE;
	m_strErrorCode			= _T("");
	m_strErrorMessage		= _T("");

	m_OnCompleteSendSms.clear();

	if( delegate.GetFn() != NULL )
		m_OnCompleteSendSms += delegate;


	// json
	Json::Value root;
	root["mobile"]	= Str2Utf8(strPhoneNumber);
 

	Json::FastWriter writter;
	string str = writter.write(root);

	DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(UserEspHost, 
																UserEspSendRegSMS,
																_T("Content-Type: application/json"), 
																_T("POST"),  
																str.c_str(), 
																80,
																MakeHttpDelegate(this, &CNDCloudUser::OnRegisterShortMessageSent),
																MakeHttpDelegate(NULL),
																MakeHttpDelegate(NULL));

	return TRUE;
}

bool CNDCloudUser::OnRegisterShortMessageSent(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	
	if( m_OnCompleteSendSms )
		m_OnCompleteSendSms(param);

	return true;
}

//-----------------------------------------------------------
// query login name exist
//
BOOL CNDCloudUser::QueryLoginNameExist(tstring strUserName, CHttpDelegateBase &delegate)
{
	m_bSuccess				= FALSE;
	m_strErrorCode			= _T("");
	m_strErrorMessage		= _T("");
	m_bLoginNameExist		= FALSE;

	m_OnCompleteQueryLoginExist.clear();

	if( delegate.GetFn() != NULL )
		m_OnCompleteQueryLoginExist += delegate;

	// generate guid
	string strGuid = NewGuid(false);

	// set cookie
	tstring strFullUrl = _T("http://");
	strFullUrl += UserEspHost;
	strFullUrl += UserEspQueryUserExist;
	strFullUrl += strUserName;

	InternetSetCookie(strFullUrl.c_str(), _T("JSESSIONID"), Ansi2Str(strGuid).c_str());

	tstring strUrl = UserEspQueryUserExist;
	strUrl += strUserName;

	DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(UserEspHost, strUrl.c_str(), _T(""), _T("GET"),  "", 80,
								MakeHttpDelegate(this, &CNDCloudUser::OnLoginExistQueried),
								MakeHttpDelegate(NULL),
								MakeHttpDelegate(NULL));

							
	return TRUE;
}

bool CNDCloudUser::OnLoginExistQueried(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = pNotify->pData;
	
	Json::Reader reader;
	Json::Value root;

	bool res = reader.parse(str, root);
	if( res )
	{
		m_bSuccess = TRUE;

		if( !root["exists"].isNull() )
			m_bLoginNameExist = root["exists"].asBool();
	}

	if( m_OnCompleteQueryLoginExist )
		m_OnCompleteQueryLoginExist(param);

	return true;
}

//-----------------------------------------------------------
// send active email
//
BOOL CNDCloudUser::SendActiveEmail(tstring strEmail, CHttpDelegateBase& delegate)
{
	m_bSuccess				= FALSE;
	m_strErrorCode			= _T("");
	m_strErrorMessage		= _T("");

	m_OnCompleteSendActiveEmail.clear();

	if( delegate.GetFn() != NULL )
		m_OnCompleteSendActiveEmail += delegate;

	// json
	Json::Value root;
	root["email"]	= Str2Utf8(strEmail);
 

	Json::FastWriter writter;
	string str = writter.write(root);


	DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(UserEspHost, UserEspSendActiveEmail, 
																_T("Content-Type: application/json"), _T("POST"),  str.c_str(), 80,
																MakeHttpDelegate(this, &CNDCloudUser::OnActiveEmailSent),
																MakeHttpDelegate(NULL),
																MakeHttpDelegate(NULL));

	return TRUE;
}

bool CNDCloudUser::OnActiveEmailSent(void* param)
{
	THttpNotify* pNotify = (THttpNotify*)param;
	pNotify->pData[pNotify->nDataSize] = '\0';

	string str = pNotify->pData;
	if( str != "" )
	{
		// register failed
		Json::Reader reader;
		Json::Value root;

		bool res = reader.parse(str, root);
		if( !res )
			return false;

		if( root["code"].isNull() )
			return false;

		m_strErrorCode = Utf82Str(root["code"].asCString());
		m_strErrorMessage = Utf82Str(root["message"].asCString());

		m_bSuccess = FALSE;

	}
	else
	{
		// register success
		m_bSuccess = TRUE;
	}


	if( m_OnCompleteSendActiveEmail )
		m_OnCompleteSendActiveEmail(param);

	return true;
}

//-----------------------------------------------------------
// cancel
//
void CNDCloudUser::CancelLogin()
{
	m_OnCompleteLogin.clear();
}

void CNDCloudUser::CancelThirdLogin()
{
	 
}

void CNDCloudUser::CancelRegister()
{
	m_OnCompleteRegister.clear();
}

void CNDCloudUser::CancelObtainCheckCodeImage()
{
	m_OnCompleteCheckCode.clear();
}

void CNDCloudUser::CancelSendRegisterShortMessage()
{
	m_OnCompleteSendSms.clear();
}

void CNDCloudUser::CancelQueryLoginNameExist()
{
	m_OnCompleteQueryLoginExist.clear();
}

void CNDCloudUser::CancelSendActiveEmail()
{
	m_OnCompleteSendActiveEmail.clear();
}

void CNDCloudUser::CancelOperaUserAvatar()
{
	m_OnCompleteSetAvatar.clear();
}

//-----------------------------------------------------------
// utility functions
//
string CNDCloudUser::MD5EncryptString(string content)
{
	if (content.length() == 0)
		return "";

	static const char *salt = "fdjf,jkgfkl";
	static unsigned char bytes[4] = { 163, 172, 161, 163 };

	char szBuffer[16] = {0};
	memcpy(szBuffer, bytes, 4);

	content.append(szBuffer);
	content.append(salt);
 
	MD5 md5(content);
	std::string result = md5.md5();

	return result;
}

string CNDCloudUser::GetAuthorizationHeader(string strHost, string strUrl, string strMethod)
{
	// calculate mac value
	INT64 curTime = 0;
	if(m_dwTickCout != 0)
		curTime = m_dwServerTime * 1000 + (GetTickCount() - m_dwTickCout);
	else
		curTime = GetMilliTime();;

	char szTime[64] = {0};
	_i64toa_s(curTime, szTime, 64, 10);

	string strRand = GenerateRandomString();
	string strNonce = szTime;
	strNonce += ":";
	strNonce += strRand;

	string strMac;
	strMac += strNonce;
	strMac += "\n";
	strMac += strMethod;
	strMac += "\n";
	strMac += strUrl;
	strMac += "\n";
	strMac += strHost;
	strMac += "\n";

	unsigned char digest[32] = {0};
	hmac_sha256_get(digest, (const uint8_t *)strMac.c_str(), strMac.length(), (const uint8_t *)Str2Ansi(m_strMacKey).c_str(), m_strMacKey.length());

	string strOut;
	CBase64::Encode(digest, 32, strOut);

	// Authorization header
	char szAuthorization[1024];
	sprintf_s(szAuthorization, "Authorization: MAC id=\"%s\",nonce=\"%s\",mac=\"%s\"", Str2Ansi(m_strAccessToken).c_str(), strNonce.c_str(), strOut.c_str());

	return szAuthorization;

}

string CNDCloudUser::GenerateRandomString()
{
	char szBuffer[32] = {0};
	srand(GetCycleCount());

	int i = 0;
	for(; i < 5; i++)
		szBuffer[i] = rand() % 26 + 'a';
	
	for(; i < 8; i++)
		szBuffer[i] = rand() % 10 + '0';

	return szBuffer;
}

int CNDCloudUser::GetCurStep()
{
	return m_nCurStep;
}

BOOL CNDCloudUser::IsSuccess()
{
	return m_bSuccess;
}

BOOL CNDCloudUser::IsLoginNameExist()
{
	return m_bLoginNameExist;
}

DWORD CNDCloudUser::GetUserId()
{
	return m_dwUserId;
}

tstring CNDCloudUser::GetUserName()
{
	return m_strUserName;
}

tstring CNDCloudUser::GetPassword()
{
	return m_strPassword;
}

tstring CNDCloudUser::GetOriganize()
{
	return m_strOriganize;
}

tstring CNDCloudUser::GetErrorCode()
{
	return m_strErrorCode;
}

tstring CNDCloudUser::GetErrorMessage()
{
	return m_strErrorMessage;
}

tstring CNDCloudUser::GetNickName()
{
	return m_strNickName;
}

tstring CNDCloudUser::GetRealName()
{
	return m_strRealName;
}

tstring CNDCloudUser::GetSex()
{
	return m_strSex;
}

tstring CNDCloudUser::GetSubjects()
{
	return m_strSubjects;
}

tstring CNDCloudUser::GetOrgNodeName()
{
	return m_strOrgNodeName;
}

CStream* CNDCloudUser::GetClassRooms()
{
	if( m_pClassroomStream != NULL )
		m_pClassroomStream->ResetCursor();

	return m_pClassroomStream;
}

void CNDCloudUser::SetClassGuid(tstring strClassGuid)
{
	m_strClassGuid = strClassGuid;
}

tstring CNDCloudUser::GetClassGuid()
{
	return m_strClassGuid;
}

void CNDCloudUser::LogOut()
{
	m_nCurStep					= UCSTEP_NONE;
	m_bSuccess					= FALSE; 
	m_dwUserId					= 0;
	m_strClassGuid				= _T("");
	m_tLastMobileRqst  = 0;

	if( m_pStudentInfoStream != NULL )
	{
		char* pBuffer = m_pStudentInfoStream->GetBuffer();
		int nBufferSize = m_pStudentInfoStream->GetBufferSize();

		if( pBuffer != NULL && nBufferSize != 0 )
			memset(pBuffer, 0, nBufferSize);
	}

	if( m_pGroupInfoStream != NULL )
	{
		char* pBuffer = m_pGroupInfoStream->GetBuffer();
		int nBufferSize = m_pGroupInfoStream->GetBufferSize();

		if( pBuffer != NULL && nBufferSize != 0 )
			memset(pBuffer, 0, nBufferSize);
	}

	if( m_pDeviceInfoStream != NULL )
	{
		char* pBuffer = m_pDeviceInfoStream->GetBuffer();
		int nBufferSize = m_pDeviceInfoStream->GetBufferSize();

		if( pBuffer != NULL && nBufferSize != 0 )
			memset(pBuffer, 0, nBufferSize);
	}
	m_nIsFreeMode = 1;

}

tstring CNDCloudUser::GetLoginResponse()
{
	return m_strLoginResponse;
}

tstring CNDCloudUser::GetBearerToken()
{
	return m_strBearerToken;
}

tstring CNDCloudUser::GetStudentInfo()
{
	if( m_pStudentInfoStream == NULL || m_pStudentInfoStream->GetDataSize() == 0 )
		return _T("");

	m_pStudentInfoStream->ResetCursor();

	tstring str = m_pStudentInfoStream->ReadString();
	return str;

}

tstring CNDCloudUser::GetGroupInfo()
{
	if( m_pGroupInfoStream == NULL || m_pGroupInfoStream->GetDataSize() == 0 )
		return _T("");


	m_pGroupInfoStream->ResetCursor();

	tstring str = m_pGroupInfoStream->ReadString();
	return str;
}

tstring CNDCloudUser::GetDeviceInfo()
{
	if( m_pDeviceInfoStream == NULL || m_pDeviceInfoStream->GetDataSize() == 0 )
		return _T("");


	m_pDeviceInfoStream->ResetCursor();

	tstring str = m_pDeviceInfoStream->ReadString();
	return str;
}

int CNDCloudUser::GetLoginType()
{
	return m_nLoginType;
}

void CNDCloudUser::SetLoginType( int loginType )
{
	m_nLoginType = loginType;
}

tstring CNDCloudUser::GetFullName()
{
	return m_strFullName;
}
 

tstring CNDCloudUser::GetTeacherGrade(tstring sFullName)
{
	// 示例"学校节点|福州教育学院附属第三小学|年级|小五|7班"
	tstring sResult = _T("");
	if (sFullName.empty())
	{
		return sResult;
	}
  
	CString strGradeName = _T("");
	CString strGradeClassNO = _T(""); 
 
 
	CString strFullName = sFullName.c_str(); 
	AfxExtractSubString(strGradeName, strFullName, 3, '|'); 
	AfxExtractSubString(strGradeClassNO, strFullName, 4, '|');  
 
	sResult = strGradeName.GetBuffer();
	//if (!sResult.empty())
	//{
	//	sResult += _T("(");
	//}
	sResult += strGradeClassNO.GetBuffer();
	//if (!sResult.empty())
	//{
	//	sResult += _T(")");
	//} 
	return sResult;
} 

//
// load user history data
//
BOOL CNDCloudUser::LoadHistoryData()
{
	tstring strPath = GetLocalPath();
	strPath += _T("\\setting\\UserHistory.dat");

	FILE* fp = NULL;
	errno_t err = _tfopen_s(&fp, strPath.c_str(),_T("rb"));
	if( err != 0 )
		return FALSE;

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* pBuffer = new char[size];
	if( pBuffer == NULL )
	{
		fclose(fp);
		return FALSE;
	}

	fread(pBuffer, size, 1, fp);
	fclose(fp);

	//
	CStream stream(pBuffer, size);
	int nCount = stream.ReadInt();


	// read each user data
	for(int i = 0; i < nCount; i++)
	{
		tstring str = stream.ReadString();

	}

	return TRUE;
}

//
// save user history data
// 
BOOL CNDCloudUser::SaveHistoryData()
{
	return TRUE;
}

tstring CNDCloudUser::GetAccessToken()
{
	return m_strAccessToken;
}

tstring CNDCloudUser::GetMacKey()
{
	return m_strMacKey;
} 

bool CNDCloudUser::OnCreateUserPhoto( void * pParam )
{
	THttpNotify* pNotify = (THttpNotify*)pParam;
	pNotify->pData[pNotify->nDataSize] = '\0';

	//
	string str = pNotify->pData; 
	return true;
}

bool CNDCloudUser::OnGetUserPhoto( void * pParam )
{
	THttpNotify* pNotify = (THttpNotify*)pParam;  
	m_strTeacherPhoto = pNotify->strFilePath; 
	return true;
}

void CNDCloudUser::GetTeacherPhoto()
{
	//SetUserAvatar(_T("d:\\09.png"), MakeHttpDelegate(this, &CNDCloudUser::OnCreateUserPhoto));
	GetUserAvatar(80, MakeHttpDelegate(this, &CNDCloudUser::OnGetUserPhoto));
}

BOOL CNDCloudUser::GetIsLoadAllTeacherInfo()
{	    
#ifdef _DEBUG0
	return FALSE;
#endif
	if (m_bUserInfoSuccess && m_bClassRoomInfoSuccess && m_bStudentInfoSuccess && m_bGroupInfoSuccess && m_bDeviceInfoSuccess)
	{
		return TRUE;
	}
	return FALSE; 
}

void CNDCloudUser::LoadTeachRoomInfo( tstring sOrgID, tstring sClassID )
{
	TCHAR szOrgId[MAX_PATH];
	_stprintf_s(szOrgId, _T("%s"), sOrgID.c_str());

	TCHAR szClassId[MAX_PATH];
	_stprintf_s(szClassId, _T("%s"), sClassID.c_str());

	m_strClassGuid = szClassId;

	TCHAR szUrl[MAX_URL_LEN];
	_stprintf_s(szUrl, ClassroomStudentInfo, szOrgId, szClassId);


	setLoadTeacherInfoState(TRUE);
	m_bStudentInfoSuccess = FALSE;  
	m_bDeviceInfoSuccess = FALSE;
	m_bGroupInfoSuccess = FALSE;
	CHttpDownloadManager* pHttpManager = HttpDownloadManager::GetInstance();
	if( pHttpManager == NULL )
		return ;

	// authorization
	string strAuthorization = GetAuthorizationHeader(Str2Ansi(ClassroomHost), Str2Ansi(szUrl), "GET");

	tstring strHeader = _T("Content-Type: application/json");
	strHeader += _T("\r\n");
	strHeader += Ansi2Str(strAuthorization);

	pHttpManager->AddTask(ClassroomHost, 
		szUrl, 
		strHeader.c_str(),
		_T("GET"), 
		"",
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnStudentInfoObtained), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) ); 

	//
	// obtain group information of first class
	//
	_stprintf_s(szUrl, ClassroomGroupInfo, szClassId); 

	strAuthorization = GetAuthorizationHeader(Str2Ansi(ClassroomHost), Str2Ansi(szUrl), "GET");

	strHeader = _T("Content-Type: application/json");
	strHeader += _T("\r\n");
	strHeader += Ansi2Str(strAuthorization);


	pHttpManager->AddTask(ClassroomHost, 
		szUrl, 
		strHeader.c_str(),
		_T("GET"), 
		"",
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnGroupInfoObtained), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) );

	//
	// obtain device information of first class
	//
	_stprintf_s(szUrl, ClassroomDeviceInfo, szClassId); 

	pHttpManager->AddTask(ClassroomHost, 
		szUrl, 
		_T(""),
		_T("GET"), 
		"",
		80, 
		MakeHttpDelegate(this, &CNDCloudUser::OnDeviceInfoObtained), 
		MakeHttpDelegate(NULL),
		MakeHttpDelegate(NULL) ); 
}

void CNDCloudUser::ReLoadTeachRoomInfo( tstring sClassID )
{ 
	if (g_vecsFullNameList.size() == 0)
	{ 
		return;
	} 
	Json::Value root;
	Json::Reader reader; 
	tstring strOrgID;
	tstring strClassID;

	for (int i = 0; i < g_vecsFullNameList.size(); i++)
	{ 
		string str = g_vecsFullNameList[i].toStyledString();

		bool res = reader.parse(str, root); 
		 
		strOrgID = Utf82Str(root["gradeinfo"]["orgid"].asCString());
		strClassID =Utf82Str(root["gradeinfo"]["classid"].asCString());
		if (strClassID.compare(sClassID.c_str()) == 0)
		{
			break;;
		}		 
	}

	LoadTeachRoomInfo(strOrgID, strClassID);
}

void CNDCloudUser::ReLoadTeachRoomInfo()
{ 
	if (g_vecsFullNameList.size() == 0)
	{ 
		return;
	} 
	Json::Value root;
	Json::Reader reader; 
	tstring strOrgID;
	tstring strClassID;

	 
	string str = g_vecsFullNameList[m_nGradeIndex].toStyledString();

	bool res = reader.parse(str, root); 

	strOrgID = Utf82Str(root["gradeinfo"]["orgid"].asCString());
	strClassID = Utf82Str(root["gradeinfo"]["classid"].asCString()); 

	LoadTeachRoomInfo(strOrgID, strClassID); 
}

tstring CNDCloudUser::GetLoadALLTeacherTip()
{
	tstring sResult = _T("");
	if (!m_bUserInfoSuccess)
	{
		sResult = _T("老师信息获取中，请稍等");
	}
	else
	{
		sResult = _T("老师信息获取完毕");
	}
	if (!m_bStudentInfoSuccess)
	{
		sResult = _T("班级中学生信息获取中，请稍等");
	}
	else
	{
		sResult = _T("班级中学生信息获取完毕");
	}
	if (!m_bGroupInfoSuccess)
	{
		sResult = _T("分组信息获取中，请稍等");
	}
	else
	{
		sResult = _T("分组信息获取完毕");
	}
	if (!m_bDeviceInfoSuccess)
	{
		sResult = _T("设备信息获取中，请稍等");
	}
	else
	{
		sResult = _T("设备信息获取完毕");
	}
	if (!m_bClassRoomInfoSuccess)
	{
		sResult = _T("班级信息获取中，请稍等");
	}
	else
	{
		sResult = _T("班级信息获取完毕");
	}
	if (GetIsLoadAllTeacherInfo())
	{
		sResult = _T("所有信息获取完毕");
	}
	else
	{
		sResult = _T("班级信息还未加载完成，请退出全屏播放重新进入");
	}
	return sResult;
}

void CNDCloudUser::SetFreeMode( int nFreeMode )
{
	m_nIsFreeMode = nFreeMode;
	if (nFreeMode == 1)
	{
		setLoadTeacherInfoState(TRUE);
		if( m_pStudentInfoStream != NULL )
		{
			char* pBuffer = m_pStudentInfoStream->GetBuffer();
			int nBufferSize = m_pStudentInfoStream->GetBufferSize();

			if( pBuffer != NULL && nBufferSize != 0 )
				memset(pBuffer, 0, nBufferSize);
		}

		if( m_pGroupInfoStream != NULL )
		{
			char* pBuffer = m_pGroupInfoStream->GetBuffer();
			int nBufferSize = m_pGroupInfoStream->GetBufferSize();

			if( pBuffer != NULL && nBufferSize != 0 )
				memset(pBuffer, 0, nBufferSize);
		}

		if( m_pDeviceInfoStream != NULL )
		{
			char* pBuffer = m_pDeviceInfoStream->GetBuffer();
			int nBufferSize = m_pDeviceInfoStream->GetBufferSize();

			if( pBuffer != NULL && nBufferSize != 0 )
				memset(pBuffer, 0, nBufferSize);
		}
	}
}

bool CNDCloudUser::CheckItemsSize(tstring strValue, tstring strItemName)
{  
	Json::Value root;
	Json::Reader reader;  


	string str = Str2Ansi(strValue);

	bool res = reader.parse(str, root); 

	int nSize = root[Str2Utf8(strItemName).c_str()].size();  
	if (nSize > 0)
	{
		return true;
	}
	return false;
}

BOOL CNDCloudUser::IcrOnStartInit( CStream* pStream )
{
	m_pFullScreenStream->ResetCursor();
	m_pFullScreenStream->WriteString(pStream->GetBuffer());
	if (GetIsLoadAllTeacherInfo())
	{
		IcrPlayer::GetInstance()->IcrOnStart(pStream);
	}
	return TRUE;
}

BOOL CNDCloudUser::IcrStart()
{
	IcrPlayer::GetInstance()->IcrOnStart(m_pFullScreenStream);
	return TRUE;
}

vector<Json::Value> CNDCloudUser::GetGradeList()
{
	return g_vecsFullNameList;
}


