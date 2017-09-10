//-----------------------------------------------------------------------
// FileName:				NDCloudResModel.cpp
//
// Desc:
//------------------------------------------------------------------------
#include "stdafx.h"
#include "Util/Util.h"
#include "NDCloudResModel.h"
#include "NDCloud/NDCloudUser.h"
#include "NDCloudQuestion.h"
#include <openssl/md5.h>

#define EduPlatformHost								_T("esp-lifecycle.web.sdp.101.com")
#define EduPlatformUrlCourseModel					_T("/v0.6/coursewares/%s")
#define EduPlatformUrlAssetsModel					_T("/v0.6/assets/%s")
#define EduPlatformUrlQuestionModel					_T("/v0.6/questions/%s")
#define EduPlatformUrlCoursewareObjectModel			_T("/v0.6/coursewareobjects/%s")


CNDCloudResourceModel::CNDCloudResourceModel()
{
	m_nResModelType = RESMODEL_NONE;
	m_bCover		= false;
}

CNDCloudResourceModel::~CNDCloudResourceModel()
{

}

void CNDCloudResourceModel::SetResourceType(int nResModelType, tstring strResType)
{
	m_nResModelType = nResModelType;

	tstring strResCode;
	for(int i = 0;i<sizeof(tagQuestionResTypeListEx)/sizeof(tagQuestionResTypeListEx[0]);i++)
	{
		if(_tcsicmp(tagQuestionResTypeListEx[i].szQuestionResType , strResType.c_str()) == 0 )
		{
			strResCode = tagQuestionResTypeListEx[i].szQuestionResDescription;
			break;
		}
	}

	/*
	{
		"identifier": "5b37d9a0-a7cb-45a1-834c-97dfd4afb316",
		"taxonpath": null,
		"taxoncode": "$RE0200",
		"taxonname": "试题"
	},
	{
		"identifier": "3eabbc1c-0181-4456-99d1-c2df20460fb1",
		"taxonpath": null,
		"taxoncode": "$RE0201",
		"taxonname": "单选题"
	}*/

	Json::Value CategoryItem;

	if( !m_Root["categories"].isNull() )
		CategoryItem = m_Root["categories"];

	Json::Value itemArray(Json::arrayValue);
	Json::Value itemMajor;
	Json::Value itemMinor;

 
	itemMajor["taxoncode"]		= "$RE0200";
	itemMinor["taxoncode"]		= Str2Utf8(strResCode);

	itemArray.append(itemMajor);
	itemArray.append(itemMinor);

	CategoryItem["res_type"]		= itemArray;
	m_Root["categories"]			= CategoryItem;
}

void CNDCloudResourceModel::SetGuid(tstring strGuid)
{
	m_Root["identifier"] = Str2Utf8(strGuid);
	m_strGuid = strGuid;
}

void CNDCloudResourceModel::SetBasicInfo( tstring strTitle, tstring strLanguage /*= _T("zh-cn")*/, tstring strDescription /*= _T("") */ )
{
	m_Root["title"]			= Str2Utf8(strTitle);
	m_Root["description"]	= Str2Utf8(strDescription);
	m_Root["language"]		= Str2Utf8(strLanguage);
}

void CNDCloudResourceModel::SetPreview(tstring strFormat, tstring strUrl)
{
	Json::Value PreviewItem(Json::objectValue);

	if( strFormat != _T("") && strUrl != _T("") )
		PreviewItem[Str2Ansi(strFormat)] = Str2Utf8(strUrl);

	m_Root["preview"] = PreviewItem;
}

void CNDCloudResourceModel::SetTechInfo(tstring strFileLocalPath, tstring strFileServerPath, tstring strFullFileServerPath /*= _T("")*/)
{
	if( m_strGuid == _T("") )
		return;

	// calculate file size
	FILE* fp = NULL;
	errno_t err = _tfopen_s(&fp, strFileLocalPath.c_str(), _T("rb"));
	if( err != 0 )
		return;

	fseek(fp, 0, SEEK_END);
	int nSize = ftell(fp);
	fclose(fp);


	// "application/vnd.ms-powerpoint"
	// source
	Json::Value SourceItem;

	int pos = strFileLocalPath.rfind('.');
	if( pos == -1 )
		return;

	tstring strFormat = strFileLocalPath.substr(pos+1);
	_tcslwr((TCHAR*)strFormat.c_str());

	if( m_nResModelType == RESMODEL_NONE )
	{
		if( strFormat == _T("ppt") || strFormat == _T("pptx") || strFormat == _T("ndpx") )
			m_nResModelType = RESMODEL_COURSEWARE;
		else
			m_nResModelType = RESMODEL_ASSETS;
	}
	

	tstring strServerFormat = GetFileFormat(strFormat); 

	pos = strFileLocalPath.rfind('\\');

	tstring strFileName = strFileLocalPath.substr(pos+1);

	//auto truncate filename to avoid length limit of server
	if (strFileName.length() > 100)
	{
		//strFormat.length() + 1  .jpg
		TCHAR szBuffer[MAX_PATH] = {0};
		_tcsncpy_s(szBuffer, strFileName.c_str(), 100 - (strFormat.length() + 1));
		strFileName = szBuffer;
		strFileName += _T(".");
		strFileName	+= strFormat;
	}

	strFileServerPath = _T("${ref-path}") + strFileServerPath;
	strFileServerPath += _T("/");
	strFileServerPath += strFileName;

	// file md5
	tstring strMD5 = CalcFileMD5(strFileLocalPath);

	// source item
	if( strFullFileServerPath != _T("") )
	{
		strFullFileServerPath = _T("${ref-path}") + strFullFileServerPath;
		SourceItem["location"]	= Str2Utf8(strFullFileServerPath);
	}
	else
		SourceItem["location"]	= Str2Utf8(strFileServerPath);

	SourceItem["format"]	= Str2Utf8(strServerFormat);
	SourceItem["size"]		= nSize;
	SourceItem["md5"]		= Str2Utf8(strMD5);

	
	Json::Value TechInfoItem;
	TechInfoItem["source"]	= SourceItem;

	// href item
	Json::Value HrefItem;

	if( strFullFileServerPath != _T("") )
		HrefItem["location"]	= Str2Utf8(strFullFileServerPath);
	else
		HrefItem["location"]	= Str2Utf8(strFileServerPath);

	HrefItem["format"]		= Str2Utf8(strServerFormat);
	HrefItem["size"]		= nSize;
	HrefItem["md5"]			= Str2Utf8(strMD5);

	TechInfoItem["href"] = HrefItem;

	// root
	m_Root["tech_info"] = TechInfoItem;
}

void CNDCloudResourceModel::SetCategoryInfo(CCategoryTree* pCategoryTree)
{
	if( pCategoryTree == NULL )
		return;

	// find section node
	tstring strSelSectionCode		= pCategoryTree->GetSelectedSectionCode();
	tstring strSelGradeCode			= pCategoryTree->GetSelectedGradeCode();
	tstring	strSelCourseCode		= pCategoryTree->GetSelectedCourseCode();
	tstring strSelEditionCode		= pCategoryTree->GetSelectedEditionCode();
	tstring strSelSubEditionCode	= pCategoryTree->GetSelectedSubEditionCode();

	CategoryNode* pSectionNode		= pCategoryTree->FindNode(strSelSectionCode);
	CategoryNode* pGradeNode		= pCategoryTree->FindNode(strSelSectionCode, strSelGradeCode);
	CategoryNode* pCourseNode		= pCategoryTree->FindNode(strSelSectionCode, strSelGradeCode, strSelCourseCode);
	CategoryNode* pEditionNode		= pCategoryTree->FindNode(strSelSectionCode, strSelGradeCode, strSelCourseCode, strSelEditionCode);
	CategoryNode* pSubEditionNode	= pCategoryTree->FindNode(strSelSectionCode, strSelGradeCode, strSelCourseCode, strSelEditionCode, strSelSubEditionCode);

	// tax on path
	tstring strTaxOnPath = _T("K12/");
	strTaxOnPath += strSelSectionCode;
	strTaxOnPath += _T("/");
	strTaxOnPath += strSelGradeCode;
	strTaxOnPath += _T("/");
	strTaxOnPath += strSelCourseCode;
	strTaxOnPath += _T("/");
	strTaxOnPath += strSelEditionCode;
	strTaxOnPath += _T("/");
	strTaxOnPath += strSelSubEditionCode;
	

	// json item
	Json::Value CategoryItem;

	if( !m_Root["categories"].isNull() )
		CategoryItem = m_Root["categories"];


	Json::Value SectionItems(Json::arrayValue);
	Json::Value GradeItems(Json::arrayValue);
	Json::Value CourseItems(Json::arrayValue);
	Json::Value EditionItems(Json::arrayValue);
	Json::Value SubEditionItems(Json::arrayValue);
	Json::Value AssetTypeItems(Json::arrayValue);

	Json::Value SectionItem;
	Json::Value GradeItem;
	Json::Value CourseItem;
	Json::Value EditionItem;
	Json::Value SubEditionItem;
	Json::Value AssetTypeItem;

	// section
	if( pSectionNode != NULL )
	{
		SectionItem["taxonpath"]	= Str2Utf8(strTaxOnPath);
		SectionItem["taxonname"]	= Str2Utf8(pSectionNode->strTitle);
		SectionItem["taxoncode"]	= Str2Utf8(pSectionNode->strNDCode);

		SectionItems.append(SectionItem);
	}

	// grade
	if( pGradeNode != NULL )
	{
		GradeItem["taxonpath"]		= Str2Utf8(strTaxOnPath);
		GradeItem["taxonname"]		= Str2Utf8(pGradeNode->strTitle);
		GradeItem["taxoncode"]		= Str2Utf8(pGradeNode->strNDCode);

		GradeItems.append(GradeItem);
	}

	// course
	if( pCourseNode != NULL )
	{
		CourseItem["taxonpath"]		= Str2Utf8(strTaxOnPath);
		CourseItem["taxonname"]		= Str2Utf8(pCourseNode->strTitle);
		CourseItem["taxoncode"]		= Str2Utf8(pCourseNode->strNDCode);

		CourseItems.append(CourseItem);
	}

	// edition
	if( pEditionNode != NULL )
	{
		EditionItem["taxonpath"]	= Str2Utf8(strTaxOnPath);
		EditionItem["taxonname"]	= Str2Utf8(pEditionNode->strTitle);
		EditionItem["taxoncode"]	= Str2Utf8(pEditionNode->strNDCode);

		EditionItems.append(EditionItem);
	}

	// sub edition
	if( pSubEditionNode != NULL )
	{
		SubEditionItem["taxonpath"]		= Str2Utf8(strTaxOnPath);
		SubEditionItem["taxonname"]		= Str2Utf8(pSubEditionNode->strTitle);
		SubEditionItem["taxoncode"]		= Str2Utf8(pSubEditionNode->strNDCode);

		SubEditionItems.append(SubEditionItem);
	}

	// assets_type
	if( m_strAssetsType != _T("") )
	{
		AssetTypeItem["taxonpath"]		= "";
		AssetTypeItem["taxoncode"]		= Str2Utf8(m_strAssetsType);

		AssetTypeItems.append(AssetTypeItem);
	}

	// categories
	CategoryItem["phase"]			= SectionItems;
	CategoryItem["grade"]			= GradeItems;
	CategoryItem["subject"]			= CourseItems;
	CategoryItem["edition"]			= EditionItems;
	CategoryItem["sub_edition"]		= SubEditionItems;
	CategoryItem["assets_type"]		= AssetTypeItems;

	m_Root["categories"] = CategoryItem;

}


void CNDCloudResourceModel::AddKeyWord(tstring strKeyWord)
{

}

void CNDCloudResourceModel::AddTag(tstring strTag)
{

}

void CNDCloudResourceModel::SetEducationInfo()
{


}

void CNDCloudResourceModel::SetLifeCycleInfo()
{
	Json::Value item;
	item["version"]		= "v0.9";
//	item["creator"]		= _T("PPTShell");
	DWORD dwUserId		= NDCloudUser::GetInstance()->GetUserId();
	TCHAR szUserId[MAX_PATH];
	_stprintf(szUserId, _T("%d"), dwUserId);
	item["creator"]		=  Str2Utf8(szUserId);
	item["enable"]		= "true";
	item["status"]		= "TRANSCODE_WAITING";
	item["provider"]	= "NetDragon Inc";
	 

	m_Root["life_cycle"] = item;
}

void CNDCloudResourceModel::SetLifeCycleStatus( tstring strStatus )
{
	m_Root["life_cycle"]["status"]		= Str2Utf8(strStatus);
}

void CNDCloudResourceModel::AddCoverage(tstring strTargetType, tstring strTarget, tstring strTargetTitle, tstring strStrategry)
{
	Json::Value coverageArray(Json::arrayValue);
	Json::Value coverage;

	if( !m_Root["coverages"].isNull() )
		coverageArray = m_Root["coverages"];

	coverage["target_type"]		= Str2Utf8(strTargetType);
	coverage["target"]			= Str2Utf8(strTarget);
	coverage["target_title"]	= Str2Utf8(strTarget);
	coverage["strategy"]		= Str2Utf8(strStrategry);

	coverageArray.append(coverage);

	m_Root["coverages"] = coverageArray;
}


void CNDCloudResourceModel::AddRelation( tstring strSource, tstring strSourceType, tstring strRelationType )
{
	Json::Value relationArray(Json::arrayValue);
	Json::Value relation;

	if( !m_Root["relations"].isNull() )
		relationArray = m_Root["relations"];

	relation["source"]			= Str2Utf8(strSource);
	relation["source_type"]		= Str2Utf8(strSourceType);
	relation["relation_type"]	= Str2Utf8(strRelationType);
	relation["enable"]			= "true";
	
	relationArray.append(relation);

	m_Root["relations"] = relationArray;
}

void CNDCloudResourceModel::AddRequirement(tstring strType, tstring strName, tstring strMinVersion, tstring strMaxVersion, tstring strValue)
{
	if( m_Root["tech_info"].isNull() || m_Root["tech_info"]["href"].isNull() )
		return;

	Json::Value requirementArray(Json::arrayValue);
	Json::Value requirement;


	// source
	if( !m_Root["tech_info"]["source"]["requirements"].isNull() )
		requirementArray = m_Root["tech_info"]["source"]["requirements"];

	requirement["type"]			= Str2Utf8(strType);
	requirement["name"]			= Str2Utf8(strName);
	requirement["min_version"]	= Str2Utf8(strMinVersion);
	requirement["max_version"]	= Str2Utf8(strMaxVersion);
	requirement["value"]		= Str2Utf8(strValue);

	requirementArray.append(requirement);
	m_Root["tech_info"]["source"]["requirements"] = requirementArray;
}

//
// Commit to database
//
DWORD CNDCloudResourceModel::CommitResourceModel(CHttpDelegateBase& delegate,void* pUserData)
{
	string strPost = m_Root.toStyledString();

	tstring strUrlPrefix;
	if( m_nResModelType == RESMODEL_COURSEWARE )
		strUrlPrefix = EduPlatformUrlCourseModel;

	else if( m_nResModelType == RESMODEL_ASSETS )
		strUrlPrefix = EduPlatformUrlAssetsModel;

	else if( m_nResModelType == RESMODEL_QUESTIONS )
		strUrlPrefix = EduPlatformUrlQuestionModel;

	else if( m_nResModelType == RESMODEL_COURSEWAREOBJECTS )
		strUrlPrefix = EduPlatformUrlCoursewareObjectModel;
	
	// Authorization header
	TCHAR szUrl[1024];
	_stprintf_s(szUrl, MAX_PATH, strUrlPrefix.c_str(), m_strGuid.c_str());

	string strMethod;
	if(m_bCover)
		strMethod = "PUT";
	else
		strMethod = "POST";
	
	string strAuthorization = NDCloudUser::GetInstance()->GetAuthorizationHeader(Str2Ansi(EduPlatformHost), Str2Ansi(szUrl), strMethod.c_str());


	tstring strHeader = _T("Content-Type: application/json");
	strHeader += _T("\r\n");
	strHeader += Ansi2Str(strAuthorization);

	//
	DWORD dwTaskId = HttpDownloadManager::GetInstance()->AddTask(EduPlatformHost, szUrl, strHeader.c_str(), Ansi2Str(strMethod).c_str(), strPost.c_str(), 80,
								delegate, 
								MakeHttpDelegate(NULL), 
								MakeHttpDelegate(NULL),TRUE,FALSE,0,pUserData);

	return dwTaskId;
}

tstring CNDCloudResourceModel::GetFileFormat(tstring strFormat)
{ 
	// 
	_tcslwr((TCHAR*)strFormat.c_str());

	if( strFormat == _T("ppt") || strFormat == _T("pptx") )
	{
		strFormat = _T("application/vnd.ms-powerpoint");
		m_strAssetsType = _T("");
	}
	else if( strFormat == _T("ndpx") )
	{
		strFormat = _T("ndf/cw-x");
		m_strAssetsType = _T("");
	}
	else if( strFormat == _T("bmp") || strFormat == _T("jpeg") || strFormat == _T("jpg") || strFormat == _T("png") || strFormat == _T("gif") )
	{
		strFormat = _T("image/") + strFormat;
		m_strAssetsType = _T("$RA0101");
	}
	else if( strFormat == _T("mp3") || strFormat == _T("wav") || strFormat == _T("wma") || strFormat == _T("mid") )
	{
		strFormat = _T("audio/") + strFormat;
		m_strAssetsType = _T("$RA0102");
	}
	else if( strFormat == _T("mp4") || strFormat == _T("mpeg") || strFormat == _T("rmvb") ||
		strFormat == _T("rm") || strFormat == _T("mpg") || strFormat == _T("avi") || strFormat == _T("wmv") ||
		strFormat == _T("mov") || strFormat == _T("asf") || strFormat == _T("3gp") || strFormat == _T("mkv") )
	{
		strFormat = _T("video/") + strFormat;
		m_strAssetsType = _T("$RA0103");
	}
	else if( strFormat == _T("flv") || strFormat == _T("swf") )
	{
		strFormat = _T("video/") + strFormat;
		m_strAssetsType = _T("$RA0104");
	}
	else if( strFormat == _T("xml") )
	{
		strFormat = _T("xml");
	}
	else
	{
		strFormat = _T("image/") + strFormat;
		m_strAssetsType = _T("$RA0101");
	}

	if( m_nResModelType == RESMODEL_QUESTIONS )
	{
		//strFormat = _T("application/edu.nd-qti-editor");
		strFormat = _T("xml");
		m_strAssetsType = _T("$RE0200");
	}
	else if( m_nResModelType == RESMODEL_COURSEWAREOBJECTS )
	{
		strFormat = _T("xml");
		m_strAssetsType = _T("$RE0400");
	}
	
	return strFormat;
}

tstring CNDCloudResourceModel::CalcFileMD5(tstring strFilePath)
{
	FILE* fp = NULL;

	errno_t err = _tfopen_s(&fp, strFilePath.c_str(), _T("rb"));
	if( err != 0 )
		return _T("");

	fseek(fp, 0, SEEK_END);
	int nSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	char* pBuffer = new char[nSize];
	if( pBuffer == NULL )
		return _T("");


	fread(pBuffer, nSize, 1, fp);
	fclose(fp);

	MD5_CTX ctx;  
	unsigned char digest[16] = {0}; 

	MD5_Init(&ctx);
	MD5_Update (&ctx, pBuffer, nSize);  
	MD5_Final (digest, &ctx);  

	delete pBuffer;

	char buf[33] = {0};  
	char tmp[3] = {0};  

	for(int i = 0; i < 16; i++ )  
	{  
		sprintf(tmp,"%02x", digest[i]);
		strcat(buf, tmp);
	}  

	return Ansi2Str(buf);
}

void CNDCloudResourceModel::SetCover( bool bCover )
{
	m_bCover = bCover;
}
