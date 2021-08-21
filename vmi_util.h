#ifndef __VMI__UTIL
#define __VMI__UTIL

#include "dana_lib_defs.h"

void copyHostInteger(unsigned char *dst, unsigned char *src, size_t srcsz);
void copyToDanaInteger(unsigned char *dst, unsigned char *src, size_t srcsz);

unsigned char* getVariableContent(VFrame *t, unsigned int index);

//get the n'th parameter as an integer, indexed from 0
size_t getParam_int(VFrame *f, unsigned int pIndex);

//get the n'th parameter as a character array, indexed from 0; the result is allocated on the heap and must be freed
char* getParam_char_array(VFrame *f, int pIndex);

//return an int
void return_int(VFrame *f, size_t v);
void return_bool(VFrame *f, bool b);

//return a character array; a new copy of the given str is made by this function
void return_char_array(VFrame *f, CoreAPI *api, char *str);

void return_char_array_direct(VFrame *f, CoreAPI *api, char *str, size_t len);

//return a byte array; a new copy of the given str is made by this function
void return_byte_array(VFrame *f, CoreAPI *api, unsigned char *str, size_t len);

//return a character array; the given str is used directly (not copied - so it must be on the heap)
void return_byte_array_direct(VFrame *f, CoreAPI *api, unsigned char *str, size_t len);

LiveArray* make_byte_array(VFrame *f, CoreAPI *api, size_t len);

LiveArray* make_byte_array_wt(VFrame *f, CoreAPI *api, GlobalTypeLink *tl, size_t len);

void free_array(CoreAPI *api, LiveArray *array);

void return_array(VFrame *f, LiveArray *array);

#endif