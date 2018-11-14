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
static const DanaTypeField function_SystemLib_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_SystemLib_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_SystemLib_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SystemLib_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SystemLib_getPlatformName_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SystemLib_getPlatformVersion_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SystemLib_getChipName_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SystemLib_getHostName_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SystemLib_getVar_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_SystemLib_getSystemFont_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &bool_def, NULL, 0, 0, 40}};
static const DanaType object_SystemLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_SystemLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_SystemLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_SystemLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_SystemLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_SystemLib_getPlatformName_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_SystemLib_getPlatformVersion_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_SystemLib_getChipName_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_SystemLib_getHostName_fields, 1},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_SystemLib_getVar_fields, 2},
{TYPE_FUNCTION, 0, 41, (DanaTypeField*) &function_SystemLib_getSystemFont_fields, 2}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_SystemLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_SystemLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_SystemLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_SystemLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_SystemLib_functions_spec[4], "getPlatformName", 15},
{(DanaType*) &object_SystemLib_functions_spec[5], "getPlatformVersion", 18},
{(DanaType*) &object_SystemLib_functions_spec[6], "getChipName", 11},
{(DanaType*) &object_SystemLib_functions_spec[7], "getHostName", 11},
{(DanaType*) &object_SystemLib_functions_spec[8], "getVar", 6},
{(DanaType*) &object_SystemLib_functions_spec[9], "getSystemFont", 13}};
static const DanaType SystemLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 10},
{TYPE_DATA, 0, 0, NULL, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField intf_def[] = {
{(DanaType*) &SystemLib_object_spec[0], ".functions", 10},
{(DanaType*) &SystemLib_object_spec[1], ".events", 7},
{(DanaType*) &SystemLib_object_spec[2], ".state", 6},
};
static unsigned char op_clone_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_equals_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_toString_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getID_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getPlatformName_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getPlatformVersion_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getChipName_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getHostName_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getVar_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getSystemFont_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_getPlatformName_thread_spec,
(size_t) op_getPlatformVersion_thread_spec,
(size_t) op_getChipName_thread_spec,
(size_t) op_getHostName_thread_spec,
(size_t) op_getVar_thread_spec,
(size_t) op_getSystemFont_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"SystemLib", 9, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 0, 0, 0, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_SystemLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_SystemLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_SystemLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_SystemLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_getPlatformName_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getPlatformName_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getPlatformName_thread_spec) -> localsDef = (size_t) &object_SystemLib_functions_spec[4];
((VFrameHeader*) op_getPlatformName_thread_spec) -> functionName = "getPlatformName";
((VFrameHeader*) op_getPlatformVersion_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getPlatformVersion_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getPlatformVersion_thread_spec) -> localsDef = (size_t) &object_SystemLib_functions_spec[5];
((VFrameHeader*) op_getPlatformVersion_thread_spec) -> functionName = "getPlatformVersion";
((VFrameHeader*) op_getChipName_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getChipName_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getChipName_thread_spec) -> localsDef = (size_t) &object_SystemLib_functions_spec[6];
((VFrameHeader*) op_getChipName_thread_spec) -> functionName = "getChipName";
((VFrameHeader*) op_getHostName_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getHostName_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getHostName_thread_spec) -> localsDef = (size_t) &object_SystemLib_functions_spec[7];
((VFrameHeader*) op_getHostName_thread_spec) -> functionName = "getHostName";
((VFrameHeader*) op_getVar_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_getVar_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_getVar_thread_spec) -> localsDef = (size_t) &object_SystemLib_functions_spec[8];
((VFrameHeader*) op_getVar_thread_spec) -> functionName = "getVar";
((VFrameHeader*) op_getSystemFont_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 41;
((VFrameHeader*) op_getSystemFont_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_getSystemFont_thread_spec) -> localsDef = (size_t) &object_SystemLib_functions_spec[9];
((VFrameHeader*) op_getSystemFont_thread_spec) -> functionName = "getSystemFont";
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
{"getPlatformName", (VFrameHeader*) op_getPlatformName_thread_spec},
{"getPlatformVersion", (VFrameHeader*) op_getPlatformVersion_thread_spec},
{"getChipName", (VFrameHeader*) op_getChipName_thread_spec},
{"getHostName", (VFrameHeader*) op_getHostName_thread_spec},
{"getVar", (VFrameHeader*) op_getVar_thread_spec},
{"getSystemFont", (VFrameHeader*) op_getSystemFont_thread_spec}};
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

