
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  cli_web_include.h
*
* @purpose   The purpose of this file is to have a central location for
* @purpose   all cli_web_mgr includes and definitions.
*
* @component cliWebMgr component
*
* @comments  none
*
* @create    03/02/2001
*
* @author    asuthan
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef INCLUDE_CLI_WEB_MGR_H
#define INCLUDE_CLI_WEB_MGR_H


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
#include "cli_web_mgr_api.h"              /* cliWebMgr public methods */

/*
********************************************************************
*                     CLI_WEB FACTORY DEFAULTS
********************************************************************
*/
#include "defaultconfig.h"

/*
**********************************************************************
*                     CLI_WEB HEADER FILES
**********************************************************************
*/

#include "cli_web_util.h"				   

/*
********************************************************************
*                     SECOND PREPROCESSOR PASS DEFINITIONS
* There are cases that some headers need to be included before
* a specific header is to be included.  This seldom occurs, but if
* needed place the second pass included in this section.
********************************************************************
*/
#include <cli_web_include.h>
#elif defined(INCLUDE_CLI_WEB_MGR_H) && !defined(PASS2_INCLUDE_CLI_WEB_MGR_H)
#define PASS2_INCLUDE_CLI_WEB_MGR_H


#endif /* INCLUDE_CLI_WEB_MGR_H */
