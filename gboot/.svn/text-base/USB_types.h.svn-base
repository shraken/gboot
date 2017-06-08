/**********************************************************************
 * gboot - USB_types.h
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

#ifndef _USB_TYPES_H_
#define _USB_TYPES_H_

typedef unsigned char BYTE;
typedef unsigned int WORD;
typedef union {unsigned int i; unsigned char c[2];} UUWORD;

//-----------------------------
// Setup Packet
//-----------------------------
typedef struct
{
   BYTE bmRequestType;        // Request recipient, type, and dir.
   BYTE bRequest;             // Specific standard request number
   UUWORD wValue;             // varies according to request
   UUWORD wIndex;             // varies according to request
   UUWORD wLength;            // Number of bytes to transfer
} setup_packet_t;

//------------------------------------------
// Device Descriptor
//------------------------------------------
typedef struct
{
   BYTE bLength;              // Size of this Descriptor in Bytes
   BYTE bDescriptorType;      // Descriptor Type (=1)
   WORD bcdUSB;               // USB Spec Release Number in BCD
   BYTE bDeviceClass;         // Device Class Code
   BYTE bDeviceSubClass;      // Device Subclass Code
   BYTE bDeviceProtocol;      // Device Protocol Code
   BYTE bMaxPacketSize0;      // Maximum Packet Size for EP0
   WORD idVendor;             // Vendor ID
   WORD idProduct;            // Product ID
   WORD bcdDevice;            // Device Release Number in BCD
   BYTE iManufacturer;        // Index of String Desc for Manufacturer
   BYTE iProduct;             // Index of String Desc for Product
   BYTE iSerialNumber;        // Index of String Desc for SerNo
   BYTE bNumConfigurations;   // Number of possible Configurations
} device_descriptor_t;

//--------------------------------------------------
// Configuration Descriptor
//--------------------------------------------------
typedef struct
{
   BYTE bLength;              // Size of this Descriptor in Bytes
   BYTE bDescriptorType;      // Descriptor Type (=2)
   WORD wTotalLength;         // Total Length of Data for this Conf
   BYTE bNumInterfaces;       // No of Interfaces supported by this
                              // Conf
   BYTE bConfigurationValue;  // Designator Value for *this*
                              // Configuration
   BYTE iConfiguration;       // Index of String Desc for this Conf
   BYTE bmAttributes;         // Configuration Characteristics (see below)
   BYTE bMaxPower;            // Max. Power Consumption in this
                              // Conf (*2mA)
} config_descriptor_t;

//----------------------------------------------
// Interface Descriptor
//----------------------------------------------
typedef struct
{
   BYTE bLength;              // Size of this Descriptor in Bytes
   BYTE bDescriptorType;      // Descriptor Type (=4)
   BYTE bInterfaceNumber;     // Number of *this* Interface (0..)
   BYTE bAlternateSetting;    // Alternative for this Interface (if any)
   BYTE bNumEndpoints;        // No of EPs used by this IF (excl. EP0)
   BYTE bInterfaceClass;      // Interface Class Code
   BYTE bInterfaceSubClass;   // Interface Subclass Code
   BYTE bInterfaceProtocol;   // Interface Protocol Code
   BYTE iInterface;           // Index of String Desc for this Interface
} interface_descriptor_t;

//------------------------------------------
// Class Descriptor
//------------------------------------------
typedef struct
{
   BYTE bLength;              // Size of this Descriptor in Bytes (=9)
   BYTE bDescriptorType;      // Descriptor Type (HID=0x21)
   WORD bcdHID;               // HID Class Specification
                              // release number (=1.01)
   BYTE bCountryCode;         // Localized country code
   BYTE bNumDescriptors;      // Number of class descriptors to follow
   BYTE bReportDescriptorType;// Report descriptor type (HID=0x22)
   WORD wItemLength;          // Total length of report descriptor table
} class_descriptor_t;

//---------------------------------------------
// Endpoint Descriptor
//---------------------------------------------
typedef struct
{
   BYTE bLength;              // Size of this Descriptor in Bytes
   BYTE bDescriptorType;      // Descriptor Type (=5)
   BYTE bEndpointAddress;     // Endpoint Address (Number + Direction)
   BYTE bmAttributes;         // Endpoint Attributes (Transfer Type)
   WORD wMaxPacketSize;       // Max. Endpoint Packet Size
   BYTE bInterval;            // Polling Interval (Interrupt) ms
} endpoint_descriptor_t;

// bmRequestType bitmaps
// USB v2.0 table 9-2
#define RT_DIR_IN                0x80  // direction IN (device to host)
#define RT_DIR_OUT               0x00  // direction OUT (host to device)
#define RT_DIR_MASK              0x80

#define RT_TYPE_STD              0x00
#define RT_TYPE_CLASS            0x20
#define RT_TYPE_VENDOR           0x40
#define RT_TYPE_RSVD             0x60
#define RT_TYPE_MASK             0x60

#define RT_RECIPIENT_DEV         0x00
#define RT_RECIPIENT_INT         0x01
#define RT_RECIPIENT_ENDP        0x02
#define RT_RECIPIENT_OTHER       0x03
#define RT_RECIPIENT_MASK        0x1f


// some pre-cooked definitions
#define OUT_DEVICE    (RT_DIR_OUT|RT_TYPE_STD|RT_RECIPIENT_DEV)  // Request made to device,
                                       // host to device (OUT)
#define IN_DEVICE     (RT_DIR_IN|RT_TYPE_STD|RT_RECIPIENT_DEV)  // Request made to device,
                                       // device to host(IN)
#define OUT_INTERFACE (RT_DIR_OUT|RT_TYPE_STD|RT_RECIPIENT_INT)  // Request made to interface,
                                       // host to device (OUT)
#define IN_INTERFACE  (RT_DIR_IN|RT_TYPE_STD|RT_RECIPIENT_INT)  // Request made to interface,
                                       // device to host(IN)
#define OUT_ENDPOINT  (RT_DIR_OUT|RT_TYPE_STD|RT_RECIPIENT_ENDP)  // Request made to endpoint,
                                       // host to device (OUT)
#define IN_ENDPOINT   (RT_DIR_IN|RT_TYPE_STD|RT_RECIPIENT_ENDP)  // Request made to endpoint,
                                       // device to host(IN)

// wIndex bitmaps
// USB v2.0 spec figure 9-2
#define IDX_EP_DIR_OUT               0x00  // direction OUT (host to device)
#define IDX_EP_DIR_IN                0x80  // direction IN (device to host)
#define IDX_EP_DIR_MASK              0x80

#define IDX_EP_EP_MASK               0x0f

//more pre-cooked definitions
#define  IN_EP1                  0x81  // Index values used by Set and Clear
                                       // feature
#define  OUT_EP1                 0x01  // commands for Endpoint_Halt

// Standard Request Codes
// USB v2.0 spec table 9-4
#define  GET_STATUS              0x00  // Code for Get Status
#define  CLEAR_FEATURE           0x01  // Code for Clear Feature
#define  SET_FEATURE             0x03  // Code for Set Feature
#define  SET_ADDRESS             0x05  // Code for Set Address
#define  GET_DESCRIPTOR          0x06  // Code for Get Descriptor
#define  SET_DESCRIPTOR          0x07  // Code for Set Descriptor(not used)
#define  GET_CONFIGURATION       0x08  // Code for Get Configuration
#define  SET_CONFIGURATION       0x09  // Code for Set Configuration
#define  GET_INTERFACE           0x0A  // Code for Get Interface
#define  SET_INTERFACE           0x0B  // Code for Set Interface
#define  SYNCH_FRAME             0x0C  // Code for Synch Frame(not used)

// Standard Descriptor Types
// USB v2.0 spec table 9-5
#define  DSC_DEVICE              0x01  // Device Descriptor
#define  DSC_CONFIG              0x02  // Configuration Descriptor
#define  DSC_STRING              0x03  // String Descriptor
#define  DSC_INTERFACE           0x04  // Interface Descriptor
#define  DSC_ENDPOINT            0x05  // Endpoint Descriptor
#define  DSC_DEV_QUALIFIER       0x06  // Device Qualifier Descriptor
#define  DSC_OTHER_SPEED_CONFIG  0x07  // Other Speed Configuration Descriptor
#define  DSC_INTERFACE_POWER     0x08  // Interface Power Descriptor

// Define wValue bitmaps for Standard Feature Selectors
// USB v2.0 spec table 9-6
#define  DEVICE_REMOTE_WAKEUP    0x01  // Remote wakeup feature (device)
#define  ENDPOINT_HALT           0x00  // Endpoint_Halt feature selector (endpoint)
#define  TEST_MODE               0x02  // (device)

// HID Descriptor Types
#define DSC_HID               0x21   // HID Class Descriptor
#define DSC_HID_REPORT        0x22   // HID Report Descriptor

// HID Request Codes
#define GET_REPORT         0x01   // Code for Get Report
#define GET_IDLE           0x02   // Code for Get Idle
#define GET_PROTOCOL       0x03   // Code for Get Protocol
#define SET_REPORT         0x09   // Code for Set Report
#define SET_IDLE           0x0A   // Code for Set Idle
#define SET_PROTOCOL       0x0B   // Code for Set Protocol


// USB Base classes
// from usb.org website
#define USB_CLASS_DEFER_TO_INTERFACE 0x00
#define USB_CLASS_AUDIO         0x01
#define USB_CLASS_COMMUNICATION 0x02
#define USB_CLASS_HID           0x03
#define USB_CLASS_PYSICAL       0x05
#define USB_CLASS_IMAGE         0x06
#define USB_CLASS_PRINTER       0x07
#define USB_CLASS_MASS_STORAGE  0x08
#define USB_CLASS_HUB           0x09
#define USB_CLASS_DCD_DATA      0x0A
#define USB_CLASS_SMART_CARD    0x0B
#define USB_CLASS_CONTENT_SECURITY 0x0D
#define USB_CLASS_VIDEO         0x0E
#define USB_CLASS_PERSONAL_HEALTHCARE 0x0F
#define USB_CLASS_DIAGNOSTIC_DEVICE  0xDC
#define USB_CLASS_WIRELESS_CONTROLLER 0xE0
#define USB_CLASS_MISC          0xEF
#define USB_CLASS_APPLICATION_SPECIFIC 0xFE
#define USB_CLASS_VENDOR_SPECIFIC 0xFF

// all of the above except DEFER_TO_INTERFACE and HUB can be used at the interface level
// COMMUNICATION, DIAGNOSTIC, MIC and VENDOR_SPECIFIC can be used at either the interface or device level


// definitions below are little endian
#define LSB 0
#define MSB 1
#define HOST_TO_USB(x) (x)
#define USB_TO_HOST(x) (x)

#endif
