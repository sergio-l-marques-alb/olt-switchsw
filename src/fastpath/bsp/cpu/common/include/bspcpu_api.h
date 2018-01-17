/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
*
* @filename  bspcpu_api.h
*
* @purpose   Define function prototypes that should be implemented for
*            each BSP.
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

/* Name of the SDM template file. Use .cfg file extension so 
 * boot menu utilities will automatically remove the file when
 * config files are deleted. */
#define SDM_TEMPLATE_FILENAME  "sdm_template.cfg"

/* Definition of SDM template file that persistently stores the 
 * next active SDM template ID. */
#define SDM_TEMPLATE_FILE_VERSION  1
typedef struct
{
  L7_uint32               version;
  L7_uint32               sdm_template_id;
  L7_ushort16             pad;
  L7_ushort16             crc;
} bspSdmTemplateData_t;

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

/**************************************************************************
*
* @purpose  Read the version information of the bootrom.
*
* @param    bootCodeVersion - pointer to store version information
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @end
*
*************************************************************************/
L7_RC_t bspCpuBootVersionGet(L7_uint32 *bootCodeVersion);



/*********************************************************************
* @purpose  Write a file to the flash file system.
*
* @param    filename @b{(input)}  name of the file to be written
* @param    data     @b{(input)}  pointer to start of data to be written to file
* @param    length   @b{(input)}  number of bytes to be written
*
* @returns  L7_SUCCESS
*           L7_ERROR if file save fails
*
* @notes    If file does not already exist, it is created.
*
* @end
*********************************************************************/
L7_RC_t bspCpuFsWrite(L7_uchar8 *filename, L7_char8* data, L7_uint32 length);

/**************************************************************************
 *
 * @purpose  Read the contents of a file
 *
 * @param    filename  @b{(input)}  File from which to read data
 * @param    buffer    @b{(input)}  where to put data
 * @param    nbytes    @b{(input)}  amount of data to read
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments    This function opens the file, reads data, and closes the file
 *
 * @end
 *
 *************************************************************************/
L7_RC_t bspCpuFsRead(L7_char8 *filename, L7_char8 *buffer, L7_int32 nbytes);

/**************************************************************************
 *
 * @purpose  Delete a file from the flash file system.
 *
 * @param    filename  @b{(input)}  Name of file to delete
 *
 * @returns  L7_SUCCESS
 * @returns  L7_ERROR
 *
 * @comments    
 *
 * @end
 *
 *************************************************************************/
L7_RC_t bspCpuFsDeleteFile(L7_char8 *filename);

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

