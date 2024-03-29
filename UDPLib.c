//Written by Barry Porter, 2016

#include "dana_lib_defs.h"
#include "nli_util.h"
#include "vmi_util.h"

// http://uw714doc.sco.com/en/SDK_netapi/sockC.PortIPv4appIPv6.html
// http://long.ccaba.upc.es/long/045Guidelines/eva/ipv6.html

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WINDOWS
// this is required to access getaddrinfo and other IPv6 functions
# if !defined( _WIN32_WINNT ) || ( _WIN332_WINNT < 0x0501 )
#  undef  _WIN32_WINNT
#  define _WIN32_WINNT 0x0501
# endif
# if !defined( WINVER ) || ( WINVER < 0x0501 )
#  undef  WINVER
#  define WINVER 0x0501
# endif
#include <Ws2tcpip.h>
#include <winsock2.h>

#include "wepoll/wepoll.h"

#ifdef WINDOWS
#ifndef IPV6_V6ONLY
#define IPV6_V6ONLY            27  //sigh: IPV6_V6ONLY is missing in mingw32
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

#ifndef OSX
#include <sys/epoll.h>
#include <fcntl.h>
#endif
#endif

#ifdef OSX
#include <dispatch/dispatch.h>
#include <sys/event.h>
#endif

#include <pthread.h>

typedef struct {
	#ifdef WINDOWS
	HANDLE fd;
	#endif
	#ifdef LINUX
	int fd;
	#endif
	#ifndef OSX
	struct epoll_event *events;
	#endif
	#ifdef OSX
	struct kevent *events;
	#endif
	size_t eventLength;
} SelectState;

static CoreAPI *api;

static GlobalTypeLink *charArrayGT = NULL;

#define MAX_ADDR 64
INSTRUCTION_DEF op_udp_bind(FrameData* cframe)
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
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	
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
	
	int master = 0;
	
	bool addressOK = true;
	
	if (getaddrinfo(addr, portstr, &hints, &adr_result) != 0)
		{
		#ifdef WINDOWS
		printf(" - UDPlib::Bind::Address construction error on %s:%s [%s]", addr, portstr, getSocketError(WSAGetLastError()));
		#endif
		addressOK = false;
		}
	
	if (addressOK)
		{
		master = socket(adr_result->ai_family, adr_result->ai_socktype, adr_result->ai_protocol);

		if (master < 0 && any_all)
			{
			addressOK = true;
			
			//attempt an IPv4 ANY instead, since the general ANY is actually an IPv6-based one
			memset(&hints, '\0', sizeof(hints));
			
			hints.ai_socktype = SOCK_DGRAM;
			hints.ai_protocol = IPPROTO_UDP;
			
			hints.ai_family = AF_INET;
			hints.ai_flags |= AI_PASSIVE;
			free(addr);
			addr = strdup("0.0.0.0");
			
			if (getaddrinfo(addr, portstr, &hints, &adr_result) != 0)
				{
				addressOK = false;
				}
			
			master = socket(adr_result -> ai_family, adr_result -> ai_socktype, adr_result -> ai_protocol);
			
			any_all = false;
			}
		
		if (any_all)
			{
			int no = 0;     
			setsockopt(master, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&no, sizeof(no));
			}
			else if (any_v6)
			{
			int yes = 1;
			setsockopt(master, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&yes, sizeof(yes));
			}
		
		if (bind(master, adr_result->ai_addr, (int)adr_result->ai_addrlen) < 0)
			{
			/*
			#ifdef WINDOWS
			api -> debugOutput(callingThread, "TCPlib::Bind error on %s:%i [%s]", inet_ntoa(host.sin_addr), port, getSocketError(WSAGetLastError()).c_str());
			#endif
			#ifdef LINUX
			api -> debugOutput(callingThread, "TCPlib::Bind error on %s:%i [%s]", inet_ntoa(host.sin_addr), port, strerror(errno));
			#endif
			*/
			
			#ifdef WINDOWS
			printf(" - UDPlib::Bind error on %s:%s [%s]", addr, portstr, getSocketError(WSAGetLastError()));
			#endif
			
			master = 0;
			}
		
		freeaddrinfo(adr_result);
		}
	
	size_t xs = master;
	
	api -> returnRaw(cframe, (unsigned char*) &xs, sizeof(size_t));
	
	free(addr);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_udp_unbind(FrameData* cframe)
	{
	size_t xs = 0;
	memcpy(&xs, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	int master = xs;
	
	#ifdef WINDOWS
	closesocket(master);
	#endif
	#ifdef LINUX
	close(master);
	#endif
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_blocking(FrameData* cframe)
	{
	size_t xs;
	memcpy(&xs, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	#ifdef WINDOWS
	int socket = xs;
	u_long on = 1;
	ioctlsocket(socket, FIONBIO, &on);
	#endif
	#ifdef LINUX
	int socket = xs;

	int flags, s;

	flags = fcntl (socket, F_GETFL, 0);
	if (flags == -1)
		{
		//TODO: error
		}

	flags |= O_NONBLOCK;
	s = fcntl (socket, F_SETFL, flags);
	if (s == -1)
		{
		//TODO: error
		}
	#endif
    
	return RETURN_OK;
	}

#define BUF_LEN 4096

INSTRUCTION_DEF op_udp_recv(FrameData *cframe)
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
	size_t alen = sizeof(ipstr);
	memset(ipstr, 0, sizeof(ipstr));
	
	unsigned char buf[BUF_LEN];
	
	struct sockaddr_storage addr;
	
	#ifdef WINDOWS
	int addr_len;
	#endif
	#ifdef LINUX
	socklen_t addr_len;
	#endif
	
	addr_len = sizeof(addr);
	
	int ssz = recvfrom(socket, (char*) buf, BUF_LEN, 0, (struct sockaddr*) &addr, &addr_len);
	unsigned int sz = ssz > 0 ? ssz : 0;
	
	unsigned char ok = sz > 0;
	
	#ifdef WINDOWS
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
	#endif
	
	//Datagram.address
	
	DanaEl* rdata = api -> getParamEl(cframe, 1);
	
	unsigned char* cnt = NULL;
	DanaEl* newArray = api -> makeArray(charArrayGT, alen, &cnt);
	memcpy(cnt, ipstr, alen);
	
	api -> setDataFieldEl(rdata, 0, newArray);
	
	//Datagram.port
	
	api -> setDataFieldInt(rdata, 1, port);
	
	//Datagram.content
	
	DanaEl* newContentArray = api -> makeArray(charArrayGT, sz, &cnt);
	memcpy(cnt, buf, sz);
	
	api -> setDataFieldEl(rdata, 2, newContentArray);
	
	api -> returnRaw(cframe, &ok, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_udp_send(FrameData* cframe)
	{
	unsigned char ok = 0;
	
	char *addr = x_getParam_char_array(api, cframe, 0);
	
	size_t port = api -> getParamInt(cframe, 1);
	
	DanaEl* contentArray = api -> getParamEl(cframe, 2);
	
	size_t alen = api -> getArrayLength(contentArray);
	
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
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_protocol = IPPROTO_UDP;
	
	int client = 0;
	
	if (getaddrinfo(addr, portstr, &hints, &adr_result) != 0)
		{
		#ifdef WINDOWS
		printf(" - UDPlib::Bind::Address construction error on %s:%s [%s]", addr, portstr, getSocketError(WSAGetLastError()));
		#endif
		}
		else
		{
		client = socket(adr_result->ai_family, adr_result->ai_socktype, adr_result->ai_protocol);
		unsigned int amt = 0;
		
		if (contentArray != NULL)
			amt = sendto(client, (char*) api -> getArrayContent(contentArray), alen, 0, adr_result->ai_addr, (int)adr_result->ai_addrlen);
		
		ok = amt == alen;
		
		freeaddrinfo(adr_result);
		}
	
	#ifdef WINDOWS
	closesocket(client);
	#endif
	
	#ifdef LINUX
	close(client);
	#endif
	
	//printf("OK: %u\n", ok);
	
	api -> returnRaw(cframe, (unsigned char*) &ok, 1);
	
	free(addr);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_create_select(FrameData* cframe)
	{
	size_t evlen = api -> getParamInt(cframe, 0);
	SelectState *state = malloc(sizeof(SelectState));

	#ifndef OSX
	state -> fd = epoll_create1(0);
	state -> events = malloc(sizeof(struct epoll_event) * evlen);
	state -> eventLength = evlen;
	#endif
	#ifdef OSX
	state -> fd = kqueue();
	state -> events = malloc(sizeof(struct kevent) * evlen);
	state -> eventLength = evlen;
	#endif

	api -> returnRaw(cframe, (unsigned char*) &state, sizeof(size_t));

	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_event_array_length(FrameData* cframe)
	{
	SelectState *state;
	memcpy(&state, api -> getParamRaw(cframe, 0), sizeof(size_t));

	size_t evlen = api -> getParamInt(cframe, 1);

	#ifndef OSX
	state -> events = realloc(state -> events, sizeof(struct epoll_event) * evlen);
	#endif
	#ifdef OSX
	state -> events = realloc(state -> events, sizeof(struct kevent) * evlen);
	#endif
	state -> eventLength = evlen;

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_socket(FrameData* cframe)
	{
	size_t xs = 0;
	SelectState *state;
	memcpy(&state, api -> getParamRaw(cframe, 0), sizeof(size_t));

	memcpy(&xs, api -> getParamRaw(cframe, 1), sizeof(size_t));
	int socketFD = xs;

	DanaEl* optDataRef = api -> getParamEl(cframe, 2);

	unsigned char ok = 0;

	#ifndef OSX
	struct epoll_event event;
	event.data.ptr = optDataRef;
	event.events = EPOLLIN;
	int s = epoll_ctl(state -> fd, EPOLL_CTL_ADD, socketFD, &event);
	if (s == -1)
		{
		//TODO: error
		}
	#endif
	#ifdef OSX
	struct kevent eventUpdate;
	EV_SET(&eventUpdate, socketFD, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, 0);
	eventUpdate.udata = optDataRef;
	if (kevent(state -> fd, &eventUpdate, 1, NULL, 0, NULL) == -1)
		{
		//TODO: error
		}
	#endif
	
	ok = 1;

	api -> returnRaw(cframe, &ok, 1);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_rem_socket(FrameData* cframe)
	{
	size_t xs = 0;
	SelectState *state;
	memcpy(&state, api -> getParamRaw(cframe, 0), sizeof(size_t));

	memcpy(&xs, api -> getParamRaw(cframe, 1), sizeof(size_t));
	int socketFD = xs;

	#ifndef OSX
	struct epoll_event event; //this parameter is unused, but old versions on Linux may crash if the parameter is NULL
	int s = epoll_ctl(state -> fd, EPOLL_CTL_DEL, socketFD, &event);
	if (s == -1)
		{
		//TODO: error
		}
	#endif
	#ifdef OSX
	struct kevent eventUpdate;
	EV_SET(&eventUpdate, socketFD, 0, EV_DELETE, 0, 0, 0);
	if (kevent(state -> fd, &eventUpdate, 1, NULL, 0, NULL) == -1)
		{
		//TODO: error
		}
	#endif

	return RETURN_OK;
	}

INSTRUCTION_DEF op_wait(FrameData* cframe)
	{
	SelectState *state;
	memcpy(&state, api -> getParamRaw(cframe, 0), sizeof(size_t));

	DanaEl* array = api -> getParamEl(cframe, 1);
	
	#ifndef OSX
	int n = epoll_wait(state -> fd, state -> events, state -> eventLength, -1);
	#endif
	#ifdef OSX
	int n = kevent(state -> fd, NULL, 0, state -> events, state -> eventLength, NULL);
	#endif

	int i = 0;
	for (i = 0; i < n; i++)
		{
		#ifndef OSX
		DanaEl* mdata = state -> events[i].data.ptr;
		#endif
		#ifdef OSX
		DanaEl* mdata = state -> events[i].udata;
		#endif
		DanaEl* mevent = api -> getDataFieldEl(mdata, 1);
		api -> setArrayCellEl(array, i, mevent);

		unsigned char code = 0;
		api -> setDataFieldRaw(mevent, 2, &code, 1);
		code = 1;
		#ifndef OSX
		if (state -> events[i].events & EPOLLIN)
		#endif
		#ifdef OSX
		if (state -> events[i].filter & EVFILT_READ)
		#endif
			{
			api -> setDataFieldRaw(mevent, 2, &code, 1);
			}
		}

	api -> returnInt(cframe, n);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_wait_time(FrameData* cframe)
	{
	SelectState *state;
	memcpy(&state, api -> getParamRaw(cframe, 0), sizeof(size_t));

	DanaEl* array = api -> getParamEl(cframe, 1);

	size_t xs = api -> getParamInt(cframe, 2);

	#ifndef OSX
	int n = epoll_wait(state -> fd, state -> events, state -> eventLength, xs);
	#endif
	#ifdef OSX
	struct timespec timeout;
	memset(&timeout, 0, sizeof(timeout));
	timeout.tv_nsec = xs * 1000 * 1000;
	int n = kevent(state -> fd, NULL, 0, state -> events, state -> eventLength, &timeout);
	#endif

	int i = 0;
	for (i = 0; i < n; i++)
		{
		#ifndef OSX
		DanaEl* mdata = state -> events[i].data.ptr;
		#endif
		#ifdef OSX
		DanaEl* mdata = state -> events[i].udata;
		#endif
		DanaEl* mevent = api -> getDataFieldEl(mdata, 1);
		api -> setArrayCellEl(array, i, mevent);

		unsigned char code = 0;
		api -> setDataFieldRaw(mevent, 2, &code, 1);
		code = 1;
		#ifndef OSX
		if (state -> events[i].events & EPOLLIN)
		#endif
		#ifdef OSX
		if (state -> events[i].filter & EVFILT_READ)
		#endif
			{
			api -> setDataFieldRaw(mevent, 2, &code, 1);
			}
		}
	
	api -> returnInt(cframe, n);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_destroy_select(FrameData* cframe)
	{
	SelectState *state;
	memcpy(&state, api -> getParamRaw(cframe, 0), sizeof(size_t));

	#ifdef WINDOWS
	epoll_close(state -> fd);
	#endif
	#ifdef LINUX
	close(state -> fd);
	#endif
	free(state -> events);
	free(state);

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
	
	setInterfaceFunction("send", op_udp_send);
	setInterfaceFunction("recv", op_udp_recv);
	setInterfaceFunction("bind", op_udp_bind);
	setInterfaceFunction("unbind", op_udp_unbind);
	setInterfaceFunction("setBlocking", op_set_blocking);

	setInterfaceFunction("createSelect", op_create_select);
	setInterfaceFunction("setEventArrayLength", op_set_event_array_length);
	setInterfaceFunction("addSocket", op_add_socket);
	setInterfaceFunction("remSocket", op_rem_socket);
	setInterfaceFunction("wait", op_wait);
	setInterfaceFunction("waitTime", op_wait_time);
	setInterfaceFunction("destroySelect", op_destroy_select);
	
	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(charArrayGT);
	}
