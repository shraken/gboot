# gboot USB HID bootloader

gboot is an 8051 USB HID bootloader designed for the Silicon Labs C8051F3xx family
of microcontrollers.  This repo is a fork of the original release distributed by
Gabriele Gorla of gglabs (http://gglabs.us).

The goal of this fork is to improve the build process to enable compilation
on multiple hosts (windows, mac, and linux).  Additionally, external binary
utilities and scripts required for binary to hex conversion and file
modification have been replaced by python scripts.  The last goal is to
provide some documentation on the internals of the bootloader and how
bootloadable binaries can be created with various toolchains.  

The original gboot bootloader was designed around F32x, F34x families but
my main focus is on support F38x and EFM8UB2 targets.  For flashing the
bootloader initially I use a J-Link adapter (EDU or PRO is fine, also 
J-Link OB the EFM8UB2 works).  

## Requirements

### gboot firmware:
sdcc 3.6 (http://sdcc.sourceforge.net/index.php#Download)
Segger J-Link Software (https://www.segger.com/downloads/jlink)
libusb 1.0.21 (https://sourceforge.net/projects/libusb/files/libusb-1.0/)

### gflash utility
python 2.6.x (https://www.python.org/)
IntelHex 2.1+ (https://pypi.python.org/pypi/IntelHex)
CMake 2.8.x+ (https://cmake.org/)

## Building gboot

Type `make FLASH_SIZE=xx VARIANT=vv`  in the `gboot` directory where
xx is 16, 32 or 64 based on the target device and vv is one of basic, 
dual_uart or F38x based on the chip used.

## Building gflash

To create the gflash binary:

`mkdir build`

`cd build`

`cmake ..`

`make`

## Flashing

Type sudo make flash in the `gboot` directory after building.