#include "StdAfx.h"
#include <string>
using namespace std;

unsigned char* _stdcall Base64Decode(const unsigned char* str, int length, int* ret_length);
unsigned char* _stdcall Base64Encode(const char* str, int length);
