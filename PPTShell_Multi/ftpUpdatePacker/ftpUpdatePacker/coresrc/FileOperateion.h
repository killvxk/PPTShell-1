#pragma once


class CFileOperateion
{
public:
	CFileOperateion();
	~CFileOperateion();

public:
	static	bool	IsDirectoryExist(LPCTSTR lpPath);

	static	bool	CreateDirectory(LPCTSTR lpPath);

	static	bool	CreatePath(LPCTSTR lpPath);

	static	bool	DeleteDirectory(LPCTSTR lpPath);

public:
	static	bool	IsFileExist(LPCTSTR lpPath);
};