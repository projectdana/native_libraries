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

typedef struct {
	EVP_CIPHER_CTX *ctx;
	bool ok;
	unsigned char *key;
	unsigned char *iv;
	} CipherState;

static CoreAPI *api;

static GlobalTypeLink *charArrayGT = NULL;

static void returnByteArray(VFrame *f, unsigned char *data, size_t len)
	{
	LiveArray *array = malloc(sizeof(LiveArray));
	memset(array, '\0', sizeof(LiveArray));
	
	array -> data = data;
	array -> length = len;
	
	array -> gtLink = charArrayGT;
	api -> incrementGTRefCount(array -> gtLink);
	array -> refi.ocm = f -> blocking -> instance;
	
	array -> refi.refCount ++;
	array -> refi.type = array -> gtLink -> typeLink;
	
	VVarLivePTR *ptrh = (VVarLivePTR*) &f -> localsData[((DanaType*) f -> localsDef) -> fields[0].offset];
	ptrh -> content = (unsigned char*) array;
	}

/************************************************
*************************************************
*************************************************
AES-CBC functions
*************************************************
*************************************************
************************************************/

INSTRUCTION_DEF op_aes_cbc_encryptInit(VFrame *cframe)
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
	
	size_t xs = 0;
	copyHostInteger((unsigned char*) &xs, getVariableContent(cframe, 0), sizeof(size_t));
	
	unsigned char *key = (unsigned char*) getParam_char_array(cframe, 1);
	
	unsigned char *iv = (unsigned char*) getParam_char_array(cframe, 2);
	
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
	
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &cst, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_encryptPart(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	unsigned char ciphertext[128];
	int len = 0;
	
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(cst -> ctx, ciphertext, &len, input -> data, input -> length))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	unsigned char *pbuf = malloc(len);
	memcpy(pbuf, ciphertext, len);
	
	returnByteArray(cframe, pbuf, len);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_encryptFinish(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
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
	
	unsigned char *pbuf = malloc(len);
	memcpy(pbuf, ciphertext, len);
	
	returnByteArray(cframe, pbuf, len);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_encryptOK(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	return_bool(cframe, cst -> ok);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_decryptInit(VFrame *cframe)
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
	
	size_t xs = 0;
	copyHostInteger((unsigned char*) &xs, getVariableContent(cframe, 0), sizeof(size_t));
	
	unsigned char *key = (unsigned char*) getParam_char_array(cframe, 1);
	
	unsigned char *iv = (unsigned char*) getParam_char_array(cframe, 2);
	
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
	
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &cst, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_decryptPart(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	unsigned char output_text[128];
	int len = 0;
	
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_DecryptUpdate(cst -> ctx, output_text, &len, input -> data, input -> length))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	unsigned char *pbuf = malloc(len);
	memcpy(pbuf, output_text, len);
	
	returnByteArray(cframe, pbuf, len);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_decryptFinish(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
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
	
	unsigned char *pbuf = malloc(len);
	memcpy(pbuf, output_text, len);
	
	returnByteArray(cframe, pbuf, len);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_cbc_decryptOK(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	return_bool(cframe, cst -> ok);
	
	return RETURN_OK;
	}

/************************************************
*************************************************
*************************************************
AES-GCM functions
*************************************************
*************************************************
************************************************/

INSTRUCTION_DEF op_aes_gcm_encryptInit(VFrame *cframe)
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
	
	size_t xs = 0;
	copyHostInteger((unsigned char*) &xs, getVariableContent(cframe, 0), sizeof(size_t));
	
	unsigned char *key = (unsigned char*) getParam_char_array(cframe, 1);
	
	unsigned char *iv = (unsigned char*) getParam_char_array(cframe, 2);
	
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
	
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &cst, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_encryptAAD(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	int len = 0;
	
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(cst -> ctx, NULL, &len, input -> data, input -> length))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_encryptPart(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	unsigned char ciphertext[128];
	int len = 0;
	
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_EncryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_EncryptUpdate(cst -> ctx, ciphertext, &len, input -> data, input -> length))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	unsigned char *pbuf = malloc(len);
	memcpy(pbuf, ciphertext, len);
	
	returnByteArray(cframe, pbuf, len);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_encryptFinish(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	unsigned char ciphertext[128];
	int len = 0;
	
    if (1 != EVP_EncryptFinal_ex(cst -> ctx, ciphertext, &len))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	unsigned char *pbuf = malloc(len);
	memcpy(pbuf, ciphertext, len);
	
	returnByteArray(cframe, pbuf, len);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_encryptGetTag(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	unsigned char tag[16]; //16 bytes is the largest possible tag at the time of writing, and is the default
	int len = 16;
	
	/* Get the tag */
    if (1 != EVP_CIPHER_CTX_ctrl(cst -> ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	unsigned char *pbuf = malloc(len);
	memcpy(pbuf, tag, len);
	
	returnByteArray(cframe, pbuf, len);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_encryptOK(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	return_bool(cframe, cst -> ok);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_decryptInit(VFrame *cframe)
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
	
	size_t xs = 0;
	copyHostInteger((unsigned char*) &xs, getVariableContent(cframe, 0), sizeof(size_t));
	
	unsigned char *key = (unsigned char*) getParam_char_array(cframe, 1);
	
	unsigned char *iv = (unsigned char*) getParam_char_array(cframe, 2);
	
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
	
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &cst, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_decryptAAD(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	int len = 0;
	
    if(1 != EVP_DecryptUpdate(cst -> ctx, NULL, &len, input -> data, input -> length))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_decryptPart(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	unsigned char output_text[128];
	int len = 0;
	
    /*
     * Provide the message to be encrypted, and obtain the encrypted output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_DecryptUpdate(cst -> ctx, output_text, &len, input -> data, input -> length))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	unsigned char *pbuf = malloc(len);
	memcpy(pbuf, output_text, len);
	
	returnByteArray(cframe, pbuf, len);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_decryptFinish(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	unsigned char output_text[128];
	int len = 0;
	
	int ret = 0;
	
    if ((ret = EVP_DecryptFinal_ex(cst -> ctx, output_text, &len)) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	unsigned char *pbuf = malloc(len);
	memcpy(pbuf, output_text, len);
	
	returnByteArray(cframe, pbuf, len);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_decryptSetTag(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	unsigned char *tag = input -> data;
	
	/* Set the tag */
    if (!EVP_CIPHER_CTX_ctrl(cst -> ctx, EVP_CTRL_GCM_SET_TAG, 16, tag))
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		cst -> ok = false;
		return RETURN_OK;
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_aes_gcm_decryptOK(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
	return_bool(cframe, cst -> ok);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_instance_cleanup(VFrame *cframe)
	{
	CipherState *cst;
	memcpy(&cst, getVariableContent(cframe, 0), sizeof(size_t));
	
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

INSTRUCTION_DEF op_rsa_oaep_init(VFrame *cframe)
	{
	LiveArray *input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 0)) -> content;
	
	unsigned char mode = getVariableContent(cframe, 1)[0];
	
	EVP_PKEY_CTX *ctx;
	EVP_PKEY *key = EVP_PKEY_new();
	
	//load the key
	BIO *biomem = BIO_new(BIO_s_mem());
	BIO_write(biomem, input -> data, input -> length);
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
	
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &ctx, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_oaep_encrypt(VFrame *cframe)
	{
	LiveArray *input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	EVP_PKEY_CTX *ctx;
	memcpy(&ctx, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t outlen = 0;
	
	/* Determine buffer length */
	if (EVP_PKEY_encrypt(ctx, NULL, &outlen, input -> data, input -> length) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	unsigned char *out = malloc(outlen);

	if (!out)
		{
		api -> throwException(cframe, "out of memory");
		return RETURN_OK;
		}

	if (EVP_PKEY_encrypt(ctx, out, &outlen, input -> data, input -> length) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		free(out);
		return RETURN_OK;
		}
	
	returnByteArray(cframe, out, outlen);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_oaep_decrypt(VFrame *cframe)
	{
	LiveArray *input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	EVP_PKEY_CTX *ctx;
	memcpy(&ctx, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t outlen = 0;
	
	/* Determine buffer length */
	if (EVP_PKEY_decrypt(ctx, NULL, &outlen, input -> data, input -> length) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	unsigned char *out = malloc(outlen);

	if (!out)
		{
		api -> throwException(cframe, "out of memory");
		return RETURN_OK;
		}

	if (EVP_PKEY_decrypt(ctx, out, &outlen, input -> data, input -> length) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		free(out);
		return RETURN_OK;
		}
	
	returnByteArray(cframe, out, outlen);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_oaep_cleanup(VFrame *cframe)
	{
	EVP_PKEY_CTX *ctx;
	memcpy(&ctx, getVariableContent(cframe, 0), sizeof(size_t));
	
	EVP_PKEY_CTX_free(ctx);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_pss_init(VFrame *cframe)
	{
	LiveArray *input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 0)) -> content;
	
	unsigned char mode = getVariableContent(cframe, 1)[0];
	
	EVP_PKEY_CTX *ctx;
	EVP_PKEY *key = EVP_PKEY_new();
	
	//load the key
	BIO *biomem = BIO_new(BIO_s_mem());
	BIO_write(biomem, input -> data, input -> length);
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
	
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &ctx, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_pss_sign(VFrame *cframe)
	{
	LiveArray *input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	EVP_PKEY_CTX *ctx;
	memcpy(&ctx, getVariableContent(cframe, 0), sizeof(size_t));
	
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
	EVP_DigestUpdate(mdctx, input -> data, input -> length);
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
	
	unsigned char *out = malloc(outlen);

	if (!out)
		{
		api -> throwException(cframe, "out of memory");
		return RETURN_OK;
		}

	if (EVP_PKEY_sign(ctx, out, &outlen, md_value, md_len) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		free(out);
		return RETURN_OK;
		}
	
	returnByteArray(cframe, out, outlen);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_pss_verify(VFrame *cframe)
	{
	EVP_PKEY_CTX *ctx;
	memcpy(&ctx, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	LiveArray *signed_input = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 2)) -> content;
	
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
	EVP_DigestUpdate(mdctx, input -> data, input -> length);
	EVP_DigestFinal_ex(mdctx, md_value, &md_len);
	EVP_MD_CTX_destroy(mdctx);
	
	// -- --
	
	int ret = EVP_PKEY_verify(ctx, signed_input -> data, signed_input -> length, md_value, md_len);
	
	if (ret < 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	return_bool(cframe, ret == 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_pss_cleanup(VFrame *cframe)
	{
	EVP_PKEY_CTX *ctx;
	memcpy(&ctx, getVariableContent(cframe, 0), sizeof(size_t));
	
	EVP_PKEY_CTX_free(ctx);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_generate_key(VFrame *cframe)
	{
	size_t keyLength = 0;
	copyHostInteger((unsigned char*) &keyLength, getVariableContent(cframe, 0), sizeof(size_t));
	
	int rc;
	
	RSA *rsa = RSA_new();
	BIGNUM *bn = BN_new();
	
	rc = BN_set_word(bn, RSA_F4);
	
	//poke the random number generator
	RAND_poll();
	
	if (RSA_generate_key_ex(rsa, keyLength, bn, NULL) != 1)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	//convert to pkey
	EVP_PKEY *pkey = EVP_PKEY_new();
	EVP_PKEY_set1_RSA(pkey, rsa);
	
	unsigned char *pp;
	unsigned char *outbuf;
	size_t mlen;
	
	// write public key to PKCS PEM
	BIO *biomem = BIO_new(BIO_s_mem());
	//rc = PEM_write_bio_RSAPublicKey(biomem, rsa); //TODO: should we be using this format instead? i.e. PKCS??? (and change the format for oaes input?)
	rc = PEM_write_bio_PUBKEY(biomem, pkey);
	mlen = BIO_get_mem_data(biomem, &pp);
	outbuf = malloc(mlen);
	BIO_read(biomem, outbuf, mlen);
	BIO_free(biomem);
	
	/* -- */
	LiveArray *newArray = malloc(sizeof(LiveArray));
	memset(newArray, '\0', sizeof(LiveArray));
	
	newArray -> data = outbuf;
	newArray -> length = mlen;
	
	newArray -> gtLink = charArrayGT;
	api -> incrementGTRefCount(newArray -> gtLink);
	newArray -> refi.ocm = cframe -> blocking -> instance;
	
	VVarLivePTR *ptrh = (VVarLivePTR*) ((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content) -> data;
	
	ptrh -> content = (unsigned char*) newArray;
	newArray -> refi.refCount ++;
	newArray -> refi.type = newArray -> gtLink -> typeLink;
	/* -- */
	
	// Write private key in PKCS PEM
	biomem = BIO_new(BIO_s_mem());
    rc = PEM_write_bio_PKCS8PrivateKey(biomem, pkey, NULL, NULL, 0, NULL, NULL);
	mlen = BIO_get_mem_data(biomem, &pp);
	outbuf = malloc(mlen);
	BIO_read(biomem, outbuf, mlen);
	BIO_free(biomem);
	
	/* -- */
	newArray = malloc(sizeof(LiveArray));
	memset(newArray, '\0', sizeof(LiveArray));
	
	newArray -> data = outbuf;
	newArray -> length = mlen;
	
	newArray -> gtLink = charArrayGT;
	api -> incrementGTRefCount(newArray -> gtLink);
	newArray -> refi.ocm = cframe -> blocking -> instance;
	
	ptrh = (VVarLivePTR*) ((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 2)) -> content) -> data;
	
	ptrh -> content = (unsigned char*) newArray;
	newArray -> refi.refCount ++;
	newArray -> refi.type = newArray -> gtLink -> typeLink;
	/* -- */
	
	BN_free(bn);
	EVP_PKEY_free(pkey);
	RSA_free(rsa);
	
	return_bool(cframe, true);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_rsa_convert_key(VFrame *cframe)
	{
	RSA *rsa = NULL;
	EVP_PKEY *pkey = EVP_PKEY_new();
	
	LiveArray *pubKeyIn = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 0)) -> content;
	LiveArray *priKeyIn = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	unsigned char inputType = getVariableContent(cframe, 2)[0];
	unsigned char outputType = getVariableContent(cframe, 5)[0];
	
	//read the key in to one of the above structures, depending on the type param
	
	bool fail = false;
	
	if (inputType == 1)
		{
		//PKCS
		BIO *biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, pubKeyIn -> data, pubKeyIn -> length);
		if (PEM_read_bio_PUBKEY(biomem, &pkey, 0, 0) == NULL) fail = true;
		BIO_free(biomem);
		
		if (fail)
			{
			EVP_PKEY_free(pkey);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		
		biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, priKeyIn -> data, priKeyIn -> length);
		if (PEM_read_bio_PrivateKey(biomem, &pkey, 0, 0) == NULL) fail = true;
		BIO_free(biomem);
		
		if (fail)
			{
			EVP_PKEY_free(pkey);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		
		//populate RSA key
		rsa = EVP_PKEY_get1_RSA(pkey);
		}
		else if (inputType == 2)
		{
		//RSA
		rsa = RSA_new();
		
		BIO *biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, pubKeyIn -> data, pubKeyIn -> length);
		if (PEM_read_bio_RSAPublicKey(biomem, &rsa, 0, 0) == NULL) fail = true;
		BIO_free(biomem);
		
		if (fail)
			{
			EVP_PKEY_free(pkey);
			RSA_free(rsa);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		
		biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, priKeyIn -> data, priKeyIn -> length);
		if (PEM_read_bio_RSAPrivateKey(biomem, &rsa, 0, 0) == NULL) fail = true;
		BIO_free(biomem);
		
		if (fail)
			{
			EVP_PKEY_free(pkey);
			RSA_free(rsa);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		
		//populate PKEY
		EVP_PKEY_set1_RSA(pkey, rsa);
		}
		else if (inputType == 4)
		{
		//ASN.1/DER
		rsa = RSA_new();
		
		//i2d_RSAPublicKey_bio i is internal, d is "der"
		BIO *biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, pubKeyIn -> data, pubKeyIn -> length);
		if (d2i_RSAPublicKey_bio(biomem, &rsa) == NULL) fail = true;
		BIO_free(biomem);
		
		if (fail)
			{
			EVP_PKEY_free(pkey);
			RSA_free(rsa);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		
		biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, priKeyIn -> data, priKeyIn -> length);
		if (d2i_RSAPrivateKey_bio(biomem, &rsa) == NULL) fail = true;
		BIO_free(biomem);
		
		if (fail)
			{
			EVP_PKEY_free(pkey);
			RSA_free(rsa);
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		
		//populate PKEY
		EVP_PKEY_set1_RSA(pkey, rsa);
		}
	
	//write out the keys
	
	unsigned char *pubKeyOut = NULL;
	size_t pubKeyOutLen = 0;
	
	unsigned char *priKeyOut = NULL;
	size_t priKeyOutLen = 0;
	
	unsigned char *pp;
	
	int rc = 0;
	
	if (outputType == 1)
		{
		BIO *biomem = BIO_new(BIO_s_mem());
		rc = PEM_write_bio_PUBKEY(biomem, pkey);
		pubKeyOutLen = BIO_get_mem_data(biomem, &pp);
		pubKeyOut = malloc(pubKeyOutLen);
		BIO_read(biomem, pubKeyOut, pubKeyOutLen);
		BIO_free(biomem);
		
		biomem = BIO_new(BIO_s_mem());
		rc = PEM_write_bio_PKCS8PrivateKey(biomem, pkey, NULL, NULL, 0, NULL, NULL);
		priKeyOutLen = BIO_get_mem_data(biomem, &pp);
		priKeyOut = malloc(priKeyOutLen);
		BIO_read(biomem, priKeyOut, priKeyOutLen);
		BIO_free(biomem);
		}
		else if (outputType == 2)
		{
		BIO *biomem = BIO_new(BIO_s_mem());
		rc = PEM_write_bio_RSAPublicKey(biomem, rsa);
		pubKeyOutLen = BIO_get_mem_data(biomem, &pp);
		pubKeyOut = malloc(pubKeyOutLen);
		BIO_read(biomem, pubKeyOut, pubKeyOutLen);
		BIO_free(biomem);
		
		biomem = BIO_new(BIO_s_mem());
		rc = PEM_write_bio_RSAPrivateKey(biomem, rsa, NULL, NULL, 0, NULL, NULL);
		priKeyOutLen = BIO_get_mem_data(biomem, &pp);
		priKeyOut = malloc(priKeyOutLen);
		BIO_read(biomem, priKeyOut, priKeyOutLen);
		BIO_free(biomem);
		}
		else if (outputType == 4)
		{
		BIO *biomem = BIO_new(BIO_s_mem());
		rc = i2d_RSAPublicKey_bio(biomem, rsa);
		pubKeyOutLen = BIO_get_mem_data(biomem, &pp);
		pubKeyOut = malloc(pubKeyOutLen);
		BIO_read(biomem, pubKeyOut, pubKeyOutLen);
		BIO_free(biomem);
		
		biomem = BIO_new(BIO_s_mem());
		rc = i2d_RSAPrivateKey_bio(biomem, rsa);
		priKeyOutLen = BIO_get_mem_data(biomem, &pp);
		priKeyOut = malloc(priKeyOutLen);
		BIO_read(biomem, priKeyOut, priKeyOutLen);
		BIO_free(biomem);
		}
	
	EVP_PKEY_free(pkey);
	RSA_free(rsa);
	
	/* -- */
	LiveArray *newArray = malloc(sizeof(LiveArray));
	memset(newArray, '\0', sizeof(LiveArray));
	
	newArray -> data = pubKeyOut;
	newArray -> length = pubKeyOutLen;
	
	newArray -> gtLink = charArrayGT;
	api -> incrementGTRefCount(newArray -> gtLink);
	newArray -> refi.ocm = cframe -> blocking -> instance;
	
	VVarLivePTR *ptrh = (VVarLivePTR*) ((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 3)) -> content) -> data;
	
	ptrh -> content = (unsigned char*) newArray;
	newArray -> refi.refCount ++;
	newArray -> refi.type = newArray -> gtLink -> typeLink;
	/* -- */
	
	/* -- */
	newArray = malloc(sizeof(LiveArray));
	memset(newArray, '\0', sizeof(LiveArray));
	
	newArray -> data = priKeyOut;
	newArray -> length = priKeyOutLen;
	
	newArray -> gtLink = charArrayGT;
	api -> incrementGTRefCount(newArray -> gtLink);
	newArray -> refi.ocm = cframe -> blocking -> instance;
	
	ptrh = (VVarLivePTR*) ((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 4)) -> content) -> data;
	
	ptrh -> content = (unsigned char*) newArray;
	newArray -> refi.refCount ++;
	newArray -> refi.type = newArray -> gtLink -> typeLink;
	/* -- */
	
	return_bool(cframe, true);
	
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
