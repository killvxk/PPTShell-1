#pragma once

//#define DOC_TYPE		_T(".txt;.doc;.docx;.rtf;.ini;.xls;.xlsx;.xml")
#define PPT_TYPE		_T(".ppt;.pptx;.ndpx;")
#define PICTURE_TYPE	_T(".jpg;.png;.jpeg;.gif;.bmp;.icon;")
#define VIDEO_TYPE		_T(".asf;.avi;.mov;.mp4;.3gp;.mpeg;.mpg;.wmv;") 
#define FLASH_TYPE		_T(".swf;")
#define VOLUME_TYPE		_T(".wav;.mp3;.wma;.mid;")
#define QUESTION_TYPE	_T(".xml;")


#define FILE_FILTER_UNDO 40000
#define FILE_FILTER 50000
#define FILE_FILTER_PPT 50001
#define FILE_FILTER_VIDEO 50002
#define FILE_FILTER_PIC 50003
#define FILE_FILTER_FLASH 50004
#define FILE_FILTER_VOLUME 50005
#define FILE_FILTER_BASIC_EXERCISES 50006
#define FILE_FILTER_INTERACTIVE_EXERCISES 50007
#define FILE_FILTER_OTHER 50008



class CFileTypeFilter
{
public:
	CFileTypeFilter(void);
	~CFileTypeFilter(void);

public:
	bool	IsCorrectType(int nType, tstring strName);
	int		GetFileType(tstring strPath);

private:
	map<int,tstring>	m_mapFilter;
};
