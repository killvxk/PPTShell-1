#pragma once
#include "InteractiveType.h"

class CInteractiveTool
{
public:
	CInteractiveTool(void);
	~CInteractiveTool(void);
	void SetName(string name);
	string GetName();
	void SetImagePath(string imagePath);
	string GetImagePath();
	void SetType(InteractiveToolType type);
	InteractiveToolType GetType();
private:
	std::string m_sName;
	std::string m_sImagePath;
	InteractiveToolType m_eType;
};
