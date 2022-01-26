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

#define PI 3.14159265

static CoreAPI *api;

static GlobalTypeLink *charArrayGT = NULL;

INSTRUCTION_DEF op_sqrt_dec(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = sqrt(d);
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_power(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	double p = api -> getParamDec(cframe, 1);
	
	d = pow(d, p);
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_root(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	double p = api -> getParamDec(cframe, 1);
	
	d = pow(d, 1.0/p);
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_log(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = log10(d);
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_natlog(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = log(d);
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_natexp(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = exp(d);
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_cosine(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = cos(d * (PI / 180.0));
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_sine(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = sin(d * (PI / 180.0));
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_tangent(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = tan(d * (PI / 180.0));
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_arc_cosine(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = acos(d * (PI / 180.0));
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_arc_sine(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = asin(d * (PI / 180.0));
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_arc_tangent(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = atan(d * (PI / 180.0));
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_cosine_hyp(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = cosh(d);
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_sine_hyp(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = sinh(d);
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_tangent_hyp(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = tanh(d);
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_arc_cosine_hyp(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = acosh(d);
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_arc_sine_hyp(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = asinh(d);
	
	api -> returnDec(cframe, d);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_arc_tangent_hyp(FrameData *cframe)
	{
	double d = api -> getParamDec(cframe, 0);
	
	d = atanh(d);
	
	api -> returnDec(cframe, d);
	
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
