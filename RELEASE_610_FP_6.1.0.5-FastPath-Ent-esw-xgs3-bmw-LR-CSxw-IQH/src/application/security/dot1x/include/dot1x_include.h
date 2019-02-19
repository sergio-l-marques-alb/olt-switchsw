/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_include.h
*
* @purpose   dot1x common include file
*
* @component dot1x
*
* @comments  none
*
* @create    04/08/2003
*
* @author    mfiorito
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DOT1X_INCLUDE_H
#define INCLUDE_DOT1X_INCLUDE_H

/*
***********************************************************************
*                           COMMON INCLUDES
***********************************************************************
*/
#include "l7_common.h"
#include "l7_resources.h"
#include "osapi.h" 
#include "osapi_support.h"
#include "sysapi.h"
#include "nimapi.h"
#include "simapi.h"
#include "dtlapi.h"
#include "trace_api.h"
#include "sysnet_api.h"
#include "fdb_api.h"
#include "log.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "filter_api.h"
#include "datatypes.h"
#include "user_mgr_api.h"
#include "dot1x_api.h"
#include "radius_api.h"
#include "user_mgr_apl.h"
#include "user_mgr_ports.h"
#include "platform_config.h"
#include "dot3ad_api.h"
#include "mirror_api.h"
#include "dot3ad_api.h"
#include "diffserv_exports.h"
#include "cli_txt_cfg_api.h"



/*
**********************************************************************
*                           STANDARD LIBRARIES
**********************************************************************
*/
#ifdef __cplusplus
  #include <stl.h>                        /* Standard Template Library */
using namespace std;
#endif

#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/*
**********************************************************************
*                   DOT1X HEADER FILES
**********************************************************************
*/
#include "dot1x_sm.h"
#include "dot1x_radius.h"
#include "dot1x_db.h"
#include "dot1x_cfg.h"
#include "dot1x_control.h"
#include "dot1x_ih.h"
#include "dot1x_txrx.h"
#include "dot1x_local.h"
#include "dot1x_outcalls.h"
#include "dot1x_debug.h"
#include "dot1x_sid.h"
#include "dot1x_mac_db.h"

#endif  /* INCLUDE_DOT1X_INCLUDE_H */
