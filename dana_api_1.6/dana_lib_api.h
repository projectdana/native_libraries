#ifndef _DANA_LIB_API
#define _DANA_LIB_API

#define LIB_API_VERSION 16

#define RETURN_OK	0

typedef struct _danaEl {
	} DanaEl;

typedef struct _frameData {
	} FrameData;

typedef struct _CoreAPI{
	const char* (*getDanaHome)();
	
	GlobalTypeLink* (*resolveGlobalTypeMapping)(const DanaType *d);
	void (*decrementGTRefCount)(GlobalTypeLink *d);
	void (*incrementGTRefCount)(GlobalTypeLink *d);
	unsigned char* (*getDecimalScalingFactor)(size_t byteWidth);
	void (*throwException)(FrameData *frame, char *reason);
	void (*incRef)(FrameData* frame, DanaEl* entity);
	void (*decRef)(FrameData* frame, DanaEl* entity);
	
	DanaEl* (*getInstanceObject)(FrameData* f);
	
	int (*pushEvent)(DanaEl* object, size_t event_sink_index, size_t event_type, DanaEl* event_details);
	
	unsigned char* (*getParamRaw)(FrameData* f, size_t paramIndex);
	size_t (*getParamInt)(FrameData* f, size_t paramIndex);
	double (*getParamDec)(FrameData* f, size_t paramIndex);
	DanaEl* (*getParamEl)(FrameData* f, size_t paramIndex);
	
	void (*returnRaw)(FrameData* f, unsigned char *val, size_t len);
	void (*returnInt)(FrameData* f, size_t val);
	void (*returnDec)(FrameData* f, double val);
	void (*returnEl)(FrameData* f, DanaEl* val);
	
	DanaEl* (*makeArray)(GlobalTypeLink *d, size_t cells);
	unsigned char* (*getArrayContent)(DanaEl* array);
	size_t (*getArrayLength)(DanaEl* array);
	void (*setArrayLength)(DanaEl* array, size_t length);
	unsigned char* (*getArrayCellRaw)(DanaEl* array, size_t index);
	size_t (*getArrayCellInt)(DanaEl* array, size_t index);
	double (*getArrayCellDec)(DanaEl* array, size_t index);
	DanaEl* (*getArrayCellEl)(DanaEl* array, size_t index);
	void (*setArrayCellRaw)(DanaEl* array, size_t index, unsigned char* val, size_t len);
	void (*setArrayCellInt)(DanaEl* array, size_t index, size_t val);
	void (*setArrayCellDec)(DanaEl* array, size_t index, double val);
	void (*setArrayCellEl)(DanaEl* array, size_t index, DanaEl *val);
	void (*destroyArray)(DanaEl* array);
	
	DanaEl* (*makeData)(GlobalTypeLink *d);
	unsigned char* (*getDataContent)(DanaEl* el);
	unsigned char* (*getDataFieldRaw)(DanaEl* array, size_t field_index);
	size_t (*getDataFieldInt)(DanaEl* array, size_t field_index);
	double (*getDataFieldDec)(DanaEl* array, size_t field_index);
	DanaEl* (*getDataFieldEl)(DanaEl* array, size_t field_index);
	void (*setDataFieldRaw)(DanaEl* array, size_t field_index, unsigned char *val, size_t len);
	void (*setDataFieldInt)(DanaEl* array, size_t field_index, size_t val);
	void (*setDataFieldDec)(DanaEl* array, size_t field_index, double val);
	void (*setDataFieldEl)(DanaEl* array, size_t field_index, DanaEl* val);
	
	} CoreAPI;

typedef Interface* (*dlLoad)(CoreAPI *capi);
typedef unsigned char (*dlGetAPIVersion)();
typedef void (*dlUnload)();

typedef int (*dlFunc)(FrameData *cframe);

#endif
