-------------------------------------------------------------------------------
 gboot - Readme.txt
-------------------------------------------------------------------------------
Copyright (c) 2010, 2013 Gabriele Gorla
gorlik@yahoo.com

gboot is a very simple USB HID bootloader for Silicon Laboratories USB 
microcontrollers. It has been developed for C8051F320 and tested on
F32x and F34x. It should work on the F38x family as well but it's untested as I
do not currently have the hardware.

There are 2 version of the bootloader depending on the features and the space 
available. Both builds fit in less than 1.5k (3 flash pages).
The tiny build for the F38x is currently 1064 bytes (when compiled with SDCC 3.3)
and it is very likely it will fit in just 2 flash pages in the future.

Acknowledgements
------------------
This work was inspired by several threads in the silicon laboratories MCU 
forums. Comments from Tsuneo were especially insightful for the practical 
implementation. The development of this software could not have been possible 
without SDCC and the availability of several other GNU tools. 


Features
------------------
Tiny build version:
- return basic information about the device and flash layout
- erase a flash page
- write to flash
- read flash
- prevent write/erase of the bootloader code
- detect HW button for firmware recovery
- reboot the MCU

The standard build as the following additional features:
- complete usb error reporting (the tiny build does almost no checks 
  on the correctnes of the USB requests)
- more advanced bootloader entry detection
- USB stack can return extended (non static) information
- can support more USB strings
- easy to extend (very limited use of acall)

System requirements
----------------------------
silicon laboratories F32x, F34x or F38x USB MCU
linux or windows computer
sdcc (3.3 recommended, 3.1 and 3.2 should work, 3.0 has issues with inline functions)
GNU make & sercord (on linux) **or**
Silicon Laboratories IDE (on windows)

Configuring gboot
----------------------------
The distribution defaults to 16k flash and one UART (F320) with the 
ID set to 0xFFFF, version to 0x10 nad PCB to 0xA0. There are a few simple 
steps to configure gboot based on the device and application.

1) Set the chip variant (required on windows only)
open the Project->Tool Chain Integration settings in the Silabs IDE
open the Assembler tab and modify the "Command line flags" based on the target
device:

For F32x and F34x with just one UART
-plosgff -Ivariants/basic

For F34x with two UARTs
-plosgff -Ivariants/dual_uart

For all F38x
-plosgff -Ivariants/F38x


2) Set the flash size (required on windows only)
open the Project->Tool Chain Integration settings in the Silabs IDE
open the Linker tab and modify the "Command line options" based on the amount 
of flash of the target device:

for 16k:
--debug --use-stdout -V -Wl -bGBOOT_CONST=0x3c00 -Wl -bUSBSEG=0x0000 -Wl -bMAINSEG=0x3c00

for 32k:
--debug --use-stdout -V -Wl -bGBOOT_CONST=0x7e00 -Wl -bUSBSEG=0x0000 -Wl -bMAINSEG=0x7e00

for 64k:
--debug --use-stdout -V -Wl -bGBOOT_CONST=0xfa00 -Wl -bUSBSEG=0x0000 -Wl -bMAINSEG=0xfa00


open gboot.h and edit the HIGH SEG define based on the flash size

for 16k:
#define HIGH_SEG    0x3C00U

for 32k:
#define HIGH_SEG    0x7E00U

for 64k:
#define HIGH_SEG    0xFA00U

3) Set MCU, USB ID and versions (optional on both linux and windows)

3A) linux only - open the makefile and edit
MCU=0xF320
PROD_PID=0xFFFF
PROD_REV=0x10
PROD_PCB=0xA0

3B) windows only - openg gboot.h and edit the following lines:

#define MCU     0xF320
#define HW_PID  0xFFFF
#define HW_REV  0x10
#define PCB_REV 0xA0

These are not strictly required, but when used properly can prevent 
flashing an incorrect firmware to the device.
This data is preserved after a flash erase and can be queried using 
bootloader commands.
If using gflash the 'i' command will show this data.


Building gboot
----------------------------
to build in linux just type "make FLASH_SIZE=xx VARIANT=vv" where xx 
is 16, 32 or 64 based on the target device and vv is one of basic, 
dual_uart or F38x based on the chip used.
All F32x are basic, F34x are either basic or dual_uart depending if
the support UART1. All F38x are F38x.

For the standard build add "TINY_BUILD=n" to the make command.
Alternatively type "make release" to build both builds for all MCU 
combinations (16k, 32k and 64k).
Scripts to do simple changes to the binary (i.e. change serial) are 
included in the "scripts" directory.

In windows open the included project file using the silabs ide and build the 
project.
Due to limitation in the linker scripts currently the silabs ide can only 
build the standard version.
A precompiled version of both builds for all the flash sizes is included in 
the bin directory of the gboot distribution.

Flashing gboot
----------------------------
To flash the bootloader you must use Silicon Labs IDE or the flasher.
I was not able to get any of the free tools (ec2drv or ec2drv-new) 
to work with my Silabs toolstick nor the F340-TB.
It seems that the firmware of the toolstick is incompatible with the ec2drv.
If you find a solution/alternative please contact me at the email address
above.

Building a firmware to work with gboot
----------------------------------------
All is required is to add --code-loc 0x0400 to the link stage of your 
firmware. Convert the intel hex file to bin using srec_cat, hex2bin or your 
favorite tool (make sure the the code start at 0 in the bin file) and use 
the included flash utility to upload the code.

In the default configuration the bootloader will automatically execute the 
flashed firmware upon power on unless P3_0 is grounded or the reset vector 
is corrupted (location 0x0400 does not contain 0x02).
The standard build has a third way to enter the bootloader by writing 0xa5 
to the first location of xram and initiating a software reset (RSTSRC should be 0x10) 

Building the firmware update utilities
----------------------------------------
See instructions in the gflash archive


Flash Layout
--------------
The bootloader occupies the first two and the last page of flash 
(0x0000-0x03ff and 0x3c00-0x3dfe on a 16k device)

gboot must use the first page of flash for the interrupt redirection table.
the bootloadedr must use the last page of flash as on Silicon Laboratories 
MCUs it cannot be erased using firmware. Also the last byte of flash is 
unusable as it contains the flash lock byte.

Permanent Device Information
----------------------------
Small bits of data can be embedded in the bootloader to provide static
information about the product to the firmware (e.g. serial number, board 
configuration, firmware PID, etc...)

To keep the HID interface as simple as possible the information is retrieved
using the flash read command at specific offsets defined by the bootloader.
Each information packet is 4 bytes. All multi byte quantities are in little 
endian format. 

name        offset  description
-----------------------------------
GBOOT_INFO   0x06   U16 gboot_version, U16 silabs MCU variant
FLASH_INFO   0x0E   U8 flash start, U8 flash end, U8 flash page size, U8 unused
DEV_INFO     0x16   U16 device USB PID, U8 device HW version, U8 PCB revision
EXT_INFO     0x1E   U32 RFU
DEV_SERIAL   0x26   U32 serial number (either BCD or binary encoded)
DEV_CONFIG   0x2E   U32 device specific bitfield


TODO
-------
implement the following features:
- implement/document reverse firmware/bootloader interface
- investigate further bootloader code size reductions

