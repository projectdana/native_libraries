//Written by Barry Porter, 2016

#include "dana_lib_defs.h"

//http://msdn.microsoft.com/en-us/library/ms724421%28v=vs.85%29.aspx
//http://msdn.microsoft.com/en-us/library/ms724318(v=vs.85).aspx

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WINDOWS
#include <Windows.h>
#endif

#ifdef LINUX
#include <time.h>
#include <math.h>
#endif

#include <time.h>
#include <sys/time.h>
#include <sys/timeb.h>

#include <pthread.h>

#include "nli_util.h"
#include "vmi_util.h"

static CoreAPI *api;

typedef struct Instance {
	#ifdef LINUX
	size_t refSeconds;
	size_t refNanoseconds;
	#endif
	#ifdef WINDOWS
	LARGE_INTEGER startingTime;
	LARGE_INTEGER frequency;
	#endif
} Instance;

static GlobalTypeLink *timeGT = NULL;

INSTRUCTION_DEF op_init(FrameData *cframe)
	{
	Instance* handle = (Instance*) malloc(sizeof(Instance));
	memset(handle, 0, sizeof(Instance));

	#ifdef WINDOWS
	QueryPerformanceFrequency(&handle -> frequency); 
	QueryPerformanceCounter(&handle -> startingTime);
	#endif

	#ifdef LINUX
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	handle -> refSeconds = t.tv_sec; 
	handle -> refNanoseconds = t.tv_nsec; 
	#endif

	api -> returnRaw(cframe, (unsigned char*) &handle, sizeof(void*));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_time(FrameData *cframe)
	{
	Instance* handle = NULL;
	memcpy(&handle, api -> getParamRaw(cframe, 0), sizeof(void*));

	size_t s;
	size_t ns;
	
	#ifdef WINDOWS
	LARGE_INTEGER endingTime;
	LARGE_INTEGER elapsedMicroseconds;
	QueryPerformanceCounter(&endingTime);
	elapsedMicroseconds.QuadPart = endingTime.QuadPart - handle -> startingTime.QuadPart;

	elapsedMicroseconds.QuadPart *= 1000000;
	elapsedMicroseconds.QuadPart /= handle -> frequency.QuadPart;

	size_t us = elapsedMicroseconds.QuadPart;

	s = us / 1000000;

	us -= s * 1000000;

	ns = us * 1000;
	#endif

	#ifdef LINUX
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);
	s = t.tv_sec - handle -> refSeconds;
	if (t.tv_sec == handle -> refSeconds)
		{
		ns  = t.tv_nsec - handle -> refNanoseconds;
		}
		else
		{
		//here we need to calculate the actual difference, potentially needing to reduce the "second" value by 1
		size_t NS_PER_SECOND = 1000000000;
		size_t nsDiff = NS_PER_SECOND - handle -> refNanoseconds;
		nsDiff += t.tv_nsec;
		
		s --;

		if (nsDiff > NS_PER_SECOND)
			{
			s ++;
			nsDiff -= NS_PER_SECOND;
			}
		
		ns = nsDiff;
		}
	#endif

	DanaEl* el = api -> makeData(timeGT);
	api -> setDataFieldInt(el, 0, s);
	api -> setDataFieldInt(el, 1, ns);

	api -> returnEl(cframe, el);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_destroy(FrameData *cframe)
	{
	Instance* handle = NULL;
	memcpy(&handle, api -> getParamRaw(cframe, 0), sizeof(void*));

	free(handle);

	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;

	timeGT = api -> resolveGlobalTypeMapping(getTypeDefinition("TimeHD"));
	
	setInterfaceFunction("init", op_init);
	setInterfaceFunction("getTime", op_get_time);
	setInterfaceFunction("destroy", op_destroy);
	
	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(timeGT);
	}
