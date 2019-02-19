/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename dot1s_exports.h
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

#ifndef  __DOT3AD_EXPORTS_H__
#define  __DOT3AD_EXPORTS_H__

#include "flex.h"


/********************Feature List supported by the component *****************************/

/* DOT3AD Component Feature List */
typedef enum
{
  L7_DOT3AD_FEATURE_ID = 0,                            /* general support statement */
  L7_DOT3AD_HASHMODE_SA_FEATURE_ID,
  L7_DOT3AD_HASHMODE_DA_FEATURE_ID,
  L7_DOT3AD_HASHMODE_SADA_FEATURE_ID,
  L7_DOT3AD_HASHMODE_SA_VLAN_ETYPE_INTF_FEATURE_ID,
  L7_DOT3AD_HASHMODE_DA_VLAN_ETYPE_INTF_FEATURE_ID,
  L7_DOT3AD_HASHMODE_SADA_VLAN_ETYPE_INTF_FEATURE_ID,
  L7_DOT3AD_HASHMODE_SIP_SPORT_FEATURE_ID,
  L7_DOT3AD_HASHMODE_DIP_DPORT_FEATURE_ID,
  L7_DOT3AD_HASHMODE_SIP_DIP_SPORT_DPORT_FEATURE_ID,
  L7_DOT3AD_HASHMODE_INTF_FEATURE_ID,
  L7_DOT3AD_HASHMODE_GLOBAL_FEATURE_ID,
  L7_DOT3AD_LAG_PRECREATE_FEATURE_ID,
  L7_DOT3AD_FEATURE_ID_TOTAL                           /* total number of enum values */
} L7_DOT3AD_FEATURE_IDS_t;

typedef enum
{
  L7_DOT3AD_HASH_MIN = 0,
  L7_DOT3AD_HASH_SA_VLAN_ETYPE_INTF,
  L7_DOT3AD_HASH_DA_VLAN_ETYPE_INTF,
  L7_DOT3AD_HASH_SADA_VLAN_ETYPE_INTF,
  L7_DOT3AD_HASH_SIP_SPORT,
  L7_DOT3AD_HASH_DIP_DPORT,
  L7_DOT3AD_HASH_SIP_DIP_SPORT_DPORT,
  L7_DOT3AD_HASH_MAX  /* Keep this last*/
}L7_DOT3AD_HASHMODES_t;



/******************** common constants defined by the component *****************************/
/**********************************************************************/
/*************              DOT1S MSTP defines                *********/
/**********************************************************************/


/*--------------------------------------*/
/*  dot3ad  Public Constants            */
/*--------------------------------------*/
#define L7_DOT3AD_PORT_ADMIN_STATE_SHORT_TIMEOUT        7
#define L7_DOT3AD_PORT_ADMIN_STATE_LONG_TIMEOUT         5
#define FD_DOT3AD_HASH_MODE               L7_DOT3AD_HASH_SADA_VLAN_ETYPE_INTF

/* This will be used at usmdb layer to map the UI hash modes to application hash modes.
 * This is done to support UI custmization if required by a customer 
 */ 

#define USMDB_LAG_HASH_SET_MAPPING_HASH1      L7_DOT3AD_HASH_SA_VLAN_ETYPE_INTF
#define USMDB_LAG_HASH_SET_MAPPING_HASH2      L7_DOT3AD_HASH_DA_VLAN_ETYPE_INTF
#define USMDB_LAG_HASH_SET_MAPPING_HASH3      L7_DOT3AD_HASH_SADA_VLAN_ETYPE_INTF
#define USMDB_LAG_HASH_SET_MAPPING_HASH4      L7_DOT3AD_HASH_SIP_SPORT
#define USMDB_LAG_HASH_SET_MAPPING_HASH5      L7_DOT3AD_HASH_DIP_DPORT
#define USMDB_LAG_HASH_SET_MAPPING_HASH6      L7_DOT3AD_HASH_SIP_DIP_SPORT_DPORT
#define USMDB_LAG_HASH_GET_MAPPING_HASH1      1
#define USMDB_LAG_HASH_GET_MAPPING_HASH2      2
#define USMDB_LAG_HASH_GET_MAPPING_HASH3      3
#define USMDB_LAG_HASH_GET_MAPPING_HASH4      4
#define USMDB_LAG_HASH_GET_MAPPING_HASH5      5
#define USMDB_LAG_HASH_GET_MAPPING_HASH6      6

#define USMDB_FD_DOT3AD_HASH_MODE             USMDB_LAG_HASH_SET_MAPPING_HASH3

/******************** conditional Override *****************************/

#ifdef INCLUDE_DOT3AD_EXPORTS_OVERRIDES
#include "dot3ad_exports_overrides.h"
#endif

#endif /* __DOT1S_EXPORTS_H__ */
