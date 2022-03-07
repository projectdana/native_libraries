CC = gcc

API_PATH = "dana_api_1.7"
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
ALL_RULES = calendar cmdln iofile iotcp ioudp dns sysinfo timer run math mysql_lib uiplane png jpg zlib clipboard ssl_lib sha_lib cipher_lib audio rwlock

ifeq ($(OS),Windows_NT)
    CCFLAGS += -DWINDOWS
	CCFLAGS += -DLIB_PLATFORM_NAME=\"win7\"
	CCFLAGS += -DMACHINE_ENDIAN_LITTLE
	INSTALL_PATH = "c:\programfiles\dana\"
	CP_CMD = copy
	PLATFORM = win7
	CCFLAGS += -shared
	NET_FLAGS = -lws2_32
	MYSQL_CONCPP_DIR= "C:/libs/MySQL Connector C 6.1"
	MYSQL_INCLUDE = -I $(MYSQL_CONCPP_DIR)/include -L $(MYSQL_CONCPP_DIR)/lib
	SQL_FLAGS = -lmysql
	PNG_FLAGS = -I "C:/libs/lpng/" "C:/libs/lpng/libpng.a" -L"C:/ProgramFiles/Dana/" -lzlib1
	JPG_FLAGS = -I "C:/libs/jpeg-9c" "C:/libs/jpeg-9c/libjpeg.a"
	ZLIB_FLAGS = -I "C:/libs/zlib" "C:/libs/zlib/libz.a"
	SSL_FLAGS = -I C:/libs/openssl/include C:/libs/openssl/libssl.a C:/libs/openssl/libcrypto.a -lws2_32 -lgdi32 -lADVAPI32 -luser32
    ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
        CCFLAGS += -DMACHINE_64
		CCFLAGS += -DLIB_CHIP_NAME=\"x64\"
		CHIP = x64
		SDL_FLAGS = -L"C:/libs/SDL/SDL2-2.0.8/x86_64-w64-mingw32/lib" -L"C:/libs/SDL/SDL2_ttf-2.0.14/x86_64-w64-mingw32/lib" -lSDL2main -lSDL2 -lSDL2_ttf -I "C:/libs/SDL/SDL2-2.0.8/i686-w64-mingw32/include/SDL2" -I "C:/libs/SDL/SDL2_ttf-2.0.14/i686-w64-mingw32/include/SDL2" -lmingw32 -mwindows -I . -I ../../compiler/ -static-libgcc"
		#SDL_FLAGS = -L"C:/libs/SDL/SDL2_ttf-2.0.14/x86_64-w64-mingw32/lib" C:/libs/SDL/SDL2-2.0.14/build/.libs/libSDL2main.a C:/libs/SDL/SDL2-2.0.14/build/.libs/libSDL2.a -lSDL2_ttf -lsetupapi -lole32 -lwinmm -limm32 -lversion -loleaut32 -I "C:/libs/SDL/SDL2-2.0.8/i686-w64-mingw32/include/SDL2" -I "C:/libs/SDL/SDL2_ttf-2.0.14/i686-w64-mingw32/include/SDL2" -lmingw32 -mwindows -I . -I ../../compiler/ -static-libgcc"
		AUDIO_FLAGS = -I "C:/libs/miniaudio"
    endif
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
        CCFLAGS += -DMACHINE_32
		CCFLAGS += -DLIB_CHIP_NAME=\"x86\"
		CHIP = x86
		SDL_FLAGS = -L"C:/libs/SDL2/build/build/.libs" -L"C:/libs/SDL/SDL2_ttf-2.0.14/i686-w64-mingw32/lib" -lSDL2main -lSDL2 -lSDL2_ttf -I "C:/libs/SDL2/include/" -I "C:/libs/SDL2_ttf/" -lmingw32 -mwindows -I . -I ../../compiler/ -static-libgcc
		AUDIO_FLAGS = -I "C:/libs/miniaudio"
    endif
else
    UNAME_S := $(shell uname -s)
	CCFLAGS += -ldl
	CCFLAGS += -DMACHINE_ENDIAN_LITTLE
	INSTALL_PATH = ~/dana/
	CP_CMD = cp
	PLATFORM = deb
	CCFLAGS += -shared -fPIC
	MATH_FLAGS = -lm
	#MYSQL_INCLUDE = -I/usr/include/mysql `mysql_config --variable=pkglibdir`/libmysqlclient.a -lpthread -lz -lm -lrt -ldl -lstdc++
	MYSQL_INCLUDE = -I ~/libs/mysql_lib/include ~/libs/mysql_lib/libmysqlclient.a -lpthread -lz -lm -lrt -ldl -lstdc++
    ifeq ($(UNAME_S),Linux)
        CCFLAGS += -DLINUX
		CCFLAGS += -DLIB_PLATFORM_NAME=\"deb\"
		PNG_FLAGS = -I "/usr/local/include/libpng16" "/usr/local/lib/libpng16.a" -lz -lm
		JPG_FLAGS = -I "~/libs/jpegsrc.v9c/jpeg-9c" "/usr/local/lib/libjpeg.a"
		ZLIB_FLAGS = "/usr/local/lib/libz.a"
		CLIPBOARD_FLAGS = -lX11
		SQL_FLAGS = ~/libs/openssl-1.1.1f/libssl.a ~/libs/openssl-1.1.1f/libcrypto.a
		SSL_FLAGS = -I ~/libs/openssl-1.1.1f/include ~/libs/openssl-1.1.1f/libssl.a ~/libs/openssl-1.1.1f/libcrypto.a
		AUDIO_FLAGS = -I ~/libs/miniaudio/ -lm -lpthread
    endif
    ifeq ($(UNAME_S),Darwin)
        CCFLAGS += -DOSX
        CCFLAGS += -DLINUX
        PLATFORM = osx
		CCFLAGS += -DLIB_PLATFORM_NAME=\"osx\"
        CCFLAGS += -DMACHINE_64
		SDL_FLAGS = /usr/local/lib/libSDL2.a -liconv -framework Cocoa -framework Carbon -framework IOKit -framework CoreAudio -framework CoreVideo -framework AudioToolbox -framework ForceFeedback -framework CoreHaptics -framework GameController -framework Metal /usr/local/lib/libSDL2_ttf.a -lfreetype -Wl,-rpath,'@executable_path/resources-ext' -I ~/Desktop/libs/
		CCFLAGS += -DLIB_CHIP_NAME=\"x64\"
		CHIP = x64
		MYSQL_INCLUDE = -I /usr/local/mysql-8.0.12-macos10.13-x86_64/include/
		SQL_FLAGS = /usr/local/lib/libcrypto.a /usr/local/lib/libssl.a /usr/local/mysql/lib/libmysqlclient.a -lpthread -lz -lm -ldl -lstdc++
		PNG_FLAGS = -I "/usr/local/include/libpng16" "/usr/local/lib/libpng16.a" -lz -lm
		JPG_FLAGS = -I "~/libs/jpegsrc.v9c/jpeg-9c" "/usr/local/lib/libjpeg.a"
		ZLIB_FLAGS = "/usr/local/lib/libz.a"
		CLIPBOARD_FLAGS = -framework ApplicationServices -x objective-c -ObjC -std=c99
		AUDIO_FLAGS = -I ~/Desktop/libs/miniaudio/ -lm -lpthread
		SSL_FLAGS = -I ~/Desktop/libs/openssl-1.1.1f/include ~/Desktop/libs/openssl-1.1.1f/libssl.a ~/Desktop/libs/openssl-1.1.1f/libcrypto.a
    endif
    ifneq ($(UNAME_S),Darwin)

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
	$(CC) -Os -s CalendarLib_dni.c $(API_PATH)/vmi_util.c CalendarLib.c -o CalendarLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) CalendarLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

cmdln:
	$(CC) -Os -s CmdLib_dni.c $(API_PATH)/vmi_util.c CmdLib.c -o CmdLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) CmdLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

iofile:
	$(CC) -Os -s FileLib_dni.c $(API_PATH)/vmi_util.c FileLib.c -o FileLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) FileLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

iotcp:
	$(CC) -Os -s TCPLib_dni.c $(API_PATH)/vmi_util.c TCPLib.c -o TCPLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(NET_FLAGS)
	$(CP_CMD) TCPLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

ioudp:
	$(CC) -Os -s UDPLib_dni.c $(API_PATH)/vmi_util.c UDPLib.c -o UDPLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(NET_FLAGS)
	$(CP_CMD) UDPLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

dns:
	$(CC) -Os -s DNSLib_dni.c $(API_PATH)/vmi_util.c DNSLib.c -o DNSLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(NET_FLAGS) -DCHIP_NAME=\"$(CHIP)\"
	$(CP_CMD) DNSLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

sysinfo:
	$(CC) -Os -s SystemLib_dni.c $(API_PATH)/vmi_util.c SystemLib.c -o SystemLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) -DCHIP_NAME=\"$(CHIP)\"
	$(CP_CMD) SystemLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

timer:
	$(CC) -Os -s Timer_dni.c $(API_PATH)/vmi_util.c Timer.c -o Timer[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) Timer[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

uiplane:
	$(CC) -Os -s UIPlaneLib_dni.c $(API_PATH)/vmi_util.c $(API_PATH)/platform_utils.c UIPlaneLib.c -o UIPlaneLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(SDL_FLAGS)
	$(CP_CMD) UIPlaneLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

run:
	$(CC) -Os -s RunLib_dni.c $(API_PATH)/vmi_util.c RunLib.c -o RunLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) RunLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

math:
	$(CC) -Os -s MathLib_dni.c $(API_PATH)/vmi_util.c MathLib.c -o MathLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(MATH_FLAGS)
	$(CP_CMD) MathLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

mysql_lib:
	$(CC) -Os -s MySQLLib_dni.c $(API_PATH)/vmi_util.c MySQLLib.c -o MySQLLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(MYSQL_INCLUDE) $(CCFLAGS) $(SQL_FLAGS)
	$(CP_CMD) MySQLLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

png:
	$(CC) -Os -s PNGLib_dni.c $(API_PATH)/vmi_util.c PNGLib.c -o PNGLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(PNG_FLAGS)
	$(CP_CMD) PNGLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

jpg:
	$(CC) -Os -s JPGLib_dni.c $(API_PATH)/vmi_util.c JPGLib.c -o JPGLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(JPG_FLAGS)
	$(CP_CMD) JPGLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

clipboard:
	$(CC) -Os -s Clipboard_dni.c $(API_PATH)/vmi_util.c Clipboard.c -o Clipboard[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(CLIPBOARD_FLAGS)
	$(CP_CMD) Clipboard[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

zlib:
	$(CC) -Os -s ZLib_dni.c $(API_PATH)/vmi_util.c ZLib.c -o ZLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(ZLIB_FLAGS)
	$(CP_CMD) ZLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

ssl_lib:
	$(CC) -Os -s SSLLib_dni.c $(API_PATH)/vmi_util.c SSLLib.c -o SSLLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(SSL_FLAGS)
	$(CP_CMD) SSLLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

sha_lib:
	$(CC) -Os -s SHALib_dni.c $(API_PATH)/vmi_util.c SHALib.c -o SHALib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(SSL_FLAGS)
	$(CP_CMD) SHALib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

cipher_lib:
	$(CC) -Os -s CipherLib_dni.c $(API_PATH)/vmi_util.c CipherLib.c -o CipherLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(SSL_FLAGS)
	$(CP_CMD) CipherLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

audio:
	$(CC) -Os -s AudioLib_dni.c $(API_PATH)/vmi_util.c AudioLib.c -o AudioLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS) $(AUDIO_FLAGS)
	$(CP_CMD) AudioLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

rwlock:
	$(CC) -Os -s RWLockLib_dni.c $(API_PATH)/vmi_util.c RWLockLib.c -o RWLockLib[$(PLATFORM).$(CHIP)].dnl $(STD_INCLUDE) $(CCFLAGS)
	$(CP_CMD) RWLockLib[$(PLATFORM).$(CHIP)].dnl "$(DANA_HOME)/resources-ext"

all: $(ALL_RULES)
