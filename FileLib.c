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

#include <errno.h>

#include <sys/stat.h>

static CoreAPI *api;

static GlobalTypeLink *charArrayGT = NULL;
static GlobalTypeLink *fileEntryGT = NULL;
static GlobalTypeLink *fileEntryArrayGT = NULL;

INSTRUCTION_DEF op_file_open(VFrame *cframe)
	{
	size_t xs = 0;
	copyHostInteger((unsigned char*) &xs, getVariableContent(cframe, 1), 1);
	
	char *path = getParam_char_array(cframe, 0);
	
	while (strchr(path, '\\') != NULL) memset(strchr(path, '\\'), '/', 1);
	
	unsigned int mode = xs;
	
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
	
	//the return value is written to local variable 0
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &fd, sizeof(size_t));
	
	free(path);
	
	return RETURN_OK;
	}

#define BUF_LEN 64
INSTRUCTION_DEF op_file_write(VFrame *cframe)
	{
	FILE *fd;
	memcpy(&fd, getVariableContent(cframe, 0), sizeof(size_t));
	
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	size_t amt = 0;
	
	//printf("io_file::write %u\n", len);
	//printf(" - %p\n", fd);
	
	//iterate through param 2's contents
	if (array != NULL)
		amt += fwrite(array -> data, sizeof(unsigned char), array -> length, fd);
	
	//the return value is written to local variable 0
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	copyHostInteger((unsigned char*) result, (unsigned char*) &amt, sizeof(size_t));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_flush(VFrame *cframe)
	{
	FILE *fd;
	memcpy(&fd, getVariableContent(cframe, 0), sizeof(size_t));
	
	int osres = fflush(fd);
	
	unsigned char res = osres == 0;
	
	//the return value is written to local variable 0
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(unsigned char));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_read(VFrame *cframe)
	{
	FILE *fd;
	memcpy(&fd, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t len = 0;
	copyHostInteger((unsigned char*) &len, getVariableContent(cframe, 1), sizeof(size_t));
	
	LiveArray *array = make_byte_array_wt(cframe, api, charArrayGT, len);
	
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
		amt = fread((array -> data + totalAmt), sizeof(unsigned char), len, fd);
		totalAmt += amt;
		len -= amt;
		}
	
	if (totalAmt > 0)
		{
		array -> length = totalAmt;
		return_array(cframe, array);
		}
		else
		{
		free_array(api, array);
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_seek(VFrame *cframe)
	{
	FILE *fd;
	memcpy(&fd, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t seekpos = 0;
	copyHostInteger((unsigned char*) &seekpos, getVariableContent(cframe, 1), sizeof(size_t));
	
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
	
	//the return value is written to local variable 0
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(unsigned char));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_size(VFrame *cframe)
	{
	FILE *fd;
	memcpy(&fd, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t fpos = ftell(fd);
	
	fseek(fd, 0, SEEK_END);
	
	size_t sz = ftell(fd);
	
	fseek(fd, fpos, SEEK_SET);
	
	return_int(cframe, sz);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_eof(VFrame *cframe)
	{
	FILE *fd;
	memcpy(&fd, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t fpos = ftell(fd);
	fseek(fd, 0, SEEK_END);
	size_t sz = ftell(fd);
	fseek(fd, fpos, SEEK_SET);
	
	unsigned char res = fpos == sz;
	
	//the return value is written to local variable 0
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(unsigned char));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_close(VFrame *cframe)
	{
	FILE *fd;
	memcpy(&fd, getVariableContent(cframe, 0), sizeof(size_t));
	
	if (fd != NULL)
		fclose(fd);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_exists(VFrame *cframe)
	{
	char *path = getParam_char_array(cframe, 0);
	
	while (strchr(path, '\\') != NULL) memset(strchr(path, '\\'), '/', 1);
	
	#ifdef WINDOWS
	//this is a patch for Windows - stat() fails for directories if you include a trailing slash...
	// (TODO: maybe update this operation to use Windows' own file system functions, not the mingw ones?)
	if (strrchr(path, '/') != NULL && ((strrchr(path, '/') - path) == strlen(path) - 1))
		{
		memset(strrchr(path, '/'), '\0', 1);
		}
	#endif
	
	//printf("io_file::opening file '%s'\n", path);
	unsigned char res = 0;
	
	struct stat st;
	if (stat(path, &st) == 0) res = 1;
	
	//the return value is written to local variable 0
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &res, sizeof(unsigned char));
	
	free(path);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_delete(VFrame *cframe)
	{
	char *path = getParam_char_array(cframe, 0);
	
	while (strchr(path, '\\') != NULL) memset(strchr(path, '\\'), '/', 1);
	
	remove(path);
	
	unsigned char ok = 1;
	
	//the return value is written to local variable 0
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &ok, sizeof(unsigned char));
	
	free(path);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_file_move(VFrame *cframe)
	{
	char *path = getParam_char_array(cframe, 0);
	
	while (strchr(path, '\\') != NULL) memset(strchr(path, '\\'), '/', 1);
	
	char *newPath = getParam_char_array(cframe, 1);
	
	while (strchr(newPath, '\\') != NULL) memset(strchr(newPath, '\\'), '/', 1);
	
	#ifdef WINDOWS
	MoveFile(path, newPath);
	#endif
	
	#ifdef LINUX
	rename(path, newPath);
	#endif
	
	unsigned char ok = 1;
	
	//the return value is written to local variable 0
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &ok, sizeof(unsigned char));
	
	free(path);
	free(newPath);
	
	return RETURN_OK;
	}

bool copyfile(char *from, char *to)
	{
	FILE *ifd = fopen(from, "rb");
	FILE *ofd = fopen(to, "wb");
	
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

INSTRUCTION_DEF op_file_copy(VFrame *cframe)
	{
	char *path = getParam_char_array(cframe, 0);
	
	while (strchr(path, '\\') != NULL) memset(strchr(path, '\\'), '/', 1);
	
	char *newPath = getParam_char_array(cframe, 1);
	
	while (strchr(newPath, '\\') != NULL) memset(strchr(newPath, '\\'), '/', 1);
	
	//uint8 recursive = getVariableContent(cframe, 2)[0];
	
	unsigned char ok = 1;
	
	copyfile(path, newPath);
	
	//the return value is written to local variable 0
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &ok, sizeof(unsigned char));
	
	free(path);
	free(newPath);
	
	return RETURN_OK;
	}

typedef struct _fii{
	LiveData *data;
	struct _fii *next;
	} FileInfoItem;

// http://stackoverflow.com/questions/612097/how-can-i-get-a-list-of-files-in-a-directory-using-c-or-c
INSTRUCTION_DEF op_get_dir_content(VFrame *cframe)
	{
	char *path = getParam_char_array(cframe, 0);
	
	LiveData *data = (LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	DanaComponent *dataOwner = cframe -> blocking -> instance;
	
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
					
					size_t sz = sizeof(VVarLivePTR); 
					newItem -> data = malloc(sizeof(LiveData)+sz);
					memset(newItem -> data, '\0', sizeof(LiveData)+sz);
					
					newItem -> data -> refi.ocm = dataOwner;
					newItem -> data -> gtLink = fileEntryGT;
					api -> incrementGTRefCount(newItem -> data -> gtLink);
					
					newItem -> data -> data = ((unsigned char*) newItem -> data) + sizeof(LiveData);
					
					VVarLivePTR *ptrh = (VVarLivePTR*) newItem -> data -> data;
					
					size_t asz = strlen(fi.cFileName);
					LiveArray *itemArray = malloc(sizeof(LiveArray)+asz);
					memset(itemArray, '\0', sizeof(LiveArray)+asz);
					itemArray -> refi.ocm = dataOwner;
					itemArray -> gtLink = charArrayGT;
					api -> incrementGTRefCount(itemArray -> gtLink);
					itemArray -> data = ((unsigned char*) itemArray) + sizeof(LiveArray);
					memcpy(itemArray -> data, fi.cFileName, asz);
					itemArray -> length = asz;
					
					ptrh -> content = (unsigned char*) itemArray;
					itemArray -> refi.refCount ++;
					itemArray -> refi.type = itemArray -> gtLink -> typeLink;
					
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
				
				size_t sz = sizeof(VVarLivePTR);
				newItem -> data = malloc(sizeof(LiveData)+sz);
				memset(newItem -> data, '\0', sizeof(LiveData)+sz);
				newItem -> data -> refi.ocm = dataOwner;
				newItem -> data -> gtLink = fileEntryGT;
				api -> incrementGTRefCount(newItem -> data -> gtLink);
				
				newItem -> data -> data = ((unsigned char*) newItem -> data) + sizeof(LiveData);
				VVarLivePTR *ptrh = (VVarLivePTR*) newItem -> data -> data;
				
				size_t asz = strlen(dp->d_name);
				LiveArray *itemArray = malloc(sizeof(LiveArray)+asz);
				memset(itemArray, '\0', sizeof(LiveArray)+asz);
				itemArray -> refi.ocm = dataOwner;
				itemArray -> gtLink = charArrayGT;
				api -> incrementGTRefCount(itemArray -> gtLink);
				itemArray -> data = ((unsigned char*) itemArray) + sizeof(LiveArray);
				memcpy(itemArray -> data, dp->d_name, asz);
				itemArray -> length = asz;
				
				ptrh -> content = (unsigned char*) itemArray;
				itemArray -> refi.refCount ++;
				itemArray -> refi.type = itemArray -> gtLink -> typeLink;
				
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
		size_t asz = sizeof(VVarLivePTR) * count;
		LiveArray *newArray = malloc(sizeof(LiveArray)+asz);
		memset(newArray, '\0', sizeof(LiveArray)+asz);
		
		newArray -> refi.ocm = dataOwner;
		newArray -> gtLink = fileEntryArrayGT;
		api -> incrementGTRefCount(newArray -> gtLink);
		newArray -> data = ((unsigned char*) newArray) + sizeof(LiveArray);
		newArray -> length = count;
		newArray -> refi.type = newArray -> gtLink -> typeLink;
		
		FileInfoItem *fw = itemList;
		int i = 0;
		for (i = 0; i < count; i++)
			{
			VVarLivePTR *ptrh = (VVarLivePTR*) (&newArray -> data[sizeof(VVarLivePTR) * i]);
			ptrh -> content = (unsigned char*) fw -> data;
			fw -> data -> refi.refCount ++;
			fw -> data -> refi.type = fw -> data -> gtLink -> typeLink;
		
			FileInfoItem *td = fw;
			fw = fw -> next;
			free(td);
			}
		
		VVarLivePTR *ptrh = (VVarLivePTR*) data -> data;
		ptrh -> content = (unsigned char*) newArray;
		newArray -> refi.refCount ++;
		}
	
	free(path);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_make_dir(VFrame *cframe)
	{
	char *path = getParam_char_array(cframe, 0);
	
	unsigned char ok = 1;
	
	#ifdef WINDOWS
	ok = CreateDirectory(path, NULL);
	#endif
	
	#ifdef LINUX
	ok = mkdir(path, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
	#endif
	
	//the return value is written to local variable 0
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &ok, sizeof(unsigned char));
	
	free(path);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_delete_dir(VFrame *cframe)
	{
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 0)) -> content;
	
	char *path = NULL;
	
	if (array != NULL)
		{
		path = malloc(array -> length + 1);
		memset(path, '\0', array -> length + 1);
		memcpy(path, array -> data, array -> length);
		}
		else
		{
		path = strdup("");
		}
	
	unsigned char ok = 0;
	if (array != NULL)
		{
		memcpy(path, array -> data, array -> length);
	
		//del directory + contents, recursively
		
		ok = 1;
		
		#ifdef WINDOWS
		ok = RemoveDirectory(path);
		#endif
		
		#ifdef LINUX
		ok = rmdir(path) == 0;
		#endif
		}
	
	//the return value is written to local variable 0
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &ok, sizeof(unsigned char));
	
	free(path);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_info(VFrame *cframe)
	{
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 0)) -> content;
	
	if (array != NULL)
		{
		char *path = NULL;
		
		path = malloc(array -> length + 1);
		memset(path, '\0', array -> length + 1);
		memcpy(path, array -> data, array -> length);
		
		unsigned char *content = ((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content) -> data;
		unsigned char *contentL2 = ((LiveData*) ((VVarLivePTR*) &content[sizeof(size_t) * 2]) -> content) -> data;
		
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

INSTRUCTION_DEF op_get_full_path(VFrame *cframe)
	{
	char *qpath = getParam_char_array(cframe, 0);
	
	#ifdef WINDOWS
	LiveArray *array = make_byte_array_wt(cframe, api, charArrayGT, MAX_PATH);
	
	if (array == NULL)
		{
		api -> throwException(cframe, "out of memory");
		return RETURN_OK;
		}
	
	if (GetFullPathNameA(qpath, (char*) array -> data, MAX_PATH, NULL) == 0)
		{
		api -> throwException(cframe, "failed to resolve path");
		free_array(api, array);
		return RETURN_OK;
		}
	
	array -> length = strlen((char*) array -> data);
	
	return_array(cframe, array);
	#endif
	#ifdef LINUX
	LiveArray *array = make_byte_array_wt(cframe, api, charArrayGT, PATH_MAX);
	
	if (array == NULL)
		{
		api -> throwException(cframe, "out of memory");
		return RETURN_OK;
		}
	
	char *ptr = realpath(qpath, (char*) array -> data);
	
	if (ptr == NULL)
		{
		api -> throwException(cframe, "failed to resolve path");
		free_array(api, array);
		return RETURN_OK;
		}
	
	array -> length = strlen((char*) array -> data);
	
	return_array(cframe, array);
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
