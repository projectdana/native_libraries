#include "dana_lib_defs.h"
#include <string.h>
#include <stdio.h>
DanaType emptyType = {TYPE_PATTERN, 0, 0, NULL, 0};
#define ADDRESS_ALIGN true
#define ADDRESS_WIDTH sizeof(size_t)
static const DanaType bool_def = 
{TYPE_LITERAL, 0x1, 1, NULL, 0};
static const DanaType Object_def;
static const DanaTypeField function_Object_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_Object_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaType char_def = 
{TYPE_LITERAL, 0x2, 1, NULL, 0};
static const DanaTypeField char_array_fields[] = {
{(DanaType*) &char_def, NULL, 0, 0, 0}};
static const DanaType char_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) char_array_fields, 1};
static const DanaTypeField function_Object_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_Object_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaType Object_functions_spec[] = {
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) function_Object_clone_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) function_Object_equals_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) function_Object_toString_fields, 1},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) function_Object_getID_fields, 1}};
static const DanaTypeField Object_functions_spec_fields[] = {
{(DanaType*) &Object_functions_spec[0], "clone", 5},
{(DanaType*) &Object_functions_spec[1], "equals", 6},
{(DanaType*) &Object_functions_spec[2], "toString", 8},
{(DanaType*) &Object_functions_spec[3], "getID", 5}};
static const DanaType Object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) Object_functions_spec_fields, 4},
{TYPE_DATA, 0, 0, NULL, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField Object_spec_fields[] = {
{(DanaType*) &Object_spec[0], ".functions", 10},
{(DanaType*) &Object_spec[1], ".events", 7},
{(DanaType*) &Object_spec[2], ".state", 6},
};
static const DanaType Object_def = {TYPE_OBJECT, 0, 0, (DanaTypeField*) Object_spec_fields, 3};
static const DanaType int_def = 
{TYPE_LITERAL, 0, sizeof(size_t), NULL, 0};
static const DanaType void_def = 
{TYPE_LITERAL, 0, 0, NULL, 0};
static const DanaTypeField int_array_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaType int_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) int_array_fields, 1};
static const DanaTypeField String_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaType String_def = 
{TYPE_DATA, 0, 16, (DanaTypeField*) String_fields, 1};
static const DanaTypeField String_array_fields[] = {
{(DanaType*) &String_def, NULL, 0, 0, 0}};
static const DanaType String_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) String_array_fields, 1};
static const DanaType dec_def = 
{TYPE_DECIMAL, 0, sizeof(size_t)*2, NULL, 0};
static const DanaTypeField dec_array_fields[] = {
{(DanaType*) &dec_def, NULL, 0, 0, 0}};
static const DanaType dec_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) dec_array_fields, 1};
static const DanaTypeField int_a_array_fields[] = {
{(DanaType*) &int_array_def, NULL, 0, 0, 0}};
static const DanaType int_a_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) int_a_array_fields, 1};
static const DanaTypeField dec_a_array_fields[] = {
{(DanaType*) &dec_array_def, NULL, 0, 0, 0}};
static const DanaType dec_a_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) dec_a_array_fields, 1};
static const DanaTypeField function_OpenCLLib_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_OpenCLLib_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_OpenCLLib_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_OpenCLLib_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_OpenCLLib_createContextSpace_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_OpenCLLib_findPlatforms_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0}};
static const DanaTypeField function_OpenCLLib_getComputeDeviceIDs_fields[] = {
{(DanaType*) &int_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_OpenCLLib_getComputeDevices_fields[] = {
{(DanaType*) &String_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_OpenCLLib_createContext_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 24}};
static const DanaTypeField function_OpenCLLib_createAsynchQueue_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_OpenCLLib_createSynchQueue_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_OpenCLLib_createArray_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32}};
static const DanaTypeField function_OpenCLLib_createMatrix_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &int_def, NULL, 0, 0, 40}};
static const DanaTypeField function_OpenCLLib_destroyMemoryArea_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_OpenCLLib_createProgram_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32}};
static const DanaTypeField function_OpenCLLib_writeIntArray_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_array_def, NULL, 0, 0, 24}};
static const DanaTypeField function_OpenCLLib_readIntArray_fields[] = {
{(DanaType*) &int_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32}};
static const DanaTypeField function_OpenCLLib_writeFloatArray_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &dec_array_def, NULL, 0, 0, 24}};
static const DanaTypeField function_OpenCLLib_readFloatArray_fields[] = {
{(DanaType*) &dec_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32}};
static const DanaTypeField function_OpenCLLib_writeIntMatrix_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_a_array_def, NULL, 0, 0, 24}};
static const DanaTypeField function_OpenCLLib_readIntMatrix_fields[] = {
{(DanaType*) &int_a_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_array_def, NULL, 0, 0, 32}};
static const DanaTypeField function_OpenCLLib_writeFloatMatrix_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &dec_a_array_def, NULL, 0, 0, 24}};
static const DanaTypeField function_OpenCLLib_readFloatMatrix_fields[] = {
{(DanaType*) &dec_a_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_array_def, NULL, 0, 0, 32}};
static const DanaTypeField function_OpenCLLib_prepareKernel_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_array_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_OpenCLLib_runKernel_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_array_def, NULL, 0, 0, 24}};
static const DanaTypeField function_OpenCLLib_destroyContextSpace_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_OpenCLLib_destroyQueue_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_OpenCLLib_destroyProgram_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_OpenCLLib_printLogs_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0}};
static const DanaType object_OpenCLLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_OpenCLLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_OpenCLLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_OpenCLLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_OpenCLLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_OpenCLLib_createContextSpace_fields, 1},
{TYPE_FUNCTION, 0, 0, (DanaTypeField*) &function_OpenCLLib_findPlatforms_fields, 1},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_OpenCLLib_getComputeDeviceIDs_fields, 1},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_OpenCLLib_getComputeDevices_fields, 1},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_OpenCLLib_createContext_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_OpenCLLib_createAsynchQueue_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_OpenCLLib_createSynchQueue_fields, 3},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_OpenCLLib_createArray_fields, 5},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_OpenCLLib_createMatrix_fields, 6},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_OpenCLLib_destroyMemoryArea_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_OpenCLLib_createProgram_fields, 4},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_OpenCLLib_writeIntArray_fields, 4},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_OpenCLLib_readIntArray_fields, 4},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_OpenCLLib_writeFloatArray_fields, 4},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_OpenCLLib_readFloatArray_fields, 4},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_OpenCLLib_writeIntMatrix_fields, 4},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_OpenCLLib_readIntMatrix_fields, 4},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_OpenCLLib_writeFloatMatrix_fields, 4},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_OpenCLLib_readFloatMatrix_fields, 4},
{TYPE_FUNCTION, 0, 56, (DanaTypeField*) &function_OpenCLLib_prepareKernel_fields, 5},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_OpenCLLib_runKernel_fields, 4},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_OpenCLLib_destroyContextSpace_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_OpenCLLib_destroyQueue_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_OpenCLLib_destroyProgram_fields, 2},
{TYPE_FUNCTION, 0, 0, (DanaTypeField*) &function_OpenCLLib_printLogs_fields, 1}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_OpenCLLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_OpenCLLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_OpenCLLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_OpenCLLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_OpenCLLib_functions_spec[4], "createContextSpace", 18},
{(DanaType*) &object_OpenCLLib_functions_spec[5], "findPlatforms", 13},
{(DanaType*) &object_OpenCLLib_functions_spec[6], "getComputeDeviceIDs", 19},
{(DanaType*) &object_OpenCLLib_functions_spec[7], "getComputeDevices", 17},
{(DanaType*) &object_OpenCLLib_functions_spec[8], "createContext", 13},
{(DanaType*) &object_OpenCLLib_functions_spec[9], "createAsynchQueue", 17},
{(DanaType*) &object_OpenCLLib_functions_spec[10], "createSynchQueue", 16},
{(DanaType*) &object_OpenCLLib_functions_spec[11], "createArray", 11},
{(DanaType*) &object_OpenCLLib_functions_spec[12], "createMatrix", 12},
{(DanaType*) &object_OpenCLLib_functions_spec[13], "destroyMemoryArea", 17},
{(DanaType*) &object_OpenCLLib_functions_spec[14], "createProgram", 13},
{(DanaType*) &object_OpenCLLib_functions_spec[15], "writeIntArray", 13},
{(DanaType*) &object_OpenCLLib_functions_spec[16], "readIntArray", 12},
{(DanaType*) &object_OpenCLLib_functions_spec[17], "writeFloatArray", 15},
{(DanaType*) &object_OpenCLLib_functions_spec[18], "readFloatArray", 14},
{(DanaType*) &object_OpenCLLib_functions_spec[19], "writeIntMatrix", 14},
{(DanaType*) &object_OpenCLLib_functions_spec[20], "readIntMatrix", 13},
{(DanaType*) &object_OpenCLLib_functions_spec[21], "writeFloatMatrix", 16},
{(DanaType*) &object_OpenCLLib_functions_spec[22], "readFloatMatrix", 15},
{(DanaType*) &object_OpenCLLib_functions_spec[23], "prepareKernel", 13},
{(DanaType*) &object_OpenCLLib_functions_spec[24], "runKernel", 9},
{(DanaType*) &object_OpenCLLib_functions_spec[25], "destroyContextSpace", 19},
{(DanaType*) &object_OpenCLLib_functions_spec[26], "destroyQueue", 12},
{(DanaType*) &object_OpenCLLib_functions_spec[27], "destroyProgram", 14},
{(DanaType*) &object_OpenCLLib_functions_spec[28], "printLogs", 9}};
static const DanaTypeField intf_events_def[] = {
};
static const DanaType OpenCLLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 29},
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_events_def, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField intf_def[] = {
{(DanaType*) &OpenCLLib_object_spec[0], ".functions", 10},
{(DanaType*) &OpenCLLib_object_spec[1], ".events", 7},
{(DanaType*) &OpenCLLib_object_spec[2], ".state", 6},
};
static unsigned char op_clone_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_equals_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_toString_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getID_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_createContextSpace_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_findPlatforms_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getComputeDeviceIDs_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getComputeDevices_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_createContext_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_createAsynchQueue_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_createSynchQueue_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_createArray_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_createMatrix_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_destroyMemoryArea_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_createProgram_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_writeIntArray_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_readIntArray_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_writeFloatArray_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_readFloatArray_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_writeIntMatrix_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_readIntMatrix_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_writeFloatMatrix_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_readFloatMatrix_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_prepareKernel_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_runKernel_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_destroyContextSpace_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_destroyQueue_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_destroyProgram_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_printLogs_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_createContextSpace_thread_spec,
(size_t) op_findPlatforms_thread_spec,
(size_t) op_getComputeDeviceIDs_thread_spec,
(size_t) op_getComputeDevices_thread_spec,
(size_t) op_createContext_thread_spec,
(size_t) op_createAsynchQueue_thread_spec,
(size_t) op_createSynchQueue_thread_spec,
(size_t) op_createArray_thread_spec,
(size_t) op_createMatrix_thread_spec,
(size_t) op_destroyMemoryArea_thread_spec,
(size_t) op_createProgram_thread_spec,
(size_t) op_writeIntArray_thread_spec,
(size_t) op_readIntArray_thread_spec,
(size_t) op_writeFloatArray_thread_spec,
(size_t) op_readFloatArray_thread_spec,
(size_t) op_writeIntMatrix_thread_spec,
(size_t) op_readIntMatrix_thread_spec,
(size_t) op_writeFloatMatrix_thread_spec,
(size_t) op_readFloatMatrix_thread_spec,
(size_t) op_prepareKernel_thread_spec,
(size_t) op_runKernel_thread_spec,
(size_t) op_destroyContextSpace_thread_spec,
(size_t) op_destroyQueue_thread_spec,
(size_t) op_destroyProgram_thread_spec,
(size_t) op_printLogs_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"OpenCLLib", 9, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 0, 0, (size_t) &bool_def, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> sub = NULL;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> sub = NULL;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> sub = NULL;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_createContextSpace_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_createContextSpace_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_createContextSpace_thread_spec) -> sub = NULL;
((VFrameHeader*) op_createContextSpace_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[4];
((VFrameHeader*) op_createContextSpace_thread_spec) -> functionName = "createContextSpace";
((VFrameHeader*) op_findPlatforms_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 0;
((VFrameHeader*) op_findPlatforms_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_findPlatforms_thread_spec) -> sub = NULL;
((VFrameHeader*) op_findPlatforms_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[5];
((VFrameHeader*) op_findPlatforms_thread_spec) -> functionName = "findPlatforms";
((VFrameHeader*) op_getComputeDeviceIDs_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_getComputeDeviceIDs_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getComputeDeviceIDs_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getComputeDeviceIDs_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[6];
((VFrameHeader*) op_getComputeDeviceIDs_thread_spec) -> functionName = "getComputeDeviceIDs";
((VFrameHeader*) op_getComputeDevices_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_getComputeDevices_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getComputeDevices_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getComputeDevices_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[7];
((VFrameHeader*) op_getComputeDevices_thread_spec) -> functionName = "getComputeDevices";
((VFrameHeader*) op_createContext_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_createContext_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_createContext_thread_spec) -> sub = NULL;
((VFrameHeader*) op_createContext_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[8];
((VFrameHeader*) op_createContext_thread_spec) -> functionName = "createContext";
((VFrameHeader*) op_createAsynchQueue_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_createAsynchQueue_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_createAsynchQueue_thread_spec) -> sub = NULL;
((VFrameHeader*) op_createAsynchQueue_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[9];
((VFrameHeader*) op_createAsynchQueue_thread_spec) -> functionName = "createAsynchQueue";
((VFrameHeader*) op_createSynchQueue_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_createSynchQueue_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_createSynchQueue_thread_spec) -> sub = NULL;
((VFrameHeader*) op_createSynchQueue_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[10];
((VFrameHeader*) op_createSynchQueue_thread_spec) -> functionName = "createSynchQueue";
((VFrameHeader*) op_createArray_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_createArray_thread_spec) -> formalParamsCount = 4;
((VFrameHeader*) op_createArray_thread_spec) -> sub = NULL;
((VFrameHeader*) op_createArray_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[11];
((VFrameHeader*) op_createArray_thread_spec) -> functionName = "createArray";
((VFrameHeader*) op_createMatrix_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_createMatrix_thread_spec) -> formalParamsCount = 5;
((VFrameHeader*) op_createMatrix_thread_spec) -> sub = NULL;
((VFrameHeader*) op_createMatrix_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[12];
((VFrameHeader*) op_createMatrix_thread_spec) -> functionName = "createMatrix";
((VFrameHeader*) op_destroyMemoryArea_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_destroyMemoryArea_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_destroyMemoryArea_thread_spec) -> sub = NULL;
((VFrameHeader*) op_destroyMemoryArea_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[13];
((VFrameHeader*) op_destroyMemoryArea_thread_spec) -> functionName = "destroyMemoryArea";
((VFrameHeader*) op_createProgram_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_createProgram_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_createProgram_thread_spec) -> sub = NULL;
((VFrameHeader*) op_createProgram_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[14];
((VFrameHeader*) op_createProgram_thread_spec) -> functionName = "createProgram";
((VFrameHeader*) op_writeIntArray_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_writeIntArray_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_writeIntArray_thread_spec) -> sub = NULL;
((VFrameHeader*) op_writeIntArray_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[15];
((VFrameHeader*) op_writeIntArray_thread_spec) -> functionName = "writeIntArray";
((VFrameHeader*) op_readIntArray_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_readIntArray_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_readIntArray_thread_spec) -> sub = NULL;
((VFrameHeader*) op_readIntArray_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[16];
((VFrameHeader*) op_readIntArray_thread_spec) -> functionName = "readIntArray";
((VFrameHeader*) op_writeFloatArray_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_writeFloatArray_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_writeFloatArray_thread_spec) -> sub = NULL;
((VFrameHeader*) op_writeFloatArray_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[17];
((VFrameHeader*) op_writeFloatArray_thread_spec) -> functionName = "writeFloatArray";
((VFrameHeader*) op_readFloatArray_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_readFloatArray_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_readFloatArray_thread_spec) -> sub = NULL;
((VFrameHeader*) op_readFloatArray_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[18];
((VFrameHeader*) op_readFloatArray_thread_spec) -> functionName = "readFloatArray";
((VFrameHeader*) op_writeIntMatrix_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_writeIntMatrix_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_writeIntMatrix_thread_spec) -> sub = NULL;
((VFrameHeader*) op_writeIntMatrix_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[19];
((VFrameHeader*) op_writeIntMatrix_thread_spec) -> functionName = "writeIntMatrix";
((VFrameHeader*) op_readIntMatrix_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_readIntMatrix_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_readIntMatrix_thread_spec) -> sub = NULL;
((VFrameHeader*) op_readIntMatrix_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[20];
((VFrameHeader*) op_readIntMatrix_thread_spec) -> functionName = "readIntMatrix";
((VFrameHeader*) op_writeFloatMatrix_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_writeFloatMatrix_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_writeFloatMatrix_thread_spec) -> sub = NULL;
((VFrameHeader*) op_writeFloatMatrix_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[21];
((VFrameHeader*) op_writeFloatMatrix_thread_spec) -> functionName = "writeFloatMatrix";
((VFrameHeader*) op_readFloatMatrix_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_readFloatMatrix_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_readFloatMatrix_thread_spec) -> sub = NULL;
((VFrameHeader*) op_readFloatMatrix_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[22];
((VFrameHeader*) op_readFloatMatrix_thread_spec) -> functionName = "readFloatMatrix";
((VFrameHeader*) op_prepareKernel_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 56;
((VFrameHeader*) op_prepareKernel_thread_spec) -> formalParamsCount = 4;
((VFrameHeader*) op_prepareKernel_thread_spec) -> sub = NULL;
((VFrameHeader*) op_prepareKernel_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[23];
((VFrameHeader*) op_prepareKernel_thread_spec) -> functionName = "prepareKernel";
((VFrameHeader*) op_runKernel_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_runKernel_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_runKernel_thread_spec) -> sub = NULL;
((VFrameHeader*) op_runKernel_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[24];
((VFrameHeader*) op_runKernel_thread_spec) -> functionName = "runKernel";
((VFrameHeader*) op_destroyContextSpace_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_destroyContextSpace_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_destroyContextSpace_thread_spec) -> sub = NULL;
((VFrameHeader*) op_destroyContextSpace_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[25];
((VFrameHeader*) op_destroyContextSpace_thread_spec) -> functionName = "destroyContextSpace";
((VFrameHeader*) op_destroyQueue_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_destroyQueue_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_destroyQueue_thread_spec) -> sub = NULL;
((VFrameHeader*) op_destroyQueue_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[26];
((VFrameHeader*) op_destroyQueue_thread_spec) -> functionName = "destroyQueue";
((VFrameHeader*) op_destroyProgram_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_destroyProgram_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_destroyProgram_thread_spec) -> sub = NULL;
((VFrameHeader*) op_destroyProgram_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[27];
((VFrameHeader*) op_destroyProgram_thread_spec) -> functionName = "destroyProgram";
((VFrameHeader*) op_printLogs_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 0;
((VFrameHeader*) op_printLogs_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_printLogs_thread_spec) -> sub = NULL;
((VFrameHeader*) op_printLogs_thread_spec) -> localsDef = (size_t) &object_OpenCLLib_functions_spec[28];
((VFrameHeader*) op_printLogs_thread_spec) -> functionName = "printLogs";
memset(&self, '\0', sizeof(self));
self.objects = objects; self.header = &header; self.header -> objectsCount = sizeof(objects) / sizeof(ObjectSpec);
objectInterfaces[0].lbp.spec = &objects[0];
return &objectInterfaces[0];
}

typedef struct{
char *name;
VFrameHeader *hdr;
} Fable;

static Fable interfaceMappings[] = {
{"clone", (VFrameHeader*) op_clone_thread_spec},
{"equals", (VFrameHeader*) op_equals_thread_spec},
{"toString", (VFrameHeader*) op_toString_thread_spec},
{"getID", (VFrameHeader*) op_getID_thread_spec},
{"createContextSpace", (VFrameHeader*) op_createContextSpace_thread_spec},
{"findPlatforms", (VFrameHeader*) op_findPlatforms_thread_spec},
{"getComputeDeviceIDs", (VFrameHeader*) op_getComputeDeviceIDs_thread_spec},
{"getComputeDevices", (VFrameHeader*) op_getComputeDevices_thread_spec},
{"createContext", (VFrameHeader*) op_createContext_thread_spec},
{"createAsynchQueue", (VFrameHeader*) op_createAsynchQueue_thread_spec},
{"createSynchQueue", (VFrameHeader*) op_createSynchQueue_thread_spec},
{"createArray", (VFrameHeader*) op_createArray_thread_spec},
{"createMatrix", (VFrameHeader*) op_createMatrix_thread_spec},
{"destroyMemoryArea", (VFrameHeader*) op_destroyMemoryArea_thread_spec},
{"createProgram", (VFrameHeader*) op_createProgram_thread_spec},
{"writeIntArray", (VFrameHeader*) op_writeIntArray_thread_spec},
{"readIntArray", (VFrameHeader*) op_readIntArray_thread_spec},
{"writeFloatArray", (VFrameHeader*) op_writeFloatArray_thread_spec},
{"readFloatArray", (VFrameHeader*) op_readFloatArray_thread_spec},
{"writeIntMatrix", (VFrameHeader*) op_writeIntMatrix_thread_spec},
{"readIntMatrix", (VFrameHeader*) op_readIntMatrix_thread_spec},
{"writeFloatMatrix", (VFrameHeader*) op_writeFloatMatrix_thread_spec},
{"readFloatMatrix", (VFrameHeader*) op_readFloatMatrix_thread_spec},
{"prepareKernel", (VFrameHeader*) op_prepareKernel_thread_spec},
{"runKernel", (VFrameHeader*) op_runKernel_thread_spec},
{"destroyContextSpace", (VFrameHeader*) op_destroyContextSpace_thread_spec},
{"destroyQueue", (VFrameHeader*) op_destroyQueue_thread_spec},
{"destroyProgram", (VFrameHeader*) op_destroyProgram_thread_spec},
{"printLogs", (VFrameHeader*) op_printLogs_thread_spec}};
void setInterfaceFunction(char *name, void *ptr){
int i = 0;
for (i = 0; i < sizeof(interfaceMappings) / sizeof(Fable); i ++){
if (strcmp(interfaceMappings[i].name, name) == 0){
interfaceMappings[i].hdr -> pcLoc = (unsigned char*) ptr;
interfaceMappings[i].hdr -> sub = NULL;
interfaceMappings[i].hdr -> localsDef = 0;
break;
}
}
}

typedef struct{
char *name;
const DanaType *dataType;
} Ex;

static Ex dataMappings[] = {
{"dec[][]", &dec_a_array_def
},
{"int[][]", &int_a_array_def
},
{"dec[]", &dec_array_def
},
{"String", &String_def
},
{"String[]", &String_array_def
},
{"int[]", &int_array_def
},
{"char[]", &char_array_def
}};
const DanaType* getTypeDefinition(char *name){
int i = 0;
for (i = 0; i < sizeof(dataMappings) / sizeof(Ex); i ++){
if (strcmp(dataMappings[i].name, name) == 0){
return dataMappings[i].dataType;
}
}
printf("Exception::type '%s' is not referenced by associated Dana interface '%s' of native library", name, ids[0].name);
return NULL;
}

