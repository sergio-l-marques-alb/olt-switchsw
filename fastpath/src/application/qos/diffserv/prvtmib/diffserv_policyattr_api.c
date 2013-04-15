/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_policyattr_api.c
*
* @purpose    DiffServ component Private MIB Policy Attribute Table
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
#include "nimapi.h"
#include "l7_diffserv_include.h"
#include "usmdb_mib_diffserv_private_api.h"


extern avlTree_t  dsmibAvlTree[];
extern void *     dsmibIndexSemId;              /* MIB tables index semaphore */

L7_uchar8  policyAttrIpDscpValSupported[DSMIB_IPDSCP_TOTAL];

/*
=================================
=================================
=================================

   POLICY ATTRIBUTE TABLE APIs

=================================
=================================
=================================
*/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating
*           a new row in the Policy Attribute Table
*
* @param    policyIndex      @b{(input)}  Policy index
* @param    policyInstIndex  @b{(input)}  Policy instance index
* @param    pPolicyAttrIndex @b{(output)} Pointer to policy attribute index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pPolicyAttrIndex value
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
L7_RC_t diffServPolicyAttrIndexNext(L7_uint32 policyIndex,
                                    L7_uint32 policyInstIndex,
                                    L7_uint32 *pPolicyAttrIndex)
{
  L7_RC_t       rc = L7_SUCCESS;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (pPolicyAttrIndex == L7_NULLPTR)
    return L7_FAILURE;

  DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

  do                                            /* single-pass control loop */
  {
    /* always output a value of 0 if the MIB table size is at maximum */
    if (dsmibMibTableSizeCheck(DSMIB_TABLE_ID_POLICY_ATTR) != L7_SUCCESS)
    {
      /* table full -- L7_SUCCESS */
      *pPolicyAttrIndex = 0;
      break;
    }

    if (dsmibPolicyAttrIndexNextRead(policyIndex, policyInstIndex, pPolicyAttrIndex) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

  } while (0);

  DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

  if (rc != L7_SUCCESS)
    *pPolicyAttrIndex = 0;

  return rc;
}

/*************************************************************************
* @purpose  Retrieve the maximum policy attribute index value allowed for the
*           Policy Attribute Table
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
L7_RC_t diffServPolicyAttrIndexMaxGet(L7_uint32 *pIndexMax)
{
  /* check inputs */
  if (pIndexMax == L7_NULLPTR)
    return L7_FAILURE;

  *pIndexMax = DSMIB_POLICY_ATTR_INDEX_MAX;
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Create a new row in the Policy Attribute Table for the
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    activateRow     @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Index value in use, row not complete
* @returns  L7_ALREADY_CONFIGURED  Index value in use, row complete
* @returns  L7_FAILURE
*
* @notes    An L7_ERROR response means the policy attribute index value is
*           currently in use, but the row is not complete.  This is typically
*           caused when multiple requestors obtain the same "index next"
*           value and race to create the new row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_ALREADY_CONFIGURED response means the policy attribute index
*           value is currently in use for a completed row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    The row status is initially set to 'not ready' and remains that
*           way until every object in the row contains a valid value.  If the
*           activateRow parameter is set to L7_TRUE, the row status will then
*           be automatically updated to 'active'.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrCreate(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex,
                                 L7_BOOL activateRow)
{
  L7_RC_t                    rc = L7_FAILURE;
  L7_uint32                  dataChanged = L7_FALSE;
  dsmibTableId_t             tableId = DSMIB_TABLE_ID_POLICY_ATTR;
  dsmibPolicyAttrEntryCtrl_t row, *pRow = L7_NULLPTR;
  dsmibPolicyInstEntryCtrl_t *pPolicyInst;

  DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId,
                           policyIndex, policyInstIndex, policyAttrIndex,
                           (L7_uchar8)activateRow);

  do
  {
    /* check inputs */
    if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
      break;
    if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
      break;
    if (DSMIB_POLICY_ATTR_INDEX_OUT_OF_RANGE(policyAttrIndex))
      break;
    if ((activateRow != L7_FALSE) && (activateRow != L7_TRUE))
      break;

    /* verify index value matches expected value (i.e., current 'IndexNext')
     * for lowest-order creation index
     */
    if (dsmibPolicyAttrIndexNextVerify(policyIndex, policyInstIndex,
                                       policyAttrIndex) != L7_SUCCESS)
      break;

    /* make sure row does not already exist (someone else could have created
     * it first)
     */
    pRow = dsmibPolicyAttrRowFind(policyIndex, policyInstIndex, policyAttrIndex);
    if (pRow != L7_NULLPTR)           /* row exists */
    {
      rc = (DSMIB_ROW_IS_COMPLETE(pRow) == L7_TRUE) ? L7_ALREADY_CONFIGURED : L7_ERROR;
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
    pRow->key.policyAttrIndex = policyAttrIndex;

    /* initialize control fields */
    pRow->tableId = tableId;
    pRow->rowInvalidFlags = DSMIB_POLICY_ATTR_FLAGS_COMMON;  /* starting value */
    pRow->pPolicyInst = pPolicyInst;        /* back ptr to policy instance row */
    pRow->rowPending = L7_TRUE;

    /* fill in initial row status and any objects with known defaults */
    pRow->mib.index = policyAttrIndex;
    pRow->mib.entryType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
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

      /* keep track of the number of attributes within a policy instance */
      pPolicyInst->attrCount++;

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
          pPolicyInst->attrCount--;

          dataChanged = L7_FALSE;       /* config change undone */
        }
        else
        {
          L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                  "diffServPolicyAttrCreate: Error deleting row after create "
                  "failure for indexes %u,%u,%u\n", policyIndex, policyInstIndex,
                  policyAttrIndex);
        }
      }
    }
    else
    {
      /* problem with AVL insertion */
      rc = L7_FAILURE;
    }

    DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

  } while (0);

  /* make sure change gets picked up when config is saved */
  if (dataChanged == L7_TRUE)
    pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;

  DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId,
                          policyIndex, policyInstIndex, policyAttrIndex,
                          (L7_uint32)pRow, (L7_uchar8)rc,
                          (L7_uchar8)activateRow);

  return rc;
}

/*************************************************************************
* @purpose  Delete a row from the Policy Attribute Table for the
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrDelete(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex)
{
  /* delete the policy attribute, but allow distiller eval to occur */
  return dsmibPolicyAttrDelete(policyIndex, policyInstIndex, policyAttrIndex, L7_FALSE);
}

/*************************************************************************
* @purpose  Delete all rows from the Policy Attribute Table for the
*           specified policy instance
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrDeleteAll(L7_uint32 policyIndex,
                                    L7_uint32 policyInstIndex)
{
  L7_RC_t                rc = L7_SUCCESS;
  L7_uint32              policyAttrIndex;
  L7_uint32              nextPolicyIndex, nextPolicyInstIndex;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;

  /* delete all attributes belonging to this policy-class instance */
  policyAttrIndex = 0;               /* start with first attribute */
  while ((diffServPolicyAttrGetNext(policyIndex, policyInstIndex, policyAttrIndex,
              &nextPolicyIndex, &nextPolicyInstIndex, &policyAttrIndex) == L7_SUCCESS) &&
         (nextPolicyIndex == policyIndex) &&
         (nextPolicyInstIndex == policyInstIndex))
  {
    if (policyAttrIndex == 0)
      break;

    /* delete the policy attribute and set flag to bypass distiller eval (caller
     * will do it)
     *
     * not sending policy notify event callbacks from here either (caller decides)
     */
    if (dsmibPolicyAttrDelete(policyIndex, policyInstIndex, policyAttrIndex, L7_TRUE)
        != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
              "%s: could not delete attribute %u from policy instance %u,%u\n",
              __FUNCTION__, policyAttrIndex, policyIndex, policyInstIndex);
      rc = L7_FAILURE;
      break;
    }

  } /* endwhile */

  return rc;
}

/*************************************************************************
* @purpose  Delete a row from the Policy Attribute Table for the
*           specified indexes
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    bypassEval      @b{(input)} Flag for skipping distiller evaluation
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE
*
* @notes    Internal function used for normal and delete-all cases.
*           When bypassEval is L7_TRUE, this is part of a higher-level
*           deletion so the distiller evaluation can be skipped for
*           performance reasons.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyAttrDelete(L7_uint32 policyIndex,
                              L7_uint32 policyInstIndex,
                              L7_uint32 policyAttrIndex,
                              L7_BOOL   bypassEval)
{
  L7_RC_t                    rc = L7_FAILURE;
  dsmibTableId_t             tableId = DSMIB_TABLE_ID_POLICY_ATTR;
  L7_BOOL                    undoIndexNext = L7_FALSE;
  L7_BOOL                    needNotify = L7_FALSE;
  dsmibPolicyAttrEntryCtrl_t *pRow = L7_NULLPTR;
  dsmibPolicyInstEntryCtrl_t *pPolicyInst;
  dsmibRowStatus_t           prevRowStatus;
  L7_BOOL                    prevRowPending;
  L7_uint32                  colorConformIndex, colorExceedIndex;

  DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId,
                           policyIndex, policyInstIndex, policyAttrIndex);

  do
  {
    /* check inputs */
    if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
      break;
    if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
      break;
    if (DSMIB_POLICY_ATTR_INDEX_OUT_OF_RANGE(policyAttrIndex))
      break;
    if ((bypassEval != L7_FALSE) && (bypassEval != L7_TRUE))
      break;

    /* find node in AVL tree */
    pRow = dsmibPolicyAttrRowFind(policyIndex, policyInstIndex, policyAttrIndex);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_ERROR;
      break;
    }

    /* obtain the policy instance row pointer and save it for later */
    pPolicyInst = pRow->pPolicyInst;

    /* change an 'ACTIVE' row status to 'NOT_IN_SERVICE' prior to row deletion
     * (this transition needed for proper device cleanup)
     */
    prevRowStatus = pRow->mib.rowStatus;
    if (prevRowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      pRow->mib.rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;

      if (dsmibRowEventCheck(DSMIB_TABLE_ID_POLICY_ATTR, pRow,
                             prevRowStatus, pRow->mib.rowStatus) != L7_SUCCESS)
        break;
    }

    /* decrement the colorRefCount values for any policing conform and exceed
     * color aware class that might be in use
     */
    colorConformIndex = colorExceedIndex = 0;

    if (pRow->mib.entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
    {
      colorConformIndex = pRow->mib.stmt.policeSimple.colorConformIndex;
    }
    else if (pRow->mib.entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
    {
      colorConformIndex = pRow->mib.stmt.policeSingleRate.colorConformIndex;
      colorExceedIndex = pRow->mib.stmt.policeSingleRate.colorExceedIndex;
    }
    else if (pRow->mib.entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
    {
      colorConformIndex = pRow->mib.stmt.policeTwoRate.colorConformIndex;
      colorExceedIndex = pRow->mib.stmt.policeTwoRate.colorExceedIndex;
    }

    if (colorConformIndex != 0)
    {
      if (dsmibPolicyAttrColorClassRefCountUpdate(colorConformIndex, 0) != L7_SUCCESS)
        break;
    }
    if (colorExceedIndex != 0)
    {
      if (dsmibPolicyAttrColorClassRefCountUpdate(colorExceedIndex, 0) != L7_SUCCESS)
        break;
    }

    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* save row pending flag for change notification */
    prevRowPending = pRow->rowPending;

    /* before deleting the actual row, check if this is the most recent attribute
     * created for this instance (successful creations use sequential policy
     * attribute index values)
     *
     * if so, set a flag to undo the indexNext value if and only if the row
     * had ever reached completion status
     */
    if (diffServPolicyAttrIsMostRecent(policyIndex, policyInstIndex,
                                       policyAttrIndex) == L7_TRUE)
    {
      if (pRow->rowPending != L7_TRUE)
        undoIndexNext = L7_TRUE;
    }

    /* delete the policy attribute row AVL tree node */
    rc = dsmibRowDelete(&dsmibAvlTree[tableId], pRow);

    if (rc == L7_SUCCESS)
    {
      /* successful row deletion
       * NOTE: Do not use pRow from this point on.
       */
      pRow = L7_NULLPTR;

      /* if this had been the most recently created policy attribute row and it
       * reached completion status at least once, undo the indexNext value
       *
       * NOTE: This is being done primarily to handle internal failure cases
       *       during the normal establishment of a row.
       */
      if (undoIndexNext == L7_TRUE)
        dsmibPolicyAttrIndexNextUndo(policyIndex, policyInstIndex);

      /* update current table size */
      genStatus.tableSize[tableId]--;

      /* this attribute is no longer in the instance */
      pPolicyInst->attrCount--;

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
                "diffServPolicyAttrDelete: couldn't delete row AVL node for attribute "
                "%u, instance %u, in policy \'%s\'\n",
                policyAttrIndex, policyInstIndex, (char *)policyName);
      }
      else
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "diffServPolicyAttrDelete: couldn't delete row AVL node for "
                "indexes %u,%u,%u\n", policyIndex, policyInstIndex,
                policyAttrIndex);
      }
      rc = L7_FAILURE;
    }

    DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

  } while (0);

  if (needNotify == L7_TRUE)
  {
    /* send notify event callbacks for this policy
     *
     * NOTE: A policy attribute deletion is considered a 'modify' event for the policy
     */
    diffServChangeNotify(policyIndex, DIFFSERV_EVENT_MODIFY, L7_NULLPTR);
  }

  DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId,
                          policyIndex, policyInstIndex, policyAttrIndex,
                          (L7_uint32)pRow, (L7_uchar8)rc);

  return rc;
}

/*********************************************************************
* @purpose  Verify that a Policy Attribute Table row exists for the
*           specified index
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrGet(L7_uint32 policyIndex,
                              L7_uint32 policyInstIndex,
                              L7_uint32 policyAttrIndex)
{
  dsmibPolicyAttrEntryCtrl_t *pRow;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_ATTR_INDEX_OUT_OF_RANGE(policyAttrIndex))
    return L7_FAILURE;

  /* find node in AVL tree */
  pRow = dsmibPolicyAttrRowFind(policyIndex, policyInstIndex, policyAttrIndex);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_ERROR;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Policy Attribute Table
*
* @param    prevPolicyIndex     @b{(input)}  Policy index to begin search
* @param    prevPolicyInstIndex @b{(input)}  Policy instance index to begin
*                                              search
* @param    prevPolicyAttrIndex @b{(input)}  Policy attribute index to begin
*                                              search
* @param    pPolicyIndex        @b{(output)} Pointer to next sequential policy
*                                              index value
* @param    pPolicyInstIndex    @b{(output)} Pointer to next sequential policy
*                                              instance index value
* @param    pPolicyAttrIndex    @b{(output)} Pointer to next sequential policy
*                                              attribute index value
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
* @notes    A prevPolicyAttrIndex value of 0 is used to find the first policy
*           attribute relative to the specified prevPolicyIndex,
*           prevPolicyInstIndex.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrGetNext(L7_uint32 prevPolicyIndex,
                                  L7_uint32 prevPolicyInstIndex,
                                  L7_uint32 prevPolicyAttrIndex,
                                  L7_uint32 *pPolicyIndex,
                                  L7_uint32 *pPolicyInstIndex,
                                  L7_uint32 *pPolicyAttrIndex)
{
  dsmibPolicyAttrEntryCtrl_t *pRow;
  dsmibPolicyAttrKey_t       keys;

  /* check inputs */
  /* NOTE: Allow any starting index value to be used. */
  if (pPolicyIndex == L7_NULLPTR)
    return L7_FAILURE;
  if (pPolicyInstIndex == L7_NULLPTR)
    return L7_FAILURE;
  if (pPolicyAttrIndex == L7_NULLPTR)
    return L7_FAILURE;

  /* find node in AVL tree
   * NOTE: Need to use direct call here due to AVL_NEXT flag.
   */
  keys.policyIndex = prevPolicyIndex;
  keys.policyInstIndex = prevPolicyInstIndex;
  keys.policyAttrIndex = prevPolicyAttrIndex;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_POLICY_ATTR],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
    /* next row not found -- must have reached end of table */
    return L7_ERROR;

  /* found next sequential table row -- set caller's output variables */
  *pPolicyIndex = pRow->key.policyIndex;
  *pPolicyInstIndex = pRow->key.policyInstIndex;
  *pPolicyAttrIndex = pRow->key.policyAttrIndex;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Policy
*           Attribute Table to determine its legitimacy
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    oid             @b{(input)} Object identifier
* @param    pValue          @b{(input)} Pointer to proposed value to be set
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Proposed value invalid
* @returns  L7_FAILURE
*
* @notes    This is a 'wrapper' function used for tracing.  The real code
*           is contained in dsmibPolicyAttrSetTest().
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrSetTest(L7_uint32 policyIndex,
                                  L7_uint32 policyInstIndex,
                                  L7_uint32 policyAttrIndex,
                                  L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t oid,
                                  void *pValue)
{
  dsmibTableId_t              tableId = DSMIB_TABLE_ID_POLICY_ATTR;
  dsmibPolicyAttrEntryCtrl_t  *pRowTrace = L7_NULLPTR;
  L7_RC_t                     rc;
  size_t                      dsTraceValueLen = 0;
  L7_uchar8                   dsTraceValueData[DS_TRACE_OBJ_VAL_LEN_MAX];

  rc = dsmibPolicyAttrSetTest(policyIndex, policyInstIndex, policyAttrIndex,
                              oid, pValue, &pRowTrace, &dsTraceValueLen);

  if (rc != L7_SUCCESS)
  {
    /* format the OID value for the trace point */
    dsTraceObjectValFormat(pValue, dsTraceValueLen, dsTraceValueData);

    DS_TRACE_PT_OBJECT_SETTEST_FAIL(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId,
                                    policyIndex, policyInstIndex, policyAttrIndex,
                                    (L7_ulong32)pRowTrace, (L7_uchar8)rc,
                                    (L7_uchar8)oid, (L7_uchar8)dsTraceValueLen,
                                    dsTraceValueData);
  }

  return rc;
}

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Policy
*           Attribute Table to determine its legitimacy
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy instance index
* @param    policyAttrIndex @b{(input)}  Policy attribute index
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(input)}  Pointer to proposed value to be set
* @param    ppRow           @b{(output)} Pointer to row ptr output location
* @param    pValueLen       @b{(output)} Pointer to value length output location
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Proposed value invalid
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
* @notes    THE _BANDWIDTH AND _EXPEDITE OBJECTS MUST BE SET IN THE
*           FOLLOWING ORDER:  1) _CRATE_UNITS, 2) _CRATE.
*
* @notes    THE _POLICE_XXX ACTION OBJECTS MUST BE SET IN THE FOLLOWING
*           ORDER:  1) _YYY_ACT, 2) YYY_VAL.
*
* @notes    The ppRow and pValueLen parms are used for tracing.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyAttrSetTest(L7_uint32 policyIndex,
                               L7_uint32 policyInstIndex,
                               L7_uint32 policyAttrIndex,
                               L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t oid,
                               void *pValue,
                               dsmibPolicyAttrEntryCtrl_t **ppRow,
                               size_t *pValueLen)
{
  dsmibPolicyAttrEntryCtrl_t *pRow;
  dsmibPolicyAttrType_t      policyAttrType;
  dsmibPolicyInstEntryCtrl_t *pPolicyInst;
  L7_uint32                  val;
  dsmibPoliceAct_t           act;
  L7_uint32                  colorIndex;
  dsmibPoliceColor_t         colorMode;
  L7_uint32                  classIndex;
  dsmibClassL3Protocol_t     classL3Proto;
  nimConfigID_t              *pConfigId;
  L7_uint32                  intIfNum;
  L7_RC_t                    rc;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_ATTR_INDEX_OUT_OF_RANGE(policyAttrIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the policy attribute row */
  pRow = dsmibPolicyAttrRowFind(policyIndex, policyInstIndex, policyAttrIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  *ppRow = pRow;

  /* get access to the policy instance row */
  pPolicyInst = pRow->pPolicyInst;
  if (pPolicyInst == L7_NULLPTR)
    return L7_FAILURE;

  dsmibPolicyAttrConvertOidToType(oid, &policyAttrType);
  rc = dsmibPolicyAttrFeaturePresentCheck(policyAttrType);
  if (rc != L7_SUCCESS)
    return rc;

  /* compare object against its allowed values */
  switch (oid)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ENTRY_TYPE:
    {
      dsmibPolicyAttrType_t  entryType = *(dsmibPolicyAttrType_t *)pValue;
      *pValueLen = sizeof(entryType);

      if ((entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE) ||
          (entryType >= L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_TOTAL))
        return L7_ERROR;

      rc = dsmibPolicyAttrFeaturePresentCheck(entryType);
      if (rc != L7_SUCCESS)
        return rc;

      if (dsmibPolicyAttrTypeCheck(policyIndex, policyInstIndex,
                                   policyAttrIndex, entryType) != L7_SUCCESS)
        return L7_ERROR;

      /* once set, this value is not allowed to change */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE))
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ASSIGN_QUEUE_QID:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MIN) ||
          (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE)
        return L7_FAILURE;
    }
    break;

  /* NOTE:  There is no settable object for the 'drop' attribute */

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COSVAL:
    {
      /* NOTE: MARK COSVAL value is of type L7_uchar8. */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
          (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COS_AS_COS2:
    {
      /* NOTE: MARK COS AS COS2 value is of type L7_BOOL. */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val != L7_TRUE) && (val != L7_FALSE))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COS2VAL:
    {
      /* NOTE: MARK COS2VAL value is of type L7_uchar8. */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
          (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_IPDSCPVAL:
    {
      /* NOTE: MARK IPDSCPVAL value is of type L7_uchar8. */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
          (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
        return L7_ERROR;

      if (policyAttrIpDscpValSupported[val] != L7_TRUE)
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_IPPRECEDENCEVAL:
    {
      /* NOTE: MARK IPPRECEDENCEVAL value is of type L7_uchar8. */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
          (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL)
        return L7_FAILURE;

      /* marking IP Precedence is not valid for IPv6 classes; if Policy Instance is for an IPv6 class, report invalid */
      if (diffServPolicyInstObjectGet(policyIndex, policyInstIndex, L7_DIFFSERV_POLICY_INST_CLASS_INDEX, &classIndex) ==
          L7_SUCCESS)
      {
        if (diffServClassObjectGet(classIndex, L7_DIFFSERV_CLASS_L3_PROTO, &classL3Proto) == L7_SUCCESS)
        {
          if (L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6 == classL3Proto)
          {
            return L7_ERROR;
          }
        }
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MIRROR_INTF:
    {
      pConfigId  = (nimConfigID_t *)pValue;
      *pValueLen = sizeof(*pConfigId);

      /* only allow mirroring to supported interface types
       *
       * NOTE: Skip this checking in special config mode, since the interfaces
       *       have not been created yet during phase 3 config apply.
       */
      if (dsmibSpecialConfigModeGet() != L7_TRUE)
      {
        /* get intIfNum to do some validity checking */
        if (nimIntIfFromConfigIDGet(pConfigId, &intIfNum) != L7_SUCCESS)
          return L7_ERROR;

        if (diffServIsValidIntf(intIfNum) != L7_TRUE)
          return L7_ERROR;
      }

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CRATE:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if (val <= 0)
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CBURST:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MIN) ||
          (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CONFORM_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);

      if ((act == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE) ||
          (act >= L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CONFORM_VAL:
    {
      /* NOTE: Police simple conform value is of type L7_uchar8. */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
        return L7_FAILURE;

      /* for proper value range checking, the value object must be set AFTER
       * the action object
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT))
        return L7_FAILURE;

      /* restrict the value depending on which action is set */
      switch (pRow->mib.stmt.policeSimple.conformAct)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
        {
          if (val != L7_TRUE && val != L7_FALSE)
          {
            return L7_ERROR;
          }
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
            return L7_ERROR;

          if (policyAttrIpDscpValSupported[val] != L7_TRUE)
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
        if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
            (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
          return L7_ERROR;
        break;

      default:
        /* invalid usage */
        return L7_FAILURE;
        /*PASSTHRU*/
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_NONCONFORM_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);

      if ((act == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE) ||
          (act >= L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_NONCONFORM_VAL:
    {
      /* NOTE: Police simple nonconform value is of type L7_uchar8. */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
        return L7_FAILURE;

      /* for proper value range checking, the value object must be set AFTER
       * the action object
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT))
        return L7_FAILURE;

      /* restrict the value depending on which action is set */
      switch (pRow->mib.stmt.policeSimple.nonconformAct)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
        {
          if (val != L7_TRUE && val != L7_FALSE)
          {
            return L7_ERROR;
          }
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
            return L7_ERROR;

          if (policyAttrIpDscpValSupported[val] != L7_TRUE)
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
        if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
            (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
          return L7_ERROR;
        break;

      default:
        /* invalid usage */
        return L7_FAILURE;
        /*PASSTHRU*/
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_INDEX:
    {
      colorIndex = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(colorIndex);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
        return L7_FAILURE;

      /* make sure platform supports color-aware policing feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID)
          != L7_TRUE)
        return L7_FAILURE;

      /* validate this color aware class index */
      if (colorIndex != 0)
      {
        rc = diffServPolicyAttrColorAwareClassValidate(policyIndex, policyInstIndex,
                                                       colorIndex);
        if (rc != L7_SUCCESS)
          return rc;
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_MODE:
    {
      colorMode = *(dsmibPoliceColor_t *)pValue;
      *pValueLen = sizeof(colorMode);

      if ((colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_NONE) ||
          (colorMode >= L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_TOTAL))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
        return L7_FAILURE;

      /* make sure platform supports color-aware policing feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID)
          != L7_TRUE)
        return L7_FAILURE;

      /* check for conflicts between the color mode field and the classifier fields */
      rc = dsmibPolicyAttrColorModeConflictCheck(policyIndex, policyInstIndex,
                                                 colorMode);
      if (rc != L7_SUCCESS)
        return rc;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_VAL:
    {
      /* NOTE: Police simple color conform value is of type L7_uchar8 */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE)
        return L7_FAILURE;

      /* for proper value range checking, the value object must be set AFTER
       * the color mode object
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE))
        return L7_FAILURE;

      /* restrict the value depending on which action is set */
      switch (pRow->mib.stmt.policeSimple.colorConformMode)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
            return L7_ERROR;

          if (policyAttrIpDscpValSupported[val] != L7_TRUE)
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
        if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
            (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
          return L7_ERROR;
        break;

      default:
        /* invalid usage */
        return L7_FAILURE;
        /*PASSTHRU*/
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CRATE:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if (val == 0)
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CBURST:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MIN) ||
          (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;

      /* NOTE: Do not check value against eburst object value here.  If
       *       there is a conflict, the 'set' function will clear the eburst
       *       object, forcing it to be respecified.
       */
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EBURST:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_EBURST_MIN) ||
          (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_EBURST_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;

      /* NOTE: Do not check value against cburst object value here.  If
       *       there is a conflict, the 'set' function will clear the cburst
       *       object, forcing it to be respecified.
       */
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CONFORM_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);

      if ((act == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE) ||
          (act >= L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CONFORM_VAL:
    {
      /* NOTE: Police single rate conform value is of type L7_uchar8. */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;

      /* for proper value range checking, the value object must be set AFTER
       * the action object
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT))
        return L7_FAILURE;

      /* restrict the value depending on which action is set */
      switch (pRow->mib.stmt.policeSingleRate.conformAct)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
        {
          if (val != L7_TRUE && val != L7_FALSE)
          {
            return L7_ERROR;
          }
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
            return L7_ERROR;

          if (policyAttrIpDscpValSupported[val] != L7_TRUE)
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
        if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
            (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
          return L7_ERROR;
        break;

      default:
        /* invalid usage */
        return L7_FAILURE;
        /*PASSTHRU*/
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EXCEED_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);

      if ((act == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE) ||
          (act >= L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EXCEED_VAL:
    {
      /* NOTE: Police single rate exceed value is of type L7_uchar8. */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;

      /* for proper value range checking, the value object must be set AFTER
       * the action object
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT))
        return L7_FAILURE;

      /* restrict the value depending on which action is set */
      switch (pRow->mib.stmt.policeSingleRate.exceedAct)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
        {
          if (val != L7_TRUE && val != L7_FALSE)
          {
            return L7_ERROR;
          }
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
            return L7_ERROR;

          if (policyAttrIpDscpValSupported[val] != L7_TRUE)
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
        if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
            (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
          return L7_ERROR;
        break;

      default:
        /* invalid usage */
        return L7_FAILURE;
        /*PASSTHRU*/
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_NONCONFORM_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);

      if ((act == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE) ||
          (act >= L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_NONCONFORM_VAL:
    {
      /* NOTE: Police single rate nonconform value is of type L7_uchar8. */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;

      /* for proper value range checking, the value object must be set AFTER
       * the action object
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT))
        return L7_FAILURE;

      /* restrict the value depending on which action is set */
      switch (pRow->mib.stmt.policeSingleRate.nonconformAct)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
        {
          if (val != L7_TRUE && val != L7_FALSE)
          {
            return L7_ERROR;
          }
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
            return L7_ERROR;

          if (policyAttrIpDscpValSupported[val] != L7_TRUE)
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
        if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
            (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
          return L7_ERROR;
        break;

      default:
        /* invalid usage */
        return L7_FAILURE;
        /*PASSTHRU*/
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_INDEX:
    {
      colorIndex = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(colorIndex);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;

      /* make sure platform supports color-aware policing feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID)
          != L7_TRUE)
        return L7_FAILURE;

      /* validate this color aware class index */
      if (colorIndex != 0)
      {
        rc = diffServPolicyAttrColorAwareClassValidate(policyIndex, policyInstIndex,
                                                       colorIndex);
        if (rc != L7_SUCCESS)
          return rc;
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_MODE:
    {
      colorMode = *(dsmibPoliceColor_t *)pValue;
      *pValueLen = sizeof(colorMode);

      if ((colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_NONE) ||
          (colorMode >= L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_TOTAL))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;

      /* make sure platform supports color-aware policing feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID)
          != L7_TRUE)
        return L7_FAILURE;

      /* check for conflicts between the color mode field and the classifier fields */
      rc = dsmibPolicyAttrColorModeConflictCheck(policyIndex, policyInstIndex,
                                                 colorMode);
      if (rc != L7_SUCCESS)
        return rc;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_VAL:
    {
      /* NOTE: Police single rate color conform value is of type L7_uchar8 */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;

      /* for proper value range checking, the value object must be set AFTER
       * the color mode object
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE))
        return L7_FAILURE;

      /* restrict the value depending on which action is set */
      switch (pRow->mib.stmt.policeSingleRate.colorConformMode)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
            return L7_ERROR;

          if (policyAttrIpDscpValSupported[val] != L7_TRUE)
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
        if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
            (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
          return L7_ERROR;
        break;

      default:
        /* invalid usage */
        return L7_FAILURE;
        /*PASSTHRU*/
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_INDEX:
    {
      colorIndex = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(colorIndex);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;

      /* make sure platform supports color-aware policing feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID)
          != L7_TRUE)
        return L7_FAILURE;

      /* validate this color aware class index */
      if (colorIndex != 0)
      {
        rc = diffServPolicyAttrColorAwareClassValidate(policyIndex, policyInstIndex,
                                                       colorIndex);
        if (rc != L7_SUCCESS)
          return rc;
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_MODE:
    {
      colorMode = *(dsmibPoliceColor_t *)pValue;
      *pValueLen = sizeof(colorMode);

      if ((colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_NONE) ||
          (colorMode >= L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_TOTAL))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;

      /* make sure platform supports color-aware policing feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID)
          != L7_TRUE)
        return L7_FAILURE;

      /* check for conflicts between the color mode field and the classifier fields */
      rc = dsmibPolicyAttrColorModeConflictCheck(policyIndex, policyInstIndex,
                                                 colorMode);
      if (rc != L7_SUCCESS)
        return rc;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_VAL:
    {
      /* NOTE: Police single rate color exceed value is of type L7_uchar8 */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE)
        return L7_FAILURE;

      /* for proper value range checking, the value object must be set AFTER
       * the color mode object
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE))
        return L7_FAILURE;

      /* restrict the value depending on which action is set */
      switch (pRow->mib.stmt.policeSingleRate.colorExceedMode)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
            return L7_ERROR;

          if (policyAttrIpDscpValSupported[val] != L7_TRUE)
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
        if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
            (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
          return L7_ERROR;
        break;

      default:
        /* invalid usage */
        return L7_FAILURE;
        /*PASSTHRU*/
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CRATE:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if (val == 0)
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;


      /* NOTE: Do not check value against prate object value here.  If
       *       there is a conflict, the 'set' function will clear the prate
       *       object, forcing it to be respecified.
       */
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CBURST:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MIN) ||
          (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_CBURST_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_PRATE:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if (val == 0)
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;

      /* NOTE: Do not check value against crate object value here.  If
       *       there is a conflict, the 'set' function will clear the crate
       *       object, forcing it to be respecified.
       */
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_PBURST:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PBURST_MIN) ||
          (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_PBURST_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CONFORM_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);

      if ((act == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE) ||
          (act >= L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CONFORM_VAL:
    {
      /* NOTE: Police two rate conform value is of type L7_uchar8. */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;

      /* for proper value range checking, the value object must be set AFTER
       * the action object
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT))
        return L7_FAILURE;

      /* restrict the value depending on which action is set */
      switch (pRow->mib.stmt.policeTwoRate.conformAct)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
        {
          if (val != L7_TRUE && val != L7_FALSE)
          {
            return L7_ERROR;
          }
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
            return L7_ERROR;

          if (policyAttrIpDscpValSupported[val] != L7_TRUE)
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
        if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
            (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
          return L7_ERROR;
        break;

      default:
        /* invalid usage */
        return L7_FAILURE;
        /*PASSTHRU*/
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_EXCEED_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);

      if ((act == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE) ||
          (act >= L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_EXCEED_VAL:
    {
      /* NOTE: Police two rate exceed value is of type L7_uchar8. */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;

      /* for proper value range checking, the value object must be set AFTER
       * the action object
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT))
        return L7_FAILURE;

      /* restrict the value depending on which action is set */
      switch (pRow->mib.stmt.policeTwoRate.exceedAct)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
        {
          if (val != L7_TRUE && val != L7_FALSE)
          {
            return L7_ERROR;
          }
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
            return L7_ERROR;

          if (policyAttrIpDscpValSupported[val] != L7_TRUE)
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
        if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
            (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
          return L7_ERROR;
        break;

      default:
        /* invalid usage */
        return L7_FAILURE;
        /*PASSTHRU*/
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_NONCONFORM_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);

      if ((act == L7_USMDB_MIB_DIFFSERV_POLICE_ACT_NONE) ||
          (act >= L7_USMDB_MIB_DIFFSERV_POLICE_ACT_TOTAL))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_NONCONFORM_VAL:
    {
      /* NOTE: Police two rate nonconform value is of type L7_uchar8. */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;

      /* for proper value range checking, the value object must be set AFTER
       * the action object
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT))
        return L7_FAILURE;

      /* restrict the value depending on which action is set */
      switch (pRow->mib.stmt.policeTwoRate.nonconformAct)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
        {
          if (val != L7_TRUE && val != L7_FALSE)
          {
            return L7_ERROR;
          }
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
            return L7_ERROR;

          if (policyAttrIpDscpValSupported[val] != L7_TRUE)
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
        if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
            (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
          return L7_ERROR;
        break;

      default:
        /* invalid usage */
        return L7_FAILURE;
        /*PASSTHRU*/
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_INDEX:
    {
      colorIndex = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(colorIndex);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;

      /* make sure platform supports color-aware policing feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID)
          != L7_TRUE)
        return L7_FAILURE;

      /* validate this color aware class index */
      if (colorIndex != 0)
      {
        rc = diffServPolicyAttrColorAwareClassValidate(policyIndex, policyInstIndex,
                                                       colorIndex);
        if (rc != L7_SUCCESS)
          return rc;
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_MODE:
    {
      colorMode = *(dsmibPoliceColor_t *)pValue;
      *pValueLen = sizeof(colorMode);

      if ((colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_NONE) ||
          (colorMode >= L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_TOTAL))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;

      /* make sure platform supports color-aware policing feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID)
          != L7_TRUE)
        return L7_FAILURE;

      /* check for conflicts between the color mode field and the classifier fields */
      rc = dsmibPolicyAttrColorModeConflictCheck(policyIndex, policyInstIndex,
                                                 colorMode);
      if (rc != L7_SUCCESS)
        return rc;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_VAL:
    {
      /* NOTE: Police two rate color conform value is of type L7_uchar8 */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;

      /* for proper value range checking, the value object must be set AFTER
       * the color mode object
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE))
        return L7_FAILURE;

      /* restrict the value depending on which action is set */
      switch (pRow->mib.stmt.policeTwoRate.colorConformMode)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
            return L7_ERROR;

          if (policyAttrIpDscpValSupported[val] != L7_TRUE)
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
        if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
            (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
          return L7_ERROR;
        break;

      default:
        /* invalid usage */
        return L7_FAILURE;
        /*PASSTHRU*/
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_INDEX:
    {
      colorIndex = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(colorIndex);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;

      /* make sure platform supports color-aware policing feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID)
          != L7_TRUE)
        return L7_FAILURE;

      /* validate this color aware class index */
      if (colorIndex != 0)
      {
        rc = diffServPolicyAttrColorAwareClassValidate(policyIndex, policyInstIndex,
                                                       colorIndex);
        if (rc != L7_SUCCESS)
          return rc;
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_MODE:
    {
      colorMode = *(dsmibPoliceColor_t *)pValue;
      *pValueLen = sizeof(colorMode);

      if ((colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_NONE) ||
          (colorMode >= L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_TOTAL))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;

      /* make sure platform supports color-aware policing feature */
      if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID)
          != L7_TRUE)
        return L7_FAILURE;

      /* check for conflicts between the color mode field and the classifier fields */
      rc = dsmibPolicyAttrColorModeConflictCheck(policyIndex, policyInstIndex,
                                                 colorMode);
      if (rc != L7_SUCCESS)
        return rc;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_VAL:
    {
      /* NOTE: Police two rate color exceed value is of type L7_uchar8 */
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE)
        return L7_FAILURE;

      /* for proper value range checking, the value object must be set AFTER
       * the color mode object
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE))
        return L7_FAILURE;

      /* restrict the value depending on which action is set */
      switch (pRow->mib.stmt.policeTwoRate.colorExceedMode)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_COS2_MAX))
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
        {
          if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MIN) ||
              (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPDSCP_MAX))
            return L7_ERROR;

          if (policyAttrIpDscpValSupported[val] != L7_TRUE)
            return L7_ERROR;
        }
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
        if ((val < L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MIN) ||
            (val > L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_IPPRECEDENCE_MAX))
          return L7_ERROR;
        break;

      default:
        /* invalid usage */
        return L7_FAILURE;
        /*PASSTHRU*/
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_REDIRECT_INTF:
    {
      pConfigId = (nimConfigID_t *)pValue;
      *pValueLen = sizeof(*pConfigId);

      /* only allow redirection to supported interface types
       *
       * NOTE: Skip this checking in special config mode, since the interfaces
       *       have not been created yet during phase 3 config apply.
       */
      if (dsmibSpecialConfigModeGet() != L7_TRUE)
      {
        /* get intIfNum to do some validity checking */
        if (nimIntIfFromConfigIDGet(pConfigId, &intIfNum) != L7_SUCCESS)
          return L7_ERROR;

        if (diffServIsValidIntf(intIfNum) != L7_TRUE)
          return L7_ERROR;
      }

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT)
        return L7_FAILURE;
    }
    break;


  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_STORAGE_TYPE:
    {
      dsmibStorageType_t  storageType = *(dsmibStorageType_t *)pValue;
      *pValueLen = sizeof(storageType);

      if (dsmibStorageTypeIsValid(storageType) != L7_TRUE)
        return L7_ERROR;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ROW_STATUS:
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
          /* only the most recent attribute row in this instance may be destroyed */
          if (diffServPolicyAttrIsMostRecent(policyIndex, policyInstIndex,
                                             policyAttrIndex) != L7_TRUE)
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
  } /* endswitch */

  /* NOTE: Put any common checking here if value depends on row status state.*/

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the specified object from the Policy Attribute Table
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy instance index
* @param    policyAttrIndex @b{(input)}  Policy attribute index
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(output)} Pointer to the object output value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Object value not yet established
* @returns  L7_FAILURE  Improper usage or other malfunction
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrObjectGet(L7_uint32 policyIndex,
                                    L7_uint32 policyInstIndex,
                                    L7_uint32 policyAttrIndex,
                                    L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t oid,
                                    void *pValue)
{
  dsmibPolicyAttrEntryCtrl_t *pRow;
  L7_RC_t                    rc;
  L7_uint32                  *p;
  dsmibPoliceAct_t           *pAct;
  L7_uint32                  *pColorIndex;
  dsmibPoliceColor_t         *pColorMode;
  nimConfigID_t              *pConfigId;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_ATTR_INDEX_OUT_OF_RANGE(policyAttrIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibPolicyAttrRowFind(policyIndex, policyInstIndex, policyAttrIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  /* check for proper object type and row invalid flag state before proceeding*/
  if ((rc = dsmibPolicyAttrObjectGetCheck(pRow, oid)) != L7_SUCCESS)
    return rc;

  /* output the current value of the object */
  switch (oid)
  {
  case L7_DIFFSERV_POLICY_ATTR_STMT_ENTRY_TYPE:
    {
      dsmibPolicyAttrType_t  *pEntryType = pValue;
      *pEntryType = pRow->mib.entryType;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_ASSIGN_QUEUE_QID:
    {
      p = pValue;
      *p = pRow->mib.stmt.assignQueue.qid;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_DROP:
    {
      dsmibTruthValue_t *pDrop = pValue;
      *pDrop = pRow->mib.stmt.drop;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COSVAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.markCos.val;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COS2VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.markCos2.val;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COS_AS_COS2:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.markCosAsCos2.val;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_MARK_IPDSCPVAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.markIpDscp.val;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_MARK_IPPRECEDENCEVAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.markIpPrecedence.val;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_MIRROR_INTF:
    {
      pConfigId = pValue;
      NIM_CONFIG_ID_COPY(pConfigId, &pRow->mib.stmt.mirror.configId);
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CRATE:
    {
      p = pValue;
      *p = pRow->mib.stmt.policeSimple.crate;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CBURST:
    {
      p = pValue;
      *p = pRow->mib.stmt.policeSimple.cburst;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CONFORM_ACT:
    {
      pAct = pValue;
      *pAct = pRow->mib.stmt.policeSimple.conformAct;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CONFORM_VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.policeSimple.conformVal;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_NONCONFORM_ACT:
    {
      pAct = pValue;
      *pAct = pRow->mib.stmt.policeSimple.nonconformAct;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_NONCONFORM_VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.policeSimple.nonconformVal;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_COLOR_CONFORM_INDEX:
    {
      pColorIndex = pValue;
      *pColorIndex = pRow->mib.stmt.policeSimple.colorConformIndex;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_COLOR_CONFORM_MODE:
    {
      pColorMode = pValue;
      *pColorMode = pRow->mib.stmt.policeSimple.colorConformMode;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_COLOR_CONFORM_VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.policeSimple.colorConformVal;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CRATE:
    {
      p = pValue;
      *p = pRow->mib.stmt.policeSingleRate.crate;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CBURST:
    {
      p = pValue;
      *p = pRow->mib.stmt.policeSingleRate.cburst;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EBURST:
    {
      p = pValue;
      *p = pRow->mib.stmt.policeSingleRate.eburst;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CONFORM_ACT:
    {
      pAct = pValue;
      *pAct = pRow->mib.stmt.policeSingleRate.conformAct;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CONFORM_VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.policeSingleRate.conformVal;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EXCEED_ACT:
    {
      pAct = pValue;
      *pAct = pRow->mib.stmt.policeSingleRate.exceedAct;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EXCEED_VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.policeSingleRate.exceedVal;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_NONCONFORM_ACT:
    {
      pAct = pValue;
      *pAct = pRow->mib.stmt.policeSingleRate.nonconformAct;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_NONCONFORM_VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.policeSingleRate.nonconformVal;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_CONFORM_INDEX:
    {
      pColorIndex = pValue;
      *pColorIndex = pRow->mib.stmt.policeSingleRate.colorConformIndex;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_CONFORM_MODE:
    {
      pColorMode = pValue;
      *pColorMode = pRow->mib.stmt.policeSingleRate.colorConformMode;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_CONFORM_VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.policeSingleRate.colorConformVal;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_EXCEED_INDEX:
    {
      pColorIndex = pValue;
      *pColorIndex = pRow->mib.stmt.policeSingleRate.colorExceedIndex;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_EXCEED_MODE:
    {
      pColorMode = pValue;
      *pColorMode = pRow->mib.stmt.policeSingleRate.colorExceedMode;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_EXCEED_VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.policeSingleRate.colorExceedVal;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CRATE:
    {
      p = pValue;
      *p = pRow->mib.stmt.policeTwoRate.crate;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CBURST:
    {
      p = pValue;
      *p = pRow->mib.stmt.policeTwoRate.cburst;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_PRATE:
    {
      p = pValue;
      *p = pRow->mib.stmt.policeTwoRate.prate;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_PBURST:
    {
      p = pValue;
      *p = pRow->mib.stmt.policeTwoRate.pburst;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CONFORM_ACT:
    {
      pAct = pValue;
      *pAct = pRow->mib.stmt.policeTwoRate.conformAct;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CONFORM_VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.policeTwoRate.conformVal;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_EXCEED_ACT:
    {
      pAct = pValue;
      *pAct = pRow->mib.stmt.policeTwoRate.exceedAct;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_EXCEED_VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.policeTwoRate.exceedVal;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_NONCONFORM_ACT:
    {
      pAct = pValue;
      *pAct = pRow->mib.stmt.policeTwoRate.nonconformAct;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_NONCONFORM_VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.policeTwoRate.nonconformVal;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_CONFORM_INDEX:
    {
      pColorIndex = pValue;
      *pColorIndex = pRow->mib.stmt.policeTwoRate.colorConformIndex;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_CONFORM_MODE:
    {
      pColorMode = pValue;
      *pColorMode = pRow->mib.stmt.policeTwoRate.colorConformMode;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_CONFORM_VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.policeTwoRate.colorConformVal;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_EXCEED_INDEX:
    {
      pColorIndex = pValue;
      *pColorIndex = pRow->mib.stmt.policeTwoRate.colorExceedIndex;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_EXCEED_MODE:
    {
      pColorMode = pValue;
      *pColorMode = pRow->mib.stmt.policeTwoRate.colorExceedMode;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_EXCEED_VAL:
    {
      p = pValue;
      *p = (L7_uint32)pRow->mib.stmt.policeTwoRate.colorExceedVal;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_REDIRECT_INTF:
    {
      pConfigId = pValue;
      NIM_CONFIG_ID_COPY(pConfigId, &pRow->mib.stmt.redirect.configId);
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STORAGE_TYPE:
    {
      dsmibStorageType_t  *pStorageType = pValue;
      *pStorageType = pRow->mib.storageType;
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_ROW_STATUS:
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
  } /* endswitch */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set the specified object in the Policy Attribute Table
*
* @param    policyIndex      @b{(input)}  Policy index
* @param    policyInstIndex  @b{(input)}  Policy instance index
* @param    policyAttrIndex  @b{(input)}  Policy attribute index
* @param    oid              @b{(input)}  Object identifier
* @param    pValue           @b{(output)} Pointer to the object set value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Not allowed for current entry type
* @returns  L7_FAILURE
*
* @notes    This is a 'wrapper' function used for tracing.  The real code
*           is contained in dsmibPolicyAttrObjectSet().
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrObjectSet(L7_uint32 policyIndex,
                                    L7_uint32 policyInstIndex,
                                    L7_uint32 policyAttrIndex,
                                    L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t oid,
                                    void *pValue)
{
  dsmibTableId_t              tableId = DSMIB_TABLE_ID_POLICY_ATTR;
  dsmibPolicyAttrEntryCtrl_t  *pRowTrace = L7_NULLPTR;
  size_t                      dsTraceValueLen = 0;
  L7_uchar8                   dsTraceValueData[DS_TRACE_OBJ_VAL_LEN_MAX];
  L7_RC_t                     rc;

  rc = dsmibPolicyAttrObjectSet(policyIndex, policyInstIndex, policyAttrIndex,
                                oid, pValue, &pRowTrace, &dsTraceValueLen);

  /* format the OID value for the trace point */
  dsTraceObjectValFormat(pValue, dsTraceValueLen, dsTraceValueData);

  DS_TRACE_PT_OBJECT_SET(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId,
                         policyIndex, policyInstIndex, policyAttrIndex,
                         (L7_ulong32)pRowTrace, (L7_uchar8)rc,
                         (L7_uchar8)oid, (L7_uchar8)dsTraceValueLen,
                         dsTraceValueData);

  return rc;
}

/*********************************************************************
* @purpose  Set the specified object in the Policy Attribute Table
*
* @param    policyIndex      @b{(input)}  Policy index
* @param    policyInstIndex  @b{(input)}  Policy instance index
* @param    policyAttrIndex  @b{(input)}  Policy attribute index
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
* @notes    THE _BANDWIDTH AND _EXPEDITE OBJECTS MUST BE SET IN THE
*           FOLLOWING ORDER:  1) _CRATE_UNITS, 2) _CRATE.
*
* @notes    THE _POLICE_XXX ACTION OBJECTS MUST BE SET IN THE FOLLOWING
*           ORDER:  1) _XXX_ACT, 2) XXX_VAL.
*
* @notes    THE _POLICE_XXX COLOR OBJECTS MUST BE SET IN THE FOLLOWING
*           ORDER:  1) _XXX_MODE, 2) XXX_VAL.
*
* @notes    The ppRow and pValueLen are used for tracing.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyAttrObjectSet(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex,
                                 L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t oid,
                                 void *pValue,
                                 dsmibPolicyAttrEntryCtrl_t **ppRow,
                                 size_t *pValueLen)
{
  dsmibPolicyAttrEntryCtrl_t *pRow;
  dsmibRowStatus_t           restoreStatus, rowStatusRaw, oldStatus, newStatus;
  L7_uint32                  val;
  dsmibPoliceAct_t           act;
  L7_uint32                  colorIndex;
  dsmibPoliceColor_t         colorMode;
  nimConfigID_t              *pConfigId;
  L7_RC_t                    rc;
  L7_BOOL                    justCompleted;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_ATTR_INDEX_OUT_OF_RANGE(policyAttrIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* reuse 'SetTest' function to check for valid oid and settable value */
  if (diffServPolicyAttrSetTest(policyIndex, policyInstIndex, policyAttrIndex, oid, pValue)
        != L7_SUCCESS)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibPolicyAttrRowFind(policyIndex, policyInstIndex, policyAttrIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  *ppRow = pRow;

  /* save a copy of the old row status for a comparison below */
  restoreStatus = rowStatusRaw = pRow->mib.rowStatus;
  if (dsmibRowStatusObservable(rowStatusRaw, (L7_uint32)pRow->rowInvalidFlags,
                               &oldStatus) != L7_SUCCESS)
    return L7_FAILURE;

  if (oid != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ROW_STATUS)
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

      rc = dsmibRowEventCheck(DSMIB_TABLE_ID_POLICY_ATTR, pRow,
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
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ENTRY_TYPE:
    {
      dsmibPolicyAttrType_t  entryType = *(dsmibPolicyAttrType_t *)pValue;
      *pValueLen = sizeof(entryType);

      /* need to replace the set of row invalid flags here based on the
       * specific entry type (also causes entry type flag to be reset)
       */
      if ((rc = dsmibPolicyAttrRowFlagsReplace(pRow, entryType)) != L7_SUCCESS)
        return rc;

      pRow->mib.entryType = entryType;

      /* also need to set up the read-only MIB object field for the
       * 'drop' policy attribute type
       */
      if (entryType == L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP)
        pRow->mib.stmt.drop = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE;

      /* in this table, some of the row definitions contain objects that have
       * default values, but this cannot be determined until the entry type
       * is established -- set up any such defaults now
       */
      if ((rc = dsmibPolicyAttrTypeDefaultSet(pRow, entryType)) != L7_SUCCESS)
        return rc;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ASSIGN_QUEUE_QID:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.assignQueue.qid = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_QUEUEID);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COSVAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.markCos.val = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COSVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COS2VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.markCos2.val = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COS2VAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COS_AS_COS2:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.markCosAsCos2.val = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COS_AS_COS2 );
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_IPDSCPVAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.markIpDscp.val = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_IPDSCPVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_IPPRECEDENCEVAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.markIpPrecedence.val = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_IPPRECEDENCEVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MIRROR_INTF:
    {
      pConfigId = (nimConfigID_t *)pValue;
      *pValueLen = sizeof(*pConfigId);
      NIM_CONFIG_ID_COPY(&pRow->mib.stmt.mirror.configId, pConfigId);
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_MIRRORINTF);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CRATE:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      pRow->mib.stmt.policeSimple.crate = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CBURST:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeSimple.cburst = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CONFORM_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);
      pRow->mib.stmt.policeSimple.conformAct = act;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT);

      /* whenever the policing _ACT object is changed, force its
       * corresponding _VAL to be re-specified by setting its row invalid flag
       * (respecification is only required for _MARKxxxx actions)
       */
      pRow->mib.stmt.policeSimple.conformVal = 0;  /* clear prior value */

      if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_IS_MARKING(act) == L7_TRUE)
      {
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CONFORM_VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeSimple.conformVal = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_NONCONFORM_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);
      pRow->mib.stmt.policeSimple.nonconformAct = act;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT);

      /* whenever the policing _ACT object is changed, force its
       * corresponding _VAL to be re-specified by setting its row invalid flag
       * (respecification is only required for _MARKxxxx actions)
       */
      pRow->mib.stmt.policeSimple.nonconformVal = 0;  /* clear prior value */

      if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_IS_MARKING(act) == L7_TRUE)
      {
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_NONCONFORM_VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeSimple.nonconformVal = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_INDEX:
    {
      colorIndex = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(colorIndex);
      if (dsmibPolicyAttrColorClassRefCountUpdate(pRow->mib.stmt.policeSimple.colorConformIndex,
                                                  colorIndex) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      pRow->mib.stmt.policeSimple.colorConformIndex = colorIndex;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMINDEX);

      /* force the _MODE to be re-specified whenever the _INDEX is set */
      DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_MODE:
    {
      colorMode = *(dsmibPoliceColor_t *)pValue;
      *pValueLen = sizeof(colorMode);
      pRow->mib.stmt.policeSimple.colorConformMode = colorMode;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE);

      /* whenever the policing color _MODE object is changed, force its
       * corresponding _VAL to be re-specified by setting its row invalid flag
       * (respecification is only required for active color aware modes)
       */
      pRow->mib.stmt.policeSimple.colorConformVal = 0;  /* clear prior value */

      if ((colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC))
      {
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeSimple.colorConformVal = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CRATE:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeSingleRate.crate = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CBURST:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeSingleRate.cburst = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST);

      /* whenever the policing _CBURST value is changed to a value
       * that is higher than an existing _EBURST value, force the _EBURST
       * object to be re-specified by setting its row invalid flag
       */
      if ((DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST))
          && (val > pRow->mib.stmt.policeSingleRate.eburst))
      {
        pRow->mib.stmt.policeSingleRate.eburst = 0;   /* clear prior value */
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EBURST:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeSingleRate.eburst = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST);

      /* whenever the policing _EBURST value is changed to a value
       * that is lower than an existing _CBURST value, force the _CBURST
       * object to be re-specified by setting its row invalid flag
       */
      if ((DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST))
          && (val < pRow->mib.stmt.policeSingleRate.cburst))
      {
        pRow->mib.stmt.policeSingleRate.cburst = 0;   /* clear prior value */
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CONFORM_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);
      pRow->mib.stmt.policeSingleRate.conformAct = act;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT);

      /* whenever the policing _ACT object is changed, force its
       * corresponding _VAL to be re-specified by setting its row invalid flag
       * (respecification is only required for _MARKxxxx actions)
       */
      pRow->mib.stmt.policeSingleRate.conformVal = 0;  /* clear prior value */

      if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_IS_MARKING(act) == L7_TRUE)
      {
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CONFORM_VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeSingleRate.conformVal = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EXCEED_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);
      pRow->mib.stmt.policeSingleRate.exceedAct = act;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT);

      /* whenever the policing _ACT object is changed, force its
       * corresponding _VAL to be re-specified by setting its row invalid flag
       * (respecification is only required for _MARKxxxx actions)
       */
      pRow->mib.stmt.policeSingleRate.exceedVal = 0;  /* clear prior value */

      if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_IS_MARKING(act) == L7_TRUE)
      {
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EXCEED_VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeSingleRate.exceedVal = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_NONCONFORM_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);
      pRow->mib.stmt.policeSingleRate.nonconformAct = act;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT);

      /* whenever the policing _ACT object is changed, force its
       * corresponding _VAL to be re-specified by setting its row invalid flag
       * (respecification is only required for _MARKxxxx actions)
       */
      pRow->mib.stmt.policeSingleRate.nonconformVal = 0;  /* clear prior value */

      if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_IS_MARKING(act) == L7_TRUE)
      {
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_NONCONFORM_VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeSingleRate.nonconformVal = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_INDEX:
    {
      colorIndex = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(colorIndex);
      if (dsmibPolicyAttrColorClassRefCountUpdate(pRow->mib.stmt.policeSingleRate.colorConformIndex,
                                                  colorIndex) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      pRow->mib.stmt.policeSingleRate.colorConformIndex = colorIndex;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMINDEX);

      /* force the _MODE to be re-specified whenever the _INDEX is set */
      DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_MODE:
    {
      colorMode = *(dsmibPoliceColor_t *)pValue;
      *pValueLen = sizeof(colorMode);
      pRow->mib.stmt.policeSingleRate.colorConformMode = colorMode;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE);

      /* whenever the policing color _MODE object is changed, force its
       * corresponding _VAL to be re-specified by setting its row invalid flag
       * (respecification is only required for active color aware modes)
       */
      pRow->mib.stmt.policeSingleRate.colorConformVal = 0;  /* clear prior value */

      if ((colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC))
      {
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeSingleRate.colorConformVal = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_INDEX:
    {
      colorIndex = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(colorIndex);
      if (dsmibPolicyAttrColorClassRefCountUpdate(pRow->mib.stmt.policeSingleRate.colorExceedIndex,
                                                  colorIndex) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      pRow->mib.stmt.policeSingleRate.colorExceedIndex = colorIndex;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDINDEX);

      /* force the _MODE to be re-specified whenever the _INDEX is set */
      DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_MODE:
    {
      colorMode = *(dsmibPoliceColor_t *)pValue;
      *pValueLen = sizeof(colorMode);
      pRow->mib.stmt.policeSingleRate.colorExceedMode = colorMode;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE);

      /* whenever the policing color _MODE object is changed, force its
       * corresponding _VAL to be re-specified by setting its row invalid flag
       * (respecification is only required for active color aware modes)
       */
      pRow->mib.stmt.policeSingleRate.colorExceedVal = 0;  /* clear prior value */

      if ((colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC))
      {
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDVAL);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeSingleRate.colorExceedVal = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CRATE:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeTwoRate.crate = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE);

      /* whenever the policing _CRATE value is changed to a value
       * that is higher than an existing _PRATE value, force the _PRATE
       * object to be re-specified by setting its row invalid flag
       */
      if ((DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE))
          && (val > pRow->mib.stmt.policeTwoRate.prate))
      {
        pRow->mib.stmt.policeTwoRate.prate = 0;       /* clear prior value */
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CBURST:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeTwoRate.cburst = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_PRATE:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeTwoRate.prate = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE);

      /* whenever the policing _PRATE value is changed to a value
       * that is lower than an existing _CRATE value, force the _CRATE
       * object to be re-specified by setting its row invalid flag
       */
      if ((DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE))
          && (val < pRow->mib.stmt.policeTwoRate.crate))
      {
        pRow->mib.stmt.policeTwoRate.crate = 0;       /* clear prior value */
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_PBURST:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeTwoRate.pburst = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_PBURST);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CONFORM_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);
      pRow->mib.stmt.policeTwoRate.conformAct = act;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT);

      /* whenever the policing _ACT object is changed, force its
       * corresponding _VAL to be re-specified by setting its row invalid flag
       * (respecification is only required for _MARKxxxx actions)
       */
      pRow->mib.stmt.policeTwoRate.conformVal = 0;  /* clear prior value */

      if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_IS_MARKING(act) == L7_TRUE)
      {
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CONFORM_VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeTwoRate.conformVal = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_EXCEED_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);
      pRow->mib.stmt.policeTwoRate.exceedAct = act;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT);

      /* whenever the policing _ACT object is changed, force its
       * corresponding _VAL to be re-specified by setting its row invalid flag
       * (respecification is only required for _MARKxxxx actions)
       */
      pRow->mib.stmt.policeTwoRate.exceedVal = 0;  /* clear prior value */

      if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_IS_MARKING(act) == L7_TRUE)
      {
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_EXCEED_VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeTwoRate.exceedVal = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_NONCONFORM_ACT:
    {
      act = *(dsmibPoliceAct_t *)pValue;
      *pValueLen = sizeof(act);
      pRow->mib.stmt.policeTwoRate.nonconformAct = act;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT);

      /* whenever the policing _ACT object is changed, force its
       * corresponding _VAL to be re-specified by setting its row invalid flag
       * (respecification is only required for _MARKxxxx actions)
       */
      pRow->mib.stmt.policeTwoRate.nonconformVal = 0;  /* clear prior value */

      if (L7_USMDB_MIB_DIFFSERV_POLICE_ACT_IS_MARKING(act) == L7_TRUE)
      {
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_NONCONFORM_VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeTwoRate.nonconformVal = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_INDEX:
    {
      colorIndex = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(colorIndex);
      if (dsmibPolicyAttrColorClassRefCountUpdate(pRow->mib.stmt.policeTwoRate.colorConformIndex,
                                                  colorIndex) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      pRow->mib.stmt.policeTwoRate.colorConformIndex = colorIndex;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMINDEX);

      /* force the _MODE to be re-specified whenever the _INDEX is set */
      DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_MODE:
    {
      colorMode = *(dsmibPoliceColor_t *)pValue;
      *pValueLen = sizeof(colorMode);
      pRow->mib.stmt.policeTwoRate.colorConformMode = colorMode;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE);

      /* whenever the policing color _MODE object is changed, force its
       * corresponding _VAL to be re-specified by setting its row invalid flag
       * (respecification is only required for active color aware modes)
       */
      pRow->mib.stmt.policeTwoRate.colorConformVal = 0;  /* clear prior value */

      if ((colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC))
      {
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeTwoRate.colorConformVal = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_INDEX:
    {
      colorIndex = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(colorIndex);
      if (dsmibPolicyAttrColorClassRefCountUpdate(pRow->mib.stmt.policeTwoRate.colorExceedIndex,
                                                  colorIndex) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      pRow->mib.stmt.policeTwoRate.colorExceedIndex = colorIndex;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDINDEX);

      /* force the _MODE to be re-specified whenever the _INDEX is set */
      DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_MODE:
    {
      colorMode = *(dsmibPoliceColor_t *)pValue;
      *pValueLen = sizeof(colorMode);
      pRow->mib.stmt.policeTwoRate.colorExceedMode = colorMode;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE);

      /* whenever the policing color _MODE object is changed, force its
       * corresponding _VAL to be re-specified by setting its row invalid flag
       * (respecification is only required for active color aware modes)
       */
      pRow->mib.stmt.policeTwoRate.colorExceedVal = 0;  /* clear prior value */

      if ((colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP) ||
          (colorMode == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC))
      {
        DSMIB_ROW_INVALID_FLAG_SET(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDVAL);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_VAL:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.stmt.policeTwoRate.colorExceedVal = (L7_uchar8)val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDVAL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_REDIRECT_INTF:
    {
      pConfigId = (nimConfigID_t *)pValue;
      *pValueLen = sizeof(*pConfigId);
      NIM_CONFIG_ID_COPY(&pRow->mib.stmt.redirect.configId, pConfigId);
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_ATTR_FLAGS_STMT_REDIRECTINTF);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_STORAGE_TYPE:
    {
      dsmibStorageType_t storageType = *(dsmibStorageType_t *)pValue;
      *pValueLen = sizeof(storageType);
      pRow->mib.storageType = storageType;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ROW_STATUS:
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
  if (oid != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ROW_STATUS)
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
      dsmibPolicyAttrIndexNextUpdate(policyIndex, policyInstIndex);

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
    if (dsmibRowEventCheck(DSMIB_TABLE_ID_POLICY_ATTR, pRow, oldStatus, newStatus)
          != L7_SUCCESS)
      return L7_FAILURE;

    if (pRow->mib.rowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY)
    {
      /* for a new row status of 'destroy', it is now time to delete the row */
      if (diffServPolicyAttrDelete(policyIndex, policyInstIndex,
                                   policyAttrIndex) != L7_SUCCESS)
      {
        pRow->mib.rowStatus = restoreStatus;
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "diffServPolicyAttrObjectSet: attempt to destroy the row failed\n");
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
* @purpose  Get the policing color aware mode and value from the specified
*           class index
*
* @param    classIndex      @b{(input)}  Class index
* @param    pColorMode      @b{(output)} Pointer to color mode output location
* @param    pColorValue     @b{(output)} Pointer to color value output location
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR      Invalid class definition for color-aware use
* @returns  L7_FAILURE    Improper usage or other malfunction
*
* @notes    The class definition used for policing color awareness is only
*           allowed to contain a single, non-excluded class match condition
*           identifying one of the supported comparison fields: COS,
*           IP DSCP, IP Precedence, or Secondary COS.  Anything else results
*           in an L7_ERROR return code.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrColorAwareClassInfoGet(L7_uint32 classIndex,
                                                 L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t *pColorMode,
                                                 L7_uint32 *pColorValue)
{
  dsmibClassEntryCtrl_t                 *pClassRow;
  L7_uint32                             classRuleIndex, nextClassIndex;
  dsmibClassRuleType_t                  classRuleType;
  dsmibPoliceColor_t                    colorMode;
  L7_DIFFSERV_CLASS_RULE_TABLE_OBJECT_t ruleObject;
  L7_uint32                             classRuleValue;
  dsmibTruthValue_t                     excludeFlag;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;
  if (pColorMode == L7_NULLPTR)
    return L7_FAILURE;
  if (pColorValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the class row */
  pClassRow = dsmibClassRowFind(classIndex);
  if (pClassRow == L7_NULLPTR)
    return L7_FAILURE;

  /* this class must contain only one class rule */
  if ((pClassRow->ruleCount != 1) && (pClassRow->refCount != 0))
    return L7_ERROR;

  classRuleIndex = 0;
  if ((diffServClassRuleGetNext(classIndex, classRuleIndex, &nextClassIndex,
                                &classRuleIndex) != L7_SUCCESS) ||
      (nextClassIndex != classIndex))
    return L7_FAILURE;

  /* inspect the class rule for the right entry type */
  if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                 L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE,
                                 &classRuleType) != L7_SUCCESS)
    return L7_FAILURE;

  switch (classRuleType)
  {
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
    colorMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS;
    ruleObject = L7_DIFFSERV_CLASS_RULE_MATCH_COS;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
    colorMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2;
    ruleObject = L7_DIFFSERV_CLASS_RULE_MATCH_COS2;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
    colorMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP;
    ruleObject = L7_DIFFSERV_CLASS_RULE_MATCH_IPDSCP;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
    colorMode = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC;
    ruleObject = L7_DIFFSERV_CLASS_RULE_MATCH_IPPRECEDENCE;
    break;

  default:
    return L7_ERROR;
    /*PASSTHRU*/
  }

  /* make sure it is a non-excluded class match condition */
  if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                 L7_DIFFSERV_CLASS_RULE_MATCH_EXCLUDE_FLAG,
                                 &excludeFlag) != L7_SUCCESS)
    return L7_FAILURE;

  if (excludeFlag != L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE)
    return L7_ERROR;

  /* get the value of the rule match field being compared */
  if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                 ruleObject, &classRuleValue) != L7_SUCCESS)
    return L7_FAILURE;

  /* output the results */
  *pColorMode = colorMode;
  *pColorValue = classRuleValue;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine if a DiffServ class is valid for use as a policing
*           color aware designation
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy instance index
* @param    classIndex      @b{(input)}  Class index value to be set
*
* @returns  L7_SUCCESS    Class is valid for color-aware use
* @returns  L7_ERROR      Invalid class definition for color-aware use
* @returns  L7_FAILURE    Improper usage or other malfunction
*
* @notes    The class definition used for policing color awareness is only
*           allowed to contain a single, non-excluded class match condition
*           identifying one of the supported comparison fields: COS,
*           IP DSCP, IP Precedence, or Secondary COS.  Also, the field
*           used in this class must not conflict with the classifier
*           match conditions for this policy-class instance.  Anything else
*           results in an L7_ERROR return code.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyAttrColorAwareClassValidate(L7_uint32 policyIndex,
                                                  L7_uint32 policyInstIndex,
                                                  L7_uint32 classIndex)
{
  L7_RC_t                               rc;
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t  colorMode;
  L7_uint32                             colorValue;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;

  /* get the color aware mode from the specified class index, which
   * will check the requirements of this class for use as a color-aware
   * specifier
   */
  rc = diffServPolicyAttrColorAwareClassInfoGet(classIndex, &colorMode,
                                                &colorValue);
  if (rc != L7_SUCCESS)
    return rc;

  /* verify that the color mode indicated by the specified class does
   * not conflict with the classifier match conditions in this policy-class
   * instance
   */
  rc = dsmibPolicyAttrColorModeConflictCheck(policyIndex, policyInstIndex,
                                             colorMode);
  if (rc != L7_SUCCESS)
    return rc;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine if color aware policing definitions are equivalent
*           for two DiffServ classes
*
* @param    classIndex1   @b{(input)}  First class index to compare
* @param    classIndex2   @b{(input)}  Second class index to compare
*
* @returns  L7_TRUE       Classes are color-aware with equivalent mode, val
* @returns  L7_FALSE      Classes not both color-aware, or not equivalent
*
* @notes    The intent of this API is to compare two color-aware class
*           definitions to determine if they match on the same color
*           field (mode) and value.  This is typically used when comparing
*           the conform and exceed color class contents.
*
* @end
*********************************************************************/
L7_BOOL diffServPolicyAttrColorAwareClassesAreEquivalent(L7_uint32 classIndex1,
                                                         L7_uint32 classIndex2)
{
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t  mode1, mode2;
  L7_uint32                             val1, val2;

  /* at least one class is color-blind */
  if ((classIndex1 == 0) || (classIndex2 == 0))
    return L7_FALSE;

  /* both are same color-aware class */
  if (classIndex1 == classIndex2)
    return L7_TRUE;

  if ((diffServPolicyAttrColorAwareClassInfoGet(classIndex1, &mode1, &val1) != L7_SUCCESS) ||
      (diffServPolicyAttrColorAwareClassInfoGet(classIndex2, &mode2, &val2) != L7_SUCCESS))
    return L7_FALSE;

  if ((L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_MODE_IS_AWARE(mode1) == L7_FALSE) ||
      (L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_MODE_IS_AWARE(mode2) == L7_FALSE))
    return L7_FALSE;

  /* when comparing a DSCP value against a Precedence value, only
   * the high-order three bits of the DSCP should be compared
   * to determine if there is an equivalence match, so convert the
   * DSCP value to its corresponding Precedence value
   */
  if ((mode1 == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP) &&
      (mode2 == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC))
  {
    mode1 = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC;
    val1 >>= DSMIB_CLASS_RULE_IP_DSCP_PREC_CONVERT_SHIFT;
  }
  else if ((mode1 == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC) &&
           (mode2 == L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP))
  {
    mode2 = L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC;
    val2 >>= DSMIB_CLASS_RULE_IP_DSCP_PREC_CONVERT_SHIFT;
  }

  /* both color-aware classes have same mode and value */
  if ((mode1 == mode2) && (val1 == val2))
    return L7_TRUE;

  /* both classes are color-aware, but different mode or value */
  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check if the mirror/redirect target interface can be used
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex @b{(input)}  Policy instance index
* @param    policyAttrIndex @b{(input)}  Policy attribute index
* @param    oid             @b{(input)}  Object identifier
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    By "usable" here, we mean that the interface is a supported
*           DiffServ interface that is in a state of either L7_INTF_ATTACHING
*           or L7_INTF_ATTACHED.
*
* @notes    The only object IDs supported are for the mirror and redirect
*           interface attributes.
*
* @end
*********************************************************************/
L7_BOOL diffServPolicyAttrTargetIntfIsUsable(L7_uint32 policyIndex,
                                             L7_uint32 policyInstIndex,
                                             L7_uint32 policyAttrIndex,
                                             L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t oid)
{
  dsmibPolicyAttrEntryCtrl_t *pRow;
  nimConfigID_t              configId;
  L7_uint32                  intIfNum;
  L7_INTF_STATES_t           intfState;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FALSE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FALSE;
  if (DSMIB_POLICY_ATTR_INDEX_OUT_OF_RANGE(policyAttrIndex))
    return L7_FALSE;

  /* get access to the row */
  pRow = dsmibPolicyAttrRowFind(policyIndex, policyInstIndex, policyAttrIndex);
  if (pRow == L7_NULLPTR)
    return L7_FALSE;

  /* check for proper object type and row invalid flag state before proceeding*/
  if (dsmibPolicyAttrObjectGetCheck(pRow, oid) != L7_SUCCESS)
    return L7_FALSE;

  /* get the target config ID */
  switch (oid)
  {
  case L7_DIFFSERV_POLICY_ATTR_STMT_MIRROR_INTF:
    {
      NIM_CONFIG_ID_COPY(&configId, &pRow->mib.stmt.mirror.configId);
    }
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_REDIRECT_INTF:
    {
      NIM_CONFIG_ID_COPY(&configId, &pRow->mib.stmt.redirect.configId);
    }
    break;

  default:
    /* invalid oid */
    return L7_FALSE;
    /*PASSTHRU*/

  } /* endswitch */

  if (nimIntIfFromConfigIDGet(&configId, &intIfNum) != L7_SUCCESS)
    return L7_FALSE;

  if (diffServIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FALSE;

  if ((diffServIsIntfAttached(intIfNum, &intfState) != L7_TRUE) ||
      ((intfState != L7_INTF_ATTACHING) && (intfState != L7_INTF_ATTACHED)))
    return L7_FALSE;

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Determine if the policy attribute is the most recently created
*           one in this policy instance (i.e., is last one in current list)
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
*
* @returns  L7_TRUE     Policy instance is most recent
* @returns  L7_FALSE    Policy instance not most recent
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL diffServPolicyAttrIsMostRecent(L7_uint32 policyIndex,
                                       L7_uint32 policyInstIndex,
                                       L7_uint32 policyAttrIndex)
{
  L7_RC_t       rc;
  L7_uint32     nextPolicy, nextInst, nextAttr;

  /* NOTE: Assumes a 'GetNext' L7_ERROR means the end of table was reached,
   *       which implies this is the last policy attribute in the table and
   *       therefore must also be the most recent attribute in its policy
   *       instance.
   */
  rc = diffServPolicyAttrGetNext(policyIndex, policyInstIndex, policyAttrIndex,
                                 &nextPolicy, &nextInst, &nextAttr);

  if ((rc == L7_SUCCESS) &&
      ((nextPolicy != policyIndex) || (nextInst != policyInstIndex)))
    return L7_TRUE;

  if (rc == L7_ERROR)
    return L7_TRUE;

  return L7_FALSE;
}

/*
=============================================
=============================================
=============================================

   POLICY ATTRIBUTE TABLE HELPER FUNCTIONS

=============================================
=============================================
=============================================
*/

/*********************************************************************
* @purpose  Provide the current value of the policy attribute index
*           next variable
*
* @param    policyIndex      @b{(input)}  Policy index
* @param    policyInstIndex  @b{(input)}  Policy instance index
* @param    pNextFree        @b{(output)} Pointer to next free index
*                                         value
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
L7_RC_t dsmibPolicyAttrIndexNextRead(L7_uint32 policyIndex,
                                     L7_uint32 policyInstIndex,
                                     L7_uint32 *pNextFree)
{
  dsmibPolicyInstEntryCtrl_t *pRow;

  /* get access to the policy instance row */
  pRow = dsmibPolicyInstRowFind(policyIndex, policyInstIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  *pNextFree = pRow->mib.attrIndexNextFree;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine next available policy attribute index value
*           and update its value
*
* @param    policyIndex      @b{(input)}  Policy index
* @param    policyInstIndex  @b{(input)}  Policy instance index
*
* @returns  void
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*
* @end
*********************************************************************/
void dsmibPolicyAttrIndexNextUpdate(L7_uint32 policyIndex,
                                    L7_uint32 policyInstIndex)
{
  dsmibPolicyInstEntryCtrl_t *pRow;
  L7_uint32                  indexNext, i;

  indexNext = DSMIB_AUTO_INCR_INDEX_NEXT_START;
  for (i = 1; i <= DSMIB_POLICY_ATTR_INDEX_MAX; i++)
  {
    if (dsmibPolicyAttrRowFind(policyIndex, policyInstIndex, i) != L7_NULLPTR)
      indexNext = i + 1;
  }

  if (indexNext > DSMIB_POLICY_ATTR_INDEX_MAX)
    indexNext = 0;

  /* get access to the policy instance row */
  pRow = dsmibPolicyInstRowFind(policyIndex, policyInstIndex);
  if (pRow == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "dsmibPolicyAttrIndexNextUpdate: ERROR -- could not find "
            "policy instance row for indexes %u,%u\n", policyIndex,
            policyInstIndex);
    return;                                     /* ERROR -- should not occur */
  }

  pRow->mib.attrIndexNextFree = indexNext;
}

/*********************************************************************
* @purpose  Undo next available policy attribute index value for a deleted row
*
* @param    policyIndex     @b{(input)}  Policy index
* @param    policyInstIndex  @b{(input)}  Policy instance index
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
void dsmibPolicyAttrIndexNextUndo(L7_uint32 policyIndex,
                                  L7_uint32 policyInstIndex)
{
  static const char *routine_name = "dsmibPolicyAttrIndexNextUndo()";
  dsmibPolicyInstEntryCtrl_t  *pRow;
  L7_uint32                   indexNext;

  /* get access to the policy instance row */
  pRow = dsmibPolicyInstRowFind(policyIndex, policyInstIndex);
  if (pRow == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: ERROR -- could not find policy instance row for indexes %u,%u\n",
            routine_name, policyIndex, policyInstIndex);
    return;                                     /* ERROR -- should not occur */
  }

  indexNext = pRow->mib.attrIndexNextFree;

  if (indexNext == 0)
    indexNext = DSMIB_POLICY_ATTR_INDEX_MAX;
  else if (indexNext > DSMIB_AUTO_INCR_INDEX_NEXT_START)
    indexNext--;
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: WARNING -- attempting to decrement an indexNext value "
            "of %u for policy instance %u,%u\n", routine_name, indexNext,
            policyIndex, policyInstIndex);
    return;
  }

  pRow->mib.attrIndexNextFree = indexNext;
}

/*********************************************************************
* @purpose  Verifies the specified policy attribute index value
*           matches its current 'IndexNext' value
*
* @param    policyIndex      @b{(input)} Policy index
* @param    policyInstIndex  @b{(input)} Policy instance index
* @param    policyAttrIndex  @b{(input)} Policy attribute index to be
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
L7_RC_t dsmibPolicyAttrIndexNextVerify(L7_uint32 policyIndex,
                                       L7_uint32 policyInstIndex,
                                       L7_uint32 policyAttrIndex)
{
  L7_uint32     currentIndex;

  if (policyAttrIndex == 0)
    return L7_FAILURE;

  /* skip this checking if currently in 'special config mode' */
  if (dsmibSpecialConfigModeGet() == L7_TRUE)
    return L7_SUCCESS;

  if (dsmibPolicyAttrIndexNextRead(policyIndex, policyInstIndex,
                                                &currentIndex) != L7_SUCCESS)
    return L7_FAILURE;

  return (policyAttrIndex == currentIndex) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Policy Attribute Table row element
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
*
* @returns  pRow        Pointer to policy attribute row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*
* @end
*********************************************************************/
dsmibPolicyAttrEntryCtrl_t *dsmibPolicyAttrRowFind(L7_uint32 policyIndex,
                                                   L7_uint32 policyInstIndex,
                                                   L7_uint32 policyAttrIndex)
{
  dsmibPolicyAttrKey_t        keys;
  dsmibPolicyAttrEntryCtrl_t  *pRow;

  /* get access to the policy row */
  keys.policyIndex = policyIndex;
  keys.policyInstIndex = policyInstIndex;
  keys.policyAttrIndex = policyAttrIndex;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_POLICY_ATTR],
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Checks if a policy attribute object value is valid to 'get'
*
* @param    pRow        @b{(input)} Policy attribute table row pointer
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
L7_RC_t dsmibPolicyAttrObjectGetCheck(dsmibPolicyAttrEntryCtrl_t *pRow,
                                      L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t oid)
{
  dsmibPolicyAttrType_t  objType;
  dsmibPolicyAttrFlags_t objFlag;
  L7_RC_t                rc;

  switch (oid)
  {
  case L7_DIFFSERV_POLICY_ATTR_STMT_ENTRY_TYPE:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE; /* not checked */
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_ASSIGN_QUEUE_QID:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_QUEUEID;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_DROP:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP;
    objFlag = DSMIB_NO_FLAG;  /* no row invalid flag for this match object */
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COSVAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COSVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COS_AS_COS2:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COS_AS_COS2;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COS2VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COS2VAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_MARK_IPDSCPVAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_IPDSCPVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_MARK_IPPRECEDENCEVAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_IPPRECEDENCEVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_MIRROR_INTF:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_MIRRORINTF;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CRATE:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CBURST:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CONFORM_ACT:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CONFORM_VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_NONCONFORM_ACT:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_NONCONFORM_VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_COLOR_CONFORM_INDEX:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMINDEX;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_COLOR_CONFORM_MODE:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_COLOR_CONFORM_VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CRATE:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CBURST:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EBURST:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_EBURST;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CONFORM_ACT:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CONFORM_VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EXCEED_ACT:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EXCEED_VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_NONCONFORM_ACT:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_NONCONFORM_VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_CONFORM_INDEX:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMINDEX;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_CONFORM_MODE:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_CONFORM_VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_EXCEED_INDEX:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDINDEX;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_EXCEED_MODE:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_EXCEED_VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CRATE:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_CRATE;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CBURST:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_CBURST;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_PRATE:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_PRATE;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_PBURST:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_PBURST;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CONFORM_ACT:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMACT;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CONFORM_VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_CONFORMVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_EXCEED_ACT:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDACT;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_EXCEED_VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_EXCEEDVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_NONCONFORM_ACT:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMACT;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_NONCONFORM_VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_NONCONFORMVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_CONFORM_INDEX:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMINDEX;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_CONFORM_MODE:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMMODE;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_CONFORM_VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLORCONFORMVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_EXCEED_INDEX:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDINDEX;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_EXCEED_MODE:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDMODE;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_EXCEED_VAL:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_COLOREXCEEDVAL;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STMT_REDIRECT_INTF:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT;
    objFlag = DSMIB_POLICY_ATTR_FLAGS_STMT_REDIRECTINTF;
    break;

  case L7_DIFFSERV_POLICY_ATTR_STORAGE_TYPE:
  case L7_DIFFSERV_POLICY_ATTR_ROW_STATUS:
    objType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE; /* not checked */
    objFlag = DSMIB_NO_FLAG;  /* no row invalid flag for this match object */
    break;

  default:
    /* invalid object id */
    return L7_FAILURE;
    /*PASSTHRU*/
  } /* endswitch */

  /* using objType = 'none' to bypass entry type check for the object */
  if (objType != L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE)
  {
    rc = dsmibPolicyAttrFeaturePresentCheck(objType);
    if (rc != L7_SUCCESS)
      return rc;

    if (pRow->mib.entryType != objType)
      return L7_FAILURE;
  }

  /* using objFlag = DSMIB_NO_FLAG to bypass flag check for the object */
  if (objFlag != DSMIB_NO_FLAG)
    if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, objFlag))
      return L7_ERROR;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Establishes default row object values in accordance with
*           Policy Attribute Table row entry type
*
* @param    pRow        @b{(input)} Policy attribute table row pointer
* @param    entryType   @b{(input)} Policy attribute entry type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  Object type mismatch
*
* @notes    Normally, row objects having default values are instantiated
*           when the table row is created.  In the Policy Attribute Table,
*           it is not known which objects need defaults until the entry
*           type is established.  Not all entry types have defaulted object
*           values.
*
* @notes    This function updates the MIB object directly without going
*           through an API function.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyAttrTypeDefaultSet(dsmibPolicyAttrEntryCtrl_t *pRow,
                                      dsmibPolicyAttrType_t entryType)
{
  /* NOTE:  No need to clear the corresponding row invalid flag for the
   *        object, since this function is called when the entry type is
   *        established.  The entry type 'set' causes new row invalid
   *        flags to be stored, but these assume the default objects are
   *        already created and therefore do not include their flags.
   */

  switch (entryType)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
    /* default objects:  conform act, nonconform act, color conform mode */
    pRow->mib.stmt.policeSimple.conformAct =
                              DSMIB_CFG_DEFAULT_POLICE_CONFORM_ACT;
    pRow->mib.stmt.policeSimple.nonconformAct =
                              DSMIB_CFG_DEFAULT_POLICE_NONCONFORM_ACT;
    pRow->mib.stmt.policeSimple.colorConformIndex =
                              DSMIB_CFG_DEFAULT_POLICE_COLOR_CONFORM_INDEX;
    pRow->mib.stmt.policeSimple.colorConformMode =
                              DSMIB_CFG_DEFAULT_POLICE_COLOR_CONFORM_MODE;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
    /* default objects:  conform act, exceed act, nonconform act,
     *                   color conform mode, color exceed mode
     */
    pRow->mib.stmt.policeSingleRate.conformAct =
                              DSMIB_CFG_DEFAULT_POLICE_CONFORM_ACT;
    pRow->mib.stmt.policeSingleRate.exceedAct =
                              DSMIB_CFG_DEFAULT_POLICE_EXCEED_ACT;
    pRow->mib.stmt.policeSingleRate.nonconformAct =
                              DSMIB_CFG_DEFAULT_POLICE_NONCONFORM_ACT;
    pRow->mib.stmt.policeSingleRate.colorConformIndex =
                              DSMIB_CFG_DEFAULT_POLICE_COLOR_CONFORM_INDEX;
    pRow->mib.stmt.policeSingleRate.colorConformMode =
                              DSMIB_CFG_DEFAULT_POLICE_COLOR_CONFORM_MODE;
    pRow->mib.stmt.policeSingleRate.colorExceedIndex =
                              DSMIB_CFG_DEFAULT_POLICE_COLOR_EXCEED_INDEX;
    pRow->mib.stmt.policeSingleRate.colorExceedMode =
                              DSMIB_CFG_DEFAULT_POLICE_COLOR_EXCEED_MODE;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
    /* default objects:  conform act, exceed act, nonconform act,
     *                   color conform mode, color exceed mode
     */
    pRow->mib.stmt.policeTwoRate.conformAct =
                              DSMIB_CFG_DEFAULT_POLICE_CONFORM_ACT;
    pRow->mib.stmt.policeTwoRate.exceedAct =
                              DSMIB_CFG_DEFAULT_POLICE_EXCEED_ACT;
    pRow->mib.stmt.policeTwoRate.nonconformAct =
                              DSMIB_CFG_DEFAULT_POLICE_NONCONFORM_ACT;
    pRow->mib.stmt.policeTwoRate.colorConformIndex =
                              DSMIB_CFG_DEFAULT_POLICE_COLOR_CONFORM_INDEX;
    pRow->mib.stmt.policeTwoRate.colorConformMode =
                              DSMIB_CFG_DEFAULT_POLICE_COLOR_CONFORM_MODE;
    pRow->mib.stmt.policeTwoRate.colorExceedIndex =
                              DSMIB_CFG_DEFAULT_POLICE_COLOR_EXCEED_INDEX;
    pRow->mib.stmt.policeTwoRate.colorExceedMode =
                              DSMIB_CFG_DEFAULT_POLICE_COLOR_EXCEED_MODE;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
    /* (no defined default objects) */
    break;

  default:
    /* invalid object id */
    return L7_FAILURE;
    /*PASSTHRU*/
  } /* endswitch */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Replaces the set of row invalid flags based on the
*           Policy Attribute Table object entry type
*
* @param    pRow        @b{(input)} Policy attribute table row pointer
* @param    entryType   @b{(input)} Policy attribute entry type
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Object value not yet established
* @returns  L7_FAILURE  Object type mismatch
*
* @notes    This function causes a complete replacement of the row invalid
*           flags value instead of just turning individual bits on.  This
*           is useful for resetting the entry type flag and setting the
*           entry type-specific flags all at once.
*
* @notes    Also handles those objects not subject to type and/or flag
*           checking.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyAttrRowFlagsReplace(dsmibPolicyAttrEntryCtrl_t *pRow,
                                       dsmibPolicyAttrType_t entryType)
{
  dsmibPolicyAttrFlags_t objFlag;

  switch (entryType)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
    objFlag = DSMIB_POLICY_ATTR_FLAGS_TYPE_ASSIGN_QUEUE;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
    objFlag = DSMIB_POLICY_ATTR_FLAGS_TYPE_DROP;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
    objFlag = DSMIB_POLICY_ATTR_FLAGS_TYPE_MARK_COSVAL;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
    objFlag = DSMIB_POLICY_ATTR_FLAGS_TYPE_MARK_COS_AS_COS2;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
    objFlag = DSMIB_POLICY_ATTR_FLAGS_TYPE_MARK_COS2VAL;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
    objFlag = DSMIB_POLICY_ATTR_FLAGS_TYPE_MARK_IPDSCPVAL;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
    objFlag = DSMIB_POLICY_ATTR_FLAGS_TYPE_MARK_IPPRECEDENCEVAL;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
    objFlag = DSMIB_POLICY_ATTR_FLAGS_TYPE_MIRROR;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
    objFlag = DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_SIMPLE;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
    objFlag = DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_SINGLERATE;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
    objFlag = DSMIB_POLICY_ATTR_FLAGS_TYPE_POLICE_TWORATE;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
    objFlag = DSMIB_POLICY_ATTR_FLAGS_TYPE_REDIRECT;
    break;

  default:
    /* invalid object id */
    return L7_FAILURE;
    /*PASSTHRU*/
  } /* endswitch */

  /* replace object row invalid flags with the new value based on entry type */
  pRow->rowInvalidFlags = objFlag;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Looks for conflicting attribute types within a policy-class
*           instance
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    policyAttrIndex @b{(input)} Policy attribute index
* @param    type            @b{(input)} Attribute type to be checked
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Type conflict within policy-class instance
* @returns  L7_FAILURE
*
* @notes    Conflicts generally come in two forms:  wrong attribute
*           for policy type (in vs. out), and incompatible attributes
*           within the same instance (e.g., marking vs. policing).
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyAttrTypeCheck(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex,
                                 dsmibPolicyAttrType_t type)
{
  dsmibPolicyAttrEntryCtrl_t  *pAttr;
  dsmibPolicyAttrType_t       entryType;
  L7_BOOL                     attrConflict;
  L7_uint32                   nextPolicyIdx, nextPolicyInstIdx, attrIdx;

  /* walk through all attributes for this policy-class instance and check
   * for any conflicts of existing types with this one
   */
  nextPolicyIdx = nextPolicyInstIdx = attrIdx = 0;  /* search from beginning */
  while (diffServPolicyAttrGetNext(policyIndex, policyInstIndex, attrIdx,
                                   &nextPolicyIdx, &nextPolicyInstIdx, &attrIdx)
         == L7_SUCCESS)
  {
    /* stop when we leave this policy instance or reach end of table */
    if ((nextPolicyIdx != policyIndex) ||
        (nextPolicyInstIdx != policyInstIndex) ||
        (attrIdx == 0))
      break;

    /* skip our own instance attribute row */
    if (attrIdx == policyAttrIndex)
      continue;

    /* get a pointer to the policy attribute */
    pAttr = dsmibPolicyAttrRowFind(policyIndex, policyInstIndex, attrIdx);
    if (pAttr == L7_NULLPTR)
      return L7_FAILURE;

    /* skip this attribute if the entryType field has not been set in the
     * attribute row
     */
    if (DSMIB_ROW_INVALID_FLAG_IS_ON(pAttr, DSMIB_POLICY_ATTR_FLAGS_STMT_ENTRYTYPE))
      continue;

    entryType = pAttr->mib.entryType;

    /* do not allow the same attribute to be used more than once for a
     * given policy-class instance
     */
    if (type == entryType)
      return L7_ERROR;

    /* for a particular requested attribute type, check for incompatibilities
     * against the entryType of the current row
     */
    attrConflict = L7_FALSE;                      /* initialize */
    switch (type)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
      /* incompatibilities:  drop */
      switch (entryType)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
        attrConflict = L7_TRUE;
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
        /* check platform feature support */
        if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                  L7_DIFFSERV_POLICY_ATTR_MARK_COS_AND_ASSIGN_QUEUE_FEATURE_ID) == L7_FALSE)
        {
          attrConflict = L7_TRUE;
        }
        break;
      default:
        break;
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      /* incompatibilities:  assign queue; mark (anything); police (anything);
       *                     redirect
       */
      switch (entryType)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
        attrConflict = L7_TRUE;
        break;
      default:
        break;
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      /* incompatibilities:  drop; mark COS2, IP DSCP, IP Precedence;
       *                     police (anything)
       */
      switch (entryType)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
        attrConflict = L7_TRUE;
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
        /* check platform feature support */
        if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                  L7_DIFFSERV_POLICY_ATTR_MARK_COS_AND_ASSIGN_QUEUE_FEATURE_ID) == L7_FALSE)
        {
          attrConflict = L7_TRUE;
        }
        break;
      default:
        break;
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      /* incompatibilities:  drop; mark COS, IP DSCP, IP Precedence;
       *                     police (anything)
       */
      switch (entryType)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
        attrConflict = L7_TRUE;
        break;
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
        /* check platform feature support */
        if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                  L7_DIFFSERV_POLICY_ATTR_MARK_COS_AND_ASSIGN_QUEUE_FEATURE_ID) == L7_FALSE)
        {
          attrConflict = L7_TRUE;
        }
        break;
      default:
        break;
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      /* incompatibilities:  drop; mark COS, COS2, IP Precedence;
       *                     police (anything)
       */
      switch (entryType)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
        attrConflict = L7_TRUE;
        break;
      default:
        break;
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      /* incompatibilities:  drop; mark COS, COS2, IP DSCP;
       *                     police (anything)
       */
      switch (entryType)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
        attrConflict = L7_TRUE;
        break;
      default:
        break;
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
      /* incompatibilities:  drop, redirect */
      switch (entryType)
      {
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
        case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
          attrConflict = L7_TRUE;
          break;
        default:
          break;
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      /* incompatibilities:  drop; mark (anything); police single-rate, two-rate */
      switch (entryType)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
        attrConflict = L7_TRUE;
        break;
      default:
        break;
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      /* incompatibilities:  drop; mark (anything); police simple, two-rate */
      switch (entryType)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
        attrConflict = L7_TRUE;
        break;
      default:
        break;
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      /* incompatibilities:  drop; mark (anything); police simple, single-rate */
      switch (entryType)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
        attrConflict = L7_TRUE;
        break;
      default:
        break;
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
      /* incompatibilities:  drop, mirror */
      switch (entryType)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
        attrConflict = L7_TRUE;
        break;
      default:
        break;
      }
      break;

    default:
      /* invalid type */
      return L7_FAILURE;
      /*PASSTHRU*/
    } /* endswitch */

    /* check if requested type is in conflict */
    if (attrConflict == L7_TRUE)
      return L7_ERROR;

  } /* endwhile */

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check for conflicts between a color mode and the class definition
*
* @param    policyIndex     @b{(input)} Policy index
* @param    policyInstIndex @b{(input)} Policy instance index
* @param    colorMode       @b{(input)} Color mode to be checked
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Color mode not compatible with policy-class instance
* @returns  L7_FAILURE
*
* @notes    A conform or exceed color mode specifies the packet field to
*           be used for determining the existing packet color.  This is
*           often used in a classifier to manipulate the outcome of the
*           policing meters, and will conflict with a packet classifier
*           that contains the same field in its definition.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyAttrColorModeConflictCheck(L7_uint32 policyIndex,
                                              L7_uint32 policyInstIndex,
                                              dsmibPoliceColor_t colorMode)
{
  L7_RC_t                     rc = L7_SUCCESS;
  dsmibClassRuleType_t        colorRule;
  L7_uint32                   classIndex;
  L7_uint32                   classIdx, ruleIdx;
  dsmibClassRuleType_t        entryType;
  dsmibTruthValue_t           excludeFlag;

  /* convert colorMode to equivalent class rule entry type to use during search
   *
   * NOTE: A color mode of IP DSCP or IP Precedence conflicts with any
   *       class rule type of IP DSCP, IP Precedence, or IP TOS, so use
   *       the general IP TOS class rule type as the comparator.
   */
  switch (colorMode)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
    colorRule = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
    colorRule = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
    colorRule = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND:
  case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED:
    /* color awareness not being used */
    return L7_SUCCESS;
    /*PASSTHRU*/

  default:
    /* unrecognized color mode */
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

  /* get the class index for this policy-class instance */
  if (diffServPolicyInstObjectGet(policyIndex, policyInstIndex,
                                  L7_DIFFSERV_POLICY_INST_CLASS_INDEX,
                                  &classIndex) != L7_SUCCESS)
    return L7_FAILURE;

  /* walk through entire reference class chain for this classIndex, looking
   * for any occurrence of a non-excluded class rule match field that is
   * the same as the color mode field
   */
  classIdx = classIndex;
  ruleIdx = 0;                                  /* start with first rule */
  while (dsmibClassRuleRefChainGetNext(classIdx, ruleIdx,
                                       &classIdx, &ruleIdx) == L7_SUCCESS)
  {
    /* only check non-excluded class rules */
    if (diffServClassRuleObjectGet(classIdx, ruleIdx,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_EXCLUDE_FLAG,
                                   &excludeFlag) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

    if (excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE)
      continue;

    /* get the class rule entry type */
    if (diffServClassRuleObjectGet(classIdx, ruleIdx,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE,
                                   &entryType) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

    /* change the entry type for comparison purposes from IP DSCP or
     * IP Precedence to IP TOS so that anything related to the TOS
     * octet is considered to conflict with a color mode of IP DSCP or
     * IP Precedence
     */
    if ((entryType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP) ||
        (entryType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE))
    {
      entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS;
    }

    /* compare the class rule entry type to the translated color mode rule
     * type(s), rejecting this color mode if there is a match
     */
    if (entryType == colorRule)
    {
      /* color mode conflict found */
      rc = L7_ERROR;
      break;
    }

  } /* endwhile ref class search */

  return rc;
}

/*********************************************************************
* @purpose  Update color reference counts for DiffServ classes used
*           by color-aware policing
*
* @param    classIndexOld   @b{(input)} Class index being unreferenced
* @param    classIndexNew   @b{(input)} Class index of new reference
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The color reference count is used to prevent a class definition
*           from changing while it is being referenced by a policy instance
*           for color-aware policing.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyAttrColorClassRefCountUpdate(L7_uint32 classIndexOld,
                                                L7_uint32 classIndexNew)
{
  L7_RC_t               rc = L7_FAILURE;
  dsmibClassEntryCtrl_t *pClassOld = L7_NULLPTR;
  dsmibClassEntryCtrl_t *pClassNew = L7_NULLPTR;

  DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

  do
  {
    if (classIndexOld != 0)
    {
      /* find node in the AVL tree */
      pClassOld = dsmibClassRowFind(classIndexOld);
      if (pClassOld == L7_NULLPTR)
        break;
    }

    if (classIndexNew != 0)
    {
      /* find node in the AVL tree */
      pClassNew = dsmibClassRowFind(classIndexNew);
      if (pClassNew == L7_NULLPTR)
        break;
    }

    /* NOTE: Only updating ref counts after all error checking is complete */

    /* decrement the color ref count in the old class row */
    if (pClassOld != L7_NULLPTR)
      pClassOld->colorRefCount--;

    /* increment the color ref count in the new class row */
    if (pClassNew != L7_NULLPTR)
      pClassNew->colorRefCount++;

    rc = L7_SUCCESS;

  } while (0);

  DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

  return rc;
}

/*********************************************************************
* @purpose  Map Policy Attribute object IDs to Policy Attribute types
*
* @param    oid              @b{(input)} Object identifier
* @param    pPolicyAttrType  @b{(output)} Pointer to Policy Attr type
*
* @returns  Void
*
* @notes    None
*
* @end
*********************************************************************/
void dsmibPolicyAttrConvertOidToType(L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_TYPE_t oid,
                                     L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t *pPolicyAttrType)
{
  switch (oid)
  {
  /* NOTE:  There are no settable objects for policy attr type '_DROP' */

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COSVAL:
    *pPolicyAttrType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COS_AS_COS2:
    *pPolicyAttrType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_COS2VAL:
    *pPolicyAttrType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_IPDSCPVAL:
    *pPolicyAttrType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MARK_IPPRECEDENCEVAL:
    *pPolicyAttrType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_MIRROR_INTF:
    *pPolicyAttrType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CRATE:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CBURST:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CONFORM_ACT:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_CONFORM_VAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_NONCONFORM_ACT:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_NONCONFORM_VAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_INDEX:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_MODE:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SIMPLE_COLOR_CONFORM_VAL:
    *pPolicyAttrType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CRATE:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CBURST:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EBURST:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CONFORM_ACT:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_CONFORM_VAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EXCEED_ACT:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_EXCEED_VAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_NONCONFORM_ACT:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_NONCONFORM_VAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_INDEX:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_MODE:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_CONFORM_VAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_INDEX:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_MODE:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_SINGLERATE_COLOR_EXCEED_VAL:
    *pPolicyAttrType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CRATE:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CBURST:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_PRATE:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_PBURST:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CONFORM_ACT:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_CONFORM_VAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_EXCEED_ACT:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_EXCEED_VAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_NONCONFORM_ACT:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_NONCONFORM_VAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_INDEX:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_MODE:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_CONFORM_VAL:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_INDEX:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_MODE:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_POLICE_TWORATE_COLOR_EXCEED_VAL:
    *pPolicyAttrType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_REDIRECT_INTF:
    *pPolicyAttrType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ENTRY_TYPE:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_STORAGE_TYPE:
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_SETTEST_OID_ROW_STATUS:
    *pPolicyAttrType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
    break;

  default:
    /* invalid object identifier */
    *pPolicyAttrType = L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE;
    break;
  } /* endswitch */
}

/*********************************************************************
* @purpose  Check the existence of the specified component feature
*           in the hardware platform
*
* @param    policyAttrType  @b{(input)} Policy Attribute type
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  Invalid policy attribute type
* @returns  L7_NOT_SUPPORTED  Feature not supported in hardware platform
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyAttrFeaturePresentCheck(L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_t policyAttrType)
{
  L7_DIFFSERV_FEATURE_IDS_t fid;
  L7_BOOL                   featurePresent;

  switch (policyAttrType)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
    fid = L7_DIFFSERV_POLICY_ATTR_ASSIGN_QUEUE_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
    fid = L7_DIFFSERV_POLICY_ATTR_DROP_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
    fid = L7_DIFFSERV_POLICY_ATTR_MARK_COSVAL_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
    fid = L7_DIFFSERV_POLICY_ATTR_MARK_COS2VAL_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
    fid = L7_DIFFSERV_POLICY_ATTR_MARK_IPDSCPVAL_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
    fid = L7_DIFFSERV_POLICY_ATTR_MARK_COSASCOS2_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
    fid = L7_DIFFSERV_POLICY_ATTR_MARK_IPPRECEDENCEVAL_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
    fid = L7_DIFFSERV_POLICY_ATTR_MIRROR_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
    fid = L7_DIFFSERV_POLICY_ATTR_POLICE_SIMPLE_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
    fid = L7_DIFFSERV_POLICY_ATTR_POLICE_SINGLERATE_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
    fid = L7_DIFFSERV_POLICY_ATTR_POLICE_TWORATE_FEATURE_ID;
    break;

  /* NOTE:  L7_DIFFSERV_POLICY_ATTR_POLICE_COLORAWARE_FEATURE_ID is checked
   *        inside the simple, single-rate, and two-rate sections of the
   *        settest function.
   */

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
    fid = L7_DIFFSERV_POLICY_ATTR_REDIRECT_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_NONE:
    /* this value is used to handle certain generic objects (row status, etc) */
    fid = L7_DIFFSERV_FEATURE_SUPPORTED;
    break;

  default:
    /* invalid policy attribute type */
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

  featurePresent = cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, fid);

  return (featurePresent == L7_TRUE) ? L7_SUCCESS : L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Determine the IP DSCP PHB feature support
*
* @param    Void
*
* @returns  Void
*
* @notes    IP DSCP value support is determined based on the
*           'feature-present' check.
*
* @notes    This function gets called when private MIB Policy support
*           is initialized.
*
* @end
*********************************************************************/
void dsmibPolicyAttrIpDscpSupportInit(void)
{
  L7_uchar8   initVal;
  L7_uint32   i;

  initVal = (L7_uchar8)cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                             L7_DIFFSERV_PHB_OTHER_FEATURE_ID);

  for (i = 1; i < DSMIB_IPDSCP_TOTAL; i++)
    policyAttrIpDscpValSupported[i] = initVal;

  /* always set 0 element to 'supported' */
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_BE] = L7_TRUE;

  /* for each separate PHB, set its dscp to 'supported' or 'not-supported' */
  initVal = (L7_uchar8)cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                             L7_DIFFSERV_PHB_EF_FEATURE_ID);
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_EF] = initVal;

  initVal = (L7_uchar8)cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                             L7_DIFFSERV_PHB_AF4X_FEATURE_ID);
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF41] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF42] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF43] = initVal;

  initVal = (L7_uchar8)cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                             L7_DIFFSERV_PHB_AF3X_FEATURE_ID);
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF31] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF32] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF33] = initVal;

  initVal = (L7_uchar8)cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                             L7_DIFFSERV_PHB_AF2X_FEATURE_ID);
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF21] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF22] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF23] = initVal;

  initVal = (L7_uchar8)cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                             L7_DIFFSERV_PHB_AF1X_FEATURE_ID);
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF11] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF12] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_AF13] = initVal;

  initVal = (L7_uchar8)cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                                             L7_DIFFSERV_PHB_CS_FEATURE_ID);
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS1] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS2] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS3] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS4] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS5] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS6] = initVal;
  policyAttrIpDscpValSupported[L7_USMDB_MIB_DIFFSERV_IP_DSCP_CS7] = initVal;
}


/*
================================================
================================================
================================================

   POLICY ATTRIBUTE TABLE DEBUG SHOW FUNCTION

================================================
================================================
================================================
*/

/*********************************************************************
* @purpose  Display the current DiffServ Policy Attribute Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsmibPolicyAttrTableShow(void)
{
  L7_uint32                  msgLvlReqd;
  dsmibPolicyAttrEntryCtrl_t *pRow;
  dsmibPolicyAttrKey_t       keys;
  avlTree_t                  *pTree = &dsmibAvlTree[DSMIB_TABLE_ID_POLICY_ATTR];
  L7_uint32                  count;
  char                       *pSpacer;

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Policy Attr Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Policy Attr Table (contains %u entries):\n\n", count);

  DIFFSERV_PRT(msgLvlReqd, " Indexes   Type ST RS TID  Flags   RP ");
  DIFFSERV_PRT(msgLvlReqd, "Type-Specific Fields                      \n");
  DIFFSERV_PRT(msgLvlReqd, "---------- ---- -- -- --- -------- -- ");
  pSpacer =                "                                     "; /* spacer */
  DIFFSERV_PRT(msgLvlReqd, "------------------------------------------\n");


  keys.policyIndex = 0;                          /* start with first entry */
  keys.policyInstIndex = 0;                      /* start with first entry */
  keys.policyAttrIndex = 0;
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    /* part A */
    DIFFSERV_PRT(msgLvlReqd, " %2u %2u %2u  %4s %2s %2s %2u  %8.8x %2s ",
                 pRow->key.policyIndex,
                 pRow->key.policyInstIndex,
                 pRow->key.policyAttrIndex,
                 dsmibPolicyAttrTypeStr[pRow->mib.entryType],
                 dsmibStgTypeStr[pRow->mib.storageType],
                 dsmibRowStatusStr[pRow->mib.rowStatus],
                 pRow->tableId,
                 pRow->rowInvalidFlags,
                 dsmibRowPendingStr[pRow->rowPending]);

    /* part B -- depends on entry type */
    switch (pRow->mib.entryType)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE:
      DIFFSERV_PRT(msgLvlReqd, "qid=%u\n",
                   pRow->mib.stmt.assignQueue.qid);
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      DIFFSERV_PRT(msgLvlReqd, "drop=%s\n",
                   dsmibTruthValueStr[pRow->mib.stmt.drop]);
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      DIFFSERV_PRT(msgLvlReqd, "cos=%u\n", pRow->mib.stmt.markCos.val);
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
      DIFFSERV_PRT(msgLvlReqd, "cosAsCos2=%u\n", pRow->mib.stmt.markCosAsCos2.val);
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      DIFFSERV_PRT(msgLvlReqd, "cos2=%u\n", pRow->mib.stmt.markCos2.val);
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      DIFFSERV_PRT(msgLvlReqd, "dscp=%u\n", pRow->mib.stmt.markIpDscp.val);
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      DIFFSERV_PRT(msgLvlReqd, "prec=%u\n", pRow->mib.stmt.markIpPrecedence.val);
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR:
      DIFFSERV_PRT(msgLvlReqd, "mirror configId: type=%s usp=%u/%u/%u\n",
                   dsmibNimIntfTypeStr[pRow->mib.stmt.mirror.configId.type],
                   (L7_uint32)pRow->mib.stmt.mirror.configId.configSpecifier.usp.unit,
                   (L7_uint32)pRow->mib.stmt.mirror.configId.configSpecifier.usp.slot,
                   (L7_uint32)pRow->mib.stmt.mirror.configId.configSpecifier.usp.port);
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      DIFFSERV_PRT(msgLvlReqd, "crate=%u cburst=%u confAct=%s\n",
                   pRow->mib.stmt.policeSimple.crate,
                   pRow->mib.stmt.policeSimple.cburst,
                   dsmibPoliceActStr[pRow->mib.stmt.policeSimple.conformAct]);
      DIFFSERV_PRT(msgLvlReqd, "%s confVal=%u nonconfAct=%s nonconfVal=%u\n",
                   pSpacer,
                   pRow->mib.stmt.policeSimple.conformVal,
                   dsmibPoliceActStr[pRow->mib.stmt.policeSimple.nonconformAct],
                   pRow->mib.stmt.policeSimple.nonconformVal);
      DIFFSERV_PRT(msgLvlReqd, "%s color: Cidx=%u Cmode=%s Cval=%u\n",
                   pSpacer,
                   pRow->mib.stmt.policeSimple.colorConformIndex,
                   dsmibPoliceColorStr[pRow->mib.stmt.policeSimple.colorConformMode],
                   pRow->mib.stmt.policeSimple.colorConformVal);
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      DIFFSERV_PRT(msgLvlReqd, "crate=%u cburst=%u eburst=%u\n",
                   pRow->mib.stmt.policeSingleRate.crate,
                   pRow->mib.stmt.policeSingleRate.cburst,
                   pRow->mib.stmt.policeSingleRate.eburst);
      DIFFSERV_PRT(msgLvlReqd, "%s confAct=%s confVal=%u excdAct=%s\n",
                   pSpacer,
                   dsmibPoliceActStr[pRow->mib.stmt.policeSingleRate.conformAct],
                   pRow->mib.stmt.policeSingleRate.conformVal,
                   dsmibPoliceActStr[pRow->mib.stmt.policeSingleRate.exceedAct]);
      DIFFSERV_PRT(msgLvlReqd, "%s excdVal=%u nonconfAct=%s nonconfVal=%u\n",
                   pSpacer,
                   pRow->mib.stmt.policeSingleRate.exceedVal,
                   dsmibPoliceActStr[pRow->mib.stmt.policeSingleRate.nonconformAct],
                   pRow->mib.stmt.policeSingleRate.nonconformVal);
      DIFFSERV_PRT(msgLvlReqd, "%s color: Cidx=%u Cmode=%s Cval=%u Eidx=%u Emode=%s Eval=%u\n",
                   pSpacer,
                   pRow->mib.stmt.policeSingleRate.colorConformIndex,
                   dsmibPoliceColorStr[pRow->mib.stmt.policeSingleRate.colorConformMode],
                   pRow->mib.stmt.policeSingleRate.colorConformVal,
                   pRow->mib.stmt.policeSingleRate.colorExceedIndex,
                   dsmibPoliceColorStr[pRow->mib.stmt.policeSingleRate.colorExceedMode],
                   pRow->mib.stmt.policeSingleRate.colorExceedVal);
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
      DIFFSERV_PRT(msgLvlReqd, "crate=%u cburst=%u prate=%u\n",
                   pRow->mib.stmt.policeTwoRate.crate,
                   pRow->mib.stmt.policeTwoRate.cburst,
                   pRow->mib.stmt.policeTwoRate.prate);
      DIFFSERV_PRT(msgLvlReqd, "%s pburst=%u confAct=%s confVal=%u\n",
                   pSpacer,
                   pRow->mib.stmt.policeTwoRate.pburst,
                   dsmibPoliceActStr[pRow->mib.stmt.policeTwoRate.conformAct],
                   pRow->mib.stmt.policeTwoRate.conformVal);
      DIFFSERV_PRT(msgLvlReqd, "%s excdAct=%s excdVal=%u\n",
                   pSpacer,
                   dsmibPoliceActStr[pRow->mib.stmt.policeTwoRate.exceedAct],
                   pRow->mib.stmt.policeTwoRate.exceedVal);
      DIFFSERV_PRT(msgLvlReqd, "%s nonconfAct=%s nonconfVal=%u\n",
                   pSpacer,
                   dsmibPoliceActStr[pRow->mib.stmt.policeTwoRate.nonconformAct],
                   pRow->mib.stmt.policeTwoRate.nonconformVal);
      DIFFSERV_PRT(msgLvlReqd, "%s color: Cidx=%u Cmode=%s Cval=%u Eidx=%u Emode=%s Eval=%u\n",
                   pSpacer,
                   pRow->mib.stmt.policeTwoRate.colorConformIndex,
                   dsmibPoliceColorStr[pRow->mib.stmt.policeTwoRate.colorConformMode],
                   pRow->mib.stmt.policeTwoRate.colorConformVal,
                   pRow->mib.stmt.policeTwoRate.colorExceedIndex,
                   dsmibPoliceColorStr[pRow->mib.stmt.policeTwoRate.colorExceedMode],
                   pRow->mib.stmt.policeTwoRate.colorExceedVal);
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT:
      DIFFSERV_PRT(msgLvlReqd, "redirect configId: type=%s usp=%u/%u/%u\n",
                   dsmibNimIntfTypeStr[pRow->mib.stmt.redirect.configId.type],
                   (L7_uint32)pRow->mib.stmt.redirect.configId.configSpecifier.usp.unit,
                   (L7_uint32)pRow->mib.stmt.redirect.configId.configSpecifier.usp.slot,
                   (L7_uint32)pRow->mib.stmt.redirect.configId.configSpecifier.usp.port);
      break;

    default:
      DIFFSERV_PRT(msgLvlReqd, "\n");
      break;
    }

    /* update search keys for next pass */
    keys = pRow->key;
  }
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

