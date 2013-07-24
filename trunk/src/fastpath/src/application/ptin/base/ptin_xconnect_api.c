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
L7_RC_t ptin_crossconnect_vlan_learn(L7_uint16 vlanId, L7_uint16 fwdVlanId, L7_BOOL macLearn)
{
  ptin_bridge_vlan_mode_t mode;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "vlanId=%u, fwdVlanId=%u, macLearn=%u", vlanId, fwdVlanId, macLearn);
  /* Validate arguments */
  if ( (vlanId == 0 || vlanId > 4095) ||
       (fwdVlanId == 0 || fwdVlanId > 4095) )
  {
    LOG_ERR(LOG_CTX_PTIN_API, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Fill structure */
  mode.vlanId       = vlanId;
  mode.fwdVlanId    = fwdVlanId;
  mode.learn_enable = macLearn & 1;
  mode.mask         = PTIN_BRIDGE_VLAN_MODE_MASK_FWDVLAN | PTIN_BRIDGE_VLAN_MODE_MASK_LEARN_EN;

  /* DTL call */
  rc = dtlPtinVlanDefinitions(&mode);

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

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

  LOG_TRACE(LOG_CTX_PTIN_API, "vlanId=%u, crossconnect=%u", vlanId, crossconnect_apply);
  /* Validate arguments */
  if ( vlanId == 0 || vlanId > 4095 )
  {
    LOG_ERR(LOG_CTX_PTIN_API, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Fill structure */
  mode.vlanId                 = vlanId;
  mode.double_tag             = double_tag;
  mode.cross_connects_enable  = crossconnect_apply & 1;
  mode.mask                   = PTIN_BRIDGE_VLAN_MODE_MASK_CROSSCONN_EN;

  /* DTL call */
  rc = dtlPtinVlanDefinitions(&mode);

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

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

  LOG_TRACE(LOG_CTX_PTIN_API, "outerVlanId=%u, innerVlanId=%u, intIfNum1=%u, intIfNum2=%u",
            outerVlanId, innerVlanId, intIfNum1, intIfNum2);
  /* Validate arguments */
  if ( outerVlanId > 4095 || innerVlanId > 4095 )
  {
    LOG_ERR(LOG_CTX_PTIN_API, "Invalid arguments");
    return L7_FAILURE;
  }

  /* Fill structure */
  cc.outerVlanId = outerVlanId;
  cc.innerVlanId = innerVlanId;
  cc.oper        = DAPI_CMD_SET;
  cc.dstUsp.unit = cc.dstUsp.slot = cc.dstUsp.port = -1;  /* intIfNum2 will reconfigure this parameter */

  /* DTL call */
  rc = dtlPtinBridgeCrossconnect(intIfNum1, intIfNum2, &cc);

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

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

  LOG_TRACE(LOG_CTX_PTIN_API,"outerVlanId=%u, innerVlanId=%u", outerVlanId, innerVlanId);
  /* Validate arguments */
  if ( outerVlanId > 4095 || innerVlanId > 4095 )
  {
    LOG_ERR(LOG_CTX_PTIN_API,"Invalid arguments");
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

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

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

  LOG_TRACE(LOG_CTX_PTIN_API, "Beginning...");

  cc.dstUsp.unit = -1;
  cc.dstUsp.slot = -1;
  cc.dstUsp.port = -1;
  cc.outerVlanId = 0;
  cc.innerVlanId = 0;
  cc.oper        = DAPI_CMD_CLEAR_ALL;

  /* DTL call */
  rc = dtlPtinBridgeCrossconnect(L7_ALL_INTERFACES,L7_ALL_INTERFACES,&cc);

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

  return rc;
}

/***************************************************************** 
 * STATIC FUNCTIONS IMPLEMENTATION
 *****************************************************************/

