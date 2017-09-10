#include "StdAfx.h"

CString MD5EncryptString(CString csInput);
CString MD5EncryptFile(CString FileNameInPut);
unsigned char* _stdcall Base64Decode(const unsigned char* str, int length, int* ret_length);
unsigned char* _stdcall Base64Encode(const char* str, int length);
