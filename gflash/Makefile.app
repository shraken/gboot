VPATH=..

CXXFLAGS=-Wall -DUSE_WX -I..

OBJECTS_CLI = gflash_cli.o

GFLASH_LIB = gflash_lib.o

OBJECTS=$(OBJECTS_CLI) $(OBJECTS_GUI) $(GFLASH_LIB) gmod.o

LIBS+=$(GFLASH_LIB)

ifeq ($(BUILD),WIN32)
AR=ar
CC=gcc
CXX=g++
BF=-win32
CFLAGS+=-I/usr/local/i586-mingw32msvc/include
CFLAGS+=-I../../
CFLAGS+=-I../../common/libusb-1.0.21/include/libusb-1.0

CXXFLAGS+=-I/usr/local/i586-mingw32msvc/include -fno-rtti -fno-exceptions
LIBS+=-L../../common/libusb-1.0.21/MinGW32/static
LIBS+=-LC:\MinGW\lib -lsetupapi -lole32 -ladvapi32 -l:libusb-1.0.a
else
CC=gcc -Wall
WXCONFIG=wx-config
endif

ifeq ($(DEBUG),YES)
        CXXFLAGS+= `$(WXCONFIG) --cxxflags --debug=yes` -ggdb
	WXLIBS= `$(WXCONFIG) --libs --debug=yes`
else
        CXXFLAGS+= `$(WXCONFIG) --cxxflags`
	WXLIBS= `$(WXCONFIG) --libs`
endif

$(APP): $(OBJECTS)
	$(CC) -o $(APP)_cli $(OBJECTS_CLI) $(LIBS)
	$(CC) -o gmod gmod.o

clean:
	rm -f *.o $(NAME) core.*
