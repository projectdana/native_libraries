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

static X509* parseCertificate(unsigned char *data, size_t len)
	{
	BIO *biomem = BIO_new(BIO_s_mem());
	BIO_write(biomem, data, len);
	X509* cert = PEM_read_bio_X509(biomem, NULL, NULL, NULL);
	if (cert == NULL)
		{
		return NULL;
		}

	BIO_free(biomem);
	//X509_free(cert);
	
	return cert;
	}

static STACK_OF(X509)* parseChain(DanaEl* array)
	{
	STACK_OF(X509) *certStack;
	
	certStack = sk_X509_new_null();
	
	int len = api -> getArrayLength(array);
	int i = 0;
	for (i = 0; i < len; i++)
		{
		DanaEl* qd = api -> getArrayCellEl(array, i);
		DanaEl *qa = api -> getDataFieldEl(qd, 0);
		
		BIO *biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, api -> getArrayContent(qa), api -> getArrayLength(qa));
		X509* cert = PEM_read_bio_X509(biomem, NULL, NULL, NULL);
		if (cert == NULL)
			{
			return NULL;
			}

		sk_X509_push(certStack, cert);

		BIO_free(biomem);
		//X509_free(cert);
		}
	
	return certStack;
	}

static void cleanupStack(STACK_OF(X509) *certStack)
	{
	unsigned int len = sk_X509_num(certStack);
	
	size_t i = 0;
	for (i = 0; i < len; i++)
		{
		X509 *cert = sk_X509_value(certStack, i);
		X509_free(cert);
		}
	
	sk_X509_free(certStack);
	}

// -- cert store --

INSTRUCTION_DEF op_create_cert_store(FrameData *cframe)
	{
	X509_STORE *store = X509_STORE_new();
	
	api -> returnRaw(cframe, (unsigned char*) &store, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_certificate(FrameData *cframe)
	{
	X509_STORE *store;
	memcpy(&store, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* certArray = api -> getParamEl(cframe, 1);
	
	X509 *cert = parseCertificate(api -> getArrayContent(certArray), api -> getArrayLength(certArray));
	
	X509_STORE_add_cert(store, cert);
	
	X509_free(cert);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_load_cert_location(FrameData *cframe)
	{
	X509_STORE *store;
	memcpy(&store, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	char *path = x_getParam_char_array(api, cframe, 1);
	
	X509_STORE_load_locations(store, path, NULL);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_free_cert_store(FrameData *cframe)
	{
	X509_STORE *store;
	memcpy(&store, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	X509_STORE_free(store);
	
	return RETURN_OK;
	}

// -- contexts --

INSTRUCTION_DEF op_create_context(FrameData *cframe)
	{
	bool serverMode = api -> getParamRaw(cframe, 0)[0];
	
    const SSL_METHOD *method;
    SSL_CTX *ctx;
	
	if (serverMode)
		{
		method = TLS_server_method();
		}
		else
		{
		method = TLS_client_method();
		}

    ctx = SSL_CTX_new(method);
    if (!ctx) {
		perror("Unable to create SSL context");
		ERR_print_errors_fp(stderr);
		exit(EXIT_FAILURE);
		}
	
	/*
	if (serverMode)
		{
		SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, NULL);
		//SSL_CTX_set1_curves_list(ctx, "P-521:P-348:P-256");
		}
	*/
	
	//const long flags = SSL_OP_NO_SSLv2 | SSL_OP_NO_SSLv3 | SSL_OP_NO_COMPRESSION;
	//SSL_CTX_set_options(ctx, flags);
	
	api -> returnRaw(cframe, (unsigned char*) &ctx, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_certificate(FrameData *cframe)
	{
	SSL_CTX *ctx;
	memcpy(&ctx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl *array = api -> getParamEl(cframe, 1);
	
	DanaEl *arrayKey = api -> getParamEl(cframe, 2);
	
	// -- certificate is provided as a base64-encoded raw PEM, so we convert to X509 first --
	BIO *biomem = BIO_new(BIO_s_mem());
	BIO_write(biomem, api -> getArrayContent(array), api -> getArrayLength(array));
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
	BIO_write(biomem, api -> getArrayContent(arrayKey), api -> getArrayLength(arrayKey));

	EVP_PKEY* pkey = NULL;
	PEM_read_bio_PrivateKey(biomem, &pkey, 0, 0);

	BIO_free(biomem);

	RSA* rsa = EVP_PKEY_get1_RSA(pkey);
	
	if (SSL_CTX_use_RSAPrivateKey(ctx, rsa) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	if (SSL_CTX_check_private_key(ctx) <= 0)
		{
		api -> throwException(cframe, ERR_error_string(ERR_get_error(), NULL));
		return RETURN_OK;
		}
	
	RSA_free(rsa);
	
	unsigned char res = 1;
	
	api -> returnRaw(cframe, &res, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_certificate_chain(FrameData *cframe)
	{
	SSL_CTX *ctx;
	memcpy(&ctx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* array = api -> getParamEl(cframe, 1);
	size_t alen = api -> getArrayLength(array);
	
	int i = 0;
	for (i = 0; i < alen; i++)
		{
		DanaEl* qd = api -> getArrayCellEl(array, i);
		
		DanaEl *qa = api -> getDataFieldEl(qd, 0);
		
		BIO *biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, api -> getArrayContent(qa), api -> getArrayLength(qa));
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
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_cipher_set(FrameData *cframe)
	{
	SSL_CTX *ctx;
	memcpy(&ctx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	size_t set = api -> getParamInt(cframe, 1);
	
	if (set == 0)
		{
		//default
		SSL_CTX_set_cipher_list(ctx, SSL_DEFAULT_CIPHER_LIST);
		SSL_CTX_set_ciphersuites(ctx, TLS_DEFAULT_CIPHERSUITES);
		}
		else if (set == 1)
		{
		//all
		SSL_CTX_set_cipher_list(ctx, "ALL");
		SSL_CTX_set_ciphersuites(ctx, "ALL");
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_free_context(FrameData *cframe)
	{
	SSL_CTX *ctx;
	memcpy(&ctx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	SSL_CTX_free(ctx);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_make_ssl(FrameData *cframe)
	{
	SSL_CTX *ctx;
	memcpy(&ctx, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	SSL *ssl;
	
	ssl = SSL_new(ctx);
	
	if (ssl == NULL)
		{
		api -> throwException(cframe, "make SSL failed");
		return RETURN_OK;
		}
	
	api -> returnRaw(cframe, (unsigned char*) &ssl, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_accept(FrameData *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	size_t xs = 0;
	memcpy(&xs, api -> getParamRaw(cframe, 1), sizeof(size_t));
	
	int socket = xs;
	
	SSL_set_fd(ssl, socket);
	
	unsigned char res = 0;
	
	if (SSL_accept(ssl) <= 0) {
		int errC = ERR_get_error();
		
		if (errC != 0)
			{
			api -> throwException(cframe, ERR_error_string(errC, NULL));
			}
		
		return RETURN_OK;
		}
		else
		{
		res = 1;
		}
	
	//return boolean
	api -> returnRaw(cframe, &res, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_connect(FrameData *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	size_t xs = 0;
	memcpy(&xs, api -> getParamRaw(cframe, 1), sizeof(size_t));
	
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
	api -> returnRaw(cframe, &res, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_peer_cert(FrameData *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	X509 *cert = SSL_get_peer_certificate(ssl);
	
	BIO *biomem = BIO_new(BIO_s_mem());
	PEM_write_bio_X509(biomem, cert);
	
	unsigned char *pbuf = NULL;
	size_t len = BIO_get_mem_data(biomem, &pbuf);
	
	DanaEl* array = api -> makeArray(charArrayGT, len);
	memcpy(api -> getArrayContent(array), pbuf, len);
	
	X509_free(cert);
	BIO_free(biomem);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_peer_cert_chain(FrameData *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	STACK_OF(X509) *sk = SSL_get_peer_cert_chain(ssl);
	
	unsigned int len = sk_X509_num(sk);
	
	if (len > 0)
		{
		//allocate a String array of length "len"
		
		DanaEl* newArray = api -> makeArray(stringArrayGT, len);
		
		size_t i = 0;
		for (i = 0; i < len; i++)
			{
			// --
			
			X509 *cert = sk_X509_value(sk, i);
			
			BIO *biomem = BIO_new(BIO_s_mem());
			PEM_write_bio_X509(biomem, cert);
			
			unsigned char *pbuf = NULL;
			size_t len = BIO_get_mem_data(biomem, &pbuf);
			
			// --
			
			DanaEl* newData = api -> makeData(stringItemGT);
			
			DanaEl* itemArray = api -> makeArray(charArrayGT, len);
			memcpy(api -> getArrayContent(itemArray), pbuf, len);
			
			api -> setDataFieldEl(newData, 0, itemArray);
			
			// -- reference in array --
			api -> setArrayCellEl(newArray, i, newData);
			
			// --
			
			BIO_free(biomem);
			}
		
		api -> returnEl(cframe, newArray);
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_verify_certificate(FrameData *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	X509_STORE *store;
	memcpy(&store, api -> getParamRaw(cframe, 1), sizeof(size_t));
	
	//get and parse both the certificate and chain, if provided
	
	DanaEl* verifyResult = api -> getParamEl(cframe, 2);
	
	DanaEl* certArray = api -> getParamEl(cframe, 3);
	
	DanaEl* certChainArray = api -> getParamEl(cframe, 4);
		
	X509 *cert = NULL;
	STACK_OF(X509) *certStack = NULL;
	
	cert = parseCertificate(api -> getArrayContent(certArray), api -> getArrayLength(certArray));
	
	if (certChainArray != NULL)
		certStack = parseChain(certChainArray);
	
	X509_STORE_CTX *ctx = X509_STORE_CTX_new();
	
	if (!ctx)
		{
		api -> throwException(cframe, "internal SSL error: failed to create STORE CTX");
		return RETURN_OK;
		}
	
	if (X509_STORE_CTX_init(ctx, store, cert, certStack) != 1)
		{
		api -> throwException(cframe, "internal SSL error: failed to init STORE CTX");
		X509_STORE_CTX_free(ctx);
		return RETURN_OK;
		}
	
	//X509_verify_cert: If a complete chain can be built and validated this function returns 1, otherwise it return zero, in exceptional circumstances it can also return a negative code.
	// - If the function fails additional error information can be obtained by examining ctx using, for example X509_STORE_CTX_get_error().
	
	size_t core_result = 0;
	
	int rc = X509_verify_cert(ctx);
	
	if (rc != 1)
		{
		api -> setDataFieldInt(verifyResult, 0, 1);
		}
	
	if (rc != 1) {
		//find the certificate causing the failure
		X509 *badCert = X509_STORE_CTX_get_current_cert(ctx);
		
		const char *failReason = X509_verify_cert_error_string(X509_STORE_CTX_get_error(ctx));
		
		/*
		X509_NAME *certsubject = X509_get_subject_name(badCert);
		BIO *biomem = BIO_new(BIO_s_mem());
		X509_NAME_print_ex(biomem, certsubject, 0, XN_FLAG_ONELINE & ~ASN1_STRFLGS_ESC_MSB); //~ASN1_STRFLGS_ESC_MSB is for UTF8 terminals
		
		unsigned char *pbuf = NULL;
		size_t len = BIO_get_mem_data(biomem, &pbuf);
		
		char *strn = malloc(len+1);
		memset(strn, '\0', len+1);
		memcpy(strn, pbuf, len);
		
		printf("%s\n", failReason);
		printf("%s\n", strn);
		
		free(strn);
		
		BIO_free(biomem);
		*/
		
		//find which certificate failed on the verify
		
		if (badCert == cert)
			{
			api -> setDataFieldEl(verifyResult, 1, certArray);
			}
			else if (certStack != NULL)
			{
			unsigned int len = sk_X509_num(certStack);
			
			size_t i = 0;
			for (i = 0; i < len; i++)
				{
				X509 *cert = sk_X509_value(certStack, i);
				
				if (cert == badCert)
					{
					DanaEl *cfd = api -> getArrayCellEl(certChainArray, i);
					DanaEl *cfa = api -> getDataFieldEl(cfd, 0);
					
					api -> setDataFieldEl(verifyResult, 1, cfa);
					
					break;
					}
				}
			}
			else
			{
			//TODO: it's something from the CertStore; get the raw certificate... (?)
			}
		
		//get a string reason for verify failure, if any
		DanaEl* reason = api -> makeArray(charArrayGT, strlen(failReason));
		memcpy(api -> getArrayContent(reason), failReason, strlen(failReason));
		
		api -> setDataFieldEl(verifyResult, 2, reason);
		}
	
	if (certStack != NULL)
		{
		cleanupStack(certStack);
		}
	
	X509_free(cert);
	
	X509_STORE_CTX_free(ctx);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_write(FrameData *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* array = api -> getParamEl(cframe, 1);
	
	int sz = 0;
	
	if (array != NULL)
		{
		sz = SSL_write(ssl, (char*) api -> getArrayContent(array), api -> getArrayLength(array));
		}
	
	//return # bytes written
	
	if (sz > 0)
		{
		api -> returnInt(cframe, sz);
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_read(FrameData *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	size_t len = api -> getParamInt(cframe, 1);
	
	//read data
	
	DanaEl* array = api -> makeArray(charArrayGT, len);
	unsigned char* cnt = api -> getArrayContent(array);
	
	if (array == NULL)
		{
		len = 0;
		return RETURN_OK;
		}
	
	int amt = 1;
	size_t totalAmt = 0;
	
	while ((len > 0) && (amt != 0))
		{
		amt = SSL_read(ssl, (char*) (cnt + totalAmt), len);
		
		if (amt < 0)
			{
			int errC = ERR_get_error();
			if (errC != SSL_ERROR_ZERO_RETURN && errC != 0)
				{
				api -> throwException(cframe, ERR_error_string(errC, NULL));
				}
			
			api -> destroyArray(array);
			return RETURN_OK;
			}
		
		totalAmt += amt;
		len -= amt;
		}
	
	if (totalAmt > 0)
		{
		api -> setArrayLength(array, totalAmt);
		api -> returnEl(cframe, array);
		}
		else
		{
		api -> destroyArray(array);
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_close_ssl(FrameData *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	SSL_shutdown(ssl);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_free_ssl(FrameData *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
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
	
	setInterfaceFunction("createCertStore", op_create_cert_store);
	setInterfaceFunction("addCertificate", op_add_certificate);
	setInterfaceFunction("loadLocation", op_load_cert_location);
	setInterfaceFunction("freeCertStore", op_free_cert_store);
	
	setInterfaceFunction("createContext", op_create_context);
	setInterfaceFunction("setCertificate", op_set_certificate);
	setInterfaceFunction("setCertificateChain", op_set_certificate_chain);
	setInterfaceFunction("setCipherSet", op_set_cipher_set);
	setInterfaceFunction("freeContext", op_free_context);
	
	setInterfaceFunction("makeSSL", op_make_ssl);
	setInterfaceFunction("accept", op_accept);
	setInterfaceFunction("connect", op_connect);
	setInterfaceFunction("getPeerCertificate", op_get_peer_cert);
	setInterfaceFunction("getPeerCertChain", op_get_peer_cert_chain);
	setInterfaceFunction("verifyCertificate", op_verify_certificate);
	setInterfaceFunction("write", op_write);
	setInterfaceFunction("read", op_read);
	setInterfaceFunction("closeSSL", op_close_ssl);
	setInterfaceFunction("freeSSL", op_free_ssl);
	
	return getPublicInterface();
	}

void unload()
	{
	cleanup_openssl();
	
	api -> decrementGTRefCount(charArrayGT);
	api -> decrementGTRefCount(stringArrayGT);
	api -> decrementGTRefCount(stringItemGT);
	}
