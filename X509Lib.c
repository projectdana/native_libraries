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
static GlobalTypeLink *dateTimeGT = NULL;

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

static DanaEl* hexString(char* str, size_t len)
	{
	unsigned char* val = NULL;
	DanaEl* field = api -> makeArray(charArrayGT, len*2, &val);

	int j = 0;
	int i = 0;
	for (i = 0; i < len; i++)
		{
		char upperChar = (str[i] & 0xF0) >> 4;
		char lowerChar = str[i] & 0x0F;

		//ascii offsets
		if(upperChar < 10)
			upperChar += 48;
		else
			upperChar += 87;

		//ascii offsets
		if(lowerChar < 10)
			lowerChar += 48;
		else
			lowerChar += 87;
		
		val[j] = upperChar;
		val[j+1] = lowerChar;
		j+=2;
		}

	return field;
	}

static DanaEl* parseDate(const ASN1_TIME* time)
	{
	DanaEl* date = api -> makeData(dateTimeGT);

	struct tm timeVal;
	ASN1_TIME_to_tm(time, &timeVal);

	unsigned char* raw = api -> getDataContent(date);
	
	uint16 *year = (uint16*) &raw[0];
	unsigned char *month = &raw[2];
	unsigned char *day = &raw[3];
	unsigned char *hour = &raw[4];
	unsigned char *minute = &raw[5];
	unsigned char *second = &raw[6];

	timeVal.tm_year += 1900;
	timeVal.tm_mon += 1;

	size_t cYear = 0;
	copyToDanaInteger((unsigned char*) &cYear, (unsigned char*) &timeVal.tm_year, sizeof(timeVal.tm_year));
	
	*year = ((uint16*) &cYear)[(sizeof(cYear)/2)-1];
	*month = timeVal.tm_mon;
	*day = timeVal.tm_mday;
	
	*hour = timeVal.tm_hour;
	*minute = timeVal.tm_min;
	*second = timeVal.tm_sec;

	return date;
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

	ASN1_OBJECT *objK = NULL;
	const unsigned char* pkd;
	X509_PUBKEY* xpk = X509_get_X509_PUBKEY(cert);
	int pklen;
	X509_ALGOR* keyAlgo = NULL;
	X509_PUBKEY_get0_param(&objK, &pkd, &pklen, &keyAlgo, xpk);
	char abuf[512];
	int res = OBJ_obj2txt(abuf, sizeof abuf, keyAlgo->algorithm, 0);
	if (res > 0)
		{
		field = api -> makeArray(charArrayGT, strlen(abuf), &val);
		memcpy(val, abuf, strlen(abuf));
		api -> setDataFieldEl(result, 3, field);
		}

	const ASN1_BIT_STRING* sigBits = NULL;
	const X509_ALGOR* sigAlgo = NULL;
	X509_get0_signature(&sigBits, &sigAlgo, cert);
	char tbuf[512];
	res = OBJ_obj2txt(tbuf, sizeof tbuf, sigAlgo->algorithm, 0);
	if (res > 0)
		{
		field = api -> makeArray(charArrayGT, strlen(tbuf), &val);
		memcpy(val, tbuf, strlen(tbuf));
		api -> setDataFieldEl(result, 7, field);
		}
	size_t len = ASN1_STRING_length(sigBits);
	const unsigned char* strData = ASN1_STRING_get0_data(sigBits);
	field = hexString((char*) strData, len);
	api -> setDataFieldEl(result, 8, field);

	//OpenSSL time format as a string is "Dec 19 00:00:00 2022 GMT"
	const ASN1_TIME* notBefore = X509_get0_notBefore(cert);
	if (notBefore != NULL)
		{
		field = parseDate(notBefore);
		api -> setDataFieldEl(result, 5, field);
		}

	const ASN1_TIME* notAfter = X509_get0_notAfter(cert);
	if (notAfter != NULL)
		{
		field = parseDate(notAfter);
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
	dateTimeGT = api -> resolveGlobalTypeMapping(getTypeDefinition("DateTime"));
	
	setInterfaceFunction("parse", op_parse);
	setInterfaceFunction("write", op_write);
	
	return getPublicInterface();
	}

void unload()
	{
	cleanup_openssl();
	
	api -> decrementGTRefCount(charArrayGT);
	}
