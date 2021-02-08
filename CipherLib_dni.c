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
static const DanaTypeField String_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaType String_def = 
{TYPE_DATA, 0, 16, (DanaTypeField*) String_fields, 1};
static const DanaTypeField function_CipherLib_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_CipherLib_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_CipherLib_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_CipherLib_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_CipherLib_aes_cbc_encryptInit_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &char_array_def, NULL, 0, 0, 16},
{(DanaType*) &char_array_def, NULL, 0, 0, 32}};
static const DanaTypeField function_CipherLib_aes_cbc_encryptPart_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &byte_array_def, NULL, 0, 0, 24}};
static const DanaTypeField function_CipherLib_aes_cbc_encryptFinish_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_CipherLib_aes_cbc_encryptOK_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1}};
static const DanaTypeField function_CipherLib_aes_cbc_decryptInit_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &char_array_def, NULL, 0, 0, 16},
{(DanaType*) &char_array_def, NULL, 0, 0, 32}};
static const DanaTypeField function_CipherLib_aes_cbc_decryptPart_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &byte_array_def, NULL, 0, 0, 24}};
static const DanaTypeField function_CipherLib_aes_cbc_decryptFinish_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_CipherLib_aes_cbc_decryptOK_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1}};
static const DanaTypeField function_CipherLib_aes_gcm_encryptInit_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &char_array_def, NULL, 0, 0, 16},
{(DanaType*) &char_array_def, NULL, 0, 0, 32}};
static const DanaTypeField function_CipherLib_aes_gcm_encryptAAD_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &byte_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_CipherLib_aes_gcm_encryptPart_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &byte_array_def, NULL, 0, 0, 24}};
static const DanaTypeField function_CipherLib_aes_gcm_encryptFinish_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_CipherLib_aes_gcm_encryptOK_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1}};
static const DanaTypeField function_CipherLib_aes_gcm_encryptGetTag_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_CipherLib_aes_gcm_decryptInit_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &char_array_def, NULL, 0, 0, 16},
{(DanaType*) &char_array_def, NULL, 0, 0, 32}};
static const DanaTypeField function_CipherLib_aes_gcm_decryptAAD_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &byte_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_CipherLib_aes_gcm_decryptPart_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &byte_array_def, NULL, 0, 0, 24}};
static const DanaTypeField function_CipherLib_aes_gcm_decryptFinish_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_CipherLib_aes_gcm_decryptSetTag_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &byte_array_def, NULL, 0, 0, 8}};
static const DanaTypeField function_CipherLib_aes_gcm_decryptOK_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1}};
static const DanaTypeField function_CipherLib_instance_cleanup_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_CipherLib_rsa_oaep_init_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &byte_def, NULL, 0, 0, 24}};
static const DanaTypeField function_CipherLib_rsa_oaep_encrypt_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &byte_array_def, NULL, 0, 0, 24}};
static const DanaTypeField function_CipherLib_rsa_oaep_decrypt_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &byte_array_def, NULL, 0, 0, 24}};
static const DanaTypeField function_CipherLib_rsa_oaep_cleanup_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_CipherLib_rsa_pss_init_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &byte_def, NULL, 0, 0, 24}};
static const DanaTypeField function_CipherLib_rsa_pss_sign_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &byte_array_def, NULL, 0, 0, 24}};
static const DanaTypeField function_CipherLib_rsa_pss_verify_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &byte_array_def, NULL, 0, 0, 16},
{(DanaType*) &byte_array_def, NULL, 0, 0, 32}};
static const DanaTypeField function_CipherLib_rsa_pss_cleanup_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_CipherLib_rsa_generate_key_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &String_def, NULL, 0, 0, 16},
{(DanaType*) &String_def, NULL, 0, 0, 32}};
static const DanaTypeField function_CipherLib_rsa_convert_key_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &char_array_def, NULL, 0, 0, 24},
{(DanaType*) &byte_def, NULL, 0, 0, 40},
{(DanaType*) &String_def, NULL, 0, 0, 48},
{(DanaType*) &String_def, NULL, 0, 0, 64},
{(DanaType*) &byte_def, NULL, 0, 0, 80}};
static const DanaType object_CipherLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_CipherLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_CipherLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_CipherLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_CipherLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_CipherLib_aes_cbc_encryptInit_fields, 4},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_CipherLib_aes_cbc_encryptPart_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_CipherLib_aes_cbc_encryptFinish_fields, 2},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_CipherLib_aes_cbc_encryptOK_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_CipherLib_aes_cbc_decryptInit_fields, 4},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_CipherLib_aes_cbc_decryptPart_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_CipherLib_aes_cbc_decryptFinish_fields, 2},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_CipherLib_aes_cbc_decryptOK_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_CipherLib_aes_gcm_encryptInit_fields, 4},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_CipherLib_aes_gcm_encryptAAD_fields, 3},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_CipherLib_aes_gcm_encryptPart_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_CipherLib_aes_gcm_encryptFinish_fields, 2},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_CipherLib_aes_gcm_encryptOK_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_CipherLib_aes_gcm_encryptGetTag_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_CipherLib_aes_gcm_decryptInit_fields, 4},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_CipherLib_aes_gcm_decryptAAD_fields, 3},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_CipherLib_aes_gcm_decryptPart_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_CipherLib_aes_gcm_decryptFinish_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_CipherLib_aes_gcm_decryptSetTag_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_CipherLib_aes_gcm_decryptOK_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_CipherLib_instance_cleanup_fields, 2},
{TYPE_FUNCTION, 0, 25, (DanaTypeField*) &function_CipherLib_rsa_oaep_init_fields, 3},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_CipherLib_rsa_oaep_encrypt_fields, 3},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_CipherLib_rsa_oaep_decrypt_fields, 3},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_CipherLib_rsa_oaep_cleanup_fields, 2},
{TYPE_FUNCTION, 0, 25, (DanaTypeField*) &function_CipherLib_rsa_pss_init_fields, 3},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_CipherLib_rsa_pss_sign_fields, 3},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_CipherLib_rsa_pss_verify_fields, 4},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_CipherLib_rsa_pss_cleanup_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_CipherLib_rsa_generate_key_fields, 4},
{TYPE_FUNCTION, 0, 81, (DanaTypeField*) &function_CipherLib_rsa_convert_key_fields, 7}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_CipherLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_CipherLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_CipherLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_CipherLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_CipherLib_functions_spec[4], "aes_cbc_encryptInit", 19},
{(DanaType*) &object_CipherLib_functions_spec[5], "aes_cbc_encryptPart", 19},
{(DanaType*) &object_CipherLib_functions_spec[6], "aes_cbc_encryptFinish", 21},
{(DanaType*) &object_CipherLib_functions_spec[7], "aes_cbc_encryptOK", 17},
{(DanaType*) &object_CipherLib_functions_spec[8], "aes_cbc_decryptInit", 19},
{(DanaType*) &object_CipherLib_functions_spec[9], "aes_cbc_decryptPart", 19},
{(DanaType*) &object_CipherLib_functions_spec[10], "aes_cbc_decryptFinish", 21},
{(DanaType*) &object_CipherLib_functions_spec[11], "aes_cbc_decryptOK", 17},
{(DanaType*) &object_CipherLib_functions_spec[12], "aes_gcm_encryptInit", 19},
{(DanaType*) &object_CipherLib_functions_spec[13], "aes_gcm_encryptAAD", 18},
{(DanaType*) &object_CipherLib_functions_spec[14], "aes_gcm_encryptPart", 19},
{(DanaType*) &object_CipherLib_functions_spec[15], "aes_gcm_encryptFinish", 21},
{(DanaType*) &object_CipherLib_functions_spec[16], "aes_gcm_encryptOK", 17},
{(DanaType*) &object_CipherLib_functions_spec[17], "aes_gcm_encryptGetTag", 21},
{(DanaType*) &object_CipherLib_functions_spec[18], "aes_gcm_decryptInit", 19},
{(DanaType*) &object_CipherLib_functions_spec[19], "aes_gcm_decryptAAD", 18},
{(DanaType*) &object_CipherLib_functions_spec[20], "aes_gcm_decryptPart", 19},
{(DanaType*) &object_CipherLib_functions_spec[21], "aes_gcm_decryptFinish", 21},
{(DanaType*) &object_CipherLib_functions_spec[22], "aes_gcm_decryptSetTag", 21},
{(DanaType*) &object_CipherLib_functions_spec[23], "aes_gcm_decryptOK", 17},
{(DanaType*) &object_CipherLib_functions_spec[24], "instance_cleanup", 16},
{(DanaType*) &object_CipherLib_functions_spec[25], "rsa_oaep_init", 13},
{(DanaType*) &object_CipherLib_functions_spec[26], "rsa_oaep_encrypt", 16},
{(DanaType*) &object_CipherLib_functions_spec[27], "rsa_oaep_decrypt", 16},
{(DanaType*) &object_CipherLib_functions_spec[28], "rsa_oaep_cleanup", 16},
{(DanaType*) &object_CipherLib_functions_spec[29], "rsa_pss_init", 12},
{(DanaType*) &object_CipherLib_functions_spec[30], "rsa_pss_sign", 12},
{(DanaType*) &object_CipherLib_functions_spec[31], "rsa_pss_verify", 14},
{(DanaType*) &object_CipherLib_functions_spec[32], "rsa_pss_cleanup", 15},
{(DanaType*) &object_CipherLib_functions_spec[33], "rsa_generate_key", 16},
{(DanaType*) &object_CipherLib_functions_spec[34], "rsa_convert_key", 15}};
static const DanaType object_CipherLib_events_spec[] = {
};
static const DanaTypeField intf_events_def[] = {
};
static const DanaType CipherLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 35},
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_events_def, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField intf_def[] = {
{(DanaType*) &CipherLib_object_spec[0], ".functions", 10},
{(DanaType*) &CipherLib_object_spec[1], ".events", 7},
{(DanaType*) &CipherLib_object_spec[2], ".state", 6},
};
static unsigned char op_clone_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_equals_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_toString_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getID_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_cbc_encryptInit_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_cbc_encryptPart_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_cbc_encryptFinish_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_cbc_encryptOK_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_cbc_decryptInit_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_cbc_decryptPart_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_cbc_decryptFinish_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_cbc_decryptOK_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_gcm_encryptInit_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_gcm_encryptAAD_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_gcm_encryptPart_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_gcm_encryptFinish_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_gcm_encryptOK_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_gcm_encryptGetTag_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_gcm_decryptInit_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_gcm_decryptAAD_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_gcm_decryptPart_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_gcm_decryptFinish_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_gcm_decryptSetTag_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_aes_gcm_decryptOK_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_instance_cleanup_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_rsa_oaep_init_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_rsa_oaep_encrypt_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_rsa_oaep_decrypt_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_rsa_oaep_cleanup_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_rsa_pss_init_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_rsa_pss_sign_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_rsa_pss_verify_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_rsa_pss_cleanup_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_rsa_generate_key_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_rsa_convert_key_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_aes_cbc_encryptInit_thread_spec,
(size_t) op_aes_cbc_encryptPart_thread_spec,
(size_t) op_aes_cbc_encryptFinish_thread_spec,
(size_t) op_aes_cbc_encryptOK_thread_spec,
(size_t) op_aes_cbc_decryptInit_thread_spec,
(size_t) op_aes_cbc_decryptPart_thread_spec,
(size_t) op_aes_cbc_decryptFinish_thread_spec,
(size_t) op_aes_cbc_decryptOK_thread_spec,
(size_t) op_aes_gcm_encryptInit_thread_spec,
(size_t) op_aes_gcm_encryptAAD_thread_spec,
(size_t) op_aes_gcm_encryptPart_thread_spec,
(size_t) op_aes_gcm_encryptFinish_thread_spec,
(size_t) op_aes_gcm_encryptOK_thread_spec,
(size_t) op_aes_gcm_encryptGetTag_thread_spec,
(size_t) op_aes_gcm_decryptInit_thread_spec,
(size_t) op_aes_gcm_decryptAAD_thread_spec,
(size_t) op_aes_gcm_decryptPart_thread_spec,
(size_t) op_aes_gcm_decryptFinish_thread_spec,
(size_t) op_aes_gcm_decryptSetTag_thread_spec,
(size_t) op_aes_gcm_decryptOK_thread_spec,
(size_t) op_instance_cleanup_thread_spec,
(size_t) op_rsa_oaep_init_thread_spec,
(size_t) op_rsa_oaep_encrypt_thread_spec,
(size_t) op_rsa_oaep_decrypt_thread_spec,
(size_t) op_rsa_oaep_cleanup_thread_spec,
(size_t) op_rsa_pss_init_thread_spec,
(size_t) op_rsa_pss_sign_thread_spec,
(size_t) op_rsa_pss_verify_thread_spec,
(size_t) op_rsa_pss_cleanup_thread_spec,
(size_t) op_rsa_generate_key_thread_spec,
(size_t) op_rsa_convert_key_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"CipherLib", 9, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 0, 0, (size_t) &bool_def, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> sub = NULL;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> sub = NULL;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> sub = NULL;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_aes_cbc_encryptInit_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_aes_cbc_encryptInit_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_aes_cbc_encryptInit_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_cbc_encryptInit_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[4];
((VFrameHeader*) op_aes_cbc_encryptInit_thread_spec) -> functionName = "aes_cbc_encryptInit";
((VFrameHeader*) op_aes_cbc_encryptPart_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_aes_cbc_encryptPart_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_aes_cbc_encryptPart_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_cbc_encryptPart_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[5];
((VFrameHeader*) op_aes_cbc_encryptPart_thread_spec) -> functionName = "aes_cbc_encryptPart";
((VFrameHeader*) op_aes_cbc_encryptFinish_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_aes_cbc_encryptFinish_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_aes_cbc_encryptFinish_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_cbc_encryptFinish_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[6];
((VFrameHeader*) op_aes_cbc_encryptFinish_thread_spec) -> functionName = "aes_cbc_encryptFinish";
((VFrameHeader*) op_aes_cbc_encryptOK_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_aes_cbc_encryptOK_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_aes_cbc_encryptOK_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_cbc_encryptOK_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[7];
((VFrameHeader*) op_aes_cbc_encryptOK_thread_spec) -> functionName = "aes_cbc_encryptOK";
((VFrameHeader*) op_aes_cbc_decryptInit_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_aes_cbc_decryptInit_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_aes_cbc_decryptInit_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_cbc_decryptInit_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[8];
((VFrameHeader*) op_aes_cbc_decryptInit_thread_spec) -> functionName = "aes_cbc_decryptInit";
((VFrameHeader*) op_aes_cbc_decryptPart_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_aes_cbc_decryptPart_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_aes_cbc_decryptPart_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_cbc_decryptPart_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[9];
((VFrameHeader*) op_aes_cbc_decryptPart_thread_spec) -> functionName = "aes_cbc_decryptPart";
((VFrameHeader*) op_aes_cbc_decryptFinish_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_aes_cbc_decryptFinish_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_aes_cbc_decryptFinish_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_cbc_decryptFinish_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[10];
((VFrameHeader*) op_aes_cbc_decryptFinish_thread_spec) -> functionName = "aes_cbc_decryptFinish";
((VFrameHeader*) op_aes_cbc_decryptOK_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_aes_cbc_decryptOK_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_aes_cbc_decryptOK_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_cbc_decryptOK_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[11];
((VFrameHeader*) op_aes_cbc_decryptOK_thread_spec) -> functionName = "aes_cbc_decryptOK";
((VFrameHeader*) op_aes_gcm_encryptInit_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_aes_gcm_encryptInit_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_aes_gcm_encryptInit_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_gcm_encryptInit_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[12];
((VFrameHeader*) op_aes_gcm_encryptInit_thread_spec) -> functionName = "aes_gcm_encryptInit";
((VFrameHeader*) op_aes_gcm_encryptAAD_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_aes_gcm_encryptAAD_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_aes_gcm_encryptAAD_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_gcm_encryptAAD_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[13];
((VFrameHeader*) op_aes_gcm_encryptAAD_thread_spec) -> functionName = "aes_gcm_encryptAAD";
((VFrameHeader*) op_aes_gcm_encryptPart_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_aes_gcm_encryptPart_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_aes_gcm_encryptPart_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_gcm_encryptPart_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[14];
((VFrameHeader*) op_aes_gcm_encryptPart_thread_spec) -> functionName = "aes_gcm_encryptPart";
((VFrameHeader*) op_aes_gcm_encryptFinish_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_aes_gcm_encryptFinish_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_aes_gcm_encryptFinish_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_gcm_encryptFinish_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[15];
((VFrameHeader*) op_aes_gcm_encryptFinish_thread_spec) -> functionName = "aes_gcm_encryptFinish";
((VFrameHeader*) op_aes_gcm_encryptOK_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_aes_gcm_encryptOK_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_aes_gcm_encryptOK_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_gcm_encryptOK_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[16];
((VFrameHeader*) op_aes_gcm_encryptOK_thread_spec) -> functionName = "aes_gcm_encryptOK";
((VFrameHeader*) op_aes_gcm_encryptGetTag_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_aes_gcm_encryptGetTag_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_aes_gcm_encryptGetTag_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_gcm_encryptGetTag_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[17];
((VFrameHeader*) op_aes_gcm_encryptGetTag_thread_spec) -> functionName = "aes_gcm_encryptGetTag";
((VFrameHeader*) op_aes_gcm_decryptInit_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_aes_gcm_decryptInit_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_aes_gcm_decryptInit_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_gcm_decryptInit_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[18];
((VFrameHeader*) op_aes_gcm_decryptInit_thread_spec) -> functionName = "aes_gcm_decryptInit";
((VFrameHeader*) op_aes_gcm_decryptAAD_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_aes_gcm_decryptAAD_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_aes_gcm_decryptAAD_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_gcm_decryptAAD_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[19];
((VFrameHeader*) op_aes_gcm_decryptAAD_thread_spec) -> functionName = "aes_gcm_decryptAAD";
((VFrameHeader*) op_aes_gcm_decryptPart_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_aes_gcm_decryptPart_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_aes_gcm_decryptPart_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_gcm_decryptPart_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[20];
((VFrameHeader*) op_aes_gcm_decryptPart_thread_spec) -> functionName = "aes_gcm_decryptPart";
((VFrameHeader*) op_aes_gcm_decryptFinish_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_aes_gcm_decryptFinish_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_aes_gcm_decryptFinish_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_gcm_decryptFinish_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[21];
((VFrameHeader*) op_aes_gcm_decryptFinish_thread_spec) -> functionName = "aes_gcm_decryptFinish";
((VFrameHeader*) op_aes_gcm_decryptSetTag_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_aes_gcm_decryptSetTag_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_aes_gcm_decryptSetTag_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_gcm_decryptSetTag_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[22];
((VFrameHeader*) op_aes_gcm_decryptSetTag_thread_spec) -> functionName = "aes_gcm_decryptSetTag";
((VFrameHeader*) op_aes_gcm_decryptOK_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_aes_gcm_decryptOK_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_aes_gcm_decryptOK_thread_spec) -> sub = NULL;
((VFrameHeader*) op_aes_gcm_decryptOK_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[23];
((VFrameHeader*) op_aes_gcm_decryptOK_thread_spec) -> functionName = "aes_gcm_decryptOK";
((VFrameHeader*) op_instance_cleanup_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_instance_cleanup_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_instance_cleanup_thread_spec) -> sub = NULL;
((VFrameHeader*) op_instance_cleanup_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[24];
((VFrameHeader*) op_instance_cleanup_thread_spec) -> functionName = "instance_cleanup";
((VFrameHeader*) op_rsa_oaep_init_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 25;
((VFrameHeader*) op_rsa_oaep_init_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_rsa_oaep_init_thread_spec) -> sub = NULL;
((VFrameHeader*) op_rsa_oaep_init_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[25];
((VFrameHeader*) op_rsa_oaep_init_thread_spec) -> functionName = "rsa_oaep_init";
((VFrameHeader*) op_rsa_oaep_encrypt_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_rsa_oaep_encrypt_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_rsa_oaep_encrypt_thread_spec) -> sub = NULL;
((VFrameHeader*) op_rsa_oaep_encrypt_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[26];
((VFrameHeader*) op_rsa_oaep_encrypt_thread_spec) -> functionName = "rsa_oaep_encrypt";
((VFrameHeader*) op_rsa_oaep_decrypt_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_rsa_oaep_decrypt_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_rsa_oaep_decrypt_thread_spec) -> sub = NULL;
((VFrameHeader*) op_rsa_oaep_decrypt_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[27];
((VFrameHeader*) op_rsa_oaep_decrypt_thread_spec) -> functionName = "rsa_oaep_decrypt";
((VFrameHeader*) op_rsa_oaep_cleanup_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_rsa_oaep_cleanup_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_rsa_oaep_cleanup_thread_spec) -> sub = NULL;
((VFrameHeader*) op_rsa_oaep_cleanup_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[28];
((VFrameHeader*) op_rsa_oaep_cleanup_thread_spec) -> functionName = "rsa_oaep_cleanup";
((VFrameHeader*) op_rsa_pss_init_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 25;
((VFrameHeader*) op_rsa_pss_init_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_rsa_pss_init_thread_spec) -> sub = NULL;
((VFrameHeader*) op_rsa_pss_init_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[29];
((VFrameHeader*) op_rsa_pss_init_thread_spec) -> functionName = "rsa_pss_init";
((VFrameHeader*) op_rsa_pss_sign_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_rsa_pss_sign_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_rsa_pss_sign_thread_spec) -> sub = NULL;
((VFrameHeader*) op_rsa_pss_sign_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[30];
((VFrameHeader*) op_rsa_pss_sign_thread_spec) -> functionName = "rsa_pss_sign";
((VFrameHeader*) op_rsa_pss_verify_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_rsa_pss_verify_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_rsa_pss_verify_thread_spec) -> sub = NULL;
((VFrameHeader*) op_rsa_pss_verify_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[31];
((VFrameHeader*) op_rsa_pss_verify_thread_spec) -> functionName = "rsa_pss_verify";
((VFrameHeader*) op_rsa_pss_cleanup_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_rsa_pss_cleanup_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_rsa_pss_cleanup_thread_spec) -> sub = NULL;
((VFrameHeader*) op_rsa_pss_cleanup_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[32];
((VFrameHeader*) op_rsa_pss_cleanup_thread_spec) -> functionName = "rsa_pss_cleanup";
((VFrameHeader*) op_rsa_generate_key_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_rsa_generate_key_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_rsa_generate_key_thread_spec) -> sub = NULL;
((VFrameHeader*) op_rsa_generate_key_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[33];
((VFrameHeader*) op_rsa_generate_key_thread_spec) -> functionName = "rsa_generate_key";
((VFrameHeader*) op_rsa_convert_key_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 81;
((VFrameHeader*) op_rsa_convert_key_thread_spec) -> formalParamsCount = 6;
((VFrameHeader*) op_rsa_convert_key_thread_spec) -> sub = NULL;
((VFrameHeader*) op_rsa_convert_key_thread_spec) -> localsDef = (size_t) &object_CipherLib_functions_spec[34];
((VFrameHeader*) op_rsa_convert_key_thread_spec) -> functionName = "rsa_convert_key";
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
{"aes_cbc_encryptInit", (VFrameHeader*) op_aes_cbc_encryptInit_thread_spec},
{"aes_cbc_encryptPart", (VFrameHeader*) op_aes_cbc_encryptPart_thread_spec},
{"aes_cbc_encryptFinish", (VFrameHeader*) op_aes_cbc_encryptFinish_thread_spec},
{"aes_cbc_encryptOK", (VFrameHeader*) op_aes_cbc_encryptOK_thread_spec},
{"aes_cbc_decryptInit", (VFrameHeader*) op_aes_cbc_decryptInit_thread_spec},
{"aes_cbc_decryptPart", (VFrameHeader*) op_aes_cbc_decryptPart_thread_spec},
{"aes_cbc_decryptFinish", (VFrameHeader*) op_aes_cbc_decryptFinish_thread_spec},
{"aes_cbc_decryptOK", (VFrameHeader*) op_aes_cbc_decryptOK_thread_spec},
{"aes_gcm_encryptInit", (VFrameHeader*) op_aes_gcm_encryptInit_thread_spec},
{"aes_gcm_encryptAAD", (VFrameHeader*) op_aes_gcm_encryptAAD_thread_spec},
{"aes_gcm_encryptPart", (VFrameHeader*) op_aes_gcm_encryptPart_thread_spec},
{"aes_gcm_encryptFinish", (VFrameHeader*) op_aes_gcm_encryptFinish_thread_spec},
{"aes_gcm_encryptOK", (VFrameHeader*) op_aes_gcm_encryptOK_thread_spec},
{"aes_gcm_encryptGetTag", (VFrameHeader*) op_aes_gcm_encryptGetTag_thread_spec},
{"aes_gcm_decryptInit", (VFrameHeader*) op_aes_gcm_decryptInit_thread_spec},
{"aes_gcm_decryptAAD", (VFrameHeader*) op_aes_gcm_decryptAAD_thread_spec},
{"aes_gcm_decryptPart", (VFrameHeader*) op_aes_gcm_decryptPart_thread_spec},
{"aes_gcm_decryptFinish", (VFrameHeader*) op_aes_gcm_decryptFinish_thread_spec},
{"aes_gcm_decryptSetTag", (VFrameHeader*) op_aes_gcm_decryptSetTag_thread_spec},
{"aes_gcm_decryptOK", (VFrameHeader*) op_aes_gcm_decryptOK_thread_spec},
{"instance_cleanup", (VFrameHeader*) op_instance_cleanup_thread_spec},
{"rsa_oaep_init", (VFrameHeader*) op_rsa_oaep_init_thread_spec},
{"rsa_oaep_encrypt", (VFrameHeader*) op_rsa_oaep_encrypt_thread_spec},
{"rsa_oaep_decrypt", (VFrameHeader*) op_rsa_oaep_decrypt_thread_spec},
{"rsa_oaep_cleanup", (VFrameHeader*) op_rsa_oaep_cleanup_thread_spec},
{"rsa_pss_init", (VFrameHeader*) op_rsa_pss_init_thread_spec},
{"rsa_pss_sign", (VFrameHeader*) op_rsa_pss_sign_thread_spec},
{"rsa_pss_verify", (VFrameHeader*) op_rsa_pss_verify_thread_spec},
{"rsa_pss_cleanup", (VFrameHeader*) op_rsa_pss_cleanup_thread_spec},
{"rsa_generate_key", (VFrameHeader*) op_rsa_generate_key_thread_spec},
{"rsa_convert_key", (VFrameHeader*) op_rsa_convert_key_thread_spec}};
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

