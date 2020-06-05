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
static const DanaTypeField FileEntry_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaType FileEntry_def = 
{TYPE_DATA, 0, 40, (DanaTypeField*) FileEntry_fields, 1};
static const DanaTypeField FileEntry_array_fields[] = {
{(DanaType*) &FileEntry_def, NULL, 0, 0, 0}};
static const DanaType FileEntry_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) FileEntry_array_fields, 1};
static const DanaTypeField DCQ_fields[] = {
{(DanaType*) &FileEntry_array_def, NULL, 0, 0, 0}};
static const DanaType DCQ_def = 
{TYPE_DATA, 0, 40, (DanaTypeField*) DCQ_fields, 1};
static const DanaType int2_def = 
{TYPE_LITERAL, 0, 2, NULL, 0};
static const DanaTypeField DateTime_fields[] = {
{(DanaType*) &int2_def, NULL, 0, 0, 0},
{(DanaType*) &byte_def, NULL, 0, 0, 0},
{(DanaType*) &byte_def, NULL, 0, 0, 0},
{(DanaType*) &byte_def, NULL, 0, 0, 0},
{(DanaType*) &byte_def, NULL, 0, 0, 0},
{(DanaType*) &byte_def, NULL, 0, 0, 0},
{(DanaType*) &int2_def, NULL, 0, 0, 0}};
static const DanaType DateTime_def = 
{TYPE_DATA, 0, 9, (DanaTypeField*) DateTime_fields, 7};
static const DanaTypeField FileInfo_fields[] = {
{(DanaType*) &byte_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &DateTime_def, NULL, 0, 0, 0}};
static const DanaType FileInfo_def = 
{TYPE_DATA, 0, 56, (DanaTypeField*) FileInfo_fields, 3};
static const DanaTypeField function_FileLib_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_FileLib_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_FileLib_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_FileLib_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_FileLib_open_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &byte_def, NULL, 0, 0, 48}};
static const DanaTypeField function_FileLib_write_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &byte_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_FileLib_read_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &int_def, NULL, 0, 0, 48}};
static const DanaTypeField function_FileLib_flush_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1}};
static const DanaTypeField function_FileLib_setPos_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &int_def, NULL, 0, 0, 9}};
static const DanaTypeField function_FileLib_getSize_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8}};
static const DanaTypeField function_FileLib_eof_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1}};
static const DanaTypeField function_FileLib_close_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_FileLib_getDirectoryContents_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 0},
{(DanaType*) &DCQ_def, NULL, 0, 0, 40}};
static const DanaTypeField function_FileLib_getInfo_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 0},
{(DanaType*) &FileInfo_def, NULL, 0, 0, 40}};
static const DanaTypeField function_FileLib_exists_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8}};
static const DanaTypeField function_FileLib_delete_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8}};
static const DanaTypeField function_FileLib_move_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &char_array_def, NULL, 0, 0, 48}};
static const DanaTypeField function_FileLib_copy_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &char_array_def, NULL, 0, 0, 48},
{(DanaType*) &bool_def, NULL, 0, 0, 88}};
static const DanaTypeField function_FileLib_createDirectory_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8}};
static const DanaTypeField function_FileLib_deleteDirectory_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8}};
static const DanaType object_FileLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_FileLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_FileLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_FileLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_FileLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 49, (DanaTypeField*) &function_FileLib_open_fields, 3},
{TYPE_FUNCTION, 0, 56, (DanaTypeField*) &function_FileLib_write_fields, 3},
{TYPE_FUNCTION, 0, 56, (DanaTypeField*) &function_FileLib_read_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_FileLib_flush_fields, 2},
{TYPE_FUNCTION, 0, 17, (DanaTypeField*) &function_FileLib_setPos_fields, 3},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_FileLib_getSize_fields, 2},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_FileLib_eof_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_FileLib_close_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_FileLib_getDirectoryContents_fields, 3},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_FileLib_getInfo_fields, 3},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_FileLib_exists_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_FileLib_delete_fields, 2},
{TYPE_FUNCTION, 0, 88, (DanaTypeField*) &function_FileLib_move_fields, 3},
{TYPE_FUNCTION, 0, 89, (DanaTypeField*) &function_FileLib_copy_fields, 4},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_FileLib_createDirectory_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_FileLib_deleteDirectory_fields, 2}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_FileLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_FileLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_FileLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_FileLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_FileLib_functions_spec[4], "open", 4},
{(DanaType*) &object_FileLib_functions_spec[5], "write", 5},
{(DanaType*) &object_FileLib_functions_spec[6], "read", 4},
{(DanaType*) &object_FileLib_functions_spec[7], "flush", 5},
{(DanaType*) &object_FileLib_functions_spec[8], "setPos", 6},
{(DanaType*) &object_FileLib_functions_spec[9], "getSize", 7},
{(DanaType*) &object_FileLib_functions_spec[10], "eof", 3},
{(DanaType*) &object_FileLib_functions_spec[11], "close", 5},
{(DanaType*) &object_FileLib_functions_spec[12], "getDirectoryContents", 20},
{(DanaType*) &object_FileLib_functions_spec[13], "getInfo", 7},
{(DanaType*) &object_FileLib_functions_spec[14], "exists", 6},
{(DanaType*) &object_FileLib_functions_spec[15], "delete", 6},
{(DanaType*) &object_FileLib_functions_spec[16], "move", 4},
{(DanaType*) &object_FileLib_functions_spec[17], "copy", 4},
{(DanaType*) &object_FileLib_functions_spec[18], "createDirectory", 15},
{(DanaType*) &object_FileLib_functions_spec[19], "deleteDirectory", 15}};
static const DanaType object_FileLib_events_spec[] = {
};
static const DanaTypeField intf_events_def[] = {
};
static const DanaType FileLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 20},
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_events_def, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField intf_def[] = {
{(DanaType*) &FileLib_object_spec[0], ".functions", 10},
{(DanaType*) &FileLib_object_spec[1], ".events", 7},
{(DanaType*) &FileLib_object_spec[2], ".state", 6},
};
static unsigned char op_clone_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_equals_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_toString_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getID_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_open_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_write_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_read_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flush_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setPos_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getSize_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_eof_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_close_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getDirectoryContents_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getInfo_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_exists_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_delete_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_move_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_copy_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_createDirectory_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_deleteDirectory_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_open_thread_spec,
(size_t) op_write_thread_spec,
(size_t) op_read_thread_spec,
(size_t) op_flush_thread_spec,
(size_t) op_setPos_thread_spec,
(size_t) op_getSize_thread_spec,
(size_t) op_eof_thread_spec,
(size_t) op_close_thread_spec,
(size_t) op_getDirectoryContents_thread_spec,
(size_t) op_getInfo_thread_spec,
(size_t) op_exists_thread_spec,
(size_t) op_delete_thread_spec,
(size_t) op_move_thread_spec,
(size_t) op_copy_thread_spec,
(size_t) op_createDirectory_thread_spec,
(size_t) op_deleteDirectory_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"FileLib", 7, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 0, 0, (size_t) &bool_def, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> sub = NULL;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> sub = NULL;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> sub = NULL;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_open_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 49;
((VFrameHeader*) op_open_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_open_thread_spec) -> sub = NULL;
((VFrameHeader*) op_open_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[4];
((VFrameHeader*) op_open_thread_spec) -> functionName = "open";
((VFrameHeader*) op_write_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 56;
((VFrameHeader*) op_write_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_write_thread_spec) -> sub = NULL;
((VFrameHeader*) op_write_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[5];
((VFrameHeader*) op_write_thread_spec) -> functionName = "write";
((VFrameHeader*) op_read_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 56;
((VFrameHeader*) op_read_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_read_thread_spec) -> sub = NULL;
((VFrameHeader*) op_read_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[6];
((VFrameHeader*) op_read_thread_spec) -> functionName = "read";
((VFrameHeader*) op_flush_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_flush_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_flush_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flush_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[7];
((VFrameHeader*) op_flush_thread_spec) -> functionName = "flush";
((VFrameHeader*) op_setPos_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 17;
((VFrameHeader*) op_setPos_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setPos_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setPos_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[8];
((VFrameHeader*) op_setPos_thread_spec) -> functionName = "setPos";
((VFrameHeader*) op_getSize_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_getSize_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_getSize_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getSize_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[9];
((VFrameHeader*) op_getSize_thread_spec) -> functionName = "getSize";
((VFrameHeader*) op_eof_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_eof_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_eof_thread_spec) -> sub = NULL;
((VFrameHeader*) op_eof_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[10];
((VFrameHeader*) op_eof_thread_spec) -> functionName = "eof";
((VFrameHeader*) op_close_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_close_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_close_thread_spec) -> sub = NULL;
((VFrameHeader*) op_close_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[11];
((VFrameHeader*) op_close_thread_spec) -> functionName = "close";
((VFrameHeader*) op_getDirectoryContents_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_getDirectoryContents_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_getDirectoryContents_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getDirectoryContents_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[12];
((VFrameHeader*) op_getDirectoryContents_thread_spec) -> functionName = "getDirectoryContents";
((VFrameHeader*) op_getInfo_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_getInfo_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_getInfo_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getInfo_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[13];
((VFrameHeader*) op_getInfo_thread_spec) -> functionName = "getInfo";
((VFrameHeader*) op_exists_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_exists_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_exists_thread_spec) -> sub = NULL;
((VFrameHeader*) op_exists_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[14];
((VFrameHeader*) op_exists_thread_spec) -> functionName = "exists";
((VFrameHeader*) op_delete_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_delete_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_delete_thread_spec) -> sub = NULL;
((VFrameHeader*) op_delete_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[15];
((VFrameHeader*) op_delete_thread_spec) -> functionName = "delete";
((VFrameHeader*) op_move_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 88;
((VFrameHeader*) op_move_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_move_thread_spec) -> sub = NULL;
((VFrameHeader*) op_move_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[16];
((VFrameHeader*) op_move_thread_spec) -> functionName = "move";
((VFrameHeader*) op_copy_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 89;
((VFrameHeader*) op_copy_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_copy_thread_spec) -> sub = NULL;
((VFrameHeader*) op_copy_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[17];
((VFrameHeader*) op_copy_thread_spec) -> functionName = "copy";
((VFrameHeader*) op_createDirectory_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_createDirectory_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_createDirectory_thread_spec) -> sub = NULL;
((VFrameHeader*) op_createDirectory_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[18];
((VFrameHeader*) op_createDirectory_thread_spec) -> functionName = "createDirectory";
((VFrameHeader*) op_deleteDirectory_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_deleteDirectory_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_deleteDirectory_thread_spec) -> sub = NULL;
((VFrameHeader*) op_deleteDirectory_thread_spec) -> localsDef = (size_t) &object_FileLib_functions_spec[19];
((VFrameHeader*) op_deleteDirectory_thread_spec) -> functionName = "deleteDirectory";
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
{"open", (VFrameHeader*) op_open_thread_spec},
{"write", (VFrameHeader*) op_write_thread_spec},
{"read", (VFrameHeader*) op_read_thread_spec},
{"flush", (VFrameHeader*) op_flush_thread_spec},
{"setPos", (VFrameHeader*) op_setPos_thread_spec},
{"getSize", (VFrameHeader*) op_getSize_thread_spec},
{"eof", (VFrameHeader*) op_eof_thread_spec},
{"close", (VFrameHeader*) op_close_thread_spec},
{"getDirectoryContents", (VFrameHeader*) op_getDirectoryContents_thread_spec},
{"getInfo", (VFrameHeader*) op_getInfo_thread_spec},
{"exists", (VFrameHeader*) op_exists_thread_spec},
{"delete", (VFrameHeader*) op_delete_thread_spec},
{"move", (VFrameHeader*) op_move_thread_spec},
{"copy", (VFrameHeader*) op_copy_thread_spec},
{"createDirectory", (VFrameHeader*) op_createDirectory_thread_spec},
{"deleteDirectory", (VFrameHeader*) op_deleteDirectory_thread_spec}};
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
{"DateTime", &DateTime_def
},
{"FileInfo", &FileInfo_def
},
{"FileEntry", &FileEntry_def
},
{"FileEntry[]", &FileEntry_array_def
},
{"DCQ", &DCQ_def
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

