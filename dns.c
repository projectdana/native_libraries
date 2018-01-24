//Written by Antreas Antoniou, 2016

#include "dana_lib_defs.h"
#include "nli_util.h"
#include "vmi_util.h"

#ifdef WINDOWS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <windows.h>
#pragma comment(lib, "ws2_32.lib")
#endif

#ifdef LINUX
#include <stdio.h> //printf
#include <string.h> //memset
#include <stdlib.h> //for exit(0);
#include <sys/socket.h>
#include <errno.h> //For errno - the error number
#include <netdb.h> //hostent
#include <arpa/inet.h>
#include <sys/utsname.h>
#include <unistd.h>
#endif

#include <sys/stat.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <pthread.h>

static CoreAPI *api;

static GlobalTypeLink *charArrayGT = NULL;

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

#ifdef WINDOWS
static int initialise(void)
{
	// Initialize Winsock
			WSADATA wsaData;
			int iResult;
	    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	    if (iResult != 0) {
	        printf("WSAStartup failed: %d\n", iResult);
	        return 1;
	    }
		return 0;
}

static void uninitialise (void)
{
    WSACleanup ();
}
#endif

#define MAX_VAR_NAME 2048

int hostname_to_ip(char *hostname , char *ip)
{
    //int sockfd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in *h;
    int rv;
    memset(&hints, '\0', sizeof(hints));
    hints.ai_family = AF_INET; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		#ifdef WINDOWS
			initialise();
		#endif
    if ( (rv = getaddrinfo( hostname , "http" , &hints , &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
		#ifdef WINDOWS
			uninitialise();
		#endif
    //loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        h = (struct sockaddr_in *) p->ai_addr;
        strcpy(ip , inet_ntoa( h->sin_addr ) );
    }

    freeaddrinfo(servinfo); // all done with this structure
    return 0;
}

INSTRUCTION_DEF op_get_host_by_name(INSTRUCTION_PARAM_LIST)
{
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 0)) -> content;

	char *vn = NULL;

	if (array != NULL)
	{
		vn = malloc(array -> length + 1);
		memset(vn, '\0', array -> length + 1);
		memcpy(vn, array -> data, array -> length);
	}
	else
	{
		vn = strdup("");
	}
	
	char ip[100];
	hostname_to_ip(vn , ip);
	char *val = ip;
	
	if (val != NULL)
	{
		returnByteArray(cframe, (unsigned char*) strdup(val), strlen(val));
	}

	free(vn);

	return RETURN_DIRECT;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	// grab global type mappings for anything that we generate here
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	
	setInterfaceFunction("getHostIP", op_get_host_by_name);
	
	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(charArrayGT);
	}
