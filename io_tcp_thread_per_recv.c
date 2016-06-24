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

/*
the below approach is not currently usable because we can never be sure when it's OK to free a DanaSocket
 - i.e. a recv() call can come in at any time and then want to query the status of the socket, which may have been freed by a disconnect thread

typedef struct{
	#ifdef WINDOWS
	SOCKET socket;
	#endif
	#ifdef LINUX
	int socket;
	#endif
	#ifdef WINDOWS
	HANDLE lock;
	#endif
	#ifdef LINUX
	sem_t lock;
	#endif
	size_t refCount;
	bool disconnected;
	} DanaSocket;
*/

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

/*
The close queue, and associated semaphores, are used to guarantee that a socket that's been opened can definitely be closed (i.e. guarantee that there's enough remaining memory / system resources to do this). There is a single "socket close" thread which works through the queue of ready-to-close sockets. A possible disadvantage of this approach is that queued sockets may take a while to close while they wait for earlier sockets in the queue to finish closing. This could potentially be solved by using a pool of socket close threads.
*/

typedef struct __ci{
	#ifdef WINDOWS
	SOCKET socket;
	#endif
	#ifdef LINUX
	int socket;
	#endif
	VFrame *frame;
	
	struct __ci *next;
	} CloseItem;

#ifdef WINDOWS
#include <Windows.h>
HANDLE queueAccessLock;
HANDLE queueSignal;
#endif

#ifdef OSX
dispatch_semaphore_t queueAccessLock;
dispatch_semaphore_t queueSignal;
#else
#ifdef LINUX
#include <semaphore.h>
sem_t queueAccessLock;
sem_t queueSignal;
#endif
#endif

static CloseItem *closeQueue;
static CloseItem *closeQueueEnd;
static CloseItem *closeQueueFree;

static bool closeQueueShutdown = false;

static void signalCloseQueue()
	{
	#ifdef WINDOWS
	ReleaseSemaphore(queueSignal, 1, NULL);
	#endif
    #ifdef OSX
    dispatch_semaphore_signal(queueSignal);
    #else
	#ifdef LINUX
	sem_post(&queueSignal);
	#endif
    #endif
	}

static void enqueueCloseSocket(VFrame *f, int socket)
	{
	#ifdef WINDOWS
	WaitForSingleObject(queueAccessLock, INFINITE);
	#endif
    #ifdef OSX
    dispatch_semaphore_wait(queueAccessLock, DISPATCH_TIME_FOREVER);
    #else
	#ifdef LINUX
	sem_wait(&queueAccessLock);
	#endif
    #endif
	
	closeQueueFree -> frame = f;
	closeQueueFree -> socket = socket;
	closeQueueFree = closeQueueFree -> next;
	
	#ifdef WINDOWS
	ReleaseSemaphore(queueAccessLock, 1, NULL);
	#endif
    #ifdef OSX
    dispatch_semaphore_signal(queueAccessLock);
    #else
	#ifdef LINUX
	sem_post(&queueAccessLock);
	#endif
    #endif
	
	//signal semaphore
	signalCloseQueue();
	}

static bool addCloseQueueItem()
	{
	CloseItem *nci = malloc(sizeof(CloseItem));
	
	if (nci == NULL) return false;
	
	memset(nci, '\0', sizeof(CloseItem));
	
	#ifdef WINDOWS
	WaitForSingleObject(queueAccessLock, INFINITE);
	#endif
    #ifdef OSX
    dispatch_semaphore_wait(queueAccessLock, DISPATCH_TIME_FOREVER);
    #else
	#ifdef LINUX
	sem_wait(&queueAccessLock);
	#endif
    #endif
	
	if (closeQueue == NULL)
		{
		closeQueue = nci;
		}
		else
		{
		closeQueueEnd -> next = nci;
		}
	closeQueueEnd = nci;
	
	if (closeQueueFree == NULL)
		closeQueueFree = nci;
	
	#ifdef WINDOWS
	ReleaseSemaphore(queueAccessLock, 1, NULL);
	#endif
    #ifdef OSX
    dispatch_semaphore_signal(queueAccessLock);
    #else
	#ifdef LINUX
	sem_post(&queueAccessLock);
	#endif
    #endif
	
	return true;
	}

static CloseItem* getCloseQueueItem()
	{
	#ifdef WINDOWS
	WaitForSingleObject(queueAccessLock, INFINITE);
	#endif
    #ifdef OSX
    dispatch_semaphore_wait(queueAccessLock, DISPATCH_TIME_FOREVER);
    #else
	#ifdef LINUX
	sem_wait(&queueAccessLock);
	#endif
    #endif
	
	CloseItem *cs = closeQueue;
	closeQueue = cs -> next;
	if (closeQueueEnd == cs)
		closeQueueEnd = closeQueue;
	if (closeQueueFree == cs)
		closeQueueFree = closeQueue;
	
	#ifdef WINDOWS
	ReleaseSemaphore(queueAccessLock, 1, NULL);
	#endif
    #ifdef OSX
    dispatch_semaphore_signal(queueAccessLock);
    #else
	#ifdef LINUX
	sem_post(&queueAccessLock);
	#endif
    #endif
	
	return cs;
	}

static void waitForCloseQueue()
	{
	#ifdef WINDOWS
	WaitForSingleObject(queueSignal, INFINITE);
	#endif
    #ifdef OSX
    dispatch_semaphore_wait(queueSignal, DISPATCH_TIME_FOREVER);
    #else
	#ifdef LINUX
	sem_wait(&queueSignal);
	#endif
    #endif
	}

#define MAX_ADDR 64
INSTRUCTION_DEF op_tcp_bind(INSTRUCTION_PARAM_LIST)
	{
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 0)) -> content;
	
	char *addr = NULL;
	
	if (array != NULL)
		{
		addr = malloc(array -> length + 1);
		memset(addr, '\0', array -> length + 1);
		memcpy(addr, array -> data, array -> length);
		}
		else
		{
		addr = strdup("");
		}
	
	size_t port = 0;
	copyHostInteger((unsigned char*) &port, getVariableContent(cframe, 1), sizeof(size_t));
	
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
		//printf(" - TCPlib::Bind::Address construction error on %s:%s [%s]", addr, portstr, getSocketError(WSAGetLastError()));
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
			connected = false;
			}
		
		if (connected)
			{
			if (listen(newSocket, MAX_PENDING) < 0)
				{
				#ifdef WINDOWS
				//printf(" - TCPlib::Listen error on %s:%s [%s]\n", addr, portstr, getSocketError(WSAGetLastError()));
				#endif
				
				connected = false;
				}
			}
		
		freeaddrinfo(adr_result);
		}
	
	size_t xs = connected ? newSocket : 0;
	
	//the return value is written to local variable 0
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset];
	memcpy(result, &xs, sizeof(size_t));
	
	free(addr);
	
	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_tcp_unbind(INSTRUCTION_PARAM_LIST)
	{
	/*
	int master = 0;
	memcpy(&master, registers[0].PR.content, sizeof(unsigned int));
	
	#ifdef WINDOWS
	closesocket(master);
	#endif
	#ifdef LINUX
	close(master);
	#endif
	
	//return void
	api -> prepRegister(returnRegister);
	returnRegister[0].PR.vsize = 0;
	returnRegister[0].type = TYPE_LITERAL;
	returnRegister[0].xtype = X_FLAT;
	*/
	
	return RETURN_DIRECT;
	}

#ifdef WINDOWS
DWORD WINAPI connect_thread( LPVOID ptr ) 
#else
static void * connect_thread(void *ptr)
#endif
	{
	#ifdef LINUX
	pthread_detach(pthread_self());
	#endif
	
	VFrame *cframe = (VFrame*) ptr;
	
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 0)) -> content;
	
	char *addr = NULL;
	
	if (array != NULL)
		{
		addr = malloc(array -> length + 1);
		memset(addr, '\0', array -> length + 1);
		memcpy(addr, array -> data, array -> length);
		}
		else
		{
		addr = strdup("");
		}
	
	size_t port = 0;
	copyHostInteger((unsigned char*) &port, getVariableContent(cframe, 1), sizeof(size_t));
	
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
		connected = true;
		
		if (connect(newSocket, adr_result->ai_addr, (int) adr_result->ai_addrlen) < 0)
			{
			//printf("TCPlib::Connect error to %s:%i [%s]\n", addr, port, getSocketError(WSAGetLastError()));
			#ifdef WINDOWS
			closesocket(newSocket);
			#endif
			
			#ifdef LINUX
			close(newSocket);
			#endif
			
			connected = false;
			}
		
		freeaddrinfo(adr_result);
		}
	
	size_t xs = connected ? newSocket : 0;
	
	//the return value is written to local variable 0
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset];
	memcpy(result, &xs, sizeof(size_t));
	
	free(addr);
	
	api -> deferredReturn(cframe);
	
	#ifdef WINDOWS
	return 0;
	#else
	return NULL;
	#endif
	}

INSTRUCTION_DEF op_tcp_connect(INSTRUCTION_PARAM_LIST)
	{
	if (! addCloseQueueItem())
		{
		size_t totalAmt = 0;
		size_t *result = (size_t*) &cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset];
		copyHostInteger((unsigned char*) result, (unsigned char*) &totalAmt, sizeof(size_t));
		
		printf("CRITICAL: out of memory in TCP::connect\n");
		
		return RETURN_DIRECT;
		}
	
	#ifdef WINDOWS
	HANDLE th = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            connect_thread,  		     // thread function name
            cframe,          // argument to thread function 
            0,                      // use default creation flags 
            NULL);   // returns the thread identifier
	
	CloseHandle(th);
	#else
	int err = 0;
	pthread_t th;
	memset(&th, '\0', sizeof(pthread_t));
	
	if ((err = pthread_create(&th, NULL, connect_thread, cframe)) != 0)
		{
		size_t totalAmt = 0;
		size_t *result = (size_t*) &cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset];
		copyHostInteger((unsigned char*) result, (unsigned char*) &totalAmt, sizeof(size_t));
		
		printf("CRITICAL: thread launch failure in TCP::connect [%s]\n", getThreadError(err));
		
		return RETURN_DIRECT;
		}
	#endif
	
	return RETURN_DEFERRED;
	}

#ifdef WINDOWS
DWORD WINAPI disconnect_thread( LPVOID ptr ) 
#else
static void * disconnect_thread(void *ptr)
#endif
	{
	while (true)
		{
		waitForCloseQueue();
        
		if (closeQueueShutdown) break;
		
		//we've been signalled, so close the next socket on the list
		CloseItem *cs = getCloseQueueItem();
		
		VFrame *cframe = cs -> frame;
		int socket = cs -> socket;
		
		#ifdef WINDOWS
		//#(X-SOC-MS-1)
		if (socket != 0)
			{
			GUID guidDisconnectEx = WSAID_DISCONNECTEX;
			DWORD bytesret = 0;
			LPFN_DISCONNECTEX disconn;
			WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guidDisconnectEx, sizeof(guidDisconnectEx), &disconn, sizeof(disconn), &bytesret, NULL, NULL);
			if (bytesret == sizeof(disconn)) disconn(socket, NULL, 0, 0); else printf("CRITICAL SOCKET ERROR\n");
			}
		closesocket(socket);
		#endif
		#ifdef LINUX
		shutdown(socket, SHUT_RDWR);
		//NOTE: do we now need to read any/all pending data on the socket?
		// - http://blog.netherlabs.nl/articles/2009/01/18/the-ultimate-so_linger-page-or-why-is-my-tcp-not-reliable
		close(socket);
		#endif
		
		free(cs);
		
		api -> deferredReturn(cframe);
		}
	
	#ifdef WINDOWS
	return 0;
	#else
	return NULL;
	#endif
	}

INSTRUCTION_DEF op_tcp_disconnect(INSTRUCTION_PARAM_LIST)
	{
	size_t xs;
	memcpy(&xs, getVariableContent(cframe, 0), sizeof(size_t));
	
	int socket = xs;
	
	enqueueCloseSocket(cframe, socket);
    
	return RETURN_DEFERRED;
	}

#ifdef WINDOWS
DWORD WINAPI accept_thread( LPVOID ptr ) 
#else
static void * accept_thread(void *ptr)
#endif
	{
	#ifdef LINUX
	pthread_detach(pthread_self());
	#endif
	
	VFrame *cframe = (VFrame*) ptr;
	
	size_t xs;
	memcpy(&xs, getVariableContent(cframe, 0), sizeof(size_t));
	
	int masterSocket = xs;
	
	int socket = 0;
	
	if ((socket = accept(masterSocket, NULL, NULL)) < 0)
		{
		#ifdef WINDOWS
		//printf("TCPlib::Accept error [%s]\n", getSocketError(WSAGetLastError()));
		#endif
		
		#ifdef LINUX
		//printf("TCPlib::Accept error [%s]\n", strerror(errno));
		#endif
		}
	
	xs = socket;
	
	//the return value is written to local variable 0
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset];
	memcpy(result, &xs, sizeof(size_t));
	
	api -> deferredReturn(cframe);
	
	#ifdef WINDOWS
	return 0;
	#else
	return NULL;
	#endif
	}

INSTRUCTION_DEF op_tcp_accept(INSTRUCTION_PARAM_LIST)
	{
	if (! addCloseQueueItem())
		{
		size_t totalAmt = 0;
		size_t *result = (size_t*) &cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset];
		copyHostInteger((unsigned char*) result, (unsigned char*) &totalAmt, sizeof(size_t));
		
		printf("CRITICAL: out of memory TCP::accept\n");
		
		return RETURN_DIRECT;
		}
	
	#ifdef WINDOWS
	HANDLE th = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            accept_thread,  		     // thread function name
            cframe,          // argument to thread function 
            0,                      // use default creation flags 
            NULL);   // returns the thread identifier
	
	CloseHandle(th);
	#else
	int err = 0;
	pthread_t th;
	memset(&th, '\0', sizeof(pthread_t));
	
	if ((err = pthread_create(&th, NULL, accept_thread, cframe)) != 0)
		{
		size_t totalAmt = 0;
		size_t *result = (size_t*) &cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset];
		copyHostInteger((unsigned char*) result, (unsigned char*) &totalAmt, sizeof(size_t));
		
		printf("CRITICAL: thread launch failure in TCP::accept [%s]\n", getThreadError(err));
		
		return RETURN_DIRECT;
		}
	#endif
	
	return RETURN_DEFERRED;
	}

#define BUF_LEN 128

#ifdef WINDOWS
DWORD WINAPI recv_thread( LPVOID ptr ) 
#else
static void * recv_thread(void *ptr)
#endif
	{
	#ifdef LINUX
	pthread_detach(pthread_self());
	#endif
	
	VFrame *cframe = (VFrame*) ptr;
	
	Component *dataOwner = ((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 2)) -> content) -> owner;
	
	size_t xs = 0;
	memcpy(&xs, getVariableContent(cframe, 0), sizeof(size_t));
	
	int socket = xs;
	
	size_t len = 0;
	copyHostInteger((unsigned char*) &len, getVariableContent(cframe, 1), sizeof(size_t));
	
	unsigned char *pbuf = malloc(len);
	
	int amt = 1;
	size_t totalAmt = 0;
	
	//iterate through param 2's contents
	while ((len > 0) && (amt != 0))
		{
		amt = recv(socket, (char*) (pbuf+totalAmt), len, 0);
		
		if (amt < 0)
			{
			//#ifdef WINDOWS
			//printf("TCPlib::recv error [%s]\n", getSocketError(WSAGetLastError()));
			//#endif
			break;
			}
		
		totalAmt += amt;
		len -= amt;
		}
	
	if (totalAmt > 0)
		{
		LiveArray *newArray = malloc(sizeof(LiveArray));
		memset(newArray, '\0', sizeof(LiveArray));
		
		newArray -> data = pbuf;
		newArray -> length = totalAmt;
		
		newArray -> gtLink = charArrayGT;
		api -> incrementGTRefCount(newArray -> gtLink);
		newArray -> owner = dataOwner;
		
		VVarLivePTR *ptrh = (VVarLivePTR*) ((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 2)) -> content) -> data;
		
		ptrh -> content = (unsigned char*) newArray;
		attachPointer(ptrh, &newArray -> scope.scopePointers);
		newArray -> refCount ++;
		((VVarLivePTR*) ((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 2)) -> content) -> data) -> typeLink = newArray -> gtLink -> typeLink;
		}
		else
		{
		free(pbuf);
		}
	
	//the return value is written to local variable 0
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset];
	copyHostInteger((unsigned char*) result, (unsigned char*) &totalAmt, sizeof(size_t));
	
	api -> deferredReturn(cframe);
	
	#ifdef WINDOWS
	return 0;
	#else
	return NULL;
	#endif
	}

INSTRUCTION_DEF op_tcp_recv(INSTRUCTION_PARAM_LIST)
	{
	#ifdef WINDOWS
	HANDLE th = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            recv_thread,  		     // thread function name
            cframe,          // argument to thread function 
            0,                      // use default creation flags 
            NULL);   // returns the thread identifier
	
	CloseHandle(th);
	#else
	int err = 0;
	pthread_t th;
	memset(&th, '\0', sizeof(pthread_t));
	
	if ((err = pthread_create(&th, NULL, recv_thread, cframe)) != 0)
		{
		size_t totalAmt = 0;
		size_t *result = (size_t*) &cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset];
		copyHostInteger((unsigned char*) result, (unsigned char*) &totalAmt, sizeof(size_t));
		
		printf("CRITICAL: thread launch failure in TCP::recv [%s]\n", getThreadError(err));
		
		return RETURN_DIRECT;
		}
	#endif
	
	return RETURN_DEFERRED;
	}

#ifdef WINDOWS
DWORD WINAPI send_thread( LPVOID ptr ) 
#else
static void * send_thread(void *ptr)
#endif
	{
	#ifdef LINUX
	pthread_detach(pthread_self());
	#endif
	
	VFrame *cframe = (VFrame*) ptr;
	
	size_t xs = 0;
	memcpy(&xs, getVariableContent(cframe, 0), sizeof(size_t));
	
	int socket = xs;
	
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
	
	size_t totalAmt = 0;
	
	if (array != NULL)
		totalAmt = send(socket, (char*) array -> data, array -> length, 0);
	
	//the return value is written to local variable 0
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset];
	copyHostInteger((unsigned char*) result, (unsigned char*) &totalAmt, sizeof(size_t));
	
	api -> deferredReturn(cframe);
	
	#ifdef WINDOWS
	return 0;
	#else
	return NULL;
	#endif
	}

INSTRUCTION_DEF op_tcp_send(INSTRUCTION_PARAM_LIST)
	{
	#ifdef WINDOWS
	HANDLE th = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            send_thread,  		     // thread function name
            cframe,          // argument to thread function 
            0,                      // use default creation flags 
            NULL);   // returns the thread identifier
	
	CloseHandle(th);
	#else
	int err = 0;
	pthread_t th;
	memset(&th, '\0', sizeof(pthread_t));
	
	if ((err = pthread_create(&th, NULL, send_thread, cframe)) != 0)
		{
		size_t totalAmt = 0;
		size_t *result = (size_t*) &cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset];
		copyHostInteger((unsigned char*) result, (unsigned char*) &totalAmt, sizeof(size_t));
		
		printf("CRITICAL: thread launch failure in TCP::send\n");
		
		return RETURN_DIRECT;
		}
	#endif
	
	return RETURN_DEFERRED;
	}

INSTRUCTION_DEF op_tcp_get_local_address(INSTRUCTION_PARAM_LIST)
	{
	size_t xs = 0;
	memcpy(&xs, getVariableContent(cframe, 0), sizeof(size_t));
	
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
	
	LiveArray *newArray = malloc(sizeof(LiveArray));
	memset(newArray, '\0', sizeof(LiveArray));
	
	newArray -> data = malloc(alen);
	newArray -> length = alen;
	memcpy(newArray -> data, ipstr, alen);
	
	newArray -> gtLink = charArrayGT;
	api -> incrementGTRefCount(newArray -> gtLink);
	newArray -> owner = cframe -> blocking -> instance;
	
	VVarLivePTR *ptrh = (VVarLivePTR*) ((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content) -> data;
	
	ptrh -> content = (unsigned char*) newArray;
	attachPointer(ptrh, &newArray -> scope.scopePointers);
	newArray -> refCount ++;
	ptrh -> typeLink = newArray -> gtLink -> typeLink;
	
	xs = port;

	unsigned int *pi = (unsigned int*) (((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content) -> data + sizeof(VVarLivePTR));
	copyHostInteger((unsigned char*) pi, (unsigned char*) &xs, sizeof(xs));
	
	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_tcp_get_remote_address(INSTRUCTION_PARAM_LIST)
	{
	size_t xs = 0;
	memcpy(&xs, getVariableContent(cframe, 0), sizeof(size_t));
	
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
	
	LiveArray *newArray = malloc(sizeof(LiveArray));
	memset(newArray, '\0', sizeof(LiveArray));
	
	newArray -> data = malloc(alen);
	newArray -> length = alen;
	memcpy(newArray -> data, ipstr, alen);
	
	newArray -> gtLink = charArrayGT;
	api -> incrementGTRefCount(newArray -> gtLink);
	newArray -> owner = cframe -> blocking -> instance;
	
	VVarLivePTR *ptrh = (VVarLivePTR*) ((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content) -> data;
	
	ptrh -> content = (unsigned char*) newArray;
	attachPointer(ptrh, &newArray -> scope.scopePointers);
	newArray -> refCount ++;
	ptrh -> typeLink = newArray -> gtLink -> typeLink;
	
	xs = port;

	unsigned int *pi = (unsigned int*) (((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content) -> data + sizeof(VVarLivePTR));
	copyHostInteger((unsigned char*) pi, (unsigned char*) &xs, sizeof(xs));
	
	return RETURN_DIRECT;
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
	api -> incrementGTRefCount(charArrayGT);
	
	setInterfaceFunction("connect", op_tcp_connect);
	setInterfaceFunction("disconnect", op_tcp_disconnect);
	setInterfaceFunction("send", op_tcp_send);
	setInterfaceFunction("recv", op_tcp_recv);
	
	setInterfaceFunction("accept", op_tcp_accept);
	setInterfaceFunction("bind", op_tcp_bind);
	setInterfaceFunction("unbind", op_tcp_unbind);
	
	setInterfaceFunction("getLocalAddress", op_tcp_get_local_address);
	setInterfaceFunction("getRemoteAddress", op_tcp_get_remote_address);
	
	// semaphore setup
	#ifdef WINDOWS
	queueAccessLock = CreateSemaphore(NULL, 1, LONG_MAX, NULL);
	queueSignal = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
	#endif
    #ifdef OSX
    dispatch_semaphore_t *sem;
    sem = &queueAccessLock;
    *sem = dispatch_semaphore_create(1);
    sem = &queueSignal;
    *sem = dispatch_semaphore_create(0);
    #else
	#ifdef LINUX
	sem_init(&queueAccessLock, 0, 1);
	sem_init(&queueSignal, 0, 0);
	#endif
    #endif
	
	#ifdef WINDOWS
	HANDLE th = CreateThread( 
            NULL,                   // default security attributes
            0,                      // use default stack size  
            disconnect_thread,  		     // thread function name
            NULL,          // argument to thread function 
            0,                      // use default creation flags 
            NULL);   // returns the thread identifier
	
	CloseHandle(th);
	#else
	int err = 0;
	pthread_t th;
	memset(&th, '\0', sizeof(pthread_t));
	
	if ((err = pthread_create(&th, NULL, disconnect_thread, NULL)) != 0)
		{
		}
	#endif
	
	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(charArrayGT);
	}