//Written by Barry Porter, 2016, last updated 2020

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

static CoreAPI *api;

static DanaType intType = {TYPE_LITERAL, 0, sizeof(size_t)};

static const DanaTypeField windowDataTypeFields[] = {
			{&intType, NULL, 0, 0, 0},
			{&intType, NULL, 0, 0, sizeof(size_t)},
			{&intType, NULL, 0, 0, sizeof(size_t)*2}
			};

static DanaType windowDataType = {TYPE_DATA, 0, sizeof(VVarLivePTR), (DanaTypeField*) windowDataTypeFields, 3};

static GlobalTypeLink *charArrayGT = NULL;
static GlobalTypeLink *integerGT = NULL;
static GlobalTypeLink *windowDataGT = NULL;

//the graphics buffer:
typedef struct _point{
	float x;
	float y;
	float z;
	int r;
	int g;
	int b;
	int a;
	struct _point *next;
	} UIPoint;

typedef struct{
	UIPoint *points;
	UIPoint *lastPoint;
	} UIPolygon;

typedef struct{
	SDL_Rect rect;

	int r;
	int g;
	int b;
	int a;
	} UIRect;

typedef struct{
	unsigned int x1;
	unsigned int y1;

	unsigned int x2;
	unsigned int y2;

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

typedef struct{
	TTF_Font *font;
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
#define UI_TYPE_RECT		2
#define UI_TYPE_TRIANGLE	3
#define UI_TYPE_CIRCLE		4
#define UI_TYPE_TEXT		5
#define UI_TYPE_LINE		6
#define UI_TYPE_POINT		7
#define UI_TYPE_BITMAP		8
#define UI_TYPE_SURFACE		9

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

	UIObject *frontBuffer;
	UIObject *backBuffer;
	UIObject *backBufferEnd;
	UIObject *surfaceStack;
	UIPolygon *lastPolygon;

	SDL_Texture *baseTexture;
	bool sceneChanged;

	VVarLivePTR clickListener;
	unsigned char* clickListenerObject;

	VVarLivePTR mouseListener;
	unsigned char* mouseListenerObject;

#ifdef WINDOWS
	HANDLE renderLock;
#endif

	ExitStatus mouseClickExitStatus;
	VVarR clickParams[3];
	ExitStatus mouseEventExitStatus;
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

static void returnByteArray(VFrame *f, unsigned char *data, size_t len)
	{
	LiveArray *array = malloc(sizeof(LiveArray));
	memset(array, '\0', sizeof(LiveArray));
	
	array -> data = data;
	array -> length = len;
	
	array -> gtLink = charArrayGT;
	api -> incrementGTRefCount(array -> gtLink);
	array -> refi.ocm = f -> blocking -> instance;
	
	array -> refi.refCount ++;
	array -> refi.type = array -> gtLink -> typeLink;
	
	VVarLivePTR *ptrh = (VVarLivePTR*) &f -> localsData[((DanaType*) f -> localsDef) -> fields[0].offset];
	ptrh -> content = (unsigned char*) array;
	ptrh -> typeLink = array -> gtLink -> typeLink;
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

static void pushMouseEvent(WindowInstance *w, size_t type, size_t button_id, size_t x, size_t y);
static void pushEvent(WindowInstance *w, size_t type);

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
	//TODO: also allow fonts to be accessed from DANA_HOME/resources-ext/fonts/
	// - so do a check to see if the file exists in winfonts, if not, check DH (or maybe the other way around! :-))

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
	char *home = getenv("DANA_HOME");
	char *danaFontDir = malloc(strlen(home) + strlen("/resources-ext/fonts/") + 1);
	strcpy(danaFontDir, home);
	strcat(danaFontDir, "/resources-ext/fonts/");

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
		printf("SDL surface error from RT for '%s': %s\n", msg, SDL_GetError());
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

		if (td -> type == UI_TYPE_POLYGON)
			{
			UIPolygon *poly = (UIPolygon*) td -> object;
			UIPoint *p = poly -> points;
			while (p != NULL)
				{
				UIPoint *dp = p;
				p = p -> next;

				free(dp);
				}
			}
			else if (td -> type == UI_TYPE_RECT || td -> type == UI_TYPE_LINE || td -> type == UI_TYPE_POINT)
			{
			//UIRect *poly = (UIRect*) td -> object;
			}
			else if (td -> type == UI_TYPE_TEXT)
			{
			UIText *poly = (UIText*) td -> object;

			free(poly -> text);
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

SDL_Texture* renderSurface(UISurface *s, SDL_Renderer *myRenderer)
	{
	SDL_Texture *baseTarget = SDL_GetRenderTarget(myRenderer);

	SDL_Texture *st = SDL_CreateTexture(myRenderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, s -> width, s -> height);
	SDL_SetRenderTarget(myRenderer, st);
	
	//NOTE: when using transparency, if we blend the background colour here (at alpha zero) against the SAME colour of text (white for white text), we get nice text; if the background colour is opposite (white for black text) we get horrible text rendering
	// - if we take the alpha channel away, blending is fine in all cases
	// - see https://discourse.libsdl.org/t/bad-text-quality-on-transparent-texture/23098
	// - a workaround is to require surfaces to have a solid background colour, and only offer to apply alpha to the whole surface (i.e., to the texture after all contents are rendered)
	// - we currently push this responsibility to Dana components, which should always render a solid background on a surface before rendering any contents
	SDL_SetTextureBlendMode(st, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(myRenderer, 255, 255, 255, 0);
	SDL_RenderClear(myRenderer);

	int xScroll = s -> xScroll;
	int yScroll = s -> yScroll;

	UIObject *pw = s -> objects;
	while (pw != NULL)
		{
		if (pw -> type == UI_TYPE_RECT)
			{
			UIRect *poly = (UIRect*) pw -> object;

			SDL_SetRenderDrawColor(myRenderer, poly -> r, poly -> g, poly -> b, poly -> a);

			SDL_Rect rc = poly -> rect;
			rc.x = rc.x - xScroll;
			rc.y = rc.y - yScroll;

			SDL_RenderFillRect(myRenderer, &rc);
			}
			else if (pw -> type == UI_TYPE_LINE)
			{
			UILine *poly = (UILine*) pw -> object;

			SDL_SetRenderDrawColor(myRenderer, poly -> r, poly -> g, poly -> b, poly -> a);
			SDL_RenderDrawLine(myRenderer, poly -> x1 - xScroll, poly -> y1 - yScroll, poly -> x2 - xScroll, poly -> y2 - yScroll);
			}
			else if (pw -> type == UI_TYPE_POINT)
			{
			UIPoint *poly = (UIPoint*) pw -> object;

			SDL_SetRenderDrawColor(myRenderer, poly -> r, poly -> g, poly -> b, poly -> a);
			SDL_RenderDrawPoint(myRenderer, poly -> x - xScroll, poly -> y - yScroll);
			}
			else if (pw -> type == UI_TYPE_TEXT)
			{
			UIText *poly = (UIText*) pw -> object;

			SDL_Color color;
			color.r = poly -> r;
			color.g = poly -> g;
			color.b = poly -> b;
			color.a = poly -> a;
			
			SDL_Texture *image = renderText(poly -> text, poly -> font, color, myRenderer);
			if (image != NULL)
				{
				renderTextureRZ(image, myRenderer, poly -> x - xScroll, poly -> y - yScroll, poly -> rotation);
				SDL_DestroyTexture(image);
				}
			}
			else if (pw -> type == UI_TYPE_SURFACE)
			{
			UISurface *poly = (UISurface*) pw -> object;

			SDL_Texture *image = renderSurface(poly, myRenderer);

			SDL_SetRenderTarget(myRenderer, st);

			if (image != NULL)
				{
				renderTexture(image, myRenderer, poly -> x - xScroll, poly -> y - yScroll, 0);
				SDL_DestroyTexture(image);
				}
			}
			else if (pw -> type == UI_TYPE_BITMAP)
			{
			UIBitmap *poly = (UIBitmap*) pw -> object;

			SDL_Texture *texture = SDL_CreateTextureFromSurface(myRenderer, poly -> surface);

			renderTexture(texture, myRenderer, poly -> x - xScroll, poly -> y - yScroll, poly -> rotation);
			
			SDL_DestroyTexture(texture);
			}

		pw = pw -> next;
		}
	
	SDL_SetTextureAlphaMod(st, s -> a);
	
	SDL_SetRenderTarget(myRenderer, baseTarget);

	return st;
	}

/*
here we render everything onto a texture (baseTexture) and then render that texture to the screen
 - there are two reasons for this: (1) it has more consistent behaviour for font rendering across different graphics adaptors; (2) it's faster if you need to repaint the window but nothing actually changed (i.e. no "sceneChanged")
*/

int DrawScene(WindowInstance *instance)
	{
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

		UIObject *pw = instance -> frontBuffer;
		while (pw != NULL)
			{
			if (pw -> type == UI_TYPE_POLYGON)
				{
				//UIPolygon *poly = (UIPolygon*) pw -> object;
				}
				else if (pw -> type == UI_TYPE_RECT)
				{
				UIRect *poly = (UIRect*) pw -> object;

				SDL_SetRenderDrawColor(instance -> renderer, poly -> r, poly -> g, poly -> b, poly -> a);
				SDL_RenderFillRect(instance -> renderer, &poly -> rect);
				}
				else if (pw -> type == UI_TYPE_LINE)
				{
				UILine *poly = (UILine*) pw -> object;

				SDL_SetRenderDrawColor(instance -> renderer, poly -> r, poly -> g, poly -> b, poly -> a);
				SDL_RenderDrawLine(instance -> renderer, poly -> x1, poly -> y1, poly -> x2, poly -> y2);
				}
				else if (pw -> type == UI_TYPE_POINT)
				{
				UIPoint *poly = (UIPoint*) pw -> object;

				SDL_SetRenderDrawColor(instance -> renderer, poly -> r, poly -> g, poly -> b, poly -> a);
				SDL_RenderDrawPoint(instance -> renderer, poly -> x, poly -> y);
				}
				else if (pw -> type == UI_TYPE_TEXT)
				{
				UIText *poly = (UIText*) pw -> object;

				SDL_Color color;
				color.r = poly -> r;
				color.g = poly -> g;
				color.b = poly -> b;
				color.a = poly -> a;

				SDL_Texture *image = renderText(poly -> text, poly -> font, color, instance -> renderer);
				if (image != NULL)
					{
					renderTextureRZ(image, instance -> renderer, poly -> x, poly -> y, poly -> rotation);
					SDL_DestroyTexture(image);
					}
				}
				else if (pw -> type == UI_TYPE_SURFACE)
				{
				UISurface *poly = (UISurface*) pw -> object;

				SDL_Texture *image = renderSurface(poly, instance -> renderer);
				
				SDL_SetRenderTarget(instance -> renderer, instance -> baseTexture);

				if (image != NULL)
					{
					renderTexture(image, instance -> renderer, poly -> x, poly -> y, 0);
					SDL_DestroyTexture(image);
					}
				}
				else if (pw -> type == UI_TYPE_BITMAP)
				{
				UIBitmap *poly = (UIBitmap*) pw -> object;

				SDL_Texture *texture = SDL_CreateTextureFromSurface(instance -> renderer, poly -> surface);

				renderTexture(texture, instance -> renderer, poly -> x, poly -> y, poly -> rotation);
				SDL_DestroyTexture(texture);
				}

			pw = pw -> next;
			}

		instance -> sceneChanged = false;
		}

	SDL_SetRenderTarget(instance -> renderer, NULL);

	renderTexture(instance -> baseTexture, instance -> renderer, 0, 0, 0);

	SDL_RenderPresent(instance -> renderer);

	return true; // Everything Went OK
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
	VFrame *vframe;
	unsigned char *resultData;
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

typedef struct{
	UIBitmap *bitmapData;
	VFrame *vframe;
	unsigned char *sourceData;
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
	VFrame *vframe;
	TTF_Font *fontHandle;
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
	VFrame *vframe;
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
static unsigned int DX_CLOSE_WINDOW = 0;
static unsigned int DX_GENERATE_TEXT_BITMAP = 0;
static unsigned int DX_GENERATE_BITMAP_SURFACE = 0;
static unsigned int DX_LOAD_FONT = 0;
static unsigned int DX_UNLOAD_FONT = 0;
static unsigned int DX_GET_TEXT_WIDTH = 0;
static unsigned int DX_SYSTEM_SHUTDOWN = 0;

static bool resizeAvailable = true;

static SDL_Surface* pixelMapToSurface(LiveData *pm)
	{
	LiveData *bitmapData = pm;
	LiveData *whData = (LiveData*) ((VVarLivePTR*) bitmapData -> data) -> content;
	LiveArray *pixelArrayH = (LiveArray*) ((VVarLivePTR*) (bitmapData -> data + sizeof(VVarLivePTR))) -> content;
	unsigned char *pixelArray = pixelArrayH -> data;

	size_t *dr_width = (size_t*) whData -> data;
	size_t *dr_height = (size_t*) (whData -> data + sizeof(size_t));

	size_t w = 0;
	size_t h = 0;

	copyHostInteger((unsigned char*) &w, (unsigned char*) dr_width, sizeof(size_t));
	copyHostInteger((unsigned char*) &h, (unsigned char*) dr_height, sizeof(size_t));

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
	
	while (!quit)
		{
		//Read user input & handle it
		
		//printf("wait-event\n");
		
		if (SDL_WaitEvent(&e)) //does this make things more jittery?
		{
		do
			{
			//printf("%u\n", e.type);
			if (e.type == SDL_QUIT)
				{
				//notify any event listeners that the system has received a shutdown request
				api -> pushEvent(systemEventObject, 0, 8, NULL);
				}
				else if (e.type == DX_SYSTEM_SHUTDOWN)
				{
				quit = true;
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
					pushEvent(myInstance, 7);
					}
					else if (e.window.event == SDL_WINDOWEVENT_RESIZED)
					{
					WindowInstance *myInstance = findWindow(e.window.windowID);
					
					if (myInstance != NULL)
						{
						pushMouseEvent(myInstance, 6, 0, e.window.data1, e.window.data2);
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
						
						//pushMouseEvent(myInstance, 5, 0, x, y);
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
					pushMouseEvent(myInstance, 2, button, screenX, screenY);
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
					pushMouseEvent(myInstance, 1, button, screenX, screenY);
					}
				}
				else if (e.type == SDL_MOUSEMOTION)
				{
				size_t screenX = e.motion.x;
				size_t screenY = e.motion.y;
				
				WindowInstance *myInstance = findWindow(e.motion.windowID);
				
				if (myInstance != NULL)
					{
					pushMouseEvent(myInstance, 3, 0, screenX, screenY);
					}
				}
				else if (e.type == SDL_KEYDOWN)
				{
				size_t keyID = e.key.keysym.scancode;
				
				WindowInstance *myInstance = findWindow(e.motion.windowID);
				
				if (myInstance != NULL)
					{
					pushMouseEvent(myInstance, 4, keyID, 0, 0);
					}
				}
				else if (e.type == SDL_KEYUP)
				{
				size_t keyID = e.key.keysym.scancode;
				
				WindowInstance *myInstance = findWindow(e.motion.windowID);
				
				if (myInstance != NULL)
					{
					pushMouseEvent(myInstance, 5, keyID, 0, 0);
					}
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
				//NOTE: performance for window drag/drop could be improved here if the rendering loop saved the entire render to a texture, then just re-fired that texture to the window instead of drawing the entire thing again (assuming nothing changed, as is usually the case right here)
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
				
				LiveData *pixelMap = (LiveData*) e.user.data2;
				
				SDL_Surface *surface = pixelMapToSurface(pixelMap);
				
				SDL_SetWindowIcon(wi -> win, surface);
				
				SDL_FreeSurface(surface);
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
				
				SDL_SetWindowFullscreen(wi -> win, SDL_WINDOW_FULLSCREEN_DESKTOP );
				
				wi -> fullScreen = true;
				
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
				//SDL_GetDesktopDisplayMode(0, &DM);
				
				LiveData *data = (LiveData*) e.user.data2;
				size_t *v = (size_t*) data -> data;
				
				size_t xv = DM.w;
				copyHostInteger((unsigned char*) v, (unsigned char*) &xv, sizeof(size_t));
				
				v ++;
				
				xv = DM.h;
				copyHostInteger((unsigned char*) v, (unsigned char*) &xv, sizeof(size_t));
				
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
				else if (e.type == DX_CLOSE_WINDOW)
				{
				CloseWindowInfo *cwi = (CloseWindowInfo*) e.user.data1;

				removeListItem(&instances, &lastInstance, cwi -> wi);

				if (cwi -> wi -> clickListener.content != NULL)
					{
					cwi -> wi -> clickListenerObject = NULL;
					cwi -> wi -> clickListener.content = NULL;
					}

				if (cwi -> wi -> mouseListener.content != NULL)
					{
					cwi -> wi -> mouseListenerObject = NULL;
					cwi -> wi -> mouseListener.content = NULL;
					}

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

				VFrame *frame = data -> vframe;

				LiveData *bitmapData = (LiveData*) data -> resultData;
				LiveData *whData = (LiveData*) ((VVarLivePTR*) bitmapData -> data) -> content;

				VVarLivePTR *arrayPTR = (VVarLivePTR*) (bitmapData -> data + sizeof(VVarLivePTR));

				//printf("rendering text with color %u:%u:%u:%u\n", data -> color.r, data -> color.g, data -> color.b, data -> color.a);

				SDL_Surface *surf = TTF_RenderText_Blended(data -> font, data -> text, data -> color);
				if (surf == NULL)
					{
					printf("SDL surface error from RT for '%s': %s\n", data -> text, SDL_GetError());
					}

				size_t *dr_width = (size_t*) whData -> data;
				size_t *dr_height = (size_t*) (whData -> data + sizeof(size_t));

				size_t w = surf -> w;
				size_t h = surf -> h;

				size_t totalPixels = w * h;
				size_t sz = 4;

				copyHostInteger((unsigned char*) dr_width, (unsigned char*) &w, sizeof(size_t));
				copyHostInteger((unsigned char*) dr_height, (unsigned char*) &h, sizeof(size_t));

				//set up the pixel array

				LiveArray *pixelArrayH = malloc(sizeof(LiveArray));
				memset(pixelArrayH, '\0', sizeof(LiveArray));

				pixelArrayH -> data = malloc(totalPixels * sz); //OK; the runtime does NOT like this length issue...!
				memset(pixelArrayH -> data, '\0', totalPixels * sz); //TODO!
				pixelArrayH -> length = totalPixels;
				// - there are two possibilities: either we can't copy arrays of records in general; or the way in which this array in initialised is wrong...

				pixelArrayH -> gtLink = charArrayGT;
				api -> incrementGTRefCount(pixelArrayH -> gtLink);

				pixelArrayH -> refi.ocm = frame -> blocking -> instance;

				arrayPTR -> content = (unsigned char*) pixelArrayH;
				pixelArrayH -> refi.refCount ++;
				pixelArrayH -> refi.type = pixelArrayH -> gtLink -> typeLink;
				arrayPTR -> typeLink = pixelArrayH -> gtLink -> typeLink;

				//printf("size: %u | %u\n", totalPixels, ((StructuredType*) pixelArrayH -> gtLink -> typeLink -> definition.content) -> size);

				//printf(" -- generate bitmap -- %u pixels in %u:%u --\n", pixelArrayH -> length, w, h);

				unsigned char *pixelArray = pixelArrayH -> data;

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

				SDL_Surface *primarySurface = pixelMapToSurface((LiveData*) data -> sourceData);
				
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
				SDL_Surface *finalSurface = NULL;
				
				if (data -> scaledWidth != primarySurface -> w || data -> scaledHeight != primarySurface -> h)
					{
					//printf("rescaling %u/%u...\n", data -> scaledWidth, data -> scaledHeight);
					finalSurface = SDL_CreateRGBSurface(0, data -> scaledWidth, data -> scaledHeight, 32, rmask, gmask, bmask, amask);
					SDL_BlitScaled(primarySurface, NULL, finalSurface, NULL);
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
				VFrame *frame = lfd -> vframe;

				TTF_Font *font = TTF_OpenFont(lfd -> fontPath, lfd -> size);
				if (font == NULL)
					{
					printf("open font error: %s [%s]\n", SDL_GetError(), TTF_GetError());
					}

				free(lfd -> fontPath);

				size_t xs = (size_t) font;
				size_t *result = (size_t*) &frame -> localsData[((DanaType*) frame -> localsDef) -> fields[0].offset];
				memcpy(result, &xs, sizeof(size_t));

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
				TTF_Font *font = lfd -> fontHandle;
				
				TTF_CloseFont(font);
				
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
				else if (e.type == DX_GET_TEXT_WIDTH)
				{
				GetTextWidthInfo *gwi = (GetTextWidthInfo*) e.user.data1;
				VFrame *frame = gwi -> vframe;

				size_t width = 0;
				int sdl_width = 0;
				TTF_SizeText(gwi -> font, gwi -> text, &sdl_width, NULL);
				width = sdl_width;

				size_t *result = (size_t*) &frame -> localsData[((DanaType*) frame -> localsDef) -> fields[0].offset];

				copyHostInteger((unsigned char*) result, (unsigned char*) &width, sizeof(size_t));

				#ifdef WINDOWS
				ReleaseSemaphore(gwi -> sem, 1, NULL);
				#endif
				#ifdef OSX
				dispatch_semaphore_signal(gwi -> sem);
				#else
				#ifdef LINUX
				sem_post(&gwi -> sem);
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
				DrawScene(wi);

				lw = lw -> next;
				}
			}

		newFrame = false;
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

INSTRUCTION_DEF op_make_window(VFrame *cframe)
	{
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
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &xs, sizeof(size_t));
	
	if (mwInfo -> instanceResult != NULL)
		mwInfo -> instanceResult -> eqObject = cframe -> io;
	
	free(mwInfo);
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_start_poly(VFrame *cframe)
	{
	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_poly_point(VFrame *cframe)
	{
	return RETURN_OK;
	}

INSTRUCTION_DEF op_end_poly(VFrame *cframe)
	{
	return RETURN_OK;
	}

static void addUIObject(WindowInstance *instance, UIObject *o)
	{
	if (instance -> surfaceStack != NULL)
		{
		UISurface *surface = (UISurface*) instance -> surfaceStack -> object;

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

INSTRUCTION_DEF op_add_rect(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = 0;
		copyHostInteger((unsigned char*) &x, getVariableContent(cframe, 1), sizeof(size_t));

		size_t y = 0;
		copyHostInteger((unsigned char*) &y, getVariableContent(cframe, 2), sizeof(size_t));

		size_t w = 0;
		copyHostInteger((unsigned char*) &w, getVariableContent(cframe, 3), sizeof(size_t));

		size_t h = 0;
		copyHostInteger((unsigned char*) &h, getVariableContent(cframe, 4), sizeof(size_t));

		unsigned char r = getVariableContent(cframe, 5)[0];

		unsigned char g = getVariableContent(cframe, 6)[0];

		unsigned char b = getVariableContent(cframe, 7)[0];

		unsigned char a = getVariableContent(cframe, 8)[0];

		// -- create the container --

		UIRect *poly = (UIRect*) malloc(sizeof(UIRect));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));
		
		addUIObject(instance, uio);

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
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_line(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t sx = 0;
		copyHostInteger((unsigned char*) &sx, getVariableContent(cframe, 1), sizeof(size_t));

		size_t sy = 0;
		copyHostInteger((unsigned char*) &sy, getVariableContent(cframe, 2), sizeof(size_t));

		size_t ex = 0;
		copyHostInteger((unsigned char*) &ex, getVariableContent(cframe, 3), sizeof(size_t));

		size_t ey = 0;
		copyHostInteger((unsigned char*) &ey, getVariableContent(cframe, 4), sizeof(size_t));

		size_t r = 0;
		copyHostInteger((unsigned char*) &r, getVariableContent(cframe, 5), 1);

		size_t g = 0;
		copyHostInteger((unsigned char*) &g, getVariableContent(cframe, 6), 1);

		size_t b = 0;
		copyHostInteger((unsigned char*) &b, getVariableContent(cframe, 7), 1);

		size_t a = 0;
		copyHostInteger((unsigned char*) &a, getVariableContent(cframe, 8), 1);

		// -- create the container --

		UILine *poly = (UILine*) malloc(sizeof(UILine));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));

		addUIObject(instance, uio);

		uio -> type = UI_TYPE_LINE;
		uio -> object = poly;

		poly -> x1 = sx;
		poly -> y1 = sy;

		poly -> x2 = ex;
		poly -> y2 = ey;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_point(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = 0;
		copyHostInteger((unsigned char*) &x, getVariableContent(cframe, 1), sizeof(size_t));

		size_t y = 0;
		copyHostInteger((unsigned char*) &y, getVariableContent(cframe, 2), sizeof(size_t));

		size_t r = 0;
		copyHostInteger((unsigned char*) &r, getVariableContent(cframe, 3), 1);

		size_t g = 0;
		copyHostInteger((unsigned char*) &g, getVariableContent(cframe, 4), 1);

		size_t b = 0;
		copyHostInteger((unsigned char*) &b, getVariableContent(cframe, 5), 1);

		size_t a = 0;
		copyHostInteger((unsigned char*) &a, getVariableContent(cframe, 6), 1);

		// -- create the container --

		UIPoint *poly = (UIPoint*) malloc(sizeof(UIPoint));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));

		addUIObject(instance, uio);

		uio -> type = UI_TYPE_POINT;
		uio -> object = poly;

		poly -> x = x;
		poly -> y = y;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_add_bitmap(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = 0;
		copyHostInteger((unsigned char*) &x, getVariableContent(cframe, 3), sizeof(size_t));

		size_t y = 0;
		copyHostInteger((unsigned char*) &y, getVariableContent(cframe, 4), sizeof(size_t));
		
		size_t sWidth = 0;
		copyHostInteger((unsigned char*) &sWidth, getVariableContent(cframe, 5), sizeof(size_t));

		size_t sHeight = 0;
		copyHostInteger((unsigned char*) &sHeight, getVariableContent(cframe, 6), sizeof(size_t));
		
		size_t rotation = 0;
		copyHostInteger((unsigned char*) &rotation, getVariableContent(cframe, 7), sizeof(size_t));

		// -- create the container --

		UIBitmap *poly = (UIBitmap*) malloc(sizeof(UIBitmap));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));

		addUIObject(instance, uio);

		uio -> type = UI_TYPE_BITMAP;
		uio -> object = poly;

		poly -> x = x;
		poly -> y = y;
		poly -> rotation = rotation;

		GenerateBitmapSurfaceData *gb = malloc(sizeof(GenerateBitmapSurfaceData));
		memset(gb, '\0', sizeof(GenerateBitmapSurfaceData));
		gb -> sourceData = ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
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

INSTRUCTION_DEF op_add_text_with(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t font_hnd = 0;
		memcpy(&font_hnd, getVariableContent(cframe, 1), sizeof(size_t));
		TTF_Font *font = (TTF_Font*) font_hnd;

		size_t x = 0;
		copyHostInteger((unsigned char*) &x, getVariableContent(cframe, 2), sizeof(size_t));

		size_t y = 0;
		copyHostInteger((unsigned char*) &y, getVariableContent(cframe, 3), sizeof(size_t));
		
		size_t rotation = 0;
		copyHostInteger((unsigned char*) &rotation, getVariableContent(cframe, 4), sizeof(size_t));

		LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 5)) -> content;

		size_t r = 0;
		copyHostInteger((unsigned char*) &r, getVariableContent(cframe, 6), 1);

		size_t g = 0;
		copyHostInteger((unsigned char*) &g, getVariableContent(cframe, 7), 1);

		size_t b = 0;
		copyHostInteger((unsigned char*) &b, getVariableContent(cframe, 8), 1);

		size_t a = 0;
		copyHostInteger((unsigned char*) &a, getVariableContent(cframe, 9), 1);

		size_t tlen = array != NULL ? array -> length : 0;

		if (tlen > 0)
			{
			// -- create the container --

			UIText *poly = (UIText*) malloc(sizeof(UIText));
			UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
			memset(uio, '\0', sizeof(UIObject));
			
			addUIObject(instance, uio);

			uio -> type = UI_TYPE_TEXT;
			uio -> object = poly;

			poly -> font = font;

			poly -> textLen = tlen;
			poly -> text = (char*) malloc(tlen + 1);
			memset(poly -> text, '\0', tlen + 1);
			memcpy(poly -> text, array -> data, tlen);

			poly -> x = x;
			poly -> y = y;

			poly -> r = r;
			poly -> g = g;
			poly -> b = b;
			poly -> a = a;
			
			poly -> rotation = rotation;
			}
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_text_width_with(VFrame *cframe)
	{
	size_t font_hnd = 0;
	memcpy(&font_hnd, getVariableContent(cframe, 0), sizeof(size_t));
	TTF_Font *font = (TTF_Font*) font_hnd;

	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;

	size_t tlen = array != NULL ? array -> length : 0;

	size_t width = 0;

	if (tlen > 0)
		{
		char *text = (char*) malloc(tlen + 1);
		memset(text, '\0', tlen + 1);
		memcpy(text, array -> data, tlen);

		GetTextWidthInfo *gwi = malloc(sizeof(GetTextWidthInfo));
		memset(gwi, '\0', sizeof(GetTextWidthInfo));

		gwi -> text = text;
		gwi -> font = font;
		gwi -> vframe = cframe;
		
		#ifdef WINDOWS
		gwi -> sem = CreateSemaphore(NULL, 0, 1, NULL);
		#endif
		#ifdef OSX
		dispatch_semaphore_t *sem;
		sem = &gwi -> sem;
		*sem = dispatch_semaphore_create(0);
		#else
		#ifdef LINUX
		sem_init(&gwi -> sem, 0, 0);
		#endif
		#endif

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_GET_TEXT_WIDTH;
		newEvent.user.data1 = gwi;

		SDL_PushEvent(&newEvent);
		
		#ifdef WINDOWS
		WaitForSingleObject(gwi -> sem, INFINITE);
		#endif
		#ifdef OSX
		dispatch_semaphore_wait(gwi -> sem, DISPATCH_TIME_FOREVER);
		#else
		#ifdef LINUX
		sem_wait(&gwi -> sem);
		#endif
		#endif

		#ifdef WINDOWS
		CloseHandle(gwi -> sem);
		#endif
		#ifdef OSX
		dispatch_release(gwi -> sem);
		#else
		#ifdef LINUX
		sem_destroy(&gwi -> sem);
		#endif
		#endif
		
		free(gwi);

		return RETURN_OK;
		}

	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	copyHostInteger((unsigned char*) result, (unsigned char*) &width, sizeof(size_t));

	return RETURN_OK;
	}

INSTRUCTION_DEF op_push_surface(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

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
		
		addUIObject(instance, newObject);

		newStackObject -> next = instance -> surfaceStack;
		instance -> surfaceStack = newStackObject;

		// --
		size_t x = 0;
		copyHostInteger((unsigned char*) &x, getVariableContent(cframe, 1), sizeof(size_t));

		size_t y = 0;
		copyHostInteger((unsigned char*) &y, getVariableContent(cframe, 2), sizeof(size_t));

		size_t w = 0;
		copyHostInteger((unsigned char*) &w, getVariableContent(cframe, 3), sizeof(size_t));

		size_t h = 0;
		copyHostInteger((unsigned char*) &h, getVariableContent(cframe, 4), sizeof(size_t));

		size_t xs = 0;
		copyHostInteger((unsigned char*) &xs, getVariableContent(cframe, 5), sizeof(size_t));

		size_t ys = 0;
		copyHostInteger((unsigned char*) &ys, getVariableContent(cframe, 6), sizeof(size_t));
		
		size_t a = 0;
		copyHostInteger((unsigned char*) &a, getVariableContent(cframe, 7), 1);
		// --

		newObject -> type = UI_TYPE_SURFACE;

		newSurface -> x = x;
		newSurface -> y = y;
		newSurface -> width = w;
		newSurface -> height = h;
		
		newSurface -> a = a;

		newSurface -> xScroll = xs;
		newSurface -> yScroll = ys;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_pop_surface(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

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

INSTRUCTION_DEF op_get_font_metrics(VFrame *cframe)
	{
	size_t font_hnd = 0;
	memcpy(&font_hnd, getVariableContent(cframe, 0), sizeof(size_t));
	TTF_Font *font = (TTF_Font*) font_hnd;

	unsigned char *cnt = ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;

	int sdl_x;
	size_t x;
	size_t *xs = (size_t*) ((LiveData*) cnt) -> data;

	//TODO: don't use any TTF_ functions outside of the main rendering loop?

	// -- --

	sdl_x = TTF_FontHeight(font);
	x = sdl_x;
	copyHostInteger((unsigned char*) xs, (unsigned char*) &x, sizeof(size_t));

	xs ++;

	sdl_x = TTF_FontAscent(font);
	x = sdl_x;
	copyHostInteger((unsigned char*) xs, (unsigned char*) &x, sizeof(size_t));

	xs ++;

	sdl_x = TTF_FontDescent(font);
	x = sdl_x * -1;
	copyHostInteger((unsigned char*) xs, (unsigned char*) &x, sizeof(size_t));

	xs ++;

	sdl_x = TTF_FontLineSkip(font);
	x = sdl_x;
	copyHostInteger((unsigned char*) xs, (unsigned char*) &x, sizeof(size_t));

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_font_name(VFrame *cframe)
	{
	size_t font_hnd = 0;
	memcpy(&font_hnd, getVariableContent(cframe, 0), sizeof(size_t));
	TTF_Font *font = (TTF_Font*) font_hnd;

	//TODO: don't use any TTF_ functions outside of the main rendering loop?

	char *val = TTF_FontFaceFamilyName(font);

	if (val != NULL)
		{
		returnByteArray(cframe, (unsigned char*) strdup(val), strlen(val));
    	}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_is_font_fixed_width(VFrame *cframe)
	{
	size_t font_hnd = 0;
	memcpy(&font_hnd, getVariableContent(cframe, 0), sizeof(size_t));
	TTF_Font *font = (TTF_Font*) font_hnd;

	//TODO: don't use any TTF_ functions outside of the main rendering loop?
	cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset] = TTF_FontFaceIsFixedWidth(font) == 0 ? 0 : 1;

	return RETURN_OK;
	}

INSTRUCTION_DEF op_get_text_bitmap_with(VFrame *cframe)
	{
	size_t font_hnd = 0;
	memcpy(&font_hnd, getVariableContent(cframe, 0), sizeof(size_t));
	TTF_Font *font = (TTF_Font*) font_hnd;

	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;

	size_t tlen = array != NULL ? array -> length : 0;

	unsigned char *cnt = ((VVarLivePTR*) getVariableContent(cframe, 2)) -> content;

	size_t r = 0;
	copyHostInteger((unsigned char*) &r, getVariableContent(cframe, 3), 1);

	size_t g = 0;
	copyHostInteger((unsigned char*) &g, getVariableContent(cframe, 4), 1);

	size_t b = 0;
	copyHostInteger((unsigned char*) &b, getVariableContent(cframe, 5), 1);

	size_t a = 0;
	copyHostInteger((unsigned char*) &a, getVariableContent(cframe, 6), 1);

	if (tlen > 0)
		{
		char *text = (char*) malloc(tlen + 1);
		memset(text, '\0', tlen + 1);
		memcpy(text, array -> data, tlen);

		//render the text to a surface as normal
		// - then get the w/h of the surface and iterate through its pixels

		GenerateTextBitmapData *gb = malloc(sizeof(GenerateTextBitmapData));
		memset(gb, '\0', sizeof(GenerateTextBitmapData));
		gb -> text = text;
		gb -> vframe = cframe;
		gb -> resultData = cnt;
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

INSTRUCTION_DEF op_commit_buffer(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

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

static void pushMouseEvent(WindowInstance *w, size_t type, size_t button_id, size_t x, size_t y)
	{
	LiveData *nd = malloc(sizeof(LiveData));
	memset(nd, '\0', sizeof(LiveData));
	
	size_t sz = sizeof(size_t) * 3;
	nd -> data = malloc(sz);
	memset(nd -> data, '\0', sz);
	
	nd -> gtLink = windowDataGT;
	api -> incrementGTRefCount(nd -> gtLink);
	nd -> refi.type = nd -> gtLink -> typeLink;
	
	unsigned char *wd_bid = nd -> data;
	unsigned char *wd_x = nd -> data + sizeof(size_t);
	unsigned char *wd_y = nd -> data + sizeof(size_t) + sizeof(size_t);
	
	copyHostInteger(wd_bid, (unsigned char*) &button_id, sizeof(size_t));
	copyHostInteger(wd_x, (unsigned char*) &x, sizeof(size_t));
	copyHostInteger(wd_y, (unsigned char*) &y, sizeof(size_t));
	
	api -> pushEvent(w -> eqObject, 0, type, nd);
	}

static void pushEvent(WindowInstance *w, size_t type)
	{
	api -> pushEvent(w -> eqObject, 0, type, NULL);
	}

INSTRUCTION_DEF op_set_size(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = 0;
		copyHostInteger((unsigned char*) &x, getVariableContent(cframe, 1), sizeof(size_t));

		size_t y = 0;
		copyHostInteger((unsigned char*) &y, getVariableContent(cframe, 2), sizeof(size_t));
		
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

INSTRUCTION_DEF op_set_position(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = 0;
		copyHostInteger((unsigned char*) &x, getVariableContent(cframe, 1), sizeof(size_t));

		size_t y = 0;
		copyHostInteger((unsigned char*) &y, getVariableContent(cframe, 2), sizeof(size_t));

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

INSTRUCTION_DEF op_set_visible(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		unsigned char v = getVariableContent(cframe, 1)[0];

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

INSTRUCTION_DEF op_set_resizable(VFrame *cframe)
	{
	if (!resizeAvailable)
		{
		api -> throwException(cframe, "function unavailable on installed version of SDL");
		return RETURN_OK;
		}
	
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		unsigned char v = getVariableContent(cframe, 1)[0];

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = v ? DX_RESIZABLE_WINDOW : DX_FIXED_SIZE_WINDOW;
		newEvent.user.data1 = instance;

		SDL_PushEvent(&newEvent);
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_set_fullscreen(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		unsigned char v = getVariableContent(cframe, 1)[0];

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = v ? DX_FULLSCREEN_WINDOW : DX_WINDOWED_WINDOW;
		newEvent.user.data1 = instance;
		newEvent.user.data2 = ((VVarLivePTR*) getVariableContent(cframe, 2)) -> content;

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

INSTRUCTION_DEF op_get_resolution(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));
	
	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_GET_RESOLUTION;
		newEvent.user.data1 = instance;
		newEvent.user.data2 = ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;

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

INSTRUCTION_DEF op_set_title(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;

		char *text = NULL;

		if (array != NULL)
			{
			text = (char*) malloc(array -> length + 1);
			memset(text, '\0', array -> length + 1);
			memcpy((unsigned char*) text, array -> data, array -> length);
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

INSTRUCTION_DEF op_set_icon(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_SET_WINDOW_ICON;
		newEvent.user.data1 = instance;
		newEvent.user.data2 = ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;

		SDL_PushEvent(&newEvent);
		}
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_maximise_window(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

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

INSTRUCTION_DEF op_minimise_window(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

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

INSTRUCTION_DEF op_get_maximised_screen_rect(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		//WindowInstance *instance = (WindowInstance*) hnd;

		size_t *xs = (size_t*) ((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content) -> data;

		#ifdef WINDOWS
		//NOTE: below only works for primary monitor - see GetMonitorInfo for other monitors (not sure how we know which monitor a window is considered to be "on")
		RECT r;
		SystemParametersInfo(SPI_GETWORKAREA, 0, &r, 0);

		size_t v = r.left;
		copyHostInteger((unsigned char*) xs, (unsigned char*) &v, sizeof(size_t));

		xs ++;

		v = r.top;
		copyHostInteger((unsigned char*) xs, (unsigned char*) &v, sizeof(size_t));

		xs ++;

		v = r.right - r.left;
		copyHostInteger((unsigned char*) xs, (unsigned char*) &v, sizeof(size_t));

		xs ++;

		v = r.bottom - r.top;
		copyHostInteger((unsigned char*) xs, (unsigned char*) &v, sizeof(size_t));
		#endif
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_close_window(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

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

INSTRUCTION_DEF op_set_background_colour(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t r = 0;
		copyHostInteger((unsigned char*) &r, getVariableContent(cframe, 1), 1);

		size_t g = 0;
		copyHostInteger((unsigned char*) &g, getVariableContent(cframe, 2), 1);

		size_t b = 0;
		copyHostInteger((unsigned char*) &b, getVariableContent(cframe, 3), 1);

		size_t a = 0;
		copyHostInteger((unsigned char*) &a, getVariableContent(cframe, 4), 1);

		instance -> backgroundColour.r = r;
		instance -> backgroundColour.g = g;
		instance -> backgroundColour.b = b;
		}

	return RETURN_OK;
	}

INSTRUCTION_DEF op_load_font(VFrame *cframe)
	{
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 0)) -> content;
	
	if (array == NULL)
		{
		api -> throwException(cframe, "font '' not found");
		return RETURN_OK;
		}
	
	size_t tlen = array != NULL ? array -> length : 0;

	char *path = (char*) malloc(tlen + 1);
	memset(path, '\0', tlen + 1);
	memcpy(path, array -> data, tlen);

	size_t sz = 0;
	copyHostInteger((unsigned char*) &sz, getVariableContent(cframe, 1), sizeof(size_t));

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

INSTRUCTION_DEF op_unload_font(VFrame *cframe)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));
	
	if (hnd != 0)
		{
		TTF_Font *font = (TTF_Font*) hnd;
		
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

INSTRUCTION_DEF op_init_media_layer(VFrame *cframe)
	{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS|SDL_INIT_TIMER|SDL_INIT_JOYSTICK|SDL_INIT_GAMECONTROLLER|SDL_INIT_HAPTIC) != 0)
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
	DX_CLOSE_WINDOW = SDL_RegisterEvents(1);
	DX_GENERATE_TEXT_BITMAP = SDL_RegisterEvents(1);
	DX_GENERATE_BITMAP_SURFACE = SDL_RegisterEvents(1);
	DX_LOAD_FONT = SDL_RegisterEvents(1);
	DX_UNLOAD_FONT = SDL_RegisterEvents(1);
	DX_GET_TEXT_WIDTH = SDL_RegisterEvents(1);
	DX_SYSTEM_SHUTDOWN = SDL_RegisterEvents(1);
	
	systemEventObject = cframe -> io;
	
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
	unsigned char *result = (unsigned char*) &cframe -> localsData[((DanaType*) cframe -> localsDef) -> fields[0].offset];
	memcpy(result, &ok, sizeof(unsigned char));
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_run_system_loop(VFrame *cframe)
	{
	render_thread();
	
	return RETURN_OK;
	}

INSTRUCTION_DEF op_shutdown(VFrame *cframe)
	{
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

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	// grab global type mappings for anything that we generate here
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	
	integerGT = api -> resolveGlobalTypeMapping(&intType);
	
	windowDataGT = api -> resolveGlobalTypeMapping(&windowDataType);
	
	setInterfaceFunction("makeWindow", op_make_window);
	setInterfaceFunction("startPoly", op_start_poly);
	setInterfaceFunction("addPolyPoint", op_add_poly_point);
	setInterfaceFunction("endPoly", op_end_poly);
	setInterfaceFunction("addRect", op_add_rect);
	setInterfaceFunction("addLine", op_add_line);
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
	setInterfaceFunction("commitBuffer", op_commit_buffer);
	setInterfaceFunction("setBackgroundColor", op_set_background_colour);
	setInterfaceFunction("maximiseWindow", op_maximise_window);
	setInterfaceFunction("minimiseWindow", op_minimise_window);
	setInterfaceFunction("getMaximisedScreenRect", op_get_maximised_screen_rect);
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
	
	SDL_version compiled;
	SDL_version linked;
	
	SDL_VERSION(&compiled);
	SDL_GetVersion(&linked);
	
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
