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
 * Generic DTL procedure
 * 
 * @author mruas (2/26/2015)
 * 
 * @param intIfNum 
 * @param msgId 
 * @param operation 
 * @param dataSize 
 * @param data 
 * 
 * @return L7_RC_t 
 */
L7_RC_t dtlPtinGeneric(L7_uint32 intIfNum, L7_uint16 msgId, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data)
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
  ptinDtlGenericMsg_t ptinDtlGeneric;
  L7_RC_t rc = L7_SUCCESS;

  /* Interface */
  if ( intIfNum == L7_ALL_INTERFACES )
  {
    ddUsp.unit = -1;
    ddUsp.slot = -1;
    ddUsp.port = -1;
  }
  else
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
      LOG_PT_ERR(LOG_CTX_DTL, "Invalid intIfNum %u!", intIfNum);
      return L7_FAILURE;
    }

    ddUsp.unit = usp.unit;
    ddUsp.slot = usp.slot;
    ddUsp.port = usp.port - 1;
  }

  /* Validate size */
  if (dataSize > PTIN_DTL_GENERICMSG_MAX_DATASIZE)
  {
    LOG_PT_ERR(LOG_CTX_DTL, "Invalid dataSize (%u > %u bytes)!", dataSize, PTIN_DTL_GENERICMSG_MAX_DATASIZE);
    return L7_FAILURE;
  }

  /* Fill parameters */
  memset(&ptinDtlGeneric, 0x00, sizeof(ptinDtlGeneric));

  ptinDtlGeneric.getOrSet = operation;
  ptinDtlGeneric.msgId    = msgId;
  ptinDtlGeneric.dataSize = dataSize;
  memcpy(ptinDtlGeneric.data, data, dataSize);

  /* Goto HAPI layer */
  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_GENERIC, (void *) &ptinDtlGeneric);

  if (rc != L7_SUCCESS)
  {
    LOG_PT_ERR(LOG_CTX_DTL, "Error rc=%u", rc);
    return rc;
  }
  else
  {
    memcpy(data, ptinDtlGeneric.data, dataSize);
  }

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
 * Vlan specific configurations
 * 
 * @param vlan_mode : descriptor with vlan configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinVlanPortControl( L7_uint32 intIfNum, ptin_vlan_mode_t *vlan_mode )
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

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_VLAN_PORT_CONTROL, (void *) vlan_mode);
  if (rc != L7_SUCCESS)
    return rc;

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
 * @param intIfNum : Interface 
 * @param bw_policer : Bandwidth policer 
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinBWPolicer( L7_uint32 intIfNum, ptin_bwPolicer_t *bw_policer )
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;

  /* Validate arguments */
  if (bw_policer == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

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

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_BW_POLICER, (void *) bw_policer);
}

/**
 * Apply/Get EVC Statistics
 *  
 * @param intIfNum : First interface for the cross-connection  
 * @param evcStats : EVC Stats structure
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinEvcStats( L7_uint32 intIfNum, ptin_evcStats_t *evcStats )
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;

  /* Validate arguments */
  if (evcStats == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

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

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_FP_COUNTERS, (void *) evcStats);
}

/**
 * Add/Remove rule to trap DHCP packets
 *  
 * @param dapiCmd
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinPacketsTrap( L7_uint32 intIfNum, DAPI_SYSTEM_CMD_t *dapiCmd )
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

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_PACKETS_TRAP_TO_CPU, (void *) dapiCmd);
}

/**
 * Storm Control configuration
 *  
 * @param intIfNum : Interface 
 * @param rateLimit: Rate Limit profile
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinStormControl( L7_uint32 intIfNum, L7_BOOL enable, ptin_stormControl_t *stormControl )
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

  stormControl->operation = (enable) ? DAPI_CMD_SET : DAPI_CMD_CLEAR;

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_STORM_CONTROL, (void *) stormControl);
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
 * Slot mode
 *  
 * @param dapiCmd
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinSlotMode( ptin_slotmode_t *dapiCmd )
{
  DAPI_USP_t ddUsp;

  /* All interfaces */
  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_SLOT_MODE, (void *) dapiCmd);
}

/**
 * Hardware procedure
 *  
 * @param intIfNum : interface
 * @param dapiCmd : data
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinHwProc( L7_uint32 intIfNum,  ptin_hwproc_t *dapiCmd )
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

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_HW_PROCEDURE, (void *) dapiCmd);
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

/**
 * Vlan Multicast configurations
 * 
 * @param mc_mode : descriptor with vlan configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinVlanBridgeMulticast( ptin_bridge_vlan_multicast_t *mc_mode )
{
  DAPI_USP_t ddUsp;
  L7_RC_t rc;

  ddUsp.unit = -1;
  ddUsp.slot = -1;
  ddUsp.port = -1;

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_VLAN_MULTICAST, (void *) mc_mode);

  return rc;
}

/**
 * Multicast egress port configurations
 * 
 * @param mc_mode : descriptor with port configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinMulticastEgressPort(L7_uint32 intIfNum, ptin_bridge_vlan_multicast_t *mc_mode )
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
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

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_MULTICAST_EGRESS_PORT, (void *) mc_mode);

  return rc;
}

/**
 * Virtual port configurations
 * 
 * @param vport : descriptor with virtual port configuration
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
L7_RC_t dtlPtinVirtualPort(L7_uint32 intIfNum, ptin_vport_t *vport )
{
  nimUSP_t usp;
  DAPI_USP_t ddUsp;
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

  rc = dapiCtl(&ddUsp, DAPI_CMD_PTIN_VIRTUAL_PORT, (void *) vport);

  return rc;
}


/**
 * L3 Host Add
 * 
 * @param intIfNum 
 * @param l3_intf 
 * @param ipAddr 
 * @param mac 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t dtlPtinL3HostAdd(L7_uint32 intIfNum, L7_int l3_intf, L7_uint32 ipAddr, L7_char8 *mac)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  st_ptin_l3 dapiCmd;

  printf("dtlPtinL3HostAdd(intIfNum=0x%x, l3_intf=0x%x, ipAddr=0x%.2x, mac=0x%.2x%.2x%.2x%.2x%.2x%.2x)\n", intIfNum, l3_intf, ipAddr, 
         (unsigned int) mac[0], (unsigned int) mac[1], (unsigned int) mac[2], (unsigned int) mac[3], (unsigned int) mac[4], (unsigned int) mac[5]);

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

  memset(&dapiCmd, 0x00, sizeof(st_ptin_l3));

  dapiCmd.cmd   = DAPI_CMD_SET;
  dapiCmd.oper  = PTIN_L3_MANAGE_HOST;

  dapiCmd.l3_intf    = l3_intf;
  dapiCmd.dstIpAddr  = ipAddr;
  memcpy(dapiCmd.dstMacAddr, mac, sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_L3, (void *) &dapiCmd);
}

/**
 * L3 Host Remove
 * 
 * @param intIfNum 
 * @param l3_intf 
 * @param ipAddr 
 * @param mac 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t dtlPtinL3HostRemove(L7_uint32 intIfNum, L7_int l3_intf, L7_uint32 ipAddr, L7_char8 *mac)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  st_ptin_l3 dapiCmd;

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

  memset(&dapiCmd, 0x00, sizeof(st_ptin_l3));

  dapiCmd.cmd   = DAPI_CMD_CLEAR;
  dapiCmd.oper  = PTIN_L3_MANAGE_HOST;

  dapiCmd.l3_intf    = l3_intf;
  dapiCmd.dstIpAddr  = ipAddr;
  memcpy(dapiCmd.dstMacAddr, mac, sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_L3, (void *) &dapiCmd);
}

/**
 * L3 Route Add
 * 
 * @param intIfNum 
 * @param l3_intf 
 * @param ipAddr 
 * @param mac 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t dtlPtinL3RouteAdd(L7_uint32 intIfNum, L7_int l3_intf, L7_uint32 ipAddr, L7_uint32 ipMask, L7_char8 *mac)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  st_ptin_l3 dapiCmd;

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

  memset(&dapiCmd, 0x00, sizeof(st_ptin_l3));

  dapiCmd.cmd   = DAPI_CMD_SET;
  dapiCmd.oper  = PTIN_L3_MANAGE_ROUTE;

  dapiCmd.l3_intf    = l3_intf;
  dapiCmd.dstIpAddr  = ipAddr;
  dapiCmd.dstIpMask  = ipMask;
  memcpy(dapiCmd.dstMacAddr, mac, sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_L3, (void *) &dapiCmd);
}

/**
 * L3 Route Remove
 * 
 * @param intIfNum 
 * @param l3_intf 
 * @param ipAddr 
 * @param mac 
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t dtlPtinL3RouteRemove(L7_uint32 intIfNum, L7_int l3_intf, L7_uint32 ipAddr, L7_uint32 ipMask, L7_char8 *mac)
{
  DAPI_USP_t ddUsp;
  nimUSP_t usp;
  st_ptin_l3 dapiCmd;

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

  memset(&dapiCmd, 0x00, sizeof(st_ptin_l3));

  dapiCmd.cmd   = DAPI_CMD_CLEAR;
  dapiCmd.oper  = PTIN_L3_MANAGE_ROUTE;

  dapiCmd.l3_intf    = l3_intf;
  dapiCmd.dstIpAddr  = ipAddr;
  dapiCmd.dstIpMask  = ipMask;
  memcpy(dapiCmd.dstMacAddr, mac, sizeof(L7_uchar8)*L7_MAC_ADDR_LEN);

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_L3, (void *) &dapiCmd);
}



/**
 * dtlPtinMEPControl
 * 
 * @author mruas (2/26/2015)
 * 
 * @param intIfNum 
 * @param dapiCmd 
 * 
 * @return L7_RC_t 
 */
L7_RC_t dtlPtinMEPControl(L7_uint32 intIfNum, hapi_mep_t *dapiCmd)
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

  return dapiCtl(&ddUsp, DAPI_CMD_PTIN_MEP_CTRL, (void *) dapiCmd);
}

