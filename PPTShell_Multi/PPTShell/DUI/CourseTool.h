#pragma once

class CCourseTool
{
public:
	CCourseTool(void);
	~CCourseTool(void);
	void SetName(string name);
	string GetName();
	void SetImagePath(string imagePath);
	string GetImagePath();
	void SetActionPath(string actionPath);
	string GetActionPath();
private:
	std::string m_sName;
	std::string m_sImagePath;
	std::string m_sActionPath;
};
