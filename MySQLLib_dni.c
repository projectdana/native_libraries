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
static const DanaType void_def = 
{TYPE_LITERAL, 0, 0, NULL, 0};
static const DanaTypeField function_MySQLLib_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_MySQLLib_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_MySQLLib_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_MySQLLib_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_MySQLLib_connect_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 48},
{(DanaType*) &char_array_def, NULL, 0, 0, 56},
{(DanaType*) &char_array_def, NULL, 0, 0, 96},
{(DanaType*) &char_array_def, NULL, 0, 0, 136}};
static const DanaTypeField function_MySQLLib_executeQuery_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &char_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_MySQLLib_fetchRow_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8}};
static const DanaTypeField function_MySQLLib_numFields_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8}};
static const DanaTypeField function_MySQLLib_getField_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &int_def, NULL, 0, 0, 48}};
static const DanaTypeField function_MySQLLib_getFieldName_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MySQLLib_close_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8}};
static const DanaType object_MySQLLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_MySQLLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_MySQLLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_MySQLLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_MySQLLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 176, (DanaTypeField*) &function_MySQLLib_connect_fields, 6},
{TYPE_FUNCTION, 0, 56, (DanaTypeField*) &function_MySQLLib_executeQuery_fields, 3},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_MySQLLib_fetchRow_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_MySQLLib_numFields_fields, 2},
{TYPE_FUNCTION, 0, 56, (DanaTypeField*) &function_MySQLLib_getField_fields, 3},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_MySQLLib_getFieldName_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_MySQLLib_close_fields, 3}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_MySQLLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_MySQLLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_MySQLLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_MySQLLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_MySQLLib_functions_spec[4], "connect", 7},
{(DanaType*) &object_MySQLLib_functions_spec[5], "executeQuery", 12},
{(DanaType*) &object_MySQLLib_functions_spec[6], "fetchRow", 8},
{(DanaType*) &object_MySQLLib_functions_spec[7], "numFields", 9},
{(DanaType*) &object_MySQLLib_functions_spec[8], "getField", 8},
{(DanaType*) &object_MySQLLib_functions_spec[9], "getFieldName", 12},
{(DanaType*) &object_MySQLLib_functions_spec[10], "close", 5}};
static const DanaType MySQLLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 11},
{TYPE_DATA, 0, 0, NULL, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField intf_def[] = {
{(DanaType*) &MySQLLib_object_spec[0], ".functions", 10},
{(DanaType*) &MySQLLib_object_spec[1], ".events", 7},
{(DanaType*) &MySQLLib_object_spec[2], ".state", 6},
};
static unsigned char op_clone_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_equals_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_toString_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getID_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_connect_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_executeQuery_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_fetchRow_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_numFields_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getField_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getFieldName_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_close_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_connect_thread_spec,
(size_t) op_executeQuery_thread_spec,
(size_t) op_fetchRow_thread_spec,
(size_t) op_numFields_thread_spec,
(size_t) op_getField_thread_spec,
(size_t) op_getFieldName_thread_spec,
(size_t) op_close_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"MySQLLib", 8, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 0, 0, 0, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> sub = NULL;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_MySQLLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> sub = NULL;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_MySQLLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> sub = NULL;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_MySQLLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_MySQLLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_connect_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 176;
((VFrameHeader*) op_connect_thread_spec) -> formalParamsCount = 5;
((VFrameHeader*) op_connect_thread_spec) -> sub = NULL;
((VFrameHeader*) op_connect_thread_spec) -> localsDef = (size_t) &object_MySQLLib_functions_spec[4];
((VFrameHeader*) op_connect_thread_spec) -> functionName = "connect";
((VFrameHeader*) op_executeQuery_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 56;
((VFrameHeader*) op_executeQuery_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_executeQuery_thread_spec) -> sub = NULL;
((VFrameHeader*) op_executeQuery_thread_spec) -> localsDef = (size_t) &object_MySQLLib_functions_spec[5];
((VFrameHeader*) op_executeQuery_thread_spec) -> functionName = "executeQuery";
((VFrameHeader*) op_fetchRow_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_fetchRow_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_fetchRow_thread_spec) -> sub = NULL;
((VFrameHeader*) op_fetchRow_thread_spec) -> localsDef = (size_t) &object_MySQLLib_functions_spec[6];
((VFrameHeader*) op_fetchRow_thread_spec) -> functionName = "fetchRow";
((VFrameHeader*) op_numFields_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_numFields_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_numFields_thread_spec) -> sub = NULL;
((VFrameHeader*) op_numFields_thread_spec) -> localsDef = (size_t) &object_MySQLLib_functions_spec[7];
((VFrameHeader*) op_numFields_thread_spec) -> functionName = "numFields";
((VFrameHeader*) op_getField_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 56;
((VFrameHeader*) op_getField_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_getField_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getField_thread_spec) -> localsDef = (size_t) &object_MySQLLib_functions_spec[8];
((VFrameHeader*) op_getField_thread_spec) -> functionName = "getField";
((VFrameHeader*) op_getFieldName_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_getFieldName_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_getFieldName_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getFieldName_thread_spec) -> localsDef = (size_t) &object_MySQLLib_functions_spec[9];
((VFrameHeader*) op_getFieldName_thread_spec) -> functionName = "getFieldName";
((VFrameHeader*) op_close_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_close_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_close_thread_spec) -> sub = NULL;
((VFrameHeader*) op_close_thread_spec) -> localsDef = (size_t) &object_MySQLLib_functions_spec[10];
((VFrameHeader*) op_close_thread_spec) -> functionName = "close";
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
{"connect", (VFrameHeader*) op_connect_thread_spec},
{"executeQuery", (VFrameHeader*) op_executeQuery_thread_spec},
{"fetchRow", (VFrameHeader*) op_fetchRow_thread_spec},
{"numFields", (VFrameHeader*) op_numFields_thread_spec},
{"getField", (VFrameHeader*) op_getField_thread_spec},
{"getFieldName", (VFrameHeader*) op_getFieldName_thread_spec},
{"close", (VFrameHeader*) op_close_thread_spec}};
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

