/**********************************************************************
 * gboot - gboot_usb_core.h
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

#ifndef _USB_CORE_H_
#define _USB_CORE_H_

#include "USB_Registers.h"

// Device states
#define  DEV_ATTACHED            0x00
#define  DEV_POWERED             0x01
#define  DEV_DEFAULT             0x02
#define  DEV_ADDRESS             0x03
#define  DEV_CONFIGURED          0x04
#define  DEV_SUSPENDED           0x05

// Endpoint States
#define  EP_IDLE                 0x00
#define  EP_TX                   0x01
#define  EP_RX                   0x02
#define  EP_HALT                 0x03
#define  EP_STALL                0x04
#define  EP_ADDRESS              0x05


#ifdef GBOOT_TINY
typedef __code unsigned char * tx_dataptr_t;
#else
typedef unsigned char * tx_dataptr_t;
#endif

typedef __data unsigned char * rx_dataptr_t;

//extern rx_dataptr_t RXDATAPTR;                // Pointer to data to receive
extern tx_dataptr_t TXDATAPTR;                // Pointer to data to send
extern unsigned char DATASIZE;                // tx/rx data lenght

extern setup_packet_t SETUP;

// USB device and endpoint status
extern unsigned char USB0_STATE;
extern unsigned char EP0_STATUS;
extern __bit EP1_STATUS;  // IDLE=0, HALT=1

// Function prototypes
void USB_POLL (void);
void ReportHandler(void);
void Handle_Request(void);
void Fifo_Read_EP0 (unsigned char __data *);

unsigned char USB_READ_REG(unsigned char a);
void USB_READ_E0CSR();

void USB_WRITE_REG_REV(unsigned char d, unsigned char a);
void USB_WRITE_E0CSR(unsigned char d);
void USB_WRITE_E0CSR_END();
void USB_WRITE_E0CSR_SOPRDY();
void USB_WRITE_INDEX(unsigned char d);

#define USB_WRITE_REG(a,d) USB_WRITE_REG_REV(d,a)

#ifdef USE_EP1
void USB_WRITE_INDEX0();
void USB_WRITE_INDEX1();
#endif

#endif      /* _USB_CORE_H_ */
