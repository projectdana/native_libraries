//Written by Barry Porter, 2021

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

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/engine.h>

#include <openssl/encoder.h>
#include <openssl/decoder.h>

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

const size_t AES_BLOCK_SIZE = 128;

typedef struct {
	EVP_CIPHER_CTX *ctx;
	bool ok;
	unsigned char *key;
	unsigned char *iv;
	} CipherState;

static CoreAPI *api;

static GlobalTypeLink *charArrayGT = NULL;

/************************************************
*************************************************
*************************************************
AES-CBC functions
*************************************************
*************************************************
************************************************/

INSTRUCTION_DEF op_aes_cbc_encryptInit(FrameData* cframe)
	{
	// https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
	
	CipherState *cst = malloc(sizeof(CipherState));
	memset(cst, '\0', sizeof(CipherState));
	
    /* Create and initialise the context */
    if (!(cst -> ctx = EVP_CIPHER_CTX_new()))
		{
        api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		free(cst);
		return RETURN_OK;
		}
	
	size_t xs = api -> getParamInt(cframe, 0);
	
	unsigned char *key = (unsigned char*) x_getParam_char_array(api, cframe, 1);
	
	unsigned char *iv = (unsigned char*) x_getParam_char_array(api, cframe, 2);
	
	if (xs == 128)
		{
		if (1 != EVP_EncryptInit_ex(cst -> ctx, EVP_aes_128_cbc(), NULL, key, iv))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
		else if (xs == 192)
		{
		if (1 != EVP_EncryptInit_ex(cst -> ctx, EVP_aes_192_cbc(), NULL, key, iv))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
		else if (xs == 256)
		{
		if (1 != EVP_EncryptInit_ex(cst -> ctx, EVP_aes_256_cbc(), NULL, key, iv))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
		else
		{
		api -> throwException(cframe, "library error: unknown key length");
		EVP_CIPHER_CTX_free(cst -> ctx);
		free(key);
		free(iv);
		free(cst);
		return RETURN_OK;
		}
	
	cst -> key = key;
	cst -> iv = iv;
	
	cst -> ok = true;
	
	api -> returnRaw(cframe, (unsigned char*) &cst, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_encryptPart(FrameData* cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* input = api -> getParamEl(cframe, 1);
	size_t alen = api -> getArrayLength(input);
	
	DanaEl* array = api -> makeArray(charArrayGT, alen + AES_BLOCK_SIZE, NULL);
	
	if (array == NULL)
		{
		api -> throwException(cframe, "out of memory");
		cst -> ok = false;
		return RETURN_OK;
		}
	
	int len = 0;
	
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if (1 != EVP_EncryptUpdate(cst -> ctx, api -> getArrayContent(array), &len, api -> getArrayContent(input), alen))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	api -> setArrayLength(array, len);
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_encryptFinish(FrameData* cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	unsigned char ciphertext[128];
	int len = 0;
	
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptFinal_ex(cst -> ctx, ciphertext, &len))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	unsigned char* cnt = NULL;
	DanaEl *array = api -> makeArray(charArrayGT, len, &cnt);
	memcpy(cnt, ciphertext, len);
	
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_encryptOK(FrameData *cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	unsigned char ok = cst -> ok;
	api -> returnRaw(cframe, &ok, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_decryptInit(FrameData* cframe)
	{
	// https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
	
	CipherState *cst = malloc(sizeof(CipherState));
	memset(cst, '\0', sizeof(CipherState));

    /* Create and initialise the context */
    if (!(cst -> ctx = EVP_CIPHER_CTX_new()))
		{
        api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		free(cst);
		return RETURN_OK;
		}
	
	size_t xs = api -> getParamInt(cframe, 0);
	
	unsigned char *key = (unsigned char*) x_getParam_char_array(api, cframe, 1);
	
	unsigned char *iv = (unsigned char*) x_getParam_char_array(api, cframe, 2);
	
	if (xs == 128)
		{
		if (1 != EVP_DecryptInit_ex(cst -> ctx, EVP_aes_128_cbc(), NULL, key, iv))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
		else if (xs == 192)
		{
		if (1 != EVP_DecryptInit_ex(cst -> ctx, EVP_aes_192_cbc(), NULL, key, iv))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
		else if (xs == 256)
		{
		if (1 != EVP_DecryptInit_ex(cst -> ctx, EVP_aes_256_cbc(), NULL, key, iv))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
		else
		{
		api -> throwException(cframe, "library error: unknown key length");
		EVP_CIPHER_CTX_free(cst -> ctx);
		free(key);
		free(iv);
		free(cst);
		return RETURN_OK;
		}
	
	cst -> key = key;
	cst -> iv = iv;
	
	cst -> ok = true;
	
	api -> returnRaw(cframe, (unsigned char*) &cst, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_decryptPart(FrameData *cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* input = api -> getParamEl(cframe, 1);
	size_t alen = api -> getArrayLength(input);
	
	DanaEl* array = api -> makeArray(charArrayGT, alen + AES_BLOCK_SIZE, NULL);
	
	if (array == NULL)
		{
		api -> throwException(cframe, "out of memory");
		cst -> ok = false;
		return RETURN_OK;
		}
	
	int len = 0;
	
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_DecryptUpdate(cst -> ctx, api -> getArrayContent(array), &len, api -> getArrayContent(input), alen))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	api -> setArrayLength(array, len);
	
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_decryptFinish(FrameData *cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	unsigned char output_text[128];
	int len = 0;
	
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_DecryptFinal_ex(cst -> ctx, output_text, &len))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	unsigned char* cnt = NULL;
	DanaEl *array = api -> makeArray(charArrayGT, len, &cnt);
	memcpy(cnt, output_text, len);
	
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_decryptOK(FrameData *cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	unsigned char ok = cst -> ok;
	api -> returnRaw(cframe, &ok, 1);
	
	return RETURN_OK;
	}

/************************************************
*************************************************
*************************************************
AES-GCM functions
*************************************************
*************************************************
************************************************/

INSTRUCTION_DEF op_aes_gcm_encryptInit(FrameData* cframe)
	{
	// https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
	
	CipherState *cst = malloc(sizeof(CipherState));
	memset(cst, '\0', sizeof(CipherState));

    /* Create and initialise the context */
    if (!(cst -> ctx = EVP_CIPHER_CTX_new()))
		{
        api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		free(cst);
		return RETURN_OK;
		}
	
	size_t xs = api -> getParamInt(cframe, 0);
	
	unsigned char *key = (unsigned char*) x_getParam_char_array(api, cframe, 1);
	
	unsigned char *iv = (unsigned char*) x_getParam_char_array(api, cframe, 2);
	
	if (xs == 128)
		{
		if (1 != EVP_EncryptInit_ex(cst -> ctx, EVP_aes_128_gcm(), NULL, NULL, NULL))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
		else if (xs == 192)
		{
		if (1 != EVP_EncryptInit_ex(cst -> ctx, EVP_aes_192_gcm(), NULL, NULL, NULL))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
		else if (xs == 256)
		{
		if (1 != EVP_EncryptInit_ex(cst -> ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
		else
		{
		api -> throwException(cframe, "library error: unknown key length");
		EVP_CIPHER_CTX_free(cst -> ctx);
		free(key);
		free(iv);
		free(cst);
		return RETURN_OK;
		}
	
	if (strlen((char*) iv) != 12)
		{
		/* Set IV length. Not necessary if this is 12 bytes (96 bits) */
		if (!EVP_CIPHER_CTX_ctrl(cst -> ctx, EVP_CTRL_GCM_SET_IVLEN, strlen((char*) iv), NULL))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
	
	if (1 != EVP_EncryptInit_ex(cst -> ctx, NULL, NULL, key, iv))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		EVP_CIPHER_CTX_free(cst -> ctx);
		free(key);
		free(iv);
		free(cst);
		return RETURN_OK;
		}
	
	cst -> key = key;
	cst -> iv = iv;
	
	cst -> ok = true;
	
	api -> returnRaw(cframe, (unsigned char*) &cst, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_encryptAAD(FrameData* cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* input = api -> getParamEl(cframe, 1);
	size_t alen = api -> getArrayLength(input);
	
	int len = 0;
	
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(cst -> ctx, NULL, &len, api -> getArrayContent(input), alen))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_encryptPart(FrameData *cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* input = api -> getParamEl(cframe, 1);
	size_t alen = api -> getArrayLength(input);
	
	DanaEl* array = api -> makeArray(charArrayGT, alen + AES_BLOCK_SIZE, NULL);
	
	if (array == NULL)
		{
		api -> throwException(cframe, "out of memory");
		cst -> ok = false;
		return RETURN_OK;
		}
	
	int len = 0;
	
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(cst -> ctx, api -> getArrayContent(array), &len, api -> getArrayContent(input), alen))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	api -> setArrayLength(array, len);
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_encryptFinish(FrameData *cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	unsigned char ciphertext[128];
	int len = 0;
	
    if (1 != EVP_EncryptFinal_ex(cst -> ctx, ciphertext, &len))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	unsigned char* cnt = NULL;
	DanaEl *array = api -> makeArray(charArrayGT, len, &cnt);
	memcpy(cnt, ciphertext, len);
	
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_encryptGetTag(FrameData* cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	unsigned char tag[16]; //16 bytes is the largest possible tag at the time of writing, and is the default
	int len = 16;
	
	/* Get the tag */
    if (1 != EVP_CIPHER_CTX_ctrl(cst -> ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	unsigned char* cnt = NULL;
	DanaEl *array = api -> makeArray(charArrayGT, len, &cnt);
	memcpy(cnt, tag, len);
	
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_encryptOK(FrameData* cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	unsigned char ok = cst -> ok;
	api -> returnRaw(cframe, &ok, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_decryptInit(FrameData *cframe)
	{
	// https://wiki.openssl.org/index.php/EVP_Symmetric_Encryption_and_Decryption
	
	CipherState *cst = malloc(sizeof(CipherState));
	memset(cst, '\0', sizeof(CipherState));

    /* Create and initialise the context */
    if (!(cst -> ctx = EVP_CIPHER_CTX_new()))
		{
        api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		free(cst);
		return RETURN_OK;
		}
	
	size_t xs = api -> getParamInt(cframe, 0);
	
	unsigned char *key = (unsigned char*) x_getParam_char_array(api, cframe, 1);
	
	unsigned char *iv = (unsigned char*) x_getParam_char_array(api, cframe, 2);
	
	if (xs == 128)
		{
		if (1 != EVP_DecryptInit_ex(cst -> ctx, EVP_aes_128_gcm(), NULL, NULL, NULL))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
		else if (xs == 192)
		{
		if (1 != EVP_DecryptInit_ex(cst -> ctx, EVP_aes_192_gcm(), NULL, NULL, NULL))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
		else if (xs == 256)
		{
		if (1 != EVP_DecryptInit_ex(cst -> ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
		else
		{
		api -> throwException(cframe, "library error: unknown key length");
		EVP_CIPHER_CTX_free(cst -> ctx);
		free(key);
		free(iv);
		free(cst);
		return RETURN_OK;
		}
	
	if (strlen((char*) iv) != 12)
		{
		/* Set IV length. Not necessary if this is 12 bytes (96 bits) */
		if (!EVP_CIPHER_CTX_ctrl(cst -> ctx, EVP_CTRL_GCM_SET_IVLEN, strlen((char*) iv), NULL))
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			EVP_CIPHER_CTX_free(cst -> ctx);
			free(key);
			free(iv);
			free(cst);
			return RETURN_OK;
			}
		}
	
	if (1 != EVP_DecryptInit_ex(cst -> ctx, NULL, NULL, key, iv))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		EVP_CIPHER_CTX_free(cst -> ctx);
		free(key);
		free(iv);
		free(cst);
		return RETURN_OK;
		}
	
	cst -> key = key;
	cst -> iv = iv;
	
	cst -> ok = true;
	
	api -> returnRaw(cframe, (unsigned char*) &cst, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_decryptAAD(FrameData *cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* input = api -> getParamEl(cframe, 1);
	size_t alen = api -> getArrayLength(input);
	
	int len = 0;
	
    if(1 != EVP_DecryptUpdate(cst -> ctx, NULL, &len, api -> getArrayContent(input), alen))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_decryptPart(FrameData *cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* input = api -> getParamEl(cframe, 1);
	size_t alen = api -> getArrayLength(input);
	
	DanaEl* array = api -> makeArray(charArrayGT, alen + AES_BLOCK_SIZE, NULL);
	
	if (array == NULL)
		{
		api -> throwException(cframe, "out of memory");
		cst -> ok = false;
		return RETURN_OK;
		}
	
	int len = 0;
	
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_DecryptUpdate(cst -> ctx, api -> getArrayContent(array), &len, api -> getArrayContent(input), alen))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	api -> setArrayLength(array, len);
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_decryptFinish(FrameData* cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	unsigned char output_text[128];
	int len = 0;
	
	int ret = 0;
	
    if ((ret = EVP_DecryptFinal_ex(cst -> ctx, output_text, &len)) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	unsigned char* cnt = NULL;
	DanaEl *array = api -> makeArray(charArrayGT, len, &cnt);
	memcpy(cnt, output_text, len);
	
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_decryptSetTag(FrameData* cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* input = api -> getParamEl(cframe, 1);
	
	unsigned char *tag = api -> getArrayContent(input);
	
	/* Set the tag */
    if (!EVP_CIPHER_CTX_ctrl(cst -> ctx, EVP_CTRL_GCM_SET_TAG, 16, tag))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_decryptOK(FrameData* cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	unsigned char ok = cst -> ok;
	api -> returnRaw(cframe, &ok, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_instance_cleanup(FrameData* cframe)
	{
	CipherState *cst;
	memcpy(&cst, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	EVP_CIPHER_CTX_free(cst -> ctx);
	free(cst -> key);
	free(cst -> iv);
	free(cst);
	
	return RETURN_OK;
	}

// RSA -- https://www.openssl.org/docs/manmaster/man3/EVP_PKEY_encrypt.html
// -- https://www.openssl.org/docs/manmaster/man3/EVP_PKEY_decrypt.html
// -- https://www.openssl.org/docs/manmaster/man3/EVP_PKEY_sign.html
// -- https://www.openssl.org/docs/manmaster/man3/EVP_PKEY_verify.html

INSTRUCTION_DEF op_rsa_oaep_init(FrameData *cframe)
	{
	DanaEl* input = api -> getParamEl(cframe, 0);
	
	unsigned char mode = api -> getParamRaw(cframe, 1)[0];
	
	EVP_PKEY_CTX *ctx;
	EVP_PKEY *key = EVP_PKEY_new();
	
	//load the key
	BIO *biomem = BIO_new(BIO_s_mem());
	BIO_write(biomem, api -> getArrayContent(input), api -> getArrayLength(input));
	if (mode == 1)
		PEM_read_bio_PUBKEY(biomem, &key, 0, 0);
		else if (mode == 2)
		PEM_read_bio_PrivateKey(biomem, &key, 0, 0);
	BIO_free(biomem);
	
	ctx = EVP_PKEY_CTX_new(key, NULL);
	if (!ctx)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	if (mode == 1)
		{
		if (EVP_PKEY_encrypt_init(ctx) <= 0)
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		}
		else if (mode == 2)
		{
		if (EVP_PKEY_decrypt_init(ctx) <= 0)
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		}
	
	if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	EVP_PKEY_free(key);
	
	api -> returnRaw(cframe, (unsigned char*) &ctx, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_oaep_encrypt(FrameData *cframe)
	{
	EVP_PKEY_CTX *ctx;
	memcpy(&ctx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* input = api -> getParamEl(cframe, 1);
	
	size_t outlen = 0;
	
	/* Determine buffer length */
	if (EVP_PKEY_encrypt(ctx, NULL, &outlen, api -> getArrayContent(input), api -> getArrayLength(input)) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	DanaEl* array = api -> makeArray(charArrayGT, outlen, NULL);
	
	if (array == NULL)
		{
		api -> throwException(cframe, "out of memory");
		return RETURN_OK;
		}

	if (EVP_PKEY_encrypt(ctx, api -> getArrayContent(array), &outlen, api -> getArrayContent(input), api -> getArrayLength(input)) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		api -> destroyArray(array);
		return RETURN_OK;
		}
	
	api -> setArrayLength(array, outlen);
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_oaep_decrypt(FrameData *cframe)
	{
	EVP_PKEY_CTX *ctx;
	memcpy(&ctx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* input = api -> getParamEl(cframe, 1);
	size_t alen = api -> getArrayLength(input);
	
	size_t outlen = 0;
	
	/* Determine buffer length */
	if (EVP_PKEY_decrypt(ctx, NULL, &outlen, api -> getArrayContent(input), alen) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	DanaEl* array = api -> makeArray(charArrayGT, alen + AES_BLOCK_SIZE, NULL);
	
	if (array == NULL)
		{
		api -> throwException(cframe, "out of memory");
		return RETURN_OK;
		}

	if (EVP_PKEY_decrypt(ctx, api -> getArrayContent(array), &outlen, api -> getArrayContent(input), alen) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		api -> destroyArray(array);
		return RETURN_OK;
		}
	
	api -> setArrayLength(array, outlen);
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_oaep_cleanup(FrameData *cframe)
	{
	EVP_PKEY_CTX *ctx;
	memcpy(&ctx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	EVP_PKEY_CTX_free(ctx);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_pss_init(FrameData *cframe)
	{
	DanaEl* input = api -> getParamEl(cframe, 0);
	
	unsigned char mode = api -> getParamRaw(cframe, 1)[0];
	
	EVP_PKEY_CTX *ctx;
	EVP_PKEY *key = EVP_PKEY_new();
	
	//load the key
	BIO *biomem = BIO_new(BIO_s_mem());
	BIO_write(biomem, api -> getArrayContent(input), api -> getArrayLength(input));
	if (mode == 1)
		PEM_read_bio_PUBKEY(biomem, &key, 0, 0);
		else if (mode == 2)
		PEM_read_bio_PrivateKey(biomem, &key, 0, 0);
	BIO_free(biomem);
	
	ctx = EVP_PKEY_CTX_new(key, NULL);
	if (!ctx)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	if (mode == 1)
		{
		if (EVP_PKEY_verify_init(ctx) <= 0)
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		}
		else if (mode == 2)
		{
		if (EVP_PKEY_sign_init(ctx) <= 0)
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		}
	
	if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	if (EVP_PKEY_CTX_set_signature_md(ctx, EVP_sha256()) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	EVP_PKEY_free(key);
	
	api -> returnRaw(cframe, (unsigned char*) &ctx, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_pss_sign(FrameData *cframe)
	{
	EVP_PKEY_CTX *ctx;
	memcpy(&ctx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* input = api -> getParamEl(cframe, 1);
	size_t alen = api -> getArrayLength(input);
	
	// -- hash the msg first --
	EVP_MD_CTX *mdctx = NULL;
	const EVP_MD *md = NULL;
	
	int hash_type = 2;
	
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len;
	
	if (hash_type == 1)
		md = EVP_sha224();
		else if (hash_type == 2)
		md = EVP_sha256();
		else if (hash_type == 3)
		md = EVP_sha384();
		else if (hash_type == 4)
		md = EVP_sha512();
		else
		{
		api -> throwException(cframe, "unknown hash type");
		return RETURN_OK;
		}
	
	mdctx = EVP_MD_CTX_create();
	EVP_DigestInit_ex(mdctx, md, NULL);
	EVP_DigestUpdate(mdctx, api -> getArrayContent(input), alen);
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_destroy(mdctx);
	
	// -- --
	
	size_t outlen = 0;
	
	/* Determine buffer length */
	if (EVP_PKEY_sign(ctx, NULL, &outlen, md_value, md_len) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	DanaEl* array = api -> makeArray(charArrayGT, outlen, NULL);
	
	if (array == NULL)
		{
		api -> throwException(cframe, "out of memory");
		return RETURN_OK;
		}

	if (EVP_PKEY_sign(ctx, api -> getArrayContent(array), &outlen, md_value, md_len) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		api -> destroyArray(array);
		return RETURN_OK;
		}
	
	api -> setArrayLength(array, outlen);
	api -> returnEl(cframe, array);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_pss_verify(FrameData *cframe)
	{
	EVP_PKEY_CTX *ctx;
	memcpy(&ctx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* input = api -> getParamEl(cframe, 1);
	DanaEl* signed_input = api -> getParamEl(cframe, 2);
	
	// -- hash the msg first --
	EVP_MD_CTX *mdctx = NULL;
	const EVP_MD *md = NULL;
	
	int hash_type = 2;
	
	unsigned char md_value[EVP_MAX_MD_SIZE];
	unsigned int md_len;
	
	if (hash_type == 1)
		md = EVP_sha224();
		else if (hash_type == 2)
		md = EVP_sha256();
		else if (hash_type == 3)
		md = EVP_sha384();
		else if (hash_type == 4)
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
	
	// -- --
	
	int ret = EVP_PKEY_verify(ctx, api -> getArrayContent(signed_input), api -> getArrayLength(signed_input), md_value, md_len);
	
	if (ret < 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	unsigned char ok = ret == 1;
	api -> returnRaw(cframe, &ok, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_pss_cleanup(FrameData *cframe)
	{
	EVP_PKEY_CTX *ctx;
	memcpy(&ctx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	EVP_PKEY_CTX_free(ctx);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_generate_key(FrameData *cframe)
	{
	size_t keyLength = api -> getParamInt(cframe, 0);
	
	int rc;
	
	BIGNUM *bn = BN_new();
	
	rc = BN_set_word(bn, RSA_F4);

	if (rc == 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	//poke the random number generator
	RAND_poll();

	EVP_PKEY* pkey = EVP_RSA_gen(keyLength);
	
	unsigned char *pp;
	size_t mlen;
	
	// write public key to PKCS PEM
	BIO *biomem = BIO_new(BIO_s_mem());
	//rc = PEM_write_bio_RSAPublicKey(biomem, rsa); //TODO: should we be using this format instead? i.e. PKCS??? (and change the format for oaes input?)
	rc = PEM_write_bio_PUBKEY(biomem, pkey);
	mlen = BIO_get_mem_data(biomem, &pp);
	DanaEl* newArray = api -> makeArray(charArrayGT, mlen, NULL);
	BIO_read(biomem, api -> getArrayContent(newArray), mlen);
	BIO_free(biomem);
	
	/* -- */
	DanaEl* rdata = api -> getParamEl(cframe, 1);
	api -> setDataFieldEl(rdata, 0, newArray);
	/* -- */
	
	// Write private key in PKCS PEM
	biomem = BIO_new(BIO_s_mem());
    rc = PEM_write_bio_PKCS8PrivateKey(biomem, pkey, NULL, NULL, 0, NULL, NULL);
	mlen = BIO_get_mem_data(biomem, &pp);
	newArray = api -> makeArray(charArrayGT, mlen, NULL);
	BIO_read(biomem, api -> getArrayContent(newArray), mlen);
	BIO_free(biomem);
	
	/* -- */
	rdata = api -> getParamEl(cframe, 2);
	api -> setDataFieldEl(rdata, 0, newArray);
	/* -- */
	
	BN_free(bn);
	EVP_PKEY_free(pkey);
	
	unsigned char ok = 1;
	api -> returnRaw(cframe, &ok, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_convert_key(FrameData* cframe)
	{
	EVP_PKEY *pkey = EVP_PKEY_new();
	
	DanaEl *pubKeyIn = api -> getParamEl(cframe, 0);
	DanaEl *priKeyIn = api -> getParamEl(cframe, 1);
	unsigned char inputType = api -> getParamRaw(cframe, 2)[0];
	unsigned char outputType = api -> getParamRaw(cframe, 5)[0];
	
	//read the key in to one of the above structures, depending on the type param
	
	bool fail = false;
	
	if (inputType == 1)
		{
		//PKCS
		BIO *biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, api -> getArrayContent(pubKeyIn), api -> getArrayLength(pubKeyIn));
		if (PEM_read_bio_PUBKEY(biomem, &pkey, 0, 0) == NULL) fail = true;
		BIO_free(biomem);
		
		if (fail)
			{
			EVP_PKEY_free(pkey);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		
		biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, api -> getArrayContent(priKeyIn), api -> getArrayLength(priKeyIn));
		if (PEM_read_bio_PrivateKey(biomem, &pkey, 0, 0) == NULL) fail = true;
		BIO_free(biomem);
		
		if (fail)
			{
			EVP_PKEY_free(pkey);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		}
		else if (inputType == 2)
		{
		OSSL_DECODER_CTX* dctx;

		//RSA
		BIO *biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, api -> getArrayContent(pubKeyIn), api -> getArrayLength(pubKeyIn));
		dctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, "PEM", NULL, "RSA", OSSL_KEYMGMT_SELECT_PUBLIC_KEY, NULL, NULL);
		if (!OSSL_DECODER_from_bio(dctx, biomem))
			{
			fail = true;
			}
		OSSL_DECODER_CTX_free(dctx);
		BIO_free(biomem);
		
		if (fail)
			{
			EVP_PKEY_free(pkey);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		
		biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, api -> getArrayContent(priKeyIn), api -> getArrayLength(priKeyIn));
		dctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, "PEM", NULL, "RSA", OSSL_KEYMGMT_SELECT_PRIVATE_KEY, NULL, NULL);
		if (!OSSL_DECODER_from_bio(dctx, biomem))
			{
			fail = true;
			}
		OSSL_DECODER_CTX_free(dctx);
		BIO_free(biomem);
		
		if (fail)
			{
			EVP_PKEY_free(pkey);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		}
		else if (inputType == 4)
		{
		OSSL_DECODER_CTX* dctx;

		//ASN.1/DER
		
		//i2d_RSAPublicKey_bio: i is internal, d is "der"
		BIO *biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, api -> getArrayContent(pubKeyIn), api -> getArrayLength(pubKeyIn));
		dctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, "DER", "type-specific", NULL, EVP_PKEY_PUBLIC_KEY, NULL, NULL);
		if (!OSSL_DECODER_from_bio(dctx, biomem))
			{
			fail = true;
			}
		OSSL_DECODER_CTX_free(dctx);
		BIO_free(biomem);
		
		if (fail)
			{
			EVP_PKEY_free(pkey);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		
		biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, api -> getArrayContent(priKeyIn), api -> getArrayLength(priKeyIn));
		dctx = OSSL_DECODER_CTX_new_for_pkey(&pkey, "DER", "type-specific", NULL, OSSL_KEYMGMT_SELECT_PRIVATE_KEY, NULL, NULL);
		if (!OSSL_DECODER_from_bio(dctx, biomem))
			{
			fail = true;
			}
		OSSL_DECODER_CTX_free(dctx);
		BIO_free(biomem);
		
		if (fail)
			{
			EVP_PKEY_free(pkey);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		}
	
	//write out the keys
	
	size_t pubKeyOutLen = 0;
	
	size_t priKeyOutLen = 0;
	
	unsigned char *pp;
	
	int rc = 0;
	
	DanaEl *arrayPub = NULL;
	DanaEl *arrayPri = NULL;
	
	if (outputType == 1)
		{
		BIO *biomem = BIO_new(BIO_s_mem());
		rc = PEM_write_bio_PUBKEY(biomem, pkey);
		if (rc == 0)
			{
			EVP_PKEY_free(pkey);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		pubKeyOutLen = BIO_get_mem_data(biomem, &pp);
		arrayPub = api -> makeArray(charArrayGT, pubKeyOutLen, NULL);
		BIO_read(biomem, api -> getArrayContent(arrayPub), pubKeyOutLen);
		BIO_free(biomem);
		
		biomem = BIO_new(BIO_s_mem());
		rc = PEM_write_bio_PKCS8PrivateKey(biomem, pkey, NULL, NULL, 0, NULL, NULL);
		if (rc == 0)
			{
			EVP_PKEY_free(pkey);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		priKeyOutLen = BIO_get_mem_data(biomem, &pp);
		arrayPri = api -> makeArray(charArrayGT, priKeyOutLen, NULL);
		BIO_read(biomem, api -> getArrayContent(arrayPri), priKeyOutLen);
		BIO_free(biomem);
		}
		else if (outputType == 2)
		{
		OSSL_ENCODER_CTX* dctx;

		BIO *biomem = BIO_new(BIO_s_mem());
		dctx = OSSL_ENCODER_CTX_new_for_pkey(pkey, OSSL_KEYMGMT_SELECT_PUBLIC_KEY, "PEM", "RSA", NULL);
		if (!OSSL_ENCODER_to_bio(dctx, biomem))
			{
			fail = true;
			}
		OSSL_ENCODER_CTX_free(dctx);
		pubKeyOutLen = BIO_get_mem_data(biomem, &pp);
		arrayPub = api -> makeArray(charArrayGT, pubKeyOutLen, NULL);
		BIO_read(biomem, api -> getArrayContent(arrayPub), pubKeyOutLen);
		BIO_free(biomem);
		
		biomem = BIO_new(BIO_s_mem());
		dctx = OSSL_ENCODER_CTX_new_for_pkey(pkey, OSSL_KEYMGMT_SELECT_PRIVATE_KEY, "PEM", "RSA", NULL);
		if (!OSSL_ENCODER_to_bio(dctx, biomem))
			{
			fail = true;
			}
		OSSL_ENCODER_CTX_free(dctx);
		priKeyOutLen = BIO_get_mem_data(biomem, &pp);
		arrayPri = api -> makeArray(charArrayGT, priKeyOutLen, NULL);
		BIO_read(biomem, api -> getArrayContent(arrayPri), priKeyOutLen);
		BIO_free(biomem);
		}
		else if (outputType == 4)
		{
		OSSL_ENCODER_CTX* dctx;

		BIO *biomem = BIO_new(BIO_s_mem());
		dctx = OSSL_ENCODER_CTX_new_for_pkey(pkey, EVP_PKEY_PUBLIC_KEY, "DER", "type-specific", NULL);
		if (!OSSL_ENCODER_to_bio(dctx, biomem))
			{
			fail = true;
			}
		OSSL_ENCODER_CTX_free(dctx);
		pubKeyOutLen = BIO_get_mem_data(biomem, &pp);
		arrayPub = api -> makeArray(charArrayGT, pubKeyOutLen, NULL);
		BIO_read(biomem, api -> getArrayContent(arrayPub), pubKeyOutLen);
		BIO_free(biomem);
		
		biomem = BIO_new(BIO_s_mem());
		dctx = OSSL_ENCODER_CTX_new_for_pkey(pkey, OSSL_KEYMGMT_SELECT_PRIVATE_KEY, "DER", "type-specific", NULL);
		if (!OSSL_ENCODER_to_bio(dctx, biomem))
			{
			fail = true;
			}
		OSSL_ENCODER_CTX_free(dctx);
		priKeyOutLen = BIO_get_mem_data(biomem, &pp);
		arrayPri = api -> makeArray(charArrayGT, priKeyOutLen, NULL);
		BIO_read(biomem, api -> getArrayContent(arrayPri), priKeyOutLen);
		BIO_free(biomem);
		}
	
	EVP_PKEY_free(pkey);
	
	/* -- */
	DanaEl* rdata = api -> getParamEl(cframe, 3);
	api -> setDataFieldEl(rdata, 0, arrayPub);
	/* -- */
	
	/* -- */
	rdata = api -> getParamEl(cframe, 4);
	api -> setDataFieldEl(rdata, 0, arrayPri);
	/* -- */
	
	unsigned char ok = 1;
	api -> returnRaw(cframe, &ok, 1);
	
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
	
	setInterfaceFunction("aes_cbc_encryptInit", op_aes_cbc_encryptInit);
	setInterfaceFunction("aes_cbc_encryptPart", op_aes_cbc_encryptPart);
	setInterfaceFunction("aes_cbc_encryptFinish", op_aes_cbc_encryptFinish);
	setInterfaceFunction("aes_cbc_encryptOK", op_aes_cbc_encryptOK);
	
	setInterfaceFunction("aes_cbc_decryptInit", op_aes_cbc_decryptInit);
	setInterfaceFunction("aes_cbc_decryptPart", op_aes_cbc_decryptPart);
	setInterfaceFunction("aes_cbc_decryptFinish", op_aes_cbc_decryptFinish);
	setInterfaceFunction("aes_cbc_decryptOK", op_aes_cbc_decryptOK);
	
	setInterfaceFunction("aes_gcm_encryptInit", op_aes_gcm_encryptInit);
	setInterfaceFunction("aes_gcm_encryptAAD", op_aes_gcm_encryptAAD);
	setInterfaceFunction("aes_gcm_encryptPart", op_aes_gcm_encryptPart);
	setInterfaceFunction("aes_gcm_encryptFinish", op_aes_gcm_encryptFinish);
	setInterfaceFunction("aes_gcm_encryptGetTag", op_aes_gcm_encryptGetTag);
	setInterfaceFunction("aes_gcm_encryptOK", op_aes_gcm_encryptOK);
	
	setInterfaceFunction("aes_gcm_decryptInit", op_aes_gcm_decryptInit);
	setInterfaceFunction("aes_gcm_decryptAAD", op_aes_gcm_decryptAAD);
	setInterfaceFunction("aes_gcm_decryptPart", op_aes_gcm_decryptPart);
	setInterfaceFunction("aes_gcm_decryptFinish", op_aes_gcm_decryptFinish);
	setInterfaceFunction("aes_gcm_decryptSetTag", op_aes_gcm_decryptSetTag);
	setInterfaceFunction("aes_gcm_decryptOK", op_aes_gcm_decryptOK);
	
	setInterfaceFunction("instance_cleanup", op_instance_cleanup);
	
	setInterfaceFunction("rsa_oaep_init", op_rsa_oaep_init);
	setInterfaceFunction("rsa_oaep_encrypt", op_rsa_oaep_encrypt);
	setInterfaceFunction("rsa_oaep_decrypt", op_rsa_oaep_decrypt);
	setInterfaceFunction("rsa_oaep_cleanup", op_rsa_oaep_cleanup);
	
	setInterfaceFunction("rsa_pss_init", op_rsa_pss_init);
	setInterfaceFunction("rsa_pss_sign", op_rsa_pss_sign);
	setInterfaceFunction("rsa_pss_verify", op_rsa_pss_verify);
	setInterfaceFunction("rsa_pss_cleanup", op_rsa_pss_cleanup);
	
	setInterfaceFunction("rsa_generate_key", op_rsa_generate_key);
	setInterfaceFunction("rsa_convert_key", op_rsa_convert_key);
	
	return getPublicInterface();
	}

void unload()
	{
	cleanup_openssl();
	
	api -> decrementGTRefCount(charArrayGT);
	}
