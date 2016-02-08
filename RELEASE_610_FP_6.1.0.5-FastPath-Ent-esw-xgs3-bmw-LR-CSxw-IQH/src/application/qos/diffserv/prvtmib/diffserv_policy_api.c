/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_policy_api.c
*
* @purpose    DiffServ component Private MIB Policy Table
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
#include "comm_mask.h"
#include "usmdb_mib_diffserv_common.h"
#include "usmdb_mib_diffserv_private_api.h"


/* external references */
extern avlTree_t  dsmibAvlTree[];
extern void *     dsmibIndexSemId;              /* MIB tables index semaphore */

/* Number of bytes in mask */
#define DSMIB_POLICY_INDEX_INDICES      (((DSMIB_POLICY_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)

/* Mask storage definitions */
typedef struct
{
  L7_uchar8   value[DSMIB_POLICY_INDEX_INDICES];
} DSMIB_POLICY_INDEX_MASK_t;

/* NOTE: Index 0 is reserved and must not be used */
static DSMIB_POLICY_INDEX_MASK_t  policyIndexInUseMask;

static L7_uint32  policyIndexNextFree;          /* next available policy index*/


/*
=======================
=======================
=======================
 
   POLICY TABLE APIs

=======================
=======================
=======================
*/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new row in the Policy Table
*
* @param    pPolicyIndex @b{(output)} Pointer to policy index value
*
* @returns  void
*
* @notes    Only outputs a value if the pPolicyIndex parm is non-null.
*
* @notes    A *pPolicyIndex output value of 0 means the table is full.
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
*
* @end
*********************************************************************/
void diffServPolicyIndexNext(L7_uint32 *pPolicyIndex)
{
  /* check inputs */
  if (pPolicyIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

  /* always output a value of 0 if the MIB table size is at maximum */
  if (dsmibMibTableSizeCheck(DSMIB_TABLE_ID_POLICY) != L7_SUCCESS)
  {
    /* table full */
    *pPolicyIndex = 0;
  }
  else
  {
    dsmibPolicyIndexNextRead(pPolicyIndex);
  }

  DIFFSERV_SEMA_GIVE(dsmibIndexSemId);
}

/*************************************************************************
* @purpose  Retrieve the maximum policy index value allowed for the 
*           Policy Table
*
* @param    pIndexMax   @b{(output)} Pointer to index max output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is purely informational.  The corresponding table 'IndexNext'
*           function must be used to obtain the current index value.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyIndexMaxGet(L7_uint32 *pIndexMax)
{
  /* check inputs */
  if (pIndexMax == L7_NULLPTR)
    return L7_FAILURE;

  *pIndexMax = DSMIB_POLICY_INDEX_MAX;
  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Create a new row in the Policy Table for the specified index
*
* @param    policyIndex @b{(input)} Policy index
* @param    activateRow @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Index value in use, row not complete
* @returns  L7_ALREADY_CONFIGURED  Index value in use, row complete
* @returns  L7_FAILURE
*
* @notes    An L7_ERROR response means the policy index value is currently 
*           in use, but the row is not complete.  This is typically
*           caused when multiple requestors obtain the same "index next"
*           value and race to create the new row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_ALREADY_CONFIGURED response means the policy index value
*           is currently in use for a completed row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    The row status is initially set to 'not ready' and remains that 
*           way until every object in the row contains a valid value.  If the
*           activateRow parameter is set to L7_TRUE, the row status will then
*           be automatically updated to 'active'.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyCreate(L7_uint32 policyIndex, 
                             L7_BOOL activateRow)
{
  L7_RC_t                rc = L7_FAILURE;
  L7_uint32              dataChanged = L7_FALSE;
  dsmibTableId_t         tableId = DSMIB_TABLE_ID_POLICY;
  dsmibPolicyEntryCtrl_t row, *pRow = L7_NULLPTR;

  DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                           policyIndex, 0, 0, 
                           (L7_uchar8)activateRow);

  do
  {
    /* check inputs */
    if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
      break;
    if ((activateRow != L7_FALSE) && (activateRow != L7_TRUE))
      break;

    /* verify index value matches expected value (i.e., current 'IndexNext')
     * for lowest-order creation index
     */
    if (dsmibPolicyIndexNextVerify(policyIndex) != L7_SUCCESS)
      break;

    /* make sure row does not already exist (someone else could have created
     * it first)
     */
    pRow = dsmibPolicyRowFind(policyIndex);
    if (pRow != L7_NULLPTR)           /* row exists */
    {
      rc = (DSMIB_ROW_IS_COMPLETE(pRow) == L7_TRUE) ? L7_ALREADY_CONFIGURED : L7_ERROR;
      break;
    }

    /* make sure there is room in the table */
    if (dsmibMibTableSizeCheck(tableId) != L7_SUCCESS)
      break;

    /* setup a temporary MIB row element structure */
    pRow = &row;
    memset(pRow, 0, sizeof(*pRow));

    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* initialize row index keys */
    pRow->key.policyIndex = policyIndex;

    /* initialize control fields invalid flags */
    pRow->tableId = tableId;
    pRow->rowInvalidFlags = DSMIB_POLICY_FLAGS_COMMON;
    pRow->refCount = 0;
    pRow->instCount = 0;
    pRow->rowPending = L7_TRUE;

    /* fill in initial row status and any objects with known defaults */
    pRow->mib.index = policyIndex;
    pRow->mib.instIndexNextFree = DSMIB_AUTO_INCR_INDEX_NEXT_START;
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

      dataChanged = L7_TRUE;            /* config change occurred */
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

  /* NOTE: Not sending notify event callbacks here since policy does not
   *       contain any meaningful instance definition yet.
   */

  DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                          policyIndex, 0, 0, 
                          (L7_uint32)pRow, (L7_uchar8)rc,
                          (L7_uchar8)activateRow);

  return rc;
}

/*************************************************************************
* @purpose  Delete a row from the Policy Table for the specified index
*
* @param    policyIndex @b{(input)} Policy index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified index, or is
*                         referenced by one or more services (interfaces)
* @returns  L7_FAILURE
*
* @notes    A policy cannot be deleted if it is currently assigned to an    
*           interface.
*
* @notes    Deletes all policy instances defined for this policy.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyDelete(L7_uint32 policyIndex)
{
  L7_RC_t                rc = L7_FAILURE;
  dsmibTableId_t         tableId = DSMIB_TABLE_ID_POLICY;
  dsmibPolicyEntryCtrl_t *pRow = L7_NULLPTR;
  L7_BOOL                needNotify = L7_FALSE;
  dsmibRowStatus_t       prevRowStatus;
  dsmibPolicyType_t      policyType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE;
  L7_uchar8              policyName[L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1];
  L7_BOOL                prevRowPending;

  DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                           policyIndex, 0, 0);

  do
  {
    /* check inputs */
    if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
      break;

    /* find node in AVL tree */
    pRow = dsmibPolicyRowFind(policyIndex);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_ERROR;
      break;
    }

    /* check references -- don't delete if policy is referenced by
     * one or more interface service attachments
     */
    if (pRow->refCount != 0)
    {
      rc = L7_ERROR;
      break;
    }

    /* check with callback registrants to make sure policy is not currently in use */
    if (diffServDeleteApprovalQuery(policyIndex) != L7_SUCCESS)
    {
      rc = L7_ERROR;
      break;
    }

    /* change an 'ACTIVE' row status to 'NOT_IN_SERVICE' prior to row deletion
     * (this transition needed for proper device cleanup)
     */
    prevRowStatus = pRow->mib.rowStatus;
    if (prevRowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      pRow->mib.rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;

      if (dsmibRowEventCheck(DSMIB_TABLE_ID_POLICY, pRow, 
                             prevRowStatus, pRow->mib.rowStatus) != L7_SUCCESS)
        break;
    }

    /* copy the policy type before the row is deleted */
    policyType = pRow->mib.type;

    /* save row pending flag for change notification */
    prevRowPending = pRow->rowPending;

    /* delete all instances belonging to this policy
     *
     * NOTE:  A single call to dsDistillerEvaluateAll() will occur after
     *        all of the instances are deleted.
     */
    if (diffServPolicyInstDeleteAll(policyIndex) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

    /* copy the policy name for use in notify event callbacks and error message below */
    strcpy((char *)policyName, (char *)pRow->mib.name);

    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* delete the policy row AVL tree node */
    rc = dsmibRowDelete(&dsmibAvlTree[tableId], pRow);

    if (rc == L7_SUCCESS)
    {
      /* successful row deletion -- update next free index value
       * NOTE: Must do this under index semaphore control.
       * NOTE: Do not use pRow from this point on.
       */
      L7_INTF_CLRMASKBIT(policyIndexInUseMask, policyIndex);
      dsmibPolicyIndexNextUpdate();

      /* update current table size */
      genStatus.tableSize[tableId]--;

      /* make sure change gets picked up when config is saved */
      pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;

      /* always call distiller after every row deletion */
      rc = dsDistillerEvaluateAll();

      /* set flag if callbacks are needed
       * (want to do this regardless of distiller outcome)
       *
       * NOTE: No point in sending a notification if deleting a row
       *       that was never fully completed.
       */
      if (prevRowPending == L7_FALSE)
        needNotify = L7_TRUE;
    }
    else
    {
      /* something's wrong */
      LOG_MSG("diffServPolicyDelete: couldn't delete row AVL node for policy "
              "\'%s\'\n", (char *)policyName);
      rc = L7_FAILURE;
    }

    DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

  } while (0);

  if (needNotify == L7_TRUE)
  {
    /* send notify event callbacks for this policy */
    diffServDeleteNotify(policyIndex, policyType, policyName);
  }

  DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                          policyIndex, 0, 0, 
                          (L7_uint32)pRow, (L7_uchar8)rc);

  return rc;
}

/*********************************************************************
* @purpose  Verify that a Policy Table row exists for the specified index
*
* @param    policyIndex @b{(input)} Policy index
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyGet(L7_uint32 policyIndex)
{
  dsmibPolicyEntryCtrl_t *pRow;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;

  /* find node in AVL tree */
  pRow = dsmibPolicyRowFind(policyIndex);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_ERROR;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Policy Table
*
* @param    prevPolicyIndex  @b{(input)}  Policy index to begin search
* @param    pPolicyIndex     @b{(output)} Pointer to next sequential policy
*                                           index value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevPolicyIndex value of 0 is used to find the first entry in
*           the table.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyGetNext(L7_uint32 prevPolicyIndex, 
                              L7_uint32 *pPolicyIndex)
{
  dsmibPolicyEntryCtrl_t *pRow;
  dsmibPolicyKey_t       keys;

  /* check inputs */
  /* NOTE: Allow any starting index value to be used. */
  if (pPolicyIndex == L7_NULLPTR)
    return L7_FAILURE;

  /* find node in AVL tree
   * NOTE: Need to use direct call here due to AVL_NEXT flag.
   */
  keys.policyIndex = prevPolicyIndex;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_POLICY], 
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
    /* next row not found -- must have reached end of table */
    return L7_ERROR;

  /* found next sequential table row -- set caller's output variables */
  *pPolicyIndex = pRow->key.policyIndex;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Reports whether a Policy is being used in any Service
*
* @param    policyIndex      @b{(input)}  Policy index to check
*
* @returns  L7_TRUE     policy is being referenced in at least one service
* @returns  L7_FALSE    policy is not being referenced any services
*
* @end
*********************************************************************/
L7_BOOL diffServIsPolicyReferenced(L7_uint32 policyIndex)
{
  dsmibPolicyEntryCtrl_t *pRow;

  pRow = dsmibPolicyRowFind(policyIndex);

  if ((pRow != L7_NULLPTR) && (pRow->refCount != 0))
    return L7_TRUE;
  else
    return L7_FALSE;
}

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Policy Table 
*           to determine its legitimacy
*
* @param    policyIndex @b{(input)} Policy index
* @param    oid         @b{(input)} Object identifier
* @param    pValue      @b{(input)} Pointer to proposed value to be set
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Proposed value invalid
* @returns  L7_FAILURE  
*
* @notes    This is a 'wrapper' function used for tracing.  The real code
*           is contained in dsmibPolicyInstSetTest().
*
* @end
*********************************************************************/
L7_RC_t diffServPolicySetTest(L7_uint32 policyIndex, 
                              L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE_t oid, 
                              void *pValue)
{
  dsmibTableId_t              tableId = DSMIB_TABLE_ID_POLICY;
  dsmibPolicyEntryCtrl_t      *pRowTrace = L7_NULLPTR;
  size_t                      dsTraceValueLen = 0;
  L7_uchar8                   dsTraceValueData[DS_TRACE_OBJ_VAL_LEN_MAX];
  L7_RC_t                     rc;

  rc = dsmibPolicySetTest(policyIndex, oid, pValue,
                          &pRowTrace, &dsTraceValueLen);

  if (rc != L7_SUCCESS)
  {
    /* format the OID value for the trace point */
    dsTraceObjectValFormat(pValue, dsTraceValueLen, dsTraceValueData);

    DS_TRACE_PT_OBJECT_SETTEST_FAIL(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                                    policyIndex, 0, 0, 
                                    (L7_ulong32)pRowTrace, (L7_uchar8)rc,
                                    (L7_uchar8)oid, (L7_uchar8)dsTraceValueLen,
                                    dsTraceValueData);
  }

  return rc;
}

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Policy Table 
*           to determine its legitimacy
*
* @param    policyIndex @b{(input)}  Policy index
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(input)}  Pointer to proposed value to be set
* @param    ppRow       @b{(output)} Pointer to row ptr output location
* @param    pValueLen   @b{(output)} Pointer to value length output location
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
* @notes    The ppRow and pValueLen parms are used for tracing.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicySetTest(L7_uint32 policyIndex, 
                           L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE_t oid, 
                           void *pValue,
                           dsmibPolicyEntryCtrl_t **ppRow,
                           size_t *pValueLen)
{
  dsmibPolicyEntryCtrl_t *pRow;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibPolicyRowFind(policyIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  *ppRow = pRow;

  /* compare object against its allowed values */
  switch (oid)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_NAME:
    {
      L7_uint32   len;
      L7_uchar8   *pName = *(L7_uchar8 **)pValue;
      L7_uint32   idx;
      L7_uchar8   usedName[L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1];

      len = (L7_uint32)strlen((char *)pName);
      *pValueLen = len;
      if ((len == 0) || (len > L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX))
        return L7_ERROR;

      /* check all other existing policy rows to make sure name is unique
       * (bypass this checking during special config mode)
       */
      if (dsmibSpecialConfigModeGet() != L7_TRUE)
      {
        idx = 0;                                  /* start from first row */
        while (diffServPolicyGetNext(idx, &idx) == L7_SUCCESS)
        {
          /* skip own row */
          if (idx == pRow->key.policyIndex)
            continue;

          if (diffServPolicyObjectGet(idx, L7_DIFFSERV_POLICY_NAME, usedName)
                != L7_SUCCESS)
            continue;

          /* compare against name in existing row */
          if (strcmp((char *)pName, (char *)usedName) == 0)
            return L7_ERROR;
        }
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE:
    {
      dsmibPolicyType_t  type = *(dsmibPolicyType_t *)pValue;
      *pValueLen = sizeof(type);

      if ((type == L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_NONE) ||
          (type >= L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_TOTAL))
        return L7_ERROR;

      /* once set, this value is not allowed to change */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_POLICY_FLAGS_TYPE))
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_STORAGE_TYPE:
    {
      dsmibStorageType_t  storageType = *(dsmibStorageType_t *)pValue;
      *pValueLen = sizeof(storageType);

      if (dsmibStorageTypeIsValid(storageType) != L7_TRUE)
        return L7_ERROR;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_ROW_STATUS:
    {
      dsmibRowStatus_t  rowStatus = *(dsmibRowStatus_t *)pValue;
      *pValueLen = sizeof(rowStatus);

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
* @purpose  Get the specified object from the Policy Table
*
* @param    policyIndex @b{(input)}  Policy index
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(output)} Pointer to the object output value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value not yet established
* @returns  L7_FAILURE  Improper usage or other malfunction
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyObjectGet(L7_uint32 policyIndex, 
                                L7_DIFFSERV_POLICY_TABLE_OBJECT_t oid,
                                void *pValue)
{
  dsmibPolicyEntryCtrl_t *pRow;
  L7_RC_t                rc;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibPolicyRowFind(policyIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  
  /* check for proper row invalid flag state before proceeding*/
  if ((rc = dsmibPolicyObjectGetCheck(pRow, oid)) != L7_SUCCESS)
    return rc;
  
  /* output the current value of the object */
  switch (oid)
  {
  case L7_DIFFSERV_POLICY_NAME:
    {
      L7_uchar8    *pName = pValue;
      strcpy((char *)pName, (char *)pRow->mib.name);
    }
    break;

  case L7_DIFFSERV_POLICY_TYPE:
    {
      dsmibPolicyType_t  *pType = pValue;
      *pType = pRow->mib.type;
    }
    break;

  case L7_DIFFSERV_POLICY_STORAGE_TYPE:
    {
      dsmibStorageType_t  *pStorageType = pValue;
      *pStorageType = pRow->mib.storageType;
    }
    break;

  case L7_DIFFSERV_POLICY_ROW_STATUS:
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
* @purpose  Set the specified object in the Policy Table
*
* @param    policyIndex @b{(input)}  Policy index
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(output)} Pointer to the object set value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    This is a 'wrapper' function used for tracing.  The real code
*           is contained in dsmibPolicyInstObjectSet().
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyObjectSet(L7_uint32 policyIndex, 
                                L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE_t oid, 
                                void *pValue)
{
  dsmibTableId_t              tableId = DSMIB_TABLE_ID_POLICY;
  dsmibPolicyEntryCtrl_t      *pRowTrace = L7_NULLPTR;
  size_t                      dsTraceValueLen = 0;
  L7_uchar8                   dsTraceValueData[DS_TRACE_OBJ_VAL_LEN_MAX];
  L7_RC_t                     rc;

  rc = dsmibPolicyObjectSet(policyIndex, oid, pValue,
                            &pRowTrace, &dsTraceValueLen);

  /* format the OID value for the trace point */
  dsTraceObjectValFormat(pValue, dsTraceValueLen, dsTraceValueData);

  DS_TRACE_PT_OBJECT_SET(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                         policyIndex, 0, 0,
                         (L7_ulong32)pRowTrace, (L7_uchar8)rc,
                         (L7_uchar8)oid, (L7_uchar8)dsTraceValueLen,
                         dsTraceValueData);

  return rc;
}

/*********************************************************************
* @purpose  Set the specified object in the Policy Table
*
* @param    policyIndex @b{(input)}  Policy index
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(output)} Pointer to the object set value
* @param    ppRow       @b{(output)} Pointer to row ptr output location
* @param    pValueLen   @b{(output)} Pointer to value length output location
*
* @returns  L7_SUCCESS  
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
L7_RC_t dsmibPolicyObjectSet(L7_uint32 policyIndex, 
                             L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE_t oid, 
                             void *pValue,
                             dsmibPolicyEntryCtrl_t **ppRow,
                             size_t *pValueLen)
{
  dsmibPolicyEntryCtrl_t *pRow;
  dsmibRowStatus_t       restoreStatus, rowStatusRaw, oldStatus, newStatus;
  L7_uchar8              oldName[L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1];
  L7_RC_t                rc;
  L7_BOOL                justCompleted;

  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* reuse 'SetTest' function to check for valid oid and settable value */
  rc = diffServPolicySetTest(policyIndex, oid, pValue);
  if (rc != L7_SUCCESS)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibPolicyRowFind(policyIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  *ppRow = pRow;
  
  /* save a copy of the old row status for a comparison below */
  restoreStatus = rowStatusRaw = pRow->mib.rowStatus;
  if (dsmibRowStatusObservable(rowStatusRaw, (L7_uint32)pRow->rowInvalidFlags, 
                               &oldStatus) != L7_SUCCESS)
    return L7_FAILURE;

  if ((oid != L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_NAME) &&
      (oid != L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_ROW_STATUS))
  {
    /* NOTE: Temporarily change an 'ACTIVE' row to 'NOT_IN_SERVICE' when 
     *       setting any row object (other than the name or row status objects).
     *       The 'oldStatus' value is changed here, so that re-activation
     *       automatically occurs below after the object value is set.
     */
    if (oldStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      /* set the actual rowStatus to notInService for the row event check */
      pRow->mib.rowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;

      rc = dsmibRowEventCheck(DSMIB_TABLE_ID_POLICY, pRow, 
                              oldStatus, pRow->mib.rowStatus);

      /* restore the actual mib.rowStatus to its previous value */
      oldStatus = pRow->mib.rowStatus;
      pRow->mib.rowStatus = restoreStatus;

      /* now check result of the row event check */
      if (rc != L7_SUCCESS)
        return L7_FAILURE;
    }
  }

  memset(oldName, 0, sizeof(oldName));

  /* set the object value and clear its row invalid flag */
  switch (oid)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_NAME:
    {
      L7_uchar8    *pName = *(L7_uchar8 **)pValue;

      /* save old name for notification event */
      osapiStrncpySafe(oldName, pRow->mib.name, sizeof(oldName));

      *pValueLen = strlen((char *)pName);
      strcpy((char *)pRow->mib.name, (char *)pName);
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_FLAGS_NAME);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_TYPE:
    {
      dsmibPolicyType_t  type = *(dsmibPolicyType_t *)pValue;
      *pValueLen = sizeof(type);

      pRow->mib.type = type;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_POLICY_FLAGS_TYPE);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_STORAGE_TYPE:
    {
      dsmibStorageType_t storageType = *(dsmibStorageType_t *)pValue;
      *pValueLen = sizeof(storageType);
      pRow->mib.storageType = storageType;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_ROW_STATUS:
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

  }

  /* set the data changed flag to L7_TRUE as the MIB object value is set */
  pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;
  
  /* when setting any object other than row status, check for a possible
   * row completion to cause the row status to be updated
   */
  if (oid != L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_ROW_STATUS)
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
      L7_INTF_SETMASKBIT(policyIndexInUseMask, policyIndex);
      dsmibPolicyIndexNextUpdate();

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
    if (dsmibRowEventCheck(DSMIB_TABLE_ID_POLICY, pRow, oldStatus, newStatus)
          != L7_SUCCESS)
      return L7_FAILURE;

    /* for a new row status of 'destroy', it is now time to delete the row */
    if (pRow->mib.rowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY)
      if (diffServPolicyDelete(policyIndex) != L7_SUCCESS)
      {
        pRow->mib.rowStatus = restoreStatus;
        LOG_MSG("diffServPolicyObjectSet: attempt to destroy the row failed\n");
        return L7_FAILURE;
      }
  }

  /* signal policy name change notification 
   * (but not when initially assigning a name to the policy)
   */
  if ((oid == L7_USMDB_MIB_DIFFSERV_POLICY_SETTEST_OID_NAME) &&
      (strlen(oldName) > 0))
  {
    diffServChangeNotify(policyIndex, DIFFSERV_EVENT_RENAME, oldName);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Translate a policy name into its associated Policy Table index
*
* @param    stringPtr     @b{(input)}  Policy name string pointer
* @param    pPolicyIndex  @b{(output)} Pointer to policy index value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is a supplemental function provided to assist the user
*           interface code with name-to-index translation.  Use the
*           name 'get' function to translate in the reverse direction.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyNameToIndex(L7_uchar8 *stringPtr, 
                                  L7_uint32 *pPolicyIndex)
{
  L7_uint32   len;
  L7_uint32   idx;
  L7_uchar8   policyName[L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1];

  /* check inputs */
  if (stringPtr == L7_NULLPTR)
    return L7_FAILURE;
  if (pPolicyIndex == L7_NULLPTR)
    return L7_FAILURE;

  /* check the length of the policy name string */
  len = (L7_uint32)strlen((char *)stringPtr);
  if ((len == 0) || (len > L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX))
    return L7_FAILURE;

  /* walk through existing policy rows looking for a name match */
  idx = 0;                                  /* start from first row */
  while (diffServPolicyGetNext(idx, &idx) == L7_SUCCESS)
  {
    if (diffServPolicyObjectGet(idx, L7_DIFFSERV_POLICY_NAME, policyName)
          != L7_SUCCESS)
      continue;

    /* compare against name in existing row */
    if (strcmp((char *)stringPtr, (char *)policyName) == 0)
    {
      *pPolicyIndex = idx;
      return L7_SUCCESS;
    }
  }

  *pPolicyIndex = 0;
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Create a single TLV containing all service policy
*           instances within it and return the data in TLV block 
*           storage provided by the caller.
*
* @param    policyIndex @{(input)} Policy index
* @param    intIfNum    @{(input)} Internal interface number   
* @param    ifDirection @{(input)} Interface direction         
* @param    tlvHandle   @{(input)} handle of the TLV block_to be
*                                  populated with policy information         
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    The information distilled into the TLV represents a snapshot of the data in
*           the policy database at time of invocation.
*
* @end
*********************************************************************/
L7_RC_t diffServPolicyTlvGet(L7_uint32 policyIndex, L7_uint32 intIfNum,
                             L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection, L7_tlvHandle_t tlvHandle)
{
  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
    return L7_FAILURE;
  if (DSMIB_SERVICE_DIR_INDEX_OUT_OF_RANGE(ifDirection))
    return L7_FAILURE;
  if (tlvHandleIsValid(tlvHandle) != L7_TRUE)
    return L7_FAILURE;

  return(dsDistillerPolicyTlvGet(policyIndex, intIfNum, ifDirection, tlvHandle));
}

/*
===================================
===================================
===================================

   POLICY TABLE HELPER FUNCTIONS

===================================
===================================
===================================
*/

/*********************************************************************
* @purpose  Initialize policy index next value
*
* @param    void
*
* @returns  void  
*
* @notes    Also initializes the policy index in-use flags.
*
* @end
*********************************************************************/
void dsmibPolicyIndexNextInit(void)
{
  /* initialize index in-use bitmask */
  memset(&policyIndexInUseMask, 0, sizeof(policyIndexInUseMask));

  /* establish initial index next value */
  dsmibPolicyIndexNextUpdate();
}

/*********************************************************************
* @purpose  Provide the current value of the policy index next variable
*
* @param    pNextFree       @b{(output)} Pointer to next free index value
*
* @returns  void
*
* @notes    Only outputs a value if the pNextFree parm is non-null.
*       
* @notes    Does not guarantee this index value will be valid at the time
*           a row create is attempted.
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
void dsmibPolicyIndexNextRead(L7_uint32 *pNextFree)
{
  if (pNextFree != L7_NULLPTR)
    *pNextFree = policyIndexNextFree;
}

/*********************************************************************
* @purpose  Determine next available policy index value  
*
* @param    void
*
* @returns  void  
*
* @notes    Always looks for first index not already in use, starting with 1.
*       
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
void dsmibPolicyIndexNextUpdate(void)
{
  L7_uint32     i, imax;

  for (i = 1, imax = DSMIB_POLICY_INDEX_MAX; i <= imax; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully completed.
     */
    if (L7_INTF_ISMASKBITSET(policyIndexInUseMask, i) == L7_FALSE)
      break;
  }
  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > imax)
    i = 0;

  policyIndexNextFree = i;
}

/*********************************************************************
* @purpose  Verifies the specified policy index value matches its
*           current 'IndexNext' value
*
* @param    policyIndex @b{(input)} Policy index to be checked
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
L7_RC_t dsmibPolicyIndexNextVerify(L7_uint32 policyIndex)
{
  L7_uint32     currentIndex;

  if (policyIndex == 0)
    return L7_FAILURE;

  /* skip this checking if currently in 'special config mode' */
  if (dsmibSpecialConfigModeGet() == L7_TRUE)
    return L7_SUCCESS;

  dsmibPolicyIndexNextRead(&currentIndex);

  return (policyIndex == currentIndex) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Policy Table row element
*
* @param    policyIndex @b{(input)} Policy index
*
* @returns  pRow        Pointer to policy row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsmibPolicyEntryCtrl_t *dsmibPolicyRowFind(L7_uint32 policyIndex)
{
  dsmibPolicyKey_t        keys;
  dsmibPolicyEntryCtrl_t  *pRow;

  /* get access to the policy row */
  keys.policyIndex = policyIndex;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_POLICY], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Checks if a policy object value is valid to 'get'
*
* @param    pRow        @b{(input)} Policy table row pointer
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
L7_RC_t dsmibPolicyObjectGetCheck(dsmibPolicyEntryCtrl_t *pRow, 
                                  L7_DIFFSERV_POLICY_TABLE_OBJECT_t oid)
{
  dsmibPolicyFlags_t objFlag;

  switch (oid)
  {
  case L7_DIFFSERV_POLICY_NAME:
    objFlag = DSMIB_POLICY_FLAGS_NAME;
    break;

  case L7_DIFFSERV_POLICY_TYPE:
    objFlag = DSMIB_POLICY_FLAGS_TYPE;
    break;

  case L7_DIFFSERV_POLICY_STORAGE_TYPE:
  case L7_DIFFSERV_POLICY_ROW_STATUS: 
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
* @purpose  Check for any policy constraints for the specified interface
*
* @param    policyIndex @b{(input)}  Policy index             
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
* @param    *pIsValid   @b{(output)} Pointer to validity result output location
*
* @returns  L7_SUCCESS  Function completed, but must check *pIsValid for result  
* @returns  L7_FAILURE  Function failed
*
* @notes    The purpose of this function is to check the policy-related
*           attributes versus the characteristics of the interface.  It 
*           does not factor in the other variables necessary for activating
*           a policy, but rather is one part of the equation.
*
* @notes    Does not explicitly check for a link up condition.  If the 
*           link is not up, the NIM call may fail to provide a data rate,
*           which affects the output value.  It is up to the caller to 
*           check the link state to determine whether the interface is
*           actually up or down, rather than infer it from the output value.
*
* @end
*********************************************************************/
L7_RC_t dsmibPolicyAttachConstraintCheck(L7_uint32 policyIndex, 
                                         L7_uint32 intIfNum,
                                         L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                         L7_BOOL *pIsValid)
{
  L7_uint32   nextPolicyIndex, policyInstIndex, nextPolicyInstIndex;
  L7_uint32   policyAttrIndex;
  dsmibPolicyAttrEntryCtrl_t *pRow;

  /* NOTE:  There are no particular constraints on inbound policy attachments.
   *        If the interface direction type is outbound, both the bandwidth
   *        allocation and shaping attributes need to be compared against 
   *        the link rate capacity of the interface.
   */

  if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
  {
    /* indicate policy attachment is OK for inbound direction */
    *pIsValid = L7_TRUE;
    return L7_SUCCESS;
  }

  /* set output value to 'false' until proven otherwise */
  *pIsValid = L7_FALSE;

  /* for each policy instance in this policy */
  policyInstIndex = 0;                        /* start with first instance */
  while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex, 
                                    &nextPolicyIndex, &policyInstIndex) 
                                                         == L7_SUCCESS) &&
         (nextPolicyIndex == policyIndex))
  {
    if (policyInstIndex == 0)
      break;

    /* for all attributes belonging to this policy-class instance */
    policyAttrIndex = 0;                     /* start with first attribute */
    while ((diffServPolicyAttrGetNext(policyIndex, policyInstIndex, 
                                      policyAttrIndex, &nextPolicyIndex, 
                                      &nextPolicyInstIndex, &policyAttrIndex)
                                                            == L7_SUCCESS) &&
           (nextPolicyIndex == policyIndex) && 
           (nextPolicyInstIndex == policyInstIndex))
    {
      if (policyAttrIndex == 0)
        break;

      pRow = dsmibPolicyAttrRowFind(policyIndex, policyInstIndex, policyAttrIndex);
      if (pRow == L7_NULLPTR)
        return L7_FAILURE;
        
      switch (pRow->mib.entryType)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:      
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
      case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
        break;

      default:
        /* invalid policy attribute type */
        return L7_FAILURE;
        /*PASSTHRU*/

      } /* endswitch */

    } /* endwhile policyAttr */
    
  } /* endwhile policyInst */
  
  /* making it this far means everything checked out OK, so indicate this 
   * to the caller by setting the output value to 'true'
   */
  *pIsValid = L7_TRUE;
  return L7_SUCCESS;
}


/*
======================================
======================================
======================================

   POLICY TABLE DEBUG SHOW FUNCTION

======================================
======================================
======================================
*/

/*********************************************************************
* @purpose  Display the current DiffServ Policy Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsmibPolicyTableShow(void)
{
  L7_uint32              msgLvlReqd;
  dsmibPolicyEntryCtrl_t *pRow;
  dsmibPolicyKey_t       keys;
  avlTree_t              *pTree = &dsmibAvlTree[DSMIB_TABLE_ID_POLICY];
  L7_uint32              count;

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  diffServPolicyIndexNext(&count);

  DIFFSERV_PRT(msgLvlReqd, "\nPolicy Index Next: %u\n\n", count);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Policy Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Policy Table (contains %u entries):\n\n", count);

  DIFFSERV_PRT(msgLvlReqd, "Index Type Next ST RS TID  Flags   RP ");
  DIFFSERV_PRT(msgLvlReqd, "RefCt  InstCt Policy-Name                   \n");
  DIFFSERV_PRT(msgLvlReqd, "----- ---- ---- -- -- --- -------- -- ");
  DIFFSERV_PRT(msgLvlReqd, "------ ------ ------------------------------\n");

  keys.policyIndex = 0;                          /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    /* part A */
    DIFFSERV_PRT(msgLvlReqd, " %3u  %4s %3u  %2s %2s %2u  %8.8x %2s ", 
                 pRow->key.policyIndex, 
                 dsmibPolicyTypeStr[pRow->mib.type], 
                 pRow->mib.instIndexNextFree, 
                 dsmibStgTypeStr[pRow->mib.storageType], 
                 dsmibRowStatusStr[pRow->mib.rowStatus], 
                 pRow->tableId, 
                 pRow->rowInvalidFlags,
                 dsmibRowPendingStr[pRow->rowPending]);
    /* part B */
    DIFFSERV_PRT(msgLvlReqd, " %4u   %4u  \'%-s\'\n", 
                 pRow->refCount, pRow->instCount, (char *)pRow->mib.name);
    
    /* update search keys for next pass */
    keys = pRow->key;
  }
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}
                             

