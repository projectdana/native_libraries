//Written by Barry Porter, 2016

#include "dana_lib_defs.h"

#ifdef WINDOWS
#include <Windows.h>
#else
#include <pthread.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "nli_util.h"
#include "vmi_util.h"

static CoreAPI *api;

#define MAX_BUF 32

static GlobalTypeLink *charArrayGT = NULL;

INSTRUCTION_DEF op_get_line(FrameData* cframe)
	{
	char buf[MAX_BUF];
	memset(buf, '\0', MAX_BUF);
	
	char *p = fgets (buf, MAX_BUF, stdin);
	char *fullText = NULL;
	size_t length = 0;
	
	while (p != NULL && strchr(p, '\n') == NULL)
		{
		fullText = realloc(fullText, length + strlen(p));
		memcpy(fullText + length, p, strlen(p));
		length += strlen(p);
		
		p = fgets (buf, MAX_BUF, stdin);
		}
	
	if (p != NULL)
		{
		strchr(p, '\n')[0] = '\0';
		
		if (strlen(p) != 0)
			{
			fullText = realloc(fullText, length + strlen(p));
			memcpy(fullText + length, p, strlen(p));
			length += strlen(p);
			
			DanaEl* array = api -> makeArray(charArrayGT, length);
			memcpy(api -> getArrayContent(array), fullText, length);
			api -> returnEl(cframe, array);
			
			free(fullText);
			}
			else if (length != 0)
			{
			DanaEl* array = api -> makeArray(charArrayGT, length);
			memcpy(api -> getArrayContent(array), fullText, length);
			api -> returnEl(cframe, array);
			free(fullText);
			}
		}
	
	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	
	setInterfaceFunction("getLine", op_get_line);
	
	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(charArrayGT);
	}
