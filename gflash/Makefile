#gboot flasher app makefile

SOURCES=*.c 
#*.cpp
NAME=gflash


linux: $(SOURCES)
	mkdir -p linux
	( cd linux; $(MAKE) -f../Makefile.app APP=$(NAME))
	mv linux/$(NAME)_cli $(NAME)_cli
	mv linux/gmod gmod
#	mv linux/$(NAME)_gui $(NAME)_gui

linux_debug:
	mkdir -p linuxd
	( cd linuxd; $(MAKE) -f../Makefile.app APP=$(NAME) DEBUG=YES)
	mv linuxd/$(NAME) $(NAME)d

win:
	mkdir win32
	( cd win32; $(MAKE) -f../Makefile.app APP=$(NAME) BUILD=WIN32)
	mv win32/$(NAME)_cli $(NAME)_cli.exe
#	mv win32/$(NAME)_gui $(NAME)_gui.exe
	strip *.exe

dbg: linux_debug

all: linux win

clean:
	rm -rf *.a core.* \
	*.exe $(NAME)_gui $(NAME)_gui $(NAME)_cli $(NAME)_dump $(NAME)_fan \
	linux linuxd win32 \
	*~
