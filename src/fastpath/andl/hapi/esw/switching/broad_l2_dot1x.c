/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* Name: broad_l2_dot1x.c
*
* Purpose: Implement Layer-2 Dot1x HAPI code
*
* Component: hapi
*
* Comments:
*
* Created by: colinw 4/16/07
*
*********************************************************************/

#include <string.h>

#include "commdefs.h"
#include "datatypes.h"
#include "osapi.h"
#include "osapi_support.h"
#include "sysapi.h"
#include "zlib.h"

#include "broad_common.h"
#include "broad_l2_std.h"
#include "broad_l2_mcast.h"
#include "broad_l3.h"
#include "broad_l2_vlan.h"
#include "broad_l2_lag.h"
#include "broad_policy.h"
#include "sysbrds.h"
#include "bcmx/port.h"
#include "bcmx/auth.h"

#include "l7_usl_bcmx_l2.h"
#include "l7_usl_api.h"
#include "l7_usl_bcmx_port.h"

#ifdef L7_QOS_PACKAGE
#include "broad_qos_common.h"
#include "broad_diffserv.h"
#endif
#include "bcm/field.h"
#include "l7utils_api.h"


void hapiBroadDot1xAuthMacUpdate(BROAD_PORT_t *hapiPortPtr, DAPI_t *dapi_g);

static void *hapiBroadDot1xSemaphore = L7_NULL;

static L7_RC_t hapiBroadDot1xMacAddrTimeoutCheck(bcmx_lport_t port,L7_enetMacAddr_t macAddr, L7_ushort16 vid,L7_BOOL *timedout, DAPI_t *dapi_g);

static L7_BOOL hapiVoiceVlanDebug=0;

void hapiBroadDot1xDebugSet(L7_BOOL val)
{
  hapiVoiceVlanDebug=val;
}

/*********************************************************************
*
* @function hapiBroadDot1xPolicyTLVHash
*
* @purpose  Hash TLV to generate pseudo unique identifier
*
* @param    L7_tlv_t *pTLV
*
* @returns  L7_uint32
*
* @notes    none
*
* @end
*
*********************************************************************/
static L7_uint32 hapiBroadDot1xPolicyTLVHash(L7_tlv_t *pTLV)
{
    return crc32(0L, (L7_uchar8*)pTLV, osapiNtohl(pTLV->length) + sizeof(pTLV->type) + sizeof(pTLV->length));
}

/*********************************************************************
* @purpose  Find the first unused bit in a group ID mask
*
* @param    mask        @b{(input)}  The interface mask to evaluate
* @param    intIfNnum   @b{(output)} The unused intIfNum in the mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if there were no unused bits
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t hapiBroadDot1xGroupIdAllocate(HAPI_DOT1X_GROUP_MASK_t *mask, L7_uint32 *groupId) 
{
  L7_uint32 ol,il;                              
  L7_uchar8 value;                              
  L7_RC_t rc = L7_ERROR;

  for (ol = 0;ol < HAPI_DOT1X_GROUP_INDICES;ol++)
  {
    value = ~(*mask).value[ol] ;

    if (value > 0)
    {
      for (il = 0;il < 8 ; il++)
      {
        if (value & (1 << il))
        {
          *(groupId) = (ol * 8) + il + 1; /* ensure that groupId's are 1-based */
          HAPI_DOT1X_GROUP_SETMASKBIT(*mask, *groupId - 1);
          rc = L7_SUCCESS;                 
          break;                                
        }
      }                                         
      break;                                    
    }
  }                                             

  return(rc);
} 

/*********************************************************************
* @purpose  Find the first unused bit in a group ID mask
*
* @param    mask        @b{(input)}  The interface mask to evaluate
* @param    intIfNnum   @b{(output)} The unused intIfNum in the mask
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR  if there were no unused bits
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t hapiBroadDot1xGroupIdFree(HAPI_DOT1X_GROUP_MASK_t *mask, L7_uint32 groupId) 
{
  L7_uint32 bitNumber;

  bitNumber = groupId - 1; /* bitNumber is zero-based */

  if (bitNumber >= HAPI_DOT1X_GROUP_INDICES)
  {
    LOG_ERROR(bitNumber);
  }

  HAPI_DOT1X_GROUP_CLRMASKBIT(*mask, bitNumber);

  return(L7_SUCCESS);
} 

/*********************************************************************
* @purpose  Creates a Diffserv policy for an authorized client on 
*           a dot1x interface.
*
* @param    
* @param    
*
* @returns  L7_RC_t
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t hapiBroadDot1xPolicyCreate(DAPI_USP_t *usp, 
                                   L7_tlv_t   *pTLV, 
                                   L7_uint32   groupId, 
                                   L7_uint32  *policyId, 
                                   DAPI_t     *dapi_g)
{
#ifdef L7_QOS_PACKAGE
  L7_RC_t                  result  = L7_FAILURE;
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *dapiPortPtr;
  HAPI_BROAD_QOS_PORT_t   *qosPort;
  HAPI_QOS_INTF_DIR_t      direction = HAPI_QOS_INTF_DIR_IN;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  qosPort     = (HAPI_BROAD_QOS_PORT_t *)hapiPortPtr->qos;

  /* If this port has an ACL, or non-client specific Diffserv policy already applied,
     return failure. */
  if (qosPort->aclds.policyId[direction] != BROAD_POLICY_INVALID)
  {
    return L7_FAILURE;
  }

  if (hapiBroadDiffServPolicyCreate(usp, pTLV, direction, groupId, L7_FALSE, policyId, dapi_g) == L7_SUCCESS)
  {
    result = hapiBroadPolicyApplyToIface(*policyId, hapiPortPtr->bcmx_lport);
  }

  if (result != L7_SUCCESS)
  {
    if (*policyId != BROAD_POLICY_INVALID)
    {
      hapiBroadPolicyDelete(*policyId);
    }
  }

  return result;
#else
  return L7_FAILURE;
#endif
}

/*********************************************************************
* @purpose  Adds the specified MAC/VLAN pair to the L2FDB as a static 
*           entry.
*
* @param    
* @param    
*
* @returns  L7_RC_t
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t hapiBroadDot1xMacAddrAdd(DAPI_USP_t *usp, L7_enetMacAddr_t macAddr, L7_ushort16 vid, L7_uint32 groupId, DAPI_t *dapi_g)
{
  L7_RC_t                       result = L7_SUCCESS;
  BROAD_PORT_t                 *hapiPortPtr;
  bcmx_l2_addr_t                l2Addr;
  L7_int32                      rc=0;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  /* Add this MAC Address to the L2 Forwarding Table stored in the NP. */
  memset(&l2Addr, 0, sizeof (bcmx_l2_addr_t));
  memcpy(l2Addr.mac, macAddr.addr, sizeof (mac_addr_t));
  l2Addr.vid = vid;
  /* add the individual mac addr */
  l2Addr.flags |= (BCM_L2_STATIC | BCM_L2_REPLACE_DYNAMIC);
  l2Addr.lport  = hapiPortPtr->bcmx_lport;
  l2Addr.group = groupId;

  /* Add MAC addr to hw ARL table */
  rc = usl_bcmx_l2_addr_add(&l2Addr, L7_NULL);
  if (L7_BCMX_OK(rc) != L7_TRUE)
  {
    result = L7_FAILURE;

    /* Failing to add dynamic entries is pretty normal, so print an error 
    ** only for the static entries.
    */
    if ((l2Addr.flags & BCM_L2_STATIC) != 0)
    {
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'bcmx_l2_addr_add' - FAILED : %d\n",
                     __FILE__, __LINE__, __FUNCTION__, rc);
    }
    return result;
  } 
  else
  {
    hapiBroadL3UpdateMacLearn(l2Addr.mac, l2Addr.vid, usp, dapi_g);
  }

  return result;
}

/*********************************************************************
* @purpose  Adds the specified MAC/VLAN pair to the L2FDB as a static 
*           entry.
*
* @param    
* @param    
*
* @returns  L7_RC_t
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t hapiBroadDot1xMacAddrDelete(L7_enetMacAddr_t macAddr, L7_ushort16 vid, DAPI_t *dapi_g)
{
  L7_RC_t         result  = L7_SUCCESS;
  L7_int32        rc      = 0;
  bcmx_l2_addr_t  l2Addr;

  memset(&l2Addr,0,sizeof(l2Addr));
  rc = bcmx_l2_addr_get(macAddr.addr, vid, &l2Addr, L7_NULL);

  if (rc == BCM_E_NOT_FOUND)
  {
    /* Entry is not present in hw table, so just return success */
    return result;
  } else if (rc == BCM_E_NONE)
  {
    /* Delete this MAC Address from the L2 Forwarding Table of BCOM ARL. */
    rc = usl_bcmx_l2_addr_delete(macAddr.addr, vid);
    if (L7_BCMX_OK(rc) != L7_TRUE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\n%s %d: In %s call to 'usl_bcmx_l2_addr_remove' - FAILED : %d\n",
                     __FILE__, __LINE__, __FUNCTION__, rc);
      return result;
    }
  }

  return result;
}

/*********************************************************************
*
* @purpose Determines if Dot1x VLAN assignment feature is supported.
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
L7_BOOL hapiBroadDot1xVFPTableSupported()
{
  L7_BOOL supported;
  static L7_BOOL first_time = L7_TRUE;
  static const bcm_sys_board_t *board_info ;

  /* First get the board info using the bcm call */
  if (first_time)
  {
    board_info = hpcBoardGet() ; ;
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
  case __BROADCOM_56685_ID:   /* PTin added: new switch 56689 (Valkyrie2) */
  case __BROADCOM_56843_ID:   /* PTin added: new switch 56843 (Trident) */
  case __BROADCOM_56820_ID:
  case __BROADCOM_56634_ID:
  case __BROADCOM_56524_ID:
  case __BROADCOM_56636_ID:
  case __BROADCOM_56643_ID:   /* PTin added: new switch 56643 (Triumph3) */
  case __BROADCOM_56340_ID:   /* PTin added: new switch 56340 (Helix4) */
    supported = L7_TRUE;
    break;
  default:
    supported = L7_FALSE;
    break;
  }

  return supported;
}
/*********************************************************************
*
* @purpose Determines if Dot1x VLAN assignment feature is supported.
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
L7_BOOL hapiBroadDot1xVlanAssignmentSupported()
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
  case __BROADCOM_56314_ID:
  case __BROADCOM_56514_ID:
  case __BROADCOM_56624_ID:
  case __BROADCOM_56680_ID:
  case __BROADCOM_56685_ID:   /* PTin added: new switch 56689 (Valkyrie2) */
  case __BROADCOM_56843_ID:   /* PTin added: new switch 56843 (Trident) */
  case __BROADCOM_56820_ID:
  case __BROADCOM_56634_ID:
  case __BROADCOM_56524_ID:
  case __BROADCOM_56636_ID:
  case __BROADCOM_56304_ID:
  case __BROADCOM_56643_ID:   /* PTin added: new switch 56643 (Triumph3) */
  case __BROADCOM_56340_ID:   /* PTin added: new switch 56340 (Helix4) */
    supported = L7_TRUE;
    break;
  default:
    supported = L7_FALSE;
    break;
  }

  return supported;

}

/*********************************************************************
*
* @purpose Determines if Dot1x Policy feature is supported.
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
L7_BOOL hapiBroadDot1xPolicySupported()
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
  case __BROADCOM_56314_ID:
  case __BROADCOM_56514_ID:
  case __BROADCOM_56624_ID:
  case __BROADCOM_56680_ID:
  case __BROADCOM_56685_ID:   /* PTin added: new switch 56689 (Valkyrie2) */
  case __BROADCOM_56843_ID:   /* PTin added: new switch 56843 (Trident) */
  case __BROADCOM_56820_ID:
  case __BROADCOM_56634_ID:
  case __BROADCOM_56524_ID:
  case __BROADCOM_56636_ID:
  case __BROADCOM_56643_ID:   /* PTin added: new switch 56643 (Triumph3) */
  case __BROADCOM_56340_ID:   /* PTin added: new switch 56340 (Helix4) */
    supported = L7_TRUE;
    break;
  default:
    supported = L7_FALSE;
    break;
  }

  return supported;

}
/*********************************************************************
 *
 * @purpose Initializes Dot1x code
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
L7_RC_t hapiBroadDot1xInit(DAPI_t *dapi_g)
{
  hapiBroadDot1xSemaphore = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
  if (hapiBroadDot1xSemaphore == L7_NULL)
  {
    LOG_ERROR(0);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Take semaphore to protect dot1x resources
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
L7_RC_t hapiBroadDot1xSemTake()
{
    L7_RC_t rc;

    rc = osapiSemaTake(hapiBroadDot1xSemaphore, L7_WAIT_FOREVER);

    return rc;
}

/*********************************************************************
*
* @purpose Give semaphore to protect dot1x resources
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
L7_RC_t hapiBroadDot1xSemGive()
{
    L7_RC_t rc;

    rc = osapiSemaGive(hapiBroadDot1xSemaphore);

    return rc;
}

/*********************************************************************
*
* @purpose Clears a client that was authorized on this port.
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
L7_RC_t hapiBroadDot1xClientHwDelete(DAPI_USP_t *usp, L7_uint32 clientIndex, DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  BROAD_PORT_t            *hapiPortPtr;
  HAPI_DOT1X_CLIENT_t     *pClient;
  L7_uint32                i;
  L7_ushort16              vlanId;
  DAPI_QVLAN_MGMT_CMD_t    dapiCmd;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  pClient = &hapiPortPtr->dot1x.client[clientIndex];

  /* Remove the MAC address from L2FDB. Need to remove entries for every VLAN this port belongs to. 
     If the application indicated that this client was assigned to a VLAN, then we
     can get away w/ removing just a single entry. */
  if (hapiBroadDot1xVlanAssignmentSupported() && (pClient->vid != 0))
  {
    /* This client was authorized on only one VLAN. */
    hapiBroadDot1xMacAddrDelete(pClient->macAddr,
                                pClient->vid,    
                                dapi_g);         
                                
  }
  else
  {
    /* This client was authorized on all VLANs. */
    for (vlanId = 1; vlanId <= L7_PLATFORM_MAX_VLAN_ID; vlanId++)
    {
      if (BROAD_IS_VLAN_MEMBER(usp, vlanId, dapi_g))
      {
        if(hapiVoiceVlanDebug)
        {
          sysapiPrintf("ClientRemove: FDB Entry Remove: mac[%02.2x:%02.2x:%02.2x:%02.2x:%02.2x:%02.2x] vlanId:[%d]\n\r",
                       pClient->macAddr.addr[0],
                       pClient->macAddr.addr[1],
                       pClient->macAddr.addr[2],
                       pClient->macAddr.addr[3],
                       pClient->macAddr.addr[4],
                       pClient->macAddr.addr[5],
                       vlanId);
        }
        hapiBroadDot1xMacAddrDelete(pClient->macAddr,
                                    vlanId,         
                                    dapi_g);         
                                    
      }
    }
  }

  if(hapiVoiceVlanDebug)
    sysapiPrintf("ClientRemove: pClient->vid:[%d] pClient->policyId:[%d]\n\r",pClient->vid, pClient->policyId);

  /* If the application indicated that this client was assigned to a VLAN, then we
     need to use Policy Manager to remove an entry. */
  if (hapiBroadDot1xVlanAssignmentSupported())
  {
    if (hapiBroadDot1xVFPTableSupported()==L7_FALSE) 
    {
        if (pClient->vid != 0)
        {
          dapiCmd.cmdData.macVlanConfig.getOrSet = DAPI_CMD_SET;
          memcpy(dapiCmd.cmdData.macVlanConfig.mac.addr, pClient->macAddr.addr, sizeof(L7_enetMacAddr_t));
          dapiCmd.cmdData.macVlanConfig.vlanId   = pClient->vid;
          
          if (hapiBroadL2VlanMacEntryDelete(usp, DAPI_CMD_MAC_VLAN_DELETE, &dapiCmd, dapi_g) != L7_SUCCESS)
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                    "\n%s %d: In %s Couldn't delete dot1x VLAN assignment policy\n",
                    __FILE__, __LINE__, __FUNCTION__);
            
            result = L7_FAILURE;
          }
        }
    }
    else
    {
      if (pClient->vlanAssignmentPolicyId != BROAD_POLICY_INVALID)
      {
          if (hapiBroadPolicyDelete(pClient->vlanAssignmentPolicyId) != L7_SUCCESS)
          {
            L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                    "\n%s %d: In %s Couldn't delete dot1x VLAN assignment policy\n",
                      __FILE__, __LINE__, __FUNCTION__);
              result = L7_FAILURE;
          }
        }
    }


  }

  /* If the application indicated that this client had a policy associated to it,
     then remove the policy using Policy Manager. Note that we can only remove
     the policy if no other clients on this port are associated to that policy. */
  if (hapiBroadDot1xPolicySupported())
  {
    if (pClient->policyId != BROAD_POLICY_INVALID)
    {
      /* check to see if any other clients are using this policy */
      for (i = 0; i < L7_DOT1X_PORT_MAX_MAC_USERS; i++)
      {
        if (i == clientIndex)
        {
          continue;
        }
        if ((hapiPortPtr->dot1x.client[i].inUse == L7_TRUE) && 
            (hapiPortPtr->dot1x.client[i].policyId == pClient->policyId))
        {
          break;
        }
      }

      if (i == L7_DOT1X_PORT_MAX_MAC_USERS)
      {
        /* No other clients are using this policy, so delete it and free the group ID. */
        if (hapiBroadPolicyDelete(pClient->policyId) != L7_SUCCESS)
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                  "\n%s %d: In %s Couldn't delete dot1x DiffServ policy\n",
                  __FILE__, __LINE__, __FUNCTION__);

          result = L7_FAILURE;
        }
        hapiBroadDot1xGroupIdFree(&hapiPortPtr->dot1x.groupMask, pClient->groupId);
      }
    }
 }
 

  return result;
}

/*********************************************************************
*
* @purpose Clears all clients that were authorized on this port.
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
L7_RC_t hapiBroadDot1xClientsPurge(DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  BROAD_PORT_t            *hapiPortPtr;
  L7_uint32                i;
  HAPI_DOT1X_CLIENT_t     *pClient;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  for (i = 0; i < L7_DOT1X_PORT_MAX_MAC_USERS; i++)
  {
    if (hapiPortPtr->dot1x.client[i].inUse == L7_TRUE)
    {
      if (hapiBroadDot1xClientHwDelete(usp, i, dapi_g) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      pClient = &hapiPortPtr->dot1x.client[i];
      pClient->inUse                  = L7_FALSE;
      memset(pClient->macAddr.addr, 0, sizeof(L7_enetMacAddr_t));
      pClient->vid                    = 0;
      pClient->policyId               = BROAD_POLICY_INVALID;
      pClient->groupId                = HAPI_DOT1X_GROUP_ID_INVALID;
      pClient->tlvSignature           = 0;
      if (hapiBroadDot1xVFPTableSupported()==L7_TRUE)
      {
        pClient->vlanAssignmentPolicyId  = BROAD_POLICY_INVALID;
      }
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Determines if any dot1x policies have been applied to this port.
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
L7_BOOL hapiBroadDot1xPoliciesApplied(DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *dapiPortPtr;
  L7_uint32                i;
  L7_BOOL                  policyApplied;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ))
  {
    return L7_FALSE;
  }

  policyApplied = L7_FALSE;

  hapiBroadDot1xSemTake();
  if ((hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED) &&
      (hapiPortPtr->dot1x.authorizedClientCount > 0))
  {
    for (i = 0; i < L7_DOT1X_PORT_MAX_MAC_USERS; i++)
    {
      if (hapiPortPtr->dot1x.client[i].inUse == L7_TRUE)
      {
        if (hapiPortPtr->dot1x.client[i].policyId != BROAD_POLICY_INVALID)
        {
          policyApplied = L7_TRUE;
          break;
        }
      }
    }
  }
  hapiBroadDot1xSemGive();

  return policyApplied;
}

/*********************************************************************
*
* @purpose Determines if the client specified by macAddr is authorized
*          on this port.
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
L7_BOOL hapiBroadDot1xIsAuthorizedClient(DAPI_USP_t *usp, L7_uchar8 *macAddr, DAPI_t *dapi_g)
{
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *dapiPortPtr;
  L7_uint32                i;
  L7_BOOL                  authorized = L7_FALSE;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ))
  {
    return L7_TRUE;
  }

  if (hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)
  {
    authorized = L7_TRUE;
  }
  else
  {
    if (hapiPortPtr->dot1x.authorizedClientCount > 0)
    {
      for (i = 0; i < L7_DOT1X_PORT_MAX_MAC_USERS; i++)
      {
        if (hapiPortPtr->dot1x.client[i].inUse == L7_TRUE)
        {
          if (memcmp(hapiPortPtr->dot1x.client[i].macAddr.addr, macAddr, 6) == 0)
          {
            authorized = L7_TRUE;
            break;
          }
        }
      }
    }
  }

  return authorized;
}

/*********************************************************************
*
* @purpose Update the L2FDB as appropriate to allow authorized clients
*          access to this VLAN.
*
* @param   
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadDot1xPortVlanAddNotify (DAPI_USP_t *usp, L7_ushort16 vlanId, DAPI_t *dapi_g)
{
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *dapiPortPtr;
  L7_RC_t                  l2FdbResult  = L7_SUCCESS;
  L7_uint32                i;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ))
  {
    return;
  }

  hapiBroadDot1xSemTake();
  if ((hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED) &&
      (hapiPortPtr->dot1x.authorizedClientCount > 0))
  {
    for (i = 0; i < L7_DOT1X_PORT_MAX_MAC_USERS; i++)
    {
      if (hapiPortPtr->dot1x.client[i].inUse == L7_TRUE)
      {
        if (hapiPortPtr->dot1x.client[i].vid == 0)
        {
          /* Add an L2FDB entry for this MAC/VLAN pair. */
          if (hapiBroadDot1xMacAddrAdd(usp, 
                                       hapiPortPtr->dot1x.client[i].macAddr,
                                       vlanId, 
                                       hapiPortPtr->dot1x.client[i].groupId,
                                       dapi_g) != L7_SUCCESS)
          {
            l2FdbResult = L7_FAILURE;
          }
        }
      }
    }
  }
  hapiBroadDot1xSemGive();

  if (l2FdbResult == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
            "\n%s %d: In %s Couldn't add one or more MAC addresses to VLAN\n",
            __FILE__, __LINE__, __FUNCTION__);
  }
}

/*********************************************************************
*
* @purpose Update the L2FDB as appropriate to disallow authorized clients
*          access to this VLAN.
*
* @param   
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadDot1xPortVlanRemoveNotify (DAPI_USP_t *usp, L7_ushort16 vlanId, DAPI_t *dapi_g)
{
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *dapiPortPtr;
  L7_RC_t                  l2FdbResult  = L7_SUCCESS;
  L7_uint32                i;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ))
  {
    return;
  }

  hapiBroadDot1xSemTake();
  if ((hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED) &&
      (hapiPortPtr->dot1x.authorizedClientCount > 0))
  {
    for (i = 0; i < L7_DOT1X_PORT_MAX_MAC_USERS; i++)
    {
      if (hapiPortPtr->dot1x.client[i].inUse == L7_TRUE)
      {
        if (hapiPortPtr->dot1x.client[i].vid == 0)
        {
          /* Add an L2FDB entry for this MAC/VLAN pair. */
          if (hapiBroadDot1xMacAddrDelete(hapiPortPtr->dot1x.client[i].macAddr, 
                                          vlanId,                              
                                          dapi_g) != L7_SUCCESS)               
                                          
          {
            l2FdbResult = L7_FAILURE;
          }
        }
      }
    }
  }
  hapiBroadDot1xSemGive();

  if (l2FdbResult == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
            "\n%s %d: In %s Couldn't delete one or more MAC addresses from VLAN\n",
            __FILE__, __LINE__, __FUNCTION__);
  }
}

/*********************************************************************
*
* @purpose  Configure 802.1x by either enabling or disabling
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_SYSTEM_DOT1X_CONFIG
* @param   *data          @b{(input)} DAPI_SYSTEM_CMD_t.cmdData.dot1xConfig
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemDot1xConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t            result  = L7_SUCCESS;
  DAPI_SYSTEM_CMD_t       *dapiCmd = (DAPI_SYSTEM_CMD_t*)data;
  BROAD_SYSTEM_t        *hapiSystemPtr;

  if (dapiCmd->cmdData.dot1xConfig.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  /* For broadcom the hardware has already been setup to
   * forward all multicast frames including 0180C2000003
   * to the application. Hence nothing do be done here.
   */
  hapiSystemPtr = (BROAD_SYSTEM_t *)(dapi_g->system->hapiSystem);
  hapiSystemPtr->dot1xMode = dapiCmd->cmdData.dot1xConfig.enable;

  return result;
}

/*********************************************************************
 *
 * @purpose  802.1x : Set the authorization mode of the specified usp
 *
 * @param   *usp           @b{(input)} The USP of the port that is to be acted upon
 * @param   *hapiPortPtr   @b{(input)} BROAD_PORT_t -- the port to be configured
 * @param   *dapi_g        @b{(input)} The driver object
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments Should only be called if an update is necessary.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadIntfDot1xAuthModeUpdate( DAPI_USP_t *usp, BROAD_PORT_t *hapiPortPtr, DAPI_t *dapi_g )
{
  static const char *routine_name = "hapiBroadIntfDot1xAuthModeUpdate()";
  L7_int32  rc;
  usl_bcm_port_learn_mode_t learnMode;

  /*
   * If an L2 flush is occurring, do not modify HW at this time.
   */
  if (hapiPortPtr->l2FlushInProgress == L7_TRUE)
  {
    return L7_SUCCESS;
  }

  if (L7_DOT1X_PORT_STATUS_UNAUTHORIZED == hapiPortPtr->dot1x.dot1xStatus )
  {
    learnMode = 0;
    rc = usl_bcmx_port_learn_set( hapiPortPtr->bcmx_lport, learnMode); /* Do not forward or learn */
    if ( L7_BCMX_OK( rc ) != L7_TRUE )
    {
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
          "\n%s %d: In %s call to 'bcmx_port_learn_set' - FAILED : %d\n",
          __FILE__,
          __LINE__,
          routine_name,
          rc );
      return( L7_FAILURE );
    }
  }
  else                        /* Port-based, set port MAC locking mode */
  {
    /*hapiBroadIntfMacLockUpdate( usp, hapiPortPtr, dapi_g );*/
     /* Restore PML configuration */

      if (hapiPortPtr->locked)
      {
         hapiBroadLearnSet(usp, BCM_PORT_LEARN_CPU, dapi_g);
      }
      else
      {
		  /* enable learning on this port */
		  if (hpcSoftwareLearningEnabled () == L7_TRUE)
              hapiBroadLearnSet(usp, (BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_FWD), dapi_g);
          else
              hapiBroadLearnSet(usp, (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD), dapi_g);
          

      }
  }

  return( L7_SUCCESS );
}

/*********************************************************************
 *
 * @purpose  802.1x : Create the violation policy.
 *
 * @param   *dapi_g        @b{(input)} The driver object
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadDot1xViolationPolicyCreate(DAPI_t *dapi_g)
{
  L7_RC_t                  result = L7_SUCCESS;
  BROAD_SYSTEM_t          *hapiSystem;
  BROAD_POLICY_RULE_t      ruleId;
  L7_ushort16              lookupStatusUnresolvedSa = BROAD_LOOKUPSTATUS_UNRESOLVED_SA;

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  if (hapiSystem->dot1xViolationPolicyId == BROAD_POLICY_INVALID)
  {
    result = L7_FAILURE;
    if (hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM) == L7_SUCCESS)
    {
      hapiBroadPolicyRuleAdd(&ruleId);
      hapiBroadPolicyRuleQualifierAdd(ruleId,
                                      BROAD_FIELD_LOOKUP_STATUS,
                                      (L7_uchar8 *)&lookupStatusUnresolvedSa,
                                      (L7_uchar8 *)&lookupStatusUnresolvedSa);
      hapiBroadPolicyRuleActionAdd(ruleId,
                                   BROAD_ACTION_TRAP_TO_CPU, 0,0,0);
      if (hapiBroadPolicyCommit(&hapiSystem->dot1xViolationPolicyId) == L7_SUCCESS)
      {
        if (hapiBroadPolicyRemoveFromAll(hapiSystem->dot1xViolationPolicyId) == L7_SUCCESS)
        {
          result = L7_SUCCESS;
        }
      }
    }
  }
  return result;
}

/*********************************************************************
 *
 * @purpose  802.1x : Install the violation policy for a port.
 *
 * @param   *usp           @b{(input)} The USP of the port that is to be acted upon
 * @param   *dapi_g        @b{(input)} The driver object
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments Should only be called if an update is necessary.
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadDot1xViolationPolicyInstall(DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  BROAD_PORT_t            *hapiPortPtr;
  BROAD_SYSTEM_t          *hapiSystem;
  L7_RC_t                  result = L7_FAILURE;

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (hapiSystem->dot1xViolationPolicyId != BROAD_POLICY_INVALID)
  {
    if ((hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED) && 
        (hapiPortPtr->dot1x.violationCallbackEnabled == L7_TRUE))
    {
      if (hapiBroadPolicyApplyToIface(hapiSystem->dot1xViolationPolicyId, hapiPortPtr->bcmx_lport) == L7_SUCCESS)
      {
        result = L7_SUCCESS;
      }
    }
    else
    {
      if (hapiBroadPolicyRemoveFromIface(hapiSystem->dot1xViolationPolicyId, hapiPortPtr->bcmx_lport) == L7_SUCCESS)
      {
        result = L7_SUCCESS;
      }
    }
  }

  return result;
}

/*********************************************************************
 *
 * @purpose  802.1x : remove an unauthorized port from all vlans under the covers 
 *           to not allow any traffic to egress on teh port while it is unauthorized.
 *
 * @param   *usp           @b{(input)} The USP of the port that is to be acted upon
 * @param   *dapi_g        @b{(input)} The driver object
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments this step is required since we no longer set the spanning tree mode of an
 *           unauthorized port to discarding in order to support guest vlan on mac-based 
 *           ports 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadDot1xPortVlanRemove(DAPI_USP_t *usp, DAPI_t *dapi_g)
{
   BROAD_PORT_t *hapiPortPtr;
   bcmx_lport_t  lport;
   L7_RC_t       result;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  hapiBroadAddRemovePortFromVlans (usp,
                                   L7_FALSE,
                                   dapi_g);

  /* Disable ingress filtering on the port */
  
   lport = hapiPortPtr->bcmx_lport;
   result = hapiBroadVlanIngressFilterSet(lport,L7_FALSE);

   return result;
}

/*********************************************************************
 *
 * @purpose  802.1x : restore a authorized port to all vlans of which it was a member 
 *                   under the covers .Reserve the opration performed in 
 *                   hapiBroadDot1xPortVlanRemove once port is authorized. 
 *
 * @param   *usp           @b{(input)} The USP of the port that is to be acted upon
 * @param   *dapi_g        @b{(input)} The driver object
 *
 * @returns  L7_SUCCESS
 * @returns  L7_FAILURE
 *
 * @comments this step is required since we no longer set the spanning tree mode of an
 *           unauthorized port to discarding in order to support guest vlan on mac-based 
 *           ports. 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadDot1xPortVlanReset(DAPI_USP_t *usp, DAPI_t *dapi_g)
{
  BROAD_PORT_t *hapiPortPtr;
  bcmx_lport_t  lport;
  L7_RC_t       result;

 hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

 hapiBroadAddRemovePortFromVlans (usp,
                                  L7_TRUE,
                                  dapi_g);

 /*Reset Ingress Filtering on the port*/
 lport = hapiPortPtr->bcmx_lport;
 result = hapiBroadVlanIngressFilterSet(lport,hapiPortPtr->ingressFilteringEnabled);
 return result;
}
/*********************************************************************
*
* @purpose  802.1x : Set the authorization status of the specified usp
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOT1X_STATUS
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dot1xStatus
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1xStatus(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *dapiPortPtr;
  L7_DOT1X_PORT_STATUS_t   status;
  BROAD_L2ADDR_FLUSH_t     l2addr_port;
  BROAD_SYSTEM_t        *hapiSystemPtr;
  int                      rv;

  if (dapiCmd->cmdData.dot1xStatus.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  /* This function is used by the application to specify the
   *  authorization status of a port. This function has to set
   *  the port into forward or discard for all spanning tree
   *  instances that this port belongs to based on the status
   *  being authorized or unauthorized.
   */

  status      = dapiCmd->cmdData.dot1xStatus.status;

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ))
  {
    return( L7_FAILURE );
  }

  /*
   * protect the VLAN database in case any other components are accessing the data
   * CAUTION: early returns must exit the crit sec
   */
   hapiBroadVlanCritSecEnter();
  
   hapiBroadDot1xSemTake();

  if (status != hapiPortPtr->dot1x.dot1xStatus)
  {
    /* Store the status */
    hapiPortPtr->dot1x.dot1xStatus = status;
    hapiPortPtr->dot1x.violationCallbackEnabled = dapiCmd->cmdData.dot1xStatus.violationCallbackEnabled;
    hapiPortPtr->dot1x.authorizedClientCount = 0;

    /* Assumes that DAPI_CMD_SYSTEM_DOT1X_CONFIG is issued before DAPI_CMD_INTF_DOT1X_STATUS
    ** when dot1x is enabled and the order is reversed when dot1x is disabled.
    */
    hapiSystemPtr = (BROAD_SYSTEM_t *)(dapi_g->system->hapiSystem);
    if (hapiSystemPtr->dot1xMode == L7_FALSE) 
    {
      hapiBroadDot1xSemGive();
      hapiBroadVlanCritSecExit();
      return result;
    }

    rv = usl_bcmx_port_dot1x_config(hapiPortPtr->bcmx_lport, status);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      LOG_ERROR(rv);    
    }

    hapiBroadDot1xAuthMacUpdate(hapiPortPtr, dapi_g);
   
    if (status == L7_DOT1X_PORT_STATUS_UNAUTHORIZED)
    {
      hapiBroadIntfDot1xAuthModeUpdate(usp, hapiPortPtr, dapi_g);
     
      /* Flush FDB */
      l2addr_port.vlanID = 0;
      l2addr_port.bcmx_lport  = 0;
      l2addr_port.tgid = 0;
      l2addr_port.flushtype = BROAD_FLUSH_BY_PORT;
      l2addr_port.bcmx_lport = hapiPortPtr->bcmx_lport;
      l2addr_port.port_is_lag = L7_FALSE;

      hapiBroadL2FlushRequest(l2addr_port);

      /* remove unauthorized port from all vlans in teh hardware to prevent egress traffic on unauthorized ports*/
      hapiBroadDot1xPortVlanRemove(usp,dapi_g);
    }
    else
    {
      /* Purge any clients that were authorized for this port. */
      hapiBroadDot1xClientsPurge(usp, dapi_g);

      /*Restore vlan assignment and ingress filtering for the port */
       hapiBroadDot1xPortVlanReset(usp,dapi_g);

      /* Restore PML configuration */
      if (hapiPortPtr->locked)
      {
         hapiBroadLearnSet(usp, BCM_PORT_LEARN_CPU, dapi_g);
      }
      else
      {
		  /* enable learning on this port */
		  if (hpcSoftwareLearningEnabled () == L7_TRUE)
              hapiBroadLearnSet(usp, (BCM_PORT_LEARN_CPU | BCM_PORT_LEARN_FWD), dapi_g);
          else
              hapiBroadLearnSet(usp, (BCM_PORT_LEARN_ARL | BCM_PORT_LEARN_FWD), dapi_g);
      }
    }

    /* Install violation policy */
    hapiBroadDot1xViolationPolicyInstall(usp, dapi_g);
  }
  else if ((hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_UNAUTHORIZED) &&
           (hapiPortPtr->dot1x.violationCallbackEnabled != dapiCmd->cmdData.dot1xStatus.violationCallbackEnabled))
  {
    hapiPortPtr->dot1x.violationCallbackEnabled = dapiCmd->cmdData.dot1xStatus.violationCallbackEnabled;

    /* Install violation policy */
    hapiBroadDot1xViolationPolicyInstall(usp, dapi_g);
  }

  hapiBroadDot1xSemGive();
  hapiBroadVlanCritSecExit();

  return result;
}

/*********************************************************************
*
* @purpose  802.1x : Authorize the specified client and possible apply
*                    a policy for that client.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOT1X_CLIENT_ADD
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dot1xClientAdd
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1xClientAdd(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result       = L7_SUCCESS;
  L7_RC_t                  l2FdbResult  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *dapiPortPtr;
  HAPI_DOT1X_CLIENT_t     *pClient;
  BROAD_POLICY_RULE_t      ruleId;
  L7_uchar8                taggedPktFormat[]   = {BROAD_VLAN_FORMAT_STAG};
  L7_uchar8                untaggedPktFormat[] = {BROAD_VLAN_FORMAT_UNTAG};
  L7_uchar8                exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                            FIELD_MASK_NONE, FIELD_MASK_NONE};
  L7_uint32                tlvSignature;
  L7_uint32                availableIndex;
  L7_uint32                i;
  L7_ushort16              vlan_id;
  DAPI_QVLAN_MGMT_CMD_t    tempDapiCmd;

  L7_BOOL                  timeout;
  L7_uchar8                macStr[20];

  if (dapiCmd->cmdData.dot1xClientAdd.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ))
  {
    return( L7_FAILURE );
  }

   if(hapiVoiceVlanDebug)
   {
      l7utilsMacAddrHexToString(dapiCmd->cmdData.dot1xClientAdd.macAddr.addr, 20, macStr);
      sysapiPrintf("hapiBroadIntfDot1xClientAdd:In client add function:mac:[%s] vlan[%d]\r\n",macStr,
                    dapiCmd->cmdData.dot1xClientAdd.vlanId);
   }

  /*
   * protect the VLAN database in case any other components are accessing the data
   * CAUTION: early returns must exit the crit sec
   */
   hapiBroadVlanCritSecEnter();
   hapiBroadDot1xSemTake();
   if (hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)
   {
     hapiBroadDot1xSemGive();
     hapiBroadVlanCritSecExit();
     if(hapiVoiceVlanDebug)
       sysapiPrintf("ClientAdd: Port Authorized!\n\r");
     return( L7_FAILURE );
   }

   /* Find an available entry. If the entry exists, return a failure. */
   availableIndex = -1;
   for (i = 0; i < L7_DOT1X_PORT_MAX_MAC_USERS; i++)
   {
     if (hapiPortPtr->dot1x.client[i].inUse == L7_FALSE)
     {
       /* Find first free entry */
       if (availableIndex == -1)
       {
         availableIndex = i;
       }
     }
     else
     {
       /* Return failure if entry already exists */
       if (memcmp(hapiPortPtr->dot1x.client[i].macAddr.addr,
                 dapiCmd->cmdData.dot1xClientAdd.macAddr.addr,
                 sizeof(L7_enetMacAddr_t)) == 0)
       {
         hapiBroadDot1xSemGive();
         hapiBroadVlanCritSecExit();
         if(hapiVoiceVlanDebug)
           sysapiPrintf("ClientAdd: Entry Exists!\n\r");
         return L7_FAILURE;
       }
     }
   }

   /* Return failure if no available entries */
   if (availableIndex == -1)
   {
     hapiBroadDot1xSemGive();
     hapiBroadVlanCritSecExit();
     return L7_FAILURE;
   }

   pClient = &hapiPortPtr->dot1x.client[availableIndex];

   pClient->inUse = L7_TRUE;
   memcpy(pClient->macAddr.addr, dapiCmd->cmdData.dot1xClientAdd.macAddr.addr, sizeof(L7_enetMacAddr_t));
   pClient->vid   = dapiCmd->cmdData.dot1xClientAdd.vlanId;

   hapiPortPtr->dot1x.authorizedClientCount++;

   if (hapiPortPtr->dot1x.authorizedClientCount == 1)
   {
     hapiBroadDot1xPortVlanReset(usp,dapi_g);
   }
   /* If the application indicates that this client needs to have a policy associated to it,
      then install the policy using Policy Manager. */
   pClient->policyId = BROAD_POLICY_INVALID;

   if (hapiBroadDot1xPolicySupported())
   {
     if (dapiCmd->cmdData.dot1xClientAdd.pTLV != L7_NULL)
     {  
       /* See if any other clients on this port are using this policy. If so,
          just re-use the policy. */
       tlvSignature = hapiBroadDot1xPolicyTLVHash(dapiCmd->cmdData.dot1xClientAdd.pTLV);
       for (i = 0; i < L7_DOT1X_PORT_MAX_MAC_USERS; i++)
       {
         if (i == availableIndex)
         {
           continue;
         }
         if ((hapiPortPtr->dot1x.client[i].inUse == L7_TRUE) && 
             (hapiPortPtr->dot1x.client[i].policyId != BROAD_POLICY_INVALID))
         {
           if (tlvSignature == hapiPortPtr->dot1x.client[i].tlvSignature)
           {
             break;
           }
         }
       }

       if (i == L7_DOT1X_PORT_MAX_MAC_USERS)
       { 
         pClient->tlvSignature = tlvSignature;

         /* No other clients are using this policy, so allocate a new group ID to be used in
            the policy and the L2FDB entries. */
         if (hapiBroadDot1xGroupIdAllocate(&hapiPortPtr->dot1x.groupMask, &pClient->groupId) == L7_SUCCESS)
         {
           /* Now create the policy */
           if (hapiBroadDot1xPolicyCreate(usp, 
                                          dapiCmd->cmdData.dot1xClientAdd.pTLV, 
                                          pClient->groupId, 
                                          &pClient->policyId, 
                                          dapi_g) != L7_SUCCESS)
           {
             L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                     "\n%s %d: In %s Couldn't Create dot1x policy\n",
                     __FILE__, __LINE__, __FUNCTION__);
             hapiBroadDot1xGroupIdFree(&hapiPortPtr->dot1x.groupMask, pClient->groupId);
             pClient->policyId     = BROAD_POLICY_INVALID;
             pClient->groupId      = HAPI_DOT1X_GROUP_ID_INVALID;
             pClient->tlvSignature = 0;

             result = L7_FAILURE;
           }
         }
         else
         {
           L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                   "\n%s %d: In %s Couldn't allocate group ID\n",
                   __FILE__, __LINE__, __FUNCTION__);
           result = L7_FAILURE;
         }
       }
       else
       {
         /* re-use the policy already in place */
         pClient->policyId     = hapiPortPtr->dot1x.client[i].policyId;
         pClient->groupId      = hapiPortPtr->dot1x.client[i].groupId;
         pClient->tlvSignature = hapiPortPtr->dot1x.client[i].tlvSignature;
       }
     }
   }
    
   /* Add the MAC address to L2FDB. Need to add an entry for every VLAN this port belongs to. 
      If the application indicates that this client is being assigned to a VLAN, then we
      can get away w/ adding just a single entry. */
   if (hapiBroadDot1xVlanAssignmentSupported() && (dapiCmd->cmdData.dot1xClientAdd.vlanId != 0))
   {
    
     /* Client only needs to be authorized on one VLAN. */
     /* Remove the client mac address associated with any other vlan, if present from the L2 table */
     for (vlan_id = 1; vlan_id <= L7_PLATFORM_MAX_VLAN_ID; vlan_id++)
     {
       if (BROAD_IS_VLAN_MEMBER(usp, vlan_id, dapi_g))
       {
         hapiBroadDot1xMacAddrDelete(pClient->macAddr,
                                     vlan_id,    
                                     dapi_g);        
       
       }
     }

     if(hapiVoiceVlanDebug)
     {
       l7utilsMacAddrHexToString(dapiCmd->cmdData.dot1xClientAdd.macAddr.addr, 20, macStr);
       sysapiPrintf("hapiBroadIntfDot1xClientAdd:ClientAdd:mac:[%s] vlan[%d]\r\n",macStr,
                    dapiCmd->cmdData.dot1xClientAdd.vlanId);
     }
     /* Add client Mac address with application specified vlan to the L2 table*/
     if (hapiBroadDot1xMacAddrAdd(usp, 
                                  dapiCmd->cmdData.dot1xClientAdd.macAddr,
                                  dapiCmd->cmdData.dot1xClientAdd.vlanId, 
                                  pClient->groupId,
                                  dapi_g) != L7_SUCCESS)
     {
       l2FdbResult = L7_FAILURE;
     }

     /* check client timeout inorder to reset the HIT_SRC bit , so that if client is absent after 5 mins, client details are removed from the application*/
     hapiBroadDot1xMacAddrTimeoutCheck(hapiPortPtr->bcmx_lport,
                                       pClient->macAddr,
                                       pClient->vid,  
                                       &timeout,
                                       dapi_g); 
   
   }
   else
   {
     if(hapiVoiceVlanDebug)
     {
       l7utilsMacAddrHexToString(dapiCmd->cmdData.dot1xClientAdd.macAddr.addr, 20, macStr);
       sysapiPrintf("hapiBroadIntfDot1xClientAdd:ClientAdd:mac:[%s] vlans[",macStr);
     }

     /* Client needs to be authorized on all VLANs that this port is a member of. */
     for (vlan_id = 1; vlan_id <= L7_PLATFORM_MAX_VLAN_ID; vlan_id++)
     {
       if (BROAD_IS_VLAN_MEMBER(usp, vlan_id, dapi_g))
       {
         if(hapiVoiceVlanDebug)
           sysapiPrintf("%d ",vlan_id); /* Remove */
         if (hapiBroadDot1xMacAddrAdd(usp, 
                                      dapiCmd->cmdData.dot1xClientAdd.macAddr,
                                      vlan_id, 
                                      pClient->groupId,
                                      dapi_g) != L7_SUCCESS)
         {
           l2FdbResult = L7_FAILURE;
         }

         /* check client timeout inorder to reset the HIT_SRC bit , so that if client is absent after 5 mins, client details are removed from the application*/
         hapiBroadDot1xMacAddrTimeoutCheck(hapiPortPtr->bcmx_lport,
                                           pClient->macAddr,
                                           vlan_id,  
                                           &timeout,
                                           dapi_g); 
       }
     }
     if(hapiVoiceVlanDebug)
       sysapiPrintf("]\n\r"); /* Remove */
   }
   if (l2FdbResult == L7_FAILURE)
   {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
            "\n%s %d: In %s Couldn't add MAC address to one or more VLANs\n",
            __FILE__, __LINE__, __FUNCTION__);
     result = L7_FAILURE;
   }

   /* If the application indicates that this client is being assigned to a VLAN, then we
      need to use Policy Manager to add an entry. */
   if (hapiBroadDot1xVlanAssignmentSupported())
   {
     if (dapiCmd->cmdData.dot1xClientAdd.vlanId != 0)
     {
       if (hapiBroadDot1xVFPTableSupported()==L7_FALSE) 
       {
         if(hapiVoiceVlanDebug)
         {
           l7utilsMacAddrHexToString(dapiCmd->cmdData.dot1xClientAdd.macAddr.addr, 20, macStr);
           sysapiPrintf("hapiBroadIntfDot1xClientAdd:VFP Table not supported:mac:[%s] vlan[%d]\r\n",macStr,
                          dapiCmd->cmdData.dot1xClientAdd.vlanId);
         }
         tempDapiCmd.cmdData.macVlanConfig.getOrSet = DAPI_CMD_SET;
         memcpy(tempDapiCmd.cmdData.macVlanConfig.mac.addr, pClient->macAddr.addr, sizeof(L7_enetMacAddr_t));
         tempDapiCmd.cmdData.macVlanConfig.vlanId   = pClient->vid;
  
         if (hapiBroadL2VlanMacEntryAdd(usp, DAPI_CMD_MAC_VLAN_CREATE, &tempDapiCmd, dapi_g) != L7_SUCCESS)
         {
           L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                   "\n%s %d: In %s Couldn't create VLAN assignment policy\n",
                   __FILE__, __LINE__, __FUNCTION__);
           result = L7_FAILURE;
         }
       }
       else
       {
         /* Attempt to add new entry to HW */
         if (hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PORT) == L7_SUCCESS) 
         {
           hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_LOOKUP);

           /* We need to create two rules here because of how the VFP VLAN action works.
             'Adding' a VLAN only works on untagged and single tagged packets.
             'Replacing' a VLAN  only works on single and double tagged packets.
              So we need to add a rule to 'Add' a VLAN for untagged packets and
              another rule to 'Replace' the VLAN for tagged packets. */
           hapiBroadPolicyRuleAdd(&ruleId);
           hapiBroadPolicyRuleQualifierAdd(ruleId, 
                                           BROAD_FIELD_MACSA, 
                                           dapiCmd->cmdData.dot1xClientAdd.macAddr.addr, 
                                           exact_match); 
           hapiBroadPolicyRuleQualifierAdd(ruleId, 
                                           BROAD_FIELD_VLAN_FORMAT, 
                                           untaggedPktFormat, 
                                           exact_match);
           hapiBroadPolicyRuleActionAdd(ruleId,
                                        BROAD_ACTION_ADD_OUTER_VID,
                                        dapiCmd->cmdData.dot1xClientAdd.vlanId, 0, 0);

           hapiBroadPolicyRuleAdd(&ruleId);
           hapiBroadPolicyRuleQualifierAdd(ruleId, 
                                           BROAD_FIELD_MACSA, 
                                           dapiCmd->cmdData.dot1xClientAdd.macAddr.addr, 
                                           exact_match); 
           hapiBroadPolicyRuleQualifierAdd(ruleId, 
                                           BROAD_FIELD_VLAN_FORMAT, 
                                           taggedPktFormat, 
                                           exact_match);
           hapiBroadPolicyRuleActionAdd(ruleId,
                                        BROAD_ACTION_SET_OUTER_VID,
                                        dapiCmd->cmdData.dot1xClientAdd.vlanId, 0, 0);

           if (hapiBroadPolicyCommit(&pClient->vlanAssignmentPolicyId) == L7_SUCCESS)
           {
             if (hapiBroadPolicyApplyToIface(pClient->vlanAssignmentPolicyId, hapiPortPtr->bcmx_lport) != L7_SUCCESS)
             {
               L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                       "\n%s %d: In %s Couldn't apply policy to interface\n",
                       __FILE__, __LINE__, __FUNCTION__);
               hapiBroadPolicyDelete(pClient->vlanAssignmentPolicyId);
               pClient->vlanAssignmentPolicyId = BROAD_POLICY_INVALID;
               result = L7_FAILURE;
             }
           }
           else
           {
             L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                    "\n%s %d: In %s Couldn't commit policy\n",
                    __FILE__, __LINE__, __FUNCTION__);
             hapiBroadPolicyDelete(pClient->vlanAssignmentPolicyId);
             pClient->vlanAssignmentPolicyId = BROAD_POLICY_INVALID;
             result = L7_FAILURE;
           }
         }
         else
         {
           L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DRIVER_COMPONENT_ID,
                   "\n%s %d: In %s Couldn't create VLAN assignment policy\n",
                     __FILE__, __LINE__, __FUNCTION__);
           result = L7_FAILURE;
         }
       }/*VFP Table Supported*/
     }
   }

   if (hapiPortPtr->dot1x.authorizedClientCount == 1)
   {
     hapiBroadDot1xAuthMacUpdate(hapiPortPtr, dapi_g);
   }
  
   hapiBroadDot1xSemGive();
   hapiBroadVlanCritSecExit();
   return result;
}

/*********************************************************************
*
* @purpose  802.1x : Unauthorize the specified client and remove applicable
*                    policies for that client.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOT1X_CLIENT_REMOVE
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dot1xClientRemove
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Gets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1xClientRemove(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *dapiPortPtr;
  HAPI_DOT1X_CLIENT_t     *pClient;
  L7_uint32                i;
  L7_uint32                clientIndex = 0;

  if (dapiCmd->cmdData.dot1xClientRemove.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ))
  {
    return( L7_FAILURE );
  }

  /*
   * protect the VLAN database in case any other components are accessing the data
   * CAUTION: early returns must exit the crit sec
   */
   hapiBroadVlanCritSecEnter();
  hapiBroadDot1xSemTake();
  if (hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)
  {
    hapiBroadDot1xSemGive();
    hapiBroadVlanCritSecExit();
    if (hapiVoiceVlanDebug)
      sysapiPrintf("ClientRemove: Port Authorized!\n\r");
    return( L7_FAILURE );
  }

  /* Find the entry. If the entry doesn't exist, return a failure. */
  for (i = 0; i < L7_DOT1X_PORT_MAX_MAC_USERS; i++)
  {
    if (hapiPortPtr->dot1x.client[i].inUse == L7_TRUE)
    {
      if (memcmp(hapiPortPtr->dot1x.client[i].macAddr.addr,
                 dapiCmd->cmdData.dot1xClientRemove.macAddr.addr,
                 sizeof(L7_enetMacAddr_t)) == 0)
      {
        clientIndex = i;
        break;
      }
    }
  }

  if (i == L7_DOT1X_PORT_MAX_MAC_USERS)
  {
    hapiBroadDot1xSemGive();
    hapiBroadVlanCritSecExit();
    if (hapiVoiceVlanDebug)
      sysapiPrintf("ClientRemove: Entry Not Found!\n\r");
    return L7_FAILURE;
  }

  result = hapiBroadDot1xClientHwDelete(usp, clientIndex, dapi_g);

  pClient = &hapiPortPtr->dot1x.client[clientIndex];
  pClient->inUse                  = L7_FALSE;
  memset(pClient->macAddr.addr, 0, sizeof(L7_enetMacAddr_t));
  pClient->vid                    = 0;
  pClient->policyId               = BROAD_POLICY_INVALID;
  pClient->groupId                = HAPI_DOT1X_GROUP_ID_INVALID;
  pClient->tlvSignature           = 0;
  if (hapiBroadDot1xVFPTableSupported()==L7_TRUE)
  {
    pClient->vlanAssignmentPolicyId  = BROAD_POLICY_INVALID;
  }

  hapiPortPtr->dot1x.authorizedClientCount--;
  if (hapiPortPtr->dot1x.authorizedClientCount == 0)
  {
    hapiBroadDot1xAuthMacUpdate(hapiPortPtr, dapi_g);
    hapiBroadDot1xPortVlanRemove(usp,dapi_g);
  }
  
  hapiBroadDot1xSemGive();
  hapiBroadVlanCritSecExit();

  return result;
}

/*********************************************************************
* @purpose  Checks if specified MAC/VLAN pair in the L2FDB has timed out.
*
* @param    
* @param    
*
* @returns  L7_RC_t
*
* @notes    
*       
* @end
*********************************************************************/
L7_RC_t hapiBroadDot1xMacAddrTimeoutCheck(bcmx_lport_t port,L7_enetMacAddr_t macAddr, L7_ushort16 vid,L7_BOOL *timedout, DAPI_t *dapi_g)
{
    usl_bcm_port_dot1x_client_t clientCmd;
    L7_int32 rv;

    *timedout = L7_FALSE;
    memset(&clientCmd,0,sizeof(usl_bcm_port_dot1x_client_t));
    memcpy(clientCmd.mac_addr,macAddr.addr,L7_ENET_MAC_ADDR_LEN);
    clientCmd.vlan_id = vid;
    clientCmd.timedout_flg = L7_FALSE;

   rv= usl_bcmx_port_dot1x_client_timeout_get(port,&clientCmd);
   if (L7_BCMX_OK(rv)==L7_TRUE) 
   {
       *timedout = clientCmd.timedout_flg;
       return L7_SUCCESS;
   }
   else
       return L7_FAILURE;

}

/*********************************************************************
*
* @purpose  802.1x : Check if client has timeout i.e. check if the BCM_SRC_HIT flag 
*                    has been set for that client MAC Address
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOT1X_CLIENT_TIMEOUT
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dot1xClientTimeout
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments Sets are not supported
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1xClientTimeout(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  L7_RC_t                  rc;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *dapiPortPtr;
  HAPI_DOT1X_CLIENT_t     *pClient;
  L7_uint32                i;
  L7_uint32                clientIndex = 0;
  L7_BOOL                  timeout;
  L7_BOOL                  all_timeout;
  L7_ushort16              vlanId;

  if (dapiCmd->cmdData.dot1xClientRemove.getOrSet != DAPI_CMD_GET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ))
  {
    return( L7_FAILURE );
  }

  if (dapiCmd->cmdData.dot1xClientTimeout.timeout  == L7_NULL)
  {
    return L7_FAILURE;
  }

  hapiBroadDot1xSemTake();
  if (hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)
  {
    hapiBroadDot1xSemGive();
    return( L7_FAILURE );
  }

  /* Find the entry. If the entry doesn't exist, return a failure. */
  for (i = 0; i < L7_DOT1X_PORT_MAX_MAC_USERS; i++)
  {
    if (hapiPortPtr->dot1x.client[i].inUse == L7_TRUE)
    {
      if (memcmp(hapiPortPtr->dot1x.client[i].macAddr.addr,
                 dapiCmd->cmdData.dot1xClientRemove.macAddr.addr,
                 sizeof(L7_enetMacAddr_t)) == 0)
      {
        clientIndex = i;
        break;
      }
    }
  }

  if (i == L7_DOT1X_PORT_MAX_MAC_USERS)
  {
    hapiBroadDot1xSemGive();
    return L7_FAILURE;
  }

 pClient = &hapiPortPtr->dot1x.client[clientIndex];
 if (pClient->vid != 0)
  {
    /* This client was authorized on only one VLAN. */
    rc = hapiBroadDot1xMacAddrTimeoutCheck(hapiPortPtr->bcmx_lport,
                                           pClient->macAddr,
                                           pClient->vid,  
                                           &timeout,
                                           dapi_g);         
    if ((rc == L7_SUCCESS) || (rc == L7_NOT_EXIST))
    {
        *dapiCmd->cmdData.dot1xClientTimeout.timeout = timeout;
        result = L7_SUCCESS;
    }
    else
    {
        result = L7_FAILURE;
    }
  }
  else
  {
    /* This client was authorized on all VLANs. */
    all_timeout = L7_TRUE;
    for (vlanId = 1; vlanId <= L7_PLATFORM_MAX_VLAN_ID; vlanId++)
    {
      if (BROAD_IS_VLAN_MEMBER(usp, vlanId, dapi_g))
      {
          timeout = L7_FALSE;
          rc =hapiBroadDot1xMacAddrTimeoutCheck(hapiPortPtr->bcmx_lport,
                                              pClient->macAddr,
                                              vlanId,
                                              &timeout,
                                              dapi_g);         
          if (timeout == L7_FALSE) {
              all_timeout = L7_FALSE;
          }

       
      }
    }
    /* timeout the client only if Mac adddress is timed out on every vlan*/
    /* mac address not present for any vlan combination..client has timed out*/
    *dapiCmd->cmdData.dot1xClientTimeout.timeout = all_timeout;
    result = L7_SUCCESS;

  }


  hapiBroadDot1xSemGive();

  return result;
}

/*********************************************************************
*
* @purpose  802.1x : Configure HW to drop packets from specified MAC/VLAN pair
*                    on the specified interface.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOT1X_CLIENT_BLOCK
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dot1xClientBlock
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1xClientBlock(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *dapiPortPtr;
  usl_bcm_port_dot1x_client_t clientCmd;


  if (dapiCmd->cmdData.dot1xClientBlock.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ))
  {
    return( L7_FAILURE );
  }

  hapiBroadDot1xSemTake();
  if (hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)
  {
    hapiBroadDot1xSemGive();
    return( L7_FAILURE );
  }

  /* Add an entry for this address to the L2X table to prevent the CPU from
     getting further traffic from this station. The dot1x application will
     ultimately add a client entry for this 'guest' or the entry will age out. */
  memset(&clientCmd,0,sizeof(usl_bcm_port_dot1x_client_t));
  memcpy(clientCmd.mac_addr, dapiCmd->cmdData.dot1xClientBlock.macAddr.addr, L7_ENET_MAC_ADDR_LEN);
  clientCmd.vlan_id = dapiCmd->cmdData.dot1xClientBlock.vlanId;

  if (usl_bcmx_port_dot1x_client_block(hapiPortPtr->bcmx_lport, &clientCmd) == BCM_E_NONE)
  {
    result = L7_SUCCESS;
  }
  else
  {
    result = L7_FAILURE;
  }

  hapiBroadDot1xSemGive();

  return result;
}

/*********************************************************************
*
* @purpose  802.1x : Configure HW to allow packets from specified MAC/VLAN pair
*                    on the specified interface.
*
* @param   *usp           @b{(input)} The USP of the port that is to be acted upon
* @param    cmd           @b{(input)} DAPI_CMD_INTF_DOT1X_CLIENT_UNBLOCK
* @param   *data          @b{(input)} DAPI_INTF_MGMT_CMD_t.cmdData.dot1xClientUnblock
* @param   *dapi_g        @b{(input)} The driver object
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadIntfDot1xClientUnblock(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                  result  = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd = (DAPI_INTF_MGMT_CMD_t*)data;
  BROAD_PORT_t            *hapiPortPtr;
  DAPI_PORT_t             *dapiPortPtr;
  usl_bcm_port_dot1x_client_t clientCmd;


  if (dapiCmd->cmdData.dot1xClientUnblock.getOrSet != DAPI_CMD_SET)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s invalid getOrSet Command\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);

  if (L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ))
  {
    return( L7_FAILURE );
  }

  hapiBroadDot1xSemTake();
  if (hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED)
  {
    hapiBroadDot1xSemGive();
    return( L7_FAILURE );
  }

  /* Remove the entry for this address from the L2X table to allow the CPU to
     get violations from this station. */
  memset(&clientCmd, 0, sizeof(clientCmd));
  memcpy(clientCmd.mac_addr, dapiCmd->cmdData.dot1xClientUnblock.macAddr.addr, L7_ENET_MAC_ADDR_LEN);
  clientCmd.vlan_id = dapiCmd->cmdData.dot1xClientUnblock.vlanId;

  if (usl_bcmx_port_dot1x_client_unblock(hapiPortPtr->bcmx_lport, &clientCmd) == BCM_E_NONE)
  {
    result = L7_SUCCESS;
  }
  else
  {
    result = L7_FAILURE;
  }

  hapiBroadDot1xSemGive();

  return result;
}

void hapiBroadDot1xAuthMacUpdate(BROAD_PORT_t *hapiPortPtr, DAPI_t *dapi_g)
{
  uint8                    mac_zero[6]={0x00,0x00,0x00,0x00,0x00,0x00};
  BROAD_SYSTEM_t          *hapiSystemPtr;

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  /* In 53115 platforms EAPOL packets are identified only by 
     destination mac address and not by type field. Becaue of 
     this EAPOL Frames having Reserved multicast addresses in DA
     filed are only received by the CPU. To receive even the unicast
     packets of EAP type we are populating the EAP configuration 
     registed with the unicast address. */
  if(hapiBroadRoboVariantCheck() == __BROADCOM_53115_ID)
  {
    if (HAPI_DOT1X_PORT_IS_AUTHORIZED(hapiPortPtr))
    {
      bcmx_auth_mac_delete(hapiPortPtr->bcmx_lport,mac_zero);
    }
    else
    {
      bcmx_auth_mac_add (hapiPortPtr->bcmx_lport,hapiSystemPtr->bridgeMacAddr.addr);
    }
  }
}

/*********************************************************************
*
* Debug
*
*********************************************************************/
extern DAPI_t *dapi_g;
void hapiBroadDebugDot1xIntf(L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  DAPI_USP_t    usp;
  BROAD_PORT_t *hapiPortPtr;
  L7_uint32     i;

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;
  if (isValidUsp(&usp,dapi_g) == L7_FALSE)
  {
    sysapiPrintf("Invalid parameters! unit:[%d] slot:[%d] port:[%d]\n\r",unit,slot,port);
    return;
  }


  hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);

  SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                 "dot1xStatus: %s\n",
                 (hapiPortPtr->dot1x.dot1xStatus == L7_DOT1X_PORT_STATUS_AUTHORIZED ? "Authorized" : "Unauthorized"));
  SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                 "authorizedClientCount: %d\n",
                 hapiPortPtr->dot1x.authorizedClientCount);
  SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                 "violationCallbackEnabled: %s\n",
                 hapiPortPtr->dot1x.violationCallbackEnabled ? "TRUE":"FALSE");
  SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                 "groupMask: ");
  for (i = 0; i < HAPI_DOT1X_GROUP_INDICES; i++)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "0x%2.2x ", hapiPortPtr->dot1x.groupMask.value[i]);
  }
  SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                 "\n");

  for (i = 0; i < L7_DOT1X_PORT_MAX_MAC_USERS; i++)
  {
    if (hapiPortPtr->dot1x.client[i].inUse == L7_TRUE)
    {
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "Index: %d\n", i);
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "  MAC: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
                     hapiPortPtr->dot1x.client[i].macAddr.addr[0],
                     hapiPortPtr->dot1x.client[i].macAddr.addr[1],
                     hapiPortPtr->dot1x.client[i].macAddr.addr[2],
                     hapiPortPtr->dot1x.client[i].macAddr.addr[3],
                     hapiPortPtr->dot1x.client[i].macAddr.addr[4],
                     hapiPortPtr->dot1x.client[i].macAddr.addr[5]);
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "  vid: %d\n",
                     hapiPortPtr->dot1x.client[i].vid);
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "  policyId: %d\n",
                     hapiPortPtr->dot1x.client[i].policyId);
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "  groupId: %d\n",
                     hapiPortPtr->dot1x.client[i].groupId);
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "  tlvSignature: 0x%8.8x\n",
                     hapiPortPtr->dot1x.client[i].tlvSignature);
      if (hapiBroadDot1xVFPTableSupported()==L7_TRUE)
      {
           SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                          "  vlanAssignmentPolicyId: %d\n",
                          hapiPortPtr->dot1x.client[i].vlanAssignmentPolicyId);
      }
    }
  }
}

void hapiBroadDebugDot1xIntfVlanRemove(L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
 DAPI_USP_t    usp;
 BROAD_PORT_t *hapiPortPtr;
 bcmx_lport_t  lport;
 L7_RC_t       result;
  
  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;
  if (isValidUsp(&usp,dapi_g) == L7_FALSE)
  {
    sysapiPrintf("Invalid parameters! unit:[%d] slot:[%d] port:[%d]\n\r",unit,slot,port);
    return;
  }


  hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);

  hapiBroadAddRemovePortFromVlans (&usp,
                                   L7_FALSE,
                                   dapi_g);

  /* Disable ingress filtering on the port */
  
   lport = hapiPortPtr->bcmx_lport;
   result = hapiBroadVlanIngressFilterSet(lport,L7_FALSE);
}

void hapiBroadDebugDot1xIntfVlanReset(L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  DAPI_USP_t    usp;
  BROAD_PORT_t *hapiPortPtr;
  bcmx_lport_t  lport;
  L7_RC_t       result;

  usp.unit = unit;
  usp.slot = slot;
  usp.port = port;
  if (isValidUsp(&usp,dapi_g) == L7_FALSE)
  {
    sysapiPrintf("Invalid parameters! unit:[%d] slot:[%d] port:[%d]\n\r",unit,slot,port);
    return;
  }


  hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);

  hapiBroadAddRemovePortFromVlans (&usp,
                                   L7_TRUE,
                                   dapi_g);

  /*Reset Ingress Filtering on the port*/
   lport = hapiPortPtr->bcmx_lport;
   result = hapiBroadVlanIngressFilterSet(lport,hapiPortPtr->ingressFilteringEnabled);
}
