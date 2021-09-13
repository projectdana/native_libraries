//Written by Barry Porter, 2016

#include "dana_lib_defs.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static CoreAPI *api;

#ifdef WINDOWS
#include <Windows.h>
#endif

#ifdef LINUX
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#endif

#include "nli_util.h"
#include "vmi_util.h"

INSTRUCTION_DEF op_timer_sleep(VFrame *cframe)
	{
	size_t ms = getParam_int(cframe, 0);
	
	#ifdef WINDOWS
	Sleep(ms);
	#endif
	#ifdef LINUX
	struct timespec ts;
	ts.tv_sec = ms / 1000;
	ts.tv_nsec = (ms - (ts.tv_sec * 1000)) * 1000000;
	nanosleep(&ts, NULL);
	#endif
	
	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	setInterfaceFunction("sleep", op_timer_sleep);
	
	return getPublicInterface();
	}

void unload()
	{
	
	}
