/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_l2_ipsg.c
*
* @purpose   IP Source Guard
*
* @component hapi
*
* @comments
*
* @create    4/02/07
*
* @author    colinw
*
* @end
*
**********************************************************************/

#include <string.h>

#include "commdefs.h"
#include "datatypes.h"
#include "osapi.h"
#include "osapi_support.h"
#include "avl_api.h"

#include "broad_common.h"
#include "broad_l2_std.h"
#include "broad_l2_ipsg.h"
#include "broad_l2_lag.h"
#include "broad_policy.h"
#include "sysbrds.h"

static void *hapiBroadIpsgSemaphore = L7_NULL;

static avlTree_t        hapiBroadIpsgTree;
static avlTreeTables_t *hapiBroadIpsgTreeHeap;
static ipsgEntry_t     *hapiBroadIpsgDataHeap;

/*********************************************************************
*
* @purpose Determines if IPSG is supported for this silicon
*
* @param   
*
* @returns L7_BOOL
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadIpsgSupported()
{
  L7_BOOL supported;
  static L7_BOOL first_time = L7_TRUE;
  static const bcm_sys_board_t *board_info ;

  /* First get the board info using the bcm call */
  if (first_time)
  {
    board_info = hpcBoardGet() ;
    first_time = L7_FALSE;
  }
  if (board_info == L7_NULL) 
  {
    return L7_FALSE;
  }

  /* Based on the board, return the family */

  switch (board_info->npd_id)
  {
  case __BROADCOM_56514_ID:
  case __BROADCOM_56624_ID:
  case __BROADCOM_56680_ID:
  case __BROADCOM_56685_ID:  /* PTin added: new switch */
  case __BROADCOM_56843_ID:  /* PTin added: new switch BCM56843 */
  case __BROADCOM_56820_ID:
  case __BROADCOM_56634_ID:
  case __BROADCOM_56524_ID:
  case __BROADCOM_56636_ID:
  case __BROADCOM_56334_ID:
    supported = L7_TRUE;
    break;
  default:
    supported = L7_FALSE;
    break;
  }

#if L7_FEAT_IPSG_ON_IFP
  supported = L7_TRUE;
#endif

  return supported;
}

/*********************************************************************
 *
 * @purpose Initializes IPSG code
 *
 * @param *dapi_g          system information
 *
 * @returns L7_RC_t result
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadIpsgInit(DAPI_t *dapi_g)
{
  BROAD_SYSTEM_t *hapiSystem;

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  if (hapiBroadIpsgSupported())
  {
    hapiBroadIpsgSemaphore = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
    if (hapiBroadIpsgSemaphore == L7_NULL)
    {
      LOG_ERROR(0);
    }

    hapiBroadIpsgTreeHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                        HAPI_BROAD_IPSG_TREE_HEAP_SIZE);
    hapiBroadIpsgDataHeap = osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                        HAPI_BROAD_IPSG_DATA_HEAP_SIZE);

    if ((hapiBroadIpsgTreeHeap == L7_NULL) || 
        (hapiBroadIpsgDataHeap == L7_NULL))
    {
      LOG_ERROR(0);
    }

    /* Initialize the storage for all the AVL trees */
    memset (&hapiBroadIpsgTree, 0, sizeof (hapiBroadIpsgTree));
    memset (hapiBroadIpsgTreeHeap, 0, HAPI_BROAD_IPSG_TREE_HEAP_SIZE);
    memset (hapiBroadIpsgDataHeap,  0, HAPI_BROAD_IPSG_DATA_HEAP_SIZE);

    /* Create the IPSG avl trees */
    avlCreateAvlTree(&hapiBroadIpsgTree,
                     hapiBroadIpsgTreeHeap,
                     hapiBroadIpsgDataHeap,
                     HAPI_BROAD_IPSG_TBL_SIZE,
                     sizeof(ipsgEntry_t), 0,
                     sizeof(ipsgSearchKey_t));
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Installs the default policy for IPSG (allows DHCP to pass
 *          and blocks all other IP traffic).
 *
 * @param *dapi_g
 *
 * @returns 
 *
 * @notes L7_RC_t
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadIpsgDefaultPolicyInstall(DAPI_t *dapi_g)
{
  L7_ushort16         ip_ethtype = L7_ETYPE_IP;
#if HAPI_BROAD_IPSG_IPV6_SUPPORTED
  L7_ushort16         ipV6_ethtype = L7_ETYPE_IPV6;
#endif
  L7_uchar8           exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE};
#if (L7_FEAT_IPSG_ON_IFP == 0)
  L7_uchar8           udp_proto[]   = {0x11};
  L7_ushort16         dhcpc_dport   = UDP_PORT_DHCP_CLNT;
  L7_ushort16         dhcps_dport   = UDP_PORT_DHCP_SERV;
#endif
  BROAD_POLICY_RULE_t ruleId;
  L7_RC_t             result = L7_SUCCESS;
  BROAD_POLICY_RULE_PRIORITY_t priority;

  BROAD_SYSTEM_t *hapiSystem;

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  if (hapiBroadIpsgSupported())
  {
#if L7_FEAT_IPSG_ON_IFP
    /* The default rule is installed w/ the system policies. We need to be careful how the IPSG rules interact
       w/ other system policies. Ultimately, we want to allow DHCP packets from any host, but drop all IPv4
       traffic from unknown hosts. When installing the default IPSG rules in the system slice in the IFP,
       the assumption is that the DHCP rules are already installed at a higher priority, and other
       IPv4 rules come after the default IPSG rule. */
    hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
    priority = BROAD_POLICY_RULE_PRIORITY_DEFAULT; /* These rules have default priority
                                                      compared to other system rules. */
#else
    hapiBroadPolicyCreate(BROAD_POLICY_TYPE_IPSG);
    hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_LOOKUP);
    priority = BROAD_POLICY_RULE_PRIORITY_LOWEST; /* These rules have low priority
                                                     compared to other IPSG rules. */

    /* Need to allow DHCP packets for DHCP snooping to work */
    hapiBroadPolicyPriorityRuleAdd(&ruleId, priority);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8*)&ip_ethtype, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO, udp_proto, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT, (L7_uchar8*)&dhcpc_dport, exact_match);

    hapiBroadPolicyPriorityRuleAdd(&ruleId, priority);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8*)&ip_ethtype, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO, udp_proto, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT, (L7_uchar8*)&dhcps_dport, exact_match);
#endif

    /* Drop all other IP packets */
    hapiBroadPolicyPriorityRuleAdd(&ruleId, priority);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8*)&ip_ethtype, exact_match);
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);

#if HAPI_BROAD_IPSG_IPV6_SUPPORTED
    /* When we support IPv6 IPSG, we'll need to include a default rule to drop IPv6 traffic. */
    hapiBroadPolicyPriorityRuleAdd(&ruleId, priority);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ipV6_ethtype, exact_match);
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
#endif

    result =  hapiBroadPolicyCommit(&hapiSystem->ipsgPolicyId);
    if (L7_SUCCESS != result)
       return result;

#if L7_FEAT_IPSG_ON_IFP
    result = hapiBroadPolicyRemoveFromAll(hapiSystem->ipsgPolicyId);
    if (L7_SUCCESS != result)
       return result;
#endif
  }

  return result;
}

/*********************************************************************
 *
 * @purpose Removes the default policy for IPSG.
 *
 * @param *dapi_g
 *
 * @returns 
 *
 * @notes none
 *
 * @end
 *
 *********************************************************************/
void hapiBroadIpsgDefaultPolicyRemove(DAPI_t *dapi_g)
{
  BROAD_SYSTEM_t *hapiSystem;

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  if (hapiBroadIpsgSupported())
  {
    if (BROAD_POLICY_INVALID != hapiSystem->ipsgPolicyId)
    {
      hapiBroadPolicyDelete(hapiSystem->ipsgPolicyId);

      hapiSystem->ipsgPolicyId = BROAD_POLICY_INVALID;
    }
  }
}

/*********************************************************************
*
* @purpose Take semaphore to protect IPSG resources
*
* @param   
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIpsgSemTake()
{
    L7_RC_t rc;

    if (!hapiBroadIpsgSupported())
      return L7_SUCCESS;

    rc = osapiSemaTake(hapiBroadIpsgSemaphore, L7_WAIT_FOREVER);

    return rc;
}

/*********************************************************************
*
* @purpose Give semaphore to protect IPSG resources
*
* @param   
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIpsgSemGive()
{
    L7_RC_t rc;

    if (!hapiBroadIpsgSupported())
      return L7_SUCCESS;

    rc = osapiSemaGive(hapiBroadIpsgSemaphore);

    return rc;
}

/*********************************************************************
*
* @purpose  Apply QOS attributes to a port or LAG interface
*
* @param    BROAD_POLICY_t    policy
* @param    DAPI_USP_t       *usp
* @param    DAPI_t           *dapi_g
*
* @returns  L7_RC_t
*
* @notes    none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIpsgApplyPolicy(BROAD_POLICY_t policy, DAPI_USP_t *usp, DAPI_t *dapi_g)
{
    DAPI_PORT_t                *dapiPortPtr;
    BROAD_PORT_t               *hapiPortPtr;
    L7_RC_t                     result = L7_SUCCESS;

    dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
    hapiPortPtr = dapiPortPtr->hapiPort;

    if (hapiPortPtr->port_is_lag)
    {
        int               i;
        DAPI_LAG_ENTRY_t *lagMemberSet;
        BROAD_PORT_t     *lagMemberPtr;
        L7_RC_t           tmpRc;

        lagMemberSet = dapiPortPtr->modeparm.lag.memberSet;

        /* apply policy to each LAG member */
        for (i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++)
        {
            if (L7_TRUE == lagMemberSet[i].inUse)
            {
                lagMemberPtr = HAPI_PORT_GET(&lagMemberSet[i].usp, dapi_g);

                tmpRc = hapiBroadPolicyApplyToIface(policy, lagMemberPtr->bcmx_lport);
                if (L7_SUCCESS != tmpRc)
                    result = tmpRc;
            }
        }
    }
    else
    {
      /* apply policy to individual port */
      result = hapiBroadPolicyApplyToIface(policy, hapiPortPtr->bcmx_lport);
    }

    return result;
}

/*********************************************************************
*
* @purpose Updates the IPSG policy memberships for a port.
*
* @param   *usp             @b{(input)} The USP of the port that is to be acted upon
* @param   oldIpsgEnabled   @b{(input)} The previous IPSG mode for this USP
* @param   newIpsgEnabled   @b{(input)} The new IPSG mode for this USP 
* @param   *uspKey          @b{(input)} USP key used to apply IPSG policies to this USP.
* @param   *dapi_g          @b{(input)} The driver object
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIpsgPortUpdate(DAPI_USP_t  *portUsp,
                                L7_BOOL      oldIpsgEnabled,
                                L7_BOOL      newIpsgEnabled,
                                DAPI_t      *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  BROAD_PORT_t   *hapiPortPtr;
  bcmx_lport_t    lport;
  L7_uchar8       temp8;
  BROAD_SYSTEM_t *hapiSystem;

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);

  lport = hapiPortPtr->bcmx_lport;

  temp8 = (oldIpsgEnabled << 4) | newIpsgEnabled;
  switch (temp8)
  {
  case 0x00:
  case 0x11:
    /* No change */
    break;

  case 0x01:
    /* Add this port to the IPSG default policy */
    rc = hapiBroadPolicyApplyToIface(hapiSystem->ipsgPolicyId, lport);
    break;
  case 0x10:
    /* Remove this port from the IPSG default policy */
    rc = hapiBroadPolicyRemoveFromIface(hapiSystem->ipsgPolicyId, lport);
    break;
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Used to configure IPSG on a port.
*
* @param   *usp             @b{(input)} The USP of the port that is to be acted upon
* @param   oldIpsgEnabled   @b{(input)} The previous IPSG mode for this USP
* @param   newIpsgEnabled   @b{(input)} The new IPSG mode for this USP 
* @param   *dapi_g          @b{(input)} The driver object
*
* @returns  void
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIpsgSet( DAPI_USP_t  *usp, 
                          L7_BOOL      oldIpsgEnabled,
                          L7_BOOL      newIpsgEnabled,
                          DAPI_t      *dapi_g )
{
  L7_int32                 rc = L7_SUCCESS;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;
  L7_uint32                i;

  dapiPortPtr = DAPI_PORT_GET( usp, dapi_g );
  hapiPortPtr = HAPI_PORT_GET( usp, dapi_g );

  /* assumes this function is only called w/ physical and LAG ports */
  if ( IS_PORT_TYPE_PHYSICAL( dapiPortPtr ) )
  {
    if ( hapiPortPtr->hapiModeparm.physical.isMemberOfLag != L7_TRUE )
    {
      rc = hapiBroadIpsgPortUpdate(usp,
                                   oldIpsgEnabled,
                                   newIpsgEnabled,
                                   dapi_g);
    }
  }
  else if ( IS_PORT_TYPE_LOGICAL_LAG( dapiPortPtr ) )
  {
    hapiBroadLagCritSecEnter();

    for ( i = 0; i < L7_MAX_MEMBERS_PER_LAG; i++ )
    {
      if ( L7_TRUE == dapiPortPtr->modeparm.lag.memberSet[i].inUse )
      {
        rc = hapiBroadIpsgPortUpdate(&dapiPortPtr->modeparm.lag.memberSet[i].usp,
                                     oldIpsgEnabled,
                                     newIpsgEnabled,
                                     dapi_g);
      }
    }
    hapiBroadLagCritSecExit();
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
*
* @purpose Notifies the IPSG component that a physical port has been
*          added to a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIpsgLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  BROAD_PORT_t   *hapiPortPtr;
  BROAD_PORT_t   *hapiLagPtr;
  ipsgEntry_t     ipsgEntry;
  ipsgEntry_t    *pIpsgEntry;

  hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);
  hapiLagPtr     = HAPI_PORT_GET(lagUsp,  dapi_g);

  if (hapiBroadIpsgSupported())
  {

    rc = hapiBroadIpsgPortUpdate(portUsp, 
                                 hapiPortPtr->ipsgEnabled,
                                 hapiLagPtr->ipsgEnabled,
                                 dapi_g);

    /* If this port was enabled for IPSG, remove any clients authorized for that port */
    if (hapiPortPtr->ipsgEnabled)
    {
      /* Remove any clients configured for this port */
      memset(&ipsgEntry, 0, sizeof(ipsgEntry));
      ipsgEntry.key.usp = *portUsp;

      pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_EXACT);
      if (pIpsgEntry == L7_NULL)
      {
        pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_NEXT);
      }
      while ((pIpsgEntry != L7_NULL) && (memcmp(&pIpsgEntry->key.usp, portUsp, sizeof(DAPI_USP_t)) == 0))
      {
        memcpy(&ipsgEntry, pIpsgEntry, sizeof(ipsgEntry));

        hapiBroadPolicyRemoveFromIface(pIpsgEntry->policyId, hapiPortPtr->bcmx_lport);

        pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_NEXT);
      }
    }

    /* If this LAG is enabled for IPSG, add any clients authorized for the LAG */
    if (hapiLagPtr->ipsgEnabled)
    {
      /* Add any clients configured for this LAG */
      memset(&ipsgEntry, 0, sizeof(ipsgEntry));
      ipsgEntry.key.usp = *lagUsp;

      pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_EXACT);
      if (pIpsgEntry == L7_NULL)
      {
        pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_NEXT);
      }
      while ((pIpsgEntry != L7_NULL) && (memcmp(&pIpsgEntry->key.usp, lagUsp, sizeof(DAPI_USP_t)) == 0))
      {
        memcpy(&ipsgEntry, pIpsgEntry, sizeof(ipsgEntry));

        hapiBroadPolicyApplyToIface(pIpsgEntry->policyId, hapiPortPtr->bcmx_lport);

        pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_NEXT);
      }
    }

  }

  return rc;
}

/*********************************************************************
*
* @purpose Notifies the IPSG component that a physical port has been
*          removed from a LAG port
*
* @param   DAPI_USP_t       *portUsp
* @param   DAPI_USP_t       *lagUsp
* @param   DAPI_t           *dapi_g
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/

L7_RC_t hapiBroadIpsgLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  BROAD_PORT_t   *hapiPortPtr;
  BROAD_PORT_t   *hapiLagPtr;
  ipsgEntry_t     ipsgEntry;
  ipsgEntry_t    *pIpsgEntry;

  hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);
  hapiLagPtr     = HAPI_PORT_GET(lagUsp,  dapi_g);

  if (hapiBroadIpsgSupported())
  {
    rc = hapiBroadIpsgPortUpdate(portUsp, 
                                 hapiLagPtr->ipsgEnabled,
                                 hapiPortPtr->ipsgEnabled,
                                 dapi_g);

    /* If the LAG was enabled for IPSG, remove any clients authorized for the LAG */
    if (hapiLagPtr->ipsgEnabled)
    {
      /* Remove any clients configured for this LAG */
      memset(&ipsgEntry, 0, sizeof(ipsgEntry));
      ipsgEntry.key.usp = *lagUsp;

      pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_EXACT);
      if (pIpsgEntry == L7_NULL)
      {
        pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_NEXT);
      }
      while ((pIpsgEntry != L7_NULL) && (memcmp(&pIpsgEntry->key.usp, lagUsp, sizeof(DAPI_USP_t)) == 0))
      {
        memcpy(&ipsgEntry, pIpsgEntry, sizeof(ipsgEntry));

        hapiBroadPolicyRemoveFromIface(pIpsgEntry->policyId, hapiPortPtr->bcmx_lport);

        pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_NEXT);
      }
    }

    /* If this port is enabled for IPSG, add any clients authorized for this port */
    if (hapiPortPtr->ipsgEnabled)
    {
      /* Add any clients configured for this port */
      memset(&ipsgEntry, 0, sizeof(ipsgEntry));
      ipsgEntry.key.usp = *portUsp;

      pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_EXACT);
      if (pIpsgEntry == L7_NULL)
      {
        pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_NEXT);
      }
      while ((pIpsgEntry != L7_NULL) && (memcmp(&pIpsgEntry->key.usp, portUsp, sizeof(DAPI_USP_t)) == 0))
      {
        memcpy(&ipsgEntry, pIpsgEntry, sizeof(ipsgEntry));

        hapiBroadPolicyApplyToIface(pIpsgEntry->policyId, hapiPortPtr->bcmx_lport);

        pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_NEXT);
      }
    }
  }

  return rc;
}

/*********************************************************************
*
* @purpose Enable/disable a port for IP Source Guard
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_IPSG_CONFIG
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfIpsgConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result       = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd     = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;
  ipsgEntry_t              ipsgEntry;
  ipsgEntry_t            *pIpsgEntry;

  if (dapiCmd->cmdData.ipsgEnable.getOrSet != DAPI_CMD_SET)
    return L7_FAILURE;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if ( ( L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ) ) &&
       ( L7_FALSE == IS_PORT_TYPE_LOGICAL_LAG( dapiPortPtr ) ) )
  {
    return( L7_FAILURE );
  }

  if (!hapiBroadIpsgSupported())
  {
    return( L7_FAILURE );
  }

  hapiBroadIpsgSemTake();

  if (dapiCmd->cmdData.ipsgEnable.enabled == L7_FALSE)
  {
    /* Remove any clients configured for this port */
    memset(&ipsgEntry, 0, sizeof(ipsgEntry));
    ipsgEntry.key.usp = *usp;

    pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_EXACT);
    if (pIpsgEntry == L7_NULL)
    {
      pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_NEXT);
    }
    while ((pIpsgEntry != L7_NULL) && (memcmp(&pIpsgEntry->key.usp, usp, sizeof(DAPI_USP_t)) == 0))
    {
      memcpy(&ipsgEntry, pIpsgEntry, sizeof(ipsgEntry));

      hapiBroadPolicyDelete(pIpsgEntry->policyId);
      avlDeleteEntry(&hapiBroadIpsgTree, pIpsgEntry);

      pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_NEXT);
    }
  }

  result = hapiBroadIpsgSet( usp, hapiPortPtr->ipsgEnabled, dapiCmd->cmdData.ipsgEnable.enabled, dapi_g);
  if (result == L7_SUCCESS)
  {
    hapiPortPtr->ipsgEnabled = dapiCmd->cmdData.ipsgEnable.enabled;
  }

  hapiBroadIpsgSemGive();

  return( result );
}

/*********************************************************************
*
* @purpose Get the IP Source Guard statistics for a port
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_IPSG_STATS_GET
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfIpsgStatsGet(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 result       = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd     = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;

  if (dapiCmd->cmdData.ipsgStats.getOrSet != DAPI_CMD_GET)
    return L7_FAILURE;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if ( ( L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ) ) &&
       ( L7_FALSE == IS_PORT_TYPE_LOGICAL_LAG( dapiPortPtr ) ) )
  {
    return( L7_FAILURE );
  }

  if (!hapiBroadIpsgSupported())
  {
    return( L7_FAILURE );
  }

  /* Not supported */
  *dapiCmd->cmdData.ipsgStats.droppedPackets = 0;

  return result;
}

/*********************************************************************
*
* @purpose Add an allowable IP Source Guard client for a port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_IPSG_CLIENT_ADD
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfIpsgClientAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result      = L7_FAILURE;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd     = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;
  ipsgEntry_t              ipsgEntry;
  ipsgEntry_t             *pIpsgEntry;
  BROAD_POLICY_RULE_t      ruleId;
  L7_uchar8                exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                            FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                            FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                            FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  L7_uchar8                null_mac[6] = {0};

  if (dapiCmd->cmdData.ipsgClientAdd.getOrSet != DAPI_CMD_SET)
    return L7_FAILURE;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if ( ( L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ) ) &&
       ( L7_FALSE == IS_PORT_TYPE_LOGICAL_LAG( dapiPortPtr ) ) )
  {
    return( L7_FAILURE );
  }

  if (!hapiBroadIpsgSupported())
  {
    return( L7_FAILURE );
  }

  if (hapiPortPtr->ipsgEnabled == L7_FALSE)
  {
    return( L7_FAILURE );
  }

  hapiBroadIpsgSemTake();

  memset(&ipsgEntry, 0, sizeof(ipsgEntry));
  ipsgEntry.key.usp = *usp;
  if (dapiCmd->cmdData.ipsgClientAdd.ip4Addr)
  {
    ipsgEntry.key.u.ip4Addr = dapiCmd->cmdData.ipsgClientAdd.ip4Addr;
  }
#if HAPI_BROAD_IPSG_IPV6_SUPPORTED
  else
  {
    ipsgEntry.key.u.ip6Addr = dapiCmd->cmdData.ipsgClientAdd.ip6Addr;
  }
#endif

  /* Check to see if the entry already exists */
  pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_EXACT);

  if (pIpsgEntry != L7_NULL)
  {
    hapiBroadIpsgSemGive();
    return L7_ALREADY_CONFIGURED;
    /* return( L7_SUCCESS); */
  }

  /* Attempt to add new entry to AVL tree */
  pIpsgEntry = avlInsertEntry(&hapiBroadIpsgTree, &ipsgEntry);

  if (pIpsgEntry == L7_NULL)
  {
    pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_EXACT);
    
    if (pIpsgEntry == L7_NULL)
    {
      /* We can't find the entry we just inserted successfully */
      hapiBroadIpsgSemGive();
      return L7_FAILURE; 
    }

    /* Attempt to add new entry to HW */
    if (hapiBroadPolicyCreate(BROAD_POLICY_TYPE_IPSG) == L7_SUCCESS)
    {
#if (L7_FEAT_IPSG_ON_IFP == 0)
      hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_LOOKUP);
#endif
      hapiBroadPolicyRuleAdd(&ruleId);
      if (dapiCmd->cmdData.ipsgClientAdd.ip4Addr)
      {
        hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_SIP, (L7_uchar8 *)&dapiCmd->cmdData.ipsgClientAdd.ip4Addr, exact_match);
      }
#if HAPI_BROAD_IPSG_IPV6_SUPPORTED
      else
      {
        hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IP6_SRC, dapiCmd->cmdData.ipsgClientAdd.ip6Addr.in6.addr8, exact_match);
      }
#endif

      /* Include the MAC if it was specified */
      if (memcmp(dapiCmd->cmdData.ipsgClientAdd.macAddr.addr, null_mac, L7_ENET_MAC_ADDR_LEN) != 0)
      {
        hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACSA, dapiCmd->cmdData.ipsgClientAdd.macAddr.addr, exact_match);
      }

      hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_PERMIT, 0, 0, 0);

      if (hapiBroadPolicyCommit(&pIpsgEntry->policyId) == L7_SUCCESS)
      {
        result = hapiBroadIpsgApplyPolicy(pIpsgEntry->policyId, usp ,dapi_g);
      }

      /* If we cannot add the entry to HW, delete the policy. */
      if (result != L7_SUCCESS)
      {
        hapiBroadPolicyDelete(pIpsgEntry->policyId);
      }
    }

    /* If we cannot add the entry to HW, delete it from the AVL tree */
    if (result != L7_SUCCESS)
    {
      avlDeleteEntry(&hapiBroadIpsgTree, pIpsgEntry);
    }
  }

  hapiBroadIpsgSemGive();

  return result;
}

/*********************************************************************
*
* @purpose Delete an allowable IP Source Guard client for a port.
*
* @param   DAPI_USP_t *usp    - needs to be a valid usp
* @param   DAPI_CMD_t  cmd    - DAPI_CMD_INTF_IPSG_CLIENT_DELETE
* @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
* @param   DAPI_t     *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfIpsgClientDelete(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result      = L7_FAILURE;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd     = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;
  ipsgEntry_t              ipsgEntry;
  ipsgEntry_t            *pIpsgEntry;

  if (dapiCmd->cmdData.ipsgClientDelete.getOrSet != DAPI_CMD_SET)
    return L7_FAILURE;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if ( ( L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ) ) &&
       ( L7_FALSE == IS_PORT_TYPE_LOGICAL_LAG( dapiPortPtr ) ) )
  {
    return( L7_FAILURE );
  }

  if (!hapiBroadIpsgSupported())
  {
    return( L7_FAILURE );
  }

  if (hapiPortPtr->ipsgEnabled == L7_FALSE)
  {
    return( L7_FAILURE );
  }

  hapiBroadIpsgSemTake();

  memset(&ipsgEntry, 0, sizeof(ipsgEntry));
  ipsgEntry.key.usp = *usp;
  if (dapiCmd->cmdData.ipsgClientDelete.ip4Addr)
  {
    ipsgEntry.key.u.ip4Addr = dapiCmd->cmdData.ipsgClientDelete.ip4Addr;
  }
#if HAPI_BROAD_IPSG_IPV6_SUPPORTED
  else
  {
    ipsgEntry.key.u.ip6Addr = dapiCmd->cmdData.ipsgClientDelete.ip6Addr;
  }
#endif

  pIpsgEntry = avlSearchLVL7(&hapiBroadIpsgTree, &ipsgEntry, AVL_EXACT);

  if (pIpsgEntry != L7_NULL)
  {
    result = hapiBroadPolicyDelete(pIpsgEntry->policyId);
    avlDeleteEntry(&hapiBroadIpsgTree, pIpsgEntry);
  }

  hapiBroadIpsgSemGive();
  return result;
}
