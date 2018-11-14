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
static const DanaTypeField function_MathLib_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_MathLib_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_MathLib_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_MathLib_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_MathLib_sqrt_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_pow_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40},
{(DanaType*) &char_array_def, NULL, 0, 0, 80}};
static const DanaTypeField function_MathLib_root_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40},
{(DanaType*) &char_array_def, NULL, 0, 0, 80}};
static const DanaTypeField function_MathLib_log_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_natlog_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_natexp_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_cos_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_sin_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_tan_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_acos_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_asin_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_atan_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_cosh_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_sinh_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_tanh_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_acosh_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_asinh_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaTypeField function_MathLib_atanh_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 40}};
static const DanaType object_MathLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_MathLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_MathLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_MathLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_MathLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_sqrt_fields, 2},
{TYPE_FUNCTION, 0, 120, (DanaTypeField*) &function_MathLib_pow_fields, 3},
{TYPE_FUNCTION, 0, 120, (DanaTypeField*) &function_MathLib_root_fields, 3},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_log_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_natlog_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_natexp_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_cos_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_sin_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_tan_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_acos_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_asin_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_atan_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_cosh_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_sinh_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_tanh_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_acosh_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_asinh_fields, 2},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_MathLib_atanh_fields, 2}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_MathLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_MathLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_MathLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_MathLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_MathLib_functions_spec[4], "sqrt", 4},
{(DanaType*) &object_MathLib_functions_spec[5], "pow", 3},
{(DanaType*) &object_MathLib_functions_spec[6], "root", 4},
{(DanaType*) &object_MathLib_functions_spec[7], "log", 3},
{(DanaType*) &object_MathLib_functions_spec[8], "natlog", 6},
{(DanaType*) &object_MathLib_functions_spec[9], "natexp", 6},
{(DanaType*) &object_MathLib_functions_spec[10], "cos", 3},
{(DanaType*) &object_MathLib_functions_spec[11], "sin", 3},
{(DanaType*) &object_MathLib_functions_spec[12], "tan", 3},
{(DanaType*) &object_MathLib_functions_spec[13], "acos", 4},
{(DanaType*) &object_MathLib_functions_spec[14], "asin", 4},
{(DanaType*) &object_MathLib_functions_spec[15], "atan", 4},
{(DanaType*) &object_MathLib_functions_spec[16], "cosh", 4},
{(DanaType*) &object_MathLib_functions_spec[17], "sinh", 4},
{(DanaType*) &object_MathLib_functions_spec[18], "tanh", 4},
{(DanaType*) &object_MathLib_functions_spec[19], "acosh", 5},
{(DanaType*) &object_MathLib_functions_spec[20], "asinh", 5},
{(DanaType*) &object_MathLib_functions_spec[21], "atanh", 5}};
static const DanaType MathLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 22},
{TYPE_DATA, 0, 0, NULL, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField intf_def[] = {
{(DanaType*) &MathLib_object_spec[0], ".functions", 10},
{(DanaType*) &MathLib_object_spec[1], ".events", 7},
{(DanaType*) &MathLib_object_spec[2], ".state", 6},
};
static unsigned char op_clone_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_equals_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_toString_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getID_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_sqrt_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_pow_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_root_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_log_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_natlog_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_natexp_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_cos_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_sin_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_tan_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_acos_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_asin_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_atan_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_cosh_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_sinh_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_tanh_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_acosh_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_asinh_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_atanh_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_sqrt_thread_spec,
(size_t) op_pow_thread_spec,
(size_t) op_root_thread_spec,
(size_t) op_log_thread_spec,
(size_t) op_natlog_thread_spec,
(size_t) op_natexp_thread_spec,
(size_t) op_cos_thread_spec,
(size_t) op_sin_thread_spec,
(size_t) op_tan_thread_spec,
(size_t) op_acos_thread_spec,
(size_t) op_asin_thread_spec,
(size_t) op_atan_thread_spec,
(size_t) op_cosh_thread_spec,
(size_t) op_sinh_thread_spec,
(size_t) op_tanh_thread_spec,
(size_t) op_acosh_thread_spec,
(size_t) op_asinh_thread_spec,
(size_t) op_atanh_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"MathLib", 7, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 0, 0, 0, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_sqrt_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_sqrt_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_sqrt_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[4];
((VFrameHeader*) op_sqrt_thread_spec) -> functionName = "sqrt";
((VFrameHeader*) op_pow_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 120;
((VFrameHeader*) op_pow_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_pow_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[5];
((VFrameHeader*) op_pow_thread_spec) -> functionName = "pow";
((VFrameHeader*) op_root_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 120;
((VFrameHeader*) op_root_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_root_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[6];
((VFrameHeader*) op_root_thread_spec) -> functionName = "root";
((VFrameHeader*) op_log_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_log_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_log_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[7];
((VFrameHeader*) op_log_thread_spec) -> functionName = "log";
((VFrameHeader*) op_natlog_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_natlog_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_natlog_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[8];
((VFrameHeader*) op_natlog_thread_spec) -> functionName = "natlog";
((VFrameHeader*) op_natexp_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_natexp_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_natexp_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[9];
((VFrameHeader*) op_natexp_thread_spec) -> functionName = "natexp";
((VFrameHeader*) op_cos_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_cos_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_cos_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[10];
((VFrameHeader*) op_cos_thread_spec) -> functionName = "cos";
((VFrameHeader*) op_sin_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_sin_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_sin_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[11];
((VFrameHeader*) op_sin_thread_spec) -> functionName = "sin";
((VFrameHeader*) op_tan_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_tan_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_tan_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[12];
((VFrameHeader*) op_tan_thread_spec) -> functionName = "tan";
((VFrameHeader*) op_acos_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_acos_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_acos_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[13];
((VFrameHeader*) op_acos_thread_spec) -> functionName = "acos";
((VFrameHeader*) op_asin_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_asin_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_asin_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[14];
((VFrameHeader*) op_asin_thread_spec) -> functionName = "asin";
((VFrameHeader*) op_atan_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_atan_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_atan_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[15];
((VFrameHeader*) op_atan_thread_spec) -> functionName = "atan";
((VFrameHeader*) op_cosh_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_cosh_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_cosh_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[16];
((VFrameHeader*) op_cosh_thread_spec) -> functionName = "cosh";
((VFrameHeader*) op_sinh_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_sinh_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_sinh_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[17];
((VFrameHeader*) op_sinh_thread_spec) -> functionName = "sinh";
((VFrameHeader*) op_tanh_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_tanh_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_tanh_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[18];
((VFrameHeader*) op_tanh_thread_spec) -> functionName = "tanh";
((VFrameHeader*) op_acosh_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_acosh_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_acosh_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[19];
((VFrameHeader*) op_acosh_thread_spec) -> functionName = "acosh";
((VFrameHeader*) op_asinh_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_asinh_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_asinh_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[20];
((VFrameHeader*) op_asinh_thread_spec) -> functionName = "asinh";
((VFrameHeader*) op_atanh_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_atanh_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_atanh_thread_spec) -> localsDef = (size_t) &object_MathLib_functions_spec[21];
((VFrameHeader*) op_atanh_thread_spec) -> functionName = "atanh";
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
{"sqrt", (VFrameHeader*) op_sqrt_thread_spec},
{"pow", (VFrameHeader*) op_pow_thread_spec},
{"root", (VFrameHeader*) op_root_thread_spec},
{"log", (VFrameHeader*) op_log_thread_spec},
{"natlog", (VFrameHeader*) op_natlog_thread_spec},
{"natexp", (VFrameHeader*) op_natexp_thread_spec},
{"cos", (VFrameHeader*) op_cos_thread_spec},
{"sin", (VFrameHeader*) op_sin_thread_spec},
{"tan", (VFrameHeader*) op_tan_thread_spec},
{"acos", (VFrameHeader*) op_acos_thread_spec},
{"asin", (VFrameHeader*) op_asin_thread_spec},
{"atan", (VFrameHeader*) op_atan_thread_spec},
{"cosh", (VFrameHeader*) op_cosh_thread_spec},
{"sinh", (VFrameHeader*) op_sinh_thread_spec},
{"tanh", (VFrameHeader*) op_tanh_thread_spec},
{"acosh", (VFrameHeader*) op_acosh_thread_spec},
{"asinh", (VFrameHeader*) op_asinh_thread_spec},
{"atanh", (VFrameHeader*) op_atanh_thread_spec}};
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

