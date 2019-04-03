//Written by Barry Porter, 2016

#include "dana_lib_defs.h"
#include "nli_util.h"
#include "vmi_util.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WINDOWS
#include <Windows.h>
#endif

#ifdef LINUX
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#endif

#include <sys/stat.h>

#include <math.h>

#include "alu.h"

#include "int_util.h"

#define PI 3.14159265

static CoreAPI *api;

static GlobalTypeLink *charArrayGT = NULL;

static double getDoubleInput(VFrame *v, size_t index)
	{
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(v, index)) -> content;
	
	char *val = NULL;
	
	if (array != NULL)
		{
		val = malloc(array -> length + 1);
		memset(val, '\0', array -> length + 1);
		memcpy(val, array -> data, array -> length);
		}
		else
		{
		val = strdup("0.0");
		}
	
	double d = strtod(val, NULL);
	
	free(val);
	
	return d;
	}

static void returnDouble(VFrame *f, double d)
	{
	char *res = malloc(100);
	memset(res, '\0', 100);
	
	if (isinf(d))
		{
		if (d < 0.0)
			strcpy(res, "-inf");
			else
			strcpy(res, "inf");
		}
		else
		{
		snprintf(res, 99, "%.20f", d);
		}
	
	LiveArray *array = malloc(sizeof(LiveArray));
	memset(array, '\0', sizeof(LiveArray));
	
	array -> data = (unsigned char*) res;
	array -> length = strlen(res);
	
	array -> gtLink = charArrayGT;
	api -> incrementGTRefCount(array -> gtLink);
	array -> owner = f -> blocking -> instance;
	
	array -> refCount ++;
	
	VVarLivePTR *ptrh = (VVarLivePTR*) &f -> localsData[((DanaType*) f -> localsDef) -> fields[0].offset];
	ptrh -> content = (unsigned char*) array;
	ptrh -> typeLink = array -> gtLink -> typeLink;
	}

INSTRUCTION_DEF op_sqrt_dec(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = sqrt(d);
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_power(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	double p = getDoubleInput(cframe, 1);
	
	d = pow(d, p);
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_root(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	double p = getDoubleInput(cframe, 1);
	
	d = pow(d, 1.0/p);
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_log(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = log10(d);
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_natlog(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = log(d);
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_natexp(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = exp(d);
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_cosine(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = cos(d * (PI / 180.0));
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_sine(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = sin(d * (PI / 180.0));
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_tangent(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = tan(d * (PI / 180.0));
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_arc_cosine(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = acos(d * (PI / 180.0));
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_arc_sine(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = asin(d * (PI / 180.0));
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_arc_tangent(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = atan(d * (PI / 180.0));
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_cosine_hyp(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = cosh(d);
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_sine_hyp(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = sinh(d);
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_tangent_hyp(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = tanh(d);
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_arc_cosine_hyp(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = acosh(d);
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_arc_sine_hyp(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = asinh(d);
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_arc_tangent_hyp(VFrame *cframe)
	{
	double d = getDoubleInput(cframe, 0);
	
	d = atanh(d);
	
	returnDouble(cframe, d);
	
	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	setInterfaceFunction("sqrt", op_sqrt_dec);
	
	setInterfaceFunction("pow", op_power);
	setInterfaceFunction("root", op_root);
	
	setInterfaceFunction("log", op_log);
	setInterfaceFunction("natlog", op_natlog);
	setInterfaceFunction("natexp", op_natexp);
	
	setInterfaceFunction("cos", op_cosine);
	setInterfaceFunction("sin", op_sine);
	setInterfaceFunction("tan", op_tangent);
	
	setInterfaceFunction("acos", op_arc_cosine);
	setInterfaceFunction("asin", op_arc_sine);
	setInterfaceFunction("atan", op_arc_tangent);
	
	setInterfaceFunction("cosh", op_cosine_hyp);
	setInterfaceFunction("sinh", op_sine_hyp);
	setInterfaceFunction("tanh", op_tangent_hyp);
	
	setInterfaceFunction("acosh", op_arc_cosine_hyp);
	setInterfaceFunction("asinh", op_arc_sine_hyp);
	setInterfaceFunction("atanh", op_arc_tangent_hyp);
	
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	
	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(charArrayGT);
	}
