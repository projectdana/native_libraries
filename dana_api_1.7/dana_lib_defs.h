#ifndef _DANA_LIB_DEFS
#define _DANA_LIB_DEFS

#define INSTRUCTION_DEF static int
#define INSTRUCTION_PARAM_LIST VFrame *cframe

#include <stddef.h>
#include "machine_types.h"
#include "dana_types.h"
#include "dana_lib_api.h"

static const DanaType IDCDT = {TYPE_OBJECT, 0, sizeof(LiveBindpoint), NULL, 0};

static const InterfaceDetails IDCID = {"IDC", 3, NULL};

#define IDC (size_t) &IDCDT, 0, 0, NULL

#endif
