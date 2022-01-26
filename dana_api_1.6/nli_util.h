#ifndef __NLI__UTIL
#define __NLI__UTIL

void setInterfaceFunction(char *name, void *ptr);

Interface* getPublicInterface();

DanaType* getTypeDefinition(char *name);

#endif