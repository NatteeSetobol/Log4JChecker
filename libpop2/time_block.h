#ifndef __time_block_h__
#define __time_block_h__
#define MAX_RECORD_TABLE 256

#include <string.h>
#ifndef COMPILER_MSVC
#include <sys/time.h>
#endif
#include <stdio.h>

#ifndef COMPILER_MSVC
#include <x86intrin.h>
#endif
#include "./required/intrinsic.h"

struct timed_block
{
	int Counter;
	timed_block(int counterInit, char* filename, int lineNumber, char* blockName);
	~timed_block();
};

struct record_info
{
	ui32 id;
	ui64 beginTick;
	ui64 endTick;
	ui64 elaspTick;
	ui32 index;
	char *name;
	struct timeval beginClock;
	struct timeval endClock;
	double elaspTime;
};

struct global_record_info_table
{
	ui32 totalRecords;
	record_info records[MAX_RECORD_TABLE];
};


#define TimedBlock() \
	timed_block timeBlock(0,(char*) __FILE__,__LINE__, (char*) __FUNCTION__);


void ShowRecordTable();


#endif

