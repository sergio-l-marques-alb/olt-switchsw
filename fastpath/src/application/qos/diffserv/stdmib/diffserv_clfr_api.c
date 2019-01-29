/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   diffserv_clfr_api.c
*
* @purpose    DiffServ Standard MIB Classifier, Classifier Element,
*             MF Clfr, and Auxiliary MF Clfr Table APIs
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

/* Classifier element precedence */
#define DIFFSERV_CLFR_ELEMENT_PRECEDENCE_RANGE 1000
      
/* IP DSCP positional mask & shift values */
#define DSSTDMIB_AUX_MF_CLFR_TOS_MASK_IPDSCP          0xFC
#define DSSTDMIB_AUX_MF_CLFR_TOS_SHIFT_IPDSCP         2

/* IP Precedence positional mask & shift values */
#define DSSTDMIB_AUX_MF_CLFR_TOS_MASK_IPPRECEDENCE    0xE0
#define DSSTDMIB_AUX_MF_CLFR_TOS_SHIFT_IPPRECEDENCE   5

/* Globals for next free indexes*/
L7_uint32 clfrIdNextFree;                       /* next available clfr id                 */
L7_uint32 clfrElemIdNextFree;                   /* next available clfr elem Id            */
L7_uint32 multiFieldClfrIdNextFree;             /* next available multi field clfr id     */
L7_uint32 auxMFClfrIdNextFree;                  /* next available aux multi field clfr id */

/* Number of bytes in mask */
#define DSSTDMIB_CLFR_ID_INDICES                (((DSSTDMIB_CLFR_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)
#define DSSTDMIB_CLFR_ELEM_ID_INDICES           (((DSSTDMIB_CLFR_ELEMENT_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)
#define DSSTDMIB_MULTI_FIELD_CLFR_ID_INDICES    (((DSSTDMIB_MULTI_FIELD_CLFR_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)
#define DSSTDMIB_AUX_MF_CLFR_ID_INDICES         (((DSSTDMIB_AUX_MF_CLFR_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)

/* Mask storage definitions */
typedef struct
{
  L7_uchar8   value[DSSTDMIB_CLFR_ID_INDICES];
} DSSTDMIB_CLFR_ID_MASK_t;

typedef struct
{
  L7_uchar8   value[DSSTDMIB_CLFR_ELEM_ID_INDICES];
} DSSTDMIB_CLFR_ELEM_ID_MASK_t;

typedef struct
{
  L7_uchar8   value[DSSTDMIB_MULTI_FIELD_CLFR_ID_INDICES];
} DSSTDMIB_MULTI_FIELD_CLFR_ID_MASK_t;

typedef struct
{
  L7_uchar8   value[DSSTDMIB_AUX_MF_CLFR_ID_INDICES];
} DSSTDMIB_AUX_MF_CLFR_ID_MASK_t;

/* NOTE: Index 0 is reserved and must not be used */
DSSTDMIB_CLFR_ID_MASK_t                 clfrIdInUseMask;
DSSTDMIB_CLFR_ELEM_ID_MASK_t            clfrElemIdInUseMask;
DSSTDMIB_MULTI_FIELD_CLFR_ID_MASK_t     mfClfrIdInUseMask;
DSSTDMIB_AUX_MF_CLFR_ID_MASK_t          auxMFClfrIdInUseMask;

/* global matrix to correlate private MIB class and class rule indexes 
 * with Aux MF Clfr ID (row 0 and column 0 not used)
 */
dsStdMibAuxMFClfrReuse_t  dsAuxReuseMatrix_g[L7_DIFFSERV_CLASS_LIM+1]
                                            [L7_DIFFSERV_RULE_PER_CLASS_LIM+1];


/*********************************************************************
  NOTE: DATA PATH TABLE EXTERNAL APIs
*********************************************************************/

/*********************************************************************
* @purpose  Verify that a Data Path Table row exists for the specified 
*           interface and direction
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServDataPathGet(L7_uint32 intIfNum,
                    L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
  dsStdMibDataPathEntryCtrl_t *pRow;
  dsStdMibDataPathKey_t       keys;
  L7_uint32                   ifIndex;
  
  if(nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
  { 
    return L7_FAILURE;
  }
  
  if(DSSTDMIB_DATAPATH_INTF_INDEX_OUT_OF_RANGE(ifIndex) == L7_TRUE)
    return L7_FAILURE;

  if(DSSTDMIB_DATAPATH_DIR_INDEX_OUT_OF_RANGE(ifDirection) == L7_TRUE)
    return L7_FAILURE;

  keys.ifIndex = ifIndex;
  keys.ifDirection = ifDirection; 
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_DATA_PATH],
                      &keys, AVL_EXACT);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Data Path Table
*
* @param    prevIntIfNum    @b{(input)}  Internal interface number to 
                                         begin search
* @param    prevIfDirection @b{(input)}  Interface direction to begin
*                                        search
* @param    pIntIfNum       @b{(output)} Pointer to next sequential
*                                        internal interface number value
* @param    pIfDirection    @b{(output)} Pointer to next sequential
*                                        interface direction value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevIntIfNum value of 0 is used to find the first
*           interface entry in the table.
*
* @notes    A prevIfDirection value of 0 is used to find the first
*           interface direction entry relative to the specified
*           prevIntIfNum.
*
* @end
*********************************************************************/
L7_RC_t diffServDataPathGetNext(L7_uint32 prevIntIfNum, 
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t prevIfDirection,
                                L7_uint32 *pIntIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t *pIfDirection)
{
  dsStdMibDataPathEntryCtrl_t *pRow;
  dsStdMibDataPathKey_t       keys;
  L7_uint32                   ifIndex;

  DIFFSERV_NULLPTR_CHECK(pIntIfNum);
  DIFFSERV_NULLPTR_CHECK(pIfDirection);
  
  if(prevIntIfNum != 0)
  {
    if(nimGetIntfIfIndex(prevIntIfNum, &ifIndex) != L7_SUCCESS)
    { 
      *pIntIfNum = 0;
      *pIfDirection = 0;
      return L7_FAILURE;
    }  
  }
  else
  {
    ifIndex = 0;
  }
  
  keys.ifIndex  = ifIndex; 
  keys.ifDirection = prevIfDirection; 
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_DATA_PATH],
                      &keys, AVL_NEXT);
  if (pRow == L7_NULLPTR)
  {
    *pIntIfNum = 0;
    *pIfDirection = 0;
    return L7_FAILURE;
  }
  
  if(nimGetIntfNumber(pRow->key.ifIndex, pIntIfNum) != L7_SUCCESS)
  {
    *pIntIfNum = 0;
    *pIfDirection = 0;
    return L7_FAILURE;
  }
  
  *pIfDirection = pRow->key.ifDirection; 
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get the value of an object in the Data Path Table based on 
*           Object Id
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified indexes
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServDataPathObjectGet(L7_uint32 intIfNum,
                        L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                        L7_DIFFSERV_DATA_PATH_TABLE_OBJECT_t objectId,
                                  void *pValue)
{
  dsStdMibDataPathEntryCtrl_t *pRow;
  dsStdMibDataPathKey_t       keys;
  L7_uint32                   ifIndex;
  
  if(nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
  { 
    return L7_FAILURE;
  }

  if(DSSTDMIB_DATAPATH_INTF_INDEX_OUT_OF_RANGE(ifIndex) == L7_TRUE)
    return L7_FAILURE;

  if(DSSTDMIB_DATAPATH_DIR_INDEX_OUT_OF_RANGE(ifDirection) == L7_TRUE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pValue);

  keys.ifIndex = ifIndex;
  keys.ifDirection = ifDirection; 
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_DATA_PATH],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
  {
    return L7_ERROR;
  }
  
  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  switch(objectId)
  {
  case L7_DIFFSERV_DATA_PATH_START:
    *(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *)pValue = 
                              dsStdMibRowPtrXlateToExt(pRow->mib.start);
    break;

  case L7_DIFFSERV_DATA_PATH_STORAGE:
    *(dsStdMibStorageType_t *)pValue = pRow->mib.storage;
    break;

  case L7_DIFFSERV_DATA_PATH_ROW_STATUS:
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
===========================
===========================
===========================

   CLASSIFIER TABLE APIs

===========================
===========================
===========================
*/


/*********************************************************************
* @purpose  Initialize draft MIB Clfr support
*
* @param    void
*
* @returns  void
*
* @notes
*       
* @end
*********************************************************************/
void dsStdMibClfrInit(void)
{
  /* initialize index in-use bitmask */
  memset(&clfrIdInUseMask, 0, sizeof(clfrIdInUseMask));

  /* establish initial index next values */
  dsStdMibClfrIdNextUpdate();
}

/*********************************************************************
* @purpose  Obtain the next available index value to be used when  
*           creating a new row in the Classifier Table
*
* @param    pClfrId @b{(output)} Pointer to classifier index value
*
* @returns  void
*
* @notes    Only outputs a value if the pClfrId parm is non-null.
*
* @notes    An output *pClfrId value of 0 means the table is full.
*
* @notes    The index value remains free until used in a subsequent
*           create request. When two or more requestors get the same
*           index value, only the first one to successfully create a 
*           new row in this table gets to keep it.  All other requestors'
*           create requests will fail due to the index being in use,
*           and they must call this function again to get a new value.
*
* @end
*********************************************************************/
void dsStdMibClfrIdNext(L7_uint32 *pClfrId)
{
  if (pClfrId != L7_NULLPTR)
  {
    if (DSSTDMIB_CLFR_INDEX_OUT_OF_RANGE(clfrIdNextFree) == L7_TRUE)
    {
      *pClfrId = 0;
    }
    else
    {
      *pClfrId = clfrIdNextFree;
    }
  }
}

/*********************************************************************
* @purpose  Determine next available clfr Id value and update its value
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
void dsStdMibClfrIdNextUpdate(void)
{
  L7_uint32    i;

  for (i = 1; i <= DSSTDMIB_CLFR_INDEX_MAX; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(clfrIdInUseMask, i) == L7_FALSE)
      break;
  }
  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > DSSTDMIB_CLFR_INDEX_MAX)
    i = 0;

  clfrIdNextFree = i;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Clfr row element
*
* @param    clfrId      @b{(input)} Clfr Id
*
* @returns  pRow        Pointer to row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsStdMibClfrEntryCtrl_t *dsStdMibClfrRowFind(L7_uint32 clfrId)
{
  dsStdMibClfrEntryCtrl_t *pRow;
  dsStdMibClfrKey_t       keys;

  keys.clfrId = clfrId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_CLFR], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Create a new row in the Classifier Table
*
* @param    pPathNext   @b{(inout)} Pointer to next data path element
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    No more index values available (table full)
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServClfrCreate(dsStdMibRowPtr_t *pPathNext)
{
  L7_RC_t                  rc = L7_FAILURE;
  dsStdMibTableId_t        tableId = DSSTDMIB_TABLE_ID_CLFR;
  dsStdMibClfrEntryCtrl_t  clfrRow, *pClfrRow = L7_NULLPTR;
  L7_uint32                clfrId = DSSTDMIB_ID_TRACE_INIT_VAL;
  L7_uint32                precedenceBase;


  DIFFSERV_NULLPTR_CHECK(pPathNext);

  do
  {
    /* get the next available clfr id */
    dsStdMibClfrIdNext(&clfrId);

    if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
    {
      DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                               clfrId, 0, 0, 
                               (L7_uchar8)L7_TRUE);
    }

    /* clfr id 0 means the table is full */
    if (clfrId == 0)
    {
      rc = L7_ERROR;
      break;
    }

    pClfrRow = &clfrRow;
    memset(pClfrRow, 0, sizeof(*pClfrRow));

    /* initialize row index keys */
    pClfrRow->key.clfrId = clfrId;

    pClfrRow->tableId = tableId;

    /* set up various classifier element precedence control values */
    precedenceBase = clfrId * DIFFSERV_CLFR_ELEMENT_PRECEDENCE_RANGE;
    pClfrRow->precedenceAllExcl = precedenceBase + 
                                (DIFFSERV_CLFR_ELEMENT_PRECEDENCE_RANGE - 1);
    pClfrRow->precedenceAll = precedenceBase + 
                            (DIFFSERV_CLFR_ELEMENT_PRECEDENCE_RANGE / 2);
    pClfrRow->precedenceAny = pClfrRow->precedenceAll - 1;

    /* Set other objects in clfr entry*/
    pClfrRow->mib.id = clfrId;
    pClfrRow->mib.storage = DSSTDMIB_DEFAULT_STORAGE_TYPE;
    pClfrRow->mib.status = L7_DIFFSERV_ROW_STATUS_ACTIVE;

    /* Insert in AVL */
    if (dsmibRowInsert(&dsStdMibAvlTree[tableId], (void *)pClfrRow) != L7_SUCCESS)
      break;

    /* successful row creation -- update next free index value*/
    L7_INTF_SETMASKBIT(clfrIdInUseMask, clfrId);
    dsStdMibClfrIdNextUpdate();

    /* update data path next row info */
    DSSTDMIB_PATH_NEXT_UPDATE(pPathNext, tableId, clfrId, 0, 0);

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            clfrId, 0, 0,
                            (L7_uint32)pClfrRow, (L7_uchar8)rc,
                            (L7_uchar8)L7_TRUE);
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete a row from the Clfr Table for the specified index
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
L7_RC_t diffServClfrDelete(dsStdMibRowPtr_t *pPathNext)
{
  L7_RC_t                         rc = L7_FAILURE;
  dsStdMibTableId_t               tableId = DSSTDMIB_TABLE_ID_CLFR;
  dsStdMibClfrEntryCtrl_t         *pClfrRow = L7_NULLPTR;
  L7_uint32                       clfrId;
  
  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* make sure tableId is correct */
  if (pPathNext->tableId != tableId)
    return L7_FAILURE;

  clfrId = pPathNext->rowIndex1;

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                             clfrId, 0, 0);
  }

  do
  {
    if(DSSTDMIB_CLFR_INDEX_OUT_OF_RANGE(clfrId) == L7_TRUE)
      break;

    pClfrRow = dsStdMibClfrRowFind(clfrId);
    if (pClfrRow == L7_NULLPTR)
    {
      *pPathNext = DSSTDMIB_ZERO_DOT_ZERO;
      rc = L7_SUCCESS;          /* consider this a success if row not found */
      break;
    }

    if (dsmibRowDelete(&dsStdMibAvlTree[tableId], pClfrRow) != L7_SUCCESS)
      break;

    /* successful row deletion -- update next free index value*/
    L7_INTF_CLRMASKBIT(clfrIdInUseMask, clfrId);
    dsStdMibClfrIdNextUpdate();

    /* update data path next row info -- there is no 'next' ptr in this elem */
    *pPathNext = DSSTDMIB_ZERO_DOT_ZERO;

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            clfrId, 0, 0,
                            (L7_uint32)pClfrRow, (L7_uchar8)rc);
  }

  return rc;
}


/*********************************************************************
  NOTE: CLASSIFIER TABLE EXTERNAL APIs
*********************************************************************/

/*************************************************************************
* @purpose  Obtain the current value of the classifier index next variable
*
* @param    pClfrIndex  @b{(output)} Pointer to classifier index value
*
* @returns  void
*
* @notes    Only outputs a value if the pClfrIndex parm is non-null.
*
* @notes    A *pClfrIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServClfrIndexNext(L7_uint32 *pClfrIndex)
{
  if (pClfrIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  dsStdMibClfrIdNext(pClfrIndex);

  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
}

/*********************************************************************
* @purpose  Verify that a Classifier Table row exists for the
*           specified classifier Id.
*
* @param    clfrId      @b{(input)} Classifier Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClfrGet(L7_uint32 clfrId)
{
  dsStdMibClfrEntryCtrl_t  *pRow;
  dsStdMibClfrKey_t        keys;

  if(DSSTDMIB_CLFR_INDEX_OUT_OF_RANGE(clfrId) == L7_TRUE)
    return L7_FAILURE;

  keys.clfrId = clfrId; 

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_CLFR],
                      &keys, AVL_EXACT);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Classifier Table
*
* @param    prevClfrId  @b{(input)}  Classifier Id to begin search
* @param    pClfrId     @b{(output)} Pointer to next sequential
*                                    classifier Id.
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevClfrId value of 0 is used to find the first classifier
*           entry in the table.
*
* @end
*********************************************************************/
L7_RC_t diffServClfrGetNext(L7_uint32 prevClfrId, L7_uint32 *pClfrId)
{
  dsStdMibClfrEntryCtrl_t  *pRow;
  dsStdMibClfrKey_t        keys;

  DIFFSERV_NULLPTR_CHECK(pClfrId);

  keys.clfrId = prevClfrId; 
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_CLFR],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
  {
    *pClfrId = 0;
    return L7_FAILURE;
  }  
  
  *pClfrId = pRow->key.clfrId;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get the value of an object in the Classifier Table based
*           on Object Id
*
* @param    clfrId      @b{(input)}  Classifier Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClfrObjectGet(L7_uint32 clfrId,
                              L7_DIFFSERV_CLFR_TABLE_OBJECT_t objectId,
                              void *pValue)
{
  dsStdMibClfrEntryCtrl_t  *pRow;
  dsStdMibClfrKey_t        keys;

  if(DSSTDMIB_CLFR_INDEX_OUT_OF_RANGE(clfrId) == L7_TRUE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pValue);

  keys.clfrId = clfrId; 
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_CLFR],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
  {
    return L7_ERROR;
  }
  
  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  switch(objectId)
  {
  case L7_DIFFSERV_CLFR_STORAGE:
    *(dsStdMibStorageType_t *)pValue = pRow->mib.storage;
    break;

  case L7_DIFFSERV_CLFR_ROW_STATUS:
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
===================================
===================================
===================================

   CLASSIFIER ELEMENT TABLE APIs

===================================
===================================
===================================
*/

/*********************************************************************
* @purpose  Initialize draft MIB Clfr Element support
*
* @param    void
*
* @returns  void
*
* @notes
*       
* @end
*********************************************************************/
void dsStdMibClfrElemInit(void)
{
  /* initialize index in-use bitmask */
  memset(&clfrElemIdInUseMask, 0, sizeof(clfrElemIdInUseMask));

  /* establish initial index next values */
  dsStdMibClfrElemIdNextUpdate();
}

/*********************************************************************
* @purpose  Obtain the next available index value to be used when
*           creating a new row in the Classifier Element Table
*
* @param    pClfrElemId   @b{(output)} Pointer to next free Classifier 
*                                      Element Id
*
* @returns  void
*
* @notes    Only outputs a value if the pClfrElemId parm is non-null.
*
* @notes    An output *pClfrElemId value of 0 means the table is full.
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
void dsStdMibClfrElemIdNext(L7_uint32 *pClfrElemId)
{
  if (pClfrElemId != L7_NULLPTR)
  {
    if (DSSTDMIB_CLFR_ELEMENT_INDEX_OUT_OF_RANGE(clfrElemIdNextFree) == L7_TRUE)
    {
      *pClfrElemId = 0;
    }
    else
    {
      *pClfrElemId = clfrElemIdNextFree;
    }
  }
}


/*********************************************************************
* @purpose  Update clfr element Id value
*
* @param    void
*
* @returns  void  
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
void dsStdMibClfrElemIdNextUpdate()
{
  L7_uint32    i;

  for (i = 1; i <= DSSTDMIB_CLFR_ELEMENT_INDEX_MAX; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(clfrElemIdInUseMask, i) == L7_FALSE)
      break;
  }
  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > DSSTDMIB_CLFR_ELEMENT_INDEX_MAX)
    i = 0;

  clfrElemIdNextFree = i;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Clfr Element row
*
* @param    clfrId      @b{(input)} Clfr Id
* @param    clfrElemId  @b{(input)} Clfr Element Id
*
* @returns  pRow        Pointer to row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsStdMibClfrElementEntryCtrl_t *dsStdMibClfrElemRowFind(L7_uint32 clfrId, 
                                                        L7_uint32 clfrElemId)
{
  dsStdMibClfrElementEntryCtrl_t *pRow;
  dsStdMibClfrElementKey_t keys;
  
  keys.clfrId = clfrId;
  keys.clfrElemId = clfrElemId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_CLFR_ELEMENT], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Create a new row in the Classifier Element Table 
*
* @param    clfrId      @b{(input)}  Classifier  Id
* @param    pSpecific   @b{(input)}  Pointer to specific Data Path
*                                    Element
* @param    precedence  @b{(input)}  Classifier Element precedence 
* @param    pPathNext   @b{(inout)}  Pointer to next data path element
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    No more index values available (table full)
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServClfrElemCreate(L7_uint32 clfrId, 
                               dsStdMibRowPtr_t *pSpecific, 
                               L7_uint32 precedence,
                               dsStdMibRowPtr_t  *pPathNext)
{
  L7_RC_t                        rc = L7_FAILURE;
  dsStdMibTableId_t              tableId = DSSTDMIB_TABLE_ID_CLFR_ELEMENT;
  dsStdMibClfrElementEntryCtrl_t clfrElemRow, *pClfrElemRow = L7_NULLPTR;
  L7_uint32                      clfrElemId = DSSTDMIB_ID_TRACE_INIT_VAL;
  dsStdMibClfrEntryCtrl_t        *pClfrRow;

  DIFFSERV_NULLPTR_CHECK(pSpecific);
  DIFFSERV_NULLPTR_CHECK(pPathNext);

  do
  {
    /* get the next available clfr element id */
    dsStdMibClfrElemIdNext(&clfrElemId);
        
    if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
    {
      DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                               clfrId, clfrElemId, 0, 
                               (L7_uchar8)L7_TRUE);
    }

    /* clfr element id 0 means the table is full */
    if (clfrElemId == 0)
    {
      rc = L7_ERROR;
      break;
    }

    pClfrRow = dsStdMibClfrRowFind(clfrId);
    if (pClfrRow == L7_NULLPTR)
      break;

    pClfrElemRow = &clfrElemRow;
    memset(pClfrElemRow, 0, sizeof(*pClfrElemRow));

    pClfrElemRow->key.clfrId = clfrId;
    pClfrElemRow->key.clfrElemId = clfrElemId;

    pClfrElemRow->tableId = tableId;

    if(precedence == DSSTDMIB_CLFR_ELEM_PRECEDENCE_ALLEXCL)
    {
      pClfrElemRow->mib.precedence = pClfrRow->precedenceAllExcl--;
    }
    else if(precedence == DSSTDMIB_CLFR_ELEM_PRECEDENCE_ALL)
    {
      pClfrElemRow->mib.precedence = pClfrRow->precedenceAll; /* always same */
    }
    else  /* DSSTDMIB_CLFR_ELEM_PRECEDENCE_ANY */
    {
      pClfrElemRow->mib.precedence = pClfrRow->precedenceAny--;
    }

    pClfrElemRow->mib.id = clfrElemId;

    pClfrElemRow->mib.specific = *pSpecific;
    pClfrElemRow->mib.next = *pPathNext;
    pClfrElemRow->mib.storage = DSSTDMIB_DEFAULT_STORAGE_TYPE;
    pClfrElemRow->mib.status = L7_DIFFSERV_ROW_STATUS_ACTIVE; 

    if (dsmibRowInsert(&dsStdMibAvlTree[tableId], pClfrElemRow) != L7_SUCCESS)
      break;

    /* successful row creation -- update next free index value*/
    L7_INTF_SETMASKBIT(clfrElemIdInUseMask, clfrElemId);
    dsStdMibClfrElemIdNextUpdate();

    /* update data path next row info */
    DSSTDMIB_PATH_NEXT_UPDATE(pPathNext, tableId, clfrId, clfrElemId, 0);

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            clfrId, clfrElemId, 0,
                            (L7_uint32)pClfrElemRow, (L7_uchar8)rc,
                            (L7_uchar8)L7_TRUE);
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete a row from the Clfr Element Table for the specified
*           indexes
*
* @param    pPathNext   @b{(inout)}  Pointer to next data path element
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClfrElemDelete(dsStdMibRowPtr_t  *pPathNext)
{
  L7_RC_t                        rc = L7_FAILURE;
  dsStdMibTableId_t              tableId = DSSTDMIB_TABLE_ID_CLFR_ELEMENT;
  dsStdMibClfrElementEntryCtrl_t *pClfrElemRow = L7_NULLPTR;
  L7_uint32                      clfrId, clfrElemId;

  
  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* make sure tableId is correct */
  if (pPathNext->tableId != tableId)
    return L7_FAILURE;

  clfrId = pPathNext->rowIndex1;
  clfrElemId = pPathNext->rowIndex2;

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                             clfrId, clfrElemId, 0);
  }

  do
  {
    if(DSSTDMIB_CLFR_INDEX_OUT_OF_RANGE(clfrId) == L7_TRUE)
      break;

    if (DSSTDMIB_CLFR_ELEMENT_INDEX_OUT_OF_RANGE(clfrElemId) == L7_TRUE)
      break;

    pClfrElemRow = dsStdMibClfrElemRowFind(clfrId, clfrElemId);
    if (pClfrElemRow == L7_NULLPTR)
    {
      *pPathNext = DSSTDMIB_ZERO_DOT_ZERO;
      rc = L7_SUCCESS;          /* consider this a success if row not found */
      break;
    }

    /* delete specific elem (Aux MFClfr) */
    if (dsStdMibPrimitiveElemDelete(&pClfrElemRow->mib.specific) != L7_SUCCESS)
      break;

    /* update data path next row info before deleting element */
    *pPathNext = pClfrElemRow->mib.next;

    /* delete the element */
    if (dsmibRowDelete(&dsStdMibAvlTree[tableId], pClfrElemRow) != L7_SUCCESS)
      break;

    /* successful row deletion -- update next free index value*/
    L7_INTF_CLRMASKBIT(clfrElemIdInUseMask, clfrElemId);
    dsStdMibClfrElemIdNextUpdate();

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            clfrId, clfrElemId, 0,
                            (L7_uint32)pClfrElemRow, (L7_uchar8)rc);
  }

  return rc;
}
 
/********************************************************************* 
  NOTE: CLASSIFIER ELEMENT TABLE EXTERNAL APIs
*********************************************************************/

/*************************************************************************
* @purpose  Obtain the current value of the classifier element index
*           next variable
*
* @param    pClfrElemIndex  @b{(output)} Pointer to classifier element
*                                        index value
*
* @returns  void
*
* @notes    Only outputs a value if the pClfrElemIndex parm is non-null.
*
* @notes    A *pClfrElemIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServClfrElemIndexNext(L7_uint32 *pClfrElemIndex)
{
  if (pClfrElemIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  dsStdMibClfrElemIdNext(pClfrElemIndex);

  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
}

/*********************************************************************
* @purpose  Verify that a Classifier Element Table row exists for the 
*           specified Classifier Id and Classifier Element Id.
*
* @param    clfrId      @b{(input)} Classifier  Id
* @param    clfrElemId  @b{(input)} Classifier Element Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClfrElemGet(L7_uint32 clfrId, L7_uint32 clfrElemId)
{
  dsStdMibClfrElementEntryCtrl_t *pRow; 
  dsStdMibClfrElementKey_t        keys;

  if(DSSTDMIB_CLFR_INDEX_OUT_OF_RANGE(clfrId) == L7_TRUE)
    return L7_FAILURE;

  if(DSSTDMIB_CLFR_ELEMENT_INDEX_OUT_OF_RANGE(clfrElemId) == L7_TRUE)
    return L7_FAILURE;

  keys.clfrId = clfrId;
  keys.clfrElemId = clfrElemId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_CLFR_ELEMENT],
                      &keys, AVL_EXACT);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential row in Classifier Element Table
*
* @param    prevClfrId      @b{(input)}  Classifier Id to begin search
* @param    prevClfrElemId  @b{(input)}  Classifier Element Id to
*                                        begin search
* @param    pClfrId         @b{(output)} Pointer to next sequential
*                                        classifier Id
* @param    pClfrElemId     @b{(output)} Pointer to next sequential
*                                        classifier Element Id.
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevClfrElemId value of 0 is used to find the first
*           classifier element entry in the table.
*
* @end
*********************************************************************/
L7_RC_t diffServClfrElemGetNext(L7_uint32 prevClfrId, 
                                L7_uint32 prevClfrElemId, 
                                L7_uint32 *pClfrId, 
                                L7_uint32 *pClfrElemId)
{
  dsStdMibClfrElementEntryCtrl_t *pRow; 
  dsStdMibClfrElementKey_t        keys;

  DIFFSERV_NULLPTR_CHECK(pClfrId);
  DIFFSERV_NULLPTR_CHECK(pClfrElemId);

  keys.clfrId = prevClfrId;
  keys.clfrElemId = prevClfrElemId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_CLFR_ELEMENT],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
  {
     *pClfrId = *pClfrElemId = 0;
     return L7_FAILURE;
  }
  
  *pClfrId = pRow->key.clfrId;
  *pClfrElemId = pRow->key.clfrElemId;
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get the value of an object in the Classifier Element Table
*           based on Object Id
*
* @param    clfrId      @b{(input)}  Classifier Id
* @param    clfrElemId  @b{(input)}  Classifier Element Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServClfrElemObjectGet(L7_uint32 clfrId, L7_uint32 clfrElemId, 
                        L7_DIFFSERV_CLFR_ELEMENT_TABLE_OBJECT_t objectId,
                                  void *pValue)
{
  dsStdMibClfrElementEntryCtrl_t *pRow; 
  dsStdMibClfrElementKey_t        keys;
  
  if(DSSTDMIB_CLFR_INDEX_OUT_OF_RANGE(clfrId) == L7_TRUE)
    return L7_FAILURE;

  if(DSSTDMIB_CLFR_ELEMENT_INDEX_OUT_OF_RANGE(clfrElemId) == L7_TRUE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pValue);
   
  keys.clfrId = clfrId;
  keys.clfrElemId = clfrElemId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_CLFR_ELEMENT],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
  {
    return L7_ERROR;
  }

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  switch(objectId)
  {
  case L7_DIFFSERV_CLFR_ELEMENT_PRECEDENCE:
    *(L7_uint32 *)pValue = pRow->mib.precedence;
    break;

  case L7_DIFFSERV_CLFR_ELEMENT_NEXT:
    *(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *)pValue = 
                              dsStdMibRowPtrXlateToExt(pRow->mib.next);
    break;

  case L7_DIFFSERV_CLFR_ELEMENT_SPECIFIC:
    *(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *)pValue = 
                              dsStdMibRowPtrXlateToExt(pRow->mib.specific);
    break;
  
  case L7_DIFFSERV_CLFR_ELEMENT_STORAGE:
    *(dsStdMibStorageType_t *)pValue = pRow->mib.storage;
    break;

  case L7_DIFFSERV_CLFR_ELEMENT_ROW_STATUS:
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
==========================================
==========================================
==========================================

   IP MULTI-FIELD CLASSIFIER TABLE APIs

==========================================
==========================================
==========================================
*/
/*********************************************************************
* @purpose  Initialize draft MIB MF Clfr support
*
* @param    void
*
* @returns  void
*
* @notes
*       
* @end
*********************************************************************/
void dsStdMibMFClfrInit(void)
{
  /* initialize index in-use bitmask */
  memset(&mfClfrIdInUseMask, 0, sizeof(mfClfrIdInUseMask));

  /* establish initial index next values */
  dsStdMibMFClfrIdNextUpdate();
}

/*********************************************************************
* @purpose  Obtain the next available index value to be used when
*           creating a new row in the Multi Field Classifier Table
*
* @param    pMfClfrId   @b{(output)} Pointer to Multi field
*                                    Classifier Id
*
* @returns  void
*
* @notes    Only outputs a value if the pMfClfrId parm is non-null.
*
* @notes    An output *pMfClfrId value of 0 means the table is full.
*
* @notes    The index value remains free until used in a subsequent '
*           request.  When two or more requestors get the same index
*           value, only the first one to successfully create a new row
*           in this table gets to keep it.  All other requestors'
*           create requests will fail due to the index being in use,
*           and they must call this function again to get a new value.
*
* @end
*********************************************************************/
void dsStdMibMFClfrIdNext(L7_uint32 *pMfClfrId)
{
  if (pMfClfrId != L7_NULLPTR)
  {
    if (DSSTDMIB_MULTI_FIELD_CLFR_INDEX_OUT_OF_RANGE(multiFieldClfrIdNextFree) == L7_TRUE)
    {
      *pMfClfrId = 0;
    }
    else
    {
      *pMfClfrId = multiFieldClfrIdNextFree;
    }
  }
}

/*********************************************************************
* @purpose  Determine next available MF clfr Id value and update its value
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
void dsStdMibMFClfrIdNextUpdate(void)
{
  L7_uint32    i;

  for (i = 1; i <= DSSTDMIB_MULTI_FIELD_CLFR_INDEX_MAX; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(mfClfrIdInUseMask, i) == L7_FALSE)
      break;
  }
  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > DSSTDMIB_MULTI_FIELD_CLFR_INDEX_MAX)
    i = 0;

  multiFieldClfrIdNextFree = i;
}

/*********************************************************************
  NOTE: IP MULTI-FIELD CLASSIFIER TABLE EXTERNAL APIs
*********************************************************************/
 
/*************************************************************************
* @purpose  Obtain the current value of the multifield classifier index
*           next variable
*
* @param    pMFClfrIndex  @b{(output)} Pointer to multifield classifier
*                                      index value
*
* @returns  void
*
* @notes    Only outputs a value if the pMFClfrIndex parm is non-null.
*
* @notes    A *pMFClfrIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServMFClfrIndexNext(L7_uint32 *pMFClfrIndex)
{
  if (pMFClfrIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  dsStdMibMFClfrIdNext(pMFClfrIndex);

  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
}

/*********************************************************************
* @purpose  Verify that a IP Multi Field Classifier Table row exists 
*           for the specified Multi Field Clfr Id.
*
* @param    mfClfrId    @b{(input)} Multi field Classifier Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServMFClfrGet(L7_uint32 mfClfrId)
{
  dsStdMibMultiFieldClfrEntryCtrl_t *pRow; 
  dsStdMibMultiFieldClfrKey_t        keys;

  if(DSSTDMIB_MULTI_FIELD_CLFR_INDEX_OUT_OF_RANGE(mfClfrId) == L7_TRUE)
    return L7_FAILURE;

  keys.multiFieldClfrId = mfClfrId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_MULTI_FIELD_CLFR],
                      &keys, AVL_EXACT);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential row in the IP Multi Field
*           Classifier Table
*
* @param    prevMfClfrId  @b{(input)}  Multi field clfr id to begin
*                                      search
* @param    pMfClfrId     @b{(output)} Pointer to next sequential
*                                      multi field clfr id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A pMfClfrId value of 0 is used to find the first multi
*           field classifier entry in the table.
*
* @end
*********************************************************************/
L7_RC_t diffServMFClfrGetNext(L7_uint32 prevMfClfrId, L7_uint32 *pMfClfrId)
{
  dsStdMibMultiFieldClfrEntryCtrl_t *pRow; 
  dsStdMibMultiFieldClfrKey_t        keys;
  
  DIFFSERV_NULLPTR_CHECK(pMfClfrId);

  keys.multiFieldClfrId = prevMfClfrId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_MULTI_FIELD_CLFR],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
  {
    *pMfClfrId = 0;
    return L7_FAILURE;
  }

  *pMfClfrId = pRow->key.multiFieldClfrId;
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get the value of an object in the Multi Field Classifier
*           Table based on Object Id
*
* @param    mfClfrId    @b{(input)}  Multi field Classifier Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServMFClfrObjectGet(L7_uint32 mfClfrId,
                L7_DIFFSERV_MULTI_FIELD_CLFR_TABLE_OBJECT_t objectId,
                                void *pValue)
{
  dsStdMibMultiFieldClfrEntryCtrl_t *pRow; 
  dsStdMibMultiFieldClfrKey_t        keys;

  if(DSSTDMIB_MULTI_FIELD_CLFR_INDEX_OUT_OF_RANGE(mfClfrId) == L7_TRUE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pValue);

  keys.multiFieldClfrId = mfClfrId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_MULTI_FIELD_CLFR],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
  {
    return L7_ERROR;
  }
  
  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  switch(objectId)
  {
  case L7_DIFFSERV_MULTI_FIELD_CLFR_ADDR_TYPE:
    *(dsStdMibInetAddrType_t *)pValue = pRow->mib.addrType;
    break;

  case L7_DIFFSERV_MULTI_FIELD_CLFR_DST_ADDR:
    *(L7_uint32 *)pValue = pRow->mib.dstAddr;
    break;

  case L7_DIFFSERV_MULTI_FIELD_CLFR_DST_PREFIX_LENGTH:
    *(L7_uint32 *)pValue = pRow->mib.dstPrefixLen;
    break;

  case L7_DIFFSERV_MULTI_FIELD_CLFR_SRC_ADDR:
    *(L7_uint32 *)pValue = pRow->mib.srcAddr;
    break;

  case L7_DIFFSERV_MULTI_FIELD_CLFR_SRC_PREFIX_LENGTH:
    *(L7_uint32 *)pValue = pRow->mib.srcPrefixLen;
    break;
    
  case L7_DIFFSERV_MULTI_FIELD_CLFR_DSCP:
    *(L7_uint32 *)pValue = (L7_uint32)pRow->mib.dscp;
    break;

  case L7_DIFFSERV_MULTI_FIELD_CLFR_FLOW_ID:
    *(L7_uint32 *)pValue = pRow->mib.flowId;
    break;

  case L7_DIFFSERV_MULTI_FIELD_CLFR_PROTOCOL:
    *(L7_uint32 *)pValue = pRow->mib.protocol;
    break;
  
  case L7_DIFFSERV_MULTI_FIELD_CLFR_DSTL4PORT_MIN:
    *(L7_uint32 *)pValue = pRow->mib.dstL4PortMin;
    break;

  case L7_DIFFSERV_MULTI_FIELD_CLFR_DSTL4PORT_MAX:
    *(L7_uint32 *)pValue = pRow->mib.dstL4PortMax;
    break;
  
  case L7_DIFFSERV_MULTI_FIELD_CLFR_SRCL4PORT_MIN:
    *(L7_uint32 *)pValue = pRow->mib.srcL4PortMin;
    break;
  
  case L7_DIFFSERV_MULTI_FIELD_CLFR_SRCL4PORT_MAX:
    *(L7_uint32 *)pValue = pRow->mib.srcL4PortMax;
    break;
  
  case L7_DIFFSERV_MULTI_FIELD_CLFR_STORAGE:
    *(dsStdMibStorageType_t *)pValue = pRow->mib.storage;
    break;
  
  case L7_DIFFSERV_MULTI_FIELD_CLFR_ROW_STATUS:
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
=================================================
=================================================
=================================================

   AUXILIARY MULTI-FIELD CLASSIFIER TABLE APIs

=================================================
=================================================
=================================================
*/

/*********************************************************************
* @purpose  Initialize draft MIB Auxiliary MF Clfr support
*
* @param    void
*
* @returns  void
*
* @notes
*       
* @end
*********************************************************************/
void dsStdMibAuxMFClfrInit(void)
{
  /* initialize index in-use bitmask */
  memset(&auxMFClfrIdInUseMask, 0, sizeof(auxMFClfrIdInUseMask));

  /* establish initial index next values */
  dsStdMibAuxMFClfrIdNextUpdate();

  /* zero out the aux reuse matrix */
  memset(dsAuxReuseMatrix_g, 0, sizeof(dsAuxReuseMatrix_g));
}

/*********************************************************************
* @purpose  Obtain the next available index value to be used when  
*           creating a new row in the Auxiliary Multi Field Classifier
*           Table
*
* @param    pAuxMFClfrId    @b{(output)} Pointer to Auxiliary Multi
*                                        Field Classifier Id
*
* @returns  void
*
* @notes    Only outputs a value if the pAuxMFClfrId parm is non-null.
*
* @notes    An output *pAuxMfClfrId value of 0 means the table is full.
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
void dsStdMibAuxMFClfrIdNext(L7_uint32 *pAuxMFClfrId)
{
  if (pAuxMFClfrId != L7_NULLPTR)
  {
    if (DSSTDMIB_AUX_MF_CLFR_INDEX_OUT_OF_RANGE(auxMFClfrIdNextFree) == L7_TRUE)
    {
      *pAuxMFClfrId = 0;
    }
    else
    {
      *pAuxMFClfrId = auxMFClfrIdNextFree;
    }
  }
}

/*********************************************************************
* @purpose  Determine next available Auxiliary MF clfr Id value and
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
void dsStdMibAuxMFClfrIdNextUpdate(void)
{
  L7_uint32    i;

  for (i = 1; i <= DSSTDMIB_AUX_MF_CLFR_INDEX_MAX; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(auxMFClfrIdInUseMask, i) == L7_FALSE)
      break;
  }
  /* Use a value of 0 to indicate all indexes currently in use */
  if (i >  DSSTDMIB_AUX_MF_CLFR_INDEX_MAX)
    i = 0;

  auxMFClfrIdNextFree = i;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Auxiliary Multi Field Clfr row
*
* @param    auxMFClfrId  @b{(input)}  Auxiliary Multi Field Clfr Id
*
* @returns  pRow        Pointer to row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsStdMibAuxMFClfrEntryCtrl_t *dsStdMibAuxMFClfrRowFind(L7_uint32 auxMFClfrId)
{
  dsStdMibAuxMFClfrEntryCtrl_t  *pRow;
  dsStdMibAuxMFClfrKey_t        keys;
  
  keys.auxMFClfrId = auxMFClfrId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_AUX_MF_CLFR], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Create a new row in the Auxiliary Multifield Classifier
*           Table 
*
* @param    pAuxMFClfrRow  @b{(input)}  Auxiliary Multi Field
*                                       Classifier row info ptr
* @param    pAuxMFClfrId   @b{(output)} Pointer to Auxiliary Multi
*                                       Field Classifier Id value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    No more index values available (table full)
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServAuxMFClfrCreate(dsStdMibAuxMFClfrEntryCtrl_t *pAuxMFClfrRow,
                                L7_uint32 *pAuxMFClfrId)
{
  L7_RC_t                   rc = L7_FAILURE;
  dsStdMibTableId_t         tableId = DSSTDMIB_TABLE_ID_AUX_MF_CLFR;
  L7_uint32                 auxMFClfrId;
  dsStdMibAuxMFClfrReuse_t  *pMatrix;

  DIFFSERV_NULLPTR_CHECK(pAuxMFClfrRow);
  DIFFSERV_NULLPTR_CHECK(pAuxMFClfrId);

  do
  {
    /* reuse an existing aux clfr that matches the same class index and 
     * class rule index as the new row
     */
    pMatrix = &dsAuxReuseMatrix_g[pAuxMFClfrRow->classIndex]
                                 [pAuxMFClfrRow->classRuleIndex];
    if (pMatrix->auxMFClfrId != 0)
    {
      *pAuxMFClfrId = auxMFClfrId = pMatrix->auxMFClfrId;
      pMatrix->refCount++;
      rc = L7_SUCCESS;
      break;
    }

    /* get the next available aux multi field clfr id */
    dsStdMibAuxMFClfrIdNext(pAuxMFClfrId);

    /* set up a local working copy of the Id */
    auxMFClfrId = *pAuxMFClfrId;

    if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
    {
      DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                               auxMFClfrId, 0, 0, 
                               (L7_uchar8)L7_TRUE);
    }

    /* check for table full */
    if (auxMFClfrId == 0)
    {
      rc = L7_ERROR;
      break;
    }

    pAuxMFClfrRow->key.auxMFClfrId = auxMFClfrId;
    pAuxMFClfrRow->mib.id = auxMFClfrId;

    if (dsmibRowInsert(&dsStdMibAvlTree[tableId], (void *)pAuxMFClfrRow) != L7_SUCCESS)
      break;

    /* successful row creation -- update next free index value*/
    L7_INTF_SETMASKBIT(auxMFClfrIdInUseMask, auxMFClfrId);
    dsStdMibAuxMFClfrIdNextUpdate();

    pMatrix->auxMFClfrId = auxMFClfrId;
    pMatrix->refCount = 1;

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            auxMFClfrId, 0, 0,
                            (L7_uint32)pAuxMFClfrRow, (L7_uchar8)rc,
                            (L7_uchar8)L7_TRUE);
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete a row from the Auxiliary Multifield Clfr Table 
*           for the specified index
*
* @param    auxMFClfrId   @b{(input)} Auxiliary Multi Field Clfr id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServAuxMFClfrDelete(L7_uint32 auxMFClfrId)
{
  L7_RC_t                      rc = L7_FAILURE;
  dsStdMibTableId_t            tableId = DSSTDMIB_TABLE_ID_AUX_MF_CLFR;
  dsStdMibAuxMFClfrEntryCtrl_t *pAuxMFClfrRow = L7_NULLPTR;
  dsStdMibAuxMFClfrReuse_t     *pMatrix;

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_PRVTMIB, (L7_uchar8)tableId, 
                             auxMFClfrId, 0, 0);
  }

  do
  {
    if(DSSTDMIB_AUX_MF_CLFR_INDEX_OUT_OF_RANGE(auxMFClfrId) == L7_TRUE)
      break;

    pAuxMFClfrRow = dsStdMibAuxMFClfrRowFind(auxMFClfrId);
    if (pAuxMFClfrRow == L7_NULLPTR)
    {
      rc = L7_SUCCESS;          /* consider this a success if row not found */
      break;
    }

    /* do not delete the aux clfr unless this is the last reference to it */
    pMatrix = &dsAuxReuseMatrix_g[pAuxMFClfrRow->classIndex]
                                 [pAuxMFClfrRow->classRuleIndex];
    if (pMatrix->refCount > 1)
    {
      pMatrix->refCount--;
      rc = L7_SUCCESS;
      break;
    }

    if (dsmibRowDelete(&dsStdMibAvlTree[tableId], pAuxMFClfrRow) != L7_SUCCESS)
      break;

    /* successful row deletion -- update next free index value*/
    L7_INTF_CLRMASKBIT(auxMFClfrIdInUseMask, auxMFClfrId);
    dsStdMibAuxMFClfrIdNextUpdate();

    pMatrix->auxMFClfrId = 0;
    pMatrix->refCount = 0;

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            auxMFClfrId, 0, 0,
                            (L7_uint32)pAuxMFClfrRow, (L7_uchar8)rc);
  }

  return rc;
}

/*********************************************************************
* @purpose  Set an object of Auxiliary Multi Field Classifier Table
*
* @param    classIndex     @b{(input)}  Class Index
* @param    classRuleIndex @b{(input)}  Class Rule Index
* @param    pAuxMFClfrRow  @b{(input)}  Pointer Aux MF Clfr Row
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes 
*       
* @end
*********************************************************************/
L7_RC_t dsStdMibAuxMFClfrObjectSet(L7_uint32 classIndex, 
                                   L7_uint32 classRuleIndex,
                                   dsStdMibAuxMFClfrEntryCtrl_t *pAuxMFClfrRow)
{
  dsmibClassRuleType_t classRuleType;

  if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                 L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE,
                                 (void *)&classRuleType) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  switch (classRuleType)
  {
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIP:
    {
      L7_uint32 dstAddr, dstMask;

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_DSTIP_ADDR, 
                                    (void *)&dstAddr) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      
      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_DSTIP_MASK, 
                                    (void *)&dstMask) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      pAuxMFClfrRow->mib.dstAddr = dstAddr;
      pAuxMFClfrRow->mib.dstMask = dstMask;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTIPV6:
    {
      L7_in6_addr_t dstAddr;
      L7_uint32 dstPrefLength;

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_DSTIPV6_ADDR, 
                                    (void *)&dstAddr) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      
      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_DSTIPV6_PLEN, 
                                    (void *)&dstPrefLength) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      memcpy(&pAuxMFClfrRow->mib.ipv6DstAddr, &dstAddr, sizeof(dstAddr));
      pAuxMFClfrRow->mib.ipv6DstPlen = dstPrefLength;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTL4PORT:
    {
      L7_uint32 dstL4PortMin, dstL4PortMax;
      
      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_DSTL4PORT_START, 
                                    (void *)&dstL4PortMin) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      
      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_DSTL4PORT_END, 
                                    (void *)&dstL4PortMax) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      pAuxMFClfrRow->mib.dstL4PortMin = dstL4PortMin;
      pAuxMFClfrRow->mib.dstL4PortMax = dstL4PortMax;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_FLOWLBLV6:
    {
      L7_uint32 ip6FlowLabel;

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_IPV6_FLOWLBL, 
                                    (void *)&ip6FlowLabel) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      pAuxMFClfrRow->mib.ipv6FlowLabel = ip6FlowLabel;

    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPDSCP:
    {
      L7_uint32 ipDscp;

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_IPDSCP, 
                                    (void *)&ipDscp) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      /* shift IP DSCP value into proper position within TOS octet */
      ipDscp <<= DSSTDMIB_AUX_MF_CLFR_TOS_SHIFT_IPDSCP;
      pAuxMFClfrRow->mib.tos = (L7_uchar8)ipDscp;
      pAuxMFClfrRow->mib.tosMask = DSSTDMIB_AUX_MF_CLFR_TOS_MASK_IPDSCP;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_PROTOCOL:
    {
      L7_uint32 protocol;
      
      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_PROTOCOL_NUM, 
                                    (void *)&protocol) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      pAuxMFClfrRow->mib.protocol = protocol; 
    }
    break;
  
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIP:
    {
      L7_uint32 srcAddr, srcMask;

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_SRCIP_ADDR,
                                    (void *)&srcAddr) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_SRCIP_MASK,
                                    (void *)&srcMask) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      pAuxMFClfrRow->mib.srcAddr = srcAddr;
      pAuxMFClfrRow->mib.srcMask = srcMask;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCIPV6:
    {
      L7_in6_addr_t srcAddr;
      L7_uint32 srcPrefLength;

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_SRCIPV6_ADDR, 
                                    (void *)&srcAddr) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      
      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_SRCIPV6_PLEN, 
                                    (void *)&srcPrefLength) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      memcpy(&pAuxMFClfrRow->mib.ipv6SrcAddr, &srcAddr, sizeof(srcAddr));
      pAuxMFClfrRow->mib.ipv6SrcPlen = srcPrefLength;
    }
    break;


  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCL4PORT:
    {
      L7_uint32 srcL4PortMin, srcL4PortMax;

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_SRCL4PORT_START,
                                    (void *)&srcL4PortMin) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_SRCL4PORT_END,
                                    (void *)&srcL4PortMax) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      pAuxMFClfrRow->mib.srcL4PortMin = srcL4PortMin;
      pAuxMFClfrRow->mib.srcL4PortMax = srcL4PortMax;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS:
    {
      L7_uint32 cos;  
      
      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_COS,
                                    (void *)&cos) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      pAuxMFClfrRow->mib.cos = (L7_int32)cos;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_COS2:
    {
      L7_uint32 cos2;  
      
      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_COS2,
                                    (void *)&cos2) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      pAuxMFClfrRow->mib.cos2 = (L7_int32)cos2;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_DSTMAC:
    {
      L7_uchar8 dstMac[L7_MAC_ADDR_LEN], dstMacMask[L7_MAC_ADDR_LEN];
      
      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_DSTMAC_ADDR,
                                    (void *)dstMac) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_DSTMAC_MASK,
                                    (void *)dstMacMask) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      memcpy(pAuxMFClfrRow->mib.dstMac, dstMac, L7_MAC_ADDR_LEN);
      memcpy(pAuxMFClfrRow->mib.dstMacMask, dstMacMask, L7_MAC_ADDR_LEN);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_ETYPE:
    {
      L7_uint32  val1, val2;

      if(dsmibEtypeKeyidTranslate(classIndex, classRuleIndex, &val1,
                                  &val2) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      pAuxMFClfrRow->mib.etypeVal1 = val1;
      pAuxMFClfrRow->mib.etypeVal2 = val2;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_EVERY:
    /* Nothing to be done here for class rule type `every` as it is
     * only a match condition to allow (or not allow) all packets
     * to belong to a class and this value is already set to `TRUE`
     * when class rule `entry type` is set.
     */
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPPRECEDENCE:
    {
      L7_uint32 ipPrecedence;

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_IPPRECEDENCE, 
                                    (void *)&ipPrecedence) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* shift IP Precedence value into proper position within TOS octet */
      ipPrecedence <<= DSSTDMIB_AUX_MF_CLFR_TOS_SHIFT_IPPRECEDENCE;
      pAuxMFClfrRow->mib.tos = (L7_uchar8)ipPrecedence;
      pAuxMFClfrRow->mib.tosMask = DSSTDMIB_AUX_MF_CLFR_TOS_MASK_IPPRECEDENCE;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_IPTOS:
    {
      L7_uchar8  tos, tosMask;

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_IPTOS_BITS, 
                                    (void *)&tos) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_IPTOS_MASK, 
                                    (void *)&tosMask) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      pAuxMFClfrRow->mib.tos = tos;
      pAuxMFClfrRow->mib.tosMask = tosMask;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS:
    /* Nothing to be done here for rule type `refclass` as
     * the reference class object is set in the Class Rule Table.
     */
    break;
         
  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_SRCMAC:
    {
      L7_uchar8 srcMac[L7_MAC_ADDR_LEN], srcMacMask[L7_MAC_ADDR_LEN];
      

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_SRCMAC_ADDR,
                                    (void *)srcMac) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      
      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_SRCMAC_MASK,
                                    (void *)srcMacMask) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
      
      memcpy(pAuxMFClfrRow->mib.srcMac, srcMac, L7_MAC_ADDR_LEN);
      memcpy(pAuxMFClfrRow->mib.srcMacMask, srcMacMask, L7_MAC_ADDR_LEN);
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID:
    {
      L7_uint32  vlanIdMin, vlanIdMax;

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_VLANID_START,
                                    (void *)&vlanIdMin) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_VLANID_END,
                                    (void *)&vlanIdMax) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      pAuxMFClfrRow->mib.vlanIdMin = vlanIdMin;
      pAuxMFClfrRow->mib.vlanIdMax = vlanIdMax;
    }
    break;

  case L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_VLANID2:
    {
      L7_uint32  vlanId2Min, vlanId2Max;

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_VLANID2_START,
                                    (void *)&vlanId2Min) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      if(diffServClassRuleObjectGet(classIndex, classRuleIndex, 
                                    L7_DIFFSERV_CLASS_RULE_MATCH_VLANID2_END,
                                    (void *)&vlanId2Max) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      pAuxMFClfrRow->mib.vlanId2Min = vlanId2Min;
      pAuxMFClfrRow->mib.vlanId2Max = vlanId2Max;
    }
    break;

  default:
    return L7_FAILURE;
    /*PASSTHRU*/
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Set Auxiliary Multi Field Classifier Table objects to
*           their default values
*
* @param    pAuxMFClfrRow  @b{(input)} Pointer to the Aux MF Clfr Row
*
* @returns  void
*
* @notes 
*       
* @end
*********************************************************************/
void dsStdMibAuxMFClfSetToDefaults(dsStdMibAuxMFClfrEntryCtrl_t *pAuxMFClfrRow)
{ 
  /* clear out entire row structure (to set fields to 0 that are not 
   * explicitly updated below
   *
   * NOTE:  The intention is to set default clfr fields to "match everything"
   *
   * NOTE:  The stored classIndex, classRuleIndex fields get zeroed out here
   */
  memset(pAuxMFClfrRow, 0, sizeof(*pAuxMFClfrRow));
  
  pAuxMFClfrRow->tableId = DSSTDMIB_TABLE_ID_AUX_MF_CLFR;

  pAuxMFClfrRow->mib.dstMask = DSSTDMIB_DEFAULT_IP_MASK;
  pAuxMFClfrRow->mib.srcMask = DSSTDMIB_DEFAULT_IP_MASK;
  pAuxMFClfrRow->mib.protocol = DSSTDMIB_DEFAULT_PROTOCOL_NUM;
  pAuxMFClfrRow->mib.dstL4PortMin = DSSTDMIB_DEFAULT_PORT_MIN;
  pAuxMFClfrRow->mib.dstL4PortMax = DSSTDMIB_DEFAULT_PORT_MAX;
  pAuxMFClfrRow->mib.srcL4PortMin = DSSTDMIB_DEFAULT_PORT_MIN;
  pAuxMFClfrRow->mib.srcL4PortMax = DSSTDMIB_DEFAULT_PORT_MAX;
  pAuxMFClfrRow->mib.cos = DSSTDMIB_DEFAULT_COS;
  pAuxMFClfrRow->mib.cos2 = DSSTDMIB_DEFAULT_COS;
  pAuxMFClfrRow->mib.etypeVal1 = DSSTDMIB_DEFAULT_ETYPE_VAL;
  pAuxMFClfrRow->mib.etypeVal2 = DSSTDMIB_DEFAULT_ETYPE_VAL;
  pAuxMFClfrRow->mib.tosMask = DSSTDMIB_DEFAULT_TOS_MASK;
  memset(pAuxMFClfrRow->mib.dstMacMask, DSSTDMIB_DEFAULT_MAC_MASK, L7_MAC_ADDR_LEN);
  memset(pAuxMFClfrRow->mib.srcMacMask, DSSTDMIB_DEFAULT_MAC_MASK, L7_MAC_ADDR_LEN);
  pAuxMFClfrRow->mib.vlanIdMin = DSSTDMIB_DEFAULT_VID_MIN;
  pAuxMFClfrRow->mib.vlanIdMax = DSSTDMIB_DEFAULT_VID_MAX;
  pAuxMFClfrRow->mib.vlanId2Min = DSSTDMIB_DEFAULT_VID_MIN;
  pAuxMFClfrRow->mib.vlanId2Max = DSSTDMIB_DEFAULT_VID_MAX;
  pAuxMFClfrRow->mib.storage = DSSTDMIB_DEFAULT_STORAGE_TYPE;
  pAuxMFClfrRow->mib.status = L7_DIFFSERV_ROW_STATUS_ACTIVE; 
}

/*********************************************************************
  NOTE: AUXILIARY MULTI-FIELD CLASSIFIER TABLE EXTERNAL APIs
*********************************************************************/
 
/*************************************************************************
* @purpose  Obtain the current value of the Auxiliary Multifield 
*           Classifier index next variable
*
* @param    pAuxMFClfrIndex  @b{(output)} Pointer to Auxiliary Multifield
*                                         Classifier index value
*
* @returns  void
*
* @notes    Only outputs a value if the pAuxMFClfrIndex parm is non-null.
*
* @notes    A *pAuxMFClfrIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServAuxMFClfrIndexNext(L7_uint32 *pAuxMFClfrIndex)
{
  if (pAuxMFClfrIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  dsStdMibAuxMFClfrIdNext(pAuxMFClfrIndex);

  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
}

/*********************************************************************
* @purpose  Verify that an Auxiliary Multi Field Classifier Table row 
*           exists for the specified Id.
*
* @param    auxMFClfrId   @b{(input)} Auxiliary Multi field
*                                     Classifier Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServAuxMFClfrGet(L7_uint32 auxMFClfrId)
{
  dsStdMibAuxMFClfrEntryCtrl_t *pRow; 
  dsStdMibAuxMFClfrKey_t       keys;

  if(DSSTDMIB_AUX_MF_CLFR_INDEX_OUT_OF_RANGE(auxMFClfrId) == L7_TRUE)
    return L7_FAILURE;

  keys.auxMFClfrId = auxMFClfrId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_AUX_MF_CLFR],
                      &keys, AVL_EXACT);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Auxiliary Multi Field
*           Classifier Table
*
* @param    prevAuxMfClfrId  @b{(input)}  Auxiliary Multi field clfr
*                                         id to begin search
* @param    pAuxMfClfrId     @b{(output)} Pointer to next sequential
*                                         aux multi field clfr id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A pAuxMfClfrId value of 0 is used to find the first
*           auxiliary multi field classifier entry in the table.
*
* @end
*********************************************************************/
L7_RC_t diffServAuxMFClfrGetNext(L7_uint32 prevAuxMfClfrId,
                                 L7_uint32 *pAuxMfClfrId)
{
  dsStdMibAuxMFClfrEntryCtrl_t *pRow; 
  dsStdMibAuxMFClfrKey_t       keys;

  DIFFSERV_NULLPTR_CHECK(pAuxMfClfrId);

  keys.auxMFClfrId = prevAuxMfClfrId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_AUX_MF_CLFR],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
  {
    *pAuxMfClfrId = 0;
    return L7_FAILURE;
  }

  *pAuxMfClfrId = pRow->key.auxMFClfrId;
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get the value of an object in the Auxiliary Multi Field 
*           Classifier Table based on Object Id
*
* @param    auxMFClfrId  @b{(input)}  Auxiliary Multi field Classifier
*                                     Id
* @param    objectId     @b{(input)}  Object Id
* @param    pvalue       @b{(output)} Pointer to the value
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServAuxMFClfrObjectGet(L7_uint32 auxMFClfrId, 
                      L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_TABLE_OBJECT_t objectId,
                                   void *pValue)
{
  dsStdMibAuxMFClfrEntryCtrl_t *pRow; 
  dsStdMibAuxMFClfrKey_t       keys;

  if(DSSTDMIB_AUX_MF_CLFR_INDEX_OUT_OF_RANGE(auxMFClfrId) == L7_TRUE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pValue);

  keys.auxMFClfrId = auxMFClfrId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_AUX_MF_CLFR],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
  {
    return L7_ERROR;
  }
  
  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);
  
  switch(objectId)
  {
  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_DST_ADDR:
    *(L7_uint32 *)pValue = pRow->mib.dstAddr;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_DST_MASK:
    *(L7_uint32 *)pValue = pRow->mib.dstMask;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_SRC_ADDR:
    *(L7_uint32 *)pValue = pRow->mib.srcAddr;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_SRC_MASK:
    *(L7_uint32 *)pValue = pRow->mib.srcMask;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_PROTOCOL:
    *(L7_uint32 *)pValue = pRow->mib.protocol;
    break;
  
  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_DSTL4PORT_MIN:
    *(L7_uint32 *)pValue = pRow->mib.dstL4PortMin;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_DSTL4PORT_MAX:
    *(L7_uint32 *)pValue = pRow->mib.dstL4PortMax;
    break;
  
  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_SRCL4PORT_MIN:
    *(L7_uint32 *)pValue = pRow->mib.srcL4PortMin;
    break;
  
  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_SRCL4PORT_MAX:
    *(L7_uint32 *)pValue = pRow->mib.srcL4PortMax;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_COS:
    *(L7_int32 *)pValue = pRow->mib.cos;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_COS2:
    *(L7_int32 *)pValue = pRow->mib.cos2;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_ETYPE_VAL1:
    *(L7_uint32 *)pValue = pRow->mib.etypeVal1;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_ETYPE_VAL2:
    *(L7_uint32 *)pValue = pRow->mib.etypeVal2;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_TOS:
    *(L7_uchar8 *)pValue = pRow->mib.tos;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_TOS_MASK:
    *(L7_uchar8 *)pValue = pRow->mib.tosMask;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_DSTMAC:
    memcpy((L7_uchar8 *)pValue, pRow->mib.dstMac, L7_MAC_ADDR_LEN); 
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_DSTMAC_MASK:
    memcpy((L7_uchar8 *)pValue, pRow->mib.dstMacMask, L7_MAC_ADDR_LEN); 
    break;
  
  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_SRCMAC:
    memcpy((L7_uchar8 *)pValue, pRow->mib.srcMac, L7_MAC_ADDR_LEN); 
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_SRCMAC_MASK:
    memcpy((L7_uchar8 *)pValue, pRow->mib.srcMacMask, L7_MAC_ADDR_LEN); 
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_VLANID_MIN:
    *(L7_uint32 *)pValue = pRow->mib.vlanIdMin;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_VLANID_MAX:
    *(L7_uint32 *)pValue = pRow->mib.vlanIdMax;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_VLANID2_MIN:
    *(L7_uint32 *)pValue = pRow->mib.vlanId2Min;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_VLANID2_MAX:
    *(L7_uint32 *)pValue = pRow->mib.vlanId2Max;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_STORAGE:
    *(dsStdMibStorageType_t *)pValue = pRow->mib.storage;
    break;

  case L7_DIFFSERV_AUX_MULTI_FIELD_CLFR_ROW_STATUS:
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
