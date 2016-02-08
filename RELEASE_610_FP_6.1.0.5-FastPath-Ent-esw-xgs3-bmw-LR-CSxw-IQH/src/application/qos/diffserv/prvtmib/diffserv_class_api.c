/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_class_api.c
*
* @purpose    DiffServ component Private MIB Class Table
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
#include "usmdb_mib_diffserv_private_api.h"


/* Number of bytes in mask */
#define DSMIB_CLASS_INDEX_INDICES       (((DSMIB_CLASS_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)

/* Mask storage definitions */
typedef struct
{
  L7_uchar8   value[DSMIB_CLASS_INDEX_INDICES];
} DSMIB_CLASS_INDEX_MASK_t;

/* NOTE: Index 0 is reserved and must not be used */
static DSMIB_CLASS_INDEX_MASK_t         classIndexInUseMask;

static L7_uint32  classIndexNextFree;   /* next available class index */

L7_char8   *pDefaultClassName = DSMIB_CFG_DEFAULT_DEFAULTCLASS_NAME;
                                                /* name of default class */

/*
======================
======================
======================

   CLASS TABLE APIs

======================
======================
======================
*/

/*************************************************************************
* @purpose  Obtain the next available index value to be used when creating  
*           a new row in the Class Table
*
* @param    pClassIndex @b{(output)} Pointer to class index value
*
* @returns  void
*
* @notes    Only outputs a value if the pClassIndex parm is non-null.
*
* @notes    A *pClassIndex output value of 0 means the table is full.
*
* @notes    The index value remains free until used in a subsequent create
*           request.  When two or more requestors get the same index value,
*           only the first one to successfully create a new row in this table
*           gets to keep it.  All other requestors' create requests will
*           fail due to the index being in use, so they must call this
*           function again to get a new value.
*
* @notes    Although an index value is considered in use once a
*           successful row create occurs, the next index value is not
*           revealed until the row is either completed or deleted.
*
* @end
*********************************************************************/
void diffServClassIndexNext(L7_uint32 *pClassIndex)
{
  /* check inputs */
  if (pClassIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

  /* always output a value of 0 if the MIB table size is at maximum */
  if (dsmibMibTableSizeCheck(DSMIB_TABLE_ID_CLASS) != L7_SUCCESS)
  {
    /* table full */
    *pClassIndex = 0;
  }
  else
  {
    dsmibClassIndexNextRead(pClassIndex);
  }

  DIFFSERV_SEMA_GIVE(dsmibIndexSemId);
}

/*************************************************************************
* @purpose  Retrieve the maximum class index value allowed for the 
*           Class Table
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
L7_RC_t diffServClassIndexMaxGet(L7_uint32 *pIndexMax)
{
  /* check inputs */
  if (pIndexMax == L7_NULLPTR)
    return L7_FAILURE;

  *pIndexMax = DSMIB_CLASS_INDEX_MAX;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a new row in the Class Table for the specified index
*
* @param    classIndex  @b{(input)} Class index
* @param    activateRow @b{(input)} Activate row when ready flag
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR               Index value in use, row not complete
* @returns  L7_ALREADY_CONFIGURED  Index value in use, row complete
* @returns  L7_FAILURE
*
* @notes    An L7_ERROR response means the class index value is currently 
*           in use, but the row is not complete.  This is typically
*           caused when multiple requestors obtain the same "index next"
*           value and race to create the new row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    An L7_ALREADY_CONFIGURED response means the class index value
*           is currently in use for a completed row.  The caller should
*           get a new index value and re-attempt to create the row.
*
* @notes    The row status is initially represented as 'not ready' and remains 
*           that way until every object in the row contains a valid value. 
*           If the activateRow parameter is set to L7_TRUE, the row status 
*           will then be automatically updated to 'active', otherwise it will
*           change to 'not in service'.
*
* @end
*********************************************************************/
L7_RC_t diffServClassCreate(L7_uint32 classIndex, 
                            L7_BOOL activateRow)
{
  L7_RC_t                rc = L7_FAILURE;
  L7_uint32              dataChanged = L7_FALSE;
  dsmibTableId_t         tableId = DSMIB_TABLE_ID_CLASS;
  dsmibClassEntryCtrl_t  row, *pRow = L7_NULLPTR;

  DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                           classIndex, 0, 0, 
                           (L7_uchar8)activateRow);

  do
  {
    /* check inputs */
    if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
      break;
    if ((activateRow != L7_FALSE) && (activateRow != L7_TRUE))
      break;

    /* verify index value matches expected value (i.e., current 'IndexNext')
     * for lowest-order creation index
     */
    if (dsmibClassIndexNextVerify(classIndex) != L7_SUCCESS)
      break;

    /* make sure row does not already exist (someone else could have created
     * it first)
     */
    pRow = dsmibClassRowFind(classIndex);
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
    pRow->key.classIndex = classIndex;

    /* initialize control fields invalid flags */
    pRow->tableId = tableId;
    pRow->rowInvalidFlags = DSMIB_CLASS_FLAGS_COMMON; /*sets aclType, aclNum flags for now*/
    pRow->refCount = 0;
    pRow->ruleCount = 0;
    pRow->colorRefCount = 0;
    pRow->refClassIndex = DSMIB_CLASS_REF_INDEX_NONE;
    pRow->rowPending = L7_TRUE;

    /* fill in initial row status and any objects with known defaults */
    pRow->mib.index = classIndex;
    pRow->mib.type = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_NONE;
    pRow->mib.ruleIndexNextFree = DSMIB_AUTO_INCR_INDEX_NEXT_START;
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

  DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                          classIndex, 0, 0, 
                          (L7_uint32)pRow, (L7_uchar8)rc,
                          (L7_uchar8)activateRow);

  return rc;
}

/*************************************************************************
* @purpose  Delete a row from the Class Table for the specified index
*
* @param    classIndex  @b{(input)} Class index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row does not exist for the specified index, or
*                         is currently being referenced
* @returns  L7_FAILURE
*
* @notes    A class cannot be deleted if it is currently referenced by one
*           or more policies.
*
* @notes    A class cannot be deleted if it is referenced by another class.
*
* @notes    Deletes all class rules defined for this class.
*
* @end
*********************************************************************/
L7_RC_t diffServClassDelete(L7_uint32 classIndex)
{
  L7_RC_t               rc = L7_FAILURE;
  dsmibTableId_t        tableId = DSMIB_TABLE_ID_CLASS;
  dsmibClassEntryCtrl_t *pRow = L7_NULLPTR;
  dsmibRowStatus_t      prevRowStatus;
  L7_uchar8             className[L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX+1];

  DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                           classIndex, 0, 0);

  do
  {
    /* check inputs */
    if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
      break;

    /* find node in AVL tree */
    pRow = dsmibClassRowFind(classIndex);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_ERROR;
      break;
    }

    /* check references -- don't delete if class is referenced by a policy
     * definition or by another class
     */
    if (pRow->refCount != 0)
    {
      rc = L7_ERROR;
      break;
    }

    /* check color references -- don't delete if class is referenced by any
     * policy policing color awareness attribute
     */
    if (pRow->colorRefCount != 0)
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

      if (dsmibRowEventCheck(DSMIB_TABLE_ID_CLASS, pRow, 
                             prevRowStatus, pRow->mib.rowStatus) != L7_SUCCESS)
        break;
    }

    /* delete all rules belonging to this class
     *
     * NOTE:  A single call to dsDistillerEvaluateAll() will occur after
     *        all of the rules are deleted.
     *
     * NOTE:  A callback notify event is not needed for class deletion,
     *        since no policies can have an active reference to this class.
     */
    if (diffServClassRuleDeleteAll(classIndex) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }

    /* copy the class name for use in error message below */
    strcpy((char *)className, (char *)pRow->mib.name);

    DIFFSERV_SEMA_TAKE(dsmibIndexSemId, L7_WAIT_FOREVER);

    /* delete the class row AVL tree node */
    rc = dsmibRowDelete(&dsmibAvlTree[tableId], pRow);

    if (rc == L7_SUCCESS)
    {
      /* successful row deletion -- update next free index value
       * NOTE: Must do this under index semaphore control.
       * NOTE: Do not use pRow from this point on.
       */
      L7_INTF_CLRMASKBIT(classIndexInUseMask, classIndex);
      dsmibClassIndexNextUpdate();

      /* update current table size */
      genStatus.tableSize[tableId]--;

      /* make sure change gets picked up when config is saved */
      pDiffServCfgData_g->cfgHdr.dataChanged = L7_TRUE;

      /* always call distiller after every row deletion */
      rc = dsDistillerEvaluateAll();
    }
    else
    {
      /* something's wrong */
      LOG_MSG("diffServClassDelete: couldn't delete row AVL node for class "
              "\'%s\'\n", (L7_char8 *)className);
      rc = L7_FAILURE;
    }

    DIFFSERV_SEMA_GIVE(dsmibIndexSemId);

  } while (0);

  DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                          classIndex, 0, 0, 
                          (L7_uint32)pRow, (L7_uchar8)rc);

  return rc;
}

/*********************************************************************
* @purpose  Verify that a Class Table row exists for the specified index
*
* @param    classIndex  @b{(input)} Class index
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClassGet(L7_uint32 classIndex)
{
  dsmibClassEntryCtrl_t *pRow;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;

  /* find node in AVL tree */
  pRow = dsmibClassRowFind(classIndex);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_ERROR;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Class Table
*
* @param    prevClassIndex  @b{(input)}  Class index to begin search
* @param    pClassIndex     @b{(output)} Pointer to next sequential class
*                                          index value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Reached end of table
* @returns  L7_FAILURE  
*
* @notes    A prevClassIndex value of 0 is used to find the first entry in
*           the table.
*
* @end
*********************************************************************/
L7_RC_t diffServClassGetNext(L7_uint32 prevClassIndex, 
                             L7_uint32 *pClassIndex)
{
  dsmibClassEntryCtrl_t *pRow;
  dsmibClassKey_t       keys;

  /* check inputs */
  /* NOTE: Allow any starting index value to be used. */
  if (pClassIndex == L7_NULLPTR)
    return L7_FAILURE;

  /* find node in AVL tree
   * NOTE: Need to use direct call here due to AVL_NEXT flag.
   */
  keys.classIndex = prevClassIndex;

  pRow = (dsmibClassEntryCtrl_t *)
           dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_CLASS], 
                        &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
    /* next row not found -- must have reached end of table */
    return L7_ERROR;

  /* found next sequential table row -- set caller's output variables */
  *pClassIndex = pRow->key.classIndex;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Class Table 
*           to determine its legitimacy
*
* @param    classIndex  @b{(input)} Class index
* @param    oid         @b{(input)} Object identifier
* @param    pValue      @b{(input)} Pointer to proposed value to be set
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Proposed value invalid
* @returns  L7_FAILURE  
*
* @notes    This is a 'wrapper' function used for tracing.  The real code
*           is contained in dsmibClassSetTest().
*
* @end
*********************************************************************/
L7_RC_t diffServClassSetTest(L7_uint32 classIndex, 
                             L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE_t oid, 
                             void *pValue)
{
  dsmibTableId_t        tableId = DSMIB_TABLE_ID_CLASS;
  dsmibClassEntryCtrl_t *pRowTrace = L7_NULLPTR;
  size_t                dsTraceValueLen = 0;
  L7_uchar8             dsTraceValueData[DS_TRACE_OBJ_VAL_LEN_MAX];
  L7_RC_t               rc;

  rc = dsmibClassSetTest(classIndex, oid, pValue,
                         &pRowTrace, &dsTraceValueLen);

  if (rc != L7_SUCCESS)
  {
    /* format the OID value for the trace point */
    dsTraceObjectValFormat(pValue, dsTraceValueLen, dsTraceValueData);

    DS_TRACE_PT_OBJECT_SETTEST_FAIL(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                                    classIndex, 0, 0, 
                                    (L7_ulong32)pRowTrace, (L7_uchar8)rc,
                                    (L7_uchar8)oid, (L7_uchar8)dsTraceValueLen,
                                    dsTraceValueData);
  }

  return rc;
}

/*********************************************************************
* @purpose  Test the proposed value of a settable object in the Class Table 
*           to determine its legitimacy
*
* @param    classIndex  @b{(input)}  Class index
* @param    oid         @b{(input)}  Object identifier
* @param    pValue      @b{(input)}  Pointer to proposed value to be set
* @param    ppRow       @b{(output)} Pointer to row ptr output location
* @param    pValueLen   @b{(output)} Pointer to value length output location
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Proposed value invalid
* @returns  L7_FAILURE  
*
* @notes    Only tests objects with max access types READ-CREATE or READ-WRITE,
*           thus all other access types return L7_FAILURE.
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
L7_RC_t dsmibClassSetTest(L7_uint32 classIndex, 
                          L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE_t oid, 
                          void *pValue,
                          dsmibClassEntryCtrl_t **ppRow,
                          size_t *pValueLen)
{
  dsmibClassEntryCtrl_t *pRow;
  dsmibClassType_t      classType;
  L7_RC_t               rc;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibClassRowFind(classIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  *ppRow = pRow;

  dsmibClassConvertOidToType(oid, &classType);
  rc = dsmibClassFeaturePresentCheck(classType);
  if (rc != L7_SUCCESS)
    return rc;

  /* compare object against its allowed values */
  switch (oid)
  {

  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_NAME:
    {
      L7_uint32   len;
      L7_uchar8   *pName = *(L7_uchar8 **)pValue;
      L7_uint32   idx;
      L7_uchar8   usedName[L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX+1];

      len = (L7_uint32)strlen((char *)pName);
      *pValueLen = len;
      if ((len == 0) || (len > L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX))
        return L7_ERROR;

      /* don't allow the default class name to be used */
      if (strcmp((char *)pName, (char *)pDefaultClassName) == 0)
        return L7_ERROR;

      /* check all other existing class rows to make sure name is unique
       * (bypass this checking during special config mode)
       */
      if (dsmibSpecialConfigModeGet() != L7_TRUE)
      {
        idx = 0;                                  /* start from first row */
        while (diffServClassGetNext(idx, &idx) == L7_SUCCESS)
        {
          /* skip own row */
          if (idx == pRow->key.classIndex)
            continue;

          if (diffServClassObjectGet(idx, L7_DIFFSERV_CLASS_NAME, usedName)
                != L7_SUCCESS)
            continue;

          /* compare against name in existing row */
          if (strcmp((char *)pName, (char *)usedName) == 0)
            return L7_ERROR;
        }
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE:
    {
      dsmibClassType_t  type = *(dsmibClassType_t *)pValue;
      L7_BOOL           aclPresent;

      *pValueLen = sizeof(type);

      if ((type == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_NONE) ||
          (type >= L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_TOTAL))
        return L7_ERROR;

      rc = dsmibClassFeaturePresentCheck(type);
      if (rc != L7_SUCCESS)
        return rc;

      if (type == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
      {
        aclPresent = diffServIsAclPresent();
        if (aclPresent != L7_TRUE)
          return L7_FAILURE;
      }

      /* once set, this value is not allowed to change */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_FLAGS_TYPE))
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_L3_PROTO:
    {
      dsmibClassL3Protocol_t l3Proto = *(dsmibClassL3Protocol_t *)pValue;

      *pValueLen = sizeof(dsmibClassL3Protocol_t);

      if ((l3Proto == L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_NONE) ||
          (l3Proto >= L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_TOTAL))
        return L7_ERROR;

      rc = dsmibClassProtoFeaturePresentCheck(l3Proto);
      if (rc != L7_SUCCESS)
        return rc;

      /* once set, this value is not allowed to change */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_FLAGS_L3PROTO))
        return L7_FAILURE;
    }
    break;

case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ACL_TYPE:
    {
      dsmibClassAclType_t aclType = *(dsmibClassAclType_t *)pValue;

      *pValueLen = sizeof(aclType);

      /* check for ACL validity only if the aclNum field is already set */
      if (dsmibSpecialConfigModeGet() != L7_TRUE)
        if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_FLAGS_ACLNUM))
          if (diffServAclNumCheckValid(aclType, pRow->mib.aclNum) != L7_SUCCESS)
            return L7_ERROR;

      /* once set, this value is not allowed to change */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_FLAGS_ACLTYPE))
        return L7_FAILURE;

      /* only valid if class type is currently set to 'acl' */
      if (pRow->mib.type != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ACL_NUMBER:
    {
      L7_uint32 aclNum = *(L7_uint32 *)pValue;

      *pValueLen = sizeof(aclNum);

      /* check for ACL validity only if the aclType field is already set */
      if (dsmibSpecialConfigModeGet() != L7_TRUE)
        if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_FLAGS_ACLTYPE))
          if (diffServAclNumCheckValid(pRow->mib.aclType, aclNum) != L7_SUCCESS)
            return L7_ERROR;

      /* once set, this value is not allowed to change */
      if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_FLAGS_ACLNUM))
        return L7_FAILURE;

      /* only valid if class type is currently set to 'acl' */
      if (pRow->mib.type != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
        return L7_FAILURE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_STORAGE_TYPE:
    {
      dsmibStorageType_t  storageType = *(dsmibStorageType_t *)pValue;

      *pValueLen = sizeof(storageType);

      if (dsmibStorageTypeIsValid(storageType) != L7_TRUE)
        return L7_ERROR;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ROW_STATUS:
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

  } /* endswitch */

  /* NOTE: Put any common checking here if value depends on row status state.*/

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the specified object from the Class Table
*
* @param    classIndex  @b{(input)} Class index
* @param    oid         @b{(input)} Object identifier 
* @param    pValue      @b{(output)} Pointer to object output value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Object value not yet established
* @returns  L7_FAILURE  Improper usage or other malfunction
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClassObjectGet(L7_uint32 classIndex,
                               L7_DIFFSERV_CLASS_TABLE_OBJECT_t oid,
                               void *pValue)
{
  dsmibClassEntryCtrl_t *pRow;
  L7_RC_t               rc;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibClassRowFind(classIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  /* check for proper row invalid flag state before proceeding*/
  if ((rc = dsmibClassObjectGetCheck(pRow, oid)) != L7_SUCCESS)
    return rc;

  /* provide the current value of the object */
  switch (oid)
  {
  case L7_DIFFSERV_CLASS_NAME:
    {
      L7_uchar8    *pName = pValue;
      strcpy((char *)pName, (char *)pRow->mib.name);
    }
    break;

  case L7_DIFFSERV_CLASS_TYPE:
    {
      dsmibClassType_t  *pType = pValue;
      *pType = pRow->mib.type;
    }
    break;

  case L7_DIFFSERV_CLASS_L3_PROTO:
    {
      dsmibClassL3Protocol_t  *pL3Proto = pValue;
      *pL3Proto = pRow->mib.l3Protocol;
    }
    break;

  case L7_DIFFSERV_CLASS_ACLTYPE:
    {
      dsmibClassAclType_t *pAclType = pValue;

      /* the aclType is only meaningful for a class type of 'acl' */
      if (pRow->mib.type != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
        return L7_FAILURE;

      *pAclType = pRow->mib.aclType;
    }
    break;

  case L7_DIFFSERV_CLASS_ACLNUM:
    {
      L7_uint32     *pAclNum = pValue;

      /* the aclNum is only meaningful for a class type of 'acl' */
      if (pRow->mib.type != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
        return L7_FAILURE;

      *pAclNum = pRow->mib.aclNum;
    }
    break;

  case L7_DIFFSERV_CLASS_STORAGE_TYPE:
    {
      dsmibStorageType_t  *pStorageType = pValue;
      *pStorageType = pRow->mib.storageType;
    }
    break;

  case L7_DIFFSERV_CLASS_ROW_STATUS:
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
* @purpose  Set the specified object in the Class Table
*
* @param    classIndex  @b{(input)} Class index
* @param    oid         @b{(input)} Object identifier 
* @param    pValue      @b{(output)} Pointer to object set value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    This is a 'wrapper' function used for tracing.  The real code
*           is contained in dsmibClassObjectSet().
*
* @end
*********************************************************************/
L7_RC_t diffServClassObjectSet(L7_uint32 classIndex,
                               L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE_t oid, 
                               void *pValue)
{
  L7_RC_t                     rc;
  dsmibTableId_t              tableId = DSMIB_TABLE_ID_CLASS;
  dsmibClassEntryCtrl_t       *pRowTrace = L7_NULLPTR;
  size_t                      dsTraceValueLen = 0;
  L7_uchar8                   dsTraceValueData[DS_TRACE_OBJ_VAL_LEN_MAX];

  rc = dsmibClassObjectSet(classIndex, oid, pValue,
                           &pRowTrace, &dsTraceValueLen);

  /* format the OID value for the trace point */
  dsTraceObjectValFormat(pValue, dsTraceValueLen, dsTraceValueData);

  DS_TRACE_PT_OBJECT_SET(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                         classIndex, 0, 0,
                         (L7_ulong32)pRowTrace, (L7_uchar8)rc,
                         (L7_uchar8)oid, (L7_uchar8)dsTraceValueLen,
                         dsTraceValueData);

  return rc;
}

/*********************************************************************
* @purpose  Set the specified object in the Class Table
*
* @param    classIndex  @b{(input)}  Class index
* @param    oid         @b{(input)}  Object identifier 
* @param    pValue      @b{(output)} Pointer to object set value
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
L7_RC_t dsmibClassObjectSet(L7_uint32 classIndex,
                            L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE_t oid, 
                            void *pValue,
                            dsmibClassEntryCtrl_t **ppRow,
                            size_t *pValueLen)
{
  dsmibClassEntryCtrl_t *pRow;
  dsmibRowStatus_t      restoreStatus, rowStatusRaw, oldStatus, newStatus;
  L7_RC_t               rc;
  L7_BOOL               justCompleted;

  /* check inputs */
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;
  if (pValue == L7_NULLPTR)
    return L7_FAILURE;

  /* reuse 'SetTest' function to check for valid oid and settable value */
  if (diffServClassSetTest(classIndex, oid, pValue) != L7_SUCCESS)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibClassRowFind(classIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  *ppRow = pRow;

  /* save a copy of the old row status for a comparison below */
  restoreStatus = rowStatusRaw = pRow->mib.rowStatus;
  if (dsmibRowStatusObservable(rowStatusRaw, (L7_uint32)pRow->rowInvalidFlags, 
                               &oldStatus) != L7_SUCCESS)
    return L7_FAILURE;

  if ((oid != L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_NAME) &&
      (oid != L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ROW_STATUS))
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

      rc = dsmibRowEventCheck(DSMIB_TABLE_ID_CLASS, pRow, 
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
  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_NAME:
    {
      L7_uchar8    *pName = *(L7_uchar8 **)pValue;

      *pValueLen = strlen((char *)pName);

      strcpy((char *)pRow->mib.name, (char *)pName);
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_FLAGS_NAME);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE:
    {
      dsmibClassType_t  type = *(dsmibClassType_t *)pValue;

      *pValueLen = sizeof(type);

      pRow->mib.type = type;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_FLAGS_TYPE);

      /* for a class type other than 'acl', clear the aclType, aclNum 
       * flags too
       */
      if (pRow->mib.type != L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
      {
        pRow->mib.aclType = L7_USMDB_MIB_DIFFSERV_CLASS_ACLTYPE_NONE; /* not used */
        DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_FLAGS_ACLTYPE);

        pRow->mib.aclNum = 0;                                         /* not used */
        DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_FLAGS_ACLNUM);
      }
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_L3_PROTO:
    {
      dsmibClassL3Protocol_t  l3Proto = *(dsmibClassL3Protocol_t *)pValue;

      *pValueLen = sizeof(l3Proto);

      pRow->mib.l3Protocol = l3Proto;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_FLAGS_L3PROTO);

    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ACL_TYPE:
    {
      dsmibClassAclType_t aclType = *(dsmibClassAclType_t *)pValue;

      *pValueLen = sizeof(aclType);

      /* read the contents of the designated access control list and 
       * set up corresponding rows in the Class Rule Table
       *
       * NOTE: This is only done when the type 'acl' class is first
       *       created.  There is no attempt to keep the class definition
       *       in sync with any subsequent modifications to the ACL contents.
       *       The only way to change an 'acl' class definition is to delete
       *       it and create a new class.
       *
       * NOTE: A class type 'acl' creation does not need to send a policy callback
       *       notify event; the 'acl' class definition cannot be changed,
       *       so 'modify' event callbacks are not necessary.
       *
       * NOTE: Skip this during 'special config mode' processing.
       *
       * NOTE: Requires both aclType and aclNum object values.  Perform
       *       the extract when the second of these two objects is set
       *       (in either order).
       */
      if (dsmibSpecialConfigModeGet() != L7_TRUE)
        if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_FLAGS_ACLNUM))
          if (diffServAclExtract(pRow, aclType, pRow->mib.aclNum) != L7_SUCCESS)
            return L7_FAILURE;

      pRow->mib.aclType = aclType;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_FLAGS_ACLTYPE);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ACL_NUMBER:
    {
      L7_uint32     aclNum = *(L7_uint32 *)pValue;

      *pValueLen = sizeof(aclNum);

      /* read the contents of the designated access control list and 
       * set up corresponding rows in the Class Rule Table
       *
       * NOTE: This is only done when the type 'acl' class is first
       *       created.  There is no attempt to keep the class definition
       *       in sync with any subsequent modifications to the ACL contents.
       *       The only way to change an 'acl' class definition is to delete
       *       it and create a new class.
       *
       * NOTE: Skip this during 'special config mode' processing.
       *
       * NOTE: Requires both aclType and aclNum object values.  Perform
       *       the extract when the second of these two objects is set
       *       (in either order).
       */
      if (dsmibSpecialConfigModeGet() != L7_TRUE)
        if (DSMIB_ROW_INVALID_FLAG_IS_OFF(pRow, DSMIB_CLASS_FLAGS_ACLTYPE))
          if (diffServAclExtract(pRow, pRow->mib.aclType, aclNum) != L7_SUCCESS)
            return L7_FAILURE;

      pRow->mib.aclNum = aclNum;
      DSMIB_ROW_INVALID_FLAG_CLR(pRow, DSMIB_CLASS_FLAGS_ACLNUM);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_STORAGE_TYPE:
    {
      dsmibStorageType_t storageType = *(dsmibStorageType_t *)pValue;
      *pValueLen = sizeof(storageType);
      pRow->mib.storageType = storageType;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ROW_STATUS:
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
  if (oid != L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ROW_STATUS)
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
      L7_INTF_SETMASKBIT(classIndexInUseMask, classIndex);
      dsmibClassIndexNextUpdate();

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
    if (dsmibRowEventCheck(DSMIB_TABLE_ID_CLASS, pRow, oldStatus, newStatus)
          != L7_SUCCESS)
      return L7_FAILURE;

    /* for a new row status of 'destroy', it is now time to delete the row */
    if (pRow->mib.rowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY)
      if (diffServClassDelete(classIndex) != L7_SUCCESS)
      {
        pRow->mib.rowStatus = restoreStatus;
        LOG_MSG("diffServClassObjectSet: attempt to destroy the row failed\n");
        return L7_FAILURE;
      }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Translate a class name into its associated Class Table index
*
* @param    namePtr     @b{(input)} Class name string pointer
* @param    pClassIndex @b{(output)} Pointer to class index value
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
L7_RC_t diffServClassNameToIndex(L7_uchar8 *namePtr, 
                                 L7_uint32 *pClassIndex)
{
  L7_uint32   len;
  L7_uint32   idx;
  L7_uchar8   className[L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX+1];

  /* check inputs */
  if (namePtr == L7_NULLPTR)
    return L7_FAILURE;
  if (pClassIndex == L7_NULLPTR)
    return L7_FAILURE;

  /* check the length of the class name string */
  len = (L7_uint32)strlen((char *)namePtr);
  if ((len == 0) || (len > L7_USMDB_MIB_DIFFSERV_CLASS_NAME_MAX))
    return L7_FAILURE;

  /* walk through existing class rows looking for a name match */
  idx = 0;                                  /* start from first row */
  while (diffServClassGetNext(idx, &idx) == L7_SUCCESS)
  {
    if (diffServClassObjectGet(idx, L7_DIFFSERV_CLASS_NAME, className)
          != L7_SUCCESS)
      continue;

    /* compare against name in existing row */
    if (strcmp((char *)namePtr, (char *)className) == 0)
    {
      *pClassIndex = idx;
      return L7_SUCCESS;
    }
  }

  *pClassIndex = 0;
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Provide the reference class index, if any, used by the specified
*           class
*
* @param    classIndex      @b{(input)}  Class index
* @param    pRefClassIndex  @b{(output)} Pointer to reference class index value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Class does not contain a reference class
* @returns  L7_FAILURE
*
* @notes    This is a supplemental function provided to assist the user
*           interface code with obtaining the reference class used by 
*           a particular class.  This simplifies the API interaction
*           for this translation, since a class can reference at most
*           one other class.
*
* @end
*********************************************************************/
L7_RC_t diffServClassToRefClass(L7_uint32 classIndex,
                                L7_uint32 *pRefClassIndex)
{
  dsmibClassEntryCtrl_t *pRow;

  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FAILURE;
  if (pRefClassIndex == L7_NULLPTR)
    return L7_FAILURE;

  /* get access to the row */
  pRow = dsmibClassRowFind(classIndex);
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;

  /* use the saved copy of the refclass index in the class control structure */
  *pRefClassIndex = pRow->refClassIndex;

  return (*pRefClassIndex != DSMIB_CLASS_REF_INDEX_NONE) ? L7_SUCCESS : L7_ERROR;
}

/*********************************************************************
* @purpose  Check if class definition is suitable for inclusion by specified
*           policy
*
* @param    classIndex      @b{(input)}  Class index
* @param    policyIndex     @b{(input)}  Policy index
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This is a supplemental function provided to assist the user
*           interface code with determining if a class can be assigned
*           to a given policy.  Some platforms impose restrictions on
*           the classifiers that can be used in a certain policy direction
*           (in/out).
*
* @end
*********************************************************************/
L7_RC_t diffServClassValidForPolicyCheck(L7_uint32 classIndex,
                                         L7_uint32 policyIndex)
{
  L7_RC_t                 rc;

  /* verify both class and policy exist (also does index range check) */
  if (diffServClassGet(classIndex) != L7_SUCCESS)
    return L7_FAILURE;
  if (diffServPolicyGet(policyIndex) != L7_SUCCESS)
    return L7_FAILURE;

  rc = dsmibPolicyInstClassValidate(policyIndex, classIndex);

  /* translate rc from internal function to match API specification */
  return (rc == L7_SUCCESS) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next policy index that references this class
*
* @param    classIndex      @b{(input)}   Class index
* @param    prevPolicyIndex @b{(input)}   Policy index to begin search
* @param    pPolicyIndex    @b{(output)}  Ptr to next sequential policy index
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR        Reached end of table
* @returns  L7_FAILURE
*
* @notes    A prevPolicyIndex value of 0 is used to find the first policy
*           that references this class.
*
* @notes    A policy is considered to reference a class if any of the 
*           policy class instances specify the class either 
*           directly, or through the class's reference chain.
*
* @end
*********************************************************************/
L7_RC_t diffServClassRefByPolicyGetNext(L7_uint32 classIndex,
                                        L7_uint32 prevPolicyIndex,
                                        L7_uint32 *pPolicyIndex)
{
  L7_uint32               policyIndex, policyInstIndex;
  L7_uint32               tempClassIndex;
  dsmibClassEntryCtrl_t   *pRefClass;

  /* verify class exists (also does index range check) */
  if (diffServClassGet(classIndex) != L7_SUCCESS)
    return L7_FAILURE;
  if (pPolicyIndex == L7_NULLPTR)
    return L7_FAILURE;

  /* walk through entire private MIB Policy Instance Table, looking for
   * the next instance that uses this class
   *
   * this API wants to iterate through policies, but it is the policy
   * instances that contain references to classes, thus start searching
   * from instance index 0 of the policy index FOLLOWING the one specified
   */
  policyIndex = prevPolicyIndex + 1;
  policyInstIndex = 0;                          /* start with first instance */
  while (diffServPolicyInstGetNext(policyIndex, policyInstIndex,
                                   &policyIndex, &policyInstIndex) == L7_SUCCESS)
  {
    if (diffServPolicyInstObjectGet(policyIndex, policyInstIndex, 
                                    L7_DIFFSERV_POLICY_INST_CLASS_INDEX, 
                                    &tempClassIndex) != L7_SUCCESS)
    {
      continue;
    }

    /* starting with the policy instance class, scan the class reference 
     * chain, if any, looking for a match against the specified class index
     */
    while (tempClassIndex != DSMIB_CLASS_REF_INDEX_NONE)
    {
      if ((pRefClass = dsmibClassRowFind(tempClassIndex)) == L7_NULLPTR)
        break;

      if (pRefClass->key.classIndex == classIndex)
      {
        /* found a policy reference to this class */
        *pPolicyIndex = policyIndex;
        return L7_SUCCESS;
      }

      tempClassIndex = pRefClass->refClassIndex;
    } /* endwhile */

  } /* endwhile policy inst */

  /* set failure code based on whether any policy instances were found */
  return (policyInstIndex == 0) ? L7_ERROR : L7_FAILURE;
}


/*
==================================
==================================
==================================

   CLASS TABLE HELPER FUNCTIONS

==================================
==================================
==================================
*/

/*********************************************************************
* @purpose  Initialize class index next value
*
* @param    void
*
* @returns  void  
*
* @notes    Also initializes the class index in-use flags.
*
* @end
*********************************************************************/
void dsmibClassIndexNextInit(void)
{
  /* initialize index in-use bitmask */
  memset(&classIndexInUseMask, 0, sizeof(classIndexInUseMask));

  /* establish initial index next value */
  dsmibClassIndexNextUpdate();
}

/*********************************************************************
* @purpose  Provide the current value of the class index next variable
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
void dsmibClassIndexNextRead(L7_uint32 *pNextFree)
{
  if (pNextFree != L7_NULLPTR)
    *pNextFree = classIndexNextFree;
}

/*********************************************************************
* @purpose  Determine next available class index value and update its value
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
void dsmibClassIndexNextUpdate(void)
{
  L7_uint32     i, imax;

  for (i = 1, imax = DSMIB_CLASS_INDEX_MAX; i <= imax; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully completed.
     */
    if (L7_INTF_ISMASKBITSET(classIndexInUseMask, i) == L7_FALSE)
      break;
  }
  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > imax)
    i = 0;

  classIndexNextFree = i;
}

/*********************************************************************
* @purpose  Verifies the specified class index value matches its
*           current 'IndexNext' value
*
* @param    classIndex  @b{(input)} Class index to be checked
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
L7_RC_t dsmibClassIndexNextVerify(L7_uint32 classIndex)
{
  L7_uint32     currentIndex;

  if (classIndex == 0)
    return L7_FAILURE;

  /* skip this checking if currently in 'special config mode' */
  if (dsmibSpecialConfigModeGet() == L7_TRUE)
    return L7_SUCCESS;

  dsmibClassIndexNextRead(&currentIndex);

  return (classIndex == currentIndex) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Class Table row element
*
* @param    classIndex  @b{(input)} Class index
*
* @returns  pRow        Pointer to class row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsmibClassEntryCtrl_t *dsmibClassRowFind(L7_uint32 classIndex)
{
  dsmibClassKey_t       keys;
  dsmibClassEntryCtrl_t *pRow;

  /* get access to the class row */
  keys.classIndex = classIndex;

  pRow = dsmibRowFind(&dsmibAvlTree[DSMIB_TABLE_ID_CLASS], 
                      &keys, AVL_EXACT);                                 

  return pRow;
}

/*********************************************************************
* @purpose  Check if a class is part of a reference chain originating with   
*           the specified class
*
* @param    classIndex    @b{(input)} Class index
* @param    refClassIndex @b{(input)} Reference chain start class index
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    No reference class validation is performed.
*
* @end
*********************************************************************/
L7_BOOL dsmibClassIsInRefChain(L7_uint32 classIndex,
                               L7_uint32 refClassIndex)
{
  dsmibClassEntryCtrl_t *pRefClass;

  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(classIndex))
    return L7_FALSE;
  if (DSMIB_CLASS_INDEX_OUT_OF_RANGE(refClassIndex))
    return L7_FALSE;

  /* get access to the row */
  while (refClassIndex != DSMIB_CLASS_REF_INDEX_NONE)
  {
    if (refClassIndex == classIndex)
      return L7_TRUE;

    /* get next reference class index in the chain */
    pRefClass = dsmibClassRowFind(refClassIndex);
    if (pRefClass == L7_NULLPTR)
      break;
    refClassIndex = pRefClass->refClassIndex;
  }

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Checks if a class object value is valid to 'get'
*
* @param    pRow        @b{(input)} Class table row pointer
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
L7_RC_t dsmibClassObjectGetCheck(dsmibClassEntryCtrl_t *pRow,
                                 L7_DIFFSERV_CLASS_TABLE_OBJECT_t oid)
{
  dsmibClassFlags_t objFlag;
  L7_RC_t           rc;

  switch (oid)
  {
  case L7_DIFFSERV_CLASS_NAME:
    objFlag = DSMIB_CLASS_FLAGS_NAME;
    break;

  case L7_DIFFSERV_CLASS_TYPE:
    objFlag = DSMIB_CLASS_FLAGS_TYPE;
    break;

  case L7_DIFFSERV_CLASS_L3_PROTO:
    objFlag = DSMIB_CLASS_FLAGS_L3PROTO;
    break;

  case L7_DIFFSERV_CLASS_ACLTYPE:
  case L7_DIFFSERV_CLASS_ACLNUM:
    if (oid == L7_DIFFSERV_CLASS_ACLTYPE)
      objFlag = DSMIB_CLASS_FLAGS_ACLTYPE;
    else
      objFlag = DSMIB_CLASS_FLAGS_ACLNUM;

    rc = dsmibClassFeaturePresentCheck(L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL);
    if (rc != L7_SUCCESS)
      return rc;
    
    break;

  case L7_DIFFSERV_CLASS_STORAGE_TYPE:
  case L7_DIFFSERV_CLASS_ROW_STATUS: 
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
* @purpose  Map Class object IDs to the Class types
*
* @param    oid         @b{(input)} Object identifier
* @param    pClassType  @b{(output)} Pointer to Class type
*
* @returns  Void  
*
* @notes    None
*
* @end
*********************************************************************/
void dsmibClassConvertOidToType(L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE_t oid,
                                L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t *pClassType)
{
  switch (oid)
  {
  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ACL_TYPE:
  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ACL_NUMBER:
    *pClassType = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_NAME:
  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_TYPE:
  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_STORAGE_TYPE:
  case L7_USMDB_MIB_DIFFSERV_CLASS_SETTEST_OID_ROW_STATUS:
    *pClassType = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_NONE;
    break;

  default:
    /* invalid object identifier */
    *pClassType = L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_NONE;
    break;
  } /* endswitch */
}

/*********************************************************************
* @purpose  Check the existence of the specified component feature
*           in the hardware platform
*
* @param    classType  @b{(input)} Class type
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  Invalid class type
* @returns  L7_NOT_SUPPORTED  Feature not supported in hardware platform
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dsmibClassFeaturePresentCheck(L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_t classType)
{
  L7_DIFFSERV_FEATURE_IDS_t fid;
  L7_BOOL                   featurePresent;

  switch (classType)
  {
  case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL:
    fid = L7_DIFFSERV_CLASS_TYPE_ALL_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ANY:
    fid = L7_DIFFSERV_CLASS_TYPE_ANY_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL:
    fid = L7_DIFFSERV_CLASS_TYPE_ACL_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_NONE:
    /* this value is used to handle certain generic objects (row status, etc) */
    fid = L7_DIFFSERV_FEATURE_SUPPORTED;
    break;

  default:
    /* invalid class type */
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

  featurePresent = cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, fid);

  return (featurePresent == L7_TRUE) ? L7_SUCCESS : L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Check the existence of the specified component feature
*           in the hardware platform
*
* @param    classL3Proto  @b{(input)} Class Layer 3 Protocol
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  Invalid class type
* @returns  L7_NOT_SUPPORTED  Feature not supported in hardware platform
*
* @notes    None
*
* @end
*********************************************************************/
L7_RC_t dsmibClassProtoFeaturePresentCheck(L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_t classL3Proto)
{
  L7_DIFFSERV_FEATURE_IDS_t fid;
  L7_BOOL                   featurePresent;

  switch (classL3Proto)
  {
  case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV4:
    fid = L7_DIFFSERV_CLASS_L3PROTO_IPV4_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_IPV6:
    fid = L7_DIFFSERV_CLASS_L3PROTO_IPV6_FEATURE_ID;
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_L3PROTOCOL_NONE:
    /* this value is used to handle certain generic objects (row status, etc) */
    fid = L7_DIFFSERV_FEATURE_SUPPORTED;
    break;

  default:
    /* invalid class type */
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

  featurePresent = cnfgrIsFeaturePresent(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, fid);

  return (featurePresent == L7_TRUE) ? L7_SUCCESS : L7_NOT_SUPPORTED;
}

/*********************************************************************
* @purpose  Populates an array with the class indexes for all entries in 
*           Class Table sorted such that any class referring to another
*           class appears after that class.
*
* @param    orderedClassList  @b{(output)} an array to receive the sorted class
*                                          indexes
* @param    pClassIndex @b{(output)} the number of entries in the output array
*
* @returns  L7_SUCCESS  at least one entry in output array
* @returns  L7_FAILURE  no entries found
*
* @notes    The array for the list must contain at least L7_DIFFSERV_CLASS_LIM
*           entries.
*
* @end
*********************************************************************/
L7_RC_t diffServClassOrderedListGet(L7_uint32 *orderedClassList, L7_uint32 *count)
{
  L7_RC_t rc;
  L7_BOOL entryMoved;
  L7_uint32 index, refIndex;
  L7_uint32 i, j;
  L7_uint32 classCount, classWithRefCount, classWithRefCountRemaining;

  struct 
  {
    L7_uint32  index;
    L7_uint32  refClassIndex;
  } classesWithReferences[L7_DIFFSERV_CLASS_LIM];

  index = 0;
  classCount = 0;
  classWithRefCount = 0;
  memset(orderedClassList, 0, L7_DIFFSERV_CLASS_LIM*sizeof(L7_uint32));
  memset(classesWithReferences, 0, sizeof(classesWithReferences));

  /* retrieve all classes and populate list arrays */
  while (diffServClassGetNext(index, &index) == L7_SUCCESS)
  {
    /* sort classes by whether they refer to another class*/
    if (diffServClassToRefClass(index, &refIndex) == L7_SUCCESS)
    {
      /* classes the refer to other classes are stored for later sorting */
      classesWithReferences[classWithRefCount].index = index;
      classesWithReferences[classWithRefCount].refClassIndex = refIndex;
      classWithRefCount++;
    }
    else
    {
      /* classes that refer to no other classes can be stored in the output list now */
      orderedClassList[classCount] = index;
      classCount++;
    }
  }

  /* 
   *  now we need to transfer any entries in the classesWithReferences[] list into the output list;
   *  any class with a reference to another class needs to appear after that class  
   */
  if (classWithRefCount > 0)
  {
    classWithRefCountRemaining = classWithRefCount;
    while (classWithRefCountRemaining > 0)
    {
      for (i=0; i<classWithRefCount; i++)
      {
        if ((classesWithReferences[i].index != 0) &&
            (classesWithReferences[i].refClassIndex != 0))
        {
          entryMoved = L7_FALSE;
          for (j=0; ((entryMoved == L7_FALSE) && (j<classCount)); j++)
          {
            if (orderedClassList[j] == classesWithReferences[i].refClassIndex)
            {
              orderedClassList[classCount] = classesWithReferences[i].index;
              classCount++;
              classesWithReferences[i].index = 0;
              classesWithReferences[i].refClassIndex = 0;
              classWithRefCountRemaining--;
              entryMoved = L7_TRUE;
            }
          }
        }
      }
    }
  }

  if (0 < classCount)
  {
    *count = classCount;
    rc = L7_SUCCESS;
  }
  else
  {
    rc = L7_FAILURE;
  }
  return(rc);
}

/*
=====================================
=====================================
=====================================

   CLASS TABLE DEBUG SHOW FUNCTION

=====================================
=====================================
=====================================
*/

/*********************************************************************
* @purpose  Display the current DiffServ Class Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsmibClassTableShow(void)
{
  L7_uint32             msgLvlReqd;
  dsmibClassEntryCtrl_t *pRow;
  dsmibClassKey_t       keys;
  avlTree_t             *pTree = &dsmibAvlTree[DSMIB_TABLE_ID_CLASS];
  L7_uint32             count;

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  diffServClassIndexNext(&count);

  DIFFSERV_PRT(msgLvlReqd, "\nClass Index Next: %u\n\n", count);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Class Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Class Table (contains %u entries):\n\n", count);

  DIFFSERV_PRT(msgLvlReqd, "Index Type L3Proto AclT Acl#  Next ST RS TID  Flags   RP ");
  DIFFSERV_PRT(msgLvlReqd, "RefCt RuleCt CoRef RefIdx Class Name                       \n");
  DIFFSERV_PRT(msgLvlReqd, "----- ---- ------- ---- ----- ---- -- -- --- -------- -- ");
  DIFFSERV_PRT(msgLvlReqd, "----- ------ ----- ------ ---------------------------------\n");

  keys.classIndex = 0;                          /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    /* part A */
    DIFFSERV_PRT(msgLvlReqd, " %3u  %3s   %4s   %-4s %5u %3u  %2s %2s  %2d %8.8x %2s ", 
                 pRow->key.classIndex, 
                 dsmibClassTypeStr[pRow->mib.type], 
                 dsmibClassL3ProtoStr[pRow->mib.l3Protocol], 
                 dsmibClassAclTypeStr[pRow->mib.aclType], 
                 pRow->mib.aclNum,
                 pRow->mib.ruleIndexNextFree,
                 dsmibStgTypeStr[pRow->mib.storageType],
                 dsmibRowStatusStr[pRow->mib.rowStatus],
                 pRow->tableId,
                 pRow->rowInvalidFlags,
                 dsmibRowPendingStr[pRow->rowPending]);

    /* part B */
    DIFFSERV_PRT(msgLvlReqd, "%4u   %3u   %4u   %4u  \'%-s\'\n", 
                 pRow->refCount, pRow->ruleCount, pRow->colorRefCount, 
                 pRow->refClassIndex, (char *)pRow->mib.name);

    /* update search keys for next pass */
    keys = pRow->key;
  }
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

void dsmibClassOrderedListShow(void)
{
  L7_uint32 msgLvlReqd;
  L7_uint32 classList[L7_DIFFSERV_CLASS_LIM];
  L7_uint32 count;
  L7_uint32 index;

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  if (diffServClassOrderedListGet(classList, &count) == L7_SUCCESS)
  {
    DIFFSERV_PRT(msgLvlReqd, "\n%u classes are in the array\n", count);
    for (index = 0; index < count; index++)
    {
      DIFFSERV_PRT(msgLvlReqd, "  classList[%u] = %u\n", index, classList[index]);
    }
  }
  else
  {
    DIFFSERV_PRT(msgLvlReqd, "Got an empty list back.\n");
  }
}

