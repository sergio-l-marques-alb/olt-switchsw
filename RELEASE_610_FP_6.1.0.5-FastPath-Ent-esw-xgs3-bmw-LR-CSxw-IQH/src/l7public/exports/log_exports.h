/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2008
*
**********************************************************************
*
* @filename log_exports.h
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

#ifndef __LOG_EXPORTS_H_
#define __LOG_EXPORTS_H_


/* LOG Component Feature List */
typedef enum
{
  L7_LOG_FEATURE_ID = 0,            /* general support statement */
  L7_LOG_PERSISTENT_FEATURE_ID,
  L7_LOG_BUFFERED_FEATURE_ID,
  L7_LOG_CONSOLE_FEATURE_ID,
  L7_LOG_SYSLOG_FEATURE_ID,
  L7_LOG_FEATURE_ID_TOTAL           /* total number of enum values */
} L7_LOG_FEATURE_IDS_t;



/******************** conditional Override *****************************/

#ifdef INCLUDE_LOG_EXPORTS_OVERRIDES
#include "log_exports_overrides.h"
#endif

#endif /* __LOG_EXPORTS_H_*/
