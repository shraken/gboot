;**********************************************************************
;* gboot - gboot_crtstart.a51
;* A simple USB HID bootloader for silicon laboratories USB mcus
;*
;* Copyright (c) 2010, 2013 Gabriele Gorla
;*
;* This program is free software: you can redistribute it and/or modify
;* it under the terms of the GNU General Public License as published by
;* the Free Software Foundation, either version 3 of the License, or
;* (at your option) any later version.
;*
;* This program is distributed in the hope that it will be useful,
;* but WITHOUT ANY WARRANTY; without even the implied warranty of
;* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;* GNU General Public License for more details.
;*
;* You should have received a copy of the GNU General Public License
;* along with this program.  If not, see <http://www.gnu.org/licenses/>.
;**********************************************************************
.module gboot_crtstart

.area HOME (CODE)
.area CSEG (CODE)

.include "chip_config.a51"

.globl _user_code_start
.globl __start__stack
.globl _gboot_main

.globl _TINY_STRINGDESCTABLE
.globl _String0Desc
.globl _String1Desc
.globl _String2Desc

.globl _GBOOT_INFO_PACKET
.globl _FLASH_INFO_PACKET
.globl _DEV_INFO_PACKET
.globl _EXT_INFO_PACKET

.globl _ZERO_PACKET
.globl _ONES_PACKET

.globl _gboot_version
.globl _user_flash_end
.globl _mcu_type
.globl _hw_pid
.globl _hw_rev
.globl _pcb_rev

.globl _rt_ok

.globl _USB0ADR
.globl _USB0DAT
.globl _USB_POLL
.globl _USB_WRITE_E0CSR_END
.globl _USB_WRITE_E0CSR_SOPRDY
.globl __USB_WRITE_E0CSR_ASM
;--------------------------------------------------------
; Stack segment in internal ram
;--------------------------------------------------------
.area SSEG (DATA)

__start__stack:
.ds 1

;--------------------------------------------------------
; reset vector and interrupt redirection
;--------------------------------------------------------
.area HOME (CODE)

gboot_interrupt_vect:
ljmp _gboot_main; reset vector
ajmp _user_code_start + 0x03 ; interrupt 0
.db #0xFF  ; unused
_GBOOT_INFO_PACKET:  ; offset 0x06
.byte (_gboot_version)      ; gboot minor version
.byte (_gboot_version>>8)   ; gboot api + gboot major version
.byte (_mcu_type)           ; silabs mcu
.byte (_mcu_type>>8)        ; silabs mcu
.db #0xFF    ; unused
ajmp _user_code_start + 0x0B; interrupt 1
.db #0xFF  ; unused
_FLASH_INFO_PACKET: ; offset 0x0e
.byte (_user_code_start>>8) ; flash first page 
.byte (_user_flash_end>>8)  ; flash last page
.byte #0x09                 ; flash page size
.byte #0xff                 ; reserved
.db #0xFF  ; unused
ajmp _user_code_start + 0x13 ; interrupt 2
.db #0xFF  ; unused
_DEV_INFO_PACKET:  ; offset 0x16
.byte (_hw_pid)    ; USB PID 
.byte (_hw_pid>>8) ; USB_PID
.byte (_hw_rev)    ; hw revision 
.byte (_pcb_rev)   ; pcb rev
.db #0xFF  ; unused
ajmp _user_code_start + 0x1B ; interrupt 3
.db #0xFF  ; unused
_EXT_INFO_PACKET: ; offset 0x1e
.byte #0xff  ; reserved
.byte #0xff  ; reserved
.byte #0xff  ; reserved
.byte #0xff  ; reserved
.db #0xFF  ; unused
ajmp _user_code_start + 0x23; interrupt 4
.db #0xFF  ; unused
_DEV_SERIAL:  ; offset 0x26
.byte #0xff  ; device serial # in bcd
.byte #0xff  ; 
.byte #0xff  ; 
.byte #0xff  ; 
.db #0xFF  ; unused
ajmp _user_code_start + 0x2B ; interrupt 5
.db #0xFF  ; unused
_DEV_CONFIG: ; offset 0x2e
.byte #0xff  ; device specific
.byte #0xff  ; 
.byte #0xff  ; 
.byte #0xff  ; 
.db #0xFF  ; unused
ajmp _user_code_start + 0x33 ; interrupt 6
.ds 6
ajmp _user_code_start + 0x3B ; interrupt 7
.ds 6
ajmp _user_code_start + 0x43 ; interrupt 8
.ds 6
ajmp _user_code_start + 0x4B ; interrupt 9
.ds 6
ajmp _user_code_start + 0x53 ; interrupt 10
.ds 6
ajmp _user_code_start + 0x5B ; interrupt 11
_TINY_STRINGDESCTABLE:
.byte _String0Desc,(_String0Desc >> 8)	
.byte _String1Desc,(_String1Desc >> 8)	
.byte _String2Desc,(_String2Desc >> 8)
ajmp _user_code_start + 0x63 ; interrupt 12
_String0Desc:
.db #0x04  ; Sring0Len
.db #0x03  ; DSC_STRING
.db #0x09  ;  
.db #0x04  ;
_ONES_PACKET:
.db #0x01  ; ones packet
.db #0x00
ajmp _user_code_start + 0x6B ; interrupt 13
_USB_POLL:
00101$:
	mov	a,_USB0ADR
	jb	acc.7,00101$
	ret
ajmp _user_code_start + 0x73 ; interrupt 14
_USB_WRITE_E0CSR_END:
;	USB0DAT = (rbSOPRDY | rbDATAEND);	
	mov r0, #0x48
	sjmp  __USB_WRITE_E0CSR_ASM
_ZERO_PACKET:
.db #0x00  ; zeros packet
.db #0x00
ajmp _user_code_start + 0x7B ; interrupt 15
.if UART1|F38x
; the vectors below are necessary only for chips with 2 UARTs (some F34x and all F38x)
.ds 6
ajmp _user_code_start + 0x83 ; interrupt 16
.endif
.if F38x
; the interrupt vectors below are necessary only for F38x
.ds 6
.ds 2 ; interrupt vector 17 is not used (reserved)
.ds 6
ajmp _user_code_start + 0x93 ; interrupt 18
.ds 6
ajmp _user_code_start + 0x9B ; interrupt 19
.ds 6
ajmp _user_code_start + 0xA3 ; interrupt 20
.endif


_USB_WRITE_E0CSR_SOPRDY:
;	USB0DAT = (rbSOPRDY);	
	mov r0, #0x40
__USB_WRITE_E0CSR_ASM:
	acall _USB_POLL 
;	USB0ADR = E0CSR;
	mov	_USB0ADR,#0x11
	mov	_USB0DAT,r0
	setb	_rt_ok       ; rt_ok=1
	ret
