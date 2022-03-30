#ifndef _DANA_LIB_API
#define _DANA_LIB_API

#define LIB_API_VERSION 17

#define RETURN_OK	0

typedef struct _danaEl {
	} DanaEl;

typedef struct _frameData {
	} FrameData;

typedef struct _CoreAPI{
	
	//get the current "Dana home" directory known to the runtime
	const char* (*getDanaHome)();
	
	//functions to access type mappings, which can then be used to instantiate data/array instances
	// - get a type mapping record, which also increments the ref count of that mapping record
	GlobalTypeLink* (*resolveGlobalTypeMapping)(const DanaType *d);
	// - decrement the ref count of a mapping record, if you're not going to use it again
	void (*decrementGTRefCount)(GlobalTypeLink *d);
	// - increment the ref count of a mapping record; this is rarely used
	void (*incrementGTRefCount)(GlobalTypeLink *d);
	
	//helper function for direct decimal arithmetic
	unsigned char* (*getDecimalScalingFactor)(size_t byteWidth);
	
	//throw an exception from a native library, giving a reason
	void (*throwException)(FrameData* frame, char *reason);
	
	//increment and decrement the reference count of an instance (data/array/object)
	// - these functions are only needed if you intend to store references to instances in global scope
	void (*incRef)(FrameData* frame, DanaEl* entity);
	void (*decRef)(FrameData* frame, DanaEl* entity);
	
	//get the instance object in which the given frame is associated, useful e.g. with pushEvent()
	DanaEl* (*getInstanceObject)(FrameData* f);
	
	//push an event to event queues listening to a given event source
	int (*pushEvent)(DanaEl* object, size_t event_sink_index, size_t event_type, DanaEl* event_details);
	
	//get a parameter to a function call, by index; choose the appropriate function from the below alternatives depending on whether you are accessing an integer, decimal, or instance (data/array/object)
	// - getting a "raw" parameter yields a direct reference to the memory of that parameter
	unsigned char* (*getParamRaw)(FrameData* f, size_t paramIndex);
	size_t (*getParamInt)(FrameData* f, size_t paramIndex);
	double (*getParamDec)(FrameData* f, size_t paramIndex);
	DanaEl* (*getParamEl)(FrameData* f, size_t paramIndex);
	
	//return a value from a function call; choose the appropriate function from the below alternatives depending on whether you are returning an integer, decimal, or instance (data/array/object)
	// - return "raw" copies the given number of bytes directly into the memory of the return value
	void (*returnRaw)(FrameData* f, unsigned char *val, size_t len);
	void (*returnInt)(FrameData* f, size_t val);
	void (*returnDec)(FrameData* f, double val);
	void (*returnEl)(FrameData* f, DanaEl* val);
	
	//functions to create and inspect arrays, including getting and setting values of individual cells
	// - make a one-dimensional array, optionally yielding a reference to the array content
	DanaEl* (*makeArray)(GlobalTypeLink *d, size_t cells, unsigned char** content);
	// - make a multi-dimensional array, giving the number of dimensions, and length of each dimension
	DanaEl* (*makeArrayMD)(GlobalTypeLink *d, size_t dimCount, size_t* dimData, unsigned char **content);
	// - get the current length of an array
	size_t (*getArrayLength)(DanaEl* array);
	// - get the current dimensions of an array
	size_t* (*getArrayDimensions)(DanaEl* array, size_t* dimCount);
	// - get a direct reference to the content of the array, useful e.g. for byte arrays
	unsigned char* (*getArrayContent)(DanaEl* array);
	// - set the current length of an array; this is only needed when changing an array's length
	void (*setArrayLength)(DanaEl* array, size_t length);
	// - get the contents of a given array cell
	unsigned char* (*getArrayCellRaw)(DanaEl* array, size_t index);
	size_t (*getArrayCellInt)(DanaEl* array, size_t index);
	double (*getArrayCellDec)(DanaEl* array, size_t index);
	DanaEl* (*getArrayCellEl)(DanaEl* array, size_t index);
	// - set the contents of a given array cell
	void (*setArrayCellRaw)(DanaEl* array, size_t index, unsigned char* val, size_t len);
	void (*setArrayCellInt)(DanaEl* array, size_t index, size_t val);
	void (*setArrayCellDec)(DanaEl* array, size_t index, double val);
	void (*setArrayCellEl)(DanaEl* array, size_t index, DanaEl *val);
	void (*destroyArray)(DanaEl* array);
	
	//functions to create and inspect data instances, including getting and setting values of fields
	DanaEl* (*makeData)(GlobalTypeLink *d);
	unsigned char* (*getDataContent)(DanaEl* el);
	unsigned char* (*getDataFieldRaw)(DanaEl* el, size_t field_index);
	size_t (*getDataFieldInt)(DanaEl* el, size_t field_index);
	double (*getDataFieldDec)(DanaEl* el, size_t field_index);
	DanaEl* (*getDataFieldEl)(DanaEl* el, size_t field_index);
	void (*setDataFieldRaw)(DanaEl* el, size_t field_index, unsigned char *val, size_t len);
	void (*setDataFieldInt)(DanaEl* el, size_t field_index, size_t val);
	void (*setDataFieldDec)(DanaEl* el, size_t field_index, double val);
	void (*setDataFieldEl)(DanaEl* el, size_t field_index, DanaEl* val);
	
	} CoreAPI;

//function provided automatically by including Dana lib API headers
typedef unsigned char (*dlGetAPIVersion)();
//function which must be provided by the native library, invoked when that library is first loaded
typedef Interface* (*dlLoad)(CoreAPI *capi);
//function which must be provided by the native library, invoked when that library is unloaded
typedef void (*dlUnload)();

//function prototype which all native library functions must follow
typedef int (*dlFunc)(FrameData *cframe);

#endif
