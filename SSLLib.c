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
static GlobalTypeLink *stringArrayGT = NULL;
static GlobalTypeLink *stringItemGT = NULL;

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
	ptrh -> typeLink = array -> gtLink -> typeLink;
	}

static LiveArray* makeStringArray(unsigned char *str, size_t len, DanaComponent *owner)
	{
	LiveArray *result = malloc(sizeof(LiveArray));
	memset(result, '\0', sizeof(LiveArray));
	result -> refi.ocm = owner;
	result -> gtLink = charArrayGT;
	api -> incrementGTRefCount(result -> gtLink);
	
	result -> data = malloc(len);
	memcpy(result -> data, str, len);
	result -> length = len;
	
	result -> refi.type = result -> gtLink -> typeLink;
	
	return result;
	}

INSTRUCTION_DEF op_create_context(VFrame *cframe)
	{
	bool serverMode = getVariableContent(cframe, 0)[0];
	
    const SSL_METHOD *method;
    SSL_CTX *ctx;
	
	if (serverMode)
		method = SSLv23_server_method();
		else
		method = SSLv23_client_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
		}
	
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &ctx, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_certificate(VFrame *cframe)
	{
	SSL_CTX *ctx;
	memcpy(&ctx, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	LiveArray *arrayKey = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 2)) -> content;
	
	// -- certificate is provided as a base64-encoded raw PEM, so we convert to X509 first --
	BIO *biomem = BIO_new(BIO_s_mem());
	BIO_write(biomem, array -> data, array -> length);
	X509* cert = PEM_read_bio_X509(biomem, NULL, NULL, NULL);
	if (cert == NULL)
		{
		api -> throwException(cframe, "parsing certificate failed");
		return RETURN_OK;
		}

	//pass into SSL
    if (SSL_CTX_use_certificate(ctx, cert) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}

	BIO_free(biomem);
	X509_free(cert);
	
	// -- we assume private key is provided as a PKCS1 encoded key file --
	// (note that newer formats may use PKCS8)
	
	biomem = BIO_new(BIO_s_mem());
	BIO_write(biomem, arrayKey -> data, arrayKey -> length);

	EVP_PKEY* pkey = NULL;
	PEM_read_bio_PrivateKey(biomem, &pkey, 0, 0);

	BIO_free(biomem);

	RSA* rsa = EVP_PKEY_get1_RSA(pkey);
	
	SSL_CTX_use_RSAPrivateKey(ctx, rsa);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_certificate_chain(VFrame *cframe)
	{
	SSL_CTX *ctx;
	memcpy(&ctx, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	VVarLivePTR *nxt = (VVarLivePTR*) array -> data;
	for (int i = 0; i < array -> length; i++)
		{
		LiveData *qd = (LiveData*) nxt -> content;
		LiveArray *qa = (LiveArray*) ((VVarLivePTR*) qd -> data) -> content;
		
		BIO *biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, qa -> data, qa -> length);
		X509* cert = PEM_read_bio_X509(biomem, NULL, NULL, NULL);
		if (cert == NULL)
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}

		//pass into SSL
		if (SSL_CTX_add1_chain_cert(ctx, cert) <= 0)
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}

		BIO_free(biomem);
		X509_free(cert);
		
		nxt ++;
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_free_context(VFrame *cframe)
	{
	SSL_CTX *ctx;
	memcpy(&ctx, getVariableContent(cframe, 0), sizeof(size_t));
	
	SSL_CTX_free(ctx);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_make_ssl(VFrame *cframe)
	{
	SSL_CTX *ctx;
	memcpy(&ctx, getVariableContent(cframe, 0), sizeof(size_t));
	
	SSL *ssl;
	
	ssl = SSL_new(ctx);
	
	if (ssl == NULL)
		{
		printf("::make SSL failed::\n");
		}
	
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &ssl, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_verify_mode(VFrame *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, getVariableContent(cframe, 0), sizeof(size_t));
	
	unsigned char mode = getVariableContent(cframe, 1)[0];
	
	unsigned char res = 1;
	
	if (mode == 0x0)
		{
		SSL_set_verify(ssl, SSL_VERIFY_NONE, NULL);
		}
		else if (mode == 0x1)
		{
		SSL_set_verify(ssl, SSL_VERIFY_PEER, NULL);
		SSL_set_verify_depth(ssl, 4);
		
		const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
		SSL_set_options(ssl, flags);
		}
	
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(unsigned char));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_verify_result(VFrame *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, getVariableContent(cframe, 0), sizeof(size_t));
	
	unsigned char res = 0;
	
	if (SSL_get_verify_result(ssl) == X509_V_OK)
		res = 1;
	
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(unsigned char));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_accept(VFrame *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t xs = 0;
	memcpy(&xs, getVariableContent(cframe, 1), sizeof(size_t));
	
	int socket = xs;
	
	SSL_set_fd(ssl, socket);
	
	unsigned char res = 0;
	
	if (SSL_accept(ssl) <= 0) {
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
		else
		{
		res = 1;
		}
	
	//return boolean
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(unsigned char));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_connect(VFrame *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t xs = 0;
	memcpy(&xs, getVariableContent(cframe, 1), sizeof(size_t));
	
	int socket = xs;
	
	SSL_set_fd(ssl, socket);
	
	unsigned char res = 0;
	
	if (SSL_connect(ssl) <= 0) {
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
		else
		{
		res = 1;
		}
	
	//return boolean
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(unsigned char));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_peer_cert(VFrame *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, getVariableContent(cframe, 0), sizeof(size_t));
	
	X509 *cert = SSL_get_peer_certificate(ssl);
	
	BIO *biomem = BIO_new(BIO_s_mem());
	PEM_write_bio_X509(biomem, cert);
	
	unsigned char *pbuf = NULL;
	size_t len = BIO_get_mem_data(biomem, &pbuf);
	
	return_byte_array(cframe, api, pbuf, len);
	
	X509_free(cert);
	BIO_free(biomem);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_peer_cert_chain(VFrame *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, getVariableContent(cframe, 0), sizeof(size_t));
	
	STACK_OF(X509) *sk = SSL_get_peer_cert_chain(ssl);
	
	unsigned int len = sk_X509_num(sk);
	
	if (len > 0)
		{
		DanaComponent *dataOwner = cframe -> instance;
		
		//allocate a String array of length "len"
		
		LiveArray *newArray = malloc(sizeof(LiveArray));
		memset(newArray, '\0', sizeof(LiveArray));
		
		newArray -> refi.ocm = dataOwner;
		newArray -> gtLink = stringArrayGT;
		api -> incrementGTRefCount(newArray -> gtLink);
		newArray -> data = malloc(sizeof(VVarLivePTR) * len);
		memset(newArray -> data, '\0', sizeof(VVarLivePTR) * len);
		newArray -> length = len;
		newArray -> refi.type = newArray -> gtLink -> typeLink;
		
		for (unsigned int i=0; i < len; i++)
			{
			// --
			
			X509 *cert = sk_X509_value(sk, i);
			
			BIO *biomem = BIO_new(BIO_s_mem());
			PEM_write_bio_X509(biomem, cert);
			
			unsigned char *pbuf = NULL;
			size_t len = BIO_get_mem_data(biomem, &pbuf);
			
			// --
			
			LiveData *newData = malloc(sizeof(LiveData));
			memset(newData, '\0', sizeof(LiveData));
			newData -> refi.ocm = dataOwner;
			newData -> gtLink = stringItemGT;
			api -> incrementGTRefCount(newData -> gtLink);
			
			newData -> data = malloc(sizeof(VVarLivePTR));
			memset(newData -> data, '\0', sizeof(VVarLivePTR));
			VVarLivePTR *ptrh = (VVarLivePTR*) newData -> data;
			
			LiveArray *itemArray = makeStringArray(pbuf, len, dataOwner);
			
			ptrh -> content = (unsigned char*) itemArray;
			ptrh -> typeLink = itemArray -> gtLink -> typeLink;
			itemArray -> refi.refCount ++;
			
			// -- reference in array --
			VVarLivePTR *ptrhA = (VVarLivePTR*) (&newArray -> data[sizeof(VVarLivePTR) * i]);
			ptrhA -> content = (unsigned char*) newData;
			ptrhA -> typeLink = newData -> gtLink -> typeLink;
			newData -> refi.refCount ++;
			newData -> refi.type = newData -> gtLink -> typeLink;
			
			// --
			
			return_byte_array(cframe, api, pbuf, len);
			
			X509_free(cert);
			BIO_free(biomem);
			}
		
		newArray -> refi.refCount = 1;
		
		VVarLivePTR *ptrh = (VVarLivePTR*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
		
		ptrh -> content = (unsigned char*) newArray;
		ptrh -> typeLink = newArray -> gtLink -> typeLink;
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_write(VFrame *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	int sz = 0;
	
	if (array != NULL)
		sz = SSL_write(ssl, (char*) array -> data, array -> length);
	
	//return # bytes written
	
	if (sz > 0)
		{
		size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
		copyHostInteger((unsigned char*) result, (unsigned char*) &sz, sizeof(size_t));
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_read(VFrame *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t len = 0;
	copyHostInteger((unsigned char*) &len, getVariableContent(cframe, 1), sizeof(size_t));
	
	//read data
	
	unsigned char *pbuf = malloc(len);
	
	if (pbuf == NULL)
		{
		len = 0;
		}
	
	int amt = 1;
	size_t totalAmt = 0;
	
	while ((len > 0) && (amt != 0))
		{
		amt = SSL_read(ssl, (char*) (pbuf+totalAmt), len);
		
		if (amt < 0)
			{
			api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
			return RETURN_OK;
			}
		
		totalAmt += amt;
		len -= amt;
		}
	
	if (totalAmt > 0)
		{
		returnByteArray(cframe, pbuf, totalAmt);
		}
		else
		{
		free(pbuf);
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_close_ssl(VFrame *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, getVariableContent(cframe, 0), sizeof(size_t));
	
	SSL_shutdown(ssl);
	SSL_free(ssl);
	
	return RETURN_OK;
	}

void init_openssl()
	{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
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
	stringArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("String[]"));
	stringItemGT = api -> resolveGlobalTypeMapping(getTypeDefinition("String"));
	
	setInterfaceFunction("createContext", op_create_context);
	setInterfaceFunction("setCertificate", op_set_certificate);
	setInterfaceFunction("setCertificateChain", op_set_certificate_chain);
	setInterfaceFunction("freeContext", op_free_context);
	
	setInterfaceFunction("makeSSL", op_make_ssl);
	
	setInterfaceFunction("accept", op_accept);
	setInterfaceFunction("connect", op_connect);
	setInterfaceFunction("getPeerCertificate", op_get_peer_cert);
	setInterfaceFunction("getPeerCertChain", op_get_peer_cert_chain);
	setInterfaceFunction("setVerifyMode", op_set_verify_mode);
	setInterfaceFunction("getResultResult", op_get_verify_result);
	setInterfaceFunction("write", op_write);
	setInterfaceFunction("read", op_read);
	
	setInterfaceFunction("closeSSL", op_close_ssl);
	
	return getPublicInterface();
	}

void unload()
	{
	cleanup_openssl();
	
	api -> decrementGTRefCount(charArrayGT);
	api -> decrementGTRefCount(stringArrayGT);
	api -> decrementGTRefCount(stringItemGT);
	}
