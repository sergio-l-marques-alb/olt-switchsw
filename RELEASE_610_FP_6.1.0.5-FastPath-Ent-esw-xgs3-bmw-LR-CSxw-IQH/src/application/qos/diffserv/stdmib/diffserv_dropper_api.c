/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   diffserv_dropper_api.c
*
* @purpose    DiffServ Standard MIB Algorithmic Drop and Random Drop
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

/* Globals for next free indexes*/
L7_uint32 algDropIdNextFree;                    /* next available algorithmic dropper id */

/* Number of bytes in mask */
#define DSSTDMIB_ALG_DROP_ID_INDICES            (((DSSTDMIB_ALG_DROP_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)

/* Mask storage definitions */
typedef struct
{
  L7_uchar8   value[DSSTDMIB_ALG_DROP_ID_INDICES];
} DSSTDMIB_ALG_DROP_ID_MASK_t;

/* NOTE: Index 0 is reserved and must not be used */
DSSTDMIB_ALG_DROP_ID_MASK_t             algDropIdInUseMask;


/*
=================================
=================================
=================================

   ALGORITHMIC DROP TABLE APIs

=================================
=================================
=================================
*/

/*********************************************************************
* @purpose  Initialize draft MIB Algorithmic Drop support
*
* @param    void
*
* @returns  void
*
* @notes
*       
* @end
*********************************************************************/
void dsStdMibAlgDropInit(void)
{
  /* initialize index in-use bitmask */
  memset(&algDropIdInUseMask, 0, sizeof(algDropIdInUseMask));

  /* establish initial index next values */
  dsStdMibAlgDropIdNextUpdate();
}

/*********************************************************************
* @purpose  Obtain the next available index value to be used when
*           creating a new row in the Algorithmic Drop Table
*
* @param    pAlgDropId   @b{(output)} Pointer to the algorithmic drop
*                                     id value
*
* @returns  void
*
* @notes    Only outputs a value if the pAlgDropId parm is non-null.
*
* @notes    An output *pAlgDropId value of 0 means the table is full.
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
void dsStdMibAlgDropIdNext(L7_uint32 *pAlgDropId)
{
  if (pAlgDropId != L7_NULLPTR)
  {
    if (DSSTDMIB_ALG_DROP_INDEX_OUT_OF_RANGE(algDropIdNextFree) == L7_TRUE)
    {
      *pAlgDropId = 0;
    }
    else
    {
      *pAlgDropId = algDropIdNextFree;
    }
  }
}

/*********************************************************************
* @purpose  Determine the next available Algorithmic Drop Id value and
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
void dsStdMibAlgDropIdNextUpdate(void)
{
  L7_uint32    i;

  for (i = 1; i <= DSSTDMIB_ALG_DROP_INDEX_MAX; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(algDropIdInUseMask, i) == L7_FALSE)
      break;
  }

  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > DSSTDMIB_ALG_DROP_INDEX_MAX)
    i = 0;

  algDropIdNextFree = i;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Alg Drop row element
*
* @param    algDropId   @b{(input)} Alg Drop Id
*
* @returns  pRow        Pointer to row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsStdMibAlgDropEntryCtrl_t *dsStdMibAlgDropRowFind(L7_uint32 algDropId)
{                             
  dsStdMibAlgDropEntryCtrl_t  *pRow;
  dsStdMibAlgDropKey_t        keys;

  keys.algDropId = algDropId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ALG_DROP], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Create a new row in the Algorithmic Drop Table for the 
*           specified index
*
* @param    policyIndex      @b{(input)}  Policy Index
* @param    policyInstIndex  @b{(input)}  Policy Inst Index
* @param    policyAttrIndex  @b{(input)}  Policy Attr Index
* @param    intIfNum         @b{(input)}  Internal Interface Number
* @param    type             @b{(input)}  Type of algorithm
* @param    pQMeasure        @b{(input)}  Pointer to an entry in the 
*                                         diffServQTable
* @param    pSpecific        @b{(input)}  Pointer to further drop
*                                         algorithm
* @param    pPathNext        @b{(inout)}  Pointer to next data path element
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
L7_RC_t diffServAlgDropCreate(L7_uint32 policyIndex, 
                              L7_uint32 policyInstIndex,
                              L7_uint32 policyAttrIndex,
                              L7_uint32 intIfNum,
                              dsStdMibAlgDropType_t type,
                              dsStdMibRowPtr_t *pQMeasure,
                              dsStdMibRowPtr_t *pSpecific,
                              dsStdMibRowPtr_t *pPathNext)
{
  L7_RC_t                    rc = L7_FAILURE;
  dsStdMibTableId_t          tableId = DSSTDMIB_TABLE_ID_ALG_DROP;
  dsStdMibAlgDropEntryCtrl_t algDropRow, *pAlgDropRow = L7_NULLPTR;
  L7_uint32                  algDropId = DSSTDMIB_ID_TRACE_INIT_VAL;
  L7_ulong64                 octetsCtr = {0,0};
  L7_ulong64                 pktsCtr = {0,0};

  DIFFSERV_NULLPTR_CHECK(pQMeasure);
  DIFFSERV_NULLPTR_CHECK(pSpecific);
  DIFFSERV_NULLPTR_CHECK(pPathNext);

  do
  {
    /* get the next available Algorithmic Drop Id */
    dsStdMibAlgDropIdNext(&algDropId);
    
    if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
    {
      DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                               algDropId, 0, 0, 
                               (L7_uchar8)L7_TRUE);
    }

    /* Algorithmic Drop Id 0 means the table is full */
    if (algDropId == 0)
    {
      rc = L7_ERROR;
      break;
    }

    /* NOTE:  Do not read the counter values from the private MIB during
     *        row creation.  It is quite possible for the standard MIB 
     *        data path to be set up before the low-level code, so the 
     *        counter source might not exist yet.  Wait until the 
     *        'objectGet' call to retrieve the value.
     */

    pAlgDropRow = &algDropRow;
    memset(pAlgDropRow, 0, sizeof(*pAlgDropRow));

    pAlgDropRow->key.algDropId = algDropId;

    pAlgDropRow->tableId = tableId;

    pAlgDropRow->ctrSource.policyIndex = policyIndex;
    pAlgDropRow->ctrSource.policyInstIndex = policyInstIndex;
    pAlgDropRow->ctrSource.policyAttrIndex = policyAttrIndex;
    pAlgDropRow->ctrSource.intIfNum = intIfNum;

    pAlgDropRow->mib.id = algDropId;

    pAlgDropRow->mib.type = type;
    pAlgDropRow->mib.next = *pPathNext;
    pAlgDropRow->mib.qMeasure = *pQMeasure;
    /* NOTE: No means of to obtain a real queue size value from the device */
    pAlgDropRow->mib.qThreshold = DSSTDMIB_DEFAULT_QTHRESHOLD;
    pAlgDropRow->mib.specific = *pSpecific;
    pAlgDropRow->mib.octets = octetsCtr;
    pAlgDropRow->mib.pkts = pktsCtr;
    pAlgDropRow->mib.randomDropOctets = octetsCtr;
    pAlgDropRow->mib.randomDropPkts = pktsCtr;
    pAlgDropRow->mib.storage = DSSTDMIB_DEFAULT_STORAGE_TYPE;
    pAlgDropRow->mib.status = L7_DIFFSERV_ROW_STATUS_ACTIVE;

    /* Insert Alg Drop Entry in AVL */
    if (dsmibRowInsert(&dsStdMibAvlTree[tableId], pAlgDropRow) != L7_SUCCESS)
      break;

    /* successful row creation -- update next free index value*/
    L7_INTF_SETMASKBIT(algDropIdInUseMask, algDropId);
    dsStdMibAlgDropIdNextUpdate();

    /* update data path next row info */
    DSSTDMIB_PATH_NEXT_UPDATE(pPathNext, tableId, algDropId, 0, 0);

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            algDropId, 0, 0,
                            (L7_uint32)pAlgDropRow, (L7_uchar8)rc,
                            (L7_uchar8)L7_TRUE);
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete a row from the Algorithmic Drop Table for the
*           specified index
*
* @param    pPathNext   @b{(inout)} Pointer to next data path element
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServAlgDropDelete(dsStdMibRowPtr_t *pPathNext)
{
  L7_RC_t                     rc = L7_FAILURE;
  dsStdMibTableId_t           tableId = DSSTDMIB_TABLE_ID_ALG_DROP;
  dsStdMibAlgDropEntryCtrl_t  *pAlgDropRow = L7_NULLPTR;
  L7_uint32                   algDropId;


  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* make sure tableId is correct */
  if (pPathNext->tableId != tableId)
    return L7_FAILURE;

  algDropId = pPathNext->rowIndex1;

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                             algDropId, 0, 0);
  }

  do
  {
    if(DSSTDMIB_ALG_DROP_INDEX_OUT_OF_RANGE(algDropId) == L7_TRUE)
       break;

    pAlgDropRow = dsStdMibAlgDropRowFind(algDropId);
    if (pAlgDropRow == L7_NULLPTR)
    {
      *pPathNext = DSSTDMIB_ZERO_DOT_ZERO;
      rc = L7_SUCCESS;          /* consider this a success if row not found */
      break;
    }

    /* delete specific elem (RandomDrop) */
    if (dsStdMibPrimitiveElemDelete(&pAlgDropRow->mib.specific) != L7_SUCCESS)
      break;

    /* NOTE:  Do not delete the qMeasure elem here.  This queue elem will
     *        be deleted during the normal traversal of the pPathNext 
     *        data path.
     */

    /* update data path next row info before deleting element */
    *pPathNext = pAlgDropRow->mib.next;

    if (dsmibRowDelete(&dsStdMibAvlTree[tableId], pAlgDropRow) != L7_SUCCESS)
      break;

    /* successful row deletion -- update next free index value*/
    L7_INTF_CLRMASKBIT(algDropIdInUseMask, algDropId);
    dsStdMibAlgDropIdNextUpdate();

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            algDropId, 0, 0,
                            (L7_uint32)pAlgDropRow, (L7_uchar8)rc);
  }

  return rc;
}

/*********************************************************************
   NOTE: ALGORITHMIC DROP TABLE EXTERNAL APIs
*********************************************************************/
 
/*************************************************************************
* @purpose  Obtain the current value of the algorithmic drop index next
*           variable
*
* @param    pAlgDropIndex  @b{(output)} Pointer to algorithmic drop
*                                       index value
*
* @returns  void
*
* @notes    Only outputs a value if the pAlgDropIndex parm is non-null.
*
* @notes    A *pAlgDropIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServAlgDropIndexNext(L7_uint32 *pAlgDropIndex)
{
  if (pAlgDropIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  dsStdMibAlgDropIdNext(pAlgDropIndex);

  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
}

/*********************************************************************
* @purpose  Verify that an Algorithmic Drop Table row exists for the 
*           specified Algorithmic Drop Id
*
* @param    algDropId   @b{(input)} Algorithmic Drop Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServAlgDropGet(L7_uint32 algDropId)
{
  dsStdMibAlgDropEntryCtrl_t  *pRow; 
  dsStdMibAlgDropKey_t         keys;
                           
  if(DSSTDMIB_ALG_DROP_INDEX_OUT_OF_RANGE(algDropId) == L7_TRUE)
    return L7_FAILURE;

  keys.algDropId = algDropId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ALG_DROP],
                      &keys, AVL_EXACT);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Algorithmic Drop
*           Table
*
* @param    prevAlgDropId  @b{(input)}  Algorithmic Drop Id to begin
*                                       search
* @param    pAlgDropId     @b{(output)} Pointer to next sequential
*                                       Algorithmic Drop Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevAlgDropId value of 0 is used to find the first
*           Algorithmic Drop Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServAlgDropGetNext(L7_uint32 prevAlgDropId, L7_uint32 *pAlgDropId)
{
  dsStdMibAlgDropEntryCtrl_t  *pRow; 
  dsStdMibAlgDropKey_t         keys;
                           
  DIFFSERV_NULLPTR_CHECK(pAlgDropId);

  keys.algDropId = prevAlgDropId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ALG_DROP],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
  {
    *pAlgDropId = 0;
    return L7_FAILURE;
  }

  *pAlgDropId = pRow->key.algDropId;
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get the value of an object in the Algorithmic Drop Table
*           based on Object Id
*
* @param    algDropId    @b{(input)}  Algorithmic Drop Id
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
L7_RC_t diffServAlgDropObjectGet(L7_uint32 algDropId, 
                                 L7_DIFFSERV_ALG_DROP_TABLE_OBJECT_t objectId,
                                 void *pValue)
{
  dsStdMibAlgDropEntryCtrl_t  *pRow; 
  dsStdMibAlgDropKey_t         keys;
  L7_RC_t                      rc = L7_SUCCESS;
  L7_ulong64                   octetsCtr, pktsCtr;
                           
  if(DSSTDMIB_ALG_DROP_INDEX_OUT_OF_RANGE(algDropId) == L7_TRUE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pValue);

  keys.algDropId = algDropId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ALG_DROP],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  switch(objectId)
  {
  case L7_DIFFSERV_ALG_DROP_TYPE:
    *(dsStdMibAlgDropType_t *)pValue = pRow->mib.type;
    break;

  case L7_DIFFSERV_ALG_DROP_NEXT:
    *(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *)pValue = 
                              dsStdMibRowPtrXlateToExt(pRow->mib.next);
    break;
  
  case L7_DIFFSERV_ALG_DROP_QMEASURE:
    *(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *)pValue = 
                              dsStdMibRowPtrXlateToExt(pRow->mib.qMeasure);
    break;

  case L7_DIFFSERV_ALG_DROP_QTHRESHOLD:
    *(L7_uint32 *)pValue = pRow->mib.qThreshold;
    break;

  case L7_DIFFSERV_ALG_DROP_SPECIFIC:
    *(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *)pValue = 
                              dsStdMibRowPtrXlateToExt(pRow->mib.specific);
    break;

  case L7_DIFFSERV_ALG_DROP_OCTETS:
  case L7_DIFFSERV_ALG_DROP_PKTS:
    if (pRow->mib.type == L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_ALWAYS_DROP)
    {
      /* update the always drop counters from the latest private MIB value */
      if (diffServPolicyPerfInObjectGet(pRow->ctrSource.policyIndex, 
            pRow->ctrSource.policyInstIndex, pRow->ctrSource.intIfNum,
            L7_DIFFSERV_POLICY_PERF_IN_HC_DISCARDED_OCTETS,
            &octetsCtr) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
        break;
      }
      if (diffServPolicyPerfInObjectGet(pRow->ctrSource.policyIndex, 
            pRow->ctrSource.policyInstIndex, pRow->ctrSource.intIfNum,
            L7_DIFFSERV_POLICY_PERF_IN_HC_DISCARDED_PACKETS,
            &pktsCtr) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
        break;
      }
      pRow->mib.octets = octetsCtr;
      pRow->mib.pkts = pktsCtr;
    }
    if (objectId == L7_DIFFSERV_ALG_DROP_OCTETS)
      *(L7_ulong64 *)pValue = pRow->mib.octets;
    else
      *(L7_ulong64 *)pValue = pRow->mib.pkts;
    break;

  case L7_DIFFSERV_ALG_DROP_STORAGE:
    *(dsStdMibStorageType_t *)pValue = pRow->mib.storage;
    break;

  case L7_DIFFSERV_ALG_DROP_ROW_STATUS:
    *(dsStdMibRowStatus_t *)pValue = pRow->mib.status;
    break;
  
  default:
    rc = L7_FAILURE;
    break;
  }
  
  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
  return rc; 
}
                                                                           
