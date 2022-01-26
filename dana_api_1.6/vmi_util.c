#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "vmi_util.h"
#include "nli_util.h"
#include "dana_lib_api.h"

unsigned char getLibAPIVersion()
	{
	return LIB_API_VERSION;
	}

void bos(unsigned char *x, unsigned int len)
	{
	unsigned int i = 0;
	unsigned int top = len - 1;
	unsigned int mid = len / 2;
	for (i = 0; i < mid; i++)
		{
		unsigned char b = x[top-i];
		x[top-i] = x[i];
		x[i] = b;
		}
	}

void be_memcpy(unsigned char *to, unsigned char *from, unsigned int len)
	{
	to --;
	from --;
	for (; len > 0; len --)
		{
		*to = *from;
		to --;
		from --;
		}
	}

void copyToDanaInteger(unsigned char *dst, unsigned char *src, size_t srcsz)
	{
	size_t fs = srcsz > sizeof(size_t) ? sizeof(size_t) : srcsz;
	be_memcpy(dst + sizeof(size_t), src + srcsz, fs);
	
	#ifdef MACHINE_ENDIAN_LITTLE
	bos(dst + (sizeof(size_t) - fs), fs);
	#endif
	}

char* x_getParam_char_array(CoreAPI* api, FrameData* f, int pIndex) {
	DanaEl *array = api -> getParamEl(f, pIndex);
	size_t len = api -> getArrayLength(array);
	unsigned char* content = api -> getArrayContent(array);
	
	char *result = NULL;
	if (array != NULL) {
		result = malloc(len + 1);
		memset(result, '\0', len + 1);
		memcpy(result, content, len);
	} else { result = strdup(""); }
	return result;
}

