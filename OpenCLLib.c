//Written by Ben Craine, 2023

/*
 * IMPORTANT OPENCL DEFINITIONS
 * Platform - An opencl implimentation, e.g CUDA, Intel Iris.
 * Context - A set of devices (belonging to the same platform), Command Queues, Kernels and Memory objects
 * Device - A physical compute device available to opencl
 * Command Queue - A queue that points to a single device, requests to execute operations on a device are sent through a command queue that points to that device
 * Kernels - A compiled, executable program associated with a set of parameters (Memory objects), built for a specific device/platform
 * Memory objects - The opencl abstraction of a reserved part of memory on a device that can be read/written or passed as a kernel parameter using opencl API calls
 */

//#define CL_TARGET_OPENCL_VERSION 100

//std lib headers
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//openCL headers
#include <CL/cl_platform.h>
#include <CL/cl.h>

//dana headers
#include "dana_api_1.7/dana_lib_defs.h"
#include "dana_api_1.7/dana_types.h"
#include "dana_api_1.7/nli_util.h"
#include "dana_api_1.7/vmi_util.h"

#define FLOAT 0
#define UINT 1

#define MAX_PLATFORMS 100
#define MAX_DEVICES 100

static CoreAPI *api;

static GlobalTypeLink* intArrayGT = NULL;
static GlobalTypeLink* charArrayGT = NULL;
static GlobalTypeLink* stringArrayGT = NULL;
static GlobalTypeLink* stringItemGT = NULL;
static GlobalTypeLink* intMatrixGT = NULL;
static GlobalTypeLink* decMatrixGT = NULL;
static GlobalTypeLink* decArrayGT = NULL;

static uint8_t alreadyInitFlag = 0;

/*
 * these global variables are set in init()
 * and MUST NOT be changed once the above
 * flag is set true or in any function other
 * than init().
 */
static cl_platform_id* globalClPlatforms;
static cl_uint numofPlatforms;
static cl_uint* numOfDevicesPerPlatform;
static cl_device_id** devices;

/* 
 * These structures are used to
 * keep state relating to a given
 * instance of a dana dana component
 * that requires the use of this library
 */

typedef struct _context_list_item {
    cl_context context;
    cl_platform_id platform;
    cl_device_id* devices;
    uint8_t numOfDevices;
    struct _context_list_item* next;
} CONTEXT_LI;

typedef struct _one_per_dana_comp {
    CONTEXT_LI* contexts;
} DANA_COMP;

void destroyContexts(DANA_COMP* contextSpace) {
    if (contextSpace == NULL || contextSpace->contexts == NULL) {
        return;
    }

    CONTEXT_LI* probe = contextSpace->contexts;
    for (; probe->next != NULL; probe = probe->next) {
        clReleaseContext(probe->context);
        for (int i = 0; i < probe->numOfDevices; i++) {
            clReleaseDevice(*(probe->devices+i));
        }
    }
    clReleaseContext(probe->context);
    for (int i = 0; i < probe->numOfDevices; i++) {
        clReleaseDevice(*(probe->devices+i));
    }

    free(contextSpace);
    return;
}

void addNewContext(DANA_COMP* danaComp, CONTEXT_LI* newContext) {
    if (danaComp->contexts == NULL) {
        danaComp->contexts = newContext;
        return;
    }

    CONTEXT_LI* probe = danaComp->contexts;
    for (; probe->next != NULL; probe = probe->next) {
    }
    probe->next = newContext;
}

int getNumOfContexts(DANA_COMP* danaComp) {
    CONTEXT_LI* probe = danaComp->contexts;
    if (danaComp->contexts == NULL) {
        return 0;
    }
    int count = 0;
    for (; probe->next != NULL; probe = probe->next) {
        count++;
    }
    count++;
    return count;
}

CONTEXT_LI* getContextByIndex(DANA_COMP* danaComp, int n) {
    CONTEXT_LI* probe = danaComp->contexts;
    if (danaComp->contexts == NULL) {
        return NULL;
    }
    int count = 0;
    for (; probe->next != NULL; probe = probe->next) {
        if (count == n) {
            return probe;
        }
        count++;
    }
    if (count == n) {
        return probe;
    }
    return NULL;
}

CONTEXT_LI* getContextByDevice(DANA_COMP* danaComp, cl_device_id device) {
    CONTEXT_LI* contextProbe = danaComp->contexts;
    if (contextProbe == NULL) {
        return NULL;
    }

    for (; contextProbe->next != NULL; contextProbe = contextProbe->next) {
        for (int i = 0; i < contextProbe->numOfDevices; i++) {
            if (contextProbe->devices[i] == device) {
                return contextProbe;
            }
        }
    }
    for (int i = 0; i < contextProbe->numOfDevices; i++) {
        if (contextProbe->devices[i] == device) {
            return contextProbe;
        }
    }

    return NULL;
}

typedef struct _log_list_item {
    char* APIFunctionCall;
    int errorCode;
    struct _log_list_item* next;
}LOG_LI;

LOG_LI* logListHead = NULL;
LOG_LI* logListEnd = NULL;

LOG_LI* newLogItem(char* func, int err) {
    LOG_LI* newLog = (LOG_LI*) malloc(sizeof(LOG_LI));
    newLog->APIFunctionCall = func;
    newLog->errorCode = err;
    newLog->next = NULL;
    return newLog;
}

void addLog(LOG_LI* adding) {
    if (logListHead == NULL) {
        logListHead = adding;
        logListEnd = adding;
        return;
    }

    logListEnd->next = adding; 
    logListEnd = adding;
    return;
}

INSTRUCTION_DEF printLogs(FrameData *cframe) {
    LOG_LI* probe = logListHead;
    if (probe == NULL) {
        return RETURN_OK;
    }
    for (; probe->next != NULL; probe = probe->next) {
        printf("%s | %d\n", probe->APIFunctionCall, probe->errorCode);
    }
    printf("%s | %d\n", probe->APIFunctionCall, probe->errorCode);
    return RETURN_OK;
}

INSTRUCTION_DEF createContextSpace(FrameData *cframe) {
    DANA_COMP* dana_component_id = (DANA_COMP*) malloc(sizeof(DANA_COMP));
    dana_component_id->contexts = NULL;
    api->returnInt(cframe, (size_t) dana_component_id);
    return RETURN_OK;
}

/*
 * This function sets the global variables that
 * relate to opencl and were declared at the top
 * of this file
 * It also creates the per dana component instance
 * state and returns a reference to that state back
 * to the caller
 * Therefore one Dana component instance may create
 * more than one DANA_COMP. This breaks the intended
 * one to one relationship between DANA_COMP and
 * a dana component instance. However the advantage of
 * this desicion is that a dana component can be made
 * that manages calls to this native library on behalf
 * of other components
 */
INSTRUCTION_DEF init(FrameData *cframe) {
    cl_int CL_err = CL_SUCCESS;

    if (alreadyInitFlag) {
        return RETURN_OK;
    }

    globalClPlatforms = (cl_platform_id*) malloc(sizeof(cl_platform_id)*MAX_PLATFORMS);
    
    CL_err = clGetPlatformIDs( MAX_PLATFORMS, globalClPlatforms, &numofPlatforms );
    if (CL_err != CL_SUCCESS) {
        printf("no opencl implimentation found\n");
        api->returnInt(cframe, (size_t) 1);
        return RETURN_OK;
    }

    devices = (cl_device_id**) malloc(sizeof(cl_device_id*)*numofPlatforms);
    numOfDevicesPerPlatform = (cl_uint*) malloc(sizeof(cl_uint)*numofPlatforms);
    
    cl_uint returnNumOfDevices = 0;
    for (int i = 0; i < numofPlatforms; i++) {
        *(devices+i) = (cl_device_id*) malloc(sizeof(cl_device_id)*MAX_DEVICES);
        CL_err = clGetDeviceIDs(*(globalClPlatforms+i), CL_DEVICE_TYPE_ALL, MAX_DEVICES, *(devices+i), &returnNumOfDevices);
        if (CL_err != CL_SUCCESS) {
            printf("error in clGetDeviceIDs: %d\n", CL_err);
        }
        *(numOfDevicesPerPlatform+i) = returnNumOfDevices;
    }

    alreadyInitFlag = 1;

    api->returnInt(cframe, (size_t) 0);

    return RETURN_OK;
}

/* Returns to the caller all the device IDs
 * available to the system from the global state
 * i.e that which was set in init(). This function
 * DOES NOT return the device IDs which a dana component
 * instance has created a context for.
 */
INSTRUCTION_DEF getComputeDeviceIDs(FrameData *cframe) {
    //cl_int CL_err = CL_SUCCESS;

    if (devices == NULL) {
        //return empty array
        DanaEl* newArray = api->makeArray(stringArrayGT, 0, NULL);
        api->returnEl(cframe, newArray);
        //exit
        return RETURN_OK;
    }

    //go thru each platform and total the devices for each platform
    int arrSize = 0;
    for (int i = 0; i < numofPlatforms; i++) {
        arrSize += *(numOfDevicesPerPlatform+i);
    }

    //grab each device C handle
    cl_device_id ids[arrSize];
    int seen = 0;
    //for each platform
    for (int i = 0; i < numofPlatforms; i++) {
        //for each device in that platform
        for (int j = 0; j < *(numOfDevicesPerPlatform+i); j++) {
            ids[seen+j] = *(*(devices+i)+j);
        }
        seen += *(numOfDevicesPerPlatform+i);
    }

    //arrange in a dana array
    DanaEl* returnArray = api->makeArray(intArrayGT, arrSize, NULL);

    for (int i = 0; i < arrSize; i++) {
        api->setArrayCellInt(returnArray, i, (size_t) ids[i]);
    }

    //return
    api->returnEl(cframe, returnArray);

    return RETURN_OK;
}

/*
 * Returns the string names of all the devices available
 * to the system. This is taken from the global state, i.e
 * all devices on the system, not just the devices the 
 * dana component instance has access to through a context.
 */
INSTRUCTION_DEF getComputeDevices(FrameData *cframe) {
    cl_int CL_err = CL_SUCCESS;
    if (devices == NULL) {
        //return empty array
        DanaEl* newArray = api->makeArray(stringArrayGT, 0, NULL);
        api->returnEl(cframe, newArray);
        //exit
        return RETURN_OK;
    }

    //go thru each platform
    int arrSize = 0;
    for (int i = 0; i < numofPlatforms; i++) {
        arrSize += *(numOfDevicesPerPlatform+i);
    }

    //grab each device ID and query opencl for the device name
    char* deviceNames[arrSize];
    int seen = 0;
    //for each platform
    for (int i = 0; i < numofPlatforms; i++) {
        //for each device in that platform
        for (int j = 0; j < *(numOfDevicesPerPlatform+i); j++) {
            char* buf = (char*) malloc(sizeof(char)*500);
            deviceNames[seen+j] = buf;
            size_t bufReturnSize;
            CL_err = clGetDeviceInfo(*(*(devices+i)+j), CL_DEVICE_NAME, sizeof(char)*500, buf, &bufReturnSize);
            *(deviceNames[seen+j]+bufReturnSize) = '\0';
        }
        seen += *(numOfDevicesPerPlatform+i);
    }

    //arrange device names in a dana array
    DanaEl* returnArray = api->makeArray(stringArrayGT, arrSize, NULL);

    for (int i = 0; i < arrSize; i++) {
        DanaEl* string = api->makeData(stringItemGT);

        size_t sublen = strlen(deviceNames[i]);

        unsigned char* cnt = NULL;
        DanaEl* charArr = api->makeArray(charArrayGT, sublen, &cnt);

        memcpy(cnt, deviceNames[i], sublen);

        api->setDataFieldEl(string, 0, charArr);

        api->setArrayCellEl(returnArray, i, string);

        free(deviceNames[i]);
    }

    //return
    api->returnEl(cframe, returnArray);

    return RETURN_OK;
}

/* A context in opencl is a set of physical devices, command queues pointing to
 * those devices (Many Qs to 1 Device), kernels and memory objects
 * All devices in a context must belong to the same platform
 * We aim to abstract away this contraint in Dana
 * This is achieved by assigning a list of contexts to a DANA_COMP
 * Then when a dana component instance makes requests like read/writes to a device
 * we search for the context that device is tied to under their corrisponding
 * DANA_COMP state
 * Return 0 on success, >0 otherwise
 */
INSTRUCTION_DEF createContext(FrameData* cframe) {
    cl_int CL_Err = CL_SUCCESS;

    //input: array of device IDs
    DanaEl* deviceArray = api->getParamEl(cframe, 0);
    cl_device_id deviceHandles[api->getArrayLength(deviceArray)];

    DANA_COMP* danaComp = (DANA_COMP*) api->getParamInt(cframe, 1);

    for (int i = 0; i < api->getArrayLength(deviceArray); i++) {
        deviceHandles[i] = (cl_device_id) api->getArrayCellInt(deviceArray, i);
    }

    //for each platform
    for (int i = 0; i < numofPlatforms; i++) {
        cl_platform_id platform = *(globalClPlatforms+i);
        cl_device_id deviceHandlesForThisPlat[MAX_DEVICES];
        int deviceForPlatCount = 0;

        //find the intersection between the devices IDs provided
        //by the caller and the Device IDs associated with this platform
        //(uses global state)
        for (int j = 0; j < *(numOfDevicesPerPlatform+i); j++) {
            for (int k = 0; k < api->getArrayLength(deviceArray); k++) {
                if (deviceHandles[k] == *(*(devices+i)+j)) {
                    deviceHandlesForThisPlat[deviceForPlatCount] = deviceHandles[k];
                    deviceForPlatCount++;
                }
            }
        }
        
        // if the intersection is the empty set then move onto the next platform
        // else continue into next code block
        if (deviceForPlatCount != 0) {
            CONTEXT_LI* newContextItem = (CONTEXT_LI*) malloc(sizeof(CONTEXT_LI));
            newContextItem->next = NULL;
            newContextItem->platform = platform;

            //strip the deviceHandlesForThisPlat array down to its mimimum required size
            cl_device_id deviceHandlesForThisPlatCut[deviceForPlatCount];
            newContextItem->devices = (cl_device_id*) malloc(sizeof(cl_device_id)*deviceForPlatCount);
            newContextItem->numOfDevices = deviceForPlatCount;
            for(int j = 0; j < deviceForPlatCount; j++) {
                deviceHandlesForThisPlatCut[j] = deviceHandlesForThisPlat[j];
                newContextItem->devices[j] = deviceHandlesForThisPlatCut[j];
            }

            //create the opencl context
            const cl_context_properties props[] = {CL_CONTEXT_PLATFORM, newContextItem->platform, 0};
            newContextItem->context = clCreateContext(props, newContextItem->numOfDevices, newContextItem->devices, NULL, NULL, &CL_Err);
            if (CL_Err != CL_SUCCESS) {
                addLog(newLogItem("clCreateContext", CL_Err));
                api->returnInt(cframe, (size_t) 1);
                return RETURN_OK;
            }

            addNewContext(danaComp, newContextItem);
        }
    }
    api->returnInt(cframe, (size_t) 0);
    return RETURN_OK;
}

INSTRUCTION_DEF createAsynchQueue(FrameData* cframe) {
    cl_int CL_err = CL_SUCCESS;
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_device_id device = (cl_device_id) rawParam; 

    DANA_COMP* danaComp = (DANA_COMP*) api->getParamInt(cframe, 1);
    CONTEXT_LI* contextItem = getContextByDevice(danaComp, device);
    cl_context context = contextItem->context;

    const cl_command_queue_properties props = CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE;

    cl_command_queue newQ = clCreateCommandQueue(context, device, props, &CL_err);
    if(CL_err != CL_SUCCESS) {
        addLog(newLogItem("clCreateCommandQueueWithProperties", CL_err));
        api->returnInt(cframe, (size_t) 0);
        return RETURN_OK;
    }

    api->returnInt(cframe, (size_t) newQ);
    return RETURN_OK;
}

INSTRUCTION_DEF createSynchQueue(FrameData* cframe) {
    cl_int CL_err = CL_SUCCESS;
    size_t rawParam = api->getParamInt(cframe, 0);

    cl_device_id device = (cl_device_id) rawParam; 

    DANA_COMP* danaComp = (DANA_COMP*) api->getParamInt(cframe, 1);
    CONTEXT_LI* contextItem = getContextByDevice(danaComp, device);
    cl_context context = contextItem->context;

    cl_command_queue newQ = clCreateCommandQueue(context, device, 0, &CL_err);
    if(CL_err != CL_SUCCESS) {
        addLog(newLogItem("clCreateCommandQueueWithProperties", CL_err));
        api->returnInt(cframe, (size_t) 0);
        return RETURN_OK;
    }

    api->returnInt(cframe, (size_t) newQ);
    return RETURN_OK;
}

INSTRUCTION_DEF createArray(FrameData* cframe) {
    cl_int CL_err = CL_SUCCESS;
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_device_id device = (cl_device_id) rawParam;

    DANA_COMP* danaComp = (DANA_COMP*) api->getParamInt(cframe, 3);
    CONTEXT_LI* contextItem = getContextByDevice(danaComp, device);
    cl_context context = contextItem->context;

    rawParam = api->getParamInt(cframe, 1);
    size_t length = (size_t) rawParam;

    rawParam = api->getParamInt(cframe, 2);
    size_t type = (size_t) rawParam;

    size_t size;
    if (type == FLOAT) {
        size = sizeof(float)*length;
    }
    else if (type == UINT) {
        size = sizeof(size_t)*length;
    }
    else {
        api->returnInt(cframe, (size_t) 0);
        return RETURN_OK;
    }

    cl_mem newArray = clCreateBuffer(context, CL_MEM_READ_WRITE, size, NULL, &CL_err);

    if (CL_err != CL_SUCCESS) {
        addLog(newLogItem("clCreateBuffer", CL_err));
        api->returnInt(cframe, (size_t) 0);
        return RETURN_OK;
    }

    api->returnInt(cframe, (size_t) newArray);

    return RETURN_OK;
}

INSTRUCTION_DEF writeIntArray(FrameData* cframe) {
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_command_queue queue = (cl_command_queue) rawParam;
    rawParam = api->getParamInt(cframe, 1);
    cl_mem memObj = (cl_mem) rawParam;

    DanaEl* hostArray = api->getParamEl(cframe, 2);
    size_t hostArrayLen = api->getArrayLength(hostArray);

    size_t* rawHostArray = (size_t*) malloc(sizeof(size_t)*hostArrayLen);
    size_t* rawHostArrayCpy = rawHostArray;
    for (int i = 0; i < hostArrayLen; i++) {
        *rawHostArrayCpy = api->getArrayCellInt(hostArray, i);
        rawHostArrayCpy++;
    }

    cl_int CL_err = clEnqueueWriteBuffer(queue, memObj, CL_TRUE, 0, hostArrayLen*sizeof(size_t), rawHostArray, 0, NULL, NULL);
    if (CL_err != CL_SUCCESS) {
        addLog(newLogItem("clEnqueueWriteBuffer", CL_err));
        api->returnInt(cframe, (size_t) 1);
        return RETURN_OK;
    }
    api->returnInt(cframe, (size_t) 0);
    free(rawHostArray);
    return RETURN_OK;    
}

INSTRUCTION_DEF readIntArray(FrameData* cframe) {
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_command_queue queue = (cl_command_queue) rawParam;
    rawParam = api->getParamInt(cframe, 1);
    cl_mem memObj = (cl_mem) rawParam;

    size_t hostArrayLen = api->getParamInt(cframe, 2);

    size_t* rawHostArray = (size_t*) malloc(sizeof(size_t)*hostArrayLen);
    size_t* toFree = rawHostArray;

    cl_int CL_err = clEnqueueReadBuffer(queue, memObj, CL_TRUE, 0, hostArrayLen*sizeof(size_t), rawHostArray, 0, NULL, NULL);
    if (CL_err != CL_SUCCESS) {
        addLog(newLogItem("clEnqueueReadBuffer", CL_err));
        api->returnEl(cframe, NULL);
        return RETURN_OK;
    }

    DanaEl* hostArray = api->makeArray(intArrayGT, hostArrayLen, NULL);
    for (int i = 0; i < hostArrayLen; i++) {
        api->setArrayCellInt(hostArray, i, *rawHostArray);
        rawHostArray++;
    }

    api->returnEl(cframe, hostArray);
    free(toFree);

    return RETURN_OK;    
}

INSTRUCTION_DEF writeFloatArray(FrameData* cframe) {
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_command_queue queue = (cl_command_queue) rawParam;
    rawParam = api->getParamInt(cframe, 1);
    cl_mem memObj = (cl_mem) rawParam;

    DanaEl* hostArray = api->getParamEl(cframe, 2);
    size_t hostArrayLen = api->getArrayLength(hostArray);

    float* rawHostArray = (float*) malloc(sizeof(float)*hostArrayLen);
    float* rawHostArrayCpy = rawHostArray;
    for (int i = 0; i < hostArrayLen; i++) {
        *rawHostArrayCpy = api->getArrayCellDec(hostArray, i);
        rawHostArrayCpy++;
    }

    cl_int CL_err = clEnqueueWriteBuffer(queue, memObj, CL_TRUE, 0, hostArrayLen*sizeof(float), rawHostArray, 0, NULL, NULL);
    if (CL_err != CL_SUCCESS) {
        addLog(newLogItem("clEnqueueWriteBuffer", CL_err));
        api->returnInt(cframe, (size_t) 1);
        return RETURN_OK;
    }
    api->returnInt(cframe, (size_t) 0);
    free(rawHostArray);
    return RETURN_OK;    
}

INSTRUCTION_DEF readFloatArray(FrameData* cframe) {
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_command_queue queue = (cl_command_queue) rawParam;
    rawParam = api->getParamInt(cframe, 1);
    cl_mem memObj = (cl_mem) rawParam;

    size_t hostArrayLen = api->getParamInt(cframe, 2);

    float* fromDevice = (float*) malloc(sizeof(float)*hostArrayLen);
    float* toFree = fromDevice;

    cl_int CL_err = clEnqueueReadBuffer(queue, memObj, CL_TRUE, 0, hostArrayLen*sizeof(float), fromDevice, 0, NULL, NULL);
    if (CL_err != CL_SUCCESS) {
        addLog(newLogItem("clEnqueueReadBuffer", CL_err));
        api->returnEl(cframe, NULL);
        return RETURN_OK;
    }


    DanaEl* danaArr = api->makeArray(decArrayGT, hostArrayLen, NULL);
    for (int i = 0; i < hostArrayLen; i++) {
        api->setArrayCellDec(danaArr, i, *fromDevice);
        fromDevice++;
    }

    api->returnEl(cframe, danaArr);
    free(toFree);

    return RETURN_OK;    
}

INSTRUCTION_DEF createMatrix(FrameData* cframe) {
    cl_int CL_err = CL_SUCCESS;
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_device_id device = (cl_device_id) rawParam;

    DANA_COMP* danaComp = (DANA_COMP*) api->getParamInt(cframe, 4);
    CONTEXT_LI* contextItem = getContextByDevice(danaComp, device);
    cl_context context = contextItem->context;

    rawParam = api->getParamInt(cframe, 1);
    size_t rows = (size_t) rawParam;

    rawParam = api->getParamInt(cframe, 2);
    size_t cols = (size_t) rawParam;

    rawParam = api->getParamInt(cframe, 3);
    size_t type = (size_t) rawParam;

    cl_image_desc desc = {CL_MEM_OBJECT_IMAGE2D, cols, rows, 0, 1, 0, 0, 0, 0, NULL};
    cl_image_format form;
    if (type == FLOAT) {
        form = (cl_image_format) {CL_R, CL_FLOAT};
    }
    else if (type == UINT) {
        form = (cl_image_format) {CL_R, CL_UNSIGNED_INT32};
    }
    else {
        api->returnInt(cframe, (size_t) 0);
        return RETURN_OK;
    }

    cl_mem newMatrix = clCreateImage(context, CL_MEM_READ_WRITE, &form, &desc, NULL, &CL_err);

    if (CL_err != CL_SUCCESS) {
        addLog(newLogItem("clCreateImage", CL_err));
        api->returnInt(cframe, (size_t) 0);
        return RETURN_OK;
    }

    api->returnInt(cframe, (size_t) newMatrix);

    return RETURN_OK;
}

INSTRUCTION_DEF writeIntMatrix(FrameData* cframe) {
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_command_queue queue = (cl_command_queue) rawParam;
    rawParam = api->getParamInt(cframe, 1);
    cl_mem memObj = (cl_mem) rawParam;

    DanaEl* hostMatrix = api->getParamEl(cframe, 2);
    size_t dim = 2; //only supporting 2d matricies
    size_t* dims = api->getArrayDimensions(hostMatrix, &dim);

    uint32_t* rawHostMatrix = (uint32_t*) malloc(sizeof(uint32_t)*dims[0]*dims[1]);
    uint32_t* rawHostMatrixCpy = rawHostMatrix;
    for (int i = 0; i < dims[0]; i++) {
        for (int j = 0; j < dims[1]; j++) {
            *rawHostMatrixCpy = api->getArrayCellInt(hostMatrix, (i*dims[1])+j);
            rawHostMatrixCpy++;
        }
    }

    size_t origin[] = {0, 0, 0};
    size_t region[] = {dims[1], dims[0], 1};
    cl_int CL_err = clEnqueueWriteImage(queue, memObj, CL_TRUE, origin, region, 0, 0, rawHostMatrix, 0, NULL, NULL);

    if (CL_err != CL_SUCCESS) {
        addLog(newLogItem("clEnqueueWriteImage", CL_err));
        api->returnInt(cframe, (size_t) 1);
        return RETURN_OK;
    }
    else {
    }
    api->returnInt(cframe, (size_t) 0);
    free(rawHostMatrix);
    return RETURN_OK;    
}

INSTRUCTION_DEF readIntMatrix(FrameData* cframe) {
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_command_queue queue = (cl_command_queue) rawParam;
    rawParam = api->getParamInt(cframe, 1);
    cl_mem memObj = (cl_mem) rawParam;

    DanaEl* matrixDims = api->getParamEl(cframe, 2);
    size_t hostMatrixLens[] = {api->getArrayCellInt(matrixDims, 0), api->getArrayCellInt(matrixDims, 1)};

    uint32_t* rawHostMatrix = (uint32_t*) malloc(sizeof(uint32_t)*hostMatrixLens[0]*hostMatrixLens[1]);
    uint32_t* toFree = rawHostMatrix;

    size_t origin[] = {0, 0, 0};
    size_t region[] = {hostMatrixLens[1], hostMatrixLens[0], 1};


    int CL_Err = clEnqueueReadImage(queue, memObj, CL_TRUE, origin, region, 0, 0, rawHostMatrix, 0, NULL, NULL);
    if (CL_Err != CL_SUCCESS) {
        addLog(newLogItem("clEnqueueReadImage", CL_Err));
        api->returnEl(cframe, NULL);
        return RETURN_OK;
    }

    DanaEl* hostMatrix = api->makeArrayMD(intMatrixGT, 2, hostMatrixLens, NULL);
    for (int i = 0; i < hostMatrixLens[0]; i++) {
        for (int j = 0; j < hostMatrixLens[1]; j++) {
            api->setArrayCellInt(hostMatrix, (i*hostMatrixLens[1])+j, *rawHostMatrix);
            rawHostMatrix++;
        }
    }

    api->returnEl(cframe, hostMatrix);
    free(toFree);

    return RETURN_OK;    
}

INSTRUCTION_DEF writeFloatMatrix(FrameData* cframe) {
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_command_queue queue = (cl_command_queue) rawParam;
    rawParam = api->getParamInt(cframe, 1);
    cl_mem memObj = (cl_mem) rawParam;

    DanaEl* hostMatrix = api->getParamEl(cframe, 2);
    size_t dim = 2; //only supporting 2d matricies
    size_t* dims = api->getArrayDimensions(hostMatrix, &dim);

    float* rawHostMatrix = (float*) malloc(sizeof(float)*dims[0]*dims[1]);
    float* rawHostMatrixCpy = rawHostMatrix;
    for (int i = 0; i < dims[0]; i++) {
        for (int j = 0; j < dims[1]; j++) {
            *rawHostMatrixCpy = api->getArrayCellDec(hostMatrix, (i*dims[1])+j);
            rawHostMatrixCpy++;
        }
    }

    size_t origin[] = {0, 0, 0};
    size_t region[] = {dims[1], dims[0], 1};
    cl_int CL_err = clEnqueueWriteImage(queue, memObj, CL_TRUE, origin, region, 0, 0, rawHostMatrix, 0, NULL, NULL);

    if (CL_err != CL_SUCCESS) {
        addLog(newLogItem("clEnqueueWriteImage", CL_err));
        api->returnInt(cframe, (size_t) 1);
        return RETURN_OK;
    }
    api->returnInt(cframe, (size_t) 0);
    free(rawHostMatrix);
    return RETURN_OK;    
}

INSTRUCTION_DEF readFloatMatrix(FrameData* cframe) {
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_command_queue queue = (cl_command_queue) rawParam;
    rawParam = api->getParamInt(cframe, 1);
    cl_mem memObj = (cl_mem) rawParam;

    DanaEl* matrixDims = api->getParamEl(cframe, 2);
    size_t hostMatrixLens[] = {api->getArrayCellInt(matrixDims, 0), api->getArrayCellInt(matrixDims, 1)};

    float* rawHostMatrix = (float*) malloc(sizeof(float)*hostMatrixLens[0]*hostMatrixLens[1]);
    float* toFree = rawHostMatrix;

    size_t origin[] = {0, 0, 0};
    size_t region[] = {hostMatrixLens[1], hostMatrixLens[0], 1};

    int CL_Err = clEnqueueReadImage(queue, memObj, CL_TRUE, origin, region, 0, 0, rawHostMatrix, 0, NULL, NULL);
    if (CL_Err != CL_SUCCESS) {
        addLog(newLogItem("clEnqueueReadImage", CL_Err));
        api->returnEl(cframe, NULL);
        return RETURN_OK;
    }

    DanaEl* hostMatrix = api->makeArrayMD(decMatrixGT, 2, hostMatrixLens, NULL);
    for (int i = 0; i < hostMatrixLens[0]; i++) {
        for (int j = 0; j < hostMatrixLens[1]; j++) {
            api->setArrayCellDec(hostMatrix, (i*hostMatrixLens[1])+j, *rawHostMatrix);
            rawHostMatrix++;
        }
    }

    api->returnEl(cframe, hostMatrix);
    free(toFree);
    return RETURN_OK;    
}

INSTRUCTION_DEF destroyMemoryArea(FrameData* cframe) {
    cl_int CL_err = CL_SUCCESS;
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_mem memObj = (cl_mem) rawParam; 
    //the next line seg faults if memObj has already been released...
    //averting this has been attemted in dana, but I'll leave this note
    //here just in case
    CL_err = clReleaseMemObject(memObj);
    if (CL_err != CL_SUCCESS) {
        addLog(newLogItem("clReleaseMemObject", CL_err));
        return RETURN_OK;
    }
    return RETURN_OK;
}

INSTRUCTION_DEF destroyContextSpace(FrameData* cframe) {
    size_t rawParam = api->getParamInt(cframe, 0);
    DANA_COMP* comp = (DANA_COMP*) rawParam;
    destroyContexts(comp);
    return RETURN_OK;
}

INSTRUCTION_DEF destroyQueue(FrameData* cframe) {
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_command_queue queue = (cl_command_queue) rawParam;
    clReleaseCommandQueue(queue);
    return RETURN_OK;
}

INSTRUCTION_DEF destroyProgram(FrameData* cframe) {
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_program prog = (cl_program) rawParam;
    clReleaseProgram(prog);
    return RETURN_OK;
}

/*
    * Input: .cl program source code
    * For each platform/context attached to a DANA_COMP attempt to build the program
    * If fails, print the compile errors
    * Return: Array of built program IDs including 0s for those that failed
*/
INSTRUCTION_DEF createProgram(FrameData* cframe) {
    cl_program prog = 0;
    cl_int CL_err = CL_SUCCESS;

    char** programStrings = (char**) malloc(sizeof(char*));
    char* programSource = x_getParam_char_array(api, cframe, 0);
    *programStrings = programSource;

    DANA_COMP* danaComp = (DANA_COMP*) api->getParamInt(cframe, 1);

    cl_device_id device = (cl_device_id) api->getParamInt(cframe, 2);

    CONTEXT_LI* contextItem = getContextByDevice(danaComp, device);

    prog = clCreateProgramWithSource(contextItem->context, 1, (const char**) programStrings, NULL, &CL_err);
    if (CL_err != CL_SUCCESS) {
        addLog(newLogItem("clCreateProgramWithSource", CL_err));
        api->returnInt(cframe, (size_t) 0);
        return RETURN_OK;
    }

    CL_err = CL_SUCCESS;
    CL_err = clBuildProgram(prog, contextItem->numOfDevices, contextItem->devices, NULL, NULL, NULL);
    if (CL_err != CL_SUCCESS) {
        addLog(newLogItem("clBuildProgram", CL_err));
        size_t len;
        char buf[2048];
        printf("CL_err = %d\n", CL_err);
        clGetProgramBuildInfo(prog, *(contextItem->devices), CL_PROGRAM_BUILD_LOG, sizeof(buf), buf, &len);
        printf("%s\n",buf);
        api->returnInt(cframe, (size_t) 0);
        return RETURN_OK;
    }

    api->returnInt(cframe, (size_t) prog);
    free(programStrings);

    return RETURN_OK;
}

/*
    * Input: program ID, number of parameters for the program, the opencl
    * memory objects that make up the parameters, the program name
    *
    * call opencl to create kernel
    * if successful call opencl to set kernel parameters
    *
    * if both succesful return kernel ID
    * if not return 0
*/
INSTRUCTION_DEF prepareKernel(FrameData* cframe) {
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_program program = (cl_program) rawParam;
    rawParam = api->getParamInt(cframe, 2);
    size_t paramCount = (size_t) rawParam;

    DanaEl* paramArray = api->getParamEl(cframe, 1);
    size_t* rawParamArray = (size_t*) malloc(sizeof(size_t)*paramCount);
    size_t* rawParamArrayCpy = rawParamArray;
    for (int i = 0; i < paramCount; i++) {
        *rawParamArrayCpy = (size_t) api->getArrayCellInt(paramArray, i);
        rawParamArrayCpy++;
    }

    char* progName = x_getParam_char_array(api, cframe, 3);

    cl_int CL_err = CL_SUCCESS;
    cl_kernel kernel = clCreateKernel(program, progName, &CL_err);

    if (CL_err != CL_SUCCESS) {
        addLog(newLogItem("clCreateKernel", CL_err));
        api->returnInt(cframe, (size_t) 0);
        return RETURN_OK;
    }

    rawParamArrayCpy = rawParamArray;
    for (int i = 0; i < paramCount; i++) {
        rawParamArrayCpy++;
    }

    CL_err = CL_SUCCESS;
    rawParamArrayCpy = rawParamArray;
    for (int i = 0; i < paramCount; i++) {
        CL_err = clSetKernelArg(kernel, i, sizeof(size_t), rawParamArrayCpy);
        if (CL_err != CL_SUCCESS) {
            addLog(newLogItem("clSetKernelArg", CL_err));
            api->returnInt(cframe, (size_t) 0);
            return RETURN_OK;
        }
        rawParamArrayCpy++;
    }
    CL_err = clSetKernelArg(kernel, paramCount, sizeof(size_t), &paramCount);

    api->returnInt(cframe, (size_t) kernel);
    free(rawParamArray);

    return RETURN_OK;

}

INSTRUCTION_DEF runKernel(FrameData* cframe) {
    size_t rawParam = api->getParamInt(cframe, 0);
    cl_kernel kernel = (cl_kernel) rawParam;

    rawParam = api->getParamInt(cframe, 1);
    cl_command_queue queue = (cl_command_queue) rawParam;

    //create an amount of kernel threads that is
    //equivilent to the size and shape of the output
    //vector/matrix of the kernel parameters
    DanaEl* rawOutputDimentions = api->getParamEl(cframe, 2);

    size_t rawArrLen = api->getArrayLength(rawOutputDimentions);
    
    size_t* globalWorkers = (size_t*) malloc(sizeof(size_t)*rawArrLen);
    for(int i = 0; i < rawArrLen; i++) {
        *(globalWorkers+i) = api->getArrayCellInt(rawOutputDimentions, i);
    }

    //synchronisation handling
    cl_event* kernel_event = (cl_event*) malloc(sizeof(cl_event));

    cl_int CL_err = CL_SUCCESS;
    CL_err = clEnqueueNDRangeKernel(queue, kernel, rawArrLen, NULL, globalWorkers, NULL, 0, NULL, kernel_event);
    if (CL_err != CL_SUCCESS) {
        addLog(newLogItem("clEnqueueNDRangeKernel", CL_err));
        api->returnInt(cframe, (size_t) 1);
        return RETURN_OK;
    }

    //wait for kernel to execute before continuing
    clWaitForEvents(1, kernel_event);

    //clean up
    clReleaseEvent(*kernel_event);
    free(kernel_event);
    free(globalWorkers);
    clRetainKernel(kernel);

    api->returnInt(cframe, (size_t) 0);
    return RETURN_OK;
}

INSTRUCTION_DEF findPlatforms(void) {
    cl_int CL_err = CL_SUCCESS;
    cl_uint numPlatforms = 0;
    
    CL_err = clGetPlatformIDs( 0, NULL, &numPlatforms );
    
    if (CL_err == CL_SUCCESS) {
        printf("%u platform(s) found\n", numPlatforms);
    }
    else {
        printf("clGetPlatformIDs(%i)\n", CL_err);
    }

    return RETURN_OK;
}

Interface* load(CoreAPI* capi) {
    api = capi;

    setInterfaceFunction("findPlatforms", findPlatforms);
    setInterfaceFunction("getComputeDeviceIDs", getComputeDeviceIDs);
    setInterfaceFunction("getComputeDevices", getComputeDevices);
    setInterfaceFunction("init", init);
    setInterfaceFunction("createContext", createContext);
    setInterfaceFunction("createAsynchQueue", createAsynchQueue);
    setInterfaceFunction("createSynchQueue", createSynchQueue);
    setInterfaceFunction("createArray", createArray);
    setInterfaceFunction("writeIntArray", writeIntArray);
    setInterfaceFunction("readIntArray", readIntArray);
    setInterfaceFunction("writeFloatArray", writeFloatArray);
    setInterfaceFunction("readFloatArray", readFloatArray);
    setInterfaceFunction("createMatrix", createMatrix);
    setInterfaceFunction("writeIntMatrix", writeIntMatrix);
    setInterfaceFunction("readIntMatrix", readIntMatrix);
    setInterfaceFunction("writeFloatMatrix", writeFloatMatrix);
    setInterfaceFunction("readFloatMatrix", readFloatMatrix);
    setInterfaceFunction("destroyMemoryArea", destroyMemoryArea);
    setInterfaceFunction("createProgram", createProgram);
    setInterfaceFunction("prepareKernel", prepareKernel);
    setInterfaceFunction("runKernel", runKernel);
    setInterfaceFunction("createContextSpace", createContextSpace);
    setInterfaceFunction("printLogs", printLogs);
    setInterfaceFunction("destroyContextSpace", destroyContextSpace);
    setInterfaceFunction("destroyProgram", destroyProgram);
    setInterfaceFunction("destroyQueue", destroyQueue);

    charArrayGT = api->resolveGlobalTypeMapping(getTypeDefinition("char[]"));
    stringArrayGT = api->resolveGlobalTypeMapping(getTypeDefinition("String[]"));
    stringItemGT = api->resolveGlobalTypeMapping(getTypeDefinition("String"));
    intArrayGT = api->resolveGlobalTypeMapping(getTypeDefinition("int[]"));
    intMatrixGT = api->resolveGlobalTypeMapping(getTypeDefinition("int[][]"));
    decArrayGT = api->resolveGlobalTypeMapping(getTypeDefinition("dec[]"));
    decMatrixGT = api->resolveGlobalTypeMapping(getTypeDefinition("dec[][]"));

    return getPublicInterface();
}

void unload() {
    api->decrementGTRefCount(charArrayGT);
    api->decrementGTRefCount(stringArrayGT);
    api->decrementGTRefCount(stringItemGT);
    api->decrementGTRefCount(intArrayGT);
    api->decrementGTRefCount(intMatrixGT);
    api->decrementGTRefCount(decArrayGT);
    api->decrementGTRefCount(decMatrixGT);
}
