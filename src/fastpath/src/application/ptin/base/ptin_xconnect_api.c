#include "ptin_xconnect_api.h"
#include "logger.h"

#include "ptin_globaldefs.h"
#include "ptin_structs.h"
#include "ptin_intf.h"

#include "dtlinclude.h"

#include "usmdb_dot3ad_api.h"

/***************************************************************** 
 * INTERNAL VARIABLES
 *****************************************************************/

/***************************************************************** 
 * INTERNAL FUNCTION PROTOTYPES
 *****************************************************************/

 /**
 * Create a multicast group
 * 
 * @param mcast_group   : Multicast group id to be returned. 
 * @param multicast_flag: Multicast flags  
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t ptin_multicast_group_create(L7_int *mcast_group, L7_uint32 multicast_flag);

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
 * Associate a multicast group to a vlan
 * 
 * @param vlanId : Vlan id
 * @param mcast_group : Multicast group id.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_vlanBridge_multicast_set(L7_uint16 vlanId, L7_int mcast_group)
{
  ptin_bridge_vlan_multicast_t mode;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( vlanId == 0 || vlanId > 4095 || mcast_group <= 0)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments (vlanId=%u)",vlanId);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_API, "vlanId=%u, mcast_group=%u", vlanId, mcast_group);

  /* Fill structure */
  mode.oper             = DAPI_CMD_SET;
  mode.vlanId           = vlanId;
  mode.multicast_group  = mcast_group;
  mode.destroy_on_clear = L7_FALSE;
  mode.multicast_flag   = BCM_MULTICAST_TYPE_VLAN;

  /* DTL call */
  rc = dtlPtinVlanBridgeMulticast(&mode);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (new MC group=%d)", rc, mode.multicast_group);

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
  ptin_bridge_vlan_multicast_t mode;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( vlanId == 0 || vlanId > 4095 || mcast_group <= 0)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments (vlanId=%u, mcast_group=%d)",vlanId,mcast_group);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_API, "vlanId=%u, mcast_group=%u", vlanId, mcast_group);

  /* Fill structure */
  mode.oper             = DAPI_CMD_CLEAR;
  mode.vlanId           = vlanId;
  mode.multicast_group  = mcast_group;
  mode.destroy_on_clear = L7_FALSE;
  mode.multicast_flag   = BCM_MULTICAST_TYPE_VLAN;

  /* DTL call */
  rc = dtlPtinVlanBridgeMulticast(&mode);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d", rc);

  return rc;
}

L7_int ptin_debug_multicast_group_l3_create(void)
{
  L7_int  mcastGroup = -1;
  ptin_multicast_group_create(&mcastGroup, BCM_MULTICAST_TYPE_L3);
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

  rc = ptin_multicast_group_create(&mcastGroup, BCM_MULTICAST_TYPE_L3);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Failed to Create Multicast Group: rc=%d", rc);
    return rc;
  }

  *mcast_group = mcastGroup;
  
  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (new MC group=0x%08x)", rc, mcastGroup);

  return rc;
}

/**
 * Create a L2 multicast group
 * 
 * @param mcast_group   : Multicast group id to be returned. 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_multicast_group_l2_create(L7_int *mcast_group)
{
  L7_int  mcastGroup = -1;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( mcast_group == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }

  rc = ptin_multicast_group_create(&mcastGroup, BCM_MULTICAST_TYPE_L2);
  if (rc != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_API, "Failed to Create Multicast Group: rc=%d", rc);
    return rc;
  }

  *mcast_group = mcastGroup;
  
  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (new MC group=0x%08x)", rc, mcastGroup);

  return rc;
}

/**
 * Create a VLAN multicast group
 * 
 * @param mcast_group   : Multicast group id to be returned. 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_multicast_group_vlan_create(L7_int *mcast_group)
{
  L7_int  mcastGroup = -1;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( mcast_group == L7_NULLPTR)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }

  rc = ptin_multicast_group_create(&mcastGroup, BCM_MULTICAST_TYPE_VLAN);
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
 * @param mcast_group   : Multicast group id to be returned. 
 * @param multicast_flag: Multicast flags  
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t ptin_multicast_group_create(L7_int *mcast_group, L7_uint32 multicast_flag)
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
  mode.vlanId           = -1;
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

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (new MC group=0x%08x)", rc, mode.multicast_group);

  return rc;
}

/**
 * Destroy a multicast group
 * 
 * @param mcast_group : Multicast group id to be destroyed.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_multicast_group_destroy(L7_int mcast_group)
{
  ptin_bridge_vlan_multicast_t mode;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( mcast_group <= 0)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid mc_group 0x%08x", mcast_group);
    return L7_FAILURE;
  }

  /* Fill structure */
  mode.oper             = DAPI_CMD_CLEAR;
  mode.vlanId           = -1;
  mode.multicast_group  = mcast_group;
  mode.destroy_on_clear = L7_TRUE;
  mode.multicast_flag   = (BCM_MULTICAST_TYPE_VLAN | BCM_MULTICAST_TYPE_L3);

  /* DTL call */
  rc = dtlPtinVlanBridgeMulticast(&mode);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (removed MC group=0x%08x)", rc, mode.multicast_group);

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
  memset(&mode, 0x00, sizeof(mode));
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
  /*Added to prevent issue OLTTS-16796: a switchover will turn off the (internal) lags (to the linecars) 
    for a short amount of period on the standby matrix. This can cause problems on the multicast because it
    will add the lag without members to the multicast replication table. To prevent it we'll directly add the
    members of the lag to the replication table*/ 
  #if PTIN_BOARD_IS_MATRIX
  if (usmDbDot3adValidIntfCheck(0, intIfNum))
  {
    static L7_uint32 members_list[PTIN_SYSTEM_N_PORTS] = {0}; /* Internal interface numbers of portChannel members */      
    L7_uint32        nElems = sizeof(members_list) / sizeof(members_list[0]);
    L7_uint32        i;
    L7_RC_t          rc = L7_FAILURE;

    /* Get list of active ports */
    if (usmDbDot3adMemberListGet(1, intIfNum, &nElems, members_list) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error reading Active Members List", intIfNum);
      return L7_FAILURE;
    }

    for (i=0; i<nElems; i++)
    {
      rc = ptin_multicast_egress_port_add(members_list[i], mcast_group, BCM_MULTICAST_TYPE_L3, l3_intf_id);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_API, "Failed to add L3 egress port: intIfNum=%u/lagIntIfNum=%u mcast_group:0x%x l3_intf_id:%d", members_list[i], intIfNum,  mcast_group, l3_intf_id);
//      return rc;
      }     
    }
    if (!nElems)
    {
      PT_LOG_WARN(LOG_CTX_API, "Number of  elements of lagIntIfNum:%u is equal to zero.", intIfNum);
      return (ptin_multicast_egress_port_add(intIfNum, mcast_group, BCM_MULTICAST_TYPE_L3, l3_intf_id));
    }
    else
    {
      return rc;
    }
  }
  else
  #endif
  {
    return (ptin_multicast_egress_port_add(intIfNum, mcast_group, BCM_MULTICAST_TYPE_L3, l3_intf_id));
  }
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
  /*Added to prevent issue OLTTS-16796: a switchover will turn off the (internal) lags (to the linecars) 
    for a short amount of period on the standby matrix. This can cause problems on the multicast because it
    will add the lag without members to the multicast replication table. To prevent it we'll directly add the
    members of the lag to the replication table*/ 
  #if PTIN_BOARD_IS_MATRIX
  if (usmDbDot3adValidIntfCheck(0, intIfNum))
  {
    static L7_uint32 members_list[PTIN_SYSTEM_N_PORTS] = {0}; /* Internal interface numbers of portChannel members */      
    L7_uint32        nElems = sizeof(members_list) / sizeof(members_list[0]);
    L7_uint32        i;
    L7_RC_t          rc = L7_FAILURE;

    /* Get list of active ports */
    if (usmDbDot3adMemberListGet(1, intIfNum, &nElems, members_list) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error reading Active Members List", intIfNum);
      return L7_FAILURE;
    }

    for (i=0; i<nElems; i++)
    {
      rc = ptin_multicast_egress_port_remove(members_list[i], mcast_group, BCM_MULTICAST_TYPE_L3, l3_intf_id);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_API, "Failed to remove L3 egress port: intIfNum=%u/intIfNum(Lag)=%u mcast_group:0x%x l3_intf_id:%d", members_list[i], intIfNum,  mcast_group, l3_intf_id);
//      return rc;
      }     
    }
    if (!nElems)
    {
      PT_LOG_WARN(LOG_CTX_API, "Number of  elements of lagIntIfNum:%u is equal to zero.", intIfNum);
      return (ptin_multicast_egress_port_remove(intIfNum, mcast_group, BCM_MULTICAST_TYPE_L3, l3_intf_id));
    }
    else
    {
      return rc;
    }
  }
  else
  #endif
  {
    return (ptin_multicast_egress_port_remove(intIfNum, mcast_group, BCM_MULTICAST_TYPE_L3, l3_intf_id));
  }
}

/**
 * Add ports to Multicast egress
 * 
 * @param intIfNum    : interface to be added
 * @param mcast_group : Multicast group id. 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_multicast_l2_egress_port_add(L7_uint32 intIfNum, L7_int mcast_group)
{
  /*Added to prevent issue OLTTS-16796: a switchover will turn off the (internal) lags (to the linecars) 
    for a short amount of period on the standby matrix. This can cause problems on the multicast because it
    will add the lag without members to the multicast replication table. To prevent it we'll directly add the
    members of the lag to the replication table*/ 
  #if PTIN_BOARD_IS_MATRIX
  if (usmDbDot3adValidIntfCheck(0, intIfNum))
  {
    static L7_uint32 members_list[PTIN_SYSTEM_N_PORTS] = {0}; /* Internal interface numbers of portChannel members */      
    L7_uint32        nElems = sizeof(members_list) / sizeof(members_list[0]);
    L7_uint32        i;
    L7_RC_t          rc = L7_FAILURE;

    /* Get list of active ports */
    if (usmDbDot3adMemberListGet(1, intIfNum, &nElems, members_list) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error reading Active Members List", intIfNum);
      return L7_FAILURE;
    }

    for (i=0; i<nElems; i++)
    {
      rc = ptin_multicast_egress_port_add(members_list[i], mcast_group, BCM_MULTICAST_TYPE_L2, -1);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_API, "Failed to add L3 egress port: intIfNum=%u/lagIntIfNum=%u mcast_group:0x%x", members_list[i], intIfNum,  mcast_group);
//      return rc;
      }     
    }
    if (!nElems)
    {
      PT_LOG_WARN(LOG_CTX_API, "Number of  elements of lagIntIfNum:%u is equal to zero.", intIfNum);
      return (ptin_multicast_egress_port_add(intIfNum, mcast_group, BCM_MULTICAST_TYPE_L2, -1));
    }
    else
    {
      return rc;
    }
  }
  else
  #endif
  {
    return (ptin_multicast_egress_port_add(intIfNum, mcast_group, BCM_MULTICAST_TYPE_L2, -1));
  }
}

/**
 * Remove port from Multicast egress
 * 
 * @param intIfNum    : interface to be removed
 * @param mcast_group : Multicast group id. 
 * @param l3_intf_id  : L3 Intf Id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
extern L7_RC_t ptin_multicast_l2_egress_port_remove(L7_uint32 intIfNum, L7_int mcast_group)
{
  /*Added to prevent issue OLTTS-16796: a switchover will turn off the (internal) lags (to the linecars) 
    for a short amount of period on the standby matrix. This can cause problems on the multicast because it
    will add the lag without members to the multicast replication table. To prevent it we'll directly add the
    members of the lag to the replication table*/ 
  #if PTIN_BOARD_IS_MATRIX
  if (usmDbDot3adValidIntfCheck(0, intIfNum))
  {
    static L7_uint32 members_list[PTIN_SYSTEM_N_PORTS] = {0}; /* Internal interface numbers of portChannel members */      
    L7_uint32        nElems = sizeof(members_list) / sizeof(members_list[0]);
    L7_uint32        i;
    L7_RC_t          rc = L7_FAILURE;

    /* Get list of active ports */
    if (usmDbDot3adMemberListGet(1, intIfNum, &nElems, members_list) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_INTF, "LAG# %u: error reading Active Members List", intIfNum);
      return L7_FAILURE;
    }

    for (i=0; i<nElems; i++)
    {
      rc = ptin_multicast_egress_port_remove(members_list[i], mcast_group, BCM_MULTICAST_TYPE_L2, -1);
      if (rc != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_API, "Failed to remove L3 egress port: intIfNum=%u/intIfNum(Lag)=%u mcast_group:0x%x", members_list[i], intIfNum,  mcast_group);
//      return rc;
      }     
    }
    if (!nElems)
    {
      PT_LOG_WARN(LOG_CTX_API, "Number of  elements of lagIntIfNum:%u is equal to zero.", intIfNum);
      return (ptin_multicast_egress_port_remove(intIfNum, mcast_group, BCM_MULTICAST_TYPE_L2, -1));
    }
    else
    {
      return rc;
    }
  }
  else
  #endif
  {
    return (ptin_multicast_egress_port_remove(intIfNum, mcast_group, BCM_MULTICAST_TYPE_L2, -1));
  }
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
  memset(&mode, 0x00, sizeof(mode));
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

/**
 * Create Virtual port
 * 
 * @param intIfNum    : interface to be added 
 * @param ext_ovid    : External outer vlan (GEMid)
 * @param ext_ivid    : External inner vlan (UNIVLAN)
 * @param int_ovid    : Internal outer vlan 
 * @param int_ivid    : Internal inner vlan  
 * @param mcast_group : Multicast group id. 
 * @param l2intf_id   : l2intf id 
 * @param macLearnMax : Maximum Learned MACs
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_virtual_port_add(L7_uint32 intIfNum,
                              L7_int ext_ovid, L7_int ext_ivid,
                              L7_int int_ovid, L7_int int_ivid,
                              L7_int mcast_group,
                              L7_int *l2intf_id,
                              L7_uint8 macLearnMax)
{
  ptin_l2intf_t l2intf;
  L7_RC_t rc = L7_SUCCESS;
  /* Validate arguments */
  if ( intIfNum == 0 || intIfNum >= L7_ALL_INTERFACES || 
       int_ovid <= 0 || int_ovid >= 4095 ||
       ext_ovid <= 0 || ext_ovid >= 4095 ||
       mcast_group <= 0)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_API, "intIfNum=%u, int_ovid=%d, int_ivid=%d, ext_ovid=%d, ext_ivid=%d, mcast_group=%u, macLearnMax=%u",
            intIfNum, int_ovid, int_ivid, ext_ovid, ext_ivid, mcast_group, macLearnMax);

  /* Fill structure */
  l2intf.oper             = DAPI_CMD_SET;
  l2intf.cmd              = PTIN_VPORT_CMD_VP_OPER;
  l2intf.int_ovid         = int_ovid;
  l2intf.int_ivid         = int_ivid;
  l2intf.ext_ovid         = ext_ovid;
  l2intf.ext_ivid         = ext_ivid;
  l2intf.virtual_gport    = -1;
  l2intf.multicast_group  = mcast_group;
  l2intf.macLearnMax      = macLearnMax;

#ifdef NGPON2_SUPPORTED
  L7_uint32  port_id;
  L7_uint8   group_ngpon2_id;

  rc = ptin_intf_intIfNum2port(intIfNum, ext_ovid, &port_id);/* FIXME TC16SXG */
  if ( rc == L7_FAILURE )
  {
    PT_LOG_ERR(LOG_CTX_API, "Error converting intfNum to ptin_port. intfNum = %u", intIfNum);
  }
  else
  {
    rc = ptin_intf_NGPON2_group_check((L7_uint8)port_id, &group_ngpon2_id);
    if ( rc == L7_SUCCESS )
    {
      l2intf.port_id  = group_ngpon2_id;
      l2intf.type     = PTIN_EVC_INTF_NGPON2;
    }
  }
#else
  l2intf.port_id  = intIfNum;
  l2intf.type     = PTIN_EVC_INTF_PHYSICAL;
#endif //NGPON2_SUPPORTED

  /* DTL call */
  rc = dtlPtinVirtualPort(intIfNum, &l2intf);

  if (rc == L7_SUCCESS)
  {
    if (l2intf.virtual_gport <= 0)
    {
      PT_LOG_ERR(LOG_CTX_API, "Finished: Invalid l2intf id %d (MC group=%d)", rc, l2intf.virtual_gport, l2intf.multicast_group);
      return L7_FAILURE;
    }
    /* Return l2intf id */
    if (l2intf_id != L7_SUCCESS)
      *l2intf_id = l2intf.virtual_gport;
  }

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (new MC group=%d, l2intf_id=%d)", rc, l2intf.multicast_group, l2intf.virtual_gport);

  return rc;
}

/**
 * Configure the Maximum Learned MACs foa a Virtual port
 * 
 * @param intIfNum    : interface to be confgured
 * @param l2intf_id   : l2intf id 
 * @param macLearnMax : Maximum Learned MACs
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_virtual_macLearnMax_set(L7_uint32 intIfNum, L7_int l2intf_id, L7_uint8 macLearnMax)
{
  ptin_l2intf_t l2intf;
  L7_RC_t rc = L7_SUCCESS;

  PT_LOG_TRACE(LOG_CTX_API, "l2intf_id=0x%x, macLearnMax=%u", l2intf_id, macLearnMax);

  /* Fill structure */

  if ( macLearnMax != (L7_uint8)-1 )
  {
      l2intf.oper         = DAPI_CMD_SET;
  }
  else
  {
      l2intf.oper         = DAPI_CMD_CLEAR;
  }
  l2intf.cmd              = PTIN_VPORT_CMD_MAXMAC_SET;
  l2intf.virtual_gport    = l2intf_id;
  l2intf.macLearnMax      = macLearnMax;

  /* DTL call */
  rc = dtlPtinVirtualPort(intIfNum, &l2intf);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d (new MC group=%d, l2intf=%d)", rc, l2intf.multicast_group, l2intf.virtual_gport);

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
  ptin_l2intf_t l2intf;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( intIfNum == 0 || intIfNum >= L7_ALL_INTERFACES ||
       virtual_gport <= 0 || mcast_group <= 0)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_API, "intIfNum=%u, virtual_gport=%d, mcast_group=%u",
            intIfNum, virtual_gport, mcast_group);

  /* Fill structure */
  l2intf.oper             = DAPI_CMD_CLEAR;
  l2intf.cmd              = PTIN_VPORT_CMD_VP_OPER;
  l2intf.int_ovid         = -1;
  l2intf.int_ivid         = -1;
  l2intf.ext_ovid         = -1;
  l2intf.ext_ivid         = -1;
  l2intf.virtual_gport    = virtual_gport;
  l2intf.multicast_group  = mcast_group;
  l2intf.macLearnMax      = (L7_uint8) -1;

  /* DTL call */
  rc = dtlPtinVirtualPort(intIfNum, &l2intf);

  PT_LOG_TRACE(LOG_CTX_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Remove Virtual port from vlans info
 * 
 * @param intIfNum    : interface to be removed
 * @param ext_ovid    : External outer vlan 
 * @param ext_ivid    : External inner vlan 
 * @param mcast_group : Multicast group id.
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_virtual_port_remove_from_vlans(L7_uint32 intIfNum, L7_int ext_ovid, L7_int ext_ivid, L7_int mcast_group)
{
  ptin_l2intf_t l2intf;
  L7_RC_t rc = L7_SUCCESS;

  /* Validate arguments */
  if ( intIfNum == 0 || intIfNum >= L7_ALL_INTERFACES || 
       ext_ovid <= 0 || ext_ovid >= 4095 ||
       mcast_group <= 0)
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }
  PT_LOG_TRACE(LOG_CTX_API, "intIfNum=%u, ext_ovid=%d, ext_ivid=%d, mcast_group=%u",
            intIfNum, ext_ovid, ext_ivid, mcast_group);

  /* Fill structure */
  l2intf.oper             = DAPI_CMD_CLEAR;
  l2intf.int_ovid         = -1;
  l2intf.int_ivid         = -1;
  l2intf.ext_ovid         = ext_ovid;
  l2intf.ext_ivid         = ext_ivid;
  l2intf.virtual_gport    = -1;
  l2intf.multicast_group  = mcast_group;

  /* DTL call */
  rc = dtlPtinVirtualPort(intIfNum, &l2intf);

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
  if ( (vlanId == 0 || vlanId > 4095) ||
       (fwdVlanId == 0 || fwdVlanId > 4095) )
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Fill structure */
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
  if ( vlanId == 0 || vlanId > 4095 )
  {
    PT_LOG_ERR(LOG_CTX_API, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Fill structure */
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

