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
static const DanaType Track_def;
static const DanaTypeField function_Track_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_Track_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_Track_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_Track_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaType Source_def;
static const DanaTypeField function_Source_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_Source_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_Source_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_Source_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField Format_fields[] = {
{(DanaType*) &byte_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaType Format_def = 
{TYPE_DATA, 0, 17, (DanaTypeField*) Format_fields, 3};
static const DanaTypeField function_Source_Source_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &byte_array_def, NULL, 0, 0, 8},
{(DanaType*) &Format_def, NULL, 0, 0, 48}};
static const DanaTypeField function_Source_getLength_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaType Source_functions_spec[] = {
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) function_Source_clone_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) function_Source_equals_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) function_Source_toString_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) function_Source_getID_fields, 1},
{TYPE_FUNCTION, 0, 88, (DanaTypeField*) function_Source_Source_fields, 3},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) function_Source_getLength_fields, 1}};
static const DanaTypeField Source_functions_spec_fields[] = {
{(DanaType*) &Source_functions_spec[0], "clone", 5},
{(DanaType*) &Source_functions_spec[1], "equals", 6},
{(DanaType*) &Source_functions_spec[2], "toString", 8},
{(DanaType*) &Source_functions_spec[3], "getID", 5},
{(DanaType*) &Source_functions_spec[4], "Source", 6},
{(DanaType*) &Source_functions_spec[5], "getLength", 9}};
static const DanaType Source_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) Source_functions_spec_fields, 6},
{TYPE_DATA, 0, 0, NULL, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField Source_spec_fields[] = {
{(DanaType*) &Source_spec[0], ".functions", 10},
{(DanaType*) &Source_spec[1], ".events", 7},
{(DanaType*) &Source_spec[2], ".state", 6},
};
static const DanaType Source_def = {TYPE_OBJECT, 0, 0, (DanaTypeField*) Source_spec_fields, 3};
static const DanaTypeField function_Track_Track_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Source_def, NULL, 0, 0, 8}};
static const DanaTypeField function_Track_setFinishEvent_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &bool_def, NULL, 0, 0, 0}};
static const DanaTypeField function_Track_setVolume_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_Track_getLengthFrames_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_Track_setPosFrame_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_Track_getPosFrame_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_Track_getLengthMS_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_Track_setPosMS_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_Track_getPosMS_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaType Track_functions_spec[] = {
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) function_Track_clone_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) function_Track_equals_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) function_Track_toString_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) function_Track_getID_fields, 1},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) function_Track_Track_fields, 2},
{TYPE_FUNCTION, 0, 1, (DanaTypeField*) function_Track_setFinishEvent_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) function_Track_setVolume_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) function_Track_getLengthFrames_fields, 1},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) function_Track_setPosFrame_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) function_Track_getPosFrame_fields, 1},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) function_Track_getLengthMS_fields, 1},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) function_Track_setPosMS_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) function_Track_getPosMS_fields, 1}};
static const DanaTypeField Track_functions_spec_fields[] = {
{(DanaType*) &Track_functions_spec[0], "clone", 5},
{(DanaType*) &Track_functions_spec[1], "equals", 6},
{(DanaType*) &Track_functions_spec[2], "toString", 8},
{(DanaType*) &Track_functions_spec[3], "getID", 5},
{(DanaType*) &Track_functions_spec[4], "Track", 5},
{(DanaType*) &Track_functions_spec[5], "setFinishEvent", 14},
{(DanaType*) &Track_functions_spec[6], "setVolume", 9},
{(DanaType*) &Track_functions_spec[7], "getLengthFrames", 15},
{(DanaType*) &Track_functions_spec[8], "setPosFrame", 11},
{(DanaType*) &Track_functions_spec[9], "getPosFrame", 11},
{(DanaType*) &Track_functions_spec[10], "getLengthMS", 11},
{(DanaType*) &Track_functions_spec[11], "setPosMS", 8},
{(DanaType*) &Track_functions_spec[12], "getPosMS", 8}};
static const DanaType Track_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) Track_functions_spec_fields, 13},
{TYPE_DATA, 0, 0, NULL, 0},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField Track_spec_fields[] = {
{(DanaType*) &Track_spec[0], ".functions", 10},
{(DanaType*) &Track_spec[1], ".events", 7},
{(DanaType*) &Track_spec[2], ".state", 6},
};
static const DanaType Track_def = {TYPE_OBJECT, 0, 0, (DanaTypeField*) Track_spec_fields, 3};
static const DanaTypeField DeviceInfo_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaType DeviceInfo_def = 
{TYPE_DATA, 0, 48, (DanaTypeField*) DeviceInfo_fields, 2};
static const DanaTypeField DeviceInfo_array_fields[] = {
{(DanaType*) &DeviceInfo_def, NULL, 0, 0, 0}};
static const DanaType DeviceInfo_array_def = 
{TYPE_ARRAY, 0, 0, (DanaTypeField*) DeviceInfo_array_fields, 1};
static const DanaTypeField function_AudioLib_clone_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_AudioLib_equals_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &Object_def, NULL, 0, 1, 8}};
static const DanaTypeField function_AudioLib_toString_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_AudioLib_getID_fields[] = {
{(DanaType*) &char_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_AudioLib_decoderLoad_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &byte_def, NULL, 0, 0, 8},
{(DanaType*) &byte_array_def, NULL, 0, 0, 16},
{(DanaType*) &byte_def, NULL, 0, 0, 56},
{(DanaType*) &int_def, NULL, 0, 0, 57},
{(DanaType*) &int_def, NULL, 0, 0, 65}};
static const DanaTypeField function_AudioLib_decoderGetLengthFrames_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8}};
static const DanaTypeField function_AudioLib_decoderGetRawData_fields[] = {
{(DanaType*) &byte_array_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 40}};
static const DanaTypeField function_AudioLib_decoderDestroy_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_AudioLib_trackLoad_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8}};
static const DanaTypeField function_AudioLib_trackSetVolume_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &int_def, NULL, 0, 0, 9}};
static const DanaTypeField function_AudioLib_trackSeek_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &int_def, NULL, 0, 0, 9}};
static const DanaTypeField function_AudioLib_trackSetFinishEvent_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &bool_def, NULL, 0, 0, 8}};
static const DanaTypeField function_AudioLib_trackGetPos_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8}};
static const DanaTypeField function_AudioLib_trackDestroy_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_AudioLib_deviceInit_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &byte_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 17},
{(DanaType*) &int_def, NULL, 0, 0, 25}};
static const DanaTypeField function_AudioLib_deviceSetDevice_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 8},
{(DanaType*) &int_def, NULL, 0, 0, 16}};
static const DanaTypeField function_AudioLib_devicePlay_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &Track_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 56}};
static const DanaTypeField function_AudioLib_deviceLoop_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &Track_def, NULL, 0, 0, 16},
{(DanaType*) &int_def, NULL, 0, 0, 56}};
static const DanaTypeField function_AudioLib_deviceStop_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1},
{(DanaType*) &int_def, NULL, 0, 0, 9}};
static const DanaTypeField function_AudioLib_deviceStopAll_fields[] = {
{(DanaType*) &bool_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 1}};
static const DanaTypeField function_AudioLib_deviceDestroy_fields[] = {
{(DanaType*) &void_def, NULL, 0, 0, 0},{(DanaType*) &int_def, NULL, 0, 0, 0}};
static const DanaTypeField function_AudioLib_getPlaybackDevices_fields[] = {
{(DanaType*) &DeviceInfo_array_def, NULL, 0, 0, 0}};
static const DanaTypeField function_AudioLib_getCaptureDevices_fields[] = {
{(DanaType*) &DeviceInfo_array_def, NULL, 0, 0, 0}};
static const DanaTypeField TrackInfo_fields[] = {
{(DanaType*) &Track_def, NULL, 0, 0, 0}};
static const DanaType TrackInfo_def = 
{TYPE_DATA, 0, 40, (DanaTypeField*) TrackInfo_fields, 1};
static const DanaTypeField event_AudioLib_trackFinished_fields[] = {
{(DanaType*) &TrackInfo_def, NULL, 0, 0, 0}};
static const DanaType object_AudioLib_functions_spec[] = {
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_AudioLib_clone_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_AudioLib_equals_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_AudioLib_toString_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_AudioLib_getID_fields, 1},
{TYPE_FUNCTION, 0, 73, (DanaTypeField*) &function_AudioLib_decoderLoad_fields, 6},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_AudioLib_decoderGetLengthFrames_fields, 2},
{TYPE_FUNCTION, 0, 48, (DanaTypeField*) &function_AudioLib_decoderGetRawData_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_AudioLib_decoderDestroy_fields, 2},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_AudioLib_trackLoad_fields, 2},
{TYPE_FUNCTION, 0, 17, (DanaTypeField*) &function_AudioLib_trackSetVolume_fields, 3},
{TYPE_FUNCTION, 0, 17, (DanaTypeField*) &function_AudioLib_trackSeek_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_AudioLib_trackSetFinishEvent_fields, 3},
{TYPE_FUNCTION, 0, 16, (DanaTypeField*) &function_AudioLib_trackGetPos_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_AudioLib_trackDestroy_fields, 2},
{TYPE_FUNCTION, 0, 33, (DanaTypeField*) &function_AudioLib_deviceInit_fields, 5},
{TYPE_FUNCTION, 0, 24, (DanaTypeField*) &function_AudioLib_deviceSetDevice_fields, 3},
{TYPE_FUNCTION, 0, 64, (DanaTypeField*) &function_AudioLib_devicePlay_fields, 4},
{TYPE_FUNCTION, 0, 64, (DanaTypeField*) &function_AudioLib_deviceLoop_fields, 4},
{TYPE_FUNCTION, 0, 17, (DanaTypeField*) &function_AudioLib_deviceStop_fields, 3},
{TYPE_FUNCTION, 0, 9, (DanaTypeField*) &function_AudioLib_deviceStopAll_fields, 2},
{TYPE_FUNCTION, 0, 8, (DanaTypeField*) &function_AudioLib_deviceDestroy_fields, 2},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_AudioLib_getPlaybackDevices_fields, 1},
{TYPE_FUNCTION, 0, 40, (DanaTypeField*) &function_AudioLib_getCaptureDevices_fields, 1}};
static const DanaTypeField intf_functions_def[] = {
{(DanaType*) &object_AudioLib_functions_spec[0], "clone", 5},
{(DanaType*) &object_AudioLib_functions_spec[1], "equals", 6},
{(DanaType*) &object_AudioLib_functions_spec[2], "toString", 8},
{(DanaType*) &object_AudioLib_functions_spec[3], "getID", 5},
{(DanaType*) &object_AudioLib_functions_spec[4], "decoderLoad", 11},
{(DanaType*) &object_AudioLib_functions_spec[5], "decoderGetLengthFrames", 22},
{(DanaType*) &object_AudioLib_functions_spec[6], "decoderGetRawData", 17},
{(DanaType*) &object_AudioLib_functions_spec[7], "decoderDestroy", 14},
{(DanaType*) &object_AudioLib_functions_spec[8], "trackLoad", 9},
{(DanaType*) &object_AudioLib_functions_spec[9], "trackSetVolume", 14},
{(DanaType*) &object_AudioLib_functions_spec[10], "trackSeek", 9},
{(DanaType*) &object_AudioLib_functions_spec[11], "trackSetFinishEvent", 19},
{(DanaType*) &object_AudioLib_functions_spec[12], "trackGetPos", 11},
{(DanaType*) &object_AudioLib_functions_spec[13], "trackDestroy", 12},
{(DanaType*) &object_AudioLib_functions_spec[14], "deviceInit", 10},
{(DanaType*) &object_AudioLib_functions_spec[15], "deviceSetDevice", 15},
{(DanaType*) &object_AudioLib_functions_spec[16], "devicePlay", 10},
{(DanaType*) &object_AudioLib_functions_spec[17], "deviceLoop", 10},
{(DanaType*) &object_AudioLib_functions_spec[18], "deviceStop", 10},
{(DanaType*) &object_AudioLib_functions_spec[19], "deviceStopAll", 13},
{(DanaType*) &object_AudioLib_functions_spec[20], "deviceDestroy", 13},
{(DanaType*) &object_AudioLib_functions_spec[21], "getPlaybackDevices", 18},
{(DanaType*) &object_AudioLib_functions_spec[22], "getCaptureDevices", 17}};
static const DanaType object_AudioLib_events_spec[] = {
{TYPE_EVENTSOURCE, 0, 0, (DanaTypeField*) &event_AudioLib_trackFinished_fields, 1}};
static const DanaTypeField intf_events_def[] = {
{(DanaType*) &object_AudioLib_events_spec[0], "trackFinished", 13}};
static const DanaType AudioLib_object_spec[] = {
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_functions_def, 23},
{TYPE_DATA, 0, 0, (DanaTypeField*) intf_events_def, 1},
{TYPE_DATA, 0, 0, NULL, 0}
};
static const DanaTypeField intf_def[] = {
{(DanaType*) &AudioLib_object_spec[0], ".functions", 10},
{(DanaType*) &AudioLib_object_spec[1], ".events", 7},
{(DanaType*) &AudioLib_object_spec[2], ".state", 6},
};
static unsigned char op_clone_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_equals_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_toString_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getID_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_decoderLoad_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_decoderGetLengthFrames_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_decoderGetRawData_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_decoderDestroy_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_trackLoad_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_trackSetVolume_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_trackSeek_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_trackSetFinishEvent_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_trackGetPos_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_trackDestroy_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_deviceInit_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_deviceSetDevice_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_devicePlay_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_deviceLoop_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_deviceStop_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_deviceStopAll_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_deviceDestroy_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getPlaybackDevices_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static unsigned char op_getCaptureDevices_thread_spec[sizeof(VFrameHeader)+sizeof(VFrame)];
static SourceHeader header;
static DanaComponent self;
static size_t interfaceFunctions[] = {
(size_t) op_clone_thread_spec,
(size_t) op_equals_thread_spec,
(size_t) op_toString_thread_spec,
(size_t) op_getID_thread_spec,
(size_t) op_decoderLoad_thread_spec,
(size_t) op_decoderGetLengthFrames_thread_spec,
(size_t) op_decoderGetRawData_thread_spec,
(size_t) op_decoderDestroy_thread_spec,
(size_t) op_trackLoad_thread_spec,
(size_t) op_trackSetVolume_thread_spec,
(size_t) op_trackSeek_thread_spec,
(size_t) op_trackSetFinishEvent_thread_spec,
(size_t) op_trackGetPos_thread_spec,
(size_t) op_trackDestroy_thread_spec,
(size_t) op_deviceInit_thread_spec,
(size_t) op_deviceSetDevice_thread_spec,
(size_t) op_devicePlay_thread_spec,
(size_t) op_deviceLoop_thread_spec,
(size_t) op_deviceStop_thread_spec,
(size_t) op_deviceStopAll_thread_spec,
(size_t) op_deviceDestroy_thread_spec,
(size_t) op_getPlaybackDevices_thread_spec,
(size_t) op_getCaptureDevices_thread_spec};
static DanaType libType = {TYPE_OBJECT, 0, 0, (DanaTypeField*) intf_def, 3};
static InterfaceDetails ids[] = {{"AudioLib", 8, &libType}};
static Interface objectInterfaces[] = {{&ids[0], {&self, NULL, NULL, interfaceFunctions, NULL, NULL}}		};
static ObjectSpec objects[] = {{objectInterfaces, 1, 0, 1, 0, (size_t) &bool_def, (size_t) &emptyType}};
Interface* getPublicInterface(){
((VFrameHeader*) op_clone_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_clone_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_clone_thread_spec) -> sub = NULL;
((VFrameHeader*) op_clone_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[0];
((VFrameHeader*) op_clone_thread_spec) -> functionName = "clone";
((VFrameHeader*) op_equals_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_equals_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_equals_thread_spec) -> sub = NULL;
((VFrameHeader*) op_equals_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[1];
((VFrameHeader*) op_equals_thread_spec) -> functionName = "equals";
((VFrameHeader*) op_toString_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_toString_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_toString_thread_spec) -> sub = NULL;
((VFrameHeader*) op_toString_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[2];
((VFrameHeader*) op_toString_thread_spec) -> functionName = "toString";
((VFrameHeader*) op_getID_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getID_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getID_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getID_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[3];
((VFrameHeader*) op_getID_thread_spec) -> functionName = "getID";
((VFrameHeader*) op_decoderLoad_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 73;
((VFrameHeader*) op_decoderLoad_thread_spec) -> formalParamsCount = 5;
((VFrameHeader*) op_decoderLoad_thread_spec) -> sub = NULL;
((VFrameHeader*) op_decoderLoad_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[4];
((VFrameHeader*) op_decoderLoad_thread_spec) -> functionName = "decoderLoad";
((VFrameHeader*) op_decoderGetLengthFrames_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_decoderGetLengthFrames_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_decoderGetLengthFrames_thread_spec) -> sub = NULL;
((VFrameHeader*) op_decoderGetLengthFrames_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[5];
((VFrameHeader*) op_decoderGetLengthFrames_thread_spec) -> functionName = "decoderGetLengthFrames";
((VFrameHeader*) op_decoderGetRawData_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 48;
((VFrameHeader*) op_decoderGetRawData_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_decoderGetRawData_thread_spec) -> sub = NULL;
((VFrameHeader*) op_decoderGetRawData_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[6];
((VFrameHeader*) op_decoderGetRawData_thread_spec) -> functionName = "decoderGetRawData";
((VFrameHeader*) op_decoderDestroy_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_decoderDestroy_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_decoderDestroy_thread_spec) -> sub = NULL;
((VFrameHeader*) op_decoderDestroy_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[7];
((VFrameHeader*) op_decoderDestroy_thread_spec) -> functionName = "decoderDestroy";
((VFrameHeader*) op_trackLoad_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_trackLoad_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_trackLoad_thread_spec) -> sub = NULL;
((VFrameHeader*) op_trackLoad_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[8];
((VFrameHeader*) op_trackLoad_thread_spec) -> functionName = "trackLoad";
((VFrameHeader*) op_trackSetVolume_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 17;
((VFrameHeader*) op_trackSetVolume_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_trackSetVolume_thread_spec) -> sub = NULL;
((VFrameHeader*) op_trackSetVolume_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[9];
((VFrameHeader*) op_trackSetVolume_thread_spec) -> functionName = "trackSetVolume";
((VFrameHeader*) op_trackSeek_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 17;
((VFrameHeader*) op_trackSeek_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_trackSeek_thread_spec) -> sub = NULL;
((VFrameHeader*) op_trackSeek_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[10];
((VFrameHeader*) op_trackSeek_thread_spec) -> functionName = "trackSeek";
((VFrameHeader*) op_trackSetFinishEvent_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_trackSetFinishEvent_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_trackSetFinishEvent_thread_spec) -> sub = NULL;
((VFrameHeader*) op_trackSetFinishEvent_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[11];
((VFrameHeader*) op_trackSetFinishEvent_thread_spec) -> functionName = "trackSetFinishEvent";
((VFrameHeader*) op_trackGetPos_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 16;
((VFrameHeader*) op_trackGetPos_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_trackGetPos_thread_spec) -> sub = NULL;
((VFrameHeader*) op_trackGetPos_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[12];
((VFrameHeader*) op_trackGetPos_thread_spec) -> functionName = "trackGetPos";
((VFrameHeader*) op_trackDestroy_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_trackDestroy_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_trackDestroy_thread_spec) -> sub = NULL;
((VFrameHeader*) op_trackDestroy_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[13];
((VFrameHeader*) op_trackDestroy_thread_spec) -> functionName = "trackDestroy";
((VFrameHeader*) op_deviceInit_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 33;
((VFrameHeader*) op_deviceInit_thread_spec) -> formalParamsCount = 4;
((VFrameHeader*) op_deviceInit_thread_spec) -> sub = NULL;
((VFrameHeader*) op_deviceInit_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[14];
((VFrameHeader*) op_deviceInit_thread_spec) -> functionName = "deviceInit";
((VFrameHeader*) op_deviceSetDevice_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 24;
((VFrameHeader*) op_deviceSetDevice_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_deviceSetDevice_thread_spec) -> sub = NULL;
((VFrameHeader*) op_deviceSetDevice_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[15];
((VFrameHeader*) op_deviceSetDevice_thread_spec) -> functionName = "deviceSetDevice";
((VFrameHeader*) op_devicePlay_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 64;
((VFrameHeader*) op_devicePlay_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_devicePlay_thread_spec) -> sub = NULL;
((VFrameHeader*) op_devicePlay_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[16];
((VFrameHeader*) op_devicePlay_thread_spec) -> functionName = "devicePlay";
((VFrameHeader*) op_deviceLoop_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 64;
((VFrameHeader*) op_deviceLoop_thread_spec) -> formalParamsCount = 3;
((VFrameHeader*) op_deviceLoop_thread_spec) -> sub = NULL;
((VFrameHeader*) op_deviceLoop_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[17];
((VFrameHeader*) op_deviceLoop_thread_spec) -> functionName = "deviceLoop";
((VFrameHeader*) op_deviceStop_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 17;
((VFrameHeader*) op_deviceStop_thread_spec) -> formalParamsCount = 2;
((VFrameHeader*) op_deviceStop_thread_spec) -> sub = NULL;
((VFrameHeader*) op_deviceStop_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[18];
((VFrameHeader*) op_deviceStop_thread_spec) -> functionName = "deviceStop";
((VFrameHeader*) op_deviceStopAll_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 9;
((VFrameHeader*) op_deviceStopAll_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_deviceStopAll_thread_spec) -> sub = NULL;
((VFrameHeader*) op_deviceStopAll_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[19];
((VFrameHeader*) op_deviceStopAll_thread_spec) -> functionName = "deviceStopAll";
((VFrameHeader*) op_deviceDestroy_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 8;
((VFrameHeader*) op_deviceDestroy_thread_spec) -> formalParamsCount = 1;
((VFrameHeader*) op_deviceDestroy_thread_spec) -> sub = NULL;
((VFrameHeader*) op_deviceDestroy_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[20];
((VFrameHeader*) op_deviceDestroy_thread_spec) -> functionName = "deviceDestroy";
((VFrameHeader*) op_getPlaybackDevices_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getPlaybackDevices_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getPlaybackDevices_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getPlaybackDevices_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[21];
((VFrameHeader*) op_getPlaybackDevices_thread_spec) -> functionName = "getPlaybackDevices";
((VFrameHeader*) op_getCaptureDevices_thread_spec) -> frameSize = sizeof(VFrame) + sizeof(VVarR) + 40;
((VFrameHeader*) op_getCaptureDevices_thread_spec) -> formalParamsCount = 0;
((VFrameHeader*) op_getCaptureDevices_thread_spec) -> sub = NULL;
((VFrameHeader*) op_getCaptureDevices_thread_spec) -> localsDef = (size_t) &object_AudioLib_functions_spec[22];
((VFrameHeader*) op_getCaptureDevices_thread_spec) -> functionName = "getCaptureDevices";
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
{"decoderLoad", (VFrameHeader*) op_decoderLoad_thread_spec},
{"decoderGetLengthFrames", (VFrameHeader*) op_decoderGetLengthFrames_thread_spec},
{"decoderGetRawData", (VFrameHeader*) op_decoderGetRawData_thread_spec},
{"decoderDestroy", (VFrameHeader*) op_decoderDestroy_thread_spec},
{"trackLoad", (VFrameHeader*) op_trackLoad_thread_spec},
{"trackSetVolume", (VFrameHeader*) op_trackSetVolume_thread_spec},
{"trackSeek", (VFrameHeader*) op_trackSeek_thread_spec},
{"trackSetFinishEvent", (VFrameHeader*) op_trackSetFinishEvent_thread_spec},
{"trackGetPos", (VFrameHeader*) op_trackGetPos_thread_spec},
{"trackDestroy", (VFrameHeader*) op_trackDestroy_thread_spec},
{"deviceInit", (VFrameHeader*) op_deviceInit_thread_spec},
{"deviceSetDevice", (VFrameHeader*) op_deviceSetDevice_thread_spec},
{"devicePlay", (VFrameHeader*) op_devicePlay_thread_spec},
{"deviceLoop", (VFrameHeader*) op_deviceLoop_thread_spec},
{"deviceStop", (VFrameHeader*) op_deviceStop_thread_spec},
{"deviceStopAll", (VFrameHeader*) op_deviceStopAll_thread_spec},
{"deviceDestroy", (VFrameHeader*) op_deviceDestroy_thread_spec},
{"getPlaybackDevices", (VFrameHeader*) op_getPlaybackDevices_thread_spec},
{"getCaptureDevices", (VFrameHeader*) op_getCaptureDevices_thread_spec}};
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
{"TrackInfo", &TrackInfo_def
},
{"DeviceInfo", &DeviceInfo_def
},
{"DeviceInfo[]", &DeviceInfo_array_def
},
{"Format", &Format_def
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

