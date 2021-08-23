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

static LiveArray* makeByteArray(VFrame *f, size_t len)
	{
	LiveArray *array = malloc(sizeof(LiveArray)+len);
	memset(array, '\0', sizeof(LiveArray)+len);
	
	array -> data = ((unsigned char*) array) + sizeof(LiveArray);
	array -> length = len;
	
	array -> gtLink = charArrayGT;
	api -> incrementGTRefCount(array -> gtLink);
	array -> refi.ocm = f -> blocking -> instance;
	
	array -> refi.type = array -> gtLink -> typeLink;
	
	return array;
	}

static void returnArray(VFrame *f, LiveArray *array)
	{
	array -> refi.refCount ++;
	
	VVarLivePTR *ptrh = (VVarLivePTR*) &f -> localsData[((DanaType*) f -> localsDef) -> fields[0].offset];
	ptrh -> content = (unsigned char*) array;
	}

static LiveArray* makeStringArray(unsigned char *str, size_t len, DanaComponent *owner)
	{
	LiveArray *result = malloc(sizeof(LiveArray)+len);
	memset(result, '\0', sizeof(LiveArray));
	result -> refi.ocm = owner;
	result -> gtLink = charArrayGT;
	api -> incrementGTRefCount(result -> gtLink);
	
	result -> data = ((unsigned char*) result) + sizeof(LiveArray);
	memcpy(result -> data, str, len);
	result -> length = len;
	
	result -> refi.type = result -> gtLink -> typeLink;
	
	return result;
	}

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

static STACK_OF(X509)* parseChain(unsigned char *data, size_t len)
	{
	STACK_OF(X509) *certStack;
	
	certStack = sk_X509_new_null();
	
	int i = 0;
	VVarLivePTR *nxt = (VVarLivePTR*) data;
	for (i = 0; i < len; i++)
		{
		LiveData *qd = (LiveData*) nxt -> content;
		LiveArray *qa = (LiveArray*) ((VVarLivePTR*) qd -> data) -> content;
		
		BIO *biomem = BIO_new(BIO_s_mem());
		BIO_write(biomem, qa -> data, qa -> length);
		X509* cert = PEM_read_bio_X509(biomem, NULL, NULL, NULL);
		if (cert == NULL)
			{
			return NULL;
			}

		sk_X509_push(certStack, cert);

		BIO_free(biomem);
		//X509_free(cert);
		
		nxt ++;
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

INSTRUCTION_DEF op_create_cert_store(VFrame *cframe)
	{
	X509_STORE *store = X509_STORE_new();
	
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &store, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_certificate(VFrame *cframe)
	{
	X509_STORE *store;
	memcpy(&store, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *certArray = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	X509 *cert = parseCertificate(certArray -> data, certArray -> length);
	
	X509_STORE_add_cert(store, cert);
	
	X509_free(cert);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_load_cert_location(VFrame *cframe)
	{
	X509_STORE *store;
	memcpy(&store, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	char *path = malloc(array -> length + 1);
	memset(path, '\0', array -> length + 1);
	
	memcpy(path, array -> data, array -> length);
	
	X509_STORE_load_locations(store, path, NULL);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_free_cert_store(VFrame *cframe)
	{
	X509_STORE *store;
	memcpy(&store, getVariableContent(cframe, 0), sizeof(size_t));
	
	X509_STORE_free(store);
	
	return RETURN_OK;
	}

// -- contexts --

INSTRUCTION_DEF op_create_context(VFrame *cframe)
	{
	bool serverMode = getVariableContent(cframe, 0)[0];
	
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
	
	//return boolean
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(unsigned char));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_certificate_chain(VFrame *cframe)
	{
	SSL_CTX *ctx;
	memcpy(&ctx, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	VVarLivePTR *nxt = (VVarLivePTR*) array -> data;
	int i = 0;
	for (i = 0; i < array -> length; i++)
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

INSTRUCTION_DEF op_set_cipher_set(VFrame *cframe)
	{
	SSL_CTX *ctx;
	memcpy(&ctx, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t set = 0;
	copyHostInteger((unsigned char*) &set, getVariableContent(cframe, 1), sizeof(size_t));
	
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
		api -> throwException(cframe, "make SSL failed");
		return RETURN_OK;
		}
	
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &ssl, sizeof(size_t));
	
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
		
		size_t asz = sizeof(VVarLivePTR) * len;
		LiveArray *newArray = malloc(sizeof(LiveArray)+asz);
		memset(newArray, '\0', sizeof(LiveArray));
		
		newArray -> refi.ocm = dataOwner;
		newArray -> gtLink = stringArrayGT;
		api -> incrementGTRefCount(newArray -> gtLink);
		newArray -> data = ((unsigned char*) newArray) + sizeof(LiveArray);
		memset(newArray -> data, '\0', sizeof(VVarLivePTR) * len);
		newArray -> length = len;
		newArray -> refi.type = newArray -> gtLink -> typeLink;
		
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
			
			size_t sz = sizeof(VVarLivePTR);
			LiveData *newData = malloc(sizeof(LiveData)+sz);
			memset(newData, '\0', sizeof(LiveData)+sz);
			newData -> refi.ocm = dataOwner;
			newData -> gtLink = stringItemGT;
			api -> incrementGTRefCount(newData -> gtLink);
			
			newData -> data = ((unsigned char*) newData) + sizeof(LiveData);
			
			VVarLivePTR *ptrh = (VVarLivePTR*) newData -> data;
			
			LiveArray *itemArray = makeStringArray(pbuf, len, dataOwner);
			
			ptrh -> content = (unsigned char*) itemArray;
			itemArray -> refi.refCount ++;
			itemArray -> refi.ocm = dataOwner;
			
			// -- reference in array --
			VVarLivePTR *ptrhA = (VVarLivePTR*) (&newArray -> data[sizeof(VVarLivePTR) * i]);
			ptrhA -> content = (unsigned char*) newData;
			newData -> refi.refCount ++;
			newData -> refi.type = newData -> gtLink -> typeLink;
			
			// --
			
			BIO_free(biomem);
			}
		
		newArray -> refi.refCount = 1;
		
		VVarLivePTR *ptrh = (VVarLivePTR*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
		
		ptrh -> content = (unsigned char*) newArray;

		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_verify_certificate(VFrame *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, getVariableContent(cframe, 0), sizeof(size_t));
	
	X509_STORE *store;
	memcpy(&store, getVariableContent(cframe, 1), sizeof(size_t));
	
	//get and parse both the certificate and chain, if provided
	
	LiveData *verifyResult = (LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 2)) -> content;
	
	LiveArray *certArray = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 3)) -> content;
	
	LiveArray *certChainArray = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 4)) -> content;
	
	DanaComponent *dataOwner = cframe -> instance;
	
	X509 *cert = NULL;
	STACK_OF(X509) *certStack = NULL;
	
	cert = parseCertificate(certArray -> data, certArray -> length);
	
	if (certChainArray != NULL)
		certStack = parseChain(certChainArray -> data, certChainArray -> length);
	
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
	
	if (rc != 1) core_result = 1;
		{
		size_t *vres = (size_t*) verifyResult -> data;
		copyHostInteger((unsigned char*) vres, (unsigned char*) &core_result, sizeof(size_t));
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
		VVarLivePTR *aptr = (VVarLivePTR*) (verifyResult -> data + sizeof(size_t));
		
		if (badCert == cert)
			{
			aptr -> content = (unsigned char*) certArray;
			certArray -> refi.refCount ++;
			}
			else if (certStack != NULL)
			{
			unsigned int len = sk_X509_num(certStack);
			VVarLivePTR *kp = (VVarLivePTR*) certChainArray -> data;
			
			size_t i = 0;
			for (i = 0; i < len; i++)
				{
				X509 *cert = sk_X509_value(certStack, i);
				
				if (cert == badCert)
					{
					LiveData *cfd = (LiveData*) kp -> content;
					LiveArray *cfa = (LiveArray*) ((VVarLivePTR*) cfd -> data) -> content;
					
					aptr -> content = (unsigned char*) cfa;
					cfa -> refi.refCount ++;
					
					break;
					}
				
				kp ++;
				}
			}
			else
			{
			//TODO: it's something from the CertStore; get the raw certificate... (?)
			}
		
		aptr ++;
		
		//get a string reason for verify failure, if any
		LiveArray *reason = makeStringArray((unsigned char*) failReason, strlen(failReason), dataOwner);
		
		aptr -> content = (unsigned char*) reason;
		reason -> refi.refCount ++;
		reason -> refi.ocm = dataOwner;
		}
	
	if (certStack != NULL)
		{
		cleanupStack(certStack);
		}
	
	X509_free(cert);
	
	X509_STORE_CTX_free(ctx);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_write(VFrame *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	int sz = 0;
	
	if (array != NULL)
		{
		sz = SSL_write(ssl, (char*) array -> data, array -> length);
		}
	
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
	
	LiveArray *array = makeByteArray(cframe, len);
	
	if (array == NULL)
		{
		len = 0;
		return RETURN_OK;
		}
	
	int amt = 1;
	size_t totalAmt = 0;
	
	while ((len > 0) && (amt != 0))
		{
		amt = SSL_read(ssl, (char*) (array -> data +totalAmt), len);
		
		if (amt < 0)
			{
			int errC = ERR_get_error();
			if (errC != SSL_ERROR_ZERO_RETURN && errC != 0)
				{
				api -> throwException(cframe, ERR_error_string(errC, NULL));
				}
			
			api -> decrementGTRefCount(array -> gtLink);
			free(array);
			return RETURN_OK;
			}
		
		totalAmt += amt;
		len -= amt;
		}
	
	if (totalAmt > 0)
		{
		array -> length = totalAmt;
		returnArray(cframe, array);
		}
		else
		{
		api -> decrementGTRefCount(array -> gtLink);
		free(array);
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_close_ssl(VFrame *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, getVariableContent(cframe, 0), sizeof(size_t));
	
	SSL_shutdown(ssl);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_free_ssl(VFrame *cframe)
	{
	SSL *ssl;
	memcpy(&ssl, getVariableContent(cframe, 0), sizeof(size_t));
	
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
