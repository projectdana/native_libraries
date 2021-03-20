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

INSTRUCTION_DEF op_execute(VFrame *cframe)
	{
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 0)) -> content;
	
	char *vn = NULL;
	
	if (array != NULL)
		{
		vn = malloc(array -> length + 1);
		memset(vn, '\0', array -> length + 1);
		memcpy(vn, array -> data, array -> length);
		}
		else
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
		
		size_t *resultValue = (size_t*) ((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content) -> data;
		
		size_t res = k;
		
		copyHostInteger((unsigned char*) resultValue, (unsigned char*) &res, sizeof(size_t));
		
	    ok = 1;
    	}
    
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	copyHostInteger((unsigned char*) result, (unsigned char*) &ok, sizeof(ok));
	
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
