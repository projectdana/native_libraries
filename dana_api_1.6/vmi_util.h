#ifndef __VMI__UTIL
#define __VMI__UTIL

#include "dana_lib_defs.h"

void copyToDanaInteger(unsigned char *dst, unsigned char *src, size_t srcsz);

char* x_getParam_char_array(CoreAPI* api, FrameData *f, int pIndex);

#endif
