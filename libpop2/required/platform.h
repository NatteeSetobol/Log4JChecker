#ifndef __PLATFORM_H__
#define __PLATFORM_H__ 

#ifdef COMPILER_MSVC
#include <intrin.h>



#define CompletePreviousReadsBeforeFutureReads _ReadBarrier()
#define CompletePreviousWritesBeforeFutureWrites _WriteBarrier()
/*
inline u64 AtmoicExchangeU64(u64 volatile *value, u64 newValue)
{
	u64 result = _InterlockedExchange64((__int64 volatile *)value, newValues);

	return result;
}

inline unsigned int AtmoicExchangeU32(unsigned int volatile *value, unsigned int  newValue)
{
	unsigned int result = _InterlockedExchange((__int32 volatile *)value, newValues);

	return result;
}
*/


/*NOTES(): Implemented only 32bits for now, maybe I will implement the 64bits and the unsigned int versions later*/
inline ui64 AtomicExchange32(ui64 volatile *value, ui64  newValue)
{
	ui64 result = _InterlockedExchange((ui64 volatile *)value, newValue);

	return result;
}

#else
#include <x86intrin.h>

#define CompletePreviousReadsBeforeFutureReads asm volatile("" ::: "memory")
#define CompletePreviousWritesBeforeFutureWrites asm volatile("" ::: "memory")


/*NOTES(): Implemented only 32bits for now, maybe I will implement the 64bits and the unsigned int versions later*/
inline int AtomicExchange32(int volatile *value, int  newValue)
{
	int result = __sync_lock_test_and_set(value, newValue);
	return result;
}

#include <sys/time.h>
#endif

#define Kbytes(Value) ((Value)*1024LL)
#define Mbytes(Value) (Kbytes(Value)*1024LL)
#define Gbytes(Value) (Mbytes(Value)*1024LL)
#define Tbytes(Value) (Gbytes(Value)*1024LL)




#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

void* PlatformAllocate(int size);

void ClearMemory(void* ptr, int size);

char* ReadFile(char* filename);

#endif
