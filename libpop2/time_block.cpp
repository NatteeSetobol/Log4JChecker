#include "time_block.h"
extern struct global_record_info_table recordTable = {};

#ifdef COMPILER_MSVC
#define GetTime(beginClock)
#else
#define GetTime(beginClock) gettimeofday(beginClock,0)
#endif

#define RecordInfo(eventName) \
		ui64 recordIndex =   ++recordTable.totalRecords;\
		GetTime(&recordTable.records[recordIndex].beginClock);\
		recordTable.records[recordIndex].beginTick= __rdtsc();\
		recordTable.records[recordIndex].index = recordIndex;\
		recordTable.records[recordIndex].name = (char*) malloc(strlen(eventName)+1);\
		strcpy(recordTable.records[recordIndex].name, eventName);\



timed_block::timed_block(int counterInit, char* filename, int lineNumber, char* blockName)
	{
	//	if (recordTable)
	//	{
	//		recordTable =(struct global_record_info_table*) malloc(sizeof(struct global_record_info_table));
	//	}
		RecordInfo(blockName);
		Counter = recordTable.totalRecords;

	}

timed_block::~timed_block()
	{
		long seconds = 0;
		long mseconds = 0;

		recordTable.records[Counter].endTick = __rdtsc();
		GetTime(&recordTable.records[Counter].endClock);

		seconds =  recordTable.records[Counter].endClock.tv_sec - recordTable.records[Counter].beginClock.tv_sec;
		mseconds =  recordTable.records[Counter].endClock.tv_usec - recordTable.records[Counter].beginClock.tv_usec;

		recordTable.records[Counter].elaspTime = seconds + mseconds*1e-6;

		recordTable.records[Counter].elaspTick = recordTable.records[Counter].endTick - recordTable.records[Counter].beginTick;
		//RecordInfo((char*)"testing end", __COUNTER__);
	}



void ShowRecordTable()
{
	for (int recordTableIndex = 0; recordTableIndex < MAX_RECORD_TABLE;recordTableIndex++)
	{
		record_info currentTable = recordTable.records[recordTableIndex];
		if (currentTable.name)
		{
			printf("%s:%f (%u ticks)\n", recordTable.records[recordTableIndex].name,
							   recordTable.records[recordTableIndex].elaspTick,
							   recordTable.records[recordTableIndex].elaspTime
				);
		}
	}


}
