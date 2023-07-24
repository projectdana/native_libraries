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
static GlobalTypeLink *x509CertificateGT = NULL;

static DanaEl* nameToDanaString(X509_NAME* name)
	{
	BIO *tmpbio = BIO_new(BIO_s_mem());
	X509_NAME_print_ex(tmpbio, name, 0, XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB); //~ASN1_STRFLGS_ESC_MSB is for UTF8 terminals
	
	unsigned char *pbuf = NULL;
	size_t len = BIO_get_mem_data(tmpbio, &pbuf);

	unsigned char* val = NULL;
	DanaEl* field = api -> makeArray(charArrayGT, len, &val);
	memcpy(val, pbuf, len);
	
	BIO_free(tmpbio);

	return field;
	}

INSTRUCTION_DEF op_parse(FrameData *cframe)
	{
	DanaEl *array = api -> getParamEl(cframe, 0);

	BIO *biomem = BIO_new(BIO_s_mem());
	BIO_write(biomem, api -> getArrayContent(array), api -> getArrayLength(array));
	X509* cert = PEM_read_bio_X509(biomem, NULL, NULL, NULL);
	if (cert == NULL)
		{
		api -> throwException(cframe, "parsing certificate failed");
		return RETURN_OK;
		}
	
	DanaEl* result = api -> makeData(x509CertificateGT);

	DanaEl* field = NULL;
	X509_NAME* name = NULL;

	ASN1_INTEGER* serialNumber = X509_get_serialNumber(cert);
	const BIGNUM* bnser = ASN1_INTEGER_to_BN(serialNumber, NULL);
	char* serialHex = BN_bn2hex(bnser);
	unsigned char* val = NULL;
	field = api -> makeArray(charArrayGT, strlen(serialHex), &val);
	memcpy(val, serialHex, strlen(serialHex));
	OPENSSL_free(serialHex);
	api -> setDataFieldEl(result, 0, field);

	name = X509_get_issuer_name(cert);
	field = nameToDanaString(name);
	api -> setDataFieldEl(result, 1, field);
	
	name = X509_get_subject_name(cert);
	field = nameToDanaString(name);
	api -> setDataFieldEl(result, 2, field);

	EVP_PKEY* key = X509_get_pubkey(cert);
	biomem = BIO_new(BIO_s_mem());
	EVP_PKEY_free(key);
	OSSL_ENCODER_CTX* encoder = OSSL_ENCODER_CTX_new_for_pkey(key, OSSL_KEYMGMT_SELECT_PUBLIC_KEY, "PEM", NULL, NULL);
	OSSL_ENCODER_to_bio(encoder, biomem);
	OSSL_ENCODER_CTX_free(encoder);
	unsigned char *pp = NULL;
	size_t mlen = BIO_get_mem_data(biomem, &pp);
	val = NULL;
	field = api -> makeArray(charArrayGT, mlen, &val);
	BIO_read(biomem, val, mlen);
	BIO_free(biomem);
	api -> setDataFieldEl(result, 4, field);
	field = api -> makeArray(charArrayGT, strlen("RSA"), &val);
	memcpy(val, "RSA", strlen("RSA"));
	api -> setDataFieldEl(result, 3, field);

	const ASN1_BIT_STRING* sigBits = NULL;
	const X509_ALGOR* sigAlgo = NULL;
	X509_get0_signature(&sigBits, &sigAlgo, cert);
	char tbuf[512];
	int res = OBJ_obj2txt(tbuf, sizeof tbuf, sigAlgo->algorithm, 0);
	if (res > 0)
		{
		field = api -> makeArray(charArrayGT, strlen(tbuf), &val);
		memcpy(val, tbuf, strlen(tbuf));
		api -> setDataFieldEl(result, 7, field);
		}
	const ASN1_OBJECT *obj = NULL;
	int type;
	const void* value;
	X509_ALGOR_get0(&obj, &type, &value, sigAlgo);
	biomem = BIO_new(BIO_s_mem());
	//i2d_X509_SIG(biomem, cert);
	//X509_signature_print(biomem, sigAlgo, sigBits);
	ASN1_STRING_print_ex(biomem, sigBits, ASN1_STRFLGS_RFC2253);
	mlen = BIO_get_mem_data(biomem, &pp);
	field = api -> makeArray(charArrayGT, mlen, &val);
	BIO_read(biomem, val, mlen);
	BIO_free(biomem);
	api -> setDataFieldEl(result, 8, field);

	const ASN1_TIME* notBefore = X509_get0_notBefore(cert);
	if (notBefore != NULL)
		{
		biomem = BIO_new(BIO_s_mem());
		ASN1_TIME_print(biomem, notBefore);
		mlen = BIO_get_mem_data(biomem, &pp);
		field = api -> makeArray(charArrayGT, mlen, &val);
		BIO_read(biomem, val, mlen);
		BIO_free(biomem);
		api -> setDataFieldEl(result, 5, field);
		}

	const ASN1_TIME* notAfter = X509_get0_notAfter(cert);
	if (notAfter != NULL)
		{
		biomem = BIO_new(BIO_s_mem());
		ASN1_TIME_print(biomem, notAfter);
		mlen = BIO_get_mem_data(biomem, &pp);
		field = api -> makeArray(charArrayGT, mlen, &val);
		BIO_read(biomem, val, mlen);
		BIO_free(biomem);
		api -> setDataFieldEl(result, 6, field);
		}

	X509_free(cert);

	api -> returnEl(cframe, result);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_write(FrameData* cframe)
	{
	
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
	x509CertificateGT = api -> resolveGlobalTypeMapping(getTypeDefinition("X509Certificate"));
	
	setInterfaceFunction("parse", op_parse);
	setInterfaceFunction("write", op_write);
	
	return getPublicInterface();
	}

void unload()
	{
	cleanup_openssl();
	
	api -> decrementGTRefCount(charArrayGT);
	}
