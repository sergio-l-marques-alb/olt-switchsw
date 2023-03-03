#include "ptin_xlate_api.h"
#include "logger.h"

#include "ptin_globaldefs.h"
#include "ptin_structs.h"
#include "ptin_intf.h"
#include "mirror_api.h"

#include "usmdb_nim_api.h"

#include "usmdb_mib_vlan_api.h"

#include "dtlinclude.h"

/**************************************************************** 
 * DATA TYPES
 ****************************************************************/

#define PTIN_XLATE_MAX_SIZE 7000

typedef struct ptinXlateKey_s
{
  L7_uint32 ptin_port;
  L7_uint32 portGroup;
  L7_uint16 outerVid;
  L7_uint16 innerVid;
} ptinXlateKey_t;

/* Client list snapshot */
typedef struct ptinXlateInfoData_s
{
  ptinXlateKey_t    key;

  L7_uint           outer_prio;
  L7_uint           inner_prio;

  L7_uint           outerVlanAction;
  L7_uint           innerVlanAction;
  L7_uint16         outerVlan_result;
  L7_uint16         innerVlan_result;

  L7_uint           outerPrioAction;
  L7_uint           innerPrioAction;
  L7_uint16         outerPrio_result;
  L7_uint16         innerPrio_result;

  L7_BOOL           remove_VLANs;
  L7_BOOL           mirror;

  void             *next;
} ptinXlateInfoData_t;

typedef struct {
    avlTree_t            avlTree;
    avlTreeTables_t     *treeHeap;
    ptinXlateInfoData_t *dataHeap;
    L7_uint              number_of_entries;
} ptinXlateAvlTree_t;

/***************************************************************** 
 * INTERNAL VARIABLES
 *****************************************************************/

/* All translation rules are saved here (3 stages) */
ptinXlateAvlTree_t database_xlate[PTIN_XLATE_STAGE_ALL];
ptinXlateAvlTree_t database_xlate_inv[PTIN_XLATE_STAGE_ALL];

/* List of portgroups for each interface */
static L7_int xlate_table_portgroup[PTIN_SYSTEM_N_PORTS];

/* PVID database */ 
static L7_uint16 xlate_table_pvid[PTIN_SYSTEM_N_INTERF];


L7_BOOL ptin_debug_xlate = 0;

void ptin_xlate_debug_enable(L7_BOOL enable)
{
  ptin_debug_xlate = enable & 1;
}


/***************************************************************** 
 * INTERNAL FUNCTION PROTOTYPES
 *****************************************************************/

/** XLATE operations */
static L7_RC_t ptin_xlate_operation(L7_int operation, L7_uint32 ptin_port, ptin_vlanXlate_t *xlate );

/**
 * XLATE database management
 */
static L7_RC_t xlate_database_init(void);
static L7_RC_t xlate_database_newVlan_get(L7_uint32 ptin_port, ptin_vlanXlate_t *vlanXlate_data);
static L7_RC_t xlate_database_oldVlan_get(L7_uint32 ptin_port, ptin_vlanXlate_t *vlanXlate_data);
static L7_RC_t xlate_database_store(L7_uint32 ptin_port, const ptin_vlanXlate_t *vlanXlate_data);
static L7_RC_t xlate_database_clear(L7_uint32 ptin_port, const ptin_vlanXlate_t *vlanXlate_data);
static L7_RC_t xlate_database_clear_all(ptin_vlanXlate_stage_enum stage);

void ptin_xlate_dump(ptin_vlanXlate_stage_enum stage, L7_BOOL inv);

/**
 * Calculate portGroup id, from interface
 * 
 * @param ptin_port : interface reference
 * @param portgroup : portgroup (to be returned)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_portgroup_from_intf(L7_uint32 ptin_port, L7_uint32 *portgroup);

static L7_RC_t ptin_xlate_PVID_init(void);

/**
 * Inline functions
 */
#define VLANXLATE_ACTION_CHARS { 'N', 'A', 'R', 'D', 'C', '?'}
inline char ptin_vlanxlate_action_getchar(ptin_vlanXlate_action_enum action)
{
  char char_list[] = VLANXLATE_ACTION_CHARS;

  if (action >= PTIN_XLATE_ACTION_MAX)
  {
    return char_list[PTIN_XLATE_ACTION_MAX];
  }
  return char_list[action];
}



/**
 * Add ingress translation entry
 * 
 * @param ptin_port : interface reference
 * @param outerVlanId : lookup outer vlan 
 * @param innerVlanId : lookup inner vlan  
 * @param newOuterVlanId : new vlan id 
 * @param newInnerVlanId : new inner vlan to be added (0 to not 
 *                       be added)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_ingress_add_2( L7_uint32 ptin_port, L7_uint16 outerVlanId, L7_uint16 innerVlanId, L7_uint16 newOuterVlanId )
{
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port=%u, outerVlanId=%u, innerVlanId=%u, newOuterVlanId=%u",
                 ptin_port, outerVlanId, innerVlanId, newOuterVlanId);

  /* Validate arguments */
  if (outerVlanId>4095 || innerVlanId>4095 || newOuterVlanId>4095)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Define structure */
  xlate.portgroup       = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.stage           = PTIN_XLATE_STAGE_INGRESS;
  xlate.outerVlan       = outerVlanId;
  xlate.innerVlan       = innerVlanId;
  xlate.outerVlan_new   = newOuterVlanId;
  xlate.innerVlan_new   = 0;
  xlate.outerVlanAction = PTIN_XLATE_ACTION_REPLACE;
  xlate.innerVlanAction = PTIN_XLATE_ACTION_DELETE;

  rc = ptin_xlate_operation(DAPI_CMD_SET, ptin_port, &xlate);

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: rc=%d", rc);

  return rc;
}

L7_RC_t ptin_xlate_egress_add_2( L7_uint32 ptin_port, L7_uint16 outerVlanId, L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId )
{
  L7_uint32 portgroup;
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port=%u, outerVlanId=%u, newOuterVlanId=%u newInnerVlanId=%u",
                 ptin_port, outerVlanId, newOuterVlanId, newInnerVlanId);

  /* Get class id */
  if (xlate_portgroup_from_intf(ptin_port, &portgroup)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR getting class id");
    return L7_FAILURE;
  }

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "portgroup=%u, outerVlanId=%u, newOuterVlanId=%u, newInnerVlanId=%u",
              portgroup, outerVlanId, newOuterVlanId, newInnerVlanId);

  /* Validate arguments */
  if (portgroup==0 || 
      outerVlanId>4095 || 
      newOuterVlanId>4095 || (newInnerVlanId!=(L7_uint16)-1 && newInnerVlanId>4095))
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Define structure */
  xlate.portgroup       = portgroup;
  xlate.stage           = PTIN_XLATE_STAGE_EGRESS;
  xlate.outerVlan       = outerVlanId;
  xlate.innerVlan       = 0;
  xlate.outerVlan_new   = newOuterVlanId;
  xlate.innerVlan_new   = newInnerVlanId;
  xlate.outerVlanAction = PTIN_XLATE_ACTION_REPLACE;
  xlate.innerVlanAction = PTIN_XLATE_ACTION_ADD;

  /* DTL call */
  rc = ptin_xlate_operation(DAPI_CMD_SET, PTIN_PORT_ALL, &xlate);

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: rc=%d", rc);

  return rc;
}



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

  PT_LOG_TRACE(LOG_CTX_XLATE, "Initializing xlate module...");

  /* Init data structs */
  rc1 = xlate_database_init();

  /* Reset pvid */
  rc2 = ptin_xlate_PVID_init();

  PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: rc1=%d, rc2=%d", rc1, rc2);  

  /* Return result */
  if (rc1!=L7_SUCCESS || rc2!=L7_SUCCESS)
  {
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}


/**
 * Get ingress translation new vlan
 * 
 * @param ptin_port : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new outer vlan id 
 * @param newInnerVlanId : new inner vlan id 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_ingress_get( L7_uint32 ptin_port, L7_uint16 outerVlanId, L7_uint16 innerVlanId,
                                L7_uint16 *newOuterVlanId, L7_uint16 *newInnerVlanId)
{
  L7_uint16 newOuterVid, newInnerVid;
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port=%u, outerVlanId=%u, innerVlanId=%u",
                 ptin_port, outerVlanId, innerVlanId);

  /* Validate arguments */
  if (outerVlanId>4095 || innerVlanId>4095)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialize new Vlan... as 0 (invalid value) */
  newOuterVid = PTIN_XLATE_NOT_DEFINED;
  newInnerVid = PTIN_XLATE_NOT_DEFINED;

  memset(&xlate, 0x00, sizeof(ptin_vlanXlate_t));
  xlate.stage     = PTIN_XLATE_STAGE_INGRESS;
  xlate.portgroup = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.outerVlan = outerVlanId;
  xlate.innerVlan = innerVlanId;

  rc = xlate_database_newVlan_get(ptin_port, &xlate);

  if (rc == L7_SUCCESS)
  {
    newOuterVid = xlate.outerVlan_new;
    newInnerVid = xlate.innerVlan_new;
  }
  else
  {
    if (ptin_debug_xlate)
      PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Entry not found");
    rc = L7_FAILURE;
  }
  #if 0
  /* Define structure */
  xlate.portgroup     = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.stage         = PTIN_XLATE_STAGE_INGRESS;
  xlate.outerVlan     = outerVlanId;
  xlate.innerVlan     = innerVlanId;
  xlate.outerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.outerAction   = PTIN_XLATE_ACTION_NONE;
  xlate.innerAction   = PTIN_XLATE_ACTION_NONE;

  /* DTL call */
  rc = ptin_xlate_operation(DAPI_CMD_GET, intIfNum, &xlate);

  /* If successfull, extract vlan to be returned */
  if ( rc == L7_SUCCESS )
  {
    newOuterVid = xlate.outerVlan_new;
    newInnerVid = xlate.innerVlan_new;
  }
  #endif

  /* Return value */
  if (rc==L7_SUCCESS)
  {
    if (newOuterVid > 4095 || newInnerVid > 4095)
    {
      rc = L7_FAILURE;
    }
    else
    {
      if (newOuterVlanId != L7_NULLPTR)   *newOuterVlanId = newOuterVid;
      if (newInnerVlanId != L7_NULLPTR)   *newInnerVlanId = newInnerVid;
    }
  }

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: newOVid=%u, newIVid=%u, rc=%d", newOuterVid, newInnerVid, rc);

  /* Return status */
  return rc;
}

/**
 * Get ingress translation original vlan
 * 
 * @param ptin_port : interface reference
 * @param outerVlanId : lookup outer vlan (to be returned) 
 * @param innerVlanId : lookup inner vlan (to be returned) 
 * @param newOuterVlanId : new outer vlan id 
 * @param newInnerVlanId : new inner vlan id   
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_ingress_get_originalVlan( L7_uint32 ptin_port, L7_uint16 *outerVlanId, L7_uint16 *innerVlanId,
                                             L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId )
{
  ptin_vlanXlate_t xlate;
  L7_uint16 oldOuterVid, oldInnerVid;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port=%u, newOuterVlanId=%u, newInnerVlanId=%u",
                 ptin_port, newOuterVlanId, newInnerVlanId);

  /* Validate arguments */
  if (newOuterVlanId > 4095 || newInnerVlanId > 4095)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialize new Vlan... as 0 (invalid value) */
  oldOuterVid = PTIN_XLATE_NOT_DEFINED;
  oldInnerVid = PTIN_XLATE_NOT_DEFINED;

  memset(&xlate, 0x00, sizeof(ptin_vlanXlate_t));
  xlate.stage         = PTIN_XLATE_STAGE_INGRESS;
  xlate.portgroup     = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.outerVlan_new = newOuterVlanId;
  xlate.innerVlan_new = newInnerVlanId;

  rc = xlate_database_oldVlan_get(ptin_port, &xlate);

  if (rc == L7_SUCCESS)
  {
    oldOuterVid = xlate.outerVlan;
    oldInnerVid = xlate.innerVlan;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Entry not found");
    rc = L7_FAILURE;
  }

  /* Return value */
  if (rc==L7_SUCCESS)
  {
    if (oldOuterVid > 4095 || oldInnerVid > 4095)
    {
      rc = L7_FAILURE;
    }
    else
    {
      if (outerVlanId != L7_NULLPTR)  *outerVlanId = oldOuterVid;
      if (innerVlanId != L7_NULLPTR)  *innerVlanId = oldInnerVid;
    }
  }

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: oldOVlan=%u, oldIVid=%u rc=%d", oldOuterVid, oldInnerVid, rc);

  /* Return status */
  return rc;
}

/**
 * Set ingress translation rule (single tag)
 * 
 * @param port 
 * @param outer_vlan 
 * @param op 
 * @param newOuterVlanId 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_xlate_ingress_set( L7_uint port, L7_uint16 outer_vlan, L7_uint op, L7_uint16 newOuterVlanId)
{
  ptin_HwEthMef10Intf_t intf_vlan;
  L7_RC_t rc;

  memset(&intf_vlan, 0x00, sizeof(intf_vlan));

  intf_vlan.intf.format = PTIN_INTF_FORMAT_PORT;
  intf_vlan.intf.value.ptin_port = port;

  if (ptin_intf_any_format(&intf_vlan.intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE,"Invalid port %d", port);
    return L7_FAILURE;
  }

  intf_vlan.vid = outer_vlan;
  intf_vlan.action_outer = op;

  rc = ptin_xlate_ingress_add(&intf_vlan, newOuterVlanId, 0, -1, -1);

  PT_LOG_TRACE(LOG_CTX_XLATE,"Operation result: rc=%d", rc);

  return rc;
}

/**
 * Set ingress translation rule (double tag)
 * 
 * @author mruas (1/20/2016)
 * 
 * @param port 
 * @param outer_vlan 
 * @param inner_vlan 
 * @param newOuterVlanId 
 * @param newInnerVlanId 
 * @param outer_op 
 * @param inner_op 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_xlate_double_ingress_set(L7_uint port, L7_uint16 outer_vlan, L7_uint16 inner_vlan, L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId, L7_uint outer_op, L7_uint inner_op)
{
  ptin_HwEthMef10Intf_t intf_vlan;
  L7_RC_t rc;

  memset(&intf_vlan, 0x00, sizeof(intf_vlan));

  intf_vlan.intf.format = PTIN_INTF_FORMAT_PORT;
  intf_vlan.intf.value.ptin_port = port;

  if (ptin_intf_any_format(&intf_vlan.intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE,"Invalid port %d", port);
    return L7_FAILURE;
  }

  intf_vlan.vid = outer_vlan;
  intf_vlan.vid_inner = inner_vlan;
  intf_vlan.action_outer = outer_op;
  intf_vlan.action_inner = inner_op;

  rc = ptin_xlate_ingress_add(&intf_vlan, newOuterVlanId, newInnerVlanId, -1, -1);

  PT_LOG_TRACE(LOG_CTX_XLATE,"Operation result: rc=%d", rc);

  return rc;
}

/**
 * Set egress translation rule (single tag)
 * 
 * @param port 
 * @param outer_vlan 
 * @param op 
 * @param newOuterVlanId 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_xlate_egress_set( L7_uint port, L7_uint16 outer_vlan, L7_uint op, L7_uint16 newOuterVlanId)
{
  ptin_HwEthMef10Intf_t intf_vlan;
  L7_RC_t rc;

  memset(&intf_vlan, 0x00, sizeof(intf_vlan));

  intf_vlan.intf.format = PTIN_INTF_FORMAT_PORT;
  intf_vlan.intf.value.ptin_port = port;

  if (ptin_intf_any_format(&intf_vlan.intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE,"Invalid port %d", port);
    return L7_FAILURE;
  }

  intf_vlan.vid = outer_vlan;
  intf_vlan.action_outer = op;

  rc = ptin_xlate_egress_add(&intf_vlan, newOuterVlanId, 0, -1, -1);

  PT_LOG_TRACE(LOG_CTX_XLATE,"Operation result: rc=%d", rc);

  return rc;
}

/**
 * Set egress translation rule (double tag)
 * 
 * @author mruas (1/20/2016)
 * 
 * @param port 
 * @param outer_vlan 
 * @param inner_vlan 
 * @param newOuterVlanId 
 * @param newInnerVlanId 
 * @param outer_op 
 * @param inner_op 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_xlate_double_egress_set(L7_uint port, L7_uint16 outer_vlan, L7_uint16 inner_vlan, L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId, L7_uint outer_op, L7_uint inner_op)
{
  ptin_HwEthMef10Intf_t intf_vlan;
  L7_RC_t rc;

  memset(&intf_vlan, 0x00, sizeof(intf_vlan));

  intf_vlan.intf.format = PTIN_INTF_FORMAT_PORT;
  intf_vlan.intf.value.ptin_port = port;

  if (ptin_intf_any_format(&intf_vlan.intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE,"Invalid port %d", port);
    return L7_FAILURE;
  }

  intf_vlan.vid = outer_vlan;
  intf_vlan.vid_inner = inner_vlan;
  intf_vlan.action_outer = outer_op;
  intf_vlan.action_inner = inner_op;

  rc = ptin_xlate_egress_add(&intf_vlan, newOuterVlanId, newInnerVlanId, -1, -1);

  PT_LOG_TRACE(LOG_CTX_XLATE,"Operation result: rc=%d", rc);

  return rc;
}

/**
 * Delete ingress translation rule (single/double tag)
 * 
 * @author mruas (1/20/2016)
 * 
 * @param port 
 * @param outer_vlan 
 * @param inner_vlan 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_xlate_ingress_clear( L7_uint port, L7_uint16 outer_vlan, L7_uint16 inner_vlan)
{
  L7_RC_t rc;

  rc = ptin_xlate_ingress_delete(port, outer_vlan, inner_vlan);

  PT_LOG_TRACE(LOG_CTX_XLATE,"Operation result: rc=%d", rc);

  return rc;
}

/**
 * Delete egress translation rule (single/double tag)
 * 
 * @author mruas (1/20/2016)
 * 
 * @param port 
 * @param outer_vlan 
 * @param inner_vlan 
 * 
 * @return L7_RC_t 
 */
L7_RC_t ptin_xlate_egress_clear( L7_uint port, L7_uint16 outer_vlan, L7_uint16 inner_vlan)
{
  L7_RC_t rc;

  rc = ptin_xlate_egress_delete(port, outer_vlan, inner_vlan);

  PT_LOG_TRACE(LOG_CTX_XLATE,"Operation result: rc=%d", rc);

  return rc;
}

/**
 * Add ingress translation entry
 *  
 * @param intf_vlan: intf/VLAN input 
 * @param newOuterVlanId : new vlan id 
 * @param newInnerVlanId : new inner vlan to be added (0 to not 
 *                       be added)
 * @param newOuterPrio : new outer prio (-1 to not be used)
 * @param newInnerPrio : new inner prio (-1 to not be used)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_ingress_add( ptin_HwEthMef10Intf_t *intf_vlan,
                                L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId,
                                L7_int newOuterPrio, L7_int newInnerPrio )
{
  L7_uint32 intIfNum, ptin_port;
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  intIfNum  = intf_vlan->intf.value.intIfNum;
  ptin_port = intf_vlan->intf.value.ptin_port;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port=%u, outerVlanId=%u, innerVlanId=%u, newOuterVlanId=%u, newInnerVlanId=%u, newOuterPrio=%u, newInnerPrio=%u",
                 intf_vlan->intf.value.ptin_port, intf_vlan->vid, intf_vlan->vid_inner, newOuterVlanId, newInnerVlanId, newOuterPrio, newInnerPrio);

  /* Validate ptin_port */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, "Invalid intIfNum %d", ptin_port);
    return L7_FAILURE;
  }

  /* Validate arguments */
  if (intf_vlan->vid>4095 || /*innerVlanId>4095 ||*/
      newOuterVlanId>4095 /*|| newInnerVlanId>4095*/)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Configure DefVID using VCAP rules (only for single tagged packets) */
  if (xlate_table_pvid[ptin_port] == intf_vlan->vid && (intf_vlan->vid_inner == 0 || intf_vlan->vid_inner >= 4096))
  {
    /* Set default VLAN */
    if (usmDbQportsPVIDSet(1, intIfNum, newOuterVlanId) != L7_SUCCESS) 
    {
      PT_LOG_ERR(LOG_CTX_INTF, "Error applying defVID %u for ptin_port %u", newOuterVlanId, ptin_port);
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_INTF, "defVID %u for ptin_port %u applied", newOuterVlanId, ptin_port);
    }
    /* Configure defVID using a VCAP rule (only for double tagged after translation) */
    if (newInnerVlanId >= 1 && newInnerVlanId <= 4095)
    {
      if (ptin_intf_vcap_defvid(ptin_port, newOuterVlanId, newInnerVlanId) != L7_SUCCESS) 
      {
        PT_LOG_ERR(LOG_CTX_XLATE,"Error configuring VCAP defVID %u+%u for ptin_port %u", newOuterVlanId, newInnerVlanId, ptin_port);
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_XLATE,"VCAP defVID %u+%u for ptin_port %u configured", newOuterVlanId, newInnerVlanId, ptin_port);
      }
    }
  }

  /* Define structure */
  memset(&xlate, 0x00, sizeof(ptin_vlanXlate_t));
  xlate.portgroup       = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.stage           = PTIN_XLATE_STAGE_INGRESS;

  xlate.outerVlan       = (intf_vlan->vid > 4095) ? 0 : intf_vlan->vid;
  xlate.innerVlan       = (intf_vlan->vid_inner > 4095) ? 0 : intf_vlan->vid_inner;
  xlate.outerVlan_new   = newOuterVlanId;
  xlate.innerVlan_new   = (newInnerVlanId > 4095) ? 0 : newInnerVlanId;

  xlate.outerPrio       = 0;
  xlate.innerPrio       = 0;
  xlate.outerPrio_new   = (newOuterPrio >= 0 && newOuterPrio <= 7) ? newOuterPrio : 0;
  xlate.innerPrio_new   = (newInnerPrio >= 0 && newInnerPrio <= 7) ? newInnerPrio : 0;

  xlate.outerVlanAction = intf_vlan->action_outer;
  xlate.innerVlanAction = intf_vlan->action_inner;
  xlate.outerPrioAction = (newOuterPrio >= 0 && newOuterPrio <= 7) ? PTIN_XLATE_ACTION_REPLACE : PTIN_XLATE_ACTION_NONE;
#if (PTIN_BOARD == PTIN_BOARD_TG16G)
  /* this operation is not supported on this switch*/
  xlate.innerPrioAction = PTIN_XLATE_ACTION_NONE;
#else
  xlate.innerPrioAction = (intf_vlan->action_inner == PTIN_XLATE_ACTION_ADD) ? PTIN_XLATE_ACTION_COPY : PTIN_XLATE_ACTION_NONE;
#endif
  /* If inner VLAN is to be added, no information should be provided about the input inner vlan (no push allowed to the inner tag) */
  if (intf_vlan->action_inner == PTIN_XLATE_ACTION_ADD)
  {
    xlate.innerVlan = 0;
  }

  /* Correct actions according to new vlan values */
  if (newOuterVlanId == 0)                /* If new VLANID is 0, do nothing */
  {
    xlate.outerVlanAction = PTIN_XLATE_ACTION_NONE;
  }
  else if (newOuterVlanId > 4095)         /* If new VLANID is -1, delete it */
  {
    xlate.outerVlanAction = PTIN_XLATE_ACTION_DELETE;
  }
//else if (intf_vlan->vid == 0)           /* If valid VLAN was provided, add a new one */
//{
//  xlate.outerVlanAction = PTIN_XLATE_ACTION_ADD;
//}

#if ( PTIN_BOARD_IS_MATRIX )
  /* No inner VLAN action on Matrix board */
  xlate.innerVlanAction = PTIN_XLATE_ACTION_NONE;
#else
  if (newInnerVlanId == 0)                /* If new VLANID is 0, do nothing */
  {
    xlate.innerVlanAction = PTIN_XLATE_ACTION_NONE;
  }
  else if (newInnerVlanId > 4095)         /* If new VLANID is -1, delete it */
  {
    xlate.innerVlanAction = PTIN_XLATE_ACTION_DELETE;
  }
//else if (intf_vlan->vid_inner == 0)     /* If no valid (input) VLAN was provided, always replace (Add is not allowed) */
//{
//  xlate.innerVlanAction = PTIN_XLATE_ACTION_REPLACE;
//}
#endif

  /* Never remove VLANs at ingress translation */
  xlate.remove_VLANs = L7_FALSE;

  /* DTL call */
  rc = ptin_xlate_operation(DAPI_CMD_SET, ptin_port, &xlate);

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Delete ingress translation entry
 * 
 * @param ptin_port : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_ingress_delete( L7_uint32 ptin_port, L7_uint16 outerVlanId, L7_uint16 innerVlanId )
{
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port=%u, outerVlanId=%u, innerVlanId=%u",
                 ptin_port, outerVlanId, innerVlanId);

  /* Validate intIfNum */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, "Invalid ptin_port %d", ptin_port);
    return L7_FAILURE;
  }

  /* Validate arguments */
  if (outerVlanId>4095 || innerVlanId>4095)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Remove VCAP rule associated to DefVid (only for single tagged packets) */
  if (xlate_table_pvid[ptin_port] == outerVlanId && (innerVlanId == 0 || innerVlanId >= 4096))
  {
    if (ptin_intf_vcap_defvid(ptin_port, 0, 0) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_XLATE,"Error removing VCAP rule from ptin_port %u", ptin_port);
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_XLATE,"VCAP rule removed from ptin_port %u", ptin_port);
    }
  }

  /* Define structure */
  memset(&xlate, 0x00, sizeof(ptin_vlanXlate_t));
  xlate.portgroup       = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.stage           = PTIN_XLATE_STAGE_INGRESS;
  xlate.outerVlan       = outerVlanId;
  xlate.innerVlan       = innerVlanId;
  xlate.outerVlan_new   = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan_new   = PTIN_XLATE_NOT_DEFINED;
  xlate.outerVlanAction = PTIN_XLATE_ACTION_NONE;
  xlate.innerVlanAction = PTIN_XLATE_ACTION_NONE;

  /* DTL call */
  rc = ptin_xlate_operation(DAPI_CMD_CLEAR, ptin_port, &xlate);

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: rc=%d", rc);

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

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Beginning...");

  /* Define structure */
  xlate.portgroup       = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.stage           = PTIN_XLATE_STAGE_INGRESS;
  xlate.outerVlan       = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan       = PTIN_XLATE_NOT_DEFINED;
  xlate.outerVlan_new   = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan_new   = PTIN_XLATE_NOT_DEFINED;
  xlate.outerVlanAction = PTIN_XLATE_ACTION_NONE;
  xlate.innerVlanAction = PTIN_XLATE_ACTION_NONE;

  /* DTL call */
  rc = ptin_xlate_operation(DAPI_CMD_CLEAR_ALL, PTIN_PORT_ALL, &xlate);

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Get egress translation new vlan
 * 
 * @param ptin_port : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new outer vlan id 
 * @param newInnerVlanId : new inner vlan id 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_get( L7_uint32 ptin_port, L7_uint16 outerVlanId, L7_uint16 innerVlanId,
                               L7_uint16 *newOuterVlanId, L7_uint16 *newInnerVlanId )
{
  L7_uint32 class_id;
  L7_uint16 newOuterVid, newInnerVid;
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port=%u, outerVlanId=%u, innerVlanId=%u",
                 ptin_port, outerVlanId, innerVlanId);

  /* Get class id */
  if (xlate_portgroup_from_intf(ptin_port, &class_id)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR getting class id");
    return L7_FAILURE;
  }

  #if 0
  rc = ptin_xlate_egress_portgroup_get(class_id, outerVlanId, innerVlanId, newOuterVlanId, newInnerVlanId);
  #endif

  /* Validate arguments */
  if (class_id == 0 || 
      outerVlanId > 4095 || innerVlanId > 4095)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialize new Vlan... as 0 (invalid value) */
  newOuterVid = PTIN_XLATE_NOT_DEFINED;
  newInnerVid = PTIN_XLATE_NOT_DEFINED;

  memset(&xlate, 0x00, sizeof(ptin_vlanXlate_t));
  xlate.stage     = PTIN_XLATE_STAGE_EGRESS;
  xlate.portgroup = class_id;
  xlate.outerVlan = outerVlanId;
  xlate.innerVlan = innerVlanId;

  rc = xlate_database_newVlan_get(PTIN_PORT_ALL, &xlate);

  if (rc == L7_SUCCESS)
  {
    newOuterVid = xlate.outerVlan_new; 
    newInnerVid = xlate.innerVlan_new; 
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Entry not found");
    rc = L7_FAILURE;
  }

  /* Return value */
  if (rc==L7_SUCCESS)
  {
    if (newOuterVid > 4095 || newInnerVid > 4095)
    {
      rc = L7_FAILURE;
    }
    else
    {
      if (newOuterVlanId != L7_NULLPTR)   *newOuterVlanId = newOuterVid;
      if (newInnerVlanId != L7_NULLPTR)   *newInnerVlanId = newInnerVid;
    }
  }

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: newOVid=%um newIVid=%u, rc=%d", newOuterVid, newInnerVid, rc);

  /* Return status */
  return rc;
}

/**
 * Get egress translation original vlan
 * 
 * @param ptin_port : interface reference
 * @param outerVlanId : lookup outer vlan (to be returned) 
 * @param innerVlanId : lookup inner vlan (to be returned)  
 * @param newOuterVlanId : new outer vlan id 
 * @param newInnerVlanId : new inner vlan id   
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_get_originalVlan( L7_uint32 ptin_port, L7_uint16 *outerVlanId, L7_uint16 *innerVlanId,
                                            L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId )
{
  L7_uint32 class_id;
  L7_uint16 oldOuterVid, oldInnerVid;
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port=%u, newOuterVlanId=%u, newInnerVlanId=%u",
                 ptin_port, newOuterVlanId, newInnerVlanId);

  /* Get class id */
  if (xlate_portgroup_from_intf(ptin_port, &class_id)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR getting class id");
    return L7_FAILURE;
  }

  #if 0
  rc = ptin_xlate_egress_portgroup_get_originalVlan(class_id, outerVlanId, innerVlanId, newOuterVlanId, newInnerVlanId);
  #endif

  /* Validate arguments */
  if (class_id == 0 || 
      newOuterVlanId > 4095 || newInnerVlanId > 4095)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialize new Vlan... as 0 (invalid value) */
  oldOuterVid = PTIN_XLATE_NOT_DEFINED;
  oldInnerVid = PTIN_XLATE_NOT_DEFINED;

  memset(&xlate, 0x00, sizeof(ptin_vlanXlate_t));
  xlate.stage         = PTIN_XLATE_STAGE_EGRESS;
  xlate.portgroup     = class_id;
  xlate.outerVlan_new = newOuterVlanId;
  xlate.innerVlan_new = newInnerVlanId;

  rc = xlate_database_oldVlan_get(PTIN_PORT_ALL, &xlate);

  if (rc == L7_SUCCESS)
  {
    oldOuterVid = xlate.outerVlan; 
    oldInnerVid = xlate.innerVlan; 
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Entry not found");
    rc = L7_FAILURE;
  }

  /* Return value */
  if (rc==L7_SUCCESS)
  {
    if (oldOuterVid > 4095 || oldInnerVid > 4095)
    {
      rc = L7_FAILURE;
    }
    else
    {
      if (outerVlanId != L7_NULLPTR)  *outerVlanId = oldOuterVid;
      if (innerVlanId != L7_NULLPTR)  *innerVlanId = oldInnerVid;
    }
  }

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: oldOVlan=%u, oldIVid=%u, rc=%d", oldOuterVid, oldInnerVid, rc);

  /* Return status */
  return rc;
}

/**
 * Add egress translation entry
 *  
 * @param intf_vlan: intf/VLAN input  
 * @param newOuterVlanId : new vlan id 
 * @param newInnerVlanId : new inner vlan id  
 * @param newOuterPrio : new outer prio (-1 to not be used)
 * @param newInnerPrio : new inner prio (-1 to not be used)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_add( ptin_HwEthMef10Intf_t *intf_vlan,
                               L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId,
                               L7_int newOuterPrio, L7_int newInnerPrio)
{
  L7_uint32 intIfNum, ptin_port, class_id;
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 unit = 0;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port=%u, outerVlanId=%u, innerVlanId=%u, newOuterVlanId=%u, newInnerVlanId=%u, newOuterPrio=%u, newInnerPrio=%u",
              intf_vlan->intf.value.ptin_port, intf_vlan->vid, intf_vlan->vid_inner, newOuterVlanId, newInnerVlanId, newOuterPrio, newInnerPrio);

  intIfNum  = intf_vlan->intf.value.intIfNum;
  ptin_port = intf_vlan->intf.value.ptin_port;

  /* Validate intIfNum */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, "Invalid ptin_port %d", ptin_port);
    return L7_FAILURE;
  }

  /* Get class id */
  if (xlate_portgroup_from_intf(ptin_port, &class_id)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR getting class id");
    return L7_FAILURE;
  }

  #if 0
  if ( (xlate_table_pvid[ptin_port] == newOuterVlanId) && (newInnerVlanId > 4095) )
  {
    rc = ptin_xlate_egress_portgroup_add(class_id, outerVlanId, innerVlanId, -1, -1);
  }
  else
  {
    rc = ptin_xlate_egress_portgroup_add(class_id, outerVlanId, innerVlanId, newOuterVlanId, newInnerVlanId);
  }
  #endif

  /* Validate arguments */
  if (class_id == 0 || 
      intf_vlan->vid > 4095 /*|| innerVlanId > 4095*/)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Define structure */
  memset(&xlate, 0x00, sizeof(ptin_vlanXlate_t));
  xlate.portgroup     = class_id;

  xlate.stage         = PTIN_XLATE_STAGE_EGRESS;

  xlate.outerVlan     = (intf_vlan->vid > 4095) ? 0 : intf_vlan->vid;
  xlate.innerVlan     = (intf_vlan->vid_inner > 4095) ? 0 : intf_vlan->vid_inner;
  xlate.outerVlan_new = (newOuterVlanId > 4095) ? 0 : newOuterVlanId;
  xlate.innerVlan_new = (newInnerVlanId > 4095) ? 0 : newInnerVlanId;

  xlate.outerPrio     = 0;
  xlate.innerPrio     = 0;
  xlate.outerPrio_new = (newOuterPrio >= 0 && newOuterPrio <= 7) ? newOuterPrio : 0;
  xlate.innerPrio_new = (newInnerPrio >= 0 && newInnerPrio <= 7) ? newInnerPrio : 0;

  xlate.outerVlanAction = intf_vlan->action_outer;
  xlate.innerVlanAction = intf_vlan->action_inner;
  xlate.outerPrioAction = (newOuterPrio >= 0 && newOuterPrio <= 7) ? PTIN_XLATE_ACTION_REPLACE : PTIN_XLATE_ACTION_NONE;
#if (PTIN_BOARD == PTIN_BOARD_TG16G)
  /* this operation is not supported on this switch*/
  xlate.innerPrioAction = PTIN_XLATE_ACTION_NONE;
#else
  xlate.innerPrioAction = (intf_vlan->action_inner == PTIN_XLATE_ACTION_ADD) ? PTIN_XLATE_ACTION_COPY : PTIN_XLATE_ACTION_NONE;
#endif
  /* If inner VLAN is to be added, no information should be provided about the input inner vlan (no push allowed to the inner tag) */
  if (intf_vlan->action_inner == PTIN_XLATE_ACTION_ADD)
  {
    xlate.innerVlan = 0;
  }

  /* Correct actions according to new vlan values */
  if (newOuterVlanId == 0)                /* If new VLANID is 0, do nothing */
  {
    xlate.outerVlanAction = PTIN_XLATE_ACTION_NONE;
  }
  else if (newOuterVlanId > 4095)         /* If new VLANID is -1, delete it */
  {
    xlate.outerVlanAction = PTIN_XLATE_ACTION_DELETE;
  }
//else if (intf_vlan->vid == 0)           /* If valid VLAN was provided, add a new one */
//{
//  xlate.outerVlanAction = PTIN_XLATE_ACTION_ADD;
//}

#if ( PTIN_BOARD_IS_MATRIX )
  /* No inner VLAN action on Matrix board */
  xlate.innerVlanAction = PTIN_XLATE_ACTION_NONE;
#else
  if (newInnerVlanId == 0)                /* If new VLANID is 0, do nothing */
  {
    xlate.innerVlanAction = PTIN_XLATE_ACTION_NONE;
  }
  else if (newInnerVlanId > 4095)         /* If new VLANID is -1, delete it */
  {
    xlate.innerVlanAction = PTIN_XLATE_ACTION_DELETE;
  }
//else if (intf_vlan->vid_inner == 0)     /* If no valid (input) VLAN was provided, always replace (Add is not allowed) */
//{
//  xlate.innerVlanAction = PTIN_XLATE_ACTION_REPLACE;
//}
#endif

  /* Remove VLANs? */
  xlate.remove_VLANs = (xlate_table_pvid[ptin_port] == newOuterVlanId);

  if (xlate.remove_VLANs)
  {
    /* Set untagged port */
    if (usmDbVlanTaggedSet(unit, intf_vlan->vid, intIfNum, L7_DOT1Q_UNTAGGED) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_EVC, "Error setting ptin_port# %u internal VLAN %u as UNtagged", ptin_port, intf_vlan->vid);
      return L7_FAILURE;
    }
  }

  /* DTL call */
  rc = ptin_xlate_operation(DAPI_CMD_SET, PTIN_PORT_ALL, &xlate);

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: ptin_port=%u, class_id=%u,  rc=%d", ptin_port, class_id, rc);

/* Mirror */
#if ( PTIN_BOARD_IS_MATRIX )

  L7_uint32 ptin_port_src, ptin_port_dst, intIfNum_Dst , sessionNum;
  L7_RC_t rc_aux = L7_SUCCESS;

  /* Check if is a mirror Port*/
   mirrorIsDestConfigured(intIfNum, &sessionNum);

  /* Convert to ptin format*/
  rc_aux = mirrorDestPortGet(1,&intIfNum_Dst); /* 1 -> SessionNum*/
  PT_LOG_TRACE(LOG_CTX_XLATE, "Mirror Configuration : rc_aux=%d", rc_aux); 

  if (rc_aux == L7_SUCCESS) 
  { 
    /* Convert to ptin format*/
    ptin_port_src = ptin_port;
    /* FIXME TC16SXG: intIfNum->ptin_port */
    ptin_intf_intIfNum2port(intIfNum_Dst, xlate.outerVlan_new /*vlan*/, &ptin_port_dst);
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port_src=%d", ptin_port_src);
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port_dst=%d", ptin_port_dst);

    /* Configure xlate to the destination mirror port */
    xlate_outer_vlan_replicate_Dstport(1, ptin_port_src , ptin_port_dst); /* 1 -> Configure xlate */
  }
  /*else -> Do nothing*/
#endif

  return rc;
}

/**
 * Delete egress translation entry
 * 
 * @param ptin_port : interface reference
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_delete( L7_uint32 ptin_port, L7_uint16 outerVlanId, L7_uint16 innerVlanId )
{
  L7_uint32 class_id;
  L7_uint32 intIfNum;
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;
  
  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port=%u, outerVlanId=%u, innerVlanId=%u",
                 ptin_port, outerVlanId, innerVlanId);

  /* Obtain intIfNum */
  if ( ptin_intf_port2intIfNum(ptin_port, &intIfNum)!=L7_SUCCESS )
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  /* Get class id */
  if (xlate_portgroup_from_intf(ptin_port, &class_id)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR getting class id");
    return L7_FAILURE;
  }

  #if 0
  rc = ptin_xlate_egress_portgroup_delete(class_id, outerVlanId, innerVlanId);
  #endif

  /* Validate arguments */
  if (class_id == 0 || 
      outerVlanId > 4095 /*|| innerVlanId > 4095*/)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Define structure */
  memset(&xlate, 0x00, sizeof(ptin_vlanXlate_t));
  xlate.portgroup       = class_id;
  xlate.stage           = PTIN_XLATE_STAGE_EGRESS;
  xlate.outerVlan       = outerVlanId;
  xlate.innerVlan       = (innerVlanId > 4095) ? 0 : innerVlanId;
  xlate.outerVlan_new   = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan_new   = PTIN_XLATE_NOT_DEFINED;
  xlate.outerVlanAction = PTIN_XLATE_ACTION_NONE;
  xlate.innerVlanAction = PTIN_XLATE_ACTION_NONE;

  /* DTL call */
  rc = ptin_xlate_operation(DAPI_CMD_CLEAR, PTIN_PORT_ALL, &xlate);

  #if 0//Already Performed Above
  /* If deletion went well... */
  if (rc == L7_SUCCESS)
  {
    rc = xlate_database_clear(L7_ALL_INTERFACES, &xlate);
  }
  #endif

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: rc=%d", rc);

/* Mirror */
#if ( PTIN_BOARD_IS_MATRIX )

  L7_RC_t rc_aux = L7_FAILURE;
  L7_uint32 ptin_port_src, ptin_port_dst, intIfNum_Dst , sessionNum;

  /* Check if is a mirror Port*/
  rc_aux = mirrorIsDestConfigured(intIfNum, &sessionNum);
  PT_LOG_TRACE(LOG_CTX_XLATE, "rc_aux=%d", rc_aux);  

  if (rc_aux == L7_TRUE) 
  {
    ptin_port_src = ptin_port;
    /* FIXME TC16SXG: intIfNum->ptin_port */
    ptin_intf_intIfNum2port(intIfNum_Dst, 0 /*Vlan*/, &ptin_port_dst);
    mirrorDestPortGet(1, &intIfNum_Dst); /* 1 -> SessionNum*/

    /* Configure xlate to the destination mirror port */
    xlate_outer_vlan_replicate_Dstport(2, ptin_port_src , ptin_port_dst); /* 2 -> Remove xlate */
  }
  /*else -> Do nothing*/
#endif

  return rc;
}

#if 0
/**
 * Get egress translation new vlan
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new outer vlan id 
 * @param newInnerVlanId : new inner vlan id  
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_portgroup_get( L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 innerVlanId,
                                         L7_uint16 *newOuterVlanId, L7_uint16 *newInnerVlanId )
{
  L7_uint16 newOuterVid, newInnerVid;
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "portgroup=%u, outerVlanId=%u, innerVlanId=%u",
              portgroup, outerVlanId, innerVlanId);

  /* Validate arguments */
  if (portgroup==0 || 
      outerVlanId>4095 || innerVlanId>4095)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialize new Vlan... as 0 (invalid value) */
  newOuterVid = PTIN_XLATE_NOT_DEFINED;
  newInnerVid = PTIN_XLATE_NOT_DEFINED;

  memset(&xlate, 0x00, sizeof(ptin_vlanXlate_t));
  xlate.stage     = PTIN_XLATE_STAGE_EGRESS;
  xlate.portgroup = portgroup;
  xlate.outerVlan = outerVlanId;
  xlate.innerVlan = innerVlanId;

  rc = xlate_database_newVlan_get(L7_ALL_INTERFACES, &xlate);

  if (rc == L7_SUCCESS)
  {
    newOuterVid = xlate.outerVlan_new; 
    newInnerVid = xlate.innerVlan_new; 
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Entry not found");
    rc = L7_FAILURE;
  }

  #if 0
  /* Define structure */
  xlate.portgroup     = portgroup;
  xlate.stage         = PTIN_XLATE_STAGE_EGRESS;
  xlate.outerVlan     = outerVlanId;
  xlate.innerVlan     = innerVlanId;
  xlate.outerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.outerAction   = PTIN_XLATE_ACTION_NONE;
  xlate.innerAction   = PTIN_XLATE_ACTION_NONE;

  /* DTL call */
  rc = ptin_xlate_operation(DAPI_CMD_GET, L7_ALL_INTERFACES, &xlate);

  /* If successfull, extract vlan to be returned */
  if ( rc == L7_SUCCESS )
  {
    newOuterVid = xlate.outerVlan_new;
    newInnerVid = xlate.innerVlan_new;
  }
  #endif

  /* Return value */
  if (rc==L7_SUCCESS)
  {
    if (newOuterVid > 4095 || newInnerVid > 4095)
    {
      rc = L7_FAILURE;
    }
    else
    {
      if (newOuterVlanId != L7_NULLPTR)   *newOuterVlanId = newOuterVid;
      if (newInnerVlanId != L7_NULLPTR)   *newInnerVlanId = newInnerVid;
    }
  }

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: newOVid=%um newIVid=%u, rc=%d", newOuterVid, newInnerVid, rc);

  /* Return status */
  return rc;
}
#endif

#if 0
/**
 * Get egress translation original vlan
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan (to be returned) 
 * @param innerVlanId : lookup inner vlan (to be returned)  
 * @param newOuterVlanId : new outer vlan id 
 * @param newInnerVlanId : new inner vlan id   
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_portgroup_get_originalVlan( L7_uint32 portgroup, L7_uint16 *outerVlanId, L7_uint16 *innerVlanId,
                                                      L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId )
{
  L7_uint16 oldOuterVid, oldInnerVid;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "portgroup=%u, newOuterVlanId=%u, newInnerVlanId=%u",
              portgroup, newOuterVlanId, newInnerVlanId);

  /* Validate arguments */
  if (portgroup==0 || 
      newOuterVlanId > 4095 || newInnerVlanId > 4095)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Initialize new Vlan... as 0 (invalid value) */
  oldOuterVid = PTIN_XLATE_NOT_DEFINED;
  oldInnerVid = PTIN_XLATE_NOT_DEFINED;

  ptin_vlanXlate_t xlate;

  memset(&xlate, 0x00, sizeof(ptin_vlanXlate_t));
  xlate.stage         = PTIN_XLATE_STAGE_EGRESS;
  xlate.portgroup     = portgroup;
  xlate.outerVlan_new = newOuterVlanId;
  xlate.innerVlan_new = newInnerVlanId;

  rc = xlate_database_oldVlan_get(L7_ALL_INTERFACES, &xlate);

  if (rc == L7_SUCCESS)
  {
    oldOuterVid = xlate.outerVlan; 
    oldInnerVid = xlate.innerVlan; 
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Entry not found");
    rc = L7_FAILURE;
  }

  /* Return value */
  if (rc==L7_SUCCESS)
  {
    if (oldOuterVid > 4095 || oldInnerVid > 4095)
    {
      rc = L7_FAILURE;
    }
    else
    {
      if (outerVlanId != L7_NULLPTR)  *outerVlanId = oldOuterVid;
      if (innerVlanId != L7_NULLPTR)  *innerVlanId = oldInnerVid;
    }
  }

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: oldOVlan=%u, oldIVid=%u, rc=%d", oldOuterVid, oldInnerVid, rc);

  /* Return status */
  return rc;
}
#endif

#if 0
/**
 * Add egress translation entry
 * 
 * @param portgroup : port group id
 * @param outerVlanId : lookup outer vlan
 * @param innerVlanId : lookup inner vlan (0 to not be used)
 * @param newOuterVlanId : new vlan id 
 * @param newInnerVlanId : new inner vlan id 
 * @param remove_VLANs : flag to remove VLANs 
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_portgroup_add( L7_uint32 portgroup, L7_uint16 outerVlanId, L7_uint16 innerVlanId,
                                         L7_uint16 newOuterVlanId, L7_uint16 newInnerVlanId, L7_BOOL remove_VLANs )
{
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "portgroup=%u, outerVlanId=%u, innerVlanId=%u, newOuterVlanId=%u, newInnerVlanId=%u",
              portgroup, outerVlanId, innerVlanId, newOuterVlanId, newInnerVlanId);

  /* Validate arguments */
  if (portgroup==0 || 
      outerVlanId>4095 /*|| innerVlanId>4095 ||
      newOuterVlanId>4095 || (newInnerVlanId!=(L7_uint16)-1 && newInnerVlanId>4095)*/)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Define structure */
  xlate.portgroup     = portgroup;
  xlate.stage         = PTIN_XLATE_STAGE_EGRESS;
  xlate.outerVlan     = outerVlanId;
  xlate.innerVlan     = (innerVlanId > 4095) ? 0 : innerVlanId;
  xlate.outerVlan_new = (newOuterVlanId > 4095) ? 0 : newOuterVlanId;
  xlate.innerVlan_new = (newInnerVlanId>4095) ? 0 : newInnerVlanId;
  xlate.outerAction   = PTIN_XLATE_ACTION_REPLACE;
#if ( PTIN_BOARD_IS_MATRIX )
  xlate.innerAction   = PTIN_XLATE_ACTION_NONE;
#else
  if (newInnerVlanId == 0)            /* If new inner VLANID is 0, do nothing */
  {
    xlate.innerAction = PTIN_XLATE_ACTION_NONE;
  }
  else if (newInnerVlanId > 4095)     /* If new inner VLANID is -1, delete it */
  {
    xlate.innerAction = PTIN_XLATE_ACTION_DELETE;
  }
  /* Valid new inner vlan id */
  else if (innerVlanId > 4095)          /* If current inner VLANID is -1, it means it does not exist */
  {
    xlate.innerAction = PTIN_XLATE_ACTION_ADD;
  }
  else                                /* Current inner vlan exists -> Do a replace */
  {
    xlate.innerAction = PTIN_XLATE_ACTION_REPLACE;
  }
  //xlate.innerAction = (newInnerVlanId>4095) ? PTIN_XLATE_ACTION_DELETE : PTIN_XLATE_ACTION_NONE;

  xlate.outerAction = (newOuterVlanId>4095) ? PTIN_XLATE_ACTION_DELETE : PTIN_XLATE_ACTION_REPLACE;
#endif

  /* DTL call */
  rc = ptin_xlate_operation(DAPI_CMD_SET, L7_ALL_INTERFACES, &xlate);

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: rc=%d", rc);

  return rc;
}
#endif

#if 0
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

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "portgroup=%u, outerVlanId=%u, innerVlanId=%u",
              portgroup, outerVlanId, innerVlanId);

  /* Validate arguments */
  if (portgroup==0 || 
      outerVlanId>4095 || innerVlanId>4095)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments");
    return L7_FAILURE;
  }

  /* Define structure */
  xlate.portgroup     = portgroup;
  xlate.stage         = PTIN_XLATE_STAGE_EGRESS;
  xlate.outerVlan     = outerVlanId;
  xlate.innerVlan     = innerVlanId;
  xlate.outerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan_new = PTIN_XLATE_NOT_DEFINED;
  xlate.outerAction   = PTIN_XLATE_ACTION_NONE;
  xlate.innerAction   = PTIN_XLATE_ACTION_NONE;

  /* DTL call */
  rc = ptin_xlate_operation(DAPI_CMD_CLEAR, L7_ALL_INTERFACES, &xlate);

  /* If deletion went well... */
  if (rc == L7_SUCCESS)
  {
    rc = xlate_database_clear(L7_ALL_INTERFACES, &xlate);
  }

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: rc=%d", rc);

  return rc;
}
#endif

/**
 * Deletes all egress translation entries
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t ptin_xlate_egress_delete_all( void )
{
  ptin_vlanXlate_t xlate;
  L7_RC_t rc = L7_SUCCESS;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Beginning...");

  /* Define structure */
  xlate.portgroup       = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.stage           = PTIN_XLATE_STAGE_EGRESS;
  xlate.outerVlan       = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan       = PTIN_XLATE_NOT_DEFINED;
  xlate.outerVlan_new   = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan_new   = PTIN_XLATE_NOT_DEFINED;
  xlate.outerVlanAction = PTIN_XLATE_ACTION_NONE;
  xlate.innerVlanAction = PTIN_XLATE_ACTION_NONE;

  /* DTL call */
  rc = ptin_xlate_operation(DAPI_CMD_CLEAR_ALL, PTIN_PORT_ALL, &xlate);

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: rc=%d", rc);

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

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Beginning...");

  /* Define structure */
  xlate.portgroup       = PTIN_XLATE_PORTGROUP_INTERFACE;
  xlate.stage           = PTIN_XLATE_STAGE_ALL;
  xlate.outerVlan       = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan       = PTIN_XLATE_NOT_DEFINED;
  xlate.outerVlan_new   = PTIN_XLATE_NOT_DEFINED;
  xlate.innerVlan_new   = PTIN_XLATE_NOT_DEFINED;
  xlate.outerVlanAction = PTIN_XLATE_ACTION_NONE;
  xlate.innerVlanAction = PTIN_XLATE_ACTION_NONE;

  /* DTL call */
  rc = ptin_xlate_operation(DAPI_CMD_CLEAR_ALL, PTIN_PORT_ALL, &xlate);

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: rc=%d", rc);

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


/**
 * Configures PVID. This information is used on egress Action
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
L7_RC_t ptin_xlate_PVID_set(L7_uint32 ptin_port, L7_uint16 vlanId)
{
  L7_uint32 unit = 0;
  L7_uint32 intIfNum;
  L7_uint16 int_defVid, int_innerVid, pvid_original;
  L7_uint32 class_id;
  ptinXlateKey_t        avl_key;
  ptinXlateInfoData_t  *avl_infoData;
  ptin_vlanXlate_t      xlate;

  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, "Invalid ptin_port %d", ptin_port);
    return L7_FAILURE;
  }

  if (vlanId == 0 || vlanId >= 4096)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "Invalid VID %u", vlanId);
    return L7_FAILURE;
  }

  /* Get intIfNum */
  if (ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Invalid ptin_port %u", ptin_port);
    return L7_FAILURE;
  }

  /* Check if nothing should be done */
  if (xlate_table_pvid[ptin_port] == vlanId)
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "Nothing to be done for ptin_port %u: VID %u is the same", ptin_port, vlanId);
    return L7_SUCCESS;
  }

  /* Default internal VLAN */
  int_defVid = 1;

  /* New VID: translation and verification */
  if (ptin_xlate_ingress_get(ptin_port, vlanId, PTIN_XLATE_NOT_DEFINED, &int_defVid, &int_innerVid) != L7_SUCCESS)
  {
    int_defVid = 1;
    if (ptin_debug_xlate)
      PT_LOG_WARN(LOG_CTX_INTF, "Could not acquire internal vlan associated to ptin_port %u/vlan %u... Assuming 1",
                  ptin_port, vlanId);
  }
  else
  {
    PT_LOG_TRACE(LOG_CTX_INTF, "Converted VID %u to internal %u+%u (ptin_port %u)", vlanId, int_defVid, int_innerVid, ptin_port);
  }

  /* Configure defVID using a VCAP rule */
  if (int_innerVid >= 1 && int_innerVid <= 4095)
  {
    if (ptin_intf_vcap_defvid(ptin_port, int_defVid, int_innerVid) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_XLATE,"Error configuring VCAP defVID %u+%u for ptin_port %u", int_defVid, int_innerVid, ptin_port);
    }
    else
    {
      PT_LOG_TRACE(LOG_CTX_XLATE,"VCAP defVID %u+%u for ptin_port %u configured", int_defVid, int_innerVid, ptin_port);
    }
  }

  /* Set default VID */
  if (usmDbQportsPVIDSet(unit, intIfNum, int_defVid) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error applying VID %u", int_defVid);
    return L7_FAILURE;
  }

  /* Set the interface belonging to this VLAN as untagged */
  if (usmDbVlanTaggedSet(1, int_defVid, intIfNum, L7_DOT1Q_UNTAGGED) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_INTF, "Error setting ptin_port %u of VID %u as untagged", ptin_port, int_defVid);
    return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_XLATE, "PVID %u applied to ptin_port %u", vlanId, ptin_port);

  /* Get old PVID */
  pvid_original = xlate_table_pvid[ptin_port];
  int_defVid    = 1;

  /* If value is valid... */
  if (pvid_original >= 1 && pvid_original <= 4095)
  {
    /* ... get correspondent internal vlan... */
    if (ptin_xlate_ingress_get(ptin_port, pvid_original, PTIN_XLATE_NOT_DEFINED, &int_defVid, &int_innerVid) == L7_SUCCESS)
    {
      /* ... and restore this interface to tagged */
      if (usmDbVlanTaggedSet(1, int_defVid, intIfNum, L7_DOT1Q_TAGGED) == L7_SUCCESS)
      {
        PT_LOG_TRACE(LOG_CTX_XLATE, "Success restoring ptin_port %u of VLAN %u to tagged type", ptin_port, vlanId);
      }
      else
      {
        PT_LOG_WARN(LOG_CTX_INTF, "Error restoring ptin_port %u of VID %u to tagged type", ptin_port, int_defVid);
      }
    }
    else
    {
      int_defVid = 1;
      PT_LOG_WARN(LOG_CTX_INTF, "Could not acquire internal vlan associated to ptin_port %u/vlan %u... Assuming 1",
                  ptin_port, pvid_original);
    }
  }
  else
  {
    pvid_original = 0;
  }

  /* Get class id, correspondent to intIfNum */
  if (xlate_portgroup_from_intf(ptin_port, &class_id) == L7_SUCCESS)
  {
    /* Search for all EGRESS XLATE entries, with external outer vlan == PVID */
    memset(&avl_key, 0x00, sizeof(ptinXlateKey_t));
    while ((avl_infoData = avlSearchLVL7(&database_xlate[PTIN_XLATE_STAGE_EGRESS].avlTree, (void *)&avl_key, AVL_NEXT)) != L7_NULLPTR)
    {
      /* Prepare next key */
      memcpy(&avl_key, &avl_infoData->key, sizeof(ptinXlateKey_t));

      /* Skip not related ports */
      if (avl_infoData->key.portGroup != class_id)
      {
        continue;
      }

      /* Define structure for each item */
      memset(&xlate, 0x00, sizeof(ptin_vlanXlate_t));
      xlate.portgroup       = avl_infoData->key.portGroup;
      xlate.stage           = PTIN_XLATE_STAGE_EGRESS;
      xlate.outerVlan       = avl_infoData->key.outerVid;
      xlate.innerVlan       = avl_infoData->key.innerVid;

      xlate.outerPrio       = avl_infoData->outer_prio;
      xlate.innerPrio       = avl_infoData->inner_prio;

      xlate.outerVlan_new   = avl_infoData->outerVlan_result;
      xlate.innerVlan_new   = avl_infoData->innerVlan_result;
      xlate.outerVlanAction = avl_infoData->outerVlanAction;
      xlate.innerVlanAction = avl_infoData->innerVlanAction;

      xlate.outerPrio_new   = avl_infoData->outerPrio_result;
      xlate.innerPrio_new   = avl_infoData->innerPrio_result;
      xlate.outerPrioAction = avl_infoData->outerPrioAction;
      xlate.innerPrioAction = avl_infoData->innerPrioAction;

      /* Entries where VLANs should be restored */
      if (avl_infoData->outerVlan_result == pvid_original)
      {
        /* Reposition vlans */
        xlate.remove_VLANs  = L7_FALSE;
      }
      /* Entries where VLANs should be removed */
      else if (avl_infoData->outerVlan_result == vlanId)
      {
        /* Remove vlans */
        xlate.remove_VLANs  = L7_TRUE;
      }
      /* Nothing to be done */
      else
      {
        continue;
      }

      /* Apply operation */
      if (ptin_xlate_operation(DAPI_CMD_SET, PTIN_PORT_ALL, &xlate) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_XLATE, "Error redefining egress xlate entry for pgroup=%u, OVid=%u, IVid=%u (remove_VLANs=%u)",
                xlate.portgroup, xlate.outerVlan, xlate.innerVlan, xlate.remove_VLANs);
      }
      else
      {
        PT_LOG_TRACE(LOG_CTX_XLATE, "Egress xlate entry for pgroup=%u, OVid=%u, IVid=%u redefined (remove_VLANs=%u)",
                  xlate.portgroup, xlate.outerVlan, xlate.innerVlan, xlate.remove_VLANs);
      }
    }

    PT_LOG_TRACE(LOG_CTX_XLATE, "Egress translations redefined");
  }

  /* Save new PVID */
  xlate_table_pvid[ptin_port] = vlanId;

  return L7_SUCCESS;
}

/**
 * Get PVID
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
L7_RC_t ptin_xlate_PVID_get(L7_uint32 ptin_port, L7_uint16 *vlanId)
{
  /* Validate arguments */
  if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, "Invalid ptin_port %d", ptin_port);
    return L7_FAILURE;
  }

  *vlanId = xlate_table_pvid[ptin_port];
  return L7_SUCCESS;
}

/**
 * Dump PVID and Portgroup information
 * 
 * @author mruas (1/13/2015)
 */
void ptin_pvid_dump(void)
{
  L7_uint port;

  printf("PortGroup and PVID information:\r\n");

  for (port = 0; port < PTIN_SYSTEM_N_INTERF; port++)
  {
    printf("Port %-2u:   PGroup=", port);

    (port < PTIN_SYSTEM_N_PORTS) ? printf("%-3u", xlate_table_portgroup[port]) : printf("---");

    printf("   PVID=%u\r\n", xlate_table_pvid[port]);
  }
}

/***************************************************************** 
 * STATIC FUNCTIONS IMPLEMENTATION
 *****************************************************************/

/**
 * Init PVID
 * 
 * @return L7_RC_t : L7_SUCCESS
 */
static L7_RC_t ptin_xlate_PVID_init(void)
{
  L7_uint32 ptin_port;

  for (ptin_port = 0; ptin_port < PTIN_SYSTEM_N_INTERF; ptin_port++)
  {
    xlate_table_pvid[ptin_port] = 1;
  }
  return L7_SUCCESS;
}

/**
 * Apply xlate operation
 *  
 * @param operation : operation (DAPI_CMD_GET / DAPI_CMD_SET / 
 *                  DAPI_CMD_CLEAR / DAPI_CMD_CLEAR_ALL)
 * @param ptin_port : interface reference
 * @param vlanXlate_data : VLAN translation info
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t ptin_xlate_operation(L7_int operation, L7_uint32 ptin_port, ptin_vlanXlate_t *xlate)
{
  L7_uint32 intIfNum;
  L7_RC_t rc = L7_SUCCESS;

  if (xlate == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if (ptin_port == PTIN_PORT_ALL)
  {
    intIfNum = L7_ALL_INTERFACES;
  }
  else if (ptin_port >= PTIN_SYSTEM_N_INTERF)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, "ptin_port %u is out of valid range", ptin_port);
    return L7_FAILURE;
  }
  else if (ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, "Can't convert ptin_port %u to intIfNum", ptin_port);
    return L7_FAILURE;
  }
  
  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "oper=%d: ptin_port=%u/intIfNum=%u/pgroup=%u, Vid=%u+%u, newVid=%u.%u+%u.%u (action %u.%u+%u.%u), rem_VLANs=%u",
              operation, ptin_port, intIfNum, xlate->portgroup,
              xlate->outerVlan, xlate->innerVlan,
              xlate->outerVlan_new  , xlate->outerPrio_new,
              xlate->innerVlan_new  , xlate->innerPrio_new,
              xlate->outerVlanAction, xlate->outerPrioAction,
              xlate->innerVlanAction, xlate->innerPrioAction,
              xlate->remove_VLANs);

  if (xlate->stage >= PTIN_XLATE_STAGE_ALL)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, "Invalid stage %u", xlate->stage);
    return L7_FAILURE;
  }

  /* Check if there is enough room for new rule */
  if (operation == DAPI_CMD_SET)
  {
    if (database_xlate[xlate->stage].number_of_entries >= PTIN_XLATE_MAX_SIZE)
    {
      PT_LOG_ERR(LOG_CTX_XLATE, "No more translation entries available!");
      return L7_NO_RESOURCES;
    }
  }
  
  /* Set operation */
  xlate->oper = operation;

  /* DTL call */
  rc = dtlPtinVlanTranslate(intIfNum, xlate);

  /* If addition went well... */
  if (rc == L7_SUCCESS)
  {
    if (ptin_debug_xlate)
      PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port %u: Success configuring entry", ptin_port);

    switch (operation)
    {
      case DAPI_CMD_SET:
        rc = xlate_database_store(ptin_port, xlate);
        break;
      case DAPI_CMD_CLEAR:
        rc = xlate_database_clear(ptin_port, xlate);
        break;
      case DAPI_CMD_CLEAR_ALL:
        rc = xlate_database_clear_all(xlate->stage);
        break;
      default:
        PT_LOG_ERR(LOG_CTX_XLATE, "ptin_port %u: Unknown operation %d", ptin_port, operation);
    }
  }
  else if (rc == L7_ALREADY_CONFIGURED)
  {
    PT_LOG_WARN(LOG_CTX_XLATE, "ptin_port %u: Entry already configured", ptin_port);
    rc = L7_SUCCESS;
  }
  else
  {
    PT_LOG_ERR(LOG_CTX_XLATE, "ptin_port %u: Error configuring entry (rc=%d)", ptin_port, rc);
  }
  
  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "Finished: rc=%d", rc);

  return rc;
}

/**
 * Initialize XLATE AVL trees
 * 
 * @return L7_RC_t : L7_SUCCESS/L7_FAILURE
 */
static L7_RC_t xlate_database_init(void)
{
  L7_int stage;

  /* Reset xlate AVL tree */
  memset(database_xlate, 0x00, sizeof(database_xlate));

  /* Translation table */
  for (stage=PTIN_XLATE_STAGE_INGRESS; stage<=PTIN_XLATE_STAGE_EGRESS; stage++)
  {
    /* Reset xlate AVL tree */
    memset(&database_xlate[stage], 0x00, sizeof(ptinXlateAvlTree_t));

    database_xlate[stage].treeHeap = (avlTreeTables_t *) osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_XLATE_MAX_SIZE * sizeof(avlTreeTables_t)); 
    database_xlate[stage].dataHeap = (ptinXlateInfoData_t *) osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_XLATE_MAX_SIZE * sizeof(ptinXlateInfoData_t)); 

    if ((database_xlate[stage].treeHeap == L7_NULLPTR) ||
        (database_xlate[stage].dataHeap == L7_NULLPTR))
    {
      PT_LOG_ERR(LOG_CTX_IGMP,"Error allocating data for XLATE AVL Trees");
      return L7_FAILURE;
    }

    /* Initialize the storage for all the AVL trees */
    memset (&database_xlate[stage].avlTree, 0x00, sizeof(avlTree_t));
    memset ( database_xlate[stage].treeHeap, 0x00, PTIN_XLATE_MAX_SIZE * sizeof(avlTreeTables_t));
    memset ( database_xlate[stage].dataHeap, 0x00, PTIN_XLATE_MAX_SIZE * sizeof(ptinXlateInfoData_t));

    // AVL Tree creations - snoopIpAvlTree
    avlCreateAvlTree(&database_xlate[stage].avlTree,
                     database_xlate[stage].treeHeap,
                     database_xlate[stage].dataHeap,
                     PTIN_XLATE_MAX_SIZE, 
                     sizeof(ptinXlateInfoData_t),
                     0x10,
                     sizeof(ptinXlateKey_t));

    /* No entries at the beginning */
    database_xlate[stage].number_of_entries = 0;
  }

  /* Inverted Translation table */
  for (stage=PTIN_XLATE_STAGE_INGRESS; stage<=PTIN_XLATE_STAGE_EGRESS; stage++)
  {
    /* Reset xlate AVL tree */
    memset(&database_xlate_inv[stage], 0x00, sizeof(ptinXlateAvlTree_t));

    database_xlate_inv[stage].treeHeap = (avlTreeTables_t *) osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_XLATE_MAX_SIZE * sizeof(avlTreeTables_t)); 
    database_xlate_inv[stage].dataHeap = (ptinXlateInfoData_t *) osapiMalloc(L7_PTIN_COMPONENT_ID, PTIN_XLATE_MAX_SIZE * sizeof(ptinXlateInfoData_t)); 

    if ((database_xlate_inv[stage].treeHeap == L7_NULLPTR) ||
        (database_xlate_inv[stage].dataHeap == L7_NULLPTR))
    {
      PT_LOG_ERR(LOG_CTX_IGMP,"Error allocating data for Inverted XLATE AVL Trees\n");
      return L7_FAILURE;
    }

    /* Initialize the storage for all the AVL trees */
    memset (&database_xlate_inv[stage].avlTree, 0x00, sizeof(avlTree_t));
    memset ( database_xlate_inv[stage].treeHeap, 0x00, PTIN_XLATE_MAX_SIZE * sizeof(avlTreeTables_t));
    memset ( database_xlate_inv[stage].dataHeap, 0x00, PTIN_XLATE_MAX_SIZE * sizeof(ptinXlateInfoData_t));

    // AVL Tree creations - snoopIpAvlTree
    avlCreateAvlTree(&database_xlate_inv[stage].avlTree,
                     database_xlate_inv[stage].treeHeap,
                     database_xlate_inv[stage].dataHeap,
                     PTIN_XLATE_MAX_SIZE, 
                     sizeof(ptinXlateInfoData_t),
                     0x10,
                     sizeof(ptinXlateKey_t));

    /* No entries at the beginning */
    database_xlate_inv[stage].number_of_entries = 0;
  }

  return L7_SUCCESS;
}

/**
 * Get new VlanId from local database
 * 
 * @param ptin_port : Interface reference
 * @param vlanXlate_data : VLAN translation info
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_newVlan_get(L7_uint32 ptin_port, ptin_vlanXlate_t *vlanXlate_data)
{
  L7_int stage;
  ptinXlateKey_t        avl_key;
  ptinXlateInfoData_t  *avl_infoData;

  /* Validate arguments */
  if ( ( vlanXlate_data->stage != PTIN_XLATE_STAGE_INGRESS && vlanXlate_data->stage != PTIN_XLATE_STAGE_EGRESS ) ||
       ( ptin_port != PTIN_PORT_ALL && ptin_port >= PTIN_SYSTEM_N_INTERF ) ||
       ( vlanXlate_data->outerVlan > 4095) )
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments (stage=%d, ptin_port=%u, oVlan=%u)",
               vlanXlate_data->stage, ptin_port, vlanXlate_data->outerVlan);
    return L7_FAILURE;
  }

  stage = vlanXlate_data->stage;

  /* Prepare key */
  memset(&avl_key, 0x00, sizeof(ptinXlateKey_t));
  avl_key.ptin_port = ptin_port;
  avl_key.portGroup = vlanXlate_data->portgroup;
  avl_key.outerVid  = vlanXlate_data->outerVlan;
  avl_key.innerVid  = vlanXlate_data->innerVlan;

  /* Search for entry, to get new vlans  */
  avl_infoData = avlSearchLVL7(&database_xlate[stage].avlTree, (void *)&avl_key, AVL_EXACT);

  /* Failed? */
  if ((ptinXlateKey_t *) avl_infoData == L7_NULLPTR)
  {
    if (ptin_debug_xlate)
      PT_LOG_ERR(LOG_CTX_XLATE, " Entry does not exist"); 
    return L7_FAILURE;
  }

  /* Return value */
  if (vlanXlate_data != L7_NULLPTR)
  {
    vlanXlate_data->outerPrio       = avl_infoData->outer_prio;
    vlanXlate_data->innerPrio       = avl_infoData->inner_prio;

    vlanXlate_data->outerVlan_new   = avl_infoData->outerVlan_result;
    vlanXlate_data->innerVlan_new   = avl_infoData->innerVlan_result;
    vlanXlate_data->outerVlanAction = avl_infoData->outerVlanAction;
    vlanXlate_data->innerVlanAction = avl_infoData->innerVlanAction;

    vlanXlate_data->outerPrio_new   = avl_infoData->outerPrio_result;
    vlanXlate_data->innerPrio_new   = avl_infoData->innerPrio_result;
    vlanXlate_data->outerPrioAction = avl_infoData->outerPrioAction;
    vlanXlate_data->innerPrioAction = avl_infoData->innerPrioAction;

    vlanXlate_data->remove_VLANs  = avl_infoData->remove_VLANs;
  }

  /* Success */
  return L7_SUCCESS;
}

/**
 * Get original VlanId from local database
 * 
 * @param ptin_port : Interface reference
 * @param vlanXlate_data : VLAN translation info
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_oldVlan_get(L7_uint32 ptin_port, ptin_vlanXlate_t *vlanXlate_data)
{
  L7_int stage;
  ptinXlateKey_t        avl_key;
  ptinXlateInfoData_t  *avl_infoData;

  /* Validate arguments */
  if ( ( vlanXlate_data->stage != PTIN_XLATE_STAGE_INGRESS && vlanXlate_data->stage != PTIN_XLATE_STAGE_EGRESS ) ||
       ( ptin_port != PTIN_PORT_ALL && ptin_port >= PTIN_SYSTEM_N_INTERF ) ||
       ( vlanXlate_data->outerVlan_new > 4095) )
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments (stage=%d, ptin_port=%u, newOVlan=%u)",
               vlanXlate_data->stage, ptin_port, vlanXlate_data->outerVlan_new);
    return L7_FAILURE;
  }

  stage = vlanXlate_data->stage;

  /* Prepare key */
  memset(&avl_key, 0x00, sizeof(ptinXlateKey_t));
  avl_key.ptin_port = ptin_port;
  avl_key.portGroup = vlanXlate_data->portgroup;
  avl_key.outerVid  = vlanXlate_data->outerVlan_new;
  avl_key.innerVid  = vlanXlate_data->innerVlan_new;

  /* Search for entry, to get new vlans  */
  avl_infoData = avlSearchLVL7(&database_xlate_inv[stage].avlTree, (void *)&avl_key, AVL_EXACT);

  /* Failed? */
  if ((ptinXlateKey_t *) avl_infoData == L7_NULLPTR)
  {
    if (ptin_debug_xlate)
      PT_LOG_ERR(LOG_CTX_XLATE, " Entry does not exist");
    return L7_FAILURE;
  }

  /* Return value */
  if (vlanXlate_data != L7_NULLPTR)
  {
    vlanXlate_data->outerPrio_new   = avl_infoData->outer_prio;
    vlanXlate_data->innerPrio_new   = avl_infoData->inner_prio;

    vlanXlate_data->outerVlan       = avl_infoData->outerVlan_result;
    vlanXlate_data->innerVlan       = avl_infoData->innerVlan_result;
    vlanXlate_data->outerVlanAction = avl_infoData->outerVlanAction;
    vlanXlate_data->innerVlanAction = avl_infoData->innerVlanAction;

    vlanXlate_data->outerPrio       = avl_infoData->outerPrio_result;
    vlanXlate_data->innerPrio       = avl_infoData->innerPrio_result;
    vlanXlate_data->outerPrioAction = avl_infoData->outerPrioAction;
    vlanXlate_data->innerPrioAction = avl_infoData->innerPrioAction;

    vlanXlate_data->remove_VLANs = avl_infoData->remove_VLANs;
  }

  /* Success */
  return L7_SUCCESS;
}

/**
 * Save translation entry in local database
 * 
 * @param ptin_port : Interface reference
 * @param vlanXlate_data : VLAN translation info
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_store(L7_uint32 ptin_port, const ptin_vlanXlate_t *vlanXlate_data)
{
  L7_int stage;
  ptinXlateKey_t        avl_key, avl_key_inv;
  ptinXlateInfoData_t  *avl_infoData;

  /* Validate arguments */
  if ( ( vlanXlate_data->stage != PTIN_XLATE_STAGE_INGRESS && vlanXlate_data->stage != PTIN_XLATE_STAGE_EGRESS ) ||
       ( ptin_port != PTIN_PORT_ALL && ptin_port >= PTIN_SYSTEM_N_INTERF ) || 
       ( vlanXlate_data->outerVlan > 4095) )
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments (stage=%d, ptin_port=%u, oVlan=%u.%u, newOVlan=%u.%u)",
            vlanXlate_data->stage, ptin_port,
            vlanXlate_data->outerVlan, vlanXlate_data->outerPrio,
            vlanXlate_data->outerVlan_new, vlanXlate_data->outerPrio_new);
    return L7_FAILURE;
  }

  if (ptin_debug_xlate)
  {
    PT_LOG_TRACE(LOG_CTX_XLATE, "Storing new entry: ptin_port=%u portGroup=%u outerVid=%u innerVid=%u",
                 ptin_port, vlanXlate_data->portgroup, vlanXlate_data->outerVlan, vlanXlate_data->innerVlan);
  }

  stage = vlanXlate_data->stage;

  /* Check if there is space in AVL tree */
  if (database_xlate[stage].number_of_entries >= PTIN_XLATE_MAX_SIZE ||
      database_xlate_inv[stage].number_of_entries >= PTIN_XLATE_MAX_SIZE)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: XLATE AVL tree is full (stage=%u, %u/%u entries)",
            stage, database_xlate[stage].number_of_entries, database_xlate_inv[stage].number_of_entries);
    return L7_FAILURE;
  }

  /* Prepare key */
  memset(&avl_key, 0x00, sizeof(ptinXlateKey_t));
  avl_key.ptin_port = ptin_port;
  avl_key.portGroup = vlanXlate_data->portgroup;
  avl_key.outerVid  = vlanXlate_data->outerVlan;
  avl_key.innerVid  = vlanXlate_data->innerVlan;

  /* Create AVL node */
  avl_infoData = avlInsertEntry(&database_xlate[stage].avlTree, (void *)&avl_key);

  /* Failed? */
  if ((ptinXlateKey_t *) avl_infoData == &avl_key)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR inserting new node");
    return L7_FAILURE;
  }

  /* We have a new item */
  if (avl_infoData == L7_NULLPTR)
  {
    /* Find new item */
    avl_infoData = (ptinXlateInfoData_t *) avlSearchLVL7(&database_xlate[stage].avlTree, (void *)&avl_key, AVL_EXACT);

    /* Check if item was successfully obtained */
    if (avl_infoData == L7_NULLPTR)
    {
      PT_LOG_ERR(LOG_CTX_XLATE, " Strange... item looks to not be created");
      return L7_FAILURE;
    }
    /* One more entry */
    database_xlate[stage].number_of_entries++;
  }
  else if ((void *) avl_infoData == (void *) &avl_key)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, "Error adding new entry");
    return L7_FAILURE;
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_XLATE, "Duplicate entry");
  }

  /* Fill information */
  avl_infoData->outer_prio        = vlanXlate_data->outerPrio;
  avl_infoData->inner_prio        = vlanXlate_data->innerPrio;

  avl_infoData->outerVlan_result  = vlanXlate_data->outerVlan_new;
  avl_infoData->innerVlan_result  = vlanXlate_data->innerVlan_new;
  avl_infoData->outerVlanAction   = vlanXlate_data->outerVlanAction;
  avl_infoData->innerVlanAction   = vlanXlate_data->innerVlanAction;

  avl_infoData->outerPrio_result  = vlanXlate_data->outerPrio_new;
  avl_infoData->innerPrio_result  = vlanXlate_data->innerPrio_new;
  avl_infoData->outerPrioAction   = vlanXlate_data->outerPrioAction;
  avl_infoData->innerPrioAction   = vlanXlate_data->innerPrioAction;

  avl_infoData->remove_VLANs = vlanXlate_data->remove_VLANs;

  /* Original AVL tree */

  /* Prepare key */
  memset(&avl_key_inv, 0x00, sizeof(ptinXlateKey_t));
  avl_key_inv.ptin_port = ptin_port;
  avl_key_inv.portGroup = vlanXlate_data->portgroup;
  avl_key_inv.outerVid  = vlanXlate_data->outerVlan_new;
  avl_key_inv.innerVid  = vlanXlate_data->innerVlan_new;

  /* Create AVL node */
  avl_infoData = avlInsertEntry(&database_xlate_inv[stage].avlTree, (void *)&avl_key_inv);

  /* Failed? */
  if ((ptinXlateKey_t *) avl_infoData == &avl_key_inv)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR inserting new node");
    avlDeleteEntry(&database_xlate[stage].avlTree, (void *)&avl_key);
    return L7_FAILURE;
  }

  /* We have a new item */
  if (avl_infoData == L7_NULLPTR)
  {
    /* Find new item */
    avl_infoData = (ptinXlateInfoData_t *) avlSearchLVL7(&database_xlate_inv[stage].avlTree, (void *)&avl_key_inv, AVL_EXACT);

    /* Check if item was successfully obtained */
    if (avl_infoData == L7_NULLPTR)
    {
      PT_LOG_ERR(LOG_CTX_XLATE, " Strange... item looks to not be created (inv)");
      avlDeleteEntry(&database_xlate[stage].avlTree, (void *)&avl_key);
      return L7_FAILURE;
    }
    /* One more entry */
    database_xlate_inv[stage].number_of_entries++;
  }
  else if ((void *) avl_infoData == (void *) &avl_key_inv)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, "Error adding new entry (inv)");
    avlDeleteEntry(&database_xlate[stage].avlTree, (void *)&avl_key);
    return L7_FAILURE;
  }
  else
  {
    PT_LOG_WARN(LOG_CTX_XLATE, "Duplicate entry (inv)");
  }

  /* Fill information */
  avl_infoData->outer_prio        = vlanXlate_data->outerPrio_new;
  avl_infoData->inner_prio        = vlanXlate_data->innerPrio_new;

  avl_infoData->outerVlan_result  = vlanXlate_data->outerVlan;
  avl_infoData->innerVlan_result  = vlanXlate_data->innerVlan;
  avl_infoData->outerVlanAction   = vlanXlate_data->outerVlanAction;
  avl_infoData->innerVlanAction   = vlanXlate_data->innerVlanAction;

  avl_infoData->outerPrio_result  = vlanXlate_data->outerPrio;
  avl_infoData->innerPrio_result  = vlanXlate_data->innerPrio;
  avl_infoData->outerPrioAction   = vlanXlate_data->outerPrioAction;
  avl_infoData->innerPrioAction   = vlanXlate_data->innerPrioAction;

  avl_infoData->remove_VLANs = vlanXlate_data->remove_VLANs;

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port %u: Entry stored", ptin_port);

  /* Success */
  return L7_SUCCESS;
}

/**
 * Clear translation entry in local database
 * 
 * @param ptin_port : Interface reference
 * @param vlanXlate_data : VLAN translation info
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_database_clear(L7_uint32 ptin_port, const ptin_vlanXlate_t *vlanXlate_data)
{
  L7_int stage;
  ptinXlateKey_t        avl_key, avl_key_inv;
  ptinXlateInfoData_t  *avl_infoData;

  /* Validate arguments */
  if ( vlanXlate_data == L7_NULLPTR )
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " vlanXlate_data:%p", vlanXlate_data);
    return L7_FAILURE;
  }

  if ( ( vlanXlate_data->stage != PTIN_XLATE_STAGE_INGRESS && vlanXlate_data->stage != PTIN_XLATE_STAGE_EGRESS ) ||
       ( ptin_port != PTIN_PORT_ALL && ptin_port >= PTIN_SYSTEM_N_INTERF ) ||
       ( vlanXlate_data->outerVlan > 4095) )
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments (stage=%d, ptin_port=%u, oVlan=%u)", vlanXlate_data->stage, ptin_port, vlanXlate_data->outerVlan);
    return L7_FAILURE;
  }

  if (ptin_debug_xlate)
  {
    PT_LOG_TRACE(LOG_CTX_XLATE, "Clearing entry: ptin_port=%u portGroup=%u outerVid=%u innerVid=%u",
                 ptin_port, vlanXlate_data->portgroup, vlanXlate_data->outerVlan, vlanXlate_data->innerVlan);
  }

  stage = vlanXlate_data->stage;

  /* Prepare key */
  memset(&avl_key, 0x00, sizeof(ptinXlateKey_t));
  avl_key.ptin_port = ptin_port;
  avl_key.portGroup = vlanXlate_data->portgroup;
  avl_key.outerVid  = vlanXlate_data->outerVlan;
  avl_key.innerVid  = vlanXlate_data->innerVlan;

  /* Search for entry, to get new vlans  */
  avl_infoData = avlSearchLVL7(&database_xlate[stage].avlTree, (void *)&avl_key, AVL_EXACT);

  /* Failed? */
  if (avl_infoData == L7_NULLPTR)
  {
    PT_LOG_WARN(LOG_CTX_XLATE, " Entry does not exist (ptin_port=%u, pgroup=%u, ovlan=%u, ivlan=%u)",
                avl_key.ptin_port, avl_key.portGroup, avl_key.outerVid, avl_key.innerVid);
    return L7_SUCCESS;
  }

  PT_LOG_TRACE(LOG_CTX_XLATE, " Found entry: ptin_port=%u, pgroup=%u, ovlan=%u, ivlan=%u -> oVlan=%u.%u (action %u.%u), iVlan=%u.%u (action %u.%u)",
            avl_key.ptin_port, avl_key.portGroup, avl_key.outerVid, avl_key.innerVid,
            avl_infoData->outerVlan_result, avl_infoData->outerPrio_result,
            avl_infoData->outerVlanAction , avl_infoData->outerPrioAction,
            avl_infoData->innerVlan_result, avl_infoData->innerPrio_result,
            avl_infoData->innerVlanAction , avl_infoData->innerPrioAction);

  /* Prepare original key */
  memset(&avl_key_inv, 0x00, sizeof(ptinXlateKey_t));
  avl_key_inv.ptin_port = ptin_port;
  avl_key_inv.portGroup = vlanXlate_data->portgroup;
  avl_key_inv.outerVid  = avl_infoData->outerVlan_result;
  avl_key_inv.innerVid  = avl_infoData->innerVlan_result;

  /* Delete inverted vlans entry */
  /* If several entries have the same result VLANs, we may have error here... ignore failure */
  avl_infoData = avlDeleteEntry(&database_xlate_inv[stage].avlTree, (void *)&avl_key_inv);

  if (avl_infoData == L7_NULLPTR)
  {
    PT_LOG_WARN(LOG_CTX_XLATE, " Failed Inverted Entry remotion (ptin_port=%u, pgroup=%u, ovlan=%u, ivlan=%u)",
                avl_key_inv.ptin_port, avl_key_inv.portGroup, avl_key_inv.outerVid, avl_key_inv.innerVid);
    //return L7_FAILURE;
  }
  else
  {
    if (database_xlate_inv[stage].number_of_entries > 0)
      database_xlate_inv[stage].number_of_entries--;
  }

  /* Delete entry */
  avl_infoData = avlDeleteEntry(&database_xlate[stage].avlTree, (void *)&avl_key);

  if (avl_infoData == L7_NULLPTR)
  {
    PT_LOG_WARN(LOG_CTX_XLATE, " Failed Inverted Entry remotion (ptin_port=%u, pgroup=%u, ovlan=%u, ivlan=%u)",
                avl_key.ptin_port, avl_key.portGroup, avl_key.outerVid, avl_key.innerVid);
    //return L7_FAILURE;
  }
  else
  {
    if (database_xlate[stage].number_of_entries > 0)
      database_xlate[stage].number_of_entries--;
  }

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "ptin_port %u: Entry cleared", ptin_port);

  /* Success */
  return L7_SUCCESS;
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
  L7_int i;

  /* Validate arguments */
  if ( stage!=PTIN_XLATE_STAGE_ALL &&
       stage!=PTIN_XLATE_STAGE_INGRESS &&
       stage!=PTIN_XLATE_STAGE_EGRESS )
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR: Invalid arguments (stage=%d)", stage);
    return L7_FAILURE;
  }

  if (ptin_debug_xlate)
  {
    PT_LOG_TRACE(LOG_CTX_XLATE, "Clearing all entries");
  }

  for (i=PTIN_XLATE_STAGE_INGRESS; i<=PTIN_XLATE_STAGE_EGRESS; i++)
  {
    if (stage == PTIN_XLATE_STAGE_ALL || stage == i)
    {
      /* Purge AVL trees */
      avlPurgeAvlTree(&database_xlate[stage].avlTree, PTIN_XLATE_MAX_SIZE);
      database_xlate[stage].number_of_entries = 0;

      avlPurgeAvlTree(&database_xlate_inv[stage].avlTree, PTIN_XLATE_MAX_SIZE);
      database_xlate_inv[stage].number_of_entries = 0;
    }
  }

  if (ptin_debug_xlate)
    PT_LOG_TRACE(LOG_CTX_XLATE, "All entries cleared");

  return L7_SUCCESS;
}

/**
 * Calculate portGroup id, from interface
 * 
 * @param ptin_port : interface reference
 * @param portgroup : portgroup (to be returned)
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
static L7_RC_t xlate_portgroup_from_intf(L7_uint32 ptin_port, L7_uint32 *portgroup)
{
  nimUSP_t usp;
  L7_uint32 class_id;
  L7_uint32 intIfNum;

  /* Obtain intIfNum */
  if (ptin_intf_port2intIfNum(ptin_port, &intIfNum) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, "Error converting ptin_port %u to intIfNum", ptin_port);
    return L7_FAILURE;
  }
  
  /* Calculate USP */
  if (nimGetUnitSlotPort(intIfNum, &usp)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " Error getting USP (intIfNum=%u, ptin_port=%u)", intIfNum, ptin_port);
    return L7_FAILURE;
  }

  /* If interface is not physical neither LAG, error */
  if ( usp.slot>=L7_MAX_PHYSICAL_SLOTS_PER_UNIT && usp.slot!=L7_LAG_SLOT_NUM )
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " Invalid interface type (usp={%d, %d, %d})", usp.unit, usp.slot, usp.port);
    return L7_FAILURE;
  }

  /* Calculate port group */
  class_id = (usp.slot*L7_MAX_PORTS_PER_SLOT) + usp.port;

  /* Validate class id */
  if ( class_id == 0 )
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " Class id is null");
    return L7_FAILURE;
  }

  /* Return class id */
  if (portgroup!=L7_NULLPTR)
  {
    *portgroup = class_id;
  }

  return L7_SUCCESS;
}

/**
 * Get all the outer vlan from port 
 * 
 * @param stage : PTIN_XLATE_STAGE_ALL, PTIN_XLATE_STAGE_INGRESS
 *             or PTIN_XLATE_STAGE_EGRESS
 * 
 * @return L7_RC_t : L7_SUCCESS or L7_FAILURE
 */
L7_RC_t xlate_outer_vlan_replicate_Dstport(L7_uint32 operation, L7_uint32 ptin_port_src , L7_uint32 ptin_port_dst)
{
  ptinXlateKey_t       avl_key;
  ptinXlateAvlTree_t   *avl_tree;
  ptinXlateInfoData_t  *avl_infoData;
  L7_uint32 class_id_src, class_id_dst ;
  ptin_HwEthMef10Intf_t intf_vlan_dst;
  L7_uint32 rc = L7_SUCCESS;         

  /* Select AVL tree */
  avl_tree = &database_xlate[PTIN_XLATE_STAGE_EGRESS];

  /* Get class id */
  if (xlate_portgroup_from_intf(ptin_port_src, &class_id_src)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR getting class id");
    return L7_FAILURE;
  }

  memset(&intf_vlan_dst, 0x00, sizeof(intf_vlan_dst));
  intf_vlan_dst.intf.format = PTIN_INTF_FORMAT_PORT;
  intf_vlan_dst.intf.value.ptin_port = ptin_port_dst;
  if (ptin_intf_any_format(&intf_vlan_dst.intf) != L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, "Error expanding dst_port %u", ptin_port_dst);
    return L7_FAILURE;
  }

  /* Get class id */
  if (xlate_portgroup_from_intf(intf_vlan_dst.intf.value.intIfNum, &class_id_dst)!=L7_SUCCESS)
  {
    PT_LOG_ERR(LOG_CTX_XLATE, " ERROR getting class id");
    return L7_FAILURE;
  }

  // Seach for all members
  /* Initialize AVL key */
  memset(&avl_key, 0x00, sizeof(ptinXlateKey_t));

  /* Run all items */
  while ((avl_infoData=(ptinXlateInfoData_t *)avlSearchLVL7(&avl_tree->avlTree, (void *)&avl_key, AVL_NEXT)) != L7_NULLPTR)
  {  
    PT_LOG_TRACE(LOG_CTX_XLATE, " avl_infoData->key.portGroup = %d",avl_infoData->key.portGroup);
    PT_LOG_TRACE(LOG_CTX_XLATE, " class_id_src = %d",class_id_src);
    PT_LOG_TRACE(LOG_CTX_XLATE, " operation = %d",operation);

    intf_vlan_dst.vid           = avl_infoData->key.outerVid;
    intf_vlan_dst.vid_inner     = avl_infoData->key.innerVid;
    intf_vlan_dst.action_outer  = avl_infoData->outerVlanAction;
    intf_vlan_dst.action_inner  = avl_infoData->innerVlanAction;

    /* Prepare next key */
    if( avl_infoData->key.portGroup == class_id_src ) /* Check if exist a portGroup that match the class_id_src */
    {
      if(operation == 1) /* Add egress xlate to the dst mirror port */
      {
        if(avl_infoData->mirror == L7_TRUE) /* Avoid loop's' with ptin_xlate_egress_add function  */
        {
          PT_LOG_TRACE(LOG_CTX_XLATE, " Already configured");
          return rc;
        }
        avl_infoData->mirror = L7_TRUE;

        PT_LOG_TRACE(LOG_CTX_XLATE, " Xlate add to %d , outer_vid %d , innerVid %d , new_outer_vid %d , new_inner_vid %d",
                     intf_vlan_dst.intf.value.intIfNum, avl_infoData->key.outerVid, avl_infoData->key.innerVid,
                     avl_infoData->outerVlan_result, avl_infoData->innerVlan_result );

        rc = ptin_xlate_egress_add(&intf_vlan_dst,
                                   avl_infoData->outerVlan_result, avl_infoData->innerVlan_result,
                                   (L7_int) avl_infoData->outerPrio_result, (L7_int) avl_infoData->innerPrio_result);

        if (rc != L7_SUCCESS)
        {
          return rc;
        }
      }
      else /* Remove egress xlate to the dst mirror port */
      {
        if(avl_infoData->mirror == L7_TRUE) 
        {
          PT_LOG_TRACE(LOG_CTX_XLATE, "intfNum_dst %d", intf_vlan_dst.intf.value.intIfNum);
          rc = ptin_xlate_egress_delete(intf_vlan_dst.intf.value.ptin_port, intf_vlan_dst.vid, intf_vlan_dst.vid_inner);
          avl_infoData->mirror = L7_FALSE;
        }
        if(rc != L7_SUCCESS)
        {
          return rc;
        }
      }
    }
    memcpy(&avl_key, &avl_infoData->key, sizeof(ptinXlateKey_t));
  } 

  return L7_SUCCESS;
}



/***************************************************************** 
 * DEVSHELL FUNCTIONS
 *****************************************************************/

void ptin_xlate_dump(ptin_vlanXlate_stage_enum stage, L7_BOOL inv)
{
  ptinXlateKey_t        avl_key;
  ptinXlateInfoData_t  *avl_infoData;
  ptinXlateAvlTree_t   *avl_tree;

  if (stage != PTIN_XLATE_STAGE_INGRESS && stage != PTIN_XLATE_STAGE_EGRESS)
  {
    printf("Invalid stage. Only allowed 1 (Ingress) or 2 (egress)\r\n");
    return;
  }

  printf("Dumping %s%s XLATE table:\r\n",
         (inv) ? "inverted " : "",
         (stage==PTIN_XLATE_STAGE_INGRESS) ? "INGRESS" : "EGRESS");

  /* Select AVL tree */
  avl_tree = (inv) ? &database_xlate_inv[stage] : &database_xlate[stage];

  /* Initialize AVL key */
  memset(&avl_key, 0x00, sizeof(ptinXlateKey_t));

  /* Run all items */
  while ((avl_infoData=(ptinXlateInfoData_t *)avlSearchLVL7(&avl_tree->avlTree, (void *)&avl_key, AVL_NEXT)) != L7_NULLPTR)
  {
    printf(" port=%-2d / group=%-2d: VLANs %4u+%-4u -> %4u.%u+%-4u.%u [Action %d.%d + %d.%d] %s\r\n",
            (signed int) avl_infoData->key.ptin_port, (signed int) avl_infoData->key.portGroup,
            avl_infoData->key.outerVid, avl_infoData->key.innerVid,
            avl_infoData->outerVlan_result, avl_infoData->outerPrio_result,
            avl_infoData->innerVlan_result, avl_infoData->innerPrio_result,
            avl_infoData->outerVlanAction , avl_infoData->outerPrioAction,
            avl_infoData->innerVlanAction , avl_infoData->innerPrioAction,
            (avl_infoData->remove_VLANs) ? "INACTIVE" : "" );

    /* Prepare next key */
    memcpy(&avl_key, &avl_infoData->key, sizeof(ptinXlateKey_t));
  }
  
  printf("Done! Total entries: %u\r\n", avl_tree->number_of_entries);
  fflush(stdout);
}

L7_RC_t devshell_ptin_xlate_ingress_get( L7_uint32 ptin_port, L7_uint16 outerVlanId, L7_uint16 innerVlanId )
{
  L7_uint16 newOuterVlanId;
  L7_RC_t rc;

  rc = ptin_xlate_ingress_get(ptin_port, outerVlanId, innerVlanId, &newOuterVlanId, L7_NULLPTR);

  printf("ptin_xlate_ingress_get(%u, %u, %u, &newOuterVlanId)=>%u newOuterVlanId=%u", ptin_port, outerVlanId, innerVlanId, rc, newOuterVlanId);

  return rc;
}

L7_RC_t devshell_ptin_xlate_ingress_get_originalVlan( L7_uint32 ptin_port, L7_uint16 newOuterVlanId )
{
  L7_uint16 outerVlanId;
  L7_RC_t rc;

  rc = ptin_xlate_ingress_get_originalVlan(ptin_port, &outerVlanId, L7_NULLPTR, newOuterVlanId, L7_NULL);

  printf("ptin_xlate_ingress_get_originalVlan(%u, &outerVlanId, %u)=>%u outerVlanId=%u", ptin_port, newOuterVlanId, rc, outerVlanId);

  return rc;
}

L7_RC_t devshell_ptin_xlate_egress_get( L7_uint32 ptin_port, L7_uint16 outerVlanId, L7_uint16 innerVlanId )
{
  L7_uint16 newOuterVlanId;
  L7_RC_t rc;

  rc = ptin_xlate_egress_get(ptin_port, outerVlanId, innerVlanId, &newOuterVlanId, L7_NULLPTR);

  printf("ptin_xlate_egress_get(%u, %u, %u, &newOuterVlanId)=>%u newOuterVlanId=%u", ptin_port, outerVlanId, innerVlanId, rc, newOuterVlanId);

  return rc;
}

L7_RC_t devshell_ptin_xlate_egress_get_originalVlan( L7_uint32 ptin_port, L7_uint16 newOuterVlanId )
{
  L7_uint16 outerVlanId;
  L7_RC_t rc;

  rc = ptin_xlate_egress_get_originalVlan(ptin_port, &outerVlanId, L7_NULLPTR, newOuterVlanId, L7_NULL);

  printf("ptin_xlate_egress_get_originalVlan(%u, &outerVlanId, %u)=>%u outerVlanId=%u", ptin_port, newOuterVlanId, rc, outerVlanId);

  return rc;
}

