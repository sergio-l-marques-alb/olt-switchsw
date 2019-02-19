/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  sysapi_cfd.c
*
* @purpose   Provide system-wide support routines
*
* @component sysapi
*
* @create    10/05/2005
*
* @author    bradyr
* @end
*
*********************************************************************/
#include <string.h>                /* for memcpy() etc... */
#include <stdarg.h>                /* for va_start, etc... */
#include "l7_common.h"

extern void GenCrcTable (void);
extern void UpdateCRC (L7_uchar8 DataByte, unsigned short * crc);
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
L7_RC_t sysapiCheckCfDCRC (L7_ushort16 expected_crc,
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

}  /* end: sysapiCheckConfigDataCRC */

