//Written by Barry Porter, 2016

#include "dana_lib_defs.h"
#include "nli_util.h"
#include "vmi_util.h"

#ifdef WINDOWS
#include <Windows.h>
#endif

#ifdef LINUX
#include <sys/utsname.h>
#include <unistd.h>
#endif

#include <sys/stat.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <pthread.h>

static CoreAPI *api;

INSTRUCTION_DEF op_execute(FrameData* cframe)
	{
	char *vn = x_getParam_char_array(api, cframe, 0);
	
	if (vn == NULL)
		{
		vn = strdup("");
		}
	
	int k = system(vn);
	unsigned char ok = 0;
	
	if (true)
		{
		//NOTE: most *nix systems only support an 8-bit return value from a child process
		#ifdef LINUX
		if (WIFEXITED(k))
			{
			k = WEXITSTATUS(k);
			}
		#endif
		
		DanaEl* dcon = api -> getParamEl(cframe, 1);
		
		size_t res = k;
		
		api -> setDataFieldInt(dcon, 0, res);
		
	    ok = 1;
    	}
    
    api -> returnRaw(cframe, (unsigned char*) &ok, 1);
    
	free(vn);
	
	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	setInterfaceFunction("execute", op_execute);
	
	return getPublicInterface();
	}

void unload()
	{
	}
