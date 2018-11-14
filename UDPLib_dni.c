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
static const DanaTypeField Datagram_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &byte_array_def, NULL, 0, 0, 0}};
static const DanaType Datagram_def = 
{TYPE_DATA, 0, 88, (DanaTypeField*) Datagram_fields, 3};
static const DanaType void_def = 
{TYPE_LITERAL, 0, 0, NULL, 0};
static const DanaTypeField function_UDPLib_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_UDPLib_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_UDPLib_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UDPLib_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UDPLib_send_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 48},
{(DanaType*) &byte_array_def, NULL, 0, 0, 56}};
static const DanaTypeField function_UDPLib_recv_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &Datagram_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UDPLib_bind_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 48}};
static const DanaTypeField function_UDPLib_unbind_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaType object_UDPLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_UDPLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_UDPLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_UDPLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_UDPLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 96, (DanaTypeField*) &function_UDPLib_send_fields, 4},
{TYPE_FUNCTION, 0, 56, (DanaTypeField*) &function_UDPLib_recv_fields, 3},
{TYPE_FUNCTION, 0, 56, (DanaTypeField*) &function_UDPLib_bind_fields, 3},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UDPLib_unbind_fields, 2}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_UDPLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_UDPLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_UDPLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_UDPLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_UDPLib_functions_spec[4], "send", 4},
{(DanaType*) &object_UDPLib_functions_spec[5], "recv", 4},
{(DanaType*) &object_UDPLib_functions_spec[6], "bind", 4},
{(DanaType*) &object_UDPLib_functions_spec[7], "unbind", 6}};
static const DanaType UDPLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 8},
{TYPE_DATA, 0, 0, NULL, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField intf_def[] = {
{(DanaType*) &UDPLib_object_spec[0], ".functions", 10},
{(DanaType*) &UDPLib_object_spec[1], ".events", 7},
{(DanaType*) &UDPLib_object_spec[2], ".state", 6},
};
static unsigned char op_clone_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_equals_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_toString_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getID_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_send_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_recv_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_bind_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_unbind_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_send_thread_spec,
(size_t) op_recv_thread_spec,
(size_t) op_bind_thread_spec,
(size_t) op_unbind_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"UDPLib", 6, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 0, 0, 0, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_send_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 96;
((VFrameHeader*) op_send_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_send_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[4];
((VFrameHeader*) op_send_thread_spec) -> functionName = "send";
((VFrameHeader*) op_recv_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 56;
((VFrameHeader*) op_recv_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_recv_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[5];
((VFrameHeader*) op_recv_thread_spec) -> functionName = "recv";
((VFrameHeader*) op_bind_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 56;
((VFrameHeader*) op_bind_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_bind_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[6];
((VFrameHeader*) op_bind_thread_spec) -> functionName = "bind";
((VFrameHeader*) op_unbind_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_unbind_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_unbind_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[7];
((VFrameHeader*) op_unbind_thread_spec) -> functionName = "unbind";
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
{"send", (VFrameHeader*) op_send_thread_spec},
{"recv", (VFrameHeader*) op_recv_thread_spec},
{"bind", (VFrameHeader*) op_bind_thread_spec},
{"unbind", (VFrameHeader*) op_unbind_thread_spec}};
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
{"Datagram", &Datagram_def
},
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

