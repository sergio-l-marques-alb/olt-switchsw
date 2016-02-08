/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_policyperf_api.c
*
* @purpose    DiffServ component Private MIB Policy Performance Table
*             API implementation, helper, and debug functions.
*
* @component  DiffServ
*
* @comments   none
*
* @create     04/22/2002
*
* @author     rjindal
* @author     gpaussa
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_diffserv_include.h"
#include "dtl_qos_diffserv.h"
#include "usmdb_mib_diffserv_common.h"


extern avlTree_t  dsmibAvlTree[];
extern void *     dsmibIndexSemId;              /* MIB tables index semaphore */

/*
=================================================
=================================================
=================================================

   POLICY-CLASS INBOUND PERFORMANCE TABLE APIs

=================================================
=================================================
=================================================
*/

/*********************************************************************
* @purpose  Verify that a Policy-Class In Perf Table row exists for the
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    intIfNum        @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyPerfInGet(L7_uint32 policyIndex,
                                L7_uint32 policyInstIndex,
                                L7_uint32 intIfNum)
{
  dsmibPolicyPerfInCtrl_t *pRow;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
    return L7_FAILURE;

  /* find node in AVL tree */
  pRow = dsmibPolicyPerfInRowFind(policyIndex, policyInstIndex, intIfNum);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_ERROR;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Policy-Class In Perf Table
*
* @param    prevPolicyIndex     @b{(input)}  Policy index to begin search
* @param    prevPolicyInstIndex @b{(input)}  Policy instance index to begin 
*                                               search
* @param    prevIntIfNum        @b{(input)}  Internal interface index to begin
*                                              search
* @param    pPolicyIndex        @b{(output)} Pointer to next sequential policy
*                                              index value
* @param    pPolicyInstIndex    @b{(output)} Pointer to next sequential policy
*                                              instance index value
* @param    pIntIfNum           @b{(output)} Pointer to next sequential internal     
*                                              interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevPolicyIndex value of 0 is used to find the first policy
*           entry in the table.
*
* @notes    A prevPolicyInstIndex value of 0 is used to find the first class
*           instance relative to the specified prevPolicyIndex.
*
* @notes    A prevIntIfNum value of 0 is used to find the first interface
*           relative to the specified prevPolicyIndex and prevPolicyInstIndex.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyPerfInGetNext(L7_uint32 prevPolicyIndex, 
                                    L7_uint32 prevPolicyInstIndex, 
                                    L7_uint32 prevIntIfNum,
                                    L7_uint32 *pPolicyIndex,
                                    L7_uint32 *pPolicyInstIndex,
                                    L7_uint32 *pIntIfNum)
{
  L7_uint32               prevIfIndex = 0;
  L7_uint32               intIfNum;
  dsmibPolicyPerfInCtrl_t *pRow;
  dsmibPolicyPerfInKey_t  keys;

  /* check inputs */
  /* NOTE: Allow any starting index value to be used. */
  if (pPolicyIndex == L7_NULLPTR)
    return L7_FAILURE;
  if (pPolicyInstIndex == L7_NULLPTR)
    return L7_FAILURE;
  if (pIntIfNum == L7_NULLPTR)
    return L7_FAILURE;

  if (prevIntIfNum > 0)
  {
    /* convert prevIntIfNum to prevIfIndex */
    if (nimGetIntfIfIndex(prevIntIfNum, &prevIfIndex) != L7_SUCCESS)
    {
      *pPolicyIndex = 0;
      *pPolicyInstIndex = 0;
      *pIntIfNum = 0;
      return L7_FAILURE;
    }
  }
  
  /* find node in AVL tree
   * NOTE: Need to use direct call here due to AVL_NEXT flag.
   */
  keys.policyIndex = prevPolicyIndex;
  keys.policyInstIndex = prevPolicyInstIndex;
  keys.ifIndex = prevIfIndex;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_POLICY_PERF_IN], 
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
    /* next row not found -- must have reached end of table */
    return L7_ERROR;

  if (nimGetIntfNumber(pRow->key.ifIndex, &intIfNum) != L7_SUCCESS)
  {
    *pPolicyIndex = 0;
    *pPolicyInstIndex = 0;
    *pIntIfNum = 0;
    return L7_FAILURE;
  }

  /* found next sequential table row -- set caller's output variables */
  *pPolicyIndex = pRow->key.policyIndex;
  *pPolicyInstIndex = pRow->key.policyInstIndex;
  *pIntIfNum = intIfNum;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the specified object from the Policy-Class In Perf Table
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy instance index
* @param    intIfNum        @b{(input)}  Internal interface number
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(output)} Pointer to object output value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    When accessing any counter object, the caller is required to set 
*           the pValue parameter to point to an output variable of type
*           L7_ulong64.  For a 32-bit counter, the pValue->high portion
*           always output as 0.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyPerfInObjectGet(L7_uint32 policyIndex, 
                                      L7_uint32 policyInstIndex, 
                                      L7_uint32 intIfNum, 
                                      L7_DIFFSERV_POLICY_PERF_IN_TABLE_OBJECT_t oid,
                                      void *pValue)
{
  dsmibPolicyPerfInCtrl_t   *pRow;
  L7_RC_t                   rc;
  L7_uint32                 ctrId;
  L7_ulong64                *pMibCtr, *pVal;
  L7_ulong64                ddCtrValue;
  L7_uint32                 instanceKey;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibPolicyPerfInRowFind(policyIndex, policyInstIndex, intIfNum);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  
  /* check for proper object type and row invalid flag state before proceeding*/
  if ((rc = dsmibPolicyPerfInObjectGetCheck(pRow, oid)) != L7_SUCCESS)
    return rc;

  /* translate MIB counter name to system counter ID and establish
   * a pointer to the MIB object
   */
  switch (oid)
  {
  case L7_DIFFSERV_POLICY_PERF_IN_HC_OFFERED_OCTETS:
    {
      ctrId = L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_IN_OFFERED_OCTETS;
      pMibCtr = &pRow->mib.HCOfferedOctets;
    }
    break;

  case L7_DIFFSERV_POLICY_PERF_IN_HC_OFFERED_PACKETS:
    {
      ctrId = L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_IN_OFFERED_PACKETS;
      pMibCtr = &pRow->mib.HCOfferedPackets;
    }
    break;

  case L7_DIFFSERV_POLICY_PERF_IN_HC_DISCARDED_OCTETS:
    {
      ctrId = L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_IN_DISCARDED_OCTETS;
      pMibCtr = &pRow->mib.HCDiscardedOctets;
    }
    break;

  case L7_DIFFSERV_POLICY_PERF_IN_HC_DISCARDED_PACKETS:
    {
      ctrId = L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_IN_DISCARDED_PACKETS;
      pMibCtr = &pRow->mib.HCDiscardedPackets;
    }
    break;

  case L7_DIFFSERV_POLICY_PERF_IN_STORAGE_TYPE:
    {
      L7_uint32      *pStorageType = pValue;
      *pStorageType = pRow->mib.storageType;
      return L7_SUCCESS;
    }
    /*PASSTHRU*/

  case L7_DIFFSERV_POLICY_PERF_IN_ROW_STATUS:
    {
      L7_uint32      *pRowStatus = pValue;
      if (dsmibRowStatusObservable(pRow->mib.rowStatus, 
                                   (L7_uint32)pRow->rowInvalidFlags, 
                                   pRowStatus) != L7_SUCCESS)
        return L7_FAILURE;
      return L7_SUCCESS;
    }
    /*PASSTHRU*/

  default:
    /* invalid oid */
    return L7_FAILURE;
    /*PASSTHRU*/
  } /* endswitch */

  /* for a supported counter, get an updated value from the device driver */
  if (ctrId != L7_PLATFORM_CTR_NOT_SUPPORTED)
  {
    /* skip any instance for which a TLV is not currently issued (since the
     * driver will not recognize the instance key)
     */
    if (dsDistillerPolicyInstIsIssued(policyIndex, policyInstIndex, intIfNum)
          == L7_TRUE)
    {
      if (dsDistillerInstanceKeyGet(policyIndex, policyInstIndex, &instanceKey)
          != L7_SUCCESS)
        return L7_FAILURE;

      /* get an updated counter value from the device driver
       * NOTE: if DTL call fails, just use current MIB value
       */
      if (dtlQosDiffServDrvrStatsInGet(intIfNum, instanceKey, ctrId, 
                                       &ddCtrValue) == L7_SUCCESS)
        *pMibCtr = ddCtrValue;
    }
  }

  /* output the latest MIB value of the counter object
   * 
   * NOTE:  The MIB supports both 32-bit and 64-bit versions, but we maintain
   *        only 64-bit counter objects (let the caller sort out what is 
   *        needed).
   */
  pVal = (L7_ulong64 *)pValue;
  *pVal = *pMibCtr;

  return L7_SUCCESS;
}


/*
==================================================
==================================================
==================================================
 
   POLICY-CLASS OUTBOUND PERFORMANCE TABLE APIs

==================================================
==================================================
==================================================
*/

/*********************************************************************
* @purpose  Verify that a Policy-Class Out Perf Table row exists for the
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    intIfNum        @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyPerfOutGet(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 intIfNum)
{
  dsmibPolicyPerfOutCtrl_t *pRow;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
    return L7_FAILURE;

  /* find node in AVL tree */
  pRow = dsmibPolicyPerfOutRowFind(policyIndex, policyInstIndex, intIfNum);
  
  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_ERROR;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Policy-Class Out Perf Table
*
* @param    prevPolicyIndex     @b{(input)}  Policy index to begin search
* @param    prevPolicyInstIndex @b{(input)}  Policy instance index to begin 
*                                               search
* @param    prevIntIfNum        @b{(input)}  Internal interface index to begin
*                                              search
* @param    pPolicyIndex        @b{(output)} Pointer to next sequential policy
*                                              index value
* @param    pPolicyInstIndex    @b{(output)} Pointer to next sequential policy
*                                              instance index value
* @param    pIntIfNum           @b{(output)} Pointer to next sequential internal     
*                                              interface number
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevPolicyIndex value of 0 is used to find the first policy
*           entry in the table.
*
* @notes    A prevPolicyInstIndex value of 0 is used to find the first class
*           instance relative to the specified prevPolicyIndex.
*
* @notes    A prevIntIfNum value of 0 is used to find the first interface
*           relative to the specified prevPolicyIndex and prevPolicyInstIndex.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyPerfOutGetNext(L7_uint32 prevPolicyIndex, 
                                     L7_uint32 prevPolicyInstIndex, 
                                     L7_uint32 prevIntIfNum,
                                     L7_uint32 *pPolicyIndex,
                                     L7_uint32 *pPolicyInstIndex,
                                     L7_uint32 *pIntIfNum)
{
  L7_uint32                prevIfIndex = 0;
  L7_uint32                intIfNum;
  dsmibPolicyPerfOutCtrl_t *pRow;
  dsmibPolicyPerfOutKey_t  keys;

  /* check inputs */
  /* NOTE: Allow any starting index value to be used. */
  if (pPolicyIndex == L7_NULLPTR)
    return L7_FAILURE;
  if (pPolicyInstIndex == L7_NULLPTR)
    return L7_FAILURE;
  if (pIntIfNum == L7_NULLPTR)
    return L7_FAILURE;

  if (prevIntIfNum > 0)
  {
    /* convert prevIntIfNum to prevIfIndex */
    if (nimGetIntfIfIndex(prevIntIfNum, &prevIfIndex) != L7_SUCCESS)
    {
      *pPolicyIndex = 0;
      *pPolicyInstIndex = 0;
      *pIntIfNum = 0;
      return L7_FAILURE;
    }
  }

  /* find node in AVL tree
   * NOTE: Need to use direct call here due to AVL_NEXT flag.
   */
  keys.policyIndex = prevPolicyIndex;
  keys.policyInstIndex = prevPolicyInstIndex;
  keys.ifIndex = prevIfIndex;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_POLICY_PERF_OUT], 
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
    /* next row not found -- must have reached end of table */
    return L7_ERROR;

  if (nimGetIntfNumber(pRow->key.ifIndex, &intIfNum) != L7_SUCCESS)
  {
    *pPolicyIndex = 0;
    *pPolicyInstIndex = 0;
    *pIntIfNum = 0;
    return L7_FAILURE;
  }

  /* found next sequential table row -- set caller's output variables */
  *pPolicyIndex = pRow->key.policyIndex;
  *pPolicyInstIndex = pRow->key.policyInstIndex;
  *pIntIfNum = intIfNum;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the specified object from the Policy-Class Out Perf Table
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy instance index
* @param    intIfNum        @b{(input)}  Internal interface number
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(output)} Pointer to the object output value  
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    When accessing any counter object, the caller is required to set 
*           the pValue parameter to point to an output variable of type
*           L7_ulong64.  For a 32-bit counter, the pValue->high portion is 
*           always output as 0.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyPerfOutObjectGet(L7_uint32 policyIndex, 
                                       L7_uint32 policyInstIndex, 
                                       L7_uint32 intIfNum, 
                                       L7_DIFFSERV_POLICY_PERF_OUT_TABLE_OBJECT_t oid,
                                       void *pValue)
{
  dsmibPolicyPerfOutCtrl_t  *pRow;
  L7_RC_t                   rc;
  L7_uint32                 ctrId = L7_PLATFORM_CTR_NOT_SUPPORTED;
  L7_ulong64                *pMibCtr, *pVal;
  L7_ulong64                ddCtrValue;
  L7_uint32                 instanceKey;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibPolicyPerfOutRowFind(policyIndex, policyInstIndex, intIfNum);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  /* check for proper object type and row invalid flag state before proceeding*/
  if ((rc = dsmibPolicyPerfOutObjectGetCheck(pRow, oid)) != L7_SUCCESS)
    return rc;

  /* translate MIB counter name to system counter ID and establish
   * a pointer to the MIB object
   */
  switch (oid)
  {
  case L7_DIFFSERV_POLICY_PERF_OUT_HC_OFFERED_OCTETS:
    {
      ctrId = L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_OUT_OFFERED_OCTETS;
      pMibCtr = &pRow->mib.HCOfferedOctets;
    }
    break;

  case L7_DIFFSERV_POLICY_PERF_OUT_HC_OFFERED_PACKETS:
    {
      ctrId = L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_OUT_OFFERED_PACKETS;
      pMibCtr = &pRow->mib.HCOfferedPackets;
    }
    break;

  case L7_DIFFSERV_POLICY_PERF_OUT_HC_DISCARDED_OCTETS:
    {
      ctrId = L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_OUT_DISCARDED_OCTETS;
      pMibCtr = &pRow->mib.HCDiscardedOctets;
    }
    break;

  case L7_DIFFSERV_POLICY_PERF_OUT_HC_DISCARDED_PACKETS:
    {
      ctrId = L7_PLATFORM_CTR_DIFFSERV_POLICY_PERF_OUT_DISCARDED_PACKETS;
      pMibCtr = &pRow->mib.HCDiscardedPackets;
    }
    break;

  case L7_DIFFSERV_POLICY_PERF_OUT_STORAGE_TYPE:
    {
      L7_uint32      *pStorageType = pValue;
      *pStorageType = pRow->mib.storageType;
      return L7_SUCCESS;
    }
    /*PASSTHRU*/

  case L7_DIFFSERV_POLICY_PERF_OUT_ROW_STATUS:
    {
      L7_uint32      *pRowStatus = pValue;
      if (dsmibRowStatusObservable(pRow->mib.rowStatus, 
                                   (L7_uint32)pRow->rowInvalidFlags, 
                                   pRowStatus) != L7_SUCCESS)
        return L7_FAILURE;
      return L7_SUCCESS;
    }
    /*PASSTHRU*/

  default:
    /* invalid oid */
    return L7_FAILURE;
    /*PASSTHRU*/
  } /* endswitch */

  /* for a supported counter, get an updated value from the device driver */
  if (ctrId != L7_PLATFORM_CTR_NOT_SUPPORTED)
  {
    /* skip any instance for which a TLV is not currently issued (since the
     * driver will not recognize the instance key)
     */
    if (dsDistillerPolicyInstIsIssued(policyIndex, policyInstIndex, intIfNum)
          == L7_TRUE)
    {
      if (dsDistillerInstanceKeyGet(policyIndex, policyInstIndex, &instanceKey)
          != L7_SUCCESS)
        return L7_FAILURE;

      /* get an updated counter value from the device driver
       * NOTE: if DTL call fails, just use current MIB value
       */
      if (dtlQosDiffServDrvrStatsOutGet(intIfNum, instanceKey, ctrId, 
                                        &ddCtrValue) == L7_SUCCESS)
        *pMibCtr = ddCtrValue;
    }
  }

  /* output the latest MIB value of the counter object
   * 
   * NOTE:  The MIB supports both 32-bit and 64-bit versions, but we maintain
   *        only 64-bit counter objects (let the caller sort out what is 
   *        needed).
   */
  pVal = (L7_ulong64 *)pValue;
  *pVal = *pMibCtr;

  return L7_SUCCESS;
}


/*
=============================================================
=============================================================
=============================================================

   POLICY-CLASS INBOUND PERFORMANCE TABLE HELPER FUNCTIONS

=============================================================
=============================================================
=============================================================
*/

/*************************************************************************
* @purpose  Create a new row in the Policy-Class In Perf Table for the 
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    intIfNum        @b{(input)} Internal interface number
* @param    activateRow     @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row already exists
* @returns  L7_FAILURE
*
* @notes    This function is only to be used internally by the component.
*
* @notes    The row status is initially set to either 'active' or 'not in   
*           service' depending on the value of the activateRow parameter.
*           Since this is a read-only table, all row objects are 
*           instantiated upon row creation.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyPerfInCreate(L7_uint32 policyIndex,
                                L7_uint32 policyInstIndex,
                                L7_uint32 intIfNum,
                                L7_BOOL activateRow)
{
  L7_RC_t                     rc = L7_FAILURE;
  dsmibTableId_t              tableId = DSMIB_TABLE_ID_POLICY_PERF_IN;
  dsmibPolicyPerfInCtrl_t     row, *pRow = L7_NULLPTR;
  dsmibPolicyInstEntryCtrl_t  *pPolicyInst;
  L7_uint32                   ifIndex;
  dsmibRowStatus_t            oldStatus, newStatus;

  DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                           policyIndex, policyInstIndex, intIfNum, 
                           (L7_uchar8)activateRow);

  do
  {
    /* check inputs */
    if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
      break;
    if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
      break;
    if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
      break;
    if ((activateRow != L7_FALSE) && (activateRow != L7_TRUE))
      break;

    /* convert intIfNum to ifIndex */
    if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
      break;

    /* make sure row does not already exist */
    if (diffServPolicyPerfInGet(policyIndex, policyInstIndex, intIfNum) 
          == L7_SUCCESS)
    {
      rc = L7_ERROR;
      break;
    }

    /* make sure there is room in the table */
    if (dsmibMibTableSizeCheck(tableId) != L7_SUCCESS)
      break;

    /* get access to the policy instance row */
    pPolicyInst = dsmibPolicyInstRowFind(policyIndex, policyInstIndex);
    if (pPolicyInst == L7_NULLPTR)
      break;

    /* setup a temporary MIB row element structure */
    pRow = &row;
    memset(pRow, 0, sizeof(*pRow));

    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* initialize row index keys */
    pRow->key.policyIndex = policyIndex;
    pRow->key.policyInstIndex = policyInstIndex;
    pRow->key.ifIndex = ifIndex;

    /* initialize control fields */
    pRow->tableId = tableId;
    pRow->rowInvalidFlags = DSMIB_POLICY_PERF_IN_FLAGS_COMMON;  /*starting value*/
    pRow->pPolicyInst = pPolicyInst;        /* back ptr to policy instance row */
    pRow->rowPending = L7_FALSE;            /* read-only row always completed */

    /* fill in initial row status and any objects with known defaults */
    /* all counters were implicitly set to zero when MIB struct was init to 0 */
    pRow->mib.storageType = DSMIB_CFG_DEFAULT_STORAGE_TYPE;

    /* since this is read-only, the row status can go directly to 'active' or
     * 'not-in-service' (the old status is used later to simulate a transition)
     */
    if (activateRow == L7_TRUE)
    {
      oldStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO;
      newStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
    }
    else
    {
      oldStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT;
      newStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;
    }
    pRow->mib.rowStatus = newStatus;

    /* add row to AVL tree in sorted order */
    rc = dsmibRowInsert(&dsmibAvlTree[tableId], pRow);

    if (rc == L7_SUCCESS)
    {
      /* update current table size */
      genStatus.tableSize[tableId]++;

      /* NOTE: No need to call dsDistillerEvaluateAll() after creation of 
       *       a policy perf table row, since it is subordinate to the 
       *       service table row management activities.
       */       
    }
    else
    {
      /* problem with AVL insertion */
      rc = L7_FAILURE;
    }

    DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

    /* make sure successful so far */
    if (rc != L7_SUCCESS)
      break;

    /* find the created row and simulate a row status transition event so the
     * rest of the system can do its thing.
     */
    pRow = dsmibPolicyPerfInRowFind(policyIndex, policyInstIndex, intIfNum);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_FAILURE;
      break;
    }

    if (dsmibRowEventCheck(tableId, pRow, oldStatus, newStatus) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

  } while (0);

  DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                          policyIndex, policyInstIndex, intIfNum, 
                          (L7_uint32)pRow, (L7_uchar8)rc,
                          (L7_uchar8)activateRow);

  return rc;
}

/*************************************************************************
* @purpose  Delete a row from the Policy-Class In Perf Table for the 
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    intIfNum        @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE
*
* @notes    This function is only to be used internally by the component.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyPerfInDelete(L7_uint32 policyIndex,
                                L7_uint32 policyInstIndex,
                                L7_uint32 intIfNum)
{
  L7_RC_t                 rc = L7_FAILURE;
  dsmibTableId_t          tableId = DSMIB_TABLE_ID_POLICY_PERF_IN;
  dsmibPolicyPerfInCtrl_t *pRow = L7_NULLPTR;
  dsmibRowStatus_t        prevRowStatus;

  DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                           policyIndex, policyInstIndex, intIfNum);

  do
  {
    /* check inputs */
    if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
      break;
    if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
      break;
    if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
      break;

    /* find node in AVL tree */
    pRow = dsmibPolicyPerfInRowFind(policyIndex, policyInstIndex, intIfNum);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_ERROR;
      break;
    }

    /* change an 'ACTIVE' row status to 'NOT_IN_SERVICE' prior to row deletion */
    prevRowStatus = pRow->mib.rowStatus;
    if (prevRowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      pRow->mib.rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;

      if (dsmibRowEventCheck(DSMIB_TABLE_ID_POLICY_PERF_IN, pRow, 
                             prevRowStatus, pRow->mib.rowStatus) != L7_SUCCESS)
        break;
    }

    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* delete the policy perf in row AVL tree node */
    rc = dsmibRowDelete(&dsmibAvlTree[tableId], pRow);

    if (rc == L7_SUCCESS)
    {
      /* successful row deletion
       * NOTE: Do not use pRow from this point on.
       */
      pRow = L7_NULLPTR;

      /* update current table size */
      genStatus.tableSize[tableId]--;

      /* NOTE: No need to call dsDistillerEvaluateAll() after deletion of 
       *       a policy perf table row, since it is subordinate to the 
       *       service table row management activities.
       */       
    }
    else
    {
      /* something's wrong */
      LOG_MSG("dsmibPolicyPerfInDelete: couldn't delete row AVL node for "
              "Policy Perf In Table indexes (%u,%u), interface %u\n", 
              policyIndex, policyInstIndex, intIfNum);

      rc = L7_FAILURE;
    }

    DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

  } while (0);

  DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                          policyIndex, policyInstIndex, intIfNum, 
                          (L7_uint32)pRow, (L7_uchar8)rc);

  return rc;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Policy-Class Inbound Performance
*           Table row element
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    intIfNum        @b{(input)} Internal interface number
*
* @returns  pRow        Pointer to policy performance inbound row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsmibPolicyPerfInCtrl_t *dsmibPolicyPerfInRowFind(L7_uint32 policyIndex,
                                                  L7_uint32 policyInstIndex,
                                                  L7_uint32 intIfNum)
{
  L7_uint32               ifIndex;
  dsmibPolicyPerfInKey_t  keys;
  dsmibPolicyPerfInCtrl_t *pRow;

  /* convert intIfNum to ifIndex */
  if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
    return L7_NULLPTR;

  /* get access to the policy row */
  keys.policyIndex = policyIndex;
  keys.policyInstIndex = policyInstIndex;
  keys.ifIndex = ifIndex;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_POLICY_PERF_IN], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Checks if a policy inbound performance object value is
*           valid to 'get'
*
* @param    pRow        @b{(input)} Policy inbound performance table
*                                   row pointer
* @param    oid         @b{(input)} Object identifier             
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Object value not yet established
* @returns  L7_FAILURE  Object type mismatch
*
* @notes    Handles those objects not subject to type and/or flag checking.
*       
* @end
*********************************************************************/
L7_RC_t dsmibPolicyPerfInObjectGetCheck(dsmibPolicyPerfInCtrl_t *pRow, 
                                        L7_DIFFSERV_POLICY_PERF_IN_TABLE_OBJECT_t oid)
{
  dsmibPolicyPerfInFlags_t objFlag;

  switch (oid)
  {
  case L7_DIFFSERV_POLICY_PERF_IN_HC_OFFERED_OCTETS:
  case L7_DIFFSERV_POLICY_PERF_IN_HC_OFFERED_PACKETS:
  case L7_DIFFSERV_POLICY_PERF_IN_HC_DISCARDED_OCTETS:
  case L7_DIFFSERV_POLICY_PERF_IN_HC_DISCARDED_PACKETS:
    objFlag = DSMIB_POLICY_PERF_IN_FLAGS_COMMON;
    break;

  case L7_DIFFSERV_POLICY_PERF_IN_STORAGE_TYPE:
  case L7_DIFFSERV_POLICY_PERF_IN_ROW_STATUS:
    objFlag = DSMIB_NO_FLAG;  /* no row invalid flag for this match object */
    break;

  default:
    /* invalid object id */
    return L7_FAILURE;
    /*PASSTHRU*/
  }

  /* using objFlag = DSMIB_NO_FLAG to bypass flag check for the object */
  if (objFlag != DSMIB_NO_FLAG)
    if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, objFlag))
      return L7_ERROR;

  return L7_SUCCESS;
}


/*
==============================================================
==============================================================
==============================================================

   POLICY-CLASS OUTBOUND PERFORMANCE TABLE HELPER FUNCTIONS

==============================================================
==============================================================
==============================================================
*/

/*************************************************************************
* @purpose  Create a new row in the Policy-Class Out Perf Table for the 
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    intIfNum        @b{(input)} Internal interface number
* @param    activateRow     @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row already exists
* @returns  L7_FAILURE
*
* @notes    This function is only to be used internally by the component.
*
* @notes    The row status is initially set to either 'active' or 'not in   
*           service' depending on the value of the activateRow parameter.
*           Since this is a read-only table, all row objects are 
*           instantiated upon row creation.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyPerfOutCreate(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 intIfNum,
                                 L7_BOOL activateRow)
{
  L7_RC_t                     rc = L7_FAILURE;
  dsmibTableId_t              tableId = DSMIB_TABLE_ID_POLICY_PERF_OUT;
  dsmibPolicyPerfOutCtrl_t    row, *pRow = L7_NULLPTR;
  dsmibPolicyInstEntryCtrl_t  *pPolicyInst;
  L7_uint32                   ifIndex;
  dsmibRowStatus_t            oldStatus, newStatus;

  DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                           policyIndex, policyInstIndex, intIfNum, 
                           (L7_uchar8)activateRow);

  do
  {
    /* check inputs */
    if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
      break;
    if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
      break;
    if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
      break;
    if ((activateRow != L7_FALSE) && (activateRow != L7_TRUE))
      break;

    /* convert intIfNum to ifIndex */
    if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
      break;

    /* make sure row does not already exist */
    if (diffServPolicyPerfOutGet(policyIndex, policyInstIndex, intIfNum) 
          == L7_SUCCESS)
    {
      rc = L7_ERROR;
      break;
    }

    /* make sure there is room in the table */
    if (dsmibMibTableSizeCheck(tableId) != L7_SUCCESS)
      break;

    /* get access to the policy instance row */
    pPolicyInst = dsmibPolicyInstRowFind(policyIndex, policyInstIndex);
    if (pPolicyInst == L7_NULLPTR)
      break;

    /* setup a temporary MIB row element structure */
    pRow = &row;
    memset(pRow, 0, sizeof(*pRow));

    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* initialize row index keys */
    pRow->key.policyIndex = policyIndex;
    pRow->key.policyInstIndex = policyInstIndex;
    pRow->key.ifIndex = ifIndex;

    /* initialize control fields */
    pRow->tableId = tableId;
    pRow->rowInvalidFlags = DSMIB_POLICY_PERF_OUT_FLAGS_COMMON;  /*starting value*/
    pRow->pPolicyInst = pPolicyInst;        /* back ptr to policy instance row */
    pRow->rowPending = L7_FALSE;            /* read-only row always completed */

    /* fill in initial row status and any objects with known defaults */
    /* all counters were implicitly set to zero when MIB struct was init to 0 */
    pRow->mib.storageType = DSMIB_CFG_DEFAULT_STORAGE_TYPE;

    /* since this is read-only, the row status can go directly to 'active' or
     * 'not-in-service' (the old status is used later to simulate a transition)
     */
    if (activateRow == L7_TRUE)
    {
      oldStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO;
      newStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
    }
    else
    {
      oldStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT;
      newStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;
    }
    pRow->mib.rowStatus = newStatus;

    /* add row to AVL tree in sorted order */
    rc = dsmibRowInsert(&dsmibAvlTree[tableId], pRow);

    if (rc == L7_SUCCESS)
    {
      /* update current table size */
      genStatus.tableSize[tableId]++;

      /* NOTE: No need to call dsDistillerEvaluateAll() after creation of 
       *       a policy perf table row, since it is subordinate to the 
       *       service table row management activities.
       */       
    }
    else
    {
      /* problem with AVL insertion */
      rc = L7_FAILURE;
    }

    DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

    /* make sure successful so far */
    if (rc != L7_SUCCESS)
      break;

    /* find the created row and simulate a row status transition event so the
     * rest of the system can do its thing.
     */
    pRow = dsmibPolicyPerfOutRowFind(policyIndex, policyInstIndex, intIfNum);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_FAILURE;
      break;
    }

    if (dsmibRowEventCheck(tableId, pRow, oldStatus, newStatus) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

  } while (0);

  DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                          policyIndex, policyInstIndex, intIfNum, 
                          (L7_uint32)pRow, (L7_uchar8)rc,
                          (L7_uchar8)activateRow);

  return rc;
}

/*************************************************************************
* @purpose  Delete a row from the Policy-Class Out Perf Table for the 
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    intIfNum        @b{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE
*
* @notes    This function is only to be used internally by the component.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyPerfOutDelete(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 intIfNum)
{
  L7_RC_t                  rc = L7_FAILURE;
  dsmibTableId_t           tableId = DSMIB_TABLE_ID_POLICY_PERF_OUT;
  dsmibPolicyPerfOutCtrl_t *pRow = L7_NULLPTR;
  dsmibRowStatus_t         prevRowStatus;

  DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                           policyIndex, policyInstIndex, intIfNum);

  do
  {
    /* check inputs */
    if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
      break;
    if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
      break;
    if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
      break;

    /* find node in AVL tree */
    pRow = dsmibPolicyPerfOutRowFind(policyIndex, policyInstIndex, intIfNum);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_ERROR;
      break;
    }

    /* change an 'ACTIVE' row status to 'NOT_IN_SERVICE' prior to row deletion */
    prevRowStatus = pRow->mib.rowStatus;
    if (prevRowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      pRow->mib.rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;

      if (dsmibRowEventCheck(DSMIB_TABLE_ID_POLICY_PERF_OUT, pRow, 
                             prevRowStatus, pRow->mib.rowStatus) != L7_SUCCESS)
        break;
    }

    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* delete the policy perf in row AVL tree node */
    rc = dsmibRowDelete(&dsmibAvlTree[tableId], pRow);

    if (rc == L7_SUCCESS)
    {
      /* successful row deletion
       * NOTE: Do not use pRow from this point on.
       */
      pRow = L7_NULLPTR;

      /* update current table size */
      genStatus.tableSize[tableId]--;

      /* NOTE: No need to call dsDistillerEvaluateAll() after deletion of 
       *       a policy perf table row, since it is subordinate to the 
       *       service table row management activities.
       */       
    }
    else
    {
      /* something's wrong */
      LOG_MSG("dsmibPolicyPerfOutDelete: couldn't delete row AVL node for "
              "Policy Perf In Table indexes (%u,%u), interface %u\n", 
              policyIndex, policyInstIndex, intIfNum);

      rc = L7_FAILURE;
    }

    DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

  } while (0);

  DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                          policyIndex, policyInstIndex, intIfNum, 
                          (L7_uint32)pRow, (L7_uchar8)rc);

  return rc;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Policy-Class Outbound Performance
*           Table row element
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    intIfNum        @b{(input)} Internal interface number
*
* @returns  pRow        Pointer to policy performance outbound row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsmibPolicyPerfOutCtrl_t *dsmibPolicyPerfOutRowFind(L7_uint32 policyIndex,
                                                    L7_uint32 policyInstIndex,
                                                    L7_uint32 intIfNum)
{
  L7_uint32                 ifIndex;
  dsmibPolicyPerfOutKey_t   keys;
  dsmibPolicyPerfOutCtrl_t  *pRow;

  /* convert intIfNum to ifIndex */
  if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
    return L7_NULLPTR;

  /* get access to the policy row */
  keys.policyIndex = policyIndex;
  keys.policyInstIndex = policyInstIndex;
  keys.ifIndex = ifIndex;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_POLICY_PERF_OUT], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Checks if a policy outbound performance object value is
*           valid to 'get'
*
* @param    pRow        @b{(input)} Policy outbound performance table
*                                   row pointer
* @param    oid         @b{(input)} Object identifier             
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Object value not yet established
* @returns  L7_FAILURE  Object type mismatch
*
* @notes    Handles those objects not subject to type and/or flag checking.
*       
* @end
*********************************************************************/
L7_RC_t dsmibPolicyPerfOutObjectGetCheck(dsmibPolicyPerfOutCtrl_t *pRow, 
                                         L7_DIFFSERV_POLICY_PERF_OUT_TABLE_OBJECT_t oid)
{
  dsmibPolicyPerfOutFlags_t objFlag;

  switch (oid)
  {
  case L7_DIFFSERV_POLICY_PERF_OUT_HC_OFFERED_OCTETS:
  case L7_DIFFSERV_POLICY_PERF_OUT_HC_OFFERED_PACKETS:
  case L7_DIFFSERV_POLICY_PERF_OUT_HC_DISCARDED_OCTETS:
  case L7_DIFFSERV_POLICY_PERF_OUT_HC_DISCARDED_PACKETS:
    objFlag = DSMIB_POLICY_PERF_OUT_FLAGS_COMMON;
    break;

  case L7_DIFFSERV_POLICY_PERF_OUT_STORAGE_TYPE:
  case L7_DIFFSERV_POLICY_PERF_OUT_ROW_STATUS:
    objFlag = DSMIB_NO_FLAG;  /* no row invalid flag for this match object */
    break;

  default:
    /* invalid object id */
    return L7_FAILURE;
    /*PASSTHRU*/
  }

  /* using objFlag = DSMIB_NO_FLAG to bypass flag check for the object */
  if (objFlag != DSMIB_NO_FLAG)
    if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, objFlag))
      return L7_ERROR;

  return L7_SUCCESS;
}


/*
========================================================================
========================================================================
========================================================================

   POLICY-CLASS (INBOUND/OUTBOUND) PERFORMANCE TABLE HELPER FUNCTIONS

========================================================================
========================================================================
========================================================================
*/

/*********************************************************************
* @purpose  Create the necessary rows in the Policy-Class (Inbound or Outbound)
*           Performance Table for the specified service interface
*
* @param    policyIndex @b{(input)} Policy index             
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    
* @returns  L7_FAILURE  
*
* @notes    Policy Performance Table rows are created automatically when a 
*           policy is attached to a service interface.  One row is created 
*           for each policy instance in said policy.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyPerfAttach(L7_uint32 policyIndex, 
                              L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
  L7_uint32     policyInstIndex, nextPolicyIndex;
  L7_RC_t       rc;
  char          *pStr;

  /* walk the Policy Instance Table for this policy and create a row 
   * in the appropriate Policy Performance Table based on ifDirection
   */
  policyInstIndex = 0;                          /* start with first instance */
  while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex, 
                          &nextPolicyIndex, &policyInstIndex) == L7_SUCCESS) &&
         (nextPolicyIndex == policyIndex))
  {
    if (policyInstIndex == 0)
      break;

    if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
    {
      pStr = "In";
      rc = dsmibPolicyPerfInCreate(policyIndex, policyInstIndex, intIfNum, 
                                   L7_TRUE);
    }

    else
    {
      pStr = "Out";
      rc = dsmibPolicyPerfOutCreate(policyIndex, policyInstIndex, intIfNum, 
                                       L7_TRUE);
    }

    if ((rc != L7_SUCCESS) && (rc != L7_ERROR))
    {
      LOG_MSG("dsmibPolicyPerfAttach: couldn't create Perf %s row for "
              "indexes %u,%u on intIfNum %u\n", pStr, policyIndex, 
              policyInstIndex, intIfNum); 
      return L7_FAILURE;
    }

  } /* endwhile */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the corresponding rows in the Policy-Class (Inbound or
*           Outbound Performance Table for the specified service interface
*
* @param    policyIndex @b{(input)} Policy index             
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    
* @returns  L7_FAILURE  
*
* @notes    Policy Performance Table rows are deleted automatically when a 
*           policy is detached from a service interface.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyPerfDetach(L7_uint32 policyIndex, 
                              L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
  L7_uint32     policyInstIndex, nextPolicyIndex;
  L7_RC_t       rc;
  char          *pStr;

  /* walk the Policy Instance Table for this policy and delete all rows 
   * in the appropriate Policy Performance Table based on ifDirection
   */
  policyInstIndex = 0;                          /* start with first instance */
  while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex, 
                          &nextPolicyIndex, &policyInstIndex) == L7_SUCCESS) &&
         (nextPolicyIndex == policyIndex))
  {
    if (policyInstIndex == 0)
      break;

    if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
    {
      pStr = "In";
      rc = dsmibPolicyPerfInDelete(policyIndex, policyInstIndex, intIfNum);
    }

    else
    {
      pStr = "Out";
      rc = dsmibPolicyPerfOutDelete(policyIndex, policyInstIndex, intIfNum);
    }

    if ((rc != L7_SUCCESS) && (rc != L7_ERROR))
    {
      LOG_MSG("dsmibPolicyPerfDetach: couldn't delete Perf %s row for "
              "indexes %u,%u on intIfNum %u\n", pStr, policyIndex, 
              policyInstIndex, intIfNum); 
      return L7_FAILURE;
    }

  } /* endwhile */

  return L7_SUCCESS;
}


/*
================================================================
================================================================
================================================================

   POLICY-CLASS INBOUND PERFORMANCE TABLE DEBUG SHOW FUNCTION

================================================================
================================================================
================================================================
*/

/*********************************************************************
* @purpose  Display the current DiffServ Policy Performance Inbound Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsmibPolicyPerfInTableShow(void)
{
  L7_uint32               msgLvlReqd;
  dsmibPolicyPerfInCtrl_t *pRow;
  dsmibPolicyPerfInKey_t  keys;
  avlTree_t               *pTree = &dsmibAvlTree[DSMIB_TABLE_ID_POLICY_PERF_IN];
  L7_uint32               count;

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);
  
  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Policy Perf Inbound Table is empty\n\n");
    return;
  }
  
  DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Policy Perf Inbound Table (contains %u entries):\n\n", count);

  DIFFSERV_PRT(msgLvlReqd, "Indexes  ST RS TID  Flags   RP ");
  DIFFSERV_PRT(msgLvlReqd, "Ctr: Offered(hi) Offered(lo) Discard(hi) Discard(lo)\n");
  DIFFSERV_PRT(msgLvlReqd, "-------- -- -- --- -------- -- ");
  DIFFSERV_PRT(msgLvlReqd, "---- ----------- ----------- ----------- -----------\n");

  keys.policyIndex = 0;                          /* start with first entry */
  keys.policyInstIndex = 0;
  keys.ifIndex = 0;
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  { 
    /* part A */
    DIFFSERV_PRT(msgLvlReqd, "%2u %2u %2u %2s %2s %2u  %8.8x %2s ", 
                 pRow->key.policyIndex, 
                 pRow->key.policyInstIndex, 
                 pRow->key.ifIndex, 
                 dsmibStgTypeStr[pRow->mib.storageType], 
                 dsmibRowStatusStr[pRow->mib.rowStatus], 
                 pRow->tableId, 
                 pRow->rowInvalidFlags,
                 dsmibRowPendingStr[pRow->rowPending]);
    
    /* part B -- octet counts */
    DIFFSERV_PRT(msgLvlReqd, "oct: %11u %11u %11u %11u\n", 
                 pRow->mib.HCOfferedOctets.high, pRow->mib.HCOfferedOctets.low, 
                 pRow->mib.HCDiscardedOctets.high, pRow->mib.HCDiscardedOctets.low);

    /* part C -- packet counts */
    DIFFSERV_PRT(msgLvlReqd, "                               "); /* spacer */
    DIFFSERV_PRT(msgLvlReqd, "pkt: %11u %11u %11u %11u\n", 
                 pRow->mib.HCOfferedPackets.high, pRow->mib.HCOfferedPackets.low, 
                 pRow->mib.HCDiscardedPackets.high, pRow->mib.HCDiscardedPackets.low);

    /* update search keys for next pass */
    keys = pRow->key;
  }
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}
                             

/*
=================================================================
=================================================================
=================================================================

   POLICY-CLASS OUTBOUND PERFORMANCE TABLE DEBUG SHOW FUNCTION

=================================================================
=================================================================
=================================================================
*/

/*********************************************************************
* @purpose  Display the current DiffServ Policy Performance Outbound Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsmibPolicyPerfOutTableShow(void)
{
  L7_uint32                msgLvlReqd;
  dsmibPolicyPerfOutCtrl_t *pRow;
  dsmibPolicyPerfOutKey_t  keys;
  avlTree_t                *pTree = &dsmibAvlTree[DSMIB_TABLE_ID_POLICY_PERF_OUT];
  L7_uint32                count;

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Policy Perf Outbound Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Policy Perf Outbound Table (contains %u entries):\n\n", count);

  DIFFSERV_PRT(msgLvlReqd, "Indexes  ST RS TID  Flags   RP ");
  DIFFSERV_PRT(msgLvlReqd, "Ctr: Offered(hi) Offered(lo) Discard(hi) Discard(lo)\n");
  DIFFSERV_PRT(msgLvlReqd, "-------- -- -- --- -------- -- ");
  DIFFSERV_PRT(msgLvlReqd, "---- ----------- ----------- ----------- -----------\n");

  keys.policyIndex = 0;                          /* start with first entry */
  keys.policyInstIndex = 0;
  keys.ifIndex = 0;                                                                              
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  { 
    /* part A */
    DIFFSERV_PRT(msgLvlReqd, "%2u %2u %2u %2s %2s %2u  %8.8x %2s ", 
                 pRow->key.policyIndex, 
                 pRow->key.policyInstIndex, 
                 pRow->key.ifIndex,
                 dsmibStgTypeStr[pRow->mib.storageType], 
                 dsmibRowStatusStr[pRow->mib.rowStatus],
                 pRow->tableId, 
                 pRow->rowInvalidFlags,
                 dsmibRowPendingStr[pRow->rowPending]);
    
    /* part B -- octet counts */
    DIFFSERV_PRT(msgLvlReqd, "oct: %11u %11u %11u %11u\n", 
                 pRow->mib.HCOfferedOctets.high, pRow->mib.HCOfferedOctets.low, 
                 pRow->mib.HCDiscardedOctets.high, pRow->mib.HCDiscardedOctets.low);

    /* part C -- packet counts */
    DIFFSERV_PRT(msgLvlReqd, "                               "); /* spacer */
    DIFFSERV_PRT(msgLvlReqd, "pkt: %11u %11u %11u %11u\n", 
                 pRow->mib.HCOfferedPackets.high, pRow->mib.HCOfferedPackets.low, 
                 pRow->mib.HCDiscardedPackets.high, pRow->mib.HCDiscardedPackets.low);

    /* update search keys for next pass */
    keys = pRow->key;
  }
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

