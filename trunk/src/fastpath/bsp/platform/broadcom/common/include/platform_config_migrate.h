
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2004-2007
*
**********************************************************************
* @filename platform_config_migrate.h
*
* @purpose FASTPATH configuration Migration definitions.
*
* @component cnfgr
*
* @comments - Only XGS supported
*
* @create 08/24/2004
*
* @author Rama Sasthri, Kristipati
*
* @end
*
**********************************************************************/

#ifndef PLATFORM_CONFIG_MIGRATE_H_INCLUDED
#define PLATFORM_CONFIG_MIGRATE_H_INCLUDED

#include "l7_common.h"

/*  MAINTENANCE NOTE:

    As each release changes, to update this file, only the parms which were
    actually affected for that release need to change. (There may be more
    defines than needed in this file to facilitate initial development.)
    
    Note that the migration routines must always use defines associated with
    a specific release in order to make them less vulnerable to breakage if
    the actual parameter changes.
    
    The last migration routine for a (xxxMigrateConfigVxToVy) must use the
    latest defines which affected that file.
    
    Note also that the component configuration version number is not typically
    incremented as a result of a change in a common scaling parameter.  The
    message logs will indicate if a component could not be migrated. This is
    the signal for the developer to enhance the component migration routine.
    
 */
    
                        
/*
Porting constants which are not local to a single component,
and which are modified from one release to another are to be in
located in a central header file.  This is largely an issue with
services branches.  This file is only to be included by config migration
routines.

For base FASTPATH, a few common porting constants from previous releases
are to be defined to ensure accurate mapping.   These include, but are
not limited to,

L7_MAX_SLOTS_PER_BOX ? L7_MAX_SLOTS_PER_BOX_REL_4_0
L7_MAX_PORTS_PER_SLOT ? L7_MAX_PORTS_PER_SLOT_REL_4_0

The existence of this file does not imply that LVL7 supports configuration
file migration of base FASTPATH reference code as a rule.
This disclaimer must be prominently displayed in the file header.
*/

   

/* platform.h */
#define L7_MAX_UNITS_PER_STACK_REL_4_0          0
#define L7_MAX_UNITS_PER_STACK_REL_4_1          1
#define L7_MAX_UNITS_PER_STACK_REL_4_2          1
#ifdef L7_STACKING_PACKAGE
#define L7_MAX_UNITS_PER_STACK_REL_4_3          8
#define L7_MAX_UNITS_PER_STACK_REL_4_4          8
#define L7_MAX_UNITS_PER_STACK_REL_5_0          8
#else
#define L7_MAX_UNITS_PER_STACK_REL_4_3          1
#define L7_MAX_UNITS_PER_STACK_REL_4_4          1
#define L7_MAX_UNITS_PER_STACK_REL_5_0          1
#endif


#define L7_MAX_PHYSICAL_SLOTS_PER_BOX_REL_4_0   1
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_4_1  1
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_4_2  1
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_4_3  1
#if L7_BCM_CHIP == L7_BCM_BRADLEY
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_4_4  1
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_5_0  1
#else
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_4_4  3
#define L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_5_0  3
#endif

#define L7_MAX_PHYSICAL_PORTS_PER_SLOT_REL_4_0  52
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT_REL_4_1  52
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT_REL_4_2  52
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT_REL_4_3  52
#if L7_BCM_CHIP == L7_BCM_BRADLEY
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT_REL_4_4  20
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT_REL_5_0  20
#else
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT_REL_4_4  52
#define L7_MAX_PHYSICAL_PORTS_PER_SLOT_REL_5_0  52
#endif

#if (LVL7_NUM_PORTS == 48)
#define L7_MAX_PORT_COUNT_REL_4_0               48
#else
#define L7_MAX_PORT_COUNT_REL_4_0               28
#endif
#define L7_MAX_PORT_COUNT_REL_4_1               \
        ( L7_MAX_UNITS_PER_STACK_REL_4_1        \
        * L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_4_1 \
        * L7_MAX_PHYSICAL_PORTS_PER_SLOT_REL_4_1)
#define L7_MAX_PORT_COUNT_REL_4_2               \
        ( L7_MAX_UNITS_PER_STACK_REL_4_2        \
        * L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_4_2 \
        * L7_MAX_PHYSICAL_PORTS_PER_SLOT_REL_4_2)
#define L7_MAX_PORT_COUNT_REL_4_3          (L7_MAX_UNITS_PER_STACK_REL_4_3 * \
                    L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_4_3 * \
                    L7_MAX_PHYSICAL_PORTS_PER_SLOT_REL_4_3)
#define L7_MAX_PORT_COUNT_REL_4_4          (L7_MAX_UNITS_PER_STACK_REL_4_4 * \
                    L7_MAX_PHYSICAL_PORTS_PER_UNIT_REL_4_4)
#define L7_MAX_PORT_COUNT_REL_5_0          (L7_MAX_UNITS_PER_STACK_REL_5_0 * \
                    L7_MAX_PHYSICAL_PORTS_PER_UNIT_REL_5_0)

#if L7_BCM_CHIP == L7_BCM_BRADLEY
#define L7_MAX_PHYSICAL_PORTS_PER_UNIT_REL_4_4  20
#define L7_MAX_PHYSICAL_PORTS_PER_UNIT_REL_5_0  20
#else
#define L7_MAX_PHYSICAL_PORTS_PER_UNIT_REL_4_4  52
#define L7_MAX_PHYSICAL_PORTS_PER_UNIT_REL_5_0  52
#endif

#define L7_MAX_PORTS_PER_SLOT_REL_4_0           52
#define L7_MAX_PORTS_PER_SLOT_REL_4_1           52
#define L7_MAX_PORTS_PER_SLOT_REL_4_2           52
#define L7_MAX_PORTS_PER_SLOT_REL_4_3           52
#if L7_BCM_CHIP == L7_BCM_BRADLEY
#define L7_MAX_PORTS_PER_SLOT_REL_4_4           20
#define L7_MAX_PORTS_PER_SLOT_REL_5_0           20
#else
#define L7_MAX_PORTS_PER_SLOT_REL_4_4           52
#define L7_MAX_PORTS_PER_SLOT_REL_5_0           52
#endif

                        
#define L7_MAX_LOGICAL_PORTS_PER_SLOT_REL_4_0   8
#define L7_MAX_LOGICAL_PORTS_PER_SLOT_REL_4_1   8
#define L7_MAX_LOGICAL_PORTS_PER_SLOT_REL_4_2   8
#define L7_MAX_LOGICAL_PORTS_PER_SLOT_REL_4_3   8

#define L7_MAX_LOGICAL_SLOTS_PER_BOX_REL_4_0    3
#define L7_MAX_LOGICAL_SLOTS_PER_UNIT_REL_4_1   2
#define L7_MAX_LOGICAL_SLOTS_PER_UNIT_REL_4_2   2
#define L7_MAX_LOGICAL_SLOTS_PER_UNIT_REL_4_3   2
#define L7_MAX_LOGICAL_SLOTS_PER_UNIT_REL_4_4   4

#define L7_MAX_CPU_PORTS_PER_SLOT_REL_4_0       1
#define L7_MAX_CPU_PORTS_PER_SLOT_REL_4_1       1
#define L7_MAX_CPU_PORTS_PER_SLOT_REL_4_2       1
#define L7_MAX_CPU_PORTS_PER_SLOT_REL_4_3       1

#define L7_MAX_CPU_SLOTS_PER_BOX_REL_4_0        1
#define L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_1       1
#define L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_2       1
#define L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_3       1
#define L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_4       1

                                              
#define L7_MAX_SLOTS_PER_BOX_REL_4_0            \
        ( L7_MAX_PHYSICAL_SLOTS_PER_BOX_REL_4_0 \
        + L7_MAX_LOGICAL_SLOTS_PER_BOX_REL_4_0  \
        + L7_MAX_CPU_SLOTS_PER_BOX_REL_4_0)
#define L7_MAX_SLOTS_PER_UNIT_REL_4_1           \
        ( L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_4_1 \
        + L7_MAX_LOGICAL_SLOTS_PER_UNIT_REL_4_1  \
        + L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_1)
#define L7_MAX_SLOTS_PER_UNIT_REL_4_2           \
        ( L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_4_2 \
        + L7_MAX_LOGICAL_SLOTS_PER_UNIT_REL_4_2  \
        + L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_2)
#define L7_MAX_SLOTS_PER_UNIT_REL_4_3            \
        (L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_4_3  \
        + L7_MAX_LOGICAL_SLOTS_PER_UNIT_REL_4_3  \
        + L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_3 )
#define L7_MAX_SLOTS_PER_UNIT_REL_4_4            \
        (L7_MAX_PHYSICAL_SLOTS_PER_UNIT_REL_4_4  \
        + L7_MAX_LOGICAL_SLOTS_PER_UNIT_REL_4_4  \
        + L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_4 )

#define L7_MAX_NUM_LAG_INTF_REL_4_0             6
#define L7_MAX_NUM_LAG_INTF_REL_4_1             6
#define L7_MAX_NUM_LAG_INTF_REL_4_2             6
#define L7_MAX_NUM_LAG_INTF_REL_4_3             6
#define L7_MAX_NUM_LAG_INTF_REL_4_4            64
#define L7_MAX_NUM_LAG_INTF_REL_5_0            64

#define L7_MAX_NUM_VLAN_INTF_REL_4_0            128
#define L7_MAX_NUM_VLAN_INTF_REL_4_1            24     /*  For this platform only, this needs to match the 4.2 values */   
#define L7_MAX_NUM_VLAN_INTF_REL_4_2            24
#define L7_MAX_NUM_VLAN_INTF_REL_4_3            128
#define L7_MAX_NUM_VLAN_INTF_REL_4_4            128
#define L7_MAX_NUM_VLAN_INTF_REL_5_0            128

#define L7_MAX_NUM_LOOPBACK_INTF_REL_4_4        8

#define L7_MAX_NUM_TUNNEL_INTF_REL_4_4          8

#define L7_MAX_NUM_STACK_INTF_REL_4_0           0
#define L7_MAX_NUM_STACK_INTF_REL_4_1           0
#define L7_MAX_NUM_STACK_INTF_REL_4_2           0
#define L7_MAX_NUM_STACK_INTF_REL_4_3           0
#define L7_MAX_NUM_STACK_INTF_REL_4_4           0

#define L7_MAX_INTERFACE_COUNT_REL_4_0     \
        ( L7_MAX_PORT_COUNT_REL_4_0        \
        + L7_MAX_CPU_SLOTS_PER_BOX_REL_4_0 \
        + L7_MAX_NUM_LAG_INTF_REL_4_0      \
        + L7_MAX_NUM_VLAN_INTF_REL_4_0     \
        + L7_MAX_NUM_STACK_INTF_REL_4_0 +1 )
#define L7_MAX_INTERFACE_COUNT_REL_4_1     \
        ( L7_MAX_PORT_COUNT_REL_4_1        \
        + L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_1 \
        + L7_MAX_NUM_LAG_INTF_REL_4_1      \
        + L7_MAX_NUM_VLAN_INTF_REL_4_1     \
        + L7_MAX_NUM_STACK_INTF_REL_4_1 +1 )
#define L7_MAX_INTERFACE_COUNT_REL_4_2     \
        ( L7_MAX_PORT_COUNT_REL_4_2        \
        + L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_2 \
        + L7_MAX_NUM_LAG_INTF_REL_4_2      \
        + L7_MAX_NUM_VLAN_INTF_REL_4_2     \
        + L7_MAX_NUM_STACK_INTF_REL_4_2 +1 )
#define L7_MAX_INTERFACE_COUNT_REL_4_3     \
        ( L7_MAX_PORT_COUNT_REL_4_3        \
        + L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_3 \
        + L7_MAX_NUM_LAG_INTF_REL_4_3      \
        + L7_MAX_NUM_VLAN_INTF_REL_4_3     \
        + L7_MAX_NUM_STACK_INTF_REL_4_3 +1 )
#define L7_MAX_INTERFACE_COUNT_REL_4_4     \
        ( L7_MAX_PORT_COUNT_REL_4_4        \
        + L7_MAX_CPU_SLOTS_PER_UNIT_REL_4_4 \
        + L7_MAX_NUM_LAG_INTF_REL_4_4      \
        + L7_MAX_NUM_VLAN_INTF_REL_4_4     \
        + L7_MAX_NUM_LOOPBACK_INTF_REL_4_4 \
        + L7_MAX_NUM_TUNNEL_INTF_REL_4_4   \
        + L7_MAX_NUM_STACK_INTF_REL_4_4 +1 )
        

#define L7_MFDB_MAX_MAC_ENTRIES_REL_4_0  256
#define L7_MFDB_MAX_MAC_ENTRIES_REL_4_1  256
#define L7_MFDB_MAX_MAC_ENTRIES_REL_4_2  256
#define L7_MFDB_MAX_MAC_ENTRIES_REL_4_3  256
#define L7_MFDB_MAX_MAC_ENTRIES_REL_4_4  256

#define  L7_NUM_RESERVED_VLANS_4_4  (L7_MAX_NUM_VLAN_INTF_REL_4_4 + 1)
#define  L7_MAX_VLANS_REL_4_0       512
#define  L7_MAX_VLANS_REL_4_1       512
#define  L7_MAX_VLANS_REL_4_2       512
#define  L7_MAX_VLANS_REL_4_3       512
#define  L7_MAX_VLANS_REL_4_4       (4094 - L7_NUM_RESERVED_VLANS_4_4)

#define L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_0 100
#define L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_1 20
#define L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_2 20
#define L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_3 20
#define L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_4 20

#define L7_MAX_STATIC_FILTER_ENTRIES_REL_5_0  (L7_MAX_FDB_STATIC_FILTER_ENTRIES_REL_4_4+L7_MFDB_MAX_MAC_ENTRIES_REL_4_4)


/* l3_platform.h */
#define     L7_RT_MAX_EQUAL_COST_ROUTES_REL_4_0   6       
#define     L7_RT_MAX_EQUAL_COST_ROUTES_REL_4_1   6       
#define     L7_RT_MAX_EQUAL_COST_ROUTES_REL_4_2   6       
#define     L7_RT_MAX_EQUAL_COST_ROUTES_REL_4_3   2       
#define     L7_RT_MAX_EQUAL_COST_ROUTES_REL_4_4   2       


/* nimapi.h */
#define NIM_INTF_INDICES_REL_4_0   ((L7_MAX_INTERFACE_COUNT_REL_4_0 - 1) / (sizeof(L7_uchar8) * 8) + 1)
#define NIM_INTF_INDICES_REL_4_1   ((L7_MAX_INTERFACE_COUNT_REL_4_1 - 1) / (sizeof(L7_uchar8) * 8) + 1)
#define NIM_INTF_INDICES_REL_4_2   ((L7_MAX_INTERFACE_COUNT_REL_4_2 - 1) / (sizeof(L7_uchar8) * 8) + 1)
#define NIM_INTF_INDICES_REL_4_3   ((L7_MAX_INTERFACE_COUNT_REL_4_3 - 1) / (sizeof(L7_uchar8) * 8) + 1)
#define NIM_INTF_INDICES_REL_4_4   ((L7_MAX_INTERFACE_COUNT_REL_4_4 - 1) / (sizeof(L7_uchar8) * 8) + 1)

/* Interface storage */
typedef struct
{
  L7_uchar8 value[NIM_INTF_INDICES_REL_4_0];
}
NIM_INTF_MASK_REL_4_0_t;

/* Interface storage */
typedef struct
{
  L7_uchar8 value[NIM_INTF_INDICES_REL_4_1];
} NIM_INTF_MASK_REL_4_1_t;

typedef struct
{
  L7_uchar8 value[NIM_INTF_INDICES_REL_4_2];
} NIM_INTF_MASK_REL_4_2_t;

typedef struct
{
  L7_uchar8 value[NIM_INTF_INDICES_REL_4_3];
} NIM_INTF_MASK_REL_4_3_t;
typedef NIM_INTF_MASK_REL_4_3_t L7_INTF_MASK_REL_4_3_t;

typedef struct
{
  L7_uchar8 value[NIM_INTF_INDICES_REL_4_4];
} NIM_INTF_MASK_REL_4_4_t;
typedef NIM_INTF_MASK_REL_4_4_t L7_INTF_MASK_REL_4_4_t;

typedef struct
{
  L7_uint32 unit;
  L7_uint32 slot;
  L7_uint32 port;
}
nimUSP_REL_4_0_t;

/* To have correspondence between VLanId and USP */
typedef struct
{
  L7_ushort16 vlanID;
  nimUSP_REL_4_0_t usp;
} L7_ipUSP_REL_4_0_t;

/* component-mask.h */
#define L7_LAST_COMPONENT_ID_REL_4_0 58
#define COMPONENT_INDICES_REL_4_0   ((L7_LAST_COMPONENT_ID_REL_4_0 - 1) / (sizeof(L7_uchar8) * 8) + 1)

/* Component Mask Storage */
typedef struct
{
  L7_uchar8 value[COMPONENT_INDICES_REL_4_0];
} COMPONENT_MASK_REL_4_0_t;

#define COMPONENT_ACQUIRED_MASK_REL_4_0_t  COMPONENT_MASK_REL_4_0_t
typedef COMPONENT_ACQUIRED_MASK_REL_4_0_t AcquiredMask_REL_4_0;

/* l3_default_cnfgr.h */
#define L7_IPMAP_INTF_MAX_COUNT_REL_4_0         \
        ( L7_MAX_PORT_COUNT_REL_4_0             \
        + L7_MAX_NUM_LAG_INTF_REL_4_0           \
        + L7_MAX_NUM_VLAN_INTF_REL_4_0 + 1)
#define L7_IPMAP_INTF_MAX_COUNT_REL_4_1         \
        ( L7_MAX_PORT_COUNT_REL_4_1             \
        + L7_MAX_NUM_LAG_INTF_REL_4_1           \
        + L7_MAX_NUM_VLAN_INTF_REL_4_1 + 1)
#define L7_IPMAP_INTF_MAX_COUNT_REL_4_2         \
        ( L7_MAX_PORT_COUNT_REL_4_2             \
        + L7_MAX_NUM_LAG_INTF_REL_4_2           \
        + L7_MAX_NUM_VLAN_INTF_REL_4_2 + 1)
#define L7_IPMAP_INTF_MAX_COUNT_REL_4_3         \
        ( L7_MAX_PORT_COUNT_REL_4_3             \
        + L7_MAX_NUM_LAG_INTF_REL_4_3           \
        + L7_MAX_NUM_VLAN_INTF_REL_4_3 + 1)
#define L7_IPMAP_INTF_MAX_COUNT_REL_4_4         \
        ( L7_MAX_PORT_COUNT_REL_4_4             \
        + L7_MAX_NUM_LAG_INTF_REL_4_4           \
        + L7_MAX_NUM_LOOPBACK_INTF_REL_4_4      \
        + L7_MAX_NUM_TUNNEL_INTF_REL_4_4        \
        + L7_MAX_NUM_VLAN_INTF_REL_4_4 + 1)
        

/* l3_defaultconfig.h */
#define FD_RTR_MAX_STATIC_ROUTES_REL_4_0        64
#define FD_RTR_MAX_STATIC_ROUTES_REL_4_1        64
#define FD_RTR_MAX_STATIC_ROUTES_REL_4_2        64
#define FD_RTR_MAX_STATIC_ROUTES_REL_4_3        64
#define FD_RTR_MAX_STATIC_ROUTES_REL_4_4        64

#define FD_RTR_MAX_STATIC_ARP_ENTRIES_REL_4_0   256
#define FD_RTR_MAX_STATIC_ARP_ENTRIES_REL_4_4   256

/* dhcps_config.h */
#define L7_DHCPS_MAX_POOL_NUMBER_REL_4_0 128
#define L7_DHCPS_MAX_POOL_NUMBER_REL_4_1 128
#define L7_DHCPS_MAX_POOL_NUMBER_REL_4_2 128
#define L7_DHCPS_MAX_POOL_NUMBER_REL_4_3  16

#define L7_DHCPS_MAX_EXCLUDE_NUMBER_REL_4_0 256
#define L7_DHCPS_MAX_EXCLUDE_NUMBER_REL_4_1 256
#define L7_DHCPS_MAX_EXCLUDE_NUMBER_REL_4_2 256
#define L7_DHCPS_MAX_EXCLUDE_NUMBER_REL_4_3 128

#define L7_DHCPS_MAX_LEASE_NUMBER_REL_4_0 1024
#define L7_DHCPS_MAX_LEASE_NUMBER_REL_4_1 1024
#define L7_DHCPS_MAX_LEASE_NUMBER_REL_4_2 1024
#define L7_DHCPS_MAX_LEASE_NUMBER_REL_4_3  256

#define L7_DHCPS_CLIENT_ID_MAXLEN_REL_4_0  256
#define L7_DHCPS_CLIENT_ID_MAXLEN_REL_4_1  256
#define L7_DHCPS_CLIENT_ID_MAXLEN_REL_4_2  256
#define L7_DHCPS_CLIENT_ID_MAXLEN_REL_4_3  255

#define L7_DHCPS_HOST_NAME_MAXLEN_REL_4_0  256
#define L7_DHCPS_HOST_NAME_MAXLEN_REL_4_1  256
#define L7_DHCPS_HOST_NAME_MAXLEN_REL_4_2  256
#define L7_DHCPS_HOST_NAME_MAXLEN_REL_4_3  255


/* acl.h */
/*#define ACL_INTF_DIR_MAX_REL_4_0                2*/   /* REL_G ACL_INTF_DIR_MAX */
#define ACL_INTF_DIR_MAX_REL_4_0                  3     /* REL_4_0_0_2 ACL_INTF_DIR_MAX */
#define ACL_MAX_DIRECTION_NUM_REL_4_1             2     /* REL_H ACL_MAX_DIRECTION_NUM */      /*  For this platform only, this needs to match the 4.2 values */
#define ACL_MAX_DIRECTION_NUM_REL_4_2             2     /* REL_H2 ACL_MAX_DIRECTION_NUM */

#define L7_ACL_MAX_LISTS_REL_4_3                  100
#define L7_ACL_MAX_LISTS_REL_4_4                  100
#define L7_ACL_MAX_LISTS_REL_5_0                  100

#define L7_ACL_MAX_RULES_PER_LIST_REL_4_3         10
#define L7_ACL_MAX_RULES_PER_LIST_REL_4_4         10
#define L7_ACL_MAX_RULES_PER_LIST_REL_5_0         10

#if L7_BCM_CHIP == L7_BCM_HELIX
#define L7_ACL_MAX_RULES_PER_MULTILIST_REL_4_3    20
#define L7_ACL_MAX_RULES_PER_MULTILIST_REL_4_4    12 
#define L7_ACL_MAX_RULES_PER_MULTILIST_REL_5_0    12 
#elif L7_BCM_CHIP == L7_BCM_BRADLEY
#define L7_ACL_MAX_RULES_PER_MULTILIST_REL_4_3    12 
#define L7_ACL_MAX_RULES_PER_MULTILIST_REL_4_4    12 
#define L7_ACL_MAX_RULES_PER_MULTILIST_REL_5_0    12 
#elif L7_BCM_CHIP == L7_BCM_FIREBOLT
#define L7_ACL_MAX_RULES_PER_MULTILIST_REL_4_3    20
#define L7_ACL_MAX_RULES_PER_MULTILIST_REL_4_4    18
#define L7_ACL_MAX_RULES_PER_MULTILIST_REL_5_0    18
#else
#define L7_ACL_MAX_RULES_PER_MULTILIST_REL_4_3    20
#define L7_ACL_MAX_RULES_PER_MULTILIST_REL_4_4    28
#define L7_ACL_MAX_RULES_PER_MULTILIST_REL_5_0    28
#endif

#define L7_ACL_LOG_RULE_LIMIT_REL_4_4             128
#define L7_ACL_LOG_RULE_LIMIT_REL_5_0             128

/* if L7_ACL_MAX_RULES_PER_MULTILIST_REL_x_y is 0, then define this as L7_ACL_MAX_LISTS_REL_x_y instead */
#define L7_ACL_MAX_LISTS_PER_INTF_DIR_REL_4_3     L7_ACL_MAX_RULES_PER_MULTILIST_REL_4_3
#define L7_ACL_MAX_LISTS_PER_INTF_DIR_REL_4_4     L7_ACL_MAX_RULES_PER_MULTILIST_REL_4_4
#define L7_ACL_MAX_LISTS_PER_INTF_DIR_REL_5_0     L7_ACL_MAX_RULES_PER_MULTILIST_REL_5_0

#define L7_ACL_MAX_RULE_NUM_REL_4_0               10
#define L7_ACL_MAX_RULE_NUM_REL_4_1               10
#define L7_ACL_MAX_RULE_NUM_REL_4_2               10
#define L7_ACL_MAX_RULE_NUM_REL_4_3               L7_ACL_MAX_RULES_PER_LIST_REL_4_3
#define L7_ACL_MAX_RULE_NUM_REL_4_4               L7_ACL_MAX_RULES_PER_LIST_REL_4_4
#define L7_ACL_MAX_RULE_NUM_REL_5_0               L7_ACL_MAX_RULES_PER_LIST_REL_5_0


/* diffserv_config.h */

#define L7_DIFFSERV_CLASS_LIM_REL_4_0             25
#define L7_DIFFSERV_CLASS_LIM_REL_4_1             25
#define L7_DIFFSERV_CLASS_LIM_REL_4_2             25
#define L7_DIFFSERV_CLASS_LIM_REL_4_3             32
#define L7_DIFFSERV_CLASS_LIM_REL_4_4             32

#define L7_DIFFSERV_CLASS_HDR_MAX_REL_4_0         (L7_DIFFSERV_CLASS_HDR_START + L7_DIFFSERV_CLASS_LIM_REL_4_0)
#define L7_DIFFSERV_CLASS_HDR_MAX_REL_4_1         (L7_DIFFSERV_CLASS_HDR_START + L7_DIFFSERV_CLASS_LIM_REL_4_1)
#define L7_DIFFSERV_CLASS_HDR_MAX_REL_4_2         (L7_DIFFSERV_CLASS_HDR_START + L7_DIFFSERV_CLASS_LIM_REL_4_2)
#define L7_DIFFSERV_CLASS_HDR_MAX_REL_4_3         (L7_DIFFSERV_CLASS_HDR_START + L7_DIFFSERV_CLASS_LIM_REL_4_3)
#define L7_DIFFSERV_CLASS_HDR_MAX_REL_4_4         (L7_DIFFSERV_CLASS_HDR_START + L7_DIFFSERV_CLASS_LIM_REL_4_4)
#define L7_DIFFSERV_CLASS_HDR_MAX_REL_4_5         L7_DIFFSERV_CLASS_HDR_MAX


#if L7_BCM_CHIP == L7_BCM_HELIX
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_0    6
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_1    6
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_2    6
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_3    6
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_4    6     /* L3/4 fields only */
#elif L7_BCM_CHIP == L7_BCM_BRADLEY
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_0    6
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_1    6
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_3    6
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_4    6     /* L3/4 fields only */
#else
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_0    6
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_1    6
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_2    6
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_3    6
#define L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_4    13        
#endif

#define L7_DIFFSERV_CLASS_RULE_LIM_REL_4_0        (L7_DIFFSERV_CLASS_LIM_REL_4_0 * \
                                                   L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_0)
#define L7_DIFFSERV_CLASS_RULE_LIM_REL_4_1        (L7_DIFFSERV_CLASS_LIM_REL_4_1 * \
                                                   L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_1)
#define L7_DIFFSERV_CLASS_RULE_LIM_REL_4_2        (L7_DIFFSERV_CLASS_LIM_REL_4_2 * \
                                                   L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_2)
#define L7_DIFFSERV_CLASS_RULE_LIM_REL_4_3        (L7_DIFFSERV_CLASS_LIM_REL_4_3 * \
                                                   L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_3)
#define L7_DIFFSERV_CLASS_RULE_LIM_REL_4_4        (L7_DIFFSERV_CLASS_LIM_REL_4_4 * \
                                                   L7_DIFFSERV_RULE_PER_CLASS_LIM_REL_4_4)

#define L7_DIFFSERV_CLASS_RULE_MAX_REL_4_0        (L7_DIFFSERV_CLASS_RULE_START + \
                                                   L7_DIFFSERV_CLASS_RULE_LIM_REL_4_0)
#define L7_DIFFSERV_CLASS_RULE_MAX_REL_4_1        (L7_DIFFSERV_CLASS_RULE_START + \
                                                   L7_DIFFSERV_CLASS_RULE_LIM_REL_4_1)
#define L7_DIFFSERV_CLASS_RULE_MAX_REL_4_2        (L7_DIFFSERV_CLASS_RULE_START + \
                                                   L7_DIFFSERV_CLASS_RULE_LIM_REL_4_2)
#define L7_DIFFSERV_CLASS_RULE_MAX_REL_4_3        (L7_DIFFSERV_CLASS_RULE_START + \
                                                   L7_DIFFSERV_CLASS_RULE_LIM_REL_4_3)
#define L7_DIFFSERV_CLASS_RULE_MAX_REL_4_4        (L7_DIFFSERV_CLASS_RULE_START + \
                                                   L7_DIFFSERV_CLASS_RULE_LIM_REL_4_4)
#define L7_DIFFSERV_CLASS_RULE_MAX_REL_4_5        L7_DIFFSERV_CLASS_RULE_MAX

                                       
#define L7_DIFFSERV_POLICY_LIM_REL_4_0            64
#define L7_DIFFSERV_POLICY_LIM_REL_4_1            12    /* For this platform only, this needs to match the 4.2 values */
#define L7_DIFFSERV_POLICY_LIM_REL_4_2            12
#define L7_DIFFSERV_POLICY_LIM_REL_4_3            64
#define L7_DIFFSERV_POLICY_LIM_REL_4_4            64

#define L7_DIFFSERV_POLICY_HDR_MAX_REL_4_0        (L7_DIFFSERV_POLICY_HDR_START + L7_DIFFSERV_POLICY_LIM_REL_4_0)
#define L7_DIFFSERV_POLICY_HDR_MAX_REL_4_1        (L7_DIFFSERV_POLICY_HDR_START + L7_DIFFSERV_POLICY_LIM_REL_4_1)
#define L7_DIFFSERV_POLICY_HDR_MAX_REL_4_2        (L7_DIFFSERV_POLICY_HDR_START + L7_DIFFSERV_POLICY_LIM_REL_4_2)
#define L7_DIFFSERV_POLICY_HDR_MAX_REL_4_3        (L7_DIFFSERV_POLICY_HDR_START + L7_DIFFSERV_POLICY_LIM_REL_4_3)
#define L7_DIFFSERV_POLICY_HDR_MAX_REL_4_4        (L7_DIFFSERV_POLICY_HDR_START + L7_DIFFSERV_POLICY_LIM_REL_4_4)
#define L7_DIFFSERV_POLICY_HDR_MAX_REL_4_5        L7_DIFFSERV_POLICY_HDR_MAX

                            
#if L7_BCM_CHIP == L7_BCM_HELIX
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_0   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_1   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_2   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_3   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_4   12
#elif L7_BCM_CHIP == L7_BCM_BRADLEY
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_0   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_1   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_2   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_3   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_4   12
#elif L7_BCM_CHIP == L7_BCM_FIREBOLT
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_0   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_1   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_2   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_3   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_4   18
#else
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_0   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_1   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_2   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_3   10
#define L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_4   28
#endif

#define L7_DIFFSERV_POLICY_INST_LIM_REL_4_0       (L7_DIFFSERV_POLICY_LIM_REL_4_0 * \
                                                   L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_0)
#define L7_DIFFSERV_POLICY_INST_LIM_REL_4_1       (L7_DIFFSERV_POLICY_LIM_REL_4_1 * \
                                                   L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_1)
#define L7_DIFFSERV_POLICY_INST_LIM_REL_4_2       (L7_DIFFSERV_POLICY_LIM_REL_4_2 * \
                                                   L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_2)
#define L7_DIFFSERV_POLICY_INST_LIM_REL_4_3       (L7_DIFFSERV_POLICY_LIM_REL_4_3 * \
                                                   L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_3)
#define L7_DIFFSERV_POLICY_INST_LIM_REL_4_4       (L7_DIFFSERV_POLICY_LIM_REL_4_4 * \
                                                   L7_DIFFSERV_INST_PER_POLICY_LIM_REL_4_4)

#define L7_DIFFSERV_POLICY_INST_MAX_REL_4_0       (L7_DIFFSERV_POLICY_INST_START + L7_DIFFSERV_POLICY_INST_LIM_REL_4_0)
#define L7_DIFFSERV_POLICY_INST_MAX_REL_4_1       (L7_DIFFSERV_POLICY_INST_START + L7_DIFFSERV_POLICY_INST_LIM_REL_4_1)
#define L7_DIFFSERV_POLICY_INST_MAX_REL_4_2       (L7_DIFFSERV_POLICY_INST_START + L7_DIFFSERV_POLICY_INST_LIM_REL_4_2)
#define L7_DIFFSERV_POLICY_INST_MAX_REL_4_3       (L7_DIFFSERV_POLICY_INST_START + L7_DIFFSERV_POLICY_INST_LIM_REL_4_3)
#define L7_DIFFSERV_POLICY_INST_MAX_REL_4_4       (L7_DIFFSERV_POLICY_INST_START + L7_DIFFSERV_POLICY_INST_LIM_REL_4_4)
#define L7_DIFFSERV_POLICY_INST_MAX_REL_4_5       L7_DIFFSERV_POLICY_INST_MAX


#define L7_DIFFSERV_ATTR_PER_INST_LIM_REL_4_0     3
#define L7_DIFFSERV_ATTR_PER_INST_LIM_REL_4_1     1     /*  For this platform only, this needs to match the 4.2 values */
#define L7_DIFFSERV_ATTR_PER_INST_LIM_REL_4_2     1
#define L7_DIFFSERV_ATTR_PER_INST_LIM_REL_4_3     3
#define L7_DIFFSERV_ATTR_PER_INST_LIM_REL_4_4     3
                                             
#define L7_DIFFSERV_POLICY_ATTR_LIM_REL_4_0       (L7_DIFFSERV_POLICY_INST_LIM_REL_4_0 * \
                                                   L7_DIFFSERV_ATTR_PER_INST_LIM_REL_4_0) 
#define L7_DIFFSERV_POLICY_ATTR_LIM_REL_4_1       (L7_DIFFSERV_POLICY_INST_LIM_REL_4_1 * \
                                                   L7_DIFFSERV_ATTR_PER_INST_LIM_REL_4_1)
#define L7_DIFFSERV_POLICY_ATTR_LIM_REL_4_2       (L7_DIFFSERV_POLICY_INST_LIM_REL_4_2 * \
                                                   L7_DIFFSERV_ATTR_PER_INST_LIM_REL_4_2)
#define L7_DIFFSERV_POLICY_ATTR_LIM_REL_4_3       (L7_DIFFSERV_POLICY_INST_LIM_REL_4_3 * \
                                                   L7_DIFFSERV_ATTR_PER_INST_LIM_REL_4_3)
#define L7_DIFFSERV_POLICY_ATTR_LIM_REL_4_4       (L7_DIFFSERV_POLICY_INST_LIM_REL_4_4 * \
                                                   L7_DIFFSERV_ATTR_PER_INST_LIM_REL_4_4)

#define L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_0       (L7_DIFFSERV_POLICY_ATTR_START + L7_DIFFSERV_POLICY_ATTR_LIM_REL_4_0)
#define L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_1       (L7_DIFFSERV_POLICY_ATTR_START + L7_DIFFSERV_POLICY_ATTR_LIM_REL_4_1)
#define L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_2       (L7_DIFFSERV_POLICY_ATTR_START + L7_DIFFSERV_POLICY_ATTR_LIM_REL_4_2)
#define L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_3       (L7_DIFFSERV_POLICY_ATTR_START + L7_DIFFSERV_POLICY_ATTR_LIM_REL_4_3)
#define L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_4       (L7_DIFFSERV_POLICY_ATTR_START + L7_DIFFSERV_POLICY_ATTR_LIM_REL_4_4)
#define L7_DIFFSERV_POLICY_ATTR_MAX_REL_4_5       L7_DIFFSERV_POLICY_ATTR_MAX


#define L7_DIFFSERV_INTF_MAX_COUNT_REL_4_0        (L7_MAX_PORT_COUNT_REL_4_0 + 1)
#define L7_DIFFSERV_INTF_MAX_COUNT_REL_4_1        (L7_MAX_PORT_COUNT_REL_4_1 + 1)
#define L7_DIFFSERV_INTF_MAX_COUNT_REL_4_2        (L7_MAX_PORT_COUNT_REL_4_2 + 1)
#define L7_DIFFSERV_INTF_MAX_COUNT_REL_4_3        (L7_MAX_PORT_COUNT_REL_4_3 + \
                                                   L7_MAX_NUM_LAG_INTF_REL_4_3 + 1)
#define L7_DIFFSERV_INTF_MAX_COUNT_REL_4_4        (L7_MAX_PORT_COUNT_REL_4_4 + \
                                                   L7_MAX_NUM_LAG_INTF_REL_4_4 + 1)
#define L7_DIFFSERV_INTF_MAX_COUNT_REL_4_5        L7_DIFFSERV_INTF_MAX_COUNT


/* cos_config.h */
#define L7_COS_INTF_MAX_COUNT_REL_4_3             (L7_MAX_PORT_COUNT_REL_4_3 + \
                                                   L7_MAX_NUM_LAG_INTF_REL_4_3 + 1)
#define L7_COS_INTF_MAX_COUNT_REL_4_4             (L7_MAX_PORT_COUNT_REL_4_4 + \
                                                   L7_MAX_NUM_LAG_INTF_REL_4_4 + 1)


/* ospf_config.h */
#define L7_OSPF_REDIST_RT_LAST_REL_4_1 6
#if (30 > L7_MAX_INTERFACE_COUNT_REL_4_1)
#define L7_OSPF_MAX_AREAS_REL_4_1               (L7_MAX_INTERFACE_COUNT_REL_4_1) 
#else
#define L7_OSPF_MAX_AREAS_REL_4_1               (30) 
#endif
#define L7_OSPF_MAX_AREA_RANGES_REL_4_1         (2*L7_OSPF_MAX_AREAS_REL_4_1)
#define L7_OSPF_MAX_VIRT_NBRS_REL_4_1           10

#define L7_RTO_MAX_PROTOCOLS_REL_4_0            17
#define L7_RTO_MAX_PROTOCOLS_REL_4_1            17
#define L7_RTO_MAX_PROTOCOLS_REL_4_2            17
#define L7_RTO_MAX_PROTOCOLS_REL_4_3            17
#define L7_RTO_MAX_PROTOCOLS_REL_4_4            23

#define REDIST_RT_LAST_REL_4_0                  6
#define REDIST_RT_LAST_REL_4_1                  6
#define REDIST_RT_LAST_REL_4_2                  6
#define REDIST_RT_LAST_REL_4_3                  6
#define REDIST_RT_LAST_REL_4_4                  7
             

/* defaultconfig.h */
#define L7_PBVLAN_MAX_NUM_GROUPS_REL_4_0 128
#define L7_PBVLAN_MAX_NUM_GROUPS_REL_4_1 128
#define L7_PBVLAN_MAX_NUM_GROUPS_REL_4_2 128
#define L7_PBVLAN_MAX_NUM_GROUPS_REL_4_3 128
#define L7_PBVLAN_MAX_NUM_GROUPS_REL_4_4 128

#endif
