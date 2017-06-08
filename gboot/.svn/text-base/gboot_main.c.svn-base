/**********************************************************************
 * gboot - gboot_main.c
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

#include "gboot_usb_interface.h"
#include "gboot.h"
#include "gboot_usb_core.h"

#pragma codeseg MAINSEG

//-----------------------------------------------------------------------------
// Global Variable Definitions
//-----------------------------------------------------------------------------
__data unsigned char OUT_PACKET[CMD_PACKET_SIZE];   // Last data packet received from host

//-----------------------------------------------------------------------------
// Local Variable Definitions
//-----------------------------------------------------------------------------
unsigned char ControlReg;           // Temporary storage for EP control

//-----------------------------------------------------------------------------
// Definitions
//-----------------------------------------------------------------------------
static void Handle_Control (void);

#define GBOOT_EXVER (GBOOT_VERSION | (GBOOT_API<<12) )

// pass defines to the assembler/linker
__code  __at(GBOOT_EXVER) unsigned char gboot_version [];
__code  __at(FLASH_START) unsigned char user_code_start [];
__code  __at(FLASH_END)   unsigned char user_flash_end [];

__code  __at(MCU_TYPE) unsigned char mcu_type [];
__code  __at(HW_PID)   unsigned char hw_pid [];
__code  __at(HW_REV)   unsigned char hw_rev [];
__code  __at(PCB_REV)  unsigned char pcb_rev [];


#ifndef USE_EP1
#define USB_WRITE_INDEX0()  USB_WRITE_REG_REV(0,INDEX)
#endif

#ifdef GBOOT_TINY
// override the default acall for functions in the far segment
#define ReportHandler()       __asm lcall _ReportHandler __endasm
#define Handle_Request()      __asm lcall _Handle_Request __endasm
#define USB_WRITE_E0CSR_END() __asm lcall _USB_WRITE_E0CSR_END __endasm

// redefine USB_POLL to local version to save space
#define USB_POLL() USB_POLL_LOCAL()

void USB_POLL_LOCAL() __naked
{
  __asm 
    ljmp _USB_POLL
  __endasm;
}

#endif

void gboot_main(void)
{
  unsigned char i;
  
  // setup the stack for the C runtime
  __asm
    mov sp,#__start__stack - 1
  __endasm;

  //  PCA0MD &= ~0x40;                    // Disable Watchdog timer
  PCA0MD = 0x00;                    // Disable Watchdog timer
  
//-----------------------------------------------------------------------------
// Start firmware unless user requested to enter the bootloader
//-----------------------------------------------------------------------------
    
#ifndef GBOOT_TINY
  // 3 ways to enter bootloader mode
  // 1- shorting P3.0 to gnd
  // 2- corrupted reset vector in firmware
  // 3- sw initiated reboot with xdata flag  
  if( 
     ( (P3_0==1) && 
       (*((__code unsigned char * )FLASH_START)==0x02) ) &&
     ( !( (RSTSRC&0x10) && (*((__xdata unsigned char * )0x0000)==0xa5) ) )
      ) {
    
    //          __asm
    //          ljmp	# FLASH_START
    //        __endasm;
    // the above is more correct as the c code will generate
    // a lcall instead of ljmp but the compiler will not
    // optimize the if statement resulting in larger code
    
    ((void (__code *) (void)) FLASH_START)();
  }

  // reset the bootloader entry flag
  *((__xdata unsigned char * )0x0000)=0x00;
#else 
  // GBOOT_TINY has only 2 ways to enter bootloader mode:
  // 1- shorting P3.0 to gnd
  // 2- corrupted reset vector in firmware  
  // (removed) 3- sw reset (it does not seem to be reliable on first power on)
#if 0
  if( 
     ( (P3_0==1) && 
       //	 ( !( RSTSRC&0x04) ) &&
       (*((__code unsigned char * )FLASH_START)==0x02) )
     
      ) {
    ((void (__code *) (void)) FLASH_START)();
  }
#endif
  __asm
    jnb	_P3_0,10102$
    mov	dptr,# FLASH_START
    clr	a
    movc	a,@a+dptr
    cjne	a,#0x02,10102$
    ljmp	# FLASH_START
10102$:
  __endasm;

#endif /* GBOOT_TINY */
  
//-----------------------------------------------------------------------------
// Sysclk_Init
//-----------------------------------------------------------------------------
  OSCICN |= 0x03;            // Configure internal oscillator for its maximum frequency
                             // and enable the missing clock detector
  // CLKMUL  = 0x00;
  CLKMUL = 0x80;            // Enable clock multiplier
  CLKMUL |= 0xC0;           // Initialize the clock multiplier
  // if the above is combined sw reboot occasionally does not work
  //    CLKMUL = 0xC0;                     // Initialize the clock multiplier
  
  VDM0CN=0x80;              // enable VDDMON - required for flashing to work
  
  // Delay for clock multiplier to begin
  i=250;
  do { 
    //    __asm nop __endasm;
  } while(--i);


  while(!(CLKMUL & 0x20));                // Wait for multiplier to lock
  // CLKSEL  = SYS_4X_DIV_2;              // Select 24MHZ system clock
  // CLKSEL |= USB_4X_CLOCK;              // Select 48MHz USB clock
  CLKSEL  = SYS_4X_DIV_2 | USB_4X_CLOCK;  // Select 48MHz USB clock
  
  //  VDM0CN=0x80; // enable VDDMON - moved up
  RSTSRC=0x02; // enable reset source
  // the above is also required for flashing to work
  
//-----------------------------------------------------------------------------
// USB_Init
//-----------------------------------------------------------------------------
  USB_WRITE_REG(POWER,  0x08);    // Force Asynchronous USB Reset
  USB0XCN = 0xE0;                 // Enable transceiver; select full speed
  USB_WRITE_REG(CLKREC, 0x80);    // Enable clock recovery, single-step mode disabled

USB_RESET:
  USB0_STATE = DEV_DEFAULT;      // Set device state to default
  USB_WRITE_REG(POWER, 0x01);    // Clear usb inhibit bit to enable USB suspend detection
  EP0_STATUS = EP_IDLE;          // Set default Endpoint Status
  EP1_STATUS = 1;                // for EP1 1=HALT
  USB_WRITE_INDEX0();            // Set Index to Endpoint Zero
  
//-----------------------------------------------------------------------------
// Main Loop
//-----------------------------------------------------------------------------
  while (1) {
    unsigned char bCommon, bIn;
    bCommon=USB_READ_REG(CMINT);    // Read all interrupt registers
    bIn=USB_READ_REG(IN1INT);       // this read also clears the register
    
    if (bCommon & rbRSTINT) {       // Handle Reset interrupt
      goto USB_RESET;
    }
    if (bIn & rbEP0) {              // Handle SETUP packet received
                                    // or packet transmitted if Endpoint 0
      Handle_Control();             // is in transmit mode
    }
  }
}

void Fifo_Read_EP0(unsigned char __data * pData)
{
#ifndef GBOOT_TINY
  if (DATASIZE) {             // Check if >0 bytes requested,
    USB0ADR = FIFO_EP0 | 0xc0;
    
    // Unload <DATASIZE> bytes from the selected FIFO
    do {
      USB_POLL();
      *pData++=USB0DAT;
    } while(--DATASIZE);
    
    //while(USB0ADR & 0x80);         // Wait for BUSY->'0' (data ready)
    USB0ADR = 0;                     // Clear auto-read
  }
#else
  pData;
  __asm
   	mov	r1,dpl
	mov	_USB0ADR,#0xE0
20101$:
	acall	_USB_POLL_LOCAL
	mov	@r1,_USB0DAT
	inc	r1
	djnz	_DATASIZE,20101$
	mov	_USB0ADR,#0x00      ; clear auto-read
  __endasm;
#endif
}


inline void Fifo_Write_EP0()
{
  // Write <DATASIZE> bytes to the EP0 FIFO
  USB_POLL();
  USB0ADR = FIFO_EP0;           // Set address (mask out bits7-6)

#ifndef GBOOT_TINY
  do {
	USB0DAT = *TXDATAPTR++;
	USB_POLL();
  } while(--DATASIZE);
#else
  __asm
    mov dpl,_TXDATAPTR
    mov dph,(_TXDATAPTR+1)
  20111$:
    clr a
    movc a,@a+dptr
    mov _USB0DAT,a
    inc dptr
    acall _USB_POLL_LOCAL
    djnz _DATASIZE,20111$
  __endasm;
#endif
}

void Handle_Control (void)
{
  USB_READ_E0CSR();
  
  if (EP0_STATUS == EP_ADDRESS) {    // Handle Status Phase of Set Address command
    USB_WRITE_REG (FADDR, SETUP.wValue.c[LSB]);
    EP0_STATUS = EP_IDLE;
  }
  
  if (ControlReg & rbSTSTL) {  // If last packet was a sent stall, reset STSTL bit and return EP0 to idle state
    USB_WRITE_E0CSR(0);
    EP0_STATUS = EP_IDLE;
    return;
  }
  
  if (ControlReg & rbSUEND) {      // If last SETUP transaction was ended prematurely then set
    // USB_WRITE_E0CSR(rbDATAEND);    // Serviced SETUP End bit and return EP0
    // USB_WRITE_E0CSR(rbSSUEND);
    USB_WRITE_E0CSR(rbSSUEND|rbDATAEND);
    EP0_STATUS = EP_IDLE;          // To idle state
  }
  
  if (EP0_STATUS == EP_IDLE) {     // If Endpoint 0 is in idle mode
    if (ControlReg & rbOPRDY)  {   // Make sure that EP 0 has an Out Packet
                                   // ready from host although if EP0 is idle, this should always be the case
      
      DATASIZE = sizeof(setup_packet_t);
      Fifo_Read_EP0((unsigned char __data *)&SETUP);
      Handle_Request();   
    }
  }
  
  if (EP0_STATUS == EP_TX) {          // See if endpoint should transmit
    if ( !(ControlReg & rbINPRDY) ) {  // Don't overwrite last packet
      USB_READ_E0CSR();               // ControlReg=USB_READ_BYTE (E0CSR);
      // Check to see if SETUP End or Out Packet received, if so do not put any new data on FIFO
      // if ((!(ControlReg & rbSUEND)) || (!(ControlReg & rbOPRDY)))
      if ( !(ControlReg & (rbSUEND | rbOPRDY)) ) {
	    Fifo_Write_EP0();
  	    EP0_STATUS = EP_IDLE;         // Return EP0 to idle state
        USB_WRITE_E0CSR(rbINPRDY|rbDATAEND);
      }
    }
  }
  
  if (EP0_STATUS == EP_RX) {     // See if endpoint should receive
    USB_READ_E0CSR();            // Read control register
    
    if (ControlReg & rbOPRDY) {  // Verify packet was received
      // Fifo_Read_EP0((unsigned char __data *) RXDATAPTR);
      Fifo_Read_EP0((unsigned char __data *)OUT_PACKET);
      
      EP0_STATUS = EP_IDLE;      // set Endpoint to IDLE
      if ( (SETUP.bRequest == SET_REPORT) ) {
	    ReportHandler();
      }
      
      USB_WRITE_E0CSR_END();
    }
  }
}


unsigned char USB_READ_REG(unsigned char a)
{
  USB_POLL();
  USB0ADR = (0x80|a);
  USB_POLL();
  return USB0DAT;
}

void USB_READ_E0CSR()
{
  ControlReg=USB_READ_REG(E0CSR);
}

void USB_WRITE_E0CSR(unsigned char d)
{
  USB_WRITE_REG_REV(d,E0CSR);
}

void USB_WRITE_REG_REV(unsigned char d, unsigned char a)
{
  USB_POLL();
  USB0ADR = (a); 
  USB0DAT = d;
}
