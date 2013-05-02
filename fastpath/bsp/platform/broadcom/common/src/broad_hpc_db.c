/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename  broad_hpc_db.c
*
* @purpose   Declare and initialize storage for use in hpc card
*            and unit descriptor databases.
*
* @component hpc
*
* @create    02/12/2003
*
* @author    jeffr 
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "dapi.h"
#include "dapi_struct.h"
#include "sysapi_hpc.h" 
#include "hpc_db.h"
#include "dapi_db.h"
#include "unitmgr_api.h"
#include "broad_init.h"
#include "broad_hpc_db.h"
#include "platform_cpu.h"

/**************************** MAC ADDRESS OFFSETS *******************/
#if L7_FEAT_UNIQUE_L3_ROUTER_MAC == 1
#define HPC_L2_MAC_OFFSET  0 //2  /* for L2 applications */   /* PTin modified: MAC address */
#define HPC_L3_MAC_OFFSET  1 //3  /* for L3 applications */   /* PTin modified: MAC address */
#else
#define HPC_L2_MAC_OFFSET  0 //2  /* for L2 applications */   /* PTin modified: MAC address */
#define HPC_L3_MAC_OFFSET  0 //2  /* for L3 applications */   /* PTin modified: MAC address */
#endif /* L7_FEAT_UNIQUE_L3_ROUTER_MAC */

/* If the above MAC address offsets needs to be different for a given
 * platform, override the above #defines in the following include file
 * (which is platform-specific).
 */
#include "broad_hpc_db.h"
/**************************** MAC ADDRESS OFFSETS *******************/

HPC_BROAD_INTF_TYPE_MAC_ALLOCATION_t uni_mac_allocation_table[] =
{
{ L7_PHYSICAL_INTF,       HPC_L2_MAC_OFFSET, HPC_L3_MAC_OFFSET}, /* physical port */
{ L7_STACK_INTF,          0, 0}, /* stack port */
{ L7_CPU_INTF,            0, 0}, /* CPU port */
/* Logical Interfaces */
{ L7_LAG_INTF,            HPC_L2_MAC_OFFSET, HPC_L3_MAC_OFFSET}, /* lag port */
{ L7_LOGICAL_VLAN_INTF,   HPC_L2_MAC_OFFSET, HPC_L3_MAC_OFFSET}, /* logical vlan port */
};

L7_uint32 uni_mac_allocation_table_num_entries = sizeof(uni_mac_allocation_table)/sizeof(HPC_BROAD_INTF_TYPE_MAC_ALLOCATION_t);


/*******************************************************************************
*
* @Data    hpc_unit_descriptor_db
*
* @purpose An array of descriptors with an entry for each unit type
*          supported.
*
* @end
*
*******************************************************************************/
/* main unit descriptor table */
HPC_UNIT_DESCRIPTOR_t hpc_unit_descriptor_db[] =
{
#include "hpc_unit_db.h"
};


L7_uint32 hpc_num_supported_unit_types = (sizeof(hpc_unit_descriptor_db)/sizeof(HPC_UNIT_DESCRIPTOR_t));

/****************************************************************
*
*  CPU Base Card Init Variable and Function Hook
*
****************************************************************/
DAPI_BASE_CARD_ENTRY_t dapiBroadBaseCardEntry = {

hapiBroadInit
};

/****************************************************************
*
*  Logical Lag Card Init Variables
*
****************************************************************/
DAPI_CARD_ENTRY_t dapiBroadLagCardEntry = {

hapiBroadLagCardInsert,hapiBroadCardRemove, NULL, 0, NULL, 0, 0, NULL, 0
};

/****************************************************************
*
*  Logical Router Card Init Variables
*
****************************************************************/
DAPI_CARD_ENTRY_t dapiBroadRouterCardEntry = {

(HAPI_CARD_FUNC_t)hapiBroadL3RouterCardInsert,hapiBroadCardRemove, NULL, 0, NULL, 0, 0, NULL, 0
};

/****************************************************************
*
*  Logical Loopback Card Init Variables
*
*  Loopbacks are a special case as there is nothing platform
*  specific to do, so we simply tie into the generic routines.
****************************************************************/
DAPI_CARD_ENTRY_t dapiBroadLoopbackCardEntry = {

hapiBroadGenericCardInsert, hapiBroadCardRemove, NULL, 0, NULL, 0, 0, NULL, 0
};

/****************************************************************
*
*  Logical Tunnel Card Init Variables
*
****************************************************************/
DAPI_CARD_ENTRY_t dapiBroadTunnelCardEntry = {

hapiBroadL3TunnelCardInsert, hapiBroadL3TunnelCardRemove, NULL, 0, NULL, 0, 0, NULL, 0
};

#if L7_WIRELESS_PACKAGE == 1
/****************************************************************
 *
 *  Logical L2 Tunnel Card Init Variables
 *
 ****************************************************************/
DAPI_CARD_ENTRY_t dapiBroadL2TunnelCardEntry = {

  hapiBroadL2TunnelCardInsert, hapiBroadL2TunnelCardRemove, NULL, 0, NULL, 0, 0, NULL, 0,
};
#endif

SYSAPI_HPC_PORT_DESCRIPTOR_t hpcPortInfoTable_CARD_CPU[]=
{
{ L7_IANA_OTHER_CPU,    /* type */
  0,                    /* defaultSpeed */
  0,                    /* phyCapabilities */
  0                     /* connectorType */
}
};

/*******************************************************************************
*
* @Data    hpc_card_descriptor_db
*
* @purpose An array of descriptors with a descriptor entry for each card type
*          supported.
*
* @end
*
*******************************************************************************/
/* main card descriptor table */
HPC_CARD_DESCRIPTOR_t hpc_card_descriptor_db[] = 
{
#include "hpc_card_db.h"
{
  /* Logical Lag Card */
  {
    L7_LOGICAL_CARD_LAG_ID,           /* cardtypeID */     
    "Logical LAG Card",               /* cardModel  */
    "Logical LAG Card",               /* cardDescription  */
    SYSAPI_CARD_TYPE_LAG,             /* type       */
    0,                                /* numOfNimPorts */
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    L7_NULLPTR,                       /* portInfo, refers to table declared above for each card type */
    &dapiBroadLagCardEntry             /* dapiCardInfo */
  }
}
,                 
{
  /* Logical Vlan Router Intf Card */
  {
    L7_LOGICAL_CARD_VLAN_ROUTER_INTF_ID, /* cardtypeID */     
    "Logical Router Card",               /* cardModel  */
    "Logical Router Card",               /* cardDescription  */
    SYSAPI_CARD_TYPE_VLAN_ROUTER,        /* type       */
    0,                                   /* numOfNimPorts */
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    L7_NULLPTR,                          /* portInfo, refers to table declared above for each card type */
    &dapiBroadRouterCardEntry             /* dapiCardInfo */
  }         
}
,                 
{
  /* Logical Loopback Intf Card */
  {
    L7_LOGICAL_CARD_LOOPBACK_INTF_ID,    /* cardtypeID */     
    "Logical Loopback Card",             /* cardModel */
    "Logical Loopback Card",             /* cardDescription */
    SYSAPI_CARD_TYPE_LOOPBACK,           /* type */
    0,                                   /* numOfNimPorts */
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    L7_NULLPTR,                          /* portInfo, refers to table declared above for each card type */
    &dapiBroadLoopbackCardEntry          /* dapiCardInfo */
  }         
}
,                 
{
  /* Logical Tunnel Intf Card */
  {
    L7_LOGICAL_CARD_TUNNEL_INTF_ID,      /* cardtypeID */     
    "Logical Tunnel Card",               /* cardModel */
    "Logical Tunnel Card",               /* cardDescription */
    SYSAPI_CARD_TYPE_TUNNEL,             /* type */
    0,                                   /* numOfNimPorts */
    SYSAPI_NO_CODE_LOAD,    /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,  /* configLoadTargetId - What configuration to load on this device */
    L7_NULLPTR,                          /* portInfo, refers to table declared above for each card type */
    &dapiBroadTunnelCardEntry            /* dapiCardInfo */
  }         
}
#if L7_WIRELESS_PACKAGE == 1
,
{
  /* CAPWAP Tunnel Intf Card */
  {
    L7_LOGICAL_CARD_CAPWAP_TUNNEL_INTF_ID, /* cardtypeID */
    "CAPWAP Tunnel Interface Card",
    "CAPWAP Tunnel Interface Card",
    SYSAPI_CARD_TYPE_CAPWAP_TUNNEL,        /* type       */
    0,                              /* numOfNiPorts */
    SYSAPI_NO_CODE_LOAD,            /* codeLoadTargetId - What code to load on this device */
    SYSAPI_NO_CONFIG_LOAD,          /* configLoadTargetId - What configuration to load on this device */
    L7_NULLPTR,                     /* portInfo, refers to table declared above for each card type*/
    &dapiBroadL2TunnelCardEntry     /* dapiCardInfo */
  }
}
#endif

};

/**************************************************************************
* @purpose  Return the number of supported units.
*
* @param    none
*
* @returns  Number of supported units.
*
* @notes 
*
* @end
*
*************************************************************************/
L7_uint32 hpcSupportedUnitsNumGet(void)
{
  return(sizeof(hpc_unit_descriptor_db)/sizeof(HPC_UNIT_DESCRIPTOR_t));
}

/**************************************************************************
* @purpose  Return the number of supported cards.
*
* @param    none
*
* @returns  Number of supported units.
*
* @notes 
*
* @end
*
*************************************************************************/
L7_uint32 hpcSupportedCardsNumGet(void)
{
  return(sizeof(hpc_card_descriptor_db)/sizeof(HPC_CARD_DESCRIPTOR_t));
}
