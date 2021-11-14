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

void dtoh(unsigned char *x, unsigned int len)
	{
	#ifdef MACHINE_ENDIAN_LITTLE
	unsigned int i = 0;
	unsigned int top = len - 1;
	unsigned int mid = len / 2;
	for (i = 0; i < mid; i++)
		{
		unsigned char b = x[top-i];
		x[top-i] = x[i];
		x[i] = b;
		}
	#endif
	}

void htod(unsigned char *x, unsigned int len)
	{
	#ifdef MACHINE_ENDIAN_LITTLE
	unsigned int i = 0;
	unsigned int top = len - 1;
	unsigned int mid = len / 2;
	for (i = 0; i < mid; i++)
		{
		unsigned char b = x[top-i];
		x[top-i] = x[i];
		x[i] = b;
		}
	#endif
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

void copyHostInteger(unsigned char *dst, unsigned char *src, size_t srcsz)
	{
	be_memcpy(dst + sizeof(size_t), src + srcsz, srcsz > sizeof(size_t) ? sizeof(size_t) : srcsz);
	
	#ifdef MACHINE_ENDIAN_LITTLE
	bos(dst, sizeof(size_t));
	#endif
	}

void copyToDanaInteger(unsigned char *dst, unsigned char *src, size_t srcsz)
	{
	size_t fs = srcsz > sizeof(size_t) ? sizeof(size_t) : srcsz;
	be_memcpy(dst + sizeof(size_t), src + srcsz, fs);
	
	#ifdef MACHINE_ENDIAN_LITTLE
	bos(dst + (sizeof(size_t) - fs), fs);
	#endif
	}

unsigned char* getVariableContent(VFrame *t, unsigned int index)
	{
	return &t -> localsData[((DanaType*) t -> localsDef) -> fields[index+1].offset];
	}

size_t getParam_int(VFrame *f, unsigned int pIndex)
	{
	size_t result = 0;
	copyHostInteger((unsigned char*) &result, getVariableContent(f, pIndex), sizeof(size_t));
	return result;
	}

char* getParam_char_array(VFrame *f, int pIndex) {
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(f, pIndex)) -> content;
	char *result = NULL;
	if (array != NULL) {
		result = malloc(array -> length + 1);
		memset(result, '\0', array -> length + 1);
		memcpy(result, array -> data, array -> length);
	} else { result = strdup(""); }
	return result;
}

void return_int(VFrame *f, size_t v)
	{
	size_t *result = (size_t*) &f -> localsData[((DanaType*) f -> localsDef) -> fields[0].offset];
	copyHostInteger((unsigned char*) result, (unsigned char*) &v, sizeof(size_t));
	}

void return_bool(VFrame *f, bool b)
	{
	size_t *result = (size_t*) &f -> localsData[((DanaType*) f -> localsDef) -> fields[0].offset];
	copyHostInteger((unsigned char*) result, (unsigned char*) &b, sizeof(unsigned char));
	}

void return_byte_array(VFrame *f, CoreAPI *api, unsigned char *str, size_t len)
	{
	GlobalTypeLink *typeLink_char = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	
	LiveArray *array = malloc(sizeof(LiveArray)+len);
	
	if (array == NULL)
		{
		api -> throwException(f, "out of memory to allocate return array");
		return;
		}
	
	memset(array, '\0', sizeof(LiveArray));
	array -> data = ((unsigned char*) array) + sizeof(LiveArray);
	memcpy(array -> data, str, len);
	array -> length = len;
	array -> gtLink = typeLink_char;
	array -> refi.type = array -> gtLink -> typeLink;
	array -> refi.ocm = f -> blocking -> instance;
	array -> refi.refCount ++;
	VVarLivePTR *ptrh = (VVarLivePTR*) &f -> localsData[((DanaType*) f -> localsDef) -> fields[0].offset];
	ptrh -> content = (unsigned char*) array;
	}

void return_char_array(VFrame *f, CoreAPI *api, char *str)
	{
	GlobalTypeLink *typeLink_char = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	
	size_t asz = strlen(str);
	LiveArray *array = malloc(sizeof(LiveArray)+asz);
	
	if (array == NULL)
		{
		api -> throwException(f, "out of memory to allocate return array");
		return;
		}
	
	memset(array, '\0', sizeof(LiveArray));
	array -> data = ((unsigned char*) array) + sizeof(LiveArray);
	memcpy(array -> data, str, asz);
	array -> length = strlen(str);
	array -> gtLink = typeLink_char;
	array -> refi.type = array -> gtLink -> typeLink;
	array -> refi.ocm = f -> blocking -> instance;
	array -> refi.refCount ++;
	VVarLivePTR *ptrh = (VVarLivePTR*) &f -> localsData[((DanaType*) f -> localsDef) -> fields[0].offset];
	ptrh -> content = (unsigned char*) array;
	}

LiveArray* make_byte_array(VFrame *f, CoreAPI *api, size_t len)
	{
	if (len > INT_MAX || (((INT_MAX - len) - 1) < sizeof(LiveArray))) return NULL;
	
	GlobalTypeLink *typeLink_char = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	
	LiveArray *array = malloc(sizeof(LiveArray)+len);
	
	if (array == NULL) return NULL;
	
	memset(array, '\0', sizeof(LiveArray)+len);
	
	array -> data = ((unsigned char*) array) + sizeof(LiveArray);
	array -> length = len;
	
	array -> gtLink = typeLink_char;
	array -> refi.ocm = f -> blocking -> instance;
	
	array -> refi.type = array -> gtLink -> typeLink;
	
	return array;
	}

LiveArray* make_byte_array_wt(VFrame *f, CoreAPI *api, GlobalTypeLink *typeLink_char, size_t len)
	{
	if (len > INT_MAX || (((INT_MAX - len) - 1) < sizeof(LiveArray))) return NULL;
	LiveArray *array = malloc(sizeof(LiveArray)+len);
	if (array == NULL) return NULL;
	memset(array, '\0', sizeof(LiveArray)+len);
	
	array -> data = ((unsigned char*) array) + sizeof(LiveArray);
	array -> length = len;
	
	array -> gtLink = typeLink_char;
	api -> incrementGTRefCount(array -> gtLink);
	array -> refi.ocm = f -> blocking -> instance;
	
	array -> refi.type = array -> gtLink -> typeLink;
	
	return array;
	}

void free_array(CoreAPI *api, LiveArray *array)
	{
	api -> decrementGTRefCount(array -> gtLink);
	free(array);
	}

void return_array(VFrame *f, LiveArray *array)
	{
	array -> refi.refCount ++;
	
	VVarLivePTR *ptrh = (VVarLivePTR*) &f -> localsData[((DanaType*) f -> localsDef) -> fields[0].offset];
	ptrh -> content = (unsigned char*) array;
	}
