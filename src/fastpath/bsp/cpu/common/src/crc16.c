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

#ifdef _L7_OS_VXWORKS_
  #include <taskLib.h>
  #include <intLib.h>
#endif

#include "l7_common.h"

#define POLYNOMIAL 0xA097  /* Use 0x8005 for CRC16, 0x1021 for SDLC/HDLC */

#define MAX_BUF_SIZE 4096

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
  if(0 > read(filedesc, &c, 1)){}
  if(0 > read(filedesc, &c, 1)){}
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

/**************************************************************************
 *
 * @purpose  Calculates the 16 bit CRC of a file in a buffer.
 *
 * @param    address starting location  to check
 * @param    size    size, in bytes, to check
 *
 * @returns  16-bit crc
 *
 * @comments none.
 *
 * @end
 *
 *************************************************************************/
L7_ushort16 bspapiCalcCrc(L7_uint32 address, L7_uint32 size)
{
  L7_ushort16 crc;
  L7_uchar8   *next_address;
  L7_int32 i;

  GenCrcTable ();
  crc = 0;
  next_address=(L7_uchar8 *)address;
  for ( i = 0; i < size; i++ )
  {
    if ( !(i % 0x10000) )
    {
#ifdef _L7_OS_VXWORKS_
      if ( intContext() == FALSE )
      {
        /* Allow tasks with the same priority to run. */
        taskDelay (0);
      }
#endif
    }
    UpdateCRC (*next_address, &crc);
    next_address++;
  };
  return(crc);
}

/**************************************************************************
*
* @purpose  Check the expected CRC against the calculated CRC to verify I2C read
*
* @param    expected_crc    What was read from the CFD
* @param    *cfd_start      Ptr to start of CFD data to check
*                                      This must NOT include the first 2 CRC bytes!
* @param    cfd_size        Number of bytes to calculate
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if CRC is bad
*
* @notes
*
* @end
*
*************************************************************************/
L7_RC_t bspapiCheckCfDCRC (L7_ushort16 expected_crc,
                           L7_uchar8 *cfd_start,
                           L7_ushort16 cfd_size)
{

  register L7_ushort16 i;
  unsigned short computed_crc;


  computed_crc = 0;

  GenCrcTable ();
  for ( i = 0; i < cfd_size; i++ )
  {

    UpdateCRC (*cfd_start, &computed_crc);
    cfd_start++;

  }

  if ( computed_crc == expected_crc )
    return(L7_SUCCESS);

  return(L7_ERROR);

}  /* end: bspapiCheckConfigDataCRC */

