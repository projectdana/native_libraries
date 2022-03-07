//Written by Barry Porter, 2019

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

#include <zlib.h>

static CoreAPI *api;

static GlobalTypeLink *byteArrayGT = NULL;

#define CHUNK 16384

typedef struct inflateStatus {
		unsigned char status;
		size_t totalLength;
		size_t lastBlockLength;
		z_stream stream;
} InflateStatus;

INSTRUCTION_DEF op_deflate_init(FrameData* cframe)
	{
	int ret;
	int level = Z_DEFAULT_COMPRESSION;
	
	z_stream *stream = malloc(sizeof(z_stream));
	
    stream -> zalloc = Z_NULL;
    stream -> zfree = Z_NULL;
    stream -> opaque = Z_NULL;
	
	//initialise deflate in "raw" mode (-15) with a large history window (i.e., 15)
	ret = deflateInit2(stream, level, Z_DEFLATED, -15, 8, Z_DEFAULT_STRATEGY);
	
	api -> returnRaw(cframe, (unsigned char*) &stream, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_deflate(FrameData* cframe)
	{
	int ret;
	int have;
	unsigned char out[CHUNK];
	
	unsigned char *result = NULL;
	unsigned int result_len = 0;
	
	z_stream *stream;
	memcpy(&stream, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* array = api -> getParamEl(cframe, 1);
	
	if (array == NULL)
		{
		api -> throwException(cframe, "null array to deflate");
		return RETURN_OK;
		}
	
	unsigned char xs = api -> getParamRaw(cframe, 2)[0];
	
	int flush = xs == 0 ? Z_NO_FLUSH : Z_FINISH;
	
	stream -> next_in = api -> getArrayContent(array);
	stream -> avail_in = api -> getArrayLength(array);
	
	do {
		stream -> avail_out = CHUNK;
		stream -> next_out = out;
		ret = deflate(stream, flush);
		have = CHUNK - stream -> avail_out;
		
		result = realloc(result, result_len + have);
		memcpy(result+result_len, out, have);
		result_len += have;
		
		} while (stream -> avail_out == 0);
	
	//return the compressed chunk as a byte array
	unsigned char* cnt = NULL;
	DanaEl* array_r = api -> makeArray(byteArrayGT, result_len, &cnt);
	memcpy(cnt, result, result_len);
	api -> returnEl(cframe, array_r);
	
	free(result);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_deflate_end(FrameData* cframe)
	{
	z_stream *stream;
	memcpy(&stream, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	deflateEnd(stream);
	
	free(stream);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_inflate_init(FrameData* cframe)
	{
	int ret;
	
	InflateStatus *ist = malloc(sizeof(InflateStatus));
	memset(ist, '\0', sizeof(InflateStatus));
	
    ist -> stream.zalloc = Z_NULL;
    ist -> stream.zfree = Z_NULL;
    ist -> stream.opaque = Z_NULL;
    ist -> stream.avail_in = 0;
    ist -> stream.next_in = Z_NULL;
	
	ret = inflateInit2(&ist -> stream, -15);
	
	api -> returnRaw(cframe, (unsigned char*) &ist, sizeof(size_t));
	
	return RETURN_OK;
	}

void zerr(int ret)
{
    fputs("zpipe: ", stderr);
    switch (ret) {
    case Z_ERRNO:
        if (ferror(stdin))
            fputs("error reading stdin\n", stderr);
        if (ferror(stdout))
            fputs("error writing stdout\n", stderr);
        break;
    case Z_STREAM_ERROR:
        fputs("invalid compression level\n", stderr);
        break;
    case Z_DATA_ERROR:
        fputs("invalid or incomplete deflate data\n", stderr);
        break;
    case Z_MEM_ERROR:
        fputs("out of memory\n", stderr);
        break;
    case Z_VERSION_ERROR:
        fputs("zlib version mismatch!\n", stderr);
    }
}

INSTRUCTION_DEF op_inflate(FrameData* cframe)
	{
	int ret;
	int have;
	unsigned char out[CHUNK];
	
	unsigned char *result = NULL;
	unsigned int result_len = 0;
	
	InflateStatus *ist;
	memcpy(&ist, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* array = api -> getParamEl(cframe, 1);
	
	if (array == NULL)
		{
		api -> throwException(cframe, "null array to inflate");
		return RETURN_OK;
		}
	
	size_t alen = api -> getArrayLength(array);
	
	ist -> stream.avail_in = alen;
	ist -> stream.next_in = api -> getArrayContent(array);
	
	do {
		ist -> stream.avail_out = CHUNK;
		ist -> stream.next_out = out;
		ret = inflate(&ist -> stream, Z_NO_FLUSH);
		switch (ret) {
		case Z_NEED_DICT:
			ret = Z_DATA_ERROR;     /* and fall through */
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			(void)inflateEnd(&ist -> stream);
			ist -> status = 2;
			//printf("RETj: %i\n", ret);
			//zerr(ret);
			return RETURN_OK;
		}
		have = CHUNK - ist -> stream.avail_out;
		
		result = realloc(result, result_len + have);
		memcpy(result+result_len, out, have);
		result_len += have;
		
	} while (ist -> stream.avail_out == 0);
	
	if (ist -> stream.avail_in != 0)
		{
		//end-of-stream
		ist -> totalLength += (alen - ist -> stream.avail_in);
		ist -> lastBlockLength = alen - ist -> stream.avail_in;
		ist -> status = 1;
		}
		else
		{
		//keep counting bytes...
		ist -> totalLength += alen;
		}
	
	//return the decompressed chunk as a byte array
	unsigned char* cnt = NULL;
	DanaEl* array_r = api -> makeArray(byteArrayGT, result_len, &cnt);
	memcpy(cnt, result, result_len);
	api -> returnEl(cframe, array_r);
	
	free(result);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_inflate_status(FrameData* cframe)
	{
	InflateStatus *ist;
	memcpy(&ist, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	api -> returnRaw(cframe, &ist -> status, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_inflate_end(FrameData* cframe)
	{
	InflateStatus *ist;
	memcpy(&ist, api -> getParamRaw(cframe, 0), sizeof(size_t));
	size_t len = ist -> totalLength;
	size_t lcLen = ist -> lastBlockLength;
	
	inflateEnd(&ist -> stream);
	
	free(ist);
	
	//return actual length of the last compressed chunk we handled
	api -> returnInt(cframe, lcLen);
	
	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	byteArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("byte[]"));
	
	setInterfaceFunction("deflateInit", op_deflate_init);
	setInterfaceFunction("deflate", op_deflate);
	setInterfaceFunction("deflateEnd", op_deflate_end);
	
	setInterfaceFunction("inflateInit", op_inflate_init);
	setInterfaceFunction("inflate", op_inflate);
	setInterfaceFunction("inflateStatus", op_inflate_status);
	setInterfaceFunction("inflateEnd", op_inflate_end);
	
	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(byteArrayGT);
	}
