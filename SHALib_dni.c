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
static const DanaType byte_def = 
{TYPE_LITERAL, 0, 1, NULL, 0};
static const DanaTypeField byte_array_fields[] = {
{(DanaType*) &byte_def, NULL, 0, 0, 0}};
static const DanaType byte_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) byte_array_fields, 1};
static const DanaTypeField function_SHALib_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_SHALib_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_SHALib_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SHALib_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SHALib_sha1_hashData_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &byte_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_SHALib_sha2_hashData_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &byte_array_def, NULL, 0, 0, 16},
{(DanaType*) &byte_def, NULL, 0, 0, 32}};
static const DanaTypeField function_SHALib_sha3_hashData_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &byte_array_def, NULL, 0, 0, 16},
{(DanaType*) &byte_def, NULL, 0, 0, 32}};
static const DanaType object_SHALib_functions_spec[] = {
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_SHALib_clone_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_SHALib_equals_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_SHALib_toString_fields, 1},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_SHALib_getID_fields, 1},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_SHALib_sha1_hashData_fields, 2},
{TYPE_FUNCTION, 0, 33, (DanaTypeField*) &function_SHALib_sha2_hashData_fields, 3},
{TYPE_FUNCTION, 0, 33, (DanaTypeField*) &function_SHALib_sha3_hashData_fields, 3}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_SHALib_functions_spec[0], "clone", 5},
{(DanaType*) &object_SHALib_functions_spec[1], "equals", 6},
{(DanaType*) &object_SHALib_functions_spec[2], "toString", 8},
{(DanaType*) &object_SHALib_functions_spec[3], "getID", 5},
{(DanaType*) &object_SHALib_functions_spec[4], "sha1_hashData", 13},
{(DanaType*) &object_SHALib_functions_spec[5], "sha2_hashData", 13},
{(DanaType*) &object_SHALib_functions_spec[6], "sha3_hashData", 13}};
static const DanaType object_SHALib_events_spec[] = {
};
static const DanaTypeField intf_events_def[] = {
};
static const DanaType SHALib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 7},
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_events_def, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField intf_def[] = {
{(DanaType*) &SHALib_object_spec[0], ".functions", 10},
{(DanaType*) &SHALib_object_spec[1], ".events", 7},
{(DanaType*) &SHALib_object_spec[2], ".state", 6},
};
static unsigned char op_clone_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_equals_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_toString_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getID_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_sha1_hashData_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_sha2_hashData_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_sha3_hashData_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_sha1_hashData_thread_spec,
(size_t) op_sha2_hashData_thread_spec,
(size_t) op_sha3_hashData_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"SHALib", 6, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 0, 0, (size_t) &bool_def, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> sub = NULL;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_SHALib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> sub = NULL;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_SHALib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> sub = NULL;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_SHALib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_SHALib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_sha1_hashData_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_sha1_hashData_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_sha1_hashData_thread_spec) -> sub = NULL;
((VFrameHeader*) op_sha1_hashData_thread_spec) -> localsDef = (size_t) &object_SHALib_functions_spec[4];
((VFrameHeader*) op_sha1_hashData_thread_spec) -> functionName = "sha1_hashData";
((VFrameHeader*) op_sha2_hashData_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 33;
((VFrameHeader*) op_sha2_hashData_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_sha2_hashData_thread_spec) -> sub = NULL;
((VFrameHeader*) op_sha2_hashData_thread_spec) -> localsDef = (size_t) &object_SHALib_functions_spec[5];
((VFrameHeader*) op_sha2_hashData_thread_spec) -> functionName = "sha2_hashData";
((VFrameHeader*) op_sha3_hashData_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 33;
((VFrameHeader*) op_sha3_hashData_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_sha3_hashData_thread_spec) -> sub = NULL;
((VFrameHeader*) op_sha3_hashData_thread_spec) -> localsDef = (size_t) &object_SHALib_functions_spec[6];
((VFrameHeader*) op_sha3_hashData_thread_spec) -> functionName = "sha3_hashData";
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
{"sha1_hashData", (VFrameHeader*) op_sha1_hashData_thread_spec},
{"sha2_hashData", (VFrameHeader*) op_sha2_hashData_thread_spec},
{"sha3_hashData", (VFrameHeader*) op_sha3_hashData_thread_spec}};
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

