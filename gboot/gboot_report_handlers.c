/**********************************************************************
 * gboot - gboot_report_handlers.c
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
#include "gboot.h"
#include "gboot_usb_interface.h"
#include "gboot_usb_core.h"

#pragma codeseg USBSEG
//#pragma codeseg MAINSEG



__bit write_enable()
{
  // prevent overwriting the bootloader
  if(OUT_PACKET[2]<(FLASH_START>>8)||OUT_PACKET[2]>(FLASH_END>>8))
    return 0;
  
  // unlock the flash
  FLKEY=0xA5;
  FLKEY=0xF1;
  return 1;
}


void ReportHandler(void)
{

  switch(OUT_PACKET[0]) {
  case CMD_WRITE:
    if((OUT_PACKET[3]!=0xff) && write_enable()) {
      __xdata unsigned char* __xdata taddr;
      taddr=(__xdata unsigned char *)(*((__data int *)&OUT_PACKET[1]));
      PSCTL|=0x01;
      *taddr=OUT_PACKET[3];
      // The cast below is required to generate smaller code
      PSCTL&=(unsigned char)~0x01;
    }
    break;
  case CMD_ERASE:
    if(write_enable()) {
  __xdata unsigned char* __xdata taddr;
  taddr=(__xdata unsigned char *)(*((__data int *)&OUT_PACKET[1]));
      PSCTL|=0x03;
      *taddr=0;
      // The cast below is required to generate smaller code
      PSCTL&=(unsigned char)~0x03;
    }
    break;
  case CMD_REBOOT:
    RSTSRC=0x10;
    //RSTSRC=0x04;
    //OSCICN=0;
    break;
  case CMD_READ:
   TXDATAPTR=(tx_dataptr_t)((__code unsigned char *)(*((__data int *)&OUT_PACKET[1])));
    break;
  default:
  }
}
