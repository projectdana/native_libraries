#ifndef _PLATFORM_UTILS
#define _PLATFORM_UTILS

typedef struct _fi{
	char *path;
	struct _fi *next;
	} FileInfo;

char* getFullFilePath(char *f);

FileInfo* getDirFiles(char *path, unsigned int *amt);

void lowercase(char *str);

#endif
