#include "ptin_xconnect_api.h"
#include "logger.h"

#include "ptin_globaldefs.h"
#include "ptin_structs.h"
#include "ptin_intf.h"

#include "dtlinclude.h"

/***************************************************************** 
 * INTERNAL VARIABLES
 *****************************************************************/

/***************************************************************** 
 * INTERNAL FUNCTION PROTOTYPES
 *****************************************************************/

 /**
 * Create a multicast group
 * 
 * @param vlanId : VLAN to be associated with 
 * @param multicast_flag: Multicast flags   
 * @param mcast_group   : Multicast group id to be returned. 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t ptin_multicast_group_create(L7_uint16 vlanId, L7_uint32 multicast_flag, L7_int *mcast_group);

/**
 * Remove port from Multicast egress
 * 
 * @param intIfNum    : interface to be removed
 * @param mcast_group : Multicast group id. 
 * @param multicast_flag: Multicast flags 
 * @param virtual_gport     : Virtual Port Identifier 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t ptin_multicast_egress_port_remove(L7_uint32 intIfNum, L7_int mcast_group, L7_uint32 multicast_flag, L7_int virtual_gport);

/**
 * Add ports to Multicast egress
 * 
 * @param intIfNum    : interface to be added
 * @param mcast_group : Multicast group id. 
 * @param multicast_flag: Multicast flags 
 * @param virtual_gport     : Virtual Port Identifier
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t ptin_multicast_egress_port_add(L7_uint32 intIfNum, L7_int mcast_group, L7_uint32 multicast_flag, L7_int virtual_gport);

/***************************************************************** 
 * EXTERNAL FUNCTIONS IMPLEMENTATION
 *****************************************************************/

/**
 * Apply or remove CPU from a Vlan ID
 * 
 * @param vlanId : Vlan Id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_vlan_cpu_set(L7_uint16 vlanId, L7_BOOL enable)
{
  ptin_vlan_mode_t vlan_mode;

  vlan_mode.vlanId = vlanId;
  vlan_mode.cpu_include = enable & 1;

  return dtlPtinVlanSettings(&vlan_mode);
}

/**
 * Add ports to a specific vlan
 *  
 * @param ptin_port : PTIN port format
 * @param vlanId : Vlan Id (0 to apply to all existent)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_vlan_port_add(L7_uint32 ptin_port, L7_uint16 vlanId)
{
  L7_uint32 intIfNum;
  ptin_vlan_mode_t vlan_mode;

  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  vlan_mode.oper = DAPI_CMD_SET;
  vlan_mode.vlanId = (vlanId > 4095) ? 0 : vlanId;
  vlan_mode.cpu_include = 0;
  vlan_mode.ddUsp.unit = vlan_mode.ddUsp.slot = vlan_mode.ddUsp.port = -1;

  return dtlPtinVlanPortControl(intIfNum, &vlan_mode);
}

/**
 * Remove port from a specific vlan
 *  
 * @param ptin_port : PTIN port format
 * @param vlanId : Vlan Id (0 to apply to all existent)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_vlan_port_remove(L7_uint32 ptin_port, L7_uint16 vlanId)
{
  L7_uint32 intIfNum;
  ptin_vlan_mode_t vlan_mode;

  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  vlan_mode.oper = DAPI_CMD_CLEAR;
  vlan_mode.vlanId = (vlanId > 4095) ? 0 : vlanId;
  vlan_mode.cpu_include = 0;
  vlan_mode.ddUsp.unit = vlan_mode.ddUsp.slot = vlan_mode.ddUsp.port = -1;

  return dtlPtinVlanPortControl(intIfNum, &vlan_mode);
}

/**
 * Remove port from a specific vlan, and flush mac table
 *  
 * @param ptin_port : PTIN port format
 * @param vlanId : Vlan Id (0 to apply to all existent)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_vlan_port_removeFlush(L7_uint32 ptin_port, L7_uint16 vlanId)
{
  L7_uint32 intIfNum;
  ptin_vlan_mode_t vlan_mode;

  if (ptin_port >= PTIN_SYSTEM_N_INTERF ||
      ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  vlan_mode.oper = DAPI_CMD_CLEAR_ALL;
  vlan_mode.vlanId = (vlanId > 4095) ? 0 : vlanId;
  vlan_mode.cpu_include = 0;
  vlan_mode.ddUsp.unit = vlan_mode.ddUsp.slot = vlan_mode.ddUsp.port = -1;

  return dtlPtinVlanPortControl(intIfNum, &vlan_mode);
}

/**
 * Remove port from a specific vlan, and flush mac table
 *  
 * @param ptin_port_old : Previous port 
 * @param ptin_port_new : New port 
 * @param vlanId : Vlan Id (0 to apply to all existent)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_vlan_port_switch(L7_uint32 ptin_port_old, L7_uint32 ptin_port_new, L7_uint16 vlanId)
{
  nimUSP_t usp;
  L7_uint32 intIfNum_new, intIfNum_prev;
  ptin_vlan_mode_t vlan_mode;

  if (ptin_port_old >= PTIN_SYSTEM_N_INTERF ||
      ptin_intf_port2intIfNum(ptin_port_old, &intIfNum_prev) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid ptin_port_old %u", ptin_port_old);
    return L7_FAILURE;
  }

  if (ptin_port_new >= PTIN_SYSTEM_N_INTERF ||
      ptin_intf_port2intIfNum(ptin_port_new, &intIfNum_new) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid ptin_port_new %u", ptin_port_new);
    return L7_FAILURE;
  }

  /* Port to be removed */
  if (nimGetUnitSlotPort(intIfNum_prev, &usp) != L7_SUCCESS)
    return L7_FAILURE;

  vlan_mode.ddUsp.unit = usp.unit;
  vlan_mode.ddUsp.slot = usp.slot;
  vlan_mode.ddUsp.port = usp.port - 1;

  vlan_mode.oper = DAPI_CMD_SET;
  vlan_mode.vlanId = (vlanId > 4095) ? 0 : vlanId;
  vlan_mode.cpu_include = 0;

  PT_LOG_TRACE(LOG_CTX_API, "ptin_port_old %u / usp={%d,%d,%d}", ptin_port_old,
            vlan_mode.ddUsp.unit, vlan_mode.ddUsp.slot, vlan_mode.ddUsp.port);

  return dtlPtinVlanPortControl(intIfNum_new, &vlan_mode);
}

/**
 * Configure flooding settings
 *  
 * @param lif    : LIF id 
 * @param vlanId : Vlan id
 * @param mcgroup_unkn_uc : Unknown UC group id. 
 * @param mcgroup_unkn_mc : Unknown MC group id. 
 * @param mcgroup_bc : Broadcast group id. 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_vlanBridge_flood_set(L7_uint32 lif, L7_uint16 vlanId, L7_int mcgroup_unkn_uc, L7_int mcgroup_unkn_mc, L7_int mcgroup_bc)
{
  ptin_bridge_vlan_mode_t mode;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_API, "lif=0x%x, vlanId=%u, mcgroup_unkn_uc=%d mcgroup_unkn_mc=%d mcgroup_bc=%d", lif, vlanId,
               mcgroup_unkn_uc, mcgroup_unkn_mc, mcgroup_bc);

  /* Fill structure */
  memset(&mode, 0x00, sizeof(mode));
  mode.lif_id = lif;
  mode.vlanId = vlanId;
  mode.multicast_group       = mcgroup_unkn_uc;
  mode.mcgroup_flood_unkn_uc = mcgroup_unkn_uc;
  mode.mcgroup_flood_unkn_mc = mcgroup_unkn_mc;
  mode.mcgroup_flood_bc      = mcgroup_bc;
  mode.mask   = PTIN_BRIDGE_VLAN_MODE_MASK_FLOOD;
  if (vlanId != 0)
  {
    mode.mask |= PTIN_BRIDGE_VLAN_MODE_MASK_MC_GROUP;
  }

  /* DTL call */
  rc = dtlPtinVlanDefinitions(&mode);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Remove multicast group association to vlan
 * 
 * @param vlanId : Vlan id
 * @param mcast_group : Multicast group id.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_vlanBridge_multicast_clear(L7_uint16 vlanId, L7_int mcast_group)
{
  L7_uint32 flags = 0;
  ptin_bridge_vlan_multicast_t mode;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( vlanId == 0 || mcast_group <= 0)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments (vlanId=%u, mcast_group=%d)",vlanId,mcast_group);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_API, "vlanId=%u, mcast_group=%u", vlanId, mcast_group);

  #if (PTIN_BOARD_IS_DNX)
  flags |= BCM_MULTICAST_TYPE_L2;
  #else
  flags |= BCM_MULTICAST_TYPE_VLAN;
  #endif

  /* Fill structure */
  mode.oper             = DAPI_CMD_CLEAR;
  mode.vlanId           = vlanId;
  mode.multicast_group  = mcast_group;
  mode.destroy_on_clear = L7_FALSE;
  mode.multicast_flag   = flags;

  /* DTL call */
  rc = dtlPtinVlanBridgeMulticast(&mode);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d", rc);

  return rc;
}

L7_int ptin_debug_multicast_group_l3_create(void)
{
  L7_int  mcastGroup = -1;
  ptin_multicast_group_create((L7_uint16) -1, BCM_MULTICAST_TYPE_L3, &mcastGroup);
  return mcastGroup;
}

/**
 * Create a L3 multicast group
 * 
 * @param mcast_group   : Multicast group id to be returned. 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_multicast_group_l3_create(L7_int *mcast_group)
{
  L7_int  mcastGroup = -1;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( mcast_group == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }

  rc = ptin_multicast_group_create((L7_uint16) -1, BCM_MULTICAST_TYPE_L3, &mcastGroup);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Failed to Create Multicast Group: rc=%d", rc);
    return rc;
  }

  *mcast_group = mcastGroup;
  
  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (new MC group=%d)", rc, mcastGroup);

  return rc;
}

/**
 * Create a VLAN multicast group
 *  
 * @param vlanId : VLAN to be associated with.  
 * @param mcast_group   : Multicast group id to be returned. 
 * @param flags : Flags 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_multicast_group_vlan_create(L7_uint16 vlanId, L7_int *mcast_group, L7_uint32 flags)
{
  L7_int    mcastGroup = -1;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( mcast_group == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }

  #if (PTIN_BOARD_IS_DNX)
  flags |= BCM_MULTICAST_TYPE_L2;
  #else
  flags |= BCM_MULTICAST_TYPE_VLAN;
  #endif

  rc = ptin_multicast_group_create(vlanId, flags, &mcastGroup);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Failed to Create Multicast Group: rc=%d", rc);
    return rc;
  }

  *mcast_group = mcastGroup;
  
  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (new MC group=%d)", rc, mcastGroup);

  return rc;
}

/**
 * Create a multicast group
 *  
 * @param vlanId : VLAN to be associated with 
 * @param multicast_flag: Multicast flags   
 * @param mcast_group   : Multicast group id to be returned. 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t ptin_multicast_group_create(L7_uint16 vlanId, L7_uint32 multicast_flag, L7_int *mcast_group)
{
  ptin_bridge_vlan_multicast_t mode;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( mcast_group == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Fill structure */
  mode.oper             = DAPI_CMD_SET;
  mode.vlanId           = vlanId;
  mode.multicast_group  = *mcast_group;
  mode.destroy_on_clear = L7_FALSE;
  mode.multicast_flag   = multicast_flag;

  /* DTL call */
  rc = dtlPtinVlanBridgeMulticast(&mode);

  /* Return output */
  if (rc == L7_SUCCESS)
  {
    *mcast_group = mode.multicast_group;
  }

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (new MC group=%d)", rc, mode.multicast_group);

  return rc;
}

/**
 * Destroy a multicast group
 * 
 * @param mcast_group : Multicast group id to be destroyed. 
 * @param flags : Flags   
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_multicast_group_destroy(L7_int mcast_group, L7_uint flags)
{
  ptin_bridge_vlan_multicast_t mode;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( mcast_group <= 0)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid mc_group %d", mcast_group);
    return L7_FAILURE;
  }

  #if (PTIN_BOARD_IS_DNX)
  flags |= BCM_MULTICAST_TYPE_L2;
  #else
  flags |= BCM_MULTICAST_TYPE_VLAN;
  #endif

  /* Fill structure */
  mode.oper             = DAPI_CMD_CLEAR;
  mode.vlanId           = 0;
  mode.multicast_group  = mcast_group;
  mode.destroy_on_clear = L7_TRUE;
  mode.multicast_flag   = flags;

  /* DTL call */
  rc = dtlPtinVlanBridgeMulticast(&mode);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (MC group=%d)", rc, mode.multicast_group);

  return rc;
}

/**
 * Add ports to Multicast egress
 * 
 * @param intIfNum    : interface to be added
 * @param mcast_group : Multicast group id. 
 * @param multicast_flag: Multicast flags 
 * @param virtual_gport     : Virtual Port
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_multicast_egress_port_add(L7_uint32 intIfNum, L7_int mcast_group, L7_uint32 multicast_flag, L7_int virtual_gport)
{
  ptin_bridge_vlan_multicast_t mode;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( intIfNum == 0 || intIfNum >= L7_ALL_INTERFACES || mcast_group <= 0)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments (intIfNum=%u)", intIfNum);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_API, "intIfNum=%u, mcast_group=%u", intIfNum, mcast_group);

  /* Fill structure */
  mode.oper             = DAPI_CMD_SET;
  mode.virtual_gport    = virtual_gport;  
  mode.multicast_group  = mcast_group;
  mode.destroy_on_clear = L7_FALSE;
  mode.multicast_flag   = multicast_flag;

  /* DTL call */
  rc = dtlPtinMulticastEgressPort(intIfNum, &mode);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (new MC group=%d)", rc, mode.multicast_group);

  return rc;
}

/**
 * Add port from Multicast egress
 * 
 * @param intIfNum    : interface to be removed
 * @param mcast_group : Multicast group id. 
 * @param multicast_flag: Multicast flags 
 * @param virtual_gport     : Virtual Port Identifier 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_multicast_egress_port_remove(L7_uint32 intIfNum, L7_int mcast_group, L7_uint32 multicast_flag, L7_int virtual_gport)
{
  ptin_bridge_vlan_multicast_t mode;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( intIfNum == 0 || intIfNum >= L7_ALL_INTERFACES || mcast_group <= 0)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments (intIfNum=%u, mcast_group=%d)", intIfNum, mcast_group);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_API, "intIfNum=%u, mcast_group=%u", intIfNum, mcast_group);

  /* Fill structure */
  mode.oper             = DAPI_CMD_CLEAR;
  mode.virtual_gport    = virtual_gport;
  mode.multicast_group  = mcast_group;
  mode.destroy_on_clear = L7_FALSE;
  mode.multicast_flag   = multicast_flag;

  /* DTL call */
  rc = dtlPtinMulticastEgressPort(intIfNum, &mode);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Add ports to Multicast egress
 * 
 * @param intIfNum    : interface to be added
 * @param mcast_group : Multicast group id. 
 * @param l3_intf_id  : L3 Intf Id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_multicast_l3_egress_port_add(L7_uint32 intIfNum, L7_int mcast_group, L7_int l3_intf_id)
{ 
  return (ptin_multicast_egress_port_add(intIfNum, mcast_group, BCM_MULTICAST_TYPE_L3, l3_intf_id));
}

/**
 * Remove port from Multicast egress
 * 
 * @param intIfNum    : interface to be removed
 * @param mcast_group : Multicast group id. 
 * @param l3_intf_id  : L3 Intf Id*
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_multicast_l3_egress_port_remove(L7_uint32 intIfNum, L7_int mcast_group, L7_int l3_intf_id)
{
  return (ptin_multicast_egress_port_remove(intIfNum, mcast_group, BCM_MULTICAST_TYPE_L3, l3_intf_id));
}

/**
 * Clean Multicast egress entries
 * 
 * @param intIfNum    : interface to be removed
 * @param mcast_group : Multicast group id.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_multicast_egress_clean(L7_int mcast_group)
{
  ptin_bridge_vlan_multicast_t mode;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( mcast_group <= 0)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments (mcast_group=%d)", mcast_group);
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_API, "mcast_group=%u", mcast_group);

  /* Fill structure */
  mode.oper             = DAPI_CMD_CLEAR_ALL;
  mode.vlanId           = -1;
  mode.multicast_group  = mcast_group;
  mode.destroy_on_clear = L7_FALSE;
  mode.multicast_flag   = (BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_TYPE_L2);

  /* DTL call */
  rc = dtlPtinMulticastEgressPort(L7_ALL_INTERFACES, &mode);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d", rc);

  return rc;
}

#if (PTIN_BOARD_IS_DNX)
/**
 * Create VSI
 * 
 * @param vsi 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_vsi_create(L7_uint16 vsi)
{
  L7_RC_t rc;
  ptinDtlVSI_t vsi_data;

  memset(&vsi_data, 0x00, sizeof(vsi_data));

  vsi_data.vsi = vsi;

  PT_LOG_TRACE(LOG_CTX_API, "Going to create vsi %u", vsi);

  rc = dtlPtinGeneric(L7_ALL_INTERFACES, PTIN_DTL_MSG_VSI, DAPI_CMD_SET, sizeof(ptinDtlVSI_t), &vsi_data);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error creating VSI %u: error %d", vsi, rc); 
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_API, "VSI %u created", vsi); 
  }
  
  return rc;
}

/**
 * Remove VSI
 * 
 * @param vsi 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_vsi_destroy(L7_uint16 vsi)
{
  L7_RC_t rc;
  ptinDtlVSI_t vsi_data;

  memset(&vsi_data, 0x00, sizeof(vsi_data));

  vsi_data.vsi = vsi;

  PT_LOG_TRACE(LOG_CTX_API, "Going to destroy vsi %u", vsi);

  rc = dtlPtinGeneric(L7_ALL_INTERFACES, PTIN_DTL_MSG_VSI, DAPI_CMD_CLEAR, sizeof(ptinDtlVSI_t), &vsi_data);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error destroying VSI %u: error %d", vsi, rc); 
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_API, "VSI %u destroyed", vsi); 
  }
  
  return rc;
}

/**
 * Add VLAN port to VSI
 * 
 * @param vsi 
 * @param vlan_port_id 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_vsi_member_add(L7_uint16 vsi, L7_uint32 vlan_port_id)
{
  L7_RC_t rc;
  ptinDtlVSI_t vsi_data;

  memset(&vsi_data, 0x00, sizeof(vsi_data));

  vsi_data.vsi = vsi;
  vsi_data.vlan_port_id = vlan_port_id;

  PT_LOG_TRACE(LOG_CTX_API, "Going to add virtual port 0x%x to vsi %u", vlan_port_id, vsi);

  rc = dtlPtinGeneric(L7_ALL_INTERFACES, PTIN_DTL_MSG_VSI_MEMBER, DAPI_CMD_SET, sizeof(ptinDtlVSI_t), &vsi_data);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error adding vlan port 0x%x to VSI %u: error %d", vlan_port_id, vsi, rc); 
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_API, "Vlan port 0x%x added to VSI %u", vlan_port_id, vsi); 
  }
  
  return rc;
}

/**
 * Remove VLAN PORT from VSI
 * 
 * @param vsi 
 * @param vlan_port_id 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_vsi_member_remove(L7_uint16 vsi, L7_uint32 vlan_port_id)
{
  L7_RC_t rc;
  ptinDtlVSI_t vsi_data;

  memset(&vsi_data, 0x00, sizeof(vsi_data));

  vsi_data.vsi = vsi;
  vsi_data.vlan_port_id = vlan_port_id;

  PT_LOG_TRACE(LOG_CTX_API, "Going to remove vlan port 0x%x to vsi %u", vlan_port_id, vsi);

  rc = dtlPtinGeneric(L7_ALL_INTERFACES, PTIN_DTL_MSG_VSI_MEMBER, DAPI_CMD_CLEAR, sizeof(ptinDtlVSI_t), &vsi_data);

  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Error removing vlan port 0x%x to VSI %u: error %d", vlan_port_id, vsi, rc); 
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_API, "Vlan port 0x%x remved from VSI %u", vlan_port_id, vsi); 
  }
  
  return rc;
}
#endif

/**
 * Create Virtual port
 * 
 * @param intIfNum    : interface to be added 
 * @param ext_ovid    : External outer vlan (GEMid)
 * @param ext_ivid    : External inner vlan (UNIVLAN)
 * @param int_ovid    : Internal outer vlan 
 * @param int_ivid    : Internal inner vlan  
 * @param mcast_group : Multicast group id. 
 * @param vport_id    : vport id 
 * @param macLearnMax : Maximum Learned MACs
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_virtual_port_add(L7_uint32 intIfNum,
                              L7_int ext_ovid, L7_int ext_ivid,
                              L7_int int_ovid, L7_int int_ivid,
                              L7_int mcast_group,
                              L7_int *vport_id,
                              L7_uint8 macLearnMax)
{
  ptin_vport_t vport;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( intIfNum == 0 || intIfNum >= L7_ALL_INTERFACES
       /*|| int_ovid <= 0 || int_ovid >= 4095
       || ext_ovid <= 0 || ext_ovid >= 4095 
       || mcast_group <= 0*/)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_API, "intIfNum=%u, int_ovid=%d, int_ivid=%d, ext_ovid=%d, ext_ivid=%d, mcast_group=%u, macLearnMax=%u",
            intIfNum, int_ovid, int_ivid, ext_ovid, ext_ivid, mcast_group, macLearnMax);

  /* Fill structure */
  memset(&vport, 0x00, sizeof(vport));
  vport.oper             = DAPI_CMD_SET;
  vport.cmd              = PTIN_VPORT_CMD_VP_OPER;
  vport.int_ovid         = int_ovid;
  vport.int_ivid         = int_ivid;
  vport.ext_ovid         = ext_ovid;
  vport.ext_ivid         = ext_ivid;
  vport.virtual_gport    = -1;
  vport.multicast_group  = mcast_group;
  vport.macLearnMax      = macLearnMax;
  vport.vsi              = 0;

  /* DTL call */
  rc = dtlPtinVirtualPort(intIfNum, &vport);

  if (rc == L7_SUCCESS)
  {
    if (vport.virtual_gport <= 0)
    {
      PT_LOG_ERR(LOG_CTX_API, "Finished: Invalid vport id %d (MC group=%d)", rc, vport.virtual_gport, vport.multicast_group);
      return L7_FAILURE;
    }
    /* Return vport id */
    if (vport_id != L7_SUCCESS)
      *vport_id = vport.virtual_gport;
  }

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (new MC group=%d, vport=%d)", rc, vport.multicast_group, vport.virtual_gport);

  return rc;
}

/**
 * Configure the Maximum Learned MACs foa a Virtual port
 * 
 * @param intIfNum    : interface to be confgured
 * @param vport_id    : vport id 
 * @param macLearnMax : Maximum Learned MACs
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_virtual_macLearnMax_set(L7_uint32 intIfNum, L7_int vport_id, L7_uint8 macLearnMax)
{
  ptin_vport_t vport;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_API, "vport_id=0x%x, macLearnMax=%u", vport_id, macLearnMax);

  /* Fill structure */

  if ( macLearnMax != (L7_uint8)-1 )
  {
      vport.oper         = DAPI_CMD_SET;
  }
  else
  {
      vport.oper         = DAPI_CMD_CLEAR;
  }
  vport.cmd              = PTIN_VPORT_CMD_MAXMAC_SET;
  vport.virtual_gport    = vport_id;
  vport.macLearnMax      = macLearnMax;

  /* DTL call */
  rc = dtlPtinVirtualPort(intIfNum, &vport);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (new MC group=%d, vport=%d)", rc, vport.multicast_group, vport.virtual_gport);

  return rc;
}

/**
 * Remove Virtual port
 * 
 * @param intIfNum      : interface to be removed
 * @param virtual_gport : Virtual port id 
 * @param mcast_group   : Multicast group id.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_virtual_port_remove(L7_uint32 intIfNum, L7_int virtual_gport, L7_int mcast_group)
{
  ptin_vport_t vport;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( intIfNum == 0 || intIfNum >= L7_ALL_INTERFACES ||
       virtual_gport <= 0 /*|| mcast_group <= 0*/)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_API, "intIfNum=%u, virtual_gport=%d, mcast_group=%u",
            intIfNum, virtual_gport, mcast_group);

  /* Fill structure */
  vport.oper             = DAPI_CMD_CLEAR;
  vport.cmd              = PTIN_VPORT_CMD_VP_OPER;
  vport.int_ovid         = -1;
  vport.int_ivid         = -1;
  vport.ext_ovid         = -1;
  vport.ext_ivid         = -1;
  vport.virtual_gport    = virtual_gport;
  vport.multicast_group  = mcast_group;
  vport.macLearnMax      = (L7_uint8) -1;

  /* DTL call */
  rc = dtlPtinVirtualPort(intIfNum, &vport);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Define MAC Learning for a particular Vlan, and its forwarding 
 * vlan 
 * 
 * @param vlanId : Outer Vlan Id
 * @param fwdVlanId : Forward vlan (vlan to use for mac 
 *                  learning)
 * @param macLearn : MAc learning on/off
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_crossconnect_vlan_learn(L7_uint16 vlanId, L7_uint16 fwdVlanId, L7_int mcast_group, L7_BOOL macLearn)
{
  ptin_bridge_vlan_mode_t mode;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_API, "vlanId=%u, fwdVlanId=%u, macLearn=%u", vlanId, fwdVlanId, macLearn);
  /* Validate arguments */
  if (vlanId == 0 || fwdVlanId == 0)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Fill structure */
  memset(&mode, 0x00, sizeof(mode));
  mode.vlanId          = vlanId;
  mode.fwdVlanId       = fwdVlanId;
  mode.learn_enable    = macLearn & 1;
  mode.mask         = PTIN_BRIDGE_VLAN_MODE_MASK_FWDVLAN | PTIN_BRIDGE_VLAN_MODE_MASK_LEARN_EN;
  if (mcast_group > 0)
  {
    mode.multicast_group = mcast_group;
    mode.mask |= PTIN_BRIDGE_VLAN_MODE_MASK_MC_GROUP;
  }

  /* DTL call */
  rc = dtlPtinVlanDefinitions(&mode);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Use crossconnects, instead of the MAC+Vlan bridging, for a 
 * particular Vlan 
 * 
 * @param vlanId : Vlan id
 * @param crossconnect_apply : use crossconnects
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_crossconnect_enable(L7_uint16 vlanId, L7_BOOL crossconnect_apply, L7_BOOL double_tag)
{
  ptin_bridge_vlan_mode_t mode;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_API, "vlanId=%u, crossconnect=%u", vlanId, crossconnect_apply);
  /* Validate arguments */
  if ( vlanId == 0 )
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Fill structure */
  memset(&mode, 0x00, sizeof(mode));
  mode.vlanId                 = vlanId;
  mode.double_tag             = double_tag;
  mode.cross_connects_enable  = crossconnect_apply & 1;
  mode.mask                   = PTIN_BRIDGE_VLAN_MODE_MASK_CROSSCONN_EN;

  /* DTL call */
  rc = dtlPtinVlanDefinitions(&mode);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Add a new crossconnect.
 * 
 * @param outerVlanId : outer vlan id
 * @param innerVlanId : inner vlan id
 * @param intIfNum1 : First interface
 * @param intIfNum2 : Second interface
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_crossconnect_add(L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint32 intIfNum1, L7_uint32 intIfNum2)
{
  ptin_bridge_crossconnect_t cc;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_API, "outerVlanId=%u, innerVlanId=%u, intIfNum1=%u, intIfNum2=%u",
            outerVlanId, innerVlanId, intIfNum1, intIfNum2);
  /* Validate arguments */
  if ( outerVlanId > 4095 || innerVlanId > 4095 )
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Fill structure */
  cc.outerVlanId = outerVlanId;
  cc.innerVlanId = innerVlanId;
  cc.oper        = DAPI_CMD_SET;
  cc.dstUsp.unit = cc.dstUsp.slot = cc.dstUsp.port = -1;  /* intIfNum2 will reconfigure this parameter */

  /* DTL call */
  rc = dtlPtinBridgeCrossconnect(intIfNum1, intIfNum2, &cc);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Delete a crossconnect.
 * 
 * @param outerVlanId : outer vlan id
 * @param innerVlanId : inner vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_crossconnect_delete(L7_uint16 outerVlanId, L7_uint16 innerVlanId)
{
  ptin_bridge_crossconnect_t cc;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_API,"outerVlanId=%u, innerVlanId=%u", outerVlanId, innerVlanId);
  /* Validate arguments */
  if ( outerVlanId > 4095 || innerVlanId > 4095 )
  {
    PT_LOG_ERR(LOG_CTX_API,"Invalid arguments");
    return L7_FAILURE;
  }

  cc.dstUsp.unit = -1;
  cc.dstUsp.slot = -1;
  cc.dstUsp.port = -1;
  cc.outerVlanId = outerVlanId;
  cc.innerVlanId = innerVlanId;
  cc.oper        = DAPI_CMD_CLEAR;

  /* DTL call */
  rc = dtlPtinBridgeCrossconnect(L7_ALL_INTERFACES, L7_ALL_INTERFACES, &cc);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Delete all crossconnects.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_crossconnect_delete_all(void)
{
  ptin_bridge_crossconnect_t cc;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_API, "Beginning...");

  cc.dstUsp.unit = -1;
  cc.dstUsp.slot = -1;
  cc.dstUsp.port = -1;
  cc.outerVlanId = 0;
  cc.innerVlanId = 0;
  cc.oper        = DAPI_CMD_CLEAR_ALL;

  /* DTL call */
  rc = dtlPtinBridgeCrossconnect(L7_ALL_INTERFACES,L7_ALL_INTERFACES,&cc);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d", rc);

  return rc;
}

/***************************************************************** 
 * STATIC FUNCTIONS IMPLEMENTATION
 *****************************************************************/

