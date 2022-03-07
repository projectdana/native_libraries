//Written by Barry Porter, 2022

#ifdef WINDOWS
#define _WIN32_WINNT 0x0600
#endif

#include "dana_lib_defs.h"

#ifdef WINDOWS
#include <Windows.h>
#include <synchapi.h>
#else
#include <pthread.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "nli_util.h"
#include "vmi_util.h"

static CoreAPI *api;

typedef struct __rel {
	#ifdef WINDOWS
	SRWLOCK lock;
	#endif
	#ifdef LINUX
	pthread_rwlock_t lock;
	#endif
	} RWLock;

INSTRUCTION_DEF op_new_lock(FrameData* cframe)
	{
	RWLock* lock = malloc(sizeof(RWLock));
	memset(lock, '\0', sizeof(RWLock));
	
	#ifdef WINDOWS
	InitializeSRWLock(&lock -> lock);
	#endif
	#ifdef LINUX
	pthread_rwlock_init(&lock -> lock, NULL);
	#endif
	
	api -> returnRaw(cframe, (unsigned char*) &lock, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_read_lock(FrameData* cframe)
	{
	size_t ptrx = 0;
	RWLock* lock = NULL;
	memcpy(&ptrx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	lock = (RWLock*) ptrx;
	
	#ifdef WINDOWS
	AcquireSRWLockShared(&lock -> lock);
	#endif
	#ifdef LINUX
	pthread_rwlock_rdlock(&lock -> lock);
	#endif
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_read_unlock(FrameData* cframe)
	{
	size_t ptrx = 0;
	RWLock* lock = NULL;
	memcpy(&ptrx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	lock = (RWLock*) ptrx;
	
	#ifdef WINDOWS
	ReleaseSRWLockShared(&lock -> lock);
	#endif
	#ifdef LINUX
	pthread_rwlock_unlock(&lock -> lock);
	#endif
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_write_lock(FrameData* cframe)
	{
	size_t ptrx = 0;
	RWLock* lock = NULL;
	memcpy(&ptrx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	lock = (RWLock*) ptrx;
	
	#ifdef WINDOWS
	AcquireSRWLockExclusive(&lock -> lock);
	#endif
	#ifdef LINUX
	pthread_rwlock_wrlock(&lock -> lock);
	#endif
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_write_unlock(FrameData* cframe)
	{
	size_t ptrx = 0;
	RWLock* lock = NULL;
	memcpy(&ptrx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	lock = (RWLock*) ptrx;
	
	#ifdef WINDOWS
	ReleaseSRWLockExclusive(&lock -> lock);
	#endif
	#ifdef LINUX
	pthread_rwlock_unlock(&lock -> lock);
	#endif
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_destroy_lock(FrameData* cframe)
	{
	size_t ptrx = 0;
	RWLock* lock = NULL;
	memcpy(&ptrx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	lock = (RWLock*) ptrx;
	
	#ifdef LINUX
	pthread_rwlock_destroy(&lock -> lock);
	#endif
	
	free(lock);
	
	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	setInterfaceFunction("newLock", op_new_lock);
	setInterfaceFunction("readLock", op_read_lock);
	setInterfaceFunction("readUnlock", op_read_unlock);
	setInterfaceFunction("writeLock", op_write_lock);
	setInterfaceFunction("writeUnlock", op_write_unlock);
	setInterfaceFunction("destroyLock", op_destroy_lock);
	
	return getPublicInterface();
	}

void unload()
	{
	}
