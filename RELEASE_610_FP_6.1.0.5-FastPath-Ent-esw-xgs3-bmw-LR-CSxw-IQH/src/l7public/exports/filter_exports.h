/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename snooping_exports.h
*
* @purpose  Defines constants and feature definitions that are shared by Management and the application 
*
* @component 
*
* @comments 
*           
*
* @Notes   This file is included by l7_product.h
*
* @created 03/26/2008
*
* @author akulkarn    
* @end
*
**********************************************************************/

#ifndef  __FILTER_EXPORTS_H__
#define  __FILTER_EXPORTS_H__

#include "l7_common.h"
#include "flex.h"



/* Static Filter Component Feature List */
typedef enum
{
  L7_FILTER_FEATURE_SUPPORTED = 0,            /* general support statement */
  L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID,
  L7_FILTER_UCAST_DEST_PORT_FILTERING_FEATURE_ID,
  L7_FILTER_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_FILTER_FEATURE_IDS_t;


/********************Feature List supported by the component *****************************/
#if 0
typedef enum
{
  L7_MFDB_FORWARD_ALL,
  L7_MFDB_FORWARD_UNREGISTERED,
  L7_MFDB_FILTER_UNREGISTERED
} L7_MFDB_VLAN_FILTER_MODE_t;
#endif
typedef enum
{
  L7_FILTER_FORWARD_ALL,
  L7_FILTER_FORWARD_UNREGISTERED,
  L7_FILTER_FILTER_UNREGISTERED
} L7_FILTER_VLAN_FILTER_MODE_t;

/* Number of bytes in mask */
#define FILTER_INTF_INDICES   ((FD_CNFGR_NIM_MAX_LAG_INTF_NUM) / (sizeof(L7_uchar8) * 8) + 1)

/*--------------------------------------*/
/*  Filtering  Public Constants         */
/*--------------------------------------*/
#define L7_FILTER_MAX_INTF  (L7_MAX_PORT_COUNT+L7_MAX_NUM_LAG_INTF)


#endif /* __FILTER_EXPORTS_H__*/
