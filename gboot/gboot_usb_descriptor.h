/**********************************************************************
 * gboot - gboot_usb_descriptor.h
 * A simple USB HID bootloader for silicon laboratories USB mcus
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

#ifndef  _USB_DESC_H_
#define  _USB_DESC_H_

#include "USB_types.h"
#include "gboot_usb_interface.h"

// GBOOT is incompatible with low speed becasue 
// it requires EP0_PACKET_SIZE to be 64 bytes
#define  EP0_PACKET_SIZE         0x40
#define  EP1_PACKET_SIZE         0x0040

//---------------------------------------------
// Configuration Descriptor for gboot
//---------------------------------------------
// From "USB Device Class Definition for Human Interface Devices (HID)".
// Section 7.1:
typedef struct {
   config_descriptor_t       hid_configuration_descriptor;
   interface_descriptor_t    hid_interface_descriptor;
   class_descriptor_t        hid_descriptor;
   endpoint_descriptor_t     hid_endpoint_in_descriptor;
}
config_descriptor_set_t;

#define HID_REPORT_DESCRIPTOR_SIZE (21)

//extern const SEG_CODE  unsigned char HID_REPORT_DESCRIPTOR_SIZE;
//typedef unsigned char hid_report_descriptor[HID_REPORT_DESCRIPTOR_SIZE];

extern __data unsigned char OUT_PACKET[CMD_PACKET_SIZE];
extern __data unsigned char IN_PACKET[CMD_PACKET_SIZE];

#endif  /* _USB_DESC_H_ */
