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

typedef struct __sem {
	#ifdef WINDOWS
	#include <Windows.h>
	HANDLE sem;
	#endif
	#ifdef OSX
	dispatch_semaphore_t sem;
	#else
	#ifdef LINUX
	sem_t sem;
	#endif
	#endif
	} Sem;

INSTRUCTION_DEF op_new_instance(FrameData* cframe)
	{
	Sem* lock = malloc(sizeof(Sem));
	memset(lock, '\0', sizeof(Sem));

	size_t initialValue = 0;
	
	#ifdef WINDOWS
	lock -> sem = CreateSemaphore(NULL, initialValue, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_t *sem;
	sem = &lock -> sem;
    *sem = dispatch_semaphore_create(initialValue);
	#else
	#ifdef LINUX
	sem_init(&lock -> sem, initialValue, 0);
	#endif
	#endif
	
	api -> returnRaw(cframe, (unsigned char*) &lock, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_wait(FrameData* cframe)
	{
	size_t ptrx = 0;
	Sem* lock = NULL;
	memcpy(&ptrx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	lock = (Sem*) ptrx;
	
	#ifdef WINDOWS
	return WaitForSingleObject(lock -> sem, INFINITE);
	#endif
	#ifdef OSX
	return dispatch_semaphore_wait(lock -> sem, DISPATCH_TIME_FOREVER);
	#else
	#ifdef LINUX
	return sem_wait(&lock -> sem);
	#endif
	#endif
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_signal(FrameData* cframe)
	{
	size_t ptrx = 0;
	Sem* lock = NULL;
	memcpy(&ptrx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	lock = (Sem*) ptrx;
	
	#ifdef WINDOWS
	ReleaseSemaphore(lock -> sem, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_signal(lock -> sem);
	#else
	#ifdef LINUX
	sem_post(&lock -> sem);
	#endif
	#endif
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_destroy(FrameData* cframe)
	{
	size_t ptrx = 0;
	Sem* lock = NULL;
	memcpy(&ptrx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	lock = (Sem*) ptrx;
	
	#ifdef WINDOWS
	CloseHandle(lock -> sem);
	#endif
    #ifdef OSX
    dispatch_release(lock -> sem);
    #else
	#ifdef LINUX
	sem_close(&lock -> sem);
	#endif
    #endif
	
	free(lock);
	
	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	setInterfaceFunction("newInstance", op_new_instance);
	setInterfaceFunction("wait", op_wait);
	setInterfaceFunction("signal", op_signal);
	setInterfaceFunction("destroy", op_destroy);
	
	return getPublicInterface();
	}

void unload()
	{
	}
