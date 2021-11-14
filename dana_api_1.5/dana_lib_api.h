#ifndef _DANA_LIB_API
#define _DANA_LIB_API

#define LIB_API_VERSION 15

#define RETURN_OK	0

typedef struct _CoreAPI{
	int (*pushEvent)(void *object, size_t event_sink_index, size_t event_type, LiveData *event_details);
	GlobalTypeLink* (*resolveGlobalTypeMapping)(const DanaType *d);
	void (*decrementGTRefCount)(GlobalTypeLink *d);
	void (*incrementGTRefCount)(GlobalTypeLink *d);
	unsigned char* (*getDecimalScalingFactor)(size_t byteWidth);
	void (*throwException)(VFrame *frame, char *reason);
	void (*incRef)(VFrame *frame, void *entity);
	void (*decRef)(VFrame *frame, void *entity);
	const char* (*getDanaHome)();
	} CoreAPI;

typedef Interface* (*dlLoad)(CoreAPI *capi);
typedef unsigned char (*dlGetAPIVersion)();
typedef void (*dlUnload)();

typedef int (*dlFunc)(VFrame *cframe);

#endif
