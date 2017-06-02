/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename file_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   
*
* @created 
*
* @author 
* @end
*
**********************************************************************/

#ifndef __FILE_EXPORTS_H_
#define __FILE_EXPORTS_H_


#define L7_MAX_FILENAME             32
#define L7_MAX_FILEPATH             96
#define L7_MAX_TFTP_FILE_NAME_SIZE  128

/*********************************************************************
*
* FILE TRANSFER SECTION
*
*********************************************************************/

/*--------------------------------------*/
/*  FILE TRANSFER CONSTANTS             */
/*--------------------------------------*/
#define L7_TRANSFER_DIRECTION_DOWN  0
#define L7_TRANSFER_DIRECTION_UP    1


/*--------------------------------------*/
/*  FILE TRANSFER TYPES                 */
/*--------------------------------------*/
typedef enum
{
  L7_TRANSFER_XMODEM,
  L7_TRANSFER_TFTP,
  L7_TRANSFER_SFTP,
  L7_TRANSFER_SCP,
  L7_TRANSFER_YMODEM,
  L7_TRANSFER_ZMODEM,
  L7_TRANSFER_HTTP,
  L7_TRANSFER_FTP,
  L7_TRANSFER_LOCAL,
  L7_TRANSFER_LAST

} L7_TRANSFER_TYPES_t;


/*--------------------------------------*/
/*  FILE DOWNLOAD TYPES                 */
/*--------------------------------------*/
typedef enum
{
  L7_FILE_DNL_CODE,                                                       /* Download operational code */
  L7_FILE_DNL_NORMAL                                                      /* Download other types */
} L7_DNL_TYPE_t;                                                          /* TBD:  Deprecated? */

typedef enum
{
  L7_FILE_TYPE_CODE,
  L7_FILE_TYPE_CONFIG,
  L7_FILE_TYPE_TXTCFG,
  L7_FILE_TYPE_ERRORLOG,
  L7_FILE_TYPE_VALGLOG,
  L7_FILE_TYPE_SYSTEM_MSGLOG,
  L7_FILE_TYPE_TRAP_LOG,
  L7_FILE_TYPE_TRACE_DUMP,
  L7_FILE_TYPE_SSHKEY_RSA1,
  L7_FILE_TYPE_SSHKEY_RSA2,
  L7_FILE_TYPE_SSHKEY_DSA,
  L7_FILE_TYPE_SSLPEM_ROOT,
  L7_FILE_TYPE_SSLPEM_SERVER,
  L7_FILE_TYPE_SSLPEM_DHWEAK,
  L7_FILE_TYPE_SSLPEM_DHSTRONG,
  L7_FILE_TYPE_STK,
  L7_FILE_TYPE_CONFIG_SCRIPT,
  L7_FILE_TYPE_CLI_BANNER,
  L7_FILE_TYPE_KERNEL,
  L7_FILE_TYPE_ACL_XML,
  L7_FILE_TYPE_DHCP_SNOOPING_BINDINGS, 
  L7_FILE_TYPE_STARTUP_LOG,
  L7_FILE_TYPE_OPERATIONAL_LOG,
  L7_FILE_TYPE_FWIMAGE,
  L7_FILE_TYPE_TR069_ACS_SSLPEM_ROOT, 
  L7_FILE_TYPE_TR069_CLIENT_SSLPRIV_KEY, 
  L7_FILE_TYPE_TR069_CLIENT_SSL_CERT, 
  L7_FILE_TYPE_AUTO_INSTALL_SCRIPT,
  L7_FILE_TYPE_FLASH,
L7_FILE_TYPE_IAS_USERS,
  L7_FILE_TYPE_LAST

} L7_FILE_TYPES_t;

typedef enum
{
  L7_NO_STATUS_CHANGE = 1,
  L7_STATUS_CHANGED,
  L7_TRANSFER_FAILED,
  L7_TRANSFER_BAD_CRC,
  L7_FLASH_FAILED,
  L7_OPR_FILE_TYPE_FAILED,
  L7_OTHER_FILE_TYPE_FAILED,
  L7_NEXT_DATA_BLOCK_RECEIVED,
  L7_TRANSFER_FILE_NOT_FOUND,
  L7_TRANSFER_SUCCESS,
  L7_TFTP_FAILED,
  L7_TRANSFER_STATUS_LAST
} L7_TRANSFER_STATUS_t;

typedef enum
{
  L7_TRANSFER_CODE_NONE,
  L7_TRANSFER_CODE_STARTING,
  L7_TRANSFER_CODE_ERROR_STARTING,
  L7_TRANSFER_CODE_WRONG_FILE_TYPE,
  L7_TRANSFER_CODE_UPDATE_CONFIG,
  L7_TRANSFER_CODE_INVALID_CONFIG,
  L7_TRANSFER_CODE_WRITING_FLASH,
  L7_TRANSFER_CODE_FLASH_FAILED,
  L7_TRANSFER_CODE_CHECKING_CRC,
  L7_TRANSFER_CODE_CRC_FAILURE,
  L7_TRANSFER_CODE_UNKNOWN_DIRECTION,
  L7_TRANSFER_CODE_SUCCESS,
  L7_TRANSFER_CODE_FAILURE
} L7_TRANSFER_CODE_t;

typedef enum
{
  L7_SAVE_CONFIG_CODE_NONE,
  L7_SAVE_CONFIG_CODE_STARTING,
  L7_SAVE_CONFIG_CODE_SUCCESS,
  L7_SAVE_CONFIG_CODE_FAILURE,
} L7_SAVE_CONFIG_CODE_t;

/****************************************
*
*  file data types used in L7_fileHdr_t
*
*****************************************/

typedef enum
{

  L7_CFG_DATA = 1,
  L7_TRACE_DATA,
  L7_INIT_PARMS,
  L7_DUMP_DATA

}L7_FILE_DATA_TYPES_t;


typedef enum
{
  L7_SAVE_CONFIG_TYPE_NONE = 0,
  L7_SAVE_CONFIG_TYPE_STARTUP = 1,
  L7_SAVE_CONFIG_TYPE_BACKUP = 2,
  L7_SAVE_CONFIG_TYPE_TEMP = 3,
  L7_SAVE_CONFIG_TYPE_RUNNING = 4,
  L7_SAVE_CONFIG_TYPE_MAX
} L7_SAVE_CONFIG_TYPE_t;


/******************** conditional Override *****************************/

#ifdef INCLUDE_FILE_EXPORTS_OVERRIDES
#include "file_exports_overrides.h"
#endif

#endif /* __FILE_EXPORTS_H_*/
