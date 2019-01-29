/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename fdb_exports.h
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

#ifndef __FDB_EXPORTS_H_
#define __FDB_EXPORTS_H_


/* FDB Component Feature List */
typedef enum
{
  L7_FDB_FEATURE_ID = 0,                   /* general support statement */
  L7_FDB_AGETIME_PER_VLAN_FEATURE_ID,
  L7_FDB_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_FDB_FEATURE_IDS_t;


/*--------------------------------------*/
/*  FWD DB Constants                    */
/*--------------------------------------*/

typedef enum
{
  L7_FDB_ADDR_FLAG_STATIC = 0,
  L7_FDB_ADDR_FLAG_LEARNED,
  L7_FDB_ADDR_FLAG_MANAGEMENT,
  L7_FDB_ADDR_FLAG_GMRP_LEARNED,
  L7_FDB_ADDR_FLAG_SELF,
  L7_FDB_ADDR_FLAG_DOT1X_STATIC,
  L7_FDB_ADDR_FLAG_L3_MANAGEMENT /* Routing Intf address */

} L7_FDB_ADDR_FLAG_t;

typedef enum l7_fdb_type_s
{
  L7_SVL =1,
  L7_IVL
} L7_FDB_TYPE_t;

#define L7_FDB_MAC_ADDR_LEN  6

#define L7_FDB_IVL_ID_LEN    2
#define L7_FDB_KEY_SIZE      (L7_FDB_IVL_ID_LEN+L7_FDB_MAC_ADDR_LEN) /* vlan id and the mac address */



/******************** conditional Override *****************************/

#ifdef INCLUDE_FDB_EXPORTS_OVERRIDES
#include "fdb_exports_overrides.h"
#endif


#endif /* __FDB_EXPORTS_H_*/


