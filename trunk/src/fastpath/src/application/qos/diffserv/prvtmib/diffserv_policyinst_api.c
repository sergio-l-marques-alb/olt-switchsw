/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_policyinst_api.c
*
* @purpose    DiffServ component Private MIB Policy-Class Instance Table
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
#include "usmdb_mib_diffserv_private_api.h"


extern avlTree_t  dsmibAvlTree[];
extern void *     dsmibIndexSemId;              /* MIB tables index semaphore */

/*
======================================
======================================
======================================

   POLICY-CLASS INSTANCE TABLE APIs

======================================
======================================
======================================
*/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating
*           a new row in the Policy-Class Instance Table
*
* @param    policyIndex      @b{(input)}  Policy index
* @param    pPolicyInstIndex @b{(output)} Pointer to instance index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pPolicyInstIndex value
*           of 0 means the table is full.
*
* @notes    The index value remains free until used in a subsequent create
*           request.  When two or more requestors get the same index value,
*           only the first one to successfully create a new row in this table
*           gets to keep it.  All other requestors' create requests will
*           fail due to the index being in use, and they must call this
*           function again to get a new value.
*
* @notes    Although an index value is considered in use once a
*           successful row create occurs, the next index value is not
*           revealed until the row is either completed or deleted.
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstIndexNext(L7_uint32 policyIndex,
                                    L7_uint32 *pPolicyInstIndex)
{
  L7_RC_t       rc = L7_SUCCESS;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (pPolicyInstIndex == L7_NULLPTR)
    return L7_FAILURE;

  DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

  do                                            /* single-pass control loop */
  {
    /* always output a value of 0 if the MIB table size is at maximum */
    if (dsmibMibTableSizeCheck(DSMIB_TABLE_ID_POLICY_INST) != L7_SUCCESS)
    {
      /* table full -- L7_SUCCESS */
      *pPolicyInstIndex = 0;
      break;
    }

    if (dsmibPolicyInstIndexNextRead(policyIndex, pPolicyInstIndex) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

  } while (0);

  DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

  if (rc != L7_SUCCESS)
    *pPolicyInstIndex = 0;

  return rc;
}

/*************************************************************************
* @purpose  Retrieve the maximum policy instance index value allowed for the
*           Policy Instance Table
*
* @param    pIndexMax   @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is purely informational.  The corresponding table 'IndexNext'
*           function must be used to obtain the current index value.
*
* @notes    There is a difference between this maximum index and the
*           maximum table size.  The latter encompasses independent
*           sets of index maximums.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstIndexMaxGet(L7_uint32 *pIndexMax)
{
  /* check inputs */
  if (pIndexMax == L7_NULLPTR)
    return L7_FAILURE;

  *pIndexMax = DSMIB_POLICY_INST_INDEX_MAX;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a new row in the Policy-Class Instance Table for the
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    activateRow     @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Index value in use, row not complete
* @returns  L7_ALREADY_CONFIGURED  Index value in use, row complete
* @returns  L7_REQUEST_DENIED      Config change not approved
* @returns  L7_FAILURE
*
* @notes    An L7_ERROR response means the policy instance index value is
*           currently in use, but the row is not complete.  This is typically
*           caused when multiple requestors obtain the same "index next"
*           value and race to create the new row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_ALREADY_CONFIGURED response means the policy instance index
*           value is currently in use for a completed row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_REQUEST_DENIED response means another application
*           has denied approval of this configuration change.
*
* @notes    The row status is initially set to 'not ready' and remains that
*           way until every object in the row contains a valid value.  If the
*           activateRow parameter is set to L7_TRUE, the row status will then
*           be automatically updated to 'active'.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstCreate(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_BOOL activateRow)
{
  L7_RC_t                    rc = L7_FAILURE;
  L7_uint32                  dataChanged = L7_FALSE;
  dsmibTableId_t             tableId = DSMIB_TABLE_ID_POLICY_INST;
  dsmibPolicyInstEntryCtrl_t row, *pRow = L7_NULLPTR;
  L7_uint32                  currCount = 0;
  dsmibPolicyEntryCtrl_t     *pPolicy;

  DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId,
                           policyIndex, policyInstIndex, 0,
                           (L7_uchar8)activateRow);

  do
  {
    /* check inputs */
    if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
      break;
    if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
      break;
    if ((activateRow != L7_FALSE) && (activateRow != L7_TRUE))
      break;

    /* verify index value matches expected value (i.e., current 'IndexNext')
     * for lowest-order creation index
     */
    if (dsmibPolicyInstIndexNextVerify(policyIndex, policyInstIndex) != L7_SUCCESS)
      break;

    /* make sure row does not already exist (someone else could have created
     * it first)
     */
    pRow = dsmibPolicyInstRowFind(policyIndex, policyInstIndex);
    if (pRow != L7_NULLPTR)           /* row exists */
    {
      rc = (DSMIB_ROW_IS_COMPLETE(pRow) == L7_TRUE) ? L7_ALREADY_CONFIGURED : L7_ERROR;
      break;
    }

    /* make sure there is room in the table */
    if (dsmibMibTableSizeCheck(tableId) != L7_SUCCESS)
      break;

    /* get access to the policy row */
    pPolicy = dsmibPolicyRowFind(policyIndex);
    if (pPolicy == L7_NULLPTR)
      break;

    /* check with callback registrants to make sure instance can be created for this policy */
    currCount = pPolicy->instCount;
    if (diffServChangeApprovalQuery(L7_DIFFSERV_CHANGE_APPROVAL_POLICY_INST_CREATE,
                                    policyIndex, &currCount) != L7_SUCCESS)
    {
      rc = L7_REQUEST_DENIED;
      break;
    }

    /* setup a temporary MIB row element structure */
    pRow = &row;
    memset(pRow, 0, sizeof(*pRow));

    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* initialize row index keys */
    pRow->key.policyIndex = policyIndex;
    pRow->key.policyInstIndex = policyInstIndex;

    /* initialize control fields */
    pRow->tableId = tableId;
    pRow->rowInvalidFlags = DSMIB_POLICY_INST_FLAGS_COMMON;  /* starting value */
    pRow->pPolicy = pPolicy;                          /* back ptr to policy row */
    pRow->attrCount = 0;
    pRow->rowPending = L7_TRUE;

    /* fill in initial row status and any objects with known defaults */
    pRow->mib.index = policyInstIndex;
    pRow->mib.attrIndexNextFree = DSMIB_AUTO_INCR_INDEX_NEXT_START;
    pRow->mib.storageType = DSMIB_CFG_DEFAULT_STORAGE_TYPE;
    /* NOTE: Setting row status to one of the 'create' values here, but this
     *       is presented externally as 'not ready' when read.
     */
    pRow->mib.rowStatus = (activateRow == L7_TRUE) ?
                          L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO :
                          L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT;

    /* add row to AVL tree in sorted order */
    rc = dsmibRowInsert(&dsmibAvlTree[tableId], pRow);

    if (rc == L7_SUCCESS)
    {
      /* successful row creation -- update current table size */
      genStatus.tableSize[tableId]++;

      /* keep track of the number of instances within a policy */
      pPolicy->instCount++;

      dataChanged = L7_TRUE;            /* config change occurred */

      /* call distiller after row creation in a "dependent" table so the change
       * gets detected
       */
      rc = dsDistillerEvaluateAll();
      if (rc != L7_SUCCESS)
      {
        /* delete the just-created row due to evaluation failure */
        if (dsmibRowDelete(&dsmibAvlTree[tableId], pRow) == L7_SUCCESS)
        {
          /* clean up the various count values that were incremented above */
          genStatus.tableSize[tableId]--;
          pPolicy->instCount--;

          dataChanged = L7_FALSE;       /* config change undone */
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                  "diffServPolicyInstCreate: Error deleting row after create "
                  "failure for indexes %u,%u\n", policyIndex, policyInstIndex);
        }
      }
    }
    else
    {
      /* problem with AVL insertion */
      rc = L7_FAILURE;
    }

    DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

    if (rc == L7_SUCCESS)
    {
      /* create an entry in the appropriate Policy Perf Table for each service
       * interface to which this instance is attached
       */
      if (dsmibPolicyInstPerfAttach(policyIndex, policyInstIndex) != L7_SUCCESS)
        rc = L7_FAILURE;
    }

  } while (0);

  /* make sure change gets picked up when config is saved */
  if (dataChanged == L7_TRUE)
    pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId,
                          policyIndex, policyInstIndex, 0,
                          (L7_uint32)pRow, (L7_uchar8)rc,
                          (L7_uchar8)activateRow);

  return rc;
}

/*************************************************************************
* @purpose  Delete a row from the Policy-Class Instance Table for the
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Row does not exist for the specified index
* @returns  L7_REQUEST_DENIED      Config change not approved
* @returns  L7_FAILURE
*
* @notes    An L7_REQUEST_DENIED response means another application
*           has denied approval of this configuration change.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstDelete(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex)
{
  /* delete the policy instance, but allow distiller eval to occur */
  return dsmibPolicyInstDelete(policyIndex, policyInstIndex, L7_FALSE);
}

/*************************************************************************
* @purpose  Delete all rows from the Policy-Class Instance Table for the
*           specified policy
*
* @param    policyIndex     @b{(input)} Policy index
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED      Config change not approved
* @returns  L7_FAILURE
*
* @notes    An L7_REQUEST_DENIED response means another application
*           has denied approval of this configuration change.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstDeleteAll(L7_uint32 policyIndex)
{
  L7_RC_t                rc = L7_SUCCESS;
  L7_uint32              policyInstIndex;
  L7_uint32              nextPolicyIndex;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;

  /* delete all instances belonging to this policy */
  policyInstIndex = 0;                           /* start with first instance */
  while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex, &nextPolicyIndex,
                                    &policyInstIndex) == L7_SUCCESS) &&
         (nextPolicyIndex == policyIndex))
  {
    if (policyInstIndex == 0)
      break;

    /* delete the policy instance and set flag to bypass distiller eval (caller
     * will do it)
     *
     * not sending policy notify event callbacks from here either (caller decides)
     */
    if (dsmibPolicyInstDelete(policyIndex, policyInstIndex, L7_TRUE) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
              "%s: could not delete instance %u from policy index %u\n",
              __FUNCTION__, policyInstIndex, policyIndex);
      rc = L7_FAILURE;
      break;
    }

  } /* endwhile */

  return rc;
}

/*************************************************************************
* @purpose  Delete a row from the Policy-Class Instance Table for the
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    bypassEval      @b{(input)} Flag for skipping distiller evaluation
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Row does not exist for the specified index
* @returns  L7_REQUEST_DENIED      Config change not approved
* @returns  L7_FAILURE
*
* @notes    Internal function used for normal and delete-all cases.
*           When bypassEval is L7_TRUE, this is part of a higher-level
*           deletion so the distiller evaluation can be skipped for
*           performance reasons.
*
* @notes    An L7_REQUEST_DENIED response means another application
*           has denied approval of this configuration change.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyInstDelete(L7_uint32 policyIndex,
                              L7_uint32 policyInstIndex,
                              L7_BOOL   bypassEval)
{
  L7_RC_t                    rc = L7_FAILURE;
  dsmibTableId_t             tableId = DSMIB_TABLE_ID_POLICY_INST;
  L7_BOOL                    undoIndexNext = L7_FALSE;
  L7_BOOL                    needNotify = L7_FALSE;
  dsmibPolicyInstEntryCtrl_t *pRow = L7_NULLPTR;
  dsmibPolicyEntryCtrl_t     *pPolicy;
  dsmibClassEntryCtrl_t      *pClass;
  dsmibRowStatus_t           prevRowStatus;
  L7_BOOL                    prevRowPending;


  DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId,
                           policyIndex, policyInstIndex, 0);

  do
  {
    /* check inputs */
    if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
      break;
    if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
      break;
    if ((bypassEval != L7_FALSE) && (bypassEval != L7_TRUE))
      break;

    /* find node in AVL tree */
    pRow = dsmibPolicyInstRowFind(policyIndex, policyInstIndex);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_ERROR;
      break;
    }

    /* check with callback registrants to make sure instance can be deleted for this policy */
    if (diffServChangeApprovalQuery(L7_DIFFSERV_CHANGE_APPROVAL_POLICY_INST_DELETE,
                                    policyIndex, L7_NULLPTR) != L7_SUCCESS)
    {
      rc = L7_REQUEST_DENIED;
      break;
    }

    /* obtain policy row pointer and save it for later */
    pPolicy = pRow->pPolicy;

    /* change an 'ACTIVE' row status to 'NOT_IN_SERVICE' prior to row deletion
     * (this transition needed for proper device cleanup)
     */
    prevRowStatus = pRow->mib.rowStatus;
    if (prevRowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      pRow->mib.rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;

      if (dsmibRowEventCheck(DSMIB_TABLE_ID_POLICY_INST, pRow,
                             prevRowStatus, pRow->mib.rowStatus) != L7_SUCCESS)
        break;
    }

    /* delete all attributes belonging to this policy-class instance */
    if (diffServPolicyAttrDeleteAll(policyIndex, policyInstIndex) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

    /* initialize class row pointer */
    pClass = L7_NULLPTR;

    /* check if class index has ever been set */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_POLICY_INST_FLAGS_CLASSINDEX))
    {
      /* find node in the AVL tree */
      pClass = dsmibClassRowFind(pRow->mib.classIndex);
      if (pClass == L7_NULLPTR)
      {
        rc = L7_ERROR;
        break;
      }
    }

    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* save row pending flag for change notification */
    prevRowPending = pRow->rowPending;

    /* before deleting the actual row, check if this is the most recent instance
     * created for this policy (successful creations use sequential policy instance
     * index values)
     *
     * if so, set a flag to undo the indexNext value if and only if the row
     * had ever reached completion status
     */
    if (diffServPolicyInstIsMostRecent(policyIndex, policyInstIndex) == L7_TRUE)
    {
      if (pRow->rowPending != L7_TRUE)
        undoIndexNext = L7_TRUE;
    }

    /* delete the policy instance row AVL tree node */
    rc = dsmibRowDelete(&dsmibAvlTree[tableId], pRow);

    if (rc == L7_SUCCESS)
    {
      /* successful row deletion
       * NOTE: Do not use pRow from this point on.
       */
      pRow = L7_NULLPTR;

      /* if this had been the most recently created policy instance row and it
       * reached completion status at least once, undo the indexNext value
       *
       * NOTE: This is being done primarily to handle internal failure cases
       *       during the normal establishment of a row.
       */
      if (undoIndexNext == L7_TRUE)
        dsmibPolicyInstIndexNextUndo(policyIndex);

      /* update current table size */
      genStatus.tableSize[tableId]--;

      /* this instance is no longer in the policy */
      pPolicy->instCount--;

      if (pClass != L7_NULLPTR)
      {
        /* decrement the refcount in the class row */
        pClass->refCount--;
      }

      /* make sure change gets picked up when config is saved */
      pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;

      if (bypassEval != L7_TRUE)
      {
        /* call distiller after row deletion */
        rc = dsDistillerEvaluateAll();

        /* set flag if callbacks are needed
         *
         * NOTE: No point in sending a notification if deleting a row
         *       that was never fully completed.
         */
        if (prevRowPending == L7_FALSE)
          needNotify = L7_TRUE;
      }
    }
    else
    {
      L7_uchar8   policyName[L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1];

      /* something's wrong */
      if (diffServPolicyObjectGet(policyIndex, L7_DIFFSERV_POLICY_NAME,
                                  policyName) == L7_SUCCESS)

      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "diffServPolicyInstDelete: couldn't delete row AVL node for "
                "instance %u in policy \'%s\'\n", policyInstIndex,
                (char *)policyName);
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "diffServPolicyInstDelete: couldn't delete row AVL node for "
                "indexes %u,%u\n", policyIndex, policyInstIndex);
      }
      rc = L7_FAILURE;
    }

    DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

    if (rc == L7_SUCCESS)
    {
      /* delete an entry from the appropriate Policy Perf Table for each service
       * interface to which this instance is attached
       */
      if (dsmibPolicyInstPerfDetach(policyIndex, policyInstIndex) != L7_SUCCESS)
        rc = L7_FAILURE;
    }

  } while (0);

  if (needNotify == L7_TRUE)
  {
    /* send notify event callbacks for this policy
     *
     * NOTE: A policy instance deletion is considered a 'modify' event for the policy
     */
    diffServChangeNotify(policyIndex, DIFFSERV_EVENT_MODIFY, L7_NULLPTR);
  }

  DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId,
                          policyIndex, policyInstIndex, 0,
                          (L7_uint32)pRow, (L7_uchar8)rc);

  return rc;
}

/*********************************************************************
* @purpose  Verify that a Policy-Class Instance Table row exists for the
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstGet(L7_uint32 policyIndex,
                              L7_uint32 policyInstIndex)
{
  dsmibPolicyInstEntryCtrl_t *pRow;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;

  /* find node in AVL tree */
  pRow = dsmibPolicyInstRowFind(policyIndex, policyInstIndex);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_ERROR;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Policy-Class Instance Table
*
* @param    prevPolicyIndex     @b{(input)}  Policy index to begin search
* @param    prevPolicyInstIndex @b{(input)}  Policy instance index to begin
*                                              search
* @param    pPolicyIndex        @b{(output)} Pointer to next sequential policy
*                                              index value
* @param    pPolicyInstIndex    @b{(output)} Pointer to next sequential policy
*                                              instance index value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE
*
* @notes    A prevPolicyIndex value of 0 is used to find the first policy
*           entry in the table.
*
* @notes    A prevPolicyInstIndex value of 0 is used to find the first policy
*           instance relative to the specified prevPolicyIndex.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstGetNext(L7_uint32 prevPolicyIndex,
                                  L7_uint32 prevPolicyInstIndex,
                                  L7_uint32 *pPolicyIndex,
                                  L7_uint32 *pPolicyInstIndex)
{
  dsmibPolicyInstEntryCtrl_t *pRow;
  dsmibPolicyInstKey_t       keys;

  /* check inputs */
  /* NOTE: Allow any starting index value to be used. */
  if (pPolicyIndex == L7_NULLPTR)
    return L7_FAILURE;
  if (pPolicyInstIndex == L7_NULLPTR)
    return L7_FAILURE;

  /* find node in AVL tree
   * NOTE: Need to use direct call here due to AVL_NEXT flag.
   */
  keys.policyIndex = prevPolicyIndex;
  keys.policyInstIndex = prevPolicyInstIndex;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_POLICY_INST],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
    /* next row not found -- must have reached end of table */
    return L7_ERROR;

  /* found next sequential table row -- set caller's output variables */
  *pPolicyIndex = pRow->key.policyIndex;
  *pPolicyInstIndex = pRow->key.policyInstIndex;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Policy-Class
*           Instance Table to determine its legitimacy
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    oid             @b{(input)} Object identifier
* @param    pValue          @b{(input)} Pointer to proposed value to be set
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Proposed value invalid
* @returns  L7_FAILURE
*
* @notes    This is a 'wrapper' function used for tracing.  The real code
*           is contained in dsmibPolicyInstSetTest().
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstSetTest(L7_uint32 policyIndex,
                                  L7_uint32 policyInstIndex,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_TYPE_t oid,
                                  void *pValue)
{
  dsmibTableId_t              tableId = DSMIB_TABLE_ID_POLICY_INST;
  dsmibPolicyInstEntryCtrl_t  *pRowTrace = L7_NULLPTR;
  size_t                      dsTraceValueLen = 0;
  L7_uchar8                   dsTraceValueData[DS_TRACE_OBJ_VAL_LEN_MAX];
  L7_RC_t                     rc;

  rc = dsmibPolicyInstSetTest(policyIndex, policyInstIndex, oid, pValue,
                              &pRowTrace, &dsTraceValueLen);

  if (rc != L7_SUCCESS)
  {
    /* format the OID value for the trace point */
    dsTraceObjectValFormat(pValue, dsTraceValueLen, dsTraceValueData);

    DS_TRACE_PT_OBJECT_SETTEST_FAIL(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId,
                                    policyIndex, policyInstIndex, 0,
                                    (L7_ulong32)pRowTrace, (L7_uchar8)rc,
                                    (L7_uchar8)oid, (L7_uchar8)dsTraceValueLen,
                                    dsTraceValueData);
  }

  return rc;
}

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Policy-Class
*           Instance Table to determine its legitimacy
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy instance index
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(input)}  Pointer to proposed value to be set
* @param    ppRow           @b{(output)} Pointer to row ptr output location
* @param    pValueLen       @b{(output)} Pointer to value length output location
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Proposed value invalid
* @returns  L7_FAILURE
*
* @notes    Only tests objects with access types READ-CREATE or READ-WRITE;
*           all other access types return L7_FAILURE.
*
* @notes    Caller must use one of the _SETTEST_OID_TYPE object identifiers
*           only (i.e., do not use the _TABLE_OBJECT type object names here).
*
* @notes    A *pointer* to the test value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @notes    The ppRow and pValueLen parms are used for tracing.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyInstSetTest(L7_uint32 policyIndex,
                               L7_uint32 policyInstIndex,
                               L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_TYPE_t oid,
                               void *pValue,
                               dsmibPolicyInstEntryCtrl_t **ppRow,
                               size_t *pValueLen)
{
  dsmibPolicyInstEntryCtrl_t      *pRow;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the policy instance row */
  pRow = dsmibPolicyInstRowFind(policyIndex, policyInstIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  *ppRow = pRow;

  /* compare object against its allowed values */
  switch (oid)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_CLASS_INDEX:
    {
      L7_uint32 classIndex = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(classIndex);

      /* check class index */
      if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
        return L7_FAILURE;

      /* make sure class exists */
      if (diffServClassGet(classIndex) != L7_SUCCESS)
        return L7_ERROR;

      /* make sure class definition is supported by this policy */
      if (dsmibPolicyInstClassValidate(policyIndex, classIndex) != L7_SUCCESS)
        return L7_ERROR;

      /* once set, this value is not allowed to change */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_POLICY_INST_FLAGS_CLASSINDEX))
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_STORAGE_TYPE:
    {
      dsmibStorageType_t  storageType = *(dsmibStorageType_t *)pValue;
      *pValueLen = sizeof(storageType);

      if (dsmibStorageTypeIsValid(storageType) != L7_TRUE)
        return L7_ERROR;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_ROW_STATUS:
    {
      dsmibRowStatus_t  rowStatus = *(dsmibRowStatus_t *)pValue;
      *pValueLen = sizeof(rowStatus);

      /* restrict certain row status 'destroy' requests
       * (unless in 'special config mode')
       */
      if (rowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY)
      {
        if (dsmibSpecialConfigModeGet() != L7_TRUE)
        {
          /* only the most recent instance row in the policy may be destroyed */
          if (diffServPolicyInstIsMostRecent(policyIndex, policyInstIndex) != L7_TRUE)
            return L7_ERROR;
        }
      }

      /* always return here for row status */
      return (dsmibRowStatusIsValid(rowStatus) == L7_TRUE) ?
               L7_SUCCESS : L7_ERROR;
    }
    /*PASSTHRU*/

  default:
    /* invalid oid */
    return L7_FAILURE;
    /*PASSTHRU*/
  }

  /* NOTE: Put any common checking here if value depends on row status state.*/

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the specified object from the Policy-Class Instance Table
*
* @param    policyIndex      @b{(input)}  Policy index
* @param    policyInstIndex  @b{(input)}  Policy instance index
* @param    oid              @b{(input)}  Object identifier
* @param    pValue           @b{(output)} Pointer to the object output value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Object value not yet established
* @returns  L7_FAILURE  Improper usage or other malfunction
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstObjectGet(L7_uint32 policyIndex,
                                    L7_uint32 policyInstIndex,
                                    L7_DIFFSERV_POLICY_INST_TABLE_OBJECT_t oid,
                                    void *pValue)
{
  dsmibPolicyInstEntryCtrl_t *pRow;
  L7_RC_t                    rc;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the policy instance row */
  pRow = dsmibPolicyInstRowFind(policyIndex, policyInstIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  /* check for proper object type and row invalid flag state before proceeding*/
  if ((rc = dsmibPolicyInstObjectGetCheck(pRow, oid)) != L7_SUCCESS)
    return rc;

  /* output the current value of the object */
  switch (oid)
  {
  case L7_DIFFSERV_POLICY_INST_CLASS_INDEX:
    {
      L7_uint32     *pClassIndex = pValue;
      *pClassIndex = pRow->mib.classIndex;
    }
    break;

  case L7_DIFFSERV_POLICY_INST_STORAGE_TYPE:
    {
      dsmibStorageType_t  *pStorageType = pValue;
      *pStorageType = pRow->mib.storageType;
    }
    break;

  case L7_DIFFSERV_POLICY_INST_ROW_STATUS:
    {
      dsmibRowStatus_t  *pRowStatus = pValue;
      if (dsmibRowStatusObservable(pRow->mib.rowStatus,
                                   (L7_uint32)pRow->rowInvalidFlags,
                                   pRowStatus) != L7_SUCCESS)
        return L7_FAILURE;
    }
    break;

  default:
    /* invalid oid */
    return L7_FAILURE;
    /*PASSTHRU*/
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the specified object in the Policy-Class Instance Table
*
* @param    policyIndex      @b{(input)}  Policy index
* @param    policyInstIndex  @b{(input)}  Policy instance index
* @param    oid              @b{(input)}  Object identifier
* @param    pValue           @b{(output)} Pointer to the object set value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Not allowed for current entry type
* @returns  L7_FAILURE
*
* @notes    This is a 'wrapper' function used for tracing.  The real code
*           is contained in dsmibPolicyInstObjectSet().
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstObjectSet(L7_uint32 policyIndex,
                                    L7_uint32 policyInstIndex,
                                    L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_TYPE_t oid,
                                    void *pValue)
{
  dsmibTableId_t              tableId = DSMIB_TABLE_ID_POLICY_INST;
  dsmibPolicyInstEntryCtrl_t  *pRowTrace = L7_NULLPTR;
  size_t                      dsTraceValueLen = 0;
  L7_uchar8                   dsTraceValueData[DS_TRACE_OBJ_VAL_LEN_MAX];
  L7_RC_t                     rc;

  rc = dsmibPolicyInstObjectSet(policyIndex, policyInstIndex, oid, pValue,
                                &pRowTrace, &dsTraceValueLen);

  /* format the OID value for the trace point */
  dsTraceObjectValFormat(pValue, dsTraceValueLen, dsTraceValueData);

  DS_TRACE_PT_OBJECT_SET(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId,
                         policyIndex, policyInstIndex, 0,
                         (L7_ulong32)pRowTrace, (L7_uchar8)rc,
                         (L7_uchar8)oid, (L7_uchar8)dsTraceValueLen,
                         dsTraceValueData);

  return rc;
}

/*********************************************************************
* @purpose  Set the specified object in the Policy-Class Instance Table
*
* @param    policyIndex      @b{(input)}  Policy index
* @param    policyInstIndex  @b{(input)}  Policy instance index
* @param    oid              @b{(input)}  Object identifier
* @param    pValue           @b{(output)} Pointer to the object set value
* @param    ppRow            @b{(output)} Pointer to row ptr output location
* @param    pValueLen        @b{(output)} Pointer to value length output location
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Not allowed for current entry type
* @returns  L7_FAILURE
*
* @notes    A *pointer* to the set value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @notes    The ppRow and pValueLen are used for tracing.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyInstObjectSet(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_TYPE_t oid,
                                 void *pValue,
                                 dsmibPolicyInstEntryCtrl_t **ppRow,
                                 size_t *pValueLen)
{
  dsmibPolicyInstEntryCtrl_t *pRow;
  dsmibRowStatus_t           restoreStatus, rowStatusRaw, oldStatus, newStatus;
  L7_RC_t                    rc;
  L7_BOOL                    justCompleted;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* reuse 'SetTest' function to check for valid oid and settable value */
  if (diffServPolicyInstSetTest(policyIndex, policyInstIndex, oid, pValue)
        != L7_SUCCESS)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibPolicyInstRowFind(policyIndex, policyInstIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  *ppRow = pRow;

  /* save a copy of the old row status for a comparison below */
  restoreStatus = rowStatusRaw = pRow->mib.rowStatus;
  if (dsmibRowStatusObservable(rowStatusRaw, (L7_uint32)pRow->rowInvalidFlags,
                               &oldStatus) != L7_SUCCESS)
    return L7_FAILURE;

  if (oid != L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_ROW_STATUS)
  {
    /* NOTE: Temporarily change an 'ACTIVE' row to 'NOT_IN_SERVICE' when
     *       setting any row object (other than the row status object itself).
     *       The 'oldStatus' value is changed here, so that re-activation
     *       automatically occurs below after the object value is set.
     */
    if (oldStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* set the actual rowStatus to notInService for the row event check */
      pRow->mib.rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;

      rc = dsmibRowEventCheck(DSMIB_TABLE_ID_POLICY_INST, pRow,
                              oldStatus, pRow->mib.rowStatus);

      /* restore the actual mib.rowStatus to its previous value */
      oldStatus = pRow->mib.rowStatus;
      pRow->mib.rowStatus = restoreStatus;

      /* now check result of the row event check */
      if (rc != L7_SUCCESS)
        return L7_FAILURE;
    }
  }

  /* set the object value and adjust the row invalid flags */
  switch (oid)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_CLASS_INDEX:
    {
      L7_uint32     classIndex = *(L7_uint32 *)pValue;
      dsmibClassEntryCtrl_t *pClass;

      *pValueLen = sizeof(classIndex);

      /* increment the new class refCount */
      pClass = dsmibClassRowFind(classIndex);
      if (pClass == L7_NULLPTR)
        return L7_FAILURE;
      pClass->refCount++;                       /* count this class reference */

      pRow->mib.classIndex = classIndex;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_INST_FLAGS_CLASSINDEX);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_STORAGE_TYPE:
    {
      dsmibStorageType_t storageType = *(dsmibStorageType_t *)pValue;
      *pValueLen = sizeof(storageType);
      pRow->mib.storageType = storageType;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_ROW_STATUS:
    {
      dsmibRowStatus_t  reqStatus = *(dsmibRowStatus_t *)pValue;
      *pValueLen = sizeof(reqStatus);

      /* determine the actual row status value to set (the MIB object is
       * changed below)
       */
      if (dsmibRowStatusUpdate(pRow->mib.rowStatus, reqStatus,
                               pRow->rowInvalidFlags,
                               &rowStatusRaw) != L7_SUCCESS)
        return L7_FAILURE;
    }
    break;

  default:
    /* invalid oid */
    return L7_FAILURE;
    /*PASSTHRU*/
  } /* endswitch */

  /* set the data changed flag to L7_TRUE as the MIB object value is set */
  pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  /* when setting any object other than row status, check for a possible
   * row completion to cause the row status to be updated
   */
  if (oid != L7_USMDB_MIB_DIFFSERV_POLICY_INST_SETTEST_OID_ROW_STATUS)
  {
    /* allow for possible update of row status due to row completion */
    dsmibRowCompleteCheck(pRow->mib.rowStatus, pRow->rowInvalidFlags,
                          &rowStatusRaw, &justCompleted);

    if (justCompleted == L7_TRUE)
    {
      /* the rowPending flag must be cleared the first time a row is completed
       * (no harm in clearing it again for subsequent passes through this
       * code path)
       */
      pRow->rowPending = L7_FALSE;

      DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

      /* successful row completion -- update next free index value
       * NOTE: Must do this under index semaphore control.
       */
      dsmibPolicyInstIndexNextUpdate(policyIndex);

      DIFFSERV_SEMA_GIVE(dsmibIndexSemId);
    }
  }

  /* always update the row status MIB object here, since it may have changed
   * from its original value
   */
  pRow->mib.rowStatus = rowStatusRaw;

  /* only take further action if the (observable) status has actually changed
   * NOTE:  May occur from explicit row status set, or due to completion
   *        of a previously created row.
   */
  if (dsmibRowStatusObservable(rowStatusRaw, (L7_uint32)pRow->rowInvalidFlags,
                               &newStatus) != L7_SUCCESS)
    return L7_FAILURE;

  if (newStatus != oldStatus)
  {
    /* NOTE: Act on important row state transitions here. This is where
     *       other parts of the system get their chance to do their thing.
     */
    if (dsmibRowEventCheck(DSMIB_TABLE_ID_POLICY_INST, pRow, oldStatus, newStatus)
          != L7_SUCCESS)
      return L7_FAILURE;

    if (pRow->mib.rowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY)
    {
      /* for a new row status of 'destroy', it is now time to delete the row */
      if (diffServPolicyInstDelete(policyIndex, policyInstIndex) != L7_SUCCESS)
      {
        pRow->mib.rowStatus = restoreStatus;
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "diffServPolicyInstObjectSet: attempt to destroy the row failed\n");
        return L7_FAILURE;
      }
    }
    else
    {
      /* send notify event callback */
      diffServChangeNotify(policyIndex, DIFFSERV_EVENT_MODIFY, L7_NULLPTR);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Find the policy instance index from the policy and class indexes
*
* @param    policyIndex       @b{(input)}  Policy index
* @param    classIndex        @b{(input)}  Class index
* @param    pPolicyInstIndex  @b{(output)} Pointer to policy instance index
*                                            value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is a supplemental function provided to assist the user
*           interface code by searching through the Policy-Class Instance
*           Table for the policy instance index whose row matches the
*           specified class index for the policy.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyInstIndexFromClass(L7_uint32 policyIndex,
                                         L7_uint32 classIndex,
                                         L7_uint32 *pPolicyInstIndex)
{
  L7_uint32 policyInstIndex;
  L7_uint32 nextPolicyIndex;
  L7_uint32 searchClassIndex;

  policyInstIndex = 0;
  while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex,
                        &nextPolicyIndex, &policyInstIndex) == L7_SUCCESS) &&
         (nextPolicyIndex == policyIndex))
  {
    if (policyInstIndex == 0)
      break;

    if(diffServPolicyInstObjectGet(policyIndex, policyInstIndex,
                                   L7_DIFFSERV_POLICY_INST_CLASS_INDEX,
                                   &searchClassIndex) != L7_SUCCESS)
      break;

    if (searchClassIndex == classIndex)
    {
      *pPolicyInstIndex = policyInstIndex;
      return L7_SUCCESS;
    }

  } /* endwhile */

  *pPolicyInstIndex = 0;
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine if the policy instance is the most recently created
*           instance in the policy (i.e., is last one in current list)
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
*
* @returns  L7_TRUE     Policy instance is most recent
* @returns  L7_FALSE    Policy instance not most recent
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL diffServPolicyInstIsMostRecent(L7_uint32 policyIndex,
                                       L7_uint32 policyInstIndex)
{
  L7_RC_t       rc;
  L7_uint32     nextPolicy, nextInst;

  /* NOTE: Assumes a 'GetNext' L7_ERROR means the end of table was reached,
   *       which implies this is the last policy instance in the table and
   *       therefore must also be the most recent instance in its policy.
   */
  rc = diffServPolicyInstGetNext(policyIndex, policyInstIndex,
                                 &nextPolicy, &nextInst);

  if ((rc == L7_SUCCESS) && (nextPolicy != policyIndex))
    return L7_TRUE;

  if (rc == L7_ERROR)
    return L7_TRUE;

  return L7_FALSE;
}


/*
==================================================
==================================================
==================================================

   POLICY-CLASS INSTANCE TABLE HELPER FUNCTIONS

==================================================
==================================================
==================================================
*/

/*********************************************************************
* @purpose  Provide the current value of the policy-class instance
*           index next variable
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    pNextFree       @b{(output)} Pointer to next free index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Does not guarantee this index value will be valid at the time
*           a row create is attempted.
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyInstIndexNextRead(L7_uint32 policyIndex, L7_uint32 *pNextFree)
{
  dsmibPolicyEntryCtrl_t *pRow;

  /* get access to the policy row */
  pRow = dsmibPolicyRowFind(policyIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  *pNextFree = pRow->mib.instIndexNextFree;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine next available policy-class instance index value
*           and update its value
*
* @param    policyIndex     @b{(input)}  Policy index
*
* @returns  void
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*
* @end
*********************************************************************/
void dsmibPolicyInstIndexNextUpdate(L7_uint32 policyIndex)
{
  dsmibPolicyEntryCtrl_t *pRow;
  L7_uint32              indexNext, i;

  indexNext = DSMIB_AUTO_INCR_INDEX_NEXT_START;
  for (i = 1; i <= DSMIB_POLICY_INST_INDEX_MAX; i++)
  {
    if (dsmibPolicyInstRowFind(policyIndex, i) != L7_NULLPTR)
      indexNext = i + 1;
  }

  if (indexNext > DSMIB_POLICY_INST_INDEX_MAX)
    indexNext = 0;

  /* get access to the policy row */
  pRow = dsmibPolicyRowFind(policyIndex);
  if (pRow == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "dsmibPolicyInstIndexNextUpdate: ERROR -- could not find "
            "policy row for index %u\n", policyIndex);
    return;                                     /* ERROR -- should not occur */
  }

  pRow->mib.instIndexNextFree = indexNext;
}

/*********************************************************************
* @purpose  Undo next available policy instance index value for a deleted row
*
* @param    policyIndex     @b{(input)}  Policy index
*
* @returns  void
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*
* @notes    CAUTION! This only works when the most recently created row
*           is deleted, and must not be called unless that row had been
*           completed.
*
* @end
*********************************************************************/
void dsmibPolicyInstIndexNextUndo(L7_uint32 policyIndex)
{
  dsmibPolicyEntryCtrl_t  *pRow;
  L7_uint32               indexNext;

  /* get access to the policy row */
  pRow = dsmibPolicyRowFind(policyIndex);
  if (pRow == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "dsmibPolicyInstIndexNextUndo: ERROR -- could not find "
            "policy row for index %u\n", policyIndex);
    return;                                     /* ERROR -- should not occur */
  }

  indexNext = pRow->mib.instIndexNextFree;

  if (indexNext == 0)
    indexNext = DSMIB_POLICY_INST_INDEX_MAX;
  else if (indexNext > DSMIB_AUTO_INCR_INDEX_NEXT_START)
    indexNext--;
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "dsmibPolicyInstIndexNextUndo: WARNING -- attempting to decrement "
            "an indexNext value of %u for policy %u\n", indexNext, policyIndex);
    return;
  }

  pRow->mib.instIndexNextFree = indexNext;
}

/*********************************************************************
* @purpose  Verifies the specified policy-class instance index value
*           matches its current 'IndexNext' value
*
* @param    policyIndex      @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index to be
*                                        checked
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Primarily intended for use when creating a new row.
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyInstIndexNextVerify(L7_uint32 policyIndex,
                                       L7_uint32 policyInstIndex)
{
  L7_uint32     currentIndex;

  if (policyInstIndex == 0)
    return L7_FAILURE;

  /* skip this checking if currently in 'special config mode' */
  if (dsmibSpecialConfigModeGet() == L7_TRUE)
    return L7_SUCCESS;

  if (dsmibPolicyInstIndexNextRead(policyIndex, &currentIndex) != L7_SUCCESS)
    return L7_FAILURE;

  return (policyInstIndex == currentIndex) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Policy Instance Table row element
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
*
* @returns  pRow        Pointer to policy instance row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*
* @end
*********************************************************************/
dsmibPolicyInstEntryCtrl_t *dsmibPolicyInstRowFind(L7_uint32 policyIndex,
                                                   L7_uint32 policyInstIndex)
{
  dsmibPolicyInstKey_t        keys;
  dsmibPolicyInstEntryCtrl_t  *pRow;

  /* get access to the policy row */
  keys.policyIndex = policyIndex;
  keys.policyInstIndex = policyInstIndex;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_POLICY_INST],
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Checks if a class definition is supported for the specified
*           policy
*
* @param    policyIndex @b{(input)} Policy index
* @param    classIndex  @b{(input)} Class index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The entire class reference chain must meet the imposed
*           policy classification restrictions.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyInstClassValidate(L7_uint32 policyIndex,
                                     L7_uint32 classIndex)
{
  dsmibPolicyType_t      policyType;
  L7_uint32              searchClassIndex;
  L7_uint32              searchPolicyIndex;
  L7_uint32              searchPolicyInstIndex;
  L7_uint32              classRuleIndex;
  dsmibClassRuleType_t   ruleEntryType;
  dsmibClassL3Protocol_t classL3Proto, searchClassL3Proto;

  /* check if we need to restrict mixing of classes with l3proto equal to ipv4 and ipv6 in the same policy */
  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                            L7_DIFFSERV_POLICY_MIXED_IPV6_FEATURE_ID)
      != L7_TRUE)
  {
    /* retrieve the l3proto setting for the class to be added */
    if (diffServClassObjectGet(classIndex,
                               L7_DIFFSERV_CLASS_L3_PROTO,
                               &classL3Proto) == L7_SUCCESS)
    {
      /* check classes already in the policy to assure they have the same l3Proto setting */
      searchPolicyInstIndex = 0;  /* start with first instance entry in this policy */
      while (diffServPolicyInstGetNext(policyIndex, searchPolicyInstIndex,
                                       &searchPolicyIndex, &searchPolicyInstIndex) == L7_SUCCESS &&
             policyIndex == searchPolicyIndex)
      {
        /* get the index of the class referenced in this instance; ignore classes that are the one being added */
        if (((diffServPolicyInstObjectGet(policyIndex, searchPolicyInstIndex,
                                         L7_DIFFSERV_POLICY_INST_CLASS_INDEX, &searchClassIndex) != L7_SUCCESS)) ||
            (searchClassIndex == classIndex))
        {
          continue;
        }
        /* ignore classes that do not have their l3proto field set  */
        if (diffServClassObjectGet(searchClassIndex,
                                   L7_DIFFSERV_CLASS_L3_PROTO,
                                   &searchClassL3Proto) != L7_SUCCESS)
        {
          continue;
        }
        if (searchClassL3Proto != classL3Proto)
        {
          /* inconsistency found, invalidate adding class to policy */
          return L7_FAILURE;
        }
      } /* while there are more instances to get in policy */
    }

  }
  /* get policy type (ignore restrictions if cannot obtain it) */
  if (diffServPolicyObjectGet(policyIndex, L7_DIFFSERV_POLICY_TYPE,
                              &policyType) != L7_SUCCESS)
    return L7_SUCCESS;

  /* at present, only outbound policies have class rule restrictions */
  if (policyType != L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT)
    return L7_SUCCESS;

  if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                            L7_DIFFSERV_POLICY_OUT_CLASS_UNRESTRICTED_FEATURE_ID)
        == L7_TRUE)
    return L7_SUCCESS;

  /* check entire class reference chain for policy classification restrictions
   *  - only allow IP DSCP and Precedence
   */
  classRuleIndex = 0;                         /* start with first rule */
  while (dsmibClassRuleRefChainGetNext(classIndex, classRuleIndex, &classIndex,
                                       &classRuleIndex) == L7_SUCCESS)
  {
    /* ignore class rules that do not have their entry type set
     *
     * NOTE: class rule setTest also checks if the class is attached to a
     *       restricted policy to prevent setting unsupported rule entry types
     */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE,
                                   &ruleEntryType) != L7_SUCCESS)
      continue;

    /* allow refclass rule entry type (reference chaining handled via loop) */
    if (ruleEntryType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS)
      continue;

    if ((ruleEntryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP) &&
        (ruleEntryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE))
      return L7_FAILURE;

  } /* endwhile class rule ref chain list */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if a Policy Instance object value is valid to 'get'
*
* @param    pRow        @b{(input)} Policy Instance table row pointer
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
L7_RC_t dsmibPolicyInstObjectGetCheck(dsmibPolicyInstEntryCtrl_t *pRow,
                                      L7_DIFFSERV_POLICY_INST_TABLE_OBJECT_t oid)
{
  dsmibPolicyInstFlags_t objFlag;

  switch (oid)
  {
  case L7_DIFFSERV_POLICY_INST_CLASS_INDEX:
    objFlag = DSMIB_POLICY_INST_FLAGS_CLASSINDEX;
    break;

  case L7_DIFFSERV_POLICY_INST_STORAGE_TYPE:
  case L7_DIFFSERV_POLICY_INST_ROW_STATUS:
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

/*********************************************************************
* @purpose  Create the necessary rows in the Policy-Class (Inbound or Outbound)
*           Performance Table for the specified policy instance on all
*           service interfaces to which it is attached
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Policy Performance Table rows are created automatically when a
*           policy is attached to a service interface.  One row is created
*           for each policy instance in said policy.  This function covers
*           the case where an instance is added to an already-attached
*           service policy.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyInstPerfAttach(L7_uint32 policyIndex,
                                  L7_uint32 policyInstIndex)
{
  L7_uint32               intIfNum;
  dsmibServiceIfDir_t     ifDirection;
  dsmibServiceEntryCtrl_t *pService;
  L7_RC_t                 rc;

  /* walk through the Service Table, creating an entry in the appropriate
   * Policy Perf Table for each interface to which this policy is attached
   */
  intIfNum = 0;                                 /* start with first interface */
  ifDirection = 0;                              /* start with first direction */
  while (diffServServiceGetNext(intIfNum, ifDirection, &intIfNum, &ifDirection)
           == L7_SUCCESS)
  {
    /* get access to the service row */
    pService = dsmibServiceRowFind(intIfNum, ifDirection);
    if (pService == L7_NULLPTR)
      return L7_FAILURE;

    if (pService->mib.policyIndex != policyIndex)
      continue;

    if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
      rc = dsmibPolicyPerfInCreate(policyIndex, policyInstIndex, intIfNum,
                                   L7_TRUE);

    else
      rc = dsmibPolicyPerfOutCreate(policyIndex, policyInstIndex, intIfNum,
                                    L7_TRUE);

    if ((rc != L7_SUCCESS) && (rc != L7_ERROR))
      return L7_FAILURE;

  } /* endwhile */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete the corresponding rows in the Policy-Class (Inbound or
*           Outbound) Performance Table for the specified policy instance
*           on all service interfaces to which it is attached
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Policy Performance Table rows are deleted automatically when a
*           policy is detached from a service interface.  This function
*           covers the case where an instance is deleted from a still-attached
*           service policy.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyInstPerfDetach(L7_uint32 policyIndex,
                                  L7_uint32 policyInstIndex)
{
  L7_uint32               intIfNum;
  dsmibServiceIfDir_t     ifDirection;
  dsmibServiceEntryCtrl_t *pService;
  L7_RC_t                 rc;

  /* walk through the Service Table, creating an entry in the appropriate
   * Policy Perf Table for each interface to which this policy is attached
   */
  intIfNum = 0;                                 /* start with first interface */
  ifDirection = 0;                              /* start with first direction */
  while (diffServServiceGetNext(intIfNum, ifDirection, &intIfNum, &ifDirection)
           == L7_SUCCESS)
  {
    /* get access to the service row */
    pService = dsmibServiceRowFind(intIfNum, ifDirection);
    if (pService == L7_NULLPTR)
      return L7_FAILURE;

    if (pService->mib.policyIndex != policyIndex)
      continue;

    if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
      rc = dsmibPolicyPerfInDelete(policyIndex, policyInstIndex, intIfNum);

    else
      rc = dsmibPolicyPerfOutDelete(policyIndex, policyInstIndex, intIfNum);

    if ((rc != L7_SUCCESS) && (rc != L7_ERROR))
      return L7_FAILURE;

  } /* endwhile */

  return L7_SUCCESS;
}


/*
=====================================================
=====================================================
=====================================================

   POLICY-CLASS INSTANCE TABLE DEBUG SHOW FUNCTION

=====================================================
=====================================================
=====================================================
*/

/*********************************************************************
* @purpose  Display the current DiffServ Policy Instance Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsmibPolicyInstTableShow(void)
{
  L7_uint32                  msgLvlReqd;
  dsmibPolicyInstEntryCtrl_t *pRow;
  dsmibPolicyInstKey_t       keys;
  avlTree_t                  *pTree = &dsmibAvlTree[DSMIB_TABLE_ID_POLICY_INST];
  L7_uint32                  count;

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Policy Inst Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Policy Inst Table (contains %u entries):\n\n", count);

  DIFFSERV_PRT(msgLvlReqd, "Indexes ClassIdx Next ST RS TID  Flags   RP AttrCt\n");
  DIFFSERV_PRT(msgLvlReqd, "------- -------- ---- -- -- --- -------- -- ------\n");

  keys.policyIndex = 0;                          /* start with first entry */
  keys.policyInstIndex = 0;
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    DIFFSERV_PRT(msgLvlReqd, " %2u %2u   %6u  %3u  %2s %2s %2u  %8.8x %2s  %4u \n",
                 pRow->key.policyIndex,
                 pRow->key.policyInstIndex,
                 pRow->mib.classIndex,
                 pRow->mib.attrIndexNextFree,
                 dsmibStgTypeStr[pRow->mib.storageType],
                 dsmibRowStatusStr[pRow->mib.rowStatus],
                 pRow->tableId,
                 pRow->rowInvalidFlags,
                 dsmibRowPendingStr[pRow->rowPending],
                 pRow->attrCount);

    /* update search keys for next pass */
    keys = pRow->key;
  }
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

