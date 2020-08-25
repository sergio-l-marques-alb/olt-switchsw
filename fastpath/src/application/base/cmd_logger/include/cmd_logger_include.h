
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  cmd_logger_include.h
*
* @purpose   The purpose of this file is to have a central location for
* @purpose   all cmdLogger includes and definitions.
*
* @component cmdLogger component
*
* @comments  none
*
* @create    25-ct-2004
*
* @author    gaunce
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_CMD_LOGGER_H
#define INCLUDE_CMD_LOGGER_H


/*
***********************************************************************
*                     COMMON INCLUDES
***********************************************************************
*/
#include <string.h>
#include "l7_common.h"                   /* lvl7 common definitions   */
#include "log.h"
#include "nvstoreapi.h"
#include "osapi.h"

/*
********************************************************************
*                     APIs and FUNCTION PROTOTYPES
********************************************************************
*/
#include "cmd_logger_api.h"              /* cmdLogger public methods */

/*
********************************************************************
*                     CMD_LOGGER FACTORY DEFAULTS
********************************************************************
*/
#include "defaultconfig.h"

/*
**********************************************************************
*                     CCMD_LOGGER HEADER FILES
**********************************************************************
*/

#include "cmd_logger_util.h"				   

/*
********************************************************************
*                     SECOND PREPROCESSOR PASS DEFINITIONS
* There are cases that some headers need to be included before
* a specific header is to be included.  This seldom occurs, but if
* needed place the second pass included in this section.
********************************************************************
*/


#endif /* INCLUDE_CMD_LOGGER_H */
