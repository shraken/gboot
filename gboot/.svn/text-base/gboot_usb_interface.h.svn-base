/**********************************************************************
 * gboot - gboot_usb_interface.h
 * A simple USB HID bootloader for silicon laboratories USB mcus
 *
 * Copyright (c) 2010, 2013 Gabriele Gorla
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **********************************************************************/

#ifndef  _GBOOT_USB_INTERFACE_H_
#define  _GBOOT_USB_INTERFACE_H_

// ----------------------------------------------------------------------------
// Device IDs
// ----------------------------------------------------------------------------

#define GBOOT_USB_VID 0x10C4
#define GBOOT_USB_PID 0x8621

// ----------------------------------------------------------------------------
// Report Sizes (in bytes)
// ----------------------------------------------------------------------------

#define CMD_PACKET_SIZE       0x04

#define CMD_REBOOT 0x0
#define CMD_ERASE  0x1
#define CMD_WRITE  0x2
#define CMD_READ   0x3

#define GBOOT_INFO_OFFSET 0x0006
#define FLASH_INFO_OFFSET 0x000e
#define DEV_INFO_OFFSET   0x0016
#define EXT_INFO_OFFSET   0x001e
#define DEV_SERIAL_OFFSET 0x0026
#define DEV_CONFIG_OFFSET 0x002e
#define DEV_RSVD1_OFFSET  0x0036
#define DEV_RSVD2_OFFSET  0x003e
#define DEV_RSVD3_OFFSET  0x0046
#define DEV_RSVD4_OFFSET  0x004e

// INFO packets layout:
// GBOOT
// unsigned char gboot major version bit 0-3, gboot api version bit 4-7
// unsigned char gboot minor version
// unsigned char silabs MCU
// unsigned char silabs MCU

// FLASH
// unsigned char code start page
// unsigned char last free flash page
// unsigned char flash erase size 2^n
// unsigned char reserved

// DEV
// unsigned char target device USB PID
// unsigned char target device USB PID
// unsigned char hardware revision
// unsigned char pcb revision

// EXT
// unsigned char reserved
// unsigned char reserved
// unsigned char reserved
// unsigned char reserved

// SERIAL
// 32-b BCD serial number

// CONFIG
// firmware specific PCB configuration

#endif
