/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_service_api.c
*
* @purpose    DiffServ component Private MIB Service Table
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
#include "usmdb_mib_diffserv_common.h"
#include "usmdb_mib_diffserv_private_api.h"


extern avlTree_t  dsmibAvlTree[];
extern void *     dsmibIndexSemId;              /* MIB tables index semaphore */

/* local function prototypes */
static void dsmibServicePerfAdd64(L7_ulong64 *pTotal, L7_ulong64 value);
static void dsmibServicePerfSub64(L7_ulong64 *pTotal, L7_ulong64 value);
static void dsmibServicePerfClear64(L7_ulong64 *pValue);
static L7_BOOL dsmibServicePerfIsZero64(L7_ulong64 *pValue);


/*
========================
========================
========================
 
   SERVICE TABLE APIs

========================
========================
========================
*/

/*************************************************************************
* @purpose  Retrieve the maximum number of DiffServ service interfaces
*           supported by the system
*
* @param    pMaxNumIntf @b{(output)} Pointer to max number of service intf
*
* @returns  void
*
* @notes    Only outputs a value if the pMaxNumIntf parm is non-null.
*
* @notes    This value represents the total number of interfaces that
*           can be configured for DiffServ.  On some platforms, this 
*           number is less than the total number of interfaces (ports)
*           in the system.  Note that this is not the value of the 
*           highest intIfNum supported.
*           
* @notes    Each service interface is defined as having an inbound and 
*           outbound data path.
*
* @end
*********************************************************************/
void diffServServiceIntfMaxGet(L7_uint32 *pMaxNumIntf)
{
  if (pMaxNumIntf != L7_NULLPTR)
    *pMaxNumIntf = L7_DIFFSERV_SERVICE_INTF_LIM;
}

/*************************************************************************
* @purpose  Create a new row in the Service Table for the specified 
*           interface and direction
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    activateRow @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Index values in use, row not complete
* @returns  L7_ALREADY_CONFIGURED  Index values in use, row complete
* @returns  L7_FAILURE
*
* @notes    An L7_ERROR response means the service index values are
*           currently in use, but the row is not complete.  This is
*           typically caused when multiple requestors use the same
*           index values and race to create the new row.  The caller
*           should use new index values and re-attempt to create the row.
*
* @notes    An L7_ALREADY_CONFIGURED response means the service index values
*           are currently in use for a completed row.  The caller should
*           use new index values and re-attempt to create the row.
*
* @notes    The row status is initially set to 'not ready' and remains that 
*           way until every object in the row contains a valid value.  If the
*           activateRow parameter is set to L7_TRUE, the row status will then
*           be automatically updated to 'active'.
*
* @end
*********************************************************************/
L7_RC_t diffServServiceCreate(L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                              L7_BOOL activateRow)
{
  L7_RC_t                 rc = L7_FAILURE;
  L7_uint32               dataChanged = L7_FALSE;
  dsmibTableId_t          tableId = DSMIB_TABLE_ID_SERVICE;
  dsmibServiceEntryCtrl_t row, *pRow = L7_NULLPTR;
  L7_uint32               ifIndex;

  DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                           intIfNum, ifDirection, 0, 
                           (L7_uchar8)activateRow);

  do
  {
    /* check inputs */
    if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
      break;
    if (DSMIB_SERVICE_DIR_INDEX_OUT_OF_RANGE(ifDirection))
      break;
    if ((activateRow != L7_FALSE) && (activateRow != L7_TRUE))
      break;

    /* make sure row does not already exist (someone else could have created
     * it first)
     */
    pRow = dsmibServiceRowFind(intIfNum, ifDirection);
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

    /* convert intIfNum to ifIndex */
    if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
      break;
    
    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* initialize row index keys */
    pRow->key.ifIndex = ifIndex;
    pRow->key.ifDirection = ifDirection;

    /* initialize control fields invalid flags */
    pRow->tableId = tableId;
    pRow->rowInvalidFlags = DSMIB_SERVICE_FLAGS_COMMON;
    pRow->intIfNum = intIfNum;
    pRow->lastUpdateTime = 0;
    pRow->rowPending = L7_TRUE;

    /* fill in initial row status and any objects with known defaults */
    pRow->mib.index = ifIndex;
    pRow->mib.direction = ifDirection;

    pRow->mib.ifOperStatus = DSMIB_CFG_DEFAULT_SERVICE_IF_OPER_STATUS;
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
      /* update current table size */
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

  DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                          intIfNum, ifDirection, 0, 
                          (L7_uint32)pRow, (L7_uchar8)rc,
                          (L7_uchar8)activateRow);

  return rc;
}

/*************************************************************************
* @purpose  Delete a row from the Service Table for the specified
*           interface and direction
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE
*
* @end
*********************************************************************/
L7_RC_t diffServServiceDelete(L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
  L7_RC_t                 rc = L7_FAILURE;
  dsmibTableId_t          tableId = DSMIB_TABLE_ID_SERVICE;
  dsmibServiceEntryCtrl_t *pRow = L7_NULLPTR;
  dsmibRowStatus_t        prevRowStatus;
  dsmibPolicyEntryCtrl_t  *pPolicy;

  DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                           intIfNum, ifDirection, 0);

  do
  {
    /* check inputs */
    if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
      break;
    if (DSMIB_SERVICE_DIR_INDEX_OUT_OF_RANGE(ifDirection))
      break;

    /* find node in AVL tree */
    pRow = dsmibServiceRowFind(intIfNum, ifDirection);
    if (pRow == L7_NULLPTR)
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

      if (dsmibRowEventCheck(DSMIB_TABLE_ID_SERVICE, pRow, 
                             prevRowStatus, pRow->mib.rowStatus) != L7_SUCCESS)
        break;
    }

    /* if deleting a service interface that has a policy attached, force the
     * deletion of the corresponding Policy Perf In/Out Table row(s)
     */
    if (pRow->mib.policyIndex != 0)
    {
      if (dsmibPolicyPerfDetach(pRow->mib.policyIndex, intIfNum, ifDirection)
            != L7_SUCCESS)
      {
        LOG_MSG("diffServServiceDelete:  unable to delete Policy Perf Table row "
                "for intf %u, dir \'%s\', policy index %u\n", intIfNum,
                (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN) ? "in" : "out",
                pRow->mib.policyIndex);
        break;
      }
    }

    /* initialize policy row pointer */
    pPolicy = L7_NULLPTR;

    /* check if policy index has ever been set */
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_SERVICE_FLAGS_POLICYINDEX))
    {
      /* find node in the AVL tree */
      pPolicy = dsmibPolicyRowFind(pRow->mib.policyIndex);
      if (pPolicy == L7_NULLPTR)
      {
        rc = L7_ERROR;
        break;
      }
    }

    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* delete the service row AVL tree node */
    rc = dsmibRowDelete(&dsmibAvlTree[tableId], pRow);

    if (rc == L7_SUCCESS)
    {
      /* successful row deletion.
       * NOTE: Do not use pRow from this point on.
       * update current table size
       */
      genStatus.tableSize[tableId]--;

      if (pPolicy != L7_NULLPTR)
      {
        /* decrement the refcount in the policy row */
        pPolicy->refCount--;
      }

      /* make sure change gets picked up when config is saved */
      pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;

      /* NOTE: Unlike the other private MIB tables, there is no reason to
       *       evaluate all policies here, since none of their definitions
       *       have changed in this code path.  The specific policy attached
       *       to this service interface was evaluated above when the row 
       *       was set to 'not in service'.
       */
    }
    else
    {
      /* something's wrong */
      LOG_MSG("diffServServiceDelete:  unable to delete row AVL node for "
              "service intf %u, dir \'%s\'\n", intIfNum,
              (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN) ? "in" : "out");
      rc = L7_FAILURE;
    }

    DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

  } while (0);

  DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                          intIfNum, ifDirection, 0, 
                          (L7_uint32)pRow, (L7_uchar8)rc);

  return rc;
}

/*********************************************************************
* @purpose  Verify that a Service Table row exists for the specified 
*           interface and direction
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServServiceGet(L7_uint32 intIfNum,
                           L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
  dsmibServiceEntryCtrl_t *pRow;

  /* check inputs */
  if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
    return L7_FAILURE;
  if (DSMIB_SERVICE_DIR_INDEX_OUT_OF_RANGE(ifDirection))
    return L7_FAILURE;

  /* find node in AVL tree */
  pRow = dsmibServiceRowFind(intIfNum, ifDirection);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_ERROR;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Service Table
*
* @param    prevIntIfNum    @b{(input)}  Internal interface number to begin
*                                          search
* @param    prevIfDirection @b{(input)}  Interface direction to begin search
* @param    pIntIfNum       @b{(output)} Pointer to next sequential internal 
*                                          interface number value
* @param    pIfDirection    @b{(output)} Pointer to next sequential
*                                          interface direction value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevIntIfNum value of 0 is used to find the first interface
*           entry the table.
*
* @notes    A prevIfDirection value of 0 is used to find the first
*           interface direction entry relative to the specified prevIntIfNum.
*
* @end
*********************************************************************/
L7_RC_t diffServServiceGetNext(L7_uint32 prevIntIfNum, 
                               L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t prevIfDirection,
                               L7_uint32 *pIntIfNum,
                               L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t *pIfDirection)
{
  L7_uint32               prevIfIndex = 0;
  L7_uint32               intIfNum;
  dsmibServiceEntryCtrl_t *pRow;
  dsmibServiceKey_t       keys;

  /* check inputs */
  /* NOTE: Allow any starting index value to be used. */
  if (pIntIfNum == L7_NULLPTR)
    return L7_FAILURE;
  if (pIfDirection == L7_NULLPTR)
    return L7_FAILURE;

  if (prevIntIfNum > 0)
  {
    /* convert prevIntIfNum to prevIfIndex */
    if (nimGetIntfIfIndex(prevIntIfNum, &prevIfIndex) != L7_SUCCESS)
    {
      *pIntIfNum = 0;
      *pIfDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE;
      return L7_FAILURE;
    }
  }

  /* find node in AVL tree
   * NOTE: Need to use direct call here due to AVL_NEXT flag.
   */
  keys.ifIndex = prevIfIndex;
  keys.ifDirection = prevIfDirection;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_SERVICE], 
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
    /* next row not found -- must have reached end of table */
    return L7_ERROR;

  /* convert ifIndex to intIfNum */
  if (nimGetIntfNumber(pRow->key.ifIndex, &intIfNum) != L7_SUCCESS)
  {
    *pIntIfNum = 0;
    *pIfDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE;
    return L7_FAILURE;
  }

  /* found next sequential table row -- set caller's output variables */
  *pIntIfNum = intIfNum;
  *pIfDirection = pRow->key.ifDirection;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Service Table 
*           to determine its legitimacy
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
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
L7_RC_t diffServServiceSetTest(L7_uint32 intIfNum, 
                               L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                               L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_TYPE_t oid, 
                               void *pValue)
{
  dsmibTableId_t          tableId = DSMIB_TABLE_ID_SERVICE;
  dsmibServiceEntryCtrl_t *pRowTrace = L7_NULLPTR;
  size_t                  dsTraceValueLen = 0;
  L7_uchar8               dsTraceValueData[DS_TRACE_OBJ_VAL_LEN_MAX];
  L7_RC_t                 rc;

  rc = dsmibServiceSetTest(intIfNum, ifDirection, oid, pValue,
                           &pRowTrace, &dsTraceValueLen);

  if (rc != L7_SUCCESS)
  {
    /* format the OID value for the trace point */
    dsTraceObjectValFormat(pValue, dsTraceValueLen, dsTraceValueData);

    DS_TRACE_PT_OBJECT_SETTEST_FAIL(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                                    intIfNum, ifDirection, 0, 
                                    (L7_ulong32)pRowTrace, (L7_uchar8)rc,
                                    (L7_uchar8)oid, (L7_uchar8)dsTraceValueLen,
                                    dsTraceValueData);
  }

  return rc;
}

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Service Table 
*           to determine its legitimacy
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
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
L7_RC_t dsmibServiceSetTest(L7_uint32 intIfNum, 
                            L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                            L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_TYPE_t oid, 
                            void *pValue,
                            dsmibServiceEntryCtrl_t **ppRow,
                            size_t *pValueLen)
{
  dsmibServiceEntryCtrl_t *pRow;

  /* check inputs */
  if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
    return L7_FAILURE;
  if (DSMIB_SERVICE_DIR_INDEX_OUT_OF_RANGE(ifDirection))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibServiceRowFind(intIfNum, ifDirection);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  *ppRow = pRow;

  /* compare object against its allowed values */
  switch (oid)
  {
  case L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_POLICY_INDEX:
    {
      L7_uint32 policyIndex = *(L7_uint32 *)pValue;
      dsmibPolicyType_t   polType, allowedPolType;

      *pValueLen = sizeof(policyIndex);

      /* check policy index */
      if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
        return L7_FAILURE;

      /* make sure policy exists and is of proper type for intf direction */
      if (diffServPolicyObjectGet(policyIndex, L7_DIFFSERV_POLICY_TYPE, &polType) 
          != L7_SUCCESS)
        return L7_ERROR;

      if (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
        allowedPolType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN;
      else
        allowedPolType = L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_OUT;

      if (polType != allowedPolType)
        return L7_ERROR;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_STORAGE_TYPE:
    {
      dsmibStorageType_t  storageType = *(dsmibStorageType_t *)pValue;
      *pValueLen = sizeof(storageType);

      if (dsmibStorageTypeIsValid(storageType) != L7_TRUE)
        return L7_ERROR;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_ROW_STATUS:
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
* @purpose  Get the specified object in the Service Table
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(output)} Pointer to object output value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
* @notes    When accessing any counter object, the caller is required to set 
*           the pValue parameter to point to an output variable of type
*           L7_ulong64.  For a 32-bit counter, the pValue->high portion is 
*           always output as 0.
*
*
* @end
*********************************************************************/
L7_RC_t diffServServiceObjectGet(L7_uint32 intIfNum,
                                 L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                 L7_DIFFSERV_SERVICE_TABLE_OBJECT_t oid,
                                 void  *pValue)
{
  dsmibServiceEntryCtrl_t *pRow;
  L7_RC_t                 rc;

  /* check inputs */
  if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
    return L7_FAILURE;
  if (DSMIB_SERVICE_DIR_INDEX_OUT_OF_RANGE(ifDirection))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibServiceRowFind(intIfNum, ifDirection);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  /* check for proper row invalid flag state before proceeding*/
  if ((rc = dsmibServiceObjectGetCheck(pRow, oid)) != L7_SUCCESS)
    return rc;

  /* update the MIB object counts 
   *
   * NOTE:  For continuity, all Service Performance counter values are
   *        updated at once for this interface/direction.  Since this
   *        is processor intensive, check elapsed time between calls
   *        and skip the update if it is considered too soon.
   */

  /* output the current value of the object */
  switch (oid)
  {
  case L7_DIFFSERV_SERVICE_POLICY_INDEX:
    {
      L7_uint32     *pPolicyIndex = pValue;
      *pPolicyIndex = pRow->mib.policyIndex;
    }
    break;

  case L7_DIFFSERV_SERVICE_IF_OPER_STATUS:
    {
      dsmibServiceIfOper_t  *pIfOperStatus = pValue;
      *pIfOperStatus = pRow->mib.ifOperStatus;
    }
    break;

  case L7_DIFFSERV_SERVICE_STORAGE_TYPE:
    {
      dsmibStorageType_t  *pStorageType = pValue;
      *pStorageType = pRow->mib.storageType;
    }
    break;

  case L7_DIFFSERV_SERVICE_ROW_STATUS:
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
* @purpose  Set the specified object in the Service Table
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(output)} Pointer to object set value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    This is a 'wrapper' function used for tracing.  The real code
*           is contained in dsmibPolicyInstObjectSet().
*
* @end
*********************************************************************/
L7_RC_t diffServServiceObjectSet(L7_uint32 intIfNum,
                                 L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                 L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_TYPE_t oid, 
                                 void  *pValue)
{
  dsmibTableId_t          tableId = DSMIB_TABLE_ID_SERVICE;
  dsmibServiceEntryCtrl_t *pRowTrace = L7_NULLPTR;
  size_t                  dsTraceValueLen = 0;
  L7_uchar8               dsTraceValueData[DS_TRACE_OBJ_VAL_LEN_MAX];
  L7_RC_t                 rc;

  rc = dsmibServiceObjectSet(intIfNum, ifDirection, oid, pValue,
                             &pRowTrace, &dsTraceValueLen);

  /* format the OID value for the trace point */
  dsTraceObjectValFormat(pValue, dsTraceValueLen, dsTraceValueData);

  DS_TRACE_PT_OBJECT_SET(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                         intIfNum, ifDirection, 0,
                         (L7_ulong32)pRowTrace, (L7_uchar8)rc,
                         (L7_uchar8)oid, (L7_uchar8)dsTraceValueLen,
                         dsTraceValueData);

  return rc;
}

/*********************************************************************
* @purpose  Set the specified object in the Service Table
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(output)} Pointer to object set value
* @param    ppRow       @b{(output)} Pointer to row ptr output location
* @param    pValueLen   @b{(output)} Pointer to value length output location
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
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
L7_RC_t dsmibServiceObjectSet(L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                              L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_TYPE_t oid, 
                              void *pValue,
                              dsmibServiceEntryCtrl_t **ppRow,
                              size_t *pValueLen)
{
  dsmibServiceEntryCtrl_t *pRow;
  dsmibRowStatus_t        restoreStatus, rowStatusRaw, oldStatus, newStatus;
  L7_RC_t                 rc;
  L7_BOOL                 justCompleted;

  /* check inputs */
  if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
    return L7_FAILURE;
  if (DSMIB_SERVICE_DIR_INDEX_OUT_OF_RANGE(ifDirection))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* reuse 'SetTest' function to check for valid oid and settable value */
  if (diffServServiceSetTest(intIfNum, ifDirection, oid, pValue) != L7_SUCCESS)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibServiceRowFind(intIfNum, ifDirection);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  *ppRow = pRow;

  /* don't do anything if setting same policy index as an existing one */
  if (oid == L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_POLICY_INDEX)
    if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_SERVICE_FLAGS_POLICYINDEX))
      if (*(L7_uint32 *)pValue == pRow->mib.policyIndex)
        return L7_SUCCESS;

  /* save a copy of the old row status for a comparison below */
  restoreStatus = rowStatusRaw = pRow->mib.rowStatus;
  if (dsmibRowStatusObservable(rowStatusRaw, (L7_uint32)pRow->rowInvalidFlags, 
                               &oldStatus) != L7_SUCCESS)
    return L7_FAILURE;

  if (oid != L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_ROW_STATUS)
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

      rc = dsmibRowEventCheck(DSMIB_TABLE_ID_SERVICE, pRow, 
                              oldStatus, pRow->mib.rowStatus);

      /* restore the actual mib.rowStatus to its previous value */
      oldStatus = pRow->mib.rowStatus;
      pRow->mib.rowStatus = restoreStatus;

      /* now check result of the row event check */
      if (rc != L7_SUCCESS)
        return L7_FAILURE;
    }
  }

  /* set the object value and clear its row invalid flag */
  switch (oid)
  {
  case L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_POLICY_INDEX:
    {
      L7_uint32     policyIndex = *(L7_uint32 *)pValue;
      dsmibPolicyEntryCtrl_t *pPolicy;
      L7_uint32     prevPolicyIndex;

      *pValueLen = sizeof(policyIndex);

      /* if overwriting policy index value for this service */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_SERVICE_FLAGS_POLICYINDEX))
      {
        /* decrement the old policy refCount */
        pPolicy = dsmibPolicyRowFind(pRow->mib.policyIndex);
        if (pPolicy == L7_NULLPTR)
          return L7_FAILURE;
        pPolicy->refCount--;             /* do not count this service reference */
      }

      prevPolicyIndex = pRow->mib.policyIndex;
      pRow->mib.policyIndex = policyIndex;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_SERVICE_FLAGS_POLICYINDEX);

      /* increment the new policy refCount */
      pPolicy = dsmibPolicyRowFind(policyIndex);
      if (pPolicy == L7_NULLPTR)
        return L7_FAILURE;
      pPolicy->refCount++;                      /* count this service reference */

      /* when setting a policy attachment, the corresponding Policy Perf
       * In/Out Table row(s) must be created
       *
       * first, delete any perf table rows belonging to the previous policy 
       * attachment, if any
       */
      if (prevPolicyIndex != 0)
      {
        if (dsmibPolicyPerfDetach(prevPolicyIndex, intIfNum, ifDirection)
              != L7_SUCCESS)
        {
          LOG_MSG("diffServServiceObjectSet:  unable to delete Policy Perf Table row "
                  "for intf %u, dir \'%s\', policy index %u\n", intIfNum,
                  (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN) ? "in" : "out",
                  prevPolicyIndex);
          return L7_FAILURE;         
        }
      }

      if (dsmibPolicyPerfAttach(policyIndex, intIfNum, ifDirection) 
           != L7_SUCCESS)
      {
        LOG_MSG("diffServServiceObjectSet:  unable to create Policy Perf Table row "
                "for intf %u, dir \'%s\', policy index %u\n", intIfNum,
                (ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN) ? "in" : "out",
                policyIndex);
        return L7_FAILURE;         
      }

      /* clear out the service performance counter values */
      memset(&pRow->mibPerf, 0, sizeof(pRow->mibPerf));
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_STORAGE_TYPE:
    {
      dsmibStorageType_t storageType = *(dsmibStorageType_t *)pValue;
      *pValueLen = sizeof(storageType);
      pRow->mib.storageType = storageType;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_ROW_STATUS:
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
  if (oid != L7_USMDB_MIB_DIFFSERV_SERVICE_SETTEST_OID_ROW_STATUS)
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

      /* NOTE: Service table does not have an 'index next' value that needs 
       *       to be updated.
       */
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
    if (dsmibRowEventCheck(DSMIB_TABLE_ID_SERVICE, pRow, oldStatus, newStatus)
          != L7_SUCCESS)
      return L7_FAILURE;

    /* for a new row status of 'destroy', it is now time to delete the row */
    if (pRow->mib.rowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY)
      if (diffServServiceDelete(intIfNum, ifDirection) != L7_SUCCESS)
      {
        pRow->mib.rowStatus = restoreStatus;
        LOG_MSG("diffServServiceObjectSet: attempt to destroy the row failed\n");
        return L7_FAILURE;
      }
  }

  return L7_SUCCESS;
}



/*
===================================
===================================
===================================
 
   SERVICE PERFORMANCE TABLE API
                                
===================================
===================================
===================================
*/

/*********************************************************************
* @purpose  Get the specified object from the Service Performance Table
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(output)} Pointer to object output value
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
L7_RC_t diffServServicePerfObjectGet(L7_uint32 intIfNum,
                                     L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                     L7_DIFFSERV_SERVICE_PERF_TABLE_OBJECT_t oid,
                                     void *pValue)
{
  dsmibServiceEntryCtrl_t   *pRow;
  L7_ulong64                *pMibCtr, *pVal;

  /* check inputs */
  if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
    return L7_FAILURE;
  if (DSMIB_SERVICE_DIR_INDEX_OUT_OF_RANGE(ifDirection))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibServiceRowFind(intIfNum, ifDirection);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  /* refresh the MIB counter values for all objects in this Service Perf
   * table row (for consistency)
   *
   * NOTE: these counts are only updated if sufficient time has passed since
   *       the last update occurred
   */
  if (dsmibServicePerfCtrUpdate(pRow) != L7_SUCCESS)
    return L7_FAILURE;

  /* translate MIB counter name to system counter ID and establish
   * a pointer to the MIB object
   */
  switch (oid)
  {
  case L7_DIFFSERV_SERVICE_PERF_HC_OFFERED_OCTETS:
    pMibCtr = &pRow->mibPerf.HCOfferedOctets;
    break;

  case L7_DIFFSERV_SERVICE_PERF_HC_OFFERED_PACKETS:
    pMibCtr = &pRow->mibPerf.HCOfferedPackets;
    break;

  case L7_DIFFSERV_SERVICE_PERF_HC_DISCARDED_OCTETS:
    pMibCtr = &pRow->mibPerf.HCDiscardedOctets;
    break;

  case L7_DIFFSERV_SERVICE_PERF_HC_DISCARDED_PACKETS:
    pMibCtr = &pRow->mibPerf.HCDiscardedPackets;
    break;

  case L7_DIFFSERV_SERVICE_PERF_HC_SENT_OCTETS:
    pMibCtr = &pRow->mibPerf.HCSentOctets;
    break;

  case L7_DIFFSERV_SERVICE_PERF_HC_SENT_PACKETS:
    pMibCtr = &pRow->mibPerf.HCSentPackets;
    break;

  default:
    /* invalid oid */
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

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
====================================
====================================
====================================

   SERVICE TABLE HELPER FUNCTIONS

====================================
====================================
====================================
*/

/*********************************************************************
* @purpose  Obtain pointer to specified Service Table row element
*
* @param    intIfNum        @b{(input)} Internal interface number
* @param    ifDirection     @b{(input)} Interface direction  
*
* @returns  pRow        Pointer to service row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsmibServiceEntryCtrl_t *dsmibServiceRowFind(L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
  L7_uint32               ifIndex;
  dsmibServiceKey_t       keys;
  dsmibServiceEntryCtrl_t *pRow;

  /* convert intIfNum to ifIndex */
  if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
    return L7_NULLPTR;

  /* get access to the policy row */
  keys.ifIndex = ifIndex;
  keys.ifDirection = ifDirection;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_SERVICE], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Checks if a service object value is valid to 'get'
*
* @param    pRow        @b{(input)} Service table row pointer
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
L7_RC_t dsmibServiceObjectGetCheck(dsmibServiceEntryCtrl_t *pRow, 
                                   L7_DIFFSERV_SERVICE_TABLE_OBJECT_t oid)
{
  dsmibServiceFlags_t objFlag;

  switch (oid)
  {
  case L7_DIFFSERV_SERVICE_POLICY_INDEX:
    objFlag = DSMIB_SERVICE_FLAGS_POLICYINDEX;
    break;

  case L7_DIFFSERV_SERVICE_IF_OPER_STATUS:
  case L7_DIFFSERV_SERVICE_STORAGE_TYPE:
  case L7_DIFFSERV_SERVICE_ROW_STATUS:
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
* @purpose  Update all counter values for the Service Performance Table row
*
* @param    pRow        @b{(input)}  Pointer to Service Perf Table row
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    This function does not allow the service perf counters to be
*           updated too frequently (provided the system up time value
*           is available from OSAPI).
*
* @notes    All octets and packets counts are updated at the same time
*           for the specified Service Performance Table row to provide
*           relative consistency of values.
*
* @end
*********************************************************************/
L7_RC_t dsmibServicePerfCtrUpdate(dsmibServiceEntryCtrl_t *pRow)
{
  L7_uint32     policyIndex, intIfNum;
  L7_uint32     lastTime, nextTime;
  L7_uint32     sysUpTime;
  L7_ulong64    oOffered, pOffered, oDiscarded, pDiscarded, oSent, pSent;
  L7_uint32     policyInstIndex;
  L7_uint32     nextPol;
  L7_ulong64    value;

  /* check inputs */
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  policyIndex = pRow->mib.policyIndex;
  intIfNum = pRow->intIfNum;

  /* validate policy index for this service row */
  if (diffServPolicyGet(policyIndex) != L7_SUCCESS)
    return L7_FAILURE;

  lastTime = pRow->lastUpdateTime;
  nextTime = lastTime + DSMIB_SERVICE_PERF_CTR_UPDATE_LIMIT;

  /* refresh the MIB counter values for all objects in this Service Perf
   * table row (for consistency).  Since this is somewhat intensive, 
   * use the system up time to refrain from doing this too frequently,
   * as is the case when someone is collecting all of the row counters
   * in successive calls to this function
   *
   * NOTE: Some OSAPI implementations return L7_FAILURE as the system up
   *       time when they encounter a problem and cannot provide a real value.
   */
  sysUpTime = osapiUpTimeRaw();

  /* force an update if the system up timer is not supported */
  if ((sysUpTime == L7_FAILURE) || 
      ((sysUpTime == 0) && (lastTime == 0)))
    nextTime = sysUpTime;

  /* also force counter update if system up timer has wrapped */
  if (sysUpTime < lastTime)
    nextTime = sysUpTime;

  /* having made the preceding adjustments, compare sys up time to the 
   * next time the counters may be updated; return without doing the
   * update if it is too soon
   */
  if (sysUpTime < nextTime)
    return L7_SUCCESS;


  /* ----- PROCEEDING WITH COUNTER UPDATE ----- */

  /* collect instance counters and sum together for their interface counter
   *         
   * use separate totals for octets and packets
   */

  /* clear the total counts */
  dsmibServicePerfClear64(&oOffered);
  dsmibServicePerfClear64(&pOffered);
  dsmibServicePerfClear64(&oDiscarded);
  dsmibServicePerfClear64(&pDiscarded);
  dsmibServicePerfClear64(&oSent);
  dsmibServicePerfClear64(&pSent);

  /* walk the Policy Instance Table for this policy, using the relevant
   * indexes to access the Policy Perf In/Out Tables
   *
   * only sum the retrieved count if the object get function returned 
   * successfully (an unsuccessful response could mean the instance
   * counter is not supported on this platform)
   */
  policyInstIndex = 0;                        /* start with first instance */
  while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex, 
                                    &nextPol, &policyInstIndex) == L7_SUCCESS)
         && (nextPol == policyIndex))
  {
    /* skip any instance for which a TLV is not currently issued (since the
     * driver will not recognize the instance key)
     */
    if (dsDistillerPolicyInstIsIssued(policyIndex, policyInstIndex, intIfNum)
          != L7_TRUE)
      continue;

    if (pRow->key.ifDirection == L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN)
    {
      if (diffServPolicyPerfInObjectGet(policyIndex, policyInstIndex, intIfNum,
                                        L7_DIFFSERV_POLICY_PERF_IN_HC_OFFERED_OCTETS,
                                        &value) == L7_SUCCESS)
        dsmibServicePerfAdd64(&oOffered, value);

      if (diffServPolicyPerfInObjectGet(policyIndex, policyInstIndex, intIfNum,
                                        L7_DIFFSERV_POLICY_PERF_IN_HC_OFFERED_PACKETS,
                                        &value) == L7_SUCCESS)
        dsmibServicePerfAdd64(&pOffered, value);

      if (diffServPolicyPerfInObjectGet(policyIndex, policyInstIndex, intIfNum,
                                        L7_DIFFSERV_POLICY_PERF_IN_HC_DISCARDED_OCTETS,
                                        &value) == L7_SUCCESS)
        dsmibServicePerfAdd64(&oDiscarded, value);

      if (diffServPolicyPerfInObjectGet(policyIndex, policyInstIndex, intIfNum,
                                        L7_DIFFSERV_POLICY_PERF_IN_HC_DISCARDED_PACKETS,
                                        &value) == L7_SUCCESS)
        dsmibServicePerfAdd64(&pDiscarded, value);

    } /* endif inbound */

    else  /* outbound */
    {
      if (diffServPolicyPerfOutObjectGet(policyIndex, policyInstIndex, intIfNum,
                                        L7_DIFFSERV_POLICY_PERF_OUT_HC_OFFERED_OCTETS,
                                        &value) == L7_SUCCESS)
        dsmibServicePerfAdd64(&oOffered, value);

      if (diffServPolicyPerfOutObjectGet(policyIndex, policyInstIndex, intIfNum,
                                        L7_DIFFSERV_POLICY_PERF_OUT_HC_OFFERED_PACKETS,
                                        &value) == L7_SUCCESS)
        dsmibServicePerfAdd64(&pOffered, value);

      if (diffServPolicyPerfOutObjectGet(policyIndex, policyInstIndex, intIfNum,
                                        L7_DIFFSERV_POLICY_PERF_OUT_HC_DISCARDED_OCTETS,
                                        &value) == L7_SUCCESS)
        dsmibServicePerfAdd64(&oDiscarded, value);

      if (diffServPolicyPerfOutObjectGet(policyIndex, policyInstIndex, intIfNum,
                                        L7_DIFFSERV_POLICY_PERF_OUT_HC_DISCARDED_PACKETS,
                                        &value) == L7_SUCCESS)
        dsmibServicePerfAdd64(&pDiscarded, value);

    } /* endif outbound */

  } /* endwhile */

  /* sent = offered - discarded
   *
   * again, use separate totals for octets and packets
   */
  /* Only do the subtraction if the 'sent' value is non-zero
   * NOTE: Handles case where 'offered' counter not supported
   */
  dsmibServicePerfAdd64(&oSent, oOffered);
  if (dsmibServicePerfIsZero64(&oSent) == L7_FALSE)
    dsmibServicePerfSub64(&oSent, oDiscarded);

  dsmibServicePerfAdd64(&pSent, pOffered);
  if (dsmibServicePerfIsZero64(&pSent) == L7_FALSE)
    dsmibServicePerfSub64(&pSent, pDiscarded);

  /* now set each Service Perf MIB counter object with its resultant value */
  pRow->mibPerf.HCOfferedOctets = oOffered;
  pRow->mibPerf.HCOfferedPackets = pOffered;
  pRow->mibPerf.HCDiscardedOctets = oDiscarded;
  pRow->mibPerf.HCDiscardedPackets = pDiscarded;
  pRow->mibPerf.HCSentOctets = oSent;
  pRow->mibPerf.HCSentPackets = pSent;

  /* remember update time */
  pRow->lastUpdateTime = sysUpTime;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the service interface operational status to 'up'
*
* @param    intIfNum        @b{(input)}  Internal interface number
* @param    ifDirection     @b{(input)}  Interface direction  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function is only used for communication within the 
*           DiffServ component.  The ifOperStatus is not settable via
*           user configuration.
*
* @end
*********************************************************************/
L7_RC_t dsmibServiceIfOperStatusUp(L7_uint32 intIfNum,
                            L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
  dsmibServiceEntryCtrl_t *pRow;

  /* check inputs */
  if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
    return L7_FAILURE;
  if (DSMIB_SERVICE_DIR_INDEX_OUT_OF_RANGE(ifDirection))
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibServiceRowFind(intIfNum, ifDirection);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  /* set the operational status to 'up' */
  pRow->mib.ifOperStatus = L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_UP;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets the service interface operational status to 'down'
*
* @param    intIfNum        @b{(input)}  Internal interface number
* @param    ifDirection     @b{(input)}  Interface direction  
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function is only used for communication within the 
*           DiffServ component.  The ifOperStatus is not settable via
*           user configuration.
*
* @end
*********************************************************************/
L7_RC_t dsmibServiceIfOperStatusDown(L7_uint32 intIfNum,
                            L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
  dsmibServiceEntryCtrl_t *pRow;

  /* check inputs */
  if (DSMIB_SERVICE_INTIFNUM_INDEX_OUT_OF_RANGE(intIfNum))
    return L7_FAILURE;
  if (DSMIB_SERVICE_DIR_INDEX_OUT_OF_RANGE(ifDirection))
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibServiceRowFind(intIfNum, ifDirection);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  /* set the operational status to 'down' */
  pRow->mib.ifOperStatus = L7_USMDB_MIB_DIFFSERV_IF_OPER_STATUS_DOWN;

  return L7_SUCCESS;
}


/*
=======================================
=======================================
=======================================

   SERVICE TABLE DEBUG SHOW FUNCTION

=======================================
=======================================
=======================================
*/

/*********************************************************************
* @purpose  Display the current DiffServ Service Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsmibServiceTableShow(void)
{
  L7_uint32     msgLvlReqd;
  dsmibServiceEntryCtrl_t *pRow;
  dsmibServiceKey_t       keys;
  avlTree_t     *pTree = &dsmibAvlTree[DSMIB_TABLE_ID_SERVICE];
  L7_uint32     count;

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  diffServServiceIntfMaxGet(&count);

  DIFFSERV_PRT(msgLvlReqd, "\nMax Num Service Intfs: %u\n\n", count);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Service Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Service Table (contains %u entries):\n\n", count);

  DIFFSERV_PRT(msgLvlReqd, "ifIdx ifDir pIdx Op ST RS TID  Flags   RP intf updtime  ");
  DIFFSERV_PRT(msgLvlReqd, "Ctr: Offered(hi) Offered(lo) Discard(hi) Discard(lo) ");
  DIFFSERV_PRT(msgLvlReqd, " Sent(hi)    Sent(lo)  \n");
  DIFFSERV_PRT(msgLvlReqd, "----- ----- ---- -- -- -- --- -------- -- ---- -------- ");
  DIFFSERV_PRT(msgLvlReqd, "---- ----------- ----------- ----------- ----------- ");
  DIFFSERV_PRT(msgLvlReqd, "----------- -----------\n");

  keys.ifIndex = 0;
  keys.ifDirection = 0;                          /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    /* part A */
    DIFFSERV_PRT(msgLvlReqd, " %3u   %3s   %2u  %2s %2s %2s %2u  %8.8x %2s  %2u  %8.8x ", 
                 pRow->key.ifIndex, 
                 dsmibServiceIfDir[pRow->key.ifDirection], 
                 pRow->mib.policyIndex, 
                 dsmibServiceIfOperStatus[pRow->mib.ifOperStatus], 
                 dsmibStgTypeStr[pRow->mib.storageType], 
                 dsmibRowStatusStr[pRow->mib.rowStatus], 
                 pRow->tableId, 
                 pRow->rowInvalidFlags, 
                 dsmibRowPendingStr[pRow->rowPending],
                 pRow->intIfNum,
                 pRow->lastUpdateTime);
    
    /* part B -- octet counts */
    DIFFSERV_PRT(msgLvlReqd, "oct: %11u %11u %11u %11u %11u %11u\n", 
                 pRow->mibPerf.HCOfferedOctets.high, pRow->mibPerf.HCOfferedOctets.low, 
                 pRow->mibPerf.HCDiscardedOctets.high, pRow->mibPerf.HCDiscardedOctets.low, 
                 pRow->mibPerf.HCSentOctets.high, pRow->mibPerf.HCSentOctets.low);

    /* part C -- packet counts */
    DIFFSERV_PRT(msgLvlReqd, "                                                        "); /* spacer */
    DIFFSERV_PRT(msgLvlReqd, "pkt: %11u %11u %11u %11u %11u %11u\n", 
                 pRow->mibPerf.HCOfferedPackets.high, pRow->mibPerf.HCOfferedPackets.low, 
                 pRow->mibPerf.HCDiscardedPackets.high, pRow->mibPerf.HCDiscardedPackets.low, 
                 pRow->mibPerf.HCSentPackets.high, pRow->mibPerf.HCSentPackets.low);

    /* update search keys for next pass */
    keys = pRow->key;
  }
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}


/*
=============================================
=============================================
=============================================

   SERVICE TABLE STATIC INTERNAL FUNCTIONS

=============================================
=============================================
=============================================
*/

/*********************************************************************
* @purpose  Adds an unsigned 64-bit value into a total value
*
* @param    pTotal      @b{(input)} Pointer to total count being summed
* @param    value       @b{(input)} Value being added to total
*
* @returns  void        
*
* @notes    Only intended for use within Service Perf operation.
*
* @end
*********************************************************************/
static void dsmibServicePerfAdd64(L7_ulong64 *pTotal, L7_ulong64 value)
{
  L7_uint32     origLow = pTotal->low;

  pTotal->low += value.low;
  pTotal->high += value.high;
  if (pTotal->low < origLow)                    /* low part wrapped */
    pTotal->high++;                             /* add carry        */
}

/*********************************************************************
* @purpose  Subtracts an unsigned 64-bit value from a total value
*
* @param    pTotal      @b{(input)} Pointer to total count being reduced
* @param    value       @b{(input)} Value being subtracted from total
*
* @returns  void        
*
* @notes    Only intended for use within Service Perf operation.
*
* @notes    Does not try to guard against an underflow.
*
* @end
*********************************************************************/
static void dsmibServicePerfSub64(L7_ulong64 *pTotal, L7_ulong64 value)
{
  L7_uint32     origLow = pTotal->low;

  pTotal->low -= value.low;
  pTotal->high -= value.high;
  if (pTotal->low > origLow)                    /* low part needs borrow*/
    pTotal->high--;                             /* subtract borrow      */
}

/*********************************************************************
* @purpose  Zero out an unsigned 64-bit value
*
* @param    pValue      @b{(input)} Pointer to value being cleared
*
* @returns  void        
*
* @notes    Only intended for use within Service Perf operation.
*
* @end
*********************************************************************/
static void dsmibServicePerfClear64(L7_ulong64 *pValue)
{
  pValue->low = 0;
  pValue->high = 0;
}

/*********************************************************************
* @purpose  Check if an unsigned 64-bit value is zero
*
* @param    pValue      @b{(input)} Pointer to value being checked
*
* @returns  void        
*
* @notes    Only intended for use within Service Perf operation.
*
* @end
*********************************************************************/
static L7_BOOL dsmibServicePerfIsZero64(L7_ulong64 *pValue)
{
  return ((pValue->high == 0) && (pValue->low == 0)) ? L7_TRUE : L7_FALSE;
}

