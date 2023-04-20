//Written by Barry Porter, 2016

#include "dana_lib_defs.h"
#include "nli_util.h"
#include "vmi_util.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <limits.h>

// IPv6 help:
// http://uw714doc.sco.com/en/SDK_netapi/sockC.PortIPv4appIPv6.html
// http://long.ccaba.upc.es/long/045Guidelines/eva/ipv6.html
// http://msdn.microsoft.com/en-us/library/windows/desktop/bb530741(v=vs.85).aspx

#ifdef WINDOWS
// :-| (this is required to access getaddrinfo and other IPv6 functions)
# if !defined( _WIN32_WINNT ) || ( _WIN32_WINNT < 0x0501 )
#  undef  _WIN32_WINNT
#  define _WIN32_WINNT 0x0501
# endif
# if !defined( WINVER ) || ( WINVER < 0x0501 )
#  undef  WINVER
#  define WINVER 0x0501
# endif

#include <Ws2tcpip.h>
#include <winsock2.h>

//#(X-SOC-MS-1): it seems we need the following hack to properly close a TCP socket:
// - "Windows closesocket() is completely broken and actually doesn't work properly"
// - http://msdn.microsoft.com/en-us/library/ms738547%28VS.85%29.aspx
#include <Mswsock.h>

#ifdef WINDOWS
#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY            27  //IPV6_V6ONLY is missing in mingw32
#endif
#endif

//Windows socket error codes
char* getSocketError(unsigned int miErrorCode)
   {
   char* errMsg =
   (miErrorCode == WSABASEERR )? "No Error" :
   (miErrorCode == WSAEINTR )? "Interrupted system call" :
   (miErrorCode == WSAEBADF )? "Bad file number" :
   (miErrorCode == WSAEACCES )? "Permission denied" :
   (miErrorCode == WSAEFAULT )? "Bad address" :
   (miErrorCode == WSAEINVAL )? "Invalid argument" :
   (miErrorCode == WSAEMFILE )? "Too many open files" :
   (miErrorCode == WSAEWOULDBLOCK )? "Operation would block" :
   (miErrorCode == WSAEINPROGRESS )? "Operation now in progress" :
   (miErrorCode == WSAEALREADY )? "Operation already in progress" :
   (miErrorCode == WSAENOTSOCK )? "Socket operation on non-socket" :
   (miErrorCode == WSAEDESTADDRREQ )? "Destination address required" :
   (miErrorCode == WSAEMSGSIZE )? "Message too long" :
   (miErrorCode == WSAEPROTOTYPE )? "Protocol wrong type for socket" :
   (miErrorCode == WSAENOPROTOOPT )? "Bad protocol option" :
   (miErrorCode == WSAEPROTONOSUPPORT )? "Protocol not supported" :
   (miErrorCode == WSAESOCKTNOSUPPORT )? "Socket type not supported" :
   (miErrorCode == WSAEOPNOTSUPP )? "Operation not supported on socket" :
   (miErrorCode == WSAEPFNOSUPPORT )? "Protocol family not supported" :
   (miErrorCode == WSAEAFNOSUPPORT )? "Address family not supported by protocol family" :
   (miErrorCode == WSAEADDRINUSE )? "Address already in use" :
   (miErrorCode == WSAEADDRNOTAVAIL )? "Can't assign requested address" :
   (miErrorCode == WSAENETDOWN )? "Network is down" :
   (miErrorCode == WSAENETUNREACH )? "Network is unreachable" :
   (miErrorCode == WSAENETRESET )? "Net dropped connection or reset" :
   (miErrorCode == WSAECONNABORTED )? "Software caused connection abort" :
   (miErrorCode == WSAECONNRESET )? "Connection reset by peer" :
   (miErrorCode == WSAENOBUFS )? "No buffer space available" :
   (miErrorCode == WSAEISCONN )? "Socket is already connected" :
   (miErrorCode == WSAENOTCONN )? "Socket is not connected" :
   (miErrorCode == WSAESHUTDOWN )? "Can't send after socket shutdown" :
   (miErrorCode == WSAETOOMANYREFS )? "Too many references can't splice" :
   (miErrorCode == WSAETIMEDOUT )? "Connection timed out" :
   (miErrorCode == WSAECONNREFUSED )? "Connection refused" :
   (miErrorCode == WSAELOOP )? "Too many levels of symbolic links" :
   (miErrorCode == WSAENAMETOOLONG )? "File name too long" :
   (miErrorCode == WSAEHOSTDOWN )? "Host is down" :
   (miErrorCode == WSAEHOSTUNREACH )? "No Route to Host" :
   (miErrorCode == WSAENOTEMPTY )? "Directory not empty" :
   (miErrorCode == WSAEPROCLIM )? "Too many processes" :
   (miErrorCode == WSAEUSERS )? "Too many users" :
   (miErrorCode == WSAEDQUOT )? "Disc Quota Exceeded" :
   (miErrorCode == WSAESTALE )? "Stale NFS file handle" :
   (miErrorCode == WSAEREMOTE )? "Too many levels of remote in path" :
   (miErrorCode == WSASYSNOTREADY )? "Network SubSystem is unavailable" :
   (miErrorCode == WSAVERNOTSUPPORTED )? "WINSOCK DLL Version out of range" :
   (miErrorCode == WSANOTINITIALISED )? "Successful WSASTARTUP not yet performed" :
   (miErrorCode == WSAHOST_NOT_FOUND )? "Host not found" :
   (miErrorCode == WSATRY_AGAIN )? "Non-Authoritative Host not found" :
   (miErrorCode == WSANO_RECOVERY )? "Non-Recoverable errors: FORMERR REFUSED NOTIMP" :
   (miErrorCode == WSANO_DATA )? "Valid name no data record of requested": "unknown error";
   
   return errMsg;
   }
#endif

#ifdef LINUX
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <signal.h>
#include <netdb.h>
#include <errno.h>
#include <pthread.h>
#endif

#ifdef OSX
#include <dispatch/dispatch.h>
#endif

//maximum pending incomings for accept()
#define MAX_PENDING 100

static CoreAPI *api;

static GlobalTypeLink *charArrayGT = NULL;

#ifdef LINUX
static char* getThreadError(int err)
	{
	if (err == EAGAIN)
		return "EAGAIN";
	
	if (err == EINVAL)
		return "EINVAL";
	
	if (err == EPERM)
		return "EPERM";
	
	return "unknown error";
	}
#endif

#define MAX_ADDR 64
INSTRUCTION_DEF op_tcp_bind(FrameData* cframe)
	{
	char *addr = x_getParam_char_array(api, cframe, 0);
	
	size_t port = api -> getParamInt(cframe, 1);
	
	char portstr[MAX_ADDR];
	memset(portstr, '\0', MAX_ADDR);
	#ifdef MACHINE_32
	#ifdef WINDOWS
	itoa(port, portstr, 10);
	#endif
	#ifdef LINUX
	snprintf(portstr, MAX_ADDR, "%u", port);
	#endif
	#endif
	#ifdef MACHINE_64
	snprintf(portstr, MAX_ADDR, "%lu", port);
	#endif
	
	struct addrinfo *adr_result = NULL;
	struct addrinfo hints; memset(&hints, '\0', sizeof(hints));
	
	hints.ai_family = AF_UNSPEC; //either IPv4 or v6; alternatively use AF_INET for v4 only or AF_INET6 for v6 only
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	//NOTE: if you specify ANY, meaning "both IPv4 AND IPv6", the only fully portable way to achieve this is to create two different sockets with IPV6_V6ONLY set to "on" for the v6 one
	// - however, forcing IPV6_V6ONLY to OFF will work on Windows Vista and later, plus Linux (i.e. will correctly listen on both v4 and v6)
	// - http://msdn.microsoft.com/en-us/library/windows/desktop/bb513665(v=vs.85).aspx
	bool any_all = false;
	bool any_v6 = false;
	
	if (strcmp(addr, "ANY") == 0)
		{
		hints.ai_flags |= AF_INET6;
		free(addr);
		addr = strdup("::");
		any_all = true;
		}
		else if (strcmp(addr, "ANYv4") == 0)
		{
		hints.ai_family = AF_INET;
		hints.ai_flags |= AI_PASSIVE;
		free(addr);
		addr = strdup("0.0.0.0");
		}
		else if (strcmp(addr, "ANYv6") == 0)
		{
		hints.ai_family = AF_INET6;
		hints.ai_flags |= AI_PASSIVE;
		free(addr);
		addr = strdup("::");
		any_v6 = true;
		}
	
	int newSocket = 0;
	bool connected = false;
	bool addressOK = true;
	
	if (getaddrinfo(addr, portstr, &hints, &adr_result) != 0)
		{
		#ifdef WINDOWS
		api -> throwException(cframe, getSocketError(WSAGetLastError()));
		#endif
		#ifdef LINUX
		api -> throwException(cframe, strerror(errno));
		#endif
		
		addressOK = false;
		}
	
	if (addressOK)
		{
		newSocket = socket(adr_result -> ai_family, adr_result -> ai_socktype, adr_result -> ai_protocol);

		if (newSocket < 0 && any_all)
			{
			addressOK = true;
			
			//attempt an IPv4 ANY instead, since the general ANY is actually an IPv6-based one
			memset(&hints, '\0', sizeof(hints));
	
			hints.ai_socktype = SOCK_STREAM;
			hints.ai_protocol = IPPROTO_TCP;
			
			hints.ai_family = AF_INET;
			hints.ai_flags |= AI_PASSIVE;
			free(addr);
			addr = strdup("0.0.0.0");
			
			if (getaddrinfo(addr, portstr, &hints, &adr_result) != 0)
				{
				addressOK = false;
				}
			
			newSocket = socket(adr_result -> ai_family, adr_result -> ai_socktype, adr_result -> ai_protocol);
			
			any_all = false;
			}
		
		connected = true;
		
		if (any_all)
			{
			int no = 0;
			setsockopt(newSocket, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no));
			}
			else if (any_v6)
			{
			int yes = 1;
			setsockopt(newSocket, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&yes, sizeof(yes));
			}
		
		struct sockaddr_storage host;
		memset(&host, 0, sizeof(host));
		
		if (bind(newSocket, adr_result->ai_addr, (int)adr_result->ai_addrlen) < 0)
			{
			#ifdef WINDOWS
			api -> throwException(cframe, getSocketError(WSAGetLastError()));
			#endif
			#ifdef LINUX
			api -> throwException(cframe, strerror(errno));
			#endif
			
			connected = false;
			}
		
		if (connected)
			{
			if (listen(newSocket, MAX_PENDING) < 0)
				{
				#ifdef WINDOWS
				api -> throwException(cframe, getSocketError(WSAGetLastError()));
				#endif
				#ifdef LINUX
				api -> throwException(cframe, strerror(errno));
				#endif
				
				connected = false;
				}
			}
		
		freeaddrinfo(adr_result);
		}
	
	size_t xs = connected ? newSocket : 0;
	
	api -> returnRaw(cframe, (unsigned char*) &xs, sizeof(size_t));
	
	free(addr);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_tcp_unbind(FrameData* cframe)
	{
	size_t xs;
	memcpy(&xs, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	#ifdef WINDOWS
	closesocket(xs);
	#endif
	#ifdef LINUX
	shutdown(xs, SHUT_RDWR);
	close(xs);
	#endif
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_tcp_connect(FrameData* cframe)
	{
	char *addr = x_getParam_char_array(api, cframe, 0);
	
	size_t port = api -> getParamInt(cframe, 1);
	
	char portstr[MAX_ADDR];
	memset(portstr, '\0', MAX_ADDR);
	#ifdef MACHINE_32
	#ifdef WINDOWS
	itoa(port, portstr, 10);
	#endif
	#ifdef LINUX
	snprintf(portstr, MAX_ADDR, "%u", port);
	#endif
	#endif
	#ifdef MACHINE_64
	snprintf(portstr, MAX_ADDR, "%lu", port);
	#endif
	
	struct addrinfo *adr_result = NULL;
	struct addrinfo hints; memset(&hints, '\0', sizeof(hints));
	
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	
	int newSocket = 0;
	bool connected = false;
	
	if (getaddrinfo(addr, portstr, &hints, &adr_result) != 0)
		{
		#ifdef WINDOWS
		//printf(" - TCPlib::connect::Address construction error on %s:%s [%s]\n", addr, portstr, getSocketError(WSAGetLastError()));
		#endif
		}
		else
		{
		newSocket = socket(adr_result->ai_family, adr_result->ai_socktype, adr_result->ai_protocol);
		
		if (newSocket < 0)
			{
			#ifdef WINDOWS
			api -> throwException(cframe, getSocketError(WSAGetLastError()));
			#endif
			#ifdef LINUX
			api -> throwException(cframe, strerror(errno));
			#endif
			}
			else
			{		
			connected = true;
			
			if (connect(newSocket, adr_result->ai_addr, (int) adr_result->ai_addrlen) < 0)
				{
				#ifdef WINDOWS
				api -> throwException(cframe, getSocketError(WSAGetLastError()));
				#endif
				#ifdef LINUX
				api -> throwException(cframe, strerror(errno));
				#endif
				
				#ifdef WINDOWS
				closesocket(newSocket);
				#endif
				
				#ifdef LINUX
				close(newSocket);
				#endif
				
				connected = false;
				}
			}
		
		freeaddrinfo(adr_result);
		}
	
	size_t xs = connected ? newSocket : 0;
	
	api -> returnRaw(cframe, (unsigned char*) &xs, sizeof(size_t));
	
	free(addr);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_tcp_disconnect(FrameData* cframe)
	{
	size_t xs;
	memcpy(&xs, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	#ifdef WINDOWS
	//#(X-SOC-MS-1)
	if (xs != 0)
		{
		GUID guidDisconnectEx = WSAID_DISCONNECTEX;
		DWORD bytesret = 0;
		LPFN_DISCONNECTEX disconn;
		WSAIoctl(xs, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidDisconnectEx, sizeof(guidDisconnectEx), &disconn, sizeof(disconn), &bytesret, NULL, NULL);
		if (bytesret == sizeof(disconn)) disconn(xs, NULL, 0, 0); else printf("CRITICAL SOCKET ERROR\n");
		}
	closesocket(xs);
	#endif
	#ifdef LINUX
	shutdown(xs, SHUT_RDWR);
	//NOTE: do we now need to read any/all pending data on the socket?
	// - http://blog.netherlabs.nl/articles/2009/01/18/the-ultimate-so_linger-page-or-why-is-my-tcp-not-reliable
	close(xs);
	#endif
    
	return RETURN_OK;
	}

INSTRUCTION_DEF op_tcp_accept(FrameData* cframe)
	{
	size_t xs;
	memcpy(&xs, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	int masterSocket = xs;
	
	int socket = 0;
	
	if ((socket = accept(masterSocket, NULL, NULL)) < 0)
		{
		//here we do nothing if it's an interrupt, which happens if we intentionally unbind an accept()ing socket
		// - we assume this is not exceptional behaviour from the perspective of the programmer
		
		#ifdef WINDOWS
		unsigned int ec = WSAGetLastError();
		if (ec != WSAEINTR)
			api -> throwException(cframe, getSocketError(ec));
		#endif
		#ifdef LINUX
		unsigned int ec = errno;
		if (ec != EINTR && ec != EINVAL)
			api -> throwException(cframe, strerror(ec));
		#endif
		
		socket = 0;
		}
	
	xs = socket;
	
	api -> returnRaw(cframe, (unsigned char*) &xs, sizeof(size_t));
	
	return RETURN_OK;
	}

#define BUF_LEN 128

INSTRUCTION_DEF op_tcp_recv(FrameData *cframe)
	{
	size_t xs;
	memcpy(&xs, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	int socket = xs;
	
	size_t len = api -> getParamInt(cframe, 1);
	
	unsigned char* content = NULL;
	DanaEl* array = api -> makeArray(charArrayGT, len, &content);
	
	if (array == NULL)
		{
		len = 0;
		api -> throwException(cframe, "out of memory to allocate recv buffer");
		return RETURN_OK;
		}
	
	int amt = 1;
	size_t totalAmt = 0;
	
	//iterate through param 2's contents
	while ((len > 0) && (amt != 0))
		{
		amt = recv(socket, (char*) (content + totalAmt), len, 0);
		
		if (amt < 0)
			{
			#ifdef LINUX
			//api -> throwException(cframe, strerror(errno));
			#endif
			break;
			}
		
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

INSTRUCTION_DEF op_tcp_send(FrameData* cframe)
	{
	size_t xs = 0;
	memcpy(&xs, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	int socket = xs;
	
	DanaEl* array = api -> getParamEl(cframe, 1);
	
	int totalAmt = 0;
	
	if (array != NULL)
		{
		totalAmt = send(socket, (char*) api -> getArrayContent(array), api -> getArrayLength(array), 0);

		if (totalAmt < 0)
			{
			#ifdef WINDOWS
			api -> throwException(cframe, getSocketError(WSAGetLastError()));
			#endif
			#ifdef LINUX
			api -> throwException(cframe, strerror(errno));
			#endif

			totalAmt = 0;
			}
		}
	
	api -> returnInt(cframe, totalAmt);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_tcp_get_local_address(FrameData* cframe)
	{
	size_t xs = 0;
	memcpy(&xs, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	int socket = xs;
	
	#ifdef WINDOWS
	char ipstr[NI_MAXHOST];
	#endif
	#ifdef LINUX
	char ipstr[INET6_ADDRSTRLEN];
	#endif
	
	int port;
	unsigned int alen = sizeof(ipstr);
	
	struct sockaddr_storage addr;
	
	#ifdef WINDOWS
	int addr_len;
	#endif
	
	#ifdef LINUX
	socklen_t addr_len;
	#endif
	
	addr_len = sizeof(addr);
	
	#ifdef WINDOWS
	getsockname(socket, (struct sockaddr*)&addr, &addr_len);
	
	if (addr.ss_family == AF_INET) {
	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    port = ntohs(s->sin_port);
	    strcpy(ipstr, inet_ntoa(s -> sin_addr));
	    alen = strlen(ipstr);
	} else { // AF_INET6
	    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
	    port = ntohs(s->sin6_port);
	    getnameinfo((struct sockaddr *) s, sizeof(struct sockaddr_in6), ipstr, sizeof(ipstr), NULL, 0, NI_NUMERICHOST);
	    alen = strlen(ipstr);
    }
	#endif
	
	#ifdef LINUX
	getsockname(socket, (struct sockaddr*)&addr, &addr_len);
	
	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    port = ntohs(s->sin_port);
	    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	} else { // AF_INET6
	    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
	    port = ntohs(s->sin6_port);
	    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}
	alen = strlen(ipstr);
	#endif
	
	DanaEl* rdata = api -> getParamEl(cframe, 1);
	
	unsigned char* cnt = NULL;
	DanaEl* newArray = api -> makeArray(charArrayGT, alen, &cnt);
	memcpy(cnt, ipstr, alen);
	
	api -> setDataFieldEl(rdata, 0, newArray);
	api -> setDataFieldInt(rdata, 1, port);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_tcp_get_remote_address(FrameData* cframe)
	{
	size_t xs = 0;
	memcpy(&xs, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	int socket = xs;
	
	#ifdef WINDOWS
	char ipstr[NI_MAXHOST];
	#endif
	#ifdef LINUX
	char ipstr[INET6_ADDRSTRLEN];
	#endif
	
	int port;
	unsigned int alen = sizeof(ipstr);
	
	struct sockaddr_storage addr;
	
	#ifdef WINDOWS
	int addr_len;
	#endif
	
	#ifdef LINUX
	socklen_t addr_len;
	#endif
	
	addr_len = sizeof(addr);
	
	#ifdef WINDOWS
	getpeername(socket, (struct sockaddr*)&addr, &addr_len);
	
	if (addr.ss_family == AF_INET) {
	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    port = ntohs(s->sin_port);
	    strncpy(ipstr, inet_ntoa(s -> sin_addr), sizeof(ipstr));
	    alen = strlen(ipstr);
	} else { // AF_INET6
	    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
	    port = ntohs(s->sin6_port);
	    getnameinfo((struct sockaddr *) s, sizeof(struct sockaddr_in6), ipstr, sizeof(ipstr), NULL, 0, NI_NUMERICHOST);
	    alen = strlen(ipstr);
    }
	#endif
	
	#ifdef LINUX
	getpeername(socket, (struct sockaddr*)&addr, &addr_len);
	
	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
	    struct sockaddr_in *s = (struct sockaddr_in *)&addr;
	    port = ntohs(s->sin_port);
	    inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
	} else { // AF_INET6
	    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
	    port = ntohs(s->sin6_port);
	    inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
	}
	alen = strlen(ipstr);
	#endif
	
	DanaEl* rdata = api -> getParamEl(cframe, 1);
	
	unsigned char* cnt = NULL;
	DanaEl* newArray = api -> makeArray(charArrayGT, alen, &cnt);
	memcpy(cnt, ipstr, alen);
	
	api -> setDataFieldEl(rdata, 0, newArray);
	api -> setDataFieldInt(rdata, 1, port);
	
	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	#ifdef WINDOWS
	WSADATA wsaData;
	
	if (WSAStartup(MAKEWORD(2, 0), &wsaData) != 0)
		{
		//api -> debugOutput(NULL, "TCPlib::Windows socket (winsock2) initialisation failed [%s]", getSocketError(WSAGetLastError()).c_str());
		}
	#endif
	
	#ifdef LINUX
	//on linux make sure we don't get killed by a SIGPIPE
	signal(SIGPIPE, SIG_IGN);
	#endif
	
	api = capi;
	
	// grab global type mappings for anything that we generate here
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	
	setInterfaceFunction("connect", op_tcp_connect);
	setInterfaceFunction("disconnect", op_tcp_disconnect);
	setInterfaceFunction("send", op_tcp_send);
	setInterfaceFunction("recv", op_tcp_recv);
	
	setInterfaceFunction("accept", op_tcp_accept);
	setInterfaceFunction("bind", op_tcp_bind);
	setInterfaceFunction("unbind", op_tcp_unbind);
	
	setInterfaceFunction("getLocalAddress", op_tcp_get_local_address);
	setInterfaceFunction("getRemoteAddress", op_tcp_get_remote_address);
	
	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(charArrayGT);
	}
