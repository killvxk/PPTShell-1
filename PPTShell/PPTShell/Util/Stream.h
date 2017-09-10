//===========================================================================
// FileName:				Stream.h
//	
// Desc:					 
//============================================================================
#ifndef _STREAM_H_
#define _STREAM_H_



class CStream
{
public:
	CStream(char* pBuffer = NULL, int nBufferSize = 0);	// memory supplied by caller
	CStream(int nAllocSize);							// memory allocated by this class
	~CStream();

	void operator= (const CStream& s); 
	void operator+= (const CStream& s);

	void	AllocMemory(int nAllocSize);
	void	ResetCursor();

	// read
	BYTE	ReadByte();
	WORD	ReadWORD();
	DWORD	ReadDWORD();
	int		ReadInt();
	BOOL	ReadBOOL();
	float	ReadFloat();
	tstring	ReadString();
	string  ReadCString();
	POINTF	ReadPointF();	
	void    Read(char* pOutBuffer, int nBufferLen);

	// write
	BOOL	WriteByte(BYTE val);
	BOOL	WriteWORD(WORD val);
	BOOL	WriteDWORD(DWORD val);
	BOOL	WriteInt(int val);
	BOOL	WriteBOOL(BOOL val);
	BOOL	WriteFloat(float val);
	BOOL	WriteString(char* str);
	BOOL	WriteString(tstring str);
	BOOL	Write(char* pInBuffer, int nBufferLen);
	BOOL	Write(int nOffset, char* pInBuffer, int nBufferLen);		// won't change cursor and data size

	//
	char*	GetBuffer();
	int		GetBufferSize();
	int		GetDataSize();
	int		GetCursor();


protected:
	char*	m_pBuffer;
	int		m_nBufferSize;
	int		m_nDataSize;
	int		m_nCursor;
	BOOL	m_bBufferAllocated;		// allocate by itself
};

#endif