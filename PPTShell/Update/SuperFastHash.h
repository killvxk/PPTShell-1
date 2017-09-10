#ifndef SUPER_FAST_HASH_H
#define SUPER_FAST_HASH_H

#include <stdio.h>

unsigned int SuperFastHashFile(const char* filename);
unsigned int SuperFastHashFilePtr(FILE* fp);
unsigned int SuperFastHashIncremental(const char* data,int len,unsigned int lastHash);
unsigned int SuperFastHash(const char* data,int length);

#endif