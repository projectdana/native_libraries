//Written by Barry Porter, 2016

//NOTE: this component is incomplete - it doesn't work on Ubuntu or OS X (I'm not sure how to match the Windows behaviour on those platforms)

/*
SDL rendering plane for graphics

// http://devcry.heiho.net/2009/09/dragging-sdlnoframe-borderless-window.html

// http://lazyfoo.net/tutorials/SDL/

// http://wiki.libsdl.org/CategoryAPI

// http://www.willusher.io/pages/sdl2/

NOTE: setting up an event filter may be a quicker way to handle text width calculation? (i.e. a filter for that specific event)
 - hm; I don't think so - event filter functions run in different threads so rendering code is not stable there
 - is it possible to set up an entirely new rendering context in a different thread, which is purely used for text width calculations? :-/

NOTE: SDL_SetEventFilter for a proper way to do interception??? (instead of forcing an override on the wndproc)

NOTE: using vsync is questionable
 - if we don't use it, may want to look at a technique of rendering everything to a texture (i.e. a surface) then pushing that texture to the screen in one hit

SDL_SetRenderTarget is another thing to render a texture as a whole...

 ----- ----- -----

*/

#include "dana_lib_defs.h"
#include "nli_util.h"
#include "vmi_util.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_syswm.h>

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

static GlobalTypeLink *charArrayGT = NULL;
static GlobalTypeLink *pixelArrayGT = NULL;
static GlobalTypeLink *integerGT = NULL;

#define TYPE_INT {TYPE_LITERAL, X_FLAT, 0, sizeof(size_t), sizeof(size_t), 0}
static const DanaType integerType = TYPE_INT;

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
	} UIText;

typedef struct{
	SDL_Surface *surface;
	int x;
	int y;
	} UIBitmap;

typedef struct{
	struct _uio *objects;
	struct _uio *lastObject;
	int x;
	int y;
	int width;
	int height;

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

typedef struct{
	RGBAInt backgroundColour;

	size_t windowX;
	size_t windowY;

	size_t windowWidth;
	size_t windowHeight;

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

	long int sdlProc;
	} WindowInstance;

typedef struct __li{
	void *data;
	struct __li *next;
	struct __li *prev;
	} ListItem;

static ListItem *instances = NULL;
static ListItem *lastInstance = NULL;

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

	//SDL_Surface *surf = TTF_RenderText_Solid(font, msg, color); //looks crap sometimes in general (inconsistent baseline)
	SDL_Surface *surf = TTF_RenderText_Blended(font, msg, color); //looks crap with complex fonts at "small" sizes (<= 12pt)
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
void renderTextureWH(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h)
	{
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy(ren, tex, NULL, &dst);
	}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
* the texture's width and height
* @param tex The source texture we want to draw
* @param rend The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y)
	{
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	renderTextureWH(tex, ren, x, y, w, h);
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

	SDL_SetTextureBlendMode(st, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(myRenderer, 0, 0, 0, 0);
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

			SDL_Texture *image = renderText(poly -> text, poly -> font, color, myRenderer);
			if (image != NULL)
				{
				renderTexture(image, myRenderer, poly -> x - xScroll, poly -> y - yScroll);
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
				renderTexture(image, myRenderer, poly -> x - xScroll, poly -> y - yScroll);
				SDL_DestroyTexture(image);
				}
			}
			else if (pw -> type == UI_TYPE_BITMAP)
			{
			UIBitmap *poly = (UIBitmap*) pw -> object;

			SDL_Texture *texture = SDL_CreateTextureFromSurface(myRenderer, poly -> surface);

			renderTexture(texture, myRenderer, poly -> x - xScroll, poly -> y - yScroll);
			SDL_DestroyTexture(texture);
			}

		pw = pw -> next;
		}

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

				SDL_Texture *image = renderText(poly -> text, poly -> font, color, instance -> renderer);
				if (image != NULL)
					{
					renderTexture(image, instance -> renderer, poly -> x, poly -> y);
					SDL_DestroyTexture(image);
					}
				}
				else if (pw -> type == UI_TYPE_SURFACE)
				{
				UISurface *poly = (UISurface*) pw -> object;

				SDL_Texture *image = renderSurface(poly, instance -> renderer);

				if (image != NULL)
					{
					renderTexture(image, instance -> renderer, poly -> x, poly -> y);
					SDL_DestroyTexture(image);
					}
				}
				else if (pw -> type == UI_TYPE_BITMAP)
				{
				UIBitmap *poly = (UIBitmap*) pw -> object;

				SDL_Texture *texture = SDL_CreateTextureFromSurface(instance -> renderer, poly -> surface);

				renderTexture(texture, instance -> renderer, poly -> x, poly -> y);
				SDL_DestroyTexture(texture);
				}

			pw = pw -> next;
			}

		#ifdef STATS
		char statString[512];

		SDL_SetRenderDrawColor(instance -> renderer, 255, 240, 240, 100);
		SDL_Rect r; r.x = 10; r.y = instance -> windowHeight - 43; r.w = 300; r.h = 40;
		SDL_RenderFillRect(instance -> renderer, &r);

		SDL_Color color;
		color.r = 60;
		color.g = 0;
		color.b = 0;

		memset(statString, '\0', 512);
		snprintf(statString, 512, "FPS: %u / IPS: %u", stats.lastFPS, stats.lastIPS);

		SDL_Texture *image = renderText(statString, stats.font, color, instance -> renderer);
		if (image != NULL)
			{
			renderTexture(image, instance -> renderer, 10, instance -> windowHeight - 40);
			SDL_DestroyTexture(image);
			}

		memset(statString, '\0', 512);
		snprintf(statString, 512, "DXPS: %u / OCPS: %u / LCPS: %u", stats.lastDXPS, stats.lastOCPS, stats.lastLCPS);

		image = renderText(statString, stats.font, color, instance -> renderer);
		if (image != NULL)
			{
			renderTexture(image, instance -> renderer, 10, instance -> windowHeight - 20);
			SDL_DestroyTexture(image);
			}

		stats.framesRendered ++;
		#endif

		instance -> sceneChanged = false;
		}

	SDL_SetRenderTarget(instance -> renderer, NULL);

	renderTexture(instance -> baseTexture, instance -> renderer, 0, 0);

	SDL_RenderPresent(instance -> renderer);

	return true; // Everything Went OK
	}

#ifdef WINDOWS
static WindowInstance* findWindowInstance(HWND window)
	{
	ListItem *lw = instances;
	while (lw != NULL)
		{
		if (((WindowInstance*) lw -> data) -> windowHandle == window)
			return (WindowInstance*) lw -> data;

		lw = lw -> next;
		}

	return NULL;
	}
#endif

static void fillRegister(VVarR *reg, unsigned char *val)
	{
	reg -> PR.content = val;
	reg -> type = TYPE_LITERAL;
	reg -> etype = sizeof(size_t);
	reg -> PR.vsize = sizeof(size_t);
	
	reg -> typeLink = integerGT -> typeLink;
	}

#ifdef WINDOWS
#ifdef MACHINE_32
long __stdcall WindowProcedureX( HWND window, unsigned int msg, WPARAM wp, LPARAM lp )
#endif
#ifdef MACHINE_64
long long __stdcall WindowProcedureX( HWND window, unsigned int msg, WPARAM wp, LPARAM lp )
#endif
	{
	//WindowInstance *myInstance = mainInstance;
	//TODO: the below is ***HORRIBLE***, find a better way!
	WindowInstance *myInstance = findWindowInstance(window);

	if (myInstance == NULL)
		{
		//printf("error: no window!\n");
		return DefWindowProc( window, msg, wp, lp );
		}

    switch(msg)
		{
        case WM_LBUTTONDOWN:
        	{
	        if (myInstance -> mouseListenerObject != NULL)
	        	{
	            if (myInstance -> mouseEventExitStatus.status == 0)
	            	{
					//unsigned int x = GET_X_LPARAM(lp);
					//unsigned int y = GET_Y_LPARAM(lp);

					POINT point;
					GetCursorPos(&point);

					size_t screenX = point.x;
					size_t screenY = point.y;
					size_t button = 1;

					copyHostInteger((unsigned char*) &myInstance -> mouseDownX, (unsigned char*) &screenX, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> mouseDownY, (unsigned char*) &screenY, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> mouseDownButton, (unsigned char*) &button, sizeof(size_t));

					fillRegister(&myInstance -> mouseParams[0], (unsigned char*) &myInstance -> mouseDownX);
					fillRegister(&myInstance -> mouseParams[1], (unsigned char*) &myInstance -> mouseDownY);
					fillRegister(&myInstance -> mouseParams[2], (unsigned char*) &myInstance -> mouseDownButton);

					api -> callFunction(myInstance -> mouseListenerObject, &myInstance -> mouseListener, 3, &myInstance -> mouseEventExitStatus, myInstance -> mouseParams, 3);
					}
				}

			myInstance -> captureCount ++;

			if (myInstance -> captureCount == 1)
				SetCapture(window);

			//return CallWindowProc((WNDPROC) sdlProc, window, msg, wp, lp);
			return 0;
        	}

        case WM_LBUTTONUP:
        	{
            //fire a click message

			//unsigned int x = GET_X_LPARAM(lp);
			//unsigned int y = GET_Y_LPARAM(lp);

			POINT point;
			GetCursorPos(&point);

			size_t screenX = point.x;
			size_t screenY = point.y;
			size_t button = 1;

			if (myInstance -> clickListenerObject != NULL)
				{
	            //our registers etc. are static/singletons, so we don't launch a 2nd thread until the first one has finished...
	            if (myInstance -> mouseClickExitStatus.status == 0 && (screenX >= myInstance -> windowX && screenX <= myInstance -> windowX + myInstance -> windowWidth && screenY >= myInstance -> windowY && screenY <= myInstance -> windowY + myInstance -> windowHeight))
	            	{
		            size_t x = screenX - myInstance -> windowX;
		            size_t y = screenY - myInstance -> windowY;

					copyHostInteger((unsigned char*) &myInstance -> clickX, (unsigned char*) &x, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> clickY, (unsigned char*) &y, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> clickButton, (unsigned char*) &button, sizeof(size_t));

					fillRegister(&myInstance -> clickParams[0], (unsigned char*) &myInstance -> clickX);
					fillRegister(&myInstance -> clickParams[1], (unsigned char*) &myInstance -> clickY);
					fillRegister(&myInstance -> clickParams[2], (unsigned char*) &myInstance -> clickButton);

					api -> callFunction(myInstance -> clickListenerObject, &myInstance -> clickListener, 3, &myInstance -> mouseClickExitStatus, myInstance -> clickParams, 3);
					}
				}

			if (myInstance -> mouseListenerObject != NULL)
				{
	            if (myInstance -> mouseEventExitStatus.status == 0)
	            	{
					copyHostInteger((unsigned char*) &myInstance -> mouseUpX, (unsigned char*) &screenX, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> mouseUpY, (unsigned char*) &screenY, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> mouseUpButton, (unsigned char*) &button, sizeof(size_t));

					fillRegister(&myInstance -> mouseParams[0], (unsigned char*) &myInstance -> mouseUpX);
					fillRegister(&myInstance -> mouseParams[1], (unsigned char*) &myInstance -> mouseUpY);
					fillRegister(&myInstance -> mouseParams[2], (unsigned char*) &myInstance -> mouseUpButton);

					api -> callFunction(myInstance -> mouseListenerObject, &myInstance -> mouseListener, 4, &myInstance -> mouseEventExitStatus, myInstance -> mouseParams, 3);
					}
				}

			myInstance -> captureCount --;

			if (myInstance -> captureCount == 0)
				ReleaseCapture();

			//return CallWindowProc((WNDPROC) sdlProc, window, msg, wp, lp);
        	return 0;
    		}

        case WM_RBUTTONDOWN:
        	{
	        if (myInstance -> mouseListenerObject != NULL)
	        	{
	            if (myInstance -> mouseEventExitStatus.status == 0)
	            	{
					//unsigned int x = GET_X_LPARAM(lp);
					//unsigned int y = GET_Y_LPARAM(lp);

					POINT point;
					GetCursorPos(&point);

					size_t screenX = point.x;
					size_t screenY = point.y;
					size_t button = 2;

					copyHostInteger((unsigned char*) &myInstance -> mouseDownX, (unsigned char*) &screenX, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> mouseDownY, (unsigned char*) &screenY, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> mouseDownButton, (unsigned char*) &button, sizeof(size_t));

					fillRegister(&myInstance -> mouseParams[0], (unsigned char*) &myInstance -> mouseDownX);
					fillRegister(&myInstance -> mouseParams[1], (unsigned char*) &myInstance -> mouseDownY);
					fillRegister(&myInstance -> mouseParams[2], (unsigned char*) &myInstance -> mouseDownButton);

					api -> callFunction(myInstance -> mouseListenerObject, &myInstance -> mouseListener, 3, &myInstance -> mouseEventExitStatus, myInstance -> mouseParams, 3);
					}
				}

			myInstance -> captureCount ++;

			if (myInstance -> captureCount == 1)
				SetCapture(window);

			//return CallWindowProc((WNDPROC) sdlProc, window, msg, wp, lp);
			return 0;
        	}

        case WM_RBUTTONUP:
        	{
            //fire a click message

			//unsigned int x = GET_X_LPARAM(lp);
			//unsigned int y = GET_Y_LPARAM(lp);

			POINT point;
			GetCursorPos(&point);

			size_t screenX = point.x;
			size_t screenY = point.y;
			size_t button = 2;

			if (myInstance -> clickListenerObject != NULL)
				{
	            //our registers etc. are static/singletons, so we don't launch a 2nd thread until the first one has finished...
	            if (myInstance -> mouseClickExitStatus.status == 0 && (screenX >= myInstance -> windowX && screenX <= myInstance -> windowX + myInstance -> windowWidth && screenY >= myInstance -> windowY && screenY <= myInstance -> windowY + myInstance -> windowHeight))
	            	{
		            size_t x = screenX - myInstance -> windowX;
		            size_t y = screenY - myInstance -> windowY;

					copyHostInteger((unsigned char*) &myInstance -> clickX, (unsigned char*) &x, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> clickY, (unsigned char*) &y, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> clickButton, (unsigned char*) &button, sizeof(size_t));

					fillRegister(&myInstance -> clickParams[0], (unsigned char*) &myInstance -> clickX);
					fillRegister(&myInstance -> clickParams[1], (unsigned char*) &myInstance -> clickY);
					fillRegister(&myInstance -> clickParams[2], (unsigned char*) &myInstance -> clickButton);

					api -> callFunction(myInstance -> clickListenerObject, &myInstance -> clickListener, 3, &myInstance -> mouseClickExitStatus, myInstance -> clickParams, 3);
					}
				}

			if (myInstance -> mouseListenerObject != NULL)
				{
	            if (myInstance -> mouseEventExitStatus.status == 0)
	            	{
					copyHostInteger((unsigned char*) &myInstance -> mouseUpX, (unsigned char*) &screenX, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> mouseUpY, (unsigned char*) &screenY, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> mouseUpButton, (unsigned char*) &button, sizeof(size_t));

					fillRegister(&myInstance -> mouseParams[0], (unsigned char*) &myInstance -> mouseUpX);
					fillRegister(&myInstance -> mouseParams[1], (unsigned char*) &myInstance -> mouseUpY);
					fillRegister(&myInstance -> mouseParams[2], (unsigned char*) &myInstance -> mouseUpButton);

					api -> callFunction(myInstance -> mouseListenerObject, &myInstance -> mouseListener, 4, &myInstance -> mouseEventExitStatus, myInstance -> mouseParams, 3);
					}
				}

			myInstance -> captureCount --;

			if (myInstance -> captureCount == 0)
				ReleaseCapture();

			//return CallWindowProc((WNDPROC) sdlProc, window, msg, wp, lp);
        	return 0;
    		}

        case WM_MBUTTONDOWN:
        	{
	        if (myInstance -> mouseListenerObject != NULL)
	        	{
	            if (myInstance -> mouseEventExitStatus.status == 0)
	            	{
					//unsigned int x = GET_X_LPARAM(lp);
					//unsigned int y = GET_Y_LPARAM(lp);

					POINT point;
					GetCursorPos(&point);

					size_t screenX = point.x;
					size_t screenY = point.y;
					size_t button = 3;

					copyHostInteger((unsigned char*) &myInstance -> mouseDownX, (unsigned char*) &screenX, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> mouseDownY, (unsigned char*) &screenY, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> mouseDownButton, (unsigned char*) &button, sizeof(size_t));

					fillRegister(&myInstance -> mouseParams[0], (unsigned char*) &myInstance -> mouseDownX);
					fillRegister(&myInstance -> mouseParams[1], (unsigned char*) &myInstance -> mouseDownY);
					fillRegister(&myInstance -> mouseParams[2], (unsigned char*) &myInstance -> mouseDownButton);

					api -> callFunction(myInstance -> mouseListenerObject, &myInstance -> mouseListener, 3, &myInstance -> mouseEventExitStatus, myInstance -> mouseParams, 3);
					}
				}

			myInstance -> captureCount ++;

			if (myInstance -> captureCount == 1)
				SetCapture(window);

			//return CallWindowProc((WNDPROC) sdlProc, window, msg, wp, lp);
			return 0;
        	}

        case WM_MBUTTONUP:
        	{
            //fire a click message

			//unsigned int x = GET_X_LPARAM(lp);
			//unsigned int y = GET_Y_LPARAM(lp);

			POINT point;
			GetCursorPos(&point);

			size_t screenX = point.x;
			size_t screenY = point.y;
			size_t button = 3;

			if (myInstance -> clickListenerObject != NULL)
				{
	            //our registers etc. are static/singletons, so we don't launch a 2nd thread until the first one has finished...
	            if (myInstance -> mouseClickExitStatus.status == 0 && (screenX >= myInstance -> windowX && screenX <= myInstance -> windowX + myInstance -> windowWidth && screenY >= myInstance -> windowY && screenY <= myInstance -> windowY + myInstance -> windowHeight))
	            	{
		            size_t x = screenX - myInstance -> windowX;
		            size_t y = screenY - myInstance -> windowY;

					copyHostInteger((unsigned char*) &myInstance -> clickX, (unsigned char*) &x, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> clickY, (unsigned char*) &y, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> clickButton, (unsigned char*) &button, sizeof(size_t));

					fillRegister(&myInstance -> clickParams[0], (unsigned char*) &myInstance -> clickX);
					fillRegister(&myInstance -> clickParams[1], (unsigned char*) &myInstance -> clickY);
					fillRegister(&myInstance -> clickParams[2], (unsigned char*) &myInstance -> clickButton);

					api -> callFunction(myInstance -> clickListenerObject, &myInstance -> clickListener, 3, &myInstance -> mouseClickExitStatus, myInstance -> clickParams, 3);
					}
				}

			if (myInstance -> mouseListenerObject != NULL)
				{
	            if (myInstance -> mouseEventExitStatus.status == 0)
	            	{
					copyHostInteger((unsigned char*) &myInstance -> mouseUpX, (unsigned char*) &screenX, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> mouseUpY, (unsigned char*) &screenY, sizeof(size_t));
					copyHostInteger((unsigned char*) &myInstance -> mouseUpButton, (unsigned char*) &button, sizeof(size_t));

					fillRegister(&myInstance -> mouseParams[0], (unsigned char*) &myInstance -> mouseUpX);
					fillRegister(&myInstance -> mouseParams[1], (unsigned char*) &myInstance -> mouseUpY);
					fillRegister(&myInstance -> mouseParams[2], (unsigned char*) &myInstance -> mouseUpButton);

					api -> callFunction(myInstance -> mouseListenerObject, &myInstance -> mouseListener, 4, &myInstance -> mouseEventExitStatus, myInstance -> mouseParams, 3);
					}
				}

			myInstance -> captureCount --;

			if (myInstance -> captureCount == 0)
				ReleaseCapture();

			//return CallWindowProc((WNDPROC) sdlProc, window, msg, wp, lp);
        	return 0;
    		}

    	//TODO: mouse capture outside the window: http://msdn.microsoft.com/en-gb/library/windows/desktop/gg153550(v=vs.85).aspx
        case WM_MOUSEMOVE:
        	{
	        //NOTE: for all of these cases, instead of ignoring additional events that happen before the current one finishes, it might be better to queue them up in a (probably fixed-size) buffer so that they get delivered still when the currently-executing handler finishes...

			if (myInstance -> mouseListenerObject != NULL)
				{
	            if (myInstance -> mouseEventExitStatus.status == 0)
	            	{
					size_t x = GET_X_LPARAM(lp);
					size_t y = GET_Y_LPARAM(lp);

					POINT point;
					GetCursorPos(&point);

					size_t screenX = point.x;
					size_t screenY = point.y;

					//printf("Mouse move: %u/%u\n", screenX, screenY);

					//check if the x and y from the lparam are the same as the global x/y; if not discard the event
					// - this is a performance-boosting filter which avoids spurious mouse move events caused by the window itself being repositioned

					if ((screenX != myInstance -> lastMouseMoveX || screenY != myInstance -> lastMouseMoveY)
						&& (myInstance -> windowX + x == screenX && myInstance -> windowY + y == screenY))
						{
						myInstance -> lastMouseMoveX = screenX;
						myInstance -> lastMouseMoveY = screenY;

						copyHostInteger((unsigned char*) &myInstance -> mouseMoveX, (unsigned char*) &screenX, sizeof(size_t));
						copyHostInteger((unsigned char*) &myInstance -> mouseMoveY, (unsigned char*) &screenY, sizeof(size_t));

						fillRegister(&myInstance -> mouseParams[0], (unsigned char*) &myInstance -> mouseMoveX);
						fillRegister(&myInstance -> mouseParams[1], (unsigned char*) &myInstance -> mouseMoveY);

						api -> callFunction(myInstance -> mouseListenerObject, &myInstance -> mouseListener, 5, &myInstance -> mouseEventExitStatus, myInstance -> mouseParams, 2);
						}
					}
				}

			return 0;
        	}

		default:
			//call SDL's window proc
			//return CallWindowProc((WNDPROC) myInstance -> sdlProc, window, msg, wp, lp);
			return DefWindowProc( window, msg, wp, lp );
		}

	return 0;
	}
#endif

#ifdef WINDOWS
unsigned int windowNumber = 0;
#endif

static void* CreateWindowNative(int w, int h, unsigned int x, unsigned int y)
	{

#ifdef WINDOWS
    HWND hwnd;
    WNDCLASS wc;

	windowNumber ++;

	char *cn = malloc(1024);
	memset(cn, '\0', 1024);
	//snprintf(cn, 1024, "DX-UI-%u", windowNumber);
	//snprintf(cn, 1024, "DX-UI-");
	strcat(cn, "DX-UI-");
	itoa(windowNumber, &cn[strlen(cn)], 10);

    wc.style = 0;
    wc.lpfnWndProc = WindowProcedureX;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hIcon = LoadIcon(0, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = cn;

    if (!RegisterClass(&wc)) {
        return 0;
    }

    hwnd = CreateWindow(cn, "", WS_POPUP, x, y, w, h, NULL, NULL, GetModuleHandle(NULL), NULL);

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, SW_HIDE);

    return hwnd;
#endif
#ifdef OSX

#else
#ifdef LINUX
	Window window = 0;
	Display *dpy = XOpenDisplay(NULL);

	if (dpy != NULL)
		{
		window = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), x, y, w, h, 0, 0, 0);
		XMapRaised(dpy, window);
		XSync(dpy, false);
		}

	return (void*) window;
#endif
#endif
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

	#ifdef WINDOWS
	myInstance -> renderLock = CreateSemaphore(NULL, 1, 1, NULL);
	#endif

	//temporary global variable:
	//mainInstance = myInstance;

	//ListItem *myInstanceItem =
	addListItem(&instances, &lastInstance, myInstance);

	#ifdef WINDOWS
	#define CREATE_NATIVE
	#endif
	#ifdef LINUX
	#define CREATE_SDL //TODO: bug in Linux where createNative causes a hang on SDL_CreateRenderer
	#endif
	#ifdef OSX
	#define CREATE_SDL //TODO: bug in Linux where createNative causes a hang on SDL_CreateRenderer
	#endif

	#ifdef CREATE_SDL
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	myInstance -> win = SDL_CreateWindow("Dana UI", myInstance -> windowX, myInstance -> windowY, myInstance -> windowWidth, myInstance -> windowHeight, SDL_WINDOW_HIDDEN | SDL_WINDOW_BORDERLESS);

	if(SDL_GetWindowWMInfo(myInstance -> win, &info)) {
		switch(info.subsystem) {
			  case SDL_SYSWM_UNKNOWN:   break;
			  #ifdef WINDOWS
			  case SDL_SYSWM_WINDOWS:   myInstance -> windowHandle = info.info.win.window;
			  							break;
			  #endif
			  #ifdef OSX
			  case SDL_SYSWM_COCOA:		myInstance -> windowHandle = info.info.cocoa.window;
			  							break;
			  #else
			  #ifdef LINUX
			  case SDL_SYSWM_X11:       myInstance -> windowHandle = info.info.x11.window;
			  							myInstance -> displayHandle = info.info.x11.display;
			  							break;
			  #endif
			  #endif
			  case SDL_SYSWM_DIRECTFB:
			  							break;
			  case SDL_SYSWM_UIKIT:
			  							break;
			  default: break;
			}
		}

	if (myInstance -> win == NULL){
		printf("null window\n");
		//SDL_GetError()
		return NULL;
	}

	#ifdef LINUX
	//XSelectInput(myInstance -> displayHandle, myInstance -> windowHandle, ButtonMotionMask);
	#endif

	#endif

	#ifdef CREATE_NATIVE
	myInstance -> windowHandle = CreateWindowNative(myInstance -> windowWidth, myInstance -> windowHeight, myInstance -> windowX, myInstance -> windowY);

	myInstance -> win = SDL_CreateWindowFrom(myInstance -> windowHandle);
	if (myInstance -> win == NULL){
		printf("Critical error in GUI rendering subsystem: WCE 001 [%s]\n", SDL_GetError());
		return NULL;
	}

	SDL_SetWindowTitle(myInstance -> win, "Dana UI");
	#endif

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

	// -- insert our custom windowproc in between Windows and SDL (for proper mouse capture) --

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
	} CloseWindowInfo;

typedef struct{
	char *text;
	TTF_Font *font;
	VFrame *vframe;
	unsigned char *resultData;
	SDL_Color color;
	} GenerateTextBitmapData;

typedef struct{
	UIBitmap *bitmapData;
	VFrame *vframe;
	unsigned char *sourceData;
	} GenerateBitmapSurfaceData;

typedef struct{
	char *fontPath;
	size_t size;
	VFrame *vframe;
	} LoadFontData;

typedef struct{
	char *text;
	TTF_Font *font;
	VFrame *vframe;
	} GetTextWidthInfo;

#ifdef WINDOWS
static HANDLE graphicsShutdownLock;
static HANDLE graphicsStartupLock;
#endif

#ifdef OSX
static dispatch_semaphore_t graphicsShutdownLock;
static dispatch_semaphore_t graphicsStartupLock;
#else
#ifdef LINUX
static sem_t graphicsShutdownLock;
static sem_t graphicsStartupLock;
#endif
#endif

static unsigned int DX_NEW_WINDOW_EVENT = 0;
static unsigned int DX_SWAP_BUFFERS_EVENT = 0;
static unsigned int DX_SET_WINDOW_POSITION = 0;
static unsigned int DX_SET_WINDOW_TITLE = 0;
static unsigned int DX_MAXIMISE_WINDOW = 0;
static unsigned int DX_MINIMISE_WINDOW = 0;
static unsigned int DX_SHOW_WINDOW = 0;
static unsigned int DX_HIDE_WINDOW = 0;
static unsigned int DX_CLOSE_WINDOW = 0;
static unsigned int DX_GENERATE_TEXT_BITMAP = 0;
static unsigned int DX_GENERATE_BITMAP_SURFACE = 0;
static unsigned int DX_LOAD_FONT = 0;
static unsigned int DX_GET_TEXT_WIDTH = 0;

#ifdef WINDOWS
DWORD WINAPI render_thread(LPVOID ptr)
#else
static void* render_thread(void *ptr)
#endif
	{
	#ifdef LINUX
	pthread_detach(pthread_self());
	#endif

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
		printf("SDL INIT FAILED");
		//std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		}

	if (TTF_Init() != 0)
		{
		//logSDLError(std::cout, "TTF_Init");
		}

	DX_NEW_WINDOW_EVENT = SDL_RegisterEvents(1);
	DX_SWAP_BUFFERS_EVENT = SDL_RegisterEvents(1);
	DX_SET_WINDOW_POSITION = SDL_RegisterEvents(1);
	DX_SET_WINDOW_TITLE = SDL_RegisterEvents(1);
	DX_MAXIMISE_WINDOW = SDL_RegisterEvents(1);
	DX_MINIMISE_WINDOW = SDL_RegisterEvents(1);
	DX_SHOW_WINDOW = SDL_RegisterEvents(1);
	DX_HIDE_WINDOW = SDL_RegisterEvents(1);
	DX_CLOSE_WINDOW = SDL_RegisterEvents(1);
	DX_GENERATE_TEXT_BITMAP = SDL_RegisterEvents(1);
	DX_GENERATE_BITMAP_SURFACE = SDL_RegisterEvents(1);
	DX_LOAD_FONT = SDL_RegisterEvents(1);
	DX_GET_TEXT_WIDTH = SDL_RegisterEvents(1);

	#ifdef WINDOWS
	graphicsShutdownLock = CreateSemaphore(NULL, 0, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_t *sem;
    sem = &graphicsShutdownLock;
    *sem = dispatch_semaphore_create(0);
	#else
	#ifdef LINUX
	sem_init(&graphicsShutdownLock, 0, 0);
	#endif
	#endif

	#ifdef WINDOWS
	ReleaseSemaphore(graphicsStartupLock, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_signal(graphicsStartupLock);
	#else
	#ifdef LINUX
	sem_post(&graphicsStartupLock);
	#endif
	#endif

	SDL_Event e;
	bool quit = false;

	//we draw a frame the first time round the loop, even if no frame has been passed in, just to clear the screen with the background colour
	bool newFrame = true;

	#ifdef STATS
	stats.font = TTF_OpenFont("C:\\ProgramFiles\\Dana\\resources-ext\\fonts\\LiberationMono.ttf", 14);
	if (stats.font == NULL)
		{
		printf("open font error: %s [%s]\n", SDL_GetError(), TTF_GetError());
		}
	#endif

	fflush(stdout);

	while (!quit)
		{
		//Read user input & handle it
		#ifdef OSX
		#else
		#ifdef LINUX
		ListItem *lw = instances;
		while (lw != NULL)
			{
			WindowInstance *wi = (WindowInstance*) lw -> data;

			XEvent xev;
			while (XCheckWindowEvent(wi -> displayHandle, wi -> windowHandle, ButtonPressMask, &xev))
				{
				printf("beats\n");
			}

			lw = lw -> next;
		}
		#endif
		#endif

		if (SDL_WaitEvent(&e)) //does this make things more jittery?
		{
		do
			{
			if (e.type == SDL_QUIT)
				{
				quit = true;
				}
				else if (e.type == SDL_WINDOWEVENT)
				{
				if (e.window.event == SDL_WINDOWEVENT_MAXIMIZED ||
					e.window.event == SDL_WINDOWEVENT_RESTORED)
					{
					newFrame = true;
					}
					//for active / inactive notifications (?):
					//SDL_WINDOWEVENT_FOCUS_GAINED
					//SDL_WINDOWEVENT_FOCUS_LOST
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
				}
				else if (e.type == DX_HIDE_WINDOW)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;
				SDL_HideWindow(wi -> win);
				}
				else if (e.type == DX_CLOSE_WINDOW)
				{
				WindowInstance *wi = (WindowInstance*) e.user.data1;

				removeListItem(&instances, &lastInstance, wi);

				if (wi -> clickListener.content != NULL)
					{
					wi -> clickListenerObject = NULL;
					detachPointer(&wi -> clickListener);
					wi -> clickListener.content = NULL;
					}

				if (wi -> mouseListener.content != NULL)
					{
					wi -> mouseListenerObject = NULL;
					detachPointer(&wi -> mouseListener);
					wi -> mouseListener.content = NULL;
					}

				if (wi -> baseTexture != NULL)
					SDL_DestroyTexture(wi -> baseTexture);

				SDL_DestroyRenderer(wi -> renderer);
				SDL_DestroyWindow(wi -> win);

				//TODO: cleanup wi -> frontBuffer / wi -> backBuffer ???
				free(wi);

				VFrame *frame = (VFrame*) e.user.data2;

				api -> deferredReturn(frame);
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

				pixelArrayH -> gtLink = pixelArrayGT;
				api -> incrementGTRefCount(pixelArrayH -> gtLink);

				pixelArrayH -> owner = frame -> blocking -> instance;

				arrayPTR -> content = (unsigned char*) pixelArrayH;
				attachPointer(arrayPTR, &pixelArrayH -> scope.scopePointers);
				pixelArrayH -> refCount ++;
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

				free(data -> text);
				free(data);

				SDL_FreeSurface(surf);

				api -> deferredReturn(frame);
				}
				else if (e.type == DX_GENERATE_BITMAP_SURFACE)
				{
				GenerateBitmapSurfaceData *data = (GenerateBitmapSurfaceData*) e.user.data1;

				LiveData *bitmapData = (LiveData*) data -> sourceData;
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

				data -> bitmapData -> surface = SDL_CreateRGBSurface(0, w, h, 32, rmask, gmask, bmask, amask);

				if (data -> bitmapData -> surface == NULL)
					{
					printf("FAIL!\n");
					}

				// -- write the pixels
				SDL_LockSurface(data -> bitmapData -> surface);
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

						((Uint32*) data -> bitmapData -> surface -> pixels)[(i*w)+j] = pixel;

						ndx += 4;
						}
					}
				SDL_UnlockSurface(data -> bitmapData -> surface);

				// -- clean up and return

				VFrame *frame = data -> vframe;

				free(data);

				api -> deferredReturn(frame);
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
				free(lfd);

				size_t xs = (size_t) font;
				size_t *result = (size_t*) &frame -> localsData[((DanaType*) ((StructuredType*) frame -> scopes[0].scope.etype) -> structure.content)[0].offset];
				memcpy(result, &xs, sizeof(size_t));

				api -> deferredReturn(frame);
				}
				else if (e.type == DX_GET_TEXT_WIDTH)
				{
				GetTextWidthInfo *gwi = (GetTextWidthInfo*) e.user.data1;
				VFrame *frame = gwi -> vframe;

				size_t width = 0;
				int sdl_width = 0;
				TTF_SizeText(gwi -> font, gwi -> text, &sdl_width, NULL);
				width = sdl_width;

				free(gwi -> text);
				free(gwi);

				size_t *result = (size_t*) &frame -> localsData[((DanaType*) ((StructuredType*) frame -> scopes[0].scope.etype) -> structure.content)[0].offset];

				copyHostInteger((unsigned char*) result, (unsigned char*) &width, sizeof(size_t));

				api -> deferredReturn(frame);
				}
			} while(SDL_PollEvent(&e)); //power through all other events to clear the queue
		}

		if (newFrame)
			{
			//fire the draw method of each window
			ListItem *lw = instances;
			while (lw != NULL)
				{
				WindowInstance *wi = (WindowInstance*) lw -> data;

				int cw, ch;
				SDL_GetWindowSize(wi -> win, &cw, &ch);
				if (cw != wi -> windowWidth || ch != wi -> windowHeight)
					SDL_SetWindowSize(wi -> win, wi -> windowWidth, wi -> windowHeight);

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

	//it's over
	SDL_Quit();

	#ifdef WINDOWS
	ReleaseSemaphore(graphicsShutdownLock, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_signal(graphicsShutdownLock);
	#else
	#ifdef LINUX
	sem_post(&graphicsShutdownLock);
	#endif
	#endif

	#ifdef WINDOWS
	return 0;
	#else
	return NULL;
	#endif
	}

#ifdef STATS
#ifdef WINDOWS
DWORD WINAPI stats_thread(LPVOID ptr)
#else
static void* stats_thread(void *ptr)
#endif
	{
	#ifdef LINUX
	pthread_detach(pthread_self());
	#endif

	size_t ms = 1024;

	while (true)
		{
		#ifdef WINDOWS
		Sleep(ms);
		#endif
		#ifdef LINUX
		struct timespec ts;
		ts.tv_sec = ms / 1000;
		ts.tv_nsec = (ms - (ts.tv_sec * 1000)) * 1000000;
		nanosleep(&ts, NULL);
		#endif

		stats.lastIPS = api -> getStat(STAT_TYPE_INSTRUCTIONS_EXECUTED);

		stats.lastDXPS = api -> getStat(STAT_TYPE_DXU_CALLS_MADE);

		stats.lastOCPS = api -> getStat(STAT_TYPE_OBJECT_CALLS_MADE);

		stats.lastLCPS = api -> getStat(STAT_TYPE_LOCAL_CALLS_MADE);

		stats.lastFPS = stats.framesRendered;
		stats.framesRendered = 0;
		}

	#ifdef WINDOWS
	return 0;
	#else
	return NULL;
	#endif
	}
#endif

#ifdef WINDOWS
DWORD WINAPI make_window_thread( LPVOID ptr )
#else
static void * make_window_thread(void *ptr)
#endif
	{
	#ifdef LINUX
	pthread_detach(pthread_self());
	#endif

	VFrame *cframe = (VFrame*) ptr;

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
	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset];
	memcpy(result, &xs, sizeof(size_t));

	api -> deferredReturn(cframe);

	free(mwInfo);

	#ifdef WINDOWS
	return 0;
	#else
	return NULL;
	#endif
	}

INSTRUCTION_DEF op_make_window(INSTRUCTION_PARAM_LIST)
	{
	#ifdef WINDOWS
	HANDLE th = CreateThread(
            NULL,                   // default security attributes
            0,                      // use default stack size
            make_window_thread,  		     // thread function name
            cframe,          // argument to thread function
            0,                      // use default creation flags
            NULL);   // returns the thread identifier

	CloseHandle(th);
	#else
	int err = 0;
	pthread_t th;
	memset(&th, '\0', sizeof(pthread_t));

	if ((err = pthread_create(&th, NULL, make_window_thread, cframe)) != 0){}
	#endif

	return RETURN_DEFERRED;
	}

INSTRUCTION_DEF op_start_poly(INSTRUCTION_PARAM_LIST)
	{
	//make a new poly for the back-buffer
	/*
	WindowInstance *instance = mainInstance;

	UIPolygon *p = (UIPolygon*) malloc(sizeof(UIPolygon));
	UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
	memset(uio, '\0', sizeof(UIObject));

	if (instance -> backBufferEnd == NULL)
		{
		instance -> backBuffer = uio;
		instance -> backBufferEnd = uio;
		}
		else
		{
		instance -> backBufferEnd -> next = uio;
		instance -> backBufferEnd = uio;
		}

	uio -> type = UI_TYPE_POLYGON;
	uio -> object = p;

	p -> points = NULL;
	p -> lastPoint = NULL;

	instance -> lastPolygon = p;
	*/
	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_add_poly_point(INSTRUCTION_PARAM_LIST)
	{
	//WindowInstance *instance = mainInstance;

	/*
	size_t x = 0;
	copyHostInteger((unsigned char*) &x, getVariableContent(cframe, 0), 4);

	size_t y = 0;
	copyHostInteger((unsigned char*) &y, getVariableContent(cframe, 1), 4);

	size_t r = 0;
	copyHostInteger((unsigned char*) &r, getVariableContent(cframe, 2), 1);

	size_t g = 0;
	copyHostInteger((unsigned char*) &g, getVariableContent(cframe, 3), 1);

	size_t b = 0;
	copyHostInteger((unsigned char*) &b, getVariableContent(cframe, 4), 1);

	size_t a = 0;
	copyHostInteger((unsigned char*) &a, getVariableContent(cframe, 5), 1);

	UIPoint *p = (UIPoint*) malloc(sizeof(UIPoint));

	p -> next = NULL;

	if (instance -> lastPolygon -> lastPoint == NULL)
		{
		instance -> lastPolygon -> points = p;
		instance -> lastPolygon -> lastPoint = p;
		}
		else
		{
		instance -> lastPolygon -> lastPoint -> next = p;
		instance -> lastPolygon -> lastPoint = p;
		}

	//p -> x = -1.0f + (2.0f * windowPixelWidth / x);
	p -> x = -1.0f + x / 100.0f;
	p -> y = 1.0f - y / 100.0f;
	p -> z = 0.0f;

	//if (r == 0) r = 1;
	//if (g == 0) g = 1;
	//if (b == 0) b = 1;

	//p -> r = r / 255.0f;
	//p -> g = g / 255.0f;
	//p -> b = b / 255.0f;
	//p -> a = a / 255.0f;

	p -> r = r;
	p -> g = g;
	p -> b = b;
	p -> a = a;
	*/

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_end_poly(INSTRUCTION_PARAM_LIST)
	{
	//WindowInstance *instance = mainInstance;

	/*
	//make a new poly for the back-buffer
	UIPolygon *poly = instance -> lastPolygon;

	unsigned int count = 0;

	UIPoint *pw = poly -> points;
	while (pw != NULL)
		{
		count ++;
		pw = pw -> next;
		}
	*/

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_add_rect(INSTRUCTION_PARAM_LIST)
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

		if (instance -> surfaceStack != NULL)
			{
			UISurface *surface = (UISurface*) instance -> surfaceStack -> object;

			if (surface -> lastObject == NULL)
				surface -> objects = uio;
				else
				surface -> lastObject -> next = uio;

			surface -> lastObject = uio;
			}
			else
			{
			if (instance -> backBufferEnd == NULL)
				instance -> backBuffer = uio;
				else
				instance -> backBufferEnd -> next = uio;

			instance -> backBufferEnd = uio;
			}

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

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_add_line(INSTRUCTION_PARAM_LIST)
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

		if (instance -> surfaceStack != NULL)
			{
			UISurface *surface = (UISurface*) instance -> surfaceStack -> object;

			if (surface -> lastObject == NULL)
				surface -> objects = uio;
				else
				surface -> lastObject -> next = uio;

			surface -> lastObject = uio;
			}
			else
			{
			if (instance -> backBufferEnd == NULL)
				instance -> backBuffer = uio;
				else
				instance -> backBufferEnd -> next = uio;

			instance -> backBufferEnd = uio;
			}

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

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_add_point(INSTRUCTION_PARAM_LIST)
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

		if (instance -> surfaceStack != NULL)
			{
			UISurface *surface = (UISurface*) instance -> surfaceStack -> object;

			if (surface -> lastObject == NULL)
				surface -> objects = uio;
				else
				surface -> lastObject -> next = uio;

			surface -> lastObject = uio;
			}
			else
			{
			if (instance -> backBufferEnd == NULL)
				instance -> backBuffer = uio;
				else
				instance -> backBufferEnd -> next = uio;

			instance -> backBufferEnd = uio;
			}

		uio -> type = UI_TYPE_POINT;
		uio -> object = poly;

		poly -> x = x;
		poly -> y = y;

		poly -> r = r;
		poly -> g = g;
		poly -> b = b;
		poly -> a = a;
		}

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_add_bitmap(INSTRUCTION_PARAM_LIST)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		size_t x = 0;
		copyHostInteger((unsigned char*) &x, getVariableContent(cframe, 2), sizeof(size_t));

		size_t y = 0;
		copyHostInteger((unsigned char*) &y, getVariableContent(cframe, 3), sizeof(size_t));

		// -- create the container --

		UIBitmap *poly = (UIBitmap*) malloc(sizeof(UIBitmap));
		UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
		memset(uio, '\0', sizeof(UIObject));

		if (instance -> surfaceStack != NULL)
			{
			UISurface *surface = (UISurface*) instance -> surfaceStack -> object;

			if (surface -> lastObject == NULL)
				surface -> objects = uio;
				else
				surface -> lastObject -> next = uio;

			surface -> lastObject = uio;
			}
			else
			{
			if (instance -> backBufferEnd == NULL)
				instance -> backBuffer = uio;
				else
				instance -> backBufferEnd -> next = uio;

			instance -> backBufferEnd = uio;
			}

		uio -> type = UI_TYPE_BITMAP;
		uio -> object = poly;

		poly -> x = x;
		poly -> y = y;

		GenerateBitmapSurfaceData *gb = malloc(sizeof(GenerateBitmapSurfaceData));
		memset(gb, '\0', sizeof(GenerateBitmapSurfaceData));
		gb -> sourceData = ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;
		gb -> vframe = cframe;
		gb -> bitmapData = poly;

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_GENERATE_BITMAP_SURFACE;
		newEvent.user.data1 = gb;

		SDL_PushEvent(&newEvent);

		return RETURN_DEFERRED;
		}

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_add_text(INSTRUCTION_PARAM_LIST)
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

		LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 3)) -> content;

		size_t r = 0;
		copyHostInteger((unsigned char*) &r, getVariableContent(cframe, 4), 1);

		size_t g = 0;
		copyHostInteger((unsigned char*) &g, getVariableContent(cframe, 5), 1);

		size_t b = 0;
		copyHostInteger((unsigned char*) &b, getVariableContent(cframe, 6), 1);

		size_t a = 0;
		copyHostInteger((unsigned char*) &a, getVariableContent(cframe, 7), 1);

		size_t tlen = array != NULL ? array -> length : 0;

		if (tlen > 0)
			{
			// -- create the container --

			UIText *poly = (UIText*) malloc(sizeof(UIText));
			UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
			memset(uio, '\0', sizeof(UIObject));

			if (instance -> surfaceStack != NULL)
				{
				UISurface *surface = (UISurface*) instance -> surfaceStack -> object;

				if (surface -> lastObject == NULL)
					surface -> objects = uio;
					else
					surface -> lastObject -> next = uio;

				surface -> lastObject = uio;
				}
				else
				{
				if (instance -> backBufferEnd == NULL)
					instance -> backBuffer = uio;
					else
					instance -> backBufferEnd -> next = uio;

				instance -> backBufferEnd = uio;
				}

			//poly -> font = defaultFont;

			uio -> type = UI_TYPE_TEXT;
			uio -> object = poly;

			poly -> textLen = tlen;
			poly -> text = (char*) malloc(tlen + 1);
			memset(poly -> text, '\0', tlen + 1);
			memcpy(poly -> text, array -> data, array -> length);

			poly -> x = x;
			poly -> y = y;

			poly -> r = r;
			poly -> g = g;
			poly -> b = b;
			poly -> a = a;
			}
		}

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_add_text_with(INSTRUCTION_PARAM_LIST)
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

		LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 4)) -> content;

		size_t r = 0;
		copyHostInteger((unsigned char*) &r, getVariableContent(cframe, 5), 1);

		size_t g = 0;
		copyHostInteger((unsigned char*) &g, getVariableContent(cframe, 6), 1);

		size_t b = 0;
		copyHostInteger((unsigned char*) &b, getVariableContent(cframe, 7), 1);

		size_t a = 0;
		copyHostInteger((unsigned char*) &a, getVariableContent(cframe, 8), 1);

		size_t tlen = array != NULL ? array -> length : 0;

		if (tlen > 0)
			{
			// -- create the container --

			UIText *poly = (UIText*) malloc(sizeof(UIText));
			UIObject *uio = (UIObject*) malloc(sizeof(UIObject));
			memset(uio, '\0', sizeof(UIObject));

			if (instance -> surfaceStack != NULL)
				{
				UISurface *surface = (UISurface*) instance -> surfaceStack -> object;

				if (surface -> lastObject == NULL)
					surface -> objects = uio;
					else
					surface -> lastObject -> next = uio;

				surface -> lastObject = uio;
				}
				else
				{
				if (instance -> backBufferEnd == NULL)
					instance -> backBuffer = uio;
					else
					instance -> backBufferEnd -> next = uio;

				instance -> backBufferEnd = uio;
				}

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
			}
		}

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_get_text_width_with(INSTRUCTION_PARAM_LIST)
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

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_GET_TEXT_WIDTH;
		newEvent.user.data1 = gwi;

		SDL_PushEvent(&newEvent);

		return RETURN_DEFERRED;
		}

	size_t *result = (size_t*) &cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset];
	copyHostInteger((unsigned char*) result, (unsigned char*) &width, sizeof(size_t));

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_push_surface(INSTRUCTION_PARAM_LIST)
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

		if (instance -> backBufferEnd == NULL)
			{
			instance -> backBuffer = newObject;
			instance -> backBufferEnd = newObject;
			}
			else
			{
			instance -> backBufferEnd -> next = newObject;
			instance -> backBufferEnd = newObject;
			}

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
		// --

		newObject -> type = UI_TYPE_SURFACE;

		newSurface -> x = x;
		newSurface -> y = y;
		newSurface -> width = w;
		newSurface -> height = h;

		newSurface -> xScroll = xs;
		newSurface -> yScroll = ys;
		}

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_pop_surface(INSTRUCTION_PARAM_LIST)
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

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_get_font_metrics(INSTRUCTION_PARAM_LIST)
	{
	size_t font_hnd = 0;
	memcpy(&font_hnd, getVariableContent(cframe, 0), sizeof(size_t));
	TTF_Font *font = (TTF_Font*) font_hnd;

	unsigned char *cnt = ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content;

	int sdl_x;
	size_t x;
	size_t *xs = (size_t*) cnt;

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

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_get_font_name(INSTRUCTION_PARAM_LIST)
	{
	size_t font_hnd = 0;
	memcpy(&font_hnd, getVariableContent(cframe, 0), sizeof(size_t));
	TTF_Font *font = (TTF_Font*) font_hnd;

	//TODO: don't use any TTF_ functions outside of the main rendering loop?

	char *val = TTF_FontFaceFamilyName(font);

	if (val != NULL)
		{
		LiveArray *newArray = malloc(sizeof(LiveArray));
		memset(newArray, '\0', sizeof(LiveArray));

		newArray -> data = (unsigned char*) strdup(val);
		newArray -> length = strlen(val);

		newArray -> gtLink = charArrayGT;
		api -> incrementGTRefCount(newArray -> gtLink);
		newArray -> owner = cframe -> blocking -> instance;

		VVarLivePTR *ptrh = (VVarLivePTR*) ((LiveData*) ((VVarLivePTR*) getVariableContent(cframe, 1)) -> content) -> data;

		ptrh -> content = (unsigned char*) newArray;
		attachPointer(ptrh, &newArray -> scope.scopePointers);
		newArray -> refCount ++;
		ptrh -> typeLink = newArray -> gtLink -> typeLink;
    	}

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_is_font_fixed_width(INSTRUCTION_PARAM_LIST)
	{
	size_t font_hnd = 0;
	memcpy(&font_hnd, getVariableContent(cframe, 0), sizeof(size_t));
	TTF_Font *font = (TTF_Font*) font_hnd;

	//TODO: don't use any TTF_ functions outside of the main rendering loop?
	cframe -> localsData[((DanaType*) ((StructuredType*) cframe -> scopes[0].scope.etype) -> structure.content)[0].offset] = TTF_FontFaceIsFixedWidth(font) == 0 ? 0 : 1;

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_get_text_bitmap_with(INSTRUCTION_PARAM_LIST)
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

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_GENERATE_TEXT_BITMAP;
		newEvent.user.data1 = gb;

		SDL_PushEvent(&newEvent);

		return RETURN_DEFERRED;
		}

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_commit_buffer(INSTRUCTION_PARAM_LIST)
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

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_set_size(INSTRUCTION_PARAM_LIST)
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

		instance -> windowWidth = x;
		instance -> windowHeight = y;

		//now we assume that the rendering loop will at some point noticed that windowWidth or windowHeight don't match current
		// - and it will do the update there
		}

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_set_position(INSTRUCTION_PARAM_LIST)
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

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_set_visible(INSTRUCTION_PARAM_LIST)
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
		}

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_set_title(INSTRUCTION_PARAM_LIST)
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

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_maximise_window(INSTRUCTION_PARAM_LIST)
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

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_minimise_window(INSTRUCTION_PARAM_LIST)
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

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_get_maximised_screen_rect(INSTRUCTION_PARAM_LIST)
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

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_close_window(INSTRUCTION_PARAM_LIST)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_CLOSE_WINDOW;
		newEvent.user.data1 = instance;
		newEvent.user.data2 = cframe;

		SDL_PushEvent(&newEvent);
		}

	return RETURN_DEFERRED;
	}

INSTRUCTION_DEF op_register_click_listener(INSTRUCTION_PARAM_LIST)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		unsigned char *vc = getVariableContent(cframe, 1);
		instance -> clickListenerObject = ((VVarLivePTR*) vc) -> content;
		attachPointer(&instance -> clickListener, (VVar*) vc);
		instance -> clickListener.content = ((VVarLivePTR*) vc) -> content;
		}

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_register_mouse_listener(INSTRUCTION_PARAM_LIST)
	{
	size_t hnd = 0;
	memcpy(&hnd, getVariableContent(cframe, 0), sizeof(size_t));

	if (hnd != 0)
		{
		WindowInstance *instance = (WindowInstance*) hnd;

		unsigned char *vc = getVariableContent(cframe, 1);
		instance -> mouseListenerObject = ((VVarLivePTR*) vc) -> content;
		attachPointer(&instance -> mouseListener, (VVar*) vc);
		instance -> mouseListener.content = ((VVarLivePTR*) vc) -> content;
		}

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_set_background_colour(INSTRUCTION_PARAM_LIST)
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

	return RETURN_DIRECT;
	}

INSTRUCTION_DEF op_load_font(INSTRUCTION_PARAM_LIST)
	{
	LiveArray *array = (LiveArray*) ((VVarLivePTR*) getVariableContent(cframe, 0)) -> content;
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
		printf("open font error: could not find font '%s'\n", path);
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

		SDL_Event newEvent;
		SDL_zero(newEvent);
		newEvent.type = DX_LOAD_FONT;
		newEvent.user.data1 = lfd;

		SDL_PushEvent(&newEvent);

		return RETURN_DEFERRED;
		}

	free(path);

	return RETURN_DIRECT;
	}

/*
//NOTE: this function is sometimes useful to do a timer "quit" click, without actual user interaction
DWORD WINAPI sleep_thread(LPVOID ptr)
	{
	printf("sleeping...\n");
	Sleep(30000);
	printf("awake...\n");
	
	WindowInstance *myInstance = (WindowInstance*) instances -> data;
	
	size_t x = 184;
	size_t y = 26;
	size_t button = 1;
	
	copyHostInteger((unsigned char*) &myInstance -> clickX, (unsigned char*) &x, sizeof(size_t));
	copyHostInteger((unsigned char*) &myInstance -> clickY, (unsigned char*) &y, sizeof(size_t));
	copyHostInteger((unsigned char*) &myInstance -> clickButton, (unsigned char*) &button, sizeof(size_t));

	fillRegister(&myInstance -> clickParams[0], (unsigned char*) &myInstance -> clickX);
	fillRegister(&myInstance -> clickParams[1], (unsigned char*) &myInstance -> clickY);
	fillRegister(&myInstance -> clickParams[2], (unsigned char*) &myInstance -> clickButton);

	api -> callFunction(myInstance -> clickListenerObject, &myInstance -> clickListener, 3, &myInstance -> mouseClickExitStatus, myInstance -> clickParams, 3);
	
	return 0;
	}
*/

static void initRendering()
	{
	#ifdef WINDOWS
	graphicsStartupLock = CreateSemaphore(NULL, 0, 1, NULL);
	#endif
	#ifdef OSX
	dispatch_semaphore_t *sem;
	sem = &graphicsStartupLock;
    *sem = dispatch_semaphore_create(0);
	#else
	#ifdef LINUX
	sem_init(&graphicsStartupLock, 0, 0);
	#endif
	#endif

	//fire off the single unified rendering thread
	#ifdef WINDOWS
	HANDLE th = CreateThread(
            NULL,               // default security attributes
            0,                  // use default stack size
            render_thread,      // thread function name
            NULL,               // argument to thread function
            0,                  // use default creation flags
            NULL);              // returns the thread identifier

	CloseHandle(th);
	
	/*
	th = CreateThread(
            NULL,               // default security attributes
            0,                  // use default stack size
            sleep_thread,      // thread function name
            NULL,               // argument to thread function
            0,                  // use default creation flags
            NULL);              // returns the thread identifier

	CloseHandle(th);
	*/
	#else
	int err = 0;
	pthread_t th;
	memset(&th, '\0', sizeof(pthread_t));
	if ((err = pthread_create(&th, NULL, render_thread, NULL)) != 0){}
	#endif

	//wait for good init
	#ifdef WINDOWS
	WaitForSingleObject(graphicsStartupLock, INFINITE);
	#endif
	#ifdef OSX
	dispatch_semaphore_wait(graphicsStartupLock, DISPATCH_TIME_FOREVER);
	#else
	#ifdef LINUX
	sem_wait(&graphicsStartupLock);
	#endif
	#endif

	#ifdef STATS
	#ifdef WINDOWS
	th = CreateThread(
            NULL,               // default security attributes
            0,                  // use default stack size
            stats_thread,      // thread function name
            NULL,               // argument to thread function
            0,                  // use default creation flags
            NULL);              // returns the thread identifier

	CloseHandle(th);
	#else
	memset(&th, '\0', sizeof(pthread_t));

	if ((err = pthread_create(&th, NULL, stats_thread, NULL)) != 0){}
	#endif
	#endif
	}

Interface* load(CoreAPI *capi)
	{
	api = capi;
	
	// grab global type mappings for anything that we generate here
	charArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("char[]"));
	api -> incrementGTRefCount(charArrayGT);
	
	pixelArrayGT = api -> resolveGlobalTypeMapping(getTypeDefinition("Pixel[]"));
	api -> incrementGTRefCount(pixelArrayGT);
	
	integerGT = api -> resolveGlobalTypeMapping(&integerType);
	api -> incrementGTRefCount(integerGT);
	
	setInterfaceFunction("makeWindow", op_make_window);
	setInterfaceFunction("startPoly", op_start_poly);
	setInterfaceFunction("addPolyPoint", op_add_poly_point);
	setInterfaceFunction("endPoly", op_end_poly);
	setInterfaceFunction("addRect", op_add_rect);
	setInterfaceFunction("addLine", op_add_line);
	setInterfaceFunction("addPoint", op_add_point);
	setInterfaceFunction("addBitmap", op_add_bitmap);
	setInterfaceFunction("addText", op_add_text);
	setInterfaceFunction("addTextWith", op_add_text_with);
	setInterfaceFunction("pushSurface", op_push_surface);
	setInterfaceFunction("popSurface", op_pop_surface);
	setInterfaceFunction("setSize", op_set_size);
	setInterfaceFunction("setPosition", op_set_position);
	setInterfaceFunction("setVisible", op_set_visible);
	setInterfaceFunction("setTitle", op_set_title);
	setInterfaceFunction("commitBuffer", op_commit_buffer);
	setInterfaceFunction("registerClickListener", op_register_click_listener);
	setInterfaceFunction("registerMouseListener", op_register_mouse_listener);
	setInterfaceFunction("setBackgroundColor", op_set_background_colour);
	setInterfaceFunction("maximiseWindow", op_maximise_window);
	setInterfaceFunction("minimiseWindow", op_minimise_window);
	setInterfaceFunction("getMaximisedScreenRect", op_get_maximised_screen_rect);
	setInterfaceFunction("closeWindow", op_close_window);
	
	setInterfaceFunction("loadFont", op_load_font);
	setInterfaceFunction("getTextWidth", op_get_text_width_with);
	setInterfaceFunction("getFontMetrics", op_get_font_metrics);
	setInterfaceFunction("getFontName", op_get_font_name);
	setInterfaceFunction("isFontFixedWidth", op_is_font_fixed_width);
	setInterfaceFunction("getTextBitmapWith", op_get_text_bitmap_with);
	
	primeFontDirectories();

	initRendering();
	
	return getPublicInterface();
	}

void unload()
	{
	SDL_Event quit_event;
	quit_event.type=SDL_QUIT;
	SDL_PushEvent(&quit_event);

	//wait for all close locks...
	#ifdef WINDOWS
	WaitForSingleObject(graphicsShutdownLock, INFINITE);
	#endif
	#ifdef OSX
	dispatch_semaphore_wait(graphicsShutdownLock, DISPATCH_TIME_FOREVER);
	#else
	#ifdef LINUX
	sem_wait(&graphicsShutdownLock);
	#endif
	#endif

	SDL_Quit();

	api -> decrementGTRefCount(charArrayGT);
	api -> decrementGTRefCount(pixelArrayGT);
	}