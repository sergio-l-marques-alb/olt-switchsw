/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   diffserv_meter_api.c
*
* @purpose    DiffServ Standard MIB Meter and Token Bucket Parameter
*             Table APIs
*
* @component  DiffServ
*
* @comments   none
*
* @create     07/05/2002
*
* @author     vbhaskar
* @end
*             
*********************************************************************/
#include "l7_diffserv_include.h"
#include "comm_mask.h"
#include "usmdb_mib_diffserv_common.h"
#include "usmdb_mib_diffserv_private_api.h"

/* Globals for next free indexes*/
L7_uint32 meterIdNextFree;                      /* next available meter id                  */
L7_uint32 tbParamIdNextFree;                    /* next available token bucket parameter id */

/* Number of bytes in mask */
#define DSSTDMIB_METER_ID_INDICES               (((DSSTDMIB_METER_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)
#define DSSTDMIB_TBPARAM_ID_INDICES             (((DSSTDMIB_TBPARAM_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)

/* Mask storage definitions */
typedef struct
{
  L7_uchar8   value[DSSTDMIB_METER_ID_INDICES];
} DSSTDMIB_METER_ID_MASK_t;

typedef struct
{
  L7_uchar8   value[DSSTDMIB_TBPARAM_ID_INDICES];
} DSSTDMIB_TBPARAM_ID_MASK_t;

/* NOTE: Index 0 is reserved and must not be used */
DSSTDMIB_METER_ID_MASK_t                meterIdInUseMask;
DSSTDMIB_TBPARAM_ID_MASK_t              tbParamIdInUseMask;


/*
======================
======================
======================

   METER TABLE APIs

======================
======================
======================
*/

/*********************************************************************
* @purpose  Initialize draft MIB Meter support
*
* @param    void
*
* @returns  void
*
* @notes
*       
* @end
*********************************************************************/
void dsStdMibMeterInit(void)
{
  /* initialize index in-use bitmask */
  memset(&meterIdInUseMask, 0, sizeof(meterIdInUseMask));

  /* establish initial index next values */
  dsStdMibMeterIdNextUpdate();
}

/*********************************************************************
* @purpose  Obtain the next available index value to be used when
*           creating a new row in the Meter Table
*
* @param    pMeterId  @b{(output)} Pointer to meter id value
*
* @returns  void
*
* @notes    Only outputs a value if the pMeterId parm is non-null.
*
* @notes    An output *pMeterId value of 0 means the table is full.
*
* @notes    The index value remains free until used in a subsequent
*           create request. When two or more requestors get the same
*           index value, only the first one to successfully create a
*           new row in this table gets to keep it. All other requestors'
*           create requests will fail due to the index being in use,
*           and they must call this function again to get a new value.
*
* @end
*********************************************************************/
void dsStdMibMeterIdNext(L7_uint32 *pMeterId)
{
  if (pMeterId != L7_NULLPTR)
  {
    if (DSSTDMIB_METER_INDEX_OUT_OF_RANGE(meterIdNextFree) == L7_TRUE)
    {
      *pMeterId = 0;
    }
    else
    {
      *pMeterId = meterIdNextFree;
    }
  }
}

/*********************************************************************
* @purpose  Determine next available Token Bucket Param Id value and
*           update its value
*
* @param    void
*
* @returns  void  
*
* @notes    Always looks for first id not already in use, starting with 1.
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
void dsStdMibMeterIdNextUpdate(void)
{
  L7_uint32    i;

  for (i = 1; i <= DSSTDMIB_METER_INDEX_MAX; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(meterIdInUseMask, i) == L7_FALSE)
      break;
  }

  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > DSSTDMIB_METER_INDEX_MAX)
    i = 0;

  meterIdNextFree = i;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Meter row element
*
* @param    meterId  @b{(input)} Meter Id
*
* @returns  pRow        Pointer to row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsStdMibMeterEntryCtrl_t *dsStdMibMeterRowFind(L7_uint32 meterId)
{
  dsStdMibMeterEntryCtrl_t  *pRow;
  dsStdMibMeterKey_t        keys;

  keys.meterId = meterId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_METER], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Create a new row in the Meter Table
*
* @param    pSucceedNext @b{(input)} Pointer to next data path element, 
                                     if traffic conforms 
* @param    pFailNext    @b{(input)} Pointer to next data path element,
                                     if traffic does not conform
* @param    pSpecific    @b{(input)} Pointer to specific data path
*                                    element
* @param    pPathNext    @b{(inout)} Pointer to next data path element
*                                                                    
* @returns  L7_SUCCESS
* @returns  L7_ERROR    No more index values available (table full)
* @returns  L7_FAILURE
*
* @notes    The pPathNext parm denotes the next element in the data path
*           to which this one chains.  This parm value is then updated
*           with this element's row ptr info.
*
* @end
*********************************************************************/
L7_RC_t diffServMeterCreate(dsStdMibRowPtr_t  *pSucceedNext,
                            dsStdMibRowPtr_t  *pFailNext,
                            dsStdMibRowPtr_t  *pSpecific,
                            dsStdMibRowPtr_t  *pPathNext)
{
  L7_RC_t                  rc = L7_FAILURE;
  dsStdMibTableId_t        tableId = DSSTDMIB_TABLE_ID_METER;
  dsStdMibMeterEntryCtrl_t meterRow, *pMeterRow = L7_NULLPTR;
  L7_uint32                meterId = DSSTDMIB_ID_TRACE_INIT_VAL;

  DIFFSERV_NULLPTR_CHECK(pSucceedNext);
  DIFFSERV_NULLPTR_CHECK(pFailNext);
  DIFFSERV_NULLPTR_CHECK(pSpecific);
  DIFFSERV_NULLPTR_CHECK(pPathNext);

  do
  {
    /* get the next available meter Id */
    dsStdMibMeterIdNext(&meterId);

    if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
    {
      DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                               meterId, 0, 0, 
                               (L7_uchar8)L7_TRUE);
    }

    /* meter id 0 means the table is full */
    if (meterId == 0)
    {
      rc = L7_ERROR;
      break;
    }

    pMeterRow = &meterRow;
    memset(pMeterRow, 0, sizeof(*pMeterRow));

    pMeterRow->key.meterId = meterId;

    pMeterRow->tableId = tableId;

    pMeterRow->mib.id = meterId;
    pMeterRow->mib.succeedNext = *pSucceedNext;
    pMeterRow->mib.failNext = *pFailNext;      
    pMeterRow->mib.specific = *pSpecific;
    pMeterRow->mib.storage = DSSTDMIB_DEFAULT_STORAGE_TYPE;
    pMeterRow->mib.status = L7_DIFFSERV_ROW_STATUS_ACTIVE;

    /* init color aware fields to 'blind' values
     *
     * NOTE:  The Color Aware table contents of this row are modified
     *        by diffServColorAwareCreate() when the meter is used for 
     *        policing.
     */
    pMeterRow->mibColor.level = DSSTDMIB_DEFAULT_COLOR_LEVEL;
    pMeterRow->mibColor.mode = DSSTDMIB_DEFAULT_COLOR_MODE;
    pMeterRow->mibColor.value = DSSTDMIB_DEFAULT_COLOR_VALUE;

    if (dsmibRowInsert(&dsStdMibAvlTree[tableId], pMeterRow) != L7_SUCCESS)
      break;

    /* successful row creation -- update next free index value*/
    L7_INTF_SETMASKBIT(meterIdInUseMask, meterId);
    dsStdMibMeterIdNextUpdate();

    /* update data path next row info */
    DSSTDMIB_PATH_NEXT_UPDATE(pPathNext, tableId, meterId, 0, 0);

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            meterId, 0, 0,
                            (L7_uint32)pMeterRow, (L7_uchar8)rc,
                            (L7_uchar8)L7_TRUE);
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete a row from the Meter Table for the specified index
*
* @param    pPathNext    @b{(inout)} Pointer to next data path element
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServMeterDelete(dsStdMibRowPtr_t *pPathNext)
{
  L7_RC_t                  rc = L7_FAILURE;
  dsStdMibTableId_t        tableId = DSSTDMIB_TABLE_ID_METER;
  dsStdMibMeterEntryCtrl_t *pMeterRow = L7_NULLPTR;
  L7_uint32                meterId;

  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* make sure tableId is correct */
  if (pPathNext->tableId != tableId)
    return L7_FAILURE;

  meterId = pPathNext->rowIndex1;

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                             meterId, 0, 0);
  }

  do
  {
    if(DSSTDMIB_METER_INDEX_OUT_OF_RANGE(meterId) == L7_TRUE)
      break;

    pMeterRow = dsStdMibMeterRowFind(meterId);
    if (pMeterRow == L7_NULLPTR)
    {
      *pPathNext = DSSTDMIB_ZERO_DOT_ZERO;
      rc = L7_SUCCESS;        /* consider this a success if row not found */
      break;
    }

    /* delete specific elem (TBParam) */
    if (dsStdMibPrimitiveElemDelete(&pMeterRow->mib.specific) != L7_SUCCESS)
      break;

    /* recursively delete the entire failNext data path */
    if (dsStdMibDataPathInstanceDelete(&pMeterRow->mib.failNext) != L7_SUCCESS)
      break;

    /* update data path next row info to succeed path before deleting element */
    *pPathNext = pMeterRow->mib.succeedNext;

    if (dsmibRowDelete(&dsStdMibAvlTree[tableId], pMeterRow) != L7_SUCCESS)
      break;

    /* successful row deletion -- update next free index value*/
    L7_INTF_CLRMASKBIT(meterIdInUseMask, meterId);
    dsStdMibMeterIdNextUpdate();

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            meterId, 0, 0,
                            (L7_uint32)pMeterRow, (L7_uchar8)rc);
  }

  return rc;
}

/*********************************************************************
   NOTE: METER TABLE EXTERNAL APIs
*********************************************************************/

/*************************************************************************
* @purpose  Obtain the current value of the meter index next variable
*
* @param    pMeterIndex  @b{(output)} Pointer to meter index value
*
* @returns  void
*
* @notes    Only outputs a value if the pMeterIndex parm is non-null.
*
* @notes    A *pMeterIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServMeterIndexNext(L7_uint32 *pMeterIndex)
{
  if (pMeterIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  dsStdMibMeterIdNext(pMeterIndex);

  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
}

/*********************************************************************
* @purpose  Verify that a Meter Table row exists for the specified 
*           Meter Id
*
* @param    meterId     @b{(input)} Meter Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServMeterGet(L7_uint32 meterId)
{
  dsStdMibMeterEntryCtrl_t  *pRow; 
  dsStdMibMeterKey_t        keys;
  
  if(DSSTDMIB_METER_INDEX_OUT_OF_RANGE(meterId) == L7_TRUE)
    return L7_FAILURE;

  keys.meterId = meterId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_METER],
                      &keys, AVL_EXACT);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Meter Table
*
* @param    prevMeterId  @b{(input)}  Meter Id to begin search
* @param    pMeterId     @b{(output)} Pointer to next sequential
*                                     Meter Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevMeterId value of 0 is used to find the first Meter 
*           Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServMeterGetNext(L7_uint32 prevMeterId, L7_uint32 *pMeterId)
{
  dsStdMibMeterEntryCtrl_t  *pRow; 
  dsStdMibMeterKey_t        keys;

  DIFFSERV_NULLPTR_CHECK(pMeterId);
  keys.meterId = prevMeterId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_METER],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
  {
    *pMeterId =0;
    return L7_FAILURE;
  }

  *pMeterId = pRow->key.meterId;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the value of an object in the Meter Table based on
*           Object Id
*
* @param    meterId    @b{(input)}  Meter Id
* @param    objectId   @b{(input)}  Object Id
* @param    pValue     @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServMeterObjectGet(L7_uint32 meterId,
                           L7_DIFFSERV_METER_TABLE_OBJECT_t objectId,
                               void *pValue)
{
  dsStdMibMeterEntryCtrl_t  *pRow; 
  dsStdMibMeterKey_t        keys;
  
  if(DSSTDMIB_METER_INDEX_OUT_OF_RANGE(meterId) == L7_TRUE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pValue);

  keys.meterId = meterId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_METER],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  switch(objectId)
  {
  case L7_DIFFSERV_METER_SUCCEED_NEXT:
    *(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *)pValue = 
                              dsStdMibRowPtrXlateToExt(pRow->mib.succeedNext);
      break;
  
  case L7_DIFFSERV_METER_FAIL_NEXT:
    *(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *)pValue = 
                              dsStdMibRowPtrXlateToExt(pRow->mib.failNext);
      break;
  
  case L7_DIFFSERV_METER_SPECIFIC:
    *(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *)pValue = 
                              dsStdMibRowPtrXlateToExt(pRow->mib.specific);
    break;

  case L7_DIFFSERV_METER_STORAGE:
    *(dsStdMibStorageType_t *)pValue = pRow->mib.storage;
    break;
  
  case L7_DIFFSERV_METER_ROW_STATUS:
    *(dsStdMibRowStatus_t *)pValue = pRow->mib.status;
    break;
     
  default:
    DIFFSERV_SEMA_GIVE(dsStdMibSemId);
    return L7_FAILURE;
    /*PASSTHRU*/
  }
  
  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
  return L7_SUCCESS; 
}


/*
=======================================
=======================================
=======================================

   TOKEN BUCKET PARAMETER TABLE APIs

=======================================
=======================================
=======================================
*/

/*********************************************************************
* @purpose  Initialize draft MIB TB param support
*
* @param    void
*
* @returns  void
*
* @notes
*       
* @end
*********************************************************************/
void dsStdMibTBParamInit(void)
{
  /* initialize index in-use bitmask */
  memset(&tbParamIdInUseMask, 0, sizeof(tbParamIdInUseMask));

  /* establish initial index next values */
  dsStdMibTBParamIdNextUpdate();
}

/*********************************************************************
* @purpose  Obtain the next available index value to be used when
*           creating a new row in the Token Bucket Parameter Table
*
* @param    pTbParamId   @b{(output)} Pointer to TB Parameter Id value
*
* @returns  void
*
* @notes    Only outputs a value if the pTbParamId parm is non-null.
*
* @notes    An output *pTbParamId value of 0 means the table is full.
*
* @notes    The index value remains free until used in a subsequent
*           create request. When two or more requestors get the same
*           index value, only the first one to successfully create a
*           new row in this table gets to keep it. All other requestors'
*           create requests will fail due to the index being in use,
*           and they must call this function again to get a new value.
*
* @end
*********************************************************************/
void dsStdMibTBParamIdNext(L7_uint32 *pTbParamId)
{
  if (pTbParamId != L7_NULLPTR)
  {
    if (DSSTDMIB_TBPARAM_INDEX_OUT_OF_RANGE(tbParamIdNextFree) == L7_TRUE)
    {
      *pTbParamId = 0;
    }
    else
    {
      *pTbParamId = tbParamIdNextFree;
    }
  }
}

/*********************************************************************
* @purpose  Determine next available Token Bucket Param Id value and
*           update its value
*
* @param    void
*
* @returns  void  
*
* @notes    Always looks for first id not already in use, starting with 1.
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
void dsStdMibTBParamIdNextUpdate(void)
{
  L7_uint32    i;

  for (i = 1; i <= DSSTDMIB_TBPARAM_INDEX_MAX; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(tbParamIdInUseMask, i) == L7_FALSE)
      break;
  }

  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > DSSTDMIB_TBPARAM_INDEX_MAX)
    i = 0;

  tbParamIdNextFree = i;
}

/*********************************************************************
* @purpose  Obtain pointer to specified TB Param row element
*
* @param    tbParamId   @b{(input)} TB Param Id
*
* @returns  pRow        Pointer to row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsStdMibTBParamEntryCtrl_t *dsStdMibTBParamRowFind(L7_uint32 tbParamId)
{
  dsStdMibTBParamEntryCtrl_t  *pRow;
  dsStdMibTBParamKey_t        keys;

  keys.tbParamId = tbParamId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_TB_PARAM], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Create a new row in the Token Bucket Parameter Table
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrIndex  @b{(input)} Policy Attr Index
* @param    actionType       @b{(input)} Action Type
* @param    rateType         @b{(input)} Rate Type
* @param    pSpecific        @b{(inout)} Pointer to meter specific output 
*                                                                    
* @returns  L7_SUCCESS
* @returns  L7_ERROR    No more index values available (table full)
* @returns  L7_FAILURE
*
* @notes    The pSpecific parm input value is ignored.  At output, this 
*           value is updated to this element's row info ptr.
*
* @end
*********************************************************************/
L7_RC_t diffServTBParamCreate(L7_uint32 policyIndex, L7_uint32 policyInstIndex, 
                              L7_uint32 policyAttrIndex,
                              DSSTDMIB_ACTION_TYPE_t actionType,
                              DSSTDMIB_RATE_TYPE_t rateType,
                              dsStdMibRowPtr_t *pSpecific)
{
  L7_RC_t                                rc = L7_FAILURE;
  dsStdMibTableId_t                      tableId = DSSTDMIB_TABLE_ID_TB_PARAM;
  dsStdMibTBParamEntryCtrl_t             tbParamRow, *pTBParamRow = L7_NULLPTR;
  L7_uint32                              tbParamId = DSSTDMIB_ID_TRACE_INIT_VAL;
  L7_BOOL                                invalidAction = L7_FALSE;
  L7_uint32                              rate;
  L7_uint32                              burstKb;
  dsStdMibTBMeterType_t                  type = 0;
  L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t policyAttrObjRate = 0, policyAttrObjBurst = 0;

  DIFFSERV_NULLPTR_CHECK(pSpecific);

  do
  {
    /* get the next available TB Parameter Id */
    dsStdMibTBParamIdNext(&tbParamId);

    if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
    {
      DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                               tbParamId, 0, 0, 
                               (L7_uchar8)L7_TRUE);
    }

    /* TB Parameter id 0 means the table is full */
    if (tbParamId == 0)
    {
      rc = L7_ERROR;
      break;
    }

    switch (actionType)
    {
    case DSSTDMIB_ACTION_TYPE_POLICE_SIMPLE:
      {
        type = L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_SIMPLE_TOKEN_BUCKET;
        policyAttrObjRate = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CRATE;
        policyAttrObjBurst = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CBURST;
      }
      break;

    case DSSTDMIB_ACTION_TYPE_POLICE_SINGLERATE:
      {
        type = L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_SRTCM_BLIND;
        policyAttrObjRate = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CRATE;

        if (rateType == DSSTDMIB_RATE_TYPE_COMMITTED)
          policyAttrObjBurst = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CBURST;
        else
          policyAttrObjBurst = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EBURST;
      }
      break;

    case DSSTDMIB_ACTION_TYPE_POLICE_TWORATE:
      {
        type = L7_USMDB_MIB_DIFFSERV_TB_METER_TYPE_TRTCM_BLIND;

        if (rateType == DSSTDMIB_RATE_TYPE_COMMITTED)
        {
          policyAttrObjRate = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CRATE;
          policyAttrObjBurst = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CBURST;
        }
        else
        {
          policyAttrObjRate = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_PRATE;
          policyAttrObjBurst = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_PBURST;
        }
      }
      break;

     /* never get to this place */
    default:
      invalidAction = L7_TRUE;
      break;
    }

    /* check for invalid action detected */
    if (invalidAction != L7_FALSE)
      break;

    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, 
                                    policyAttrIndex, policyAttrObjRate,
                                    &rate) != L7_SUCCESS)
      break;

    /* policing */
    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, 
                                    policyAttrIndex, policyAttrObjBurst,
                                    &burstKb) != L7_SUCCESS)
    {
      break;
    }

    pTBParamRow = &tbParamRow;
    memset(pTBParamRow, 0, sizeof(*pTBParamRow));

    pTBParamRow->key.tbParamId = tbParamId;    

    pTBParamRow->tableId = tableId; 

    pTBParamRow->mib.id = tbParamId;                       
    pTBParamRow->mib.type = type;
    pTBParamRow->mib.rate = rate;
    pTBParamRow->mib.burstSize = burstKb * DSSTDMIB_1K;
    /* rate in kbps and time in microseconds */
    pTBParamRow->mib.interval = (L7_uint32)max((1000/rate), DSSTDMIB_TB_PARAM_INTERVAL_MIN); 
    pTBParamRow->mib.storage = DSSTDMIB_DEFAULT_STORAGE_TYPE;
    pTBParamRow->mib.status = L7_DIFFSERV_ROW_STATUS_ACTIVE;

    if (dsmibRowInsert(&dsStdMibAvlTree[tableId], pTBParamRow) != L7_SUCCESS)
      break;

    /* successful row creation -- update next free index value*/
    L7_INTF_SETMASKBIT(tbParamIdInUseMask, tbParamId);
    dsStdMibTBParamIdNextUpdate();

    /* update data path next row info */
    DSSTDMIB_PATH_NEXT_UPDATE(pSpecific, tableId, tbParamId, 0, 0);

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            tbParamId, 0, 0,
                            (L7_uint32)pTBParamRow, (L7_uchar8)rc,
                            (L7_uchar8)L7_TRUE);
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete a row from the Token Bucket Param Table for the
*           specified index
*
* @param    tbParamId   @b{(input)} Token Bucket Param Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServTBParamDelete(L7_uint32 tbParamId)
{
  L7_RC_t                    rc = L7_FAILURE;
  dsStdMibTableId_t          tableId = DSSTDMIB_TABLE_ID_TB_PARAM;
  dsStdMibTBParamEntryCtrl_t *pTBParamRow = L7_NULLPTR;

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                             tbParamId, 0, 0);
  }

  do
  {
    if(DSSTDMIB_TBPARAM_INDEX_OUT_OF_RANGE(tbParamId) == L7_TRUE)
      break;

    pTBParamRow = dsStdMibTBParamRowFind(tbParamId);
    if (pTBParamRow == L7_NULLPTR)
    {
      rc = L7_SUCCESS;          /* consider this a success if row not found */
      break;
    }

    if (dsmibRowDelete(&dsStdMibAvlTree[tableId], pTBParamRow) != L7_SUCCESS)
      break;

    /* successful row deletion -- update next free index value*/
    L7_INTF_CLRMASKBIT(tbParamIdInUseMask, tbParamId);
    dsStdMibTBParamIdNextUpdate();

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            tbParamId, 0, 0,
                            (L7_uint32)pTBParamRow, (L7_uchar8)rc);
  }

  return rc;
}

/*********************************************************************
   NOTE: TOKEN BUCKET PARAMETER TABLE EXTERNAL APIs
*********************************************************************/

/*************************************************************************
* @purpose  Obtain the current value of the TB param index next variable
*
* @param    pTBParamIndex  @b{(output)} Pointer to TB param index value
*
* @returns  void
*
* @notes    Only outputs a value if the pTBParamIndex parm is non-null.
*
* @notes    A *pTBParamIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServTBParamIndexNext(L7_uint32 *pTBParamIndex)
{
  if (pTBParamIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  dsStdMibTBParamIdNext(pTBParamIndex);

  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
}

/*********************************************************************
* @purpose  Verify that a Token Bucket Parameter Table row exists for 
*           the specified TB Parameter Id
*
* @param    tbParamId   @b{(input)} TB Parameter Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServTBParamGet(L7_uint32 tbParamId)
{
  dsStdMibTBParamEntryCtrl_t *pRow; 
  dsStdMibTBParamKey_t        keys;

  if(DSSTDMIB_TBPARAM_INDEX_OUT_OF_RANGE(tbParamId) == L7_TRUE)
    return L7_FAILURE;

  keys.tbParamId = tbParamId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_TB_PARAM],
                      &keys, AVL_EXACT);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Token Bucket
*           Parameter Table 
*
* @param    prevTbParamId @b{(input)}  TB Parameter Id to begin search
* @param    pTbParamId    @b{(output)} Pointer to next sequential
*                                      TB Parameter Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevTbParamId value of 0 is used to find the first
*           TB parameter Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServTBParamGetNext(L7_uint32 prevTbParamId,
                               L7_uint32 *pTbParamId)
{
  dsStdMibTBParamEntryCtrl_t *pRow; 
  dsStdMibTBParamKey_t        keys;

  DIFFSERV_NULLPTR_CHECK(pTbParamId);

  keys.tbParamId = prevTbParamId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_TB_PARAM],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
  {       
    *pTbParamId = 0;
    return L7_FAILURE;
  }

  *pTbParamId = pRow->key.tbParamId;
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get the value of an object in the Token Bucket Parameter
*           Table based on Object Id
*
* @param    tbParamId    @b{(input)}  TB Parameter Id
* @param    objectId     @b{(input)}  Object Id
* @param    pValue       @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServTBParamObjectGet(L7_uint32 tbParamId,
                         L7_DIFFSERV_TB_PARAM_TABLE_OBJECT_t objectId,
                                 void *pValue)
{
  dsStdMibTBParamEntryCtrl_t *pRow; 
  dsStdMibTBParamKey_t        keys;
  
  if(DSSTDMIB_TBPARAM_INDEX_OUT_OF_RANGE(tbParamId) == L7_TRUE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pValue);

  keys.tbParamId = tbParamId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_TB_PARAM],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  switch(objectId)
  {
  case L7_DIFFSERV_TB_PARAM_TYPE:
    *(dsStdMibTBMeterType_t *)pValue = pRow->mib.type;
    break;

  case L7_DIFFSERV_TB_PARAM_RATE:
    *(L7_uint32 *)pValue = pRow->mib.rate;
    break;

  case L7_DIFFSERV_TB_PARAM_BURST_SIZE:
    *(L7_uint32 *)pValue = pRow->mib.burstSize;
    break;
  
  case L7_DIFFSERV_TB_PARAM_INTERVAL:
    *(L7_uint32 *)pValue = pRow->mib.interval;
    break;

  case L7_DIFFSERV_TB_PARAM_STORAGE:
    *(dsStdMibStorageType_t *)pValue = pRow->mib.storage;
    break;

  case L7_DIFFSERV_TB_PARAM_ROW_STATUS:
    *(dsStdMibRowStatus_t *)pValue = pRow->mib.status;
    break;
  
  default:
    DIFFSERV_SEMA_GIVE(dsStdMibSemId);
    return L7_FAILURE;
    /*PASSTHRU*/
  }
  
  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
  return L7_SUCCESS; 
}


/*
=============================
=============================
=============================

   COLOR AWARE TABLE APIs

=============================
=============================
=============================
*/

/*********************************************************************
* @purpose  Initialize standard MIB Color Aware support
*
* @param    void
*
* @returns  void
*
* @notes    Augments the Meter Table, so does not maintain its own free index.
*       
* @end
*********************************************************************/
void dsStdMibColorAwareInit(void)
{
  /* no initialization needed */
}

/*********************************************************************
* @purpose  Obtain pointer to specified Color Aware row element
*
* @param    meterId     @b{(input)} Meter Id
*
* @returns  pRow        Pointer to row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Augments the Meter Table, so does not maintain its own free index.
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsStdMibMeterEntryCtrl_t *dsStdMibColorAwareRowFind(L7_uint32 meterId)
{
  dsStdMibMeterEntryCtrl_t  *pRow;
  dsStdMibMeterKey_t        keys;

  keys.meterId = meterId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_METER], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Create a new row in the Color Aware Table
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrIndex  @b{(input)} Policy Attr Index
* @param    policingType     @b{(input)} Policing type
* @param    colorLevel       @b{(input)} Policing color level
* @param    meterId          @b{(input)} Meter Id referencing this row
*                                                                    
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Augments the Meter Table, so does not maintain its own free index.
*
* @notes    Must be called after diffServMeterCreate().
*
* @end
*********************************************************************/
L7_RC_t diffServColorAwareCreate(L7_uint32 policyIndex, 
                                 L7_uint32 policyInstIndex, 
                                 L7_uint32 policyAttrIndex,
                                 DSSTDMIB_ACTION_TYPE_t policingType,
                                 dsStdMibColorLevel_t colorLevel,
                                 L7_uint32 meterId)
{
  L7_RC_t                                rc = L7_FAILURE;
  dsStdMibTableId_t                      tableId = DSSTDMIB_TABLE_ID_COLOR_AWARE;
  dsStdMibMeterEntryCtrl_t               *pMeterRow = L7_NULLPTR;
  L7_BOOL                                invalidAction = L7_FALSE;
  L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_t   prvtColorMode;
  L7_uint32                              prvtColorValue;
  dsStdMibColorMode_t                    colorMode;
  L7_uint32                              value;
  L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t policyAttrObjMode = 0, policyAttrObjVal = 0;

  do
  {
    if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
    {
      DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                               meterId, 0, 0, 
                               (L7_uchar8)L7_TRUE);
    }

    if (DSSTDMIB_METER_INDEX_OUT_OF_RANGE(meterId) == L7_TRUE)
      break;

    /* the meter table row must already exist, since the color aware row
     * only augments it
     */
    pMeterRow = dsStdMibColorAwareRowFind(meterId);
    if (pMeterRow == L7_NULLPTR)
    {
      break;
    }

    /* determine which policing color value to obtain */
    switch (policingType)
    {
    case DSSTDMIB_ACTION_TYPE_POLICE_SIMPLE:
      {
        if (colorLevel == L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_CONFORM)
        {
          policyAttrObjMode = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_COLOR_CONFORM_MODE;
          policyAttrObjVal = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_COLOR_CONFORM_VAL;
        }
        else
        {
          invalidAction = L7_TRUE;
        }
      }
      break;

    case DSSTDMIB_ACTION_TYPE_POLICE_SINGLERATE:
      {
        if (colorLevel == L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_CONFORM)
        {
          policyAttrObjMode = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_CONFORM_MODE;
          policyAttrObjVal = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_CONFORM_VAL;
        }
        else if (colorLevel == L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_EXCEED)
        {
          policyAttrObjMode = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_EXCEED_MODE;
          policyAttrObjVal = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_COLOR_EXCEED_VAL;
        }
        else
        {
          invalidAction = L7_TRUE;
        }
      }
      break;

    case DSSTDMIB_ACTION_TYPE_POLICE_TWORATE:
      {
        if (colorLevel == L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_CONFORM)
        {
          policyAttrObjMode = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_CONFORM_MODE;
          policyAttrObjVal = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_CONFORM_VAL;
        }
        else if (colorLevel == L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_EXCEED)
        {
          policyAttrObjMode = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_EXCEED_MODE;
          policyAttrObjVal = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_COLOR_EXCEED_VAL;
        }
        else
        {
          invalidAction = L7_TRUE;
        }
      }
      break;

     /* all other actions invalid for this table */
    default:
      invalidAction = L7_TRUE;
      break;
    }

    /* check for invalid action detected */
    if (invalidAction != L7_FALSE)
      break;

    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, 
                                    policyAttrIndex, policyAttrObjMode,
                                    &prvtColorMode) != L7_SUCCESS)
      break;

    /* get the color aware field match value */
    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, 
                                    policyAttrIndex, policyAttrObjVal,
                                    &prvtColorValue) != L7_SUCCESS)
      break;

    /* translate private MIB color mode into standard MIB color type and value*/
    value = 0;
    switch (prvtColorMode)
    {
    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_BLIND:
      colorMode = L7_USMDB_MIB_DIFFSERV_COLOR_MODE_BLIND;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS:
      colorMode = L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_COS;
      value = prvtColorValue;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_COS2:
      colorMode = L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_COS2;
      value = prvtColorValue;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPDSCP:
      colorMode = L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_IPDSCP;
      value = prvtColorValue;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_IPPREC:
      colorMode = L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_IPPREC;
      value = prvtColorValue;
      break;

    case L7_USMDB_MIB_DIFFSERV_POLICE_COLOR_UNUSED:
      colorMode = L7_USMDB_MIB_DIFFSERV_COLOR_MODE_AWARE_UNUSED;
      break;

    default:
      colorMode = L7_USMDB_MIB_DIFFSERV_COLOR_MODE_NONE;
      break;
    }

    pMeterRow->mibColor.level = colorLevel;
    pMeterRow->mibColor.mode = colorMode;
    pMeterRow->mibColor.value = value;

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            meterId, 0, 0,
                            (L7_uint32)pMeterRow, (L7_uchar8)rc,
                            (L7_uchar8)L7_TRUE);
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete a row from the Color Aware Table for the specified
*           index
*
* @param    meterId     @b{(input)} Meter Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Augments the Meter Table, so does not maintain its own free index.
*
* @notes    The diffServMeterDelete() function deletes the table row
*           containing the color aware objects.
*
* @end
*********************************************************************/
L7_RC_t diffServColorAwareDelete(L7_uint32 meterId)
{
  L7_RC_t                   rc = L7_FAILURE;
  dsStdMibTableId_t         tableId = DSSTDMIB_TABLE_ID_COLOR_AWARE;
  dsStdMibMeterEntryCtrl_t  *pRow = L7_NULLPTR;

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                             meterId, 0, 0);
  }

  do
  {
    if(DSSTDMIB_METER_INDEX_OUT_OF_RANGE(meterId) == L7_TRUE)
       break;

    pRow = dsStdMibColorAwareRowFind(meterId);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_SUCCESS;          /* consider this a success if row not found */
      break;
    }

    /* NOTE:  Do not actually delete the row -- let diffServMeterDelete() 
     *        handle it instead.
     */

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            meterId, 0, 0,                                                
                            (L7_uint32)pRow, (L7_uchar8)rc);
  }

  return rc;
}
                                                                            
/*********************************************************************
 NOTE: COLOR AWARE TABLE EXTERNAL APIs
*********************************************************************/
 
/*********************************************************************
* @purpose  Get the value of an object in the Color Aware Table based
*           on Object Id
*
* @param    meterId       @b{(input)}  Meter Id
* @param    objectId      @b{(input)}  Object Id
* @param    pValue        @b{(output)} Pointer to object value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    Augments the Meter Table.
*
* @end
*********************************************************************/
L7_RC_t diffServColorAwareObjectGet(L7_uint32 meterId,
                                    L7_DIFFSERV_COLOR_AWARE_TABLE_OBJECT_t objectId,
                                    void *pValue)
{
  L7_RC_t                   rc = L7_SUCCESS;
  dsStdMibMeterEntryCtrl_t  *pRow; 
  dsStdMibMeterKey_t        keys;
  
  if(DSSTDMIB_METER_INDEX_OUT_OF_RANGE(meterId) == L7_TRUE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pValue);

  keys.meterId = meterId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_METER],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
    return L7_ERROR;
  
  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  switch(objectId)
  {
  case L7_DIFFSERV_COLOR_AWARE_LEVEL:
    *(dsStdMibColorLevel_t *)pValue = pRow->mibColor.level;
    break;

  case L7_DIFFSERV_COLOR_AWARE_MODE:
    *(dsStdMibColorMode_t *)pValue = pRow->mibColor.mode;
    break;

  case L7_DIFFSERV_COLOR_AWARE_VALUE:
    *(L7_uint32 *)pValue = pRow->mibColor.value;
    break;

  default:
    rc = L7_FAILURE;
    break;

  } /* endswitch */
  
  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
  return rc; 
}

