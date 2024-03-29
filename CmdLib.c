//Written by Barry Porter, 2016

#include "dana_lib_defs.h"

#ifdef WINDOWS
#include <Windows.h>
#else
#include <pthread.h>
#include <termios.h>
#include <unistd.h>   
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
			
			unsigned char *cnt = NULL;
			DanaEl* array = api -> makeArray(charArrayGT, length, &cnt);
			memcpy(cnt, fullText, length);
			api -> returnEl(cframe, array);
			
			free(fullText);
			}
			else if (length != 0)
			{
			unsigned char *cnt = NULL;
			DanaEl* array = api -> makeArray(charArrayGT, length, &cnt);
			memcpy(cnt, fullText, length);
			api -> returnEl(cframe, array);
			free(fullText);
			}
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_line_secret(FrameData* cframe)
	{
	char buf[MAX_BUF];
	memset(buf, '\0', MAX_BUF);

	#ifdef LINUX
    static struct termios oldt, newt;

    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ECHO);          

    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
	#endif
	#ifdef WINDOWS
    DWORD orig = 0;
    HANDLE hi = INVALID_HANDLE_VALUE;

    DWORD access = GENERIC_READ | GENERIC_WRITE;
    hi = CreateFileA("CONIN$", access, 0, 0, OPEN_EXISTING, 0, 0);
    if (!GetConsoleMode(hi, &orig))
		{
		return RETURN_OK;
		}
    DWORD mode = orig;
    mode |= ENABLE_PROCESSED_INPUT;
    mode |= ENABLE_LINE_INPUT;
    mode &= ~ENABLE_ECHO_INPUT;
    if (!SetConsoleMode(hi, mode))
		{
		return RETURN_OK;
		}
	#endif
	
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
			
			unsigned char *cnt = NULL;
			DanaEl* array = api -> makeArray(charArrayGT, length, &cnt);
			memcpy(cnt, fullText, length);
			api -> returnEl(cframe, array);
			
			free(fullText);
			}
			else if (length != 0)
			{
			unsigned char *cnt = NULL;
			DanaEl* array = api -> makeArray(charArrayGT, length, &cnt);
			memcpy(cnt, fullText, length);
			api -> returnEl(cframe, array);
			free(fullText);
			}
		}
	
	printf("\n");
	
	#ifdef LINUX
    tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
	#endif
	#ifdef WINDOWS
    SetConsoleMode(hi, orig);
    CloseHandle(hi);
	#endif

	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	
	setInterfaceFunction("getLine", op_get_line);
	setInterfaceFunction("getLineSecret", op_get_line_secret);
	
	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(charArrayGT);
	}
