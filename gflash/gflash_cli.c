/**********************************************************************
 * gflash - gflash_cli.c
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
#include <ctype.h>
#include <libusb-1.0/libusb.h>

#include "gboot/gboot_usb_interface.h"
#include "gflash_lib.h"

static struct libusb_device_handle *devh = NULL;


static void print_data(int addr, unsigned char *p, int len)
{
  int j;
 printf("%04x: ",addr);
  for(j=0;j<len;j++) printf("%02x ", ((unsigned char)p[j]) );


  printf("    ");
  for(j=0;j<len;j++) {
    if(isprint(p[j]))
       printf("%c", ((unsigned char)p[j]) );
    else printf(".");
  }
  printf("\n");

}

static int print_data_hex(unsigned int addr)
{
  unsigned char buf[CMD_PACKET_SIZE*2];

  gboot_cmd_read(devh, addr, buf);
  gboot_cmd_read(devh, addr+4, buf+4);
  
  print_data(addr, buf, CMD_PACKET_SIZE*2);
   return 0;
}

static int print_info()
{
  struct gboot_info ginfo;

  //  int user_flash_size;
  //  int gboot_version;
  //  unsigned char buf[CMD_PACKET_SIZE];
  gboot_get_info(devh, &ginfo);

    //  gboot_cmd_info(devh, CMD_INFO_GBOOT, buf);
  //  print_data(0, buf, CMD_PACKET_SIZE);
    printf("GBOOT version: %x.%x\n", (ginfo.gboot_version)>>8, 
	   (ginfo.gboot_version)&0xff );

  printf("API supported: %x\n",ginfo.api_ver);
  printf("MCU: %04X\n",ginfo.mcu);

  //  gboot_cmd_info(devh, CMD_INFO_FLASH, buf);
  //  print_data(0, buf, CMD_PACKET_SIZE);
  printf("flash start: 0x%04x\n",ginfo.flash_start);
  printf("flash end:   0x%04x\n",ginfo.flash_end);
  printf("flash erase size: %d\n",ginfo.flash_erase_size);


  //  gboot_cmd_info(devh, CMD_INFO_DEV, buf);
  printf("Target PID: %04x\n",ginfo.usb_pid);
  printf("HW rev: %x.%x\n",(ginfo.hw_ver)>>4,(ginfo.hw_ver)&0xf);

  if(ginfo.gboot_version>=0x020) {
    printf("PCB rev: %X%02d\n",(ginfo.pcb_rev)>>4,(ginfo.pcb_rev)&0xf);

    //    gboot_cmd_info(devh, CMD_INFO_SERIAL, buf);
    printf("Serial: %08x\n",ginfo.serial);
    //    gboot_cmd_info(devh, CMD_INFO_FEAT, buf);
    printf("Features: %08x\n",ginfo.dev_config);
    printf("Ext info: %08x\n",ginfo.ext_info);
  }

  printf("\nMax code size: %5.1fk (0x%02x bytes)\n", ginfo.user_flash_size/1024., ginfo.user_flash_size);

   return 0;
}

static int program (char *fname)
{
  FILE *f;
  int i,s;
  unsigned char fw[16*1024];
  //  int tiny;

  f=fopen(fname,"rb");
  if(f==NULL) {
    printf("file open error\n");
    return 1;
  }
  s=fread(fw, 1, 16*1024, f);
  fclose(f);

  printf("firmware %s size: %d\n",fname,s);

  // int page_size,fstart,fend,fsize;
  unsigned char buf[CMD_PACKET_SIZE];
  struct gboot_info ginfo;

  gboot_get_info(devh,&ginfo);

  //  gboot_cmd_info(devh,CMD_INFO_FLASH,buf);

  //  page_size=1<<(buf[2]&0x0f);

  //  fstart=buf[0]<<8;
  //  fend=0xff+(buf[1]<<8);
  //  fsize=1+fend-fstart;


  if (s>ginfo.user_flash_size) {
    printf("file is too big size=%d flash size=%d\n",s,ginfo.user_flash_size);
    return 2;
  }

  printf("erasing flash ");
  // erase no more than necessary to fit the fw
  for(i=0;i<s;i+=ginfo.flash_erase_size) {
    gboot_cmd_erase(devh, ginfo.flash_start + i );
    printf(".");
    fflush(NULL);
  }

  printf("\n");

  printf("programming flash ");
  for(i=0;i<s;i++) {
    gboot_cmd_write(devh, ginfo.flash_start+i,fw[i]);
    if((i&0x1ff)==0x1ff) {
      printf(".");
      fflush(NULL);
    }
    }
  printf("\n");

  //  if(!tiny) {
    printf("verifying flash ");
    for(i=0;i<s;i+=4) {
      int j;
      gboot_cmd_read(devh, ginfo.flash_start + i, buf);
      
      for(j=0; (j<4) && (i+j<s) ;j++) {
	if(buf[j]!=fw[i+j]) {
	  printf("error at offset %04x\n",ginfo.flash_start+i);
	  printf("expected: %02x  read: %02x",fw[i+j],buf[j]);
	  return 3;
	}
      }
      if((i&0x1ff)==0x1fc) {
	printf(".");
	fflush(NULL);
      }
    }
    printf("\n");
    //  }

  return 0;
}

static void print_usage()
{
  printf("usage:\n");
  printf("gflash_cli i     print flash info\n");
  printf("gflash_cli m     dump all memory\n");
  printf("gflash_cli r xxxx  read mem at location xxxx\n");
  printf("gflash_cli e xxxx  erase page containing location xxxx\n");
  printf("gflash_cli w xx yy write byte at xxxx with yy\n");
  printf("gflash_cli p file  program the file into the mcu\n");
  printf("gflash_cli b       reboot\n");
}

int main(int argc, char *argv[])
{
  
  int r = 1;
  
  r = libusb_init(NULL);
  if (r < 0) {
    fprintf(stderr, "Failed to initialize libusb\n");
    exit(1);
  }
//    libusb_set_debug(NULL,7);
  
  if(argc>1) {
    devh = gboot_open();
    if (devh==NULL) {
      fprintf(stderr, "Could not find/open gboot device\n");
      goto out;
    }
    //	printf("Successfully found GBOOT\n");
    
    int i;
    
    if(argc==2 && *argv[1]=='m') { 
      for(i=0;i<0x3e00;i+=8) {
	r=print_data_hex(i);
      }
    } else if(argc==2 && *argv[1]=='i') {
      r=print_info();
    } else if(argc==2 && *argv[1]=='b') {
      r=gboot_cmd_reboot(devh);
    } else if(argc==3 && *argv[1]=='r') {
      r=print_data_hex(atoi(argv[2]));
    } else if(argc==3 && *argv[1]=='p') {
      r=program(argv[2]);
    } else if(argc==3 && *argv[1]=='e') {
      r=gboot_cmd_erase(devh, atoi(argv[2]));
    } else if(argc==4 && *argv[1]=='w') {
      r=gboot_cmd_write(devh, atoi(argv[2]),atoi(argv[3]));
    } else {
      print_usage();
    }
    
  out:
    gboot_close(devh);
  } else print_usage();

  libusb_exit(NULL);
  return r >= 0 ? r : -r;
}
