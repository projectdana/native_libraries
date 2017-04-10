#ifndef _DANA_TYPES_
#define _DANA_TYPES_

#include <stdbool.h>

#include "dana_semaphore.h"

#define TYPE_NULL		0
#define TYPE_LITERAL	1
#define TYPE_PATTERN	2
#define TYPE_FUNCTION	3
#define TYPE_BINDPOINT	4
#define TYPE_OBJECT		5
#define TYPE_DATA		6
#define TYPE_ARRAY		7
#define TYPE_VAR		8
#define TYPE_THREAD		9
#define TYPE_MUTEX		10
#define TYPE_VLH_LNK	11
#define TYPE_VLH_CNT	12
#define TYPE_PTRH		13
#define TYPE_DECIMAL	14

#define X_FLAT				0
#define X_VLIST_CNT			1
#define X_VLIST_LNK			2
#define X_POINTER			16

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

typedef struct livethread{
	void *a;
	void *b;
	void *c;
	void *d;
	} LiveThread;

typedef struct vv{
	void *a;
	void *ptrs;
	size_t b;
	size_t c;
	unsigned char d; unsigned char e;
	size_t etype;
	} VVar;

typedef struct scope{
	VVar scopePointers;
	size_t flags;
	} LiveScope;

typedef struct lvl{
	unsigned char *content;
	size_t vsize;
	void *last;
	} LiveVL;

typedef struct lcntvl{
	unsigned char *content;
	struct vv *ptrs;
	size_t offset;
	size_t vsize;
	LiveScope scope;
	} LiveCntVL;

typedef struct vvlptr{
	unsigned char *content;
	struct vv *next;
	struct vv *prev;
	size_t vsize;
	size_t lsize;
	LiveVL *vl;
	void *vlf;
	void *vlTop;
	void *vlBot;
	struct _s_danaType *typeLink;
	struct component *owner;
	unsigned char readOnly;
	} VVarLivePTR;

typedef struct _RPTR{
	unsigned char *content;
	struct vv *next;
	struct vv *prev;
	size_t vsize;
	size_t lsize;
	LiveVL *vl;
	void *vlf;
	void *vlTop;
	void *vlBot;
	struct component *owner;
	} RPTR;

struct _s_danaType;

typedef struct rvv{
	RPTR PR;
	//unsigned char *content;
	unsigned char type; unsigned char xtype;
	size_t etype;
	struct _s_danaType *typeLink;
	unsigned char readOnly;
	} VVarR;

typedef struct oi{
	VVarLivePTR bindpoint;
	struct lo *home;
	struct oi *next;
	size_t knr;
	} ObjectIdentity;

typedef struct lo{
	ObjectIdentity identity;
	ObjectIdentity *identities;
	unsigned char *state;
	void *spec;
	void *stl;
	void *otl;
	void *tstl;
	void *pe;
	void *phe;
	void *lcj;
	void *ts;
	size_t flags;
	size_t refCount;
	struct component *owner;
	LiveScope scope;
	} LiveObject;

typedef struct _live_data{
	unsigned char *data;
	struct __globalTypeLink *gtLink;
	size_t flags;
	size_t refCount;
	size_t orc;
	void *gce;
	struct component *owner;
	LiveScope scope;
	
	size_t csa;
	size_t csb;
	size_t csc;
	size_t csd;
	size_t cse;
#ifdef MACHINE_32
	#ifdef LINUX
	size_t csf;
	#endif
	#ifdef WINDOWS
	size_t csf;
	#endif
#endif
	} LiveData;

typedef struct{
	unsigned char *data;
	struct __globalTypeLink *gtLink;
	size_t flags;
	size_t refCount;
	size_t orc;
	void *gce;
	struct component *owner;
	LiveScope scope;
	size_t length;
	} LiveArray;

typedef struct _s_danaType{
	uint8 typeClass;
	uint8 container;
	uint8 flags;
	size_t esize;
	size_t length;
	size_t offset;
	VVarLivePTR definition;
	} DanaType;

typedef struct thrhdr{
	size_t frameSize;
	size_t registerCount;
	size_t formalParamsCount;
	unsigned char *pcLoc;
	void *el;
	size_t scopeCount;
	char *functionName;
	} VFrameHeader;

typedef struct __est{
	size_t status;
	size_t result;
	} ExitStatus;

typedef struct __sc{
	VVar scope;
	struct __sc *next;
	} ScopeContainer;

typedef struct vthread{
	unsigned char *pc;
	unsigned char *eii;
	VFrameHeader *header;
	struct component *instance;
	struct component *host;
	LiveObject *hostObject;
	struct vthread *a;
	struct vthread *b;
	struct vthread *blocking;
	struct vthread *c;
	ExitStatus *exitStatus;
	LiveThread liveThread;
	VVarR *registers;
	VVarR *registerPtr;
	ScopeContainer *scopes;
	ScopeContainer *scopeStack;
	unsigned char *localsData;
	LiveObject *io;
	void *om;
	void *pr;
	void *pa;
	struct vthread *nrf;
	size_t rr;
	size_t ri;
	char *rex;
	size_t rex1;
	size_t rex2;
	size_t rex3;
	VVarR *rr1;
	VVarR *rr2;
	unsigned char *pe;
	unsigned char pec;
	size_t ln;
	unsigned char f;
	unsigned char xf;
	unsigned char inss;
	void* to;
	void* totc;
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
	VVar globalsRef;
	unsigned char *globalsData;
	SourceHeader *header;
	LiveVL instanceList;
	VVarLivePTR createdItems;
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
	} Component;

typedef struct{
	LiveCntVL name;
	LiveCntVL structure;
	size_t size;
	} StructuredType;

typedef struct{
	LiveCntVL name;
	LiveCntVL structure;
	size_t size;
	size_t flags;
	} StructuredInterfaceType;

typedef struct{
	LiveCntVL name;
	LiveCntVL structure;
	size_t size;
	} StructuredFunctionType;

typedef struct intfdet{
	size_t interfaceDefinitionAddr;
	size_t objectDefinitionAddr;
	size_t variantNameLen;
	char *variantName;
	size_t *functionBindings;
	} InterfaceDetails;

#endif
