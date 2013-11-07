/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename  broad_llpf.c
*
* @purpose   This file contains the hapi interface for LLPF Configuration
*
* @component hapi
*
* @comments
*
* @create    10/15/2009
*
* @author    Vijayanand K(kvijayan)
*
* @end
*
**********************************************************************/


#include "flex.h"

#ifdef L7_LLPF_PACKAGE
/* Include this file only when LLPF package is present */
#include "broad_common.h"
#include "broad_policy.h"
#include "broad_llpf.h"
#include "broad_l2_lag.h"


/* Trace flags for LLPF */
L7_BOOL broadLlpfDebug = L7_FALSE; 

/* Trace Configure Options */
void hapiBroadDebugLlpfSet(L7_BOOL debugFlag)
{
  broadLlpfDebug = debugFlag;
}

/* Used to set bit correstponding to block type */
#define LLPF_PORT_BLOCK_PROTO_SET(__hapiPtr, __blockType)           \
    __hapiPtr->llpfBlockProtoEnable = (( __hapiPtr->llpfBlockProtoEnable) | (1 <<__blockType))

/* Used to clear bit correstponding to block type */
#define LLPF_PORT_BLOCK_PROTO_CLEAR( __hapiPtr, __blockType)         \
    __hapiPtr->llpfBlockProtoEnable = (( __hapiPtr->llpfBlockProtoEnable) & (~(1 <<__blockType)))

static void *hapiBroadLlpfSemaphore = L7_NULL;

/*********************************************************************
*
* @purpose Take semaphore to protect LLPF resources
*
* @param  None 
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLlpfSemTake()
{
    L7_RC_t rc;

    rc = osapiSemaTake(hapiBroadLlpfSemaphore, L7_WAIT_FOREVER);

    return rc;
}

/*********************************************************************
*
* @purpose Give semaphore to protect LLPF resources
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
L7_RC_t hapiBroadLlpfSemGive()
{
    L7_RC_t rc;

    rc = osapiSemaGive(hapiBroadLlpfSemaphore);

    return rc;
}
/*********************************************************************
*
* @purpose Updates the Llpf policy memberships for a port.
*
* @param   *usp             @b{(input)} The USP of the port that is to be acted upon
* @param   oldLlpfEnabled   @b{(input)} The previous Llpf mode for this USP
* @param   newLlpfEnabled   @b{(input)} The new Llpf mode for this USP 
* @param   policyId         @b{(input)} The policy Id corresponding to block type 
* @param   *uspKey          @b{(input)} USP key used to apply Llpf policies to this USP.
* @param   *dapi_g          @b{(input)} The driver object
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadLlpfPortUpdate(DAPI_USP_t  *portUsp,
                                       L7_BOOL      oldLlpfEnabled,
                                       L7_BOOL      newLlpfEnabled,
                                       BROAD_POLICY_t policyId,
                                       DAPI_t      *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  BROAD_PORT_t   *hapiPortPtr;
  bcmx_lport_t    lport;
  L7_uchar8       temp8;

  hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);

  lport = hapiPortPtr->bcmx_lport;

  temp8 = (oldLlpfEnabled << 4) | newLlpfEnabled;
  switch (temp8)
  {
  case 0x00:
  case 0x11:
    /* No change */
    break;

  case 0x01:
    /* Add this port to the Llpf default policy */
    rc = hapiBroadPolicyApplyToIface(policyId, lport);

    /*Debug trace*/
    HAPI_BROAD_LLPF_DEBUG(\
        "LLPF policy with id(%d) Install on port:Unit %d,slot %d port %d lport 0x%x\n" \
        ,policyId,portUsp->unit, portUsp->slot,portUsp->port,hapiPortPtr->bcmx_lport);

    break;
  case 0x10:
    /* Remove this port from the Llpf default policy */
    rc = hapiBroadPolicyRemoveFromIface(policyId, lport);

    /*Debug trace*/
    HAPI_BROAD_LLPF_DEBUG(\
        "LLPF policy with id(%d) Remove on port:Unit %d,slot %d port %d lport  0x%x\n" \
        ,policyId,portUsp->unit, portUsp->slot,portUsp->port,hapiPortPtr->bcmx_lport);

    break;
  }
 
   if (L7_SUCCESS != rc)
   {
     /*Debug trace*/
     HAPI_BROAD_LLPF_DEBUG(\
        "Failed to Install (or) Remove LLPF policy with id(%d) on port:Unit %d,slot %d port %d lport 0x%x\n" \
         ,policyId,portUsp->unit, portUsp->slot,portUsp->port,hapiPortPtr->bcmx_lport);
     /*log message */
   }
 
  return rc;
}

/*********************************************************************
*
* @purpose  Used to configure Llpf on a port.
*
* @param   *usp             @b{(input)} The USP of the port that is to be acted upon
* @param   oldLlpfEnabled   @b{(input)} The previous Llpf mode for this USP
* @param   newLlpfEnabled   @b{(input)} The new Llpf mode for this USP 
* @param   policyId         @b{(input)} The policy Id corresponding to block type 
* @param   *dapi_g          @b{(input)} The driver object
*
* @returns  void
*
* @end
*
*********************************************************************/
static L7_RC_t hapiBroadLlpfSet( DAPI_USP_t  *usp, 
                          L7_BOOL      oldLlpfEnabled,
                          L7_BOOL      newLlpfEnabled,
                          BROAD_POLICY_t policyId,
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
      rc = hapiBroadLlpfPortUpdate(usp,
                                   oldLlpfEnabled,
                                   newLlpfEnabled,
                                   policyId,
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
        rc = hapiBroadLlpfPortUpdate(&dapiPortPtr->modeparm.lag.memberSet[i].usp,
                                     oldLlpfEnabled,
                                     newLlpfEnabled,
                                     policyId,
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
 * @purpose Helper routine to Install LLPF policies 
 *
 * @param   *dapi_g          @b{(input)} system information.
 * @param   *dstMac          @b{(input)} The Destination MAC address for filtering.
 * @param   *macMask         @b{(input)} The Destination MAC address mask for filtering.
 * @param   *etherType       @b{(input)} The Ethernet Type of frame for filtering.
 * @param   *policyId        @b{(output)} The policy ID of the rules given by poilicy manager.
 *
 * @returns L7_SUCCESS
 * @returns L7_FAILURE 
 *
 * @notes  
 *
 * @end
 *
 *********************************************************************/
static L7_RC_t hapiBroadLlpfPolicyInstallProcess(DAPI_t                *dapi_g,
                                                 L7_uchar8             *dstMac,
                               	                 L7_uchar8             *macMask,
                                                 L7_ushort16            ethertype,
                                                 BROAD_POLICY_t        *policyId,
                                                 BROAD_POLICY_ACTION_t *actionArray,
                                                 L7_uint32             *parmArray,
                                                 L7_uint32              actionCount)
{
  BROAD_POLICY_RULE_t     ruleId       = BROAD_POLICY_RULE_INVALID;
  L7_uchar8               exact_match[] = {FIELD_MASK_ALL, FIELD_MASK_ALL, FIELD_MASK_ALL, 
                                           FIELD_MASK_ALL, FIELD_MASK_NONE, FIELD_MASK_NONE};
  L7_uchar8               l2FormatSnap[] = {BROAD_L2_FORMAT_SNAP};
  L7_uchar8               l2FormatSnapMask[] = {FIELD_MASK_NONE};
  BROAD_POLICY_SNAP_HEADER_t snapHeader;
  L7_uint32                  i;

  do
  {
    /* Create SYSTEM_PORT type policy for LLPF */
    if (L7_SUCCESS == hapiBroadPolicyCreate(BROAD_POLICY_TYPE_LLPF))
    {
      hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_LOOKUP);

      /* Add Rule */
      if (L7_SUCCESS != hapiBroadPolicyRuleAdd(&ruleId))
      {
        hapiBroadPolicyCreateCancel();
        break;
      }
      
      if ((dstMac != L7_NULL) && (macMask != L7_NULL))
      {
        /* Qualify MAC DA */
        if (L7_SUCCESS != hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, dstMac, macMask))
        {
          hapiBroadPolicyCreateCancel();
          break;
        }
      }
      /* Qualify Ethertype if it is not null */
      if(ethertype != L7_NULL)
      { 
        memset(&snapHeader, 0x00, sizeof(BROAD_POLICY_SNAP_HEADER_t));
        snapHeader.type = ethertype;
        if (L7_SUCCESS != hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_L2_FORMAT, 
                                                          l2FormatSnap, l2FormatSnapMask))
        {
          hapiBroadPolicyCreateCancel();
          break;
        }

        if (L7_SUCCESS != hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_SNAP,
                                                          (L7_uchar8 *)&snapHeader, exact_match))
        {
          hapiBroadPolicyCreateCancel();
          break;
        }
      }

      for (i = 0; i < actionCount; i++)
      {
        /* Action add */
        if (L7_SUCCESS != hapiBroadPolicyRuleActionAdd(ruleId, actionArray[i], parmArray[i], 0, 0))
        {
          hapiBroadPolicyCreateCancel();
          break;
        }
      }

      /* Policy commit */
      if (L7_SUCCESS == hapiBroadPolicyCommit(policyId))
      {
        return L7_SUCCESS;
      }
    }
  }while(0);

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose Notifies the Llpf component that a physical port has been
*          added to a LAG port
*
* @param   DAPI_USP_t       *portUsp @b{(input)} system information for a physical port.
* @param   DAPI_USP_t       *lagUsp  @b{(input)} system information for a PORT CHANNEL.
* @param   DAPI_t           *dapi_g  @b{(input)} system information.
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLlpfLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  BROAD_PORT_t   *hapiPortPtr;
  BROAD_PORT_t   *hapiLagPtr;
  L7_BOOL   portEnabled,lagEnabled;
  BROAD_SYSTEM_t *hapiSystem;
  L7_uint32 protocolIndex;

  hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);
  hapiLagPtr     = HAPI_PORT_GET(lagUsp,  dapi_g);

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;


  for(protocolIndex = 0;protocolIndex < L7_LLPF_BLOCK_TYPE_LAST;protocolIndex++) 
  {
    /* check if block type is valid. */
    if(hapiSystem->llpfPolicyIds[protocolIndex]== BROAD_POLICY_INVALID)
    {
      continue;
    }

    /* check if proto is set on Physical interface */
    portEnabled = LLPF_PORT_BLOCK_PROTO_ISSET(hapiPortPtr, protocolIndex);
    /* check if proto is set on Lag interface */
    lagEnabled  = LLPF_PORT_BLOCK_PROTO_ISSET(hapiLagPtr,  protocolIndex);

    /* Update corresponding policy state */
    if (protocolIndex == L7_LLPF_BLOCK_TYPE_ISDP)
    {
      /* ISDP is a special case since the ISDP component cares about ISDP packets. */
      rc = hapiBroadIsdpPortUpdate(portUsp,
                                   portEnabled,
                                   lagEnabled,
                                   hapiPortPtr->isdpEnable,
                                   hapiLagPtr->isdpEnable,
                                   dapi_g);
    }
    else
    {
      rc = hapiBroadLlpfPortUpdate(portUsp, 
                                   portEnabled,
                                   lagEnabled,
                                   hapiSystem->llpfPolicyIds[protocolIndex], 
                                   dapi_g);
    }
    if(rc!=L7_SUCCESS)
    {
     /*log*/
     /*Debug trace*/
      HAPI_BROAD_LLPF_DEBUG(\
            "Failed to update the port state during LAG Add Notify \
             Unit %d Slot:%d Port %d lport  0x%x\n" \
             ,portUsp->unit, portUsp->slot,portUsp->port,hapiPortPtr->bcmx_lport);
      continue;
    }
  }

  /* always return succcess. any failures are communicated to user through log.*/
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Notifies the Llpf component that a physical port has been
*          removed from a LAG port
*
* @param   DAPI_USP_t       *portUsp @b{(input)} system information for a physical port.
* @param   DAPI_USP_t       *lagUsp  @b{(input)} system information for a PORT CHANNEL.
* @param   DAPI_t           *dapi_g  @b{(input)} system information.
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLlpfLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g)
{
  L7_RC_t         rc = L7_SUCCESS;
  BROAD_PORT_t   *hapiPortPtr;
  BROAD_PORT_t   *hapiLagPtr;
  BROAD_SYSTEM_t *hapiSystem;
  L7_uint32 protocolIndex;
  L7_BOOL   portEnabled,lagEnabled;

  hapiPortPtr    = HAPI_PORT_GET(portUsp, dapi_g);
  hapiLagPtr     = HAPI_PORT_GET(lagUsp,  dapi_g);

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  for(protocolIndex = 0; protocolIndex < L7_LLPF_BLOCK_TYPE_LAST; protocolIndex++) 
  {
    /* check if block type is valid. */
    if(hapiSystem->llpfPolicyIds[protocolIndex]== BROAD_POLICY_INVALID)
    {
      continue;
    }

    /* check if proto is set on Physical interface */
    portEnabled = LLPF_PORT_BLOCK_PROTO_ISSET(hapiPortPtr, protocolIndex);
    /* check if proto is set on Lag interface */
    lagEnabled  = LLPF_PORT_BLOCK_PROTO_ISSET(hapiLagPtr,  protocolIndex);

    /* Update corresponding policy state */
    if (protocolIndex == L7_LLPF_BLOCK_TYPE_ISDP)
    {
      /* ISDP is a special case since the ISDP component cares about ISDP packets. */
      rc = hapiBroadIsdpPortUpdate(portUsp,
                                   lagEnabled,
                                   portEnabled,
                                   hapiLagPtr->isdpEnable,
                                   hapiPortPtr->isdpEnable,
                                   dapi_g);
    }
    else
    {
      rc = hapiBroadLlpfPortUpdate(portUsp, 
                                   lagEnabled,
                                   portEnabled,
                                   hapiSystem->llpfPolicyIds[protocolIndex], 
                                   dapi_g);
    }
    if(rc!=L7_SUCCESS)
    {
      /*log*/
      L7_LOGF(L7_LOG_SEVERITY_WARNING,L7_LLPF_COMPONENT_ID,
              "Failed to update the port state during LAG Delete Notify Port %d lport 0x%x\n", 
               portUsp->port,hapiPortPtr->bcmx_lport);
      /*debug*/
      HAPI_BROAD_LLPF_DEBUG(\
            "Failed to update the port state during LAG Delete Notify \
             Unit %d Slot:%d Port %d lport 0x%x\n" \
             ,portUsp->unit, portUsp->slot,portUsp->port,hapiPortPtr->bcmx_lport);
      continue;
    }
  }

  return L7_SUCCESS;
}



/*********************************************************************
*
* @purpose Hooks in basic dapi cmds
*
* @param   DAPI_PORT_t *dapiPortPtr - used to hook in commands for a port
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLlpfPortInit(DAPI_PORT_t *dapiPortPtr)
{

  L7_RC_t result = L7_SUCCESS;

  dapiPortPtr->cmdTable[DAPI_CMD_INTF_LLPF_CONFIG] =
               (HAPICTLFUNCPTR_t)hapiBroadIntfLlpfBlockModeConfig;

  return result;
}


/*********************************************************************
 *
 * @purpose Install routine to install system port polices 
 *                                                   for LLPF component
 *
 * @param   DAPI_t           *dapi_g  @b{(input)} system information.
 *
 * @returns L7_RC_t     result
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadLlpfPolicyInstall(DAPI_t *dapi_g)
{
  L7_RC_t               result = L7_FAILURE;
  L7_uchar8             llpfDstMac[]   = {0x01, 0x00, 0x0c, 0xcc, 0xcc, 0xcc};
  L7_uchar8             llpfDstMac1[]   = {0x01, 0x00, 0x0c, 0xcc, 0xcc, 0xcd};
  L7_uchar8             llpfblockall_match[]= {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                               FIELD_MASK_NONE, FIELD_MASK_NONE, 0xf0};
  BROAD_SYSTEM_t       *hapiSystem;
  BROAD_POLICY_ACTION_t actionArray[2];
  L7_uint32             parmArray[2];
  L7_uint32             actionCount;

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  if (hapiBroadLlpfSemaphore == L7_NULL)
  {
    hapiBroadLlpfSemaphore = osapiSemaMCreate(OSAPI_SEM_Q_FIFO);
  }
  if (hapiBroadLlpfSemaphore == L7_NULL)
  {
    LOG_ERROR(0);
  }  

  /* ISDP BLOCK POLICY */
  hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_ISDP] = BROAD_POLICY_INVALID;
  hapiSystem->isdpSysId                              = BROAD_POLICY_INVALID;

  if((cnfgrIsFeaturePresent(L7_LLPF_COMPONENT_ID, L7_LLPF_BLOCK_ISDP_FEATURE_ID) == L7_TRUE) &&
     (cnfgrIsComponentPresent(L7_ISDP_COMPONENT_ID) == L7_TRUE))
  {
    /* If ISDP package is present, then we need to install 2 separate rules for
       ISDP packets with different actions. Each port will be applied to the 
       appropriate policy based on the port's configuration for ISDP and LLPF.
       The rules will be as follows:
       1) ISDP = off, LLPF[ISDP] = on         : action = drop
       2) ISDP = on,  LLPF[ISDP] = don't care : action = drop, and copy to cpu (i.e. trap to cpu)
     */

    /* Rule 1 */
    if(cnfgrIsFeaturePresent(L7_LLPF_COMPONENT_ID, L7_LLPF_BLOCK_ISDP_FEATURE_ID) == L7_TRUE)
    {
      actionArray[0] = BROAD_ACTION_HARD_DROP;
      parmArray[0]   = 0;
      actionCount    = 1;

      result = hapiBroadLlpfPolicyInstallProcess(dapi_g, L7_NULL, L7_NULL, ISDP_ETHERTYPE,
                                                 &hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_ISDP],
                                                 actionArray, parmArray, actionCount);
      if(result != L7_SUCCESS)
      {
        HAPI_BROAD_LLPF_DEBUG(\
            "Failed to install LLPF policy with Mac:%x:%x:%x:%x:%x:%x and Ethertype:%x",
            llpfDstMac[0],llpfDstMac[1],llpfDstMac[2],llpfDstMac[3],
            llpfDstMac[4],llpfDstMac[5],ISDP_ETHERTYPE);

        hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_ISDP] = BROAD_POLICY_INVALID;
        return L7_FAILURE;
      }
    }

    /* Rule 2 */
    actionArray[0] = BROAD_ACTION_HARD_DROP;
    parmArray[0]   = 0;
    actionArray[1] = BROAD_ACTION_COPY_TO_CPU;
    parmArray[1]   = HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS;
    actionCount    = 2;

    result = hapiBroadLlpfPolicyInstallProcess(dapi_g, L7_NULL, L7_NULL, ISDP_ETHERTYPE,
                                               &hapiSystem->isdpSysId,
                                               actionArray, parmArray, actionCount);
    if(result != L7_SUCCESS)
    {
      HAPI_BROAD_LLPF_DEBUG(\
          "Failed to install LLPF policy with Mac:%x:%x:%x:%x:%x:%x and Ethertype:%x",
          llpfDstMac[0],llpfDstMac[1],llpfDstMac[2],llpfDstMac[3],
          llpfDstMac[4],llpfDstMac[5],ISDP_ETHERTYPE);

      hapiSystem->isdpSysId = BROAD_POLICY_INVALID;
      return L7_FAILURE;
    }
  }
  else
  {
    if(cnfgrIsFeaturePresent(L7_LLPF_COMPONENT_ID, L7_LLPF_BLOCK_ISDP_FEATURE_ID) == L7_TRUE)
    {
      actionArray[0] = BROAD_ACTION_HARD_DROP;
      parmArray[0]   = 0;
      actionCount    = 1;

      result = hapiBroadLlpfPolicyInstallProcess(dapi_g, L7_NULL, L7_NULL, ISDP_ETHERTYPE,
                                                 &hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_ISDP],
                                                 actionArray, parmArray, actionCount);
      if(result != L7_SUCCESS)
      {
        HAPI_BROAD_LLPF_DEBUG(\
            "Failed to install LLPF policy with Mac:%x:%x:%x:%x:%x:%x and Ethertype:%x",
            llpfDstMac[0],llpfDstMac[1],llpfDstMac[2],llpfDstMac[3],
            llpfDstMac[4],llpfDstMac[5],ISDP_ETHERTYPE);

        hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_ISDP] = BROAD_POLICY_INVALID;
        return L7_FAILURE;
      }
    }
  }

  /* The rest of the policies simply drop the packet. */
  actionArray[0] = BROAD_ACTION_HARD_DROP;
  parmArray[0]   = 0;
  actionCount    = 1;

  /* VTP BLOCK POLICY */
  hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_VTP] = BROAD_POLICY_INVALID;
  if(cnfgrIsFeaturePresent(L7_LLPF_COMPONENT_ID,L7_LLPF_BLOCK_VTP_FEATURE_ID) == L7_TRUE)
  {
    result = hapiBroadLlpfPolicyInstallProcess(dapi_g, L7_NULL, L7_NULL, VTP_ETHERTYPE, 
                                               &hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_VTP],
                                               actionArray, parmArray, actionCount);
    if(result != L7_SUCCESS)
    {
      HAPI_BROAD_LLPF_DEBUG(\
        "Failed to install LLPF policy with Mac:%x:%x:%x:%x:%x:%x and Ethertype:%x",
         llpfDstMac[0],llpfDstMac[1],llpfDstMac[2],llpfDstMac[3],
         llpfDstMac[4],llpfDstMac[5],VTP_ETHERTYPE);
      
      hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_VTP] = BROAD_POLICY_INVALID;
      return L7_FAILURE;
    }
  }

  /* DTP BLOCK POLICY */
  hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_DTP] = BROAD_POLICY_INVALID;
  if(cnfgrIsFeaturePresent(L7_LLPF_COMPONENT_ID,L7_LLPF_BLOCK_DTP_FEATURE_ID) == L7_TRUE)
  {
    result = hapiBroadLlpfPolicyInstallProcess(dapi_g, L7_NULL, L7_NULL, DTP_ETHERTYPE, 
                                               &hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_DTP],
                                               actionArray, parmArray, actionCount);
    if(result != L7_SUCCESS)
    {
      HAPI_BROAD_LLPF_DEBUG(\
         "Failed to install LLPF policy with Mac:%x:%x:%x:%x:%x:%x and Ethertype:%x",
           llpfDstMac[0],llpfDstMac[1],llpfDstMac[2],llpfDstMac[3],
           llpfDstMac[4],llpfDstMac[5],DTP_ETHERTYPE);
      
      hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_DTP] = BROAD_POLICY_INVALID;
      return L7_FAILURE;
    }
  }

  /* UDLD BLOCK POLICY */
  hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_UDLD] = BROAD_POLICY_INVALID;
  if(cnfgrIsFeaturePresent(L7_LLPF_COMPONENT_ID,L7_LLPF_BLOCK_UDLD_FEATURE_ID) == L7_TRUE)
  {
    result = hapiBroadLlpfPolicyInstallProcess(dapi_g, L7_NULL, L7_NULL, UDLD_ETHERTYPE, 
                                               &hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_UDLD],
                                               actionArray, parmArray, actionCount);
    if(result != L7_SUCCESS)
    {
      HAPI_BROAD_LLPF_DEBUG(\
         "Failed to install LLPF policy with Mac:%x:%x:%x:%x:%x:%x and Ethertype:%x",
          llpfDstMac[0],llpfDstMac[1],llpfDstMac[2],llpfDstMac[3],
          llpfDstMac[4],llpfDstMac[5],UDLD_ETHERTYPE);
       
      hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_UDLD] = BROAD_POLICY_INVALID;
      return L7_FAILURE;
    }
  }

  /* PAGP BLOCK POLICY */
  hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_PAGP] = BROAD_POLICY_INVALID;
  if(cnfgrIsFeaturePresent(L7_LLPF_COMPONENT_ID,L7_LLPF_BLOCK_PAGP_FEATURE_ID) == L7_TRUE)
  {
    result = hapiBroadLlpfPolicyInstallProcess(dapi_g, L7_NULL, L7_NULL, PAGP_ETHERTYPE, 
                                               &hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_PAGP],
                                               actionArray, parmArray, actionCount);
    if(result != L7_SUCCESS)
    {
      HAPI_BROAD_LLPF_DEBUG(\
         "Failed to install LLPF policy with Mac:%x:%x:%x:%x:%x:%x and Ethertype:%x",
          llpfDstMac[0],llpfDstMac[1],llpfDstMac[2],llpfDstMac[3],
          llpfDstMac[4],llpfDstMac[5],PAGP_ETHERTYPE);
      
      hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_PAGP] = BROAD_POLICY_INVALID;
      return L7_FAILURE;
    }
  }

  /* SSTP BLOCK POLICY */
  hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_SSTP] = BROAD_POLICY_INVALID;
  if(cnfgrIsFeaturePresent(L7_LLPF_COMPONENT_ID,L7_LLPF_BLOCK_PAGP_FEATURE_ID) == L7_TRUE)
  {
    result = hapiBroadLlpfPolicyInstallProcess(dapi_g, L7_NULL, L7_NULL, SSTP_ETHERTYPE, 
                                               &hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_SSTP],
                                               actionArray, parmArray, actionCount);
    if(result != L7_SUCCESS)
    {
      HAPI_BROAD_LLPF_DEBUG(\
          "Failed to install LLPF policy with mac:%x:%x:%x:%x:%x:%x",
          llpfDstMac1[0],llpfDstMac1[1],llpfDstMac1[2],
          llpfDstMac1[3],llpfDstMac1[4],llpfDstMac1[5]);
       
      hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_SSTP] = BROAD_POLICY_INVALID;
      return L7_FAILURE;
    }
  }

  /* ALL BLOCK POLICY */
  hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_ALL] = BROAD_POLICY_INVALID;
  if(cnfgrIsFeaturePresent(L7_LLPF_COMPONENT_ID,L7_LLPF_BLOCK_ALL_FEATURE_ID) == L7_TRUE)
  {
    result = hapiBroadLlpfPolicyInstallProcess(dapi_g, llpfDstMac, llpfblockall_match, L7_NULL, 
                                               &hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_ALL], 
                                               actionArray, parmArray, actionCount);
    if(result != L7_SUCCESS)
    {

      HAPI_BROAD_LLPF_DEBUG(\
      "Failed to install LLPF policy with mac:%x:%x:%x:%x:%x:%x",
      llpfDstMac[0] ,llpfDstMac[1],llpfDstMac[2],
      llpfDstMac[3],llpfDstMac[4],llpfDstMac[5]);
      
      hapiSystem->llpfPolicyIds[L7_LLPF_BLOCK_TYPE_ALL] = BROAD_POLICY_INVALID;
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose Routine to remove LLPF polices 
 *
 * @param   DAPI_t           *dapi_g  @b{(input)} system information.
 *
 * @returns L7_RC_t     result
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadLlpfPolicyRemove(DAPI_t *dapi_g)
{
  L7_RC_t              result = L7_FAILURE;
  BROAD_SYSTEM_t      *hapiSystem;
  L7_LLPF_BLOCK_TYPE_t llpfBlockType;

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  for (llpfBlockType = 0; llpfBlockType < L7_LLPF_BLOCK_TYPE_LAST; llpfBlockType++)
  {
    if (hapiSystem->llpfPolicyIds[llpfBlockType] != BROAD_POLICY_INVALID)
    {
      result = hapiBroadPolicyDelete(hapiSystem->llpfPolicyIds[llpfBlockType]);
      if (result != L7_SUCCESS)
      {
        break;
      }
      hapiSystem->llpfPolicyIds[llpfBlockType] = BROAD_POLICY_INVALID;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
 *
 * @purpose To Enable/Disable LLPF block mode on a port.
 *
 * @param   DAPI_USP_t *usp    - needs to be a valid usp
 * @param   DAPI_CMD_t  cmd    - DAPI_CMD_
 * @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
 * @param   DAPI_t     *dapi_g - the driver object
 *
 * @returns L7_RC_t result
 *
 * @notes 
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadIntfLlpfBlockModeConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                 rc = L7_SUCCESS;
  DAPI_INTF_MGMT_CMD_t    *dapiCmd      = (DAPI_INTF_MGMT_CMD_t*)data;
  DAPI_PORT_t             *dapiPortPtr;
  BROAD_PORT_t            *hapiPortPtr;
  BROAD_SYSTEM_t          *hapiSystem;
  BROAD_POLICY_t          policyId = BROAD_POLICY_INVALID;
  L7_LLPF_BLOCK_TYPE_t    llpfBlockType = L7_LLPF_BLOCK_TYPE_ALL+1;
  L7_BOOL                 portEnabled;

  if (dapiCmd->cmdData.llpfConfig.getOrSet != DAPI_CMD_SET)
  {
    /*Debug trace*/
     HAPI_BROAD_LLPF_DEBUG(\
            "Invalid LLPF DAPI COMMAND (%d)" \
             ,dapiCmd->cmdData.llpfConfig.getOrSet);

    return L7_FAILURE;
  }

  dapiPortPtr = DAPI_PORT_GET(usp, dapi_g);
  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
   
  /* Only Physical and LAG interfaces should be allowed.*/
  if ( ( L7_FALSE == IS_PORT_TYPE_PHYSICAL( dapiPortPtr ) ) &&
        ( L7_FALSE == IS_PORT_TYPE_LOGICAL_LAG( dapiPortPtr ) ) )
  {
    /*Debug trace*/
     HAPI_BROAD_LLPF_DEBUG(\
            "Invalid LLPF Port Number to configure LLPF block Types \
             port: Unit %d, slot %d port %d lport  0x%x\n" \
             ,usp->unit,usp->slot,usp->port,hapiPortPtr->bcmx_lport);

    return( L7_FAILURE );
  }

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  /* get the block type */
  llpfBlockType = dapiCmd->cmdData.llpfConfig.blockType;

  /* check if block type is valid. If valid, get the corresponding policyId */
  if(llpfBlockType < L7_LLPF_BLOCK_TYPE_LAST)
  {
    if(hapiSystem->llpfPolicyIds[llpfBlockType]!= BROAD_POLICY_INVALID)
    {
      policyId = hapiSystem->llpfPolicyIds[llpfBlockType];
    }
  }

  if(policyId != BROAD_POLICY_INVALID)
  {
    /* LLPF critical section enter */
    hapiBroadLlpfSemTake();

    /* check if proto is already set on Physical interface */
    portEnabled = LLPF_PORT_BLOCK_PROTO_ISSET(hapiPortPtr,dapiCmd->cmdData.llpfConfig.blockType);

    if (llpfBlockType == L7_LLPF_BLOCK_TYPE_ISDP)
    {
      /* ISDP is a special case since the ISDP component cares about ISDP packets. */
      rc = hapiBroadIsdpPolicySet(usp, 
                                  portEnabled, 
                                  dapiCmd->cmdData.llpfConfig.enable,
                                  hapiPortPtr->isdpEnable,
                                  hapiPortPtr->isdpEnable,
                                  dapi_g);
    }
    else
    {
      /* Enable/Disable block setting for a corresponding interface */
      rc = hapiBroadLlpfSet( usp,
                             portEnabled,
                             dapiCmd->cmdData.llpfConfig.enable,
                             policyId,
                             dapi_g);
    }

    /* LLPF Critical Section Exit */
    hapiBroadLlpfSemGive();
  }

  if (L7_SUCCESS != rc)
  {
    /*log*/
    L7_LOGF(L7_LOG_SEVERITY_WARNING,L7_LLPF_COMPONENT_ID,
            "Failed to set/unset(%d) LLPF BlockType (%d) with Policy ID(%d) port %d lport  0x%x\n",
             usp->port,hapiPortPtr->bcmx_lport); 
             
    /*Debug trace*/
     HAPI_BROAD_LLPF_DEBUG(\
            "Failed to set/unset(%d) LLPF BlockType (%d) with Policy ID(%d) \
             on port: Unit %d, slot %d port %d lport  0x%x\n" \
             ,portEnabled,llpfBlockType,policyId,usp->unit \
             ,usp->slot,usp->port,hapiPortPtr->bcmx_lport);

    return rc;
  }
  else
  {
    /* update port pointer */
    if (L7_TRUE == dapiCmd->cmdData.llpfConfig.enable)
    {
      LLPF_PORT_BLOCK_PROTO_SET(hapiPortPtr, dapiCmd->cmdData.llpfConfig.blockType);
    }
    else
    {
      LLPF_PORT_BLOCK_PROTO_CLEAR(hapiPortPtr, dapiCmd->cmdData.llpfConfig.blockType);
    } 
    HAPI_BROAD_LLPF_DEBUG(\
            "Succesfully Updated LLPF Port Pointer for protocol(%d) with PolicyID(%d) for port: Unit %d, slot %d port %d lport  0x%x\n" \
             ,llpfBlockType,policyId,usp->unit \
             ,usp->slot,usp->port,hapiPortPtr->bcmx_lport);

  }

  return rc;
}
#endif
