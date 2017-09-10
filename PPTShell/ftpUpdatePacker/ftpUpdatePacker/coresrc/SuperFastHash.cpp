#include "stdafx.h"
#include "SuperFastHash.h"

#define get16bits(d) (*((const uint16_t*)(d)))

static const int INCREMENTAL_READ_BLOCK = 65536;
typedef unsigned int uint32_t;
typedef unsigned short uint16_t;

unsigned int SuperFastHash(const char* data,int length)
{
	int cbRemaining = length;
	unsigned int lastHash = length;
	int offset = 0;

	while(cbRemaining >= INCREMENTAL_READ_BLOCK)
	{
		lastHash = SuperFastHashIncremental(data+offset,INCREMENTAL_READ_BLOCK,lastHash);
		cbRemaining -= INCREMENTAL_READ_BLOCK;
		offset += INCREMENTAL_READ_BLOCK;
	}

	if(cbRemaining > 0)
	{
		lastHash = SuperFastHashIncremental(data+offset,cbRemaining,lastHash);
	}

	return lastHash;
}

unsigned int SuperFastHashFile(const char* filename)
{
	FILE* fp = fopen(filename,"rb");
	if(fp == NULL)
	{
		return 0;
	}
	unsigned int hash = SuperFastHashFilePtr(fp);
	fclose(fp);
	return hash;
}

unsigned int SuperFastHashFilePtr(FILE* fp)
{
	fseek(fp,0,SEEK_END);
	int length = ftell(fp);
	fseek(fp,0,SEEK_SET);

	int cbRemaining = length;
	unsigned int lastHash = length;
	char readBlock[INCREMENTAL_READ_BLOCK];

	//printf("line:%d hash:%u\n",__LINE__,lastHash);


	while(cbRemaining >= (int)sizeof(readBlock))
	{
		fread(readBlock,sizeof(readBlock),1,fp);
		lastHash = SuperFastHashIncremental(readBlock,(int)sizeof(readBlock),lastHash);
		cbRemaining -= (int)sizeof(readBlock);

		//printf("line:%d hash:%u\n",__LINE__,lastHash);
	}

	if(cbRemaining > 0)
	{
		fread(readBlock,cbRemaining,1,fp);
		lastHash = SuperFastHashIncremental(readBlock,cbRemaining,lastHash);
	}
	return lastHash;
}

unsigned int SuperFastHashIncremental(const char* data,int len,unsigned int lastHash)
{
	uint32_t hash = (uint32_t)lastHash;
	uint32_t tmp;
	int rem;

	if((len <= 0) || (data == NULL))
	{
		return 0;
	}

	rem = len & 3;
	len >>= 2;

	for(;len > 0;len--)
	{
		hash += get16bits(data);
		tmp = (get16bits(data+2)<<11)^hash;
		hash = (hash << 16)^tmp;
		data += 2*sizeof(uint16_t);
		hash += hash>>11;

		//printf("line:%d hash:%u\n",__LINE__,hash);
	}

	switch(rem)
	{
	case 3:
		hash += get16bits(data);
		hash ^= hash << 16;
		hash ^= data[sizeof(uint16_t)]<<18;
		hash += hash >> 11;
		break;
	case 2:
		hash += get16bits(data);
		hash ^=hash << 11;
		hash += hash >> 17;
		break;
	case 1:
		hash += *data;
		hash ^= hash << 10;
		hash += hash >> 1;
		break;
	}

	//printf("line:%d hash:%u\n",__LINE__,hash);

	hash ^= hash << 3;
	hash += hash >> 5;
	hash ^= hash << 4;
	hash += hash >> 17;
	hash ^= hash << 25;
	hash += hash >> 6;

	return (unsigned int)hash;
}