BUILD=std

JLINK_DEVICE_TYPE=EFM8UB20F64G

MCU=F380
PROD_PID=ffff
PROD_REV=10
PROD_PCB=A0
# variant can be one of: basic, dual_uart, F38x
VARIANT=basic


EXT_INFO=ffffffff
PROD_SERIAL=ffffffff
PROD_CONFIG=ffffffff

FLASH_SIZE=16
FW_START=0x0400

CC = sdcc
AS = sdas8051

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

BUILD_DIR = build
GBOOT_C_SRCS=$(wildcard ./*.c)
GBOOT_C_OBJS=$(GBOOT_C_SRCS:./%.c=./$(BUILD_DIR)/%.rel)

GBOOT_ASM_SRCS=$(wildcard ./*.a51)
GBOOT_ASM_OBJS=$(GBOOT_ASM_SRCS:./%.a51=./$(BUILD_DIR)/%.rel)

all: gboot.bin

directories:
	mkdir -p build
#ifeq ($(OS),Windows_NT)
#	if not exist "build" mkdir build
#else
#	mkdir -p build
#endif

gboot.bin: gboot.hex
	srec_cat -Disable_Sequence_Warnings $(BUILD_DIR)/gboot.hex -Intel -Output $(BUILD_DIR)/gboot.bin -Binary 

gboot.hex: directories $(GBOOT_C_OBJS) $(GBOOT_ASM_OBJS)
	$(CC) $(MFLAGS) $(LFLAGS) $(GBOOT_C_OBJS) $(GBOOT_ASM_OBJS) -o $(BUILD_DIR)/gboot.ihx
	packihx $(BUILD_DIR)/gboot.ihx > $(BUILD_DIR)/gboot.hex
	python scripts/set_gboot_params.py -f $(BUILD_DIR)/gboot.hex -m $(MCU) -p $(PROD_PID) -e $(EXT_INFO) -r $(PROD_PCB) -s $(PROD_SERIAL) -c $(PROD_CONFIG) -v $(PROD_REV)
	cat $(BUILD_DIR)/gboot.mem

clean:
	rm -rf build
	rm -rf flash.jlink

distclean:
	make clean
	rm -rf bin	

bin_copy:
	make clean
	make
	cp $(BUILD_DIR)/gboot.bin bin/gboot_$(MCU)_$(FLASH_SIZE)k_$(BUILD).bin
	cp $(BUILD_DIR)/gboot.hex bin/gboot_$(MCU)_$(FLASH_SIZE)k_$(BUILD).hex
	cp $(BUILD_DIR)/gboot.mem bin/gboot_$(MCU)_$(FLASH_SIZE)k_$(BUILD).mem
	cp $(BUILD_DIR)/gboot.map bin/gboot_$(MCU)_$(FLASH_SIZE)k_$(BUILD).map
    

release: distclean
ifeq ($(OS),Windows_NT)
	mkdir bin
else
	mkdir -p bin
endif
	make bin_copy BUILD=std  MCU=0xF320 FLASH_SIZE=16 VARIANT=basic
	make bin_copy BUILD=tiny MCU=0xF320 FLASH_SIZE=16 VARIANT=basic
	make bin_copy BUILD=std  MCU=0xF341 FLASH_SIZE=32 VARIANT=dual_uart
	make bin_copy BUILD=tiny MCU=0xF341 FLASH_SIZE=32 VARIANT=dual_uart
	make bin_copy BUILD=std  MCU=0xF340 FLASH_SIZE=64 VARIANT=dual_uart
	make bin_copy BUILD=tiny MCU=0xF340 FLASH_SIZE=64 VARIANT=dual_uart
	make bin_copy BUILD=std  MCU=0xF343 FLASH_SIZE=32 VARIANT=basic
	make bin_copy BUILD=tiny MCU=0xF343 FLASH_SIZE=32 VARIANT=basic
	make bin_copy BUILD=std  MCU=0xF342 FLASH_SIZE=64 VARIANT=basic
	make bin_copy BUILD=tiny MCU=0xF342 FLASH_SIZE=64 VARIANT=basic
	make bin_copy BUILD=std  MCU=0xF382 FLASH_SIZE=32 VARIANT=F38x
	make bin_copy BUILD=tiny MCU=0xF382 FLASH_SIZE=32 VARIANT=F38x
	make bin_copy BUILD=std  MCU=0xF380 FLASH_SIZE=64 VARIANT=F38x
	make bin_copy BUILD=tiny MCU=0xF380 FLASH_SIZE=64 VARIANT=F38x

	make clean


$(BUILD_DIR)/%.rel: %.c *.h
	$(CC) -c $(CFLAGS) $(JMP_SIZE) $(MFLAGS) $< -o $(BUILD_DIR)/

$(BUILD_DIR)/%.rel: %.a51
	$(AS) -jxlspo -Ivariants/$(VARIANT) $@ $< 

JLINK_OPTS = -device $(JLINK_DEVICE_TYPE) -if c2 -speed 4000
# flashing windows vs. posix
ifeq ($(OS),Windows_NT)
JLINK = JLink $(JLINK_OPTS)
else
JLINK = JLinkExe $(JLINK_OPTS)
endif # flashing windows vs. posix

flash: flash.jlink
	$(JLINK) flash.jlink

flash.jlink:
	printf "erase\nloadbin build/gboot.hex,0\nexit\n" > flash.jlink