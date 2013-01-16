#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>

#include "l7_common.h"


#define NUM_BYTES 1024
#define SWAP32(val) \
        ((L7_uint32)( \
                (((L7_uint32)(val) & (L7_uint32)0x0000ffffUL) << 16) | \
                (((L7_uint32)(val) & (L7_uint32)0xffff0000UL) >> 16) ))

L7_RC_t osapiSwapFile(L7_uchar8 *filename) 
{
  int fd;
  L7_uint32 *dst,i,j,rbytes=0,wbytes=0,totoalbyteswritten=0;
  unsigned long size,offset;
  L7_uchar8 *buf,temp;

  buf = malloc(NUM_BYTES);
  memset ( buf, 0,NUM_BYTES);
  fd=open(filename,O_RDWR);
  if(fd==-1) {
    printf("error: cannot open input file %s",filename);
    return (L7_FAILURE);
  }
  size = lseek(fd,0,SEEK_END);
  j=size%4;
  if(j != 0)
  {
    temp =0x00;
    for ( i=j;i<4;i++ )
    {
      if((wbytes = write(fd,&temp,1)) < 1 )
      {
        printf("Unable to write to file %s",filename);
        close(fd);
        free(buf);
        return (L7_FAILURE);
      }
      size++;
    }
  }
  lseek(fd,0,SEEK_SET);
  offset = 0;
  while(offset < size)
  {
    if ((rbytes = read(fd, (L7_uchar8 *)buf, NUM_BYTES)) < 0)
    {
      printf("Unable to read from file %s",filename);
      close(fd);
      free(buf);
      return (L7_FAILURE);
    }

    dst = (L7_uint32 *)buf;

    for (i=0; i < rbytes; i += 4)
    {
      *dst = SWAP32(*dst);
      dst++;
    }
    lseek(fd,offset,SEEK_SET);
    if((wbytes = write(fd,buf,rbytes)) < rbytes )
    {
      printf("Unable to write to file %s",filename);
      close(fd);
      free(buf);
      return (L7_FAILURE);
    }
    totoalbyteswritten += wbytes;
    offset += rbytes;
  }
  close(fd);
  free(buf);
  return (L7_SUCCESS);
}

