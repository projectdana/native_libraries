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
static const DanaTypeField String_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaType String_def = 
{TYPE_DATA, 0, 40, (DanaTypeField*) String_fields, 1};
static const DanaTypeField String_array_fields[] = {
{(DanaType*) &String_def, NULL, 0, 0, 0}};
static const DanaType String_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) String_array_fields, 1};
static const DanaType void_def = 
{TYPE_LITERAL, 0, 0, NULL, 0};
static const DanaTypeField function_SSLLib_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_SSLLib_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_SSLLib_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SSLLib_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SSLLib_createContext_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &bool_def, NULL, 0, 0, 8}};
static const DanaTypeField function_SSLLib_setCertificate_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &byte_array_def, NULL, 0, 0, 16},
{(DanaType*) &byte_array_def, NULL, 0, 0, 56}};
static const DanaTypeField function_SSLLib_setCertificateChain_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &String_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_SSLLib_setVerifyMode_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &byte_def, NULL, 0, 0, 9}};
static const DanaTypeField function_SSLLib_getVerifyResult_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1}};
static const DanaTypeField function_SSLLib_freeContext_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SSLLib_makeSSL_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8}};
static const DanaTypeField function_SSLLib_accept_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &int_def, NULL, 0, 0, 9}};
static const DanaTypeField function_SSLLib_connect_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &int_def, NULL, 0, 0, 9}};
static const DanaTypeField function_SSLLib_getPeerCertificate_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 40}};
static const DanaTypeField function_SSLLib_getPeerCertChain_fields[] = {
{(DanaType*) &String_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 40}};
static const DanaTypeField function_SSLLib_write_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &byte_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_SSLLib_read_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &int_def, NULL, 0, 0, 48}};
static const DanaTypeField function_SSLLib_closeSSL_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaType object_SSLLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_SSLLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_SSLLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_SSLLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_SSLLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_SSLLib_createContext_fields, 2},
{TYPE_FUNCTION, 0, 96, (DanaTypeField*) &function_SSLLib_setCertificate_fields, 4},
{TYPE_FUNCTION, 0, 56, (DanaTypeField*) &function_SSLLib_setCertificateChain_fields, 3},
{TYPE_FUNCTION, 0, 10, (DanaTypeField*) &function_SSLLib_setVerifyMode_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_SSLLib_getVerifyResult_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_SSLLib_freeContext_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_SSLLib_makeSSL_fields, 2},
{TYPE_FUNCTION, 0, 17, (DanaTypeField*) &function_SSLLib_accept_fields, 3},
{TYPE_FUNCTION, 0, 17, (DanaTypeField*) &function_SSLLib_connect_fields, 3},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_SSLLib_getPeerCertificate_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_SSLLib_getPeerCertChain_fields, 2},
{TYPE_FUNCTION, 0, 56, (DanaTypeField*) &function_SSLLib_write_fields, 3},
{TYPE_FUNCTION, 0, 56, (DanaTypeField*) &function_SSLLib_read_fields, 3},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_SSLLib_closeSSL_fields, 2}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_SSLLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_SSLLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_SSLLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_SSLLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_SSLLib_functions_spec[4], "createContext", 13},
{(DanaType*) &object_SSLLib_functions_spec[5], "setCertificate", 14},
{(DanaType*) &object_SSLLib_functions_spec[6], "setCertificateChain", 19},
{(DanaType*) &object_SSLLib_functions_spec[7], "setVerifyMode", 13},
{(DanaType*) &object_SSLLib_functions_spec[8], "getVerifyResult", 15},
{(DanaType*) &object_SSLLib_functions_spec[9], "freeContext", 11},
{(DanaType*) &object_SSLLib_functions_spec[10], "makeSSL", 7},
{(DanaType*) &object_SSLLib_functions_spec[11], "accept", 6},
{(DanaType*) &object_SSLLib_functions_spec[12], "connect", 7},
{(DanaType*) &object_SSLLib_functions_spec[13], "getPeerCertificate", 18},
{(DanaType*) &object_SSLLib_functions_spec[14], "getPeerCertChain", 16},
{(DanaType*) &object_SSLLib_functions_spec[15], "write", 5},
{(DanaType*) &object_SSLLib_functions_spec[16], "read", 4},
{(DanaType*) &object_SSLLib_functions_spec[17], "closeSSL", 8}};
static const DanaType object_SSLLib_events_spec[] = {
};
static const DanaTypeField intf_events_def[] = {
};
static const DanaType SSLLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 18},
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_events_def, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField intf_def[] = {
{(DanaType*) &SSLLib_object_spec[0], ".functions", 10},
{(DanaType*) &SSLLib_object_spec[1], ".events", 7},
{(DanaType*) &SSLLib_object_spec[2], ".state", 6},
};
static unsigned char op_clone_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_equals_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_toString_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getID_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_createContext_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setCertificate_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setCertificateChain_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setVerifyMode_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getVerifyResult_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_freeContext_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_makeSSL_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_accept_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_connect_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getPeerCertificate_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getPeerCertChain_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_write_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_read_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_closeSSL_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_createContext_thread_spec,
(size_t) op_setCertificate_thread_spec,
(size_t) op_setCertificateChain_thread_spec,
(size_t) op_setVerifyMode_thread_spec,
(size_t) op_getVerifyResult_thread_spec,
(size_t) op_freeContext_thread_spec,
(size_t) op_makeSSL_thread_spec,
(size_t) op_accept_thread_spec,
(size_t) op_connect_thread_spec,
(size_t) op_getPeerCertificate_thread_spec,
(size_t) op_getPeerCertChain_thread_spec,
(size_t) op_write_thread_spec,
(size_t) op_read_thread_spec,
(size_t) op_closeSSL_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"SSLLib", 6, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 0, 0, (size_t) &bool_def, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> sub = NULL;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> sub = NULL;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> sub = NULL;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_createContext_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_createContext_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_createContext_thread_spec) -> sub = NULL;
((VFrameHeader*) op_createContext_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[4];
((VFrameHeader*) op_createContext_thread_spec) -> functionName = "createContext";
((VFrameHeader*) op_setCertificate_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 96;
((VFrameHeader*) op_setCertificate_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_setCertificate_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setCertificate_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[5];
((VFrameHeader*) op_setCertificate_thread_spec) -> functionName = "setCertificate";
((VFrameHeader*) op_setCertificateChain_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 56;
((VFrameHeader*) op_setCertificateChain_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setCertificateChain_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setCertificateChain_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[6];
((VFrameHeader*) op_setCertificateChain_thread_spec) -> functionName = "setCertificateChain";
((VFrameHeader*) op_setVerifyMode_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 10;
((VFrameHeader*) op_setVerifyMode_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setVerifyMode_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setVerifyMode_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[7];
((VFrameHeader*) op_setVerifyMode_thread_spec) -> functionName = "setVerifyMode";
((VFrameHeader*) op_getVerifyResult_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_getVerifyResult_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_getVerifyResult_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getVerifyResult_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[8];
((VFrameHeader*) op_getVerifyResult_thread_spec) -> functionName = "getVerifyResult";
((VFrameHeader*) op_freeContext_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_freeContext_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_freeContext_thread_spec) -> sub = NULL;
((VFrameHeader*) op_freeContext_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[9];
((VFrameHeader*) op_freeContext_thread_spec) -> functionName = "freeContext";
((VFrameHeader*) op_makeSSL_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_makeSSL_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_makeSSL_thread_spec) -> sub = NULL;
((VFrameHeader*) op_makeSSL_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[10];
((VFrameHeader*) op_makeSSL_thread_spec) -> functionName = "makeSSL";
((VFrameHeader*) op_accept_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 17;
((VFrameHeader*) op_accept_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_accept_thread_spec) -> sub = NULL;
((VFrameHeader*) op_accept_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[11];
((VFrameHeader*) op_accept_thread_spec) -> functionName = "accept";
((VFrameHeader*) op_connect_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 17;
((VFrameHeader*) op_connect_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_connect_thread_spec) -> sub = NULL;
((VFrameHeader*) op_connect_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[12];
((VFrameHeader*) op_connect_thread_spec) -> functionName = "connect";
((VFrameHeader*) op_getPeerCertificate_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_getPeerCertificate_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_getPeerCertificate_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getPeerCertificate_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[13];
((VFrameHeader*) op_getPeerCertificate_thread_spec) -> functionName = "getPeerCertificate";
((VFrameHeader*) op_getPeerCertChain_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_getPeerCertChain_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_getPeerCertChain_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getPeerCertChain_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[14];
((VFrameHeader*) op_getPeerCertChain_thread_spec) -> functionName = "getPeerCertChain";
((VFrameHeader*) op_write_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 56;
((VFrameHeader*) op_write_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_write_thread_spec) -> sub = NULL;
((VFrameHeader*) op_write_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[15];
((VFrameHeader*) op_write_thread_spec) -> functionName = "write";
((VFrameHeader*) op_read_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 56;
((VFrameHeader*) op_read_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_read_thread_spec) -> sub = NULL;
((VFrameHeader*) op_read_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[16];
((VFrameHeader*) op_read_thread_spec) -> functionName = "read";
((VFrameHeader*) op_closeSSL_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_closeSSL_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_closeSSL_thread_spec) -> sub = NULL;
((VFrameHeader*) op_closeSSL_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[17];
((VFrameHeader*) op_closeSSL_thread_spec) -> functionName = "closeSSL";
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
{"createContext", (VFrameHeader*) op_createContext_thread_spec},
{"setCertificate", (VFrameHeader*) op_setCertificate_thread_spec},
{"setCertificateChain", (VFrameHeader*) op_setCertificateChain_thread_spec},
{"setVerifyMode", (VFrameHeader*) op_setVerifyMode_thread_spec},
{"getVerifyResult", (VFrameHeader*) op_getVerifyResult_thread_spec},
{"freeContext", (VFrameHeader*) op_freeContext_thread_spec},
{"makeSSL", (VFrameHeader*) op_makeSSL_thread_spec},
{"accept", (VFrameHeader*) op_accept_thread_spec},
{"connect", (VFrameHeader*) op_connect_thread_spec},
{"getPeerCertificate", (VFrameHeader*) op_getPeerCertificate_thread_spec},
{"getPeerCertChain", (VFrameHeader*) op_getPeerCertChain_thread_spec},
{"write", (VFrameHeader*) op_write_thread_spec},
{"read", (VFrameHeader*) op_read_thread_spec},
{"closeSSL", (VFrameHeader*) op_closeSSL_thread_spec}};
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
{"String", &String_def
},
{"String[]", &String_array_def
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

