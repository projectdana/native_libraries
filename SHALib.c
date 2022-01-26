//Written by Barry Porter, 2020

/*
With openssl source code:

 -- Linux --

./config
make

We can then link against static libraries.

 -- Windows --
 
handy things: https://sourceforge.net/p/mingw-w64/wiki2/BuildingOpenSSL/
 - in MSYS2, I use /Configure mingw64 then make

 -- OS X --


*/

#include "dana_lib_defs.h"
#include "nli_util.h"
#include "vmi_util.h"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/bio.h>

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

static GlobalTypeLink *charArrayGT = NULL;

INSTRUCTION_DEF op_hash_data_sha1(FrameData* cframe)
	{
	// https://www.openssl.org/docs/man1.0.2/man3/EVP_DigestInit.html
	// - https://www.openssl.org/docs/man1.1.0/man3/EVP_get_digestbyname.html
	
	EVP_MD_CTX *mdctx = NULL;
	const EVP_MD *md = NULL;
	
	DanaEl* input = api -> getParamEl(cframe, 0);
	
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len;
	
	md = EVP_sha1();
	
	mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, api -> getArrayContent(input), api -> getArrayLength(input));
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_destroy(mdctx);
	
	/*
	printf("Digest is: ");
	int i = 0;
	for (i = 0; i < md_len; i++)
		printf("%02x", md_value[i]);
	printf("\n");
	*/
	
	DanaEl* array = api -> makeArray(charArrayGT, md_len);
	memcpy(api -> getArrayContent(array), md_value, md_len);
	
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_hash_data_sha2(FrameData *cframe)
	{
	// https://www.openssl.org/docs/man1.0.2/man3/EVP_DigestInit.html
	// - https://www.openssl.org/docs/man1.1.0/man3/EVP_get_digestbyname.html
	
	EVP_MD_CTX *mdctx = NULL;
	const EVP_MD *md = NULL;
	
	DanaEl* input = api -> getParamEl(cframe, 0);
	unsigned char type = api -> getParamRaw(cframe, 1)[0];
	
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len;
	
	if (type == 1)
		md = EVP_sha224();
		else if (type == 2)
		md = EVP_sha256();
		else if (type == 3)
		md = EVP_sha384();
		else if (type == 4)
		md = EVP_sha512();
		else
		{
		api -> throwException(cframe, "unknown hash type");
		return RETURN_OK;
		}
	
	mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, api -> getArrayContent(input), api -> getArrayLength(input));
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_destroy(mdctx);
	
	/*
	printf("Digest is: ");
	int i = 0;
	for (i = 0; i < md_len; i++)
		printf("%02x", md_value[i]);
	printf("\n");
	*/
	
	DanaEl* array = api -> makeArray(charArrayGT, md_len);
	memcpy(api -> getArrayContent(array), md_value, md_len);
	
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_hash_data_sha3(FrameData *cframe)
	{
	// https://www.openssl.org/docs/man1.0.2/man3/EVP_DigestInit.html
	// - https://www.openssl.org/docs/man1.1.0/man3/EVP_get_digestbyname.html
	
	EVP_MD_CTX *mdctx = NULL;
	const EVP_MD *md = NULL;
	
	DanaEl* input = api -> getParamEl(cframe, 0);
	unsigned char type = api -> getParamRaw(cframe, 1)[0];
	
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len;
	
	if (type == 1)
		md = EVP_sha3_224();
		else if (type == 2)
		md = EVP_sha3_256();
		else if (type == 3)
		md = EVP_sha3_384();
		else if (type == 4)
		md = EVP_sha3_512();
		else
		{
		api -> throwException(cframe, "unknown hash type");
		return RETURN_OK;
		}
	
	mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, api -> getArrayContent(input), api -> getArrayLength(input));
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_destroy(mdctx);
	
	/*
	printf("Digest is: ");
	int i = 0;
	for (i = 0; i < md_len; i++)
		printf("%02x", md_value[i]);
	printf("\n");
	*/
	
	DanaEl* array = api -> makeArray(charArrayGT, md_len);
	memcpy(api -> getArrayContent(array), md_value, md_len);
	
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

void init_openssl()
	{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
	OpenSSL_add_all_digests();
	}

void cleanup_openssl()
	{
    EVP_cleanup();
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	init_openssl();
	
	// grab global type mappings for anything that we generate here
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	
	setInterfaceFunction("sha1_hashData", op_hash_data_sha1);
	setInterfaceFunction("sha2_hashData", op_hash_data_sha2);
	setInterfaceFunction("sha3_hashData", op_hash_data_sha3);
	
	return getPublicInterface();
	}

void unload()
	{
	cleanup_openssl();
	
	api -> decrementGTRefCount(charArrayGT);
	}
