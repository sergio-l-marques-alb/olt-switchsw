/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_distiller_tlv.c
*
* @purpose    DiffServ component Distiller TLV builder
*
* @component  DiffServ
*
* @comments   none
*
* @create     07/25/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
#include "l7_diffserv_include.h"
#include "diffserv_tlv.h"
#include "dtl_qos_diffserv.h"
#include "tlv_api.h"
#include "osapi_support.h"
#include "usmdb_mib_diffserv_private_api.h"

/* external references */
extern dsDstlCtrl_t   *pDsDstlCtrl;             /*principal control struct ptr*/

/* utility function for use internal to this file only */
static void dsDstlIpv6AddrPrint(L7_uint32 msgLvlReqd, L7_uchar8 *pAddr);

/*********************************************************************
* @purpose  Initialize the Distiller TLV support code
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvInit(void)
{
  L7_tlvHandle_t  tlvHandle = 0;
  L7_RC_t         rc;

  /* register with the TLV utility to obtain a TLV block
   * NOTE: the Distiller only works on one TLV block at a time
   */
  rc = tlvRegister(L7_QOS_DIFFSERV_TLV_BLOCK_SIZE_MAX,
                   L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                   (L7_uchar8 *)"DiffServ", &tlvHandle);
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("dsDstlTlvInit:  Unable to register with the TLV utility, rc=%d\n",
            rc);
    tlvHandle = 0;
  }

  pDsDstlCtrl->tlvHandle = tlvHandle;
  return rc;
}


/*********************************************************************
* @purpose  UnInitialize the Distiller TLV support code
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
void dsDstlTlvFini(void)
{
  L7_tlvHandle_t  tlvHandle = 0;
  L7_RC_t rc;

  /* register with the TLV utility to obtain a TLV block
   * NOTE: the Distiller only works on one TLV block at a time
   */
  if (pDsDstlCtrl->tlvHandle != 0)
  {
    if ((rc = tlvUnregister(pDsDstlCtrl->tlvHandle)) != L7_SUCCESS)
    {
      LOG_MSG("dsDstlTlvFini:  Unable to Unregister with the TLV utility, rc=%d\n", rc);
    }
    tlvHandle = 0;
  }

  pDsDstlCtrl->tlvHandle = tlvHandle;
}

/*********************************************************************
* @purpose  Create and issue TLVs to communicate the service policy 
*           for the specified interface to the low-level code
*
* @param    policyIndex @{(input)} Policy index
* @param    intIfNum    @{(input)} Internal interface number   
* @param    ifDirection @{(input)} Interface direction         
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    This is an arbiter function that decides whether separate
*           TLVs need to be issued for each policy instance, or if 
*           a single TLV is required which contains all instances.
*
* @notes    The logic contained within each of the called functions is
*           very similar, but there are enough differences in execution
*           to warrant using two separate functions for clarity.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPolicyAdd(L7_uint32 policyIndex, L7_uint32 intIfNum,
                           dsmibServiceIfDir_t ifDirection)
{
  L7_BOOL       comboFlag = L7_DIFFSERV_TLV_POLICY_COMBINED;
  L7_RC_t       rc;

  DS_TRACE_PT_POL_TLV_ADD_ENTER(policyIndex, intIfNum, ifDirection, comboFlag);

  if (comboFlag != L7_TRUE)
    rc = dsDstlTlvPolicyAddSeparate(policyIndex, intIfNum, ifDirection);
  else
    rc = dsDstlTlvPolicyAddCombined(policyIndex, intIfNum, ifDirection);

  return rc;
}

/*********************************************************************
* @purpose  Create and issue separate TLVs for each service policy instance 
*
* @param    policyIndex @{(input)} Policy index
* @param    intIfNum    @{(input)} Internal interface number   
* @param    ifDirection @{(input)} Interface direction         
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @notes    This is the "normal" method of communicating the service 
*           policy to the low-level code.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPolicyAddSeparate(L7_uint32 policyIndex, L7_uint32 intIfNum,
                                   dsmibServiceIfDir_t ifDirection)
{
  L7_RC_t             rc = L7_FAILURE;          /*in case policy doesn't exist*/
  L7_uint32           key = DSDSTL_TLV_INST_KEY_NOT_ISSUED;
  L7_tlvHandle_t      tlvHandle;
  L7_uint32           ifIndex;
  L7_uint32           policyInstIndex, nextPolicy;
  dsDstlIntfNode_t    node, *pNode;

  tlvHandle = pDsDstlCtrl->tlvHandle;

  /* convert intIfNum to ifIndex */
  if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
    return L7_FAILURE;

  /* walk the Policy Instance Table, creating a QOS DiffServ Classifier 
   * Instance TLV for each policy-class instance in this policy
   */
  policyInstIndex = 0;                          /* start with first instance */
  while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex,
                          &nextPolicy, &policyInstIndex) == L7_SUCCESS) &&
         (nextPolicy == policyIndex))
  {
    if (tlvCreate(tlvHandle) != L7_SUCCESS)
      return L7_ERROR;

    /*NOTE: all subsequent error conditions require tlvDelete before returning*/
    do
    {
      dsDstlTlvInstToKey(policyIndex, policyInstIndex, &key);

      if ((rc = dsDstlTlvPolicyAddInstBuild(tlvHandle, key, policyIndex,
                                            policyInstIndex, intIfNum)) 
            != L7_SUCCESS)
        break;

      /* complete and issue the TLV block... */
      rc = dsDstlTlvPolicyAddTlvIssue(tlvHandle, intIfNum, ifDirection);

      /* TLV type and length is needed for the trace point */
      {
        L7_ushort16   tlvType = 0;
        L7_uint32     tlvLen = 0;
        L7_tlv_t      *pTlv;

        if (tlvQuery(tlvHandle, &pTlv, &tlvLen) == L7_SUCCESS)
          tlvType = osapiNtohs(pTlv->type);

        DS_TRACE_PT_POL_TLV_ADD_DTL(policyIndex, policyInstIndex, intIfNum,
                                    ifDirection, tlvHandle, rc, tlvType, tlvLen,
                                    key);
      }

      /* ...now check TLV issue result */
      if (rc != L7_SUCCESS)
        break;

      /* set the TLV issued flag for this instance */
      memset(&node, 0, sizeof(node));
      node.key.policyIndex = policyIndex;
      node.key.ifIndex = ifIndex;

      DIFFSERV_SEMA_TAKE(pDsDstlCtrl->ollSemId, L7_WAIT_FOREVER);

      pNode = (dsDstlIntfNode_t *)avlSearchLVL7(pDsDstlCtrl->pOLL, &node, 
                                                AVL_EXACT);

      if (pNode != L7_NULLPTR)
      {
        pNode->tlvIssued[policyInstIndex] = key;
      }
      else
      {
        LOG_MSG("dsDstlTlvPolicyAddSeparate: TLV issued, but could not update "
                "TLV issued flags for policy %u, ifIndex %u\n", policyIndex, 
                ifIndex);
        rc = L7_FAILURE;
      }

      DIFFSERV_SEMA_GIVE(pDsDstlCtrl->ollSemId);

    } while (0);

    /* free the TLV resources (must occur after the DTL command is issued)
     *
     * NOTE: This requires the low-level code to copy whatever it needs 
     *       from the TLV content prior to the synchronous return from
     *       the command invocation (even if command completes asynchronously).
     *
     * NOTE: Any error that occurs after a successful TLV creation must
     *       flow through here to ensure the TLV is deleted.
     */
    if (tlvDelete(tlvHandle) != L7_SUCCESS)
    {
      LOG_MSG("dsDstlTlvPolicyAddSeparate: Could not delete TLV for handle %u\n",
              (L7_uint32)tlvHandle);
      rc = L7_ERROR;
    }

    /* check rc to terminate while loop early if any error has occurred */
    if (rc != L7_SUCCESS)
      break;

  } /* endwhile policy inst get next */

  DS_TRACE_PT_POL_TLV_ADD_SEP_EXIT(policyIndex, policyInstIndex, intIfNum,
                                   ifDirection, key, rc);

  return rc;
}

/*********************************************************************
* @purpose  Create and issue a single TLV containing all service policy
*           instances within it
*
* @param    policyIndex @{(input)} Policy index
* @param    intIfNum    @{(input)} Internal interface number   
* @param    ifDirection @{(input)} Interface direction         
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @notes    This is the "special case" method of communicating the service 
*           policy to the low-level code.  This is used when all the 
*           information is needed at the same time, typically due to 
*           device classifier setup requirements.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPolicyAddCombined(L7_uint32 policyIndex, L7_uint32 intIfNum,
                                   dsmibServiceIfDir_t ifDirection)
{
  L7_uint32               key = DSDSTL_TLV_INST_KEY_NOT_ISSUED;
  L7_RC_t                 rc;
  L7_tlvHandle_t          tlvHandle;
  L7_uint32               ifIndex;
  L7_uint32               i;
  L7_uint32               localTlvIssued[L7_DIFFSERV_INST_PER_POLICY_LIM+1];
  dsmibPolicyEntryCtrl_t  *pPolicy;
  diffServTlvPolicyAdd_t  policyAdd;
  L7_uint32               numInst;
  L7_uint32               policyInstIndex, nextPolicy;
  dsDstlIntfNode_t        node, *pNode;

  tlvHandle = pDsDstlCtrl->tlvHandle;

  /* convert intIfNum to ifIndex */
  if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
    return L7_FAILURE;

  /* initialize local 'tlvIssued' array */
  for (i = 0; i <= L7_DIFFSERV_INST_PER_POLICY_LIM; i++)
    localTlvIssued[i] = DSDSTL_TLV_INST_KEY_NOT_ISSUED;

  /* find policy row */
  pPolicy = dsmibPolicyRowFind(policyIndex);
  if (pPolicy == L7_NULLPTR)
    return L7_FAILURE;

  /* make sure there is at least one policy instance to process */
  if (pPolicy->instCount == 0)
    return L7_SUCCESS;

  /* set up the TLV block for building this TLV */
  if (tlvCreate(tlvHandle) != L7_SUCCESS)
    return L7_ERROR;

  /*NOTE: all subsequent error conditions require tlvDelete before returning*/
  do
  {
    memset(&policyAdd, 0, sizeof(policyAdd));
    policyAdd.instanceCount = (L7_uint32)osapiHtonl(pPolicy->instCount);

    if ((rc = tlvOpen(tlvHandle, L7_QOS_DIFFSERV_TLV_POLICY_ADD_TYPE,
                      L7_QOS_DIFFSERV_TLV_POLICY_ADD_LEN,
                      (L7_uchar8 *)&policyAdd)) != L7_SUCCESS)
      break;

    /* walk the Policy Instance Table, creating a QOS DiffServ Classifier 
     * Instance sub-TLV for each policy-class instance in this policy
     */
    numInst = 0;
    policyInstIndex = 0;                          /* start with first instance */
    while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex,
                            &nextPolicy, &policyInstIndex) == L7_SUCCESS) &&
           (nextPolicy == policyIndex))
    {
      dsDstlTlvInstToKey(policyIndex, policyInstIndex, &key);

      if ((rc = dsDstlTlvPolicyAddInstBuild(tlvHandle, key, policyIndex,
                                            policyInstIndex, intIfNum)) 
            != L7_SUCCESS)
        break;

      /* remember the policy instance key for later */
      localTlvIssued[policyInstIndex] = key;

      numInst++;

    } /* endwhile policy inst get next */

    /* check result of instance table walk */
    if (rc != L7_SUCCESS)
      break;

    /* verify the expected number of policy instances were processed */
    if (numInst != pPolicy->instCount)
    {
      rc = L7_FAILURE;
      break;
    }

    /* close the QOS DiffServ Policy Add TLV */
    if ((rc = tlvClose(tlvHandle, 
                       L7_QOS_DIFFSERV_TLV_POLICY_ADD_TYPE)) != L7_SUCCESS)
      break;

    /* complete and issue the TLV block... */
    rc = dsDstlTlvPolicyAddTlvIssue(tlvHandle, intIfNum, ifDirection);

    /* TLV type and length is needed for the trace point */
    {
      L7_ushort16   tlvType = 0;
      L7_uint32     tlvLen = 0;
      L7_tlv_t      *pTlv;

      if (tlvQuery(tlvHandle, &pTlv, &tlvLen) == L7_SUCCESS)
        tlvType = osapiNtohs(pTlv->type);

      DS_TRACE_PT_POL_TLV_ADD_DTL(policyIndex, policyInstIndex, intIfNum,
                                  ifDirection, tlvHandle, rc, tlvType, tlvLen,
                                  key);
    }

    /* ...now check TLV issue result */
    if (rc != L7_SUCCESS)
      break;

    /* now that the TLV was successfully issued, set the 'TLV issued' flag 
     * for each classifier instance included in the combined TLV
     */
    memset(&node, 0, sizeof(node));
    node.key.policyIndex = policyIndex;
    node.key.ifIndex = ifIndex;

    DIFFSERV_SEMA_TAKE(pDsDstlCtrl->ollSemId, L7_WAIT_FOREVER);

    pNode = (dsDstlIntfNode_t *)avlSearchLVL7(pDsDstlCtrl->pOLL, &node,
                                              AVL_EXACT);
    if (pNode != L7_NULLPTR)
    {
      /* NOTE: entry 0 not used */
      for (i = 1; i <= L7_DIFFSERV_INST_PER_POLICY_LIM; i++)
      {
        if (localTlvIssued[i] != DSDSTL_TLV_INST_KEY_NOT_ISSUED)
          pNode->tlvIssued[i] = localTlvIssued[i];
      }
    }
    else
    {
      LOG_MSG("dsDstlTlvPolicyAddCombined: TLVs issued, but could not update "
              "TLV issued flags for policy %u, ifIndex %u\n", policyIndex, 
              ifIndex);
      rc = L7_FAILURE;
    }

    DIFFSERV_SEMA_GIVE(pDsDstlCtrl->ollSemId);

  } while (0);

  /* free the TLV resources (must occur after the DTL command is issued)
   *
   * NOTE: This requires the low-level code to copy whatever it needs 
   *       from the TLV content prior to the synchronous return from
   *       the command invocation (even if command completes asynchronously).
   *
   * NOTE: Any error that occurs after a successful TLV creation must
   *       flow through here to ensure the TLV is deleted.
   */
  if (tlvDelete(tlvHandle) != L7_SUCCESS)
  {
    LOG_MSG("dsDstlTlvPolicyAddCombined: Could not delete TLV for handle %u\n",
            (L7_uint32)tlvHandle);
    rc = L7_ERROR;
  }

  DS_TRACE_PT_POL_TLV_ADD_COMBO_EXIT(policyIndex, intIfNum, ifDirection,
                                     key, rc);

  return rc;
}

/*********************************************************************
* @purpose  Create a single TLV containing all service policy
*           instances within it and return the data in TLV block 
*           storage provided by the caller.
*
* @param    policyIndex @{(input)} Policy index
* @param    intIfNum    @{(input)} Internal interface number   
* @param    ifDirection @{(input)} Interface direction         
* @param    tlvHandle   @{(input)} pointer to an L7_tlvHandle_to be
*                                  populated with policy information         
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only. 
*
* @notes    The information distilled into the TLV represents a snapshot of the data in
*           the policy database at time of invocation.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPolicyCombinedTlvGet(L7_uint32 policyIndex, L7_uint32 intIfNum,
                                      dsmibServiceIfDir_t ifDirection, L7_tlvHandle_t tlvHandle)
{
  L7_uint32               key = DSDSTL_TLV_INST_KEY_NOT_ISSUED;
  L7_RC_t                 rc;
  dsmibPolicyEntryCtrl_t  *pPolicy;
  diffServTlvPolicyAdd_t  policyAdd;
  L7_uint32               numInst;
  L7_uint32               policyInstIndex, nextPolicy;
  L7_tlv_t                *pTlv;
  L7_uint32               tlvLen;

  /* find policy row */
  pPolicy = dsmibPolicyRowFind(policyIndex);
  if (pPolicy == L7_NULLPTR)
    return L7_FAILURE;

  /* make sure there is at least one policy instance to process */
  if (pPolicy->instCount == 0)
    return L7_SUCCESS;

  /* set up the TLV block for building this TLV */
  if (tlvCreate(tlvHandle) != L7_SUCCESS)
    return L7_ERROR;

  /*NOTE: all subsequent error conditions require tlvDelete before returning*/
  do
  {
    memset(&policyAdd, 0, sizeof(policyAdd));
    policyAdd.instanceCount = (L7_uint32)osapiHtonl(pPolicy->instCount);

    if ((rc = tlvOpen(tlvHandle, L7_QOS_DIFFSERV_TLV_POLICY_ADD_TYPE,
                      L7_QOS_DIFFSERV_TLV_POLICY_ADD_LEN,
                      (L7_uchar8 *)&policyAdd)) != L7_SUCCESS)
      break;

    /* walk the Policy Instance Table, creating a QOS DiffServ Classifier 
     * Instance sub-TLV for each policy-class instance in this policy
     */
    numInst = 0;
    policyInstIndex = 0;                          /* start with first instance */
    while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex, 
                                      &nextPolicy, &policyInstIndex) == L7_SUCCESS) &&
           (nextPolicy == policyIndex))
    {
      dsDstlTlvInstToKey(policyIndex, policyInstIndex, &key);

      if ((rc = dsDstlTlvPolicyAddInstBuild(tlvHandle, key, policyIndex,
                                            policyInstIndex, intIfNum)) 
            != L7_SUCCESS)
        break;

      numInst++;

    } /* endwhile policy inst get next */

    /* check result of instance table walk */
    if (rc != L7_SUCCESS)
      break;

    /* verify the expected number of policy instances were processed */
    if (numInst != pPolicy->instCount)
    {
      rc = L7_FAILURE;
      break;
    }

    /* close the QOS DiffServ Policy Add TLV */
    if ((rc = tlvClose(tlvHandle, 
                       L7_QOS_DIFFSERV_TLV_POLICY_ADD_TYPE)) != L7_SUCCESS)
      break;

    /* complete the entire TLV */
    if ((rc = tlvComplete(tlvHandle)) != L7_SUCCESS)
      break;

    /* query the completed TLV location and length */
    if ((rc = tlvQuery(tlvHandle, &pTlv, &tlvLen)) != L7_SUCCESS)
      break;

    /* DEBUG: display contents of the TLV (under msgLvl control) */
    (void)dsDstlTlvParse(pTlv, intIfNum);

  } while (0);

  /* if any error encountered after the tlvCreate call, clean up the partial TLV record */
  if (L7_SUCCESS != rc)
  {
    (void)tlvDelete(tlvHandle);
  }

  return rc;
}

/*********************************************************************
* @purpose  Build a TLV for a single policy-class instance
*
* @param    tlvHandle       @{(input)} TLV block handle         
* @param    instanceKey     @{(input)} TLV instance key         
* @param    policyIndex     @{(input)} Policy index
* @param    policyInstIndex @{(input)} Policy instance index
* @param    intIfNum        @{(input)} Internal interface number   
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPolicyAddInstBuild(L7_tlvHandle_t tlvHandle,
                                    L7_uint32 instanceKey,
                                    L7_uint32 policyIndex, 
                                    L7_uint32 policyInstIndex,
                                    L7_uint32 intIfNum)
{
  L7_RC_t               rc;
  diffServTlvClsfInst_t clsfInst;

  memset(&clsfInst, 0, sizeof(clsfInst));
  clsfInst.instanceKey = (L7_uint32)osapiHtonl(instanceKey);

  do
  {
    if ((rc = tlvOpen(tlvHandle, L7_QOS_DIFFSERV_TLV_CLSF_INST_TYPE,
                      L7_QOS_DIFFSERV_TLV_CLSF_INST_LEN,
                      (L7_uchar8 *)&clsfInst)) != L7_SUCCESS)
      break;

    /* first, build up the Class Definition sub-TLV */
    if ((rc = dsDstlTlvClassDefBuild(tlvHandle, policyIndex, 
                                     policyInstIndex)) != L7_SUCCESS)
      break;

    /* next, build up the Policy Definition sub-TLV */
    if ((rc = dsDstlTlvPolicyDefBuild(tlvHandle, policyIndex, 
                                      policyInstIndex, intIfNum)) 
         != L7_SUCCESS)
      break;

    /* close the QOS DiffServ Classifier Instance TLV */
    if ((rc = tlvClose(tlvHandle, 
                       L7_QOS_DIFFSERV_TLV_CLSF_INST_TYPE)) != L7_SUCCESS)
      break;

  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Complete the policy add TLV and issue it to the low-level code
*
* @param    tlvHandle   @{(input)} TLV block handle         
* @param    intIfNum    @{(input)} Internal interface number   
* @param    ifDirection @{(input)} Interface direction         
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @notes    Completes either an individually-issued classifier instance TLV,
*           or the all-in-one policy add TLV that contains all of the 
*           classifier instance TLVs within it.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPolicyAddTlvIssue(L7_tlvHandle_t tlvHandle, 
                                   L7_uint32 intIfNum,
                                   dsmibServiceIfDir_t ifDirection)
{
  L7_RC_t       rc;
  L7_tlv_t      *pTlv;
  L7_uint32     tlvLen;
  L7_BOOL       outboundFlag;

  do
  {
    /* complete the entire TLV */
    if ((rc = tlvComplete(tlvHandle)) != L7_SUCCESS)
      break;

    /* query the completed TLV location and length */
    if ((rc = tlvQuery(tlvHandle, &pTlv, &tlvLen)) != L7_SUCCESS)
      break;

    /* DEBUG: display contents of the TLV (under msgLvl control) */
    (void)dsDstlTlvParse(pTlv, intIfNum);

    outboundFlag = (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT) ?
                      L7_TRUE : L7_FALSE;

    /* issue DTL command to add this policy instance to the low-level */
    if ((rc = dtlQosDiffServPolicyInstanceAdd(intIfNum, outboundFlag,
                                              pTlv, tlvLen)) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

    /* NOTE: reaching here means rc == L7_SUCCESS */

  } while (0);

  return rc;
}

/*********************************************************************
* @purpose  Create and issue a TLV to delete the service policy 
*           for the specified interface from the low-level code
*
* @param    policyIndex @{(input)} Policy index
* @param    intIfNum    @{(input)} Internal interface number   
* @param    ifDirection @{(input)} Interface direction         
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    It is possible for this function to be called when the 
*           DiffServ administrative mode is already disabled, in 
*           which case no TLVs should be sent to DTL.  Checking the
*           tlvIssued flag for each policy instance before including
*           the instance key in the deletion list enforces this.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPolicyDel(L7_uint32 policyIndex, L7_uint32 intIfNum,
                           dsmibServiceIfDir_t ifDirection)
{
  L7_RC_t                   rc = L7_SUCCESS;    /* in case no policy instances */
  L7_RC_t                   tempRc;
  L7_tlvHandle_t            tlvHandle;
  L7_uint32                 ifIndex;
  dsDstlIntfNode_t          node, *pNode;
  diffServTlvInstDelList_t  instDelList;
  L7_uint32                 i;
  L7_uint32                 count, key;
  L7_tlv_t                  *pTlv;
  L7_uint32                 tlvLen;

  tlvHandle = pDsDstlCtrl->tlvHandle;

  DS_TRACE_PT_POL_TLV_DEL_ENTER(policyIndex, intIfNum, ifDirection);

  do
  {
    /* convert intIfNum to ifIndex */
    if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

    /* access the interface node from the policy OLL */
    memset(&node, 0, sizeof(node));
    node.key.policyIndex = policyIndex;
    node.key.ifIndex = ifIndex;

    DIFFSERV_SEMA_TAKE(pDsDstlCtrl->ollSemId, L7_WAIT_FOREVER);

    pNode = (dsDstlIntfNode_t *)avlSearchLVL7(pDsDstlCtrl->pOLL, &node, 
                                              AVL_EXACT);

    /* no interface node for this policy implies no instance TLVs to delete */
    if (pNode == L7_NULLPTR)
    {
      DIFFSERV_SEMA_GIVE(pDsDstlCtrl->ollSemId);
      rc = L7_SUCCESS;
      break;
    }

    /* walk the tlvIssued array for this interface node, accumulating instance
     * keys for each instance in this policy whose tlvIssued flag is set
     *
     * NOTE: each tlvIssued flag is reset below, after a successful DTL call
     */
    memset(&instDelList, 0, sizeof(instDelList));
    count = 0;
    for (i = 1; i <= L7_DIFFSERV_INST_PER_POLICY_LIM; i++)
    {
      key = pNode->tlvIssued[i];

      if (key != DSDSTL_TLV_INST_KEY_NOT_ISSUED)
        instDelList.instanceKey[count++] = (L7_uint32)osapiHtonl(key);
    }
    instDelList.keyCount = (L7_uint32)osapiHtonl(count);

    DIFFSERV_SEMA_GIVE(pDsDstlCtrl->ollSemId);

    /* Build and send a TLV if at least one policy instance exists */
    if (count != 0)
    {
      /* create a QOS DiffServ Instance Deletion List TLV with one instance
       * key per policy instance
       */
      if (tlvCreate(tlvHandle) != L7_SUCCESS)
      {
        rc = L7_ERROR;
        break;
      }

      do
      {
        L7_uint32   length;

        /* compute actual TLV entry length based on number of keys used */
        length = L7_QOS_DIFFSERV_TLV_INST_DEL_LIST_MIN_LEN + 
                   (count * sizeof(instDelList.instanceKey[0]));

        if ((rc = tlvWrite(tlvHandle, L7_QOS_DIFFSERV_TLV_INST_DEL_LIST_TYPE,
                           length, (L7_uchar8 *)&instDelList))
             != L7_SUCCESS)
          break;

        if ((rc = tlvComplete(tlvHandle)) != L7_SUCCESS)
          break;

        /* query the completed TLV location and length */
        if ((rc = tlvQuery(tlvHandle, &pTlv, &tlvLen)) != L7_SUCCESS)
          break;

      } while (0);

      /* only issue the TLV if it was successfully built */
      if (rc == L7_SUCCESS)
      {
        L7_BOOL           outboundFlag;
        dsDstlIntfNode_t  *pNodeCheck;
        L7_uint32         polIdx, polInstIdx;

        /* DEBUG: display contents of the TLV (under msgLvl control) */
        (void)dsDstlTlvParse(pTlv, intIfNum);

        /* issue DTL command to delete the policy instances from the low-level
         * 
         * NOTE: If an error occurs, just log it and continue.  Exiting here
         *       with a failure prior to cleaning up the TLV issued array can
         *       cause a Service Table row deletion operation to fail.
         */
        outboundFlag = (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT) ?
                          L7_TRUE : L7_FALSE;

        tempRc = dtlQosDiffServPolicyInstanceDelete(intIfNum, outboundFlag,
                                                    pTlv, tlvLen);

        if (tempRc != L7_SUCCESS)
        {
          LOG_MSG("dsDstlTlvPolicyDel: DiffServ deletion TLV failure for "
                  "interface %u %s, key count %u\n", intIfNum, 
                  (outboundFlag != L7_TRUE) ? "inbound" : "outbound",
                  count);
        }

        DS_TRACE_PT_POL_TLV_DEL_DTL(policyIndex, intIfNum, ifDirection,
                                    tlvHandle, tempRc, osapiNtohs(pTlv->type), 
                                    tlvLen);

        /* For each instance key in the deletion TLV, clear its TLV issued flag*/
        DIFFSERV_SEMA_TAKE(pDsDstlCtrl->ollSemId, L7_WAIT_FOREVER);

        /* sanity check that the interface node pointer is still the same */
        pNodeCheck = (dsDstlIntfNode_t *)avlSearchLVL7(pDsDstlCtrl->pOLL, &node, 
                                                       AVL_EXACT);

        if (pNode == pNodeCheck)                /* intf node still same */
        {
          for (i = 0; i < count; i++)
          {
            key = (L7_uint32)osapiNtohl(instDelList.instanceKey[i]);
            dsDstlTlvKeyToInst(key, &polIdx, &polInstIdx);
            if (polIdx != policyIndex)
            {
              LOG_MSG("dsDstlTlvPolicyDel:  Key to Inst translation got "
                      "policy index %u, expected %u\n", polIdx, policyIndex);
              rc = L7_FAILURE;
              continue;                           /* process rest of loop */
            }
            pNode->tlvIssued[polInstIdx] = DSDSTL_TLV_INST_KEY_NOT_ISSUED;
          } /* endfor */
        } /* endif intf node still same */

        else
        {
          /* intf node pointer changed from value we started with */
          LOG_MSG("dsDstlTlvPolicyDel:  Intf node ptr changed unexpectedly "
                  "from 0x%8.8x to 0x%8.8x\n", (L7_uint32)pNode, 
                  (L7_uint32)pNodeCheck);
          rc = L7_FAILURE;
        }

        DIFFSERV_SEMA_GIVE(pDsDstlCtrl->ollSemId);

      } /* endif TLV successfully built */

      /* free the TLV resources (after the DTL command is issued)
       *
       * NOTE: This requires the low-level code to copy whatever it needs
       *       from the TLV contents prior to the synchronous return from
       *       the command invocation (even if command completes asynchronously).
       *
       * NOTE: Any error that occurs after a successful TLV creation must
       *       flow through here to ensure the TLV is deleted.
       */
       if (tlvDelete(tlvHandle) != L7_SUCCESS)
       {
         LOG_MSG("dsDstlTlvPolicyDel: Could not delete TLV for handle %u\n",
                 (L7_uint32)tlvHandle);
         rc = L7_ERROR;
       }

    } /* endif count != 0 */

  } while (0);

  DS_TRACE_PT_POL_TLV_DEL_EXIT(policyIndex, intIfNum, ifDirection, rc);

  return rc;
}

/*********************************************************************
* @purpose  Build Class Definition portion of QOS DiffServ Classifier
*           Instance TLV
*
* @param    tlvHandle       @{(input)} TLV block handle
* @param    policyIndex     @{(input)} Policy index
* @param    policyInstIndex @{(input)} Policy instance index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @notes    Constructs classifier rules corresponding to a particular
*           policy instance definition.
*
* @notes    Appends to TLV block previously created by the caller.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvClassDefBuild(L7_tlvHandle_t tlvHandle, 
                               L7_uint32 policyIndex,
                               L7_uint32 policyInstIndex)
{
  L7_BOOL                   etypeTlvWritten = L7_FALSE;
  L7_RC_t                   rc;
  L7_uint32                 classIndex, classRuleIndex;
  dsmibClassType_t          classType;
  dsmibClassL3Protocol_t    classL3Proto;
  dsDstlRuleOrder_t         ruleOrderInfo;
  L7_uint32                 i;
  dsDstlRuleListElem_t      *pListElem;
  dsDstlTlvNative_t         *pBuf;
  dsDstlTlvWorkInfo_t       tlvWorkInfo, ipEtypeTlv;

  /* get class index from this policy instance */
  if ((rc = diffServPolicyInstObjectGet(policyIndex, policyInstIndex,
                                        L7_DIFFSERV_POLICY_INST_CLASS_INDEX,
                                        &classIndex)) != L7_SUCCESS)
    return L7_FAILURE;

  /* get class type */
  if ((rc = diffServClassObjectGet(classIndex, L7_DIFFSERV_CLASS_TYPE,
                                   &classType)) != L7_SUCCESS)
    return L7_FAILURE;

  /* get class Layer 3 Protocol setting */
  if ((rc = diffServClassObjectGet(classIndex, L7_DIFFSERV_CLASS_L3_PROTO,
                                   &classL3Proto)) != L7_SUCCESS)
    return L7_FAILURE;

  /* build a list of rules for this class (including all nested reference
   * classes) in the proper order for presentation to the low-level driver
   */

  /* initialize rule order list contents */
  memset(&ruleOrderInfo, 0, sizeof(ruleOrderInfo));
  ruleOrderInfo.ruleCount = 0;
  ruleOrderInfo.needSummaryOpen = 
    (classType == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL) ? L7_TRUE : L7_FALSE;

  if (classType == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL) 
  {
    /* invoke 'acl' rule order list build function */
    rc = dsDstlTlvRuleListBuildAcl(classIndex, &ruleOrderInfo);
  }
  else
  {
    /* invoke 'all/any' rule order list build function (USES RECURSION!!) */
    rc = dsDstlTlvRuleListBuild(classIndex, &ruleOrderInfo, 0);
  }
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  /* if no rules exist, there is nothing else to do here
   *
   * NOTE:  Return with failure to cause TLV creation to abort.  All DiffServ
   *        TLV policy instances require both a class definition and a policy
   *        definition sub-TLV.  A class with no rules at this point causes 
   *        the TLV class definition to be skipped, which is invalid.
   */
  if (ruleOrderInfo.ruleCount == 0)
    return L7_FAILURE;

  /* build an IPv4 Ethertype Match TLV for possible use in the class rule def*/
  memset(&ipEtypeTlv, 0, sizeof(ipEtypeTlv));
  ipEtypeTlv.tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_ETYPE_TYPE;
  ipEtypeTlv.tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_ETYPE_LEN;
  pBuf = (dsDstlTlvNative_t *)(ipEtypeTlv.tlvVal);
  /* check whether this class is IPv4 or IPv6 and adjust the etype rule data */
  if (L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6 == classL3Proto)
  {
    pBuf->etype.etypeValue1 = osapiHtons((L7_ushort16)L7_QOS_ETYPE_ID_IPV6);
  }
  else
  {
    pBuf->etype.etypeValue1 = osapiHtons((L7_ushort16)L7_QOS_ETYPE_ID_IPV4);
  }
  pBuf->etype.etypeValue2 = osapiHtons((L7_ushort16)0);
  pBuf->etype.checkStdHdrFlag = L7_TRUE;

  /* open the Class Definition TLV (since at least one rule exists) */
  if ((rc = tlvOpen(tlvHandle, L7_QOS_DIFFSERV_TLV_CLASS_DEF_TYPE,
                    L7_QOS_DIFFSERV_TLV_CLASS_DEF_LEN,
                    L7_NULLPTR)) != L7_SUCCESS)
    return rc;

  /* use completed rule order list (and its flags) to build appropriate
   * set of class rule TLVs
   */
  for (i = 0; i < ruleOrderInfo.ruleCount; i++)
  {
    pListElem = &ruleOrderInfo.list[i];

    classIndex = pListElem->classIndex;
    classRuleIndex = pListElem->classRuleIndex;

    /* conditionally open the class rule definition sub-TLV */
    if (pListElem->open == L7_TRUE)
    {
      etypeTlvWritten = L7_FALSE;
      pBuf = (dsDstlTlvNative_t *)(tlvWorkInfo.tlvVal);
      memset(pBuf, 0, L7_QOS_DIFFSERV_TLV_CLASS_RULE_DEF_LEN);
      pBuf->classRuleDef.denyFlag = (L7_uchar8)pListElem->deny;
      if ((rc = tlvOpen(tlvHandle, L7_QOS_DIFFSERV_TLV_CLASS_RULE_DEF_TYPE,
                        L7_QOS_DIFFSERV_TLV_CLASS_RULE_DEF_LEN, 
                        (L7_uchar8 *)pBuf)) 
           != L7_SUCCESS)
        break;
    }

    /* check forceEvery flag before obtaining class match info
     *
     * NOTE:  Must not use classIndex, classRuleIndex when forceEvery is true
     */
    if (pListElem->forceEvery == L7_TRUE)
    {
      /* set up a 'match every' entry in the TLV working buffer
       *
       * NOTE:  Use a "Match Ethertype IPv4/6" rule instead if the 
       *        needIpv4/6Etype flag is set, since that will restrict the    
       *        'match every' entry to just 'match every ipv4/6 packet'.
       */
      if (pListElem->needIpEtype == L7_FALSE)
      {
        memset(&tlvWorkInfo, 0, sizeof(tlvWorkInfo));
        tlvWorkInfo.tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_EVERY_TYPE;
        tlvWorkInfo.tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_EVERY_LEN;
      }
      else
      {
        memcpy(&tlvWorkInfo, &ipEtypeTlv, sizeof(tlvWorkInfo));
        /* set a local flag so this IP entry does not get written again
         * when the class rule definition sub-TLV is closed
         */
        etypeTlvWritten = L7_TRUE;
      }
    }
    else
    {
      /* obtain specific type, length, and value info for the class rule
       * into the TLV working buffer
       */
      if ((rc = dsDstlTlvClassMatchInfoGet(classIndex, classRuleIndex,
                                           &tlvWorkInfo)) != L7_SUCCESS)
        break;

      /* if this class match TLV contains an Ethertype, set the local
       * flag since there is no need to write an IPv4 entry at the end
       * of the class rule definition sub-TLV.
       */
      if (tlvWorkInfo.tlvType == L7_QOS_DIFFSERV_TLV_CLASS_MATCH_ETYPE_TYPE)
      {
        etypeTlvWritten = L7_TRUE;

        /* if an explicit IPv4 Ethertype is needed per the list element
         * flag, set a flag in the TLV so that HAPI can check the ver|hlen
         * field in the IPv4 header
         */
        if (pListElem->needIpEtype == L7_TRUE)
        {
          L7_ushort16 eTypeId;

          /* NOTE:  The tlvVal contents are in NETWORK byte order here! */
          pBuf = (dsDstlTlvNative_t *)tlvWorkInfo.tlvVal;
          if (classL3Proto == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6)
          { 
            eTypeId = (L7_ushort16)L7_QOS_ETYPE_ID_IPV6;
          }
          else
          {
            eTypeId = (L7_ushort16)L7_QOS_ETYPE_ID_IPV4;
          }
          if (pBuf->etype.etypeValue1 == osapiHtons(eTypeId))
            pBuf->etype.checkStdHdrFlag = L7_TRUE;
        }
      }
    }

    /* write the match criterion TLV */
    if ((rc = tlvWrite(tlvHandle, tlvWorkInfo.tlvType, tlvWorkInfo.tlvLen, 
                       tlvWorkInfo.tlvVal)) != L7_SUCCESS)
      break;

    /* conditionally close the class rule definition sub-TLV */
    if (pListElem->close == L7_TRUE)
    {
      /* write an extra IPv4 or IPv6 Ethertype Match TLV entry for a class rule
       * group that contains any IPv4 or IPv6 field classifier, provided an 
       * Ethertype was not already written into this class rule definition
       * sub-TLV
       */
      if ((pListElem->needIpEtype == L7_TRUE) && 
          (etypeTlvWritten == L7_FALSE))
      {
        memcpy(&tlvWorkInfo, &ipEtypeTlv, sizeof(tlvWorkInfo));
        if ((rc = tlvWrite(tlvHandle, tlvWorkInfo.tlvType, tlvWorkInfo.tlvLen, 
                           tlvWorkInfo.tlvVal)) != L7_SUCCESS)
          break;
      }

      if ((rc = tlvClose(tlvHandle, L7_QOS_DIFFSERV_TLV_CLASS_RULE_DEF_TYPE))
           != L7_SUCCESS)
        break;
    }

  } /* endfor i */

  /* if all went well, close the Class Definition TLV */
  if (rc == L7_SUCCESS)
    rc = tlvClose(tlvHandle, L7_QOS_DIFFSERV_TLV_CLASS_DEF_TYPE);

  return rc;
}

/*********************************************************************
* @purpose  Fill in the rule order list based on class type
*
* @param    classIndex      @{(input)} Class index
* @param    pRuleOrderInfo  @{(input)} Pointer to rule order list array
* @param    depth           @{(input)} Recursion depth (0 for external callers)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @notes    CAUTION: RECURSIVE FUNCTION!!  Required for handling nested
*           class references.  Try to minimize stack space usage (e.g.,
*           minimize local variables).
*
* @notes    The depth parameter must be set to 0 by all external callers
*           of this function!  This value is incremented by 1 for each 
*           recursive call.
*
* @notes    Do not use this function for class type 'acl' (use the 
*           dsDstlTlvRuleListBuildAcl() function instead).
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvRuleListBuild(L7_uint32 classIndex, 
                               dsDstlRuleOrder_t *pRuleOrderInfo,
                               L7_uint32 depth)
{
  L7_RC_t                   rc;
  dsmibClassType_t          classType;
  L7_uint32                 classRuleIndex;
  L7_uint32                 nextClass;
  dsmibClassRuleType_t      classRuleType;
  L7_uint32                 refClassIndex;
  dsmibTruthValue_t         excludeFlag;
  L7_uint32                 i;

  /* get class type */
  if ((rc = diffServClassObjectGet(classIndex, L7_DIFFSERV_CLASS_TYPE,
                                   &classType)) != L7_SUCCESS)
    return L7_FAILURE;

  /* this function does not support class type 'acl' */
  if (classType == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL) 
    return L7_FAILURE;

  /* determine TLV presentation order of each class rule
   *
   * NOTE:  The 'exclude' rules for a class type 'all' must be presented
   *        ahead of the non-exclude rules in the TLV, since this implies
   *        a higher precedence in the hardware classifiers.  These
   *        non-exclude rules are then grouped under one Class Rule
   *        Definition TLV.
   *
   * NOTE:  The Distiller is responsible for combining all reference class
   *        nesting into a single cohesive TLV class definition (recall,
   *        a reference class rule cannot have its 'exclude' flag set).
   *        This is done through recursion.
   */

  classRuleIndex = 0;                       /* start with first class rule */
  while ((diffServClassRuleGetNext(classIndex, classRuleIndex,
                          &nextClass, &classRuleIndex) == L7_SUCCESS) &&
         (nextClass == classIndex))
  {
    /* get the class rule entry type */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE,
                                   &classRuleType) != L7_SUCCESS)
      return L7_FAILURE;

    /* handle nested reference classes (up to one per each class) */
    if (classRuleType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS)
    {
      /* get the reference class index */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                    L7_DIFFSERV_CLASS_RULE_MATCH_REFCLASS_INDEX,
                                    &refClassIndex) != L7_SUCCESS)
        return L7_FAILURE;

      /* RECURSIVELY CALL THIS FUNCTION to process reference class rules
       *
       * NOTE: Assumes private MIB code has done proper checking of all 
       *       reference class requirements when its index object was set.
       *
       * NOTE: Skip this refclass rule if the ref class index has been deleted.
       */
      if (refClassIndex != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_REFCLASS_NONE)
        if (dsDstlTlvRuleListBuild(refClassIndex, pRuleOrderInfo, depth+1) 
             != L7_SUCCESS)
          return L7_FAILURE;

      /*----------- continuation point after recursion processing -----------*/
      /* do not include actual refclass rule in rule order list */
      continue;

    } /* endif refclass rule type */

    /* check if rule order list is full (should never happen) */
    if (pRuleOrderInfo->ruleCount >= DSMIB_CLASS_REF_NESTED_RULE_MAX)
      return L7_FAILURE;

    /* get the class rule 'exclude' flag */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_EXCLUDE_FLAG,
                                   &excludeFlag) != L7_SUCCESS)
      return L7_FAILURE;

    /* insert rule info at front of order list if this is an 'exclude' rule
     * for a class type 'all'; otherwise append to end of rule order list
     */
    if ((classType == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL) && 
        (excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE))
    {
      /* shift all entries down by one position */
      for (i = pRuleOrderInfo->ruleCount; i > 0; i--)
        pRuleOrderInfo->list[i] = pRuleOrderInfo->list[i-1];
      /* NOTE: i == 0 after for loop exits */
    }
    else
    {
      i = pRuleOrderInfo->ruleCount;
    }

    /* save indexes that identify this class rule */
    pRuleOrderInfo->list[i].classIndex = classIndex;
    pRuleOrderInfo->list[i].classRuleIndex = classRuleIndex;
    pRuleOrderInfo->list[i].classRuleType = classRuleType;

    /* translate 'exclude' flag to 'deny' flag value */
    pRuleOrderInfo->list[i].deny = 
      (excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE) ? 
        L7_TRUE : L7_FALSE;

    /* set up a TLV 'open' flag based on circumstances of this class */
    if ((classType != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL) ||
        (excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE))
    {
      pRuleOrderInfo->list[i].open = L7_TRUE;
      pRuleOrderInfo->list[i].close = L7_TRUE;
    }
    else
    {
      /* reaching here means it is a class type 'all' and a non-excluded
       * rule -- the first such rule in the class causes a new Class Rule
       * Definition TLV open, whereas subsequent rule TLVs reside within  
       * that open TLV
       */
      pRuleOrderInfo->list[i].open = pRuleOrderInfo->needSummaryOpen;
      pRuleOrderInfo->needSummaryOpen = L7_FALSE;
    }

    pRuleOrderInfo->ruleCount++;

  } /* endwhile class rule get next */

  /* set the 'close' flag for the last entry in the list
   *
   * NOTE: This is either an 'any' or excluded entry, which already has its
   *       'closed' flag set, or the last non-excluded 'all' entry.
   *
   * NOTE: Only do this when the recursion depth is at 0, since then it is
   *       really the last list entry.
   */
  if (depth == 0)
  {
    i = pRuleOrderInfo->ruleCount;
    if (i > 0)
      pRuleOrderInfo->list[i-1].close = L7_TRUE;

    /* run the completed list through a cleanup routine to account for 
     * 'match every' rules, or rules whose masked value is zero
     */
    dsDstlTlvRuleListRefine(pRuleOrderInfo);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Fill in the rule order list for class type 'acl'
*
* @param    classIndex      @{(input)} Class index
* @param    pRuleOrderInfo  @{(input)} Pointer to rule order list array
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @notes    Does not require recursion, since class type 'acl' does not
*           support reference classes.
* 
* @notes    Do not use for class type other than 'acl'.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvRuleListBuildAcl(L7_uint32 classIndex, 
                                  dsDstlRuleOrder_t *pRuleOrderInfo)
{
  dsmibClassType_t          classType;
  L7_uint32                 classRuleIndex, nextClass;
  dsmibClassRuleType_t      classRuleType;
  dsmibTruthValue_t         excludeFlag;
  L7_uint32                 i;
  L7_uint32                 prevArid, arid;

  /* get class type */
  if (diffServClassObjectGet(classIndex, L7_DIFFSERV_CLASS_TYPE,
                             &classType) != L7_SUCCESS)
    return L7_FAILURE;

  /* this function only supports class type 'acl' */
  if (classType != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL) 
    return L7_FAILURE;

  /* determine TLV presentation order of each class rule
   *
   * NOTE:  The rules for an 'acl' class are listed as a single set per
   *        the classIndex, but are actually sub-divided into groups
   *        corresponding to the ACL Rule from which they were derived.
   *        Each such group is identified by its (nonzero) access rule
   *        identifier (arid) value.  DiffServ class rules with the same 
   *        arid belong together just like a class type 'all'.  Different
   *        arid groups are evaluated sequentially, just like a class type
   *        'any'.  From a TLV perspective, we are building one classifier
   *        that has hybrid characteristics.  The 'open' and 'close' flags
   *        are used to indicate an arid group of ANDed class rules.
   *
   * NOTE:  The 'exclude' flag is the same for all class rules within an 
   *        arid group.  The exclude flag value is used in the Class Rule
   *        Definition sub-TLV, meaning a packet matching the AND condition
   *        of the group's fields is either permitted (i.e., receives the
   *        DiffServ policy treatment for the 'acl' class) or denied (i.e.,
   *        receives best-effort service).
   */

  /* initialize working variables */
  i = 0;
  prevArid = DSMIB_CLASS_RULE_ARID_NONE;

  classRuleIndex = 0;                       /* start with first class rule */
  while ((diffServClassRuleGetNext(classIndex, classRuleIndex,
                          &nextClass, &classRuleIndex) == L7_SUCCESS) &&
         (nextClass == classIndex))
  {
    /* check if rule order list is full (should never happen) */
    if (i >= DSMIB_CLASS_REF_NESTED_RULE_MAX)
      return L7_FAILURE;

    /* get the class rule entry type */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE,
                                   &classRuleType) != L7_SUCCESS)
      return L7_FAILURE;

    /* get the class rule 'exclude' flag */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_EXCLUDE_FLAG,
                                   &excludeFlag) != L7_SUCCESS)
      return L7_FAILURE;

    /* get the ACL rule identifier for this class rule */
    if (diffServClassRuleAclRuleIdGet(classIndex, classRuleIndex, &arid)
          != L7_SUCCESS)
      return L7_FAILURE;

    /* always append a rule at the end of the rule order list, regardless of
     * the 'exclude' flag value
     */

    /* save indexes that identify this class rule */
    pRuleOrderInfo->list[i].classIndex = classIndex;
    pRuleOrderInfo->list[i].classRuleIndex = classRuleIndex;
    pRuleOrderInfo->list[i].classRuleType = classRuleType;

    /* translate 'exclude' flag to 'deny' flag value */
    pRuleOrderInfo->list[i].deny = 
      (excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE) ? 
        L7_TRUE : L7_FALSE;

    /* set up a TLV 'open' flag whenever the arid value changes */
    if (arid != prevArid)
    {
      if (i > 0)                               /* not processing first entry */
        pRuleOrderInfo->list[i-1].close = L7_TRUE;
      pRuleOrderInfo->list[i].open = L7_TRUE;
      prevArid = arid;
    }
    else
      pRuleOrderInfo->list[i].open = L7_FALSE;

    /* initially set the 'close' flag to false for this entry */
    pRuleOrderInfo->list[i].close = L7_FALSE;

    i++;

  } /* endwhile class rule get next */

  /* always set the 'close' flag for the last entry in the list */
  if (i > 0)
    pRuleOrderInfo->list[i-1].close = L7_TRUE;

  pRuleOrderInfo->ruleCount = i;

  /* run the completed list through a cleanup routine to account for 
   * 'match every' rules, or rules whose masked value is zero
   */
  dsDstlTlvRuleListRefine(pRuleOrderInfo);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Refine an ordered list of TLV class rules to eliminate don't care
*           classifier fields
*
* @param    pRuleOrderInfo  @{(input)} Pointer to rule order list array
*
* @returns  void
*
* @notes    For Distiller internal use only.
*
* @notes    Updates the forceEvery flag in the rule order list, where needed.
*           Eliminates non-essential maskable rules whose mask is 0.
*
* @end
*********************************************************************/
void dsDstlTlvRuleListRefine(dsDstlRuleOrder_t *pRuleOrderInfo)
{
  L7_BOOL                   foundValid = L7_FALSE;
  L7_BOOL                   needIpEtype = L7_FALSE;
  L7_BOOL                   checkIpHdr = L7_FALSE;
  L7_uint32                 i, k, count;
  L7_uint32                 classIndex, classRuleIndex;
  dsmibClassRuleType_t      classRuleType;
  dsDstlRuleOrder_t         tempRules;

  count = pRuleOrderInfo->ruleCount;

  /* this first loop establishes the forceEvery flag value */
  for (i = 0; i < count; i++)
  {
    pRuleOrderInfo->list[i].forceEvery = L7_FALSE;
    pRuleOrderInfo->list[i].needIpEtype = L7_FALSE;

    classIndex = pRuleOrderInfo->list[i].classIndex;
    classRuleIndex = pRuleOrderInfo->list[i].classRuleIndex;

    /* get class rule type */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE,
                                   (void *)&classRuleType) == L7_SUCCESS)
    {
      /* set the forceEvery flag if this is a 'match every' rule */
      if (classRuleType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY)
        pRuleOrderInfo->list[i].forceEvery = L7_TRUE;
    }

    /* also set the forceEvery flag if this rule contains a mask field of 0 */
    if (dsDstlTlvMaskableFieldCheck(classIndex, classRuleIndex) != L7_SUCCESS)
      pRuleOrderInfo->list[i].forceEvery = L7_TRUE;

    /* set the needIpEtype flag if this is a IPv4 or IPv6 match field rule */
    if (dsDstlTlvIpFieldCheck(classIndex, classRuleIndex) == L7_SUCCESS)
    {
      pRuleOrderInfo->list[i].needIpEtype = L7_TRUE;

      /* set a flag to be used below in the event an Ethertype rule is processed */
      checkIpHdr = L7_TRUE;
    }
  } /* endfor i (first loop) */

  /* use a temporary struct to build a new list from the existing one */
  memset(&tempRules, 0, sizeof(tempRules));
  tempRules.needSummaryOpen = pRuleOrderInfo->needSummaryOpen;
  k = 0;                                      /* init tempRules list index */

  /* this second loop rebuilds the rule order list based on latest flags */
  for (i = 0; i < count; i++)
  {
    if ((pRuleOrderInfo->list[i].open == L7_TRUE) &&
        (pRuleOrderInfo->list[i].close == L7_TRUE))
    {
      /* single rule list entry */
      memcpy(&tempRules.list[k], &pRuleOrderInfo->list[i], 
             sizeof(tempRules.list[k]));        /* copy whole list element */
      k++;
      /* no need to continue beyond a 'match every' entry in the rule order list */
      if (tempRules.list[k-1].forceEvery == L7_TRUE)
        break;                                  /* exit loop */
    } /* endif single rule list entry */

    else
    {
      /* multiple rule list group */ 
      if (pRuleOrderInfo->list[i].open == L7_TRUE)
      {
        tempRules.list[k].open = L7_TRUE;
        /* Initially create a forceEvery entry to be used in the event that
         * all rules in this group contain a mask of zero.  This 'canned'
         * entry is overlayed with a legitimate entry in all other cases.
         *
         * NOTE: The deny flag is the same for each rule in a multiple rule
         *       list group.  It is always off by definition for class type 
         *       'all', since the group is the set of non-excluded rules. 
         *       The deny flag used for a class type 'acl' group was derived
         *       from the original ACL rule permit/deny value when the DiffServ
         *       class was created.
         */
        tempRules.list[k].deny = pRuleOrderInfo->list[i].deny;
        tempRules.list[k].forceEvery = L7_TRUE;
        tempRules.list[k].needIpEtype = L7_FALSE;

        foundValid = L7_FALSE;
        needIpEtype = L7_FALSE;
      }

      /* Make note of any IP field rule.  This is used later when the 
       * class rule definition TLV is closed to either add an IPv4 or IPv6 Ethertype 
       * match entry to the TLV, or replace the forceEvery entry with it.
       *
       * While the first loop set this flag entry by entry, this second loop
       * collects the information and sets it in the group entry containing
       * the 'close' flag, since that is where it will ultimately be used.
       */
      if (pRuleOrderInfo->list[i].needIpEtype == L7_TRUE)
        needIpEtype = L7_TRUE;

      /* Recall, the forceEvery flag was set in preceding loop if the rule
       * was configured as 'match every', or if a maskable rule type contains
       * a mask of zero.  In a multi-rule group, these rules can safely be
       * ignored as a "don't care", provided there is at least one other rule 
       * in the group that does not have its forceEvery flag set.
       * 
       * In the event that all rules in this group have their forceEvery flag 
       * set, the entire group is treated as a single 'match every' rule,
       * thereby using the 'canned' forceEvery entry created above.
       */
      if (pRuleOrderInfo->list[i].forceEvery != L7_TRUE)
      {
        tempRules.list[k].classIndex = pRuleOrderInfo->list[i].classIndex;
        tempRules.list[k].classRuleIndex = pRuleOrderInfo->list[i].classRuleIndex;
        tempRules.list[k].classRuleType = pRuleOrderInfo->list[i].classRuleType;
        tempRules.list[k].deny = pRuleOrderInfo->list[i].deny;
        tempRules.list[k].forceEvery = L7_FALSE;
        if ((tempRules.list[k].classRuleType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE) &&
            (checkIpHdr == L7_TRUE))
          tempRules.list[k].needIpEtype = L7_TRUE;
        else
          tempRules.list[k].needIpEtype = L7_FALSE;
        k++;
        foundValid = L7_TRUE;
      }

      /* When the last entry of the group is reached, if the foundValid flag 
       * is not True, no legitimate rule entries were found.  Nothing has been
       * written to list element 'k' since the 'canned' forceEvery entry
       * was made.  Increment index k here to lock in usage of this canned entry.
       */
      if (pRuleOrderInfo->list[i].close == L7_TRUE)
      {
        if (foundValid != L7_TRUE)
          k++;                                  /* accept 'canned' forceEvery entry */

        /* At this point, k should always be greater than 0, but adding a
         * check as a safeguard.
         */
        if (k > 0)
        {
          tempRules.list[k-1].close = L7_TRUE;

          /* Set the needIpEtype flag in the group's 'close' entry if any 
           * IPv4 or IPv6 classifier fields were seen in this group.
           */
          tempRules.list[k-1].needIpEtype = needIpEtype;

          /* no need to continue beyond a 'match every' entry in the rule order list */
          if (tempRules.list[k-1].forceEvery == L7_TRUE)
            break;                                /* exit loop */
        }
      } /* endif last entry  */
    } /* endelse multiple rule list group */

  } /* endfor i (second loop) */

  /* replace the original rule order list with the temp list contents */
  memcpy((L7_char8 *)pRuleOrderInfo, (L7_char8 *)&tempRules, sizeof(tempRules));
  pRuleOrderInfo->ruleCount = k;
}

/*********************************************************************
* @purpose  Indicates whether a maskable class rule field is meaningful  
*           (i.e., has a non-zero mask) for use in a TLV
*
* @param    classIndex     @{(input)}  Class index 
* @param    classRuleIndex @{(input)}  Class rule index 
*
* @returns  L7_SUCCESS  field non-maskable, or masked field is nonzero
* @returns  L7_FAILURE  masked field resolves to zero
*
* @notes    For Distiller internal use only.
*
* @notes    The reason behind this type of checking is to avoid including
*           rules in a TLV whose mask is 0, since this wastes space in the           
*           device classifier table.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvMaskableFieldCheck(L7_uint32 classIndex, 
                                    L7_uint32 classRuleIndex)
{
  L7_RC_t               rc = L7_FAILURE;
  dsmibClassRuleType_t  ruleType;
  L7_uint32             mask32, val;
  L7_ushort16           mask16;
  L7_uchar8             macMask[L7_MAC_ADDR_LEN];
  L7_uchar8             nullMac[L7_MAC_ADDR_LEN] = {0,0,0,0,0,0};

  /* get the class rule entry type */
  if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                 L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE,
                                 &ruleType) != L7_SUCCESS)
    return L7_FAILURE;

  switch (ruleType)
  {

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
    /* get dest IP mask value from class rule match object */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_DSTIP_MASK,
                                   &mask32) == L7_SUCCESS)
    {
      if (mask32 != 0)
        rc = L7_SUCCESS;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
    /* get dest IPv6 prefix length value from class rule match object */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_DSTIPV6_PLEN,
                                   &val) == L7_SUCCESS)
    {
      if (val != 0)
        rc = L7_SUCCESS;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
    /* get dest MAC mask value from class rule match object */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_DSTMAC_MASK,
                                   macMask) == L7_SUCCESS)
    {
      if (memcmp(macMask, nullMac, sizeof(macMask)) != 0)
        rc = L7_SUCCESS;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
    /* get IP TOS mask value from class rule match object */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_IPTOS_MASK,
                                   &mask16) == L7_SUCCESS)
    {
      if (mask16 != 0)
        rc = L7_SUCCESS;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
    /* get protocol number from class rule match object */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_PROTOCOL_NUM,
                                   &val) == L7_SUCCESS)
    {
      /* a protocol of 'ip' implies a mask of 0, since the protocol field
       * is not to be compared in the classifer definition (all other protocol
       * values imply a TLV rule mask of 0xFF)
       */
      if (val != L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IP)
        rc = L7_SUCCESS;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
    /* get src IP mask value from class rule match object */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_SRCIP_MASK,
                                   &mask32) == L7_SUCCESS)
    {
      if (mask32 != 0)
        rc = L7_SUCCESS;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
    /* get source IPv6 prefix length value from class rule match object */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_SRCIPV6_PLEN,
                                   &val) == L7_SUCCESS)
    {
      if (val != 0)
        rc = L7_SUCCESS;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
    /* get src MAC mask value from class rule match object */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_SRCMAC_MASK,
                                   macMask) == L7_SUCCESS)
    {
      if (memcmp(macMask, nullMac, sizeof(macMask)) != 0)
        rc = L7_SUCCESS;
    }
    break;

  default:
    /* all other class rule types are non-maskable fields, so accept them */
    rc = L7_SUCCESS;
    break;

  } /* endswitch */

  return rc;
}

/*********************************************************************
* @purpose  Indicates whether a class rule contains an IP field  
*
* @param    classIndex     @{(input)}  Class index 
* @param    classRuleIndex @{(input)}  Class rule index 
*
* @returns  L7_SUCCESS  rule contains IP field
* @returns  L7_FAILURE  rule does not contain IP field
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvIpFieldCheck(L7_uint32 classIndex, 
                              L7_uint32 classRuleIndex)
{
  L7_RC_t               rc = L7_FAILURE;
  dsmibClassRuleType_t  ruleType;

  /* get the class rule entry type */
  if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                 L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE,
                                 &ruleType) != L7_SUCCESS)
    return L7_FAILURE;

  switch (ruleType)
  {
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
    /* these are the IP classifier fields */
    rc = L7_SUCCESS;
    break;

  default:
    /* all other class rule types are not IP fields */
    break;

  } /* endswitch */

  return rc;
}

/*********************************************************************
* @purpose  Provide the type, length, and value details for the specified
*           class rule
*
* @param    classIndex     @{(input)}  Class index 
* @param    classRuleIndex @{(input)}  Class rule index 
* @param    pTlvWorkInfo   @{(input)}  Pointer to TLV working info struct
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvClassMatchInfoGet(L7_uint32 classIndex, 
                                   L7_uint32 classRuleIndex,
                                   dsDstlTlvWorkInfo_t *pTlvWorkInfo)
{
  dsDstlTlvNative_t     *pBuf = (dsDstlTlvNative_t *)(pTlvWorkInfo->tlvVal);
  dsmibClassRuleType_t  ruleType;
  L7_uint32             tlvType, tlvLen;
  L7_uint32             val;
  L7_ushort16           val16;
  L7_uchar8             val8;

  /* get the class rule entry type */
  if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                 L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE,
                                 &ruleType) != L7_SUCCESS)
    return L7_FAILURE;

  /* initialize TLV working buffer construct */
  memset(pTlvWorkInfo, 0, sizeof(*pTlvWorkInfo));

  /* NOTE: value field stored in NETWORK byte order in work info struct */
  switch (ruleType)
  {

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS_LEN;

      /* get COS value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_COS,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      val8 = (L7_uchar8)val;
      pBuf->cos.cosValue = val8;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS2_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS2_LEN;

      /* get Secondary COS value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_COS2,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      val8 = (L7_uchar8)val;
      pBuf->cos2.cosValue = val8;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIP_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIP_LEN;

      /* get dest IP addr value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_DSTIP_ADDR,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      pBuf->dstIp.ipAddr = (L7_uint32)osapiHtonl(val);

      /* get dest IP mask value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_DSTIP_MASK,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      pBuf->dstIp.ipMask = (L7_uint32)osapiHtonl(val);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIPV6_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIPV6_LEN;

      /* get IPv6 destination address value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_DSTIPV6_ADDR,
                                     &pBuf->dstIpv6.ipv6Addr) != L7_SUCCESS)
        return L7_FAILURE;


      /* get IPv6 destination prefix length value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_DSTIPV6_PLEN,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      (void)sysapiPrefixLenToNetMask(val, sizeof(pBuf->dstIpv6.ipv6Mask), pBuf->dstIpv6.ipv6Mask);  /* ignore rc here */
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTL4PORT_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTL4PORT_LEN;

      /* get dest L4 start port value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_DSTL4PORT_START,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      val16 = (L7_ushort16)val;
      pBuf->dstL4Port.portStart = osapiHtons(val16);

      /* get dest L4 end port value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_DSTL4PORT_END,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      val16 = (L7_ushort16)val;
      pBuf->dstL4Port.portEnd = osapiHtons(val16);

      /* create a layer 4 port mask */
      val16 = (L7_ushort16)L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTL4PORT_MASK;
      pBuf->dstL4Port.portMask = osapiHtons(val16);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTMAC_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTMAC_LEN;

      /* get dest MAC addr value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_DSTMAC_ADDR,
                                     pBuf->dstMac.macAddr) != L7_SUCCESS)
        return L7_FAILURE;

      /* get dest MAC mask value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_DSTMAC_MASK,
                                     pBuf->dstMac.macMask) != L7_SUCCESS)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
    {
      L7_uint32     etypeVal1, etypeVal2;

      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_ETYPE_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_ETYPE_LEN;

      /* NOTE:  Certain Ethertype keywords require two Ethertype values to
       *        cover packets belonging to the protocol.  The second value
       *        is also obtained here and presented to the driver to indicate
       *        a second hardware classifier is needed.
       */
      if (dsmibEtypeKeyidTranslate(classIndex, classRuleIndex, &etypeVal1,
                                   &etypeVal2) != L7_SUCCESS)
        return L7_FAILURE;

      val16 = (L7_ushort16)etypeVal1;
      pBuf->etype.etypeValue1 = osapiHtons(val16);

      val16 = (L7_ushort16)etypeVal2;
      pBuf->etype.etypeValue2 = osapiHtons(val16);

      pBuf->etype.checkStdHdrFlag = L7_FALSE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_EVERY_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_EVERY_LEN;

      /* this TLV contains no native data */
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPV6_FLOWLBL_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPV6_FLOWLBL_LEN;

      /* get IPv6 Flow Label value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_IPV6_FLOWLBL,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      pBuf->ipv6FlowLabel.ipv6FlowLabel = (L7_uint32)osapiHtonl(val);
      val = L7_QOS_DIFFSERV_TLV_MATCH_IPV6FLOWLBL_MASK;
      pBuf->ipv6FlowLabel.ipv6FlowLabelMask = (L7_uint32)osapiHtonl(val);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
    {
      /* NOTE: this function uses the IP TOS TLV definition */
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPTOS_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPTOS_LEN;

      /* get IP DSCP value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_IPDSCP,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      /* store with bits shifted into proper position */
      val8 = (L7_uchar8)val;
      pBuf->ipTos.tosValue = 
        (val8 << L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPDSCP_SHIFT) &
          L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPDSCP_MASK;

      /* create an IP DSCP mask */
      pBuf->ipTos.tosMask = 
        (L7_uchar8)L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPDSCP_MASK;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
    {
      /* NOTE: this function uses the IP TOS TLV definition */
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPTOS_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPTOS_LEN;

      /* get IP Precedence value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_IPPRECEDENCE,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      /* store with bits shifted into proper position */
      val8 = (L7_uchar8)val;
      pBuf->ipTos.tosValue = 
        (val8 << L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPPRECEDENCE_SHIFT) &
          L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPPRECEDENCE_MASK;

      /* create an IP Precedence mask */
      pBuf->ipTos.tosMask = 
        (L7_uchar8)L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPPRECEDENCE_MASK;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPTOS_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPTOS_LEN;

      /* get TOS bits value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_IPTOS_BITS,
                                     &pBuf->ipTos.tosValue) != L7_SUCCESS)
        return L7_FAILURE;

      /* get dest MAC mask value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_IPTOS_MASK,
                                     &pBuf->ipTos.tosMask) != L7_SUCCESS)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_PROTOCOL_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_PROTOCOL_LEN;

      /* get protocol number from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_PROTOCOL_NUM,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      pBuf->protocol.protoNumValue = (L7_uchar8)val;

      /* create a protocol number mask */
      if (val == L7_USMDB_MIB_DIFFSERV_LAYER_4_PROTOCOL_IP)
        val8 = (L7_uchar8)L7_QOS_DIFFSERV_TLV_CLASS_MATCH_PROTOCOL_IP_MASK;
      else
        val8 = (L7_uchar8)L7_QOS_DIFFSERV_TLV_CLASS_MATCH_PROTOCOL_MASK;
      pBuf->protocol.protoNumMask = val8;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS:
    /* refclass rules should not appear in rule order list */
    LOG_MSG("dsDstlTlvClassMatchInfoGet: Rule order list contains ref "
            "class for class index %u, rule index %u\n", 
            classIndex, classRuleIndex);
    return L7_FAILURE;
    /*PASSTHRU*/

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIP_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIP_LEN;

      /* get src IP addr value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_SRCIP_ADDR,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      pBuf->srcIp.ipAddr = (L7_uint32)osapiHtonl(val);

      /* get src IP mask value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_SRCIP_MASK,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      pBuf->srcIp.ipMask = (L7_uint32)osapiHtonl(val);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIPV6_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIPV6_LEN;

      /* get IPv6 destination address value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_SRCIPV6_ADDR,
                                     &pBuf->srcIpv6.ipv6Addr) != L7_SUCCESS)
        return L7_FAILURE;


      /* get IPv6 destination prefix length value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_SRCIPV6_PLEN,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      (void)sysapiPrefixLenToNetMask(val, sizeof(pBuf->srcIpv6.ipv6Mask), pBuf->srcIpv6.ipv6Mask);  /* ignore rc here */
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCL4PORT_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCL4PORT_LEN;

      /* get src L4 start port value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_SRCL4PORT_START,
                                   &val) != L7_SUCCESS)
        return L7_FAILURE;

      val16 = (L7_ushort16)val;
      pBuf->srcL4Port.portStart = osapiHtons(val16);

      /* get src L4 end port value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_SRCL4PORT_END,
                                   &val) != L7_SUCCESS)
        return L7_FAILURE;

      val16 = (L7_ushort16)val;
      pBuf->srcL4Port.portEnd = osapiHtons(val16);

      /* create a layer 4 port mask */
      val16 = (L7_ushort16)L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCL4PORT_MASK;
      pBuf->srcL4Port.portMask = osapiHtons(val16);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCMAC_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCMAC_LEN;

      /* get src MAC addr value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_SRCMAC_ADDR,
                                     pBuf->srcMac.macAddr) != L7_SUCCESS)
        return L7_FAILURE;

      /* get src MAC mask value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_SRCMAC_MASK,
                                     pBuf->srcMac.macMask) != L7_SUCCESS)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID_LEN;

      /* get VLAN ID range start value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_VLANID_START,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      val16 = (L7_ushort16)val;
      pBuf->vlanId.vidStart = osapiHtons(val16);

      /* get VLAN ID range end value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_VLANID_END,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      val16 = (L7_ushort16)val;
      pBuf->vlanId.vidEnd = osapiHtons(val16);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
    {
      tlvType = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID2_TYPE;
      tlvLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID2_LEN;

      /* get Secondary VLAN ID range start value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_VLANID2_START,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      val16 = (L7_ushort16)val;
      pBuf->vlanId2.vidStart = osapiHtons(val16);

      /* get Secondary VLAN ID range end value from class rule match object */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_VLANID2_END,
                                     &val) != L7_SUCCESS)
        return L7_FAILURE;

      val16 = (L7_ushort16)val;
      pBuf->vlanId2.vidEnd = osapiHtons(val16);
    }
    break;

  default:
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

  /* NOTE: type and length stored in HOST byte order in work info struct */
  pTlvWorkInfo->tlvType = tlvType;
  pTlvWorkInfo->tlvLen = tlvLen;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build Policy Definition portion of QOS DiffServ Classifier
*           Instance TLV
*
* @param    tlvHandle       @{(input)} TLV block handle
* @param    policyIndex     @{(input)} Policy index
* @param    policyInstIndex @{(input)} Policy instance index
* @param    intIfNum        @{(input)} Internal interface number   
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @notes    Constructs policy attributes corresponding to a particular
*           policy instance definition.
*
* @notes    Appends to TLV block previously created by the caller.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPolicyDefBuild(L7_tlvHandle_t tlvHandle, 
                                L7_uint32 policyIndex,
                                L7_uint32 policyInstIndex,
                                L7_uint32 intIfNum)
{
  L7_uint32             attrCount = 0;
  L7_RC_t               rc;
  L7_uint32             policyAttrIndex;
  L7_uint32             nextPolicy, nextInst;
  dsDstlTlvWorkInfo_t   tlvWorkInfo;

  /* open the Policy Definition TLV
   *
   * NOTE: There is at least one attribute TLV presented to the low level
   *       code.  If there are no defined attributes for this instance,
   *       a "best-effort (low)" attribute is created.
   */
  if ((rc = tlvOpen(tlvHandle, L7_QOS_DIFFSERV_TLV_POLICY_DEF_TYPE,
                    L7_QOS_DIFFSERV_TLV_POLICY_DEF_LEN,
                    L7_NULLPTR)) != L7_SUCCESS)
    return rc;

  /* walk the list of attributes for this policy instance */
  policyAttrIndex = 0;                          /* start with first attribute */
  while ((diffServPolicyAttrGetNext(policyIndex, policyInstIndex, 
            policyAttrIndex, &nextPolicy, &nextInst, &policyAttrIndex)
          == L7_SUCCESS) &&
         (nextPolicy == policyIndex) && (nextInst == policyInstIndex))
  {
    /* increment the count before building the attribute TLV(s) -- prevents 
     * building the default "best-effort" TLV below if there is some
     * kind of failure here
     */
    attrCount++;

    /* build the TLV(s) for this policy attribute */
    rc = dsDstlTlvPolicyAttrBuild(tlvHandle, policyIndex, 
                                  policyInstIndex, policyAttrIndex, 
                                  intIfNum, &tlvWorkInfo);

    /* special case:  if a mirror or redirect attr target interface is
     *                not currently attached, as indicated by L7_NOT_EXIST
     *                rc, pretend the attr doesn't exist for purposes of
     *                building a best-effort TLV
     */
    if (rc == L7_NOT_EXIST)
    {
      attrCount--;
      rc = L7_SUCCESS;                  /* treat this case same as success */
    }

    if (rc != L7_SUCCESS)
      break;

  } /* endwhile walk attributes */

  /* if there are no defined attributes, create a "best-effort (low)" TLV 
   * instead
   */
  if (attrCount == 0)
    rc = dsDstlTlvPolicyBestEffortBuild(tlvHandle, &tlvWorkInfo);

  /* if all went well, close the Policy Definition TLV */
  if (rc == L7_SUCCESS)
    rc = tlvClose(tlvHandle, L7_QOS_DIFFSERV_TLV_POLICY_DEF_TYPE);

  return rc;
}

/*********************************************************************
* @purpose  Build the appropriate TLV(s) for the specified policy attribute
*
* @param    tlvHandle       @{(input)} TLV block handle   
* @param    policyIndex     @{(input)} Policy index 
* @param    policyInstIndex @{(input)} Policy instance index 
* @param    policyAttrIndex @{(input)} Policy attribute index 
* @param    intIfNum        @{(input)} Internal interface number   
* @param    pTlvWorkInfo    @{(input)} Pointer to TLV working info struct
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        Problem with TLV
* @returns  L7_NOT_EXIST    Mirror or redirect interface not attached
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPolicyAttrBuild(L7_tlvHandle_t tlvHandle,
                                 L7_uint32 policyIndex, 
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex,
                                 L7_uint32 intIfNum,
                                 dsDstlTlvWorkInfo_t *pTlvWorkInfo)
{
  L7_RC_t               rc = L7_FAILURE;
  L7_BOOL               immedExit = L7_FALSE;
  dsDstlTlvNative_t     *pBuf = (dsDstlTlvNative_t *)(pTlvWorkInfo->tlvVal);
  dsmibPolicyAttrType_t attrType;
  L7_uint32             val;
  L7_uchar8             val8;
  nimConfigID_t         configId;
  nimUSP_t              nimUsp;

  /* NOTE: This function assumes the private MIB code has done its job to
   *       ensure that all attributes for this policy instance are valid
   *       and there are no attribute type conflicts.
   */

  /* get the policy attribute entry type */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                  L7_DIFFSERV_POLICY_ATTR_STMT_ENTRY_TYPE,
                                  &attrType) != L7_SUCCESS)
    return L7_FAILURE;

  /* initialize TLV working buffer construct */
  memset(pTlvWorkInfo, 0, sizeof(*pTlvWorkInfo));

  /* NOTE: Value field stored in NETWORK byte order in work info struct. */
  switch (attrType)
  {

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
    {
      pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_ASSIGN_QUEUE_TYPE;
      pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_ASSIGN_QUEUE_LEN;

      /* get assigned queue id value from policy attribute object */
      if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex,
                                      policyAttrIndex,
                                      L7_DIFFSERV_POLICY_ATTR_STMT_ASSIGN_QUEUE_QID,
                                      &val) != L7_SUCCESS)
        return L7_FAILURE;

      val8 = (L7_uchar8)val;
      pBuf->assignQueue.qid = val8;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
    {
      pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_DROP_TYPE;
      pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_DROP_LEN;

      /* this TLV contains no native data */
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
    {
      pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_TYPE;
      pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_LEN;

      /* get cos mark value from policy attribute object */
      if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex,
                                      policyAttrIndex,
                                      L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COSVAL,
                                      &val) != L7_SUCCESS)
        return L7_FAILURE;

      val8 = (L7_uchar8)val;
      pBuf->markCos.cosMark = val8;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
    {
      pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_AS_COS2_TYPE;
      pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_AS_COS2_LEN;

      /* get cos mark value from policy attribute object */
      if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex,
                                      policyAttrIndex,
                                      L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COS_AS_COS2,
                                      &val) != L7_SUCCESS)
        return L7_FAILURE;

      pBuf->markCosAsCos2.cosMarkAsCos2= val;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
    {
      pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COS2VAL_TYPE;
      pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COS2VAL_LEN;

      /* get secondary cos mark value from policy attribute object */
      if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex,
                                      policyAttrIndex,
                                      L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COS2VAL,
                                      &val) != L7_SUCCESS)
        return L7_FAILURE;

      val8 = (L7_uchar8)val;
      pBuf->markCos2.cosMark = val8;
    }
    break;


  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
    {
      pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPDSCPVAL_TYPE;
      pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPDSCPVAL_LEN;

      /* get IP DSCP value from policy attribute object */
      if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex,
                                      policyAttrIndex,
                                      L7_DIFFSERV_POLICY_ATTR_STMT_MARK_IPDSCPVAL,
                                      &val) != L7_SUCCESS)
        return L7_FAILURE;

      val8 = (L7_uchar8)val;
      pBuf->markIpDscp.dscpMark = val8;
    }
    break;


  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
    {
      pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_TYPE;
      pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_LEN;

      /* get IP Precedence value from policy attribute object */
      if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex,
                                      policyAttrIndex,
                                      L7_DIFFSERV_POLICY_ATTR_STMT_MARK_IPPRECEDENCEVAL,
                                      &val) != L7_SUCCESS)
        return L7_FAILURE;

      val8 = (L7_uchar8)val;
      pBuf->markIpPrecedence.ipPrecMark = val8;
    }
    break;


  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
    {
      pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MIRROR_TYPE;
      pTlvWorkInfo->tlvLen  = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MIRROR_LEN;

      /* get mirror configId from policy attribute object */
      if (diffServPolicyAttrObjectGet(policyIndex,
                                      policyInstIndex,
                                      policyAttrIndex,
                                      L7_DIFFSERV_POLICY_ATTR_STMT_MIRROR_INTF,
                                      &configId) != L7_SUCCESS)
        return L7_FAILURE;

      /* only tell HAPI if target interface is in attaching or attached state
       * (use L7_NOT_EXIST rc here to signal special condition to the caller)
       */
      if (diffServPolicyAttrTargetIntfIsUsable(policyIndex,
                                               policyInstIndex,
                                               policyAttrIndex,
                                               L7_DIFFSERV_POLICY_ATTR_STMT_MIRROR_INTF)
          != L7_TRUE)
        return L7_NOT_EXIST;

      /* translate configId into unit, slot, port */
      if (nimUspFromConfigIDGet(&configId, &nimUsp) != L7_SUCCESS)
        return L7_FAILURE;

      val = nimUsp.unit;
      pBuf->mirror.intfUnit = (L7_uint32)osapiHtonl(val);
      val = nimUsp.slot;
      pBuf->mirror.intfSlot = (L7_uint32)osapiHtonl(val);
      val = nimUsp.port - 1;                    /* driver needs 0-based port */
      pBuf->mirror.intfPort = (L7_uint32)osapiHtonl(val);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
    {
      /* build all policing TLVs then exit immediately */
      rc = dsDstlTlvPoliceSimple(tlvHandle, policyIndex,
                                 policyInstIndex, policyAttrIndex,
                                 pTlvWorkInfo);
      immedExit = L7_TRUE;
    }
    break;


  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
    {
      /* build all policing TLVs then exit immediately */
      rc = dsDstlTlvPoliceSingleRate(tlvHandle, policyIndex,
                                     policyInstIndex, policyAttrIndex,
                                     pTlvWorkInfo);
      immedExit = L7_TRUE;
    }
    break;


  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
    {
      /* build all policing TLVs then exit immediately */
      rc = dsDstlTlvPoliceTwoRate(tlvHandle, policyIndex,
                                  policyInstIndex, policyAttrIndex,
                                  pTlvWorkInfo);
      immedExit = L7_TRUE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
    {
      pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_REDIRECT_TYPE;
      pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_REDIRECT_LEN;

      /* get redirect configId from policy attribute object */
      if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex,
                                      policyAttrIndex,
                                      L7_DIFFSERV_POLICY_ATTR_STMT_REDIRECT_INTF,
                                      &configId) != L7_SUCCESS)
        return L7_FAILURE;

      /* only tell HAPI if target interface is in attaching or attached state
       * (use L7_NOT_EXIST rc here to signal special condition to the caller)
       */
      if (diffServPolicyAttrTargetIntfIsUsable(policyIndex,
                                               policyInstIndex,
                                               policyAttrIndex,
                                               L7_DIFFSERV_POLICY_ATTR_STMT_REDIRECT_INTF)
          != L7_TRUE)
        return L7_NOT_EXIST;

      /* translate configId into unit, slot, port */
      if (nimUspFromConfigIDGet(&configId, &nimUsp) != L7_SUCCESS)
        return L7_FAILURE;

      val = nimUsp.unit;
      pBuf->redirect.intfUnit = (L7_uint32)osapiHtonl(val);
      val = nimUsp.slot;
      pBuf->redirect.intfSlot = (L7_uint32)osapiHtonl(val);
      val = nimUsp.port - 1;                    /* driver needs 0-based port */
      pBuf->redirect.intfPort = (L7_uint32)osapiHtonl(val);
    }
    break;

  default:
    rc = L7_FAILURE;
    immedExit = L7_TRUE;
    break;

  } /* endswitch */

  if (immedExit == L7_FALSE)
  {
    /* making it this far means there is a TLV to write */
    rc = tlvWrite(tlvHandle, pTlvWorkInfo->tlvType, pTlvWorkInfo->tlvLen, 
                  pTlvWorkInfo->tlvVal);
  }

  return rc;
}

/*********************************************************************
* @purpose  Provide the type, length, and value details for a 
*           best-effort (low) policy attribute
*
* @param    tlvHandle     @{(input)} TLV block handle
* @param    pTlvWorkInfo  @{(input)} Pointer to TLV working info struct
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @notes    This function is used to create a best-effort (low) Bandwidth
*           Allocation TLV when there are no attributes specified for a
*           policy instance.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPolicyBestEffortBuild(L7_tlvHandle_t tlvHandle, 
                                       dsDstlTlvWorkInfo_t *pTlvWorkInfo)
{
  dsDstlTlvNative_t *pBuf = (dsDstlTlvNative_t *)(pTlvWorkInfo->tlvVal);
  L7_uint32         val;
  L7_uchar8         val8;
  L7_RC_t           rc;

  /* initialize TLV working buffer construct */
  memset(pTlvWorkInfo, 0, sizeof(*pTlvWorkInfo));

  pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_BW_ALLOC_TYPE;
  pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_BW_ALLOC_LEN;

  /* minimum bandwidth is not specified */
  val = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_BANDWIDTH_MIN_NONE;
  pBuf->bwAlloc.minBw = (L7_uint32)osapiHtonl(val);

  /* maximum bandwidth is unlimited */
  val = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_BANDWIDTH_MAX_UNLIM;
  pBuf->bwAlloc.maxBw = (L7_uint32)osapiHtonl(val);

  /* maximum burst size is set from factory default */
  val = FD_QOS_DIFFSERV_EXPEDITE_CBURST;
  pBuf->bwAlloc.maxBurst = (L7_uint32)osapiHtonl(val);

  /* use best-effort (low) scheduling */
  val8 = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_SCHEDULER_BE_LO;
  pBuf->bwAlloc.schedType = val8;

  /* write the best-effort Bandwidth Allocation TLV */
  rc = tlvWrite(tlvHandle, pTlvWorkInfo->tlvType, pTlvWorkInfo->tlvLen, 
                pTlvWorkInfo->tlvVal);

  return rc;
}

/*********************************************************************
* @purpose  Extract the configuration parameters for simple policing
*           and build the appropriate set of TLVs
*
* @param    tlvHandle       @{(input)} TLV block handle   
* @param    policyIndex     @{(input)} Policy index 
* @param    policyInstIndex @{(input)} Policy instance index 
* @param    policyAttrIndex @{(input)} Policy attribute index 
* @param    pTlvWorkInfo    @{(input)} Pointer to TLV working info struct
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPoliceSimple(L7_tlvHandle_t tlvHandle,
                              L7_uint32 policyIndex, 
                              L7_uint32 policyInstIndex,
                              L7_uint32 policyAttrIndex,
                              dsDstlTlvWorkInfo_t *pTlvWorkInfo)
{
  dsDstlTlvNative_t *pBuf = (dsDstlTlvNative_t *)(pTlvWorkInfo->tlvVal);
  L7_uint32         colorMode, colorValue;
  L7_uchar8         colorType;
  L7_uint32         dataRate, burstSize;
  dsmibPoliceAct_t  action;
  L7_uint32         markAct, markVal;
  L7_RC_t           rc;


  /* initialize TLV working buffer construct */
  memset(pTlvWorkInfo, 0, sizeof(*pTlvWorkInfo));

  /* set type and length */
  pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_TYPE;
  pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_LEN;

  /* get color conform mode from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_COLOR_CONFORM_MODE,
                                  &colorMode) != L7_SUCCESS)
    return L7_FAILURE;

  /* build Police Policy Definition native TLV fields (in network byte order) */
  pBuf->policeDef.style = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_STYLE_SIMPLE;

  if (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND)
  {
    /* in color-blind mode, all other color-related fields remain set to zero */
    pBuf->policeDef.colorMode = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORMODE_BLIND;
  }
  else
  {
    pBuf->policeDef.colorMode = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORMODE_AWARE;

    /* translate color conform mode into TLV value */ 
    switch (colorMode)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_COS;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_COS2;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPDSCP;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPPREC;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED:
    default:
      return L7_FAILURE;
      /*PASSTHRU*/
    }

    /* get color conform value from policy attribute object */
    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_COLOR_CONFORM_VAL,
                                    &colorValue) != L7_SUCCESS)
      return L7_FAILURE;

    pBuf->policeDef.colorConformType = colorType;
    pBuf->policeDef.colorConformValue = (L7_uchar8)colorValue;

    /* color exceed values not used for simple policing */
    pBuf->policeDef.colorExceedType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_UNUSED;
    pBuf->policeDef.colorExceedValue = 0;

  } /* endelse color aware */

  /* open the container Policing Policy TLV */
  if ((rc = tlvOpen(tlvHandle, pTlvWorkInfo->tlvType, pTlvWorkInfo->tlvLen,
                    pTlvWorkInfo->tlvVal)) != L7_SUCCESS)
    return rc;

  /* get committed rate from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                          L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CRATE,
                                  &dataRate) != L7_SUCCESS)
    return L7_FAILURE;

  /* get committed burst from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                          L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CBURST,
                                  &burstSize) != L7_SUCCESS)
    return L7_FAILURE;

  /* get conform action from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CONFORM_ACT,
                                  &action) != L7_SUCCESS)
    return L7_FAILURE;

  /* translate action to a TLV mark action flag value */
  markAct = dsDstlTlvPoliceActionXlate(action);

  /* get conform mark value from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CONFORM_VAL,
                                  &markVal) != L7_SUCCESS)
    return L7_FAILURE;

  /* build conform TLVs */
  if ((rc = dsDstlTlvPoliceConformBuild(tlvHandle, dataRate, burstSize,
                                        markAct, markVal, pTlvWorkInfo))
       != L7_SUCCESS)
    return rc;

  /* get nonconform action from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_NONCONFORM_ACT,
                                  &action) != L7_SUCCESS)
    return L7_FAILURE;

  /* translate action to a TLV mark action flag value */
  markAct = dsDstlTlvPoliceActionXlate(action);

  /* get nonconform mark value from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_NONCONFORM_VAL,
                                  &markVal) != L7_SUCCESS)
    return L7_FAILURE;

  /* build nonconform TLVs */
  if ((rc = dsDstlTlvPoliceNonconformBuild(tlvHandle, markAct, markVal, 
                                           pTlvWorkInfo))
       != L7_SUCCESS)
    return rc;

  /* close the Policy Definition TLV */
  rc = tlvClose(tlvHandle, L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_TYPE);

  return rc;
}

/*********************************************************************
* @purpose  Extract the configuration parameters for single-rate policing
*           and build the appropriate set of TLVs
*
* @param    tlvHandle       @{(input)} TLV block handle   
* @param    policyIndex     @{(input)} Policy index 
* @param    policyInstIndex @{(input)} Policy instance index 
* @param    policyAttrIndex @{(input)} Policy attribute index 
* @param    pTlvWorkInfo    @{(input)} Pointer to TLV working info struct
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPoliceSingleRate(L7_tlvHandle_t tlvHandle,
                                  L7_uint32 policyIndex, 
                                  L7_uint32 policyInstIndex,
                                  L7_uint32 policyAttrIndex,
                                  dsDstlTlvWorkInfo_t *pTlvWorkInfo)
{
  dsDstlTlvNative_t *pBuf = (dsDstlTlvNative_t *)(pTlvWorkInfo->tlvVal);
  L7_uint32         colorMode, colorValue;
  L7_uchar8         colorType;
  L7_uint32         dataRate, burstSize;
  dsmibPoliceAct_t  action;
  L7_uint32         markAct, markVal;
  L7_RC_t           rc;

  /* initialize TLV working buffer construct */
  memset(pTlvWorkInfo, 0, sizeof(*pTlvWorkInfo));

  /* set type and length */
  pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_TYPE;
  pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_LEN;

  /* get color conform mode from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_CONFORM_MODE,
                                  &colorMode) != L7_SUCCESS)
    return L7_FAILURE;

  /* build Police Policy Definition native TLV fields (in network byte order) */
  pBuf->policeDef.style = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_STYLE_SINGLERATE;

  if (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND)
  {
    /* in color-blind mode, all other color-related fields remain set to zero */
    pBuf->policeDef.colorMode = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORMODE_BLIND;
  }
  else
  {
    pBuf->policeDef.colorMode = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORMODE_AWARE;

    /* translate color conform mode into TLV value */ 
    switch (colorMode)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_COS;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_COS2;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPDSCP;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPPREC;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED:
    default:
      return L7_FAILURE;
      /*PASSTHRU*/
    }

    /* get color conform value from policy attribute object */
    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_CONFORM_VAL,
                                    &colorValue) != L7_SUCCESS)
      return L7_FAILURE;

    pBuf->policeDef.colorConformType = colorType;
    pBuf->policeDef.colorConformValue = (L7_uchar8)colorValue;

    /* get color exceed mode from policy attribute object */
    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_EXCEED_MODE,
                                    &colorMode) != L7_SUCCESS)
      return L7_FAILURE;

    /* translate color exceed mode into TLV value */ 
    switch (colorMode)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_COS;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_COS2;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPDSCP;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPPREC;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND:
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_UNUSED;
      break;
    default:
      return L7_FAILURE;
      /*PASSTHRU*/
    }

    /* get color exceed value from policy attribute object */
    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_EXCEED_VAL,
                                    &colorValue) != L7_SUCCESS)
      return L7_FAILURE;

    pBuf->policeDef.colorExceedType = colorType;
    pBuf->policeDef.colorExceedValue = (L7_uchar8)colorValue;

  } /* endelse color aware */

  /* open the container Policing Policy TLV */
  if ((rc = tlvOpen(tlvHandle, pTlvWorkInfo->tlvType, pTlvWorkInfo->tlvLen,
                    pTlvWorkInfo->tlvVal)) != L7_SUCCESS)
    return rc;

  /* get committed rate from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                          L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CRATE,
                                  &dataRate) != L7_SUCCESS)
    return L7_FAILURE;

  /* get committed burst from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                          L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CBURST,
                                  &burstSize) != L7_SUCCESS)
    return L7_FAILURE;

  /* get conform action from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CONFORM_ACT,
                                  &action) != L7_SUCCESS)
    return L7_FAILURE;

  /* translate action to a TLV mark action flag value */
  markAct = dsDstlTlvPoliceActionXlate(action);

  /* get conform mark value from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CONFORM_VAL,
                                  &markVal) != L7_SUCCESS)
    return L7_FAILURE;

  /* build conform TLVs */
  if ((rc = dsDstlTlvPoliceConformBuild(tlvHandle, dataRate, burstSize,
                                        markAct, markVal, pTlvWorkInfo))
       != L7_SUCCESS)
    return rc;

  /* get excess burst from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                          L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EBURST,
                                  &burstSize) != L7_SUCCESS)
    return L7_FAILURE;

  /* get exceed action from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EXCEED_ACT,
                                  &action) != L7_SUCCESS)
    return L7_FAILURE;

  /* translate action to a TLV mark action flag value */
  markAct = dsDstlTlvPoliceActionXlate(action);

  /* get exceed mark value from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EXCEED_VAL,
                                  &markVal) != L7_SUCCESS)
    return L7_FAILURE;

  /* build exceed TLVs (uses same dataRate as conform) */
  if ((rc = dsDstlTlvPoliceExceedBuild(tlvHandle, dataRate, burstSize,
                                       markAct, markVal, pTlvWorkInfo))
       != L7_SUCCESS)
    return rc;

  /* get nonconform action from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_NONCONFORM_ACT,
                                  &action) != L7_SUCCESS)
    return L7_FAILURE;

  /* translate action to a TLV mark action flag value */
  markAct = dsDstlTlvPoliceActionXlate(action);

  /* get nonconform mark value from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_NONCONFORM_VAL,
                                  &markVal) != L7_SUCCESS)
    return L7_FAILURE;

  /* build nonconform TLVs */
  if ((rc = dsDstlTlvPoliceNonconformBuild(tlvHandle, markAct, markVal, 
                                           pTlvWorkInfo))
       != L7_SUCCESS)
    return rc;

  /* close the Policy Definition TLV */
  rc = tlvClose(tlvHandle, L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_TYPE);

  return rc;
}

/*********************************************************************
* @purpose  Extract the configuration parameters for two-rate policing
*           and build the appropriate set of TLVs
*
* @param    tlvHandle       @{(input)} TLV block handle   
* @param    policyIndex     @{(input)} Policy index 
* @param    policyInstIndex @{(input)} Policy instance index 
* @param    policyAttrIndex @{(input)} Policy attribute index 
* @param    pTlvWorkInfo    @{(input)} Pointer to TLV working info struct
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPoliceTwoRate(L7_tlvHandle_t tlvHandle,
                               L7_uint32 policyIndex, 
                               L7_uint32 policyInstIndex,
                               L7_uint32 policyAttrIndex,
                               dsDstlTlvWorkInfo_t *pTlvWorkInfo)
{
  dsDstlTlvNative_t *pBuf = (dsDstlTlvNative_t *)(pTlvWorkInfo->tlvVal);
  L7_uint32         colorMode, colorValue;
  L7_uchar8         colorType;
  L7_uint32         dataRate, burstSize;
  dsmibPoliceAct_t  action;
  L7_uint32         markAct, markVal;
  L7_RC_t           rc;

  /* initialize TLV working buffer construct */
  memset(pTlvWorkInfo, 0, sizeof(*pTlvWorkInfo));

  /* set type and length */
  pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_TYPE;
  pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_LEN;

  /* get color conform mode from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_CONFORM_MODE,
                                  &colorMode) != L7_SUCCESS)
    return L7_FAILURE;

  /* build Police Policy Definition native TLV fields (in network byte order) */
  pBuf->policeDef.style = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_STYLE_TWORATE;

  if (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND)
  {
    /* in color-blind mode, all other color-related fields remain set to zero */
    pBuf->policeDef.colorMode = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORMODE_BLIND;
  }
  else
  {
    pBuf->policeDef.colorMode = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORMODE_AWARE;

    /* translate color conform mode into TLV value */ 
    switch (colorMode)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_COS;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_COS2;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPDSCP;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPPREC;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED:
    default:
      return L7_FAILURE;
      /*PASSTHRU*/
    }

    /* get color conform value from policy attribute object */
    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_CONFORM_VAL,
                                    &colorValue) != L7_SUCCESS)
      return L7_FAILURE;

    pBuf->policeDef.colorConformType = colorType;
    pBuf->policeDef.colorConformValue = (L7_uchar8)colorValue;

    /* get color exceed mode from policy attribute object */
    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_EXCEED_MODE,
                                    &colorMode) != L7_SUCCESS)
      return L7_FAILURE;

    /* translate color exceed mode into TLV value */ 
    switch (colorMode)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_COS;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_COS2;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPDSCP;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_IPPREC;
      break;
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND:
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED:
      colorType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_COLORAWARE_UNUSED;
      break;
    default:
      return L7_FAILURE;
      /*PASSTHRU*/
    }

    /* get color exceed value from policy attribute object */
    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_EXCEED_VAL,
                                    &colorValue) != L7_SUCCESS)
      return L7_FAILURE;

    pBuf->policeDef.colorExceedType = colorType;
    pBuf->policeDef.colorExceedValue = (L7_uchar8)colorValue;

  } /* endelse color aware */

  /* open the container Policing Policy TLV */
  if ((rc = tlvOpen(tlvHandle, pTlvWorkInfo->tlvType, pTlvWorkInfo->tlvLen,
                    pTlvWorkInfo->tlvVal)) != L7_SUCCESS)
    return rc;

  /* get committed rate from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                          L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CRATE,
                                  &dataRate) != L7_SUCCESS)
    return L7_FAILURE;

  /* get committed burst from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                          L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CBURST,
                                  &burstSize) != L7_SUCCESS)
    return L7_FAILURE;

  /* get conform action from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CONFORM_ACT,
                                  &action) != L7_SUCCESS)
    return L7_FAILURE;

  /* translate action to a TLV mark action flag value */
  markAct = dsDstlTlvPoliceActionXlate(action);

  /* get conform mark value from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CONFORM_VAL,
                                  &markVal) != L7_SUCCESS)
    return L7_FAILURE;

  /* build conform TLVs */
  if ((rc = dsDstlTlvPoliceConformBuild(tlvHandle, dataRate, burstSize,
                                        markAct, markVal, pTlvWorkInfo))
       != L7_SUCCESS)
    return rc;

  /* get peak rate from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                          L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_PRATE,
                                  &dataRate) != L7_SUCCESS)
    return L7_FAILURE;

  /* get peak burst from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                          L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_PBURST,
                                  &burstSize) != L7_SUCCESS)
    return L7_FAILURE;

  /* get exceed action from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_EXCEED_ACT,
                                  &action) != L7_SUCCESS)
    return L7_FAILURE;

  /* translate action to a TLV mark action flag value */
  markAct = dsDstlTlvPoliceActionXlate(action);

  /* get exceed mark value from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                    L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_EXCEED_VAL,
                                  &markVal) != L7_SUCCESS)
    return L7_FAILURE;

  /* build exceed TLVs (uses same dataRate as conform) */
  if ((rc = dsDstlTlvPoliceExceedBuild(tlvHandle, dataRate, burstSize,
                                       markAct, markVal, pTlvWorkInfo))
       != L7_SUCCESS)
    return rc;

  /* get nonconform action from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_NONCONFORM_ACT,
                                  &action) != L7_SUCCESS)
    return L7_FAILURE;

  /* translate action to a TLV mark action flag value */
  markAct = dsDstlTlvPoliceActionXlate(action);

  /* get nonconform mark value from policy attribute object */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                  L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_NONCONFORM_VAL,
                                  &markVal) != L7_SUCCESS)
    return L7_FAILURE;

  /* build nonconform TLVs */
  if ((rc = dsDstlTlvPoliceNonconformBuild(tlvHandle, markAct, markVal, 
                                           pTlvWorkInfo))
       != L7_SUCCESS)
    return rc;

  /* close the Policy Definition TLV */
  rc = tlvClose(tlvHandle, L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_TYPE);

  return rc;
}

/*********************************************************************
* @purpose  Build the Police Conform TLV
*
* @param    tlvHandle     @{(input)} TLV block handle   
* @param    dataRate      @{(input)} Data rate    
* @param    burstSize     @{(input)} Burst size             
* @param    markAct       @{(input)} Mark action flag
* @param    markVal       @{(input)} Mark value               
* @param    pTlvWorkInfo  @{(input)} Pointer to TLV working info struct
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPoliceConformBuild(L7_tlvHandle_t tlvHandle,
                                    L7_uint32 dataRate,
                                    L7_uint32 burstSize,
                                    L7_uint32 markAct,
                                    L7_uint32 markVal,
                                    dsDstlTlvWorkInfo_t *pTlvWorkInfo)
{
  dsDstlTlvNative_t *pBuf = (dsDstlTlvNative_t *)(pTlvWorkInfo->tlvVal);
  L7_RC_t           rc;

  /* initialize TLV working buffer construct */
  memset(pTlvWorkInfo, 0, sizeof(*pTlvWorkInfo));

  /* set type and length */
  pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_CONFORM_TYPE;
  pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_CONFORM_LEN;

  /* build value field (in network byte order) */
  pBuf->policeConform.dataRate = (L7_uint32)osapiHtonl(dataRate);
  pBuf->policeConform.burstSize = (L7_uint32)osapiHtonl(burstSize);
  pBuf->policeConform.actionFlag = (L7_uchar8)markAct;
  pBuf->policeConform.markValue = (L7_uchar8)markVal;

  /* write the Police Conform TLV */
  if ((rc = tlvWrite(tlvHandle, pTlvWorkInfo->tlvType, pTlvWorkInfo->tlvLen,
                     pTlvWorkInfo->tlvVal)) != L7_SUCCESS)
    return rc;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build the Police Exceed TLV
*
* @param    tlvHandle     @{(input)} TLV block handle   
* @param    dataRate      @{(input)} Data rate    
* @param    burstSize     @{(input)} Burst size             
* @param    markAct       @{(input)} Mark action flag
* @param    markVal       @{(input)} Mark value               
* @param    pTlvWorkInfo  @{(input)} Pointer to TLV working info struct
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPoliceExceedBuild(L7_tlvHandle_t tlvHandle,
                                   L7_uint32 dataRate,
                                   L7_uint32 burstSize,
                                   L7_uint32 markAct,
                                   L7_uint32 markVal,
                                   dsDstlTlvWorkInfo_t *pTlvWorkInfo)
{
  dsDstlTlvNative_t *pBuf = (dsDstlTlvNative_t *)(pTlvWorkInfo->tlvVal);
  L7_RC_t           rc;

  /* initialize TLV working buffer construct */
  memset(pTlvWorkInfo, 0, sizeof(*pTlvWorkInfo));

  /* set type and length */
  pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_EXCEED_TYPE;
  pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_EXCEED_LEN;

  /* build value field (in network byte order) */
  pBuf->policeExceed.dataRate = (L7_uint32)osapiHtonl(dataRate);
  pBuf->policeExceed.burstSize = (L7_uint32)osapiHtonl(burstSize);
  pBuf->policeExceed.actionFlag = (L7_uchar8)markAct;
  pBuf->policeExceed.markValue = (L7_uchar8)markVal;

  /* write the Police Exceed TLV */
  if ((rc = tlvWrite(tlvHandle, pTlvWorkInfo->tlvType, pTlvWorkInfo->tlvLen,
                     pTlvWorkInfo->tlvVal)) != L7_SUCCESS)
    return rc;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build the Police Nonconform TLV
*
* @param    tlvHandle     @{(input)} TLV block handle   
* @param    markAct       @{(input)} Mark action flag
* @param    markVal       @{(input)} Mark value               
* @param    pTlvWorkInfo  @{(input)} Pointer to TLV working info struct
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvPoliceNonconformBuild(L7_tlvHandle_t tlvHandle,
                                       L7_uint32 markAct,
                                       L7_uint32 markVal,
                                       dsDstlTlvWorkInfo_t *pTlvWorkInfo)
{
  dsDstlTlvNative_t *pBuf = (dsDstlTlvNative_t *)(pTlvWorkInfo->tlvVal);
  L7_RC_t           rc;

  /* initialize TLV working buffer construct */
  memset(pTlvWorkInfo, 0, sizeof(*pTlvWorkInfo));

  /* set type and length */
  pTlvWorkInfo->tlvType = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_NONCONFORM_TYPE;
  pTlvWorkInfo->tlvLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_NONCONFORM_LEN;

  /* build value field (in network byte order) */
  pBuf->policeNonconform.actionFlag = (L7_uchar8)markAct;
  pBuf->policeNonconform.markValue = (L7_uchar8)markVal;

  /* write the Police Nonconform TLV */
  if ((rc = tlvWrite(tlvHandle, pTlvWorkInfo->tlvType, pTlvWorkInfo->tlvLen,
                     pTlvWorkInfo->tlvVal)) != L7_SUCCESS)
    return rc;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Translates MIB object police action value to TLV action flag
*
* @param    action      @{(input)} Action value from MIB
*
* @returns  markAct     TLV mark action flag
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
L7_uint32 dsDstlTlvPoliceActionXlate(dsmibPoliceAct_t action) 
{
  L7_uint32     markAct;

  switch (action)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
    markAct = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_DROP;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
    markAct = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKCOS;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
    markAct = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKCOS_AS_COS2;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
    markAct = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKCOS2;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
    markAct = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKDSCP;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
    markAct = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_MARKPREC;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
  default:
    markAct = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_ACT_SEND;
    break;
  }

  return markAct;
}

/*********************************************************************
* @purpose  Provide instance key value for specified instance indexes
*
* @param    policyIndex     @{(input)}  Policy index
* @param    policyInstIndex @{(input)}  Policy instance index   
* @param    pKey            @{(output)} Pointer to key output location
*
* @returns  void
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
void dsDstlTlvInstToKey(L7_uint32 policyIndex, 
                        L7_uint32 policyInstIndex, 
                        L7_uint32 *pKey)
{
  *pKey = pDsDstlCtrl->policy[policyIndex].inst2KeyTable[policyInstIndex];
}

/*********************************************************************
* @purpose  Provide instance indexes for specified instance key value
*
* @param    key              @{(input)}  Instance key
* @param    pPolicyIndex     @{(output)} Pointer to policy index output location
* @param    pPolicyInstIndex @{(output)} Pointer to policy instance index output
*                                          location
*
* @returns  void
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
void dsDstlTlvKeyToInst(L7_uint32 key, 
                        L7_uint32 *pPolicyIndex,
                        L7_uint32 *pPolicyInstIndex)
{
  *pPolicyIndex = pDsDstlCtrl->key2InstTable[key].policyIndex;
  *pPolicyInstIndex = pDsDstlCtrl->key2InstTable[key].policyInstIndex;
}

/*********************************************************************
* @purpose  Parse and display TLV contents
*
* @param    pTlv        @{(input)} TLV start location
* @param    intIfNum    @{(input)} Internal interface number (or 0)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Requires a DiffServ message level of 'LO' to show any output.
*
* @notes    Specify the intIfNum parameter as 0 if this TLV is not 
*           associated with a particular interface.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvParse(L7_tlv_t *pTlv, L7_uint32 intIfNum)
{
  L7_uint32     msgLvlReqd;

  /* set minimum required message level for showing any output */
  msgLvlReqd = DIFFSERV_MSGLVL_MED;

  /* check for the desired message level setting up front before displaying */
  if ((pDiffServInfo_g == L7_NULLPTR) ||
      (pDiffServInfo_g->diffServMsgLvl < msgLvlReqd))
    return L7_SUCCESS;

  /* print a title line */
  if (intIfNum != 0)
  {
    DIFFSERV_PRT(msgLvlReqd, "\nParsing TLV at location 0x%8.8x (intf %u):\n\n",
                 (L7_uint32)pTlv, intIfNum);
  }
  else
  {
    DIFFSERV_PRT(msgLvlReqd, "\nParsing TLV at location 0x%8.8x:\n\n",  
                 (L7_uint32)pTlv);
  }

  /* use the TLV utility to traverse the TLV
   * (it uses our designated show function to display the TLV contents)
   */
  if (tlvParse(pTlv, dsDstlTlvEntryDisplay) != L7_SUCCESS)
  {
    DIFFSERV_PRT(msgLvlReqd,
                 "\n  >>> Error while parsing contents of TLV type=0x%4.4x\n\n",
                 pTlv->type);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/* internal display strings */
static char *bwAllocSchedTypeStr[] =        /* bandwidth alloc scheduler type */
{
  "**invalid**",
  "SP",
  "WRR/WFQ",
  "BE-high",
  "BE-low"
};

static char *policeStyleStr[] =             /* policing style */
{
  "**invalid**",
  "simple",
  "single rate",
  "two rate"
};

static char *policeColorModeStr[] =         /* policing color mode */
{
  "**invalid**",
  "color blind",
  "color aware"
};

static char *policeColorAwareStr[] =        /* policing color aware type */
{
  "unused",
  "cos",
  "cos2",
  "dscp",
  "prec",
  "noexceed"
};

static char *policeActionStr[] =            /* policing action */
{
  "**invalid**",
  "drop",
  "mark cos",
  "mark cos2",
  "mark dscp",
  "mark prec",
  "send"
};

/*********************************************************************
* @purpose  Display formatted content of a single TLV entry native info
*
* @param    pTlv        @{(input)}  TLV start location
* @param    nestLvl     @{(input)}  Nesting level of this TLV (0=top level)
* @param    pSize       @{(output)} Pointer to TLV entry size output location
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    The nestLvl parameter indicates the nesting depth of this TLV
*           relative to the top-level (i.e., outermost) TLV.  A top-level
*           TLV is designated by a nestLvl of 0.
*
* @notes    The user function must provide the TOTAL size of the TLV, which
*           includes the size of the type and length fields, based on its
*           internal knowledge of the TLV type-based definition.
*
* @notes    Distiller internal use only.
*
* @notes    This parser is specific to the DiffServ Distiller TLV definitions.
*           This function name is passed to the TLV utility when parsing the
*           TLV.
*
* @notes    Only displays the TLV type, length and native data.
*
* @notes    Output regulated by DiffServ message level control.
*
* @end
*********************************************************************/
L7_RC_t dsDstlTlvEntryDisplay(L7_tlv_t *pTlv, L7_uint32 nestLvl, 
                              L7_uint32 *pEntrySize)
{
  dsDstlTlvNative_t *pVal = (dsDstlTlvNative_t *)pTlv->valueStart;
  L7_uint32         step = L7_QOS_DIFFSERV_PARSE_MARGIN_STEP;
  L7_uchar8         spaceChar = ' ';
  L7_uint32         nativeLen;
  L7_uint32         msgLvlReqdHdr, msgLvlReqd;
  L7_uint32         indent;
  L7_ushort16       tlvType, tlvLength;
  L7_uint32         val;
  L7_ushort16       val16;
  L7_uchar8         val8;
  L7_uint32         i;

  char              *fmtStrHdr, *fmtStrVal;
  char              *pBanner;
  char              spaceHdr[L7_QOS_DIFFSERV_PARSE_MARGIN_MAX+1];
  char              spaceVal[L7_QOS_DIFFSERV_PARSE_MARGIN_MAX+
                             L7_QOS_DIFFSERV_PARSE_MARGIN_STEP+1];

  /* initialize output value */
  *pEntrySize = 0;

  /* set minimum required message level for showing any output */
  msgLvlReqdHdr = DIFFSERV_MSGLVL_MED;
  msgLvlReqd = DIFFSERV_MSGLVL_LO;

  /* format string for common part of all value displays used here */
  fmtStrHdr = "%s0x%4.4x (L=%u) \'%s\'\n"; 
  fmtStrVal = "%s%-.30s: ";                     /* do not use '\n' here */

  /* set up the header line and value line spacer strings per current nest level
   *
   * NOTE: Filling each array with 'space' chars, then writing end-of-string
   *       char at appropriate termination position.
   */
  indent = (nestLvl+1) * step;          /* initial indent for this nest level */
  if (indent > L7_QOS_DIFFSERV_PARSE_MARGIN_MAX)
    indent = L7_QOS_DIFFSERV_PARSE_MARGIN_MAX;
  memset(spaceHdr, spaceChar, sizeof(spaceHdr));
  spaceHdr[indent] = '\0';
  memset(spaceVal, spaceChar, sizeof(spaceVal));
  spaceVal[indent+step] = '\0';

  tlvType = osapiNtohs(pTlv->type);
  tlvLength = osapiNtohs(pTlv->length);

  switch (tlvType)
  {

  /*---------------------------------------*/
  /* QoS DiffServ Functional Category TLVs */
  /*---------------------------------------*/

  case L7_QOS_DIFFSERV_TLV_CLSF_INST_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLSF_INST_LEN;
      pBanner = "QoS DiffServ Classifier Instance";
      DIFFSERV_PRT(msgLvlReqdHdr, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "instance key";
      val = (L7_uint32)osapiNtohl(pVal->clsfInst.instanceKey);
      DIFFSERV_PRT(msgLvlReqdHdr, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqdHdr, "%u\n", val);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_INST_DEL_LIST_TYPE:
    {
      L7_uint32   keyCount;

      nativeLen = L7_QOS_DIFFSERV_TLV_INST_DEL_LIST_MIN_LEN;  /* minimal len */
      pBanner = "QoS DiffServ Instance Deletion List";
      DIFFSERV_PRT(msgLvlReqdHdr, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "key count";
      keyCount = (L7_uint32)osapiNtohl(pVal->instDelList.keyCount);
      DIFFSERV_PRT(msgLvlReqdHdr, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqdHdr, "%u\n", keyCount);

      if (keyCount > 0)
      {
        pBanner = "instance key(s)";
        DIFFSERV_PRT(msgLvlReqdHdr, fmtStrVal, spaceVal, pBanner);
        for (i = 0; i < keyCount; i++)
        {
          val = (L7_uint32)osapiNtohl(pVal->instDelList.instanceKey[i]);
          DIFFSERV_PRT(msgLvlReqdHdr, "%u ", val);
          nativeLen += (L7_uint32)sizeof(pVal->instDelList.instanceKey[i]);
        }
        DIFFSERV_PRT(msgLvlReqdHdr, "\n");
      }
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ADD_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ADD_LEN;
      pBanner = "QoS DiffServ Instance Addition List";
      DIFFSERV_PRT(msgLvlReqdHdr, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "instance count";
      val = (L7_uint32)osapiNtohl(pVal->policyAdd.instanceCount);
      DIFFSERV_PRT(msgLvlReqdHdr, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqdHdr, "%u\n", val);
    }
    break;


  /*-------------------------------*/
  /* Class Rule Specification TLVs */
  /*-------------------------------*/

  case L7_QOS_DIFFSERV_TLV_CLASS_DEF_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_DEF_LEN;
      pBanner = "Class Definition";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_RULE_DEF_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_RULE_DEF_LEN;
      pBanner = "Class Rule Definition";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "deny flag";
      val8 = pVal->classRuleDef.denyFlag;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS_LEN;
      pBanner = "Class of Service (CoS) match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "cos value";
      val8 = pVal->cos.cosValue;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS2_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_COS2_LEN;
      pBanner = "Secondary Class of Service (CoS2) match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "cos2 value";
      val8 = pVal->cos2.cosValue;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIP_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIP_LEN;
      pBanner = "Dst IP Address match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "ip addr";
      val = (L7_uint32)osapiNtohl(pVal->dstIp.ipAddr);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%8.8x\n", val);

      pBanner = "ip mask";
      val = (L7_uint32)osapiNtohl(pVal->dstIp.ipMask);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%8.8x\n", val);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIPV6_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTIPV6_LEN;
      pBanner = "Dst IPv6 Address match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "ipv6 addr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      dsDstlIpv6AddrPrint(msgLvlReqd, pVal->dstIpv6.ipv6Addr);
      DIFFSERV_PRT(msgLvlReqd, "\n"); 

      pBanner = "ipv6 mask";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      dsDstlIpv6AddrPrint(msgLvlReqd, pVal->dstIpv6.ipv6Mask);
      DIFFSERV_PRT(msgLvlReqd, "\n"); 
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTL4PORT_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTL4PORT_LEN;
      pBanner = "Dst L4 Port Range match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "port start";
      val16 = osapiNtohs(pVal->dstL4Port.portStart);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val16);

      pBanner = "port end  ";
      val16 = osapiNtohs(pVal->dstL4Port.portEnd);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val16);

      pBanner = "port mask ";
      val16 = osapiNtohs(pVal->dstL4Port.portMask);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%4.4x\n", val16);

      pBanner = "(rsvd)    ";
      val16 = osapiNtohs(pVal->dstL4Port.rsvd1);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%4.4x\n", val16);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTMAC_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_DSTMAC_LEN;
      pBanner = "Dst MAC Address match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "mac addr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      for (i = 0; i < L7_MAC_ADDR_LEN-1; i++)
      {
        val8 = pVal->dstMac.macAddr[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x:", val8);
      }
      val8 = pVal->dstMac.macAddr[i];
      DIFFSERV_PRT(msgLvlReqd, "%2.2x\n", val8);

      pBanner = "mac mask";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      for (i = 0; i < L7_MAC_ADDR_LEN-1; i++)
      {
        val8 = pVal->dstMac.macMask[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x:", val8);
      }
      val8 = pVal->dstMac.macMask[i];
      DIFFSERV_PRT(msgLvlReqd, "%2.2x\n", val8);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_ETYPE_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_ETYPE_LEN;
      pBanner = "Ethertype match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "etype value1   ";
      val16 = osapiNtohs(pVal->etype.etypeValue1);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%4.4x\n", val16);

      pBanner = "etype value2   ";
      val16 = osapiNtohs(pVal->etype.etypeValue2);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%4.4x\n", val16);

      pBanner = "checkStdHdrFlag";
      val8 = pVal->etype.checkStdHdrFlag;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%2.2x\n", val8);

      pBanner = "(rsvd)         ";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(pVal->etype.rsvd1); i++)
      {
        val8 = pVal->etype.rsvd1[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", val8);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_EVERY_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_EVERY_LEN;
      pBanner = "\"Match Every\" match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPV6_FLOWLBL_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPV6_FLOWLBL_LEN;
      pBanner = "IPv6 Flow Label match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "flow label value";
      val = pVal->ipv6FlowLabel.ipv6FlowLabel;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%8.8x\n", val);
      pBanner = "flow label mask";
      val = pVal->ipv6FlowLabel.ipv6FlowLabelMask;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%8.8x\n", val);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPTOS_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_IPTOS_LEN;
      pBanner = "IP Type of Service (ToS) match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "tos value";
      val8 = pVal->ipTos.tosValue;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%2.2x", val8);
      /* check if the TOS mask is one of the recognized values and display a
       * decimal equivalent for the DSCP or Precedence value
       */
      if (pVal->ipTos.tosMask == DSMIB_CLASS_RULE_IP_DSCP_MASK)
      {
        val = (L7_uint32)val8 >> DSMIB_CLASS_RULE_IP_DSCP_SHIFT;
        DIFFSERV_PRT(msgLvlReqd, " (dscp %u)", val);
      }
      else if (pVal->ipTos.tosMask == DSMIB_CLASS_RULE_IP_PRECEDENCE_MASK)
      {
        val = (L7_uint32)val8 >> DSMIB_CLASS_RULE_IP_PRECEDENCE_SHIFT;
        DIFFSERV_PRT(msgLvlReqd, " (prec %u)", val);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");

      pBanner = "tos mask ";
      val8 = pVal->ipTos.tosMask;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%2.2x\n", val8);

      pBanner = "(rsvd)    ";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(pVal->ipTos.rsvd1); i++)
      {
        val8 = pVal->ipTos.rsvd1[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", val8);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_PROTOCOL_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_PROTOCOL_LEN;
      pBanner = "Protocol Number match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "proto num value";
      val8 = pVal->protocol.protoNumValue;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "proto num mask ";
      val8 = pVal->protocol.protoNumMask;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%2.2x\n", val8);

      pBanner = "(rsvd)    ";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(pVal->protocol.rsvd1); i++)
      {
        val8 = pVal->protocol.rsvd1[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", val8);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIP_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIP_LEN;
      pBanner = "Src IP Address match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "ip addr";
      val = (L7_uint32)osapiNtohl(pVal->srcIp.ipAddr);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%8.8x\n", val);

      pBanner = "ip mask";
      val = (L7_uint32)osapiNtohl(pVal->srcIp.ipMask);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%8.8x\n", val);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIPV6_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCIPV6_LEN;
      pBanner = "Src IPv6 Address match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "ipv6 addr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      dsDstlIpv6AddrPrint(msgLvlReqd, pVal->srcIpv6.ipv6Addr);
      DIFFSERV_PRT(msgLvlReqd, "\n"); 

      pBanner = "ipv6 mask";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      dsDstlIpv6AddrPrint(msgLvlReqd, pVal->srcIpv6.ipv6Mask);
      DIFFSERV_PRT(msgLvlReqd, "\n"); 
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCL4PORT_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCL4PORT_LEN;
      pBanner = "Src L4 Port Range match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "port start";
      val16 = osapiNtohs(pVal->srcL4Port.portStart);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val16);

      pBanner = "port end  ";
      val16 = osapiNtohs(pVal->srcL4Port.portEnd);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val16);

      pBanner = "port mask ";
      val16 = osapiNtohs(pVal->srcL4Port.portMask);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%4.4x\n", val16);

      pBanner = "(rsvd)    ";
      val16 = osapiNtohs(pVal->srcL4Port.rsvd1);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x%4.4x\n", val16);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCMAC_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_SRCMAC_LEN;
      pBanner = "Src MAC Address match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "mac addr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      for (i = 0; i < L7_MAC_ADDR_LEN-1; i++)
      {
        val8 = pVal->srcMac.macAddr[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x:", val8);
      }
      val8 = pVal->srcMac.macAddr[i];
      DIFFSERV_PRT(msgLvlReqd, "%2.2x\n", val8);

      pBanner = "mac mask";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      for (i = 0; i < L7_MAC_ADDR_LEN-1; i++)
      {
        val8 = pVal->srcMac.macMask[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x:", val8);
      }
      val8 = pVal->srcMac.macMask[i];
      DIFFSERV_PRT(msgLvlReqd, "%2.2x\n", val8);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID_LEN;
      pBanner = "VLAN ID Range match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "vid start";
      val16 = osapiNtohs(pVal->vlanId.vidStart);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val16);

      pBanner = "vid end  ";
      val16 = osapiNtohs(pVal->vlanId.vidEnd);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val16);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID2_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_CLASS_MATCH_VLANID2_LEN;
      pBanner = "VLAN ID2 Range match";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "vid start";
      val16 = osapiNtohs(pVal->vlanId2.vidStart);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val16);

      pBanner = "vid end  ";
      val16 = osapiNtohs(pVal->vlanId2.vidEnd);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val16);
    }
    break;


  /*-------------------------------------*/
  /* Policy Attribute Specification TLVs */
  /*-------------------------------------*/

  case L7_QOS_DIFFSERV_TLV_POLICY_DEF_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_DEF_LEN;
      pBanner = "Policy Definition";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_ASSIGN_QUEUE_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_ASSIGN_QUEUE_LEN;
      pBanner = "Assign Queue attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "queue id";
      val8 = pVal->assignQueue.qid;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)  ";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(pVal->assignQueue.rsvd1); i++)
      {
        val8 = pVal->assignQueue.rsvd1[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", val8);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_BW_ALLOC_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_BW_ALLOC_LEN;
      pBanner = "Bandwidth Allocation attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "min bw    ";
      val = (L7_uint32)osapiNtohl(pVal->bwAlloc.minBw);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u Kbps\n", val);

      pBanner = "max bw    ";
      val = (L7_uint32)osapiNtohl(pVal->bwAlloc.maxBw);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u Kbps\n", val);

      pBanner = "max burst ";
      val = (L7_uint32)osapiNtohl(pVal->bwAlloc.maxBurst);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u KB\n", val);

      pBanner = "sched type";
      val8 = pVal->bwAlloc.schedType;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u (%s)\n",
                   val8, bwAllocSchedTypeStr[val8]);

      pBanner = "(rsvd)    ";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(pVal->bwAlloc.rsvd1); i++)
      {
        val8 = pVal->bwAlloc.rsvd1[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", val8);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_DROP_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_DROP_LEN;
      pBanner = "Drop attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_LEN;
      pBanner = "CoS Mark attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "cos mark value";
      val8 = pVal->markCos.cosMark;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)        ";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(pVal->markCos.rsvd1); i++)
      {
        val8 = pVal->markCos.rsvd1[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", val8);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_AS_COS2_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COSVAL_AS_COS2_LEN;
      pBanner = "CoS as Secondary CoS Mark attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "cos as secondary cos mark value";
      val = pVal->markCosAsCos2.cosMarkAsCos2;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val);

      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COS2VAL_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_COS2VAL_LEN;
      pBanner = "Secondary CoS Mark attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "cos2 mark value";
      val8 = pVal->markCos2.cosMark;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)         ";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(pVal->markCos2.rsvd1); i++)
      {
        val8 = pVal->markCos2.rsvd1[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", val8);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPDSCPVAL_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPDSCPVAL_LEN;
      pBanner = "IP DSCP Mark attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "dscp mark value";
      val8 = pVal->markIpDscp.dscpMark;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)         ";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(pVal->markIpDscp.rsvd1); i++)
      {
        val8 = pVal->markIpDscp.rsvd1[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", val8);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_LEN;
      pBanner = "IP Precedence Mark attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "precedence mark value";
      val8 = pVal->markIpPrecedence.ipPrecMark;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)               ";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(pVal->markIpPrecedence.rsvd1); i++)
      {
        val8 = pVal->markIpPrecedence.rsvd1[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", val8);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MIRROR_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_MIRROR_LEN;
      pBanner = "Mirror attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "intf unit";
      val = (L7_uint32)osapiHtonl(pVal->mirror.intfUnit);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val);

      pBanner = "intf slot";
      val = (L7_uint32)osapiHtonl(pVal->mirror.intfSlot);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val);

      pBanner = "intf port";
      val = (L7_uint32)osapiHtonl(pVal->mirror.intfPort);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_DEF_LEN;
      pBanner = "Policing Policy Definition";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "style              ";
      val8 = pVal->policeDef.style;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u (%s)\n",
                   val8, policeStyleStr[val8]);

      pBanner = "color mode         ";
      val8 = pVal->policeDef.colorMode;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u (%s)\n",
                   val8, policeColorModeStr[val8]);

      pBanner = "color conform type ";
      val8 = pVal->policeDef.colorConformType;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u (%s)\n",
                   val8, policeColorAwareStr[val8]);

      pBanner = "color conform value";
      val8 = pVal->policeDef.colorConformValue;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "color exceed type  ";
      val8 = pVal->policeDef.colorExceedType;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u (%s)\n",
                   val8, policeColorAwareStr[val8]);

      pBanner = "color exceed value ";
      val8 = pVal->policeDef.colorExceedValue;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)             ";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(pVal->policeDef.rsvd1); i++)
      {
        val8 = pVal->policeDef.rsvd1[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", val8);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_CONFORM_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_CONFORM_LEN;
      pBanner = "Police Conform attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "data rate  ";
      val = (L7_uint32)osapiNtohl(pVal->policeConform.dataRate);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u Kbps\n", val);

      pBanner = "burst size ";
      val = (L7_uint32)osapiNtohl(pVal->policeConform.burstSize);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u KB\n", val);

      pBanner = "action flag";
      val8 = pVal->policeConform.actionFlag;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u (%s)\n",
                   val8, policeActionStr[val8]);

      pBanner = "mark value ";
      val8 = pVal->policeConform.markValue;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)     ";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(pVal->policeConform.rsvd1); i++)
      {
        val8 = pVal->policeConform.rsvd1[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", val8);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_EXCEED_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_EXCEED_LEN;
      pBanner = "Police Exceed attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "data rate  ";
      val = (L7_uint32)osapiNtohl(pVal->policeExceed.dataRate);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u Kbps\n", val);

      pBanner = "burst size ";
      val = (L7_uint32)osapiNtohl(pVal->policeExceed.burstSize);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u KB\n", val);

      pBanner = "action flag";
      val8 = pVal->policeExceed.actionFlag;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u (%s)\n",
                   val8, policeActionStr[val8]);

      pBanner = "mark value ";
      val8 = pVal->policeExceed.markValue;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)     ";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(pVal->policeExceed.rsvd1); i++)
      {
        val8 = pVal->policeExceed.rsvd1[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", val8);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_NONCONFORM_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_POLICE_NONCONFORM_LEN;
      pBanner = "Police Nonconform attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "action flag";
      val8 = pVal->policeNonconform.actionFlag;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u (%s)\n",
                   val8, policeActionStr[val8]);

      pBanner = "mark value ";
      val8 = pVal->policeNonconform.markValue;
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val8);

      pBanner = "(rsvd)     ";
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "0x");
      for (i = 0; i < (L7_uint32)sizeof(pVal->policeNonconform.rsvd1); i++)
      {
        val8 = pVal->policeNonconform.rsvd1[i];
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", val8);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_RANDOMDROP_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_RANDOMDROP_LEN;
      pBanner = "Random Drop attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "min thresh    ";
      val = (L7_uint32)osapiNtohl(pVal->randomDrop.minThresh);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u bytes\n", val);

      pBanner = "max thresh    ";
      val = (L7_uint32)osapiNtohl(pVal->randomDrop.maxThresh);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u bytes\n", val);

      pBanner = "max drop prob ";
      val = (L7_uint32)osapiNtohl(pVal->randomDrop.maxDropProb);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u percent\n", val);

      pBanner = "sampling rate ";
      val = (L7_uint32)osapiNtohl(pVal->randomDrop.samplingRate);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u usec\n", val);

      pBanner = "decay exponent";
      val = (L7_uint32)osapiNtohl(pVal->randomDrop.decayExponent);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_REDIRECT_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_REDIRECT_LEN;
      pBanner = "Redirect attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "intf unit";
      val = (L7_uint32)osapiHtonl(pVal->redirect.intfUnit);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val);

      pBanner = "intf slot";
      val = (L7_uint32)osapiHtonl(pVal->redirect.intfSlot);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val);

      pBanner = "intf port";
      val = (L7_uint32)osapiHtonl(pVal->redirect.intfPort);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u\n", val);
    }
    break;

  case L7_QOS_DIFFSERV_TLV_POLICY_ATTR_SHAPING_TYPE:
    {
      nativeLen = L7_QOS_DIFFSERV_TLV_POLICY_ATTR_SHAPING_LEN;
      pBanner = "Shaping attr";
      DIFFSERV_PRT(msgLvlReqd, fmtStrHdr, spaceHdr, tlvType, tlvLength, 
                   pBanner);

      pBanner = "committed rate";
      val = (L7_uint32)osapiNtohl(pVal->shaping.commitRate);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u Kbps\n", val);

      pBanner = "max rate      ";
      val = (L7_uint32)osapiNtohl(pVal->shaping.maxRate);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u Kbps\n", val);

      pBanner = "max burst size";
      val = (L7_uint32)osapiNtohl(pVal->shaping.maxBurst);
      DIFFSERV_PRT(msgLvlReqd, fmtStrVal, spaceVal, pBanner);
      DIFFSERV_PRT(msgLvlReqd, "%u KB\n", val);
    }
    break;


  default:
    DIFFSERV_PRT(msgLvlReqd, "\n%s>>> Invalid TLV type: 0x%4.4x <<<\n\n", 
                 spaceHdr, tlvType);
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

  *pEntrySize = (L7_uint32)L7_TLV_HEADER_SIZE + nativeLen;
  return L7_SUCCESS;
}

static void dsDstlIpv6AddrPrint(L7_uint32 msgLvlReqd, L7_uchar8 *pAddr)
{
  L7_uint32     i;

  for (i = 0; i < (L7_IP6_ADDR_LEN-2); i += 2)
  {
    DIFFSERV_PRT(msgLvlReqd, "%2.2x%2.2x:", pAddr[i], pAddr[i+1]);
  }
  DIFFSERV_PRT(msgLvlReqd, "%2.2x%2.2x", pAddr[i], pAddr[i+1]);
}


L7_tlvHandle_t      dsTlvDebugTlvHandle = 0;

void dsTlvDebugGetTlv(L7_uint32 policyIndex, L7_uint32 intIfNum, dsmibServiceIfDir_t ifDirection)
{
  if (dsTlvDebugTlvHandle != 0)
  {
    (void)dsDstlTlvPolicyCombinedTlvGet(policyIndex, intIfNum, ifDirection, dsTlvDebugTlvHandle);
    /* free the TLV resources (must occur after the DTL command is issued)
     *
     * NOTE: This requires the low-level code to copy whatever it needs 
     *       from the TLV content prior to the synchronous return from
     *       the command invocation (even if command completes asynchronously).
     *
     * NOTE: Any error that occurs after a successful TLV creation must
     *       flow through here to ensure the TLV is deleted.
     */

    if (tlvDelete(dsTlvDebugTlvHandle) != L7_SUCCESS)
    {
      LOG_MSG("dsTlvDebugGetTlv: Could not delete TLV for handle %u\n",
              (L7_uint32)dsTlvDebugTlvHandle);
    }
  }
}

L7_RC_t dsTlvDebugGetTlvInit(void)
{
  L7_tlvHandle_t  tlvHandle = 0;
  L7_RC_t         rc;

  /* register with the TLV utility to obtain a TLV block
   * NOTE: the Distiller only works on one TLV block at a time
   */
  rc = tlvRegister(L7_QOS_DIFFSERV_TLV_BLOCK_SIZE_MAX,
                   L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                   (L7_uchar8 *)"DsDebug", &tlvHandle);
  if (rc != L7_SUCCESS)
  {
    LOG_MSG("dsTlvDebugInit:  Unable to register with the TLV utility, rc=%d\n",
            rc);
    tlvHandle = 0;
  }

  dsTlvDebugTlvHandle = tlvHandle;
  return rc;
}
