/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
* @filename   bspapi.h
*
* @purpose    BSP API
*
* @component  bsp component
*
* @comments   none
*
* @create     05/02/2006
*
* @author     Brady Rutherford
*
* @end
*             
**********************************************************************/

#ifndef BSP_API_H
#define BSP_API_H

#include <time.h>

#include "l7_common.h"
#include "iplsec.h"
#include "stk.h"

#define L7_SERVICE_PORT_MAX_LENGTH  12
/*
 * filesystem path info
 */
#define CONFIG_PATH     "/usr/local/ptin/log/fastpath/"     /* PTin modified: paths */
#define EXEC_PATH       "/usr/local/ptin/sbin/"             /* PTin modified: paths */
#ifdef _L7_OS_VXWORKS_
  #define DOWNLOAD_PATH   "/tmp"                            /* PTin modified: paths */
  #define DOWNLOAD_CODE_FILE  "code.stk"
#else
  #define DOWNLOAD_PATH   "/tmp/"                           /* PTin modified: paths */
#endif

/* Name of the update script.
 * */
#define UPDATE_SCRIPT_NAME    "UPDATE"
#define UPDATE_KERNEL_SCRIPT_NAME     "UPDATE_KERNEL"
#define UPDATE_BOOTROM_SCRIPT_NAME    "UPDATE_BOOTCODE"
#define VPD_FILE                      "fastpath.vpd"
#define USYSTEM_FILE                  "/tmp/uSystem"  /* PTin modified: paths */
#define CODE_TIME_SIZE   32


/* Do not add any more fields in this structure. It
** breaks backwards compatibility for CDA.
*/
typedef struct
{
  L7_BOOL   valid;
  L7_uchar8 rel;
  L7_uchar8 ver;
  L7_uchar8 maint_level;
  L7_uchar8 build_num;

#if L7_FEAT_STACKING_K_COMPAT
  L7_uchar8 bootTimestamp[CODE_TIME_SIZE];
#endif

} codeVersion_t;


/**************************************************************************
*
* Serial Port parameter types.
*
* @notes For SYSAPISERIALPORT_CHARSIZE, valid values are between L7_SERIALPORT_MIN_CHAR_SIZE and L7_SERIALPORT_MAX_CHAR_SIZE.
*        For SYSAPISERIALPORT_PARITYTYPE, specify either L7_PARITY_EVEN, L7_PARITY_ODD, L7_DISABLE.
*        For SYSAPISERIALPORT_STOPBITS, specify either L7_STOP_BIT_1 or L7_STOP_BIT_2.
*
*************************************************************************/
typedef enum
{

    SYSAPISERIALPORT_CHARSIZE,
    SYSAPISERIALPORT_FLOWCONTROL,
    SYSAPISERIALPORT_PARITYTYPE,
    SYSAPISERIALPORT_STOPBITS

} SYSAPISERIALPORT_PARMS_t;


/*********************************************************************
* @purpose  Returns the IPL Model Tag for this unit
*
* @returns  IPL_MODEL_TAG
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 bspapiIplModelGet(void);

/*********************************************************************
* @purpose  Returns the Default Baud Rate for this unit
*
* @returns  L7_DEFAULT_BAUDRATE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 bspapiDefaultBaudRateGet(void);

/*********************************************************************
* @purpose  Returns the Service Port Name for this unit
*
* @returns  Service port name.
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_char8 *bspapiServicePortNameGet(void);

/*********************************************************************
* @purpose  Returns the Service Port Unit
*
* @returns  Service port name.
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_uint32 bspapiServicePortUnitGet(void);

/**************************************************************************
*
* @purpose  Retrieve a serial port setting
*
* @param    parm specifies which serial port parameter to retrieve
* @param    result ptr where to place parameter setting
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE if parm is not supported or recognized
*
* @end
*
*************************************************************************/
L7_RC_t bspapiSerialPortParmGet ( L7_uint32 parm, L7_uint32 *result );

/**************************************************************************
*
* @purpose  Enables the Hardware Watchdog
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @end
*
*************************************************************************/
L7_RC_t bspapiWatchdogEnable( void );

/**************************************************************************
*
* @purpose  Disables the Hardware Watchdog
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @end
*
*************************************************************************/
L7_RC_t bspapiWatchdogDisable( void );

/**************************************************************************
*
* @purpose  Services the Hardware Watchdog
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @end
*
*************************************************************************/
L7_RC_t bspapiWatchdogService( void );

/**************************************************************************
*
* @purpose  Retrieve the Hardware Watchdog Service Interval
*
* @param    void
*
* @returns  Service interval in milliseconds
*
* @end
*
*************************************************************************/
int bspapiWatchdogInterval( void );

/*********************************************************************
* @purpose  Dump BSP debug information
*
* @param    void
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t bspapiDebugRegisteredSysInfoDump(void);

/***************************************************************************
*
* @purpose Return file descriptor for the console port.
*
* @comments Return file descriptor for the console port.
*
* @returns file descriptor for the console port.
*
* @end
*
***************************************************************************/
int bspapiConsoleFdGet(void);
int bspapiConsoleFdOutGet(void);

/***************************************************************************
*
* @purpose Sets file descriptor for the console port.
*
* @comments Sets file descriptor for the console port.
*
* @returns L7_SUCCESS
*
* @end
*
***************************************************************************/
L7_RC_t bspapiConsoleFdSet(int consoleFd);

/*********************************************************************
* @purpose Read time from the RTC
*
* @param    time ptr where to write time values
*
* @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
*
* @end
*********************************************************************/
L7_RC_t bspapiRTCRead(struct tm *rtcTime);

/**************************************************************************
*
* @purpose  Crash box with a given task ID.
*
* @param    taskID - task to save context for.
*
* @returns  Does not return
*
* @end
*
*************************************************************************/
void bspapiCrash( int taskID );

/**************************************************************************
*
* @purpose  Reset the switch
*
* @param    none.
*
* @returns  none.
*
* @end
*
*************************************************************************/
void bspapiSwitchReset (void);

/***************************************************************************
*
* @purpose Returns the Board ID for the CPU.
*
* @param    none.
*
* @returns Board ID
*
* @end
*
***************************************************************************/
/* Physical CPU cards */
#define CARD_CPU_BMW_REV_1_ID               0x82450000
#define CARD_CPU_DNI_REV_1_ID               0x85410000
#define CARD_CPU_ALPHA_REV_1_ID             0x82A10000
#define CARD_CPU_RAPTOR_REV_1_ID            0x56218000
#define CARD_CPU_CFM_REV_1_ID               0x11250000
#define CARD_CPU_NSX_REV_1_ID               0x11250000
#define CARD_CPU_LM_REV_1_ID                0x47040000
#define CARD_CPU_JAG_REV_1_ID               0x47040000
#define CARD_CPU_LINE_REV_1_ID              0x17010000
#define CARD_CPU_GTX_REV_1_ID               0x11250002
#define CARD_CPU_GTO_REV_1_ID               0x85480000
#define CARD_CPU_KEYSTONE_REV_1_ID          0x53003000
#define CARD_CPU_CN52XX_REV_1_ID            0x52300000

L7_uint32 bspapiCpuBoardIdGet(void);

#define START_OPR_CODE                  1
#define START_BOOT_MENU                 2
#define START_DIAGS                     3
#define START_OPR_CODE_PASSWD_RECOVERY  4
#define START_VXSHELL                   5
/**************************************************************************
*
* @purpose  Returns the startType for how this code was started.
*
* @param    
*
* @returns  startType
*
* @end
*
*************************************************************************/
L7_uint32 bspapiStartTypeGet();

/*********************************************************************
 * @purpose  Updates the bootloader from the supplied Image file.
 *
 * @param    fileName Image file to fetch the bootloader from
 *
 * @returns  L7_SUCCESS on success
 *           L7_FAILURE otherwise
 *
 * @comments
 *
 * @end
 *********************************************************************/
L7_uint32 bspapiBootCodeUpdate(char *fileName);

/**************************************************************************
*
* @purpose  Read the version information of the bootrom.
*
* @param    bootCodeVersion - pointer to store version information
* @param    bootCodeVerSize - size of version information location
s
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @end
*
*************************************************************************/
L7_RC_t bspapiBootCodeVersionRead(L7_char8 *bootCodeVersion,
                                  L7_uint32 bootCodeVerSize);

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
L7_ushort16 bspapiCalcCrc(L7_uint32 address, L7_uint32 size);

/**************************************************************************
* @purpose  Read the CPU's MAC address. If MAC address cannot be read,
*           do not update macString.
*
* @param    mac - pointer to MAC address string
*
* @returns  L7_SUCCESS - MAC address read.
* @returns  L7_FAILURE - Unable to read the MAC address
*
* @comments This function returns the local MAC address stored in 
*           non-volatile storage. Most applications should call the
*           sim API simMacAddrGet to get the system MAC address for
*           the stack.
*
* @end
**************************************************************************/
L7_RC_t bspapiMacAddrGet(L7_char8 *mac);

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
L7_RC_t bspapiCheckCfDCRC (L7_ushort16 expected_crc, L7_uchar8 *cfd_start, L7_ushort16 cfd_size);

/***************************************************************************
*
* @purpose Determine if the MAC address can be assigned to this switch
*
* @param   macAddr - pointer to MAC address
*
* @returns L7_TRUE  - Mac address is valid
* @returns L7_FALSE - Mac address is invalid
*
* @end
*
***************************************************************************/
L7_BOOL bspapiSwitchMacAddrValid(L7_char8 *macAddr);

#define DIM_MAX_FILENAME_SIZE           39 
/*********************************************************************
* @purpose  Determines if the file name is a valid image file
*           
* @param    fileName    @b{(input)}  File name to validate
* @param    imageId     @b{(input)}  Pointer to returned image ID.
*
* @returns  L7_TRUE   if filename is a valid image
* @returns  L7_FALSE  if filename is invalid 
*
* @end
*********************************************************************/
L7_BOOL bspapiImageFileNameValid(char *fileName, L7_uint32 *imageId);

/*********************************************************************
* @purpose  Retrieves the file name for the current active image 
*           
* @param    fileName    @b{(output)}  Location to copy the  
*                                     file name for the active image
*
* @returns  L7_SUCCESS  on successful execution
* @returns  L7_FAILURE  if no image is marked as active yet
*
* @notes    File name is copied.
*
* @end
*********************************************************************/
L7_RC_t bspapiActiveImageFileNameGet(char *fileName);

/*********************************************************************
* @purpose  Retrieves the file name for the current backup image 
*           
* @param    fileName    @b{(output)}  Location to copy file name of the  
*                                     backup image
*
* @returns  L7_SUCCESS  on successful execution
* @returns  L7_FAILURE  if there is no backup image
*
* @notes    File name is copied.
*
* @end
*********************************************************************/
L7_RC_t bspapiBackupImageFileNameGet(char *fileName);

/*********************************************************************
 * @purpose  Determines if the image name passed is valid and maps it
 *           to the correct file name
 *
 * @param    imageName   @b{(input)}   Image name to validate/resolve
 * @param    fileName    @b{(output)}  Location to copy name of the
 *                                     file
 *
 * @returns  L7_TRUE      if valid image name is passed
 * @returns  L7_FALSE     if image name is invalid
 *
 * @notes    File name is copied.
 *
 * @end
 *********************************************************************/
L7_BOOL bspapiImageFileNameResolve(char *imageName, char *fileName);

/*********************************************************************
* @purpose  Sets the supplied image as the active image for the 
*           subsequent re-boots
*           
* @param    imageName       @b{(input)}  image to be activated
* @param    updateBootCode  @b{(input)}  flag to command a boot code update
*
* @returns  L7_SUCCESS              on successful execution
* @returns  L7_NOT_EXIST            if the specified image could not be found
* @return   L7_ALREADY_CONFIGURED   if the backup image is already active
* @return   L7_DEPENDENCY_NOT_MET   if the bootrom cannot boot image
* @return   L7_FAILURE              on internal error
*
* @notes    Internal data structures are updated with the information.
*           Image currently active is marked as backup and the specified
*           image is marked as active.
*
* @end
*********************************************************************/
L7_RC_t bspapiImageActivate(char *fileName, L7_BOOL updateBootCode);

/*********************************************************************
* @purpose  Adds an image to the list of images 
*           
* @param    srcFileName   @b{(input)}  image file to add
* @param    dstImageName  @b{(input)}  destination image name
*
* @returns  L7_SUCCESS   on successful execution
* @returns  L7_FAILURE   if image cannot be added
* 
* @end
*********************************************************************/
L7_RC_t bspapiImageAdd(char *srcFileName, char *dstFileName);

/*********************************************************************
* @purpose  Deletes an image from the list of boot images 
*           
* @param    imageName    @b{(input)}  name of the image to be deleted
*
* @returns  L7_SUCCESS        on successful execution
* @returns  L7_NOT_EXIST      if the image could not be found.
* @returns  L7_REQUEST_DENIED if the specified image is active/activated.
*
* @notes    current active image is never deleted.
*
* @end
*********************************************************************/
L7_RC_t bspapiImageDelete(char *fileName);

/*********************************************************************
* @purpose  Checks if the backup image is activated 
*           
* @param    none
*
* @returns  L7_TRUE  if backup image is valid & activated
*           L7_FALSE otherwise
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL bspapiIsBackupImageActivated(void);


/*********************************************************************
* @purpose  Indicates if the image exists in the file system.
*
* @param    name    @b{(input)}  Name of the file containing the image
*
* @returns  L7_TRUE  when the image exists
* @returns  L7_FALSE when the image does not exist
*
* @notes
* @end
*********************************************************************/
L7_BOOL bspapiImageExists(char *name);

/*********************************************************************
* @purpose  Returns the image file name for the given ID.
*
* @param    imageId     @b{(input)}  image ID to retrieve file name
* @param    fileName    @b{(input)}  buffer to copy the file name
*
* @returns  L7_SUCCESS          on successful execution
*
* @end
*********************************************************************/
L7_RC_t bspapiImageFileNameGet(L7_uint32 imageId, L7_char8 *fileName);

/*********************************************************************
* @purpose  Associates a given text description for an image
*           
* @param    image         @b{(input)}  image name
* @param    descr         @b{(input)}  text to be associated
* 
* @returns  L7_SUCCESS   on successful execution
* @returns  L7_NOT_EXIST if the specified image could not be found
*
* @end
*********************************************************************/
L7_RC_t bspapiImageDescrSet(char *image, char *descr);

/*********************************************************************
* @purpose  retrieves the text description for an image
*           
* @param    image         @b{(input)}  image name
* @param    descr         @b{(input)}  pointer to copy the text to
* 
* @returns  L7_SUCCESS   on successful execution
* @returns  L7_NOT_EXIST if the specified image could not be found
*
* @end
*********************************************************************/
L7_RC_t bspapiImageDescrGet(char *image, char *descr);

/*********************************************************************
 * @purpose  Returns the name for the backup image
 *           
 * @param    imageName    @b{(input)}  buffer to copy the image name
 *
 * @returns  none.
 * 
 * @end
 *********************************************************************/
L7_RC_t bspapiBackupImageNameGet(char *imageName);

/*********************************************************************
 * @purpose  Returns the name for the active image
 *           
 * @param    imageName    @b{(input)}  buffer to copy the image name
 *
 * @returns  none.
 * 
 * @end
 *********************************************************************/
L7_RC_t bspapiActiveImageNameGet(char *imageName);

/*********************************************************************
* @purpose  Determines if the image name is a valid
*           
* @param    imageName   @b{(input)}  Image name to validate
*
* @returns  L7_TRUE   if image name is valid
* @returns  L7_FALSE  if image name is not supported
*
* @end
*********************************************************************/
L7_BOOL bspapiImageNameValid(char *imageName);

/**************************************************************************
 *
 * @purpose  Reads the opr header from STK & OPR files.
 *
 * @param    file_name  Operational code file on RAM disk.
 *
 * @returns  L7_SUCCESS   on successful execution
 * @returns  L7_NOT_EXIST if the specified file does not exist
 * @returns  L7_FAILURE   if the specified file is not valid
 *
 * @comments  This function understands the STK and the OPR files.
 *            STK files have the specified 'tags' in the header.
 *            this function returns the appropriate opr header from
 *            the STK file / OPR file.
 *
 * @end
 *
 *************************************************************************/
L7_RC_t bspapiOprHeaderRead(char *fileName, oprHeader_t *opr);

/*********************************************************************
* @purpose  Persistently save the next active SDM template ID.
*
* @param    templateId @b{(input)}  template ID
*
* @returns  L7_SUCCESS
*           L7_ERROR if file save fails
*
* @notes    Initial implementation saves as a file on the flash file 
*           system. Could be overridden on a platform to save somewhere
*           else.
*
* @end
*********************************************************************/
L7_RC_t bspapiNextSdmTemplateSave(L7_uint32 templateId);

/*********************************************************************
* @purpose  Retrieve the next active SDM template ID from persistent storage.
*
* @param    nextTemplateId @b{(output)} Template ID read from persistent storage
*
* @returns  L7_SUCCESS
*           L7_ERROR if file corrupted
*
* @notes    Initial implementation reads from a file on the flash file 
*           system. Could be overridden on a platform to retrieve from 
*           somewhere else.
*
* @end
*********************************************************************/
L7_RC_t bspapiNextSdmTemplateGet(L7_uint32 *nextTemplateId);

/*********************************************************************
* @purpose  Remove the next active SDM template ID from persistent storage.
*
* @param    void
*
* @returns  L7_SUCCESS
*           L7_ERROR if file save fails
*
* @notes    Default implementation deletes SDM template file.
*
* @end
*********************************************************************/
L7_RC_t bspapiNextSdmTemplateClear(void);

/*********************************************************************
 * @purpose  retrieves the code version of the image
 *
 * @param    image         @b{(input)}  image name
 * @param    version       @b{(output)}  pointer to the code version
 *
 * @returns  L7_SUCCESS   on successful execution
 * @returns  L7_FAILURE   if unable to read version
 *
 * @end
 *********************************************************************/
 L7_RC_t bspapiImageVersionGet(L7_char8* filename, codeVersion_t* version);

/**************************************************************************
 *
 * @purpose  Reads the header from STK files and validate it
 *
 * @param    fileName      Operational code file.
 * @param    validate_crc  flag to validate crc or not.
 *
 * @returns  none.
 *
 * @comments  This function understands STK files.
 *            STK files have the specified 'tags' in the header.
 *            This function validates the given image.
 *
 * @end
 *
 *************************************************************************/
 STK_RC_t bspapiValidateImage(L7_uchar8 *fileName, L7_BOOL validate_crc);

#endif /* BSP_API_H */
