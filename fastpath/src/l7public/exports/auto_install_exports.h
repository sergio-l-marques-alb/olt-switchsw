/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename auto_install_exports.h
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
* @created 07/17/2008
*
* @author colinw
* @end
*
**********************************************************************/

#ifndef __AUTO_INSTALL_EXPORTS_H_
#define __AUTO_INSTALL_EXPORTS_H_

/* AUTOINSTALL Component Feature List */
typedef enum
{
  L7_AUTOINSTALL_FEATURE_ID = 0,                   /* general support statement */
  L7_AUTOINSTALL_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_AUTOINSTALL_FEATURE_IDS_t;


#define AUTO_INSTALL_TEMP_CONFIG_FILENAME    "AutoInstall.scr"
#define AUTO_INSTALL_NETWORK_CONFIG_FILE     "fp-net.cfg"
#define AUTO_INSTALL_DEFAULT_CONFIG_FILE     "host.cfg"

#define AUTO_INSTALL_TFTP_TOTAL_REQUESTS_NUMBER      6
#define AUTO_INSTALL_TFTP_UNICAST_REQUESTS_NUMBER    3

/******************** conditional Override *****************************/

#ifdef INCLUDE_AUTO_INSTALL_EXPORTS_OVERRIDES
#include "auto_install_exports_overrides.h"
#endif


#endif /* __AUTO_INSTALL_EXPORTS_H_*/


