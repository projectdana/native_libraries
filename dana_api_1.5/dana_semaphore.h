#ifndef _DANA_SEMAPHORE_
#define _DANA_SEMAPHORE_

#ifdef WINDOWS
#include <Windows.h>
#endif

#ifdef LINUX
#include <semaphore.h>
#endif

#ifdef OSX
#include <dispatch/dispatch.h>
#endif

typedef struct _pi_semaphore{
	#ifdef WINDOWS
	#include <Windows.h>
	HANDLE sem;
	#endif
	#ifdef OSX
	dispatch_semaphore_t sem;
	#else
	#ifdef LINUX
	sem_t sem;
	#endif
	#endif
	} Semaphore;

void xsemaphore_init(Semaphore *s, unsigned int initialValue);
int xsemaphore_wait(Semaphore *s);
void xsemaphore_post(Semaphore *s);
void xsemaphore_destroy(Semaphore *s);

#endif
