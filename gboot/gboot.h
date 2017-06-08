/**********************************************************************
 * gboot - gboot.h
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

#ifndef  _GBOOT_H_
#define  _GBOOT_H_

//-----------------------------------------------------------------------------
// User Configuration Options
//-----------------------------------------------------------------------------
#ifndef HIGH_SEG
#define HIGH_SEG    0x3C00U
#endif


// NOTE: these work only in the silabs IDE as they are overwritten by the scripts
// in the linux makefile
#define MCU_TYPE 0xF320
#define HW_PID   0xFFFF
#define HW_REV   0x10
#define PCB_REV  0xA0


//-----------------------------------------------------------------------------
// End of User Configuration Options
//-----------------------------------------------------------------------------

#include <C8051F320.h>
#include "gboot_usb_descriptor.h"

#define GBOOT_VERSION 0x0029
#ifdef  GBOOT_TINY
#define GBOOT_API 0x0
#else
#define GBOOT_API 0x1
#endif

#ifndef FLASH_START
#define FLASH_START 0x0400
#endif


#define FLASH_END (HIGH_SEG-1)


#define SYSCLK             24000000    // SYSCLK frequency in Hz

// CLKSEL SFR USB clock selections
#define USB_4X_CLOCK       0x00
#define USB_INT_OSC_DIV_2  0x10
#define USB_EXT_OSC        0x20
#define USB_EXT_OSC_DIV_2  0x30
#define USB_EXT_OSC_DIV_3  0x40
#define USB_EXT_OSC_DIV_4  0x50

// CLKSEL SFR System clock selections
#define SYS_INT_OSC        0x00
#define SYS_EXT_OSC        0x01
#define SYS_4X_DIV_2       0x02

#endif
