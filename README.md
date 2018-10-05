[![Build Status](https://travis-ci.org/shraken/usbhid_buddy.svg?branch=master)](https://travis-ci.org/shraken/usbhid_buddy)

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

## Vagrant

A vagrant box is provided for simplified virtual machine building and provisioning.  Vagrant uses a 
VirtualBox virtual machine running running x64 trusty Ubuntu image.  The vagrant box that is created contains the required tools and dependencies for building both `gboot` and `gflash`
projects.  Additionally, the vagrant box provides USB pass through and mounts the required JLink and
gboot USB devices. 

### Requirements

1. [VirtualBox](https://www.virtualbox.org/wiki/Downloads)
2. [VirtualBox Extension Pack](http://download.virtualbox.org/virtualbox/5.2.2/Oracle_VM_VirtualBox_Extension_Pack-5.2.2-119230.vbox-extpack)
3. [Vagrant](https://www.vagrantup.com/downloads.html)
4. [SEGGER JLink Linux x64](https://www.segger.com/downloads/jlink)

The VirtualBox Extension Pack is required for USB 2.0/3.0 devices.  Close the VirtualBox application before executing the below commands.  On Windows systems, just open the *.vbox-extpack file.

```shell
wget http://download.virtualbox.org/virtualbox/5.2.2/Oracle_VM_VirtualBox_Extension_Pack-5.2.2-119230.vbox-extpack
open -W Oracle_VM_VirtualBox_Extension_Pack-5.2.2-119230.vbox-extpack
rm Oracle_VM_VirtualBox_Extension_Pack-5.2.2-119230.vbox-extpack
```

*NOTE: You must fill out the info form and download the latest JLink Linux x64 software package from SEGGER with the above link to flash the gboot bootloader project for the first time.  Alternatively, you can copy the hex file to a native OS and program using traditional SiLabs 8-bit USB debug adapters.

### Build and Flashing It

Attach the SEGGER JLink adapter to the host system before running the below commands.  

Build the vagrant image.
```
vagrant up
vagrant ssh
```

This will drop you in a terminal on the Ubuntu virtual machine from which you can build and flash. 

To build and flash the `gboot` firmware for the F380 type with 64 kB ROM size execute the following.  See [gboot/ReadMe.txt](gboot/ReadMe.txt) for different build flags.
```
cd /vagrant/gboot
make FLASH_SIZE=64 VARIANT=F38x
```

To build and flash the `gflash` utility execute the following.
```
cd /vagrant/gflash
mkdir build
cd build
cmake ..
make
```

### Using It

Once `gboot` has been flashed, you can 

## Traditional

The traditional route is provided below for building on a native OS.  The vagrant route is the recommended approach.

### Requirements

#### gboot firmware:
1. [sdcc 3.6](http://sdcc.sourceforge.net/index.php#Download)
2. [Segger J-Link Software](https://www.segger.com/downloads/jlink)
3. [libusb 1.0.21](https://sourceforge.net/projects/libusb/files/libusb-1.0/)

#### gflash utility
1. [python 2.6.x ](https://www.python.org/)
2. [IntelHex 2.1+](https://pypi.python.org/pypi/IntelHex)
3. [CMake 2.8.x+](https://cmake.org/)

### Building gboot

Type `make FLASH_SIZE=xx VARIANT=vv`  in the `gboot` directory where
xx is 16, 32 or 64 based on the target device and vv is one of basic, 
dual_uart or F38x based on the chip used.

### Building gflash

To create the gflash binary:

```
mkdir build
cd build
cmake ..
make
```

### Flashing

Type sudo make flash in the `gboot` directory after building.