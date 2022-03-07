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
#ifndef OSX
#include <sys/sysinfo.h>
#endif
#ifdef OSX
#include <sys/sysctl.h>
#endif
#include <errno.h>
#endif

#include <sys/stat.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <pthread.h>

static CoreAPI *api;

static GlobalTypeLink *charArrayGT = NULL;

#define MAX_VAR_NAME 2048

INSTRUCTION_DEF op_get_platform_name(FrameData* cframe)
	{
	char *name = "Unknown";

	#ifdef WINDOWS
	name = "Windows";
	#endif

	#ifdef LINUX
	struct utsname nm;
	if (uname(&nm) == 0)
		{
		name = nm.sysname;
		}
	#endif
	
	unsigned char* cnt = NULL;
	DanaEl* array = api -> makeArray(charArrayGT, strlen(name), &cnt);
	memcpy(cnt, name, strlen(name));
	api -> returnEl(cframe, array);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_platform_version(FrameData* cframe)
	{
	char sres[MAX_VAR_NAME];
	memset(sres, '\0', sizeof(sres));

	#ifdef WINDOWS
	char ires[MAX_VAR_NAME];
	memset(ires, '\0', sizeof(ires));

	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(osvi);

	GetVersionEx(&osvi);

	size_t majorVer = osvi.dwMajorVersion;
	size_t minorVer = osvi.dwMinorVersion;
	size_t buildNum = osvi.dwBuildNumber;

	itoa(majorVer, ires, 10);
	strcat(sres, ires);

	memset(ires, '\0', sizeof(ires));
	strcat(sres, ".");

	itoa(minorVer, ires, 10);
	strcat(sres, ires);

	memset(ires, '\0', sizeof(ires));
	strcat(sres, " [");

	itoa(buildNum, ires, 10);
	strcat(sres, ires);

	strcat(sres, "]");
	#endif

	#ifdef LINUX
	struct utsname name;
	if (uname(&name) == 0)
		{
		snprintf(sres, MAX_VAR_NAME, "%s [%s]", name.release, name.version);
		}
	#endif
	
	unsigned char* cnt = NULL;
	DanaEl* array = api -> makeArray(charArrayGT, strlen(sres), &cnt);
	memcpy(cnt, sres, strlen(sres));
	api -> returnEl(cframe, array);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_chip_name(FrameData* cframe)
	{
	unsigned char* cnt = NULL;
	DanaEl* array = api -> makeArray(charArrayGT, strlen(CHIP_NAME), &cnt);
	memcpy(cnt, CHIP_NAME, strlen(CHIP_NAME));
	api -> returnEl(cframe, array);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_host_name(FrameData* cframe)
	{
	char sres[MAX_VAR_NAME];
	memset(sres, 0, sizeof(sres));

	#ifdef WINDOWS
	DWORD len = MAX_VAR_NAME;
	GetComputerNameEx(ComputerNameDnsFullyQualified, sres, &len);
	#endif

	#ifdef LINUX
	gethostname(sres, MAX_VAR_NAME);
	#endif

	unsigned char* cnt = NULL;
	DanaEl* array = api -> makeArray(charArrayGT, strlen(sres), &cnt);
	memcpy(cnt, sres, strlen(sres));
	api -> returnEl(cframe, array);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_variable(FrameData* cframe)
	{
	char *vn = x_getParam_char_array(api, cframe, 0);

	char *val = getenv(vn);

	if (val != NULL)
		{
		unsigned char* cnt = NULL;
		DanaEl* array = api -> makeArray(charArrayGT, strlen(val), &cnt);
		memcpy(cnt, val, strlen(val));
		api -> returnEl(cframe, array);
    	}

	free(vn);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_system_font(FrameData* cframe)
	{
	unsigned char monospaced = api -> getParamRaw(cframe, 0)[0];

	char *home = api -> getDanaHome();

	if (home != NULL)
		{
		char *fontPath = NULL;

		if (monospaced)
			{
			fontPath = malloc(strlen(home) + strlen("/resources-ext/fonts/LiberationMono.ttf") + 1);
			memset(fontPath, '\0', strlen(home) + strlen("/resources-ext/fonts/LiberationMono.ttf") + 1);
			strcpy(fontPath, home);
			strcat(fontPath, "/resources-ext/fonts/LiberationMono.ttf");
			}
			else
			{
			fontPath = malloc(strlen(home) + strlen("/resources-ext/fonts/SourceSansPro.ttf") + 1);
			memset(fontPath, '\0', strlen(home) + strlen("/resources-ext/fonts/SourceSansPro.ttf") + 1);
			strcpy(fontPath, home);
			strcat(fontPath, "/resources-ext/fonts/SourceSansPro.ttf");
			}

		struct stat st;
		if (stat(fontPath, &st) == 0)
			{
			unsigned char* cnt = NULL;
			DanaEl* array = api -> makeArray(charArrayGT, strlen(fontPath), &cnt);
			memcpy(cnt, fontPath, strlen(fontPath));
			api -> returnEl(cframe, array);
			}

		free(fontPath);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_core_count(FrameData* cframe)
	{
	size_t res = 0;

	#ifdef WINDOWS
	SYSTEM_INFO sInfo;
	GetSystemInfo(&sInfo);

	res = sInfo.dwNumberOfProcessors;
	#endif

	#ifdef LINUX
	int q = sysconf(_SC_NPROCESSORS_ONLN);

	if (q == -1)
		{
		api -> throwException(cframe, strerror(errno));
		return RETURN_OK;
		}

	res = q;
	#endif
	
	api -> returnInt(cframe, res);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_memory_size(FrameData* cframe)
	{
	size_t res = 0;

	#ifdef WINDOWS
	unsigned long long q;
	if (!GetPhysicallyInstalledSystemMemory(&q))
		{
		//api -> throwException(cframe, GetLastError()); //TODO: use FormatMessage()
		api -> throwException(cframe, "operation failed");
		return RETURN_OK;
		}

	res = q / 1024;
	#endif

	#ifdef LINUX
	#ifndef OSX
	struct sysinfo sInfo;

	if (sysinfo(&sInfo) == -1)
		{
		api -> throwException(cframe, strerror(errno));
		return RETURN_OK;
		}

	res = (sInfo.totalram / 1024) / 1024;
	#endif
	#ifdef OSX
	int mib[] = {CTL_HW, HW_MEMSIZE};
	int64_t value = 0;
	size_t len = sizeof(value);
	if (sysctl(mib, 2, &value, &len, NULL, 0) == -1)
		{
		}

	res = (value / 1024) / 1024;
	#endif
	#endif

	api -> returnInt(cframe, res);

	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));

	setInterfaceFunction("getPlatformName", op_get_platform_name);
	setInterfaceFunction("getPlatformVersion", op_get_platform_version);
	setInterfaceFunction("getChipName", op_get_chip_name);
	setInterfaceFunction("getHostName", op_get_host_name);
	setInterfaceFunction("getVar", op_get_variable);
	setInterfaceFunction("getSystemFont", op_get_system_font);

	setInterfaceFunction("getCoreCount", op_get_core_count);
	setInterfaceFunction("getMemorySize", op_get_memory_size);

	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(charArrayGT);
	}
