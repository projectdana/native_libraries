#ifndef _DANA_TYPES_
#define _DANA_TYPES_

#include <stdbool.h>

#include "dana_semaphore.h"

#define TYPE_NULL			0
#define TYPE_LITERAL		1
#define TYPE_PATTERN		2
#define TYPE_FUNCTION		3
#define TYPE_UNA			4
#define TYPE_OBJECT			5
#define TYPE_DATA			6
#define TYPE_ARRAY			7
#define TYPE_VAR			8
#define TYPE_UND			9
#define TYPE_EVENTSOURCE	10
#define TYPE_UNC			11
#define TYPE_UNB			12
#define TYPE_PTRH			13
#define TYPE_DECIMAL		14

#define X_FLAT				0
#define X_POINTER			16

typedef struct vvlptr{
	unsigned char *c;
	unsigned char r;
	unsigned char s;
	} VVarLivePTR;

typedef struct livebindpoint{
	void *t;
	void *spec;
	void *b;
	void *f;
	void *n;
	void *p;
	} LiveBindpoint;

typedef struct __interface{
	void *d;
	LiveBindpoint lbp;
	} Interface;
	
struct _s_danaType;

typedef struct rvv{
	size_t a;
	size_t b;
	size_t c;
	unsigned char d;
	unsigned char e;
	unsigned char f;
	size_t g;
	size_t h;
	} VVarR;

typedef struct _s_danaType{
	uint8 typeClass;
	uint8 flags;
	size_t esize;
	struct _p_danaField *fields;
	size_t fieldCount;
	} DanaType;

typedef struct _p_danaField{
	DanaType *type;
	char *fieldName;
	size_t fieldNameLength;
	uint8 flags;
	size_t offset;
} DanaTypeField;

typedef struct thrhdr{
	size_t frameSize;
	struct thrhdr *sub;
	size_t formalParamsCount;
	size_t formalParamsCore;
	unsigned char *pcLoc;
	void *el;
	size_t localsDef;
	char *functionName;
	size_t xr;
	size_t xrr;
	size_t xrd;
	} VFrameHeader;

typedef struct vthread{
	unsigned char *pc;
	unsigned char *eii;
	size_t ln;
	unsigned int *khd;
	struct component *instance;
	size_t io;
	void *tc;
	struct vthread *a;
	struct vthread *blocking;
	struct vthread *c;
	unsigned int *frc;
	size_t localsDef;
	unsigned char *localsData;
	size_t svec;
	unsigned char fa;
	unsigned char fb;
	unsigned char fc;
	unsigned char fd;
	void *tea;
	void *teb;
	void *tec;
	} VFrame;

typedef struct{
	size_t cs;
	size_t od;
	size_t bd;
	size_t objectsCount;
	size_t bc;
	size_t cif;
	size_t css;
	size_t tm;
	size_t tmc;
	char *sourceName;
	uint16 capabilities;
	} SourceHeader;

typedef struct objectspec{
	Interface *interfaces;
	size_t interfacesCount;
	size_t oif;
	size_t oatf;
	size_t orwf;
	size_t tss;
	size_t ss;
	} ObjectSpec;

typedef struct __globalTypeLink{
	struct _s_danaType *typeLink;
	size_t refCount;
	
	Semaphore lock;
	
	struct __globalTypeLink *next;
	struct __globalTypeLink *prev;
	} GlobalTypeLink;

typedef struct component{
	void *gref;
	unsigned char *globalsData;
	SourceHeader *header;
	VVarLivePTR ci;
	void *ho;
	ObjectSpec *objects;
	void *bs;
	size_t *globalTypeMappings;
	void *no;
	void *tt;
	void *c;
	void *a;
	void *d;
	void *cs;
	size_t rc;
	unsigned char fg;
	} DanaComponent;

typedef struct intfdet{
	char *name;
	size_t nameLength;
	DanaType *type;
	size_t variantNameLen;
	char *variantName;
	size_t *functionBindings;
	size_t flags;
	} InterfaceDetails;

#endif
