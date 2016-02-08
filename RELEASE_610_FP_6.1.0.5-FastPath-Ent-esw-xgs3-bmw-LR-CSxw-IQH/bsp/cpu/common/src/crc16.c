/***************************************************************************
**
**  MODULE : crc16.c
**                     
**  FUNCTION :        
** This file contains routines to perform byte-at-a-time CRC computation.
**
** The program is derived from pascal program written by Donald R. Blake.
**                                                                  
**  CHANGE SUMMARY :                                               
**    date   SID  Init    Action                                  
**   ----------------------------                                
**   22oct93 1.1  ALT     Created file.                         
**                                                            
***************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "l7_common.h"

#define POLYNOMIAL 0xA097  /* Use 0x8005 for CRC16, 0x1021 for SDLC/HDLC */

#define MAX_BUF_SIZE 128

static unsigned short CrcTable[256]; /* CRC lookup table */

/************************************************************************
** NAME: UpdateCRC
**
** This function updates existing CRC 
**
** INPUT:
**	DataByte - Received data byte
**	
** INPUT/OUTPUT:
**	crc - Current value of CRC.
************************************************************************/
extern void UpdateCRC (unsigned char DataByte, unsigned short *crc)
{
 register unsigned long new_crc;

 new_crc = *crc;
 new_crc = (new_crc << 8) ^ (CrcTable[new_crc >> 8] ^ DataByte);
 *crc = (unsigned short) new_crc;
};


/************************************************************************
** NAME: GenCrcTable
**
** This function computes CRC look-up table.
************************************************************************/
extern void GenCrcTable (void)
{
 unsigned short CrcCode;
 unsigned char DataByte;
 unsigned long  index, BitNo;
 unsigned long  CarryBit, DataBit;

 for (index = 0; index <= 255; index++) {
   CrcCode = 0;
   DataByte = index;

   for (BitNo = 1; BitNo <= 8; BitNo++) {
	CarryBit = ((CrcCode & 0x8000) != 0);
	DataBit = ((DataByte & 0x80) != 0);
	CrcCode = CrcCode << 1;
	if (CarryBit ^ DataBit) CrcCode = CrcCode ^ POLYNOMIAL;
	DataByte = DataByte << 1;
     }; /* End For */
   CrcTable [index] = CrcCode;
  }; /* End for */
};

/**************************************************************************
 *
 * @purpose  This routine computes the CRC of a file in the file system.
 *
 * @param    file_name Name of file.
 * @param    file_size Number of bytes expected in the file.
 *
 * @returns  16-bit crc
 *
 * @comments none.
 *
 * @end
 *
 *************************************************************************/
L7_ushort16 file_crc_compute(L7_uchar8 * file_name, L7_uint32 file_size)
{
  L7_int32  filedesc;
  L7_ushort16 computed_crc;
  L7_uchar8 c;
  L7_uint32 num_chars;
  L7_uint32 delay_count = 0;
  L7_uchar8 buf [MAX_BUF_SIZE];
  L7_int32 buf_size;
  L7_int32 i;

  GenCrcTable ();

  /* Compute CRC.
   */
  filedesc = open(file_name, O_RDONLY, 0);
  if ( filedesc == -1 )
  {
    return(0);
  }
  /* Skip the first two bytes of the image. The two bytes contain
  ** CRC.
  */
  (void)read(filedesc, &c, 1);
  (void)read(filedesc, &c, 1);
  computed_crc = 0;
  num_chars = 2;
  do
  {
    /* We need to take a little break every 1000th read in order to let
    ** other tasks run.
    */
    if ( !(delay_count++ % 1000) )
      sleep(1);

    buf_size = read(filedesc, buf, MAX_BUF_SIZE);
    if (buf_size == -1)
    {
      printf("Read error\n");
      buf_size = 0;
    }

    for ( i = 0; ((i < buf_size) && (num_chars < file_size)); i++ )
    {
      c = buf[i];
      num_chars++;
      UpdateCRC (c, &computed_crc);
    }

  } while ( (num_chars < file_size) && (buf_size == MAX_BUF_SIZE) );
  (void)close(filedesc);
  return(computed_crc);
}
