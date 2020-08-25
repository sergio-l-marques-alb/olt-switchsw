/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename dot1q_exports.h
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

#ifndef __DOT1Q_EXPORTS_H_
#define __DOT1Q_EXPORTS_H_

#include "datatypes.h"

#define L7_MAX_VLAN_NAME            33

/* DOT1Q Component Feature List */
typedef enum
{
  L7_DOT1Q_FEATURE_ID = 0,                            /* general support statement */
  L7_DOT1Q_INGRESSFILTER_FEATURE_ID,   /* Ingress filtering is enabled by default and no
                                                       * configuration commands their after
                                                       */
  L7_DOT1Q_FEATURE_ID_TOTAL                           /* total number of enum values */
} L7_DOT1Q_FEATURE_IDS_t;


/* DOT1P Component Feature List */
typedef enum
{
  L7_DOT1P_FEATURE_SUPPORTED = 0,            /* general support statement */
  L7_DOT1P_USER_PRIORITY_PER_INTERFACE_FEATURE_ID,
  L7_DOT1P_CONFIGURE_TRAFFIC_CLASS_FEATURE_ID,
  L7_DOT1P_CONFIGURE_USER_REGENT_FEATURE_ID,
  L7_DOT1P_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_DOT1P_FEATURE_IDS_t;


/* Double Vlan Tag Component Feature List */
typedef enum
{
  L7_DVLANTAG_FEATURE_SUPPORTED = 0,            /* general support statement */
  L7_DVLANTAG_ETHERTYPE_FEATURE_ID,       /* whether ethertype is configurable */
  L7_DVLANTAG_CUSTID_FEATURE_ID,        /*whether customer id is configurable */
  L7_DVLANTAG_ETHERTYPE_PER_INTERFACE_FEATURE_ID, /* whether ethertype is set from global config mode */
  L7_DVLANTAG_MULTI_TPIDS_FEATURE_ID, /* whether Multiple TPIDs is supported or not */
  L7_DVLANTAG_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_DVLANTAG_FEATURE_IDS_t;


/*********************************************************************
*
* COMPONENT DEFINITIONS
*
*********************************************************************/

#define L7_DOT1D_BRIDGING_TYPE      2     /* set bridging type as transparent-only(2) */


/*--------------------------------------*/
/*  PBVLAN Constants                    */
/*--------------------------------------*/

/* Group Definition */
#define L7_PBVLAN_NULL_GROUP      0

/* Vlan Definition */
#define L7_PBVLAN_NULL_VLANID  0

/* Protocol Definition */
#define L7_PBVLAN_INVALID_PROTOCOL  0

/*--------------------------------------*/
/*  DOT1Q Constants                     */
/*--------------------------------------*/

#define DOT1Q_NULL_VLAN           0       /* Null VLAN  */
/* VLAN Registration Types */
#define L7_DOT1Q_DEFAULT 1
#define L7_DOT1Q_ADMIN   2
#define L7_DOT1Q_DYNAMIC 3
#define L7_DOT1Q_WS_DYNAMIC 4
#define L7_DOT1Q_IP_INTERNAL_VLAN 5  
#define L7_DOT1Q_DOT1X_REGISTERED 6  


typedef enum
{
  L7_DOT1Q_TYPE_NOT_PARTICIPATING,                          /* Not Participating */
  L7_DOT1Q_TYPE_DEFAULT,                                    /* Default VLAN, always valued at 1 */
  L7_DOT1Q_TYPE_ADMIN,                                      /* Administratively configured */
  L7_DOT1Q_TYPE_GVRP_REGISTERED,                            /* Dynamically configured  */  
  L7_DOT1Q_TYPE_WS_L2_TNNL_REGISTERED,                      /* Dynamically configured */
  L7_DOT1Q_TYPE_IP_INTERNAL_VLAN,                           /* Dynamically configured */
  L7_DOT1Q_TYPE_DOT1X_REGISTERED,                           /* Dynamically configured */
} dot1qVlanTypeVal_t, dot1QPortTypeVal_t;

/* Port Participation Constants */
#define L7_DOT1Q_FIXED 1
#define L7_DOT1Q_NORMAL_REGISTRATION 2
#define L7_DOT1Q_FORBIDDEN 3

/* Tagging Constants */
#define L7_DOT1Q_UNTAGGED 0
#define L7_DOT1Q_TAGGED 1

/* Multicast Filtering Constants */
#define L7_DOT1Q_FORWARD_ALL 1
#define L7_DOT1Q_FORWARD_UNREGISTERED 2
#define L7_DOT1Q_FILTER_UNREGISTERED 0

/* Ingress Policies */
typedef enum
{
 L7_DOT1Q_ADMIT_ALL = 1,
 L7_DOT1Q_ADMIN_ONLY_VLAN_TAGGED,
 L7_DOT1Q_ADMIN_ONLY_VLAN_UNTAGGED
} L7_DOT1Q_ACCEPT_FRAME_TYPE_t;

/* VID Definitions */
#define L7_DOT1Q_DEFAULT_VID_INDEX 0
#define L7_DOT1Q_IP_UNUSED_VID 1
#define L7_DOT1Q_IPX_UNUSED_VID 2
#define L7_DOT1Q_NETBIOS_UNUSED_VID 3

/* Min/Max Ranges */
#define L7_DOT1Q_MIN_VLAN_ID      1
#define L7_DOT1Q_MIN_VLAN_ID_CREATE      2
#define L7_DOT1Q_MAX_VLAN_ID      L7_PLATFORM_MAX_VLAN_ID
#define L7_DOT1Q_NULL_VLAN_ID     0

/* Default VLAN */
#define L7_DOT1Q_DEFAULT_VLAN     1 


/* IVL,SVL Support */
#define L7_DOT1Q_IVL_ENABLED      L7_TRUE
#define L7_DOT1Q_SVL_ENABLED      L7_FALSE

/*The max vlan id that can be used as per the dot1q protocol is 4094 */
/*the L7_DOT1Q_MAX_VALN_ID may not always be 4094, for e.g. some vlan ids */
/*may be reserved for certain scenarios in different platforms, hence the */
/*need for a define which other components may use to get 4094 */
/*care must be taken that the L7_DOT1Q_MAX_VLAN_ID is always less or equal */
/*to L7_MAX_VLAN_ID */
#define L7_MAX_VLAN_ID            4094

typedef enum
{
    DOT1Q_SWPORT_MODE_NONE = 0,/* This is used by applications like gvrp who do not care about the mode*/
    DOT1Q_SWPORT_MODE_GENERAL,
    DOT1Q_SWPORT_MODE_ACCESS,
    DOT1Q_SWPORT_MODE_TRUNK
} DOT1Q_SWPORT_MODE_t;

/* structure for dot1qVIDListGet call from DTL */
typedef struct
{
  L7_uint32 vid;
  L7_uint32 tag;
} dot1qVidList_t;

#define DOT1Q_ADMIN_VLAN          4095    /* Admin VLAN */
#define DOT1D_MAX_TRAFFIC_CLASS_PRIORITY   7

/* Internal VLAN Defines */
#define L7_DOT1Q_MAX_INTERNAL_VLANS     128 
#define L7_DOT1Q_MAX_INTERNAL_VLANS_DESCR     128 

/* Whether internal VLANs are assigned moving up from the 
 * base internal VLAN ID, or down. */
typedef enum
{
  DOT1Q_INT_VLAN_POLICY_ASCENDING = 0,
  DOT1Q_INT_VLAN_POLICY_DESCENDING
} DOT1Q_INTERNAL_VLAN_POLICY;

/******************************************************************/
/*************       Start DVLANTAG defines               *********/
/******************************************************************/

#define L7_DVLANTAG_PER_PORT_TPID_MASK         FD_DVLANTAG_PER_PORT_TPID_MASK
#define L7_DVLANTAG_DEFAULT_ETHERTYPE          FD_DVLANTAG_DEFAULT_ETHERTYPE
#define L7_DVLANTAG_MAX_ETHERTYPE              FD_DVLANTAG_MAX_ETHERTYPE
#define L7_DVLANTAG_MIN_ETHERTYPE              FD_DVLANTAG_MIN_ETHERTYPE
#define L7_DVLANTAG_VMAN_ETHERTYPE             FD_DVLANTAG_VMAN_ETHERTYPE
#define L7_DVLANTAG_802_1Q_ETHERTYPE           FD_DVLANTAG_802_1Q_ETHERTYPE
#define L7_DVLANTAG_DEFAULT_CUSTID             FD_DVLANTAG_DEFAULT_CUSTID
#define L7_DVLANTAG_MAX_CUSTID                 FD_DVLANTAG_MAX_CUSTID
#define L7_DVLANTAG_MIN_CUSTID                 FD_DVLANTAG_MIN_CUSTID
#define L7_DVLANTAG_DEFAULT_PORT_MODE          FD_DVLANTAG_DEFAULT_PORT_MODE

/******************************************************************/
/*************       End DVLANTAG defines                 *********/
/******************************************************************/


/******************** conditional Override *****************************/

#ifdef INCLUDE_DOT1Q_EXPORTS_OVERRIDES
#include "dot1q_exports_overrides.h"
#endif


#endif /* __DOT1Q_EXPORTS_H_*/

