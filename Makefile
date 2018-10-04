CC = gcc

API_PATH = "dana_api_1.4"
STD_INCLUDE = -I $(API_PATH)
CCFLAGS = -Wall -fno-strict-aliasing
OUTPUT_FILE=

INSTALL_PATH=
CP_CMD=
CHIP=
PLATFORM=
SDL_FLAGS=
NET_FLAGS=
MATH_FLAGS=
SQL_FLAGS=
ALL_RULES = calendar cmdln iofile iotcp ioudp dns sysinfo timer run math mysql_lib

ifeq ($(OS),Windows_NT)
    CCFLAGS += -DWINDOWS
	CCFLAGS += -DPC
	CCFLAGS += -DLIB_PLATFORM_NAME=\"win7\"
	CCFLAGS += -DMACHINE_ENDIAN_LITTLE
	OUTPUT_FILE = dana.exe
	PRECOMPILE = windres cmdres.rc cmdres.o
	EXTRA_O_FILES = cmdres.o
	INSTALL_PATH = "c:\programfiles\dana\"
	CP_CMD = copy
	PLATFORM = win7
	CCFLAGS += -shared
	NET_FLAGS = -lws2_32
	ALL_RULES += uiplane
	MYSQL_CONCPP_DIR= "C:/libs/MySQL Connector C 6.1"
	MYSQL_INCLUDE = -I $(MYSQL_CONCPP_DIR)/include -L $(MYSQL_CONCPP_DIR)/lib
	SQL_FLAGS = -lmysql
	PNG_FLAGS = -I "C:/libs/lpng1635/" "C:/libs/lpng1635/libpng.a" -L"C:/ProgramFiles/Dana/" -lzlib1
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        CCFLAGS += -DMACHINE_64
		CCFLAGS += -DLIB_CHIP_NAME=\"x64\"
		CHIP = x64
#		SDL_FLAGS = -L"C:/libs/SDL/SDL2-2.0.8/x86_64-w64-mingw32/lib" "C:/libs/SDL/SDL2_ttf-2.0.14/x86_64-w64-mingw32/lib/libSDL2_ttf.a" -lSDL2main -lSDL2 -I "C:/libs/SDL/SDL2-2.0.8/i686-w64-mingw32/include/SDL2" -I "C:/libs/SDL/SDL2_ttf-2.0.14/i686-w64-mingw32/include/SDL2" -lmingw32 -mwindows -I . -I ../../compiler/ -lfreetype -static-libgcc"
		SDL_FLAGS = -L"C:/libs/SDL/SDL2-2.0.8/x86_64-w64-mingw32/lib" -L"C:/libs/SDL/SDL2_ttf-2.0.14/x86_64-w64-mingw32/lib" -lSDL2main -lSDL2 -lSDL2_ttf -I "C:/libs/SDL/SDL2-2.0.8/i686-w64-mingw32/include/SDL2" -I "C:/libs/SDL/SDL2_ttf-2.0.14/i686-w64-mingw32/include/SDL2" -lmingw32 -mwindows -I . -I ../../compiler/ -static-libgcc"
#		SDL_FLAGS = "C:/libs/SDL/SDL2-2.0.8/x86_64-w64-mingw32/lib/libSDL2.dll.a" "C:/libs/SDL/SDL2-2.0.8/x86_64-w64-mingw32/lib/libSDL2main.a" -L"C:/libs/SDL/SDL2_ttf-2.0.14/x86_64-w64-mingw32/lib" -lSDL2_ttf -I "C:/libs/SDL/SDL2-2.0.8/i686-w64-mingw32/include/SDL2" -I "C:/libs/SDL/SDL2_ttf-2.0.14/i686-w64-mingw32/include/SDL2" -lmingw32 -mwindows -I . -I ../../compiler/ -static-libgcc"
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        CCFLAGS += -DMACHINE_32
		CCFLAGS += -DLIB_CHIP_NAME=\"x86\"
		CHIP = x86
		SDL_FLAGS = -L"C:/libs/SDL/SDL2-2.0.8/i686-w64-mingw32/lib" -L"C:/libs/SDL/SDL2_ttf-2.0.14/i686-w64-mingw32/lib" -lSDL2main -lSDL2 -lSDL2_ttf -I "C:/libs/SDL/SDL2-2.0.8/i686-w64-mingw32/include/SDL2" -I "C:/libs/SDL/SDL2_ttf-2.0.14/i686-w64-mingw32/include/SDL2" -lmingw32 -mwindows -I . -I ../../compiler/ -static-libgcc
    endif
else
    UNAME_S := $(shell uname -s)
	OUTPUT_FILE = dana
	CCFLAGS += -ldl
	CCFLAGS += -DPC
	CCFLAGS += -DMACHINE_ENDIAN_LITTLE
	INSTALL_PATH = ~/dana/
	CP_CMD = cp
	PLATFORM = deb
	CCFLAGS += -shared -fPIC
	MATH_FLAGS = -lm
	MYSQL_INCLUDE = -I/usr/include/mysql `mysql_config --variable=pkglibdir`/libmysqlclient.a -lpthread -lz -lm -lrt -ldl -lstdc++
    ifeq ($(UNAME_S),Linux)
        CCFLAGS += -DLINUX
		CCFLAGS += -DLIB_PLATFORM_NAME=\"deb\"
		PNG_FLAGS = -I "/usr/local/include/libpng16" "/usr/local/lib/libpng16.a"
    endif
    ifeq ($(UNAME_S),Darwin)
        CCFLAGS += -DOSX
        CCFLAGS += -DLINUX
        PLATFORM = osx
		CCFLAGS += -DLIB_PLATFORM_NAME=\"osx\"
        CCFLAGS += -DMACHINE_64
		SDL_FLAGS = -framework SDL2 -framework SDL2_ttf
		CCFLAGS += -DLIB_CHIP_NAME=\"x64\"
		CHIP = x64
		MYSQL_INCLUDE = -I /usr/local/mysql-8.0.12-macos10.13-x86_64/include/
		SQL_FLAGS = -L/usr/local/mysql-8.0.12-macos10.13-x86_64/lib/ -lmysqlclient
    endif
    ifneq ($(UNAME_S),Darwin)
		ALL_RULES += uiplane
		
        UNAME_P := $(shell uname -p)
        ifeq ($(UNAME_P),x86_64)
            CCFLAGS += -DMACHINE_64
            CCFLAGS += -DLIB_CHIP_NAME=\"x64\"
            CHIP = x64
			SDL_FLAGS = /usr/local/lib/libSDL2main.a /usr/local/lib/libSDL2.a /usr/local/lib/libSDL2_ttf.a -lm -lfreetype
        endif
        ifneq ($(filter %86,$(UNAME_P)),)
            CCFLAGS += -DMACHINE_32
            CCFLAGS += -DLIB_CHIP_NAME=\"x86\"
            CHIP = x86
			SDL_FLAGS = /usr/local/lib/libSDL2main.a /usr/local/lib/libSDL2.a /usr/local/lib/libSDL2_ttf.a -lm -lfreetype
        endif
        ifneq ($(filter arm%,$(UNAME_P)),)
            CCFLAGS += -DARM
        endif
        UNAME_N := $(shell uname -n)
        ifneq ($(filter %raspberrypi,$(UNAME_N)),)
            CCFLAGS += -DMACHINE_32
            CCFLAGS += -DLIB_CHIP_NAME=\"armv6\"
            CHIP = armv6
			SDL_FLAGS = /usr/local/lib/libSDL2main.a /usr/local/lib/libSDL2.a /usr/local/lib/libSDL2_ttf.a -lm -lfreetype -L/opt/vc/lib -lbcm_host
        endif
    endif
endif

calendar:
	$(CC) -Os -s CalendarLib_dni.c vmi_util.c CalendarLib.c -o CalendarLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) CalendarLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

cmdln:
	$(CC) -Os -s CmdLib_dni.c vmi_util.c CmdLib.c -o CmdLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) CmdLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

iofile:
	$(CC) -Os -s FileLib_dni.c vmi_util.c FileLib.c -o FileLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) FileLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

iotcp:
	$(CC) -Os -s TCPLib_dni.c vmi_util.c TCPLib.c -o TCPLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(NET_FLAGS)
	$(CP_CMD) TCPLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

ioudp:
	$(CC) -Os -s UDPLib_dni.c vmi_util.c UDPLib.c -o UDPLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(NET_FLAGS)
	$(CP_CMD) UDPLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

dns:
	$(CC) -Os -s DNSLib_dni.c vmi_util.c DNSLib.c -o DNSLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(NET_FLAGS) -DCHIP_NAME=\"$(CHIP)\"
	$(CP_CMD) DNSLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

sysinfo:
	$(CC) -Os -s SystemLib_dni.c vmi_util.c SystemLib.c -o SystemLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) -DCHIP_NAME=\"$(CHIP)\"
	$(CP_CMD) SystemLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

timer:
	$(CC) -Os -s Timer_dni.c vmi_util.c Timer.c -o Timer[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) Timer[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

uiplane:
	$(CC) -Os -s UIPlaneLib_dni.c vmi_util.c $(API_PATH)/platform_utils.c UIPlaneLib.c -o UIPlaneLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(SDL_FLAGS)
	$(CP_CMD) UIPlaneLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

run:
	$(CC) -Os -s RunLib_dni.c vmi_util.c RunLib.c -o RunLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) RunLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

math:
	$(CC) -Os -s MathLib_dni.c vmi_util.c alu.c int_util.c MathLib.c -o MathLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(MATH_FLAGS)
	$(CP_CMD) MathLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

mysql_lib:
	$(CC) -Os -s MySQLLib_dni.c vmi_util.c MySQLLib.c -o MySQLLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(MYSQL_INCLUDE) $(CCFLAGS) $(SQL_FLAGS)
	$(CP_CMD) MySQLLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

clipboard:
	$(CC) -Os -s Clipboard_dni.c vmi_util.c Clipboard.c -o Clipboard[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) Clipboard[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

png:
	$(CC) -Os -s PNGLib_dni.c vmi_util.c PNGLib.c -o PNGLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(PNG_FLAGS)
	$(CP_CMD) PNGLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

all: $(ALL_RULES)
