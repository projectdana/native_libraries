#include "dana_lib_defs.h"
#include <string.h>
DanaType emptyType = {TYPE_PATTERN, 0, 0, NULL, 0};
#define ADDRESS_ALIGN true
#define ADDRESS_WIDTH sizeof(size_t)
static const DanaType bool_def = 
{TYPE_LITERAL, 0, 1, NULL, 0};
static const DanaType Object_def;
static const DanaTypeField function_Object_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_Object_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaType char_def = 
{TYPE_LITERAL, 0, 1, NULL, 0};
static const DanaTypeField char_array_fields[] = {
{(DanaType*) &char_def, NULL, 0, 0, 0}};
static const DanaType char_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) char_array_fields, 1};
static const DanaTypeField function_Object_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_Object_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaType Object_functions_spec[] = {
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) function_Object_clone_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) function_Object_equals_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) function_Object_toString_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) function_Object_getID_fields, 1}};
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
static const DanaType byte_def = 
{TYPE_LITERAL, 0, 1, NULL, 0};
static const DanaTypeField byte_array_fields[] = {
{(DanaType*) &byte_def, NULL, 0, 0, 0}};
static const DanaType byte_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) byte_array_fields, 1};
static const DanaType void_def = 
{TYPE_LITERAL, 0, 0, NULL, 0};
static const DanaTypeField function_ZLib_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_ZLib_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_ZLib_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_ZLib_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_ZLib_deflateInit_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_ZLib_deflate_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &byte_array_def, NULL, 0, 0, 48},
{(DanaType*) &bool_def, NULL, 0, 0, 88}};
static const DanaTypeField function_ZLib_deflateEnd_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_ZLib_inflateInit_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_ZLib_inflate_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &byte_array_def, NULL, 0, 0, 48}};
static const DanaTypeField function_ZLib_inflateStatus_fields[] = {
{(DanaType*) &byte_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1}};
static const DanaTypeField function_ZLib_inflateEnd_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8}};
static const DanaType object_ZLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_ZLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_ZLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_ZLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_ZLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_ZLib_deflateInit_fields, 1},
{TYPE_FUNCTION, 0, 89, (DanaTypeField*) &function_ZLib_deflate_fields, 4},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_ZLib_deflateEnd_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_ZLib_inflateInit_fields, 1},
{TYPE_FUNCTION, 0, 88, (DanaTypeField*) &function_ZLib_inflate_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_ZLib_inflateStatus_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_ZLib_inflateEnd_fields, 2}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_ZLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_ZLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_ZLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_ZLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_ZLib_functions_spec[4], "deflateInit", 11},
{(DanaType*) &object_ZLib_functions_spec[5], "deflate", 7},
{(DanaType*) &object_ZLib_functions_spec[6], "deflateEnd", 10},
{(DanaType*) &object_ZLib_functions_spec[7], "inflateInit", 11},
{(DanaType*) &object_ZLib_functions_spec[8], "inflate", 7},
{(DanaType*) &object_ZLib_functions_spec[9], "inflateStatus", 13},
{(DanaType*) &object_ZLib_functions_spec[10], "inflateEnd", 10}};
static const DanaType ZLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 11},
{TYPE_DATA, 0, 0, NULL, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField intf_def[] = {
{(DanaType*) &ZLib_object_spec[0], ".functions", 10},
{(DanaType*) &ZLib_object_spec[1], ".events", 7},
{(DanaType*) &ZLib_object_spec[2], ".state", 6},
};
static unsigned char op_clone_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_equals_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_toString_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getID_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_deflateInit_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_deflate_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_deflateEnd_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_inflateInit_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_inflate_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_inflateStatus_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_inflateEnd_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_deflateInit_thread_spec,
(size_t) op_deflate_thread_spec,
(size_t) op_deflateEnd_thread_spec,
(size_t) op_inflateInit_thread_spec,
(size_t) op_inflate_thread_spec,
(size_t) op_inflateStatus_thread_spec,
(size_t) op_inflateEnd_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"ZLib", 4, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 0, 0, 0, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_ZLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_ZLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_ZLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_ZLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_deflateInit_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_deflateInit_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_deflateInit_thread_spec) -> localsDef = (size_t) &object_ZLib_functions_spec[4];
((VFrameHeader*) op_deflateInit_thread_spec) -> functionName = "deflateInit";
((VFrameHeader*) op_deflate_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 89;
((VFrameHeader*) op_deflate_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_deflate_thread_spec) -> localsDef = (size_t) &object_ZLib_functions_spec[5];
((VFrameHeader*) op_deflate_thread_spec) -> functionName = "deflate";
((VFrameHeader*) op_deflateEnd_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_deflateEnd_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_deflateEnd_thread_spec) -> localsDef = (size_t) &object_ZLib_functions_spec[6];
((VFrameHeader*) op_deflateEnd_thread_spec) -> functionName = "deflateEnd";
((VFrameHeader*) op_inflateInit_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_inflateInit_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_inflateInit_thread_spec) -> localsDef = (size_t) &object_ZLib_functions_spec[7];
((VFrameHeader*) op_inflateInit_thread_spec) -> functionName = "inflateInit";
((VFrameHeader*) op_inflate_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 88;
((VFrameHeader*) op_inflate_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_inflate_thread_spec) -> localsDef = (size_t) &object_ZLib_functions_spec[8];
((VFrameHeader*) op_inflate_thread_spec) -> functionName = "inflate";
((VFrameHeader*) op_inflateStatus_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_inflateStatus_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_inflateStatus_thread_spec) -> localsDef = (size_t) &object_ZLib_functions_spec[9];
((VFrameHeader*) op_inflateStatus_thread_spec) -> functionName = "inflateStatus";
((VFrameHeader*) op_inflateEnd_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_inflateEnd_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_inflateEnd_thread_spec) -> localsDef = (size_t) &object_ZLib_functions_spec[10];
((VFrameHeader*) op_inflateEnd_thread_spec) -> functionName = "inflateEnd";
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
{"deflateInit", (VFrameHeader*) op_deflateInit_thread_spec},
{"deflate", (VFrameHeader*) op_deflate_thread_spec},
{"deflateEnd", (VFrameHeader*) op_deflateEnd_thread_spec},
{"inflateInit", (VFrameHeader*) op_inflateInit_thread_spec},
{"inflate", (VFrameHeader*) op_inflate_thread_spec},
{"inflateStatus", (VFrameHeader*) op_inflateStatus_thread_spec},
{"inflateEnd", (VFrameHeader*) op_inflateEnd_thread_spec}};
void setInterfaceFunction(char *name, void *ptr){
int i = 0;
for (i = 0; i < sizeof(interfaceMappings) / sizeof(Fable); i ++){
if (strcmp(interfaceMappings[i].name, name) == 0){
interfaceMappings[i].hdr -> pcLoc = (unsigned char*) ptr;
interfaceMappings[i].hdr -> registerCount = 1;
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
{"byte[]", &byte_array_def
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
return NULL;
}

