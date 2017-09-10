//-----------------------------------------------------------------------
// FileName:				NDCloudResModel.h
//
// Desc:
//------------------------------------------------------------------------
#ifndef _ND_CLOUD_RESOURCE_MODEL_H_
#define _ND_CLOUD_RESOURCE_MODEL_H_

#include "ThirdParty/json/json.h"
#include "NDCloud/CategoryTree.h"
#include "Util/Singleton.h"

enum
{
	RESMODEL_NONE,
	RESMODEL_COURSEWARE,
	RESMODEL_ASSETS,
	RESMODEL_QUESTIONS,
	RESMODEL_COURSEWAREOBJECTS,
};

class CNDCloudResourceModel
{
public:
	CNDCloudResourceModel();
	~CNDCloudResourceModel();


	// Belows are needed
	void    SetResourceType(int nResModelType, tstring strResType);
	void	SetGuid(tstring strGuid);
	void	SetBasicInfo(tstring strTitle, tstring strLanguage = _T("zh-cn"), tstring strDescription = _T("") );
	void	SetPreview(tstring strFormat = _T(""), tstring strUrl = _T(""));
	void	SetTechInfo(tstring strFileLocalPath, tstring strFileServerPath, tstring strFullFileServerPath = _T(""));
	void	SetCategoryInfo(CCategoryTree* pCategoryTree);
	
	// Belows are optional
	void	AddKeyWord(tstring strKeyWord) OPTIONAL;
	void	AddTag(tstring strTag) OPTIONAL;
	void	SetCustomProperity() OPTIONAL;
	void	SetLifeCycleInfo() OPTIONAL;
	void	SetLifeCycleStatus(tstring strStatus) OPTIONAL;
	void	SetEducationInfo() OPTIONAL;
	void	SetCopyRightInfo() OPTIONAL;
	void	AddCoverage(tstring strTargetType, tstring strTarget, tstring strTargetTitle, tstring strStrategry) OPTIONAL;
	void    AddRelation(tstring strSource, tstring strSourceType = _T("chapters"), tstring strRelationType = _T("ASSOCIATE")) OPTIONAL;
	void	AddRequirement(tstring strType, tstring strName, tstring strMinVersion, tstring strMaxVersion, tstring strValue);

	// Commit to database
	DWORD	CommitResourceModel(CHttpDelegateBase& delegate,void* pUserData=NULL);

	// Cover
	void	SetCover(bool bCover);

	tstring	GetFileFormat(tstring strFormat);
protected:
	
	tstring CalcFileMD5(tstring strFilePath);
 
protected:
	int					m_nResModelType;
	Json::Value			m_Root;
	tstring				m_strGuid;
	tstring				m_strAssetsType; 
	
	bool				m_bCover;
};


#endif