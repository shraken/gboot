/**********************************************************************
 * gflash - gmod.c
 * gboot binary image editor
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


unsigned char fwbuf[16*1024];


static int print_info()
{
  int user_flash_size;

  printf("GBOOT version: %x.%x\n",fwbuf[0x07]&0x0f,fwbuf[0x06]);
  printf("API supported: %x\n",fwbuf[0x07]>>4);
  printf("MCU: %02X%02X\n",fwbuf[0x08],fwbuf[0x09]);


  //  print_data(0, buf, CMD_PACKET_SIZE);
  printf("flash start: 0x%04x\n",fwbuf[0x0e]<<8);
  printf("flash end:   0x%04x\n",(fwbuf[0x0f]<<8)+0xff);
  printf("flash erase size: %d\n",1<<(fwbuf[0x10]&0xf));

  user_flash_size=(fwbuf[0x0f]-fwbuf[0x0e]+1)*256;

  //  print_data(0, buf, CMD_PACKET_SIZE);
  printf("Target PID: %02x%02x\n",fwbuf[0x17],fwbuf[0x16]);
  printf("HW rev: %x.%x\n",fwbuf[0x18]>>4,fwbuf[0x18]&0xf);
  printf("PCB rev: %X%02d\n",fwbuf[0x19]>>4,fwbuf[0x19]&0xf);

  printf("serial: %02x%02x%02x%02x\n",fwbuf[0x26],fwbuf[0x27],
	 fwbuf[0x28],fwbuf[0x29]);

  printf("feature: %02x%02x%02x%02x\n",fwbuf[0x2e],fwbuf[0x2f],
	 fwbuf[0x30],fwbuf[0x31]);

	 
  //  gboot_cmd_info(devh, CMD_INFO_EXT, buf);
  //  print_data(0, buf, CMD_PACKET_SIZE);

  printf("\nMax code size: %5.1fk (0x%02x bytes)\n", user_flash_size/1024., user_flash_size);

  return 0; 
   }

static int load_image (char *fname)
{
  FILE *f;
  int i,s;
  //  int tiny;

  f=fopen(fname,"rb");
  if(f==NULL) {
    printf("file open error\n");
    return 0;
  }
  s=fread(fwbuf, 1, 16*1024, f);
  fclose(f);

  printf("firmware %s size: %d\n",fname,s);

  return s;
}

static void save_image (char *fname, int s)
{
  FILE *f;

  f=fopen(fname,"wb");
  if(f==NULL) {
    printf("file open error\n");
    return 0;
  }
  fwrite(fwbuf, 1, s, f);
  fclose(f);

}
/*
  int page_size,fstart,fend,fsize;
  unsigned char buf[CMD_PACKET_SIZE];

  gboot_cmd_info(devh,CMD_INFO_FLASH,buf);

  page_size=1<<(buf[2]&0x0f);

  fstart=buf[0]<<8;
  fend=0xff+(buf[1]<<8);
  fsize=1+fend-fstart;


  if (s>fsize) {
    printf("file is too big size=%d flash size=%d\n",s,fsize);
    return 2;
  }

  printf("erasing flash ");
  // erase no more than necessary to fit the fw
  for(i=0;i<s;i+=page_size) {
    gboot_cmd_erase(devh, i+fstart);
    printf(".");
    fflush(NULL);
  }

  printf("\n");

  printf("programming flash ");
  for(i=0;i<s;i++) {
    gboot_cmd_write(devh, fstart+i,fw[i]);
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
      gboot_cmd_read(devh, i+fstart, buf);
      
      for(j=0; (j<4) && (i+j<s) ;j++) {
	if(buf[j]!=fw[i+j]) {
	  printf("error at offset %04x\n",i+fstart);
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
  }*/

static void print_usage()
{
  printf("usage:\n");
  printf("gmod file i     print gboot image info\n");
  printf("gmod file p xxxx  change image usb pid\n");
  printf("gmod file h xx    change image hw revision\n");
  printf("gmod file b xx    change image pcb revision\n");
  printf("gmod file s xxxxxxxx change image serial number\n");
  printf("gmod file f xxxxxxxx change image feature set\n");
}



int main(int argc, char *argv[])
{
  
  int s = 0;
  
  if(argc>1) {
    s=load_image(argv[1]);
    if (s==0) {
      fprintf(stderr, "Could not open image file\n");
      return -1;
    }
    //	printf("Successfully found GBOOT\n");
    
    int i;
    
    if(argc==3 && *argv[2]=='i') { 
      print_info();
    } else if(argc==4) {
      int par=strtol(argv[3],NULL,16);
      switch(*argv[2]) {
      case 'p':
	fwbuf[0x16]=par&0xff;
	fwbuf[0x17]=(par&0xff00)>>8;
	break;
      case 'h':
	fwbuf[0x18]=par;
	break;
      case 'b':
	fwbuf[0x19]=par;
	break;
      case 's':
	// no endian reversal it's bcd
	fwbuf[0x26]=(par&0xff000000)>>24;
	fwbuf[0x27]=(par&0x00ff0000)>>16;
	fwbuf[0x28]=(par&0x0000ff00)>>8;
	fwbuf[0x29]=(par&0x000000ff);
	break;
      case 'f':
	// no endian reversal it's bcd
	fwbuf[0x2e]=(par&0xff000000)>>24;
	fwbuf[0x2f]=(par&0x00ff0000)>>16;
	fwbuf[0x30]=(par&0x0000ff00)>>8;
	fwbuf[0x31]=(par&0x000000ff);
	break;
      default:
	printf("undefined command\n");
	return -1;
      }
      save_image(argv[1],s);
    } else {
      print_usage();
    }
    

  } else print_usage();

}
