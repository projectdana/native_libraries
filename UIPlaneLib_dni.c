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
static const DanaType byte_def = 
{TYPE_LITERAL, 0, 1, NULL, 0};
static const DanaTypeField WH_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaType WH_def = 
{TYPE_DATA, 0, 16, (DanaTypeField*) WH_fields, 2};
static const DanaTypeField byte_array_fields[] = {
{(DanaType*) &byte_def, NULL, 0, 0, 0}};
static const DanaType byte_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) byte_array_fields, 1};
static const DanaTypeField PixelMap_fields[] = {
{(DanaType*) &WH_def, NULL, 0, 0, 0},
{(DanaType*) &byte_array_def, NULL, 0, 0, 0}};
static const DanaType PixelMap_def = 
{TYPE_DATA, 0, 80, (DanaTypeField*) PixelMap_fields, 2};
static const DanaTypeField Rect_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaType Rect_def = 
{TYPE_DATA, 0, 32, (DanaTypeField*) Rect_fields, 4};
static const DanaTypeField FontMetrics_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaType FontMetrics_def = 
{TYPE_DATA, 0, 32, (DanaTypeField*) FontMetrics_fields, 4};
static const DanaTypeField function_UIPlaneLib_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_UIPlaneLib_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_UIPlaneLib_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_makeWindow_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_startPoly_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_addPolyPoint_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &byte_def, NULL, 0, 0, 16},
{(DanaType*) &byte_def, NULL, 0, 0, 17},
{(DanaType*) &byte_def, NULL, 0, 0, 18},
{(DanaType*) &byte_def, NULL, 0, 0, 19}};
static const DanaTypeField function_UIPlaneLib_endPoly_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_addRect_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 41},
{(DanaType*) &byte_def, NULL, 0, 0, 42},
{(DanaType*) &byte_def, NULL, 0, 0, 43}};
static const DanaTypeField function_UIPlaneLib_addLine_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 41},
{(DanaType*) &byte_def, NULL, 0, 0, 42},
{(DanaType*) &byte_def, NULL, 0, 0, 43}};
static const DanaTypeField function_UIPlaneLib_addPoint_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &byte_def, NULL, 0, 0, 24},
{(DanaType*) &byte_def, NULL, 0, 0, 25},
{(DanaType*) &byte_def, NULL, 0, 0, 26},
{(DanaType*) &byte_def, NULL, 0, 0, 27}};
static const DanaTypeField function_UIPlaneLib_addBitmap_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &PixelMap_def, NULL, 0, 0, 8},
{(DanaType*) &Rect_def, NULL, 0, 0, 48},
{(DanaType*) &int_def, NULL, 0, 0, 88},
{(DanaType*) &int_def, NULL, 0, 0, 96},
{(DanaType*) &int_def, NULL, 0, 0, 104},
{(DanaType*) &int_def, NULL, 0, 0, 112},
{(DanaType*) &int_def, NULL, 0, 0, 120}};
static const DanaTypeField function_UIPlaneLib_addText_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &char_array_def, NULL, 0, 0, 24},
{(DanaType*) &byte_def, NULL, 0, 0, 64},
{(DanaType*) &byte_def, NULL, 0, 0, 65},
{(DanaType*) &byte_def, NULL, 0, 0, 66},
{(DanaType*) &byte_def, NULL, 0, 0, 67}};
static const DanaTypeField function_UIPlaneLib_addTextWith_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &char_array_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 72},
{(DanaType*) &byte_def, NULL, 0, 0, 73},
{(DanaType*) &byte_def, NULL, 0, 0, 74},
{(DanaType*) &byte_def, NULL, 0, 0, 75}};
static const DanaTypeField function_UIPlaneLib_pushSurface_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &int_def, NULL, 0, 0, 48},
{(DanaType*) &byte_def, NULL, 0, 0, 56}};
static const DanaTypeField function_UIPlaneLib_popSurface_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_setSize_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_setPosition_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_setVisible_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &bool_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_setResizable_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &bool_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_setFullScreen_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &bool_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_setTitle_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &char_array_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_setIcon_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &PixelMap_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_commitBuffer_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_getResolution_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &WH_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_setBackgroundColor_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &byte_def, NULL, 0, 0, 8},
{(DanaType*) &byte_def, NULL, 0, 0, 9},
{(DanaType*) &byte_def, NULL, 0, 0, 10},
{(DanaType*) &byte_def, NULL, 0, 0, 11}};
static const DanaTypeField function_UIPlaneLib_maximiseWindow_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_minimiseWindow_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_getMaximisedScreenRect_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &Rect_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_closeWindow_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_loadFont_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 48}};
static const DanaTypeField function_UIPlaneLib_getTextWidth_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &char_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_getFontMetrics_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &FontMetrics_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_getFontName_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 40}};
static const DanaTypeField function_UIPlaneLib_isFontFixedWidth_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1}};
static const DanaTypeField function_UIPlaneLib_getTextBitmapWith_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &PixelMap_def, NULL, 0, 0, 48},
{(DanaType*) &byte_def, NULL, 0, 0, 88},
{(DanaType*) &byte_def, NULL, 0, 0, 89},
{(DanaType*) &byte_def, NULL, 0, 0, 90},
{(DanaType*) &byte_def, NULL, 0, 0, 91}};
static const DanaTypeField function_UIPlaneLib_unloadFont_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaType object_UIPlaneLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_UIPlaneLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_UIPlaneLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_UIPlaneLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_UIPlaneLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_makeWindow_fields, 1},
{TYPE_FUNCTION, 0, 0, (DanaTypeField*) &function_UIPlaneLib_startPoly_fields, 1},
{TYPE_FUNCTION, 0, 20, (DanaTypeField*) &function_UIPlaneLib_addPolyPoint_fields, 7},
{TYPE_FUNCTION, 0, 0, (DanaTypeField*) &function_UIPlaneLib_endPoly_fields, 1},
{TYPE_FUNCTION, 0, 44, (DanaTypeField*) &function_UIPlaneLib_addRect_fields, 10},
{TYPE_FUNCTION, 0, 44, (DanaTypeField*) &function_UIPlaneLib_addLine_fields, 10},
{TYPE_FUNCTION, 0, 28, (DanaTypeField*) &function_UIPlaneLib_addPoint_fields, 8},
{TYPE_FUNCTION, 0, 128, (DanaTypeField*) &function_UIPlaneLib_addBitmap_fields, 9},
{TYPE_FUNCTION, 0, 68, (DanaTypeField*) &function_UIPlaneLib_addText_fields, 9},
{TYPE_FUNCTION, 0, 76, (DanaTypeField*) &function_UIPlaneLib_addTextWith_fields, 10},
{TYPE_FUNCTION, 0, 57, (DanaTypeField*) &function_UIPlaneLib_pushSurface_fields, 9},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_popSurface_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_setSize_fields, 4},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_setPosition_fields, 4},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_UIPlaneLib_setVisible_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_UIPlaneLib_setResizable_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_UIPlaneLib_setFullScreen_fields, 3},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_UIPlaneLib_setTitle_fields, 3},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_UIPlaneLib_setIcon_fields, 3},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_commitBuffer_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_UIPlaneLib_getResolution_fields, 3},
{TYPE_FUNCTION, 0, 12, (DanaTypeField*) &function_UIPlaneLib_setBackgroundColor_fields, 6},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_maximiseWindow_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_minimiseWindow_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_UIPlaneLib_getMaximisedScreenRect_fields, 3},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_closeWindow_fields, 2},
{TYPE_FUNCTION, 0, 56, (DanaTypeField*) &function_UIPlaneLib_loadFont_fields, 3},
{TYPE_FUNCTION, 0, 56, (DanaTypeField*) &function_UIPlaneLib_getTextWidth_fields, 3},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_UIPlaneLib_getFontMetrics_fields, 3},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_UIPlaneLib_getFontName_fields, 2},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_UIPlaneLib_isFontFixedWidth_fields, 2},
{TYPE_FUNCTION, 0, 92, (DanaTypeField*) &function_UIPlaneLib_getTextBitmapWith_fields, 8},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_unloadFont_fields, 2}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_UIPlaneLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_UIPlaneLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_UIPlaneLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_UIPlaneLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_UIPlaneLib_functions_spec[4], "makeWindow", 10},
{(DanaType*) &object_UIPlaneLib_functions_spec[5], "startPoly", 9},
{(DanaType*) &object_UIPlaneLib_functions_spec[6], "addPolyPoint", 12},
{(DanaType*) &object_UIPlaneLib_functions_spec[7], "endPoly", 7},
{(DanaType*) &object_UIPlaneLib_functions_spec[8], "addRect", 7},
{(DanaType*) &object_UIPlaneLib_functions_spec[9], "addLine", 7},
{(DanaType*) &object_UIPlaneLib_functions_spec[10], "addPoint", 8},
{(DanaType*) &object_UIPlaneLib_functions_spec[11], "addBitmap", 9},
{(DanaType*) &object_UIPlaneLib_functions_spec[12], "addText", 7},
{(DanaType*) &object_UIPlaneLib_functions_spec[13], "addTextWith", 11},
{(DanaType*) &object_UIPlaneLib_functions_spec[14], "pushSurface", 11},
{(DanaType*) &object_UIPlaneLib_functions_spec[15], "popSurface", 10},
{(DanaType*) &object_UIPlaneLib_functions_spec[16], "setSize", 7},
{(DanaType*) &object_UIPlaneLib_functions_spec[17], "setPosition", 11},
{(DanaType*) &object_UIPlaneLib_functions_spec[18], "setVisible", 10},
{(DanaType*) &object_UIPlaneLib_functions_spec[19], "setResizable", 12},
{(DanaType*) &object_UIPlaneLib_functions_spec[20], "setFullScreen", 13},
{(DanaType*) &object_UIPlaneLib_functions_spec[21], "setTitle", 8},
{(DanaType*) &object_UIPlaneLib_functions_spec[22], "setIcon", 7},
{(DanaType*) &object_UIPlaneLib_functions_spec[23], "commitBuffer", 12},
{(DanaType*) &object_UIPlaneLib_functions_spec[24], "getResolution", 13},
{(DanaType*) &object_UIPlaneLib_functions_spec[25], "setBackgroundColor", 18},
{(DanaType*) &object_UIPlaneLib_functions_spec[26], "maximiseWindow", 14},
{(DanaType*) &object_UIPlaneLib_functions_spec[27], "minimiseWindow", 14},
{(DanaType*) &object_UIPlaneLib_functions_spec[28], "getMaximisedScreenRect", 22},
{(DanaType*) &object_UIPlaneLib_functions_spec[29], "closeWindow", 11},
{(DanaType*) &object_UIPlaneLib_functions_spec[30], "loadFont", 8},
{(DanaType*) &object_UIPlaneLib_functions_spec[31], "getTextWidth", 12},
{(DanaType*) &object_UIPlaneLib_functions_spec[32], "getFontMetrics", 14},
{(DanaType*) &object_UIPlaneLib_functions_spec[33], "getFontName", 11},
{(DanaType*) &object_UIPlaneLib_functions_spec[34], "isFontFixedWidth", 16},
{(DanaType*) &object_UIPlaneLib_functions_spec[35], "getTextBitmapWith", 17},
{(DanaType*) &object_UIPlaneLib_functions_spec[36], "unloadFont", 10}};
static const DanaType UIPlaneLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 37},
{TYPE_DATA, 0, 0, NULL, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField intf_def[] = {
{(DanaType*) &UIPlaneLib_object_spec[0], ".functions", 10},
{(DanaType*) &UIPlaneLib_object_spec[1], ".events", 7},
{(DanaType*) &UIPlaneLib_object_spec[2], ".state", 6},
};
static unsigned char op_clone_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_equals_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_toString_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getID_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_makeWindow_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_startPoly_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addPolyPoint_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_endPoly_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addRect_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addLine_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addPoint_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addBitmap_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addText_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addTextWith_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_pushSurface_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_popSurface_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setSize_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setPosition_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setVisible_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setResizable_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setFullScreen_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setTitle_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setIcon_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_commitBuffer_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getResolution_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setBackgroundColor_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_maximiseWindow_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_minimiseWindow_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getMaximisedScreenRect_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_closeWindow_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_loadFont_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getTextWidth_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getFontMetrics_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getFontName_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_isFontFixedWidth_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getTextBitmapWith_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_unloadFont_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_makeWindow_thread_spec,
(size_t) op_startPoly_thread_spec,
(size_t) op_addPolyPoint_thread_spec,
(size_t) op_endPoly_thread_spec,
(size_t) op_addRect_thread_spec,
(size_t) op_addLine_thread_spec,
(size_t) op_addPoint_thread_spec,
(size_t) op_addBitmap_thread_spec,
(size_t) op_addText_thread_spec,
(size_t) op_addTextWith_thread_spec,
(size_t) op_pushSurface_thread_spec,
(size_t) op_popSurface_thread_spec,
(size_t) op_setSize_thread_spec,
(size_t) op_setPosition_thread_spec,
(size_t) op_setVisible_thread_spec,
(size_t) op_setResizable_thread_spec,
(size_t) op_setFullScreen_thread_spec,
(size_t) op_setTitle_thread_spec,
(size_t) op_setIcon_thread_spec,
(size_t) op_commitBuffer_thread_spec,
(size_t) op_getResolution_thread_spec,
(size_t) op_setBackgroundColor_thread_spec,
(size_t) op_maximiseWindow_thread_spec,
(size_t) op_minimiseWindow_thread_spec,
(size_t) op_getMaximisedScreenRect_thread_spec,
(size_t) op_closeWindow_thread_spec,
(size_t) op_loadFont_thread_spec,
(size_t) op_getTextWidth_thread_spec,
(size_t) op_getFontMetrics_thread_spec,
(size_t) op_getFontName_thread_spec,
(size_t) op_isFontFixedWidth_thread_spec,
(size_t) op_getTextBitmapWith_thread_spec,
(size_t) op_unloadFont_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"UIPlaneLib", 10, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 0, 0, 0, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_makeWindow_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_makeWindow_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_makeWindow_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[4];
((VFrameHeader*) op_makeWindow_thread_spec) -> functionName = "makeWindow";
((VFrameHeader*) op_startPoly_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 0;
((VFrameHeader*) op_startPoly_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_startPoly_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[5];
((VFrameHeader*) op_startPoly_thread_spec) -> functionName = "startPoly";
((VFrameHeader*) op_addPolyPoint_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 20;
((VFrameHeader*) op_addPolyPoint_thread_spec) -> formalParamsCount = 6;
((VFrameHeader*) op_addPolyPoint_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[6];
((VFrameHeader*) op_addPolyPoint_thread_spec) -> functionName = "addPolyPoint";
((VFrameHeader*) op_endPoly_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 0;
((VFrameHeader*) op_endPoly_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_endPoly_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[7];
((VFrameHeader*) op_endPoly_thread_spec) -> functionName = "endPoly";
((VFrameHeader*) op_addRect_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 44;
((VFrameHeader*) op_addRect_thread_spec) -> formalParamsCount = 9;
((VFrameHeader*) op_addRect_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[8];
((VFrameHeader*) op_addRect_thread_spec) -> functionName = "addRect";
((VFrameHeader*) op_addLine_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 44;
((VFrameHeader*) op_addLine_thread_spec) -> formalParamsCount = 9;
((VFrameHeader*) op_addLine_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[9];
((VFrameHeader*) op_addLine_thread_spec) -> functionName = "addLine";
((VFrameHeader*) op_addPoint_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 28;
((VFrameHeader*) op_addPoint_thread_spec) -> formalParamsCount = 7;
((VFrameHeader*) op_addPoint_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[10];
((VFrameHeader*) op_addPoint_thread_spec) -> functionName = "addPoint";
((VFrameHeader*) op_addBitmap_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 128;
((VFrameHeader*) op_addBitmap_thread_spec) -> formalParamsCount = 8;
((VFrameHeader*) op_addBitmap_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[11];
((VFrameHeader*) op_addBitmap_thread_spec) -> functionName = "addBitmap";
((VFrameHeader*) op_addText_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 68;
((VFrameHeader*) op_addText_thread_spec) -> formalParamsCount = 8;
((VFrameHeader*) op_addText_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[12];
((VFrameHeader*) op_addText_thread_spec) -> functionName = "addText";
((VFrameHeader*) op_addTextWith_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 76;
((VFrameHeader*) op_addTextWith_thread_spec) -> formalParamsCount = 9;
((VFrameHeader*) op_addTextWith_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[13];
((VFrameHeader*) op_addTextWith_thread_spec) -> functionName = "addTextWith";
((VFrameHeader*) op_pushSurface_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 57;
((VFrameHeader*) op_pushSurface_thread_spec) -> formalParamsCount = 8;
((VFrameHeader*) op_pushSurface_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[14];
((VFrameHeader*) op_pushSurface_thread_spec) -> functionName = "pushSurface";
((VFrameHeader*) op_popSurface_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_popSurface_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_popSurface_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[15];
((VFrameHeader*) op_popSurface_thread_spec) -> functionName = "popSurface";
((VFrameHeader*) op_setSize_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_setSize_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_setSize_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[16];
((VFrameHeader*) op_setSize_thread_spec) -> functionName = "setSize";
((VFrameHeader*) op_setPosition_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_setPosition_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_setPosition_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[17];
((VFrameHeader*) op_setPosition_thread_spec) -> functionName = "setPosition";
((VFrameHeader*) op_setVisible_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_setVisible_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setVisible_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[18];
((VFrameHeader*) op_setVisible_thread_spec) -> functionName = "setVisible";
((VFrameHeader*) op_setResizable_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_setResizable_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setResizable_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[19];
((VFrameHeader*) op_setResizable_thread_spec) -> functionName = "setResizable";
((VFrameHeader*) op_setFullScreen_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_setFullScreen_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setFullScreen_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[20];
((VFrameHeader*) op_setFullScreen_thread_spec) -> functionName = "setFullScreen";
((VFrameHeader*) op_setTitle_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_setTitle_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setTitle_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[21];
((VFrameHeader*) op_setTitle_thread_spec) -> functionName = "setTitle";
((VFrameHeader*) op_setIcon_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_setIcon_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setIcon_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[22];
((VFrameHeader*) op_setIcon_thread_spec) -> functionName = "setIcon";
((VFrameHeader*) op_commitBuffer_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_commitBuffer_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_commitBuffer_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[23];
((VFrameHeader*) op_commitBuffer_thread_spec) -> functionName = "commitBuffer";
((VFrameHeader*) op_getResolution_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_getResolution_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_getResolution_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[24];
((VFrameHeader*) op_getResolution_thread_spec) -> functionName = "getResolution";
((VFrameHeader*) op_setBackgroundColor_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 12;
((VFrameHeader*) op_setBackgroundColor_thread_spec) -> formalParamsCount = 5;
((VFrameHeader*) op_setBackgroundColor_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[25];
((VFrameHeader*) op_setBackgroundColor_thread_spec) -> functionName = "setBackgroundColor";
((VFrameHeader*) op_maximiseWindow_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_maximiseWindow_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_maximiseWindow_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[26];
((VFrameHeader*) op_maximiseWindow_thread_spec) -> functionName = "maximiseWindow";
((VFrameHeader*) op_minimiseWindow_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_minimiseWindow_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_minimiseWindow_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[27];
((VFrameHeader*) op_minimiseWindow_thread_spec) -> functionName = "minimiseWindow";
((VFrameHeader*) op_getMaximisedScreenRect_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_getMaximisedScreenRect_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_getMaximisedScreenRect_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[28];
((VFrameHeader*) op_getMaximisedScreenRect_thread_spec) -> functionName = "getMaximisedScreenRect";
((VFrameHeader*) op_closeWindow_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_closeWindow_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_closeWindow_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[29];
((VFrameHeader*) op_closeWindow_thread_spec) -> functionName = "closeWindow";
((VFrameHeader*) op_loadFont_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 56;
((VFrameHeader*) op_loadFont_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_loadFont_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[30];
((VFrameHeader*) op_loadFont_thread_spec) -> functionName = "loadFont";
((VFrameHeader*) op_getTextWidth_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 56;
((VFrameHeader*) op_getTextWidth_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_getTextWidth_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[31];
((VFrameHeader*) op_getTextWidth_thread_spec) -> functionName = "getTextWidth";
((VFrameHeader*) op_getFontMetrics_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_getFontMetrics_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_getFontMetrics_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[32];
((VFrameHeader*) op_getFontMetrics_thread_spec) -> functionName = "getFontMetrics";
((VFrameHeader*) op_getFontName_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_getFontName_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_getFontName_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[33];
((VFrameHeader*) op_getFontName_thread_spec) -> functionName = "getFontName";
((VFrameHeader*) op_isFontFixedWidth_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_isFontFixedWidth_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_isFontFixedWidth_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[34];
((VFrameHeader*) op_isFontFixedWidth_thread_spec) -> functionName = "isFontFixedWidth";
((VFrameHeader*) op_getTextBitmapWith_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 92;
((VFrameHeader*) op_getTextBitmapWith_thread_spec) -> formalParamsCount = 7;
((VFrameHeader*) op_getTextBitmapWith_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[35];
((VFrameHeader*) op_getTextBitmapWith_thread_spec) -> functionName = "getTextBitmapWith";
((VFrameHeader*) op_unloadFont_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_unloadFont_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_unloadFont_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[36];
((VFrameHeader*) op_unloadFont_thread_spec) -> functionName = "unloadFont";
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
{"makeWindow", (VFrameHeader*) op_makeWindow_thread_spec},
{"startPoly", (VFrameHeader*) op_startPoly_thread_spec},
{"addPolyPoint", (VFrameHeader*) op_addPolyPoint_thread_spec},
{"endPoly", (VFrameHeader*) op_endPoly_thread_spec},
{"addRect", (VFrameHeader*) op_addRect_thread_spec},
{"addLine", (VFrameHeader*) op_addLine_thread_spec},
{"addPoint", (VFrameHeader*) op_addPoint_thread_spec},
{"addBitmap", (VFrameHeader*) op_addBitmap_thread_spec},
{"addText", (VFrameHeader*) op_addText_thread_spec},
{"addTextWith", (VFrameHeader*) op_addTextWith_thread_spec},
{"pushSurface", (VFrameHeader*) op_pushSurface_thread_spec},
{"popSurface", (VFrameHeader*) op_popSurface_thread_spec},
{"setSize", (VFrameHeader*) op_setSize_thread_spec},
{"setPosition", (VFrameHeader*) op_setPosition_thread_spec},
{"setVisible", (VFrameHeader*) op_setVisible_thread_spec},
{"setResizable", (VFrameHeader*) op_setResizable_thread_spec},
{"setFullScreen", (VFrameHeader*) op_setFullScreen_thread_spec},
{"setTitle", (VFrameHeader*) op_setTitle_thread_spec},
{"setIcon", (VFrameHeader*) op_setIcon_thread_spec},
{"commitBuffer", (VFrameHeader*) op_commitBuffer_thread_spec},
{"getResolution", (VFrameHeader*) op_getResolution_thread_spec},
{"setBackgroundColor", (VFrameHeader*) op_setBackgroundColor_thread_spec},
{"maximiseWindow", (VFrameHeader*) op_maximiseWindow_thread_spec},
{"minimiseWindow", (VFrameHeader*) op_minimiseWindow_thread_spec},
{"getMaximisedScreenRect", (VFrameHeader*) op_getMaximisedScreenRect_thread_spec},
{"closeWindow", (VFrameHeader*) op_closeWindow_thread_spec},
{"loadFont", (VFrameHeader*) op_loadFont_thread_spec},
{"getTextWidth", (VFrameHeader*) op_getTextWidth_thread_spec},
{"getFontMetrics", (VFrameHeader*) op_getFontMetrics_thread_spec},
{"getFontName", (VFrameHeader*) op_getFontName_thread_spec},
{"isFontFixedWidth", (VFrameHeader*) op_isFontFixedWidth_thread_spec},
{"getTextBitmapWith", (VFrameHeader*) op_getTextBitmapWith_thread_spec},
{"unloadFont", (VFrameHeader*) op_unloadFont_thread_spec}};
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
{"FontMetrics", &FontMetrics_def
},
{"Rect", &Rect_def
},
{"byte[]", &byte_array_def
},
{"WH", &WH_def
},
{"PixelMap", &PixelMap_def
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

