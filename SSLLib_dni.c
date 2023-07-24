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
static const DanaType void_def = 
{TYPE_LITERAL, 0, 0, NULL, 0};
static const DanaType byte_def = 
{TYPE_LITERAL, 0, 1, NULL, 0};
static const DanaTypeField byte_array_fields[] = {
{(DanaType*) &byte_def, NULL, 0, 0, 0}};
static const DanaType byte_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) byte_array_fields, 1};
static const DanaTypeField String_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaType String_def = 
{TYPE_DATA, 0, 16, (DanaTypeField*) String_fields, 1};
static const DanaTypeField String_array_fields[] = {
{(DanaType*) &String_def, NULL, 0, 0, 0}};
static const DanaType String_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) String_array_fields, 1};
static const DanaTypeField VerifyStatus_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &char_array_def, NULL, 0, 0, 0+sizeof(size_t)},
{(DanaType*) &char_array_def, NULL, 0, 0, 0+sizeof(size_t)+(sizeof(size_t)*2)}};
static const DanaType VerifyStatus_def = 
{TYPE_DATA, 0, 40, (DanaTypeField*) VerifyStatus_fields, 3};
static const DanaTypeField TLSStatus_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaType TLSStatus_def = 
{TYPE_DATA, 0, 8, (DanaTypeField*) TLSStatus_fields, 1};
static const DanaTypeField function_SSLLib_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_SSLLib_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_SSLLib_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SSLLib_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SSLLib_createCertStore_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SSLLib_addCertificate_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &char_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_SSLLib_loadLocation_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &char_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_SSLLib_freeCertStore_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SSLLib_createContext_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &bool_def, NULL, 0, 0, 8}};
static const DanaTypeField function_SSLLib_setCertificate_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &byte_array_def, NULL, 0, 0, 16},
{(DanaType*) &byte_array_def, NULL, 0, 0, 32}};
static const DanaTypeField function_SSLLib_setCertificateChain_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &String_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_SSLLib_setCipherSet_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &int_def, NULL, 0, 0, 9}};
static const DanaTypeField function_SSLLib_freeContext_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SSLLib_makeSSL_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8}};
static const DanaTypeField function_SSLLib_accept_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &int_def, NULL, 0, 0, 9}};
static const DanaTypeField function_SSLLib_accept_nb_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_SSLLib_connect_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &int_def, NULL, 0, 0, 9}};
static const DanaTypeField function_SSLLib_connect_nb_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_SSLLib_getPeerCertificate_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_SSLLib_getPeerCertChain_fields[] = {
{(DanaType*) &String_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_SSLLib_verifyCertificate_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &VerifyStatus_def, NULL, 0, 0, 24},
{(DanaType*) &char_array_def, NULL, 0, 0, 40},
{(DanaType*) &String_array_def, NULL, 0, 0, 56}};
static const DanaTypeField function_SSLLib_write_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &byte_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_SSLLib_write_nb_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &byte_array_def, NULL, 0, 0, 16},
{(DanaType*) &TLSStatus_def, NULL, 0, 0, 32}};
static const DanaTypeField function_SSLLib_read_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24}};
static const DanaTypeField function_SSLLib_read_nb_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &TLSStatus_def, NULL, 0, 0, 32}};
static const DanaTypeField function_SSLLib_closeSSL_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SSLLib_closeSSL_nb_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_SSLLib_freeSSL_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaType object_SSLLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_SSLLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_SSLLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_SSLLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_SSLLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_SSLLib_createCertStore_fields, 1},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_SSLLib_addCertificate_fields, 3},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_SSLLib_loadLocation_fields, 3},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_SSLLib_freeCertStore_fields, 2},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_SSLLib_createContext_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_SSLLib_setCertificate_fields, 4},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_SSLLib_setCertificateChain_fields, 3},
{TYPE_FUNCTION, 0, 17, (DanaTypeField*) &function_SSLLib_setCipherSet_fields, 3},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_SSLLib_freeContext_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_SSLLib_makeSSL_fields, 2},
{TYPE_FUNCTION, 0, 17, (DanaTypeField*) &function_SSLLib_accept_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_SSLLib_accept_nb_fields, 3},
{TYPE_FUNCTION, 0, 17, (DanaTypeField*) &function_SSLLib_connect_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_SSLLib_connect_nb_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_SSLLib_getPeerCertificate_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_SSLLib_getPeerCertChain_fields, 2},
{TYPE_FUNCTION, 0, 72, (DanaTypeField*) &function_SSLLib_verifyCertificate_fields, 6},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_SSLLib_write_fields, 3},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_SSLLib_write_nb_fields, 4},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_SSLLib_read_fields, 3},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_SSLLib_read_nb_fields, 4},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_SSLLib_closeSSL_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_SSLLib_closeSSL_nb_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_SSLLib_freeSSL_fields, 2}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_SSLLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_SSLLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_SSLLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_SSLLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_SSLLib_functions_spec[4], "createCertStore", 15},
{(DanaType*) &object_SSLLib_functions_spec[5], "addCertificate", 14},
{(DanaType*) &object_SSLLib_functions_spec[6], "loadLocation", 12},
{(DanaType*) &object_SSLLib_functions_spec[7], "freeCertStore", 13},
{(DanaType*) &object_SSLLib_functions_spec[8], "createContext", 13},
{(DanaType*) &object_SSLLib_functions_spec[9], "setCertificate", 14},
{(DanaType*) &object_SSLLib_functions_spec[10], "setCertificateChain", 19},
{(DanaType*) &object_SSLLib_functions_spec[11], "setCipherSet", 12},
{(DanaType*) &object_SSLLib_functions_spec[12], "freeContext", 11},
{(DanaType*) &object_SSLLib_functions_spec[13], "makeSSL", 7},
{(DanaType*) &object_SSLLib_functions_spec[14], "accept", 6},
{(DanaType*) &object_SSLLib_functions_spec[15], "accept_nb", 9},
{(DanaType*) &object_SSLLib_functions_spec[16], "connect", 7},
{(DanaType*) &object_SSLLib_functions_spec[17], "connect_nb", 10},
{(DanaType*) &object_SSLLib_functions_spec[18], "getPeerCertificate", 18},
{(DanaType*) &object_SSLLib_functions_spec[19], "getPeerCertChain", 16},
{(DanaType*) &object_SSLLib_functions_spec[20], "verifyCertificate", 17},
{(DanaType*) &object_SSLLib_functions_spec[21], "write", 5},
{(DanaType*) &object_SSLLib_functions_spec[22], "write_nb", 8},
{(DanaType*) &object_SSLLib_functions_spec[23], "read", 4},
{(DanaType*) &object_SSLLib_functions_spec[24], "read_nb", 7},
{(DanaType*) &object_SSLLib_functions_spec[25], "closeSSL", 8},
{(DanaType*) &object_SSLLib_functions_spec[26], "closeSSL_nb", 11},
{(DanaType*) &object_SSLLib_functions_spec[27], "freeSSL", 7}};
static const DanaTypeField intf_events_def[] = {
};
static const DanaType SSLLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 28},
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
static unsigned char op_createCertStore_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addCertificate_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_loadLocation_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_freeCertStore_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_createContext_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setCertificate_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setCertificateChain_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setCipherSet_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_freeContext_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_makeSSL_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_accept_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_accept_nb_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_connect_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_connect_nb_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getPeerCertificate_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getPeerCertChain_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_verifyCertificate_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_write_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_write_nb_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_read_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_read_nb_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_closeSSL_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_closeSSL_nb_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_freeSSL_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_createCertStore_thread_spec,
(size_t) op_addCertificate_thread_spec,
(size_t) op_loadLocation_thread_spec,
(size_t) op_freeCertStore_thread_spec,
(size_t) op_createContext_thread_spec,
(size_t) op_setCertificate_thread_spec,
(size_t) op_setCertificateChain_thread_spec,
(size_t) op_setCipherSet_thread_spec,
(size_t) op_freeContext_thread_spec,
(size_t) op_makeSSL_thread_spec,
(size_t) op_accept_thread_spec,
(size_t) op_accept_nb_thread_spec,
(size_t) op_connect_thread_spec,
(size_t) op_connect_nb_thread_spec,
(size_t) op_getPeerCertificate_thread_spec,
(size_t) op_getPeerCertChain_thread_spec,
(size_t) op_verifyCertificate_thread_spec,
(size_t) op_write_thread_spec,
(size_t) op_write_nb_thread_spec,
(size_t) op_read_thread_spec,
(size_t) op_read_nb_thread_spec,
(size_t) op_closeSSL_thread_spec,
(size_t) op_closeSSL_nb_thread_spec,
(size_t) op_freeSSL_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"SSLLib", 6, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 0, 0, (size_t) &bool_def, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> sub = NULL;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> sub = NULL;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> sub = NULL;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_createCertStore_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_createCertStore_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_createCertStore_thread_spec) -> sub = NULL;
((VFrameHeader*) op_createCertStore_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[4];
((VFrameHeader*) op_createCertStore_thread_spec) -> functionName = "createCertStore";
((VFrameHeader*) op_addCertificate_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_addCertificate_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_addCertificate_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addCertificate_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[5];
((VFrameHeader*) op_addCertificate_thread_spec) -> functionName = "addCertificate";
((VFrameHeader*) op_loadLocation_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_loadLocation_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_loadLocation_thread_spec) -> sub = NULL;
((VFrameHeader*) op_loadLocation_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[6];
((VFrameHeader*) op_loadLocation_thread_spec) -> functionName = "loadLocation";
((VFrameHeader*) op_freeCertStore_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_freeCertStore_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_freeCertStore_thread_spec) -> sub = NULL;
((VFrameHeader*) op_freeCertStore_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[7];
((VFrameHeader*) op_freeCertStore_thread_spec) -> functionName = "freeCertStore";
((VFrameHeader*) op_createContext_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_createContext_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_createContext_thread_spec) -> sub = NULL;
((VFrameHeader*) op_createContext_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[8];
((VFrameHeader*) op_createContext_thread_spec) -> functionName = "createContext";
((VFrameHeader*) op_setCertificate_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_setCertificate_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_setCertificate_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setCertificate_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[9];
((VFrameHeader*) op_setCertificate_thread_spec) -> functionName = "setCertificate";
((VFrameHeader*) op_setCertificateChain_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_setCertificateChain_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setCertificateChain_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setCertificateChain_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[10];
((VFrameHeader*) op_setCertificateChain_thread_spec) -> functionName = "setCertificateChain";
((VFrameHeader*) op_setCipherSet_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 17;
((VFrameHeader*) op_setCipherSet_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setCipherSet_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setCipherSet_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[11];
((VFrameHeader*) op_setCipherSet_thread_spec) -> functionName = "setCipherSet";
((VFrameHeader*) op_freeContext_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_freeContext_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_freeContext_thread_spec) -> sub = NULL;
((VFrameHeader*) op_freeContext_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[12];
((VFrameHeader*) op_freeContext_thread_spec) -> functionName = "freeContext";
((VFrameHeader*) op_makeSSL_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_makeSSL_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_makeSSL_thread_spec) -> sub = NULL;
((VFrameHeader*) op_makeSSL_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[13];
((VFrameHeader*) op_makeSSL_thread_spec) -> functionName = "makeSSL";
((VFrameHeader*) op_accept_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 17;
((VFrameHeader*) op_accept_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_accept_thread_spec) -> sub = NULL;
((VFrameHeader*) op_accept_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[14];
((VFrameHeader*) op_accept_thread_spec) -> functionName = "accept";
((VFrameHeader*) op_accept_nb_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_accept_nb_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_accept_nb_thread_spec) -> sub = NULL;
((VFrameHeader*) op_accept_nb_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[15];
((VFrameHeader*) op_accept_nb_thread_spec) -> functionName = "accept_nb";
((VFrameHeader*) op_connect_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 17;
((VFrameHeader*) op_connect_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_connect_thread_spec) -> sub = NULL;
((VFrameHeader*) op_connect_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[16];
((VFrameHeader*) op_connect_thread_spec) -> functionName = "connect";
((VFrameHeader*) op_connect_nb_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_connect_nb_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_connect_nb_thread_spec) -> sub = NULL;
((VFrameHeader*) op_connect_nb_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[17];
((VFrameHeader*) op_connect_nb_thread_spec) -> functionName = "connect_nb";
((VFrameHeader*) op_getPeerCertificate_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_getPeerCertificate_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_getPeerCertificate_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getPeerCertificate_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[18];
((VFrameHeader*) op_getPeerCertificate_thread_spec) -> functionName = "getPeerCertificate";
((VFrameHeader*) op_getPeerCertChain_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_getPeerCertChain_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_getPeerCertChain_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getPeerCertChain_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[19];
((VFrameHeader*) op_getPeerCertChain_thread_spec) -> functionName = "getPeerCertChain";
((VFrameHeader*) op_verifyCertificate_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 72;
((VFrameHeader*) op_verifyCertificate_thread_spec) -> formalParamsCount = 5;
((VFrameHeader*) op_verifyCertificate_thread_spec) -> sub = NULL;
((VFrameHeader*) op_verifyCertificate_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[20];
((VFrameHeader*) op_verifyCertificate_thread_spec) -> functionName = "verifyCertificate";
((VFrameHeader*) op_write_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_write_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_write_thread_spec) -> sub = NULL;
((VFrameHeader*) op_write_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[21];
((VFrameHeader*) op_write_thread_spec) -> functionName = "write";
((VFrameHeader*) op_write_nb_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_write_nb_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_write_nb_thread_spec) -> sub = NULL;
((VFrameHeader*) op_write_nb_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[22];
((VFrameHeader*) op_write_nb_thread_spec) -> functionName = "write_nb";
((VFrameHeader*) op_read_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_read_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_read_thread_spec) -> sub = NULL;
((VFrameHeader*) op_read_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[23];
((VFrameHeader*) op_read_thread_spec) -> functionName = "read";
((VFrameHeader*) op_read_nb_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_read_nb_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_read_nb_thread_spec) -> sub = NULL;
((VFrameHeader*) op_read_nb_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[24];
((VFrameHeader*) op_read_nb_thread_spec) -> functionName = "read_nb";
((VFrameHeader*) op_closeSSL_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_closeSSL_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_closeSSL_thread_spec) -> sub = NULL;
((VFrameHeader*) op_closeSSL_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[25];
((VFrameHeader*) op_closeSSL_thread_spec) -> functionName = "closeSSL";
((VFrameHeader*) op_closeSSL_nb_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_closeSSL_nb_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_closeSSL_nb_thread_spec) -> sub = NULL;
((VFrameHeader*) op_closeSSL_nb_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[26];
((VFrameHeader*) op_closeSSL_nb_thread_spec) -> functionName = "closeSSL_nb";
((VFrameHeader*) op_freeSSL_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_freeSSL_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_freeSSL_thread_spec) -> sub = NULL;
((VFrameHeader*) op_freeSSL_thread_spec) -> localsDef = (size_t) &object_SSLLib_functions_spec[27];
((VFrameHeader*) op_freeSSL_thread_spec) -> functionName = "freeSSL";
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
{"createCertStore", (VFrameHeader*) op_createCertStore_thread_spec},
{"addCertificate", (VFrameHeader*) op_addCertificate_thread_spec},
{"loadLocation", (VFrameHeader*) op_loadLocation_thread_spec},
{"freeCertStore", (VFrameHeader*) op_freeCertStore_thread_spec},
{"createContext", (VFrameHeader*) op_createContext_thread_spec},
{"setCertificate", (VFrameHeader*) op_setCertificate_thread_spec},
{"setCertificateChain", (VFrameHeader*) op_setCertificateChain_thread_spec},
{"setCipherSet", (VFrameHeader*) op_setCipherSet_thread_spec},
{"freeContext", (VFrameHeader*) op_freeContext_thread_spec},
{"makeSSL", (VFrameHeader*) op_makeSSL_thread_spec},
{"accept", (VFrameHeader*) op_accept_thread_spec},
{"accept_nb", (VFrameHeader*) op_accept_nb_thread_spec},
{"connect", (VFrameHeader*) op_connect_thread_spec},
{"connect_nb", (VFrameHeader*) op_connect_nb_thread_spec},
{"getPeerCertificate", (VFrameHeader*) op_getPeerCertificate_thread_spec},
{"getPeerCertChain", (VFrameHeader*) op_getPeerCertChain_thread_spec},
{"verifyCertificate", (VFrameHeader*) op_verifyCertificate_thread_spec},
{"write", (VFrameHeader*) op_write_thread_spec},
{"write_nb", (VFrameHeader*) op_write_nb_thread_spec},
{"read", (VFrameHeader*) op_read_thread_spec},
{"read_nb", (VFrameHeader*) op_read_nb_thread_spec},
{"closeSSL", (VFrameHeader*) op_closeSSL_thread_spec},
{"closeSSL_nb", (VFrameHeader*) op_closeSSL_nb_thread_spec},
{"freeSSL", (VFrameHeader*) op_freeSSL_thread_spec}};
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
{"TLSStatus", &TLSStatus_def
},
{"VerifyStatus", &VerifyStatus_def
},
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

