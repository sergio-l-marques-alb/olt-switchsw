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

#ifndef  __DOT1S_EXPORTS_H__
#define  __DOT1S_EXPORTS_H__

#include "flex.h"


/********************Feature List supported by the component *****************************/

/* DOT1S Component Feature List */
typedef enum
{
  L7_DOT1S_FEATURE_ID = 0,                   /* general support statement */
  L7_DOT1S_FEATURE_ROOTGUARD,
  L7_DOT1S_FEATURE_BPDUGUARD,
  L7_DOT1S_FEATURE_LOOPGUARD,
  L7_DOT1S_FEATURE_BPPDUFILTER,
  L7_DOT1S_FEATURE_BPPDUFLOOD,
  L7_DOT1S_FEATURE_ID_TOTAL                  /* total number of enum values */
} L7_DOT1S_FEATURE_IDS_t;



/******************** common constants defined by the component *****************************/
/**********************************************************************/
/*************              DOT1S MSTP defines                *********/
/**********************************************************************/


#define     L7_DOT1S_AUTO_CALC_PATH_COST    0
#define     L7_MAX_MSTI                     64
#define     DOT1S_TC_FLAG_MASK              0x01
#define     DOT1S_PROPOSAL_FLAG_MASK        0x02
#define     DOT1S_PORT_ROLE_FLAG_MASK       0x0C
#define     DOT1S_LEARNING_FLAG_MASK        0x10
#define     DOT1S_FORWARDING_FLAG_MASK      0x20
#define     DOT1S_AGREEMENT_FLAG_MASK       0x40
#define     DOT1S_TCNACK_FLAG_MASK          0x80
#define     DOT1S_MASTER_FLAG_MASK          0x80
#define     DOT1S_TCNACK_TC_FLAG_MASK       0x81
#define     DOT1S_CIST_INDEX                0
#define     DOT1S_CIST_ID                   0
#define     L7_DOT1S_CIST_INSTANCE          DOT1S_CIST_ID
#define     L7_DOT1S_MAX_HOPS               BRIDGE_MAX_HOPS

/* Bridge ID masks */
#define     DOT1S_BRIDGE_PRIORITY_MASK      0xF000
#define     L7_DOT1S_BRIDGE_PRI_USMDB_MASK  0x0000F000
#define     DOT1S_MSTID_MASK                0x0FFF

/* Port ID masks */
#define     DOT1S_PORT_PRIORITY_MASK        0xF000
#define     L7_DOT1S_PORT_PRI_USMDB_MASK    0x000000F0
#define     DOT1S_PORTNUM_MASK              0x0FFF

#define     DOT1S_PORT_ROLE_MASTER          0
#define     DOT1S_PORT_ROLE_MASTER_MASK     0x00
#define     DOT1S_PORT_ROLE_ALT_BCK         1
#define     DOT1S_PORT_ROLE_ALT_BCK_MASK    0x04
#define     DOT1S_PORT_ROLE_ROOT            2
#define     DOT1S_PORT_ROLE_ROOT_MASK       0x08
#define     DOT1S_PORT_ROLE_DESIG           3
#define     DOT1S_PORT_ROLE_DESIG_MASK      0x0C
/* Name of 32 characters: Used in the BPDU (do not change) */
#define     DOT1S_MAX_CONFIG_NAME_SIZE      32 
/* String foor config name*/
#define     L7_DOT1S_MAX_CONFIG_NAME (DOT1S_MAX_CONFIG_NAME_SIZE + 1)
#define     DOT1S_CONFIG_DIGEST_SIZE        16 /* Used in the BPDU (do not change) */
#define     DOT1S_MAX_PORT_COUNT            4095

/*there needs to be a corresponding DAPI define for each of the below*/
#define     L7_DOT1S_DISCARDING             1
#define     L7_DOT1S_LEARNING               2
#define     L7_DOT1S_FORWARDING             3
#define     L7_DOT1S_DISABLED               4
#define     L7_DOT1S_MANUAL_FWD             5
#define     L7_DOT1S_NOT_PARTICIPATE        6
#define     L7_DOT1S_BPDU_GUARD             7

/****************************************
*  LVL7 INSTANCE OF DOT1S
*****************************************/
typedef enum
{
  L7_DOT1S_EVENT_FORWARDING = 1,
  L7_DOT1S_EVENT_NOT_FORWARDING,
  L7_DOT1S_EVENT_INSTANCE_CREATE,
  L7_DOT1S_EVENT_INSTANCE_DELETE,
  L7_DOT1S_EVENT_TOPOLOGY_CHANGE,
  L7_DOT1S_EVENT_LAST
} L7_DOT1S_EVENT_t;

typedef enum
{
  DOT1S_FORCE_VERSION_DOT1D = 0, /* IEEE 802.1d Spanning Tree */
  DOT1S_FORCE_VERSION_UNUSED,    /* unused */
  DOT1S_FORCE_VERSION_DOT1W,     /* IEEE 802.1w Rapid Spanning Tree */
  DOT1S_FORCE_VERSION_DOT1S,     /* IEEE 802.1s Multiple Spanning Tree */

  DOT1S_FORCE_VERSION_LAST       /* must be last */

}DOT1S_FORCE_VERSION_t;

/* Port Roles
 * Note: Any modifications to this will also require
 * modifications to the DOT1S_PORT_ROLE_t enum in
 * dot1s_db.h
 */
typedef enum
{
  L7_DOT1S_ROLE_DISABLED = 0, /* DisabledPort */
  L7_DOT1S_ROLE_ROOT,         /* RootPort */
  L7_DOT1S_ROLE_DESIGNATED,   /* DesignatedPort */
  L7_DOT1S_ROLE_ALTERNATE,    /* AlternatePort */
  L7_DOT1S_ROLE_BACKUP,       /* BakupPort */
  L7_DOT1S_ROLE_MASTER        /* MasterPort */

}L7_DOT1S_PORT_ROLE_t;

/* miscellaneous dot1s range values */
#define L7_DOT1S_MAX_FORMAT_SELECTOR        255
#define L7_DOT1S_MIN_FORMAT_SELECTOR        0

#define L7_DOT1S_MAX_REVISION_VALUE         65535
#define L7_DOT1S_MIN_REVISION_VALUE         0

#define L7_DOT1S_BRIDGE_MAXAGE_MAX          40
#define L7_DOT1S_BRIDGE_MAXAGE_MIN          6

#define L7_DOT1S_BRIDGE_MAXHOP_MAX          40
#define L7_DOT1S_BRIDGE_MAXHOP_MIN          6

#define L7_DOT1S_BRIDGE_HELLOTIME_MAX       10
#define L7_DOT1S_BRIDGE_HELLOTIME_MIN       1

#define L7_DOT1S_BRIDGE_HOLDCOUNT_MAX       10
#define L7_DOT1S_BRIDGE_HOLDCOUNT_MIN       1

#define L7_DOT1S_BRIDGE_FORWARDDELAY_MAX    30
#define L7_DOT1S_BRIDGE_FORWARDDELAY_MIN    4

#define L7_DOT1S_PRIORITY_MAX               61440
#define L7_DOT1S_PRIORITY_MIN               0

#define L7_DOT1S_FORCETRUE_VAL              0
#define L7_DOT1S_FORCEFALSE_VAL             1
#define L7_DOT1S_FORCEAUTO_VAL              2

#define L7_DOT1S_MAX_PORT_PATHCOST          200000000
#define L7_DOT1S_MIN_PORT_PATHCOST          0 /* Auto */
#define L7_DOT1S_AUTO_PORT_PATHCOST         L7_DOT1S_MIN_PORT_PATHCOST
#define L7_DOT1S_CONFIGURABLE_PATHCOST_MIN  1

#define L7_DOT1S_PORT_PRIORITY_MAX          240
#define L7_DOT1S_PORT_PRIORITY_MIN          0

#define L7_DOT1S_MSTID_MIN                  1
#define L7_DOT1S_MSTID_MAX                  4094
#define L7_DOT1S_MSTID_ALL                  L7_DOT1S_MSTID_MAX + 1

#define L7_DOT1S_MIN_VLAN_ID                1

#define L7_DOT1S_MAX_PORT_ROLE_LENGTH       15
#define L7_DOT1S_MAX_PORTFWDSTATE_LENGTH    18


/* -------------------------------------------------------------*/
/*  Used to determine version of 802.1 to support.  Selection   */
/*  specified in platform.h                                     */
/* -------------------------------------------------------------*/
typedef enum
{
  L7_BRIDGE_SPEC_802DOT1D = 1, /* IEEE 802.1d Spanning Tree */
  L7_BRIDGE_SPEC_802DOT1S,     /* IEEE 802.1s Multiple Spanning Tree */

  L7_BRIDGE_SPEC__LAST         /* must be last */

}L7_BRIDGE_SPEC_t;

/*platform counters need to go in platform.h*/
#define L7_PLATFORM_CTR_DOT1S_STP_RX            ((L7_DOT1S_COMPONENT_ID << 16) | 0x0000)
#define L7_PLATFORM_CTR_DOT1S_RSTP_RX           ((L7_DOT1S_COMPONENT_ID << 16) | 0x0001)
#define L7_PLATFORM_CTR_DOT1S_MSTP_RX           ((L7_DOT1S_COMPONENT_ID << 16) | 0x0002)
#define L7_PLATFORM_CTR_DOT1S_STP_TX            ((L7_DOT1S_COMPONENT_ID << 16) | 0x0003)
#define L7_PLATFORM_CTR_DOT1S_RSTP_TX           ((L7_DOT1S_COMPONENT_ID << 16) | 0x0004)
#define L7_PLATFORM_CTR_DOT1S_MSTP_TX           ((L7_DOT1S_COMPONENT_ID << 16) | 0x0005)
#define L7_PLATFORM_CTR_DOT1S_RX_DISCARDS       ((L7_DOT1S_COMPONENT_ID << 16) | 0x0006)
#define L7_PLATFORM_CTR_DOT1S_TX_DISCARDS       ((L7_DOT1S_COMPONENT_ID << 16) | 0x0007)



/******************** conditional Override *****************************/

#ifdef INCLUDE_DOT1S_EXPORTS_OVERRIDES
#include "dot1s_exports_overrides.h"
#endif

#endif /* __DOT1S_EXPORTS_H__ */
