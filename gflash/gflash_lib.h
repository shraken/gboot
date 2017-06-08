/**********************************************************************
 * gflash - gflash_lib.h
 * flash update utility for Silabs USB MCUs using the gboot bootloader
 *
 * Copyright (c) 2010 Gabriele Gorla
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

#ifndef GFLASH_LIB_H
#define GFLASH_LIB_H

#include <libusb-1.0/libusb.h>
#include "gboot/gboot_usb_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

  struct gboot_info {
    unsigned short gboot_version;
    unsigned short mcu;
    unsigned int flash_start;
    unsigned int flash_end;
    unsigned int flash_erase_size;
    unsigned int user_flash_size;
    unsigned short usb_pid;
    unsigned char hw_ver;
    unsigned char pcb_rev;
    unsigned int  ext_info;
    unsigned int  serial;
    unsigned int  dev_config;
    unsigned char api_ver;
  };

 
  struct libusb_device_handle *gboot_open(void);
  int gboot_close(struct libusb_device_handle *devh);
  int gboot_cmd_reboot(libusb_device_handle *devh);
  int gboot_cmd_write(libusb_device_handle *devh, unsigned int addr, unsigned char d);
  
  int gboot_cmd_erase(libusb_device_handle *devh, unsigned int addr);
  int gboot_cmd_read(libusb_device_handle *devh,
		     unsigned int addr,   unsigned char buf[CMD_PACKET_SIZE]);

  int gboot_get_info(libusb_device_handle *devh, struct gboot_info *info);

  int gboot_recv(libusb_device_handle *devh, unsigned char packet[CMD_PACKET_SIZE]);
  int gboot_send(libusb_device_handle *devh, unsigned char packet[CMD_PACKET_SIZE]);


#ifdef __cplusplus
}
#endif

#endif
