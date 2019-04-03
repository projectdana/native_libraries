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

static void returnByteArray(VFrame *f, unsigned char *data, size_t len)
	{
	LiveArray *array = malloc(sizeof(LiveArray));
	memset(array, '\0', sizeof(LiveArray));
	
	array -> data = data;
	array -> length = len;
	
	array -> gtLink = charArrayGT;
	api -> incrementGTRefCount(array -> gtLink);
	array -> owner = f -> blocking -> instance;
	
	array -> refCount ++;
	
	VVarLivePTR *ptrh = (VVarLivePTR*) &f -> localsData[((DanaType*) f -> localsDef) -> fields[0].offset];
	ptrh -> content = (unsigned char*) array;
	ptrh -> typeLink = array -> gtLink -> typeLink;
	}

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
		fd = fopen(path, "ab");
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

INSTRUCTION_DEF op_file_read(VFrame *cframe)
	{
	FILE *fd;
	memcpy(&fd, getVariableContent(cframe, 0), sizeof(size_t));
	
	size_t len = 0;
	copyHostInteger((unsigned char*) &len, getVariableContent(cframe, 1), sizeof(size_t));
	
	unsigned char *pbuf = malloc(len);
	
	if (pbuf == NULL)
		{
		len = 0;
		}
	
	size_t amt = 1;
	size_t totalAmt = 0;
	
	//read data up to length of param 2 or until we run out of data, whichever is first
	while ((len > 0) && (amt != 0))
		{
		amt = fread((pbuf + totalAmt), sizeof(unsigned char), len, fd);
		totalAmt += amt;
		len -= amt;
		}
	
	if (totalAmt > 0)
		{
		returnByteArray(cframe, pbuf, totalAmt);
		}
		else
		{
		free(pbuf);
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
					
					newItem -> data = malloc(sizeof(LiveData));
					
					memset(newItem -> data, '\0', sizeof(LiveData));
					newItem -> data -> owner = dataOwner;
					newItem -> data -> gtLink = fileEntryGT;
					api -> incrementGTRefCount(newItem -> data -> gtLink);
					
					newItem -> data -> data = malloc(sizeof(VVarLivePTR));
					
					memset(newItem -> data -> data, '\0', sizeof(VVarLivePTR));
					VVarLivePTR *ptrh = (VVarLivePTR*) newItem -> data -> data;
					
					LiveArray *itemArray = malloc(sizeof(LiveArray));
					memset(itemArray, '\0', sizeof(LiveArray));
					itemArray -> owner = dataOwner;
					itemArray -> gtLink = charArrayGT;
					api -> incrementGTRefCount(itemArray -> gtLink);
					itemArray -> data = malloc(strlen(fi.cFileName));
					memcpy(itemArray -> data, fi.cFileName, strlen(fi.cFileName));
					itemArray -> length = strlen(fi.cFileName);
					
					ptrh -> content = (unsigned char*) itemArray;
					ptrh -> typeLink = itemArray -> gtLink -> typeLink;
					itemArray -> refCount ++;
					
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
				
				newItem -> data = malloc(sizeof(LiveData));
				memset(newItem -> data, '\0', sizeof(LiveData));
				newItem -> data -> owner = dataOwner;
				newItem -> data -> gtLink = fileEntryGT;
				newItem -> data -> gtLink -> refCount ++;
				
				newItem -> data -> data = malloc(sizeof(VVarLivePTR));
				memset(newItem -> data -> data, '\0', sizeof(VVarLivePTR));
				VVarLivePTR *ptrh = (VVarLivePTR*) newItem -> data -> data;
				
				LiveArray *itemArray = malloc(sizeof(LiveArray));
				memset(itemArray, '\0', sizeof(LiveArray));
				itemArray -> owner = dataOwner;
				itemArray -> gtLink = charArrayGT;
				itemArray -> gtLink -> refCount ++;
				itemArray -> data = malloc(strlen(dp->d_name));
				memcpy(itemArray -> data, dp->d_name, strlen(dp->d_name));
				itemArray -> length = strlen(dp->d_name);
				
				ptrh -> content = (unsigned char*) itemArray;
				ptrh -> typeLink = itemArray -> gtLink -> typeLink;
				itemArray -> refCount ++;
				
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
		LiveArray *newArray = malloc(sizeof(LiveArray));
		memset(newArray, '\0', sizeof(LiveArray));
		
		newArray -> owner = dataOwner;
		newArray -> gtLink = fileEntryArrayGT;
		newArray -> gtLink -> refCount ++;
		newArray -> data = malloc(sizeof(VVarLivePTR) * count);
		memset(newArray -> data, '\0', sizeof(VVarLivePTR) * count);
		newArray -> length = count;
		
		FileInfoItem *fw = itemList;
		int i = 0;
		for (i = 0; i < count; i++)
			{
			VVarLivePTR *ptrh = (VVarLivePTR*) (&newArray -> data[sizeof(VVarLivePTR) * i]);
			ptrh -> content = (unsigned char*) fw -> data;
			ptrh -> typeLink = fw -> data -> gtLink -> typeLink;
			fw -> data -> refCount ++;
			
			FileInfoItem *td = fw;
			fw = fw -> next;
			free(td);
			}
		
		VVarLivePTR *ptrh = (VVarLivePTR*) data -> data;
		ptrh -> content = (unsigned char*) newArray;
		ptrh -> typeLink = newArray -> gtLink -> typeLink;
		newArray -> refCount ++;
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
		SYSTEMTIME modified;
		FileTimeToSystemTime(&fi.ftLastWriteTime, &modified);
		
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
	
	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(charArrayGT);
	api -> decrementGTRefCount(fileEntryGT);
	api -> decrementGTRefCount(fileEntryArrayGT);
	}
