/**********************************************************************
 * gboot - gboot_usb_descriptor.c
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

//-----------------------------------------------------------------------------
// Device Config
//-----------------------------------------------------------------------------

#define USB_VER		0x0200	// USB specification revision
#define USB_VID		GBOOT_USB_VID	// Vendor ID
#define USB_PID		GBOOT_USB_PID	// Product ID
#define DEV_REV		0x0001	// Device Release number
#define HID_VER		0x0111	// HID specification version
#define DEV_POWER	100     // device power comsumption in mA

#include "gboot.h"
#include "gboot_usb_interface.h"
#include "gboot_usb_descriptor.h"

//-----------------------------------------------------------------------------
// Descriptor Declarations
//-----------------------------------------------------------------------------

#pragma constseg GBOOT_CONST

const __code device_descriptor_t DEVICEDESC =
{

   sizeof(device_descriptor_t),        // bLength
   DSC_DEVICE,                         // bDescriptorType
   HOST_TO_USB(USB_VER),               // bcdUSB
   0x00,                               // bDeviceClass
   0x00,                               // bDeviceSubClass
   0x00,                               // bDeviceProtocol
   EP0_PACKET_SIZE,                    // bMaxPacketSize0
   HOST_TO_USB(USB_VID),               // idVendor
   HOST_TO_USB(USB_PID),               // idProduct
   HOST_TO_USB(DEV_REV),               // bcdDevice
   0x01,                               // iManufacturer
   0x02,                               // iProduct
   0x00,                               // iSerialNumber
   0x01                                // bNumConfigurations
};

const __code config_descriptor_set_t HIDCONFIGDESC =
{

{ // configuration descriptor
   sizeof(config_descriptor_t),        // Length
   DSC_CONFIG,                         // Type
   HOST_TO_USB(sizeof(config_descriptor_set_t)),  // Totallength 
   0x01,                               // NumInterfaces
   0x01,                               // bConfigurationValue
   0x00,                               // iConfiguration
   0x80,                               // bmAttributes
   DEV_POWER/2                         // MaxPower (in 2mA units)
},

{ // interface descriptor
   sizeof(interface_descriptor_t),     // bLength
   DSC_INTERFACE,                      // bDescriptorType
   0x00,                               // bInterfaceNumber
   0x00,                               // bAlternateSetting
   0x01,                               // bNumEndpoints
   USB_CLASS_HID,                      // bInterfaceClass
   0x00,                               // bInterfaceSubClass
   0x00,                               // bInterfaceProcotol
   0x00                                // iInterface
},
{ // hid descriptor
   sizeof(class_descriptor_t),         // bLength
   DSC_HID,                            // bDescriptorType
   HOST_TO_USB(HID_VER),               // bcdHID
   0x00,                               // bCountryCode
   0x01,                               // bNumDescriptors
   0x22,                               // bDescriptorType
   HOST_TO_USB(sizeof(HIDREPORTDESC))  // wItemLength (tot. len. of report
                                       // descriptor)
},
{ // endpoint descriptor
   sizeof(endpoint_descriptor_t),      // bLength
   DSC_ENDPOINT,                       // bDescriptorType
   0x81,                               // bEndpointAddress
   0x03,                               // bmAttributes
   HOST_TO_USB(EP1_PACKET_SIZE),       // MaxPacketSize (LITTLE ENDIAN)
   10                                  // bInterval
}
};

const __code unsigned char HIDREPORTDESC[HID_REPORT_DESCRIPTOR_SIZE] =
{
    0x06, 0x00, 0xff,              // USAGE_PAGE (Vendor Defined Page 1)
    0x09, 0x01,                    // USAGE (Vendor Usage 1)
    0xa1, 0x01,                    // COLLECTION (Application)
    // 0x85, RFEAT_CMD_ID,              // Report ID
    0x95, CMD_PACKET_SIZE,         //   REPORT_COUNT ()
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x26, 0xff, 0x00,              //   LOGICAL_MAXIMUM (255)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x09, 0x01,                    //   USAGE (Vendor Usage 1)
    0xB1, 0x02,                    //   FEATURE (Data,Var,Abs)
    0xC0                           //   end Application Collection
};


extern __code unsigned char String0Desc [];

__code unsigned char String1Desc [] =
{
  sizeof(String1Desc), DSC_STRING,
  'G', 0,
  'G', 0,
  'L', 0,
  'A', 0,
  'B', 0,
  'S', 0
}; 

__code unsigned char String2Desc [] =
{
  sizeof(String2Desc), DSC_STRING,
  'G', 0,
  'B', 0,
  'O', 0,
  'O', 0,
  'T', 0
}; 

#ifndef GBOOT_TINY
const __code unsigned char* __code STRINGDESCTABLE [] =
{
   String0Desc,
   String1Desc,
   String2Desc
};
#endif

//#define NUM_DESC_STRINGS (sizeof(STRINGDESCTABLE)/sizeof(unsigned char *))
