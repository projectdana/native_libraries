#include "dana_lib_defs.h"
#include <string.h>
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
static const DanaType byte_def = 
{TYPE_LITERAL, 0, 1, NULL, 0};
static const DanaTypeField byte_array_fields[] = {
{(DanaType*) &byte_def, NULL, 0, 0, 0}};
static const DanaType byte_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) byte_array_fields, 1};
static const DanaTypeField Datagram_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0+(sizeof(size_t)*2)},
{(DanaType*) &byte_array_def, NULL, 0, 0, 0+(sizeof(size_t)*2)+sizeof(size_t)}};
static const DanaType Datagram_def = 
{TYPE_DATA, 0, 40, (DanaTypeField*) Datagram_fields, 3};
static const DanaType void_def = 
{TYPE_LITERAL, 0, 0, NULL, 0};
static const DanaTypeField Data_fields[] = {
};
static const DanaType Data_def = 
{TYPE_DATA, 0, 0, (DanaTypeField*) Data_fields, 0};
static const DanaTypeField Data_array_fields[] = {
{(DanaType*) &Data_def, NULL, 0, 0, 0}};
static const DanaType Data_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) Data_array_fields, 1};
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
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &byte_array_def, NULL, 0, 0, 32}};
static const DanaTypeField function_UDPLib_recv_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &Datagram_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UDPLib_bind_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 24}};
static const DanaTypeField function_UDPLib_setBlocking_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &bool_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UDPLib_unbind_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UDPLib_createSelect_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UDPLib_setEventArrayLength_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &int_def, NULL, 0, 0, 9}};
static const DanaTypeField function_UDPLib_addSocket_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &int_def, NULL, 0, 0, 9},
{(DanaType*) &Data_def, NULL, 0, 0, 24}};
static const DanaTypeField function_UDPLib_remSocket_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UDPLib_wait_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &Data_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UDPLib_waitTime_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &Data_array_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 32}};
static const DanaTypeField function_UDPLib_destroySelect_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaType object_UDPLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UDPLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UDPLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_UDPLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_UDPLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_UDPLib_send_fields, 4},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_UDPLib_recv_fields, 3},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_UDPLib_bind_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_UDPLib_setBlocking_fields, 3},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UDPLib_unbind_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_UDPLib_createSelect_fields, 2},
{TYPE_FUNCTION, 0, 17, (DanaTypeField*) &function_UDPLib_setEventArrayLength_fields, 3},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_UDPLib_addSocket_fields, 4},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_UDPLib_remSocket_fields, 3},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_UDPLib_wait_fields, 3},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_UDPLib_waitTime_fields, 4},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UDPLib_destroySelect_fields, 2}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_UDPLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_UDPLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_UDPLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_UDPLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_UDPLib_functions_spec[4], "send", 4},
{(DanaType*) &object_UDPLib_functions_spec[5], "recv", 4},
{(DanaType*) &object_UDPLib_functions_spec[6], "bind", 4},
{(DanaType*) &object_UDPLib_functions_spec[7], "setBlocking", 11},
{(DanaType*) &object_UDPLib_functions_spec[8], "unbind", 6},
{(DanaType*) &object_UDPLib_functions_spec[9], "createSelect", 12},
{(DanaType*) &object_UDPLib_functions_spec[10], "setEventArrayLength", 19},
{(DanaType*) &object_UDPLib_functions_spec[11], "addSocket", 9},
{(DanaType*) &object_UDPLib_functions_spec[12], "remSocket", 9},
{(DanaType*) &object_UDPLib_functions_spec[13], "wait", 4},
{(DanaType*) &object_UDPLib_functions_spec[14], "waitTime", 8},
{(DanaType*) &object_UDPLib_functions_spec[15], "destroySelect", 13}};
static const DanaTypeField intf_events_def[] = {
};
static const DanaType UDPLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 16},
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_events_def, 0},
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
static unsigned char op_setBlocking_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_unbind_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_createSelect_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setEventArrayLength_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addSocket_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_remSocket_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_wait_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_waitTime_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_destroySelect_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
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
(size_t) op_setBlocking_thread_spec,
(size_t) op_unbind_thread_spec,
(size_t) op_createSelect_thread_spec,
(size_t) op_setEventArrayLength_thread_spec,
(size_t) op_addSocket_thread_spec,
(size_t) op_remSocket_thread_spec,
(size_t) op_wait_thread_spec,
(size_t) op_waitTime_thread_spec,
(size_t) op_destroySelect_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"UDPLib", 6, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 0, 0, (size_t) &bool_def, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> sub = NULL;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> sub = NULL;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> sub = NULL;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_send_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_send_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_send_thread_spec) -> sub = NULL;
((VFrameHeader*) op_send_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[4];
((VFrameHeader*) op_send_thread_spec) -> functionName = "send";
((VFrameHeader*) op_recv_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_recv_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_recv_thread_spec) -> sub = NULL;
((VFrameHeader*) op_recv_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[5];
((VFrameHeader*) op_recv_thread_spec) -> functionName = "recv";
((VFrameHeader*) op_bind_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_bind_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_bind_thread_spec) -> sub = NULL;
((VFrameHeader*) op_bind_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[6];
((VFrameHeader*) op_bind_thread_spec) -> functionName = "bind";
((VFrameHeader*) op_setBlocking_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_setBlocking_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setBlocking_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setBlocking_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[7];
((VFrameHeader*) op_setBlocking_thread_spec) -> functionName = "setBlocking";
((VFrameHeader*) op_unbind_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_unbind_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_unbind_thread_spec) -> sub = NULL;
((VFrameHeader*) op_unbind_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[8];
((VFrameHeader*) op_unbind_thread_spec) -> functionName = "unbind";
((VFrameHeader*) op_createSelect_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_createSelect_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_createSelect_thread_spec) -> sub = NULL;
((VFrameHeader*) op_createSelect_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[9];
((VFrameHeader*) op_createSelect_thread_spec) -> functionName = "createSelect";
((VFrameHeader*) op_setEventArrayLength_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 17;
((VFrameHeader*) op_setEventArrayLength_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setEventArrayLength_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setEventArrayLength_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[10];
((VFrameHeader*) op_setEventArrayLength_thread_spec) -> functionName = "setEventArrayLength";
((VFrameHeader*) op_addSocket_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_addSocket_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_addSocket_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addSocket_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[11];
((VFrameHeader*) op_addSocket_thread_spec) -> functionName = "addSocket";
((VFrameHeader*) op_remSocket_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_remSocket_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_remSocket_thread_spec) -> sub = NULL;
((VFrameHeader*) op_remSocket_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[12];
((VFrameHeader*) op_remSocket_thread_spec) -> functionName = "remSocket";
((VFrameHeader*) op_wait_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_wait_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_wait_thread_spec) -> sub = NULL;
((VFrameHeader*) op_wait_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[13];
((VFrameHeader*) op_wait_thread_spec) -> functionName = "wait";
((VFrameHeader*) op_waitTime_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_waitTime_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_waitTime_thread_spec) -> sub = NULL;
((VFrameHeader*) op_waitTime_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[14];
((VFrameHeader*) op_waitTime_thread_spec) -> functionName = "waitTime";
((VFrameHeader*) op_destroySelect_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_destroySelect_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_destroySelect_thread_spec) -> sub = NULL;
((VFrameHeader*) op_destroySelect_thread_spec) -> localsDef = (size_t) &object_UDPLib_functions_spec[15];
((VFrameHeader*) op_destroySelect_thread_spec) -> functionName = "destroySelect";
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
{"setBlocking", (VFrameHeader*) op_setBlocking_thread_spec},
{"unbind", (VFrameHeader*) op_unbind_thread_spec},
{"createSelect", (VFrameHeader*) op_createSelect_thread_spec},
{"setEventArrayLength", (VFrameHeader*) op_setEventArrayLength_thread_spec},
{"addSocket", (VFrameHeader*) op_addSocket_thread_spec},
{"remSocket", (VFrameHeader*) op_remSocket_thread_spec},
{"wait", (VFrameHeader*) op_wait_thread_spec},
{"waitTime", (VFrameHeader*) op_waitTime_thread_spec},
{"destroySelect", (VFrameHeader*) op_destroySelect_thread_spec}};
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
{"Data[]", &Data_array_def
},
{"Data", &Data_def
},
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

