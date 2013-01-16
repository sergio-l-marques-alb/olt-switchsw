/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dvlantag_include.h
*
* @purpose   Double Vlan Tagging include header
*
* @component DvlanTag
*
* @comments 
*
* @create    8/14/2003
*
* @author    skalyanam
*
* @end
*             
**********************************************************************/
#ifndef INCLUDE_DTAG_INCLUDE_H
#define INCLUDE_DTAG_INCLUDE_H

/*
 ***********************************************************************
 *                           COMMON INCLUDES
 ***********************************************************************
 */

#include "l7_common.h"
#include "osapi.h" 
#include "osapi_support.h"
#include "sysapi.h"
#include "nimapi.h"
#include "simapi.h"
#include "dtlapi.h"
#include "trace_api.h"
#include "sysnet_api.h"
#include "dot1q_api.h"
#include "fdb_api.h"
#include "log.h"
#include "nvstoreapi.h"
#include "defaultconfig.h"
#include "default_cnfgr.h"
#include "filter_api.h"
#include "datatypes.h"
#include "trapapi.h"
#include "dot3ad_api.h"
#include "mirror_api.h"
#include "l7_ip_api.h"

/*
 ********************************************************************
 *                           APIs 
 ********************************************************************
 */
#include "dvlantag_api.h"

/*
 **********************************************************************
 *                   DTAG HEADER FILES
 **********************************************************************
 */
#include "dvlantag_cfg.h"
#include "dvlantag_ih.h"
 
#endif/*INCLUDE_DTAG_INCLUDE_H*/
