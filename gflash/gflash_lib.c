/**********************************************************************
 * gflash - gflash_lib.c
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

#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "libusb.h"

#include <gboot/gboot_usb_interface.h>
#include "gflash_lib.h"

#define VERSION "0.1.0"
#define VENDOR_ID GBOOT_USB_VID
#define PRODUCT_ID GBOOT_USB_PID

#define HID_GET_REPORT                0x01
#define HID_GET_IDLE                  0x02
#define HID_GET_PROTOCOL              0x03
#define HID_SET_REPORT                0x09
#define HID_SET_IDLE                  0x0A
#define HID_SET_PROTOCOL              0x0B
#define HID_REPORT_TYPE_INPUT         0x01
#define HID_REPORT_TYPE_OUTPUT        0x02
#define HID_REPORT_TYPE_FEATURE       0x03

#define CTRL_IN		(LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE)
#define CTRL_OUT	(LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE)

//#define PACKET_SIZE RFEAT_FAN_CFG_SIZE

//const static int INTERFACE=0;
#define ENDPOINT_INT_IN 0x81 /* endpoint 0x81 address for IN */
#define ENDPOINT_INT_OUT 0x01  /* endpoint 1 address for OUT */

const static int TIMEOUT=5000; /* timeout in ms */

/*const static int info_offset[] = {
  GBOOT_INFO_OFFSET,
  FLASH_INFO_OFFSET,
  DEV_INFO_OFFSET,
  EXT_INFO_OFFSET,
  DEV_SERIAL_OFFSET,
  DEV_FEATURE_OFFSET
  };*/

struct libusb_device_handle *gboot_open(void)
{
  libusb_device_handle *devh;
  int r;

  fprintf(stderr, "looking for %04x %04x\n",VENDOR_ID,PRODUCT_ID);

  devh = libusb_open_device_with_vid_pid(NULL, VENDOR_ID, PRODUCT_ID);

  if(devh==NULL) {
    fprintf(stderr, "cannot find device\n");
    return NULL;
  }

#ifndef WIN32
  r= libusb_kernel_driver_active(devh, 0);
  if(r) {
    r= libusb_detach_kernel_driver(devh, 0);
    if (r < 0) {
      fprintf(stderr, "cannot detach kernel driver %d\n", r);
      goto out;
    }
  }
#endif
  
  r = libusb_set_configuration(devh, 1);
  if (r < 0) {
    fprintf(stderr, "libusb_set_configuration error %d\n", r);
    goto out;
  }
  //  printf("Successfully set usb configuration 1\n");
  r = libusb_claim_interface(devh, 0);
  if (r < 0) {
    fprintf(stderr, "libusb_claim_interface error %d\n", r);
    goto out;
  }
  //  printf("Successfully claimed interface\n");
  
  return devh;
 out:
  libusb_close(devh);
  return NULL;
}


int gboot_close(struct libusb_device_handle *devh)
{
  if(devh) {
    libusb_release_interface(devh, 0);
    libusb_close(devh);
    return 0;
  }
  return -1;
}

int gboot_cmd_write(libusb_device_handle *devh,
			   unsigned int addr, unsigned char d)
{
  unsigned char buf[CMD_PACKET_SIZE];

  buf[0]=CMD_WRITE;
  buf[1]=addr&0xff;
  buf[2]=addr>>8;
  buf[3]=d;
  return gboot_send(devh,buf);
}

int gboot_cmd_erase(libusb_device_handle *devh,
			   unsigned int addr)
{
  unsigned char buf[CMD_PACKET_SIZE];

  buf[0]=CMD_ERASE;
  buf[1]=addr&0xff;
  buf[2]=addr>>8;
  buf[3]=0;
  return gboot_send(devh,buf);
}

int gboot_cmd_reboot(libusb_device_handle *devh)
{
  unsigned char buf[CMD_PACKET_SIZE];

  buf[0]=CMD_REBOOT;
  buf[1]=0;
  buf[2]=0;
  buf[3]=0;
  return gboot_send(devh,buf);
}


int gboot_cmd_read(libusb_device_handle *devh,
		   unsigned int addr,   unsigned char buf[CMD_PACKET_SIZE])
{
  int r;

  buf[0]=CMD_READ;
  buf[1]=addr&0xff;
  buf[2]=addr>>8;
  buf[3]=0;
  r = gboot_send(devh,buf);
  if(r==0 ) r = gboot_recv(devh,buf);
  return r;
}

int gboot_get_info(libusb_device_handle *devh, struct gboot_info *info)
{
  int r;
  unsigned char buf[CMD_PACKET_SIZE];

  r=gboot_cmd_read(devh, GBOOT_INFO_OFFSET, buf);
  if(r) return r;

  //  printf("GBOOT version: %x.%x\n",buf[1]&0x0f,buf[0]);
  info->gboot_version=((buf[1]&0x0f)<<8)|buf[0];
  info->api_ver=buf[1]>>4;
  info->mcu=(buf[3]<<8)|buf[2];

  r=gboot_cmd_read(devh, FLASH_INFO_OFFSET, buf);
  if(r) return r;
  //  print_data(0, buf, CMD_PACKET_SIZE);
  info->flash_start=buf[0]<<8;
  info->flash_end=(buf[1]<<8)+0xff;
  info->flash_erase_size=1<<(buf[2]&0xf);
  info->user_flash_size=(buf[1]-buf[0]+1)*256;

  r=gboot_cmd_read(devh, DEV_INFO_OFFSET, buf);
  if(r) return r;
  info->usb_pid=(buf[1]<<8)|buf[0];
  info->hw_ver=buf[2];

  if(info->gboot_version>=0x020) {
    info->pcb_rev=buf[3];
    gboot_cmd_read(devh, DEV_SERIAL_OFFSET, buf);
    info->serial=(buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|buf[0];
    //gboot_cmd_read(devh, DEV_FEATURE_OFFSET, buf);
    //info->dev_config=(buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|buf[0];
    gboot_cmd_read(devh, EXT_INFO_OFFSET, buf);
    info->ext_info=(buf[3]<<24)|(buf[2]<<16)|(buf[1]<<8)|buf[0];
  } else {
    info->pcb_rev=0;
    info->serial=0;
    info->dev_config=0;
    info->ext_info=0;
  }

  return 0;
  //  printf("\nMax code size: %5.1fk (0x%02x bytes)\n", user_flash_size/1024., user_flash_size);
}

/*int gboot_cmd_info(libusb_device_handle *devh, int info_idx,
		   unsigned char buf[CMD_PACKET_SIZE])
{
  if(info_idx<0||info_idx>5) return -1;
  return gboot_cmd_read(devh,info_offset[info_idx],buf);
  }*/



int gboot_send(libusb_device_handle *devh, unsigned char packet[CMD_PACKET_SIZE])
{
  int r;
  
  r = libusb_control_transfer(devh, CTRL_OUT, HID_SET_REPORT,
			      (HID_REPORT_TYPE_FEATURE<<8), 
			      0, packet, CMD_PACKET_SIZE, TIMEOUT);
  
  if (r < 0) {
    fprintf(stderr, "Control Out error %d\n", r);
    return r;
  }
  
  return 0;
}


int gboot_recv(libusb_device_handle *devh, unsigned char packet[CMD_PACKET_SIZE])
{
  int r;
  
  r = libusb_control_transfer(devh, CTRL_IN, HID_GET_REPORT,
			      (HID_REPORT_TYPE_FEATURE<<8), 
			      0, packet, CMD_PACKET_SIZE, TIMEOUT);
  
  if (r < 0) {
    fprintf(stderr, "Control IN error %d\n", r);
    return r;
  }
  
  return 0;
}

