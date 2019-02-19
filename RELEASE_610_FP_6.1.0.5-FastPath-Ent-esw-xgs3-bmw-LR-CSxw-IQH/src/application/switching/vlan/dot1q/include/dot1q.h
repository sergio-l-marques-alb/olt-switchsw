/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename dot1Q.h
*
* @purpose dot1Q Defines, Data Structures, and Prototypes
*
* @component dot1Q
*
* @comments none
*
* @create 08/14/2000
*
* @author wjacobs,djohnson
* @end
*
**********************************************************************/
#ifndef _DOT1Q_H_
#define _DOT1Q_H_

#include "statsapi.h"
#include "l7_product.h"
#include "platform_counters.h"
#include "dot1q_api.h"

#include "avl_api.h"
#include "defaultconfig.h"
#include "dot1q_mask.h"

#define DOT1Q_DEFAULT_FDB_ID FD_DOT1Q_DEFAULT_VLAN
#define DOT1Q_INTF_MAX_COUNT    (L7_MAX_PORT_COUNT + L7_MAX_NUM_LAG_INTF +1)

typedef enum {
  DOT1Q_PHASE_INIT_0 = 0,
  DOT1Q_PHASE_INIT_1,
  DOT1Q_PHASE_INIT_2,
  DOT1Q_PHASE_WMU,
  DOT1Q_PHASE_INIT_3,
  DOT1Q_PHASE_EXECUTE,
  DOT1Q_PHASE_UNCONFIG_1,
  DOT1Q_PHASE_UNCONFIG_2_IN_PROGRESS,
  DOT1Q_PHASE_UNCONFIG_2,
} dot1qCnfgrState_t;


typedef enum
{
  DOT1Q_DEFAULT_PVID,
  DOT1Q_IP_PVID,
  DOT1Q_IPX_PVID,
  DOT1Q_NETBIOS_PVID,
  DOT1Q_MAX_PVID    
} portNProtocolVid_t;


typedef struct
{
  L7_uint32   NumTrafficClasses;      /* 1-8 */
  L7_uint32   DefaultUserPriority;    /* 0-7 */
  L7_ushort16 Mapping[L7_DOT1P_MAX_PRIORITY+1];             /* priority to traffic class */ /* dot1dUserPriorityRegenTable */
}dot1p_prio_t;

#ifdef DOT1Q_STATS
static L7_uint32 dot1q_counters_switch[] =
{
  /* switch counters */
  L7_PLATFORM_CTR_DYNAMIC_VLAN_COUNT,
  L7_PLATFORM_CTR_MOST_VLAN_COUNT,
  L7_PLATFORM_CTR_FAILED_VLAN_CREATIONS,
  L7_PLATFORM_CTR_VLAN_DELETION_COUNT
};

/* Statistics - registered in dot1qStatsCreate */
static L7_uint32 dot1q_counters_port[] =
{  
  /* VLAN Ingress Statistics */
  L7_PLATFORM_CTR_LOCAL_TRAFFIC_DISCARDS,
  L7_PLATFORM_CTR_RX_PAUSE_FRAMES,
  L7_PLATFORM_CTR_UNACCEPTABLE_FRAME_TYPE_DISCARDS,
  L7_PLATFORM_CTR_INGRESS_FILTER_DISCARDS,
  L7_PLATFORM_CTR_INGRESS_VLAN_VIABLE_DISCARDS,
  L7_PLATFORM_CTR_MULTICAST_TREE_VIABLE_DISCARDS,
  L7_PLATFORM_CTR_RESERVED_ADDRESS_DISCARDS,
  L7_PLATFORM_CTR_BCAST_STORM_RECOVERY_DISCARDS,
  L7_PLATFORM_CTR_CFI_DISCARDS,
  L7_PLATFORM_CTR_UPSTREAM_THRESHOLD_DISCARDS,

  /* VLAN Egress Statistics */ 
  L7_PLATFORM_CTR_TX_PAUSE_FRAMES,
  L7_PLATFORM_CTR_PORT_MEMBERSHIP_DISCARDS,
  L7_PLATFORM_CTR_EGRESS_VLAN_VIABLE_DISCARDS
};
#endif

typedef struct dot1qInfo_s
{
  L7_uint32         initialized;              /* dot1q code initialized       */
  L7_uint32         version;                  /* IEEE 802.1Q version number   */
  L7_uint32         MaxVLANID;                
  L7_uint32         MaxNumVlans;              
  L7_uint32         CurrentVLANCount;
  L7_uint32         DynamicVLANCount;
  L7_uint32         MostVLANCount;  
  L7_uint32         FailedVLANCreations;
  L7_uint32         VLANDeletionCount;        /* Instances of VLAN deletion   */
  L7_uint32         log_enabled;              /* dot1q-specific logging       */
  L7_uint32         deviceCapabilities;       
  L7_uint32         TrafficClassesEnabled;    /* true/false */
} dot1qInfo_t;


#if DOT1Q_FUTURE_FUNC_GROUP_FILTER
typedef struct groupFilterSet_s
{
  NIM_INTF_MASK_t     forwardAllMask;    /* Forward All Groups           */
  NIM_INTF_MASK_t     forwardUnregMask;  /* Filter Unregistered Groups   */
} groupFilterSet_t;
#endif

/* current port information */
typedef struct dot1q_vlan_oper_s
{
  NIM_INTF_MASK_t     currentEgressPorts;
  NIM_INTF_MASK_t     taggedSet;                 /* This data is purposely duplicated
                                                    in the config structure */
  dot1qVlanTypeVal_t  status;                    /* one of dot1qVlanTypeVal_t */
  L7_timespec         creationTime;
} dot1q_vlan_oper_t;

/* static port information */
typedef struct dot1q_vlan_cfg_s
{
  L7_uchar8         name[L7_MAX_VLAN_NAME];     /* User-defined name     */  
  NIM_INTF_MASK_t   staticEgressPorts;
  NIM_INTF_MASK_t   forbiddenEgressPorts;
  NIM_INTF_MASK_t   taggedSet;
#if DOT1Q_FUTURE_FUNC_GROUP_FILTER
  groupFilterSet_t  groupFilter;
#endif

} dot1q_vlan_cfg_t;


#define DOT1Q_CFG_FILENAME  "dot1q.cfg"
#define DOT1Q_CFG_VER_1      0x1
#define DOT1Q_CFG_VER_2      0x2
#define DOT1Q_CFG_VER_3      0x3
#define DOT1Q_CFG_VER_4      0x4
#define DOT1Q_CFG_VER_5      0x5
#define DOT1Q_CFG_VER_6		 0x6
#define DOT1Q_CFG_VER_CURRENT DOT1Q_CFG_VER_6


typedef enum 
{
   VLAN_MEMBERSHIP_CONFIG = 0,
   VLAN_TAGGING_CONFIG,
   INGRESS_FILTERING_CONFIG,
   ACCEPTABLE_FRAME_TYPE_CONFIG,
   DEFAULT_PVID_CONFIG
} vlanConfigParms_t;

/* DOT1Q vlan-only configuration data */
typedef struct dot1qVlanCfgData_s
{
  L7_uint32   vlanId;
  dot1q_vlan_cfg_t vlanCfg;
      
} dot1qVlanCfgData_t;


/* AVL tree data entry */
typedef struct dot1q_vlan_dataEntry_s
{
  L7_uint32            vlanId;           /* 1-4094 valid, 4095 for implementation use   */
                                            /* DO NOT MOVE FROM THIS LOCATION              */  
  dot1q_vlan_oper_t    current;          
  dot1qVlanCfgData_t   *admin;           /* Pointer to VLAN configuration data */

  struct dot1q_vlan_dataEntry_s *next;
} dot1q_vlan_dataEntry_t;  

/* DOT1Q operational port configuration */
typedef struct dot1qIntfData_s
{
  dot1p_prio_t    priority;                      /* 802.1p port priority */  /* TBD- duplicate? */         
  L7_uint32       portCapabilities;              
  AcquiredMask    acquiredList;
  L7_uchar8       switchport_mode;       /* access/trunk/ general*/
  L7_ushort16     access_vlan;             /* only valid if mode is access */
  L7_uint32       pvid[DOT1Q_MAX_PVID];   /* operational pvid for the port */
  L7_uint32       ingressFilter;         /*operational ingress filter mode */
} dot1qIntfData_t;

/* DOT1Q port configuration */

typedef struct dot1qIntfCfg_s
{
  L7_uint32       VID[DOT1Q_MAX_PVID];           /* 1-4094 valid, 4095 reserved */
  L7_uchar8       acceptFrameType;               /* Admit All Frames or Admit VLAN_Tagged only*/
  L7_uchar8       enableIngressFiltering;        /* Enable/Disable*/
  L7_uchar8       switchport_mode;       /* access/trunk/ general*/
  L7_ushort16     access_vlan;             /* only valid if mode is access */
  dot1p_prio_t    priority;                      /* 802.1p port priority */           
} dot1qIntfCfg_t;

typedef struct dot1qIntfCfgData_s
{
  nimConfigID_t     configId;
  dot1qIntfCfg_t    portCfg;
} dot1qIntfCfgData_t;

/* DOT1Q operational global configuration */
typedef struct dot1qGlobData_s
{
  dot1p_prio_t    priority;                      /* 802.1p port priority */           
} dot1qGlobData_t;

/* DOT1Q global configuration */
typedef struct dot1qGlobCfgData_s
{
  dot1p_prio_t    priority;                      /* 802.1p port priority */ 

  /* First VLAN ID assigned to a VLAN routing interface. */
  L7_uint32 baseInternalVlan; 

  /* Whether to move up or down in the VLAN ID space from the base internal
   * VLAN ID when allocating internal VLAN IDs. */
  e_Internal_Vlan_Policy internalVlanPolicy;

} dot1qGlobCfgData_t;

/* Entire DOT1Q (port+vlan) configuration file */
typedef struct dot1qCfgData_s
{
  L7_fileHdr_t       hdr;
  dot1qGlobCfgData_t Qglobal;                    /* global dot1q cfg (vs. per-intf) */
  dot1qIntfCfgData_t Qports[DOT1Q_INTF_MAX_COUNT + 1];
  dot1qVlanCfgData_t Qvlan[L7_MAX_VLANS + 1];
  L7_VLAN_MASK_t  QvlanIntf;
  L7_uint32          traceFlags;
#define     DOT1Q_TRACE_ENABLED     0x01    /* If set, tracing is enabled */
#define     DOT1Q_TRACE_TO_CONSOLE  0x02    /* If set, print to console.  Else simply log the message */
  L7_uint32          checkSum;
} dot1qCfgData_t;

/*-------------*/
/*    MACROS   */
/*-------------*/

#define DOT1Q_LOG_MSG(format, args...)  {\
        if ( (dot1qInfo->log_enabled) == L7_TRUE) {\
             LOG_COMPONENT_MSG(L7_DOT1Q_COMPONENT_ID, format, ##args);  \
                }\
        }


#define IS_VALID_DOT1Q_VLANID(vlanID) \
        ( (vlanID > 0) || (vlanID < DOT1Q_ADMIN_VLAN) )


/*--------------------------*/
/*    DOT1Q REGISTRATION    */
/*--------------------------*/

/* Notify function name length */
#define MAX_DOT1Q_FUNC_NAME     32

typedef struct
{
  L7_uint32   eventMask;
  L7_uint32   registrarID;
  L7_uchar8   func_name[MAX_DOT1Q_FUNC_NAME];
  L7_RC_t     (*notify_request)(dot1qNotifyData_t *vlanData, L7_uint32 intIfNum, 
                                L7_uint32 event);
} dot1qVlanRequestNotifyList_t;

typedef struct dot1qVlanCheckNotifyList_s
{
  L7_uint32   registrarID;
  L7_uchar8   func_name[MAX_DOT1Q_FUNC_NAME];
  L7_RC_t     (*notify_request)(L7_uint32 vlanId, L7_uint32 intIfNum, 
                                NIM_INTF_MASK_t *mMask,L7_uint32 event);
} dot1qVlanCheckNotifyList_t;


/*---------------------------------*/
/*    DOT1Q FUNCTION PROTOTYPES    */
/*---------------------------------*/

L7_RC_t dot1qIntfChangeProcess(L7_uint32 intIfNum, NIM_EVENT_COMPLETE_INFO_t status);
L7_RC_t dot1qIntfChangeCallback(L7_uint32 intIfNum, L7_uint32 intIfEvent, NIM_CORRELATOR_t correlator);
void dot1qStatGet(pCounterValue_entry_t c);
L7_RC_t dot1qStatsSystemCreate();


/* dot1q_util.c */
L7_BOOL dot1qIntfIsSettable(L7_uint32 intIfNum);
L7_RC_t dot1qReverseMask(NIM_INTF_MASK_t in, NIM_INTF_MASK_t* out);

L7_BOOL isVlanConfigAllowed(L7_uint32 intIfNum, vlanConfigParms_t configFlag, L7_uint32 value );


#endif /* _DOT1Q_H_*/

