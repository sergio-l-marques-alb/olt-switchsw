/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename  bspcpu_api.h
*
* @purpose   Define function prototypes that should be implemented for
*            eac BSP.
*
* @component bsp
*
* @create    06/10/2005
*
* @author    Brady Rutherford
*
* @end
*
*********************************************************************/
#ifndef BSPCPU_API_H
#define BSPCPU_API_H

#include "l7_common.h"

/**************************************************************************
* @purpose  Perform all hardware specific initialization for a CPU complex.
*
* @param    none
*
* @returns  L7_SUCCESS - CPU initialization completed.
* @returns  L7_FAILURE - Problem detected in CPU initialization.
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t bspCpuInit(void);


/**************************************************************************
* @purpose  Read the CPU's serial number. If the serial number cannot be read,
*           do not update serialString.
*
* @param    serialString - pointer to serial number string
*
* @returns  L7_SUCCESS - serial number read.
* @returns  L7_FAILURE - Unable to read the serial number.
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t bspCpuReadSerialNum(L7_char8 *serialString);
/**************************************************************************
* @purpose  Read the Board's hardware version. 
*
* @param    verString - pointer to hardware version string
*
* @returns  L7_SUCCESS - hardware version read.
* @returns  L7_FAILURE - Unable to read the hardware version.
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t bspCpuReadHwVersion(L7_char8 *verString);

/**************************************************************************
* @purpose  Read the CPU's service Tag
*
* @param    serviceString - pointer to serial number string
*
* @returns  L7_SUCCESS - serial number read.
* @returns  L7_FAILURE - Unable to read the serial number.
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t bspCpuReadServiceTag(L7_char8 *serviceString);

/**************************************************************************
* @purpose  Read the CPU's asset Tag
*
* @param    assetString - pointer to serial number string
*
* @returns  L7_SUCCESS - serial number read.
* @returns  L7_FAILURE - Unable to read the serial number.
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t bspCpuReadAssetTag(L7_char8 *assetString);

/**************************************************************************
* @purpose  Write the CPU's asset Tag
*
* @param    assetString - pointer to serial number string
*
* @returns  L7_SUCCESS - serial number read.
* @returns  L7_FAILURE - Unable to read the serial number.
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t bspCpuWriteAssetTag(L7_char8 *assetString);

/**************************************************************************
* @purpose  Read the CPU's MAC address. If MAC address cannot be read,
*           do not update macString.
*
* @param    mac - pointer to MAC address string
* @param    macCount - pointer to MAC address count
*
* @returns  L7_SUCCESS - MAC address read.
* @returns  L7_FAILURE - Unable to read the MAC address
*
* @comments none
*
* @end
**************************************************************************/
L7_RC_t bspCpuReadMac(L7_char8 *mac, L7_uchar8 *macCount);
/**************************************************************************
* @purpose  Read the Boards identifier.
*
* @param    board_id - pointer to board identifier value
*
* @returns  L7_SUCCESS - board identifier read.
* @returns  L7_FAILURE - Unable to read the board identifier.
*
* @comments
*
* @end
**************************************************************************/
L7_RC_t bspCpuReadBoardID(L7_char8 *board_id);
/*********************************************************************
* @purpose  Sets the stack ID LEDs
* 
* @comments 
* 
* @end
*********************************************************************/
void sysStackLedSet(L7_BOOL master, L7_uint32 unit_number);

/**************************************************************************
* @purpose  Returns the card index of the slot requested
*
* @param    slotNum @b{(input)} requested slot ID
*
* @returns  card index of slot requested. (-1 if no card present)
*
* @end
**************************************************************************/
int bspCpuReadCardID(L7_uint32 slotNum);

/*******************************************************************************
** NAME: logNvStore 
**
**  CPU-specific function to store log entry in NVRAM.
**
*******************************************************************************/
void logNvStore(L7_uchar8 *buf);

/*******************************************************************************
** NAME: logNvRetrieve 
**
**  CPU-specific function to retrieve log entry from NVRAM.
**  If BSP can't get the data then the buffer should not be touched.
**
*******************************************************************************/
void logNvRetrieve(L7_uchar8 *buf);

void GenCrcTable ( void );
void UpdateCRC (unsigned char DataByte, unsigned short * crc);

/**************************************************************************
*
* @purpose  Display VPD information.
*
* @param    none
*
* @returns  none
*
* @comments none
*
* @end
*
*************************************************************************/
void print_vpd_in_flash ();

void reformatFileSystem(void);
#endif

