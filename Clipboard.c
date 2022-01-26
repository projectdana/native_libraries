#include "dana_lib_defs.h"
#include "nli_util.h"
#include "vmi_util.h"

#ifdef WINDOWS
#include <Windows.h>
#endif

#ifdef LINUX
//interacting with the clipboard on Linux via native x11 is astoundingly complicated
// - the implementation here is not ideal, but mostly works...(improvements welcome...)
// - note that, if our Dana program exits, on Linux the clipboard content is no longer available as it resides at the application
#ifndef OSX
	#include <X11/Xlib.h>
	#include <X11/Xatom.h>
	#include <limits.h>
	#include <pthread.h>
#endif
#endif

#ifdef OSX
#include <ApplicationServices/ApplicationServices.h>
#endif

#include <sys/stat.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//reference material:
//https://stackoverflow.com/questions/27378318/c-get-string-from-clipboard-on-linux
//https://github.com/bstpierre/gtk-examples/blob/master/c/clipboard_simple.c
//https://stackoverflow.com/questions/1868734/how-to-copy-to-clipboard-with-x11 (OS X)
//https://jtanx.github.io/2016/08/19/a-cross-platform-clipboard-library/
//http://www.idevgames.com/forums/thread-463.html
//https://developer.apple.com/documentation/applicationservices/applicationservices_functions?language=objc
// https://developer.apple.com/documentation/applicationservices/1458917-pasteboardcopyitemflavordata?language=objc
// https://www.uninformativ.de/blog/postings/2017-04-02/0/POSTING-en.html
// http://www.atmosp.physics.utoronto.ca/internal/SX6/g1ae02e/chap4.html
//  -- see XChangeProperty selection, SelectionRequest, XSendEvent

static CoreAPI *api;

static GlobalTypeLink *charArrayGT = NULL;

#ifdef LINUX
#ifndef OSX

Display *display;
unsigned long color;
Window window;

pthread_t threadHandle;

static Atom bufid;
static Atom fmtid;
static Atom incrid;
static Atom propid;
static Atom xtargets;

static bool pendingGet = false;
static char *resultGet = NULL;

static char *setValue = NULL;

pthread_mutexattr_t mAttr;
pthread_mutex_t clipboardLock;
pthread_mutex_t pendingGetLock;

sem_t sem;

XDestroyWindowEvent dwe;

XSelectionEvent sev;

#endif
#endif

#ifdef LINUX
#ifndef OSX
static void* clip_thread(void *ptr)
	{
	XEvent event;

	do
		{
		XNextEvent(display, &event);

		//printf("::event\n");

		if (event.type == SelectionNotify && event.xselection.selection == bufid)
			{
			unsigned long ressize, restail;
			int resbits;
			
			pthread_mutex_lock(&pendingGetLock);
			if (pendingGet)
				{
				if (event.xselection.property)
					{
					XGetWindowProperty(display, window, propid, 0, LONG_MAX/4, False, AnyPropertyType,
					  &fmtid, &resbits, &ressize, &restail, (unsigned char**)&resultGet);

					if (fmtid == incrid)
						{
						//TODO: add support for this! (incremental reading of content in chunks)
						}

					//resume lock

					pendingGet = false;
					sem_post(&sem);
					}
					else // request failed, e.g. owner can't convert to the target format
					{
					//resume lock

					pendingGet = false;
					sem_post(&sem);
					}
				}
			pthread_mutex_unlock(&pendingGetLock);
			}
			else if (event.type == SelectionRequest && setValue != NULL)
			{
			XSelectionRequestEvent *srev = (XSelectionRequestEvent*)&event.xselectionrequest;

			/*
			char *an = XGetAtomName(display, srev->property);
		    printf("Request for '%s'\n", an);
		    if (an != NULL)
		    	{
		        XFree(an);
		        }
		    an = XGetAtomName(display, srev->target);
		    printf("Target '%s'\n", an);
			*/

			if (srev -> target == fmtid)
				{
				//this is requesting utf8 formatted content
				XChangeProperty(display, srev->requestor, srev->property, fmtid, 8, PropModeReplace, (unsigned char *) setValue, strlen(setValue));

				sev.type = SelectionNotify;
			    sev.requestor = srev -> requestor;
			    sev.selection = srev -> selection;
			    sev.target = srev -> target;
			    sev.property = srev -> property;
			    sev.time = srev -> time;

				XSendEvent(display, srev -> requestor, 0, 0, (XEvent*) &sev);
				}
				else if (srev -> target == xtargets)
				{
				//this is a special kind of request which is asking for the set of format types we're able to support
				// - we just reply with our utf8 format
				XChangeProperty(display, srev->requestor, srev->property, XA_ATOM, 32, PropModeReplace, (unsigned char*)&fmtid, 1);

				sev.type = SelectionNotify;
			    sev.requestor = srev -> requestor;
			    sev.selection = srev -> selection;
			    sev.target = srev -> target;
			    sev.property = srev -> property;
			    sev.time = srev -> time;

				XSendEvent(display, srev -> requestor, 0, 0, (XEvent*) &sev);
				}
				else
				{
				//not a format we understand - reply with "none"
				sev.type = SelectionNotify;
			    sev.requestor = srev -> requestor;
			    sev.selection = srev -> selection;
			    sev.target = srev -> target;
			    sev.property = None;
			    sev.time = srev -> time;

				XSendEvent(display, srev -> requestor, 0, 0, (XEvent*) &sev);
				}
			}

		} while (event.type != DestroyNotify);

	return NULL;
	}
#endif
#endif

INSTRUCTION_DEF op_set_content(FrameData* cframe)
	{
	char *vn = x_getParam_char_array(api, cframe, 0);

	#ifdef WINDOWS
	if (OpenClipboard(NULL))
		{
		EmptyClipboard();
		
		size_t slen = strlen(vn);
		
		//ANSI text:
		/*
		HGLOBAL hglbCopy = GlobalAlloc(GMEM_MOVEABLE, ((slen+1) * sizeof(TCHAR)));
		LPTSTR lptstrCopy = GlobalLock(hglbCopy);
        memcpy(lptstrCopy, vn, slen * sizeof(TCHAR));
		lptstrCopy[slen] = (TCHAR) 0; // null character
        GlobalUnlock(hglbCopy);

		SetClipboardData(CF_TEXT, hglbCopy);
		*/
		
		//we assume UTF-8 text as input from Dana, but Windows needs UTF-16, so we convert...
		int reqLen = MultiByteToWideChar(CP_UTF8, 0, vn, slen, NULL, 0);
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, (reqLen + 1) * sizeof(wchar_t));
		wchar_t* buffer = (wchar_t*)GlobalLock(hMem);
		MultiByteToWideChar(CP_UTF8, 0, vn, slen, buffer, reqLen);
		GlobalUnlock(hMem);
		
		SetClipboardData(CF_UNICODETEXT, hMem);
		
		CloseClipboard();
		}
	#endif

	#ifdef LINUX
	#ifndef OSX
	if (display == NULL)
		{
		api -> throwException(cframe, "clipboard unavailable (requires x11)");
		return RETURN_OK;
		}
	
	pthread_mutex_lock(&clipboardLock);

	//pop this content into a global variable and respond to SelectionRequest events...
	setValue = strdup(vn);

	XSetSelectionOwner(display, bufid, window, CurrentTime);
	XFlush(display);

	pthread_mutex_unlock(&clipboardLock);
	#endif
	#endif

	#ifdef OSX
    PasteboardRef clipboard;
    if (PasteboardCreate(kPasteboardClipboard, &clipboard) != noErr) {
        return RETURN_OK;
    }

    if (PasteboardClear(clipboard) != noErr) {
        CFRelease(clipboard);
        return RETURN_OK;
    }

	size_t slen = strlen(vn);

    CFDataRef data = CFDataCreateWithBytesNoCopy(kCFAllocatorDefault, vn, slen+1, kCFAllocatorNull);
    if (data != NULL)
		{
		OSStatus err;
		err = PasteboardPutItemFlavor(clipboard, NULL, kUTTypeUTF8PlainText, data, 0);
		CFRelease(data);
		}

    CFRelease(clipboard);
	#endif

	free(vn);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_content(FrameData* cframe)
	{
	#ifdef WINDOWS
	if (IsClipboardFormatAvailable(CF_UNICODETEXT) && OpenClipboard(NULL))
		{
		HGLOBAL hMem = GetClipboardData(CF_UNICODETEXT);
		if (hMem != NULL)
			{
			wchar_t* buffer = (wchar_t*)GlobalLock(hMem);
			
			if (buffer != NULL)
				{
				int srclen = wcslen(buffer);
				int reqLen = WideCharToMultiByte(CP_UTF8, 0, buffer, srclen, NULL, 0, NULL, NULL);
				
				DanaEl* array = api -> makeArray(charArrayGT, reqLen+1);
				unsigned char* content = api -> getArrayContent(array);
				
				WideCharToMultiByte(CP_UTF8, 0, buffer, srclen, (char*) content, reqLen+1, NULL, NULL);
				
				api -> setArrayLength(array, reqLen);
				api -> returnEl(cframe, array);

				GlobalUnlock(hMem);
				}
			}
		CloseClipboard();
		}
		else if (IsClipboardFormatAvailable(CF_TEXT) && OpenClipboard(NULL))
		{
		HGLOBAL hglb = GetClipboardData(CF_TEXT);
		if (hglb != NULL)
			{
			LPTSTR lptstr = GlobalLock(hglb);
			if (lptstr != NULL)
				{
				DanaEl* array = api -> makeArray(charArrayGT, strlen(lptstr));
				unsigned char* content = api -> getArrayContent(array);
				
				memcpy(content, lptstr, strlen(lptstr));
				
				api -> returnEl(cframe, array);
				
				GlobalUnlock(hglb);
				}
			}
		CloseClipboard();
		}
	#endif
	
	#ifdef LINUX
	#ifndef OSX
	if (display == NULL)
		{
		api -> throwException(cframe, "clipboard unavailable (requires x11)");
		return RETURN_OK;
		}
	
	pthread_mutex_lock(&clipboardLock);

	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);

	//wait for a maximum of 100ms
	ts.tv_nsec += 100000000;
	if (ts.tv_nsec>=1000000000) {
		ts.tv_sec+=1;
		ts.tv_nsec-=1000000000;
	}

	pendingGet = true;

	XConvertSelection(display, bufid, fmtid, propid, window, CurrentTime);
	XFlush(display);

	sem_timedwait(&sem, &ts);
	
	pthread_mutex_lock(&pendingGetLock);

	if (!pendingGet && resultGet != NULL)
		{
		DanaEl* array = api -> makeArray(charArrayGT, strlen(resultGet));
		unsigned char* content = api -> getArrayContent(array);
		memcpy(content, resultGet, strlen(resultGet));
		api -> returnEl(cframe, array);
		
		XFree(resultGet);
		}
	
	pendingGet = false;
	resultGet = NULL;
	
	pthread_mutex_unlock(&pendingGetLock);

	pthread_mutex_unlock(&clipboardLock);
	#endif
	#endif

	#ifdef OSX
	PasteboardRef clipboard;
	if (PasteboardCreate(kPasteboardClipboard, &clipboard) != noErr) {
			return RETURN_OK;
	}

	ItemCount  itemCount;

	PasteboardSynchronize( clipboard );
	PasteboardGetItemCount( clipboard, &itemCount );
	
	if (itemCount > 0)
		{
		size_t itemIndex = 1;

		PasteboardItemID    itemID;

		PasteboardGetItemIdentifier(clipboard, itemIndex, &itemID);

		CFDataRef flavorData;

		if (PasteboardCopyItemFlavorData(clipboard, itemID, kUTTypeUTF8PlainText, &flavorData) >= 0)
			{
			const char *pszString = (const char *)CFDataGetBytePtr(flavorData);
			
			DanaEl* array = api -> makeArray(charArrayGT, strlen(pszString));
			unsigned char* content = api -> getArrayContent(array);
			memcpy(content, pszString, strlen(pszString));
			api -> returnEl(cframe, array);
			}

		CFRelease(clipboard);
		}
	#endif

	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	
	setInterfaceFunction("setContent", op_set_content);
	setInterfaceFunction("getContent", op_get_content);

	#ifdef LINUX
	#ifndef OSX
	XInitThreads();
    display = XOpenDisplay(NULL);
	
	if (display != NULL)
		{
		color = BlackPixel(display, DefaultScreen(display));
		window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, color, color);

		pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE_NP);
		pthread_mutex_init(&clipboardLock, &mAttr);
		pthread_mutex_init(&pendingGetLock, &mAttr);
		
		sem_init(&sem, 0, 0);

		bufid = XInternAtom(display, "CLIPBOARD", False);
		fmtid = XInternAtom(display, "UTF8_STRING", False);
		incrid = XInternAtom(display, "INCR", False);
		propid = XInternAtom(display, "XSEL_DATA", False);
		xtargets = XInternAtom(display, "TARGETS", False);

		int err = 0;
		memset(&threadHandle, '\0', sizeof(pthread_t));
		if ((err = pthread_create(&threadHandle, NULL, clip_thread, NULL)) != 0){}
		}
	#endif
	#endif

	return getPublicInterface();
	}

void unload()
	{
	#ifdef LINUX
	#ifndef OSX
	if (display != NULL)
		{
		dwe.type = DestroyNotify;
		XSendEvent(display, window, 0, 0, (XEvent*) &dwe);
		XDestroyWindow(display, window);
		XFlush(display);
		pthread_join(threadHandle, NULL);
		XCloseDisplay(display);
		
		pthread_mutex_destroy(&clipboardLock);
		pthread_mutex_destroy(&pendingGetLock);
		}
	#endif
	#endif
	}
