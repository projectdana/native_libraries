CC = gcc

API_PATH = "dana_api_1.7"
STD_INCLUDE = -I $(API_PATH)
CCFLAGS = -Wall -fno-strict-aliasing
OUTPUT_FILE=

INSTALL_PATH=
CP_CMD=
CHIP=
PLATFORM=
ALL_RULES = calendar cmdln iofile iotcp ioudp dns sysinfo timer run math mysql_lib uiplane png jpg zlib clipboard ssl_lib sha_lib cipher_lib audio rwlock

ifeq ($(OS),Windows_NT)
    CCFLAGS += -DWINDOWS
	CCFLAGS += -DLIB_PLATFORM_NAME=\"win\"
	CCFLAGS += -DMACHINE_ENDIAN_LITTLE
	INSTALL_PATH = "c:\programfiles\dana\"
	CP_CMD = copy
	PLATFORM = win
	CCFLAGS += -shared
	NET_LIBS = -lws2_32
	MYSQL_CONCPP_DIR= "C:/libs/MySQL Connector C 6.1"
	MYSQL_INCLUDE = -I $(MYSQL_CONCPP_DIR)/include -L $(MYSQL_CONCPP_DIR)/lib
	MYSQL_LIBS = -lmysql
	PNG_INCLUDE = -I "C:/libs/lpng/"
	PNG_LIBS = "C:/libs/lpng/libpng.a" -L"C:/ProgramFiles/Dana/" $(ZLIB_LIBS)
	JPG_INCLUDE = -I "C:/libs/jpeg-9c"
	JPG_LIBS = "C:/libs/jpeg-9c/libjpeg.a"
	ZLIB_INCLUDE = -I "C:/libs/zlib"
	ZLIB_LIBS = "C:/libs/zlib/libz.a"
	SSL_INCLUDE = -I C:/libs/openssl-1.1.1/include
	SSL_LIBS = C:/libs/openssl-1.1.1/libssl.a C:/libs/openssl-1.1.1/libcrypto.a -lws2_32 -lgdi32 -lADVAPI32 -luser32
	AUDIO_INCLUDE = -I "C:/libs/miniaudio"
	AUDIO_LIBS = -lm -lpthread
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        CCFLAGS += -DMACHINE_64
		CCFLAGS += -DLIB_CHIP_NAME=\"x64\"
		CHIP = x64
		SDL_INCLUDE = -I "C:/libs/SDL/SDL-release-2.24.1/include" -I C:/libs/SDL/SDL2_ttf-2.20.1 -I C:/libs/SDL2_gfx
		SDL_LIBS = C:/msys64/mingw64/lib/libSDL2main.a C:/msys64/mingw64/lib/libSDL2.a C:/msys64/mingw64/lib/libSDL2_ttf.a C:/libs/SDL2_gfx/.libs/libSDL2_gfx.a -lmingw32 -lImm32 -lVersion -lwinmm -lgdi32 -lADVAPI32 -luser32 -lole32 -loleaut32 -lshell32 -lsetupapi -lrpcrt4 -mwindows -static-libgcc"
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        CCFLAGS += -DMACHINE_32
		CCFLAGS += -DLIB_CHIP_NAME=\"x86\"
		CHIP = x86
		SDL_INCLUDE = -I "C:/libs/SDL/SDL-release-2.24.1/include" -I C:/libs/SDL/SDL2_ttf-2.20.1 -I C:/libs/SDL2_gfx
		SDL_LIBS = C:/msys32/mingw32/lib/libSDL2main.a C:/msys32/mingw32/lib/libSDL2.a C:/msys32/mingw32/lib/libSDL2_ttf.a C:/libs/SDL2_gfx/.libs/libSDL2_gfx.a -lmingw32 -lImm32 -lVersion -lwinmm -lADVAPI32 -luser32 -lole32 -loleaut32 -lshell32 -lsetupapi -lrpcrt4 -lUsp10 -lgdi32 -mwindows -static-libgcc"
    endif
else
    UNAME_S := $(shell uname -s)
	CCFLAGS += -ldl
	CCFLAGS += -DMACHINE_ENDIAN_LITTLE
	INSTALL_PATH = ~/dana/
	CP_CMD = cp
	CCFLAGS += -shared -fPIC
	MATH_LIBS = -lm
	SSL_INCLUDE = -I ~/libs/openssl-1.1.1/include
	SSL_LIBS = ~/libs/openssl-1.1.1/libssl.a ~/libs/openssl-1.1.1/libcrypto.a
	MYSQL_INCLUDE = -I ~/libs/mysql_lib/include
	MYSQL_LIBS = ~/libs/mysql_lib/libmysqlclient.a -lpthread -lz -lm -lrt -ldl -lstdc++ $(SSL_LIBS)
	AUDIO_INCLUDE = -I ~/libs/miniaudio/
	AUDIO_LIBS = -lm -lpthread
	ZLIB_LIBS = ~/libs/zlib_lib/libz.a
	JPG_INCLUDE = -I ~/libs/jpeg_lib/include
	JPG_LIBS = ~/libs/jpeg_lib/libjpeg.a
	PNG_INCLUDE = -I ~/libs/png_lib/include/libpng16
	PNG_LIBS = ~/libs/png_lib/libpng.a -lz -lm
    ifeq ($(UNAME_S),Linux)
        CCFLAGS += -DLINUX
		CCFLAGS += -DLIB_PLATFORM_NAME=\"deb\"
		PLATFORM = deb
		CLIPBOARD_LIBS = -lX11
		SDL_LIBS = /usr/local/lib/libSDL2main.a /usr/local/lib/libSDL2.a /usr/local/lib/libSDL2_ttf.a ~/libs/SDL2_gfx/.libs/libSDL2_gfx.a -lm -lfreetype
		SDL_INCLUDE = -I ~/libs/SDL2_gfx -I /usr/local/include/SDL2/
    endif
    ifeq ($(UNAME_S),Darwin)
        CCFLAGS += -DOSX
        CCFLAGS += -DLINUX
        PLATFORM = osx
		CCFLAGS += -DLIB_PLATFORM_NAME=\"osx\"
        CCFLAGS += -DMACHINE_64
		CCFLAGS += -DLIB_CHIP_NAME=\"x64\"
		CHIP = x64
		SDL_LIBS = /usr/local/lib/libSDL2.a -liconv -framework Cocoa -framework Carbon -framework IOKit -framework CoreAudio -framework CoreVideo -framework AudioToolbox -framework ForceFeedback -framework CoreHaptics -framework GameController -framework Metal /usr/local/lib/libSDL2_ttf.a ~/libs/SDL2_gfx/.libs/libSDL2_gfx.a -lfreetype -Wl,-rpath,'@executable_path/resources-ext'
		SDL_INCLUDE = -I ~/libs/ -I ~/libs/SDL2_gfx -I ~/libs/SDL2/
		MYSQL_INCLUDE = -I /usr/local/mysql-8.0.12-macos10.13-x86_64/include/
		MYSQL_LIBS = /usr/local/lib/libcrypto.a /usr/local/lib/libssl.a /usr/local/mysql/lib/libmysqlclient.a -lpthread -lz -lm -ldl -lstdc++
		CLIPBOARD_LIBS = -framework ApplicationServices -x objective-c -ObjC -std=c99
    endif
    ifneq ($(UNAME_S),Darwin)
        UNAME_P := $(shell uname -p)
        ifeq ($(UNAME_P),x86_64)
            CCFLAGS += -DMACHINE_64
            CCFLAGS += -DLIB_CHIP_NAME=\"x64\"
            CHIP = x64
        endif
        ifneq ($(filter %86,$(UNAME_P)),)
            CCFLAGS += -DMACHINE_32
            CCFLAGS += -DLIB_CHIP_NAME=\"x86\"
            CHIP = x86
        endif
        ifneq ($(filter arm%,$(UNAME_P)),)
            CCFLAGS += -DARM
        endif
        UNAME_N := $(shell uname -n)
        ifneq ($(filter %raspberrypi,$(UNAME_N)),)
            CCFLAGS += -DMACHINE_32
            CCFLAGS += -DLIB_CHIP_NAME=\"armv6\"
            CHIP = armv6
			SDL_LIBS = /usr/local/lib/libSDL2main.a /usr/local/lib/libSDL2.a /usr/local/lib/libSDL2_ttf.a ~/libs/SDL2_gfx/.libs/libSDL2_gfx.a -lm -lfreetype -L/opt/vc/lib -lbcm_host
        endif
    endif
endif

calendar:
	$(CC) -Os -s CalendarLib_dni.c $(API_PATH)/vmi_util.c CalendarLib.c -o CalendarLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) -lm
	$(CP_CMD) CalendarLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

cmdln:
	$(CC) -Os -s CmdLib_dni.c $(API_PATH)/vmi_util.c CmdLib.c -o CmdLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) CmdLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

iofile:
	$(CC) -Os -s FileLib_dni.c $(API_PATH)/vmi_util.c FileLib.c -o FileLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) FileLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

iotcp:
	$(CC) -Os -s TCPLib_dni.c $(API_PATH)/vmi_util.c TCPLib.c -o TCPLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(NET_LIBS)
	$(CP_CMD) TCPLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

ioudp:
	$(CC) -Os -s UDPLib_dni.c $(API_PATH)/vmi_util.c UDPLib.c -o UDPLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(NET_LIBS)
	$(CP_CMD) UDPLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

dns:
	$(CC) -Os -s DNSLib_dni.c $(API_PATH)/vmi_util.c DNSLib.c -o DNSLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(NET_LIBS) -DCHIP_NAME=\"$(CHIP)\"
	$(CP_CMD) DNSLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

sysinfo:
	$(CC) -Os -s SystemLib_dni.c $(API_PATH)/vmi_util.c SystemLib.c -o SystemLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) -DCHIP_NAME=\"$(CHIP)\"
	$(CP_CMD) SystemLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

timer:
	$(CC) -Os -s TimerLib_dni.c $(API_PATH)/vmi_util.c Timer.c -o TimerLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) TimerLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext"

uiplane:
	$(CC) -Os -s UIPlaneLib_dni.c $(API_PATH)/vmi_util.c $(API_PATH)/platform_utils.c UIPlaneLib.c -o UIPlaneLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(SDL_INCLUDE) $(SDL_LIBS)
	$(CP_CMD) UIPlaneLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

run:
	$(CC) -Os -s RunLib_dni.c $(API_PATH)/vmi_util.c RunLib.c -o RunLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) RunLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

math:
	$(CC) -Os -s MathLib_dni.c $(API_PATH)/vmi_util.c MathLib.c -o MathLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(MATH_LIBS)
	$(CP_CMD) MathLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

mysql_lib:
	$(CC) -Os -s MySQLLib_dni.c $(API_PATH)/vmi_util.c MySQLLib.c -o MySQLLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(MYSQL_INCLUDE) $(CCFLAGS) $(MYSQL_LIBS)
	$(CP_CMD) MySQLLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

png:
	$(CC) -Os -s PNGLib_dni.c $(API_PATH)/vmi_util.c PNGLib.c -o PNGLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(PNG_INCLUDE) $(PNG_LIBS)
	$(CP_CMD) PNGLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

jpg:
	$(CC) -Os -s JPGLib_dni.c $(API_PATH)/vmi_util.c JPGLib.c -o JPGLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(JPG_INCLUDE) $(JPG_LIBS)
	$(CP_CMD) JPGLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

clipboard:
	$(CC) -Os -s ClipboardLib_dni.c $(API_PATH)/vmi_util.c Clipboard.c -o ClipboardLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(CLIPBOARD_LIBS)
	$(CP_CMD) ClipboardLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

zlib:
	$(CC) -Os -s ZLib_dni.c $(API_PATH)/vmi_util.c ZLib.c -o ZLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(ZLIB_LIBS)
	$(CP_CMD) ZLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

ssl_lib:
	$(CC) -Os -s SSLLib_dni.c $(API_PATH)/vmi_util.c SSLLib.c -o SSLLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(SSL_INCLUDE) $(CCFLAGS) $(SSL_LIBS)
	$(CP_CMD) SSLLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

sha_lib:
	$(CC) -Os -s SHALib_dni.c $(API_PATH)/vmi_util.c SHALib.c -o SHALib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(SSL_INCLUDE) $(CCFLAGS) $(SSL_LIBS)
	$(CP_CMD) SHALib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

cipher_lib:
	$(CC) -Os -s CipherLib_dni.c $(API_PATH)/vmi_util.c CipherLib.c -o CipherLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(SSL_INCLUDE) $(CCFLAGS) $(SSL_LIBS)
	$(CP_CMD) CipherLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

audio:
	$(CC) -Os -s AudioLib_dni.c $(API_PATH)/vmi_util.c AudioLib.c -o AudioLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(AUDIO_INCLUDE) $(AUDIO_LIBS)
	$(CP_CMD) AudioLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

rwlock:
	$(CC) -Os -s RWLockLib_dni.c $(API_PATH)/vmi_util.c RWLockLib.c -o RWLockLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) RWLockLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/components/resources-ext/"

all: $(ALL_RULES)
