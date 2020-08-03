#ifdef WINDOWS
#include <Windows.h>
#endif

#ifdef LINUX
#include <dirent.h>
#endif

#include <ctype.h>

#include <stdlib.h>

#include <string.h>

#include <stddef.h>

#include <sys/stat.h>

#include "platform_utils.h"

void lowercase(char *str)
	{
	unsigned int i = 0;
	for (; str[i]; i++)
		{
		str[i] = tolower(str[i]);
		}
	}

char* getFullFilePath(char *f)
	{
	//http://www.unix.com/shell-programming-scripting/116024-how-get-fully-qualified-path-name.html
	//http://msdn.microsoft.com/en-us/library/aa364963%28VS.85%29.aspx
	#ifdef WINDOWS
	char *dbuf = malloc(512);
	unsigned int len = 0;
	
	if ((len = GetFullPathName(f, 512, dbuf, NULL)) > 512)
		{
		free(dbuf);
		dbuf = malloc(len);
		GetFullPathName(f, len, dbuf, NULL);
		}
		else if (len == 0)
		{
		free(dbuf);
		return "";
		}
	
	while (strchr(dbuf, '\\') != NULL)
		{
		memset(strchr(dbuf, '\\'), '/', 1);
		}
	
	lowercase(dbuf);
	#endif
	
	#ifdef LINUX
	char *dbuf = realpath(f, NULL);
	#endif
	
	struct stat st;
	if (stat(dbuf, &st) != 0)
		{
		free(dbuf);
		return "";
		}
		else if (st.st_mode & S_IFREG)
		{
		//it's a file; ditch the filename
		memset(strrchr(dbuf, '/'), '\0', 1);
		}
	
	if (dbuf[strlen(dbuf)] == '/') memset(strrchr(dbuf, '/'), '\0', 1);
	
	return dbuf;
	}

FileInfo* getDirFiles(char *path, unsigned int *amt)
	{
	FileInfo *first = NULL;
	FileInfo *last = NULL;
	
	#ifdef WINDOWS
	char *np = malloc(strlen(path) + 3);
	memset(np, '\0', strlen(path) + 3);
	strcpy(np, path);
	strcat(np, "/*");
	
	WIN32_FIND_DATA fi;
	
	HANDLE hnd = FindFirstFile(np, &fi);
	
	if (hnd != NULL)
		{
		do
			{
			if (strcmp(fi.cFileName, ".") != 0 && strcmp(fi.cFileName, "..") != 0)
				{
				FileInfo *nfi = malloc(sizeof(FileInfo)); memset(nfi, '\0', sizeof(FileInfo));
				
				nfi -> path = malloc(strlen(path) + strlen(fi.cFileName) + 2);
				memset(nfi -> path, '\0', strlen(path) + strlen(fi.cFileName) + 2);
				
				//printf("name: %s\n", fi.cFileName);
				
				strcpy(nfi -> path, path);
				strcat(nfi -> path, "/");
				strcat(nfi -> path, fi.cFileName);
				
				if (last == NULL)
					{
					first = nfi;
					last = nfi;
					}
					else
					{
					last -> next = nfi;
					last = nfi;
					}
				
				(*amt) ++;
				}
			} while(FindNextFile(hnd, &fi));
		
		FindClose(hnd);
		}
	
	free(np);
	#endif
	
	#ifdef LINUX
	while (strchr(path, '\\') != NULL) memset(strchr(path, '\\'), '/', 1);
	
	if (strlen(path) == 0)
		strcpy(path, ".");
	
	struct dirent *dp;
	DIR *dir = opendir(path);
	
	if (dir != NULL)
		{
		while (dir != NULL && ((dp=readdir(dir)) != NULL))
			{
			if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
				{
				FileInfo *nfi = malloc(sizeof(FileInfo)); memset(nfi, '\0', sizeof(FileInfo));
				
				nfi -> path = malloc(strlen(path) + strlen(dp->d_name) + 2);
				memset(nfi -> path, '\0', strlen(path) + strlen(dp->d_name) + 2);
				
				//printf("name: %s\n", fi.cFileName);
				
				strcpy(nfi -> path, path);
				strcat(nfi -> path, "/");
				strcat(nfi -> path, dp->d_name);
				
				if (last == NULL)
					{
					first = nfi;
					last = nfi;
					}
					else
					{
					last -> next = nfi;
					last = nfi;
					}
				
				(*amt) ++;
				}
			}
		closedir(dir);
		}
	#endif
	
	return first;
	}
