/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename mfdb_exports.h
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
* @created 03/26/2008
*
* @author akulkarn    
* @end
*
**********************************************************************/

#ifndef __MFDB_EXPORTS_H_
#define __MFDB_EXPORTS_H_


typedef enum
{
  L7_MFDB_FEATURE_ID = 0,                  /* general support statement */
  L7_MFDB_STATICFILTERING_FEATURE_ID,
  L7_MFDB_FEATURE_ID_TOTAL                 /* total number of enum values */
} L7_MFDB_FEATURE_IDS_t;

/*--------------------------------------*/
/*  MFDB Public Constants               */
/*--------------------------------------*/

typedef enum
{
  L7_MFDB_PROTOCOL_STATIC = 1,
  L7_MFDB_PROTOCOL_GMRP,
  L7_MFDB_PROTOCOL_IGMP,
  L7_MFDB_PROTOCOL_MLD,

  /* Total number of protocols has to always be one more than */
  /* L7_MFDB_MAX_USERS, defined in defaultconfig.h            */
  L7_MFDB_TOTAL_NUM_PROTOCOLS                                             /* total number of protocols */
                                                                          /* (for error checking)      */
} L7_MFDB_PROTOCOL_TYPE_t;

typedef enum
{
  L7_MFDB_TYPE_STATIC,
  L7_MFDB_TYPE_DYNAMIC
} L7_MFDB_ENTRY_TYPE_t;

/* MFDB table entry description definitions */
#define L7_MFDB_COMPONENT_DESCR_STRING_LEN  16
#define L7_MFDB_MGMT_CONFIGURED     "Mgmt Config"                         /* management configured entries */
#define L7_MFDB_NETWORK_CONFIGURED  "Network Config"                      /* network configured entries    */
#define L7_MFDB_NETWORK_ASSISTED    "Network Assist"                      /* network assisted entries      */

#define L7_MFDB_VLANID_LEN    2
#define L7_MFDB_KEY_SIZE      (L7_MFDB_VLANID_LEN + L7_MAC_ADDR_LEN)




/******************** conditional Override *****************************/

#ifdef INCLUDE_MFDB_EXPORTS_OVERRIDES
#include "mfdb_exports_overrides.h"
#endif


#endif /* __MFDB_EXPORTS_H_*/


