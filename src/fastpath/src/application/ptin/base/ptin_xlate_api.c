#include "ptin_xlate_api.h"
#include "logger.h"

#include "ptin_globaldefs.h"
#include "ptin_structs.h"
#include "ptin_intf.h"

#include "usmdb_nim_api.h"

#include "dtlinclude.h"

/***************************************************************** 
 * INTERNAL VARIABLES
 *****************************************************************/

/* Backup ingress and egress translations to a quick "get"...
   only applies to single outer-vlan translations */
static L7_uint16 xlate_table_ingress_oldVlan[PTIN_SYSTEM_N_INTERF][4096];
static L7_uint16 xlate_table_ingress_newVlan[PTIN_SYSTEM_N_INTERF][4096];

static L7_uint16 xlate_table_egress_oldVlan[PTIN_SYSTEM_N_INTERF][4096];
static L7_uint16 xlate_table_egress_newVlan[PTIN_SYSTEM_N_INTERF][4096];

/* List of portgroups for each interface */
static L7_int xlate_table_portgroup[PTIN_SYSTEM_N_PORTS];

/***************************************************************** 
 * INTERNAL FUNCTION PROTOTYPES
 *****************************************************************/

/**
 * Get new VlanId from local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param intIfNum : Interface reference
 * @param outerVlanId : outer vlan id
 * @param newOuterVlanId : New outer vlan id (to be returned)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_newVlan_get(ptin_vlanXlate_stage_enum stage, L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 *newOuterVlanId);

/**
 * Get new VlanId from local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param portgroup : port group reference
 * @param outerVlanId : outer vlan id
 * @param newOuterVlanId : New outer vlan id (to be returned)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_portgroup_newVlan_get(ptin_vlanXlate_stage_enum stage, L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 *newOuterVlanId);

/**
 * Get original VlanId from local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param intIfNum : Interface reference
 * @param outerVlanId : outer vlan id (to be returned)
 * @param newOuterVlanId : New outer vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_oldVlan_get(ptin_vlanXlate_stage_enum stage, L7_uint32 intIfNum, L7_uint16 *outerVlanId, L7_uint16 newOuterVlanId);

/**
 * Get original VlanId from local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param portgroup : port group reference
 * @param outerVlanId : outer vlan id (to be returned)
 * @param newOuterVlanId : New outer vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_portgroup_oldVlan_get(ptin_vlanXlate_stage_enum stage, L7_uint32 portgroup, L7_uint16 *outerVlanId, L7_uint16 newOuterVlanId);

/**
 * Save translation entry in local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param intIfNum : Interface reference
 * @param outerVlanId : outer vlan id
 * @param newOuterVlanId : New outer vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_store(ptin_vlanXlate_stage_enum stage, L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 newOuterVlanId);

/**
 * Save translation entry in local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param portgroup : port group reference
 * @param outerVlanId : outer vlan id
 * @param newOuterVlanId : New outer vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_portgroup_store(ptin_vlanXlate_stage_enum stage, L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 newOuterVlanId);

/**
 * Clear translation entry in local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param intIfNum : Interface reference
 * @param outerVlanId : outer vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_clear(ptin_vlanXlate_stage_enum stage, L7_uint32 intIfNum, L7_uint16 outerVlanId);

/**
 * Clear translation entry in local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param portgroup : port group reference
 * @param outerVlanId : outer vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_portgroup_clear(ptin_vlanXlate_stage_enum stage, L7_uint32 portgroup, L7_uint16 outerVlanId);

/**
 * Clear all translation entries in local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_clear_all(ptin_vlanXlate_stage_enum stage);

/**
 * Calculate portGroup id, from interface
 * 
 * @param intIfNum : interface reference
 * @param portgroup : portgroup (to be returned)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_portgroup_from_intf(L7_uint32 intIfNum, L7_uint32 *portgroup);


/***************************************************************** 
 * EXTERNAL FUNCTIONS IMPLEMENTATION
 *****************************************************************/

/**
 * Module initialization function
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_init(void)
{
  L7_RC_t rc1, rc2;

  LOG_TRACE(LOG_CTX_PTIN_API, "Initializing xlate module...");

  /* Init data structs */
  rc1 = xlate_database_clear_all(PTIN_XLATE_STAGE_ALL);

  /* Reset class ids */
  rc2 = ptin_xlate_portgroup_reset_all();

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc1=%d, rc2=%d", rc1, rc2);

  /* Return result */
  if (rc1!=L7_SUCCESS || rc2!=L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/**
 * Get portgroup of a specific interface
 * 
 * @param intIfNum : interface reference
 * @param portgroup : port group id (to be returned)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_portgroup_get(L7_uint32 intIfNum, L7_uint32 *portgroup)
{
  L7_uint32 ptin_port;

  LOG_TRACE(LOG_CTX_PTIN_API, "intIfNum=%u", intIfNum);

  /* Validate arguments */
  if ( ptin_intf_intIfNum2port(intIfNum, &ptin_port)!=L7_SUCCESS || ptin_port>=PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid interface");
    return L7_FAILURE;
  }

  /* Extract class id for this interface */
  if (portgroup!=L7_NULLPTR)
  {
    *portgroup = xlate_table_portgroup[ptin_port];
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=0");

  return L7_SUCCESS;
}

/**
 * Set portgroup to a specific interface
 * 
 * @param intIfNum : interface reference
 * @param portgroup : port group id 
 *                  (PTIN_XLATE_PORTGROUP_INTERFACE to reset)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_portgroup_set(L7_uint32 intIfNum, L7_uint32 portgroup)
{
  L7_uint32 ptin_port, ptin_port_i;
  L7_uint32 sysIntfType;
  L7_uint32 intf_members[PTIN_SYSTEM_N_PORTS], intf_members_n, i;
  L7_uint32 class_id;
  ptin_vlanXlate_classId_t group;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "intIfNum=%u portgroup=%u", intIfNum, portgroup);

  /* Validate interface */
  if ( ptin_intf_intIfNum2port(intIfNum, &ptin_port)!=L7_SUCCESS || ptin_port>=PTIN_SYSTEM_N_PORTS )
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid interface");
    return L7_FAILURE;
  }

  /* Class id to be used */
  if (portgroup==PTIN_XLATE_PORTGROUP_INTERFACE)
  {
    if (xlate_portgroup_from_intf(intIfNum, &class_id)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_API, " Error getting class id");
      return L7_FAILURE;
    }
  }
  else
  {
    class_id = portgroup;
  }

  /* Interface should be physical or LAG */
  if ( usmDbIntfTypeGet(intIfNum, &sysIntfType)!=L7_SUCCESS )
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Unable to find interface type");
    return L7_FAILURE;
  }

  /* Determine list of physical ports to be configured */
  if (sysIntfType==L7_PHYSICAL_INTF)
  {
    intf_members_n  = 1;
    intf_members[0] = intIfNum;
  }
  else if (sysIntfType==L7_LAG_INTF)
  {
    intf_members_n = PTIN_SYSTEM_N_PORTS;
    dot3adMemberListGet(intIfNum, &intf_members_n, intf_members);
  }
  else
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid interface type");
    return L7_FAILURE;
  }

  /* Check if there is members to be processed */
  if (intf_members_n==0)
  {
    LOG_WARNING(LOG_CTX_PTIN_API, " WARN: No interfaces to be processed");
    return L7_SUCCESS;
  }

  /* Set portgroup for all physical ports */
  for (i=0; i<intf_members_n; i++)
  {
    /* Interface should be physical */
    if ( usmDbIntfTypeGet(intf_members[i], &sysIntfType)!=L7_SUCCESS || 
         (sysIntfType!=L7_PHYSICAL_INTF /*&& sysIntfType!=L7_LAG_INTF*/))
    {
      continue;
    }

    /* Fill structure */
    group.oper = DAPI_CMD_SET;
    group.class_id = class_id;
  
    /* DTL call */
    if (dtlPtinVlanTranslateEgressPortsGroup(intf_members[i], &group)==L7_SUCCESS)
    {
      /* If successfull, set the portgroup to each physical port */
      if ( ptin_intf_intIfNum2port(intf_members[i], &ptin_port_i)==L7_SUCCESS && ptin_port_i<PTIN_SYSTEM_N_PORTS )
      {
        xlate_table_portgroup[ptin_port_i] = class_id;
      }
    }
    else
    {
      rc = L7_FAILURE;
    }
  }

  /* Update class id */
  if (rc==L7_SUCCESS)
  {
    /* If successfull, set the portgroup to the "global" port */
    xlate_table_portgroup[ptin_port] = class_id;
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Reset class ids to all interfaces
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_portgroup_reset_all(void)
{
  L7_uint32 ptin_port, intIfNum;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "Resetting class ids...");

  /* Run all interfaces */
  for (ptin_port=0; ptin_port<PTIN_SYSTEM_N_PORTS; ptin_port++ )
  {
    /* Default class id */
    xlate_table_portgroup[ptin_port] = 0;

    /* Extract and validate intIfNum */
    if ( ptin_intf_port2intIfNum(ptin_port, &intIfNum)!=L7_SUCCESS)
    {
      continue;
    }

    /* Apply default class id */
    if (ptin_xlate_portgroup_set(intIfNum, PTIN_XLATE_PORTGROUP_INTERFACE)!=L7_SUCCESS)
    {
      rc = L7_FAILURE;
      continue;
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Get ingress translation new vlan
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_ingress_get( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint16 *newOuterVlanId )
{
  L7_uint16 newVlanId;
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "intIfNum=%u, outerVlanId=%u, innerVlanId=%u",
            intIfNum, outerVlanId, innerVlanId);

  /* Validate arguments */
  if (outerVlanId>4095 || innerVlanId>4095)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialize new Vlan... as 0 (invalid value) */
  newVlanId = PTIN_XLATE_NOT_DEFINED;

  /* If innerVlanId is not defined, we can get this value directly to the local database */
  if (innerVlanId == PTIN_XLATE_NOT_DEFINED)
  {
    rc = xlate_database_newVlan_get(PTIN_XLATE_STAGE_INGRESS, intIfNum, outerVlanId, &newVlanId);
  }
  /* Otherwise, goto HAPI to discover vlan to be returned */
  else
  {
    /* Define structure */
    xlate.oper          = DAPI_CMD_GET;
    xlate.portgroup     = PTIN_XLATE_PORTGROUP_INTERFACE;
    xlate.stage         = PTIN_XLATE_STAGE_INGRESS;
    xlate.outerVlan     = outerVlanId;
    xlate.innerVlan     = innerVlanId;
    xlate.outerVlan_new = PTIN_XLATE_NOT_DEFINED;
    xlate.innerVlan_new = PTIN_XLATE_NOT_DEFINED;
    xlate.outerAction   = PTIN_XLATE_ACTION_NONE;
    xlate.innerAction   = PTIN_XLATE_ACTION_NONE;
  
    /* DTL call */
    rc = dtlPtinVlanTranslate(intIfNum, &xlate);
  
    /* If successfull, extract vlan to be returned */
    if ( rc == L7_SUCCESS )
    {
      newVlanId = xlate.outerVlan_new;
    }
  }

  /* Return value */
  if (rc==L7_SUCCESS)
  {
    if (newVlanId>4095)
    {
      rc = L7_FAILURE;
    }
    else
    {
      if (newOuterVlanId!=L7_NULLPTR)
        *newOuterVlanId = newVlanId;
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: newVlan=%u, rc=%d", newVlanId, rc);

  /* Return status */
  return rc;
}

/**
 * Get ingress translation original vlan
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan (to be returned)
 * @param newOuterVlanId : new vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_ingress_get_originalVlan( L7_uint32 intIfNum, L7_uint16 *outerVlanId, L7_uint16 newOuterVlanId )
{
  L7_uint16 oldVlanId;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "intIfNum=%u, newOuterVlanId=%u",
            intIfNum, newOuterVlanId);

  /* Validate arguments */
  if (newOuterVlanId>4095)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialize new Vlan... as 0 (invalid value) */
  oldVlanId = PTIN_XLATE_NOT_DEFINED;

  rc = xlate_database_oldVlan_get(PTIN_XLATE_STAGE_INGRESS, intIfNum, &oldVlanId, newOuterVlanId);

  /* Return value */
  if (rc==L7_SUCCESS)
  {
    if (oldVlanId>4095)
    {
      rc = L7_FAILURE;
    }
    else
    {
      if (outerVlanId!=L7_NULLPTR)
        *outerVlanId = oldVlanId;
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: oldVlan=%u, rc=%d", oldVlanId, rc);

  /* Return status */
  return rc;
}

/**
 * Add ingress translation entry
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan 
 * @param innerVlanId : lookup inner vlan  
 * @param newOuterVlanId : new vlan id 
 * @param newInnerVlanId : new inner vlan to be added (0 to not 
 *                       be added)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_ingress_add( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId )
{
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "intIfNum=%u, outerVlanId=%u, innerVlanId=%u, newOuterVlanId=%u, newInnerVlanId=%u",
            intIfNum, outerVlanId, innerVlanId, newOuterVlanId, newInnerVlanId);

  /* Validate arguments */
  if (outerVlanId>4095 || innerVlanId>4095 ||
      newOuterVlanId>4095 || newInnerVlanId>4095)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Define structure */
  xlate.oper          = DAPI_CMD_SET;
  xlate.portgroup     = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.stage         = PTIN_XLATE_STAGE_INGRESS;
  xlate.outerVlan     = outerVlanId;
  xlate.innerVlan     = innerVlanId;
  xlate.outerVlan_new = newOuterVlanId;
  xlate.innerVlan_new = newInnerVlanId;
  xlate.outerAction   = PTIN_XLATE_ACTION_REPLACE;
#if ( PTIN_BOARD_IS_MATRIX )
  xlate.innerAction   = PTIN_XLATE_ACTION_NONE;
#else
  xlate.innerAction   = (newInnerVlanId!=0) ? PTIN_XLATE_ACTION_ADD : PTIN_XLATE_ACTION_NONE;
#endif

  /* DTL call */
  rc = dtlPtinVlanTranslate(intIfNum, &xlate);

  /* If addition went well... */
  if (rc == L7_SUCCESS)
  {
    /* single-vlan translation: store this entry in a local database for quick access */
    rc = xlate_database_store(PTIN_XLATE_STAGE_INGRESS, intIfNum, outerVlanId, newOuterVlanId);
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Delete ingress translation entry
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_ingress_delete( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId )
{
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "intIfNum=%u, outerVlanId=%u, innerVlanId=%u",
            intIfNum, outerVlanId, innerVlanId);

  /* Validate arguments */
  if (outerVlanId>4095 || innerVlanId>4095)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Define structure */
  xlate.oper          = DAPI_CMD_CLEAR;
  xlate.portgroup     = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.stage         = PTIN_XLATE_STAGE_INGRESS;
  xlate.outerVlan     = outerVlanId;
  xlate.innerVlan     = innerVlanId;
  xlate.outerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.outerAction   = PTIN_XLATE_ACTION_NONE;
  xlate.innerAction   = PTIN_XLATE_ACTION_NONE;

  /* DTL call */
  rc = dtlPtinVlanTranslate(intIfNum, &xlate);

  /* If deletion went well... */
  if (rc == L7_SUCCESS)
  {
    /* Only if is a single-vlan translation, clear this entry in local database */
    if ( innerVlanId == PTIN_XLATE_NOT_DEFINED )
    {
      rc = xlate_database_clear(PTIN_XLATE_STAGE_INGRESS, intIfNum, outerVlanId);
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Deletes all ingress translation entries
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_ingress_delete_all( void )
{
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "Beginning...");

  /* Define structure */
  xlate.oper          = DAPI_CMD_CLEAR_ALL;
  xlate.portgroup     = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.stage         = PTIN_XLATE_STAGE_INGRESS;
  xlate.outerVlan     = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan     = PTIN_XLATE_NOT_DEFINED;
  xlate.outerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.outerAction   = PTIN_XLATE_ACTION_NONE;
  xlate.innerAction   = PTIN_XLATE_ACTION_NONE;

  /* DTL call */
  rc = dtlPtinVlanTranslate(L7_ALL_INTERFACES, &xlate);

  /* If deletion went well... */
  if (rc == L7_SUCCESS)
  {
    rc = xlate_database_clear_all(PTIN_XLATE_STAGE_INGRESS);
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Get egress translation new vlan
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_get( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint16 *newOuterVlanId )
{
  L7_uint32 class_id;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "intIfNum=%u, outerVlanId=%u, innerVlanId=%u",
            intIfNum, outerVlanId, innerVlanId);

  /* Get class id */
  if (xlate_portgroup_from_intf(intIfNum, &class_id)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR getting class id");
    return L7_FAILURE;
  }

  rc = ptin_xlate_egress_portgroup_get(class_id, outerVlanId, innerVlanId, newOuterVlanId);

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: newVlan=%u, rc=%d", *newOuterVlanId, rc);

  /* Return status */
  return rc;
}

/**
 * Get egress translation original vlan
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan (to be returned)
 * @param newOuterVlanId : new vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_get_originalVlan( L7_uint32 intIfNum, L7_uint16 *outerVlanId, L7_uint16 newOuterVlanId )
{
  L7_uint32 class_id;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "intIfNum=%u, newOuterVlanId=%u",
            intIfNum, newOuterVlanId);

  /* Get class id */
  if (xlate_portgroup_from_intf(intIfNum, &class_id)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR getting class id");
    return L7_FAILURE;
  }

  rc = ptin_xlate_egress_portgroup_get_originalVlan(class_id, outerVlanId, newOuterVlanId);

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: oldVlan=%u, rc=%d", *outerVlanId, rc);

  /* Return status */
  return rc;
}

/**
 * Add egress translation entry
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new vlan id 
 * @param newInnerVlanId : new inner vlan id  
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_add( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId )
{
  L7_uint32 class_id;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "intIfNum=%u, outerVlanId=%u, innerVlanId=%u, newOuterVlanId=%u",
            intIfNum, outerVlanId, innerVlanId, newOuterVlanId);

  /* Get class id */
  if (xlate_portgroup_from_intf(intIfNum, &class_id)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR getting class id");
    return L7_FAILURE;
  }

  rc = ptin_xlate_egress_portgroup_add(class_id, outerVlanId, innerVlanId, newOuterVlanId, newInnerVlanId);

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Delete egress translation entry
 * 
 * @param intIfNum : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_delete( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId )
{
  L7_uint32 class_id;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "intIfNum=%u, outerVlanId=%u, innerVlanId=%u",
            intIfNum, outerVlanId, innerVlanId);

  /* Get class id */
  if (xlate_portgroup_from_intf(intIfNum, &class_id)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR getting class id");
    return L7_FAILURE;
  }

  rc = ptin_xlate_egress_portgroup_delete(class_id, outerVlanId, innerVlanId);

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Get egress translation new vlan
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_portgroup_get( L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint16 *newOuterVlanId )
{
  L7_uint16 newVlanId;
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "portgroup=%u, outerVlanId=%u, innerVlanId=%u",
            portgroup, outerVlanId, innerVlanId);

  /* Validate arguments */
  if (portgroup==0 || 
      outerVlanId>4095 || innerVlanId>4095)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialize new Vlan... as 0 (invalid value) */
  newVlanId = PTIN_XLATE_NOT_DEFINED;

  /* If innerVlanId is not defined, we can get this value directly to the local database */
  if (innerVlanId == PTIN_XLATE_NOT_DEFINED)
  {
    rc = xlate_database_portgroup_newVlan_get(PTIN_XLATE_STAGE_EGRESS, portgroup, outerVlanId, &newVlanId);
  }
  /* Otherwise, goto HAPI to discover vlan to be returned */
  else
  {
    /* Define structure */
    xlate.oper          = DAPI_CMD_GET;
    xlate.portgroup     = portgroup;
    xlate.stage         = PTIN_XLATE_STAGE_EGRESS;
    xlate.outerVlan     = outerVlanId;
    xlate.innerVlan     = innerVlanId;
    xlate.outerVlan_new = PTIN_XLATE_NOT_DEFINED;
    xlate.innerVlan_new = PTIN_XLATE_NOT_DEFINED;
    xlate.outerAction   = PTIN_XLATE_ACTION_NONE;
    xlate.innerAction   = PTIN_XLATE_ACTION_NONE;

    /* DTL call */
    rc = dtlPtinVlanTranslate(L7_ALL_INTERFACES, &xlate);

    /* If successfull, extract vlan to be returned */
    if ( rc == L7_SUCCESS )
    {
      newVlanId = xlate.outerVlan_new;
    }
  }

  /* Return value */
  if (rc==L7_SUCCESS)
  {
    if (newVlanId>4095)
    {
      rc = L7_FAILURE;
    }
    else
    {
      if (newOuterVlanId!=L7_NULLPTR)
        *newOuterVlanId = newVlanId;
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: newVlan=%u, rc=%d", newVlanId, rc);

  /* Return status */
  return rc;
}

/**
 * Get egress translation original vlan
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan (to be returned)
 * @param newOuterVlanId : new vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_portgroup_get_originalVlan( L7_uint32 portgroup, L7_uint16 *outerVlanId, L7_uint16 newOuterVlanId )
{
  L7_uint16 oldVlanId;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "portgroup=%u, newOuterVlanId=%u",
            portgroup, newOuterVlanId);

  /* Validate arguments */
  if (portgroup==0 || 
      newOuterVlanId>4095)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialize new Vlan... as 0 (invalid value) */
  oldVlanId = PTIN_XLATE_NOT_DEFINED;

  rc = xlate_database_portgroup_oldVlan_get(PTIN_XLATE_STAGE_EGRESS, portgroup, &oldVlanId, newOuterVlanId);

  /* Return value */
  if (rc==L7_SUCCESS)
  {
    if (oldVlanId>4095)
    {
      rc = L7_FAILURE;
    }
    else
    {
      if (outerVlanId!=L7_NULLPTR)
        *outerVlanId = oldVlanId;
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: oldVlan=%u, rc=%d", oldVlanId, rc);

  /* Return status */
  return rc;
}

/**
 * Add egress translation entry
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new vlan id 
 * @param newInnerVlanId : new inner vlan id 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_portgroup_add( L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId )
{
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "portgroup=%u, outerVlanId=%u, innerVlanId=%u, newOuterVlanId=%u, newInnerVlanId=%u",
            portgroup, outerVlanId, innerVlanId, newOuterVlanId, newInnerVlanId);

  /* Validate arguments */
  if (portgroup==0 || 
      outerVlanId>4095 || innerVlanId>4095 || 
      newOuterVlanId>4095 || (newInnerVlanId!=(L7_uint16)-1 && newInnerVlanId>4095))
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Define structure */
  xlate.oper          = DAPI_CMD_SET;
  xlate.portgroup     = portgroup;
  xlate.stage         = PTIN_XLATE_STAGE_EGRESS;
  xlate.outerVlan     = outerVlanId;
  xlate.innerVlan     = innerVlanId;
  xlate.outerVlan_new = newOuterVlanId;
  xlate.innerVlan_new = (newInnerVlanId>4095) ? 0 : newInnerVlanId;
  xlate.outerAction   = PTIN_XLATE_ACTION_REPLACE;
#if ( PTIN_BOARD_IS_MATRIX )
  xlate.innerAction   = PTIN_XLATE_ACTION_NONE;
#else
  xlate.innerAction = (newInnerVlanId>4095) ? PTIN_XLATE_ACTION_DELETE : PTIN_XLATE_ACTION_NONE;
  //xlate.innerAction   = (innerVlanId!=0) ? PTIN_XLATE_ACTION_DELETE : PTIN_XLATE_ACTION_NONE;
#endif

  /* DTL call */
  rc = dtlPtinVlanTranslate(L7_ALL_INTERFACES, &xlate);

  /* If addition went well... */
  if (rc == L7_SUCCESS)
  {
    /* Only if is a single-vlan translation, store this entry in a local database for quick access */
    if ( innerVlanId == PTIN_XLATE_NOT_DEFINED )
    {
      rc = xlate_database_portgroup_store(PTIN_XLATE_STAGE_EGRESS, portgroup, outerVlanId, newOuterVlanId);
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Delete egress translation entry
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_portgroup_delete( L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 innerVlanId )
{
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "portgroup=%u, outerVlanId=%u, innerVlanId=%u",
            portgroup, outerVlanId, innerVlanId);

  /* Validate arguments */
  if (portgroup==0 || 
      outerVlanId>4095 || innerVlanId>4095)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Define structure */
  xlate.oper          = DAPI_CMD_CLEAR;
  xlate.portgroup     = portgroup;
  xlate.stage         = PTIN_XLATE_STAGE_EGRESS;
  xlate.outerVlan     = outerVlanId;
  xlate.innerVlan     = innerVlanId;
  xlate.outerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.outerAction   = PTIN_XLATE_ACTION_NONE;
  xlate.innerAction   = PTIN_XLATE_ACTION_NONE;

  /* DTL call */
  rc = dtlPtinVlanTranslate(L7_ALL_INTERFACES, &xlate);

  /* If deletion went well... */
  if (rc == L7_SUCCESS)
  {
    /* Only if is a single-vlan translation, clear this entry in local database */
    if ( innerVlanId == PTIN_XLATE_NOT_DEFINED )
    {
      rc = xlate_database_portgroup_clear(PTIN_XLATE_STAGE_EGRESS, portgroup, outerVlanId);
    }
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Deletes all egress translation entries
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_delete_all( void )
{
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "Beginning...");

  /* Define structure */
  xlate.oper          = DAPI_CMD_CLEAR_ALL;
  xlate.portgroup     = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.stage         = PTIN_XLATE_STAGE_EGRESS;
  xlate.outerVlan     = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan     = PTIN_XLATE_NOT_DEFINED;
  xlate.outerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.outerAction   = PTIN_XLATE_ACTION_NONE;
  xlate.innerAction   = PTIN_XLATE_ACTION_NONE;

  /* DTL call */
  rc = dtlPtinVlanTranslate(L7_ALL_INTERFACES, &xlate);

  /* If deletion went well... */
  if (rc == L7_SUCCESS)
  {
    rc = xlate_database_clear_all(PTIN_XLATE_STAGE_EGRESS);
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Deletes all ingress+egress translation entries
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_delete_all( void )
{
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  LOG_TRACE(LOG_CTX_PTIN_API, "Beginning...");

  /* Define structure */
  xlate.oper          = DAPI_CMD_CLEAR_ALL;
  xlate.portgroup     = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.stage         = PTIN_XLATE_STAGE_ALL;
  xlate.outerVlan     = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan     = PTIN_XLATE_NOT_DEFINED;
  xlate.outerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.outerAction   = PTIN_XLATE_ACTION_NONE;
  xlate.innerAction   = PTIN_XLATE_ACTION_NONE;

  /* DTL call */
  rc = dtlPtinVlanTranslate(L7_ALL_INTERFACES, &xlate);

  /* If deletion went well... */
  if (rc == L7_SUCCESS)
  {
    rc = xlate_database_clear_all(PTIN_XLATE_STAGE_ALL);
  }

  LOG_TRACE(LOG_CTX_PTIN_API, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Effectively removes all deleted translation entries
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
L7_RC_t ptin_xlate_delete_flush( void )
{
  /* TODO: This function works as a delay between xlate deletion and effective deletion from local database */

  return L7_SUCCESS;
}


/***************************************************************** 
 * STATIC FUNCTIONS IMPLEMENTATION
 *****************************************************************/

/**
 * Get new VlanId from local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param intIfNum : Interface reference
 * @param outerVlanId : outer vlan id
 * @param newOuterVlanId : New outer vlan id (to be returned)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_newVlan_get(ptin_vlanXlate_stage_enum stage, L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 *newOuterVlanId)
{
  L7_uint32 ptin_port=(L7_uint32)-1;
  L7_uint16 newVlanId = PTIN_XLATE_NOT_DEFINED;

  /* Validate arguments */
  if ( ( stage!=PTIN_XLATE_STAGE_INGRESS && stage!=PTIN_XLATE_STAGE_EGRESS ) ||
       ( ptin_intf_intIfNum2port(intIfNum, &ptin_port)!=L7_SUCCESS ) ||
       ( outerVlanId>4095) )
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid arguments (stage=%d, intIfNum=%u, ptin_port=%u, oVlan=%u)", stage, intIfNum, ptin_port, outerVlanId);
    return L7_FAILURE;
  }

  /* Get value */
  if (stage==PTIN_XLATE_STAGE_EGRESS)
  {
    newVlanId = xlate_table_egress_newVlan[ptin_port][outerVlanId];
  }
  else
  {
    newVlanId = xlate_table_ingress_newVlan[ptin_port][outerVlanId];
  }

  /* Return value */
  if (newOuterVlanId!=L7_NULLPTR)
  {
    *newOuterVlanId = newVlanId;
  }

  /* Success */
  return L7_SUCCESS;
}

/**
 * Get original VlanId from local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param intIfNum : Interface reference
 * @param outerVlanId : outer vlan id (to be returned)
 * @param newOuterVlanId : New outer vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_oldVlan_get(ptin_vlanXlate_stage_enum stage, L7_uint32 intIfNum, L7_uint16 *outerVlanId, L7_uint16 newOuterVlanId)
{
  L7_uint32 ptin_port=(L7_uint32)-1;
  L7_uint16 oldVlanId = PTIN_XLATE_NOT_DEFINED;

  /* Validate arguments */
  if ( ( stage!=PTIN_XLATE_STAGE_INGRESS && stage!=PTIN_XLATE_STAGE_EGRESS ) ||
       ( ptin_intf_intIfNum2port(intIfNum, &ptin_port)!=L7_SUCCESS ) ||
       ( newOuterVlanId>4095) )
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid arguments (stage=%d, intIfNum=%u, ptin_port=%u, newOVlan=%u)", stage, intIfNum, ptin_port, newOuterVlanId);
    return L7_FAILURE;
  }

  /* Get value */
  if (stage==PTIN_XLATE_STAGE_EGRESS)
  {
    oldVlanId = xlate_table_egress_oldVlan[ptin_port][newOuterVlanId];
  }
  else
  {
    oldVlanId = xlate_table_ingress_oldVlan[ptin_port][newOuterVlanId];
  }

  /* Return value */
  if (outerVlanId!=L7_NULLPTR)
  {
    *outerVlanId = oldVlanId;
  }

  /* Success */
  return L7_SUCCESS;
}

/**
 * Save translation entry in local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param intIfNum : Interface reference
 * @param outerVlanId : outer vlan id
 * @param newOuterVlanId : New outer vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_store(ptin_vlanXlate_stage_enum stage, L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 newOuterVlanId)
{
  L7_uint32 ptin_port=(L7_uint32)-1;

  /* Validate arguments */
  if ( ( stage!=PTIN_XLATE_STAGE_INGRESS && stage!=PTIN_XLATE_STAGE_EGRESS ) ||
       ( ptin_intf_intIfNum2port(intIfNum, &ptin_port)!=L7_SUCCESS ) ||
       ( outerVlanId>4095) ||
       ( newOuterVlanId>4095 ) )
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid arguments (stage=%d, intIfNum=%u, ptin_port=%u, oVlan=%u, newOVlan=%u)", stage, intIfNum, ptin_port, outerVlanId, newOuterVlanId);
    return L7_FAILURE;
  }

  /* Store value */
  if (stage==PTIN_XLATE_STAGE_EGRESS)
  {
    xlate_table_egress_newVlan[ptin_port][outerVlanId]    = newOuterVlanId;
    xlate_table_egress_oldVlan[ptin_port][newOuterVlanId] = outerVlanId;
  }
  else
  {
    xlate_table_ingress_newVlan[ptin_port][outerVlanId]    = newOuterVlanId;
    xlate_table_ingress_oldVlan[ptin_port][newOuterVlanId] = outerVlanId;
  }

  /* Success */
  return L7_SUCCESS;
}

/**
 * Clear translation entry in local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param intIfNum : Interface reference
 * @param outerVlanId : outer vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_clear(ptin_vlanXlate_stage_enum stage, L7_uint32 intIfNum, L7_uint16 outerVlanId)
{
  L7_uint32 ptin_port=(L7_uint32)-1;
  L7_uint16 newOuterVlanId;

  /* Validate arguments */
  if ( ( stage!=PTIN_XLATE_STAGE_INGRESS && stage!=PTIN_XLATE_STAGE_EGRESS ) ||
       ( ptin_intf_intIfNum2port(intIfNum, &ptin_port)!=L7_SUCCESS ) ||
       ( outerVlanId>4095) )
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid arguments (stage=%d, intIfNum=%u, ptin_port=%u, oVlan=%u)", stage, intIfNum, ptin_port, outerVlanId);
    return L7_FAILURE;
  }

  /* Clear value */
  if (stage==PTIN_XLATE_STAGE_EGRESS)
  {
    newOuterVlanId = xlate_table_egress_newVlan[ptin_port][outerVlanId];

    xlate_table_egress_newVlan[ptin_port][outerVlanId] = PTIN_XLATE_NOT_DEFINED;
    if (newOuterVlanId<=4095)
      xlate_table_egress_oldVlan[ptin_port][newOuterVlanId] = PTIN_XLATE_NOT_DEFINED;
  }
  else
  {
    newOuterVlanId = xlate_table_ingress_newVlan[ptin_port][outerVlanId];

    xlate_table_ingress_newVlan[ptin_port][outerVlanId] = PTIN_XLATE_NOT_DEFINED;
    if (newOuterVlanId<=4095)
      xlate_table_ingress_oldVlan[ptin_port][newOuterVlanId] = PTIN_XLATE_NOT_DEFINED;
  }

  /* Success */
  return L7_SUCCESS;
}

/**
 * Get new VlanId from local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param portgroup : port group reference
 * @param outerVlanId : outer vlan id
 * @param newOuterVlanId : New outer vlan id (to be returned)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_portgroup_newVlan_get(ptin_vlanXlate_stage_enum stage, L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 *newOuterVlanId)
{
  L7_uint32 ptin_port=(L7_uint32)-1;
  L7_uint32 intIfNum;

  /* Find ptin_port correspondent to this portgroup */
  for (ptin_port=0; ptin_port<PTIN_SYSTEM_N_PORTS && xlate_table_portgroup[ptin_port]!=portgroup; ptin_port++);
  if (ptin_port>=PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: No interface was found using portgroup %u", portgroup);
    return L7_FAILURE;
  }

  /* Calculate intIfNum */
  if (ptin_intf_port2intIfNum(ptin_port, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Port %u looks to not exist", ptin_port);
    return L7_FAILURE;
  }

  /* Call standard function */
  return xlate_database_newVlan_get(stage, intIfNum, outerVlanId, newOuterVlanId);
}

/**
 * Get original VlanId from local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param portgroup : port group reference
 * @param outerVlanId : outer vlan id (to be returned)
 * @param newOuterVlanId : New outer vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_portgroup_oldVlan_get(ptin_vlanXlate_stage_enum stage, L7_uint32 portgroup, L7_uint16 *outerVlanId, L7_uint16 newOuterVlanId)
{
  L7_uint32 ptin_port=(L7_uint32)-1;
  L7_uint32 intIfNum;

  /* Find ptin_port correspondent to this portgroup */
  for (ptin_port=0; ptin_port<PTIN_SYSTEM_N_PORTS && xlate_table_portgroup[ptin_port]!=portgroup; ptin_port++);
  if (ptin_port>=PTIN_SYSTEM_N_PORTS)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: No interface was found using portgroup %u", portgroup);
    return L7_FAILURE;
  }

  /* Calculate intIfNum */
  if (ptin_intf_port2intIfNum(ptin_port, &intIfNum)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Port %u looks to not exist", ptin_port);
    return L7_FAILURE;
  }

  /* Call standard function */
  return xlate_database_oldVlan_get(stage, intIfNum, outerVlanId, newOuterVlanId);
}

/**
 * Save translation entry in local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param portgroup : port group reference
 * @param outerVlanId : outer vlan id
 * @param newOuterVlanId : New outer vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_portgroup_store(ptin_vlanXlate_stage_enum stage, L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 newOuterVlanId)
{
  L7_uint32 ptin_port=(L7_uint32)-1;
  L7_uint32 intIfNum;
  L7_RC_t rc = L7_SUCCESS;

  /* Find ptin_port correspondent to this portgroup */
  for (ptin_port=0; ptin_port<PTIN_SYSTEM_N_PORTS; ptin_port++)
  {
    /* Only process ports with the specified portgroup */
    if (xlate_table_portgroup[ptin_port]!=portgroup)  continue;

    /* Calculate intIfNum */
    if (ptin_intf_port2intIfNum(ptin_port, &intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Port %u looks to not exist", ptin_port);
      rc = L7_FAILURE;
      continue;
    }

    /* Call standard function */
    if (xlate_database_store(stage, intIfNum, outerVlanId, newOuterVlanId)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_API, " ERROR: xlate_database_store failed");
      rc = L7_FAILURE;
      continue;
    }
  }

  /* Return global result */
  return rc;
}

/**
 * Clear translation entry in local database
 * 
 * @param stage : PTIN_XLATE_STAGE_INGRESS or 
 *             PTIN_XLATE_STAGE_EGRESS
 * @param portgroup : port group reference
 * @param outerVlanId : outer vlan id
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_portgroup_clear(ptin_vlanXlate_stage_enum stage, L7_uint32 portgroup, L7_uint16 outerVlanId)
{
  L7_uint32 ptin_port=(L7_uint32)-1;
  L7_uint32 intIfNum;
  L7_RC_t rc = L7_SUCCESS;

  /* Find ptin_port correspondent to this portgroup */
  for (ptin_port=0; ptin_port<PTIN_SYSTEM_N_PORTS; ptin_port++)
  {
    /* Only process ports with the specified portgroup */
    if (xlate_table_portgroup[ptin_port]!=portgroup)  continue;

    /* Calculate intIfNum */
    if (ptin_intf_port2intIfNum(ptin_port, &intIfNum)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Port %u looks to not exist", ptin_port);
      rc = L7_FAILURE;
      continue;
    }

    /* Call standard function */
    if (xlate_database_clear(stage, intIfNum, outerVlanId)!=L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_PTIN_API, " ERROR: xlate_database_clear failed");
      rc = L7_FAILURE;
      continue;
    }
  }

  /* Return global result */
  return rc;
}

/**
 * Clear all translation entries in local database
 * 
 * @param stage : PTIN_XLATE_STAGE_ALL, PTIN_XLATE_STAGE_INGRESS
 *             or PTIN_XLATE_STAGE_EGRESS
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_clear_all(ptin_vlanXlate_stage_enum stage)
{
  /* Validate arguments */
  if ( stage!=PTIN_XLATE_STAGE_ALL &&
       stage!=PTIN_XLATE_STAGE_INGRESS &&
       stage!=PTIN_XLATE_STAGE_EGRESS )
  {
    LOG_ERR(LOG_CTX_PTIN_API, " ERROR: Invalid arguments (stage=%d)", stage);
    return L7_FAILURE;
  }

  /* Init data structs */
  if (stage==PTIN_XLATE_STAGE_ALL || stage==PTIN_XLATE_STAGE_EGRESS)
  {
    memset(xlate_table_egress_oldVlan, 0x00, sizeof(xlate_table_egress_oldVlan));
    memset(xlate_table_egress_newVlan, 0x00, sizeof(xlate_table_egress_newVlan));
  }

  if (stage==PTIN_XLATE_STAGE_ALL || stage==PTIN_XLATE_STAGE_INGRESS)
  {
    memset(xlate_table_ingress_oldVlan, 0x00, sizeof(xlate_table_ingress_oldVlan));
    memset(xlate_table_ingress_newVlan, 0x00, sizeof(xlate_table_ingress_newVlan));
  }

  return L7_SUCCESS;
}

/**
 * Calculate portGroup id, from interface
 * 
 * @param intIfNum : interface reference
 * @param portgroup : portgroup (to be returned)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_portgroup_from_intf(L7_uint32 intIfNum, L7_uint32 *portgroup)
{
  nimUSP_t usp;
  L7_uint32 class_id;

  /* Calculate USP */
  if (nimGetUnitSlotPort(intIfNum, &usp)!=L7_SUCCESS)
  {
    LOG_ERR(LOG_CTX_PTIN_API, " Error getting USP (intIfNum=%u)", intIfNum);
    return L7_FAILURE;
  }

  /* If interface is not physical neither LAG, error */
  if ( usp.slot>=L7_MAX_PHYSICAL_SLOTS_PER_UNIT && usp.slot!=L7_LAG_SLOT_NUM )
  {
    LOG_ERR(LOG_CTX_PTIN_API, " Invalid interface type (usp={%d, %d, %d})", usp.unit, usp.slot, usp.port);
    return L7_FAILURE;
  }

  /* Calculate port group */
  class_id = (usp.slot*L7_MAX_PORTS_PER_SLOT) + usp.port;

  /* Validate class id */
  if ( class_id == 0 )
  {
    LOG_ERR(LOG_CTX_PTIN_API, " Class id is null");
    return L7_FAILURE;
  }

  /* Return class id */
  if (portgroup!=L7_NULLPTR)
  {
    *portgroup = class_id;
  }

  return L7_SUCCESS;
}

/***************************************************************** 
 * DEVSHELL FUNCTIONS
 *****************************************************************/

L7_RC_t devshell_ptin_xlate_portgroup_get(L7_uint32 intIfNum)
{
  L7_uint32 portGroup;
  L7_RC_t rc;

  rc = ptin_xlate_portgroup_get(intIfNum, &portGroup);

  printf("ptin_xlate_portgroup_get(%u, &portgroup)=>%u portGroup=%u", intIfNum, rc, portGroup);

  return rc;
}

L7_RC_t devshell_ptin_xlate_ingress_get( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId )
{
  L7_uint16 newOuterVlanId;
  L7_RC_t rc;

  rc = ptin_xlate_ingress_get(intIfNum, outerVlanId, innerVlanId, &newOuterVlanId);

  printf("ptin_xlate_ingress_get(%u, %u, %u, &newOuterVlanId)=>%u newOuterVlanId=%u", intIfNum, outerVlanId, innerVlanId, rc, newOuterVlanId);

  return rc;
}

L7_RC_t devshell_ptin_xlate_ingress_get_originalVlan( L7_uint32 intIfNum, L7_uint16 newOuterVlanId )
{
  L7_uint16 outerVlanId;
  L7_RC_t rc;

  rc = ptin_xlate_ingress_get_originalVlan(intIfNum, &outerVlanId, newOuterVlanId);

  printf("ptin_xlate_ingress_get_originalVlan(%u, &outerVlanId, %u)=>%u outerVlanId=%u", intIfNum, newOuterVlanId, rc, outerVlanId);

  return rc;
}

L7_RC_t devshell_ptin_xlate_egress_get( L7_uint32 intIfNum, L7_uint16 outerVlanId, L7_uint16 innerVlanId )
{
  L7_uint16 newOuterVlanId;
  L7_RC_t rc;

  rc = ptin_xlate_egress_get(intIfNum, outerVlanId, innerVlanId, &newOuterVlanId);

  printf("ptin_xlate_egress_get(%u, %u, %u, &newOuterVlanId)=>%u newOuterVlanId=%u", intIfNum, outerVlanId, innerVlanId, rc, newOuterVlanId);

  return rc;
}

L7_RC_t devshell_ptin_xlate_egress_get_originalVlan( L7_uint32 intIfNum, L7_uint16 newOuterVlanId )
{
  L7_uint16 outerVlanId;
  L7_RC_t rc;

  rc = ptin_xlate_egress_get_originalVlan(intIfNum, &outerVlanId, newOuterVlanId);

  printf("ptin_xlate_egress_get_originalVlan(%u, &outerVlanId, %u)=>%u outerVlanId=%u", intIfNum, newOuterVlanId, rc, outerVlanId);

  return rc;
}

L7_RC_t devshell_ptin_xlate_egress_portgroup_get( L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 innerVlanId )
{
  L7_uint16 newOuterVlanId;
  L7_RC_t rc;

  rc = ptin_xlate_egress_portgroup_get(portgroup, outerVlanId, innerVlanId, &newOuterVlanId);

  printf("ptin_xlate_egress_portgroup_get(%u, %u, %u, &newOuterVlanId)=>%u newOuterVlanId=%u", portgroup, outerVlanId, innerVlanId, rc, newOuterVlanId);

  return rc;
}

L7_RC_t devshell_ptin_xlate_egress_portgroup_get_originalVlan( L7_uint32 portgroup, L7_uint16 newOuterVlanId )
{
  L7_uint16 outerVlanId;
  L7_RC_t rc;

  rc = ptin_xlate_egress_portgroup_get_originalVlan(portgroup, &outerVlanId, newOuterVlanId);

  printf("ptin_xlate_egress_portgroup_get_originalVlan(%u, &outerVlanId, %u)=>%u outerVlanId=%u", portgroup, newOuterVlanId, rc, outerVlanId);

  return rc;
}

