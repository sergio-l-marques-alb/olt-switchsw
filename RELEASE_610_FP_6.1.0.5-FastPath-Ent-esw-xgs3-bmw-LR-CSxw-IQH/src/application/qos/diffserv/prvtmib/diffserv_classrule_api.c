/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_classrule_api.c
*
* @purpose    DiffServ component Private MIB Class Rule Table
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


/*
===========================
===========================
===========================

   CLASS RULE TABLE APIs

===========================
===========================
===========================
*/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new row in the Class Rule Table
*
* @param    classIndex      @b{(input)}  Class index
* @param    pClassRuleIndex @b{(output)} Pointer to index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    An L7_SUCCESS return with an output *pClassRuleIndex value
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
L7_RC_t diffServClassRuleIndexNext(L7_uint32 classIndex,
                                   L7_uint32 *pClassRuleIndex)
{
  L7_RC_t       rc = L7_SUCCESS;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;
  if (pClassRuleIndex == L7_NULLPTR)
    return L7_FAILURE;

  DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

  do                                            /* single-pass control loop */
  {
    /* always output a value of 0 if the MIB table size is at maximum */
    if (dsmibMibTableSizeCheck(DSMIB_TABLE_ID_CLASS_RULE) != L7_SUCCESS)
    {
      /* table full -- L7_SUCCESS */
      *pClassRuleIndex = 0;
      break;
    }

    if (dsmibClassRuleIndexNextRead(classIndex, pClassRuleIndex) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

  } while (0);

  DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

  if (rc != L7_SUCCESS)
    *pClassRuleIndex = 0;

  return rc;
}

/*************************************************************************
* @purpose  Retrieve the maximum class rule index value allowed for the 
*           Class Rule Table
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
L7_RC_t diffServClassRuleIndexMaxGet(L7_uint32 *pIndexMax)
{
  /* check inputs */
  if (pIndexMax == L7_NULLPTR)
    return L7_FAILURE;

  *pIndexMax = DSMIB_CLASS_RULE_INDEX_MAX;
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Create a new row in the Class Rule Table for the specified indexes
*
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    activateRow     @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Rule index value in use, row not complete
* @returns  L7_ALREADY_CONFIGURED  Rule index value in use, row complete
* @returns  L7_FAILURE
*
* @notes    An L7_ERROR response means the class rule index value is currently
*           in use, but the row is not complete.  This is typically
*           caused when multiple requestors obtain the same "index next"
*           value and race to create the new row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_ALREADY_CONFIGURED response means the class rule index
*           value is currently in use for a completed row.  The caller
*           should get a new index value and re-attempt to create the row.
*
* @notes    The row status is initially set to 'not ready' and remains that 
*           way until every object in the row contains a valid value.  If the
*           activateRow parameter is set to L7_TRUE, the row status will then
*           be automatically updated to 'active'.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleCreate(L7_uint32 classIndex, 
                                L7_uint32 classRuleIndex,
                                L7_BOOL activateRow)
{
  L7_RC_t                   rc = L7_FAILURE;
  L7_uint32                 dataChanged = L7_FALSE;
  dsmibTableId_t            tableId = DSMIB_TABLE_ID_CLASS_RULE;
  dsmibClassRuleEntryCtrl_t row, *pRow = L7_NULLPTR;
  dsmibClassEntryCtrl_t     *pClass;

  DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                           classIndex, classRuleIndex, 0, 
                           (L7_uchar8)activateRow);

  do
  {
    /* check inputs */
    if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
      break;
    if (DSMIB_CLASS_RULE_INDEX_OUT_OF_RANGE(classRuleIndex))
      break;
    if ((activateRow != L7_FALSE) && (activateRow != L7_TRUE))
      break;

    /* verify index value matches expected value (i.e., current 'IndexNext')
     * for lowest-order creation index
     */
    if (dsmibClassRuleIndexNextVerify(classIndex, classRuleIndex) != L7_SUCCESS)
      break;

    /* make sure row does not already exist (someone else could have created
     * it first)
     */
    pRow = dsmibClassRuleRowFind(classIndex, classRuleIndex);
    if (pRow != L7_NULLPTR)           /* row exists */
    {
      rc = (DSMIB_ROW_IS_COMPLETE(pRow) == L7_TRUE) ? L7_ALREADY_CONFIGURED : L7_ERROR;
      break;
    }

    /* make sure there is room in the table */
    if (dsmibMibTableSizeCheck(tableId) != L7_SUCCESS)
      break;

    /* get access to the class row */
    pClass = dsmibClassRowFind(classIndex);
    if (pClass == L7_NULLPTR)
      break;

    /* don't create new rule if class is referenced by any policy policing
     * color awareness attribute
     */
    if (pClass->colorRefCount != 0)
      break;

    /* make sure the class type has been set, since it is used in certain
     * cases for class rule validation
     */
    if ((pClass->mib.type == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_NONE) ||
        (pClass->mib.type >= L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_TOTAL))
      break;

    /* setup a temporary MIB row element structure */
    pRow = &row;
    memset(pRow, 0, sizeof(*pRow));

    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* initialize row index keys */
    pRow->key.classIndex = classIndex;
    pRow->key.classRuleIndex = classRuleIndex;

    /* initialize control fields */
    pRow->tableId = tableId;
    pRow->rowInvalidFlags = DSMIB_CLASS_RULE_FLAGS_COMMON;  /* starting value */
    pRow->pClass = pClass;                        /* back ptr to class row */
    pRow->rowPending = L7_TRUE;
    pRow->arid = DSMIB_CLASS_RULE_ARID_NONE;

    /* fill in initial row status and any objects with known defaults */
    pRow->mib.index = classRuleIndex;
    pRow->mib.entryType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
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

      /* keep track of the number of rules within a class */
      pClass->ruleCount++;

      dataChanged = L7_TRUE;            /* config change occurred */

      /* call distiller after row creation in a "dependent" table so the change
       * gets detected
       *
       * NOTE: Skip this for class type 'acl' -- it will be done after the
       *       entire class is created.
       */
      if (pClass->mib.type != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
      {
        rc = dsDistillerEvaluateAll();
        if (rc != L7_SUCCESS)
        {
          /* delete the just-created row due to evaluation failure */
          if (dsmibRowDelete(&dsmibAvlTree[tableId], pRow) == L7_SUCCESS)
          {
            /* clean up the various count values that were incremented above */
            genStatus.tableSize[tableId]--;
            pClass->ruleCount--;

            dataChanged = L7_FALSE;       /* config change undone */
          }
          else
          {
            LOG_MSG("diffServClassRuleCreate: Error deleting row after create "
                    "failure for indexes %u,%u\n", classIndex, classRuleIndex);
          }
        }
      } /* endif class type not 'acl' */
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
                          classIndex, classRuleIndex, 0, 
                          (L7_uint32)pRow, (L7_uchar8)rc,
                          (L7_uchar8)activateRow);

  return rc;
}

/*************************************************************************
* @purpose  Delete a row from the Class Rule Table for the specified index
*
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE
*
* @notes    In general, class rules cannot be deleted from the user interface.
*           Instead, deletion normally occurs due to the deletion of a class
*           row, which in turn deletes each class rule within it.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleDelete(L7_uint32 classIndex, 
                                L7_uint32 classRuleIndex)
{
  /* delete the class rule, but allow distiller eval to occur */
  return dsmibClassRuleDelete(classIndex, classRuleIndex, L7_FALSE);
}

/*************************************************************************
* @purpose  Delete all rows from the Class Rule Table for the specified class
*
* @param    classIndex      @b{(input)} Class index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    In general, class rules cannot be deleted from the user interface.
*           Instead, deletion normally occurs due to the deletion of a class
*           row, which in turn deletes each class rule within it.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleDeleteAll(L7_uint32 classIndex)
{
  L7_RC_t       rc = L7_SUCCESS;
  L7_uint32     classRuleIndex;
  L7_uint32     nextClassIndex;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;

  /* delete all rules belonging to this class */
  classRuleIndex = 0;                           /* start with first rule */
  while ((diffServClassRuleGetNext(classIndex, classRuleIndex, &nextClassIndex, 
                                   &classRuleIndex) == L7_SUCCESS) &&
         (nextClassIndex == classIndex))
  {
    if (classRuleIndex == 0)
      break;

    /* delete the policy instance and set flag to bypass distiller eval (caller 
     * will do it)
     *
     * not sending policy notify event callbacks from here either (let caller 
     * decide)
     */
    if (dsmibClassRuleDelete(classIndex, classRuleIndex, L7_TRUE) != L7_SUCCESS)
    {
      LOG_MSG("%s: could not delete rule %u from class index %u\n",
              __FUNCTION__, classRuleIndex, classIndex);
      rc = L7_FAILURE;
      break;
    }

  } /* endwhile */

  return rc;
}

/*************************************************************************
* @purpose  Delete a row from the Class Rule Table for the specified index
*
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    bypassEval      @b{(input)} Flag for skipping distiller evaluation
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE
*
* @notes    In general, class rules cannot be deleted from the user interface.
*           Instead, deletion normally occurs due to the deletion of a class
*           row, which in turn deletes each class rule within it.
*
* @notes    Internal function used for normal and delete-all cases.
*           When bypassEval is L7_TRUE, this is part of a higher-level
*           deletion so the distiller evaluation can be skipped for 
*           performance reasons.
*
* @end
*********************************************************************/
L7_RC_t dsmibClassRuleDelete(L7_uint32 classIndex, 
                             L7_uint32 classRuleIndex,
                             L7_BOOL   bypassEval)
{
  L7_RC_t                   rc = L7_FAILURE;
  dsmibTableId_t            tableId = DSMIB_TABLE_ID_CLASS_RULE;
  L7_BOOL                   undoIndexNext = L7_FALSE;
  L7_BOOL                   needNotify = L7_FALSE;
  dsmibClassRuleEntryCtrl_t *pRow = L7_NULLPTR;
  dsmibClassEntryCtrl_t     *pClass, *pRefClass;
  dsmibRowStatus_t          prevRowStatus;
  L7_BOOL                   prevRowPending;

  DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                           classIndex, classRuleIndex, 0);

  do
  {
    /* check inputs */
    if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
      break;
    if (DSMIB_CLASS_RULE_INDEX_OUT_OF_RANGE(classRuleIndex))
      break;
    if ((bypassEval != L7_FALSE) && (bypassEval != L7_TRUE))
      break;

    /* find node in AVL tree */
    pRow = dsmibClassRuleRowFind(classIndex, classRuleIndex);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_ERROR;
      break;
    }

    /* obtain class row pointer and save it for later */
    pClass = pRow->pClass;

    /* don't delete rule if class is referenced by any policy policing
     * color awareness attribute
     */
    if (pClass->colorRefCount != 0)
      break;

    /* if this is a 'refclass' rule, also obtain the referenced class row 
     * pointer (this pointer must be null if not a 'refclass' rule)
     *
     * NOTE:  The 'get check' function does the necessary object type checking
     */
    pRefClass = L7_NULLPTR;                       /* MUST be init'd to null */
    if (dsmibClassRuleObjectGetCheck(pRow, 
            L7_DIFFSERV_CLASS_RULE_MATCH_REFCLASS_INDEX) == L7_SUCCESS)
    {
      if (pRow->mib.match.refClassIndex != 
          L7_USMDB_MIB_DIFFSERV_CLASS_RULE_REFCLASS_NONE)
      {
        pRefClass = dsmibClassRowFind(pRow->mib.match.refClassIndex);
        if (pRefClass == L7_NULLPTR)
          break;
      }
    }

    /* change an 'ACTIVE' row status to 'NOT_IN_SERVICE' prior to row deletion
     * (this transition needed for proper device cleanup)
     *
     * NOTE: Skip this for class type 'acl' -- it will be done after the
     *       entire class is deleted.
     */
    prevRowStatus = pRow->mib.rowStatus;
    if (pClass->mib.type != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
    {
      if (prevRowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
      {
        pRow->mib.rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;

        if (dsmibRowEventCheck(DSMIB_TABLE_ID_CLASS_RULE, pRow, 
                               prevRowStatus, pRow->mib.rowStatus) != L7_SUCCESS)
          break;
      }
    }

    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* save row pending flag for change notification */
    prevRowPending = pRow->rowPending;

    /* before deleting the actual row, check if this is the most recent rule
     * created for this class (successful creations use sequential class rule
     * index values)
     *
     * if so, set a flag to undo the indexNext value if and only if the row
     * had ever reached completion status
     */
    if (diffServClassRuleIsMostRecent(classIndex, classRuleIndex) == L7_TRUE)
    {
      if (pRow->rowPending != L7_TRUE)
        undoIndexNext = L7_TRUE;
    }

    /* delete the class rule row AVL tree node */
    rc = dsmibRowDelete(&dsmibAvlTree[tableId], pRow);

    if (rc == L7_SUCCESS)
    {
      /* successful row deletion
       * NOTE: Do not use pRow from this point on.
       */
      pRow = L7_NULLPTR;

      /* if this had been the most recently created class rule row and it 
       * reached completion status at least once, undo the indexNext value
       * 
       * NOTE: This is being done primarily to handle internal failure cases
       *       during the normal establishment of a row.
       */
      if (undoIndexNext == L7_TRUE)
        dsmibClassRuleIndexNextUndo(classIndex);

      /* update current table size */
      genStatus.tableSize[tableId]--;

      /* this rule is no longer in the class */
      pClass->ruleCount--;

      /* a non-null pRefClass ptr indicates this was a 'refclass' rule, so 
       * decrement the reference count in the referenced class row and clear
       * the class reference rule usage flag
       */
      if (pRefClass != L7_NULLPTR)
      {
        pRefClass->refCount--;
        pClass->refClassIndex = DSMIB_CLASS_REF_INDEX_NONE;
      }

      /* make sure change gets picked up when config is saved */
      pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;

      if (bypassEval != L7_TRUE)
      {
        /* call distiller after row deletion
         *
         * NOTE: Skip this for class type 'acl' -- it will be done after the
         *       entire class is deleted.  Callbacks are not necessary for
         *       type 'acl' classes.
         */
        if (pClass->mib.type != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
        {
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
    }
    else
    {
      L7_uchar8   className[L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX+1];

      /* something's wrong */
      if (diffServClassObjectGet(classIndex, L7_DIFFSERV_CLASS_NAME, className)
            == L7_SUCCESS)
      {
        LOG_MSG("diffServClassRuleDelete: couldn't delete row AVL node for rule "
                "%u in class \'%s\'\n", classRuleIndex, (char *)className);
      }
      else
      {
        LOG_MSG("diffServClassRuleDelete: couldn't delete row AVL node for "
                "indexes %u,%u\n", classIndex, classRuleIndex);
      }
      rc = L7_FAILURE;
    }

    DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

  } while (0);

  if (needNotify == L7_TRUE)
  {
    /* send callbacks for all policies that reference the affected class */
    dsmibClassRulePolicyChangeNotify(classIndex);
  }

  DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                          classIndex, classRuleIndex, 0, 
                          (L7_uint32)pRow, (L7_uchar8)rc);

  return rc;
}

/*********************************************************************
* @purpose  Verify that a Class Rule Table row exists for the specified indexes
*
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleGet(L7_uint32 classIndex, 
                             L7_uint32 classRuleIndex)
{
  dsmibClassRuleEntryCtrl_t *pRow;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;
  if (DSMIB_CLASS_RULE_INDEX_OUT_OF_RANGE(classRuleIndex))
    return L7_FAILURE;

  /* find node in AVL tree */
  pRow = dsmibClassRuleRowFind(classIndex, classRuleIndex);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_ERROR;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Class Rule Table
*
* @param    prevClassIndex      @b{(input)}  Class index to begin search
* @param    prevClassRuleIndex  @b{(input)}  Class rule index to begin search
* @param    pClassIndex         @b{(output)} Pointer to next sequential class
*                                              index value
* @param    pClassRuleIndex     @b{(output)} Pointer to next sequential class
*                                              rule index value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevClassIndex of 0 is used to find the first class in the table.
*
* @notes    A prevClassRuleIndex of 0 is used to find the first rule relative
*           to the specified prevClassIndex.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleGetNext(L7_uint32 prevClassIndex, 
                                 L7_uint32 prevClassRuleIndex,
                                 L7_uint32 *pClassIndex,
                                 L7_uint32 *pClassRuleIndex)
{
  dsmibClassRuleEntryCtrl_t *pRow;
  dsmibClassRuleKey_t       keys;

  /* check inputs */
  /* NOTE: Allow any starting index value to be used. */
  if (pClassIndex == L7_NULLPTR)
    return L7_FAILURE;
  if (pClassRuleIndex == L7_NULLPTR)
    return L7_FAILURE;

  /* find node in AVL tree
   * NOTE: Need to use direct call here due to AVL_NEXT flag.
   */
  keys.classIndex = prevClassIndex;
  keys.classRuleIndex = prevClassRuleIndex;
  pRow = (dsmibClassRuleEntryCtrl_t *)
           dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_CLASS_RULE], 
                        &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
    /* next row not found -- must have reached end of table */
    return L7_ERROR;

  /* found next sequential table row -- set caller's output variables */
  *pClassIndex = pRow->key.classIndex;
  *pClassRuleIndex = pRow->key.classRuleIndex;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Class Rule  
*           Table to determine its legitimacy
*
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
* @param    oid             @b{(input)} Object identifier
* @param    pValue          @b{(input)} Pointer to proposed value to be set
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR          Proposed value invalid
* @returns  L7_NOT_SUPPORTED  Feature is not supported on this platform
* @returns  L7_FAILURE  
*
* @notes    This is a 'wrapper' function used for tracing.  The real code
*           is contained in dsmibClassSetTest().
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleSetTest(L7_uint32 classIndex,
                                 L7_uint32 classRuleIndex,
                                 L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_TYPE_t oid,
                                 void *pValue)
{
  dsmibTableId_t              tableId = DSMIB_TABLE_ID_CLASS_RULE;
  dsmibClassRuleEntryCtrl_t   *pRowTrace = L7_NULLPTR;
  size_t                      dsTraceValueLen = 0;
  L7_uchar8                   dsTraceValueData[DS_TRACE_OBJ_VAL_LEN_MAX];
  L7_RC_t                     rc;

  rc = dsmibClassRuleSetTest(classIndex, classRuleIndex, oid, pValue,
                             &pRowTrace, &dsTraceValueLen);

  if (rc != L7_SUCCESS)
  {
    /* format the OID value for the trace point */
    dsTraceObjectValFormat(pValue, dsTraceValueLen, dsTraceValueData);

    DS_TRACE_PT_OBJECT_SETTEST_FAIL(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                                    classIndex, classRuleIndex, 0, 
                                    (L7_ulong32)pRowTrace, (L7_uchar8)rc,
                                    (L7_uchar8)oid, (L7_uchar8)dsTraceValueLen,
                                    dsTraceValueData);
  }

  return rc;
}

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Class Rule  
*           Table to determine its legitimacy
*
* @param    classIndex      @b{(input)}  Class index
* @param    classRuleIndex  @b{(input)}  Class rule index
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(input)}  Pointer to proposed value to be set
* @param    ppRow           @b{(output)} Pointer to row ptr output location
* @param    pValueLen       @b{(output)} Pointer to value length output location
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR          Proposed value invalid
* @returns  L7_NOT_SUPPORTED  Feature is not supported on this platform
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
* @notes    THE REQUIRED 'SET' ORDER FOR A ROW IN THIS TABLE IS:  1) entry
*           type, 2) exclude flag, 3) individual match object(s).
*
* @notes    The ppRow and pValueLen parms are used for tracing.
*
* @end
*********************************************************************/
L7_RC_t dsmibClassRuleSetTest(L7_uint32 classIndex,
                              L7_uint32 classRuleIndex,
                              L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_TYPE_t oid,
                              void *pValue,
                              dsmibClassRuleEntryCtrl_t **ppRow,
                              size_t *pValueLen)
{
  dsmibClassRuleEntryCtrl_t *pRow;
  dsmibClassRuleFlags_t     flag;
  dsmibClassRuleType_t      classRuleType;
  dsmibClassEntryCtrl_t     *pClass;
  L7_uint32                 val;
  L7_RC_t                   rc;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;
  if (DSMIB_CLASS_RULE_INDEX_OUT_OF_RANGE(classRuleIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the class rule row */
  pRow = dsmibClassRuleRowFind(classIndex, classRuleIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  *ppRow = pRow;

  /* get access to the class row */
  pClass = pRow->pClass;
  if (pClass == L7_NULLPTR)
    return L7_FAILURE;

  /* NOTE: Exclude flag feature support always passes this checking, but is
   *       tested for real in its case statement.
   */
  dsmibClassRuleConvertOidToType(oid, &classRuleType);
  rc = dsmibClassRuleFeaturePresentCheck(classRuleType);
  if (rc != L7_SUCCESS)
    return rc;
  
  /* using a flag variable to perform additional checking after the 
   * switch statement for a match criterion object (a zero flag value
   * means object is not a match criterion)
   */
  flag = 0;

  /* compare object against its allowed values */
  switch (oid)
  {

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ENTRY_TYPE:
    {
      dsmibClassRuleType_t  entryType = *(dsmibClassRuleType_t *)pValue;
      *pValueLen = sizeof(entryType);

      if ((entryType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE) ||
          (entryType >= L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_TOTAL))
        return L7_ERROR;

      rc = dsmibClassRuleFeaturePresentCheck(entryType);
      if (rc != L7_SUCCESS)
        return rc;

      /* check that the total number of rules for all nested class 
       * references does not exceed a maximum limit
       *
       * NOTE:  The class rule row was already created, so this rule
       *        will be counted as part of the class, hence any class
       *        reference chains using this class can be checked against
       *        the allowed upper limit of nested rules.
       */
      if (dsmibClassRuleRefLimitCheck() != L7_SUCCESS)
        return L7_FAILURE;

      /* NOTE: Cannot validate the class rule entry type against all policy 
       *       instances here, since the entry type has not been set in the
       *       class rule row.  This is done when the class rule entry type 
       *       field is set.
       */

      /* do not allow entry type to be set for a class type 'acl' rule
       * (unless in 'special config mode')
       *
       * NOTE: Blocking the setting of the entry type effectively blocks
       *       the setting of the exclude flag and the various type-based
       *       class rule match objects, which is what we want to happen.
       *       Basically, this allows only the storage type and row status
       *       objects to be set via USMDB.
       */
      if (pClass->mib.type == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
        if (dsmibSpecialConfigModeGet() != L7_TRUE)
          return L7_FAILURE;
      
      /* once set, this value is not allowed to change */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE))
        return L7_FAILURE;

      /* only one 'refclass' rule allowed per class definition */
      if (entryType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS)
        if  (pClass->refClassIndex != DSMIB_CLASS_REF_INDEX_NONE)
          return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_COS:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MIN) || 
          (val > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS)
        return L7_FAILURE;

      flag = DSMIB_CLASS_RULE_FLAGS_MATCH_COS;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_COS2:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MIN) || 
          (val > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_COS_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2)
        return L7_FAILURE;

      flag = DSMIB_CLASS_RULE_FLAGS_MATCH_COS;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIP_ADDR:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIP_MASK:
    {
      /* NOTE: Any numeric value is allowed. */
      *pValueLen = sizeof(L7_uint32);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP)
        return L7_FAILURE;

      if (oid == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIP_ADDR)
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR;
      else
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIPV6_ADDR:
    {
      *pValueLen = sizeof(L7_in6_addr_t);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6)
        return L7_FAILURE;

      flag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6ADDR;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIPV6_ADDR_PLEN:
    {
      *pValueLen = sizeof(L7_uint32);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6)
        return L7_FAILURE;

      flag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6PLEN;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTL4PORT_START:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTL4PORT_END:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MIN) || 
          (val > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != 
          L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT)
        return L7_FAILURE;

      if (oid == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTL4PORT_START)
      {
        /* starting value cannot be greater than ending value (if set) */
        if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END))
          if (val > pRow->mib.match.dstL4Port.end)
            return L7_ERROR;
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START;
      }
      else
      {
        /* ending value cannot be less than starting value (if set) */
        if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START))
          if (val < pRow->mib.match.dstL4Port.start)
            return L7_ERROR;
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END;
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTMAC_ADDR:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTMAC_MASK:
    {
      /* NOTE: Any numeric value is allowed. */
      *pValueLen = sizeof(L7_MAC_ADDR_LEN);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC)
        return L7_FAILURE;

      if (oid == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTMAC_ADDR)
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR;
      else
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ETYPE_KEYID:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < (L7_uint32)L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPEKEYID_MIN) || 
          (val > (L7_uint32)L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPEKEYID_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE)
        return L7_FAILURE;

      flag = DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_KEYID;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ETYPE_VALUE:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MIN) || 
          (val > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_ETYPE_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE)
        return L7_FAILURE;

      /* value requires Etype 'custom' keyword be already set */
      if ((DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_KEYID)) ||
          (pRow->mib.match.etype.keyid != L7_QOS_ETYPE_KEYID_CUSTOM))
        return L7_ERROR;

      flag = DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_VALUE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_FLOWLBLV6:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_FLOWLBLV6_MIN) || 
          (val > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_FLOWLBLV6_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6)
        return L7_FAILURE;

      flag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6FLOWLBL;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPDSCP:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPDSCP_MIN) || 
          (val > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPDSCP_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP)
        return L7_FAILURE;

      flag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPPRECEDENCE:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPPRECEDENCE_MIN) || 
          (val > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPPRECEDENCE_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != 
          L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE)
        return L7_FAILURE;

      flag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPTOS_BITS:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPTOS_MASK:
    {
      /* NOTE: Both Bits and Mask values are of type L7_uchar8. */
      val = (L7_uint32)(*(L7_uchar8 *)pValue);
      *pValueLen = sizeof(L7_uchar8);

      if ((val < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPTOS_MIN) || 
          (val > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_IPTOS_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != 
          L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS)
        return L7_FAILURE;

      if (oid == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPTOS_BITS)
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS;
      else
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_PROTOCOL_NUM:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_PROTOCOL_MIN) || 
          (val > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_PROTOCOL_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != 
          L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL)
        return L7_FAILURE;

      flag = DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_REFCLASS_INDEX:
    {
      dsmibClassEntryCtrl_t *pRef;
      L7_RC_t               rc;

      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS)
        return L7_FAILURE;

      /* Special Case:  if removing a reference class, make sure the request
       *                is legitimate (none of the other checking is needed)
       */
      if (val == (L7_uint32)L7_USMDB_MIB_DIFFSERV_CLASS_RULE_REFCLASS_NONE)
      {
        /* reject removal if the field has not been set or there is no 
         * existing class reference
         */
        if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX))
          return L7_ERROR;
        if (pRow->mib.match.refClassIndex == val)
          return L7_ERROR;

        break;                                  /* exit switch stmt */
      }

      /* check reference class index */
      if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(val))
        return L7_FAILURE;

      /* cannot reference own class */
      if (val == classIndex)
        return L7_ERROR;

      /* make sure a 'refclass' rule is not already defined for the class */
      if (pClass->refClassIndex != DSMIB_CLASS_REF_INDEX_NONE)
        return L7_FAILURE;

      /* make sure reference class exists */
      pRef = dsmibClassRowFind(val);
      if (pRef == L7_NULLPTR)
        return L7_FAILURE;

      /* both class types must match (class type 'acl' not allowed for either)*/
      if (pClass->mib.type != pRef->mib.type)
        return L7_ERROR;
      if (pClass->mib.type == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
        return L7_ERROR;

      /* both class l3proto settings must be consistent */
      if (pClass->mib.l3Protocol != pRef->mib.l3Protocol)
        return L7_ERROR;

      /* don't allow sequential class references to form a loop */
      while (pRef->refClassIndex != DSMIB_CLASS_REF_INDEX_NONE)
      {
        /* reject if any class in the reference chain references this class */
        if (pRef->refClassIndex == classIndex)
          return L7_FAILURE;

        pRef = dsmibClassRowFind(pRef->refClassIndex);
        if (pRef == L7_NULLPTR)
          return L7_FAILURE;
      }

      /* check that the total number of rules for all nested class 
       * references does not exceed a maximum limit
       *
       * NOTE:  Need to check this again here, since the check for the
       *        entry type could not cover the new reference class.
       *
       * NOTE:  This requires that we TEMPORARILY set the refClassIndex 
       *        of the set-test class, then put it back to 'none' afterward.
       */

      pClass->refClassIndex = val;              /* TEMPORARY!!! */

      rc = dsmibClassRuleRefLimitCheck();
      if (rc == L7_SUCCESS)
      {
        /* check for restrictions on multiple rules with same entry type */
        if (dsmibClassRuleIsRuleAllowed(classIndex, classRuleIndex) != L7_TRUE)
        {
          /* just set rc here...don't exit until after refClassIndex restored */
          rc = L7_FAILURE;
        }

        /* check for restrictions imposed by policies using this rule
         * NOTE: do this here while the refClassIndex is still set favorably
         */
        else if (dsmibClassRuleInstanceValidate(pRow) != L7_SUCCESS)
        {
          /* just set rc here...don't exit until after refClassIndex restored */
          rc = L7_ERROR;
        }
      }

      pClass->refClassIndex = DSMIB_CLASS_REF_INDEX_NONE; /* RESTORE as it was*/

      if (rc != L7_SUCCESS)
        return rc;

      flag = DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIP_ADDR:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIP_MASK:
    {
      /* NOTE: Any numeric value is allowed. */
      *pValueLen = sizeof(L7_uint32);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP)
        return L7_FAILURE;

      if (oid == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIP_ADDR)
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR;
      else
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIPV6_ADDR:
    {
      *pValueLen = sizeof(L7_in6_addr_t);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6)
        return L7_FAILURE;

      flag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6ADDR;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIPV6_ADDR_PLEN:
    {
      *pValueLen = sizeof(L7_uint32);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6)
        return L7_FAILURE;

      flag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6PLEN;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCL4PORT_START:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCL4PORT_END:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MIN) || 
          (val > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_L4PORT_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != 
          L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT)
        return L7_FAILURE;

      if (oid == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCL4PORT_START)
      {
        /* starting value cannot be greater than ending value (if set) */
        if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END))
          if (val > pRow->mib.match.srcL4Port.end)
            return L7_ERROR;
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START;
      }
      else
      {
        /* ending value cannot be less than starting value (if set) */
        if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START))
          if (val < pRow->mib.match.srcL4Port.start)
            return L7_ERROR;
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END;
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCMAC_ADDR:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCMAC_MASK:
    {
      /* NOTE: Any numeric value is allowed. */
      *pValueLen = sizeof(L7_MAC_ADDR_LEN);

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC)
        return L7_FAILURE;

      if (oid == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCMAC_ADDR)
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR;
      else
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID_START:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID_END:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) || 
          (val > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID)
        return L7_FAILURE;

      if (oid == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID_START)
      {
        /* start VLAN ID cannot be greater than end VLAN ID (if set) */
        if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND))
          if (val > pRow->mib.match.vlanId.end)
            return L7_ERROR;
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART;
      }
      else
      {
        /* end VLAN ID cannot be less than start VALN ID (if set) */
        if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART))
          if (val < pRow->mib.match.vlanId.start)
            return L7_ERROR;
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND;
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID2_START:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID2_END:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      if ((val < L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MIN) || 
          (val > L7_USMDB_MIB_DIFFSERV_CLASS_RULE_VLANID_MAX))
        return L7_ERROR;

      /* only valid if appropriate entry type is already set */
      if (pRow->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2)
        return L7_FAILURE;

      if (oid == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID2_START)
      {
        /* starting value cannot be greater than ending value (if set) */
        if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND))
          if (val > pRow->mib.match.vlanId2.end)
            return L7_ERROR;
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART;
      }
      else
      {
        /* end value cannot be less than starting value (if set) */
        if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART))
          if (val < pRow->mib.match.vlanId2.start)
            return L7_ERROR;
        flag = DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND;
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_EXCLUDE_FLAG:
    {
      dsmibTruthValue_t excludeFlag = *(dsmibTruthValue_t *)pValue;
      *pValueLen = sizeof(excludeFlag);

      if ((excludeFlag != L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE) &&
          (excludeFlag != L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE))
        return L7_ERROR;

      /* only valid if the entry type field is already set */
      if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE))
        return L7_FAILURE;

      /* only check if the exclude flag feature is supported when attempting
       * to set the flag value to 'true'
       *
       * NOTE: Recall that the exclude flag must always be set to true or 
       *       false for every class rule row.  If this feature is not 
       *       supported by the platform, then the only legitimate setting
       *       is 'false'.
       */
      if (excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE)
        if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, 
                                  L7_DIFFSERV_CLASS_MATCH_EXCLUDE_FEATURE_ID)
              != L7_TRUE)
          return L7_NOT_SUPPORTED;

      /* only 'false' allowed for a 'refclass' entry type */
      if (pRow->mib.entryType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS)
        if (excludeFlag != L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE)
          return L7_FAILURE;

      /* once set, this value is not allowed to change */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG))
        return L7_FAILURE;

      /* check for restrictions on multiple rules with same entry type */
      if (excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE)
      {
        if (dsmibClassRuleIsRuleAllowed(classIndex, classRuleIndex) != L7_TRUE)
          return L7_FAILURE;
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_STORAGE_TYPE:
    {
      dsmibStorageType_t  storageType = *(dsmibStorageType_t *)pValue;
      *pValueLen = sizeof(storageType);

      if (dsmibStorageTypeIsValid(storageType) != L7_TRUE)
        return L7_ERROR;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ROW_STATUS:
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
          /* never allow a class type 'acl' rule to be destroyed */
          if (pClass->mib.type == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
            return L7_FAILURE;

          /* only the most recent rule row in the class may be destroyed */
          if (diffServClassRuleIsMostRecent(classIndex, classRuleIndex) != L7_TRUE)
            return L7_FAILURE;
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

  /* follow-up checking specifically for class rule match criterion objects,
   * as indicated by a nonzero flag variable at this point
   */
  if (flag != 0)
  {
    /* all match criteria objects rely on the following set order sequence:
     * 1) entry type, 2) exclude flag, 3) individual match object(s)
     *
     * recall, the row invalid flag bit ON indicates the corresponding object
     * field is NOT set
     */
    if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG))
      return L7_FAILURE;

    /* once set, a match value is not allowed to change for a given class rule*/
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, flag))
      return L7_FAILURE;

  } /* endif flag != 0 */

  /* NOTE: Put any common checking here if value depends on row status state.*/

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the specified object from the Class Rule Table
*
* @param    classIndex      @b{(input)}  Class index
* @param    classRuleIndex  @b{(input)}  Class rule index
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(output)} Pointer to the object output value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value not yet established
* @returns  L7_FAILURE  Improper usage or other malfunction
*
* @notes    An object can only be provided if the request matches the
*           class rule entry type and if the object's value has been
*           instantiated.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleObjectGet(L7_uint32 classIndex, 
                                   L7_uint32 classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_TABLE_OBJECT_t oid,
                                   void *pValue)
{
  dsmibClassRuleEntryCtrl_t *pRow;
  L7_RC_t                   rc;
  L7_uint32                 *p;
  L7_uchar8                 *p8, *pMac;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;
  if (DSMIB_CLASS_RULE_INDEX_OUT_OF_RANGE(classRuleIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibClassRuleRowFind(classIndex, classRuleIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  /* check for proper object type and row invalid flag state before proceeding*/
  if ((rc = dsmibClassRuleObjectGetCheck(pRow, oid)) != L7_SUCCESS)
    return rc;

  /* output the current value of the object */
  switch (oid)
  {
  case L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE:
    {
      dsmibClassRuleType_t  *pType = pValue;
      *pType = pRow->mib.entryType;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_COS:
    {
      p = pValue;
      *p = pRow->mib.match.cos;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_COS2:
    {
      p = pValue;
      *p = pRow->mib.match.cos2;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTIP_ADDR:
    {
      p = pValue;
      *p = pRow->mib.match.dstIp.addr;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTIP_MASK:
    {
      p = pValue;
      *p = pRow->mib.match.dstIp.mask;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTIPV6_ADDR:
    {
      memcpy(pValue, &pRow->mib.match.dstIpv6.ip6prefix.in6Addr, 
             sizeof(pRow->mib.match.dstIpv6.ip6prefix.in6Addr));
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTIPV6_PLEN:
    {
      p = pValue;
      *p = pRow->mib.match.dstIpv6.ip6prefix.in6PrefixLen;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTL4PORT_START:
    {
      p = pValue;
      *p = pRow->mib.match.dstL4Port.start;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTL4PORT_END:
    {
      p = pValue;
      *p = pRow->mib.match.dstL4Port.end;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTMAC_ADDR:
    {
      pMac = pValue;
      memcpy(pMac, pRow->mib.match.dstMac.addr, L7_MAC_ADDR_LEN);
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTMAC_MASK:
    {
      pMac = pValue;
      memcpy(pMac, pRow->mib.match.dstMac.mask, L7_MAC_ADDR_LEN);
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_ETYPE_KEYID:
    {
      p = pValue;
      *p = pRow->mib.match.etype.keyid;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_ETYPE_VALUE:
    {
      p = pValue;
      *p = pRow->mib.match.etype.value;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_EVERY:
    {
      dsmibTruthValue_t *pEvery = pValue;
      *pEvery = pRow->mib.match.matchEvery;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_IPV6_FLOWLBL:
    {
      p = pValue;
      *p = pRow->mib.match.ipv6FlowLabel;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_IPDSCP:
    {
      p = pValue;
      *p = pRow->mib.match.ipDscp;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_IPPRECEDENCE:
    {
      p = pValue;
      *p = pRow->mib.match.ipPrecedence;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_IPTOS_BITS:
    {
      p8 = pValue;
      *p8 = pRow->mib.match.ipTos.bits;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_IPTOS_MASK:
    {
      p8 = pValue;
      *p8 = pRow->mib.match.ipTos.mask;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_PROTOCOL_NUM:
    {
      p = pValue;
      *p = pRow->mib.match.protocolNum;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_REFCLASS_INDEX:
    {
      p = pValue;
      *p = pRow->mib.match.refClassIndex;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCIP_ADDR:
    {
      p = pValue;
      *p = pRow->mib.match.srcIp.addr;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCIP_MASK:
    {
      p = pValue;
      *p = pRow->mib.match.srcIp.mask;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCIPV6_ADDR:
    {
      memcpy(pValue, &pRow->mib.match.srcIpv6.ip6prefix.in6Addr, 
             sizeof(pRow->mib.match.srcIpv6.ip6prefix.in6Addr));
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCIPV6_PLEN:
    {
      p = pValue;
      *p = pRow->mib.match.srcIpv6.ip6prefix.in6PrefixLen;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCL4PORT_START:
    {
      p = pValue;
      *p = pRow->mib.match.srcL4Port.start;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCL4PORT_END:
    {
      p = pValue;
      *p = pRow->mib.match.srcL4Port.end;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCMAC_ADDR:
    {
      pMac = pValue;
      memcpy(pMac, pRow->mib.match.srcMac.addr, L7_MAC_ADDR_LEN);
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCMAC_MASK:
    {
      pMac = pValue;
      memcpy(pMac, pRow->mib.match.srcMac.mask, L7_MAC_ADDR_LEN);
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_VLANID_START:
    {
      p = pValue;
      *p = pRow->mib.match.vlanId.start;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_VLANID_END:
    {
      p = pValue;
      *p = pRow->mib.match.vlanId.end;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_VLANID2_START:
    {
      p = pValue;
      *p = pRow->mib.match.vlanId2.start;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_VLANID2_END:
    {
      p = pValue;
      *p = pRow->mib.match.vlanId2.end;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_EXCLUDE_FLAG:
    {
      dsmibTruthValue_t *pExcludeFlag = pValue;
      *pExcludeFlag = pRow->mib.excludeFlag;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_STORAGE_TYPE:
    {
      dsmibStorageType_t  *pStorageType = pValue;
      *pStorageType = pRow->mib.storageType;
    }
    break;

  case L7_DIFFSERV_CLASS_RULE_ROW_STATUS: 
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
* @purpose  Set the specified object in the Class Rule Table
*
* @param    classIndex      @b{(input)}  Class index
* @param    classRuleIndex  @b{(input)}  Class rule index
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(output)} Pointer to the object set value
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
L7_RC_t diffServClassRuleObjectSet(L7_uint32 classIndex, 
                                   L7_uint32 classRuleIndex,
                                   L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_TYPE_t oid,
                                   void *pValue)
{
  dsmibTableId_t            tableId = DSMIB_TABLE_ID_CLASS_RULE;
  dsmibClassRuleEntryCtrl_t *pRowTrace = L7_NULLPTR;
  size_t                    dsTraceValueLen = 0;
  L7_uchar8                 dsTraceValueData[DS_TRACE_OBJ_VAL_LEN_MAX];
  L7_RC_t                   rc;

  rc = dsmibClassRuleObjectSet(classIndex, classRuleIndex, oid, pValue,
                               &pRowTrace, &dsTraceValueLen);

  /* format the OID value for the trace point */
  dsTraceObjectValFormat(pValue, dsTraceValueLen, dsTraceValueData);

  DS_TRACE_PT_OBJECT_SET(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                         classIndex, classRuleIndex, 0,
                         (L7_ulong32)pRowTrace, (L7_uchar8)rc,
                         (L7_uchar8)oid, (L7_uchar8)dsTraceValueLen,
                         dsTraceValueData);

  return rc;
}

/*********************************************************************
* @purpose  Set the specified object in the Class Rule Table
*
* @param    classIndex      @b{(input)}  Class index
* @param    classRuleIndex  @b{(input)}  Class rule index
* @param    oid             @b{(input)}  Object identifier
* @param    pValue          @b{(output)} Pointer to the object set value
* @param    ppRow           @b{(output)} Pointer to row ptr output location
* @param    pValueLen       @b{(output)} Pointer to value length output location
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not allowed for current entry type
* @returns  L7_FAILURE  
*
* @notes    A *pointer* to the set value is used here to allow for objects
*           of different types.  This means all character array/string objects
*           are passed in as L7_uchar8**.
*
* @notes    THE REQUIRED 'SET' ORDER FOR A ROW IN THIS TABLE IS:  1) entry
*           type, 2) exclude flag, 3) individual match object(s).
*
* @notes    The ppRow and pValueLen are used for tracing.
*
* @end
*********************************************************************/
L7_RC_t dsmibClassRuleObjectSet(L7_uint32 classIndex, 
                                L7_uint32 classRuleIndex,
                                L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_TYPE_t oid,
                                void *pValue,
                                dsmibClassRuleEntryCtrl_t **ppRow,
                                size_t *pValueLen)
{
  dsmibClassEntryCtrl_t     *pRefClass = L7_NULLPTR;
  L7_uint32                 origRefClassIndex = DSMIB_CLASS_REF_INDEX_NONE;
  dsmibClassRuleEntryCtrl_t *pRow, origRow;
  dsmibRowStatus_t          restoreStatus, rowStatusRaw, oldStatus, newStatus;
  L7_uint32                 val;
  L7_uchar8                 val8, *pMac;
  L7_RC_t                   rc;
  L7_BOOL                   justCompleted;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;
  if (DSMIB_CLASS_RULE_INDEX_OUT_OF_RANGE(classRuleIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* reuse 'SetTest' function to check for valid oid and settable value */
  if (diffServClassRuleSetTest(classIndex, classRuleIndex, oid, pValue) 
        != L7_SUCCESS)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibClassRuleRowFind(classIndex, classRuleIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  *ppRow = pRow;

  /* save the original contents of the class rule row in its entirety
   * in case an 'undo' of the set value is needed should the value
   * cause a conflict
   */
  memcpy(&origRow, pRow, sizeof(origRow));

  /* save a copy of the old row status for a comparison below */
  restoreStatus = rowStatusRaw = pRow->mib.rowStatus;
  if (dsmibRowStatusObservable(rowStatusRaw, (L7_uint32)pRow->rowInvalidFlags, 
                               &oldStatus) != L7_SUCCESS)
    return L7_FAILURE;

  if (oid != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ROW_STATUS)
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

      rc = dsmibRowEventCheck(DSMIB_TABLE_ID_CLASS_RULE, pRow, 
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
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ENTRY_TYPE:
    {
      dsmibClassRuleType_t  entryType = *(dsmibClassRuleType_t *)pValue;
      dsmibClassRuleType_t  savedEntryType;
      dsmibClassRuleFlags_t savedRowInvalidFlags;

      *pValueLen = sizeof(entryType);

      /* save the original value of the entry type and row invalid flags that
       * are about to be modified
       */
      savedEntryType = pRow->mib.entryType;
      savedRowInvalidFlags = pRow->rowInvalidFlags;

      /* need to replace the set of row invalid flags here based on the 
       * specific entry type (also causes entry type flag to be reset)
       */
      if ((rc = dsmibClassRuleRowFlagsReplace(pRow, entryType)) != L7_SUCCESS)
        return rc;

      pRow->mib.entryType = entryType;

      /* now that the entry type field is set, validate it for all policy 
       * instances using this class rule
       */
      if (dsmibClassRuleInstanceValidate(pRow) != L7_SUCCESS)
      {
        /* restore entry type and row invalid flags to the way they were */
        pRow->mib.entryType = savedEntryType;
        pRow->rowInvalidFlags = savedRowInvalidFlags;

        return L7_FAILURE;              /* same rc as a setTest failure */
      }

      /* also need to set up the read-only MIB object field for the 
       * 'match every' rule type
       */
      if (entryType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY)
        pRow->mib.match.matchEvery = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE;

    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_COS:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.cos = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_COS);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_COS2:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.cos2 = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_COS);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIP_ADDR:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.dstIp.addr = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIP_MASK:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.dstIp.mask = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIPV6_ADDR:
    {
      *pValueLen = sizeof(L7_in6_addr_t);
      memcpy(&pRow->mib.match.dstIpv6.ip6prefix.in6Addr, pValue, *pValueLen);
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6ADDR);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIPV6_ADDR_PLEN:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.dstIpv6.ip6prefix.in6PrefixLen = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6PLEN);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTL4PORT_START:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.dstL4Port.start = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTL4PORT_END:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.dstL4Port.end = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTMAC_ADDR:
    {
      pMac = (L7_uchar8 *)pValue;
      *pValueLen = sizeof(L7_MAC_ADDR_LEN);
      memcpy(pRow->mib.match.dstMac.addr, pMac, L7_MAC_ADDR_LEN);
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTMAC_MASK:
    {
      pMac = (L7_uchar8 *)pValue;
      *pValueLen = sizeof(L7_MAC_ADDR_LEN);
      memcpy(pRow->mib.match.dstMac.mask, pMac, L7_MAC_ADDR_LEN);
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ETYPE_KEYID:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.etype.keyid = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_KEYID);
      /* do not allow an etype value to be set if this is a non-custom keyid */
      if (pRow->mib.match.etype.keyid != L7_QOS_ETYPE_KEYID_CUSTOM)
        DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_VALUE);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ETYPE_VALUE:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.etype.value = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_VALUE);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_FLOWLBLV6:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.ipv6FlowLabel = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6FLOWLBL);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPDSCP:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.ipDscp = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPPRECEDENCE:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.ipPrecedence = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPTOS_BITS:
    {
      val8 = *(L7_uchar8 *)pValue;
      *pValueLen = sizeof(val8);
      pRow->mib.match.ipTos.bits = val8;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPTOS_MASK:
    {
      val8 = *(L7_uchar8 *)pValue;
      *pValueLen = sizeof(val8);
      pRow->mib.match.ipTos.mask = val8;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_PROTOCOL_NUM:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.protocolNum = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_REFCLASS_INDEX:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);

      /* save existing reference class index in case restore needed below */
      origRefClassIndex = pRow->pClass->refClassIndex;

      /* update the reference count in the referenced class
       * save reference class index to allow only one reference rule per class
       */
      if (val != (L7_uint32)L7_USMDB_MIB_DIFFSERV_CLASS_RULE_REFCLASS_NONE)
      {
        pRefClass = dsmibClassRowFind(val);
        if (pRefClass == L7_NULLPTR)
          return L7_FAILURE;
        pRefClass->refCount++;                  /* count this class reference */
        pRow->pClass->refClassIndex = val;
      }
      else
      {
        pRefClass = dsmibClassRowFind(pRow->mib.match.refClassIndex);
        if (pRefClass == L7_NULLPTR)
          return L7_FAILURE;
        pRefClass->refCount--;                  /* remove class reference     */
        pRow->pClass->refClassIndex = DSMIB_CLASS_REF_INDEX_NONE;
      }

      pRow->mib.match.refClassIndex = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIP_ADDR:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.srcIp.addr = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIP_MASK:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.srcIp.mask = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIPV6_ADDR:
    {
      *pValueLen = sizeof(L7_in6_addr_t);
      memcpy(&pRow->mib.match.srcIpv6.ip6prefix.in6Addr, pValue, *pValueLen);
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6ADDR);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIPV6_ADDR_PLEN:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.srcIpv6.ip6prefix.in6PrefixLen = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6PLEN);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCL4PORT_START:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.srcL4Port.start = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCL4PORT_END:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.srcL4Port.end = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCMAC_ADDR:
    {
      pMac = (L7_uchar8 *)pValue;
      *pValueLen = sizeof(L7_MAC_ADDR_LEN);
      memcpy(pRow->mib.match.srcMac.addr, pMac, L7_MAC_ADDR_LEN);
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCMAC_MASK:
    {
      pMac = (L7_uchar8 *)pValue;
      *pValueLen = sizeof(L7_MAC_ADDR_LEN);
      memcpy(pRow->mib.match.srcMac.mask, pMac, L7_MAC_ADDR_LEN);
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID_START:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.vlanId.start = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID_END:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.vlanId.end = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID2_START:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.vlanId2.start = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID2_END:
    {
      val = *(L7_uint32 *)pValue;
      *pValueLen = sizeof(val);
      pRow->mib.match.vlanId2.end = val;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_EXCLUDE_FLAG:
    {
      dsmibTruthValue_t excludeFlag = *(dsmibTruthValue_t *)pValue;
      *pValueLen = sizeof(excludeFlag);
      pRow->mib.excludeFlag = excludeFlag;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_STORAGE_TYPE:
    {
      dsmibStorageType_t storageType = *(dsmibStorageType_t *)pValue;
      *pValueLen = sizeof(storageType);
      pRow->mib.storageType = storageType;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ROW_STATUS:
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
  if (oid != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ROW_STATUS)
  {
    /* allow for possible update of row status due to row completion */
    dsmibRowCompleteCheck(pRow->mib.rowStatus, pRow->rowInvalidFlags, 
                          &rowStatusRaw, &justCompleted);

    if (justCompleted == L7_TRUE)
    {
      /* check for a duplicate object value for the completed row against
       * all other class rule rows in every reference class chain that uses
       * this class (this also checks against the other rules in the same 
       * class)
       */
      if (dsmibClassRuleIsValueAllowed(classIndex, classRuleIndex) != L7_TRUE)
      {
        /* a class rule object value conflict exists, so restore the entire
         * row to its original value at entry to this function as if this
         * 'set' operation had never occurred
         *
         * NOTE: Must also undo the updates made when setting a refclass index.
         */

        if (oid == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_REFCLASS_INDEX)
        {
          if (pRefClass != L7_NULLPTR)
          {
            pRow->pClass->refClassIndex = origRefClassIndex;

            /* adjustment depends on whether we attempted a ref class add or
             * remove operation
             */
            if (pRow->mib.match.refClassIndex != 
                (L7_uint32)L7_USMDB_MIB_DIFFSERV_CLASS_RULE_REFCLASS_NONE)
              pRefClass->refCount--;
            else
              pRefClass->refCount++;
          }
        }

        memcpy(pRow, &origRow, sizeof(*pRow));

        return L7_FAILURE;
      }

      /* the rowPending flag must be cleared the first time a row is completed
       * (no harm in clearing it again for subsequent passes through this
       * code path)
       */
      pRow->rowPending = L7_FALSE;

      DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);
      
      /* successful row completion -- update next free index value
       * NOTE: Must do this under index semaphore control.
       */
      dsmibClassRuleIndexNextUpdate(classIndex);

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
    if (dsmibRowEventCheck(DSMIB_TABLE_ID_CLASS_RULE, pRow, oldStatus, newStatus)
          != L7_SUCCESS)
      return L7_FAILURE;

    if (pRow->mib.rowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY)
    {
      /* for a new row status of 'destroy', it is now time to delete the row */
      if (diffServClassRuleDelete(classIndex, classRuleIndex) != L7_SUCCESS)
      {
        pRow->mib.rowStatus = restoreStatus;
        LOG_MSG("diffServClassRuleObjectSet: attempt to destroy the row failed\n");
        return L7_FAILURE;
      }
    }
    else
    {
      /* send callbacks for all policies that reference the affected class */
      dsmibClassRulePolicyChangeNotify(classIndex);
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the ACL rule identifier, if any, for the specified entry
*           in the Class Rule Table
*
* @param    classIndex      @b{(input)}  Class index
* @param    classRuleIndex  @b{(input)}  Class rule index
* @param    pAclRuleId      @b{(output)} Pointer to ACL rule ID output value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Not an 'acl' class rule
* @returns  L7_FAILURE  Improper usage or other malfunction
*
* @notes    This is a supplemental function provided to assist the user
*           interface code with correlating the specified class rule to 
*           the access list rule from which it was derived.  This is only
*           meaningful for a class type 'acl'.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleAclRuleIdGet(L7_uint32 classIndex, 
                                      L7_uint32 classRuleIndex,
                                      L7_uint32 *pAclRuleId)
{
  dsmibClassRuleEntryCtrl_t *pRow;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;
  if (DSMIB_CLASS_RULE_INDEX_OUT_OF_RANGE(classRuleIndex))
    return L7_FAILURE;
  if (pAclRuleId == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the class rule row */
  pRow = dsmibClassRuleRowFind(classIndex, classRuleIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  if ((pRow->pClass->mib.type != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL) ||
      (pRow->arid == DSMIB_CLASS_RULE_ARID_NONE))
    return L7_ERROR;

  /* output ACL rule ID value */
  *pAclRuleId = pRow->arid;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the Class Rule index of the Reference Class Rule for
*           the specified Class
*
* @param    classIndex       @b{(input)} Class index
* @param    pClassRuleIndex  @b{(output)} Pointer to Class Rule index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE  Specified Class has no Reference Class attached
*
* @notes    The class rule index can only be provided if the request
*           matches the class rule entry type of:  _REFCLASS.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleRefClassRuleIndexFind(L7_uint32 classIndex,
                                               L7_uint32 *pClassRuleIndex)
{
  dsmibClassRuleEntryCtrl_t *pRow;
  L7_uint32                 nextClassIndex;
  L7_uint32                 classRuleIndex;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;
  if (pClassRuleIndex == L7_NULLPTR)
    return L7_FAILURE;

  /* get the reference class rule index */
  classRuleIndex = 0;
  while ((diffServClassRuleGetNext(classIndex, classRuleIndex,
                       &nextClassIndex, &classRuleIndex) == L7_SUCCESS) &&
         (nextClassIndex == classIndex))
  {
    pRow = dsmibClassRuleRowFind(classIndex, classRuleIndex);
    if (pRow->mib.entryType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS)
    {
      if (pRow->mib.match.refClassIndex != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_REFCLASS_NONE)
      {
        *pClassRuleIndex = classRuleIndex;
        return L7_SUCCESS;
      }
    }
  }

  *pClassRuleIndex = 0;
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Remove the referenced class index value from the specified
*           class
*
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR  Not allowed for current class rule entry type
* @returns  L7_FAILURE  
*
* @notes    Usage of this function is only allowed for a match entry type
*           of:  _REFCLASS.
*
* @notes    The actual class rule row will not be deleted, but the
*           reference class rule index value will be set to 0 (zero)
*           so that the user knows the rule is not available for re-use.
*           An exception to this is if the reference class rule is the
*           most recently created in the class, it is deleted so that
*           it may be reused.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRuleRefClassIndexRemove(L7_uint32 classIndex,
                                             L7_uint32 classRuleIndex)
{
  L7_RC_t       rc;
  L7_uint32     val;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;
  if (DSMIB_CLASS_RULE_INDEX_OUT_OF_RANGE(classRuleIndex))
    return L7_FAILURE;

  /* if this is the most recent rule created for this class, delete it 
   * in order to reclaim the row for future use
   *
   * otherwise, need to leave the class rule row in place and set the    
   * reference class index to 'none' (so that the user will know the
   * rule is no longer available for the class) -- the ObjectSet function
   * handles the class rule row state transitions so that the appropriate
   * communications are made to the device driver
   * 
   * NOTE: If not for the fact of giving the user some indication why the
   *       class fills up faster than expected, we could have simply deleted
   *       the refclass rule here for all cases and everything would have
   *       been much cleaner.  The class would not be able to reuse rules
   *       deleted (except for the most recent rule), but it could handle 
   *       the resultant classRuleIndex numbering gaps.
   */
  if (diffServClassRuleIsMostRecent(classIndex, classRuleIndex) == L7_TRUE)
  {
    rc = diffServClassRuleDelete(classIndex, classRuleIndex);
  }

  else
  {
    val = (L7_uint32)L7_USMDB_MIB_DIFFSERV_CLASS_RULE_REFCLASS_NONE;
    rc = diffServClassRuleObjectSet(classIndex, classRuleIndex, 
                    L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_REFCLASS_INDEX,
                                    &val);
  }

  return rc;
}

/*********************************************************************
* @purpose  Determine if the class rule is the most recently created  
*           rule in the class (i.e., is last one in current list)  
*
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
*
* @returns  L7_TRUE     Class rule is most recent
* @returns  L7_FALSE    Class rule not most recent
*
* @notes
*       
* @end
*********************************************************************/
L7_BOOL diffServClassRuleIsMostRecent(L7_uint32 classIndex, 
                                      L7_uint32 classRuleIndex)
{
  L7_RC_t       rc;
  L7_uint32     nextClass, nextRule;

  /* NOTE: Assumes a 'GetNext' L7_ERROR means the end of table was reached,
   *       which implies this is the last class rule in the table and 
   *       therefore must also be the most recent rule in its class.
   */
  rc = diffServClassRuleGetNext(classIndex, classRuleIndex, 
                                &nextClass, &nextRule);

  if ((rc == L7_SUCCESS) && (nextClass != classIndex))
    return L7_TRUE;

  if (rc == L7_ERROR)
    return L7_TRUE;

  return L7_FALSE;
}


/*
=======================================
=======================================
=======================================

   CLASS RULE TABLE HELPER FUNCTIONS

=======================================
=======================================
=======================================
*/

/*********************************************************************
* @purpose  Determine next sequential row in the Class Rule Table taking
*           into account the reference class chain.
*
* @param    prevClassIndex      @b{(input)}  Class index to begin search
* @param    prevClassRuleIndex  @b{(input)}  Class rule index to begin search
* @param    pClassIndex         @b{(output)} Pointer to next sequential class
*                                              index value
* @param    pClassRuleIndex     @b{(output)} Pointer to next sequential class
*                                              rule index value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of reference chain
* @returns  L7_FAILURE  
*
* @notes    A prevClassIndex of 0 is used to find the first class in the table.
*
* @notes    A prevClassRuleIndex of 0 is used to find the first rule relative
*           to the specified prevClassIndex.
*
* @notes    This is similar to diffServClassRuleGetNext, except when the rules
*           for a class are exhausted, the search continues with the next
*           class in a reference class chain, if any.
*
* @notes    DOES NOT PRESERVE ACTUAL RULE EVALUATION ORDERING.  The rules for
*           a subsequent class in the chain are presented after the end of
*           the current set of class rules is reached (no recursion at the
*           point where the refclass rule entry type is located).
*
* @end
*********************************************************************/
L7_RC_t dsmibClassRuleRefChainGetNext(L7_uint32 prevClassIndex, 
                                      L7_uint32 prevClassRuleIndex,
                                      L7_uint32 *pClassIndex,
                                      L7_uint32 *pClassRuleIndex)
{
  L7_RC_t                   rc = L7_ERROR;
  dsmibTableId_t            tableId = DSMIB_TABLE_ID_CLASS_RULE;
  dsmibClassRuleKey_t       keys;
  dsmibClassRuleEntryCtrl_t *pRow;
  L7_RC_t                   tempRc;

  /* find node in AVL tree
   * NOTE: Need to use direct call here due to AVL_NEXT flag.
   */
  keys.classIndex = prevClassIndex;
  keys.classRuleIndex = prevClassRuleIndex;

  do
  {
    pRow = (dsmibClassRuleEntryCtrl_t *)
           dsmibRowFind(&dsmibAvlTree[tableId], &keys, AVL_NEXT);

    if (pRow != L7_NULLPTR)
    {
      if ((pRow->key.classIndex == prevClassIndex) ||
          (prevClassIndex == (L7_uint32)DSMIB_CLASS_REF_INDEX_NONE))
      {
        /* found next sequential table row -- set caller's output variables */
        *pClassIndex = pRow->key.classIndex;
        *pClassRuleIndex = pRow->key.classRuleIndex;

        rc = L7_SUCCESS;
        break;
      }
    }

    /* end of table reached or classIndex changed, so try the next class in 
     * the reference chain
     *
     * loop terminates here when no more reference classes exist in the chain
     */
    if (prevClassIndex == (L7_uint32)DSMIB_CLASS_REF_INDEX_NONE)
      break;
    if ((tempRc = diffServClassToRefClass(prevClassIndex, &prevClassIndex))
          != L7_SUCCESS)
    {
      rc = tempRc;
      break;
    }

    keys.classIndex = prevClassIndex;
    keys.classRuleIndex = 0;                    /* find first rule in class */

  } while (prevClassIndex != (L7_uint32)DSMIB_CLASS_REF_INDEX_NONE);

  return rc;
}

/*********************************************************************
* @purpose  Provide the current value of the class rule index next variable
*
* @param    classIndex      @b{(input)}  Class index
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
L7_RC_t dsmibClassRuleIndexNextRead(L7_uint32 classIndex, L7_uint32 *pNextFree)
{
  dsmibClassEntryCtrl_t *pRow;

  /* get access to the class row */
  pRow = dsmibClassRowFind(classIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  *pNextFree = pRow->mib.ruleIndexNextFree;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine next available class rule index value and update its value
*
* @param    classIndex      @b{(input)}  Class index
*
* @returns  void  
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
void dsmibClassRuleIndexNextUpdate(L7_uint32 classIndex)
{
  dsmibClassEntryCtrl_t *pRow;
  L7_uint32             indexNext, i;

  indexNext = DSMIB_AUTO_INCR_INDEX_NEXT_START;
  for (i = 1; i <= DSMIB_CLASS_RULE_INDEX_MAX; i++)
  {
    if (dsmibClassRuleRowFind(classIndex, i) != L7_NULLPTR)
      indexNext = i + 1;
  }

  if (indexNext > DSMIB_CLASS_RULE_INDEX_MAX)
    indexNext = 0;

  /* get access to the class row */
  pRow = dsmibClassRowFind(classIndex);
  if (pRow == L7_NULLPTR)
  {
    LOG_MSG("dsmibClassRuleIndexNextUpdate: ERROR -- could not find "
            "class row for index %u\n", classIndex);
    return;                                     /* ERROR -- should not occur */
  }

  pRow->mib.ruleIndexNextFree = indexNext;
}

/*********************************************************************
* @purpose  Undo next available class rule index value for a deleted row   
*
* @param    classIndex      @b{(input)}  Class index
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
void dsmibClassRuleIndexNextUndo(L7_uint32 classIndex)
{
  dsmibClassEntryCtrl_t *pRow;
  L7_uint32             indexNext;

  /* get access to the class row */
  pRow = dsmibClassRowFind(classIndex);
  if (pRow == L7_NULLPTR)
  {  
    LOG_MSG("dsmibClassRuleIndexNextUndo: ERROR -- could not find "
            "class row for index %u\n", classIndex);
    return;                                     /* ERROR -- should not occur */
  }

  indexNext = pRow->mib.ruleIndexNextFree;

  if (indexNext == 0)
    indexNext = DSMIB_CLASS_RULE_INDEX_MAX;
  else if (indexNext > DSMIB_AUTO_INCR_INDEX_NEXT_START)
    indexNext--;
  else
  {  
    LOG_MSG("dsmibClassRuleIndexNextUndo: WARNING -- attempting to decrement "
            "an indexNext value of %u for class %u\n", indexNext, classIndex);
    return;
  }

  pRow->mib.ruleIndexNextFree = indexNext;
}

/*********************************************************************
* @purpose  Verifies the specified class rule index value matches its
*           current 'IndexNext' value
*
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index to be checked
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
L7_RC_t dsmibClassRuleIndexNextVerify(L7_uint32 classIndex, 
                                      L7_uint32 classRuleIndex)
{
  L7_uint32     currentIndex;

  if (classRuleIndex == 0)
    return L7_FAILURE;

  /* skip this checking if currently in 'special config mode' */
  if (dsmibSpecialConfigModeGet() == L7_TRUE)
    return L7_SUCCESS;

  if (dsmibClassRuleIndexNextRead(classIndex, &currentIndex) != L7_SUCCESS)
    return L7_FAILURE;

  return (classRuleIndex == currentIndex) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Class Rule Table row element
*
* @param    classIndex      @b{(input)}  Class index
* @param    classRuleIndex  @b{(input)}  Class rule index
*
* @returns  pRow        Pointer to class rule row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsmibClassRuleEntryCtrl_t *dsmibClassRuleRowFind(L7_uint32 classIndex, 
                                                 L7_uint32 classRuleIndex)
{
  dsmibClassRuleKey_t       keys;
  dsmibClassRuleEntryCtrl_t *pRow;

  /* get access to the class row */
  keys.classIndex = classIndex;
  keys.classRuleIndex = classRuleIndex;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_CLASS_RULE], 
                       &keys, AVL_EXACT);

  return pRow;
}

/*************************************************************************
* @purpose  Create a class rule row for an Access List rule definition
*
* @param    classIndex      @b{(input)}  Class index
* @param    pRowPtr         @b{(output)} Pointer to row ptr output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsmibClassRuleAclCreate(L7_uint32 classIndex, 
                                dsmibClassRuleEntryCtrl_t **pRowPtr)
{
  L7_uint32     classRuleIndex;
  L7_uint32     i, imax;
  L7_RC_t       rc;

  #define DSMIB_ACL_CREATE_ATTEMPTS_MAX         2
  #define DSMIB_ACL_CREATE_SLEEP_TIME_MS        50

  /* init output ptr to null for error paths */
  *pRowPtr = L7_NULLPTR;

  /* make a finite number of attempts to create a new row in the Class Rule
   * Table
   */
  imax = DSMIB_ACL_CREATE_ATTEMPTS_MAX;
  for (i = 1; i <= imax; i++)
  {
    if ((diffServClassRuleIndexNext(classIndex, &classRuleIndex) != L7_SUCCESS) 
         || (classRuleIndex == 0))
      return L7_FAILURE;

    rc = diffServClassRuleCreate(classIndex, classRuleIndex, L7_TRUE);

    /* analyze outcome based on return code */
    if (rc == L7_SUCCESS)
      break;

    /* quit if maximum attempts have been reached before doing another sleep 
     * cycle
     */
    if (i == imax)
      return L7_FAILURE;

    if ((rc == L7_NOT_SUPPORTED) || (rc == L7_ERROR))
    {
      osapiSleepMSec(DSMIB_ACL_CREATE_SLEEP_TIME_MS);
      continue;
    }

    /* all other unsuccessful return codes result in failure */
    return L7_FAILURE;

  } /* endfor */

  /* get access to the class rule row */
  *pRowPtr = dsmibClassRuleRowFind(classIndex, classRuleIndex);

  return (*pRowPtr != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*************************************************************************
* @purpose  Delete all class rule row(s) for an Access List rule definition
*
* @param    classIndex      @b{(input)}  Class index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Indiscriminately deletes the class rule rows associated with 
*           the specified classIndex for a class type 'acl'.  The rows may
*           be in any state of completion.
*
* @end
*********************************************************************/
L7_RC_t dsmibClassRuleAclDelete(L7_uint32 classIndex)
{
  dsmibClassEntryCtrl_t     *pClass;
  L7_uint32                 classRuleIndex, nextClass;

  /* get access to the class row */
  pClass = dsmibClassRowFind(classIndex);
  if (pClass == L7_NULLPTR)
    return L7_FAILURE;

  /* only allowed for class type 'acl' */
  if (pClass->mib.type != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
    return L7_FAILURE;

  /* walk the Class Rule Table for this class, deleting each class rule */
  classRuleIndex = 0;                           /* start with first rule */
  while ((diffServClassRuleGetNext(classIndex, classRuleIndex, &nextClass, 
                                   &classRuleIndex) == L7_SUCCESS) &&
         (nextClass == classIndex))
  {
    if (classRuleIndex == 0)
      break;

    if (diffServClassRuleDelete(classIndex, classRuleIndex) != L7_SUCCESS)
    {
      LOG_MSG("dsmibClassRuleAclDelete: Could not delete rule %u from "
              "class \'%s\'\n", classRuleIndex, (char *)pClass->mib.name);
      return L7_FAILURE;
    }
  } /* endwhile */

  /* for completeness, re-initialize the indexNext value for this class */
  pClass->mib.ruleIndexNextFree = DSMIB_AUTO_INCR_INDEX_NEXT_START;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Checks if a class rule entry type is supported for all 
*           policy instances that reference the owner class
*
* @param    pRow          @b{(input)} Class rule table row pointer
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Each policy instance with a valid class index must be 
*           checked against all classes in its defined reference chain,
*           not just against the owning class for this class rule row.
*
* @notes    During SetTest processing, the owning class was temporarily
*           updated to include a reference to a proposed reference class,
*           which allows the policy instance class validation to check
*           it.
*       
* @end
*********************************************************************/
L7_RC_t dsmibClassRuleInstanceValidate(dsmibClassRuleEntryCtrl_t *pRow)
{
  L7_uint32           policyIndex, policyInstIndex;
  L7_uint32           classIndex, instClassIndex;

  classIndex = pRow->pClass->key.classIndex;

  /* walk the entire policy instance table */
  policyIndex = policyInstIndex = 0;            /* start with first instance */
  while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex, &policyIndex, 
                                    &policyInstIndex) == L7_SUCCESS))
  {
    /* ignore policy instances that do not have their class index set
     *
     * NOTE: policy instance setTest also checks if the class definition
     *       contains any rules that are not supported by a restricted policy
     */
    if (diffServPolicyInstObjectGet(policyIndex, policyInstIndex, 
                                    L7_DIFFSERV_POLICY_INST_CLASS_INDEX,
                                    &instClassIndex) != L7_SUCCESS)
      continue;

    /* determine if the owner class for this rule belongs to a class reference 
     * chain anchored by the policy instance class
     */
    if (dsmibClassIsInRefChain(classIndex, instClassIndex) != L7_TRUE)
      continue;

    /* check for policy restrictions against all rules in class reference chain*/
    if (dsmibPolicyInstClassValidate(policyIndex, instClassIndex) != L7_SUCCESS)
      return L7_FAILURE;

  } /* endwhile */
                            
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Send notify event callbacks to all policies affected by class change
*
* @param    classIndex      @b{(input)} Class index
*
* @returns  void
*
* @comments    
*
* @end
*********************************************************************/
void dsmibClassRulePolicyChangeNotify(L7_uint32 classIndex)
{
  L7_uint32     policyIndex;

  /* send callbacks for all policies that reference the affected class */
  policyIndex = 0;
  while (diffServClassRefByPolicyGetNext(classIndex, policyIndex, 
                                         &policyIndex) == L7_SUCCESS)
  {
    /* NOTE: A class rule deletion is considered a 'modify' event for the policy */
    diffServChangeNotify(policyIndex, DIFFSERV_EVENT_MODIFY, L7_NULLPTR);
  }
}

/*********************************************************************
* @purpose  Checks if a class rule object value is valid to 'get'
*
* @param    pRow        @b{(input)} Class rule table row pointer
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
L7_RC_t dsmibClassRuleObjectGetCheck(dsmibClassRuleEntryCtrl_t *pRow,
                                     L7_DIFFSERV_CLASS_RULE_TABLE_OBJECT_t oid)
{
  dsmibClassRuleType_t  objType;
  dsmibClassRuleFlags_t objFlag;
  L7_RC_t               rc;

  switch (oid)
  {
  case L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE; /* not checked */
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_COS:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_COS;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_COS2:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_COS;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTIP_ADDR:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTIP_MASK:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTIPV6_ADDR:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6ADDR;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTIPV6_PLEN:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6PLEN;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTL4PORT_START:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTL4PORT_END:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTMAC_ADDR:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_DSTMAC_MASK:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_ETYPE_KEYID:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_KEYID;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_ETYPE_VALUE:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_ETYPE_VALUE;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_EVERY:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY;
    objFlag = DSMIB_NO_FLAG;  /* no row invalid flag for this match object */
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_IPV6_FLOWLBL:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6FLOWLBL;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_IPDSCP:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPDSCP;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_IPPRECEDENCE:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPPRECEDENCE;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_IPTOS_BITS:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSBITS;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_IPTOS_MASK:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPTOSMASK;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_PROTOCOL_NUM:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_PROTOCOLNUM;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_REFCLASS_INDEX:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_REFCLASSINDEX;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCIP_ADDR:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPADDR;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCIP_MASK:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPMASK;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCIPV6_ADDR:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6ADDR;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCIPV6_PLEN:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_IPV6PLEN;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCL4PORT_START:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_START;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCL4PORT_END:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_L4PORT_END;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCMAC_ADDR:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_MACADDR;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_SRCMAC_MASK:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_MACMASK;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_VLANID_START:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_VLANID_END:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_VLANID2_START:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_VIDSTART;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_VLANID2_END:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2;
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_VIDEND;
    break;

  case L7_DIFFSERV_CLASS_RULE_MATCH_EXCLUDE_FLAG:
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE; /* not checked */
    objFlag = DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG;
    break;

  case L7_DIFFSERV_CLASS_RULE_STORAGE_TYPE:
  case L7_DIFFSERV_CLASS_RULE_ROW_STATUS: 
    objType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE; /* not checked */
    objFlag = DSMIB_NO_FLAG;  /* no row invalid flag for this match object */
    break;

  default:
    /* invalid object id */
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

  /* using objType = 'none' to bypass entry type check for the object */
  if (objType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE)
  {
    rc = dsmibClassRuleFeaturePresentCheck(objType);
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
* @purpose  Check if all class reference rule chains are within limits
*
* @param    void
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    This function is assumed to be called after class rule row 
*           creation, but before the row becomes active.  It may be used
*           when setting any class rule entry type, although special 
*           preparation is needed when setting the reference class index
*           row object (see setTest function).
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
L7_RC_t dsmibClassRuleRefLimitCheck(void)
{
  L7_RC_t               rc = L7_SUCCESS;
  L7_uint32             idx;
  L7_uint32             nestRuleCount;
  dsmibClassEntryCtrl_t *pRef;

  /* skip this checking if currently in 'special config mode' */
  if (dsmibSpecialConfigModeGet() == L7_TRUE)
    return L7_SUCCESS;

  /* check that the total number of rules for the nested class 
   * references does not exceed a maximum limit
   *
   * NOTE:  Must consider predecessor class references when counting the
   *        total number of class rules due to adding a class reference 
   *        anywhere in the chain.
   *
   * NOTE:  The easiest way to do this is to walk through every defined
   *        class that contains a reference and calculate the total number
   *        of nested rules using it as the anchor class.
   */
  idx = 0;                                  /* start from first class row */
  while (diffServClassGetNext(idx, &idx) == L7_SUCCESS)
  {
    nestRuleCount = 0;

    pRef = dsmibClassRowFind(idx);
    if (pRef == L7_NULLPTR)
    {
      rc = L7_FAILURE;
      break;
    }

    /* walk through class reference chain to accumulate rule counts */
    while (pRef != L7_NULLPTR)
    {
      nestRuleCount += pRef->ruleCount;

      if (pRef->refClassIndex == DSMIB_CLASS_REF_INDEX_NONE)
        break;

      pRef = dsmibClassRowFind(pRef->refClassIndex);
      if (pRef == L7_NULLPTR)
      {
        /* ERROR: set nestRuleCount above max value to force an 
         * L7_FAILURE return code below
         */
        nestRuleCount = DSMIB_CLASS_REF_NESTED_RULE_MAX+1;
        break;
      }

    } /* endwhile class ref chain walk */

    /* check the nested rule count against its allowed maximum
     *
     * NOTE: Recall that a proposed class rule is already included
     *       in the nestRuleCount value, since its row creation
     *       caused the corresponding class ruleCount value to be 
     *       incremented.
     */
    if (nestRuleCount > DSMIB_CLASS_REF_NESTED_RULE_MAX)
    {
      rc = L7_FAILURE;
      break;
    }

  } /* endwhile class get next */

  return rc;
}

/*********************************************************************
* @purpose  Check if the specified class rule entry type value is allowed
*
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
*
* @returns  L7_TRUE     Entry type allowed
* @returns  L7_FALSE    Entry type not allowed
*
* @notes    A class may have multiple rule entries of the same type, 
*           but a class type of 'all' must only have one non-excluded
*           rule of a given type.  
*
* @notes    This function must be called when setting the exclude flag field
*           to 'false'.  Sometimes, depending on the object set
*           order among multiple rows, the entry type fields can be set 
*           successfully, but the conflict cannot be determined until the
*           exclude flag is being set.  Since this is likely to be called
*           by the 'setTest' function before the exclude flag field is 
*           actually set, it is ASSUMED that the class rule's proposed
*           exclude flag value is 'false'.
*
* @notes    Also called when establishing a class reference to make sure there
*           are no duplicate non-excluded rules in the combined class of type
*           'all'.  This function does not enforce any other 'refclass' 
*           restrictions, however.
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
L7_BOOL dsmibClassRuleIsRuleAllowed(L7_uint32 classIndex, 
                                    L7_uint32 classRuleIndex)
{
  static const char *routine_name = "dsmibClassRuleIsRuleAllowed()";
  dsmibClassRuleEntryCtrl_t *pRow, *pRule;
  dsmibClassEntryCtrl_t     *pClass, *pRefClass;
  dsmibClassRuleType_t      entryType;
  L7_uint32                 classIdx, ruleIdx;
  L7_uint32                 typesFoundInit, typesFound, f;
  L7_uint32                 l2TypesFound;
  L7_uint32                 relevantClassList[L7_DIFFSERV_CLASS_LIM];
  L7_uint32                 relevantClassCount;
  L7_uint32                 rd, wrt, currentClass;

  /* skip this checking if currently in 'special config mode' */
  if (dsmibSpecialConfigModeGet() == L7_TRUE)
    return L7_TRUE;

  /* get access to the class rule row */
  pRow = dsmibClassRuleRowFind(classIndex, classRuleIndex);
  if (pRow == L7_NULLPTR)
    return L7_FALSE;

  /* get access to the class row */
  pClass = pRow->pClass;
  if (pClass == L7_NULLPTR)
    return L7_FALSE;

  /* a class type other than 'all' can have multiple rows of same entry type */
  if (pClass->mib.type != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL)
    return L7_TRUE;

  /* check if requested rule and class l3_proto settings are compatible */
  if (pClass->mib.l3Protocol == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4)
  {
    switch (pRow->mib.entryType)
    {
    /* the following entryTypes are incompatible for a class with this l3Protocol setting */
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
      {
        return L7_FALSE;
      }
      break;
    default:
      /* otherwise, OK */
      break;
    } /* endswitch pRule->mib.entryType */
  }
  if (pClass->mib.l3Protocol == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6)
  {
    switch (pRow->mib.entryType)
    {
    /* the following entryTypes are incompatible for a class with this l3Protocol setting */
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
      {
        return L7_FALSE;
      }
      break;
    default:
      /* otherwise, OK */
      break;
    } /* endswitch pRule->mib.entryType */
  }

  /* using the specified classIndex as a basis, construct a list of all
   * reference class chains that contain the specified class
   *
   * NOTE:  Starting with the specified class, successively search for each
   *        class that references it, then each class that references those
   *        classes, and so forth.  The end result is a list of reference
   *        class chain origins only, not every class in the chain.
   *
   * NOTE:  This is intentionally different from the list derived in 
   *        dsmibClassRuleIsValueAllowed(), so the logic is maintained
   *        separately.
   */
  memset(relevantClassList, 0, sizeof(relevantClassList));
  rd = 0;                                       /* list read index            */
  wrt = rd + 1;                                 /* list write index           */
  relevantClassList[rd] = classIndex;           /* start with specified class */
  while (rd < wrt)
  {
    /* search the Class Table for classes whose refClassIndex match the
     * current 'rd' class, add any such class to the list 'wrt' location and
     * zero out the 'rd' location in preparation for list cleanup processing
     *
     * NOTE: The 'wrt' index starts out one position ahead of the 'rd'
     *       index and remains one or more positions ahead of 'rd' until
     *       all relevant classes are found, at which time 'rd' and 'wrt'
     *       become equal.
     *
     * NOTE: Each class can reference at most one other class, and circular
     *       references are not allowed.  Thus, in the worst case each class
     *       can appear at most once in this list.
     */
    currentClass = relevantClassList[rd];
    classIdx = 0;                               /* start with first class */
    while (diffServClassGetNext(classIdx, &classIdx) == L7_SUCCESS)
    {
      pRefClass = dsmibClassRowFind(classIdx);
      if (pRefClass == L7_NULLPTR)
        continue;

      if (pRefClass->refClassIndex == relevantClassList[rd])
      {
        if (wrt < L7_DIFFSERV_CLASS_LIM)
        {
          relevantClassList[wrt++] = classIdx;
          currentClass = 0;
        }
      }
    } /* endwhile class walk */

    /* current class is set to zero in list if it was referenced by another 
     * class
     * 
     * advance 'rd' index since done checking current class for references
     * to it
     */
    relevantClassList[rd++] = currentClass;

  } /* endwhile rd < wrt */
  relevantClassCount = wrt;

  /* post-process the list by clearing out all entries that were set to zero */
  rd = wrt = 0;
  while (rd < relevantClassCount)
  {
    if (relevantClassList[rd] != 0)
      relevantClassList[wrt++] = relevantClassList[rd];
    rd++;
  }
  relevantClassCount = wrt;
  /* zero out remaininig original list entries */
  for (; wrt < rd; wrt++)
    relevantClassList[wrt] = 0;


  /* for class type 'all', a non-excluded match criterion can only appear 
   * once within the entire class definition, including all nested reference
   * classes
   *
   * NOTE:  This logic relies on the required set order of entry type 
   *        before exclude flag within a class rule row.  Assumes the
   *        exclude flag value is 'false' for this class rule row.
   *
   * NOTE:  Ignores other 'refclass' restrictions here -- such checking 
   *        needs to be done elsewhere.
   *
   * NOTE:  The IP DSCP, IP Precedence and IP TOS fields count as the 
   *        the same field for the purposes of the class type 'all' 
   *        non-excluded test.  Use the TOS entry type flag to represent
   *        all three in the checking loop below.
   */

  /* expect the entry type field to be set by now */
  if (DSMIB_ROW_INVALID_FLAG_IS_ON(pRow, DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE))
    return L7_TRUE;
  entryType = pRow->mib.entryType;

  /* initialize a flags field to track all entry types found during search
   * 
   * start with the flag value of this entry type to detect its duplicate,
   */
  if ((entryType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP) ||
      (entryType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE))
    f = (L7_uint32)(1 << L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS);
  else
    f = (L7_uint32)(1 << entryType);

  typesFoundInit = f;

  for (rd = 0; rd < relevantClassCount; rd++)
  {
    typesFound = typesFoundInit;        /* init for each refclass chain */

    /* for each reference class chain anchor contained in the derived
     * relevant class list, check each class in the chain for any 
     * disallowed duplicate class rule types
     */
    classIdx = relevantClassList[rd];
    ruleIdx = 0;                                /* start with first rule */
    while (dsmibClassRuleRefChainGetNext(classIdx, ruleIdx, 
                                         &classIdx, &ruleIdx) == L7_SUCCESS)
    {
      /* skip our own class rule row */
      if ((classIdx == classIndex) && (ruleIdx == classRuleIndex))
        continue;

      /* get a pointer to the class rule */
      pRule = dsmibClassRuleRowFind(classIdx, ruleIdx);
      if (pRule == L7_NULLPTR)
      {
        LOG_MSG("%s: row find error for indexes %u,%u\n", 
                routine_name, classIdx, ruleIdx);
        return L7_FALSE;
      }

      /* look for class rule entry type same as ours (except for 'refclass' 
       * type), then check if its exclude flag is set to 'false' -- reject 
       * entry type value if so
       *
       * since each class can have one 'refclass' rule, multiple occurrences of
       * this entry type can be seen throughout a complete reference class 
       * definition, so ignore 'refclass' rules here
       *
       * remember, an 'IS_OFF' row flag means the object value exists in the row
       */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_ENTRYTYPE))
        if (pRule->mib.entryType != L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS)
          if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRule, DSMIB_CLASS_RULE_FLAGS_MATCH_EXCLUDEFLAG))
            if (pRule->mib.excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE)
            {
              /* accumulate the various rule entry types encountered during this
               * search; when an existing non-excluded entry type is detected a 
               * second time, it is a duplicate rule
               *
               * NOTE:  The use of a flag bit only works for up to 31 different
               *        class rule entry type defs (assumed to be consecutively
               *        numbered starting at 1).  If a larger set of values is 
               *        needed, this algorithm can be changed to use an array
               *        of booleans instead of bit flags.
               *
               * NOTE:  Remember to use TOS entry type to set up checking flag
               *        for IP DSCP and IP Precedence entry types.
               */
              if ((pRule->mib.entryType == 
                   L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP) ||
                  (pRule->mib.entryType == 
                   L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE))
                f = (L7_uint32)(1 << L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS);
              else
                f = (L7_uint32)(1 << pRule->mib.entryType);

              if ((typesFound & f) != 0)
                return L7_FALSE;                /* duplicate type found */

              typesFound |= f;
            } /* endif */

    } /* endwhile ref class search */

    /* check if this is an attempt to have L2 match criteria in an IPv6 class */
    if (pClass->mib.l3Protocol == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6)
    {
      l2TypesFound = typesFound & ((1 << L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS) |
                                   (1 << L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2) |
                                   (1 << L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC) |
                                   (1 << L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE) |
                                   (1 << L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC) |
                                   (1 << L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID) |
                                   (1 << L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2) |
                                   (1 << L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC));
      if (0 != l2TypesFound)
      {
        if (cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, L7_DIFFSERV_CLASS_MATCH_IPV6_MAC_FEATURE_ID) != L7_TRUE)
        {
          return L7_FALSE;                /* platform does not support L2 match criteria in IPv6 class */
        }
      }
    }
  } /* endfor rd loop */

  return L7_TRUE;
}

/*********************************************************************
* @purpose  Check if the class rule row object value is allowed within
*           the context of its class and any reference class chains
*
* @param    classIndex      @b{(input)} Class index
* @param    classRuleIndex  @b{(input)} Class rule index
*
* @returns  L7_TRUE     Class rule value allowed
* @returns  L7_FALSE    Class rule value not allowed
*
* @notes    Only allowing one occurrence of a given class rule entry type
*           containing a particular object value within an entire reference
*           class chain (regardless of the exclude flag setting).  Note that
*           this is a different kind of test from the 'IsRuleAllowed' checking.
*
* @notes    This function is called whenever a class rule row is completed.
*           Any incomplete row is skipped during the evaluation processing.
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
L7_BOOL dsmibClassRuleIsValueAllowed(L7_uint32 classIndex, 
                                     L7_uint32 classRuleIndex)
{
  static const char *routine_name = "dsmibClassRuleIsValueAllowed()";
  dsmibClassEntryCtrl_t     *pClass, *pRefClass;
  dsmibClassRuleEntryCtrl_t *pRule, *pRefRule;
  L7_uint32                 relevantClassList[L7_DIFFSERV_CLASS_LIM];
  L7_uint32                 relevantClassCount;
  L7_uint32                 rd, wrt;
  L7_uint32                 classIdx, nextClass, ruleIdx;
  L7_uint32                 refClassIdx, refNextClass, refRuleIdx;

  /* skip this checking if currently in 'special config mode' */
  if (dsmibSpecialConfigModeGet() == L7_TRUE)
    return L7_TRUE;

  /* get access to the class rule row */
  pRule = dsmibClassRuleRowFind(classIndex, classRuleIndex);
  if (pRule == L7_NULLPTR)
    return L7_FALSE;

  /* get access to the class row */
  pClass = pRule->pClass;
  if (pClass == L7_NULLPTR)
    return L7_FALSE;

  /* class type 'acl' is not subject to this checking */
  if (pClass->mib.type == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
    return L7_TRUE;

  /* using the specified classIndex as a basis, construct a list of all
   * relevant classes belonging to any reference class chain that contains
   * the specified class
   *
   * NOTE:  Starting with the specified class, successively search for each
   *        class that references it, then each class that references those
   *        classes, and so forth.
   */
  memset(relevantClassList, 0, sizeof(relevantClassList));
  rd = 0;                                       /* list read index            */
  wrt = rd + 1;                                 /* list write index           */
  relevantClassList[rd] = classIndex;           /* start with specified class */
  while (rd < wrt)
  {
    /* search the Class Table for classes whose refClassIndex match the
     * current 'rd' class, and add any such class to the list 'wrt' location
     *
     * NOTE: The 'wrt' index starts out one position ahead of the 'rd'
     *       index and remains one or more positions ahead of 'rd' until
     *       all relevant classes are found, at which time 'rd' and 'wrt'
     *       become equal.
     *
     * NOTE: Each class can reference at most one other class, and circular
     *       references are not allowed.  Thus, in the worst case each class
     *       can appear at most once in this list.
     */
    classIdx = 0;                               /* start with first class */
    while (diffServClassGetNext(classIdx, &classIdx) == L7_SUCCESS)
    {
      pRefClass = dsmibClassRowFind(classIdx);
      if (pRefClass == L7_NULLPTR)
        continue;

      if (pRefClass->refClassIndex == relevantClassList[rd])
      {
        if (wrt < L7_DIFFSERV_CLASS_LIM)
          relevantClassList[wrt++] = classIdx;
      }
    } /* endwhile class walk */

    /* advance 'rd' index since done checking current class for references
     * to it
     */
    rd++;
  } /* endwhile rd < wrt */
  relevantClassCount = wrt;

  /* for each class in the relevant class list, compare each of its rules
   * against all other rules in its own class as well as every class in 
   * its reference class chain (only consider completed class rule rows here)
   *
   * a duplicate rule is one with the same entryType and whose mib.match
   * fields are identical (assumes unused portion of each field is always
   * zero)
   */
  for (rd = 0; rd < relevantClassCount; rd++)
  {
    classIdx = relevantClassList[rd];

    /* get a pointer to the relevant class row */
    pClass = dsmibClassRowFind(classIdx);
    if (pClass == L7_NULLPTR)
    {
      LOG_MSG("%s: relevant class row find error for index %u\n", 
              routine_name, classIdx);
      return L7_FALSE;
    }

    /* walk rules in this relevant class list entry */
    ruleIdx = 0;                                /* start with first rule */
    while ((diffServClassRuleGetNext(classIdx, ruleIdx, &nextClass, &ruleIdx)
             == L7_SUCCESS) &&
           (nextClass == classIdx) && (ruleIdx != 0))
    {
      /* get a pointer to the class rule */
      pRule = dsmibClassRuleRowFind(classIdx, ruleIdx);
      if (pRule == L7_NULLPTR)
      {
        LOG_MSG("%s: relevant class rule row find error for indexes %u,%u\n", 
                routine_name, classIdx, ruleIdx);
        return L7_FALSE;
      }

      /* make sure this comparitor rule is complete */
      if (DSMIB_ROW_IS_COMPLETE(pRule) != L7_TRUE)
        continue;

      /* now that we have one of the rules in the current class to work with,
       * compare it to each rule in the referenced class chain (don't 
       * compare against itself, however)
       */
      refClassIdx = classIdx;                   /* start with own class */

      while (refClassIdx != DSMIB_CLASS_REF_INDEX_NONE)
      {
        /* get a pointer to the reference class row (save it for later) */
        pRefClass = dsmibClassRowFind(refClassIdx);
        if (pRefClass == L7_NULLPTR)
        {
          LOG_MSG("%s: refclass row find error for index %u\n", 
                  routine_name, refClassIdx);
          return L7_FALSE;
        }

        /* walk all rules in current reference class */
        refRuleIdx = 0;                           /* start with first rule */
        while ((diffServClassRuleGetNext(refClassIdx, refRuleIdx, 
                                         &refNextClass, &refRuleIdx)
                 == L7_SUCCESS) &&
               (refNextClass == refClassIdx) && (refRuleIdx != 0))
        {
          /* skip the comparator class rule row from its relevant class entry */
          if ((refClassIdx == classIdx) && (refRuleIdx == ruleIdx))
            continue;

          /* get a pointer to the class rule */
          pRefRule = dsmibClassRuleRowFind(refClassIdx, refRuleIdx);
          if (pRefRule == L7_NULLPTR)
          {
            LOG_MSG("%s: refclass rule row find error for indexes %u,%u\n", 
                    routine_name, refClassIdx, refRuleIdx);
            return L7_FALSE;
          }

          /* skip an incomplete row or a rule entry type of 'refclass' */
          if (DSMIB_ROW_IS_COMPLETE(pRefRule) != L7_TRUE)
            continue;
          if (pRefRule->mib.entryType == 
                L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS)
            continue;

          /* check for duplicate rule:  same entry type and object value
           *
           * NOTE: Although the mib.match content is specific to the 
           *       type of entry, we are doing a compare of the entire
           *       field so a switch statement is not needed here (assumes
           *       unused portion of mib.match field is always zero).
           *
           * NOTE: The exclude flag is not being checked here, since each 
           *       combination of class type and exclude flag value result
           *       in the same action when a duplicate rule is found.
           */
          if (pRefRule->mib.entryType == pRule->mib.entryType)
          {
            if (memcmp(&pRefRule->mib.match, &pRule->mib.match, 
                       sizeof(pRule->mib.match)) == L7_NULL)
              return L7_FALSE;
          }

          /* check for compatibility between Ethertype and IP match rules */
          if (dsmibClassRuleEtypeCompatCheck(pRule, pRefRule) != L7_SUCCESS)
            return L7_FALSE;

        } /* endwhile walk all rules in current reference class */

        /* update the refClassIdx with the next class in the reference chain */
        refClassIdx = pRefClass->refClassIndex;

      } /* endwhile refClassIdx != DSMIB_CLASS_REF_INDEX_NONE */

    } /* endwhile walk rules in this relevant class entry */

  } /* endfor relevant class list 'rd' loop */

  /* NOTE: Reaching this point means all relevant class rules were 
   *       cross-checked and no duplicates were found.
   */
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Replaces the set of row invalid flags based on the 
*           Class Rule Table object entry type
*
* @param    pRow        @b{(input)} Class rule table row pointer
* @param    entryType   @b{(input)} Class rule entry type             
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
L7_RC_t dsmibClassRuleRowFlagsReplace(dsmibClassRuleEntryCtrl_t *pRow,
                                      dsmibClassRuleType_t entryType)
{
  dsmibClassRuleFlags_t objFlag;

  switch (entryType)
  {
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_COS;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_COS2;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_DSTIP;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_DSTIPV6;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_DSTL4PORT;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_DSTMAC;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_ETYPE;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_EVERY;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_IP6VFLOWLBL;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_IPDSCP;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_IPPRECEDENCE;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_IPTOS;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_PROTOCOL;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_REFCLASS;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_SRCIP;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_SRCIPV6;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_SRCL4PORT;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_SRCMAC;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_VLANID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
    objFlag = DSMIB_CLASS_RULE_FLAGS_TYPE_VLANID2;
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
* @purpose  Map Class Rule object IDs to the Class Rule types
*
* @param    oid             @b{(input)} Object identifier
* @param    pClassRuleType  @b{(output)} Pointer to Class Rule type
*
* @returns  void  
*
* @notes    None
*
* @end
*********************************************************************/
void dsmibClassRuleConvertOidToType(L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_TYPE_t oid,
                                    L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t *pClassRuleType)
{
  switch (oid)
  {
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_COS:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_COS2:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIP_ADDR:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIP_MASK:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIPV6_ADDR:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTIPV6_ADDR_PLEN:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTL4PORT_START:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTL4PORT_END:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTMAC_ADDR:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_DSTMAC_MASK:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ETYPE_KEYID:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ETYPE_VALUE:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE;
    break;

  /* NOTE:  There are no settable objects for rule type '_EVERY' */

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_FLOWLBLV6:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPDSCP:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPPRECEDENCE:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPTOS_BITS:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_IPTOS_MASK:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_PROTOCOL_NUM:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_REFCLASS_INDEX:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIP_ADDR:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIP_MASK:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIPV6_ADDR:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCIPV6_ADDR_PLEN:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCL4PORT_START:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCL4PORT_END:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCMAC_ADDR:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_SRCMAC_MASK:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID_START:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID_END:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID2_START:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_VLANID2_END:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ENTRY_TYPE:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_EXCLUDE_FLAG:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_STORAGE_TYPE:
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_SETTEST_OID_ROW_STATUS:
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
    break;

  default:
    /* invalid object identifier */
    *pClassRuleType = L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE;
    break;
  } /* endswitch */
}

/*********************************************************************
* @purpose  Check for Ethertype rule compatibility within a class
*
* @param    *pRow1      @b{(input)} Ptr to one class rule row
* @param    *pRow2      @b{(input)} Ptr to another class rule row
*
* @returns  L7_SUCCESS  Compatible class rules
* @returns  L7_FAILURE  Incompatible class rules
*
* @notes    A non-IP Ethertype key is incompatible with any class rules
*           that match on a field within the IP portion of a packet.
*           Rules with an exclude flag set are excepted, since they
*           end up as a separate hardware classifier entry.
*
* @end
*********************************************************************/
L7_RC_t dsmibClassRuleEtypeCompatCheck(dsmibClassRuleEntryCtrl_t *pRow1,
                                       dsmibClassRuleEntryCtrl_t *pRow2)
{
  dsmibClassRuleEntryCtrl_t *pRowEtype = L7_NULLPTR;
  dsmibClassRuleType_t      entryType1, entryType2, compareEntryType = 0;
  L7_RC_t                   rc;

  /* check inputs */
  if ((pRow1 == L7_NULLPTR) || (pRow2 == L7_NULLPTR))
    return L7_FAILURE;

  /* only want to check the non-excluded rules within a class type 'all'
   *
   * NOTE:  Class type for the two class rows are assumed to be the same.
   */
  if ((pRow1->pClass->mib.type != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL)  ||
      (pRow1->mib.excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE) ||
      (pRow2->mib.excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE))
    return L7_SUCCESS;

  entryType1 = pRow1->mib.entryType;
  entryType2 = pRow2->mib.entryType;

  if (entryType1 == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE)
  {
    pRowEtype = pRow1;
    compareEntryType = entryType2;
  }
  else if (entryType2 == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE)
  {
    pRowEtype = pRow2;
    compareEntryType = entryType1;
  }

  rc = L7_SUCCESS;

  if (pRowEtype != L7_NULLPTR)
  {
    if (pRowEtype->pClass->mib.l3Protocol == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6)
    {
      /* check if this is an IP Ethertype */
      if (pRowEtype->mib.match.etype.keyid == L7_QOS_ETYPE_KEYID_IPV6)
        return L7_SUCCESS;

      if ((pRowEtype->mib.match.etype.keyid == L7_QOS_ETYPE_KEYID_CUSTOM) &&
          (pRowEtype->mib.match.etype.value == L7_QOS_ETYPE_ID_IPV6))
        return L7_SUCCESS;

      /* a non-IP Ethertype exists -- check for existing IP rule type fields */
      switch (compareEntryType)
      {
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
        rc = L7_FAILURE;
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
        rc = L7_FAILURE;
        break;

      default:
        /* no conflicts */
        break;

      } /* endswitch */
    }
    else
    {
      /* check if this is an IP Ethertype */
      if (pRowEtype->mib.match.etype.keyid == L7_QOS_ETYPE_KEYID_IPV4)
        return L7_SUCCESS;

      if ((pRowEtype->mib.match.etype.keyid == L7_QOS_ETYPE_KEYID_CUSTOM) &&
          (pRowEtype->mib.match.etype.value == L7_QOS_ETYPE_ID_IPV4))
        return L7_SUCCESS;

      /* a non-IP Ethertype exists -- check for existing IP rule type fields */
      switch (compareEntryType)
      {
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
        rc = L7_FAILURE;
        break;

      case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
        rc = L7_FAILURE;
        break;

      default:
        /* no conflicts */
        break;

      } /* endswitch */
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Translate the Ethertype keyid into a value  
*
* @param    classIndex     @{(input)}   Class index 
* @param    classRuleIndex @{(input)}   Class rule index 
* @param    *pValue        @{(output)}  Ptr to Ethertype value output
* @param    *pValue2       @{(output)}  Ptr to Ethertype value2 output
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Some Ethertype keywords translate into two Ethertype values. 
*           In this case, pValue2 is output as a nonzero value.           
*
* @end
*********************************************************************/
L7_RC_t dsmibEtypeKeyidTranslate(L7_uint32 classIndex, 
                                 L7_uint32 classRuleIndex,
                                 L7_uint32 *pValue1,
                                 L7_uint32 *pValue2)
{
  dsmibEtypeKeyid_t     keyid;
  L7_uint32             val1, val2;


  /* get Ethertype keyid from class rule match object */
  if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                 L7_DIFFSERV_CLASS_RULE_MATCH_ETYPE_KEYID,
                                 &keyid) != L7_SUCCESS)
    return L7_FAILURE;


  val1 = val2 = DSMIB_CLASS_RULE_ETYPE_VALUE_UNUSED;

  /* NOTE:  Only the 'custom' keyword uses the val values read
   *        from the MIB.  All other keywords use a well-known value and
   *        mask.  Some Ethertype keywords require two Ethertype values
   *        to represent the protocol.
   */

  switch (keyid)
  {
  case L7_QOS_ETYPE_KEYID_CUSTOM:
    /* get Ethertype value from class rule match object */
    if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                   L7_DIFFSERV_CLASS_RULE_MATCH_ETYPE_VALUE,
                                   &val1) != L7_SUCCESS)
      return L7_FAILURE;
    break;

  case L7_QOS_ETYPE_KEYID_APPLETALK:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_APPLETALK;
    break;

  case L7_QOS_ETYPE_KEYID_ARP:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_ARP;
    break;

  case L7_QOS_ETYPE_KEYID_IBMSNA:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_IBMSNA;
    break;

  case L7_QOS_ETYPE_KEYID_IPV4:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_IPV4;
    break;

  case L7_QOS_ETYPE_KEYID_IPV6:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_IPV6;
    break;

  case L7_QOS_ETYPE_KEYID_IPX:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_IPX;
    break;

  case L7_QOS_ETYPE_KEYID_MPLSMCAST:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_MPLSMCAST;
    break;

  case L7_QOS_ETYPE_KEYID_MPLSUCAST:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_MPLSUCAST;
    break;

  case L7_QOS_ETYPE_KEYID_NETBIOS:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_NETBIOS;
    break;

  case L7_QOS_ETYPE_KEYID_NOVELL:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_NOVELL_ID1;
    val2 = (L7_uint32)L7_QOS_ETYPE_ID_NOVELL_ID2;
    break;

  case L7_QOS_ETYPE_KEYID_PPPOE:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_PPPOE_ID1;
    val2 = (L7_uint32)L7_QOS_ETYPE_ID_PPPOE_ID2;
    break;

  case L7_QOS_ETYPE_KEYID_RARP:
    val1 = (L7_uint32)L7_QOS_ETYPE_ID_RARP;
    break;

  default:
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

  /* set up the output pointers with the local values */
  *pValue1 = val1;
  *pValue2 = val2;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check the existence of the specified component feature
*           in the hardware platform
*
* @param    classRuleType  @b{(input)} Class Rule type
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE        Invalid class rule type
* @returns  L7_NOT_SUPPORTED  Feature not supported in hardware platform
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dsmibClassRuleFeaturePresentCheck(L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_t classRuleType)
{
  L7_DIFFSERV_FEATURE_IDS_t fid;
  L7_BOOL                   featurePresent;

  switch (classRuleType)
  {
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
    fid = L7_DIFFSERV_CLASS_MATCH_COS_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
    fid = L7_DIFFSERV_CLASS_MATCH_COS2_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
    fid = L7_DIFFSERV_CLASS_MATCH_DSTIP_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
    fid = L7_DIFFSERV_CLASS_MATCH_IPV6_DSTIP_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
    fid = L7_DIFFSERV_CLASS_MATCH_DSTL4PORT_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
    fid = L7_DIFFSERV_CLASS_MATCH_DSTMAC_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
    fid = L7_DIFFSERV_CLASS_MATCH_ETYPE_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY:
    fid = L7_DIFFSERV_CLASS_MATCH_EVERY_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
    fid = L7_DIFFSERV_CLASS_MATCH_IPV6_FLOWLBL_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
    fid = L7_DIFFSERV_CLASS_MATCH_IPDSCP_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
    fid = L7_DIFFSERV_CLASS_MATCH_IPPRECEDENCE_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
    fid = L7_DIFFSERV_CLASS_MATCH_IPTOS_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
    fid = L7_DIFFSERV_CLASS_MATCH_PROTOCOL_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS:
    fid = L7_DIFFSERV_CLASS_MATCH_REFCLASS_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
    fid = L7_DIFFSERV_CLASS_MATCH_SRCIP_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
    fid = L7_DIFFSERV_CLASS_MATCH_IPV6_SRCIP_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
    fid = L7_DIFFSERV_CLASS_MATCH_SRCL4PORT_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
    fid  = L7_DIFFSERV_CLASS_MATCH_SRCMAC_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:
    fid = L7_DIFFSERV_CLASS_MATCH_VLANID_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
    fid = L7_DIFFSERV_CLASS_MATCH_VLANID2_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_NONE:
    /* this value is used to handle certain generic objects (row status, etc) */
    fid = L7_DIFFSERV_FEATURE_SUPPORTED;
    break;

  default:
    /* invalid class rule type */
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

  featurePresent = cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, fid);
  
  return (featurePresent == L7_TRUE) ? L7_SUCCESS : L7_NOT_SUPPORTED;
}


/*
==========================================
==========================================
==========================================

   CLASS RULE TABLE DEBUG SHOW FUNCTION

==========================================
==========================================
==========================================
*/

/*********************************************************************
* @purpose  Display the current DiffServ Class Rule Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsmibClassRuleTableShow(void)
{
  L7_uint32                 msgLvlReqd;
  dsmibClassRuleEntryCtrl_t *pRow;
  dsmibClassRuleKey_t       keys;
  avlTree_t                 *pTree = &dsmibAvlTree[DSMIB_TABLE_ID_CLASS_RULE];
  L7_uint32                 count;
  L7_uint32                 i;

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Class Rule Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Class Rule Table (contains %u entries):\n\n", 
              count);

  DIFFSERV_PRT(msgLvlReqd, "Indexes Arid Type Excl ST RS TID  Flags   RP ");
  DIFFSERV_PRT(msgLvlReqd, "Type-Specific Fields                       \n");
  DIFFSERV_PRT(msgLvlReqd, "------- ---- ---- ---- -- -- --- -------- -- ");
  DIFFSERV_PRT(msgLvlReqd, "-------------------------------------------\n");

  keys.classIndex = 0;                          /* start with first entry */
  keys.classRuleIndex = 0;                      /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    /* part A */
    DIFFSERV_PRT(msgLvlReqd, " %2u %2u  %4u %4s  %1s   %2s %2s %3u %8.8x %2s ", 
                 pRow->key.classIndex, 
                 pRow->key.classRuleIndex, 
                 pRow->arid,
                 dsmibClassRuleTypeStr[pRow->mib.entryType], 
                 dsmibTruthValueStr[pRow->mib.excludeFlag],
                 dsmibStgTypeStr[pRow->mib.storageType],
                 dsmibRowStatusStr[pRow->mib.rowStatus],
                 pRow->tableId,
                 pRow->rowInvalidFlags,
                 dsmibRowPendingStr[pRow->rowPending]);

    /* part B -- depends on entry type */
    switch (pRow->mib.entryType)
    {
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
      DIFFSERV_PRT(msgLvlReqd, "cos=%u\n", 
                   pRow->mib.match.cos);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
      DIFFSERV_PRT(msgLvlReqd, "cos2=%u\n", 
                   pRow->mib.match.cos2);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
      DIFFSERV_PRT(msgLvlReqd, "ipaddr=0x%8.8x ipmask=0x%8.8x\n", 
                   pRow->mib.match.dstIp.addr, pRow->mib.match.dstIp.mask);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
      DIFFSERV_PRT(msgLvlReqd, "ipv6addr=0x");
      for (i=0; i < L7_IP6_ADDR_LEN; i++)
      {
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", pRow->mib.match.dstIpv6.ip6prefix.in6Addr.in6.addr8[i]);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
      DIFFSERV_PRT(msgLvlReqd, "                                            ");
      DIFFSERV_PRT(msgLvlReqd, " ipv6plen=0x%8.8x\n", pRow->mib.match.dstIpv6.ip6prefix.in6PrefixLen);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
      DIFFSERV_PRT(msgLvlReqd, "startport=%u endport=%u\n", 
                   pRow->mib.match.dstL4Port.start, 
                   pRow->mib.match.dstL4Port.end);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
      DIFFSERV_PRT(msgLvlReqd, "macaddr=0x"); 
      for (i = 0; i < L7_MAC_ADDR_LEN-1; i++)
      {
        DIFFSERV_PRT(msgLvlReqd, "%2.2x:", 
                     pRow->mib.match.dstMac.addr[i]);
      }
      DIFFSERV_PRT(msgLvlReqd, "%2.2x ", 
                   pRow->mib.match.dstMac.addr[i]);

      DIFFSERV_PRT(msgLvlReqd, "macmask=0x"); 
      for (i = 0; i < L7_MAC_ADDR_LEN-1; i++)
      {
        DIFFSERV_PRT(msgLvlReqd, "%2.2x:", 
                     pRow->mib.match.dstMac.mask[i]);
      }
      DIFFSERV_PRT(msgLvlReqd, "%2.2x\n", 
                   pRow->mib.match.dstMac.mask[i]);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
      DIFFSERV_PRT(msgLvlReqd, "etype: keyid=%u(%s) value=0x%8.8x\n", 
                   pRow->mib.match.etype.keyid, 
                   dsmibEtypeKeyidStr[pRow->mib.match.etype.keyid],
                   pRow->mib.match.etype.value);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY:
      DIFFSERV_PRT(msgLvlReqd, "match every=%s\n", 
                   dsmibTruthValueStr[pRow->mib.match.matchEvery]);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
      DIFFSERV_PRT(msgLvlReqd, "ipv6 flow label=%u\n", 
                   pRow->mib.match.ipv6FlowLabel);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
      DIFFSERV_PRT(msgLvlReqd, "ip dscp=%u\n", 
                   pRow->mib.match.ipDscp);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
      DIFFSERV_PRT(msgLvlReqd, "ip precedence=%u\n", 
                   pRow->mib.match.ipPrecedence);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
      DIFFSERV_PRT(msgLvlReqd, "tosbits=0x%2.2x tosmask=0x%2.2x\n", 
                   pRow->mib.match.ipTos.bits, pRow->mib.match.ipTos.mask);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
      DIFFSERV_PRT(msgLvlReqd, "protocol num=%u\n", 
                   pRow->mib.match.protocolNum);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS:
      {
        L7_uchar8   name[L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX+1];
        L7_RC_t     rc;

        DIFFSERV_PRT(msgLvlReqd, "ref class: index=%u ", 
                     pRow->mib.match.refClassIndex);
        rc = diffServClassObjectGet(pRow->mib.match.refClassIndex, 
                                    L7_DIFFSERV_CLASS_NAME, name);
        if (rc == L7_SUCCESS)
        {
          DIFFSERV_PRT(msgLvlReqd, "name=\'%s\'", (char *)name);
        }
        DIFFSERV_PRT(msgLvlReqd, "\n");
      }
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
      DIFFSERV_PRT(msgLvlReqd, "ipaddr=0x%8.8x ipmask=0x%8.8x\n", 
                   pRow->mib.match.srcIp.addr, pRow->mib.match.srcIp.mask);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
      DIFFSERV_PRT(msgLvlReqd, "ipv6addr=0x");
      for (i=0; i < L7_IP6_ADDR_LEN; i++)
      {
        DIFFSERV_PRT(msgLvlReqd, "%2.2x", pRow->mib.match.srcIpv6.ip6prefix.in6Addr.in6.addr8[i]);
      }
      DIFFSERV_PRT(msgLvlReqd, "\n");
      DIFFSERV_PRT(msgLvlReqd, "                                            ");
      DIFFSERV_PRT(msgLvlReqd, " ipv6plen=0x%8.8x\n", pRow->mib.match.srcIpv6.ip6prefix.in6PrefixLen);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
      DIFFSERV_PRT(msgLvlReqd, "startport=%u endport=%u\n", 
                   pRow->mib.match.srcL4Port.start, 
                   pRow->mib.match.srcL4Port.end);
      break;
    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
      DIFFSERV_PRT(msgLvlReqd, "macaddr=0x"); 
      for (i = 0; i < L7_MAC_ADDR_LEN-1; i++)
      {
        DIFFSERV_PRT(msgLvlReqd, "%2.2x:", 
                     pRow->mib.match.srcMac.addr[i]);
      }
      DIFFSERV_PRT(msgLvlReqd, "%2.2x ", 
                   pRow->mib.match.srcMac.addr[i]);

      DIFFSERV_PRT(msgLvlReqd, "macmask=0x"); 
      for (i = 0; i < L7_MAC_ADDR_LEN-1; i++)
      {
        DIFFSERV_PRT(msgLvlReqd, "%2.2x:", 
                     pRow->mib.match.srcMac.mask[i]);
      }
      DIFFSERV_PRT(msgLvlReqd, "%2.2x\n", 
                   pRow->mib.match.srcMac.mask[i]);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:
      DIFFSERV_PRT(msgLvlReqd, "vlan id: start=%u end=%u\n", 
                   pRow->mib.match.vlanId.start, pRow->mib.match.vlanId.end);
      break;

    case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
      DIFFSERV_PRT(msgLvlReqd, "vlan id2: start=%u end=%u\n", 
                   pRow->mib.match.vlanId2.start, pRow->mib.match.vlanId2.end);
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

