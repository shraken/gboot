BUILD=tiny

MCU=0xF320
PROD_PID=0xffff
PROD_REV=0x10
PROD_PCB=0xA0
# variant can be one of: basic, dual_uart, F38x
VARIANT=basic


EXT_INFO=0xffffffff
PROD_SERIAL=0xffffffff
PROD_CONFIG=0xffffffff

FLASH_SIZE=16
FW_START=0x0400

#CC = sdcc
#AS = sdas8051
#CC = /usr/local/sdcc30/bin/sdcc
#AS = /usr/local/sdcc30/bin/sdas8051
CC = /mnt/share/local/sdcc33/bin/sdcc
AS = /mnt/share/local/sdcc33/bin/sdas8051

# now both tiny and standard are 1.5k and use first 2 and last page of flash
ifeq ($(FLASH_SIZE),16)
HIGH_SEG_T=0x3c00
HIGH_SEG_S=0x3c00
#HIGH_SEG_S=0x3a00
endif
ifeq ($(FLASH_SIZE),32)
HIGH_SEG_T=0x7e00
HIGH_SEG_S=0x7e00
#HIGH_SEG_S=0x7c00
endif
ifeq ($(FLASH_SIZE),64)
HIGH_SEG_T=0xfa00
HIGH_SEG_S=0xfa00
#HIGH_SEG_S=0xf800
endif


# CSIZE should be empty_flash-1 to account for the lock byte
ifeq ($(BUILD),tiny)
HIGH_SEG=$(HIGH_SEG_T)
CSIZE=0x05ff
CFLAGS = -DGBOOT_TINY 
JMP_SIZE= --acall-ajmp
LFLAGS = --xram-loc 0x4000 --xram-size 0x0400 --code-loc 0x0000 --code-size $(CSIZE) -Wl -bGBOOT_CONST=$(HIGH_SEG) -Wl -bUSBSEG=0x0000 -Wl -bMAINSEG=$(HIGH_SEG)
else
HIGH_SEG=$(HIGH_SEG_S)
CSIZE=0x05ff
CFLAGS = -DUSB_FULL_DECODE
LFLAGS = --xram-loc 0x4000 --xram-size 0x0400 --code-loc 0x0000 --code-size $(CSIZE) -Wl -bGBOOT_CONST=$(HIGH_SEG) -Wl -bUSBSEG=0x0000 -Wl -bMAINSEG=$(HIGH_SEG)
#LFLAGS = --xram-loc 0x4000 --xram-size 0x0400 --code-loc 0x0000 --code-size $(CSIZE) -Wl -bGBOOT_CONST=0x0000 -Wl -bUSBSEG=$(HIGH_SEG) -Wl -bMAINSEG=0x0000
endif

CFLAGS += --std-c99 
#CFLAGS += --peep-file opt-rules.txt
MFLAGS = --model-small 
CFLAGS+= -DFLASH_START=$(FW_START) -DHIGH_SEG=$(HIGH_SEG)U

all: gboot.bin

OBJECTS = gboot_main.rel \
	  gboot_report_handlers.rel \
	  gboot_usb_descriptor.rel \
	  gboot_usb_core.rel \
	  gboot_crtstart.rel

gboot.bin: gboot.hex
	srec_cat -Disable_Sequence_Warnings gboot.hex -Intel -Output gboot.bin -Binary 

gboot.hex: $(OBJECTS)
	$(CC) -o gboot.out $(MFLAGS) $(LFLAGS) $(OBJECTS)
	srec_cat -Disable_Sequence_Warnings gboot.out -Intel -Output gboot.hex -Intel
	scripts/set_mcu gboot.hex $(MCU)
	scripts/set_pid gboot.hex $(PROD_PID)
	scripts/set_ver gboot.hex $(PROD_REV)
	scripts/set_pcbrev gboot.hex $(PROD_PCB)
	scripts/set_ext_info gboot.hex $(EXT_INFO)
	scripts/set_serial gboot.hex $(PROD_SERIAL)
	scripts/set_config gboot.hex $(PROD_CONFIG)
	cat gboot.mem

clean:
	rm -f core *~ \#* *.cdb *.asm *.rel *.hex *.lst *.map *.omf *.lk tmp.out \
	    *.rst *.sym *.lnk *.lib *.bin *.adb *.mem *.#* gboot.bin gboot.out

distclean:
	make clean
	rm -rf bin	

bin_copy:
	make clean
	make
	cp gboot.bin bin/gboot_$(MCU)_$(FLASH_SIZE)k_$(BUILD).bin
	cp gboot.hex bin/gboot_$(MCU)_$(FLASH_SIZE)k_$(BUILD).hex
	cp gboot.mem bin/gboot_$(MCU)_$(FLASH_SIZE)k_$(BUILD).mem
	cp gboot.map bin/gboot_$(MCU)_$(FLASH_SIZE)k_$(BUILD).map
    

release:
	mkdir -p bin
	# configurations for F32x
	make bin_copy BUILD=std  MCU=0xF320 FLASH_SIZE=16 VARIANT=basic
	make bin_copy BUILD=tiny MCU=0xF320 FLASH_SIZE=16 VARIANT=basic
	# configurations for F34x
	make bin_copy BUILD=std  MCU=0xF341 FLASH_SIZE=32 VARIANT=dual_uart
	make bin_copy BUILD=tiny MCU=0xF341 FLASH_SIZE=32 VARIANT=dual_uart
	make bin_copy BUILD=std  MCU=0xF340 FLASH_SIZE=64 VARIANT=dual_uart
	make bin_copy BUILD=tiny MCU=0xF340 FLASH_SIZE=64 VARIANT=dual_uart
	make bin_copy BUILD=std  MCU=0xF343 FLASH_SIZE=32 VARIANT=basic
	make bin_copy BUILD=tiny MCU=0xF343 FLASH_SIZE=32 VARIANT=basic
	make bin_copy BUILD=std  MCU=0xF342 FLASH_SIZE=64 VARIANT=basic
	make bin_copy BUILD=tiny MCU=0xF342 FLASH_SIZE=64 VARIANT=basic
	# configurations for F38x
	make bin_copy BUILD=std  MCU=0xF382 FLASH_SIZE=32 VARIANT=F38x
	make bin_copy BUILD=tiny MCU=0xF382 FLASH_SIZE=32 VARIANT=F38x
	make bin_copy BUILD=std  MCU=0xF380 FLASH_SIZE=64 VARIANT=F38x
	make bin_copy BUILD=tiny MCU=0xF380 FLASH_SIZE=64 VARIANT=F38x

	make clean


%.rel: %.c *.h
	$(CC) -c $(CFLAGS) $(JMP_SIZE) $(MFLAGS) $<

%.rel: %.a51
	$(AS) -jxlosp -Ivariants/$(VARIANT) $<
