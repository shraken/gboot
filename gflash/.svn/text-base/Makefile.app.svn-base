VPATH=..

CXXFLAGS=-Wall -DUSE_WX -I..

OBJECTS_CLI = gflash_cli.o

#OBJECTS_GUI = gflash_gui.o

GFLASH_LIB = gflash_lib.o

OBJECTS=$(OBJECTS_CLI) $(OBJECTS_GUI) $(GFLASH_LIB) gmod.o

LIBS+=$(GFLASH_LIB)
LIBS+=-lusb-1.0

ifeq ($(BUILD),WIN32)
AR=i586-mingw32msvc-ar
CC=i586-mingw32msvc-gcc
CXX=i586-mingw32msvc-g++
WXCONFIG=i586-msvc-wx-config 
BF=-win32
CFLAGS+=-I/usr/local/i586-mingw32msvc/include
CXXFLAGS+=-I/usr/local/i586-mingw32msvc/include -fno-rtti -fno-exceptions
LIBS+=-L/usr/local/i586-mingw32msvc/lib -lsetupapi -lole32 -ladvapi32 
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
#	$(CXX) -o $(APP)_gui $(OBJECTS_GUI) $(LIBS) $(WXLIBS)

clean:
	rm -f *.o $(NAME) core.*
