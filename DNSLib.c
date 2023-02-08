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

INSTRUCTION_DEF op_get_host_by_name(FrameData* cframe)
	{
	char *vn = x_getParam_char_array(api, cframe, 0);
	
	if (strlen(vn) < 1)
		{
		api -> throwException(cframe, "unknown name to resolve");
		free(vn);
        return RETURN_OK;
		}

	char ip[100];
	memset(ip, '\0', 100);
	
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in *h;
    int rv;
    memset(&hints, '\0', sizeof(hints));
    hints.ai_family = AF_INET; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	#ifdef WINDOWS
	unsigned long iplen = 100;
	//this function will fail on Windows if given an IPv6 IP-address while in AF_INET mode, so we detect...
	if (strstr(vn, ":") != NULL)
		hints.ai_family = AF_INET6;
	initialise();
	#endif
	
    if ((rv = getaddrinfo(vn, NULL, &hints, &servinfo)) != 0)
		{
		api -> throwException(cframe, (char*) gai_strerror(rv));
		free(vn);
        return RETURN_OK;
		}
	
	#ifdef WINDOWS
	uninitialise();
	#endif
	
    //loop through the results (we could return the whole list, but here we return the last one)
    for (p = servinfo; p != NULL; p = p->ai_next)
		{
		#ifdef WINDOWS
		WSAAddressToString(p->ai_addr, p->ai_addrlen, NULL, ip, &iplen);
		//printf("iplen: %u / res %i / %i\n", iplen, res, WSAGetLastError());
		#endif
		#ifdef LINUX
		h = (struct sockaddr_in *) p->ai_addr;
        strcpy(ip, inet_ntoa(h->sin_addr));
		#endif
		}

    freeaddrinfo(servinfo); // all done with this structure
	
	char *val = ip;
	
	if (val != NULL)
		{
		unsigned char* cnt = NULL;
		DanaEl* array = api -> makeArray(charArrayGT, strlen(val), &cnt);
		memcpy(cnt, val, strlen(val));
		api -> returnEl(cframe, array);
		}

	free(vn);

	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	
	setInterfaceFunction("getHostIP", op_get_host_by_name);
	
	#ifdef WINDOWS
	WSADATA wsaData;
	
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		{
		//api -> debugOutput(NULL, "TCPlib::Windows socket (winsock2) initialisation failed [%s]", getSocketError(WSAGetLastError()).c_str());
		}
	#endif
	
	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(charArrayGT);
	}
