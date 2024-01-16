//Written by Barry Porter, 2016, last updated 2023

//This is a graphics library using SDL2 for its rendering.

/*
NOTE: Since Dana v222 we need to statically link against the SDL libraries, to avoid additional dependency installs outside of Dana. On Linux, we need to download the SDL2 and SDL2ttf source code, and configure+make+make-install them. For SDL2 and SDL2ttf we need to update CFLAGS to include -fPIC because we're linking against a .so; for SDL itself we use --configure-sndio=no in configure to avoid linking against this extra library. We currently assume that lfreetype is installed on Linux as standard. We're aware that, in general, there are good reasons not to statically link SDL; we don't think these reasons apply here because the Dana runtime already dynamically links against this library, so it can be swapped out easily at the Dana level.
*/

/*
On Ubuntu, for the main SDL2 build we want these libraries to maximise device support:

sudo apt-get install libpulse-dev libaudio-dev libx11-dev libxext-dev libxrandr-dev libxcursor-dev libxi-dev libxinerama-dev libxxf86vm-dev libxss-dev libgl1-mesa-dev libesd0-dev 

And the procedure is:

mkdir build
cd build
../configure --enable-sndio=no
(add -fPIC to Makefile, if not done via configure)
make
make install

For SDL2ttf we need:

sudo apt-get install libfreetype6-dev

And the procedure is:

./configure
(add -fPIC to Makefile, if not done via configure)
make
make install
*/

/*
On Raspbian things are a bit sketchy; see https://forums.libsdl.org/viewtopic.php?p=39671

We can either link with -L/opt/vc/lib -lbcm_host, or we can use ./configure --disable-video-x11 --disable-video-opengl
*/

#include "dana_lib_defs.h"
#include "nli_util.h"
#include "vmi_util.h"

#ifdef WINDOWS
#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_syswm.h>
#endif

#ifdef LINUX
#ifndef OSX
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_syswm.h>
#endif
#ifdef OSX
#include <SDL2/SDL.h>
#include <SDL2_ttf/SDL_ttf.h>
#include <SDL2/SDL_syswm.h>
#endif
#endif

#include <SDL2_rotozoom.h>
#include <SDL2_gfxPrimitives.h>
#include <SDL2_framerate.h>

#include <SDL2/SDL_opengl.h>

#include <limits.h>

#include <sys/stat.h>

#include "platform_utils.h"

//#define STATS

#ifdef WINDOWS
#include <Windows.h>
#include <Windowsx.h>
#endif

#ifdef LINUX
#include <unistd.h>
#include <sys/utsname.h>
#include <pthread.h>
#include <semaphore.h>
#endif

#ifdef OSX
#include <dispatch/dispatch.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WINDOWS
	#ifdef MACHINE_64
	#define lockedInc(X) InterlockedIncrement64((long long*) X)
	#endif
	#ifdef MACHINE_32
	#define lockedInc(X) InterlockedIncrement((long*) X)
	#endif
#endif
#ifdef LINUX
#define lockedInc(X) __sync_add_and_fetch(X, 1)
#endif

#ifdef WINDOWS
	#ifdef MACHINE_64
	#define lockedDec(X) InterlockedDecrement64((long long*) X)
	#endif
	#ifdef MACHINE_32
	#define lockedDec(X) InterlockedDecrement((long*) X)
	#endif
#endif
#ifdef LINUX
#define lockedDec(X) __sync_sub_and_fetch(X, 1)
#endif

#ifdef WINDOWS
#define startCriticalSection(X) EnterCriticalSection(X)
#endif
#ifdef LINUX
#define startCriticalSection(X) pthread_mutex_lock(X)
#endif

#ifdef WINDOWS
#define stopCriticalSection(X) LeaveCriticalSection(X)
#endif
#ifdef LINUX
#define stopCriticalSection(X) pthread_mutex_unlock(X)
#endif

#ifdef WINDOWS
static CRITICAL_SECTION measureTextLock;
#endif
#ifdef LINUX
//create mutex attribute variable
static pthread_mutexattr_t mAttr;
static pthread_mutex_t measureTextLock;
#endif

static bool initComplete = false;

static CoreAPI *api;

static DanaType intType = {TYPE_LITERAL, 0, sizeof(size_t)};

static const DanaTypeField windowDataTypeFields[] = {
			{&intType, NULL, 0, 0, 0},
			{&intType, NULL, 0, 0, sizeof(size_t)},
			{&intType, NULL, 0, 0, sizeof(size_t)*2},
			{&intType, NULL, 0, 0, sizeof(size_t)*3},
			{&intType, NULL, 0, 0, sizeof(size_t)*4}
			};

static DanaType windowDataType = {TYPE_DATA, 0, sizeof(size_t)*5, (DanaTypeField*) windowDataTypeFields, 5};

/*
//for the _dni ::

static const DanaTypeField DropEventData_fields[] = {
{(DanaType*) &int_def, NULL, 0, 0, 0},
{(DanaType*) &int_def, NULL, 0, 0, sizeof(size_t)},
{(DanaType*) &char_array_def, NULL, 0, 0, sizeof(size_t) + sizeof(size_t)}};
static const DanaType DropEventData_def = 
{TYPE_DATA, 0, 32, (DanaTypeField*) DropEventData_fields, 3};
*/

static GlobalTypeLink *charArrayGT = NULL;
static GlobalTypeLink *pixelMapGT = NULL;
static GlobalTypeLink *whGT = NULL;
static GlobalTypeLink *integerGT = NULL;
static GlobalTypeLink *windowDataGT = NULL;
static GlobalTypeLink *dropDataGT = NULL;
static GlobalTypeLink *flowEventArrayGT = NULL;
static GlobalTypeLink *flowEventGT = NULL;

//the graphics buffer:
typedef struct _point{
	float x;
	float y;
	float z;
	int r;
	int g;
	int b;
	int a;
	} UIPoint;

typedef struct{
	double *xPoints;
	double *yPoints;
	short int *xPoints_si;
	short int *yPoints_si;
	int nPoints;
	int isteps;
	int thickness;
	int r;
	int g;
	int b;
	int a;
	} UIPolygon;

typedef struct{
	SDL_Rect rect;

	size_t thickness;

	int r;
	int g;
	int b;
	int a;
	} UIRect;

typedef struct{
	SDL_Rect rect;

	size_t start;
	size_t end;
	size_t thickness;

	bool chord;
	bool antiAlias;

	int r;
	int g;
	int b;
	int a;
	} UIArc;

typedef struct{
	unsigned int x1;
	unsigned int y1;

	unsigned int x2;
	unsigned int y2;

	unsigned int thickness;

	int r;
	int g;
	int b;
	int a;
	} UILine;

typedef struct{
	int r;
	int g;
	int b;
	int a;
	} RGBAInt;

typedef struct {
	TTF_Font *font;
	size_t refCount;
	} FontHolder;

typedef struct{
	FontHolder *font;
	char *text;
	unsigned int textLen;
	int x;
	int y;
	int r;
	int g;
	int b;
	int a;
	int rotation;
	} UIText;

typedef struct{
	SDL_Surface *surface;
	int x;
	int y;
	int rotation;
	} UIBitmap;

typedef struct{
	struct _uio *objects;
	struct _uio *lastObject;
	int x;
	int y;
	int width;
	int height;
	
	int a; //alpha here is the alpha of the post-rendered surface, and all of its contents

	int xScroll;
	int yScroll;
	} UISurface;

#define UI_TYPE_POLYGON		1
#define UI_TYPE_POLYGON_OUT	2
#define UI_TYPE_RECT		3
#define UI_TYPE_CIRCLE		4
#define UI_TYPE_TEXT		5
#define UI_TYPE_LINE		6
#define UI_TYPE_POINT		7
#define UI_TYPE_BITMAP		8
#define UI_TYPE_SURFACE		9
#define UI_TYPE_ELLIPSE		10
#define UI_TYPE_ELLIPSE_OUT	11
#define UI_TYPE_POLYGON_BEZ		12
#define UI_TYPE_POLYGON_BEZ_OUT	13
#define UI_TYPE_CURVE_BEZ	14
#define UI_TYPE_ARC			15
#define UI_TYPE_PIE			16

typedef struct _uio{
	unsigned int type;
	void *object;
	struct _uio *next;
	} UIObject;

//TODO: the following state will probably be in "internal state" of my object, eventually...

typedef struct __event_item {
	size_t type;
	size_t button_id;
	size_t x;
	size_t y;
	
	struct __event_item *next;
} EventItem;

typedef struct{
	RGBAInt backgroundColour;

	size_t windowX;
	size_t windowY;

	size_t windowWidth;
	size_t windowHeight;
	
	size_t windowedWidth;
	size_t windowedHeight;
	
	bool fullScreen;

#ifdef WINDOWS
	HWND windowHandle;
#endif

#ifdef OSX
	NSWindow *windowHandle;
#else
#ifdef LINUX
	Window windowHandle;
	Display *displayHandle;
#endif
#endif

	#ifdef WINDOWS
		HANDLE stateSem;
	#endif
	#ifdef OSX
		dispatch_semaphore_t stateSem;
	#else
	#ifdef LINUX
		sem_t stateSem;
	#endif
	#endif

	SDL_Window *win;
	SDL_Renderer *renderer;

	SDL_Cursor* currentCursor;

	UIObject *frontBuffer;
	UIObject *backBuffer;
	UIObject *backBufferEnd;
	UIObject *surfaceStack;
	UIPolygon *lastPolygon;

	SDL_Texture *baseTexture;
	bool sceneChanged;

	bool visible;

#ifdef WINDOWS
	HANDLE renderLock;
#endif

	VVarR clickParams[3];
	VVarR mouseParams[3];
	size_t clickX;
	size_t clickY;
	size_t clickButton;
	size_t mouseDownX;
	size_t mouseDownY;
	size_t mouseDownButton;
	size_t mouseUpX;
	size_t mouseUpY;
	size_t mouseUpButton;

	size_t lastMouseMoveX;
	size_t lastMouseMoveY;
	size_t mouseMoveX;
	size_t mouseMoveY;

	size_t captureCount;
	
	EventItem *eventQueue;
	EventItem *eventQueueEnd;
	
	void *eqObject;

	bool capturePixels;
	
	unsigned int ID;

	long int sdlProc;
	} WindowInstance;

typedef struct __li{
	void *data;
	struct __li *next;
	struct __li *prev;
	} ListItem;

static ListItem *instances = NULL;
static ListItem *lastInstance = NULL;

static void *systemEventObject = NULL;

typedef struct __wi_cs {
	unsigned char type;
	int w;
	int h;
	int hotpointX;
	int hotpointY;
	DanaEl* customCursor;
	} SetCursorInstance;

#ifdef STATS
typedef struct{
	TTF_Font *font;
	size_t lastFPS;
	size_t framesRendered;
	size_t lastIPS;
	size_t lastDXPS;
	size_t lastOCPS;
	size_t lastLCPS;
	#define AVG_STAT_LEN 3
	size_t ipsArray[AVG_STAT_LEN];
	size_t lastIPSIndex;
	} StatList;

static StatList stats;
#endif

#ifdef WINDOWS
static HANDLE frameworkShutdownLock;
#endif

#ifdef OSX
static dispatch_semaphore_t frameworkShutdownLock;
#else
#ifdef LINUX
static sem_t frameworkShutdownLock;
#endif
#endif

static void returnByteArray(FrameData* f, unsigned char *data, size_t len)
	{
	unsigned char* cnt = NULL;
	DanaEl* array = api -> makeArray(charArrayGT, len, &cnt);
	
	memcpy(cnt, data, len);
	
	api -> returnEl(f, array);
	}

void semaphore_init(Semaphore *s, unsigned int initialValue)
	{
	#ifdef WINDOWS
	s -> sem = CreateSemaphore(NULL, initialValue, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_t *sem;
	sem = &s -> sem;
    *sem = dispatch_semaphore_create(initialValue);
	#else
	#ifdef LINUX
	sem_init(&s -> sem, initialValue, 0);
	#endif
	#endif
	}

int semaphore_wait(Semaphore *s)
	{
	#ifdef WINDOWS
	return WaitForSingleObject(s -> sem, INFINITE);
	#endif
	#ifdef OSX
	return dispatch_semaphore_wait(s -> sem, DISPATCH_TIME_FOREVER);
	#else
	#ifdef LINUX
	return sem_wait(&s -> sem);
	#endif
	#endif
	}

void semaphore_post(Semaphore *s)
	{
	#ifdef WINDOWS
	ReleaseSemaphore(s -> sem, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_signal(s -> sem);
	#else
	#ifdef LINUX
	sem_post(&s -> sem);
	#endif
	#endif
	}

void semaphore_destroy(Semaphore *s)
	{
	#ifdef WINDOWS
	CloseHandle(s -> sem);
	#endif
    #ifdef OSX
    dispatch_release(s -> sem);
    #else
	#ifdef LINUX
	sem_close(&s -> sem);
	#endif
    #endif
	}

static void pushMouseEvent(WindowInstance *w, size_t type, size_t button_id, size_t x, size_t y, size_t exd1, size_t exd2);
static void pushEvent(WindowInstance *w, size_t type);
static void pushDropEvent(WindowInstance *w, char* path, size_t x, size_t y);

static ListItem* addListItem(ListItem **lst, ListItem **last, void *data)
	{
	ListItem *ni = malloc(sizeof(ListItem));
	memset(ni, '\0', sizeof(ListItem));

	ni -> data = data;

	if (*lst == NULL)
		*lst = ni;
		else
		(*last) -> next = ni;

	if (*last != NULL)
		ni -> prev = *last;

	*last = ni;

	return ni;
	}

static void removeListItem(ListItem **lst, ListItem **last, void *data)
	{
	ListItem *w = *lst;

	while (w != NULL)
		{
		if (w -> data == data)
			{
			if (w -> prev != NULL)
				w -> prev -> next = w -> next;
				else
				(*lst) = w -> next;

			if (w -> next != NULL)
				w -> next -> prev = w -> prev;
				else
				(*last) = w -> prev;

			if ((*lst) == NULL) (*last) = NULL;

			free(w);

			return;
			}

		w = w -> next;
		}
	}

typedef struct{
	const char *directory;
	} FontDirectory;

#ifdef WINDOWS
static FontDirectory fontDirectories[] = {{""}, {""}};
#endif
#ifdef LINUX
static FontDirectory fontDirectories[] = {{""}, {""}};
#endif

bool findFontByAlias(const char *fontAlias, const char *dir, char *result, unsigned int resultLen)
	{
	//scan dir for all font files (.ttf) and see if any have the given typeface alias
	unsigned int fileCount = 0;

	FileInfo *files = getDirFiles((char*) dir, &fileCount);

	FileInfo *fw = files;
	while (fw != NULL)
		{
		//printf(" - chk %s\n", fw -> path);

		struct stat st;
		stat(fw -> path, &st);
		if (st.st_mode & S_IFREG)
			{
			//compile
			if (strstr(fw -> path, ".ttf") != NULL)
				{
				TTF_Font *f = TTF_OpenFont(fw -> path, 12);

				if (f != NULL)
					{
					char *val = TTF_FontFaceFamilyName(f);
					if (strcmp(fontAlias, val) == 0)
						{
						TTF_CloseFont(f);
						strcpy(result, fw -> path);
						return true;
						}

					TTF_CloseFont(f);
					}
				}
			}
			/*
			else if (st.st_mode & S_IFDIR)
			{
			if (findFontByAlias(fw -> path, newbranch, pdp))
				return true;
			}
			*/

		fw = fw -> next;
		}

	return false;
	}

bool findFont(const char *ttfFileName, char *result, unsigned int resultLen)
	{
	bool literalFile = false;

	if (strlen(ttfFileName) >= 5 && strcmp(&ttfFileName[strlen(ttfFileName)-4], ".ttf") == 0)
		literalFile = true;

	if (literalFile)
		{
		memset(result, '\0', resultLen);
		strcpy(result, ttfFileName);

		struct stat st;
		if (stat(result, &st) == 0)
			return true;

		//check system fonts directories
		int i = 0;
		for (i = 0; i < sizeof(fontDirectories) / sizeof(FontDirectory); i++)
			{
			memset(result, '\0', resultLen);
			strcpy(result, fontDirectories[i].directory);
			strcat(result, ttfFileName);

			struct stat st;
			if (stat(result, &st) == 0)
				return true;
			}
		}
		else
		{
		//it's a font alias; search all font files
		if (findFontByAlias(ttfFileName, "", result, resultLen))
			return true;

		//check system fonts directories
		int i = 0;
		for (i = 0; i < sizeof(fontDirectories) / sizeof(FontDirectory); i++)
			{
			if (findFontByAlias(ttfFileName, fontDirectories[i].directory, result, resultLen))
				return true;
			}
		}

	return false;
	}

void primeFontDirectories()
	{
	//char *home = getenv("DANA_HOME");
	const char *home = api -> getDanaHome();
	char *danaFontDir = NULL;
	if (home != NULL)
		{
		danaFontDir = malloc(strlen(home) + strlen("/components/resources-ext/fonts/") + 1);
		strcpy(danaFontDir, home);
		strcat(danaFontDir, "/components/resources-ext/fonts/");
		}
		else
		{
		printf("UI library::DANA_HOME NOT SET! Built-in fonts will not be loaded.\n");
		danaFontDir = strdup("");
		}
	
	fontDirectories[0].directory = danaFontDir;

	#ifdef WINDOWS
	char winDir[1024];
	GetWindowsDirectory(winDir, 1024);
	strcat(winDir, "\\Fonts\\");
	fontDirectories[1].directory = strdup(winDir);
	#endif
	#ifdef OSX
	fontDirectories[1].directory = "/System/Library/Fonts/";
	#else
	#ifdef LINUX
	fontDirectories[1].directory = "/usr/share/fonts/truetype/ubuntu-font-family/";
	#endif
	#endif
	}

SDL_Texture* renderText(char *msg, TTF_Font *font, SDL_Color color, SDL_Renderer *rnd)
	{
	//first render to a surface as that's what TTF_RenderText returns, then load that surface into a texture

	//SDL_Surface *surf = TTF_RenderText_Solid(font, msg, color); //looks bad sometimes in general (inconsistent baseline)
	SDL_Surface *surf = TTF_RenderUTF8_Blended(font, msg, color); //looks bad with complex fonts at "small" sizes (<= 12pt)
	if (surf == NULL)
		{
		printf("UTF-8 text rendering error for '%s': %s\n", msg, SDL_GetError());
		return NULL;
		}

	SDL_Texture *texture = SDL_CreateTextureFromSurface(rnd, surf);
	if (texture == NULL)
		{
		printf("create texture error\n");
		return NULL;
		}
	
	SDL_SetTextureAlphaMod(texture, color.a);

	//Clean up the surface and font
	SDL_FreeSurface(surf);
	return texture;
	}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, with some desired
* width and height
* @param tex The source texture we want to draw
* @param rend The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
* @param w The width of the texture to draw
* @param h The height of the texture to draw
*/
void renderTextureWH(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h, int r)
	{
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopyEx(ren, tex, NULL, &dst, r, NULL, SDL_FLIP_NONE);
	}

//rotation around the x/y position
void renderTextureWHRZ(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h, int r)
	{
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_Point pnt;
	pnt.x = 0;
	pnt.y = 0;
	SDL_RenderCopyEx(ren, tex, NULL, &dst, r, &pnt, SDL_FLIP_NONE);
	}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
* the texture's width and height
* @param tex The source texture we want to draw
* @param rend The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int r)
	{
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	renderTextureWH(tex, ren, x, y, w, h, r);
	}

/**
* rotation around the x/y position
*/
void renderTextureRZ(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int r)
	{
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	renderTextureWHRZ(tex, ren, x, y, w, h, r);
	}

static void cleanupBuffer(UIObject *buf)
	{
	while (buf != NULL)
		{
		UIObject *td = buf;

		buf = buf -> next;

		if (td -> type == UI_TYPE_POLYGON || td -> type == UI_TYPE_POLYGON_OUT || td -> type == UI_TYPE_CURVE_BEZ || td -> type == UI_TYPE_POLYGON_BEZ || td -> type == UI_TYPE_POLYGON_BEZ_OUT)
			{
			UIPolygon *poly = (UIPolygon*) td -> object;

			free(poly -> xPoints);
			free(poly -> yPoints);

			if (poly -> xPoints_si != NULL) free(poly -> xPoints_si);
			if (poly -> yPoints_si != NULL) free(poly -> yPoints_si);
			}
			else if (td -> type == UI_TYPE_TEXT)
			{
			UIText *poly = (UIText*) td -> object;

			free(poly -> text);
			
			//dec refcount on the font object, and cleanup if zero...
			FontHolder *fHold = poly -> font;
			
			if (lockedDec(&fHold -> refCount) == 0)
				{
				TTF_CloseFont(fHold -> font);
				free(fHold);
				}
			}
			else if (td -> type == UI_TYPE_BITMAP)
			{
			UIBitmap *poly = (UIBitmap*) td -> object;

			SDL_FreeSurface(poly -> surface);
			}
			else if (td -> type == UI_TYPE_SURFACE)
			{
			cleanupBuffer(((UISurface*) td -> object) -> objects);
			}

		free(td -> object);
		free(td);
		}
	}

void drawObjects(SDL_Renderer* renderer, UIObject* objects)
	{
	SDL_Texture *baseTarget = SDL_GetRenderTarget(renderer);

	UIObject *pw = objects;
	while (pw != NULL)
		{
		if (pw -> type == UI_TYPE_POINT)
			{
			UIPoint *poly = (UIPoint*) pw -> object;

			SDL_SetRenderDrawColor(renderer, poly -> r, poly -> g, poly -> b, poly -> a);
			SDL_RenderDrawPoint(renderer, poly -> x, poly -> y);
			}
			else if (pw -> type == UI_TYPE_LINE)
			{
			UILine *poly = (UILine*) pw -> object;
			
			thickLineRGBA(renderer, poly -> x1, poly -> y1, poly -> x2, poly -> y2, poly -> thickness, poly -> r, poly -> g, poly -> b, poly -> a);
			}
			else if (pw -> type == UI_TYPE_RECT)
			{
			UIRect *poly = (UIRect*) pw -> object;

			SDL_SetRenderDrawColor(renderer, poly -> r, poly -> g, poly -> b, poly -> a);
			SDL_RenderFillRect(renderer, &poly -> rect);
			}
			else if (pw -> type == UI_TYPE_ELLIPSE)
			{
			UIRect *poly = (UIRect*) pw -> object;

			//filledEllipseRGBA(renderer, poly -> rect.x, poly -> rect.y, poly -> rect.w, poly -> rect.h, poly -> r, poly -> g, poly -> b, poly -> a);
			aaFilledEllipseRGBA(renderer, poly -> rect.x, poly -> rect.y, poly -> rect.w, poly -> rect.h, poly -> r, poly -> g, poly -> b, poly -> a);
			}
			else if (pw -> type == UI_TYPE_ELLIPSE_OUT)
			{
			UIRect *poly = (UIRect*) pw -> object;

			if (poly -> thickness == 1)
				{
				aaellipseRGBA(renderer, poly -> rect.x, poly -> rect.y, poly -> rect.w, poly -> rect.h, poly -> r, poly -> g, poly -> b, poly -> a);
				}
				else
				{
				//thickEllipseRGBA(instance -> renderer, poly -> rect.x, poly -> rect.y, poly -> rect.w, poly -> rect.h, poly -> r, poly -> g, poly -> b, poly -> a, poly -> thickness);

				aaArcRGBA(renderer, poly -> rect.x, poly -> rect.y, poly -> rect.w, poly -> rect.h, 0.0, 360.0, poly -> thickness, poly -> r, poly -> g, poly -> b, poly -> a);
				}
			}
			else if (pw -> type == UI_TYPE_POLYGON)
			{
			UIPolygon *poly = (UIPolygon*) pw -> object;

			//filledPolygonRGBA(renderer, poly -> xPoints, poly -> yPoints, poly -> nPoints, poly -> r, poly -> g, poly -> b, poly -> a);
			aaFilledPolygonRGBA(renderer, poly -> xPoints, poly -> yPoints, poly -> nPoints, poly -> r, poly -> g, poly -> b, poly -> a);
			}
			else if (pw -> type == UI_TYPE_POLYGON_OUT)
			{
			UIPolygon *poly = (UIPolygon*) pw -> object;

			if (poly -> thickness == 1)
				{
				aapolygonRGBA(renderer, poly -> xPoints_si, poly -> yPoints_si, poly -> nPoints, poly -> r, poly -> g, poly -> b, poly -> a);
				}
				else
				{
				//TODO.
				}
			}
			else if (pw -> type == UI_TYPE_POLYGON_BEZ)
			{
			UIPolygon *poly = (UIPolygon*) pw -> object;

			//filledPolyBezierRGBA(renderer, poly -> xPoints, poly -> yPoints, poly -> nPoints, poly -> r, poly -> g, poly -> b, poly -> a);
			aaFilledPolyBezierRGBA(renderer, poly -> xPoints, poly -> yPoints, poly -> nPoints, poly -> isteps, poly -> r, poly -> g, poly -> b, poly -> a);
			}
			else if (pw -> type == UI_TYPE_CURVE_BEZ)
			{
			UIPolygon *poly = (UIPolygon*) pw -> object;

			aaBezierRGBA(renderer, poly -> xPoints, poly -> yPoints, poly -> nPoints, poly -> isteps, poly -> thickness, poly -> r, poly -> g, poly -> b, poly -> a);
			}
			else if (pw -> type == UI_TYPE_ARC)
			{
			UIArc *poly = (UIArc*) pw -> object;

			if (poly -> antiAlias)
				aaArcRGBA(renderer, poly -> rect.x, poly -> rect.y, poly -> rect.w, poly -> rect.h, poly -> start + 270, poly -> end + 270, poly -> thickness, poly -> r, poly -> g, poly -> b, poly -> a);
				else
				thickArcRGBA(renderer, poly -> rect.x, poly -> rect.y, poly -> rect.w, poly -> start + 270, poly -> end + 270, poly -> r, poly -> g, poly -> b, poly -> a, poly -> thickness);
			}
			else if (pw -> type == UI_TYPE_PIE)
			{
			UIArc *poly = (UIArc*) pw -> object;

			aaFilledPieRGBA(renderer, poly -> rect.x, poly -> rect.y, poly -> rect.w, poly -> rect.h, poly -> start + 270, poly -> end + 270, poly -> chord, poly -> r, poly -> g, poly -> b, poly -> a);
			}
			else if (pw -> type == UI_TYPE_TEXT)
			{
			UIText *poly = (UIText*) pw -> object;

			SDL_Color color;
			color.r = poly -> r;
			color.g = poly -> g;
			color.b = poly -> b;
			color.a = poly -> a;

			SDL_Texture *image = renderText(poly -> text, poly -> font -> font, color, renderer);
			if (image != NULL)
				{
				renderTextureRZ(image, renderer, poly -> x, poly -> y, poly -> rotation);
				SDL_DestroyTexture(image);
				}
			}
			else if (pw -> type == UI_TYPE_SURFACE)
			{
			UISurface *poly = (UISurface*) pw -> object;

			SDL_Texture *st = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, poly -> width, poly -> height);
			SDL_SetRenderTarget(renderer, st);
			
			//NOTE: when using transparency, if we blend the background colour here (at alpha zero) against the SAME colour of text (white for white text), we get nice text; if the background colour is opposite (white for black text) we get horrible text rendering
			// - if we take the alpha channel away, blending is fine in all cases
			// - see https://discourse.libsdl.org/t/bad-text-quality-on-transparent-texture/23098
			// - a workaround is to require surfaces to have a solid background colour, and only offer to apply alpha to the whole surface (i.e., to the texture after all contents are rendered)
			// - we currently push this responsibility to Dana components, which should always render a solid background on a surface before rendering any contents
			SDL_SetTextureBlendMode(st, SDL_BLENDMODE_BLEND);
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
			SDL_RenderClear(renderer);

			drawObjects(renderer, poly -> objects);

			SDL_SetTextureAlphaMod(st, poly -> a);
			
			SDL_SetRenderTarget(renderer, baseTarget);

			if (st != NULL)
				{
				renderTexture(st, renderer, poly -> x, poly -> y, 0);
				SDL_DestroyTexture(st);
				}
			}
			else if (pw -> type == UI_TYPE_BITMAP)
			{
			UIBitmap *poly = (UIBitmap*) pw -> object;

			SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, poly -> surface);

			renderTexture(texture, renderer, poly -> x, poly -> y, poly -> rotation);
			SDL_DestroyTexture(texture);
			}

		pw = pw -> next;
		}
	}

/*
here we render everything onto a texture (baseTexture) and then render that texture to the screen
 - there are two reasons for this: (1) it has more consistent behaviour for font rendering across different graphics adaptors; (2) it's faster if you need to repaint the window but nothing actually changed (i.e. no "sceneChanged")
*/

static unsigned char* DrawScene(WindowInstance* instance)
	{
	unsigned char* pixelBuf = NULL;

	//background colour
	SDL_SetRenderDrawColor(instance -> renderer, instance -> backgroundColour.r, instance -> backgroundColour.g, instance -> backgroundColour.b, 255);
	
	SDL_SetRenderDrawBlendMode(instance -> renderer, SDL_BLENDMODE_BLEND);
	
	SDL_RenderClear(instance -> renderer);
	
	if (instance -> sceneChanged || instance -> baseTexture == NULL)
		{
		if (instance -> baseTexture != NULL)
			SDL_DestroyTexture(instance -> baseTexture);
		
		instance -> baseTexture = SDL_CreateTexture(instance -> renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, instance -> windowWidth, instance -> windowHeight);
		SDL_SetRenderTarget(instance -> renderer, instance -> baseTexture);

		SDL_SetTextureBlendMode(instance -> baseTexture, SDL_BLENDMODE_BLEND);

		//SDL_SetRenderDrawColor(instance -> renderer, 0, 0, 0, 0);
		SDL_SetRenderDrawColor(instance -> renderer, instance -> backgroundColour.r, instance -> backgroundColour.g, instance -> backgroundColour.b, 255);
		SDL_RenderClear(instance -> renderer);

		drawObjects(instance -> renderer, instance -> frontBuffer);

		instance -> sceneChanged = false;
		}

	SDL_SetRenderTarget(instance -> renderer, NULL);

	renderTexture(instance -> baseTexture, instance -> renderer, 0, 0, 0);

	if (instance -> capturePixels)
		{
		int w, h;
		SDL_QueryTexture(instance -> baseTexture, NULL, NULL, &w, &h);

		SDL_Rect dst;
		dst.x = 0;
		dst.y = 0;
		dst.w = w;
		dst.h = h;

		unsigned char* buf = malloc(w * h * 4);

		SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
		SDL_RenderReadPixels(instance -> renderer, &dst, SDL_PIXELFORMAT_ABGR8888, surface -> pixels, surface -> pitch);
		memcpy(buf, surface -> pixels, w * h * 4);
		SDL_FreeSurface(surface);

		pixelBuf = buf;
		}

	SDL_RenderPresent(instance -> renderer);

	return pixelBuf;
	}

static WindowInstance* findWindow(unsigned int ID)
	{
	ListItem *lw = instances;
	while (lw != NULL)
		{
		if (((WindowInstance*) lw -> data) -> ID == ID)
			return (WindowInstance*) lw -> data;

		lw = lw -> next;
		}

	return NULL;
	}

static WindowInstance* createNewWindow()
	{
	WindowInstance *myInstance = malloc(sizeof(WindowInstance));
	memset(myInstance, '\0', sizeof(WindowInstance));

	myInstance -> backgroundColour.r = 220;
	myInstance -> backgroundColour.g = 220;
	myInstance -> backgroundColour.b = 230;
	myInstance -> backgroundColour.a = 255;

	myInstance -> windowX = 100;
	myInstance -> windowY = 80;

	myInstance -> windowWidth = 500;
	myInstance -> windowHeight = 500;
	
	myInstance -> windowedWidth = 500;
	myInstance -> windowedHeight = 500;
	
	#ifdef WINDOWS
	myInstance -> stateSem = CreateSemaphore(NULL, 0, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_t *sem;
	sem = &myInstance -> stateSem;
	*sem = dispatch_semaphore_create(0);
	#else
	#ifdef LINUX
	sem_init(&myInstance -> stateSem, 0, 0);
	#endif
	#endif
	
	#ifdef WINDOWS
	myInstance -> renderLock = CreateSemaphore(NULL, 1, 1, NULL);
	#endif
	
	//temporary global variable:
	//mainInstance = myInstance;

	//ListItem *myInstanceItem =
	addListItem(&instances, &lastInstance, myInstance);

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	myInstance -> win = SDL_CreateWindow("Dana UI", myInstance -> windowX, myInstance -> windowY, myInstance -> windowWidth, myInstance -> windowHeight, SDL_WINDOW_HIDDEN);
	myInstance -> ID = SDL_GetWindowID(myInstance -> win);
	
	if (myInstance -> win == NULL){
		printf("null window\n");
		//SDL_GetError()
		return NULL;
	}

	//attempt to create our preferred renderer
	myInstance -> renderer = SDL_CreateRenderer(myInstance -> win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

	if (myInstance -> renderer == NULL)
		{
		//couldn't find a renderer matching our above preferences, so fall back on any available renderer
		//(this sometimes achieves highly suspect results - could be argued that we should just fail here instead...)
		myInstance -> renderer = SDL_CreateRenderer(myInstance -> win, -1, 0);
		}

	if (myInstance -> renderer == NULL)
		{
		printf("Critical error in GUI rendering subsystem: RCE 001 [%s]\n", SDL_GetError());
		return NULL;
		}

	return myInstance;
	}

typedef struct{
#ifdef WINDOWS
	HANDLE sem;
#endif
#ifdef OSX
	dispatch_semaphore_t sem;
#else
#ifdef LINUX
	sem_t sem;
#endif
#endif
	WindowInstance *instanceResult;
	} MakeWindowInfo;

typedef struct{
#ifdef WINDOWS
	HANDLE sem;
#endif
#ifdef OSX
	dispatch_semaphore_t sem;
#else
#ifdef LINUX
	sem_t sem;
#endif
#endif
	WindowInstance *wi;
	} CloseWindowInfo;

typedef struct{
	char *text;
	TTF_Font *font;
	FrameData* vframe;
	DanaEl *resultData;
	SDL_Color color;
#ifdef WINDOWS
	HANDLE sem;
#endif
#ifdef OSX
	dispatch_semaphore_t sem;
#else
#ifdef LINUX
	sem_t sem;
#endif
#endif
	} GenerateTextBitmapData;

typedef struct _gpinfo{
#ifdef WINDOWS
	HANDLE sem;
#endif
#ifdef OSX
	dispatch_semaphore_t sem;
#else
#ifdef LINUX
	sem_t sem;
#endif
#endif

	unsigned char* pixels;
	size_t width;
	size_t height;

	WindowInstance *wi;

	struct _gpinfo* next;
	} GetPixelsInfo;

typedef struct{
	UIBitmap *bitmapData;
	FrameData* vframe;
	DanaEl* sourceData;
	size_t scaledWidth;
	size_t scaledHeight;
#ifdef WINDOWS
	HANDLE sem;
#endif
#ifdef OSX
	dispatch_semaphore_t sem;
#else
#ifdef LINUX
	sem_t sem;
#endif
#endif
	} GenerateBitmapSurfaceData;

typedef struct{
	char *fontPath;
	size_t size;
	FrameData* vframe;
	FontHolder *fontHandle;
#ifdef WINDOWS
	HANDLE sem;
#endif
#ifdef OSX
	dispatch_semaphore_t sem;
#else
#ifdef LINUX
	sem_t sem;
#endif
#endif
	} LoadFontData;

typedef struct{
	char *text;
	TTF_Font *font;
	FrameData* vframe;
#ifdef WINDOWS
	HANDLE sem;
#endif
#ifdef OSX
	dispatch_semaphore_t sem;
#else
#ifdef LINUX
	sem_t sem;
#endif
#endif
	} GetTextWidthInfo;

static unsigned int DX_NEW_WINDOW_EVENT = 0;
static unsigned int DX_SWAP_BUFFERS_EVENT = 0;
static unsigned int DX_SET_WINDOW_POSITION = 0;
static unsigned int DX_SET_WINDOW_TITLE = 0;
static unsigned int DX_SET_WINDOW_ICON = 0;
static unsigned int DX_SET_WINDOW_CURSOR = 0;
static unsigned int DX_MAXIMISE_WINDOW = 0;
static unsigned int DX_MINIMISE_WINDOW = 0;
static unsigned int DX_SHOW_WINDOW = 0;
static unsigned int DX_HIDE_WINDOW = 0;
static unsigned int DX_SET_WINDOW_SIZE = 0;
static unsigned int DX_RESIZABLE_WINDOW = 0;
static unsigned int DX_FIXED_SIZE_WINDOW = 0;
static unsigned int DX_FULLSCREEN_WINDOW = 0;
static unsigned int DX_WINDOWED_WINDOW = 0;
static unsigned int DX_GET_RESOLUTION = 0;
static unsigned int DX_GET_PIXELS = 0;
static unsigned int DX_CLOSE_WINDOW = 0;
static unsigned int DX_GENERATE_TEXT_BITMAP = 0;
static unsigned int DX_GENERATE_BITMAP_SURFACE = 0;
static unsigned int DX_LOAD_FONT = 0;
static unsigned int DX_UNLOAD_FONT = 0;
static unsigned int DX_SYSTEM_SHUTDOWN = 0;

static bool resizeAvailable = true;

static SDL_Surface* pixelMapToSurface(DanaEl* pm)
	{
	DanaEl* whData = api -> getDataFieldEl(pm, 0);
	DanaEl* pixelArrayH = api -> getDataFieldEl(pm, 1);
	unsigned char *pixelArray = api -> getArrayContent(pixelArrayH);

	size_t w = api -> getDataFieldInt(whData, 0);
	size_t h = api -> getDataFieldInt(whData, 1);

	//printf(" -- draw bitmap -- %u pixels in %u:%u --\n", pixelArrayH -> length, w, h);

	//printf("creating %u * %u surface\n", w, h);

	// -- generate the surface with matching pixel data
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		Uint32 rmask = 0xff000000;
		Uint32 gmask = 0x00ff0000;
		Uint32 bmask = 0x0000ff00;
		Uint32 amask = 0x000000ff;
	#else
		Uint32 rmask = 0x000000ff;
		Uint32 gmask = 0x0000ff00;
		Uint32 bmask = 0x00ff0000;
		Uint32 amask = 0xff000000;
	#endif

	SDL_Surface *primarySurface = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);

	if (primarySurface == NULL)
		{
		return NULL;
		}

	// -- write the pixels
	SDL_LockSurface(primarySurface);
	int i = 0;
	int j = 0;
	int ndx = 0;
	for (i = 0;  i < h; i++)
		{
		for (j = 0; j < w; j++)
			{
			Uint32 pixel = 0;

			unsigned char *cdata = &pixelArray[ndx];

			((unsigned char*) &pixel)[0] = cdata[0];
			((unsigned char*) &pixel)[1] = cdata[1];
			((unsigned char*) &pixel)[2] = cdata[2];
			((unsigned char*) &pixel)[3] = cdata[3];

			//printf("pixel %u is %u:%u:%u:%u\n", (i*w)+j, cdata[0], cdata[1], cdata[2], cdata[3]);

			((Uint32*) primarySurface -> pixels)[(i*w)+j] = pixel;

			ndx += 4;
			}
		}
	SDL_UnlockSurface(primarySurface);
	
	return primarySurface;
	}

static void render_thread()
	{
	SDL_Event e;
	bool quit = false;

	//we draw a frame the first time round the loop, even if no frame has been passed in, just to clear the screen with the background colour
	bool newFrame = true;

	fflush(stdout);
	
	//notify any event listeners that the renderer is now ready to go...
	api -> pushEvent(systemEventObject, 0, 0, NULL);

	initComplete = true;
	
	while (!quit)
		{
		//Read user input & handle it
		
		//printf("wait-event\n");

		GetPixelsInfo* pixelGetList = NULL;
		
		if (SDL_WaitEvent(&e)) //does this make things more jittery?
		{
		do
			{
			//printf("%u\n", e.type);
			if (e.type == SDL_QUIT)
				{
				//notify any event listeners that the system has received a shutdown request
				api -> pushEvent(systemEventObject, 0, 10, NULL);
				}
				else if (e.type == DX_SYSTEM_SHUTDOWN)
				{
				quit = true;
				}
				else if (e.type == SDL_RENDER_TARGETS_RESET || e.type == SDL_RENDER_DEVICE_RESET)
				{
				newFrame = true;
				
				//also force a full redraw of the buffered texture for all windows...
				ListItem *lw = instances;
				while (lw != NULL)
					{
					WindowInstance *myInstance = (WindowInstance*) lw -> data;
					
					if (myInstance -> baseTexture != NULL)
						{
						SDL_DestroyTexture(myInstance -> baseTexture);
						myInstance -> baseTexture = NULL;
						}

					lw = lw -> next;
					}
				}
				else if (e.type == SDL_WINDOWEVENT)
				{
				//printf(" -- %u\n", e.window.event);
				//printf(" -- [%u][%u][%u][%u][%u][%u][%u][%u][%u]\n", SDL_WINDOWEVENT_SHOWN, SDL_WINDOWEVENT_HIDDEN, SDL_WINDOWEVENT_EXPOSED, SDL_WINDOWEVENT_MOVED, SDL_WINDOWEVENT_RESIZED, SDL_WINDOWEVENT_SIZE_CHANGED, SDL_WINDOWEVENT_MINIMIZED, SDL_WINDOWEVENT_MAXIMIZED, SDL_WINDOWEVENT_RESTORED);
				
				if (e.window.event == SDL_WINDOWEVENT_MAXIMIZED ||
					e.window.event == SDL_WINDOWEVENT_RESTORED ||
					e.window.event == SDL_WINDOWEVENT_EXPOSED ||
					e.window.event == SDL_WINDOWEVENT_FOCUS_GAINED)
					{
					newFrame = true;
					}
					else if (e.window.event == SDL_WINDOWEVENT_CLOSE)
					{
					WindowInstance *myInstance = findWindow(e.window.windowID);
					pushEvent(myInstance, 10);
					}
					else if (e.window.event == SDL_WINDOWEVENT_RESIZED)
					{
					WindowInstance *wi = findWindow(e.window.windowID);
					
					if (wi != NULL)
						{
						//printf("size now %d x %d\n", e.window.data1, e.window.data2);
						
						wi -> windowWidth = e.window.data1;
						wi -> windowHeight = e.window.data2;
						
						wi -> windowedWidth = e.window.data1;
						wi -> windowedHeight = e.window.data2;
						
						pushMouseEvent(wi, 7, 0, wi -> windowWidth, wi -> windowHeight, 0, 0);
						
						newFrame = true;
						}
					}
					else if (e.window.event == SDL_WINDOWEVENT_MOVED)
					{
					WindowInstance *myInstance = findWindow(e.window.windowID);
					
					if (myInstance != NULL)
						{
						int x = e.window.data1;
						int y = e.window.data2;
						
						if (x < 0) x = 0;
						if (y < 0) y = 0;

						#ifdef LINUX
						int top, bottom, left, right;
						SDL_GetWindowBordersSize(myInstance -> win, &top, &left, &bottom, &right);
						if (y > top) y -= top;
						#endif
						
						pushMouseEvent(myInstance, 8, 0, x, y, 0, 0);
						}
					
					newFrame = true;
					}
					//for active / inactive notifications (?):
					//SDL_WINDOWEVENT_FOCUS_GAINED
					//SDL_WINDOWEVENT_FOCUS_LOST
				}
				else if (e.type == SDL_MOUSEBUTTONDOWN)
				{
				size_t screenX = e.button.x;
				size_t screenY = e.button.y;
				
				size_t button = 0;
				
				if (e.button.button == SDL_BUTTON_LEFT)
					button = 1;
					else if (e.button.button == SDL_BUTTON_RIGHT)
					button = 2;
				
				WindowInstance *myInstance = findWindow(e.button.windowID);
				
				if (myInstance != NULL)
					{
					pushMouseEvent(myInstance, 2, button, screenX, screenY, 0, 0);
					}
				}
				else if (e.type == SDL_MOUSEBUTTONUP)
				{
				size_t screenX = e.button.x;
				size_t screenY = e.button.y;
				
				size_t button = 0;
				
				if (e.button.button == SDL_BUTTON_LEFT)
					button = 1;
					else if (e.button.button == SDL_BUTTON_RIGHT)
					button = 2;
				
				WindowInstance *myInstance = findWindow(e.button.windowID);

				if (myInstance != NULL)
					{
					//e.button.clicks has the number of clicks stored in it, for multi-click events like a double-click
					pushMouseEvent(myInstance, 1, button, screenX, screenY, e.button.clicks, 0);
					}
				}
				else if (e.type == SDL_MOUSEMOTION)
				{
				if (e.motion.x < 0) e.motion.x = 0;
				if (e.motion.y < 0) e.motion.y = 0;

				size_t screenX = e.motion.x;
				size_t screenY = e.motion.y;

				WindowInstance *myInstance = findWindow(e.motion.windowID);
				
				if (myInstance != NULL)
					{
					pushMouseEvent(myInstance, 3, 0, screenX, screenY, 0, 0);
					}
				}
				else if (e.type == SDL_MOUSEWHEEL)
				{
				int deltaX = e.wheel.x;
				int deltaY = e.wheel.y;

				WindowInstance *myInstance = findWindow(e.wheel.windowID);

				size_t xAdd = 0;
				size_t xSub = 0;
				size_t yAdd = 0;
				size_t ySub = 0;

				if (deltaX < 0)
					xSub = deltaX * -1;
					else
					xAdd = deltaX;
				
				if (deltaY < 0)
					ySub = deltaY * -1;
					else
					yAdd = deltaY;
				
				if (myInstance != NULL)
					{
					pushMouseEvent(myInstance, 4, 0, xAdd, yAdd, xSub, ySub);
					}
				}
				else if (e.type == SDL_KEYDOWN)
				{
				size_t keyID = e.key.keysym.scancode;
				
				WindowInstance *myInstance = findWindow(e.motion.windowID);
				
				if (myInstance != NULL)
					{
					pushMouseEvent(myInstance, 5, keyID, 0, 0, 0, 0);
					}
				}
				else if (e.type == SDL_KEYUP)
				{
				size_t keyID = e.key.keysym.scancode;
				
				WindowInstance *myInstance = findWindow(e.motion.windowID);
				
				if (myInstance != NULL)
					{
					pushMouseEvent(myInstance, 6, keyID, 0, 0, 0, 0);
					}
				}
				else if (e.type == SDL_DROPFILE)
				{
				char* dropped_filedir = e.drop.file;
				
				WindowInstance *myInstance = findWindow(e.drop.windowID);
				
				if (myInstance != NULL)
					{
					int x, y, winX, winY;
					//GetGlobalMouseState: as on some platforms GetMouseState will be zero for a file drop
					//SDL_GetMouseState(&x, &y);
					SDL_GetGlobalMouseState(&x, &y);
					// - get the current actual window position, not the last-recorded-API-position
					SDL_GetWindowPosition(myInstance -> win, &winX, &winY);
					x -= winX;
					y -= winY;
					pushDropEvent(myInstance, dropped_filedir, x, y);
					}
				
				SDL_free(dropped_filedir);
				}
				else if (e.type == DX_NEW_WINDOW_EVENT)
				{
				WindowInstance *nw = createNewWindow();

				((MakeWindowInfo*) e.user.data1) -> instanceResult = nw;
				#ifdef WINDOWS
				ReleaseSemaphore(((MakeWindowInfo*) e.user.data1) -> sem, 1, NULL);
				#endif
				#ifdef OSX
				dispatch_semaphore_signal(((MakeWindowInfo*) e.user.data1) -> sem);
				#else
				#ifdef LINUX
				sem_post(&((MakeWindowInfo*) e.user.data1) -> sem);
				#endif
				#endif
				}
				else if (e.type == DX_SWAP_BUFFERS_EVENT)
				{
				//NOTE: since intermediate frames (those that are skipped because a newer one comes in) never get rendered, so doing all this cleanup is kind of a waste of cycles :-/ (the problem is that it's not predictable which frames will be skipped...)
				// - often 5 or 6 frames are skipped in a row (meaning there are 7 pending frames and we just scoot to the newest one)
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				UIObject *newBuffer = (UIObject*) e.user.data2;

				UIObject *oldBuffer = wi -> frontBuffer;
				wi -> frontBuffer = newBuffer;
				cleanupBuffer(oldBuffer);

				newFrame = true;

				wi -> sceneChanged = true;
				}
				else if (e.type == DX_SET_WINDOW_POSITION)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				SDL_SetWindowPosition(wi -> win, wi -> windowX, wi -> windowY);

				//a new frame here seems overkill, but it fixes a rendering bug on some Windows installations when dragging the window
				newFrame = true;
				}
				else if (e.type == DX_SET_WINDOW_TITLE)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				SDL_SetWindowTitle(wi -> win, (char*) e.user.data2);
				free(e.user.data2);
				}
				else if (e.type == DX_SET_WINDOW_ICON)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				
				DanaEl *pixelMap = (DanaEl*) e.user.data2;
				
				SDL_Surface *surface = pixelMapToSurface(pixelMap);
				
				SDL_SetWindowIcon(wi -> win, surface);
				
				SDL_FreeSurface(surface);
				}
				else if (e.type == DX_SET_WINDOW_CURSOR)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				
				SetCursorInstance* sci = (SetCursorInstance*) e.user.data2;

				SDL_Cursor* nc = NULL;

				if (sci -> type == 0)
					{
					nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
					}
					else if (sci -> type == 1)
					{
					SDL_ShowCursor(SDL_DISABLE);
					}
					else if (sci -> type == 2)
					{
					//a custom-provided image, with an x/y hotpoint
					DanaEl* pixelMap = api -> getDataFieldEl(sci -> customCursor, 0);
					SDL_Surface *surface = pixelMapToSurface(pixelMap);

					nc = SDL_CreateColorCursor(surface, api -> getDataFieldInt(sci -> customCursor, 1), api -> getDataFieldInt(sci -> customCursor, 2));

					SDL_FreeSurface(surface);

					api -> decRef(NULL, sci -> customCursor);
					}
					else if (sci -> type == 3)
					{
					nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
					}
					else if (sci -> type == 4)
					{
					nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
					}
					else if (sci -> type == 5)
					{
					nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
					}
					else if (sci -> type == 6)
					{
					nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
					}
					else if (sci -> type == 7)
					{
					nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
					}
					else if (sci -> type == 8)
					{
					nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
					}
					else if (sci -> type == 9)
					{
					nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
					}
					else if (sci -> type == 10)
					{
					nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
					}
				
				if (sci -> type != 1)
					{
					SDL_ShowCursor(SDL_ENABLE);
					}
				
				/*
				SDL_SYSTEM_CURSOR_CROSSHAIR
				SDL_SYSTEM_CURSOR_WAITARROW
				SDL_SYSTEM_CURSOR_NO
				*/
				
				if (nc != NULL)
					{
					SDL_SetCursor(nc);

					//if there's a cursor already set (and about to no longer be user), we need to use SDL_FreeCursor on that one
					if (wi -> currentCursor != NULL)
						{
						SDL_FreeCursor(wi -> currentCursor);
						}
					
					wi -> currentCursor = nc;
					}
				
				free(sci);
				}
				else if (e.type == DX_MAXIMISE_WINDOW)
				{
				//WindowInstance *wi = (WindowInstance*) e.user.data1;
				//ShowWindow(wi -> windowHandle, SW_MAXIMIZE);
				}
				else if (e.type == DX_MINIMISE_WINDOW)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				SDL_MinimizeWindow(wi -> win);
				}
				else if (e.type == DX_SHOW_WINDOW)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				SDL_ShowWindow(wi -> win);

				newFrame = true;

				wi -> visible = true;
				
				//re-build the texture for the now-visible window
				wi -> sceneChanged = true;
				
				#ifdef WINDOWS
				ReleaseSemaphore(wi -> stateSem, 1, NULL);
				#endif
				#ifdef OSX
				dispatch_semaphore_signal(wi -> stateSem);
				#else
				#ifdef LINUX
				sem_post(&wi -> stateSem);
				#endif
				#endif
				}
				else if (e.type == DX_HIDE_WINDOW)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				SDL_HideWindow(wi -> win);

				wi -> visible = false;
				
				#ifdef WINDOWS
				ReleaseSemaphore(wi -> stateSem, 1, NULL);
				#endif
				#ifdef OSX
				dispatch_semaphore_signal(wi -> stateSem);
				#else
				#ifdef LINUX
				sem_post(&wi -> stateSem);
				#endif
				#endif
				}
				else if (e.type == DX_SET_WINDOW_SIZE)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				
				if (!wi -> fullScreen)
					{
					wi -> windowWidth = wi -> windowedWidth;
					wi -> windowHeight = wi -> windowedHeight;
					
					SDL_SetWindowSize(wi -> win, wi -> windowWidth, wi -> windowHeight);
					}
				
				newFrame = true;
				}
				else if (e.type == DX_RESIZABLE_WINDOW)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				SDL_SetWindowResizable(wi -> win, true);
				}
				else if (e.type == DX_FIXED_SIZE_WINDOW)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				SDL_SetWindowResizable(wi -> win, false);
				}
				else if (e.type == DX_FULLSCREEN_WINDOW)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				
				SDL_DisplayMode DM;
				SDL_GetCurrentDisplayMode(0, &DM);
				
				wi -> windowWidth = DM.w;
				wi -> windowHeight = DM.h;

				#ifdef LINUX
				//TODO: for some reason this hide/show needed on Linux to allow rendering into the now-fullscreen window area
				//TODO: try removing this workaround in newer SDL releases, and retest visual/FullScreen.o to see if it works any better
				if (wi -> visible)
					{
					SDL_HideWindow(wi -> win);
					}
				#endif

				SDL_SetWindowFullscreen(wi -> win, SDL_WINDOW_FULLSCREEN_DESKTOP);
				
				#ifdef LINUX
				if (wi -> visible)
					{
					SDL_ShowWindow(wi -> win);
					}
				#endif
				
				wi -> fullScreen = true;

				//re-build the texture for the now-visible window
				newFrame = true;
				wi -> sceneChanged = true;
				
				#ifdef WINDOWS
				ReleaseSemaphore(wi -> stateSem, 1, NULL);
				#endif
				#ifdef OSX
				dispatch_semaphore_signal(wi -> stateSem);
				#else
				#ifdef LINUX
				sem_post(&wi -> stateSem);
				#endif
				#endif
				}
				else if (e.type == DX_WINDOWED_WINDOW)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				
				wi -> windowWidth = wi -> windowedWidth;
				wi -> windowHeight = wi -> windowedHeight;
				
				SDL_SetWindowFullscreen(wi -> win, 0);
				
				wi -> fullScreen = false;

				//re-build the texture for the now-visible window
				newFrame = true;
				wi -> sceneChanged = true;
				
				#ifdef WINDOWS
				ReleaseSemaphore(wi -> stateSem, 1, NULL);
				#endif
				#ifdef OSX
				dispatch_semaphore_signal(wi -> stateSem);
				#else
				#ifdef LINUX
				sem_post(&wi -> stateSem);
				#endif
				#endif
				}
				else if (e.type == DX_GET_RESOLUTION)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				
				SDL_DisplayMode DM;
				SDL_GetCurrentDisplayMode(0, &DM);
				
				DanaEl* data = (DanaEl*) e.user.data2;
				api -> setDataFieldInt(data, 0, DM.w);
				api -> setDataFieldInt(data, 1, DM.h);
				
				#ifdef WINDOWS
				ReleaseSemaphore(wi -> stateSem, 1, NULL);
				#endif
				#ifdef OSX
				dispatch_semaphore_signal(wi -> stateSem);
				#else
				#ifdef LINUX
				sem_post(&wi -> stateSem);
				#endif
				#endif
				}
				else if (e.type == DX_GET_PIXELS)
				{
				GetPixelsInfo *cwi = (GetPixelsInfo*) e.user.data1;

				cwi -> wi -> capturePixels = true;

				cwi -> next = pixelGetList;

				pixelGetList = cwi;

				newFrame = true;
				}
				else if (e.type == DX_CLOSE_WINDOW)
				{
				CloseWindowInfo *cwi = (CloseWindowInfo*) e.user.data1;

				removeListItem(&instances, &lastInstance, cwi -> wi);
				
				if (cwi -> wi -> baseTexture != NULL)
					SDL_DestroyTexture(cwi -> wi -> baseTexture);
				
				SDL_DestroyRenderer(cwi -> wi -> renderer);
				SDL_DestroyWindow(cwi -> wi -> win);
				
				//TODO: cleanup wi -> frontBuffer / wi -> backBuffer ???
				free(cwi -> wi);
				
				#ifdef WINDOWS
				ReleaseSemaphore(cwi -> sem, 1, NULL);
				#endif
				#ifdef OSX
				dispatch_semaphore_signal(cwi -> sem);
				#else
				#ifdef LINUX
				sem_post(&cwi -> sem);
				#endif
				#endif
				}
				else if (e.type == DX_GENERATE_TEXT_BITMAP)
				{
				GenerateTextBitmapData *data = (GenerateTextBitmapData*) e.user.data1;

				DanaEl *bitmapData = data -> resultData;
				DanaEl *whData = api -> getDataFieldEl(bitmapData, 0);
				
				//printf("rendering text with color %u:%u:%u:%u\n", data -> color.r, data -> color.g, data -> color.b, data -> color.a);

				SDL_Surface *surf = TTF_RenderText_Blended(data -> font, data -> text, data -> color);
				if (surf == NULL)
					{
					printf("SDL surface error from RT for '%s': %s\n", data -> text, SDL_GetError());
					}

				size_t w = surf -> w;
				size_t h = surf -> h;

				size_t totalPixels = w * h;
				size_t sz = 4;
				
				api -> setDataFieldInt(whData, 0, w);
				api -> setDataFieldInt(whData, 1, h);
				
				//set up the pixel array
			
				size_t asz = totalPixels * sz;
				
				unsigned char* pixelArray = NULL;
				DanaEl* pixelArrayH = api -> makeArray(charArrayGT, asz, &pixelArray);
				
				api -> setDataFieldEl(bitmapData, 1, pixelArrayH);
				
				//printf("size: %u | %u\n", totalPixels, ((StructuredType*) pixelArrayH -> gtLink -> typeLink -> definition.content) -> size);

				//printf(" -- generate bitmap -- %u pixels in %u:%u --\n", pixelArrayH -> length, w, h);

				SDL_PixelFormat *fmt;
				fmt = surf -> format;

				SDL_LockSurface(surf);

				int i = 0;
				int j = 0;
				int ndx = 0;
				for  (i = 0; i < h; i++)
					{
					for (j = 0; j < w; j++)
						{
						//read the next pixel
						unsigned char red;
						unsigned char green;
						unsigned char blue;
						unsigned char alpha;

						Uint32 pixel = 0;
						Uint32 temp = 0;

						pixel = ((Uint32*) surf -> pixels)[(i*w)+j];

						temp = pixel & fmt->Rmask;
						temp = temp >> fmt->Rshift;
						temp = temp << fmt->Rloss;
						red = (Uint8) temp;

						temp = pixel & fmt->Gmask;
						temp = temp >> fmt->Gshift;
						temp = temp << fmt->Gloss;
						green = (Uint8) temp;

						temp = pixel & fmt->Bmask;
						temp = temp >> fmt->Bshift;
						temp = temp << fmt->Bloss;
						blue = (Uint8) temp;

						temp = pixel & fmt->Amask;
						temp = temp >> fmt->Ashift;
						temp = temp << fmt->Aloss;
						alpha = (Uint8) temp;

						//printf("pixel %u is %u:%u:%u:%u\n", (i*w)+j, red, green, blue, alpha);

						unsigned char *cdata = &pixelArray[ndx];

						cdata[0] = red;
						cdata[1] = green;
						cdata[2] = blue;
						cdata[3] = alpha;

						ndx += 4;
						}
					}

				SDL_UnlockSurface(surf);

				SDL_FreeSurface(surf);

				#ifdef WINDOWS
				ReleaseSemaphore(data -> sem, 1, NULL);
				#endif
				#ifdef OSX
				dispatch_semaphore_signal(data -> sem);
				#else
				#ifdef LINUX
				sem_post(&data -> sem);
				#endif
				#endif
				}
				else if (e.type == DX_GENERATE_BITMAP_SURFACE)
				{
				GenerateBitmapSurfaceData *data = (GenerateBitmapSurfaceData*) e.user.data1;

				SDL_Surface *primarySurface = pixelMapToSurface(data -> sourceData);
				
				SDL_Surface *finalSurface = NULL;
				
				if (data -> scaledWidth != primarySurface -> w || data -> scaledHeight != primarySurface -> h)
					{
					//printf("rescaling %u/%u...\n", data -> scaledWidth, data -> scaledHeight);
					float zoomX = (float) data -> scaledWidth / (float) primarySurface -> w;
					float zoomY = (float) data -> scaledHeight / (float) primarySurface -> h;

					finalSurface = zoomSurface(primarySurface, zoomX, zoomY, SMOOTHING_ON);
					SDL_FreeSurface(primarySurface);
					}
					else
					{
					finalSurface = primarySurface;
					}
				
				data -> bitmapData -> surface = finalSurface;
				
				// -- clean up and return

				#ifdef WINDOWS
				ReleaseSemaphore(data -> sem, 1, NULL);
				#endif
				#ifdef OSX
				dispatch_semaphore_signal(data -> sem);
				#else
				#ifdef LINUX
				sem_post(&data -> sem);
				#endif
				#endif
				}
				else if (e.type == DX_LOAD_FONT)
				{
				LoadFontData *lfd = (LoadFontData*) e.user.data1;
				FrameData* frame = lfd -> vframe;
				
				TTF_Font *font = TTF_OpenFont(lfd -> fontPath, lfd -> size);
				if (font == NULL)
					{
					printf("open font error: %s [%s]\n", SDL_GetError(), TTF_GetError());
					}
				
				free(lfd -> fontPath);

				//note: we could support font styles, like:
				//TTF_SetFontStyle(font, TTF_STYLE_BOLD);
				
				FontHolder *fhold = malloc(sizeof(FontHolder));
				fhold -> refCount = 1;
				fhold -> font = font;
				
				size_t xs = (size_t) fhold;
				api -> returnRaw(frame, (unsigned char*) &xs, sizeof(size_t));
				
				#ifdef WINDOWS
				ReleaseSemaphore(lfd -> sem, 1, NULL);
				#endif
				#ifdef OSX
				dispatch_semaphore_signal(lfd -> sem);
				#else
				#ifdef LINUX
				sem_post(&lfd -> sem);
				#endif
				#endif
				}
				else if (e.type == DX_UNLOAD_FONT)
				{
				LoadFontData *lfd = (LoadFontData*) e.user.data1;
				
				FontHolder *fhld = lfd -> fontHandle;
				TTF_Font *font = fhld -> font;
				
				if (lockedDec(&fhld -> refCount) == 0)
					{
					TTF_CloseFont(font);
					free(fhld);
					}
				
				#ifdef WINDOWS
				ReleaseSemaphore(lfd -> sem, 1, NULL);
				#endif
				#ifdef OSX
				dispatch_semaphore_signal(lfd -> sem);
				#else
				#ifdef LINUX
				sem_post(&lfd -> sem);
				#endif
				#endif
				}
			} while(SDL_PollEvent(&e)); //go through all other events to clear the queue
		}
		else
		{
		//printf(SDL_GetError());
		}

		if (newFrame)
			{
			//fire the draw method of each window
			ListItem *lw = instances;
			while (lw != NULL)
				{
				WindowInstance *wi = (WindowInstance*) lw -> data;

				startCriticalSection(&measureTextLock);
				unsigned char* buf = DrawScene(wi);
				stopCriticalSection(&measureTextLock);

				if (buf != NULL)
					{
					//make a copy for each requester
					bool used = false;

					GetPixelsInfo *pw = pixelGetList;
					while (pw != NULL)
						{
						if (pw -> wi == wi)
							{
							pw -> width = wi -> windowWidth;
							pw -> height = wi -> windowHeight;
							
							if (!used)
								{
								pw -> pixels = buf;
								}
								else
								{
								unsigned char* cpy = malloc(pw -> width * pw -> height * 4);
								memcpy(cpy, buf, pw -> width * pw -> height * 4);
								pw -> pixels = buf;
								}

							used = true;
							}

						pw = pw -> next;
						}
					}

				lw = lw -> next;
				}
			}

		newFrame = false;

		//finish pixel capture, if requested
		GetPixelsInfo *pw = pixelGetList;
		while (pw != NULL)
			{
			//we get the current item and move the pointer on before signalling the current item, in case the current item gets cleaned up right after signalling it (at which point pw -> next would then fail)
			GetPixelsInfo *dw = pw;
			pw = pw -> next;

			#ifdef WINDOWS
			ReleaseSemaphore(dw -> sem, 1, NULL);
			#endif
			#ifdef OSX
			dispatch_semaphore_signal(dw -> sem);
			#else
			#ifdef LINUX
			sem_post(&dw -> sem);
			#endif
			#endif
			}
		}
	
	//destroy all remaining windows...
	ListItem *lw = instances;
	while (lw != NULL)
		{
		SDL_DestroyRenderer(((WindowInstance*) lw -> data) -> renderer);
		SDL_DestroyWindow(((WindowInstance*) lw -> data) -> win);
		lw = lw -> next;
		}

	#ifdef WINDOWS
	ReleaseSemaphore(frameworkShutdownLock, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_signal(frameworkShutdownLock);
	#else
	#ifdef LINUX
	sem_post(&frameworkShutdownLock);
	#endif
	#endif
	}

INSTRUCTION_DEF op_make_window(FrameData* cframe)
	{
	if (!initComplete)
		{
		api -> throwException(cframe, "rendering framework has not been initialised");
		return RETURN_OK;
		}
	
	MakeWindowInfo *mwInfo = malloc(sizeof(MakeWindowInfo));
	memset(mwInfo, '\0', sizeof(MakeWindowInfo));
	
	#ifdef WINDOWS
	mwInfo -> sem = CreateSemaphore(NULL, 0, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_t *sem;
	sem = &mwInfo -> sem;
	*sem = dispatch_semaphore_create(0);
	#else
	#ifdef LINUX
	sem_init(&mwInfo -> sem, 0, 0);
	#endif
	#endif
	
	//push a create window event to the SDL thread
	SDL_Event newEvent;
	SDL_zero(newEvent);
	newEvent.type = DX_NEW_WINDOW_EVENT;
	newEvent.user.data1 = mwInfo;
	
	SDL_PushEvent(&newEvent);
	
	#ifdef WINDOWS
	WaitForSingleObject(mwInfo -> sem, INFINITE);
	#endif
	#ifdef OSX
	dispatch_semaphore_wait(mwInfo -> sem, DISPATCH_TIME_FOREVER);
	#else
	#ifdef LINUX
	sem_wait(&mwInfo -> sem);
	#endif
	#endif
	
	#ifdef WINDOWS
	CloseHandle(mwInfo -> sem);
	#endif
	#ifdef OSX
	dispatch_release(mwInfo -> sem);
	#else
	#ifdef LINUX
	sem_destroy(&mwInfo -> sem);
	#endif
	#endif
	
	//return "mwInfo -> instanceResult" as an unsigned int
	size_t xs = (size_t) mwInfo -> instanceResult;
	api -> returnRaw(cframe, (unsigned char*) &xs, sizeof(size_t));
	
	if (mwInfo -> instanceResult != NULL)
		mwInfo -> instanceResult -> eqObject = api -> getInstanceObject(cframe);
	
	free(mwInfo);
	
	return RETURN_OK;
	}

static void addUIObject(WindowInstance *instance, UIObject *o)
	{
	if (instance -> surfaceStack != NULL)
		{
		UISurface *surface = (UISurface*) instance -> surfaceStack -> object;

		//decide on scroll position
		//TODO: we could also decide on whether or not the object will render at all (due to e.g. all of its points being out of bounds)
		if (o -> type == UI_TYPE_RECT || o -> type == UI_TYPE_ARC || o -> type == UI_TYPE_PIE || o -> type == UI_TYPE_ELLIPSE || o -> type == UI_TYPE_ELLIPSE_OUT)
			{
			((UIRect*) o -> object) -> rect.x -= surface -> xScroll;
			((UIRect*) o -> object) -> rect.y -= surface -> yScroll;
			}
			else if (o -> type == UI_TYPE_TEXT)
			{
			((UIText*) o -> object) -> x -= surface -> xScroll;
			((UIText*) o -> object) -> y -= surface -> yScroll;
			}
			else if (o -> type == UI_TYPE_LINE)
			{
			((UILine*) o -> object) -> x1 -= surface -> xScroll;
			((UILine*) o -> object) -> y1 -= surface -> yScroll;

			((UILine*) o -> object) -> x2 -= surface -> xScroll;
			((UILine*) o -> object) -> y2 -= surface -> yScroll;
			}
			else if (o -> type == UI_TYPE_POINT)
			{
			((UIPoint*) o -> object) -> x -= surface -> xScroll;
			((UIPoint*) o -> object) -> y -= surface -> yScroll;
			}
			else if (o -> type == UI_TYPE_SURFACE)
			{
			((UISurface*) o -> object) -> x -= surface -> xScroll;
			((UISurface*) o -> object) -> y -= surface -> yScroll;
			}
			else if (o -> type == UI_TYPE_BITMAP)
			{
			((UIBitmap*) o -> object) -> x -= surface -> xScroll;
			((UIBitmap*) o -> object) -> y -= surface -> yScroll;
			}

		if (surface -> lastObject == NULL)
			surface -> objects = o;
			else
			surface -> lastObject -> next = o;

		surface -> lastObject = o;
		}
		else
		{
		if (instance -> backBufferEnd == NULL)
			instance -> backBuffer = o;
			else
			instance -> backBufferEnd -> next = o;

		instance -> backBufferEnd = o;
		}
	}

INSTRUCTION_DEF op_add_rect(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = api -> getParamInt(cframe, 1);
		size_t y = api -> getParamInt(cframe, 2);
		size_t w = api -> getParamInt(cframe, 3);
		size_t h = api -> getParamInt(cframe, 4);
		unsigned char r = api -> getParamRaw(cframe, 5)[0];
		unsigned char g = api -> getParamRaw(cframe, 6)[0];
		unsigned char b = api -> getParamRaw(cframe, 7)[0];
		unsigned char a = api -> getParamRaw(cframe, 8)[0];

		// -- create the container --

		UIRect *poly = (UIRect*) malloc(sizeof(UIRect));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));

		uio -> type = UI_TYPE_RECT;
		uio -> object = poly;

		poly -> rect.x = x;
		poly -> rect.y = y;

		poly -> rect.w = w;
		poly -> rect.h = h;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;

		addUIObject(instance, uio);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_ellipse(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = api -> getParamInt(cframe, 1);
		size_t y = api -> getParamInt(cframe, 2);
		size_t w = api -> getParamInt(cframe, 3);
		size_t h = api -> getParamInt(cframe, 4);
		unsigned char r = api -> getParamRaw(cframe, 5)[0];
		unsigned char g = api -> getParamRaw(cframe, 6)[0];
		unsigned char b = api -> getParamRaw(cframe, 7)[0];
		unsigned char a = api -> getParamRaw(cframe, 8)[0];

		// -- create the container --

		UIRect *poly = (UIRect*) malloc(sizeof(UIRect));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));

		uio -> type = UI_TYPE_ELLIPSE;
		uio -> object = poly;

		poly -> rect.x = x;
		poly -> rect.y = y;

		poly -> rect.w = w;
		poly -> rect.h = h;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;

		addUIObject(instance, uio);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_ellipse_outline(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = api -> getParamInt(cframe, 1);
		size_t y = api -> getParamInt(cframe, 2);
		size_t w = api -> getParamInt(cframe, 3);
		size_t h = api -> getParamInt(cframe, 4);
		size_t thickness = api -> getParamInt(cframe, 5);
		unsigned char r = api -> getParamRaw(cframe, 6)[0];
		unsigned char g = api -> getParamRaw(cframe, 7)[0];
		unsigned char b = api -> getParamRaw(cframe, 8)[0];
		unsigned char a = api -> getParamRaw(cframe, 9)[0];

		// -- create the container --

		UIRect *poly = (UIRect*) malloc(sizeof(UIRect));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));

		uio -> type = UI_TYPE_ELLIPSE_OUT;
		uio -> object = poly;

		poly -> rect.x = x;
		poly -> rect.y = y;

		poly -> rect.w = w;
		poly -> rect.h = h;

		poly -> thickness = thickness;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;

		addUIObject(instance, uio);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_arc(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = api -> getParamInt(cframe, 1);
		size_t y = api -> getParamInt(cframe, 2);
		size_t w = api -> getParamInt(cframe, 3);
		size_t h = api -> getParamInt(cframe, 4);
		size_t start = api -> getParamInt(cframe, 5);
		size_t end = api -> getParamInt(cframe, 6);
		size_t thickness = api -> getParamInt(cframe, 7);
		unsigned char r = api -> getParamRaw(cframe, 8)[0];
		unsigned char g = api -> getParamRaw(cframe, 9)[0];
		unsigned char b = api -> getParamRaw(cframe, 10)[0];
		unsigned char a = api -> getParamRaw(cframe, 11)[0];
		unsigned char antiAlias = api -> getParamRaw(cframe, 12)[0];

		// -- create the container --

		UIArc *poly = (UIArc*) malloc(sizeof(UIArc));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));

		uio -> type = UI_TYPE_ARC;
		uio -> object = poly;

		poly -> rect.x = x;
		poly -> rect.y = y;

		poly -> rect.w = w;
		poly -> rect.h = h;

		poly -> thickness = thickness;
		poly -> start = start;
		poly -> end = end;

		poly -> antiAlias = antiAlias;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;

		addUIObject(instance, uio);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_pie(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = api -> getParamInt(cframe, 1);
		size_t y = api -> getParamInt(cframe, 2);
		size_t w = api -> getParamInt(cframe, 3);
		size_t h = api -> getParamInt(cframe, 4);
		size_t start = api -> getParamInt(cframe, 5);
		size_t end = api -> getParamInt(cframe, 6);
		unsigned char chord = api -> getParamRaw(cframe, 7)[0];
		unsigned char r = api -> getParamRaw(cframe, 8)[0];
		unsigned char g = api -> getParamRaw(cframe, 9)[0];
		unsigned char b = api -> getParamRaw(cframe, 10)[0];
		unsigned char a = api -> getParamRaw(cframe, 11)[0];

		// -- create the container --

		UIArc *poly = (UIArc*) malloc(sizeof(UIArc));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));
		
		uio -> type = UI_TYPE_PIE;
		uio -> object = poly;

		poly -> rect.x = x;
		poly -> rect.y = y;

		poly -> rect.w = w;
		poly -> rect.h = h;

		poly -> start = start;
		poly -> end = end;

		poly -> chord = chord;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;

		addUIObject(instance, uio);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_polygon(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		DanaEl* points = api -> getParamEl(cframe, 1);

		unsigned char r = api -> getParamRaw(cframe, 2)[0];
		unsigned char g = api -> getParamRaw(cframe, 3)[0];
		unsigned char b = api -> getParamRaw(cframe, 4)[0];
		unsigned char a = api -> getParamRaw(cframe, 5)[0];

		// -- create the container --

		UIPolygon *poly = (UIPolygon*) malloc(sizeof(UIPolygon));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));
		
		addUIObject(instance, uio);

		uio -> type = UI_TYPE_POLYGON;
		uio -> object = poly;

		size_t nPoints = api -> getArrayLength(points);

		poly -> xPoints = malloc(sizeof(double) * nPoints);
		poly -> yPoints = malloc(sizeof(double) * nPoints);

		poly -> xPoints_si = NULL;
		poly -> yPoints_si = NULL;

		int i = 0;
		for (i = 0; i < nPoints; i++)
			{
			DanaEl* it = api -> getArrayCellEl(points, i);

			poly -> xPoints[i] = api -> getDataFieldInt(it, 0);
			poly -> yPoints[i] = api -> getDataFieldInt(it, 1);
			}
		
		poly -> nPoints = nPoints;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_polygon_outline(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		DanaEl* points = api -> getParamEl(cframe, 1);
		size_t thickness = api -> getParamInt(cframe, 2);
		unsigned char r = api -> getParamRaw(cframe, 3)[0];
		unsigned char g = api -> getParamRaw(cframe, 4)[0];
		unsigned char b = api -> getParamRaw(cframe, 5)[0];
		unsigned char a = api -> getParamRaw(cframe, 6)[0];

		// -- create the container --

		UIPolygon *poly = (UIPolygon*) malloc(sizeof(UIPolygon));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));
		
		addUIObject(instance, uio);

		uio -> type = UI_TYPE_POLYGON_OUT;
		uio -> object = poly;

		size_t nPoints = api -> getArrayLength(points);

		poly -> xPoints = malloc(sizeof(double) * nPoints);
		poly -> yPoints = malloc(sizeof(double) * nPoints);

		poly -> xPoints_si = malloc(sizeof(short int) * nPoints);
		poly -> yPoints_si = malloc(sizeof(short int) * nPoints);

		int i = 0;
		for (i = 0; i < nPoints; i++)
			{
			DanaEl* it = api -> getArrayCellEl(points, i);

			poly -> xPoints[i] = api -> getDataFieldInt(it, 0);
			poly -> yPoints[i] = api -> getDataFieldInt(it, 1);

			poly -> xPoints_si[i] = api -> getDataFieldInt(it, 0);
			poly -> yPoints_si[i] = api -> getDataFieldInt(it, 1);
			}
		
		poly -> nPoints = nPoints;

		poly -> thickness = thickness;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_polygon_bezier(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		DanaEl* points = api -> getParamEl(cframe, 1);
		size_t isteps = api -> getParamInt(cframe, 2);
		unsigned char r = api -> getParamRaw(cframe, 3)[0];
		unsigned char g = api -> getParamRaw(cframe, 4)[0];
		unsigned char b = api -> getParamRaw(cframe, 5)[0];
		unsigned char a = api -> getParamRaw(cframe, 6)[0];

		// -- create the container --

		UIPolygon *poly = (UIPolygon*) malloc(sizeof(UIPolygon));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));
		
		addUIObject(instance, uio);

		uio -> type = UI_TYPE_POLYGON_BEZ;
		uio -> object = poly;

		size_t nPoints = api -> getArrayLength(points);

		poly -> xPoints = malloc(sizeof(double) * nPoints);
		poly -> yPoints = malloc(sizeof(double) * nPoints);

		poly -> xPoints_si = NULL;
		poly -> yPoints_si = NULL;

		poly -> xPoints_si = malloc(sizeof(short int) * nPoints);
		poly -> yPoints_si = malloc(sizeof(short int) * nPoints);

		int i = 0;
		for (i = 0; i < nPoints; i++)
			{
			DanaEl* it = api -> getArrayCellEl(points, i);

			poly -> xPoints[i] = api -> getDataFieldInt(it, 0);
			poly -> yPoints[i] = api -> getDataFieldInt(it, 1);

			poly -> xPoints_si[i] = api -> getDataFieldInt(it, 0);
			poly -> yPoints_si[i] = api -> getDataFieldInt(it, 1);
			}
		
		poly -> nPoints = nPoints;

		poly -> isteps = isteps;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_polygon_bezier_outline(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		DanaEl* points = api -> getParamEl(cframe, 1);
		size_t isteps = api -> getParamInt(cframe, 2);
		size_t thickness = api -> getParamInt(cframe, 3);
		unsigned char r = api -> getParamRaw(cframe, 4)[0];
		unsigned char g = api -> getParamRaw(cframe, 5)[0];
		unsigned char b = api -> getParamRaw(cframe, 6)[0];
		unsigned char a = api -> getParamRaw(cframe, 7)[0];

		// -- create the container --

		UIPolygon *poly = (UIPolygon*) malloc(sizeof(UIPolygon));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));
		
		addUIObject(instance, uio);

		uio -> type = UI_TYPE_POLYGON_BEZ_OUT;
		uio -> object = poly;

		size_t nPoints = api -> getArrayLength(points);

		poly -> xPoints = malloc(sizeof(double) * nPoints);
		poly -> yPoints = malloc(sizeof(double) * nPoints);

		poly -> xPoints_si = malloc(sizeof(short int) * nPoints);
		poly -> yPoints_si = malloc(sizeof(short int) * nPoints);

		int i = 0;
		for (i = 0; i < nPoints; i++)
			{
			DanaEl* it = api -> getArrayCellEl(points, i);

			poly -> xPoints[i] = api -> getDataFieldInt(it, 0);
			poly -> yPoints[i] = api -> getDataFieldInt(it, 1);

			poly -> xPoints_si[i] = api -> getDataFieldInt(it, 0);
			poly -> yPoints_si[i] = api -> getDataFieldInt(it, 1);
			}
		
		poly -> nPoints = nPoints;

		poly -> isteps = isteps;

		poly -> thickness = thickness;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_curve(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		DanaEl* points = api -> getParamEl(cframe, 1);
		size_t isteps = api -> getParamInt(cframe, 2);
		size_t thickness = api -> getParamInt(cframe, 3);
		unsigned char r = api -> getParamRaw(cframe, 4)[0];
		unsigned char g = api -> getParamRaw(cframe, 5)[0];
		unsigned char b = api -> getParamRaw(cframe, 6)[0];
		unsigned char a = api -> getParamRaw(cframe, 7)[0];

		// -- create the container --

		UIPolygon *poly = (UIPolygon*) malloc(sizeof(UIPolygon));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));
		
		addUIObject(instance, uio);

		uio -> type = UI_TYPE_CURVE_BEZ;
		uio -> object = poly;

		size_t nPoints = api -> getArrayLength(points);

		poly -> xPoints = malloc(sizeof(double) * nPoints);
		poly -> yPoints = malloc(sizeof(double) * nPoints);

		poly -> xPoints_si = malloc(sizeof(short int) * nPoints);
		poly -> yPoints_si = malloc(sizeof(short int) * nPoints);

		int i = 0;
		for (i = 0; i < nPoints; i++)
			{
			DanaEl* it = api -> getArrayCellEl(points, i);

			poly -> xPoints[i] = api -> getDataFieldInt(it, 0);
			poly -> yPoints[i] = api -> getDataFieldInt(it, 1);

			poly -> xPoints_si[i] = api -> getDataFieldInt(it, 0);
			poly -> yPoints_si[i] = api -> getDataFieldInt(it, 1);
			}
		
		poly -> nPoints = nPoints;

		poly -> isteps = isteps;
		poly -> thickness = thickness;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_line(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t sx = api -> getParamInt(cframe, 1);
		size_t sy = api -> getParamInt(cframe, 2);
		size_t ex = api -> getParamInt(cframe, 3);
		size_t ey = api -> getParamInt(cframe, 4);
		size_t thick = api -> getParamInt(cframe, 5);
		size_t r = api -> getParamRaw(cframe, 6)[0];
		size_t g = api -> getParamRaw(cframe, 7)[0];
		size_t b = api -> getParamRaw(cframe, 8)[0];
		size_t a = api -> getParamRaw(cframe, 9)[0];
		//size_t aa = api -> getParamRaw(cframe, 10)[0];

		// -- create the container --

		UILine *poly = (UILine*) malloc(sizeof(UILine));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));

		uio -> type = UI_TYPE_LINE;
		uio -> object = poly;

		poly -> x1 = sx;
		poly -> y1 = sy;

		poly -> x2 = ex;
		poly -> y2 = ey;
		poly -> thickness = thick;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;

		addUIObject(instance, uio);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_point(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = api -> getParamInt(cframe, 1);

		size_t y = api -> getParamInt(cframe, 2);

		size_t r = api -> getParamRaw(cframe, 3)[0];

		size_t g = api -> getParamRaw(cframe, 4)[0];

		size_t b = api -> getParamRaw(cframe, 5)[0];

		size_t a = api -> getParamRaw(cframe, 6)[0];

		// -- create the container --

		UIPoint *poly = (UIPoint*) malloc(sizeof(UIPoint));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));

		uio -> type = UI_TYPE_POINT;
		uio -> object = poly;

		poly -> x = x;
		poly -> y = y;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;

		addUIObject(instance, uio);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_bitmap(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;
		
		DanaEl* bitmapData = api -> getParamEl(cframe, 1);
		
		if (bitmapData == NULL)
			{
			api -> throwException(cframe, "null pointer");
			return RETURN_OK;
			}
		
		DanaEl *whData = api -> getDataFieldEl(bitmapData, 0);
		DanaEl *pixelArrayH = api -> getDataFieldEl(bitmapData, 1);
		
		if (whData == NULL || pixelArrayH == NULL || api -> getArrayContent(pixelArrayH) == NULL)
			{
			api -> throwException(cframe, "null pointer");
			return RETURN_OK;
			}

		size_t x = api -> getParamInt(cframe, 3);

		size_t y = api -> getParamInt(cframe, 4);
		
		size_t sWidth = api -> getParamInt(cframe, 5);

		size_t sHeight = api -> getParamInt(cframe, 6);
		
		size_t rotation = api -> getParamInt(cframe, 7);

		// -- create the container --

		UIBitmap *poly = (UIBitmap*) malloc(sizeof(UIBitmap));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));

		uio -> type = UI_TYPE_BITMAP;
		uio -> object = poly;

		poly -> x = x;
		poly -> y = y;
		poly -> rotation = rotation;

		addUIObject(instance, uio);

		GenerateBitmapSurfaceData *gb = malloc(sizeof(GenerateBitmapSurfaceData));
		memset(gb, '\0', sizeof(GenerateBitmapSurfaceData));
		gb -> sourceData = api -> getParamEl(cframe, 1);
		gb -> vframe = cframe;
		gb -> bitmapData = poly;
		gb -> scaledHeight = sHeight;
		gb -> scaledWidth = sWidth;
		
		#ifdef WINDOWS
		gb -> sem = CreateSemaphore(NULL, 0, 1, NULL);
		#endif
		#ifdef OSX
		dispatch_semaphore_t *sem;
		sem = &gb -> sem;
		*sem = dispatch_semaphore_create(0);
		#else
		#ifdef LINUX
		sem_init(&gb -> sem, 0, 0);
		#endif
		#endif

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_GENERATE_BITMAP_SURFACE;
		newEvent.user.data1 = gb;

		SDL_PushEvent(&newEvent);
		
		#ifdef WINDOWS
		WaitForSingleObject(gb -> sem, INFINITE);
		#endif
		#ifdef OSX
		dispatch_semaphore_wait(gb -> sem, DISPATCH_TIME_FOREVER);
		#else
		#ifdef LINUX
		sem_wait(&gb -> sem);
		#endif
		#endif

		#ifdef WINDOWS
		CloseHandle(gb -> sem);
		#endif
		#ifdef OSX
		dispatch_release(gb -> sem);
		#else
		#ifdef LINUX
		sem_destroy(&gb -> sem);
		#endif
		#endif
		
		free(gb);

		return RETURN_OK;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_text_with(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t font_hnd = 0;
		memcpy(&font_hnd, api -> getParamRaw(cframe, 1), sizeof(size_t));
		FontHolder *fHold = (FontHolder*) font_hnd;
		
		lockedInc(&fHold -> refCount);

		size_t x = api -> getParamInt(cframe, 2);
		size_t y = api -> getParamInt(cframe, 3);
		size_t rotation = api -> getParamInt(cframe, 4);
		DanaEl* array = api -> getParamEl(cframe, 5);
		size_t r = api -> getParamRaw(cframe, 6)[0];
		size_t g = api -> getParamRaw(cframe, 7)[0];
		size_t b = api -> getParamRaw(cframe, 8)[0];
		size_t a = api -> getParamRaw(cframe, 9)[0];

		size_t tlen = api -> getArrayLength(array);

		if (tlen > 0)
			{
			// -- create the container --

			UIText *poly = (UIText*) malloc(sizeof(UIText));
			UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
			memset(uio, '\0', sizeof(UIObject));
			
			uio -> type = UI_TYPE_TEXT;
			uio -> object = poly;

			poly -> font = fHold;

			poly -> textLen = tlen;
			poly -> text = (char*) malloc(tlen + 1);
			memset(poly -> text, '\0', tlen + 1);
			memcpy(poly -> text, api -> getArrayContent(array), tlen);

			poly -> x = x;
			poly -> y = y;

			poly -> r = r;
			poly -> g = g;
			poly -> b = b;
			poly -> a = a;
			
			poly -> rotation = rotation;

			addUIObject(instance, uio);
			}
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_text_width_with(FrameData* cframe)
	{
	size_t font_hnd = 0;
	memcpy(&font_hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));
	FontHolder *fHold = (FontHolder*) font_hnd;
	TTF_Font *font = fHold -> font;

	DanaEl* array = api -> getParamEl(cframe, 1);

	size_t tlen = api -> getArrayLength(array);

	size_t width = 0;

	if (tlen > 0)
		{
		//note: we could avoid this malloc if we wrote an extra SDL_ttf function which took the (UTF-8) string length as a parameter...
		char *text = (char*) malloc(tlen + 1);
		memset(text, '\0', tlen + 1);
		memcpy(text, api -> getArrayContent(array), tlen);

		int count = 0;
		int pixels = 0;
		startCriticalSection(&measureTextLock);
		TTF_MeasureUTF8(font, text, INT_MAX, &pixels, &count);
		stopCriticalSection(&measureTextLock);

		api -> returnInt(cframe, pixels);

		free(text);

		return RETURN_OK;
		}
	
	api -> returnInt(cframe, width);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_push_surface(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		UISurface *newSurface = malloc(sizeof(UISurface));
		memset(newSurface, '\0', sizeof(UISurface));

		UIObject *newObject = malloc(sizeof(UIObject));
		memset(newObject, '\0', sizeof(UIObject));

		UIObject *newStackObject = malloc(sizeof(UIObject));
		memset(newStackObject, '\0', sizeof(UIObject));

		newObject -> object = newSurface;
		newStackObject -> object = newSurface;

		// --
		size_t x = api -> getParamInt(cframe, 1);
		size_t y = api -> getParamInt(cframe, 2);
		size_t w = api -> getParamInt(cframe, 3);
		size_t h = api -> getParamInt(cframe, 4);
		size_t xs = api -> getParamInt(cframe, 5);
		size_t ys = api -> getParamInt(cframe, 6);
		size_t a = api -> getParamRaw(cframe, 7)[0];
		// --

		newObject -> type = UI_TYPE_SURFACE;

		newSurface -> x = x;
		newSurface -> y = y;
		newSurface -> width = w;
		newSurface -> height = h;
		
		newSurface -> a = a;

		newSurface -> xScroll = xs;
		newSurface -> yScroll = ys;

		addUIObject(instance, newObject);

		newStackObject -> next = instance -> surfaceStack;
		instance -> surfaceStack = newStackObject;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_pop_surface(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		UIObject *so = instance -> surfaceStack;

		if (so != NULL)
			{
			instance -> surfaceStack = so -> next;

			free(so);
			}
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_font_metrics(FrameData* cframe)
	{
	size_t font_hnd = 0;
	memcpy(&font_hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	FontHolder *fHold = (FontHolder*) font_hnd;
	TTF_Font *font = fHold -> font;
	
	DanaEl* mdata = api -> getParamEl(cframe, 1);

	int sdl_x;

	//TODO: don't use any TTF_ functions outside of the main rendering loop?

	// -- --

	sdl_x = TTF_FontHeight(font);
	
	api -> setDataFieldInt(mdata, 0, sdl_x);
	
	sdl_x = TTF_FontAscent(font);
	
	api -> setDataFieldInt(mdata, 1, sdl_x);

	sdl_x = TTF_FontDescent(font);
	
	api -> setDataFieldInt(mdata, 2, sdl_x * -1);

	sdl_x = TTF_FontLineSkip(font);
	
	api -> setDataFieldInt(mdata, 3, sdl_x);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_font_name(FrameData* cframe)
	{
	size_t font_hnd = 0;
	memcpy(&font_hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));
	FontHolder *fHold = (FontHolder*) font_hnd;
	TTF_Font *font = fHold -> font;

	//TODO: don't use any TTF_ functions outside of the main rendering loop?

	char *val = TTF_FontFaceFamilyName(font);

	if (val != NULL)
		{
		returnByteArray(cframe, (unsigned char*) strdup(val), strlen(val));
    	}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_is_font_fixed_width(FrameData* cframe)
	{
	size_t font_hnd = 0;
	memcpy(&font_hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));
	FontHolder *fHold = (FontHolder*) font_hnd;
	TTF_Font *font = fHold -> font;

	//TODO: don't use any TTF_ functions outside of the main rendering loop?
	unsigned char is = TTF_FontFaceIsFixedWidth(font) == 0 ? 0 : 1;
	api -> returnRaw(cframe, &is, 1);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_text_bitmap_with(FrameData* cframe)
	{
	size_t font_hnd = 0;
	memcpy(&font_hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));
	FontHolder *fHold = (FontHolder*) font_hnd;
	TTF_Font *font = fHold -> font;

	DanaEl* array = api -> getParamEl(cframe, 1);

	size_t tlen = api -> getArrayLength(array);
	
	DanaEl* pixelcnt = api -> getParamEl(cframe, 2);

	size_t r = api -> getParamRaw(cframe, 3)[0];

	size_t g = api -> getParamRaw(cframe, 4)[0];

	size_t b = api -> getParamRaw(cframe, 5)[0];

	size_t a = api -> getParamRaw(cframe, 6)[0];

	if (tlen > 0)
		{
		char *text = (char*) malloc(tlen + 1);
		memset(text, '\0', tlen + 1);
		memcpy(text, api -> getArrayContent(array), tlen);

		//render the text to a surface as normal
		// - then get the w/h of the surface and iterate through its pixels

		GenerateTextBitmapData *gb = malloc(sizeof(GenerateTextBitmapData));
		memset(gb, '\0', sizeof(GenerateTextBitmapData));
		gb -> text = text;
		gb -> vframe = cframe;
		gb -> resultData = pixelcnt;
		gb -> font = font;
		gb -> color.r = r;
		gb -> color.g = g;
		gb -> color.b = b;
		gb -> color.a = a;

		#ifdef WINDOWS
		gb -> sem = CreateSemaphore(NULL, 0, 1, NULL);
		#endif
		#ifdef OSX
		dispatch_semaphore_t *sem;
		sem = &gb -> sem;
		*sem = dispatch_semaphore_create(0);
		#else
		#ifdef LINUX
		sem_init(&gb -> sem, 0, 0);
		#endif
		#endif
		
		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_GENERATE_TEXT_BITMAP;
		newEvent.user.data1 = gb;

		SDL_PushEvent(&newEvent);
		
		#ifdef WINDOWS
		WaitForSingleObject(gb -> sem, INFINITE);
		#endif
		#ifdef OSX
		dispatch_semaphore_wait(gb -> sem, DISPATCH_TIME_FOREVER);
		#else
		#ifdef LINUX
		sem_wait(&gb -> sem);
		#endif
		#endif

		#ifdef WINDOWS
		CloseHandle(gb -> sem);
		#endif
		#ifdef OSX
		dispatch_release(gb -> sem);
		#else
		#ifdef LINUX
		sem_destroy(&gb -> sem);
		#endif
		#endif
		
		free(gb);

		return RETURN_OK;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_commit_buffer(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_SWAP_BUFFERS_EVENT;
		newEvent.user.data1 = instance;
		newEvent.user.data2 = instance -> backBuffer;

		instance -> backBuffer = NULL;
		instance -> backBufferEnd = NULL;

		SDL_PushEvent(&newEvent);
		}

	return RETURN_OK;
	}

static void pushMouseEvent(WindowInstance *w, size_t type, size_t button_id, size_t x, size_t y, size_t ext1, size_t ext2)
	{
	DanaEl *nd = api -> makeData(windowDataGT);
	
	api -> setDataFieldInt(nd, 0, button_id);
	api -> setDataFieldInt(nd, 1, x);
	api -> setDataFieldInt(nd, 2, y);
	api -> setDataFieldInt(nd, 3, ext1);
	api -> setDataFieldInt(nd, 4, ext2);
	
	api -> pushEvent(w -> eqObject, 0, type, nd);
	}

static void normalisePath(char *p)
	{
	char *q = NULL;
	while ((q = strchr(p, '\\')) != NULL) q[0] = '/';
	}

static void pushDropEvent(WindowInstance *w, char* path, size_t x, size_t y)
	{
	DanaEl* nd = api -> makeData(dropDataGT);
	
	api -> setDataFieldInt(nd, 0, x);
	api -> setDataFieldInt(nd, 1, y);
	
	size_t asz = strlen(path);
	unsigned char* cnt = NULL;
	DanaEl* na = api -> makeArray(charArrayGT, asz, &cnt);
	memcpy(cnt, path, asz);
	normalisePath((char*) cnt);
	
	api -> setDataFieldEl(nd, 2, na);
	
	api -> pushEvent(w -> eqObject, 0, 9, nd);
	}

static void pushEvent(WindowInstance *w, size_t type)
	{
	api -> pushEvent(w -> eqObject, 0, type, NULL);
	}

INSTRUCTION_DEF op_set_size(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = api -> getParamInt(cframe, 1);

		size_t y = api -> getParamInt(cframe, 2);
		
		instance -> windowedWidth = x;
		instance -> windowedHeight = y;
		
		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_SET_WINDOW_SIZE;
		newEvent.user.data1 = instance;

		SDL_PushEvent(&newEvent);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_position(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = api -> getParamInt(cframe, 1);

		size_t y = api -> getParamInt(cframe, 2);

		instance -> windowX = x;
		instance -> windowY = y;

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_SET_WINDOW_POSITION;
		newEvent.user.data1 = instance;

		SDL_PushEvent(&newEvent);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_visible(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		unsigned char v = api -> getParamRaw(cframe, 1)[0];

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = v ? DX_SHOW_WINDOW : DX_HIDE_WINDOW;
		newEvent.user.data1 = instance;

		SDL_PushEvent(&newEvent);
		
		#ifdef WINDOWS
		WaitForSingleObject(instance -> stateSem, INFINITE);
		#endif
		#ifdef OSX
		dispatch_semaphore_wait(instance -> stateSem, DISPATCH_TIME_FOREVER);
		#else
		#ifdef LINUX
		sem_wait(&instance -> stateSem);
		#endif
		#endif
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_resizable(FrameData* cframe)
	{
	if (!resizeAvailable)
		{
		api -> throwException(cframe, "function unavailable on installed version of SDL");
		return RETURN_OK;
		}
	
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		unsigned char v = api -> getParamRaw(cframe, 1)[0];

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = v ? DX_RESIZABLE_WINDOW : DX_FIXED_SIZE_WINDOW;
		newEvent.user.data1 = instance;

		SDL_PushEvent(&newEvent);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_fullscreen(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		unsigned char v = api -> getParamRaw(cframe, 1)[0];

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = v ? DX_FULLSCREEN_WINDOW : DX_WINDOWED_WINDOW;
		newEvent.user.data1 = instance;

		SDL_PushEvent(&newEvent);
		
		#ifdef WINDOWS
		WaitForSingleObject(instance -> stateSem, INFINITE);
		#endif
		#ifdef OSX
		dispatch_semaphore_wait(instance -> stateSem, DISPATCH_TIME_FOREVER);
		#else
		#ifdef LINUX
		sem_wait(&instance -> stateSem);
		#endif
		#endif
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_resolution(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_GET_RESOLUTION;
		newEvent.user.data1 = instance;
		newEvent.user.data2 = api -> getParamEl(cframe, 1);

		SDL_PushEvent(&newEvent);
		
		#ifdef WINDOWS
		WaitForSingleObject(instance -> stateSem, INFINITE);
		#endif
		#ifdef OSX
		dispatch_semaphore_wait(instance -> stateSem, DISPATCH_TIME_FOREVER);
		#else
		#ifdef LINUX
		sem_wait(&instance -> stateSem);
		#endif
		#endif
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_title(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;
		
		DanaEl* array = api -> getParamEl(cframe, 1);
		
		size_t tlen = api -> getArrayLength(array);
		
		char *text = NULL;

		if (array != NULL)
			{
			text = (char*) malloc(tlen + 1);
			memset(text, '\0', tlen + 1);
			memcpy((unsigned char*) text, api -> getArrayContent(array), tlen);
			}
			else
			{
			text = strdup("");
			}

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_SET_WINDOW_TITLE;
		newEvent.user.data1 = instance;
		newEvent.user.data2 = text;

		SDL_PushEvent(&newEvent);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_icon(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;
		
		DanaEl* bitmapData = api -> getParamEl(cframe, 1);
		
		if (bitmapData == NULL)
			{
			api -> throwException(cframe, "null pointer");
			return RETURN_OK;
			}
		
		DanaEl* whData = api -> getDataFieldEl(bitmapData, 0);
		DanaEl* pixelArrayH = api -> getDataFieldEl(bitmapData, 1);
		
		if (whData == NULL || pixelArrayH == NULL || api -> getArrayContent(pixelArrayH) == NULL)
			{
			api -> throwException(cframe, "null pointer");
			return RETURN_OK;
			}
		
		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_SET_WINDOW_ICON;
		newEvent.user.data1 = instance;
		newEvent.user.data2 = bitmapData;

		SDL_PushEvent(&newEvent);
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_cursor(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		unsigned char cursorType = api -> getParamRaw(cframe, 1)[0];
		DanaEl* cursorData = api -> getParamEl(cframe, 2);

		SetCursorInstance* sci = malloc(sizeof(SetCursorInstance));

		sci -> type = cursorType;

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_SET_WINDOW_CURSOR;
		newEvent.user.data1 = instance;
		newEvent.user.data2 = sci;

		if (cursorData != NULL)
			{
			api -> incRef(cframe, cursorData);
			sci -> customCursor = cursorData;
			}

		SDL_PushEvent(&newEvent);
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_maximise_window(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_MAXIMISE_WINDOW;
		newEvent.user.data1 = instance;

		SDL_PushEvent(&newEvent);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_minimise_window(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_MINIMISE_WINDOW;
		newEvent.user.data1 = instance;

		SDL_PushEvent(&newEvent);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_maximised_screen_rect(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		//WindowInstance *instance = (WindowInstance*) hnd;
		
		DanaEl* rdata = api -> getParamEl(cframe, 1);
		
		#ifdef WINDOWS
		//NOTE: below only works for primary monitor - see GetMonitorInfo for other monitors (not sure how we know which monitor a window is considered to be "on")
		RECT r;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);
		
		api -> setDataFieldInt(rdata, 0, r.left);
		api -> setDataFieldInt(rdata, 1, r.top);
		api -> setDataFieldInt(rdata, 2, r.right - r.left);
		api -> setDataFieldInt(rdata, 3, r.bottom - r.top);
		#endif
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_pixels(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;
		
		GetPixelsInfo *mwInfo = malloc(sizeof(GetPixelsInfo));
		memset(mwInfo, '\0', sizeof(GetPixelsInfo));

		#ifdef WINDOWS
		mwInfo -> sem = CreateSemaphore(NULL, 0, 1, NULL);
		#endif
		#ifdef OSX
		dispatch_semaphore_t *sem;
		sem = &mwInfo -> sem;
		*sem = dispatch_semaphore_create(0);
		#else
		#ifdef LINUX
		sem_init(&mwInfo -> sem, 0, 0);
		#endif
		#endif
		
		mwInfo -> wi = instance;

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_GET_PIXELS;
		newEvent.user.data1 = mwInfo;
		newEvent.user.data2 = cframe;

		SDL_PushEvent(&newEvent);
		
		#ifdef WINDOWS
		WaitForSingleObject(mwInfo -> sem, INFINITE);
		#endif
		#ifdef OSX
		dispatch_semaphore_wait(mwInfo -> sem, DISPATCH_TIME_FOREVER);
		#else
		#ifdef LINUX
		sem_wait(&mwInfo -> sem);
		#endif
		#endif
		
		//semaphore_destroy();

		#ifdef WINDOWS
		CloseHandle(mwInfo -> sem);
		#endif
		#ifdef OSX
		dispatch_release(mwInfo -> sem);
		#else
		#ifdef LINUX
		sem_destroy(&mwInfo -> sem);
		#endif
		#endif

		//create the return value (a byte array)
		DanaEl* el = api -> makeData(pixelMapGT);
		DanaEl* elWH = api -> makeData(whGT);

		unsigned char* content = NULL;
		DanaEl* array = api -> makeArray(charArrayGT, mwInfo -> width * mwInfo -> height * 4, &content);
		memcpy(content, mwInfo -> pixels, mwInfo -> width * mwInfo -> height * 4);

		api -> setDataFieldInt(elWH, 0, mwInfo -> width);
		api -> setDataFieldInt(elWH, 1, mwInfo -> height);

		api -> setDataFieldEl(el, 0, elWH);
		api -> setDataFieldEl(el, 1, array);

		free(mwInfo -> pixels);
		free(mwInfo);

		api -> returnEl(cframe, el);
		
		return RETURN_OK;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_close_window(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;
		
		CloseWindowInfo *mwInfo = malloc(sizeof(CloseWindowInfo));
		memset(mwInfo, '\0', sizeof(CloseWindowInfo));

		#ifdef WINDOWS
		mwInfo -> sem = CreateSemaphore(NULL, 0, 1, NULL);
		#endif
		#ifdef OSX
		dispatch_semaphore_t *sem;
		sem = &mwInfo -> sem;
		*sem = dispatch_semaphore_create(0);
		#else
		#ifdef LINUX
		sem_init(&mwInfo -> sem, 0, 0);
		#endif
		#endif
		
		mwInfo -> wi = instance;

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_CLOSE_WINDOW;
		newEvent.user.data1 = mwInfo;
		newEvent.user.data2 = cframe;

		SDL_PushEvent(&newEvent);
		
		#ifdef WINDOWS
		WaitForSingleObject(mwInfo -> sem, INFINITE);
		#endif
		#ifdef OSX
		dispatch_semaphore_wait(mwInfo -> sem, DISPATCH_TIME_FOREVER);
		#else
		#ifdef LINUX
		sem_wait(&mwInfo -> sem);
		#endif
		#endif
		
		//semaphore_destroy();

		#ifdef WINDOWS
		CloseHandle(mwInfo -> sem);
		#endif
		#ifdef OSX
		dispatch_release(mwInfo -> sem);
		#else
		#ifdef LINUX
		sem_destroy(&mwInfo -> sem);
		#endif
		#endif
		
		free(mwInfo);
		
		return RETURN_OK;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_background_colour(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t r = api -> getParamRaw(cframe, 1)[0];

		size_t g = api -> getParamRaw(cframe, 2)[0];

		size_t b = api -> getParamRaw(cframe, 3)[0];

		//size_t a = api -> getParamRaw(cframe, 4)[0];

		instance -> backgroundColour.r = r;
		instance -> backgroundColour.g = g;
		instance -> backgroundColour.b = b;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_load_font(FrameData* cframe)
	{
	if (!initComplete)
		{
		api -> throwException(cframe, "rendering framework has not been initialised");
		return RETURN_OK;
		}
	
	DanaEl* array = api -> getParamEl(cframe, 0);
	
	if (array == NULL)
		{
		api -> throwException(cframe, "font '' not found");
		return RETURN_OK;
		}
	
	size_t tlen = api -> getArrayLength(array);

	char *path = (char*) malloc(tlen + 1);
	memset(path, '\0', tlen + 1);
	memcpy(path, api -> getArrayContent(array), tlen);
	
	size_t sz = api -> getParamInt(cframe, 1);

	//load the typeface...
	char *fontPath = malloc(2048);
	memset(fontPath, '\0', 2048);

	if (!findFont(path, fontPath, 2048))
		{
		api -> throwException(cframe, "open font error: could not find font");
		free(fontPath);
		//return of 0 is automatic
		}
		else
		{
		LoadFontData *lfd = malloc(sizeof(LoadFontData));
		memset(lfd, '\0', sizeof(LoadFontData));

		lfd -> fontPath = fontPath;
		lfd -> size = sz;
		lfd -> vframe = cframe;
		
		#ifdef WINDOWS
		lfd -> sem = CreateSemaphore(NULL, 0, 1, NULL);
		#endif
		#ifdef OSX
		dispatch_semaphore_t *sem;
		sem = &lfd -> sem;
		*sem = dispatch_semaphore_create(0);
		#else
		#ifdef LINUX
		sem_init(&lfd -> sem, 0, 0);
		#endif
		#endif
		
		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_LOAD_FONT;
		newEvent.user.data1 = lfd;
		
		SDL_PushEvent(&newEvent);
		
		#ifdef WINDOWS
		WaitForSingleObject(lfd -> sem, INFINITE);
		#endif
		#ifdef OSX
		dispatch_semaphore_wait(lfd -> sem, DISPATCH_TIME_FOREVER);
		#else
		#ifdef LINUX
		sem_wait(&lfd -> sem);
		#endif
		#endif
		
		#ifdef WINDOWS
		CloseHandle(lfd -> sem);
		#endif
		#ifdef OSX
		dispatch_release(lfd -> sem);
		#else
		#ifdef LINUX
		sem_destroy(&lfd -> sem);
		#endif
		#endif
		
		free(lfd);

		return RETURN_OK;
		}

	free(path);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_unload_font(FrameData* cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, api -> getParamRaw(cframe, 0), sizeof(size_t));
	
	if (hnd != 0)
		{
		FontHolder *font = (FontHolder*) hnd;
		
		LoadFontData *lfd = malloc(sizeof(LoadFontData));
		memset(lfd, '\0', sizeof(LoadFontData));

		lfd -> fontHandle = font;
		lfd -> vframe = cframe;
		
		#ifdef WINDOWS
		lfd -> sem = CreateSemaphore(NULL, 0, 1, NULL);
		#endif
		#ifdef OSX
		dispatch_semaphore_t *sem;
		sem = &lfd -> sem;
		*sem = dispatch_semaphore_create(0);
		#else
		#ifdef LINUX
		sem_init(&lfd -> sem, 0, 0);
		#endif
		#endif
		
		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_UNLOAD_FONT;
		newEvent.user.data1 = lfd;
		
		SDL_PushEvent(&newEvent);
		
		#ifdef WINDOWS
		WaitForSingleObject(lfd -> sem, INFINITE);
		#endif
		#ifdef OSX
		dispatch_semaphore_wait(lfd -> sem, DISPATCH_TIME_FOREVER);
		#else
		#ifdef LINUX
		sem_wait(&lfd -> sem);
		#endif
		#endif
		
		#ifdef WINDOWS
		CloseHandle(lfd -> sem);
		#endif
		#ifdef OSX
		dispatch_release(lfd -> sem);
		#else
		#ifdef LINUX
		sem_destroy(&lfd -> sem);
		#endif
		#endif
		
		free(lfd);

		return RETURN_OK;
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_init_media_layer(FrameData* cframe)
	{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_TIMER|SDL_INIT_JOYSTICK|SDL_INIT_GAMECONTROLLER|SDL_INIT_HAPTIC) != 0)
	//if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
		printf("SDL INIT FAILED\n");
		}

	if (TTF_Init() != 0)
		{
		//logSDLError(std::cout, "TTF_Init");
		}

	DX_NEW_WINDOW_EVENT = SDL_RegisterEvents(1);
	DX_SWAP_BUFFERS_EVENT = SDL_RegisterEvents(1);
	DX_SET_WINDOW_POSITION = SDL_RegisterEvents(1);
	DX_SET_WINDOW_TITLE = SDL_RegisterEvents(1);
	DX_SET_WINDOW_ICON = SDL_RegisterEvents(1);
	DX_SET_WINDOW_CURSOR = SDL_RegisterEvents(1);
	DX_MAXIMISE_WINDOW = SDL_RegisterEvents(1);
	DX_MINIMISE_WINDOW = SDL_RegisterEvents(1);
	DX_SHOW_WINDOW = SDL_RegisterEvents(1);
	DX_HIDE_WINDOW = SDL_RegisterEvents(1);
	DX_SET_WINDOW_SIZE = SDL_RegisterEvents(1);
	DX_RESIZABLE_WINDOW = SDL_RegisterEvents(1);
	DX_FIXED_SIZE_WINDOW = SDL_RegisterEvents(1);
	DX_FULLSCREEN_WINDOW = SDL_RegisterEvents(1);
	DX_WINDOWED_WINDOW = SDL_RegisterEvents(1);
	DX_GET_RESOLUTION = SDL_RegisterEvents(1);
	DX_GET_PIXELS = SDL_RegisterEvents(1);
	DX_CLOSE_WINDOW = SDL_RegisterEvents(1);
	DX_GENERATE_TEXT_BITMAP = SDL_RegisterEvents(1);
	DX_GENERATE_BITMAP_SURFACE = SDL_RegisterEvents(1);
	DX_LOAD_FONT = SDL_RegisterEvents(1);
	DX_UNLOAD_FONT = SDL_RegisterEvents(1);
	DX_SYSTEM_SHUTDOWN = SDL_RegisterEvents(1);
	
	systemEventObject = api -> getInstanceObject(cframe);
	
	#ifdef WINDOWS
	frameworkShutdownLock = CreateSemaphore(NULL, 0, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_t *sem;
    sem = &frameworkShutdownLock;
    *sem = dispatch_semaphore_create(0);
	#else
	#ifdef LINUX
	sem_init(&frameworkShutdownLock, 0, 0);
	#endif
	#endif
	
	unsigned char ok = 1;
	api -> returnRaw(cframe, &ok, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_run_system_loop(FrameData* cframe)
	{
	render_thread();
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_shutdown(FrameData* cframe)
	{
	if (!initComplete)
		{
		api -> throwException(cframe, "rendering framework has not been initialised");
		return RETURN_OK;
		}
	
	SDL_Event newEvent;
	newEvent.type = DX_SYSTEM_SHUTDOWN;

	SDL_PushEvent(&newEvent);
	
	//wait for close-confirm (otherwise our event loop can fail to exit, if we call SDL_Quit() before it's received the shutdown event)
	#ifdef WINDOWS
	WaitForSingleObject(frameworkShutdownLock, INFINITE);
	#endif
	#ifdef OSX
	dispatch_semaphore_wait(frameworkShutdownLock, DISPATCH_TIME_FOREVER);
	#else
	#ifdef LINUX
	sem_wait(&frameworkShutdownLock);
	#endif
	#endif
	
	SDL_Quit();
	
	return RETURN_OK;
	}

/* ******************************************************************************** */
/* ******************************************************************************** */
/* ******************************************************************************** */
/* ******************************************************************************** */
/* ******************************* Direct Render API ****************************** */
/* ******************************************************************************** */
/* ******************************************************************************** */
/* ******************************************************************************** */
/* ******************************************************************************** */
/* ******************************************************************************** */

const int MAX_EVENTS = 128;

typedef struct _surf_el {
	SDL_Texture* surface;
	size_t xPos;
	size_t yPos;
	size_t xScroll;
	size_t yScroll;
	struct _surf_el* next;
} SurfaceElement;

typedef struct {
	SDL_Window *win;
	SDL_Renderer *renderer;

	FPSmanager fpsManager;

	SDL_GLContext glContext;

	SDL_Cursor* currentCursor;
	
	bool visible;
	bool fullScreen;

	int windowWidth;
	int windowHeight;

	DanaEl* eventList;
	DanaEl** eventElements;
	unsigned char** eventContents;

	SurfaceElement* surfaces;

	size_t xScroll;
	size_t yScroll;

	bool mode3D;

} FlowInstance;

INSTRUCTION_DEF op_flow_init_media_layer(FrameData* cframe)
	{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS|SDL_INIT_TIMER|SDL_INIT_JOYSTICK|SDL_INIT_GAMECONTROLLER|SDL_INIT_HAPTIC) != 0)
		{
		api -> throwException(cframe, "SDL initialisation failed");
		return RETURN_OK;
		}

	if (TTF_Init() != 0)
		{
		api -> throwException(cframe, "TTF initialisation failed");
		return RETURN_OK;
		}
	
	unsigned char ok = 1;
	api -> returnRaw(cframe, &ok, 1);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_make_window(FrameData* cframe)
	{
	FlowInstance* instance = malloc(sizeof(FlowInstance));
	memset(instance, 0, sizeof(FlowInstance));

	size_t framerate = api -> getParamInt(cframe, 0);

	instance -> windowWidth = 640;
	instance -> windowHeight = 480;

	instance -> mode3D = api -> getParamRaw(cframe, 1)[0];

	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	instance -> win = SDL_CreateWindow("Dana UI", 100, 100, instance -> windowWidth, instance -> windowHeight, SDL_WINDOW_HIDDEN);
	
	if (instance -> win == NULL){
		api -> throwException(cframe, "SDL window creation failed");
		return RETURN_OK;
	}

	if (!instance -> mode3D)
		{
		//attempt to create our preferred renderer
		instance -> renderer = SDL_CreateRenderer(instance -> win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);

		if (instance -> renderer == NULL)
			{
			//couldn't find a renderer matching our above preferences, so fall back on any available renderer
			//(this sometimes achieves highly suspect results - could be argued that we should just fail here instead...)
			instance -> renderer = SDL_CreateRenderer(instance -> win, -1, 0);
			}

		if (instance -> renderer == NULL)
			{
			api -> throwException(cframe, "SDL renderer creation failed");
			return RETURN_OK;
			}
		}
		else
		{
		instance -> glContext = SDL_GL_CreateContext(instance -> win);
		}
	
	if (framerate != 0)
		{
		SDL_initFramerate(&instance -> fpsManager);
		SDL_setFramerate(&instance -> fpsManager, framerate);
		}
	
	//instantiate & populate our event array, which we continuously re-use the instances of
	instance -> eventList = api -> makeArray(flowEventArrayGT, MAX_EVENTS, NULL);
	instance -> eventElements = malloc(sizeof(DanaEl*) * MAX_EVENTS);
	instance -> eventContents = malloc(sizeof(unsigned char*) * MAX_EVENTS);
	int i = 0;
	for (i = 0; i < MAX_EVENTS; i++)
		{
		DanaEl* ev = api -> makeData(flowEventGT);
		api -> setArrayCellEl(instance -> eventList, i, ev);
		instance -> eventElements[i] = ev;
		instance -> eventContents[i] = api -> getDataContent(ev);
		}
	
	api -> returnRaw(cframe, (unsigned char*) &instance, sizeof(void*));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_set_visible(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	unsigned char v = api -> getParamRaw(cframe, 1)[0];
	
	if (v)
		{
		SDL_ShowWindow(instance -> win);
		}
		else
		{
		SDL_HideWindow(instance -> win);
		}
	
	instance -> visible = v;

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_set_resizable(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	unsigned char v = api -> getParamRaw(cframe, 1)[0];

	SDL_SetWindowResizable(instance -> win, v);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_set_position(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	size_t x = api -> getParamInt(cframe, 1);
	size_t y = api -> getParamInt(cframe, 2);

	SDL_SetWindowPosition(instance -> win, x, y);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_set_size(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	size_t w = api -> getParamInt(cframe, 1);
	size_t h = api -> getParamInt(cframe, 2);

	SDL_SetWindowSize(instance -> win, w, h);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_set_fullscreen(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	unsigned char v = api -> getParamRaw(cframe, 1)[0];

	if (v)
		{
		//SDL_DisplayMode DM;
		//SDL_GetCurrentDisplayMode(0, &DM);
		
		//instance -> windowWidth = DM.w;
		//instance -> windowHeight = DM.h;

		#ifdef LINUX
		//TODO: for some reason this hide/show needed on Linux to allow rendering into the now-fullscreen window area
		//TODO: try removing this workaround in newer SDL releases, and retest visual/FullScreen.o to see if it works any better
		if (instance -> visible)
			{
			SDL_HideWindow(instance -> win);
			}
		#endif

		SDL_SetWindowFullscreen(instance -> win, SDL_WINDOW_FULLSCREEN_DESKTOP);
		
		#ifdef LINUX
		if (instance -> visible)
			{
			SDL_ShowWindow(instance -> win);
			}
		#endif
		
		instance -> fullScreen = true;
		}
		else
		{
		//wi -> windowWidth = wi -> windowedWidth;
		//wi -> windowHeight = wi -> windowedHeight;
		
		SDL_SetWindowFullscreen(instance -> win, 0);
		
		instance -> fullScreen = false;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_set_title(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	char *str = x_getParam_char_array(api, cframe, 1);

	SDL_SetWindowTitle(instance -> win, str);

	free(str);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_set_icon(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	DanaEl *pixelMap = api -> getParamEl(cframe, 1);
	
	SDL_Surface *surface = pixelMapToSurface(pixelMap);
	
	SDL_SetWindowIcon(instance -> win, surface);
	
	SDL_FreeSurface(surface);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_set_cursor(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	unsigned char cursorType = api -> getParamRaw(cframe, 1)[0];
	DanaEl* customCursor = api -> getParamEl(cframe, 2);

	SDL_Cursor* nc = NULL;

	if (cursorType == 0)
		{
		nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
		}
		else if (cursorType == 1)
		{
		SDL_ShowCursor(SDL_DISABLE);
		}
		else if (cursorType == 2)
		{
		//a custom-provided image, with an x/y hotpoint
		DanaEl* pixelMap = api -> getDataFieldEl(customCursor, 0);
		SDL_Surface *surface = pixelMapToSurface(pixelMap);

		nc = SDL_CreateColorCursor(surface, api -> getDataFieldInt(customCursor, 1), api -> getDataFieldInt(customCursor, 2));

		SDL_FreeSurface(surface);
		}
		else if (cursorType == 3)
		{
		nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
		}
		else if (cursorType == 4)
		{
		nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
		}
		else if (cursorType == 5)
		{
		nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
		}
		else if (cursorType == 6)
		{
		nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
		}
		else if (cursorType == 7)
		{
		nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
		}
		else if (cursorType == 8)
		{
		nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
		}
		else if (cursorType == 9)
		{
		nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
		}
		else if (cursorType == 10)
		{
		nc = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
		}
	
	if (cursorType != 1)
		{
		SDL_ShowCursor(SDL_ENABLE);
		}
	
	/*
	SDL_SYSTEM_CURSOR_CROSSHAIR
	SDL_SYSTEM_CURSOR_WAITARROW
	SDL_SYSTEM_CURSOR_NO
	*/
	
	if (nc != NULL)
		{
		SDL_SetCursor(nc);

		//if there's a cursor already set (and about to no longer be user), we need to use SDL_FreeCursor on that one
		if (instance -> currentCursor != NULL)
			{
			SDL_FreeCursor(instance -> currentCursor);
			}
		
		instance -> currentCursor = nc;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_close_window(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));
	
	SDL_DestroyRenderer(instance -> renderer);
	SDL_DestroyWindow(instance -> win);

	return RETURN_OK;
	}

const int EV_TYPE_QUIT = 1;
const int EV_TYPE_WINDOW = 2;
const int EV_TYPE_MOUSE = 3;
const int EV_TYPE_KEYBOARD = 4;

const int EV_MOUSE_DOWN = 1;
const int EV_MOUSE_UP = 2;
const int EV_MOUSE_MOVE = 3;
const int EV_MOUSE_WHEEL = 4;

const int EV_KEY_DOWN = 1;
const int EV_KEY_UP = 2;

const int EV_WINDOW_RESIZE = 1;
const int EV_WINDOW_MOVE = 2;

static void copyUint32(unsigned char* dest, unsigned char* src)
	{
	#ifdef MACHINE_ENDIAN_LITTLE
	dest[0] = src[3];
	dest[1] = src[2];
	dest[2] = src[1];
	dest[3] = src[0];
	#else
	dest[0] = src[0];
	dest[1] = src[1];
	dest[2] = src[2];
	dest[3] = src[3];
	#endif
	}

INSTRUCTION_DEF op_flow_get_events(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	int i = 0;
	SDL_Event e;
	while (SDL_PollEvent(&e))
		{
		DanaEl* el = instance -> eventElements[i];
		unsigned char* content = instance -> eventContents[i];

		i ++;

		if (e.type == SDL_QUIT)
			{
			content[3] = EV_TYPE_QUIT;
			}
			else if (e.type == SDL_RENDER_TARGETS_RESET || e.type == SDL_RENDER_DEVICE_RESET)
			{
			//
			}
			else if (e.type == SDL_WINDOWEVENT)
			{
			if (e.window.event == SDL_WINDOWEVENT_CLOSE)
				{
				content[3] = EV_TYPE_QUIT;
				}
				else if (e.window.event == SDL_WINDOWEVENT_RESIZED)
				{
				content[3] = EV_TYPE_WINDOW;
				content[7] = EV_WINDOW_RESIZE;

				Uint32 w = e.window.data1;
				Uint32 h = e.window.data2;

				copyUint32(&content[8], (unsigned char*) &w);
				copyUint32(&content[12], (unsigned char*) &h);
				}
				else if (e.window.event == SDL_WINDOWEVENT_MOVED)
				{
				content[3] = EV_TYPE_WINDOW;
				content[7] = EV_WINDOW_MOVE;

				Uint32 x = e.window.data1;
				Uint32 y = e.window.data2;

				copyUint32(&content[8], (unsigned char*) &x);
				copyUint32(&content[12], (unsigned char*) &y);
				}
			}
			else if (e.type == SDL_MOUSEBUTTONDOWN)
			{
			content[3] = EV_TYPE_MOUSE;
			content[7] = EV_MOUSE_DOWN;

			Uint32 screenX = e.button.x;
			Uint32 screenY = e.button.y;
			
			unsigned char button = 0;
			
			if (e.button.button == SDL_BUTTON_LEFT)
				button = 1;
				else if (e.button.button == SDL_BUTTON_RIGHT)
				button = 2;
				
			content[11] = button;
			
			copyUint32(&content[12], (unsigned char*) &screenX);
			copyUint32(&content[16], (unsigned char*) &screenY);
			}
			else if (e.type == SDL_MOUSEBUTTONUP)
			{
			content[3] = EV_TYPE_MOUSE;
			content[7] = EV_MOUSE_UP;

			Uint32 screenX = e.button.x;
			Uint32 screenY = e.button.y;
			
			unsigned char button = 0;
			
			if (e.button.button == SDL_BUTTON_LEFT)
				button = 1;
				else if (e.button.button == SDL_BUTTON_RIGHT)
				button = 2;
				
			content[11] = button;

			copyUint32(&content[12], (unsigned char*) &screenX);
			copyUint32(&content[16], (unsigned char*) &screenY);
			}
			else if (e.type == SDL_MOUSEMOTION)
			{
			content[3] = EV_TYPE_MOUSE;
			content[7] = EV_MOUSE_MOVE;

			if (e.motion.x < 0) e.motion.x = 0;
			if (e.motion.y < 0) e.motion.y = 0;

			Uint32 screenX = e.motion.x;
			Uint32 screenY = e.motion.y;

			copyUint32(&content[8], (unsigned char*) &screenX);
			copyUint32(&content[12], (unsigned char*) &screenY);
			}
			else if (e.type == SDL_MOUSEWHEEL)
			{
			content[3] = EV_TYPE_MOUSE;
			content[7] = EV_MOUSE_WHEEL;

			int deltaX = e.wheel.x;
			int deltaY = e.wheel.y;

			size_t xAdd = 0;
			size_t xSub = 0;
			size_t yAdd = 0;
			size_t ySub = 0;

			if (deltaX < 0)
				xSub = deltaX * -1;
				else
				xAdd = deltaX;
			
			if (deltaY < 0)
				ySub = deltaY * -1;
				else
				yAdd = deltaY;
			
			copyUint32(&content[8], (unsigned char*) &xSub);
			copyUint32(&content[12], (unsigned char*) &xAdd);
			copyUint32(&content[16], (unsigned char*) &ySub);
			copyUint32(&content[20], (unsigned char*) &yAdd);
			}
			else if (e.type == SDL_KEYDOWN)
			{
			content[3] = EV_TYPE_KEYBOARD;
			content[7] = EV_KEY_DOWN;

			Uint32 keyID = e.key.keysym.scancode;

			copyUint32(&content[8], (unsigned char*) &keyID);
			}
			else if (e.type == SDL_KEYUP)
			{
			content[3] = EV_TYPE_KEYBOARD;
			content[7] = EV_KEY_UP;

			Uint32 keyID = e.key.keysym.scancode;

			copyUint32(&content[8], (unsigned char*) &keyID);
			}
			else if (e.type == SDL_DROPFILE)
			{
			//
			}

		if (i + 1 >= MAX_EVENTS)
			break;
		}
	
	api -> setArrayLength(instance -> eventList, i);

	api -> returnEl(cframe, instance -> eventList);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_render_begin(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));
	
	if (!instance -> mode3D)
		{
		SDL_SetRenderDrawColor(instance -> renderer, 0, 0, 0, 255);
		
		SDL_SetRenderDrawBlendMode(instance -> renderer, SDL_BLENDMODE_BLEND);
		
		SDL_RenderClear(instance -> renderer);
		
		SDL_SetRenderTarget(instance -> renderer, NULL);
		}
		else
		{
		//glViewport(0, 0, instance -> windowWidth, instance -> windowHeight);
    	//glClearColor(1.f, 0.f, 1.f, 0.f);
    	//glClear(GL_COLOR_BUFFER_BIT);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_render_end(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));
	
	if (!instance -> mode3D)
		{
		SDL_RenderPresent(instance -> renderer);
		}
		else
		{
		//SDL_GL_SwapWindow(instance -> win);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_wait(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));
	
	SDL_framerateDelay(&instance -> fpsManager);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_get_pixels(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	int w, h;
	SDL_GetWindowSize(instance -> win, &w, &h);

	DanaEl* el = api -> makeData(pixelMapGT);
	DanaEl* elWH = api -> makeData(whGT);

	unsigned char* content = NULL;
	DanaEl* array = api -> makeArray(charArrayGT, w * h * 4, &content);

	api -> setDataFieldInt(elWH, 0, w);
	api -> setDataFieldInt(elWH, 1, h);

	api -> setDataFieldEl(el, 0, elWH);
	api -> setDataFieldEl(el, 1, array);

	/*
	SDL_Texture* testTexture = SDL_CreateTexture(instance -> renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
	SDL_QueryTexture(testTexture, NULL, NULL, &w, &h);
	SDL_DestroyTexture(testTexture);
	*/
	
	SDL_Rect dst;
	dst.x = 0;
	dst.y = 0;
	dst.w = w;
	dst.h = h;

	SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
	SDL_RenderReadPixels(instance -> renderer, &dst, SDL_PIXELFORMAT_ABGR8888, surface -> pixels, surface -> pitch);
	memcpy(content, surface -> pixels, w * h * 4);
	SDL_FreeSurface(surface);

	api -> returnEl(cframe, el);

	return RETURN_OK;
	}

// -- 2D drawing commands --

INSTRUCTION_DEF op_flow_point(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	size_t x = api -> getParamInt(cframe, 1);
	size_t y = api -> getParamInt(cframe, 2);
	unsigned char r = api -> getParamRaw(cframe, 3)[0];
	unsigned char g = api -> getParamRaw(cframe, 4)[0];
	unsigned char b = api -> getParamRaw(cframe, 5)[0];
	unsigned char a = api -> getParamRaw(cframe, 6)[0];

	SDL_SetRenderDrawColor(instance -> renderer, r, g, b, a);
	SDL_RenderDrawPoint(instance -> renderer, x - instance -> xScroll, y - instance -> yScroll);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_line(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	size_t x1 = api -> getParamInt(cframe, 1);
	size_t y1 = api -> getParamInt(cframe, 2);
	size_t x2 = api -> getParamInt(cframe, 3);
	size_t y2 = api -> getParamInt(cframe, 4);
	size_t thickness = api -> getParamInt(cframe, 5);
	unsigned char r = api -> getParamRaw(cframe, 6)[0];
	unsigned char g = api -> getParamRaw(cframe, 7)[0];
	unsigned char b = api -> getParamRaw(cframe, 8)[0];
	unsigned char a = api -> getParamRaw(cframe, 9)[0];

	thickLineRGBA(instance -> renderer, x1, y1, x2, y2, thickness, r, g, b, a);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_rect(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));
	
	size_t x = api -> getParamInt(cframe, 1);
	size_t y = api -> getParamInt(cframe, 2);
	size_t w = api -> getParamInt(cframe, 3);
	size_t h = api -> getParamInt(cframe, 4);
	unsigned char r = api -> getParamRaw(cframe, 5)[0];
	unsigned char g = api -> getParamRaw(cframe, 6)[0];
	unsigned char b = api -> getParamRaw(cframe, 7)[0];
	unsigned char a = api -> getParamRaw(cframe, 8)[0];

	SDL_Rect rect;
	rect.x = x - instance -> xScroll;
	rect.y = y - instance -> yScroll;
	rect.w = w;
	rect.h = h;

	SDL_SetRenderDrawColor(instance -> renderer, r, g, b, a);
	SDL_RenderFillRect(instance -> renderer, &rect);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_curve(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	DanaEl* points = api -> getParamEl(cframe, 1);
	size_t isteps = api -> getParamInt(cframe, 2);
	size_t thickness = api -> getParamInt(cframe, 3);
	unsigned char r = api -> getParamRaw(cframe, 4)[0];
	unsigned char g = api -> getParamRaw(cframe, 5)[0];
	unsigned char b = api -> getParamRaw(cframe, 6)[0];
	unsigned char a = api -> getParamRaw(cframe, 7)[0];

	size_t nPoints = api -> getArrayLength(points);

	double* xPoints = malloc(sizeof(double) * nPoints);
	double* yPoints = malloc(sizeof(double) * nPoints);

	short int* xPoints_si = malloc(sizeof(short int) * nPoints);
	short int* yPoints_si = malloc(sizeof(short int) * nPoints);

	int i = 0;
	for (i = 0; i < nPoints; i++)
		{
		DanaEl* it = api -> getArrayCellEl(points, i);

		xPoints[i] = api -> getDataFieldInt(it, 0);
		yPoints[i] = api -> getDataFieldInt(it, 1);

		xPoints_si[i] = api -> getDataFieldInt(it, 0);
		yPoints_si[i] = api -> getDataFieldInt(it, 1);
		}

	aaBezierRGBA(instance -> renderer, xPoints, yPoints, nPoints, isteps, thickness, r, g, b, a);

	free(xPoints);
	free(yPoints);

	free(xPoints_si);
	free(yPoints_si);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_ellipse(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	size_t x = api -> getParamInt(cframe, 1);
	size_t y = api -> getParamInt(cframe, 2);
	size_t w = api -> getParamInt(cframe, 3);
	size_t h = api -> getParamInt(cframe, 4);
	unsigned char r = api -> getParamRaw(cframe, 5)[0];
	unsigned char g = api -> getParamRaw(cframe, 6)[0];
	unsigned char b = api -> getParamRaw(cframe, 7)[0];
	unsigned char a = api -> getParamRaw(cframe, 8)[0];

	aaFilledEllipseRGBA(instance -> renderer, x, y, w, h, r, g, b, a);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_ellipse_outline(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	size_t x = api -> getParamInt(cframe, 1);
	size_t y = api -> getParamInt(cframe, 2);
	size_t w = api -> getParamInt(cframe, 3);
	size_t h = api -> getParamInt(cframe, 4);
	size_t thickness = api -> getParamInt(cframe, 5);
	unsigned char r = api -> getParamRaw(cframe, 6)[0];
	unsigned char g = api -> getParamRaw(cframe, 7)[0];
	unsigned char b = api -> getParamRaw(cframe, 8)[0];
	unsigned char a = api -> getParamRaw(cframe, 9)[0];

	if (thickness == 1)
		{
		aaellipseRGBA(instance -> renderer, x, y, w, h, r, g, b, a);
		}
		else
		{
		//thickEllipseRGBA(instance -> renderer, poly -> rect.x, poly -> rect.y, poly -> rect.w, poly -> rect.h, poly -> r, poly -> g, poly -> b, poly -> a, poly -> thickness);

		aaArcRGBA(instance -> renderer, x, y, w, h, 0.0, 360.0, thickness, r, g, b, a);
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_arc(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	bool antiAlias = false;

	size_t x = api -> getParamInt(cframe, 1);
	size_t y = api -> getParamInt(cframe, 2);
	size_t w = api -> getParamInt(cframe, 3);
	size_t h = api -> getParamInt(cframe, 4);
	size_t start = api -> getParamInt(cframe, 5);
	size_t end = api -> getParamInt(cframe, 6);
	size_t thickness = api -> getParamInt(cframe, 7);
	unsigned char r = api -> getParamRaw(cframe, 8)[0];
	unsigned char g = api -> getParamRaw(cframe, 9)[0];
	unsigned char b = api -> getParamRaw(cframe, 10)[0];
	unsigned char a = api -> getParamRaw(cframe, 11)[0];

	if (antiAlias)
		aaArcRGBA(instance -> renderer, x, y, w, h, start + 270, end + 270, thickness, r, g, b, a);
		else
		thickArcRGBA(instance -> renderer, x, y, w, start + 270, end + 270, r, g, b, a, thickness);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_pie(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	size_t x = api -> getParamInt(cframe, 1);
	size_t y = api -> getParamInt(cframe, 2);
	size_t w = api -> getParamInt(cframe, 3);
	size_t h = api -> getParamInt(cframe, 4);
	size_t start = api -> getParamInt(cframe, 5);
	size_t end = api -> getParamInt(cframe, 6);
	unsigned char chord = api -> getParamRaw(cframe, 7)[0];
	unsigned char r = api -> getParamRaw(cframe, 8)[0];
	unsigned char g = api -> getParamRaw(cframe, 9)[0];
	unsigned char b = api -> getParamRaw(cframe, 10)[0];
	unsigned char a = api -> getParamRaw(cframe, 11)[0];

	aaFilledPieRGBA(instance -> renderer, x, y, w, h, start + 270, end + 270, chord, r, g, b, a);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_polygon(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	DanaEl* points = api -> getParamEl(cframe, 1);

	unsigned char r = api -> getParamRaw(cframe, 2)[0];
	unsigned char g = api -> getParamRaw(cframe, 3)[0];
	unsigned char b = api -> getParamRaw(cframe, 4)[0];
	unsigned char a = api -> getParamRaw(cframe, 5)[0];

	size_t nPoints = api -> getArrayLength(points);

	double* xPoints = malloc(sizeof(double) * nPoints);
	double* yPoints = malloc(sizeof(double) * nPoints);

	int i = 0;
	for (i = 0; i < nPoints; i++)
		{
		DanaEl* it = api -> getArrayCellEl(points, i);

		xPoints[i] = api -> getDataFieldInt(it, 0);
		yPoints[i] = api -> getDataFieldInt(it, 1);
		}

	aaFilledPolygonRGBA(instance -> renderer, xPoints, yPoints, nPoints, r, g, b, a);

	free(xPoints);
	free(yPoints);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_polygon_outline(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	DanaEl* points = api -> getParamEl(cframe, 1);
	size_t thickness = api -> getParamInt(cframe, 2);
	unsigned char r = api -> getParamRaw(cframe, 3)[0];
	unsigned char g = api -> getParamRaw(cframe, 4)[0];
	unsigned char b = api -> getParamRaw(cframe, 5)[0];
	unsigned char a = api -> getParamRaw(cframe, 6)[0];

	size_t nPoints = api -> getArrayLength(points);

	double* xPoints = malloc(sizeof(double) * nPoints);
	double* yPoints = malloc(sizeof(double) * nPoints);

	short int* xPoints_si = malloc(sizeof(short int) * nPoints);
	short int* yPoints_si = malloc(sizeof(short int) * nPoints);

	int i = 0;
	for (i = 0; i < nPoints; i++)
		{
		DanaEl* it = api -> getArrayCellEl(points, i);

		xPoints[i] = api -> getDataFieldInt(it, 0);
		yPoints[i] = api -> getDataFieldInt(it, 1);

		xPoints_si[i] = api -> getDataFieldInt(it, 0);
		yPoints_si[i] = api -> getDataFieldInt(it, 1);
		}
	
	if (thickness == 1)
		{
		aapolygonRGBA(instance -> renderer, xPoints_si, yPoints_si, nPoints, r, g, b, a);
		}
		else
		{
		//TODO.
		}
	
	free(xPoints);
	free(yPoints);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_polygon_bezier(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	DanaEl* points = api -> getParamEl(cframe, 1);
	size_t isteps = api -> getParamInt(cframe, 2);
	unsigned char r = api -> getParamRaw(cframe, 3)[0];
	unsigned char g = api -> getParamRaw(cframe, 4)[0];
	unsigned char b = api -> getParamRaw(cframe, 5)[0];
	unsigned char a = api -> getParamRaw(cframe, 6)[0];

	size_t nPoints = api -> getArrayLength(points);

	double* xPoints = malloc(sizeof(double) * nPoints);
	double* yPoints = malloc(sizeof(double) * nPoints);

	short int* xPoints_si = malloc(sizeof(short int) * nPoints);
	short int* yPoints_si = malloc(sizeof(short int) * nPoints);

	int i = 0;
	for (i = 0; i < nPoints; i++)
		{
		DanaEl* it = api -> getArrayCellEl(points, i);

		xPoints[i] = api -> getDataFieldInt(it, 0);
		yPoints[i] = api -> getDataFieldInt(it, 1);

		xPoints_si[i] = api -> getDataFieldInt(it, 0);
		yPoints_si[i] = api -> getDataFieldInt(it, 1);
		}

	aaFilledPolyBezierRGBA(instance -> renderer, xPoints, yPoints, nPoints, isteps, r, g, b, a);

	free(xPoints);
	free(yPoints);

	free(xPoints_si);
	free(yPoints_si);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_polygon_bezier_outline(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	DanaEl* points = api -> getParamEl(cframe, 1);
	/*
	size_t isteps = api -> getParamInt(cframe, 2);
	size_t thickness = api -> getParamInt(cframe, 3);
	unsigned char r = api -> getParamRaw(cframe, 4)[0];
	unsigned char g = api -> getParamRaw(cframe, 5)[0];
	unsigned char b = api -> getParamRaw(cframe, 6)[0];
	unsigned char a = api -> getParamRaw(cframe, 7)[0];
	*/

	size_t nPoints = api -> getArrayLength(points);

	double* xPoints = malloc(sizeof(double) * nPoints);
	double* yPoints = malloc(sizeof(double) * nPoints);

	short int* xPoints_si = malloc(sizeof(short int) * nPoints);
	short int* yPoints_si = malloc(sizeof(short int) * nPoints);

	int i = 0;
	for (i = 0; i < nPoints; i++)
		{
		DanaEl* it = api -> getArrayCellEl(points, i);

		xPoints[i] = api -> getDataFieldInt(it, 0);
		yPoints[i] = api -> getDataFieldInt(it, 1);

		xPoints_si[i] = api -> getDataFieldInt(it, 0);
		yPoints_si[i] = api -> getDataFieldInt(it, 1);
		}
	
	free(xPoints);
	free(yPoints);

	free(xPoints_si);
	free(yPoints_si);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_bitmap(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	DanaEl* bitmapData = api -> getParamEl(cframe, 1);
		
	if (bitmapData == NULL)
		{
		api -> throwException(cframe, "null pointer");
		return RETURN_OK;
		}
	
	DanaEl *whData = api -> getDataFieldEl(bitmapData, 0);
	DanaEl *pixelArrayH = api -> getDataFieldEl(bitmapData, 1);
	
	if (whData == NULL || pixelArrayH == NULL || api -> getArrayContent(pixelArrayH) == NULL)
		{
		api -> throwException(cframe, "null pointer");
		return RETURN_OK;
		}

	size_t x = api -> getParamInt(cframe, 3);
	size_t y = api -> getParamInt(cframe, 4);
	size_t sWidth = api -> getParamInt(cframe, 5);
	size_t sHeight = api -> getParamInt(cframe, 6);
	size_t rotation = api -> getParamInt(cframe, 7);

	SDL_Surface *primarySurface = pixelMapToSurface(bitmapData);
				
	SDL_Surface *finalSurface = NULL;
	
	if (sWidth != primarySurface -> w || sHeight != primarySurface -> h)
		{
		//printf("rescaling %u/%u...\n", data -> scaledWidth, data -> scaledHeight);
		float zoomX = (float) sWidth / (float) primarySurface -> w;
		float zoomY = (float) sHeight / (float) primarySurface -> h;

		finalSurface = zoomSurface(primarySurface, zoomX, zoomY, SMOOTHING_ON);
		SDL_FreeSurface(primarySurface);
		}
		else
		{
		finalSurface = primarySurface;
		}
	
	SDL_Texture *texture = SDL_CreateTextureFromSurface(instance -> renderer, finalSurface);

	renderTexture(texture, instance -> renderer, x, y, rotation);
	SDL_DestroyTexture(texture);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_text_with(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	FontHolder* fontInstance = NULL;
	memcpy(&fontInstance, api -> getParamRaw(cframe, 1), sizeof(void*));

	size_t x = api -> getParamInt(cframe, 2);
	size_t y = api -> getParamInt(cframe, 3);
	size_t rotation = api -> getParamInt(cframe, 4);
	DanaEl* array = api -> getParamEl(cframe, 5);
	size_t r = api -> getParamRaw(cframe, 6)[0];
	size_t g = api -> getParamRaw(cframe, 7)[0];
	size_t b = api -> getParamRaw(cframe, 8)[0];
	size_t a = api -> getParamRaw(cframe, 9)[0];

	size_t tlen = api -> getArrayLength(array);

	if (tlen > 0)
		{
		char* text = malloc(tlen + 1);
		memset(text, '\0', tlen + 1);
		memcpy(text, api -> getArrayContent(array), tlen);

		SDL_Color color;
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;

		SDL_Texture *image = renderText(text, fontInstance -> font, color, instance -> renderer);
		if (image != NULL)
			{
			renderTextureRZ(image, instance -> renderer, x, y, rotation);
			SDL_DestroyTexture(image);
			}
		}
	
	return RETURN_OK;
	}

// flow font loading

INSTRUCTION_DEF op_flow_load_font(FrameData* cframe)
	{
	char* path = x_getParam_char_array(api, cframe, 0);

	size_t fontSize = api -> getParamInt(cframe, 1);

	char *fontPath = malloc(2048);
	memset(fontPath, '\0', 2048);

	if (!findFont(path, fontPath, 2048))
		{
		api -> throwException(cframe, "open font error: could not find font");
		free(fontPath);
		free(path);
		return RETURN_OK;
		}
		else
		{
		free(path);
		}
	
	TTF_Font *font = TTF_OpenFont(fontPath, fontSize);
	if (font == NULL)
		{
		//printf("open font error: %s [%s]\n", SDL_GetError(), TTF_GetError());
		api -> throwException(cframe, "open font error: could not load font");
		return RETURN_OK;
		}
	
	free(fontPath);

	//note: we could support font styles, like:
	//TTF_SetFontStyle(font, TTF_STYLE_BOLD);
	
	FontHolder *fhold = malloc(sizeof(FontHolder));
	fhold -> refCount = 1;
	fhold -> font = font;

	api -> returnRaw(cframe, (unsigned char*) &fhold, sizeof(void*));

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_unload_font(FrameData* cframe)
	{
	FontHolder* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	TTF_CloseFont(instance -> font);
	free(instance);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_get_text_width_with(FrameData* cframe)
	{
	FontHolder* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	DanaEl* array = api -> getParamEl(cframe, 1);

	size_t tlen = api -> getArrayLength(array);

	size_t width = 0;

	if (tlen > 0)
		{
		//note: we could avoid this malloc if we wrote an extra SDL_ttf function which took the (UTF-8) string length as a parameter...
		char *text = (char*) malloc(tlen + 1);
		memset(text, '\0', tlen + 1);
		memcpy(text, api -> getArrayContent(array), tlen);

		int count = 0;
		int pixels = 0;
		TTF_MeasureUTF8(instance -> font, text, INT_MAX, &pixels, &count);

		api -> returnInt(cframe, pixels);

		free(text);

		return RETURN_OK;
		}
	
	api -> returnInt(cframe, width);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_get_text_bitmap_with(FrameData* cframe)
	{
	FontHolder* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	DanaEl* array = api -> getParamEl(cframe, 1);

	size_t tlen = api -> getArrayLength(array);
	
	DanaEl* pixelcnt = api -> getParamEl(cframe, 2);

	size_t r = api -> getParamRaw(cframe, 3)[0];

	size_t g = api -> getParamRaw(cframe, 4)[0];

	size_t b = api -> getParamRaw(cframe, 5)[0];

	size_t a = api -> getParamRaw(cframe, 6)[0];

	if (tlen > 0)
		{
		SDL_Color color;
		color.r = r;
		color.g = g;
		color.b = b;
		color.a = a;

		char *text = (char*) malloc(tlen + 1);
		memset(text, '\0', tlen + 1);
		memcpy(text, api -> getArrayContent(array), tlen);

		DanaEl *bitmapData = pixelcnt;
		DanaEl *whData = api -> getDataFieldEl(bitmapData, 0);
		
		//printf("rendering text with color %u:%u:%u:%u\n", data -> color.r, data -> color.g, data -> color.b, data -> color.a);

		SDL_Surface *surf = TTF_RenderText_Blended(instance -> font, text, color);
		if (surf == NULL)
			{
			printf("SDL surface error from RT for '%s': %s\n", text, SDL_GetError());
			}

		size_t w = surf -> w;
		size_t h = surf -> h;

		size_t totalPixels = w * h;
		size_t sz = 4;
		
		api -> setDataFieldInt(whData, 0, w);
		api -> setDataFieldInt(whData, 1, h);
		
		//set up the pixel array
	
		size_t asz = totalPixels * sz;
		
		unsigned char* pixelArray = NULL;
		DanaEl* pixelArrayH = api -> makeArray(charArrayGT, asz, &pixelArray);
		
		api -> setDataFieldEl(bitmapData, 1, pixelArrayH);
		
		//printf("size: %u | %u\n", totalPixels, ((StructuredType*) pixelArrayH -> gtLink -> typeLink -> definition.content) -> size);

		//printf(" -- generate bitmap -- %u pixels in %u:%u --\n", pixelArrayH -> length, w, h);

		SDL_PixelFormat *fmt;
		fmt = surf -> format;

		SDL_LockSurface(surf);

		int i = 0;
		int j = 0;
		int ndx = 0;
		for  (i = 0; i < h; i++)
			{
			for (j = 0; j < w; j++)
				{
				//read the next pixel
				unsigned char red;
				unsigned char green;
				unsigned char blue;
				unsigned char alpha;

				Uint32 pixel = 0;
				Uint32 temp = 0;

				pixel = ((Uint32*) surf -> pixels)[(i*w)+j];

				temp = pixel & fmt->Rmask;
				temp = temp >> fmt->Rshift;
				temp = temp << fmt->Rloss;
				red = (Uint8) temp;

				temp = pixel & fmt->Gmask;
				temp = temp >> fmt->Gshift;
				temp = temp << fmt->Gloss;
				green = (Uint8) temp;

				temp = pixel & fmt->Bmask;
				temp = temp >> fmt->Bshift;
				temp = temp << fmt->Bloss;
				blue = (Uint8) temp;

				temp = pixel & fmt->Amask;
				temp = temp >> fmt->Ashift;
				temp = temp << fmt->Aloss;
				alpha = (Uint8) temp;

				//printf("pixel %u is %u:%u:%u:%u\n", (i*w)+j, red, green, blue, alpha);

				unsigned char *cdata = &pixelArray[ndx];

				cdata[0] = red;
				cdata[1] = green;
				cdata[2] = blue;
				cdata[3] = alpha;

				ndx += 4;
				}
			}

		SDL_UnlockSurface(surf);

		SDL_FreeSurface(surf);

		free(text);
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_get_font_name(FrameData* cframe)
	{
	FontHolder* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	char *val = TTF_FontFaceFamilyName(instance -> font);

	if (val != NULL)
		{
		returnByteArray(cframe, (unsigned char*) strdup(val), strlen(val));
    	}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_is_font_fixed_width(FrameData* cframe)
	{
	FontHolder* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	unsigned char is = TTF_FontFaceIsFixedWidth(instance -> font) == 0 ? 0 : 1;
	api -> returnRaw(cframe, &is, 1);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_get_font_metrics(FrameData* cframe)
	{
	FontHolder* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	DanaEl* mdata = api -> getParamEl(cframe, 1);

	int sdl_x;

	sdl_x = TTF_FontHeight(instance -> font);
	
	api -> setDataFieldInt(mdata, 0, sdl_x);
	
	sdl_x = TTF_FontAscent(instance -> font);
	
	api -> setDataFieldInt(mdata, 1, sdl_x);

	sdl_x = TTF_FontDescent(instance -> font);
	
	api -> setDataFieldInt(mdata, 2, sdl_x * -1);

	sdl_x = TTF_FontLineSkip(instance -> font);
	
	api -> setDataFieldInt(mdata, 3, sdl_x);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_push_surface(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	SurfaceElement* surface = malloc(sizeof(SurfaceElement));
	memset(surface, '\0', sizeof(SurfaceElement));

	surface -> next = instance -> surfaces;

	instance -> surfaces = surface;

	// --

	size_t x = api -> getParamInt(cframe, 1);
	size_t y = api -> getParamInt(cframe, 2);
	size_t w = api -> getParamInt(cframe, 3);
	size_t h = api -> getParamInt(cframe, 4);
	size_t xs = api -> getParamInt(cframe, 5);
	size_t ys = api -> getParamInt(cframe, 6);
	size_t a = api -> getParamRaw(cframe, 7)[0];

	// --

	SDL_Texture *st = SDL_CreateTexture(instance -> renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
	SDL_SetRenderTarget(instance -> renderer, st);

	surface -> surface = st;

	surface -> xPos = x;
	surface -> yPos = y;

	surface -> xScroll = xs;
	surface -> yScroll = ys;

	instance -> xScroll = xs;
	instance -> yScroll = ys;
			
	//NOTE: when using transparency, if we blend the background colour here (at alpha zero) against the SAME colour of text (white for white text), we get nice text; if the background colour is opposite (white for black text) we get horrible text rendering
	// - if we take the alpha channel away, blending is fine in all cases
	// - see https://discourse.libsdl.org/t/bad-text-quality-on-transparent-texture/23098
	// - a workaround is to require surfaces to have a solid background colour, and only offer to apply alpha to the whole surface (i.e., to the texture after all contents are rendered)
	// - we currently push this responsibility to Dana components, which should always render a solid background on a surface before rendering any contents
	SDL_SetTextureBlendMode(st, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(instance -> renderer, 255, 255, 255, 0);
	SDL_RenderClear(instance -> renderer);

	return RETURN_OK;
	}

INSTRUCTION_DEF op_flow_pop_surface(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	//SDL_SetTextureAlphaMod(st, poly -> a);

	SurfaceElement* surface = instance -> surfaces;
	instance -> surfaces = surface -> next;

	if (instance -> surfaces != NULL)
		{
		SDL_SetRenderTarget(instance -> renderer, instance -> surfaces -> surface);
		instance -> xScroll = instance -> surfaces -> xScroll;
		instance -> yScroll = instance -> surfaces -> yScroll;
		}
		else
		{
		SDL_SetRenderTarget(instance -> renderer, NULL);
		instance -> xScroll = 0;
		instance -> yScroll = 0;
		}

	renderTexture(surface -> surface, instance -> renderer, surface -> xPos, surface -> yPos, 0);
	SDL_DestroyTexture(surface -> surface);
	
	free(surface);
	
	return RETURN_OK;
	}

//3D

INSTRUCTION_DEF op_flow_vertices(FrameData* cframe)
	{
	FlowInstance* instance = NULL;
	memcpy(&instance, api -> getParamRaw(cframe, 0), sizeof(void*));

	/*
	DanaEl* array = api -> getParamEl(cframe, 1);
	size_t r = api -> getParamRaw(cframe, 2)[0];
	size_t g = api -> getParamRaw(cframe, 3)[0];
	size_t b = api -> getParamRaw(cframe, 4)[0];
	size_t a = api -> getParamRaw(cframe, 5)[0];
	*/


	return RETURN_OK;
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	// grab global type mappings for anything that we generate here
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	integerGT = api -> resolveGlobalTypeMapping(&intType);
	
	windowDataGT = api -> resolveGlobalTypeMapping(&windowDataType);
	dropDataGT = api -> resolveGlobalTypeMapping(getTypeDefinition("DropEventData"));
	pixelMapGT = api -> resolveGlobalTypeMapping(getTypeDefinition("PixelMap"));
	whGT = api -> resolveGlobalTypeMapping(getTypeDefinition("WH"));

	flowEventGT = api -> resolveGlobalTypeMapping(getTypeDefinition("FlowEvent"));
	flowEventArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("FlowEvent[]"));

	//Desktop UI function bindings
	
	setInterfaceFunction("makeWindow", op_make_window);
	setInterfaceFunction("addRect", op_add_rect);
	setInterfaceFunction("addEllipse", op_add_ellipse);
	setInterfaceFunction("addEllipseOutline", op_add_ellipse_outline);
	setInterfaceFunction("addArc", op_add_arc);
	setInterfaceFunction("addPie", op_add_pie);
	setInterfaceFunction("addPolygon", op_add_polygon);
	setInterfaceFunction("addPolygonOutline", op_add_polygon_outline);
	setInterfaceFunction("addPolygonBezier", op_add_polygon_bezier);
	setInterfaceFunction("addPolygonBezierOutline", op_add_polygon_bezier_outline);
	setInterfaceFunction("addLine", op_add_line);
	setInterfaceFunction("addCurve", op_add_curve);
	setInterfaceFunction("addPoint", op_add_point);
	setInterfaceFunction("addBitmap", op_add_bitmap);
	setInterfaceFunction("addTextWith", op_add_text_with);
	setInterfaceFunction("pushSurface", op_push_surface);
	setInterfaceFunction("popSurface", op_pop_surface);
	setInterfaceFunction("setSize", op_set_size);
	setInterfaceFunction("setPosition", op_set_position);
	setInterfaceFunction("setVisible", op_set_visible);
	setInterfaceFunction("setResizable", op_set_resizable);
	setInterfaceFunction("setFullScreen", op_set_fullscreen);
	setInterfaceFunction("getResolution", op_get_resolution);
	setInterfaceFunction("setTitle", op_set_title);
	setInterfaceFunction("setIcon", op_set_icon);
	setInterfaceFunction("setCursor", op_set_cursor);
	setInterfaceFunction("commitBuffer", op_commit_buffer);
	setInterfaceFunction("setBackgroundColor", op_set_background_colour);
	setInterfaceFunction("maximiseWindow", op_maximise_window);
	setInterfaceFunction("minimiseWindow", op_minimise_window);
	setInterfaceFunction("getMaximisedScreenRect", op_get_maximised_screen_rect);
	setInterfaceFunction("getPixels", op_get_pixels);
	setInterfaceFunction("closeWindow", op_close_window);
	
	setInterfaceFunction("initMediaLayer", op_init_media_layer);
	setInterfaceFunction("runSystemLoop", op_run_system_loop);
	setInterfaceFunction("shutdown", op_shutdown);
	
	setInterfaceFunction("loadFont", op_load_font);
	setInterfaceFunction("getTextWidth", op_get_text_width_with);
	setInterfaceFunction("getFontMetrics", op_get_font_metrics);
	setInterfaceFunction("getFontName", op_get_font_name);
	setInterfaceFunction("isFontFixedWidth", op_is_font_fixed_width);
	setInterfaceFunction("getTextBitmapWith", op_get_text_bitmap_with);
	setInterfaceFunction("unloadFont", op_unload_font);

	//flow function bindings
	setInterfaceFunction("flow_initMediaLayer", op_flow_init_media_layer);
	setInterfaceFunction("flow_makeWindow", op_flow_make_window);
	setInterfaceFunction("flow_setVisible", op_flow_set_visible);
	setInterfaceFunction("flow_setResizable", op_flow_set_resizable);
	setInterfaceFunction("flow_setSize", op_flow_set_size);
	setInterfaceFunction("flow_setPosition", op_flow_set_position);
	setInterfaceFunction("flow_setFullScreen", op_flow_set_fullscreen);
	setInterfaceFunction("flow_setTitle", op_flow_set_title);
	setInterfaceFunction("flow_setIcon", op_flow_set_icon);
	setInterfaceFunction("flow_setCursor", op_flow_set_cursor);
	setInterfaceFunction("flow_closeWindow", op_flow_close_window);

	setInterfaceFunction("flow_getEvents", op_flow_get_events);
	setInterfaceFunction("flow_renderBegin", op_flow_render_begin);
	setInterfaceFunction("flow_renderEnd", op_flow_render_end);
	setInterfaceFunction("flow_wait", op_flow_wait);
	setInterfaceFunction("flow_getPixels", op_flow_get_pixels);

	setInterfaceFunction("flow_point", op_flow_point);
	setInterfaceFunction("flow_line", op_flow_line);
	setInterfaceFunction("flow_rect", op_flow_rect);
	setInterfaceFunction("flow_curve", op_flow_curve);
	setInterfaceFunction("flow_ellipse", op_flow_ellipse);
	setInterfaceFunction("flow_ellipseOutline", op_flow_ellipse_outline);
	setInterfaceFunction("flow_arc", op_flow_arc);
	setInterfaceFunction("flow_pie", op_flow_pie);
	setInterfaceFunction("flow_polygon", op_flow_polygon);
	setInterfaceFunction("flow_polygonOutline", op_flow_polygon_outline);
	setInterfaceFunction("flow_polygonBezier", op_flow_polygon_bezier);
	setInterfaceFunction("flow_polygonBezierOutline", op_flow_polygon_bezier_outline);
	setInterfaceFunction("flow_bitmap", op_flow_bitmap);
	setInterfaceFunction("flow_textWith", op_flow_text_with);

	setInterfaceFunction("flow_loadFont", op_flow_load_font);
	setInterfaceFunction("flow_getTextWidth", op_flow_get_text_width_with);
	setInterfaceFunction("flow_getFontMetrics", op_flow_get_font_metrics);
	setInterfaceFunction("flow_getFontName", op_flow_get_font_name);
	setInterfaceFunction("flow_isFontFixedWidth", op_flow_is_font_fixed_width);
	setInterfaceFunction("flow_getTextBitmapWith", op_flow_get_text_bitmap_with);
	setInterfaceFunction("flow_unloadFont", op_flow_unload_font);

	setInterfaceFunction("flow_pushSurface", op_flow_push_surface);
	setInterfaceFunction("flow_popSurface", op_flow_pop_surface);

	// flow 3D
	//setInterfaceFunction("flow_vertices", op_flow_vertices);
	
	#ifdef WINDOWS
	InitializeCriticalSection(&measureTextLock);
	#endif
	
	#ifdef LINUX
	pthread_mutexattr_init(&mAttr);
	#ifdef OSX
	pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE);
	#else
	pthread_mutexattr_settype(&mAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	#endif
	pthread_mutex_init(&measureTextLock, &mAttr);
	#endif

	SDL_version compiled;
	SDL_version linked;
	
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);

	//ask SDL to send us mouse clicks which also activate the window (this is pretty standard behaviour on desktop apps)
	// - if we don't set this hint, a window-activating mouse click won't then e.g. click a button which is under the mouse at that location
	SDL_SetHint(SDL_HINT_MOUSE_FOCUS_CLICKTHROUGH, "1");
	
	if (linked.major == 2 && linked.minor == 0 && linked.patch < 8)
		{
		printf("Warning: graphics library is designed for SDL 2.0.8 or later, you have version %u.%u.%u. Some functionality may be reduced.\n", linked.major, linked.minor, linked.patch);
		resizeAvailable = false;
		}
	
	primeFontDirectories();
	
	return getPublicInterface();
	}

void unload()
	{
	api -> decrementGTRefCount(charArrayGT);
	}
