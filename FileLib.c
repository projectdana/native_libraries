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

#ifdef OSX
#include <sys/syslimits.h>
#endif

#include <errno.h>

#include <sys/stat.h>

static CoreAPI *api;

static GlobalTypeLink *charArrayGT = NULL;
static GlobalTypeLink *fileEntryGT = NULL;
static GlobalTypeLink *fileEntryArrayGT = NULL;

INSTRUCTION_DEF op_file_open(FrameData *cframe)
	{
	char *path = x_getParam_char_array(api, cframe, 0);

	while (strchr(path, '\\') != NULL) memset(strchr(path, '\\'), '/', 1);
	
	unsigned int mode = api -> getParamRaw(cframe, 1)[0];

	FILE *fd = NULL;

	if (mode == 0)
		{
		fd = fopen(path, "rb");
		}
		else if (mode == 1)
		{
		//if the file doesn't exist we open it in a mode that will create it (then immediately close it again)
		// - if we use this mode in general, it will erase the file's contents every time, which is not what we want here
		struct stat st;
		if (stat(path, &st) != 0)
			{
			fd = fopen(path, "wb");
			if (fd != NULL)
				fclose(fd);
			fd = NULL;
			}

		//now open the file for read/write, without altering its contents
		fd = fopen(path, "rb+");
		}
		else if (mode == 2)
		{
		//create the file from scratch, erasing contents if it already exists
		fd = fopen(path, "wb+");
		}

	if (fd == NULL)
		api -> throwException(cframe, strerror(errno));
	
	api -> returnRaw(cframe, (unsigned char*) &fd, sizeof(size_t));

	free(path);

	return RETURN_OK;
	}

#define BUF_LEN 64
INSTRUCTION_DEF op_file_write(FrameData* cframe)
	{
	FILE *fd;
	memcpy(&fd, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	DanaEl* array = api -> getParamEl(cframe, 1);
	size_t len = api -> getArrayLength(array);
	unsigned char* cnt = api -> getArrayContent(array);
	
	size_t amt = 0;
	
	//iterate through param 2's contents
	if (array != NULL)
		amt += fwrite(cnt, sizeof(unsigned char), len, fd);
	
	api -> returnInt(cframe, amt);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_flush(FrameData* cframe)
	{
	FILE *fd;
	memcpy(&fd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	int osres = fflush(fd);

	unsigned char res = osres == 0;

	api -> returnRaw(cframe, &res, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_read(FrameData* cframe)
	{
	FILE *fd;
	memcpy(&fd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	size_t len = api -> getParamInt(cframe, 1);
	
	unsigned char *cnt = NULL;
	DanaEl* array = api -> makeArray(charArrayGT, len, &cnt);
	
	if (array == NULL)
		{
		len = 0;
		api -> throwException(cframe, "out of memory");
		return RETURN_OK;
		}

	size_t amt = 1;
	size_t totalAmt = 0;

	//read data up to length of param 2 or until we run out of data, whichever is first
	while ((len > 0) && (amt != 0))
		{
		amt = fread((cnt + totalAmt), sizeof(unsigned char), len, fd);
		totalAmt += amt;
		len -= amt;
		}

	if (totalAmt > 0)
		{
		api -> setArrayLength(array, totalAmt);
		api -> returnEl(cframe, array);
		}
		else
		{
		api -> destroyArray(array);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_seek(FrameData* cframe)
	{
	FILE *fd;
	memcpy(&fd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	size_t seekpos = api -> getParamInt(cframe, 1);

	//check size
	size_t fpos = ftell(fd);
	fseek(fd, 0, SEEK_END);
	size_t sz = ftell(fd);
	fseek(fd, fpos, SEEK_SET);

	//check we're not seeking past the end of the file (some OSs will allow this; we don't)
	if (seekpos > sz)
		{
		api -> throwException(cframe, "attempt to seek beyond end of file");
		return RETURN_OK;
		}

	//try to seek at an OS level
	unsigned char res = 0;

	if (fseek(fd, seekpos, SEEK_SET) == 0)
		{
		res = 1;
		}
		else
		{
		res = 0;
		fseek(fd, fpos, SEEK_SET);
		}
	
	api -> returnRaw(cframe, &res, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_size(FrameData* cframe)
	{
	FILE *fd;
	memcpy(&fd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	size_t fpos = ftell(fd);

	fseek(fd, 0, SEEK_END);

	size_t sz = ftell(fd);

	fseek(fd, fpos, SEEK_SET);
	
	api -> returnInt(cframe, sz);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_eof(FrameData* cframe)
	{
	FILE *fd;
	memcpy(&fd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	size_t fpos = ftell(fd);
	fseek(fd, 0, SEEK_END);
	size_t sz = ftell(fd);
	fseek(fd, fpos, SEEK_SET);

	unsigned char res = fpos == sz;
	
	api -> returnRaw(cframe, &res, 1);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_close(FrameData* cframe)
	{
	FILE *fd;
	memcpy(&fd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (fd != NULL)
		fclose(fd);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_exists(FrameData* cframe)
	{
	char *path = x_getParam_char_array(api, cframe, 0);

	while (strchr(path, '\\') != NULL) memset(strchr(path, '\\'), '/', 1);

	#ifdef WINDOWS
	//this is a patch for Windows - stat() fails for directories if you include a trailing slash...
	// (TODO: maybe update this to use Windows' own file system functions, not the mingw ones?)
	if (strrchr(path, '/') != NULL && ((strrchr(path, '/') - path) == strlen(path) - 1))
		{
		memset(strrchr(path, '/'), '\0', 1);
		}
	#endif

	//printf("io_file::opening file '%s'\n", path);
	unsigned char res = 0;

	struct stat st;
	if (stat(path, &st) == 0) res = 1;
	
	api -> returnRaw(cframe, &res, 1);
	
	free(path);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_delete(FrameData* cframe)
	{
	char *path = x_getParam_char_array(api, cframe, 0);

	while (strchr(path, '\\') != NULL) memset(strchr(path, '\\'), '/', 1);

	remove(path);

	unsigned char ok = 1;
	
	api -> returnRaw(cframe, &ok, 1);

	free(path);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_move(FrameData* cframe)
	{
	char *path = x_getParam_char_array(api, cframe, 0);

	while (strchr(path, '\\') != NULL) memset(strchr(path, '\\'), '/', 1);

	char *newPath = x_getParam_char_array(api, cframe, 1);

	while (strchr(newPath, '\\') != NULL) memset(strchr(newPath, '\\'), '/', 1);

	#ifdef WINDOWS
	MoveFile(path, newPath);
	#endif

	#ifdef LINUX
	rename(path, newPath);
	#endif

	unsigned char ok = 1;
	
	api -> returnRaw(cframe, &ok, 1);
	
	free(path);
	free(newPath);

	return RETURN_OK;
	}

bool copyfile(char *from, char *to)
	{
	FILE *ifd = fopen(from, "rb");
	if (ifd == NULL) return false;
	FILE *ofd = fopen(to, "wb");

	if (ofd == NULL)
		{
		fclose(ifd);
		return false;
		}

	char buffer[32768];
	size_t n = 0;

	while ((n = fread(buffer, sizeof(char), sizeof(buffer), ifd)) > 0)
		{
		if (fwrite(buffer, sizeof(char), n, ofd) != n)
			{
			fclose(ifd);
			fclose(ofd);
			remove(to);
			return false;
			}
		}

	fclose(ifd);
	fclose(ofd);

	return true;
	}

INSTRUCTION_DEF op_file_copy(FrameData* cframe)
	{
	char *path = x_getParam_char_array(api, cframe, 0);

	while (strchr(path, '\\') != NULL) memset(strchr(path, '\\'), '/', 1);

	char *newPath = x_getParam_char_array(api, cframe, 1);

	while (strchr(newPath, '\\') != NULL) memset(strchr(newPath, '\\'), '/', 1);

	//uint8 recursive = getVariableContent(cframe, 2)[0];

	unsigned char ok = 1;

	ok = copyfile(path, newPath);

	if (ok == 0)
		{
		api -> throwException(cframe, "failed to copy file");
		}
	
	api -> returnRaw(cframe, &ok, 1);
	
	free(path);
	free(newPath);

	return RETURN_OK;
	}

typedef struct _fii{
	DanaEl* data;
	struct _fii *next;
	} FileInfoItem;

// http://stackoverflow.com/questions/612097/how-can-i-get-a-list-of-files-in-a-directory-using-c-or-c
INSTRUCTION_DEF op_get_dir_content(FrameData* cframe)
	{
	char *path = x_getParam_char_array(api, cframe, 0);
	
	DanaEl* data = api -> getParamEl(cframe, 1);

	FileInfoItem *itemList = NULL;
	FileInfoItem *lastItem = NULL;
	int count = 0;

	#ifdef WINDOWS
	DWORD dwAttrib = GetFileAttributes(path);
	int exists = dwAttrib & FILE_ATTRIBUTE_DIRECTORY ? 1 : 0;

	if (exists)
		{
		if (path[strlen(path)-1] != '/' && path[strlen(path)-1] != '\\')
			{
			path = realloc(path, strlen(path)+2);
			strcat(path, "/");
			}

		path = realloc(path, strlen(path)+2);
		strcat(path, "*");

		WIN32_FIND_DATA fi;
		HANDLE hnd = FindFirstFile(path, &fi);

		if (hnd != INVALID_HANDLE_VALUE)
			{
			do
				{
				if (strcmp(fi.cFileName, ".") != 0 && strcmp(fi.cFileName, "..") != 0)
					{
					count ++;

					FileInfoItem *newItem = malloc(sizeof(FileInfoItem));
					memset(newItem, '\0', sizeof(FileInfoItem));
					
					newItem -> data = api -> makeData(fileEntryGT);
					
					size_t asz = strlen(fi.cFileName);
					
					unsigned char* cnt = NULL;
					DanaEl* fName = api -> makeArray(charArrayGT, asz, &cnt);
					memcpy(cnt, fi.cFileName, asz);
					
					api -> setDataFieldEl(newItem -> data, 0, fName);

					if (itemList == NULL)
						itemList = newItem;
						else
						lastItem -> next = newItem;
					lastItem = newItem;
					}
				} while(FindNextFile(hnd, &fi));

			FindClose(hnd);
			}
		}
	#endif

	#ifdef LINUX
	while (strchr(path, '\\') != NULL) memset(strchr(path, '\\'), '/', 1);

	if (strlen(path) == 0)
		{
		free(path);
		path = strdup(".");
		}

	struct dirent *dp;
	DIR *dir = opendir(path);

	if (dir != NULL)
		{
		while (dir != NULL && ((dp=readdir(dir)) != NULL))
			{
			if (strcmp(dp->d_name, ".") != 0 && strcmp(dp->d_name, "..") != 0)
				{
				count ++;

				FileInfoItem *newItem = malloc(sizeof(FileInfoItem));
				memset(newItem, '\0', sizeof(FileInfoItem));

				newItem -> data = api -> makeData(fileEntryGT);
				
				size_t asz = strlen(dp->d_name);
				unsigned char* cnt = NULL;
				DanaEl* fName = api -> makeArray(charArrayGT, asz, &cnt);
				
				memcpy(cnt, dp->d_name, asz);
				
				api -> setDataFieldEl(newItem -> data, 0, fName);
				
				if (itemList == NULL)
					itemList = newItem;
					else
					lastItem -> next = newItem;
				lastItem = newItem;
				}
			}
		closedir(dir);
		}
	#endif

	if (count > 0)
		{
		DanaEl* newArray = api -> makeArray(fileEntryArrayGT, count, NULL);
		
		FileInfoItem *fw = itemList;
		int i = 0;
		for (i = 0; i < count; i++)
			{
			api -> setArrayCellEl(newArray, i, fw -> data);
			
			FileInfoItem *td = fw;
			fw = fw -> next;
			free(td);
			}
		
		api -> setDataFieldEl(data, 0, newArray);
		}

	free(path);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_make_dir(FrameData* cframe)
	{
	char *path = x_getParam_char_array(api, cframe, 0);

	unsigned char ok = 1;

	#ifdef WINDOWS
	ok = CreateDirectory(path, NULL);
	#endif

	#ifdef LINUX
	ok = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
	#endif

	api -> returnRaw(cframe, &ok, 1);

	free(path);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_delete_dir(FrameData* cframe)
	{
	char *path = x_getParam_char_array(api, cframe, 0);

	unsigned char ok = 0;

	#ifdef WINDOWS
	ok = RemoveDirectory(path);
	#endif

	#ifdef LINUX
	ok = rmdir(path) == 0;
	#endif
	
	api -> returnRaw(cframe, &ok, 1);
	
	free(path);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_info(FrameData* cframe)
	{
	DanaEl *array = api -> getParamEl(cframe, 0);

	if (array != NULL)
		{
		char *path = x_getParam_char_array(api, cframe, 0);
		
		DanaEl* info = api -> getParamEl(cframe, 1);
		
		DanaEl* modInfo = api -> getDataFieldEl(info, 2);
		
		unsigned char *content = api -> getDataContent(info);
		unsigned char *contentL2 = api -> getDataContent(modInfo);

		size_t ndx = 0;

		unsigned char *type = content;
		ndx += sizeof(unsigned char);

		size_t *size = (size_t*) &content[ndx];
		ndx += sizeof(size_t);

		ndx = 0;

		uint16 *modifiedYear = (uint16*) &contentL2[ndx];
		ndx += sizeof(uint16);

		unsigned char *modifiedMonth = &contentL2[ndx];
		ndx += sizeof(unsigned char);

		unsigned char *modifiedDay = &contentL2[ndx];
		ndx += sizeof(unsigned char);

		unsigned char *modifiedHour = &contentL2[ndx];
		ndx += sizeof(unsigned char);

		unsigned char *modifiedMinute = &contentL2[ndx];
		ndx += sizeof(unsigned char);

		unsigned char *modifiedSecond = &contentL2[ndx];

		#ifdef WINDOWS
		int exists = 0;
		DWORD dwAttrib = GetFileAttributes(path);
		exists = (dwAttrib != INVALID_FILE_ATTRIBUTES && !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) ? 1 : 0;

		WIN32_FILE_ATTRIBUTE_DATA fi;
		GetFileAttributesEx(path, GetFileExInfoStandard, &fi);

		if (fi.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
			*type = 2;
			}
			else if (exists)
			{
			*type = 1;
			}
			else
			{
			*type = 0;
			}

		if (*type == 1)
			{
			//size
			copyToDanaInteger((unsigned char*) size, (unsigned char*) &fi.nFileSizeLow, sizeof(fi.nFileSizeLow));
			}

		//printf("\ntype for '%s': %u\n", path, *type);

		//modified time
		SYSTEMTIME modifiedUTC;
		SYSTEMTIME modified;
		FileTimeToSystemTime(&fi.ftLastWriteTime, &modifiedUTC);
		SystemTimeToTzSpecificLocalTime(NULL, &modifiedUTC, &modified);

		size_t ci = 0;
		copyToDanaInteger((unsigned char*) &ci, (unsigned char*) &modified.wYear, sizeof(modified.wYear));

		*modifiedYear = ((uint16*) &ci)[(sizeof(ci)/2)-1];
		*modifiedMonth = modified.wMonth;
		*modifiedDay = modified.wDay;
		*modifiedHour = modified.wHour;
		*modifiedMinute = modified.wMinute;
		*modifiedSecond = modified.wSecond;
		#endif

		#ifdef LINUX
		while (strchr(path, '\\') != NULL) memset(strchr(path, '\\'), '/', 1);

		struct stat st;

		if (stat(path, &st) == 0)
			{
			if (st.st_mode & S_IFDIR)
				*type = 2;
				else if (st.st_mode & S_IFREG)
				*type = 1;

			if (st.st_mode & S_IFREG)
				{
				size_t xs = st.st_size;
				copyToDanaInteger((unsigned char*) size, (unsigned char*) &xs, sizeof(xs));
				}

			struct tm modified; memset(&modified, '\0', sizeof(modified));

			//gmtime_r(&st.st_mtime, &modified); //UTC
			localtime_r(&st.st_mtime, &modified);

			modified.tm_year += 1900;
			modified.tm_mon += 1;

			size_t ci = 0;
			size_t xs = modified.tm_year;
			copyToDanaInteger((unsigned char*) &ci, (unsigned char*) &xs, sizeof(xs));

			*modifiedYear = ((uint16*) &ci)[(sizeof(ci)/2)-1];
			*modifiedMonth = modified.tm_mon;
			*modifiedDay = modified.tm_mday;
			*modifiedHour = modified.tm_hour;
			*modifiedMinute = modified.tm_min;
			*modifiedSecond = modified.tm_sec;
			}
		#endif

		free(path);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_full_path(FrameData* cframe)
	{
	char *qpath = x_getParam_char_array(api, cframe, 0);

	#ifdef WINDOWS
	unsigned char* cnt = NULL;
	DanaEl* array = api -> makeArray(charArrayGT, PATH_MAX, &cnt);

	if (array == NULL)
		{
		api -> throwException(cframe, "out of memory");
		return RETURN_OK;
		}

	if (GetFullPathNameA(qpath, MAX_PATH, (char*) cnt, NULL) == 0)
		{
		api -> throwException(cframe, "failed to resolve path");
		api -> destroyArray(array);
		return RETURN_OK;
		}

	while (strchr((char*) cnt, '\\') != NULL) memset(strchr((char*) cnt, '\\'), '/', 1);
	
	api -> setArrayLength(array, strlen((char*) cnt));

	api -> returnEl(cframe, array);
	#endif
	#ifdef LINUX
	unsigned char* cnt = NULL;
	DanaEl* array = api -> makeArray(charArrayGT, PATH_MAX, &cnt);
	
	if (array == NULL)
		{
		api -> throwException(cframe, "out of memory");
		return RETURN_OK;
		}

	char *ptr = realpath(qpath, (char*) cnt);

	if (ptr == NULL)
		{
		api -> throwException(cframe, "failed to resolve path");
		api -> destroyArray(array);
		return RETURN_OK;
		}

	api -> setArrayLength(array, strlen((char*) cnt));
	
	api -> returnEl(cframe, array);
	#endif

	free(qpath);

	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;

	// grab global type mappings for anything that we generate here
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));

	fileEntryGT = api -> resolveGlobalTypeMapping(getTypeDefinition("FileEntry"));

	fileEntryArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("FileEntry[]"));

	setInterfaceFunction("open", op_file_open);
	setInterfaceFunction("write", op_file_write);
	setInterfaceFunction("read", op_file_read);
	setInterfaceFunction("flush", op_file_flush);
	setInterfaceFunction("setPos", op_file_seek);
	setInterfaceFunction("getSize", op_file_size);
	setInterfaceFunction("eof", op_file_eof);
	setInterfaceFunction("close", op_file_close);

	setInterfaceFunction("getDirectoryContents", op_get_dir_content);
	setInterfaceFunction("getInfo", op_get_info);
	setInterfaceFunction("exists", op_file_exists);
	setInterfaceFunction("delete", op_file_delete);
	setInterfaceFunction("move", op_file_move);
	setInterfaceFunction("copy", op_file_copy);
	setInterfaceFunction("createDirectory", op_make_dir);
	setInterfaceFunction("deleteDirectory", op_delete_dir);
	setInterfaceFunction("getFullPath", op_get_full_path);

	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(charArrayGT);
	api -> decrementGTRefCount(fileEntryGT);
	api -> decrementGTRefCount(fileEntryArrayGT);
	}
