#include "dtlinclude.h"
#include "ptin_globaldefs.h"
#include "ptin_structs.h"
#include "dtl_ptin.h"
#include "logger.h"

/**
 * Initialize DTL PTin module
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinInit(void)
{
  DAPI_USP_t ddUsp;
  L7_RC_t rc;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_INIT, NULL);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/**
 * Set Port Ext definitions
 * 
 * @param intIfNum : interface
 * @param mefExt : MEF Ext parameters
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinL2PortExtSet(L7_uint32 intIfNum, ptin_HWPortExt_t *mefExt)
{
  DAPI_USP_t ddUsp;
  nimUSP_t   usp;
  L7_RC_t    rc;

  if (intIfNum==L7_ALL_INTERFACES)
  {
    ddUsp.unit = -1;
    ddUsp.slot = -1;
    ddUsp.port = -1;
  }
  else
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
      return L7_FAILURE;

    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  /* Set Operation */
  mefExt->operation = DAPI_CMD_SET;

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_PORTEXT, (void *) mefExt);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/**
 * Get MEF Ext definitions
 * 
 * @param intIfNum : interface
 * @param mefExt : MEF Ext parameters (output)
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinL2PortExtGet(L7_uint32 intIfNum, ptin_HWPortExt_t *mefExt)
{
  DAPI_USP_t ddUsp;
  nimUSP_t   usp;
  L7_RC_t    rc;

  if (intIfNum==L7_ALL_INTERFACES)
  {
    ddUsp.unit = -1;
    ddUsp.slot = -1;
    ddUsp.port = -1;
  }
  else
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
      return L7_FAILURE;

    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  /* Set Operation */
  mefExt->operation = DAPI_CMD_GET;

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_PORTEXT, (void *) mefExt);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/**
 * Read counters (physical interfaces)
 *  
 * Note: currently masks are ignored, meaning that all values are read
 * 
 * @param data Structure with port# and masks (port# must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinCountersRead(ptin_HWEthRFC2819_PortStatistics_t *data)
{
  DAPI_USP_t ddUsp;
  L7_RC_t rc;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_COUNTERS_READ, (void *) data);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}


/**
 * Clear counters (physical interfaces)
 *  
 * Note: currently masks are ignored, but the use of such structure as the 
 * input parameter is to allow selective fields clear 
 * 
 * @param data Structure with port# and masks (port# must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinCountersClear(ptin_HWEthRFC2819_PortStatistics_t *data)
{
  DAPI_USP_t ddUsp;
  L7_RC_t rc;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_COUNTERS_CLEAR, (void *) data);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}


/**
 * Get counters activity (physical interfaces)
 *  
 * Note: at the moment, masks are ignored, therefore all values 
 * are read for all ports)
 * 
 * @param data Structure with port# and masks (port# must be set)
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinCountersActivityGet(ptin_HWEth_PortsActivity_t *data)
{
  DAPI_USP_t ddUsp;
  L7_RC_t rc;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_COUNTERS_ACTIVITY_GET, (void *) data);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/**
 * Make use of ingress/egress vlan translations
 *  
 * @param intIfNum : Interface to apply vlan translation  
 * @param xlate_ptr : descriptor for translation data 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinVlanTranslate( L7_uint32 intIfNum, ptin_vlanXlate_t *xlate_ptr )
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t rc;

  if (intIfNum==L7_ALL_INTERFACES)
  {
    ddUsp.unit = -1;
    ddUsp.slot = -1;
    ddUsp.port = -1;
  }
  else
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
      return L7_FAILURE;
  
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_VLAN_XLATE, (void *) xlate_ptr);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/**
 * Define groups of ports for vlan egress translations
 *  
 * @param intIfNum : Interface to be applied to a portGroup  
 * @param portsGroup : descriptor with ports group data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinVlanTranslateEgressPortsGroup( L7_uint32 intIfNum, ptin_vlanXlate_classId_t *portsGroup )
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t rc;

  if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  ddUsp.unit = usp.unit;
  ddUsp.slot = usp.slot;
  ddUsp.port = usp.port - 1;

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_VLAN_XLATE_EGRESS_PORTGROUP, (void *) portsGroup);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/**
 * Vlan general configurations
 * 
 * @param vlan_mode : descriptor with vlan configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinVlanSettings( ptin_vlan_mode_t *vlan_mode )
{
  DAPI_USP_t ddUsp;
  L7_RC_t rc;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_VLAN_SETTINGS, (void *) vlan_mode);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/**
 * Vlan specific configurations
 * 
 * @param vlan_mode : descriptor with vlan configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinVlanDefinitions( ptin_bridge_vlan_mode_t *vlan_mode )
{
  DAPI_USP_t ddUsp;
  L7_RC_t rc;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_VLAN_DEFS, (void *) vlan_mode);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/**
 * Cross-connections 
 *  
 * @param intIfNum : First interface for the cross-connection 
 * @param intIfNum2 : Second interface for the cross-connection
 * @param cross_connect : descriptor with cross-connect data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinBridgeCrossconnect( L7_uint32 intIfNum, L7_uint32 intIfNum2, ptin_bridge_crossconnect_t *cross_connect )
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  L7_RC_t rc;

  /* First interface */
  if ( intIfNum == L7_ALL_INTERFACES )
  {
    ddUsp.unit = -1;
    ddUsp.slot = -1;
    ddUsp.port = -1;
  }
  else
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
      return L7_FAILURE;
  
    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  /* Second interface */
  if (intIfNum2 != 0)
  {
    if ( intIfNum2 == L7_ALL_INTERFACES )
    {
      cross_connect->dstUsp.unit = -1;
      cross_connect->dstUsp.slot = -1;
      cross_connect->dstUsp.port = -1;
    }
    else
    {
      if (nimGetUnitSlotPort(intIfNum2, &usp) != L7_SUCCESS)
        return L7_FAILURE;
  
      cross_connect->dstUsp.unit = usp.unit;
      cross_connect->dstUsp.slot = usp.slot;
      cross_connect->dstUsp.port = usp.port - 1;
    }
  }

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_VLAN_CROSSCONNECT, (void *) cross_connect);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/**
 * Apply/Get bandwidth policer
 *  
 * @param bw_policer : Bandwidth policer 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinBWPolicer( ptin_bwPolicer_t *bw_policer )
{
  DAPI_USP_t ddUsp;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_BW_POLICER, (void *) bw_policer);
}

/**
 * Apply/Get EVC Statistics
 *  
 * @param evcStats : EVC Stats structure
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinEvcStats( ptin_evcStats_t *evcStats )
{
  DAPI_USP_t ddUsp;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_FP_COUNTERS, (void *) evcStats);
}

/**
 * Add/Remove rule to trap DHCP packets
 *  
 * @param dapiCmd
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinDhcpPktsTrap( L7_uint32 intIfNum, DAPI_SYSTEM_CMD_t *dapiCmd )
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;

  /* First interface */
  if ( intIfNum == L7_ALL_INTERFACES )
  {
    ddUsp.unit = -1;
    ddUsp.slot = -1;
    ddUsp.port = -1;
  }
  else
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
      return L7_FAILURE;

    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_DHCP_PKTS_TRAP_TO_CPU, (void *) dapiCmd);
}

/**
 * Enable/Disable a Rate Limiter
 *  
 * @param intIfNum : Interface 
 * @param rateLimit: Rate Limit profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinRateLimit( L7_uint32 intIfNum, L7_BOOL enable, ptin_pktRateLimit_t *rateLimit )
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;

  /* First interface */
  if ( intIfNum == L7_ALL_INTERFACES )
  {
    ddUsp.unit = -1;
    ddUsp.slot = -1;
    ddUsp.port = -1;
  }
  else
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
      return L7_FAILURE;

    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  rateLimit->operation = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_PACKET_RATE_LIMIT, (void *) rateLimit);
}

/**
 * PRBS tx/rx
 *  
 * @param dapiCmd
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinPcsPrbs( L7_uint32 intIfNum, DAPI_SYSTEM_CMD_t *dapiCmd )
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;

  /* First interface */
  if ( intIfNum == L7_ALL_INTERFACES )
  {
    ddUsp.unit = -1;
    ddUsp.slot = -1;
    ddUsp.port = -1;
  }
  else
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
      return L7_FAILURE;

    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_PCS_PRBS, (void *) dapiCmd);
}

/**
 * Consult available hardware resources
 *  
 * @param resources: Available resources
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinHwResources( st_ptin_policy_resources *resources )
{
  DAPI_USP_t ddUsp;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  memset(resources,0x00,sizeof(st_ptin_policy_resources));

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_HW_RESOURCES, (void *) resources);
}

