/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename transfer_exports.h
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
* @created 07/29/2008
*
* @author cpverne
* @end
*
**********************************************************************/

#ifndef __TRANSFER_EXPORTS_H_
#define __TRANSFER_EXPORTS_H_

#define L7_SCRIPTS_STARTUP_CONFIG_FILENAME "startup-config"
#define L7_SCRIPTS_BACKUP_CONFIG_FILENAME  "backup-config"
#define L7_SCRIPTS_RUNNING_CONFIG_FILENAME "running-config"
#define L7_SCRIPTS_TEMP_CONFIG_FILENAME    "temp-config.scr"

/* CLI Text Based Config */
typedef enum
{
  CONFIG_SCRIPT_WAIT_FOR_UNIT_CFG_TIME  = 400,
  CONFIG_SCRIPT_MAX_UNIT_CFG_ITERATIONS = 10,
  CONFIG_SCRIPT_PAUSE_BETWEEN_CMDS_TIME = 50
} cliTextConfigLocalConstants_t;

#define L7_MAX_URL_LENGTH           160
#define L7_MAX_FILENAME             32
#undef L7_MAX_FILEPATH
#define L7_MAX_FILEPATH             ( ( 96 > L7_MAX_URL_LENGTH ) ? 96 : L7_MAX_URL_LENGTH )
#define L7_MAX_TFTP_FILE_NAME_SIZE  128

/******************** conditional Override *****************************/

#ifdef INCLUDE_TRANSFER_EXPORTS_OVERRIDES
#include "transfer_exports_overrides.h"
#endif

#endif /* __TRANSFER_EXPORTS_H_*/
