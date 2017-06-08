/**********************************************************************
 * gboot - gboot_usb_core.c
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
#include "gboot_usb_core.h"
#include "gboot_usb_descriptor.h"
#include "gboot_usb_interface.h"

#pragma codeseg  USBSEG
#pragma constseg USBSEG

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------

#ifdef GBOOT_TINY
#define STRINGDESCTABLE TINY_STRINGDESCTABLE
#define USB_POLL() __asm acall _USB_POLL __endasm
#endif

// Redefine existing variable names to refer to the descriptors within the
// HID configuration descriptor.
#define ConfigDesc      (HIDCONFIGDESC.hid_configuration_descriptor)
#define HidDesc         (HIDCONFIGDESC.hid_descriptor)

//-----------------------------------------------------------------------------
// External Global Variable Definitions
//-----------------------------------------------------------------------------
extern __code device_descriptor_t DEVICEDESC;
extern __code config_descriptor_set_t HIDCONFIGDESC;
extern __code BYTE HIDREPORTDESC[];

extern __code unsigned char* __code STRINGDESCTABLE [];

extern __code unsigned char ONES_PACKET[2];
extern __code unsigned char ZERO_PACKET[2];


//-----------------------------------------------------------------------------
// Global Variable Definitions
//-----------------------------------------------------------------------------
unsigned char USB0_STATE;           // Holds the current USB State
unsigned char EP0_STATUS;           // Holds the status for endpoint 0
__bit EP1_STATUS;                   // IDLE=0, HALT=1


setup_packet_t SETUP;               // Buffer for current device request information

//rx_dataptr_t RXDATAPTR;           // Pointer to received data
tx_dataptr_t TXDATAPTR;             // Pointer to data to send
unsigned char DATASIZE;             // Size of data to return

__bit rt_ok;


#if 0
// these are defined in gboot_crtstart.a51
void USB_WRITE_E0CSR_END(void)
{
  USB_POLL();
  USB0ADR = E0CSR;
  USB0DAT = (rbSOPRDY | rbDATAEND);
  rt_ok=1;
}

void USB_WRITE_E0CSR_SOPRDY(void)
{
  USB_POLL();
  USB0ADR = E0CSR;
  USB0DAT = (rbSOPRDY);
  rt_ok=1;
}
#endif

#ifdef USE_EP1
void USB_WRITE_INDEX0()
{
  USB_POLL();
  USB0ADR = INDEX; 
  USB0DAT = 0;
}

void USB_WRITE_INDEX1()
{
  USB_POLL();
  USB0ADR = INDEX;
  USB0DAT = 1;
}
#endif

#ifndef GBOOT_TINY
#define SET_ZERO_PACKET() SET_PACKET(0)
#else
void SET_ZERO_PACKET()
{
  __asm
    mov	_TXDATAPTR, #(_ZERO_PACKET)
    mov	(_TXDATAPTR + 1), #( _ZERO_PACKET >> 8 )
    mov	_DATASIZE, #0x01
  __endasm;

  EP0_STATUS = EP_TX;    // Put endpoint into transmit mode
  USB_WRITE_E0CSR_SOPRDY();
}
#endif

void SET_PACKET(__bit c)
{
#ifndef GBOOT_TINY
  TXDATAPTR = (tx_dataptr_t)&ZERO_PACKET;
  if(c)   TXDATAPTR=(tx_dataptr_t)&ONES_PACKET;
  
  DATASIZE=1;
  EP0_STATUS = EP_TX;    // Put endpoint into transmit mode
  USB_WRITE_E0CSR_SOPRDY();
#else
  c; // avoid unused parameter warning
  __asm
    acall _SET_ZERO_PACKET
    jnb	  _SET_PACKET_PARM_1,00102$
    mov	  _TXDATAPTR, #(_ONES_PACKET)
00102$:
  __endasm;
#endif
}


#ifdef USB_FULL_DECODE
static __bit check_IDX0_LEN(unsigned char len)
{
#ifndef GBOOT_TINY
  if((SETUP.wIndex.i==0)&&(SETUP.wLength.i==len))
    return 1;
  return 0;
#else
  len; // avoid unused parameter warning
  __asm
    ;	if((SETUP.wIndex.i==0)&&(SETUP.wLength==len))
    mov	a,(_SETUP + 0x0004)
    orl	a,((_SETUP + 0x0004) + 1)
    orl	a,((_SETUP + 0x0006) + 1)
    clr	c
    jnz	00102$
    mov	a,dpl
    ;	mov	a,r2
    cjne	a,(_SETUP + 0x0006),00102$
    ;   return 1;
    setb c
00102$:
  __endasm;
#endif
}

static __bit check_VAL0_IDX0_LEN(unsigned char l)
{
  if(SETUP.wValue.i==0) return check_IDX0_LEN(l);
  else return 0;
}

#endif

//-----------------------------------------------------------------------------
// Get_Status
//-----------------------------------------------------------------------------
inline void Get_Status (void)                // This routine returns a two byte
{                                     // status packet to the host  
#ifdef USB_FULL_DECODE
  if ( (SETUP.wValue.i  &&          // If non-zero return length or data length not
	(SETUP.wLength.i != 2)) || SETUP.wIndex.c[MSB] )  // Equal to 2
    return;
  
  switch(SETUP.bmRequestType) {        // Determine recipient 
  case IN_DEVICE:
    if (SETUP.wIndex.c[LSB]==0)
      SET_ZERO_PACKET(); // Send 0x00, bus power and no remote wake-up supported
    break;
  case IN_INTERFACE:
    if ((USB0_STATE == DEV_CONFIGURED) && SETUP.wIndex.c[LSB])
      SET_ZERO_PACKET(); // Only valid if device is configured and non-zero index
    break;
  case IN_ENDPOINT:
    if ((USB0_STATE == DEV_CONFIGURED) 
        && (SETUP.wIndex.c[LSB] == IN_EP1) )
      SET_PACKET(EP1_STATUS);
    break;
  default:
    break;
  }
#else 
  if(SETUP.bmRequestType==IN_ENDPOINT) 
      SET_PACKET(EP1_STATUS);
  else 
    SET_ZERO_PACKET();
#endif /* USB_FULL_DECODE */
  DATASIZE = 2;
}



#if USB_FULL_DECODE
//-----------------------------------------------------------------------------
// Handle_Feature
//-----------------------------------------------------------------------------
inline void Handle_Feature()          // This routine will set/clear the EP Halt
{                                   // feature for endpoint 1
  
  // Make sure device is configured, SETUP data
  if ((USB0_STATE == DEV_CONFIGURED) &&
      (SETUP.bmRequestType == OUT_ENDPOINT)&&
      // endpoint feature is selected
      (SETUP.wValue.c[LSB] == ENDPOINT_HALT) &&
      (SETUP.wIndex.c[LSB] == IN_EP1) )
    {
#ifdef USE_EP1
      USB_WRITE_INDEX1(); // Set feature endpoint 1 halt
      if(SETUP.bRequest == SET_FEATURE) {
      	USB_WRITE_REG (EINCSR1, rbInSDSTL);
	EP1_STATUS=1;
      } else {
	USB_WRITE_REG (EINCSR1, rbInCLRDT);
	EP1_STATUS=0;
      }
      
      USB_WRITE_INDEX0();
#else
      EP1_STATUS=(SETUP.bRequest == SET_FEATURE)?1:0;
#endif
      USB_WRITE_E0CSR_END();
    }
}
#endif

//-----------------------------------------------------------------------------
// Set_Address
//-----------------------------------------------------------------------------
inline void Set_Address (void)                // Set new function address
{
#ifdef USB_FULL_DECODE
  // Request must be directed to device
  // with index and length set to zero.
  if ((SETUP.bmRequestType == OUT_DEVICE) &&
      check_IDX0_LEN(0) &&
      SETUP.wValue.c[MSB] == 0 && (SETUP.wValue.c[LSB] & 0x80) == 0 )
#else
    // FIXME: may be able to get rid of the if below as well
    if((SETUP.wValue.c[LSB] & 0x80) == 0)
#endif
      {
	EP0_STATUS = EP_ADDRESS;          // Set endpoint zero to update
	// address next status phase
	if (SETUP.wValue.c[LSB] != 0) {
	  USB0_STATE = DEV_ADDRESS;        // Indicate that device state is now address
	} else {
	  USB0_STATE = DEV_DEFAULT;        // If new address was 0x00, return
	}                                   // device to default state
	USB_WRITE_E0CSR_END();
      }
}

//-----------------------------------------------------------------------------
// Get_Descriptor
//-----------------------------------------------------------------------------
inline void Get_Descriptor (void)             // This routine sets the data pointer
{                                      // and size to correct descriptor and
                                       // sets the endpoint status to transmit
  
  switch(SETUP.wValue.c[MSB])         // Determine which type of descriptor
    {                                   // was requested, and set data ptr and
    case DSC_DEVICE:                 // size accordingly
      TXDATAPTR = (tx_dataptr_t) &DEVICEDESC;
      // DATASIZE = DEVICEDESC.bLength;
      DATASIZE= sizeof(device_descriptor_t);
      break;
      
    case DSC_CONFIG:
      TXDATAPTR = (tx_dataptr_t) &ConfigDesc;
      // must swap if using BIG ENDIAN compiler
      // DATASIZE=USB_TO_HOST(ConfigDesc.wTotalLength);
      DATASIZE=sizeof(config_descriptor_set_t);
      break;
      
    case DSC_STRING:
      // gboot is hardcoded to 2 string it also does not handle multiple languages
      if (SETUP.wValue.c[LSB] > 2)   // If asking for string that's N/A
	    return;

#ifndef GBOOT_TINY
      TXDATAPTR = (tx_dataptr_t) STRINGDESCTABLE[SETUP.wValue.c[LSB]];
#else
      // force SDCC to generate 8-bit math
      // in the tiny build the string table ***must*** be located in the first 250 bytes of flash
	  __asm
	    mov a,(_SETUP+2)
		rl a
		add a,#(_TINY_STRINGDESCTABLE)
		mov dpl,a
		clr a
		mov dph,a
		movc a,@a+dptr
		mov _TXDATAPTR,a
		mov a,#1
		movc a,@a+dptr
		mov (_TXDATAPTR+1),a
	  __endasm;
#endif
      DATASIZE = *TXDATAPTR;
      break;
    case DSC_HID:                       // HID class descriptor
      TXDATAPTR = (tx_dataptr_t)&HidDesc;
      // DATASIZE = HidDesc.bLength;
      DATASIZE=sizeof(class_descriptor_t);
      break;
    case DSC_HID_REPORT:                // HID report descriptor
      TXDATAPTR = (tx_dataptr_t)&HIDREPORTDESC;
      DATASIZE = HID_REPORT_DESCRIPTOR_SIZE;
      break;
      
    default:
      return; // unsupported request
    }
  
  if ((SETUP.wLength.c[LSB] < DATASIZE)) {
    DATASIZE = SETUP.wLength.c[LSB];
  }
  
  USB_WRITE_E0CSR_SOPRDY(); // Service SETUP Packet
  EP0_STATUS = EP_TX;             // Put endpoint in transmit mode
}

//-----------------------------------------------------------------------------
// Get_Configuration
//-----------------------------------------------------------------------------
inline void Get_Configuration (void)          // This routine returns current
{                                      // configuration value
#ifdef USB_FULL_DECODE
  // This request must be directed to the device
  // and value==0, index==0, len==1
  if ( (SETUP.bmRequestType == IN_DEVICE)    &&
       check_VAL0_IDX0_LEN(1) ) 
#endif
    {
      SET_PACKET(USB0_STATE == DEV_CONFIGURED); 
      // If the device is configured return 1 else return 0
    }
}

//-----------------------------------------------------------------------------
// Set_Configuration
//-----------------------------------------------------------------------------
inline void Set_Configuration (void)          // This routine allows host to change
{                                      // current device configuration value
#ifdef USB_FULL_DECODE
  // Device must be addressed before configured
  // and request recipient must be the device
  // the index and length words must be zero
  
  if ((USB0_STATE != DEV_DEFAULT) &&
      (SETUP.bmRequestType == OUT_DEVICE) &&
      check_IDX0_LEN(0) &&
      SETUP.wValue.c[MSB]  == 0 && (SETUP.wValue.c[LSB] < 2))
#endif
    {
      // gboot only supports config = 0,1
      //    if (SETUP.wValue.c[LSB]) {
      if (SETUP.wValue.c[LSB]==1) {
	// Any configuration request
	// results in configuration being set to 1
	USB0_STATE = DEV_CONFIGURED;

#ifdef USE_EP1
	USB_WRITE_INDEX1();   // Change index to endpoint 1
	// Set DIRSEL to indicate endpoint 1 is IN/OUT
	USB_WRITE_REG (EINCSR2, rbInSPLIT);
	USB_WRITE_INDEX0();   // Set index back to endpoint 0
	
#endif
	EP1_STATUS=0;       // Set endpoint status to idle (enabled)
      } else {
	USB0_STATE = DEV_ADDRESS;     // Unconfigures device by setting state
	EP1_STATUS=1;                 // to address, and changing endpoint 1
      }
      
      USB_WRITE_E0CSR_END();
    }
}

//-----------------------------------------------------------------------------
// Get_Interface
//-----------------------------------------------------------------------------
inline void Get_Interface (void)              // This routine returns 0x00, since
{                                      // only one interface is supported by
                                       // gboot
#ifdef USB_FULL_DECODE
  // If device is configured
  // and  recipient is an interface
  // and val==0, index==0, lenght==1
  if ((USB0_STATE == DEV_CONFIGURED) &&
      (SETUP.bmRequestType == IN_INTERFACE) &&
      check_VAL0_IDX0_LEN(1) ) 
#endif
    {
      SET_ZERO_PACKET();
    }
}

//-----------------------------------------------------------------------------
// Set_Interface
//-----------------------------------------------------------------------------
inline void Set_Interface(void)
{
#ifdef USB_FULL_DECODE
  // Make sure request is directed at interface
  // and all other packet values are set to zero
  if ( (SETUP.bmRequestType == OUT_INTERFACE)  &&
       check_VAL0_IDX0_LEN(0) )
#endif
    { 
      USB_WRITE_E0CSR_END(); 
    }
}


void Handle_Request(void)
{
  rt_ok=0;
  
  switch(SETUP.bmRequestType&RT_TYPE_MASK) {
  case RT_TYPE_STD:
    switch (SETUP.bRequest) {   // handle standard requests
    case GET_STATUS:
      Get_Status();
      break;
    case CLEAR_FEATURE:
#ifndef USB_FULL_DECODE
      EP1_STATUS=0;
      USB_WRITE_E0CSR_END();
      break;
#endif
    case SET_FEATURE:
#ifndef USB_FULL_DECODE
      EP1_STATUS=1;
      USB_WRITE_E0CSR_END();
#else
      Handle_Feature();
#endif
      break;
    case SET_ADDRESS:
      Set_Address();
      break;
    case GET_DESCRIPTOR:
      Get_Descriptor();
      break;
    case GET_CONFIGURATION:
      Get_Configuration();
      break;
    case SET_CONFIGURATION:
      Set_Configuration();
      break;
    case GET_INTERFACE:
      Get_Interface();
      break;
    case SET_INTERFACE:
      Set_Interface();
      break;
    default:
    }
    break;
  case RT_TYPE_CLASS:
    switch (SETUP.bRequest) {
    case GET_REPORT:
      // TXDATAPTR is updated by the read command 
      EP0_STATUS = EP_TX;            // Endpoint 0 in transmit mode
      break;
    case SET_REPORT:
      //      RXDATAPTR = (rx_dataptr_t)&OUT_PACKET;
      // receiving to OUT_PACKET is implied if not a setup packet
      EP0_STATUS = EP_RX;            // Endpoint 0 in receive mode
      break;
    default:
      goto end;
    }
    DATASIZE = CMD_PACKET_SIZE;
    USB_WRITE_E0CSR_SOPRDY();
    break;
    // case RT_TYPE_VENDOR:
    //   break;
  default:
  }
  
 end:
#ifndef GBOOT_TINY
  if(!rt_ok) {
        USB_WRITE_E0CSR(rbSDSTL);   // Set the send stall bit
    EP0_STATUS = EP_STALL;      // Put the endpoint in stall status
  }
#else
  __asm
    jb _rt_ok, 20200$
    mov     r0,#0x20
    acall   __USB_WRITE_E0CSR_ASM
    mov     _EP0_STATUS,#0x04
20200$:
  __endasm;
#endif

}
