#include "dana_lib_defs.h"
#include <string.h>
#include <stdio.h>
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
static const DanaTypeField Point_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0+sizeof(size_t)}};
static const DanaType Point_def = 
{TYPE_DATA, 0, 16, (DanaTypeField*) Point_fields, 2};
static const DanaTypeField Point_array_fields[] = {
{(DanaType*) &Point_def, NULL, 0, 0, 0}};
static const DanaType Point_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) Point_array_fields, 1};
static const DanaTypeField WH_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0+sizeof(size_t)}};
static const DanaType WH_def = 
{TYPE_DATA, 0, 16, (DanaTypeField*) WH_fields, 2};
static const DanaTypeField byte_array_fields[] = {
{(DanaType*) &byte_def, NULL, 0, 0, 0}};
static const DanaType byte_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) byte_array_fields, 1};
static const DanaTypeField PixelMap_fields[] = {
{(DanaType*) &WH_def, NULL, 0, 0, 0},
{(DanaType*) &byte_array_def, NULL, 0, 0, 0+(sizeof(size_t)*2)}};
static const DanaType PixelMap_def = 
{TYPE_DATA, 0, 32, (DanaTypeField*) PixelMap_fields, 2};
static const DanaTypeField Rect_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0+sizeof(size_t)},
{(DanaType*) &int_def, NULL, 0, 0, 0+sizeof(size_t)+sizeof(size_t)},
{(DanaType*) &int_def, NULL, 0, 0, 0+sizeof(size_t)+sizeof(size_t)+sizeof(size_t)}};
static const DanaType Rect_def = 
{TYPE_DATA, 0, 32, (DanaTypeField*) Rect_fields, 4};
static const DanaTypeField Cursor_fields[] = {
{(DanaType*) &PixelMap_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0+(sizeof(size_t)*2)},
{(DanaType*) &int_def, NULL, 0, 0, 0+(sizeof(size_t)*2)+sizeof(size_t)}};
static const DanaType Cursor_def = 
{TYPE_DATA, 0, 32, (DanaTypeField*) Cursor_fields, 3};
static const DanaTypeField FontMetrics_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0+sizeof(size_t)},
{(DanaType*) &int_def, NULL, 0, 0, 0+sizeof(size_t)+sizeof(size_t)},
{(DanaType*) &int_def, NULL, 0, 0, 0+sizeof(size_t)+sizeof(size_t)+sizeof(size_t)}};
static const DanaType FontMetrics_def = 
{TYPE_DATA, 0, 32, (DanaTypeField*) FontMetrics_fields, 4};
static const DanaType int4_def = 
{TYPE_LITERAL, 0, 4, NULL, 0};
static const DanaTypeField FlowEvent_fields[] = {
{(DanaType*) &int4_def, NULL, 0, 0, 0},
{(DanaType*) &int4_def, NULL, 0, 0, 0+4},
{(DanaType*) &int4_def, NULL, 0, 0, 0+4+4},
{(DanaType*) &int4_def, NULL, 0, 0, 0+4+4+4},
{(DanaType*) &int4_def, NULL, 0, 0, 0+4+4+4+4},
{(DanaType*) &int4_def, NULL, 0, 0, 0+4+4+4+4+4}};
static const DanaType FlowEvent_def = 
{TYPE_DATA, 0, 24, (DanaTypeField*) FlowEvent_fields, 6};
static const DanaTypeField FlowEvent_array_fields[] = {
{(DanaType*) &FlowEvent_def, NULL, 0, 0, 0}};
static const DanaType FlowEvent_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) FlowEvent_array_fields, 1};
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
static const DanaTypeField function_UIPlaneLib_addPoint_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &byte_def, NULL, 0, 0, 24},
{(DanaType*) &byte_def, NULL, 0, 0, 25},
{(DanaType*) &byte_def, NULL, 0, 0, 26},
{(DanaType*) &byte_def, NULL, 0, 0, 27}};
static const DanaTypeField function_UIPlaneLib_addLine_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 48},
{(DanaType*) &byte_def, NULL, 0, 0, 49},
{(DanaType*) &byte_def, NULL, 0, 0, 50},
{(DanaType*) &byte_def, NULL, 0, 0, 51},
{(DanaType*) &bool_def, NULL, 0, 0, 52}};
static const DanaTypeField function_UIPlaneLib_addCurve_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &Point_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 41},
{(DanaType*) &byte_def, NULL, 0, 0, 42},
{(DanaType*) &byte_def, NULL, 0, 0, 43},
{(DanaType*) &bool_def, NULL, 0, 0, 44}};
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
static const DanaTypeField function_UIPlaneLib_addEllipse_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 41},
{(DanaType*) &byte_def, NULL, 0, 0, 42},
{(DanaType*) &byte_def, NULL, 0, 0, 43}};
static const DanaTypeField function_UIPlaneLib_addEllipseOutline_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 48},
{(DanaType*) &byte_def, NULL, 0, 0, 49},
{(DanaType*) &byte_def, NULL, 0, 0, 50},
{(DanaType*) &byte_def, NULL, 0, 0, 51}};
static const DanaTypeField function_UIPlaneLib_addArc_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &int_def, NULL, 0, 0, 48},
{(DanaType*) &int_def, NULL, 0, 0, 56},
{(DanaType*) &byte_def, NULL, 0, 0, 64},
{(DanaType*) &byte_def, NULL, 0, 0, 65},
{(DanaType*) &byte_def, NULL, 0, 0, 66},
{(DanaType*) &byte_def, NULL, 0, 0, 67},
{(DanaType*) &bool_def, NULL, 0, 0, 68}};
static const DanaTypeField function_UIPlaneLib_addPie_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &int_def, NULL, 0, 0, 48},
{(DanaType*) &bool_def, NULL, 0, 0, 56},
{(DanaType*) &byte_def, NULL, 0, 0, 57},
{(DanaType*) &byte_def, NULL, 0, 0, 58},
{(DanaType*) &byte_def, NULL, 0, 0, 59},
{(DanaType*) &byte_def, NULL, 0, 0, 60}};
static const DanaTypeField function_UIPlaneLib_addPolygon_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &Point_array_def, NULL, 0, 0, 8},
{(DanaType*) &byte_def, NULL, 0, 0, 24},
{(DanaType*) &byte_def, NULL, 0, 0, 25},
{(DanaType*) &byte_def, NULL, 0, 0, 26},
{(DanaType*) &byte_def, NULL, 0, 0, 27}};
static const DanaTypeField function_UIPlaneLib_addPolygonOutline_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &Point_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &byte_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 33},
{(DanaType*) &byte_def, NULL, 0, 0, 34},
{(DanaType*) &byte_def, NULL, 0, 0, 35}};
static const DanaTypeField function_UIPlaneLib_addPolygonBezier_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &Point_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &byte_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 33},
{(DanaType*) &byte_def, NULL, 0, 0, 34},
{(DanaType*) &byte_def, NULL, 0, 0, 35}};
static const DanaTypeField function_UIPlaneLib_addPolygonBezierOutline_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &Point_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 41},
{(DanaType*) &byte_def, NULL, 0, 0, 42},
{(DanaType*) &byte_def, NULL, 0, 0, 43}};
static const DanaTypeField function_UIPlaneLib_addBitmap_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &PixelMap_def, NULL, 0, 0, 8},
{(DanaType*) &Rect_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &int_def, NULL, 0, 0, 48},
{(DanaType*) &int_def, NULL, 0, 0, 56},
{(DanaType*) &int_def, NULL, 0, 0, 64},
{(DanaType*) &int_def, NULL, 0, 0, 72}};
static const DanaTypeField function_UIPlaneLib_addTextWith_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &char_array_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 56},
{(DanaType*) &byte_def, NULL, 0, 0, 57},
{(DanaType*) &byte_def, NULL, 0, 0, 58},
{(DanaType*) &byte_def, NULL, 0, 0, 59}};
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
static const DanaTypeField function_UIPlaneLib_setCursor_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &byte_def, NULL, 0, 0, 8},
{(DanaType*) &Cursor_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_commitBuffer_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_getPixels_fields[] = {
{(DanaType*) &PixelMap_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16}};
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
static const DanaTypeField function_UIPlaneLib_initMediaLayer_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_runSystemLoop_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_shutdown_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_loadFont_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 24}};
static const DanaTypeField function_UIPlaneLib_getTextWidth_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &char_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_getFontMetrics_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &FontMetrics_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_getFontName_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_isFontFixedWidth_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1}};
static const DanaTypeField function_UIPlaneLib_getTextBitmapWith_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &PixelMap_def, NULL, 0, 0, 24},
{(DanaType*) &byte_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 41},
{(DanaType*) &byte_def, NULL, 0, 0, 42},
{(DanaType*) &byte_def, NULL, 0, 0, 43}};
static const DanaTypeField function_UIPlaneLib_unloadFont_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_flow_initMediaLayer_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_flow_makeWindow_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &bool_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_flow_setVisible_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &bool_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_flow_setFullScreen_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &bool_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_flow_setResizable_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &bool_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_flow_setPosition_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_flow_setSize_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_flow_setTitle_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &char_array_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_flow_setIcon_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &PixelMap_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_flow_setCursor_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &byte_def, NULL, 0, 0, 8},
{(DanaType*) &Cursor_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_flow_closeWindow_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_flow_getEvents_fields[] = {
{(DanaType*) &FlowEvent_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_flow_renderBegin_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_flow_renderEnd_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_flow_wait_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_flow_getPixels_fields[] = {
{(DanaType*) &PixelMap_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_flow_point_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &byte_def, NULL, 0, 0, 24},
{(DanaType*) &byte_def, NULL, 0, 0, 25},
{(DanaType*) &byte_def, NULL, 0, 0, 26},
{(DanaType*) &byte_def, NULL, 0, 0, 27}};
static const DanaTypeField function_UIPlaneLib_flow_line_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 48},
{(DanaType*) &byte_def, NULL, 0, 0, 49},
{(DanaType*) &byte_def, NULL, 0, 0, 50},
{(DanaType*) &byte_def, NULL, 0, 0, 51},
{(DanaType*) &bool_def, NULL, 0, 0, 52}};
static const DanaTypeField function_UIPlaneLib_flow_curve_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &Point_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 41},
{(DanaType*) &byte_def, NULL, 0, 0, 42},
{(DanaType*) &byte_def, NULL, 0, 0, 43},
{(DanaType*) &bool_def, NULL, 0, 0, 44}};
static const DanaTypeField function_UIPlaneLib_flow_rect_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 41},
{(DanaType*) &byte_def, NULL, 0, 0, 42},
{(DanaType*) &byte_def, NULL, 0, 0, 43}};
static const DanaTypeField function_UIPlaneLib_flow_ellipse_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 41},
{(DanaType*) &byte_def, NULL, 0, 0, 42},
{(DanaType*) &byte_def, NULL, 0, 0, 43}};
static const DanaTypeField function_UIPlaneLib_flow_ellipseOutline_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 48},
{(DanaType*) &byte_def, NULL, 0, 0, 49},
{(DanaType*) &byte_def, NULL, 0, 0, 50},
{(DanaType*) &byte_def, NULL, 0, 0, 51}};
static const DanaTypeField function_UIPlaneLib_flow_arc_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &int_def, NULL, 0, 0, 48},
{(DanaType*) &int_def, NULL, 0, 0, 56},
{(DanaType*) &byte_def, NULL, 0, 0, 64},
{(DanaType*) &byte_def, NULL, 0, 0, 65},
{(DanaType*) &byte_def, NULL, 0, 0, 66},
{(DanaType*) &byte_def, NULL, 0, 0, 67},
{(DanaType*) &bool_def, NULL, 0, 0, 68}};
static const DanaTypeField function_UIPlaneLib_flow_pie_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &int_def, NULL, 0, 0, 48},
{(DanaType*) &bool_def, NULL, 0, 0, 56},
{(DanaType*) &byte_def, NULL, 0, 0, 57},
{(DanaType*) &byte_def, NULL, 0, 0, 58},
{(DanaType*) &byte_def, NULL, 0, 0, 59},
{(DanaType*) &byte_def, NULL, 0, 0, 60}};
static const DanaTypeField function_UIPlaneLib_flow_polygon_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &Point_array_def, NULL, 0, 0, 8},
{(DanaType*) &byte_def, NULL, 0, 0, 24},
{(DanaType*) &byte_def, NULL, 0, 0, 25},
{(DanaType*) &byte_def, NULL, 0, 0, 26},
{(DanaType*) &byte_def, NULL, 0, 0, 27}};
static const DanaTypeField function_UIPlaneLib_flow_polygonOutline_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &Point_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &byte_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 33},
{(DanaType*) &byte_def, NULL, 0, 0, 34},
{(DanaType*) &byte_def, NULL, 0, 0, 35}};
static const DanaTypeField function_UIPlaneLib_flow_polygonBezier_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &Point_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &byte_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 33},
{(DanaType*) &byte_def, NULL, 0, 0, 34},
{(DanaType*) &byte_def, NULL, 0, 0, 35}};
static const DanaTypeField function_UIPlaneLib_flow_polygonBezierOutline_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &Point_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &byte_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 41},
{(DanaType*) &byte_def, NULL, 0, 0, 42},
{(DanaType*) &byte_def, NULL, 0, 0, 43}};
static const DanaTypeField function_UIPlaneLib_flow_bitmap_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &PixelMap_def, NULL, 0, 0, 8},
{(DanaType*) &Rect_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &int_def, NULL, 0, 0, 48},
{(DanaType*) &int_def, NULL, 0, 0, 56},
{(DanaType*) &int_def, NULL, 0, 0, 64},
{(DanaType*) &int_def, NULL, 0, 0, 72}};
static const DanaTypeField function_UIPlaneLib_flow_textWith_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &char_array_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 56},
{(DanaType*) &byte_def, NULL, 0, 0, 57},
{(DanaType*) &byte_def, NULL, 0, 0, 58},
{(DanaType*) &byte_def, NULL, 0, 0, 59}};
static const DanaTypeField function_UIPlaneLib_flow_pushSurface_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 24},
{(DanaType*) &int_def, NULL, 0, 0, 32},
{(DanaType*) &int_def, NULL, 0, 0, 40},
{(DanaType*) &int_def, NULL, 0, 0, 48},
{(DanaType*) &byte_def, NULL, 0, 0, 56}};
static const DanaTypeField function_UIPlaneLib_flow_popSurface_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_UIPlaneLib_flow_loadFont_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 24}};
static const DanaTypeField function_UIPlaneLib_flow_getTextWidth_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &char_array_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_flow_getFontMetrics_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &FontMetrics_def, NULL, 0, 0, 8}};
static const DanaTypeField function_UIPlaneLib_flow_getFontName_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_UIPlaneLib_flow_isFontFixedWidth_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1}};
static const DanaTypeField function_UIPlaneLib_flow_getTextBitmapWith_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &char_array_def, NULL, 0, 0, 8},
{(DanaType*) &PixelMap_def, NULL, 0, 0, 24},
{(DanaType*) &byte_def, NULL, 0, 0, 40},
{(DanaType*) &byte_def, NULL, 0, 0, 41},
{(DanaType*) &byte_def, NULL, 0, 0, 42},
{(DanaType*) &byte_def, NULL, 0, 0, 43}};
static const DanaTypeField function_UIPlaneLib_flow_unloadFont_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField WindowEventData_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0+sizeof(size_t)},
{(DanaType*) &int_def, NULL, 0, 0, 0+sizeof(size_t)+sizeof(size_t)},
{(DanaType*) &int_def, NULL, 0, 0, 0+sizeof(size_t)+sizeof(size_t)+sizeof(size_t)},
{(DanaType*) &int_def, NULL, 0, 0, 0+sizeof(size_t)+sizeof(size_t)+sizeof(size_t)+sizeof(size_t)}};
static const DanaType WindowEventData_def = 
{TYPE_DATA, 0, 40, (DanaTypeField*) WindowEventData_fields, 5};
static const DanaTypeField DropEventData_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0+sizeof(size_t)},
{(DanaType*) &char_array_def, NULL, 0, 0, 0+sizeof(size_t)+sizeof(size_t)}};
static const DanaType DropEventData_def = 
{TYPE_DATA, 0, 32, (DanaTypeField*) DropEventData_fields, 3};
static const DanaTypeField event_UIPlaneLib_ready_fields[] = {
};
static const DanaTypeField event_UIPlaneLib_mouseUp_fields[] = {
{(DanaType*) &WindowEventData_def, NULL, 0, 0, 0}};
static const DanaTypeField event_UIPlaneLib_mouseDown_fields[] = {
{(DanaType*) &WindowEventData_def, NULL, 0, 0, 0}};
static const DanaTypeField event_UIPlaneLib_mouseMove_fields[] = {
{(DanaType*) &WindowEventData_def, NULL, 0, 0, 0}};
static const DanaTypeField event_UIPlaneLib_mouseWheel_fields[] = {
{(DanaType*) &WindowEventData_def, NULL, 0, 0, 0}};
static const DanaTypeField event_UIPlaneLib_keyDown_fields[] = {
{(DanaType*) &WindowEventData_def, NULL, 0, 0, 0}};
static const DanaTypeField event_UIPlaneLib_keyUp_fields[] = {
{(DanaType*) &WindowEventData_def, NULL, 0, 0, 0}};
static const DanaTypeField event_UIPlaneLib_resize_fields[] = {
{(DanaType*) &WindowEventData_def, NULL, 0, 0, 0}};
static const DanaTypeField event_UIPlaneLib_move_fields[] = {
{(DanaType*) &WindowEventData_def, NULL, 0, 0, 0}};
static const DanaTypeField event_UIPlaneLib_drop_fields[] = {
{(DanaType*) &DropEventData_def, NULL, 0, 0, 0}};
static const DanaTypeField event_UIPlaneLib_close_fields[] = {
};
static const DanaTypeField event_UIPlaneLib_post_shutdown_fields[] = {
};
static const DanaType object_UIPlaneLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_UIPlaneLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_UIPlaneLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_makeWindow_fields, 1},
{TYPE_FUNCTION, 0, 28, (DanaTypeField*) &function_UIPlaneLib_addPoint_fields, 8},
{TYPE_FUNCTION, 0, 53, (DanaTypeField*) &function_UIPlaneLib_addLine_fields, 12},
{TYPE_FUNCTION, 0, 45, (DanaTypeField*) &function_UIPlaneLib_addCurve_fields, 10},
{TYPE_FUNCTION, 0, 44, (DanaTypeField*) &function_UIPlaneLib_addRect_fields, 10},
{TYPE_FUNCTION, 0, 44, (DanaTypeField*) &function_UIPlaneLib_addEllipse_fields, 10},
{TYPE_FUNCTION, 0, 52, (DanaTypeField*) &function_UIPlaneLib_addEllipseOutline_fields, 11},
{TYPE_FUNCTION, 0, 69, (DanaTypeField*) &function_UIPlaneLib_addArc_fields, 14},
{TYPE_FUNCTION, 0, 61, (DanaTypeField*) &function_UIPlaneLib_addPie_fields, 13},
{TYPE_FUNCTION, 0, 28, (DanaTypeField*) &function_UIPlaneLib_addPolygon_fields, 7},
{TYPE_FUNCTION, 0, 36, (DanaTypeField*) &function_UIPlaneLib_addPolygonOutline_fields, 8},
{TYPE_FUNCTION, 0, 36, (DanaTypeField*) &function_UIPlaneLib_addPolygonBezier_fields, 8},
{TYPE_FUNCTION, 0, 44, (DanaTypeField*) &function_UIPlaneLib_addPolygonBezierOutline_fields, 9},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_UIPlaneLib_addBitmap_fields, 9},
{TYPE_FUNCTION, 0, 60, (DanaTypeField*) &function_UIPlaneLib_addTextWith_fields, 11},
{TYPE_FUNCTION, 0, 57, (DanaTypeField*) &function_UIPlaneLib_pushSurface_fields, 9},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_popSurface_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_setSize_fields, 4},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_setPosition_fields, 4},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_UIPlaneLib_setVisible_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_UIPlaneLib_setResizable_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_UIPlaneLib_setFullScreen_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_setTitle_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_setIcon_fields, 3},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_UIPlaneLib_setCursor_fields, 4},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_commitBuffer_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_getPixels_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_getResolution_fields, 3},
{TYPE_FUNCTION, 0, 12, (DanaTypeField*) &function_UIPlaneLib_setBackgroundColor_fields, 6},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_maximiseWindow_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_minimiseWindow_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_getMaximisedScreenRect_fields, 3},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_closeWindow_fields, 2},
{TYPE_FUNCTION, 0, 1, (DanaTypeField*) &function_UIPlaneLib_initMediaLayer_fields, 1},
{TYPE_FUNCTION, 0, 0, (DanaTypeField*) &function_UIPlaneLib_runSystemLoop_fields, 1},
{TYPE_FUNCTION, 0, 0, (DanaTypeField*) &function_UIPlaneLib_shutdown_fields, 1},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_UIPlaneLib_loadFont_fields, 3},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_UIPlaneLib_getTextWidth_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_getFontMetrics_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_getFontName_fields, 2},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_UIPlaneLib_isFontFixedWidth_fields, 2},
{TYPE_FUNCTION, 0, 44, (DanaTypeField*) &function_UIPlaneLib_getTextBitmapWith_fields, 8},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_unloadFont_fields, 2},
{TYPE_FUNCTION, 0, 1, (DanaTypeField*) &function_UIPlaneLib_flow_initMediaLayer_fields, 1},
{TYPE_FUNCTION, 0, 17, (DanaTypeField*) &function_UIPlaneLib_flow_makeWindow_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_UIPlaneLib_flow_setVisible_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_UIPlaneLib_flow_setFullScreen_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_UIPlaneLib_flow_setResizable_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_flow_setPosition_fields, 4},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_flow_setSize_fields, 4},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_flow_setTitle_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_flow_setIcon_fields, 3},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_UIPlaneLib_flow_setCursor_fields, 4},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_flow_closeWindow_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_flow_getEvents_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_flow_renderBegin_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_flow_renderEnd_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_flow_wait_fields, 2},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_flow_getPixels_fields, 2},
{TYPE_FUNCTION, 0, 28, (DanaTypeField*) &function_UIPlaneLib_flow_point_fields, 8},
{TYPE_FUNCTION, 0, 53, (DanaTypeField*) &function_UIPlaneLib_flow_line_fields, 12},
{TYPE_FUNCTION, 0, 45, (DanaTypeField*) &function_UIPlaneLib_flow_curve_fields, 10},
{TYPE_FUNCTION, 0, 44, (DanaTypeField*) &function_UIPlaneLib_flow_rect_fields, 10},
{TYPE_FUNCTION, 0, 44, (DanaTypeField*) &function_UIPlaneLib_flow_ellipse_fields, 10},
{TYPE_FUNCTION, 0, 52, (DanaTypeField*) &function_UIPlaneLib_flow_ellipseOutline_fields, 11},
{TYPE_FUNCTION, 0, 69, (DanaTypeField*) &function_UIPlaneLib_flow_arc_fields, 14},
{TYPE_FUNCTION, 0, 61, (DanaTypeField*) &function_UIPlaneLib_flow_pie_fields, 13},
{TYPE_FUNCTION, 0, 28, (DanaTypeField*) &function_UIPlaneLib_flow_polygon_fields, 7},
{TYPE_FUNCTION, 0, 36, (DanaTypeField*) &function_UIPlaneLib_flow_polygonOutline_fields, 8},
{TYPE_FUNCTION, 0, 36, (DanaTypeField*) &function_UIPlaneLib_flow_polygonBezier_fields, 8},
{TYPE_FUNCTION, 0, 44, (DanaTypeField*) &function_UIPlaneLib_flow_polygonBezierOutline_fields, 9},
{TYPE_FUNCTION, 0, 80, (DanaTypeField*) &function_UIPlaneLib_flow_bitmap_fields, 9},
{TYPE_FUNCTION, 0, 60, (DanaTypeField*) &function_UIPlaneLib_flow_textWith_fields, 11},
{TYPE_FUNCTION, 0, 57, (DanaTypeField*) &function_UIPlaneLib_flow_pushSurface_fields, 9},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_flow_popSurface_fields, 2},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_UIPlaneLib_flow_loadFont_fields, 3},
{TYPE_FUNCTION, 0, 32, (DanaTypeField*) &function_UIPlaneLib_flow_getTextWidth_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_flow_getFontMetrics_fields, 3},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_UIPlaneLib_flow_getFontName_fields, 2},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_UIPlaneLib_flow_isFontFixedWidth_fields, 2},
{TYPE_FUNCTION, 0, 44, (DanaTypeField*) &function_UIPlaneLib_flow_getTextBitmapWith_fields, 8},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_UIPlaneLib_flow_unloadFont_fields, 2}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_UIPlaneLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_UIPlaneLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_UIPlaneLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_UIPlaneLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_UIPlaneLib_functions_spec[4], "makeWindow", 10},
{(DanaType*) &object_UIPlaneLib_functions_spec[5], "addPoint", 8},
{(DanaType*) &object_UIPlaneLib_functions_spec[6], "addLine", 7},
{(DanaType*) &object_UIPlaneLib_functions_spec[7], "addCurve", 8},
{(DanaType*) &object_UIPlaneLib_functions_spec[8], "addRect", 7},
{(DanaType*) &object_UIPlaneLib_functions_spec[9], "addEllipse", 10},
{(DanaType*) &object_UIPlaneLib_functions_spec[10], "addEllipseOutline", 17},
{(DanaType*) &object_UIPlaneLib_functions_spec[11], "addArc", 6},
{(DanaType*) &object_UIPlaneLib_functions_spec[12], "addPie", 6},
{(DanaType*) &object_UIPlaneLib_functions_spec[13], "addPolygon", 10},
{(DanaType*) &object_UIPlaneLib_functions_spec[14], "addPolygonOutline", 17},
{(DanaType*) &object_UIPlaneLib_functions_spec[15], "addPolygonBezier", 16},
{(DanaType*) &object_UIPlaneLib_functions_spec[16], "addPolygonBezierOutline", 23},
{(DanaType*) &object_UIPlaneLib_functions_spec[17], "addBitmap", 9},
{(DanaType*) &object_UIPlaneLib_functions_spec[18], "addTextWith", 11},
{(DanaType*) &object_UIPlaneLib_functions_spec[19], "pushSurface", 11},
{(DanaType*) &object_UIPlaneLib_functions_spec[20], "popSurface", 10},
{(DanaType*) &object_UIPlaneLib_functions_spec[21], "setSize", 7},
{(DanaType*) &object_UIPlaneLib_functions_spec[22], "setPosition", 11},
{(DanaType*) &object_UIPlaneLib_functions_spec[23], "setVisible", 10},
{(DanaType*) &object_UIPlaneLib_functions_spec[24], "setResizable", 12},
{(DanaType*) &object_UIPlaneLib_functions_spec[25], "setFullScreen", 13},
{(DanaType*) &object_UIPlaneLib_functions_spec[26], "setTitle", 8},
{(DanaType*) &object_UIPlaneLib_functions_spec[27], "setIcon", 7},
{(DanaType*) &object_UIPlaneLib_functions_spec[28], "setCursor", 9},
{(DanaType*) &object_UIPlaneLib_functions_spec[29], "commitBuffer", 12},
{(DanaType*) &object_UIPlaneLib_functions_spec[30], "getPixels", 9},
{(DanaType*) &object_UIPlaneLib_functions_spec[31], "getResolution", 13},
{(DanaType*) &object_UIPlaneLib_functions_spec[32], "setBackgroundColor", 18},
{(DanaType*) &object_UIPlaneLib_functions_spec[33], "maximiseWindow", 14},
{(DanaType*) &object_UIPlaneLib_functions_spec[34], "minimiseWindow", 14},
{(DanaType*) &object_UIPlaneLib_functions_spec[35], "getMaximisedScreenRect", 22},
{(DanaType*) &object_UIPlaneLib_functions_spec[36], "closeWindow", 11},
{(DanaType*) &object_UIPlaneLib_functions_spec[37], "initMediaLayer", 14},
{(DanaType*) &object_UIPlaneLib_functions_spec[38], "runSystemLoop", 13},
{(DanaType*) &object_UIPlaneLib_functions_spec[39], "shutdown", 8},
{(DanaType*) &object_UIPlaneLib_functions_spec[40], "loadFont", 8},
{(DanaType*) &object_UIPlaneLib_functions_spec[41], "getTextWidth", 12},
{(DanaType*) &object_UIPlaneLib_functions_spec[42], "getFontMetrics", 14},
{(DanaType*) &object_UIPlaneLib_functions_spec[43], "getFontName", 11},
{(DanaType*) &object_UIPlaneLib_functions_spec[44], "isFontFixedWidth", 16},
{(DanaType*) &object_UIPlaneLib_functions_spec[45], "getTextBitmapWith", 17},
{(DanaType*) &object_UIPlaneLib_functions_spec[46], "unloadFont", 10},
{(DanaType*) &object_UIPlaneLib_functions_spec[47], "flow_initMediaLayer", 19},
{(DanaType*) &object_UIPlaneLib_functions_spec[48], "flow_makeWindow", 15},
{(DanaType*) &object_UIPlaneLib_functions_spec[49], "flow_setVisible", 15},
{(DanaType*) &object_UIPlaneLib_functions_spec[50], "flow_setFullScreen", 18},
{(DanaType*) &object_UIPlaneLib_functions_spec[51], "flow_setResizable", 17},
{(DanaType*) &object_UIPlaneLib_functions_spec[52], "flow_setPosition", 16},
{(DanaType*) &object_UIPlaneLib_functions_spec[53], "flow_setSize", 12},
{(DanaType*) &object_UIPlaneLib_functions_spec[54], "flow_setTitle", 13},
{(DanaType*) &object_UIPlaneLib_functions_spec[55], "flow_setIcon", 12},
{(DanaType*) &object_UIPlaneLib_functions_spec[56], "flow_setCursor", 14},
{(DanaType*) &object_UIPlaneLib_functions_spec[57], "flow_closeWindow", 16},
{(DanaType*) &object_UIPlaneLib_functions_spec[58], "flow_getEvents", 14},
{(DanaType*) &object_UIPlaneLib_functions_spec[59], "flow_renderBegin", 16},
{(DanaType*) &object_UIPlaneLib_functions_spec[60], "flow_renderEnd", 14},
{(DanaType*) &object_UIPlaneLib_functions_spec[61], "flow_wait", 9},
{(DanaType*) &object_UIPlaneLib_functions_spec[62], "flow_getPixels", 14},
{(DanaType*) &object_UIPlaneLib_functions_spec[63], "flow_point", 10},
{(DanaType*) &object_UIPlaneLib_functions_spec[64], "flow_line", 9},
{(DanaType*) &object_UIPlaneLib_functions_spec[65], "flow_curve", 10},
{(DanaType*) &object_UIPlaneLib_functions_spec[66], "flow_rect", 9},
{(DanaType*) &object_UIPlaneLib_functions_spec[67], "flow_ellipse", 12},
{(DanaType*) &object_UIPlaneLib_functions_spec[68], "flow_ellipseOutline", 19},
{(DanaType*) &object_UIPlaneLib_functions_spec[69], "flow_arc", 8},
{(DanaType*) &object_UIPlaneLib_functions_spec[70], "flow_pie", 8},
{(DanaType*) &object_UIPlaneLib_functions_spec[71], "flow_polygon", 12},
{(DanaType*) &object_UIPlaneLib_functions_spec[72], "flow_polygonOutline", 19},
{(DanaType*) &object_UIPlaneLib_functions_spec[73], "flow_polygonBezier", 18},
{(DanaType*) &object_UIPlaneLib_functions_spec[74], "flow_polygonBezierOutline", 25},
{(DanaType*) &object_UIPlaneLib_functions_spec[75], "flow_bitmap", 11},
{(DanaType*) &object_UIPlaneLib_functions_spec[76], "flow_textWith", 13},
{(DanaType*) &object_UIPlaneLib_functions_spec[77], "flow_pushSurface", 16},
{(DanaType*) &object_UIPlaneLib_functions_spec[78], "flow_popSurface", 15},
{(DanaType*) &object_UIPlaneLib_functions_spec[79], "flow_loadFont", 13},
{(DanaType*) &object_UIPlaneLib_functions_spec[80], "flow_getTextWidth", 17},
{(DanaType*) &object_UIPlaneLib_functions_spec[81], "flow_getFontMetrics", 19},
{(DanaType*) &object_UIPlaneLib_functions_spec[82], "flow_getFontName", 16},
{(DanaType*) &object_UIPlaneLib_functions_spec[83], "flow_isFontFixedWidth", 21},
{(DanaType*) &object_UIPlaneLib_functions_spec[84], "flow_getTextBitmapWith", 22},
{(DanaType*) &object_UIPlaneLib_functions_spec[85], "flow_unloadFont", 15}};
static const DanaType object_UIPlaneLib_events_spec[] = {
{TYPE_EVENTSOURCE, 0, 0, (DanaTypeField*) &event_UIPlaneLib_ready_fields, 0},
{TYPE_EVENTSOURCE, 0, 0, (DanaTypeField*) &event_UIPlaneLib_mouseUp_fields, 1},
{TYPE_EVENTSOURCE, 0, 0, (DanaTypeField*) &event_UIPlaneLib_mouseDown_fields, 1},
{TYPE_EVENTSOURCE, 0, 0, (DanaTypeField*) &event_UIPlaneLib_mouseMove_fields, 1},
{TYPE_EVENTSOURCE, 0, 0, (DanaTypeField*) &event_UIPlaneLib_mouseWheel_fields, 1},
{TYPE_EVENTSOURCE, 0, 0, (DanaTypeField*) &event_UIPlaneLib_keyDown_fields, 1},
{TYPE_EVENTSOURCE, 0, 0, (DanaTypeField*) &event_UIPlaneLib_keyUp_fields, 1},
{TYPE_EVENTSOURCE, 0, 0, (DanaTypeField*) &event_UIPlaneLib_resize_fields, 1},
{TYPE_EVENTSOURCE, 0, 0, (DanaTypeField*) &event_UIPlaneLib_move_fields, 1},
{TYPE_EVENTSOURCE, 0, 0, (DanaTypeField*) &event_UIPlaneLib_drop_fields, 1},
{TYPE_EVENTSOURCE, 0, 0, (DanaTypeField*) &event_UIPlaneLib_close_fields, 0},
{TYPE_EVENTSOURCE, 0, 0, (DanaTypeField*) &event_UIPlaneLib_post_shutdown_fields, 0}};
static const DanaTypeField intf_events_def[] = {
{(DanaType*) &object_UIPlaneLib_events_spec[0], "ready", 5},
{(DanaType*) &object_UIPlaneLib_events_spec[1], "mouseUp", 7},
{(DanaType*) &object_UIPlaneLib_events_spec[2], "mouseDown", 9},
{(DanaType*) &object_UIPlaneLib_events_spec[3], "mouseMove", 9},
{(DanaType*) &object_UIPlaneLib_events_spec[4], "mouseWheel", 10},
{(DanaType*) &object_UIPlaneLib_events_spec[5], "keyDown", 7},
{(DanaType*) &object_UIPlaneLib_events_spec[6], "keyUp", 5},
{(DanaType*) &object_UIPlaneLib_events_spec[7], "resize", 6},
{(DanaType*) &object_UIPlaneLib_events_spec[8], "move", 4},
{(DanaType*) &object_UIPlaneLib_events_spec[9], "drop", 4},
{(DanaType*) &object_UIPlaneLib_events_spec[10], "close", 5},
{(DanaType*) &object_UIPlaneLib_events_spec[11], "post_shutdown", 13}};
static const DanaType UIPlaneLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 86},
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_events_def, 12},
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
static unsigned char op_addPoint_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addLine_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addCurve_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addRect_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addEllipse_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addEllipseOutline_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addArc_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addPie_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addPolygon_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addPolygonOutline_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addPolygonBezier_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addPolygonBezierOutline_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_addBitmap_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
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
static unsigned char op_setCursor_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_commitBuffer_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getPixels_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getResolution_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_setBackgroundColor_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_maximiseWindow_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_minimiseWindow_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getMaximisedScreenRect_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_closeWindow_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_initMediaLayer_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_runSystemLoop_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_shutdown_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_loadFont_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getTextWidth_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getFontMetrics_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getFontName_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_isFontFixedWidth_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getTextBitmapWith_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_unloadFont_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_initMediaLayer_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_makeWindow_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_setVisible_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_setFullScreen_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_setResizable_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_setPosition_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_setSize_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_setTitle_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_setIcon_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_setCursor_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_closeWindow_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_getEvents_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_renderBegin_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_renderEnd_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_wait_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_getPixels_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_point_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_line_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_curve_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_rect_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_ellipse_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_ellipseOutline_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_arc_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_pie_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_polygon_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_polygonOutline_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_polygonBezier_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_polygonBezierOutline_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_bitmap_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_textWith_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_pushSurface_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_popSurface_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_loadFont_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_getTextWidth_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_getFontMetrics_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_getFontName_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_isFontFixedWidth_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_getTextBitmapWith_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_flow_unloadFont_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_makeWindow_thread_spec,
(size_t) op_addPoint_thread_spec,
(size_t) op_addLine_thread_spec,
(size_t) op_addCurve_thread_spec,
(size_t) op_addRect_thread_spec,
(size_t) op_addEllipse_thread_spec,
(size_t) op_addEllipseOutline_thread_spec,
(size_t) op_addArc_thread_spec,
(size_t) op_addPie_thread_spec,
(size_t) op_addPolygon_thread_spec,
(size_t) op_addPolygonOutline_thread_spec,
(size_t) op_addPolygonBezier_thread_spec,
(size_t) op_addPolygonBezierOutline_thread_spec,
(size_t) op_addBitmap_thread_spec,
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
(size_t) op_setCursor_thread_spec,
(size_t) op_commitBuffer_thread_spec,
(size_t) op_getPixels_thread_spec,
(size_t) op_getResolution_thread_spec,
(size_t) op_setBackgroundColor_thread_spec,
(size_t) op_maximiseWindow_thread_spec,
(size_t) op_minimiseWindow_thread_spec,
(size_t) op_getMaximisedScreenRect_thread_spec,
(size_t) op_closeWindow_thread_spec,
(size_t) op_initMediaLayer_thread_spec,
(size_t) op_runSystemLoop_thread_spec,
(size_t) op_shutdown_thread_spec,
(size_t) op_loadFont_thread_spec,
(size_t) op_getTextWidth_thread_spec,
(size_t) op_getFontMetrics_thread_spec,
(size_t) op_getFontName_thread_spec,
(size_t) op_isFontFixedWidth_thread_spec,
(size_t) op_getTextBitmapWith_thread_spec,
(size_t) op_unloadFont_thread_spec,
(size_t) op_flow_initMediaLayer_thread_spec,
(size_t) op_flow_makeWindow_thread_spec,
(size_t) op_flow_setVisible_thread_spec,
(size_t) op_flow_setFullScreen_thread_spec,
(size_t) op_flow_setResizable_thread_spec,
(size_t) op_flow_setPosition_thread_spec,
(size_t) op_flow_setSize_thread_spec,
(size_t) op_flow_setTitle_thread_spec,
(size_t) op_flow_setIcon_thread_spec,
(size_t) op_flow_setCursor_thread_spec,
(size_t) op_flow_closeWindow_thread_spec,
(size_t) op_flow_getEvents_thread_spec,
(size_t) op_flow_renderBegin_thread_spec,
(size_t) op_flow_renderEnd_thread_spec,
(size_t) op_flow_wait_thread_spec,
(size_t) op_flow_getPixels_thread_spec,
(size_t) op_flow_point_thread_spec,
(size_t) op_flow_line_thread_spec,
(size_t) op_flow_curve_thread_spec,
(size_t) op_flow_rect_thread_spec,
(size_t) op_flow_ellipse_thread_spec,
(size_t) op_flow_ellipseOutline_thread_spec,
(size_t) op_flow_arc_thread_spec,
(size_t) op_flow_pie_thread_spec,
(size_t) op_flow_polygon_thread_spec,
(size_t) op_flow_polygonOutline_thread_spec,
(size_t) op_flow_polygonBezier_thread_spec,
(size_t) op_flow_polygonBezierOutline_thread_spec,
(size_t) op_flow_bitmap_thread_spec,
(size_t) op_flow_textWith_thread_spec,
(size_t) op_flow_pushSurface_thread_spec,
(size_t) op_flow_popSurface_thread_spec,
(size_t) op_flow_loadFont_thread_spec,
(size_t) op_flow_getTextWidth_thread_spec,
(size_t) op_flow_getFontMetrics_thread_spec,
(size_t) op_flow_getFontName_thread_spec,
(size_t) op_flow_isFontFixedWidth_thread_spec,
(size_t) op_flow_getTextBitmapWith_thread_spec,
(size_t) op_flow_unloadFont_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"UIPlaneLib", 10, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 1, 0, (size_t) &bool_def, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> sub = NULL;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> sub = NULL;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> sub = NULL;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_makeWindow_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_makeWindow_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_makeWindow_thread_spec) -> sub = NULL;
((VFrameHeader*) op_makeWindow_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[4];
((VFrameHeader*) op_makeWindow_thread_spec) -> functionName = "makeWindow";
((VFrameHeader*) op_addPoint_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 28;
((VFrameHeader*) op_addPoint_thread_spec) -> formalParamsCount = 7;
((VFrameHeader*) op_addPoint_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addPoint_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[5];
((VFrameHeader*) op_addPoint_thread_spec) -> functionName = "addPoint";
((VFrameHeader*) op_addLine_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 53;
((VFrameHeader*) op_addLine_thread_spec) -> formalParamsCount = 11;
((VFrameHeader*) op_addLine_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addLine_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[6];
((VFrameHeader*) op_addLine_thread_spec) -> functionName = "addLine";
((VFrameHeader*) op_addCurve_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 45;
((VFrameHeader*) op_addCurve_thread_spec) -> formalParamsCount = 9;
((VFrameHeader*) op_addCurve_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addCurve_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[7];
((VFrameHeader*) op_addCurve_thread_spec) -> functionName = "addCurve";
((VFrameHeader*) op_addRect_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 44;
((VFrameHeader*) op_addRect_thread_spec) -> formalParamsCount = 9;
((VFrameHeader*) op_addRect_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addRect_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[8];
((VFrameHeader*) op_addRect_thread_spec) -> functionName = "addRect";
((VFrameHeader*) op_addEllipse_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 44;
((VFrameHeader*) op_addEllipse_thread_spec) -> formalParamsCount = 9;
((VFrameHeader*) op_addEllipse_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addEllipse_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[9];
((VFrameHeader*) op_addEllipse_thread_spec) -> functionName = "addEllipse";
((VFrameHeader*) op_addEllipseOutline_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 52;
((VFrameHeader*) op_addEllipseOutline_thread_spec) -> formalParamsCount = 10;
((VFrameHeader*) op_addEllipseOutline_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addEllipseOutline_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[10];
((VFrameHeader*) op_addEllipseOutline_thread_spec) -> functionName = "addEllipseOutline";
((VFrameHeader*) op_addArc_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 69;
((VFrameHeader*) op_addArc_thread_spec) -> formalParamsCount = 13;
((VFrameHeader*) op_addArc_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addArc_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[11];
((VFrameHeader*) op_addArc_thread_spec) -> functionName = "addArc";
((VFrameHeader*) op_addPie_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 61;
((VFrameHeader*) op_addPie_thread_spec) -> formalParamsCount = 12;
((VFrameHeader*) op_addPie_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addPie_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[12];
((VFrameHeader*) op_addPie_thread_spec) -> functionName = "addPie";
((VFrameHeader*) op_addPolygon_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 28;
((VFrameHeader*) op_addPolygon_thread_spec) -> formalParamsCount = 6;
((VFrameHeader*) op_addPolygon_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addPolygon_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[13];
((VFrameHeader*) op_addPolygon_thread_spec) -> functionName = "addPolygon";
((VFrameHeader*) op_addPolygonOutline_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 36;
((VFrameHeader*) op_addPolygonOutline_thread_spec) -> formalParamsCount = 7;
((VFrameHeader*) op_addPolygonOutline_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addPolygonOutline_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[14];
((VFrameHeader*) op_addPolygonOutline_thread_spec) -> functionName = "addPolygonOutline";
((VFrameHeader*) op_addPolygonBezier_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 36;
((VFrameHeader*) op_addPolygonBezier_thread_spec) -> formalParamsCount = 7;
((VFrameHeader*) op_addPolygonBezier_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addPolygonBezier_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[15];
((VFrameHeader*) op_addPolygonBezier_thread_spec) -> functionName = "addPolygonBezier";
((VFrameHeader*) op_addPolygonBezierOutline_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 44;
((VFrameHeader*) op_addPolygonBezierOutline_thread_spec) -> formalParamsCount = 8;
((VFrameHeader*) op_addPolygonBezierOutline_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addPolygonBezierOutline_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[16];
((VFrameHeader*) op_addPolygonBezierOutline_thread_spec) -> functionName = "addPolygonBezierOutline";
((VFrameHeader*) op_addBitmap_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_addBitmap_thread_spec) -> formalParamsCount = 8;
((VFrameHeader*) op_addBitmap_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addBitmap_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[17];
((VFrameHeader*) op_addBitmap_thread_spec) -> functionName = "addBitmap";
((VFrameHeader*) op_addTextWith_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 60;
((VFrameHeader*) op_addTextWith_thread_spec) -> formalParamsCount = 10;
((VFrameHeader*) op_addTextWith_thread_spec) -> sub = NULL;
((VFrameHeader*) op_addTextWith_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[18];
((VFrameHeader*) op_addTextWith_thread_spec) -> functionName = "addTextWith";
((VFrameHeader*) op_pushSurface_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 57;
((VFrameHeader*) op_pushSurface_thread_spec) -> formalParamsCount = 8;
((VFrameHeader*) op_pushSurface_thread_spec) -> sub = NULL;
((VFrameHeader*) op_pushSurface_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[19];
((VFrameHeader*) op_pushSurface_thread_spec) -> functionName = "pushSurface";
((VFrameHeader*) op_popSurface_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_popSurface_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_popSurface_thread_spec) -> sub = NULL;
((VFrameHeader*) op_popSurface_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[20];
((VFrameHeader*) op_popSurface_thread_spec) -> functionName = "popSurface";
((VFrameHeader*) op_setSize_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_setSize_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_setSize_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setSize_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[21];
((VFrameHeader*) op_setSize_thread_spec) -> functionName = "setSize";
((VFrameHeader*) op_setPosition_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_setPosition_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_setPosition_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setPosition_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[22];
((VFrameHeader*) op_setPosition_thread_spec) -> functionName = "setPosition";
((VFrameHeader*) op_setVisible_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_setVisible_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setVisible_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setVisible_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[23];
((VFrameHeader*) op_setVisible_thread_spec) -> functionName = "setVisible";
((VFrameHeader*) op_setResizable_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_setResizable_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setResizable_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setResizable_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[24];
((VFrameHeader*) op_setResizable_thread_spec) -> functionName = "setResizable";
((VFrameHeader*) op_setFullScreen_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_setFullScreen_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setFullScreen_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setFullScreen_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[25];
((VFrameHeader*) op_setFullScreen_thread_spec) -> functionName = "setFullScreen";
((VFrameHeader*) op_setTitle_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_setTitle_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setTitle_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setTitle_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[26];
((VFrameHeader*) op_setTitle_thread_spec) -> functionName = "setTitle";
((VFrameHeader*) op_setIcon_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_setIcon_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_setIcon_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setIcon_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[27];
((VFrameHeader*) op_setIcon_thread_spec) -> functionName = "setIcon";
((VFrameHeader*) op_setCursor_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_setCursor_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_setCursor_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setCursor_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[28];
((VFrameHeader*) op_setCursor_thread_spec) -> functionName = "setCursor";
((VFrameHeader*) op_commitBuffer_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_commitBuffer_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_commitBuffer_thread_spec) -> sub = NULL;
((VFrameHeader*) op_commitBuffer_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[29];
((VFrameHeader*) op_commitBuffer_thread_spec) -> functionName = "commitBuffer";
((VFrameHeader*) op_getPixels_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_getPixels_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_getPixels_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getPixels_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[30];
((VFrameHeader*) op_getPixels_thread_spec) -> functionName = "getPixels";
((VFrameHeader*) op_getResolution_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_getResolution_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_getResolution_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getResolution_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[31];
((VFrameHeader*) op_getResolution_thread_spec) -> functionName = "getResolution";
((VFrameHeader*) op_setBackgroundColor_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 12;
((VFrameHeader*) op_setBackgroundColor_thread_spec) -> formalParamsCount = 5;
((VFrameHeader*) op_setBackgroundColor_thread_spec) -> sub = NULL;
((VFrameHeader*) op_setBackgroundColor_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[32];
((VFrameHeader*) op_setBackgroundColor_thread_spec) -> functionName = "setBackgroundColor";
((VFrameHeader*) op_maximiseWindow_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_maximiseWindow_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_maximiseWindow_thread_spec) -> sub = NULL;
((VFrameHeader*) op_maximiseWindow_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[33];
((VFrameHeader*) op_maximiseWindow_thread_spec) -> functionName = "maximiseWindow";
((VFrameHeader*) op_minimiseWindow_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_minimiseWindow_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_minimiseWindow_thread_spec) -> sub = NULL;
((VFrameHeader*) op_minimiseWindow_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[34];
((VFrameHeader*) op_minimiseWindow_thread_spec) -> functionName = "minimiseWindow";
((VFrameHeader*) op_getMaximisedScreenRect_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_getMaximisedScreenRect_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_getMaximisedScreenRect_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getMaximisedScreenRect_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[35];
((VFrameHeader*) op_getMaximisedScreenRect_thread_spec) -> functionName = "getMaximisedScreenRect";
((VFrameHeader*) op_closeWindow_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_closeWindow_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_closeWindow_thread_spec) -> sub = NULL;
((VFrameHeader*) op_closeWindow_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[36];
((VFrameHeader*) op_closeWindow_thread_spec) -> functionName = "closeWindow";
((VFrameHeader*) op_initMediaLayer_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 1;
((VFrameHeader*) op_initMediaLayer_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_initMediaLayer_thread_spec) -> sub = NULL;
((VFrameHeader*) op_initMediaLayer_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[37];
((VFrameHeader*) op_initMediaLayer_thread_spec) -> functionName = "initMediaLayer";
((VFrameHeader*) op_runSystemLoop_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 0;
((VFrameHeader*) op_runSystemLoop_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_runSystemLoop_thread_spec) -> sub = NULL;
((VFrameHeader*) op_runSystemLoop_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[38];
((VFrameHeader*) op_runSystemLoop_thread_spec) -> functionName = "runSystemLoop";
((VFrameHeader*) op_shutdown_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 0;
((VFrameHeader*) op_shutdown_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_shutdown_thread_spec) -> sub = NULL;
((VFrameHeader*) op_shutdown_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[39];
((VFrameHeader*) op_shutdown_thread_spec) -> functionName = "shutdown";
((VFrameHeader*) op_loadFont_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_loadFont_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_loadFont_thread_spec) -> sub = NULL;
((VFrameHeader*) op_loadFont_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[40];
((VFrameHeader*) op_loadFont_thread_spec) -> functionName = "loadFont";
((VFrameHeader*) op_getTextWidth_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_getTextWidth_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_getTextWidth_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getTextWidth_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[41];
((VFrameHeader*) op_getTextWidth_thread_spec) -> functionName = "getTextWidth";
((VFrameHeader*) op_getFontMetrics_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_getFontMetrics_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_getFontMetrics_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getFontMetrics_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[42];
((VFrameHeader*) op_getFontMetrics_thread_spec) -> functionName = "getFontMetrics";
((VFrameHeader*) op_getFontName_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_getFontName_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_getFontName_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getFontName_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[43];
((VFrameHeader*) op_getFontName_thread_spec) -> functionName = "getFontName";
((VFrameHeader*) op_isFontFixedWidth_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_isFontFixedWidth_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_isFontFixedWidth_thread_spec) -> sub = NULL;
((VFrameHeader*) op_isFontFixedWidth_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[44];
((VFrameHeader*) op_isFontFixedWidth_thread_spec) -> functionName = "isFontFixedWidth";
((VFrameHeader*) op_getTextBitmapWith_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 44;
((VFrameHeader*) op_getTextBitmapWith_thread_spec) -> formalParamsCount = 7;
((VFrameHeader*) op_getTextBitmapWith_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getTextBitmapWith_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[45];
((VFrameHeader*) op_getTextBitmapWith_thread_spec) -> functionName = "getTextBitmapWith";
((VFrameHeader*) op_unloadFont_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_unloadFont_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_unloadFont_thread_spec) -> sub = NULL;
((VFrameHeader*) op_unloadFont_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[46];
((VFrameHeader*) op_unloadFont_thread_spec) -> functionName = "unloadFont";
((VFrameHeader*) op_flow_initMediaLayer_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 1;
((VFrameHeader*) op_flow_initMediaLayer_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_flow_initMediaLayer_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_initMediaLayer_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[47];
((VFrameHeader*) op_flow_initMediaLayer_thread_spec) -> functionName = "flow_initMediaLayer";
((VFrameHeader*) op_flow_makeWindow_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 17;
((VFrameHeader*) op_flow_makeWindow_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_flow_makeWindow_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_makeWindow_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[48];
((VFrameHeader*) op_flow_makeWindow_thread_spec) -> functionName = "flow_makeWindow";
((VFrameHeader*) op_flow_setVisible_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_flow_setVisible_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_flow_setVisible_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_setVisible_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[49];
((VFrameHeader*) op_flow_setVisible_thread_spec) -> functionName = "flow_setVisible";
((VFrameHeader*) op_flow_setFullScreen_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_flow_setFullScreen_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_flow_setFullScreen_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_setFullScreen_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[50];
((VFrameHeader*) op_flow_setFullScreen_thread_spec) -> functionName = "flow_setFullScreen";
((VFrameHeader*) op_flow_setResizable_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_flow_setResizable_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_flow_setResizable_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_setResizable_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[51];
((VFrameHeader*) op_flow_setResizable_thread_spec) -> functionName = "flow_setResizable";
((VFrameHeader*) op_flow_setPosition_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_flow_setPosition_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_flow_setPosition_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_setPosition_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[52];
((VFrameHeader*) op_flow_setPosition_thread_spec) -> functionName = "flow_setPosition";
((VFrameHeader*) op_flow_setSize_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_flow_setSize_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_flow_setSize_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_setSize_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[53];
((VFrameHeader*) op_flow_setSize_thread_spec) -> functionName = "flow_setSize";
((VFrameHeader*) op_flow_setTitle_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_flow_setTitle_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_flow_setTitle_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_setTitle_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[54];
((VFrameHeader*) op_flow_setTitle_thread_spec) -> functionName = "flow_setTitle";
((VFrameHeader*) op_flow_setIcon_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_flow_setIcon_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_flow_setIcon_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_setIcon_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[55];
((VFrameHeader*) op_flow_setIcon_thread_spec) -> functionName = "flow_setIcon";
((VFrameHeader*) op_flow_setCursor_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_flow_setCursor_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_flow_setCursor_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_setCursor_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[56];
((VFrameHeader*) op_flow_setCursor_thread_spec) -> functionName = "flow_setCursor";
((VFrameHeader*) op_flow_closeWindow_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_flow_closeWindow_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_flow_closeWindow_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_closeWindow_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[57];
((VFrameHeader*) op_flow_closeWindow_thread_spec) -> functionName = "flow_closeWindow";
((VFrameHeader*) op_flow_getEvents_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_flow_getEvents_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_flow_getEvents_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_getEvents_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[58];
((VFrameHeader*) op_flow_getEvents_thread_spec) -> functionName = "flow_getEvents";
((VFrameHeader*) op_flow_renderBegin_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_flow_renderBegin_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_flow_renderBegin_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_renderBegin_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[59];
((VFrameHeader*) op_flow_renderBegin_thread_spec) -> functionName = "flow_renderBegin";
((VFrameHeader*) op_flow_renderEnd_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_flow_renderEnd_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_flow_renderEnd_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_renderEnd_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[60];
((VFrameHeader*) op_flow_renderEnd_thread_spec) -> functionName = "flow_renderEnd";
((VFrameHeader*) op_flow_wait_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_flow_wait_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_flow_wait_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_wait_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[61];
((VFrameHeader*) op_flow_wait_thread_spec) -> functionName = "flow_wait";
((VFrameHeader*) op_flow_getPixels_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_flow_getPixels_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_flow_getPixels_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_getPixels_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[62];
((VFrameHeader*) op_flow_getPixels_thread_spec) -> functionName = "flow_getPixels";
((VFrameHeader*) op_flow_point_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 28;
((VFrameHeader*) op_flow_point_thread_spec) -> formalParamsCount = 7;
((VFrameHeader*) op_flow_point_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_point_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[63];
((VFrameHeader*) op_flow_point_thread_spec) -> functionName = "flow_point";
((VFrameHeader*) op_flow_line_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 53;
((VFrameHeader*) op_flow_line_thread_spec) -> formalParamsCount = 11;
((VFrameHeader*) op_flow_line_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_line_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[64];
((VFrameHeader*) op_flow_line_thread_spec) -> functionName = "flow_line";
((VFrameHeader*) op_flow_curve_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 45;
((VFrameHeader*) op_flow_curve_thread_spec) -> formalParamsCount = 9;
((VFrameHeader*) op_flow_curve_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_curve_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[65];
((VFrameHeader*) op_flow_curve_thread_spec) -> functionName = "flow_curve";
((VFrameHeader*) op_flow_rect_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 44;
((VFrameHeader*) op_flow_rect_thread_spec) -> formalParamsCount = 9;
((VFrameHeader*) op_flow_rect_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_rect_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[66];
((VFrameHeader*) op_flow_rect_thread_spec) -> functionName = "flow_rect";
((VFrameHeader*) op_flow_ellipse_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 44;
((VFrameHeader*) op_flow_ellipse_thread_spec) -> formalParamsCount = 9;
((VFrameHeader*) op_flow_ellipse_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_ellipse_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[67];
((VFrameHeader*) op_flow_ellipse_thread_spec) -> functionName = "flow_ellipse";
((VFrameHeader*) op_flow_ellipseOutline_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 52;
((VFrameHeader*) op_flow_ellipseOutline_thread_spec) -> formalParamsCount = 10;
((VFrameHeader*) op_flow_ellipseOutline_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_ellipseOutline_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[68];
((VFrameHeader*) op_flow_ellipseOutline_thread_spec) -> functionName = "flow_ellipseOutline";
((VFrameHeader*) op_flow_arc_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 69;
((VFrameHeader*) op_flow_arc_thread_spec) -> formalParamsCount = 13;
((VFrameHeader*) op_flow_arc_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_arc_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[69];
((VFrameHeader*) op_flow_arc_thread_spec) -> functionName = "flow_arc";
((VFrameHeader*) op_flow_pie_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 61;
((VFrameHeader*) op_flow_pie_thread_spec) -> formalParamsCount = 12;
((VFrameHeader*) op_flow_pie_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_pie_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[70];
((VFrameHeader*) op_flow_pie_thread_spec) -> functionName = "flow_pie";
((VFrameHeader*) op_flow_polygon_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 28;
((VFrameHeader*) op_flow_polygon_thread_spec) -> formalParamsCount = 6;
((VFrameHeader*) op_flow_polygon_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_polygon_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[71];
((VFrameHeader*) op_flow_polygon_thread_spec) -> functionName = "flow_polygon";
((VFrameHeader*) op_flow_polygonOutline_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 36;
((VFrameHeader*) op_flow_polygonOutline_thread_spec) -> formalParamsCount = 7;
((VFrameHeader*) op_flow_polygonOutline_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_polygonOutline_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[72];
((VFrameHeader*) op_flow_polygonOutline_thread_spec) -> functionName = "flow_polygonOutline";
((VFrameHeader*) op_flow_polygonBezier_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 36;
((VFrameHeader*) op_flow_polygonBezier_thread_spec) -> formalParamsCount = 7;
((VFrameHeader*) op_flow_polygonBezier_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_polygonBezier_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[73];
((VFrameHeader*) op_flow_polygonBezier_thread_spec) -> functionName = "flow_polygonBezier";
((VFrameHeader*) op_flow_polygonBezierOutline_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 44;
((VFrameHeader*) op_flow_polygonBezierOutline_thread_spec) -> formalParamsCount = 8;
((VFrameHeader*) op_flow_polygonBezierOutline_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_polygonBezierOutline_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[74];
((VFrameHeader*) op_flow_polygonBezierOutline_thread_spec) -> functionName = "flow_polygonBezierOutline";
((VFrameHeader*) op_flow_bitmap_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 80;
((VFrameHeader*) op_flow_bitmap_thread_spec) -> formalParamsCount = 8;
((VFrameHeader*) op_flow_bitmap_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_bitmap_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[75];
((VFrameHeader*) op_flow_bitmap_thread_spec) -> functionName = "flow_bitmap";
((VFrameHeader*) op_flow_textWith_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 60;
((VFrameHeader*) op_flow_textWith_thread_spec) -> formalParamsCount = 10;
((VFrameHeader*) op_flow_textWith_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_textWith_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[76];
((VFrameHeader*) op_flow_textWith_thread_spec) -> functionName = "flow_textWith";
((VFrameHeader*) op_flow_pushSurface_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 57;
((VFrameHeader*) op_flow_pushSurface_thread_spec) -> formalParamsCount = 8;
((VFrameHeader*) op_flow_pushSurface_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_pushSurface_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[77];
((VFrameHeader*) op_flow_pushSurface_thread_spec) -> functionName = "flow_pushSurface";
((VFrameHeader*) op_flow_popSurface_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_flow_popSurface_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_flow_popSurface_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_popSurface_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[78];
((VFrameHeader*) op_flow_popSurface_thread_spec) -> functionName = "flow_popSurface";
((VFrameHeader*) op_flow_loadFont_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_flow_loadFont_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_flow_loadFont_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_loadFont_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[79];
((VFrameHeader*) op_flow_loadFont_thread_spec) -> functionName = "flow_loadFont";
((VFrameHeader*) op_flow_getTextWidth_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 32;
((VFrameHeader*) op_flow_getTextWidth_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_flow_getTextWidth_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_getTextWidth_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[80];
((VFrameHeader*) op_flow_getTextWidth_thread_spec) -> functionName = "flow_getTextWidth";
((VFrameHeader*) op_flow_getFontMetrics_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_flow_getFontMetrics_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_flow_getFontMetrics_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_getFontMetrics_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[81];
((VFrameHeader*) op_flow_getFontMetrics_thread_spec) -> functionName = "flow_getFontMetrics";
((VFrameHeader*) op_flow_getFontName_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_flow_getFontName_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_flow_getFontName_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_getFontName_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[82];
((VFrameHeader*) op_flow_getFontName_thread_spec) -> functionName = "flow_getFontName";
((VFrameHeader*) op_flow_isFontFixedWidth_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_flow_isFontFixedWidth_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_flow_isFontFixedWidth_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_isFontFixedWidth_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[83];
((VFrameHeader*) op_flow_isFontFixedWidth_thread_spec) -> functionName = "flow_isFontFixedWidth";
((VFrameHeader*) op_flow_getTextBitmapWith_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 44;
((VFrameHeader*) op_flow_getTextBitmapWith_thread_spec) -> formalParamsCount = 7;
((VFrameHeader*) op_flow_getTextBitmapWith_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_getTextBitmapWith_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[84];
((VFrameHeader*) op_flow_getTextBitmapWith_thread_spec) -> functionName = "flow_getTextBitmapWith";
((VFrameHeader*) op_flow_unloadFont_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_flow_unloadFont_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_flow_unloadFont_thread_spec) -> sub = NULL;
((VFrameHeader*) op_flow_unloadFont_thread_spec) -> localsDef = (size_t) &object_UIPlaneLib_functions_spec[85];
((VFrameHeader*) op_flow_unloadFont_thread_spec) -> functionName = "flow_unloadFont";
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
{"addPoint", (VFrameHeader*) op_addPoint_thread_spec},
{"addLine", (VFrameHeader*) op_addLine_thread_spec},
{"addCurve", (VFrameHeader*) op_addCurve_thread_spec},
{"addRect", (VFrameHeader*) op_addRect_thread_spec},
{"addEllipse", (VFrameHeader*) op_addEllipse_thread_spec},
{"addEllipseOutline", (VFrameHeader*) op_addEllipseOutline_thread_spec},
{"addArc", (VFrameHeader*) op_addArc_thread_spec},
{"addPie", (VFrameHeader*) op_addPie_thread_spec},
{"addPolygon", (VFrameHeader*) op_addPolygon_thread_spec},
{"addPolygonOutline", (VFrameHeader*) op_addPolygonOutline_thread_spec},
{"addPolygonBezier", (VFrameHeader*) op_addPolygonBezier_thread_spec},
{"addPolygonBezierOutline", (VFrameHeader*) op_addPolygonBezierOutline_thread_spec},
{"addBitmap", (VFrameHeader*) op_addBitmap_thread_spec},
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
{"setCursor", (VFrameHeader*) op_setCursor_thread_spec},
{"commitBuffer", (VFrameHeader*) op_commitBuffer_thread_spec},
{"getPixels", (VFrameHeader*) op_getPixels_thread_spec},
{"getResolution", (VFrameHeader*) op_getResolution_thread_spec},
{"setBackgroundColor", (VFrameHeader*) op_setBackgroundColor_thread_spec},
{"maximiseWindow", (VFrameHeader*) op_maximiseWindow_thread_spec},
{"minimiseWindow", (VFrameHeader*) op_minimiseWindow_thread_spec},
{"getMaximisedScreenRect", (VFrameHeader*) op_getMaximisedScreenRect_thread_spec},
{"closeWindow", (VFrameHeader*) op_closeWindow_thread_spec},
{"initMediaLayer", (VFrameHeader*) op_initMediaLayer_thread_spec},
{"runSystemLoop", (VFrameHeader*) op_runSystemLoop_thread_spec},
{"shutdown", (VFrameHeader*) op_shutdown_thread_spec},
{"loadFont", (VFrameHeader*) op_loadFont_thread_spec},
{"getTextWidth", (VFrameHeader*) op_getTextWidth_thread_spec},
{"getFontMetrics", (VFrameHeader*) op_getFontMetrics_thread_spec},
{"getFontName", (VFrameHeader*) op_getFontName_thread_spec},
{"isFontFixedWidth", (VFrameHeader*) op_isFontFixedWidth_thread_spec},
{"getTextBitmapWith", (VFrameHeader*) op_getTextBitmapWith_thread_spec},
{"unloadFont", (VFrameHeader*) op_unloadFont_thread_spec},
{"flow_initMediaLayer", (VFrameHeader*) op_flow_initMediaLayer_thread_spec},
{"flow_makeWindow", (VFrameHeader*) op_flow_makeWindow_thread_spec},
{"flow_setVisible", (VFrameHeader*) op_flow_setVisible_thread_spec},
{"flow_setFullScreen", (VFrameHeader*) op_flow_setFullScreen_thread_spec},
{"flow_setResizable", (VFrameHeader*) op_flow_setResizable_thread_spec},
{"flow_setPosition", (VFrameHeader*) op_flow_setPosition_thread_spec},
{"flow_setSize", (VFrameHeader*) op_flow_setSize_thread_spec},
{"flow_setTitle", (VFrameHeader*) op_flow_setTitle_thread_spec},
{"flow_setIcon", (VFrameHeader*) op_flow_setIcon_thread_spec},
{"flow_setCursor", (VFrameHeader*) op_flow_setCursor_thread_spec},
{"flow_closeWindow", (VFrameHeader*) op_flow_closeWindow_thread_spec},
{"flow_getEvents", (VFrameHeader*) op_flow_getEvents_thread_spec},
{"flow_renderBegin", (VFrameHeader*) op_flow_renderBegin_thread_spec},
{"flow_renderEnd", (VFrameHeader*) op_flow_renderEnd_thread_spec},
{"flow_wait", (VFrameHeader*) op_flow_wait_thread_spec},
{"flow_getPixels", (VFrameHeader*) op_flow_getPixels_thread_spec},
{"flow_point", (VFrameHeader*) op_flow_point_thread_spec},
{"flow_line", (VFrameHeader*) op_flow_line_thread_spec},
{"flow_curve", (VFrameHeader*) op_flow_curve_thread_spec},
{"flow_rect", (VFrameHeader*) op_flow_rect_thread_spec},
{"flow_ellipse", (VFrameHeader*) op_flow_ellipse_thread_spec},
{"flow_ellipseOutline", (VFrameHeader*) op_flow_ellipseOutline_thread_spec},
{"flow_arc", (VFrameHeader*) op_flow_arc_thread_spec},
{"flow_pie", (VFrameHeader*) op_flow_pie_thread_spec},
{"flow_polygon", (VFrameHeader*) op_flow_polygon_thread_spec},
{"flow_polygonOutline", (VFrameHeader*) op_flow_polygonOutline_thread_spec},
{"flow_polygonBezier", (VFrameHeader*) op_flow_polygonBezier_thread_spec},
{"flow_polygonBezierOutline", (VFrameHeader*) op_flow_polygonBezierOutline_thread_spec},
{"flow_bitmap", (VFrameHeader*) op_flow_bitmap_thread_spec},
{"flow_textWith", (VFrameHeader*) op_flow_textWith_thread_spec},
{"flow_pushSurface", (VFrameHeader*) op_flow_pushSurface_thread_spec},
{"flow_popSurface", (VFrameHeader*) op_flow_popSurface_thread_spec},
{"flow_loadFont", (VFrameHeader*) op_flow_loadFont_thread_spec},
{"flow_getTextWidth", (VFrameHeader*) op_flow_getTextWidth_thread_spec},
{"flow_getFontMetrics", (VFrameHeader*) op_flow_getFontMetrics_thread_spec},
{"flow_getFontName", (VFrameHeader*) op_flow_getFontName_thread_spec},
{"flow_isFontFixedWidth", (VFrameHeader*) op_flow_isFontFixedWidth_thread_spec},
{"flow_getTextBitmapWith", (VFrameHeader*) op_flow_getTextBitmapWith_thread_spec},
{"flow_unloadFont", (VFrameHeader*) op_flow_unloadFont_thread_spec}};
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
{"DropEventData", &DropEventData_def
},
{"WindowEventData", &WindowEventData_def
},
{"FlowEvent", &FlowEvent_def
},
{"FlowEvent[]", &FlowEvent_array_def
},
{"FontMetrics", &FontMetrics_def
},
{"Cursor", &Cursor_def
},
{"Rect", &Rect_def
},
{"byte[]", &byte_array_def
},
{"WH", &WH_def
},
{"PixelMap", &PixelMap_def
},
{"Point", &Point_def
},
{"Point[]", &Point_array_def
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
printf("Exception::type '%s' is not referenced by associated Dana interface '%s' of native library", name, ids[0].name);
return NULL;
}

