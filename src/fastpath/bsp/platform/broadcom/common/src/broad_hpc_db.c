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

#include "logger.h"
#include "ptin_globaldefs.h"

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

/* PTin added */
#if (PTIN_BOARD == PTIN_BOARD_CXO640G)
/**
 * Build a WC map from the array of port modes
 * 
 * @param slot_mode : Slot modes
 * @param retMap    : Map to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS - Valid map 
 *                   L7_NOT_SUPPORTED - Map not valid
 *                   L7_FAILURE - Error processing file
 */
L7_RC_t hpcConfigWCmap_build(L7_uint32 *slot_mode, HAPI_WC_PORT_MAP_t *retMap)
{
  L7_int  i, matrix_iter;
  L7_int  slot, port;
  L7_uint total_lanes;

  L7_uint8  mx_board_ver;
  L7_int    ptp_wc_index = -1;
  L7_int    ptp_wc_lane  = -1;
  L7_int    wc_index, wc_lane, wc_group, speedG;
  L7_uint32 bw_max[WC_MAX_GROUPS], ports_per_segment[WC_MAX_GROUPS/WC_SEGMENT_N_GROUPS];
  L7_BOOL   wclanes_in_use[WC_MAX_NUMBER][WC_MAX_LANES];

  HAPI_WC_PORT_MAP_t wcMap[L7_MAX_PHYSICAL_PORTS_PER_UNIT];

  /* Pointer to WC map */
  HAPI_WC_SLOT_MAP_t *WCSlotMap;

  /* Validate arguments */
  if (slot_mode==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_STARTUP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Test the two matrices */
  for (matrix_iter=0; matrix_iter<2; matrix_iter++)
  {
    if (matrix_iter==0)
    {
      LOG_TRACE(LOG_CTX_STARTUP,"Testing the other matrix...");
    }
    else
    {
      LOG_TRACE(LOG_CTX_STARTUP,"Testing current matrix...");
    }

    /* First, test the other matrix */
    /* Finalize with the current matrix to return correct interface map */
    if (is_matrix_protection())
    {
      switch (matrix_iter)
      {
      case 0:
        WCSlotMap = dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_WORK;   /* First matrix to be tested (the other) */
        break;
      default:
        WCSlotMap = dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_PROT;   /* Last matrix to be tested (current) */
        break;
      }
    }
    else
    {
      switch (matrix_iter)
      {
      case 0:
        WCSlotMap = dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_PROT;   /* First matrix to be tested (the other) */
        break;
      default:
        WCSlotMap = dapiBroadBaseWCSlotMap_CARD_BROAD_64_TENGIG_56846_REV_1_WORK;   /* Last matrix to be tested (current) */
        break;
      }
    }

    /* Clear temp var */
    memset(wcMap, 0x00, sizeof(wcMap));
    memset(bw_max, 0x00, sizeof(bw_max));
    memset(ports_per_segment, 0x00, sizeof(ports_per_segment));
    memset(wclanes_in_use, 0x00, sizeof(wclanes_in_use));

    /* PTP port only exists for hw versions 1 and 2 */
    mx_board_ver = matrix_board_version();
    if (mx_board_ver < 2)
    {
      switch (mx_board_ver)
      {
        case 1:
          ptp_wc_index = 0; ptp_wc_lane = 0;
          break;
        default:
          ptp_wc_index = 1; ptp_wc_lane = 0;
      }

      /* Which WC group is being used? */
      wc_group = WCSlotMap[ptp_wc_index].wcGroup;

      /* WC Lane is marked as is in use to not be used for other purposes */
      wclanes_in_use[ptp_wc_index][ptp_wc_lane] = L7_TRUE;
      /* Update bandwidth */
      bw_max[wc_group] += 1;    /* 1 Gb */
    }

    /* Run all provided slots */
    for (slot=1, port=0; slot<=PTIN_SYS_SLOTS_MAX /*&& port<L7_MAX_PHYSICAL_PORTS_PER_UNIT*/; slot++)
    {
      switch (slot_mode[slot-1])
      {
      case WC_SLOT_MODE_1x1G:
        speedG = 1;   total_lanes = 1;
        break;
      case WC_SLOT_MODE_2x1G:
        speedG = 1;   total_lanes = 2;
        break;
      case WC_SLOT_MODE_3x1G:
        speedG = 1;   total_lanes = 3;
        break;
      case WC_SLOT_MODE_4x1G:
        speedG = 1;   total_lanes = 4;
        break;
      case WC_SLOT_MODE_1x10G:
        speedG = 10;  total_lanes = 1;
        break;
      case WC_SLOT_MODE_2x10G:
        speedG = 10;  total_lanes = 2;
        break;
      case WC_SLOT_MODE_3x10G:
        speedG = 10;  total_lanes = 3;
        break;
      case WC_SLOT_MODE_4x10G:
        speedG = 10;  total_lanes = 4;
        break;
      case WC_SLOT_MODE_1x40G:
        speedG = 40;  total_lanes = 1;
        break;
      case WC_SLOT_MODE_2x40G:
        speedG = 40;  total_lanes = 2;
        break;
      case WC_SLOT_MODE_3x40G:
        speedG = 40;  total_lanes = 3;
        break;
      case WC_SLOT_MODE_1x100G:
        speedG = 100; total_lanes = 1;
        break;
      default:
        speedG = 0;   total_lanes = 0;
      }

      if (speedG==0 || total_lanes==0)
      {
        continue;
      }

      /* Run all lanes of each slot */
      for (i=0; i<total_lanes /*&& port<L7_MAX_PHYSICAL_PORTS_PER_UNIT*/; i++)
      {
        /* Find the first WC connected to this slot */
        for (wc_index=0; wc_index<WC_MAX_NUMBER; wc_index++)
        {
          /* Is this WC assigned to this slot? */
          if (WCSlotMap[wc_index].slotIdx != slot)
            continue;

          /* Get WC group*/
          wc_group = WCSlotMap[wc_index].wcGroup;
          if (wc_group >= WC_MAX_GROUPS)
          {
            LOG_ERR(LOG_CTX_STARTUP,"Invalid WC group (%u) for WC %u", wc_group, wc_index);
            return L7_FAILURE;
          }

          /* We have found a WC. Find the first lane free to be searched */

          wc_lane = (WC_MAX_LANES - (total_lanes % WC_MAX_LANES)) % WC_MAX_LANES;
          if (speedG>10)  wc_lane = 0;    /* Use always lane 0, if speed is higher than 10Gbps */

          LOG_TRACE(LOG_CTX_STARTUP,"First lane search for WC %u: %u", wc_index, wc_lane);

          /* Search for the first free lane */
          while (wc_lane < WC_MAX_LANES && wclanes_in_use[wc_index][wc_lane])
          {
            wc_lane++;
          }
          /* Not found: go to the next WC */
          if (wc_lane >= WC_MAX_LANES)
          {
            LOG_ERR(LOG_CTX_STARTUP,"I was here! WC %u, lane %u (speedG=%u)", wc_index, wc_lane, speedG);
            continue;
          }

          /* Check if there is enough lanes for this mode (one lane used up to 10Gs) */
          if ((wc_lane + (speedG-1)/10 + 1) > WC_MAX_LANES)
          {
            LOG_ERR(LOG_CTX_STARTUP,"I was here! WC %u, lane %u (speedG=%u)", wc_index, wc_lane, speedG);
            continue;
          }

          /* We have a WC index and a lane */

          /* Check if there is available BW to use this lane: if there is, we have found a valid WC */
          if ((bw_max[wc_group]+speedG) <= WC_GROUP_MAX_BW &&
              (ports_per_segment[wc_group/WC_SEGMENT_N_GROUPS]+1) <= WC_SEGMENT_MAX_PORTS)
            break;
        }
        /* If no free and valid lanes were found, we have an error */
        if (wc_index >= WC_MAX_NUMBER)
        {
          LOG_ERR(LOG_CTX_STARTUP,"No WC found for slot %u, lane %u", slot, i);
          return L7_NOT_SUPPORTED;
        }

        if (port<L7_MAX_PHYSICAL_PORTS_PER_UNIT)
        {
          /* We have a valid WC, and a valid lane */
          wcMap[port].portNum  = port;
          wcMap[port].slotNum  = slot;
          wcMap[port].wcIdx    = wc_index;
          wcMap[port].wcLane   = wc_lane;
          wcMap[port].wcSpeedG = speedG;
          LOG_TRACE(LOG_CTX_STARTUP,"Port %2u: slotNum=%2u, wcIdx=%2u, wcLane=%u wcSpeedG=%2u", port,
                    wcMap[port].slotNum, wcMap[port].wcIdx, wcMap[port].wcLane, wcMap[port].wcSpeedG);
        }

        /* Lane is now in use */
        wclanes_in_use[wc_index][wc_lane] = L7_TRUE;

        /* Update temp variables */
        ports_per_segment[wc_group/WC_SEGMENT_N_GROUPS]++;
        bw_max[wc_group] += speedG;
        port++;
      }
    }

    /* PTP port only exists for hw versions 1 and 2 */
    if (mx_board_ver < 2)
    {
      if (port<L7_MAX_PHYSICAL_PORTS_PER_UNIT)
      {
        slot     = WCSlotMap[ptp_wc_index].slotIdx;
        wc_group = WCSlotMap[ptp_wc_index].wcGroup;

        /* Check if there is available BW to use PTP port */
        if ((bw_max[wc_group] + 1 /*GB*/) > WC_GROUP_MAX_BW ||
            (ports_per_segment[wc_group/WC_SEGMENT_N_GROUPS]+1) > WC_SEGMENT_MAX_PORTS)
        {
          LOG_ERR(LOG_CTX_STARTUP,"No WC found for PTP port (slot=%u, wc=%u, lane=%u)", slot, ptp_wc_index, ptp_wc_lane);
          return L7_NOT_SUPPORTED;
        }

        wcMap[port].portNum  = port;
        wcMap[port].slotNum  = slot;
        wcMap[port].wcIdx    = ptp_wc_index;
        wcMap[port].wcLane   = ptp_wc_lane;
        wcMap[port].wcSpeedG = 1;     /* 1G */
        LOG_TRACE(LOG_CTX_STARTUP,"Port %2u: slotNum=%2u, wcIdx=%2u, wcLane=%u wcSpeedG=%2u (PTP port)",
                  wcMap[port].portNum,
                  wcMap[port].slotNum,
                  wcMap[port].wcIdx,
                  wcMap[port].wcLane,
                  wcMap[port].wcSpeedG);

        /* Update temp variables */
        ports_per_segment[wc_group/WC_SEGMENT_N_GROUPS]++;
        bw_max[wc_group] += 1;

        port++;   /* Next port */
      }
      else
      {
        LOG_ERR(LOG_CTX_STARTUP,"Could not allocate PTP port!");
        return L7_NOT_SUPPORTED;
      }
    }

    /* Validate maximum number of ports */
    if (port > L7_MAX_PHYSICAL_PORTS_PER_UNIT)
    {
      LOG_ERR(LOG_CTX_STARTUP,"More than 64 ports allocated (%u)!", port);
      return L7_NOT_SUPPORTED;
    }

    /* Do not use remaining ports */
    #if 0
    /* Fill remaining ports, with 1G ports */
    while (port<L7_MAX_PHYSICAL_PORTS_PER_UNIT)
    {
      /* Run all WCs searching for free lanes */
      for (wc_index=0; wc_index<WC_MAX_NUMBER; wc_index++)
      {
        /* Slot index */
        slot = WCSlotMap[wc_index].slotIdx;

        /* Skip not used WCs */
        if (WCSlotMap[wc_index].slotIdx < 0)
          continue;

        /* Do not consider slots at 40G or more */
        if (slot_mode[slot-1]==WC_SLOT_MODE_1x40G ||
            slot_mode[slot-1]==WC_SLOT_MODE_2x40G ||
            slot_mode[slot-1]==WC_SLOT_MODE_3x40G ||
            slot_mode[slot-1]==WC_SLOT_MODE_1x100G)
          continue;

        /* Find the first free lane of this WC  */
        for (wc_lane=0; wc_lane<WC_MAX_LANES && wclanes_in_use[wc_index][wc_lane]; wc_lane++);
        /* Not found: go to the next WC */
        if (wc_lane >= WC_MAX_LANES)
          continue;

        /* We have a valid WC and lane */

        /* Get WC group*/
        wc_group = WCSlotMap[wc_index].wcGroup;
        if (wc_group >= WC_MAX_GROUPS)
        {
          LOG_ERR(LOG_CTX_STARTUP,"Invalid WC group (%u) for WC %u", wc_group, wc_index);
          return L7_FAILURE;
        }

        /* Check if there is available BW to use this lane: if there is, we have found a valid WC */
        if ((bw_max[wc_group]+1) <= WC_GROUP_MAX_BW &&
            (ports_per_segment[wc_group/WC_SEGMENT_N_GROUPS]+1) <= WC_SEGMENT_MAX_PORTS)
          break;
      }
      /* If no free and valid lanes were found, we have an error */
      if (wc_index >= WC_MAX_NUMBER)
      {
        LOG_ERR(LOG_CTX_STARTUP,"No WC lane found for port %u", port);
        #if 1
        wcMap[port].portNum  = port;
        wcMap[port].slotNum  = WCSlotMap[wc_index].slotIdx;
        wcMap[port].wcIdx    = 0;
        wcMap[port].wcLane   = 0;
        wcMap[port].wcSpeedG = 0;
        LOG_TRACE(LOG_CTX_STARTUP,"Port %2u: slotNum=%2u, wcIdx=%2u, wcLane=%u wcSpeedG=%2u", port,
                  wcMap[port].slotNum, wcMap[port].wcIdx, wcMap[port].wcLane, wcMap[port].wcSpeedG);
        #else
        return L7_NOT_SUPPORTED;
        #endif
      }
      else
      {
        /* We have a valid WC, and a valid lane */
        wcMap[port].portNum  = port;
        wcMap[port].slotNum  = WCSlotMap[wc_index].slotIdx;
        wcMap[port].wcIdx    = wc_index;
        wcMap[port].wcLane   = wc_lane;
        wcMap[port].wcSpeedG = 1;       /* 1 Gbps */
        LOG_TRACE(LOG_CTX_STARTUP,"Port %2u: slotNum=%2u, wcIdx=%2u, wcLane=%u wcSpeedG=%2u", port,
                  wcMap[port].slotNum, wcMap[port].wcIdx, wcMap[port].wcLane, wcMap[port].wcSpeedG);

        /* Lane in use */
        wclanes_in_use[wc_index][wc_lane] = L7_TRUE;

        /* Update temp variables */
        ports_per_segment[wc_group/WC_SEGMENT_N_GROUPS]++;
        bw_max[wc_group] += 1;    /* 1 Gbps */
      }

      port++;
    }
    #endif

    LOG_TRACE(LOG_CTX_STARTUP,"Valid map!");
  }

  /* Return port map */
  if (retMap!=L7_NULLPTR)
  {
    /* print allocated ports */
    if (port<L7_MAX_PHYSICAL_PORTS_PER_UNIT)
    {
      LOG_WARNING(LOG_CTX_STARTUP,"Less than %u allocated ports: %u !",L7_MAX_PHYSICAL_PORTS_PER_UNIT, port);
    }

    memcpy(retMap, &wcMap, sizeof(wcMap));
  }

  return L7_SUCCESS;
}

#elif (PTIN_BOARD == PTIN_BOARD_CXO160G)

/**
 * Build a WC map from the array of port modes
 * 
 * @param slot_mode : Slot modes
 * @param retMap    : Map to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS - Valid map 
 *                   L7_NOT_SUPPORTED - Map not valid
 *                   L7_FAILURE - Error processing file
 */
L7_RC_t hpcConfigWCmap_build(L7_uint32 *slot_mode, HAPI_WC_PORT_MAP_t *retMap)
{
  L7_int  slot, port, i;
  L7_uint speedG, total_lanes;
  HAPI_WC_PORT_MAP_t wcMap[L7_MAX_PHYSICAL_PORTS_PER_UNIT];

  /* Validate arguments */
  if (slot_mode == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_STARTUP,"Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialzie port map */
  memset(wcMap, 0x00, sizeof(wcMap));
  for (i=0; i < L7_MAX_PHYSICAL_PORTS_PER_UNIT; i++)
  {
    wcMap[i].portNum  =  0;
    wcMap[i].slotNum  = -1;
    wcMap[i].wcIdx    = -1;
    wcMap[i].wcLane   = -1;
    wcMap[i].wcSpeedG =  0;
  }

  /* Frontal ports */
  for (port=0; port < CXO160G_FRONTAL_PORTS && port < L7_MAX_PHYSICAL_PORTS_PER_UNIT; port++)
  {
    wcMap[port].portNum  = port;
    wcMap[port].slotNum  = -1;
    wcMap[port].wcIdx    = -1;
    wcMap[port].wcLane   =  0;
    wcMap[port].wcSpeedG = 10;    /* frontal ports at 10G */
    LOG_TRACE(LOG_CTX_STARTUP,"Port %2u: slotNum=%2d, wcIdx=%2d, wcLane=%d wcSpeedG=%2u", port,
              wcMap[port].slotNum, wcMap[port].wcIdx, wcMap[port].wcLane, wcMap[port].wcSpeedG);
  }

  /* Run all provided slots */
  for (slot = PTIN_SYS_LC_SLOT_MIN; slot <= PTIN_SYS_LC_SLOT_MAX && port < L7_MAX_PHYSICAL_PORTS_PER_UNIT; slot++)
  {
    switch (slot_mode[slot-1])
    {
      case WC_SLOT_MODE_1x1G:
      case WC_SLOT_MODE_2x1G:
      case WC_SLOT_MODE_3x1G:
      case WC_SLOT_MODE_4x1G:
        speedG = 1;   total_lanes = 4;
        slot_mode[slot-1] = WC_SLOT_MODE_4x1G;
        break;
      case WC_SLOT_MODE_1x10G:
      case WC_SLOT_MODE_2x10G:
      case WC_SLOT_MODE_3x10G:
      case WC_SLOT_MODE_4x10G:
        speedG = 10;  total_lanes = 4;
        slot_mode[slot-1] = WC_SLOT_MODE_4x10G;
        break;
      case WC_SLOT_MODE_1x20G:
      case WC_SLOT_MODE_2x20G:
        speedG = 20;  total_lanes = 2;
        slot_mode[slot-1] = WC_SLOT_MODE_2x20G;
        break;
      default:
        speedG = 40;  total_lanes = 1;
        slot_mode[slot-1] = WC_SLOT_MODE_1x40G;
        break;
    }

    if (speedG==0 || total_lanes==0)
    {
      continue;
    }

    /* Run all lanes of each slot */
    for (i=0; i<total_lanes && port<L7_MAX_PHYSICAL_PORTS_PER_UNIT; i++)
    {
      /* We have a valid WC, and a valid lane */
      wcMap[port].portNum  = port;
      wcMap[port].slotNum  = slot;
      wcMap[port].wcIdx    = -1;
      wcMap[port].wcLane   =  i;
      wcMap[port].wcSpeedG = speedG;
      LOG_TRACE(LOG_CTX_STARTUP,"Port %2u: slotNum=%2d, wcIdx=%2d, wcLane=%d wcSpeedG=%2u", port,
                wcMap[port].slotNum, wcMap[port].wcIdx, wcMap[port].wcLane, wcMap[port].wcSpeedG);
      port++;
    }
  }

  /* Check if we have available ports */
  if (port >= L7_MAX_PHYSICAL_PORTS_PER_UNIT)
  {
    LOG_ERR(LOG_CTX_STARTUP,"All ports used up");
    return L7_FAILURE;
  }

  /* Last port is a 1G port */
  /* We have a valid WC, and a valid lane */
  wcMap[port].portNum  = port;
  wcMap[port].slotNum  = -1;
  wcMap[port].wcIdx    = -1;
  wcMap[port].wcLane   =  0;
  wcMap[port].wcSpeedG =  1;
  LOG_TRACE(LOG_CTX_STARTUP,"Port %2u: slotNum=%2d, wcIdx=%2d, wcLane=%d wcSpeedG=%2u", port,
            wcMap[port].slotNum, wcMap[port].wcIdx, wcMap[port].wcLane, wcMap[port].wcSpeedG);
  port++;

  /* Return port map */
  if (retMap!=L7_NULLPTR)
  {
    /* print allocated ports */
    if (port<L7_MAX_PHYSICAL_PORTS_PER_UNIT)
    {
      LOG_WARNING(LOG_CTX_STARTUP,"Less than %u allocated ports: %u !",L7_MAX_PHYSICAL_PORTS_PER_UNIT, port);
    }

    memcpy(retMap, &wcMap, sizeof(wcMap));
  }

  return L7_SUCCESS;
}
#endif

