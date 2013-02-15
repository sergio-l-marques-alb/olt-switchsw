/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   diffserv_action_api.c
*
* @purpose    DiffServ Standard MIB Action, Mark COS, Mark DSCP,
*             Mark IP Precedence, and Count Action Table APIs
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
L7_uint32 actionIdNextFree;                     /* next available action id       */   
L7_uint32 countActIdNextFree;                   /* next available count act id    */   
L7_uint32 assignQueueIdNextFree;                /* next available assign queue id */   
L7_uint32 redirectIdNextFree;                   /* next available redirect id     */   
L7_uint32 mirrorIdNextFree;                     /* next available mirror id       */   

#define DSSTDMIB_ACTION_ID_INDICES              (((DSSTDMIB_ACTION_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)
#define DSSTDMIB_COUNT_ACT_ID_INDICES           (((DSSTDMIB_COUNT_ACT_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)
#define DSSTDMIB_ASSIGN_QUEUE_ID_INDICES        (((DSSTDMIB_ASSIGN_QUEUE_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)
#define DSSTDMIB_REDIRECT_ID_INDICES            (((DSSTDMIB_REDIRECT_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)
#define DSSTDMIB_MIRROR_ID_INDICES              (((DSSTDMIB_MIRROR_INDEX_MAX + 1) / L7_INTF_MASK_UNIT) + 1)

/* Mask storage definitions */
typedef struct
{
  L7_uchar8   value[DSSTDMIB_ACTION_ID_INDICES];
} DSSTDMIB_ACTION_ID_MASK_t;

typedef struct
{
  L7_uchar8   value[DSSTDMIB_COUNT_ACT_ID_INDICES];
} DSSTDMIB_COUNT_ACT_ID_MASK_t;

typedef struct
{
  L7_uchar8   value[DSSTDMIB_ASSIGN_QUEUE_ID_INDICES];
} DSSTDMIB_ASSIGN_QUEUE_ID_MASK_t;

typedef struct
{
  L7_uchar8   value[DSSTDMIB_REDIRECT_ID_INDICES];
} DSSTDMIB_REDIRECT_ID_MASK_t;

typedef struct
{
  L7_uchar8   value[DSSTDMIB_MIRROR_ID_INDICES];
} DSSTDMIB_MIRROR_ID_MASK_t;

/* NOTE: Index 0 is reserved and must not be used */
DSSTDMIB_ACTION_ID_MASK_t               actionIdInUseMask;
DSSTDMIB_COUNT_ACT_ID_MASK_t            countActIdInUseMask;
DSSTDMIB_ASSIGN_QUEUE_ID_MASK_t         assignQueueIdInUseMask;
DSSTDMIB_REDIRECT_ID_MASK_t             redirectIdInUseMask;
DSSTDMIB_MIRROR_ID_MASK_t               mirrorIdInUseMask;


/*
=======================
=======================
=======================

   ACTION TABLE APIs

=======================
=======================
=======================
*/

/*********************************************************************
* @purpose  Initialize draft MIB Action support
*
* @param    void
*
* @returns  void
*
* @notes
*       
* @end
*********************************************************************/
void dsStdMibActionInit(void)
{
  /* initialize index in-use bitmask */
  memset(&actionIdInUseMask, 0, sizeof(actionIdInUseMask));

  /* establish initial index next values */
  dsStdMibActionIdNextUpdate();
}

/*********************************************************************
* @purpose  Obtain the next available index value to be used when  
*           creating a new row in the Action Table
*
* @param    pActionId   @b{(output)}  Pointer to the action Id value
*
* @returns  void
*
* @notes    Only outputs a value if the pActionId parm is non-null.
*
* @notes    An output *pActionId value of 0 means the table is full.
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
void dsStdMibActionIdNext(L7_uint32 *pActionId)
{
  if (pActionId != L7_NULLPTR)
  {
    if (DSSTDMIB_ACTION_INDEX_OUT_OF_RANGE(actionIdNextFree) == L7_TRUE)
    {
      *pActionId = 0;
    }
    else
    {
      *pActionId = actionIdNextFree;
    }
  }
}

/*********************************************************************
* @purpose  Determine the next available Action Id value and update
*           its value
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
void dsStdMibActionIdNextUpdate(void)
{
  L7_uint32    i;

  for (i = 1; i <= DSSTDMIB_ACTION_INDEX_MAX; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(actionIdInUseMask, i) == L7_FALSE)
      break;
  }

  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > DSSTDMIB_ACTION_INDEX_MAX)
    i = 0;

  actionIdNextFree = i;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Action row element
*
* @param    actionId  @b{(input)} Action Id
*
* @returns  pRow        Pointer to row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsStdMibActionEntryCtrl_t *dsStdMibActionRowFind(L7_uint32 actionId)
{
  dsStdMibActionEntryCtrl_t *pRow;
  dsStdMibActionKey_t       keys;

  keys.actionId = actionId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ACTION], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Create a new row in the Action Table 
*
* @param    intIfNum   @b{(input)} Internal interface number
* @param    pSpecific  @b{(input)} Pointer to specific Data Path Element
* @param    pPathNext  @b{(inout)} Pointer to next data path element
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
L7_RC_t diffServActionCreate(L7_uint32        intIfNum, 
                             dsStdMibRowPtr_t *pSpecific, 
                             dsStdMibRowPtr_t *pPathNext)
{
  L7_RC_t                   rc = L7_FAILURE;
  dsStdMibTableId_t         tableId = DSSTDMIB_TABLE_ID_ACTION;
  dsStdMibActionEntryCtrl_t actionRow, *pActionRow = L7_NULLPTR;
  L7_uint32                 actionId = (L7_uint32)-1; /* init value for trace */
  L7_uint32                 ifIndex;

  DIFFSERV_NULLPTR_CHECK(pSpecific);
  DIFFSERV_NULLPTR_CHECK(pPathNext);

  do
  {
    /* get the next available action id */
    dsStdMibActionIdNext(&actionId);

    if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
    {
      DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                               actionId, 0, 0, 
                               (L7_uchar8)L7_TRUE);
    }

    /* action id 0 means the table is full */
    if (actionId == 0)
    {
      rc = L7_ERROR;
      break;
    }

    /* get ifIndex from intIfNum */
    if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
      break;

    pActionRow = &actionRow;
    memset(pActionRow, 0, sizeof(*pActionRow));

    pActionRow->key.actionId = actionId;

    pActionRow->tableId = tableId;

    pActionRow->mib.id = actionId;
    pActionRow->mib.interface = ifIndex;
    pActionRow->mib.next = *pPathNext;
    pActionRow->mib.specific = *pSpecific;
    pActionRow->mib.storage = DSSTDMIB_DEFAULT_STORAGE_TYPE;
    pActionRow->mib.status = L7_DIFFSERV_ROW_STATUS_ACTIVE;

    /* Insert Action Entry in AVL */
    if (dsmibRowInsert(&dsStdMibAvlTree[tableId], pActionRow) != L7_SUCCESS)
      break;

    /* successful row creation -- update next free index value*/
    L7_INTF_SETMASKBIT(actionIdInUseMask, actionId);
    dsStdMibActionIdNextUpdate();

    /* update data path next row info */
    DSSTDMIB_PATH_NEXT_UPDATE(pPathNext, tableId, actionId, 0, 0);

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            actionId, 0, 0,
                            (L7_uint32)pActionRow, (L7_uchar8)rc,
                            (L7_uchar8)L7_TRUE);
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete a row from the Action Table for the specified index
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
L7_RC_t diffServActionDelete(dsStdMibRowPtr_t *pPathNext)
{
  L7_RC_t                   rc = L7_FAILURE;
  dsStdMibTableId_t         tableId = DSSTDMIB_TABLE_ID_ACTION;
  dsStdMibActionEntryCtrl_t *pActionRow = L7_NULLPTR;
  L7_uint32                 actionId;


  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* make sure tableId is correct */
  if (pPathNext->tableId != tableId)
    return L7_FAILURE;

  actionId = pPathNext->rowIndex1;

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                             actionId, 0, 0);
  }

  do
  {
    if(DSSTDMIB_ACTION_INDEX_OUT_OF_RANGE(actionId) == L7_TRUE)
       break;

    pActionRow = dsStdMibActionRowFind(actionId);
    if (pActionRow == L7_NULLPTR)
    {
      *pPathNext = DSSTDMIB_ZERO_DOT_ZERO;
      rc = L7_SUCCESS;          /* consider this a success if row not found */
      break;
    }

    /* delete specific elem (XxxxMarkAct, CountAct, AssignQueue, Mirror, or Redirect)*/
    if (dsStdMibPrimitiveElemDelete(&pActionRow->mib.specific) != L7_SUCCESS)
      break;

    /* update data path next row info before deleting element */
    *pPathNext = pActionRow->mib.next;

    if (dsmibRowDelete(&dsStdMibAvlTree[tableId], pActionRow) != L7_SUCCESS)
      return L7_FAILURE;

    /* successful row deletion -- update next free index value*/
    L7_INTF_CLRMASKBIT(actionIdInUseMask, actionId);
    dsStdMibActionIdNextUpdate();

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            actionId, 0, 0,
                            (L7_uint32)pActionRow, (L7_uchar8)rc);
  }

  return rc;
}
  
/*********************************************************************
   NOTE: ACTION TABLE EXTERNAL APIs
*********************************************************************/

/*************************************************************************
* @purpose  Obtain the current value of the action index next variable
*
* @param    pActionIndex  @b{(output)} Pointer to action index value
*
* @returns  void
*
* @notes    Only outputs a value if the pActionIndex parm is non-null.
*
* @notes    A *pActionIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServActionIndexNext(L7_uint32 *pActionIndex)
{
  if (pActionIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  dsStdMibActionIdNext(pActionIndex);

  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
}

/*********************************************************************
* @purpose  Verify that an Action Table row exists for the specified
*           Action id
*
* @param    actionId    @b{(input)} Action Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServActionGet(L7_uint32 actionId)
{
  dsStdMibActionEntryCtrl_t *pRow; 
  dsStdMibActionKey_t        keys;

  if(DSSTDMIB_ACTION_INDEX_OUT_OF_RANGE(actionId) == L7_TRUE)
    return L7_FAILURE;

  keys.actionId = actionId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ACTION],
                      &keys, AVL_EXACT);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Action Table
*
* @param    prevActionId  @b{(input)}  Action id to begin search
* @param    pActionId     @b{(output)} Pointer to next sequential
*                                      Action id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevActionId value of 0 is used to find the first Action 
*           Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServActionGetNext(L7_uint32 prevActionId, L7_uint32 *pActionId)
{
  dsStdMibActionEntryCtrl_t *pRow; 
  dsStdMibActionKey_t        keys;

  DIFFSERV_NULLPTR_CHECK(pActionId);

  keys.actionId = prevActionId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ACTION],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
  {
    *pActionId = 0;
    return L7_FAILURE;
  }

  *pActionId = pRow->key.actionId;
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get the value of an object in the Action Table based on
*           Object Id
*
* @param    actionId    @b{(input)}  Action Id
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
L7_RC_t diffServActionObjectGet(L7_uint32 actionId, 
                            L7_DIFFSERV_ACTION_TABLE_OBJECT_t objectId,
                                void *pValue)
{
  dsStdMibActionEntryCtrl_t *pRow; 
  dsStdMibActionKey_t        keys;
  
  if(DSSTDMIB_ACTION_INDEX_OUT_OF_RANGE(actionId) == L7_TRUE)
    return L7_FAILURE;
  
  DIFFSERV_NULLPTR_CHECK(pValue);

  keys.actionId = actionId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ACTION],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
  {
    return L7_ERROR;
  }
    
  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  switch(objectId)
  {
  case L7_DIFFSERV_ACTION_INTERFACE:
    *(L7_uint32 *)pValue = pRow->mib.interface;
    break;

  case L7_DIFFSERV_ACTION_NEXT:
    *(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *)pValue = 
                              dsStdMibRowPtrXlateToExt(pRow->mib.next);
    break;

  case L7_DIFFSERV_ACTION_SPECIFIC:
    *(L7_USMDB_MIB_DIFFSERV_ROW_PTR_t *)pValue = 
                              dsStdMibRowPtrXlateToExt(pRow->mib.specific);
    break;

  case L7_DIFFSERV_ACTION_STORAGE:
    *(dsStdMibStorageType_t *)pValue = pRow->mib.storage;
    break;

  case L7_DIFFSERV_ACTION_ROW_STATUS:
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
================================
================================
================================

   COS MARK ACTION TABLE APIs

================================
================================
================================
*/

/*********************************************************************
 NOTE: COS MARK ACTION TABLE EXTERNAL APIs
**********************************************************************/

/*********************************************************************
* @purpose  Verify that a COS Mark Action Table row exists for the 
*           specified COS value
*
* @param    cos  @b{(input)} COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    must use this function to determine if cos 0 row exists 
*
* @end
*********************************************************************/
L7_RC_t diffServCosMarkActGet(L7_uint32 cos)
{
  if(DSSTDMIB_COS_MARK_ACT_INDEX_OUT_OF_RANGE(cos) == L7_TRUE)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine next sequential row in the COS Mark Action Table
*
* @param    prevCos  @b{(input)}  COS value to begin search
* @param    pCos     @b{(output)} Pointer to next sequential COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevCos of 0 yields first non-0 row in the table
*
* @end
*********************************************************************/
L7_RC_t diffServCosMarkActGetNext(L7_uint32 prevCos, L7_uint32 *pCos)
{
  DIFFSERV_NULLPTR_CHECK(pCos);

  if(DSSTDMIB_COS_MARK_ACT_INDEX_OUT_OF_RANGE(prevCos+1) == L7_TRUE)
  {
    *pCos = 0;
    return L7_FAILURE;
  }
    
  *pCos = prevCos+1; 
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get the COS value for the specified COS mark action table
*           row index
*
* @param    cos       @b{(input)}  Index COS value
* @param    objectId  @b{(input)}  Object Id
* @param    pCos      @b{(output)} Pointer to the output COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServCosMarkActObjectGet(L7_uint32 cos,
                       L7_DIFFSERV_COS_MARK_ACT_TABLE_OBJECT_t objectId,
                                    L7_uint32 *pCos)
{
  if(DSSTDMIB_COS_MARK_ACT_INDEX_OUT_OF_RANGE(cos) == L7_TRUE)
    return L7_FAILURE;

  if (objectId != L7_DIFFSERV_COS_MARK_ACT_COS)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pCos);

  *pCos = cos;

  return L7_SUCCESS;
}

/*
=================================================
=================================================
=================================================

   SECONDARY COS (COS2) MARK ACTION TABLE APIs

=================================================
=================================================
=================================================
*/

/*********************************************************************
 NOTE: SECONDARY COS (COS2) MARK ACTION TABLE EXTERNAL APIs
**********************************************************************/

/*********************************************************************
* @purpose  Verify that a Secondary COS Mark Action Table row exists 
*           for the specified COS value
*
* @param    cos         @b{(input)} Secondary COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    must use this function to determine if secondary cos 0 row exists 
*
* @end
*********************************************************************/
L7_RC_t diffServCos2MarkActGet(L7_uint32 cos)
{
  if(DSSTDMIB_COS2_MARK_ACT_INDEX_OUT_OF_RANGE(cos) == L7_TRUE)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Secondary COS Mark Action Table
*
* @param    prevCos     @b{(input)}  Secondary COS value to begin search
* @param    pCos        @b{(output)} Pointer to next sequential Secondary COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevCos of 0 yields first non-0 row in the table
*
* @end
*********************************************************************/
L7_RC_t diffServCos2MarkActGetNext(L7_uint32 prevCos, L7_uint32 *pCos)
{
  DIFFSERV_NULLPTR_CHECK(pCos);

  if(DSSTDMIB_COS2_MARK_ACT_INDEX_OUT_OF_RANGE(prevCos+1) == L7_TRUE)
  {
    *pCos = 0;
    return L7_FAILURE;
  }
    
  *pCos = prevCos+1; 
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get the COS value for the specified Secondary COS mark action
*           table row index
*
* @param    cos       @b{(input)}  Index Secondary COS value
* @param    objectId  @b{(input)}  Object Id
* @param    pCos      @b{(output)} Pointer to the output Secondary COS value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServCos2MarkActObjectGet(L7_uint32 cos,
                                     L7_DIFFSERV_COS2_MARK_ACT_TABLE_OBJECT_t objectId,
                                     L7_uint32 *pCos)
{
  if(DSSTDMIB_COS2_MARK_ACT_INDEX_OUT_OF_RANGE(cos) == L7_TRUE)
    return L7_FAILURE;

  if (objectId != L7_DIFFSERV_COS2_MARK_ACT_COS)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pCos);

  *pCos = cos;

  return L7_SUCCESS;
}

/*
=================================
=================================
=================================

   DSCP MARK ACTION TABLE APIs

=================================
=================================
=================================
*/

/*********************************************************************
 NOTE: DSCP MARK ACTION TABLE EXTERNAL APIs
*********************************************************************/
 
/*********************************************************************
* @purpose  Verify that a DSCP Mark Action Table row exists for the 
*           specified DSCP value
*
* @param    dscp @b{(input)} DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    must use this function to determine if dscp 0 row exists 
*
* @end
*********************************************************************/
L7_RC_t diffServDscpMarkActGet(L7_uint32 dscp)
{
  if(DSSTDMIB_DSCP_MARK_ACT_INDEX_OUT_OF_RANGE(dscp) == L7_TRUE)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine next sequential row in the DSCP Mark Action Table
*
* @param    prevDscp  @b{(input)}  DSCP value to begin search
* @param    pDscp     @b{(output)} Pointer to next sequential DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevDscp of 0 yields first non-0 row in the table
*
* @end
*********************************************************************/
L7_RC_t diffServDscpMarkActGetNext(L7_uint32 prevDscp, L7_uint32 *pDscp)
{
  DIFFSERV_NULLPTR_CHECK(pDscp);

  if(DSSTDMIB_DSCP_MARK_ACT_INDEX_OUT_OF_RANGE(prevDscp+1) == L7_TRUE)
  {
    *pDscp = 0;
    return L7_FAILURE;
  }
  
  *pDscp = prevDscp+1;
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get DSCP value for the specified DSCP Mark Action Table
*           row index
*
* @param    dscp         @b{(input)}  Index DSCP value 
* @param    objectId     @b{(input)}  Object Id
* @param    pDscp        @b{(output)} Pointer to the ouput DSCP value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServDscpMarkActObjectGet(L7_uint32 dscp,
                        L7_DIFFSERV_DSCP_MARK_ACT_TABLE_OBJECT_t objectId,
                                     L7_uint32 *pDscp)
{
  if(DSSTDMIB_DSCP_MARK_ACT_INDEX_OUT_OF_RANGE(dscp) == L7_TRUE)
    return L7_FAILURE;

  if (objectId != L7_DIFFSERV_DSCP_MARK_ACT_DSCP)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pDscp);

  *pDscp = dscp;
  return L7_SUCCESS;
}

/*
==========================================
==========================================
==========================================

   IP PRECEDENCE MARK ACTION TABLE APIs

==========================================
==========================================
==========================================
*/

/*********************************************************************
* @purpose  Verify that an IP Precedence Mark Action Table row exists
*           for the specified IP Precedence value
*
* @param    ipPrecedence @b{(input)} IP Precedence value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    must use this function to determine if ipPrecedence 0 row
*           exists 
*
* @end
*********************************************************************/
L7_RC_t diffServIpPrecMarkActGet(L7_uint32 ipPrecedence)
{
  if(DSSTDMIB_IPPREC_MARK_ACT_INDEX_OUT_OF_RANGE(ipPrecedence) == L7_TRUE)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine next sequential row in the IP Precedence Mark
*           Action Table
*
* @param    prevIpPrecedence  @b{(input)}  Ip Precedence value to 
*                                          begin search
* @param    pIpPrecedence     @b{(output)} Pointer to next sequential
*                                          IP Precedence value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevIpPrecedence of 0 yields first non-0 row in the table
*
* @end
*********************************************************************/
L7_RC_t diffServIpPrecMarkActGetNext(L7_uint32 prevIpPrecedence,
                                     L7_uint32 *pIpPrecedence)
{
  DIFFSERV_NULLPTR_CHECK(pIpPrecedence);

  if(DSSTDMIB_IPPREC_MARK_ACT_INDEX_OUT_OF_RANGE(prevIpPrecedence+1) == L7_TRUE)
  {
    *pIpPrecedence = 0;
    return L7_FAILURE;
  }
  
  *pIpPrecedence = prevIpPrecedence+1; 
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get IP Precedence value for the specified IP Precedence 
*           Mark Action Table row index
*
* @param    ipPrecedence   @b{(input)}  Index Ip Precedence value 
* @param    objectId       @b{(input)}  Object Id
* @param    pIpPrecedence  @b{(output)} Pointer to the output IP 
*                                       Precedence value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServIpPrecMarkActObjectGet(L7_uint32 ipPrecedence,
                      L7_DIFFSERV_IPPREC_MARK_ACT_TABLE_OBJECT_t objectId,
                                       L7_uint32 *pIpPrecedence)
{
  if(DSSTDMIB_IPPREC_MARK_ACT_INDEX_OUT_OF_RANGE(ipPrecedence) == L7_TRUE)
    return L7_FAILURE;

  if (objectId != L7_DIFFSERV_IPPREC_MARK_ACT_IPPRECEDENCE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pIpPrecedence);

  *pIpPrecedence = ipPrecedence; 

  return L7_SUCCESS;
}

/*
=============================
=============================
=============================

   COUNT ACTION TABLE APIs

=============================
=============================
=============================
*/

/*********************************************************************
* @purpose  Initialize draft MIB Count Action support
*
* @param    void
*
* @returns  void
*
* @notes
*       
* @end
*********************************************************************/
void dsStdMibCountActInit(void)
{
  /* initialize index in-use bitmask */
  memset(&countActIdInUseMask, 0, sizeof(countActIdInUseMask));

  /* establish initial index next values */
  dsStdMibCountActIdNextUpdate();
}

/*********************************************************************
* @purpose  Obtain the next available index value to be used when
*           creating a new row in the Count Action Table
*
* @param    pCountActId   @b{(output)} Pointer to the Count action
*                                      Id value
*
* @returns  void
*
* @notes    Only outputs a value if the pCountActId parm is non-null.
*
* @notes    An output *pCountActId value of 0 means the table is full.
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
void dsStdMibCountActIdNext(L7_uint32 *pCountActId)
{
  if (pCountActId != L7_NULLPTR)
  {
    if (DSSTDMIB_COUNT_ACT_INDEX_OUT_OF_RANGE(countActIdNextFree) == L7_TRUE)
    {
      *pCountActId = 0;
    }
    else
    {
      *pCountActId = countActIdNextFree;
    }
  }
}

/*********************************************************************
* @purpose  Determine next available Count Action Id value and update
*           its value
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
void dsStdMibCountActIdNextUpdate(void)
{
  L7_uint32    i;

  for (i = 1; i <= DSSTDMIB_COUNT_ACT_INDEX_MAX; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(countActIdInUseMask, i) == L7_FALSE)
      break;
  }

  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > DSSTDMIB_COUNT_ACT_INDEX_MAX)
    i = 0;

  countActIdNextFree = i;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Count Action row element
*
* @param    countActId  @b{(input)} Count Action Id
*
* @returns  pRow        Pointer to row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsStdMibCountActEntryCtrl_t *dsStdMibCountActRowFind(L7_uint32 countActId)
{
  dsStdMibCountActEntryCtrl_t *pRow;
  dsStdMibCountActKey_t       keys;

  keys.countActId = countActId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_COUNT_ACT], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Create a new row in the Count Action Table 
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    countType        @b{(input)} Count Type(sent discarded etc)
* @param    pCountActId      @b{(output)}Pointer to CountAction Id value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    No more index values available (table full)
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServCountActCreate(L7_uint32 policyIndex, 
                               L7_uint32 policyInstIndex, 
                               L7_uint32 intIfNum,
                               DSSTDMIB_COUNT_TYPE_t countType,
                               L7_uint32 *pCountActId)
{
  L7_RC_t                     rc = L7_FAILURE;
  dsStdMibTableId_t           tableId = DSSTDMIB_TABLE_ID_COUNT_ACT;
  dsStdMibCountActEntryCtrl_t countActRow, *pCountActRow = L7_NULLPTR;
  L7_uint32                   countActId;
  L7_ulong64                  octets = {0,0};
  L7_ulong64                  pkts = {0,0};
  fpPerfObjectGet_t           fpPerfObjectGet;
  L7_uint32                   octetsCtrId, pktsCtrId;

  DIFFSERV_NULLPTR_CHECK(pCountActId);

  do
  {
    /* get the next available count action id */
    dsStdMibCountActIdNext(pCountActId);

    /* set up a local working copy of the id */
    countActId = *pCountActId;

    if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
    {
      DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                               countActId, 0, 0, 
                               (L7_uchar8)L7_TRUE);
    }

    /* count action id 0 means the table is full */
    if (countActId == 0)
    {
      rc = L7_ERROR;
      break;
    }

    /* set up pointer to relevant policy perf table function per direction
     * NOTE: init to outbound function ptr, but override for inbound counter
     */
    fpPerfObjectGet = diffServPolicyPerfOutObjectGet;

    switch(countType)
    {
    /* sent traffic */
    case DSSTDMIB_COUNT_TYPE_IN_OFFERED:
      octetsCtrId = L7_DIFFSERV_POLICY_PERF_IN_HC_OFFERED_OCTETS;
      pktsCtrId = L7_DIFFSERV_POLICY_PERF_IN_HC_OFFERED_PACKETS;
      fpPerfObjectGet = (fpPerfObjectGet_t)diffServPolicyPerfInObjectGet;
      break;

    case DSSTDMIB_COUNT_TYPE_OUT_OFFERED:
      octetsCtrId = L7_DIFFSERV_POLICY_PERF_OUT_HC_OFFERED_OCTETS;
      pktsCtrId = L7_DIFFSERV_POLICY_PERF_OUT_HC_OFFERED_PACKETS;
      fpPerfObjectGet = (fpPerfObjectGet_t)diffServPolicyPerfOutObjectGet;
      break;

    default:
      return L7_FAILURE;
      /*PASSTHRU*/
    } /* endswitch */

    /* NOTE:  Do not read the counter values from the private MIB during
     *        row creation.  It is quite possible for the standard MIB 
     *        data path to be set up before the low-level code, so the 
     *        counter source might not exist yet.  Wait until the 
     *        'objectGet' call to retrieve the value.
     */

    pCountActRow = &countActRow;
    memset(pCountActRow, 0, sizeof(*pCountActRow));

    pCountActRow->key.countActId = countActId;

    pCountActRow->tableId = tableId;

    pCountActRow->ctrSource.countType = countType;
    pCountActRow->ctrSource.fpPerfObjectGet = fpPerfObjectGet;
    pCountActRow->ctrSource.policyIndex = policyIndex;
    pCountActRow->ctrSource.policyInstIndex = policyInstIndex;
    pCountActRow->ctrSource.intIfNum = intIfNum;
    pCountActRow->ctrSource.octetsCtrId = octetsCtrId;
    pCountActRow->ctrSource.pktsCtrId = pktsCtrId;

    pCountActRow->mib.id = countActId;
    pCountActRow->mib.octets = octets;
    pCountActRow->mib.pkts = pkts;
    pCountActRow->mib.storage = DSSTDMIB_DEFAULT_STORAGE_TYPE; 
    pCountActRow->mib.status = L7_DIFFSERV_ROW_STATUS_ACTIVE;

    /* Insert Count Action Entry in AVL */
    if (dsmibRowInsert(&dsStdMibAvlTree[tableId],
                       (void *)pCountActRow) != L7_SUCCESS)
      break;

    /* successful row creation -- update next free index value*/
    L7_INTF_SETMASKBIT(countActIdInUseMask, countActId);
    dsStdMibCountActIdNextUpdate();

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            countActId, 0, 0,
                            (L7_uint32)pCountActRow, (L7_uchar8)rc,
                            (L7_uchar8)L7_TRUE);
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete a row from the Count Act Table for the specified
*           index
*
* @param    countActId   @b{(input)} Count Action Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServCountActDelete(L7_uint32 countActId)
{
  L7_RC_t                     rc = L7_FAILURE;
  dsStdMibTableId_t           tableId = DSSTDMIB_TABLE_ID_COUNT_ACT;
  dsStdMibCountActEntryCtrl_t *pRow = L7_NULLPTR;

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                             countActId, 0, 0);
  }

  do
  {
    if(DSSTDMIB_COUNT_ACT_INDEX_OUT_OF_RANGE(countActId) == L7_TRUE)
       break;

    pRow = dsStdMibCountActRowFind(countActId);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_SUCCESS;          /* consider this a success if row not found */
      break;
    }

    if (dsmibRowDelete(&dsStdMibAvlTree[tableId], pRow) != L7_SUCCESS)
      break;

    /* successful row deletion -- update next free index value*/
    L7_INTF_CLRMASKBIT(countActIdInUseMask, countActId);
    dsStdMibCountActIdNextUpdate();

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            countActId, 0, 0,                                                
                            (L7_uint32)pRow, (L7_uchar8)rc);
  }

  return rc;
}
                                                                            
/*********************************************************************
 NOTE: COUNT ACTION TABLE EXTERNAL APIs
*********************************************************************/
 
/*************************************************************************
* @purpose  Obtain the current value of count action index next variable
*
* @param    pCountActIndex  @b{(output)} Pointer to count action index value
*
* @returns  void
*
* @notes    Only outputs a value if the pCountActIndex parm is non-null.
*
* @notes    A *pCountActIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServCountActIndexNext(L7_uint32 *pCountActIndex)
{
  if (pCountActIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  dsStdMibCountActIdNext(pCountActIndex);

  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
}

/*********************************************************************
* @purpose  Verify that a Count Action Table row exists for the 
*           specified Count Action Id
*
* @param    countActId  @b{(input)} Count Action Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServCountActGet(L7_uint32 countActId)
{
  dsStdMibCountActEntryCtrl_t *pRow; 
  dsStdMibCountActKey_t       keys;
                           
  if(DSSTDMIB_COUNT_ACT_INDEX_OUT_OF_RANGE(countActId) == L7_TRUE)
    return L7_FAILURE;

  keys.countActId = countActId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_COUNT_ACT],
                      &keys, AVL_EXACT);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Count Action Table
*
* @param    prevCountActId  @b{(input)}  Count Action Id to begin
*                                        search
* @param    pCountActId     @b{(output)} Pointer to next sequential
*                                        Count Action Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevCountActId value of 0 is used to find the first
*           Count Action Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServCountActGetNext(L7_uint32 prevCountActId,
                                L7_uint32 *pCountActId)
{
  dsStdMibCountActEntryCtrl_t *pRow; 
  dsStdMibCountActKey_t       keys;
                           
  DIFFSERV_NULLPTR_CHECK(pCountActId);

  keys.countActId = prevCountActId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_COUNT_ACT],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
  {
    *pCountActId = 0;
    return L7_FAILURE;
  }

  *pCountActId = pRow->key.countActId;
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get the value of an object in the Count Action Table based
*           on Object Id
*
* @param    countActId  @b{(input)}  Count Action Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to counter value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServCountActObjectGet(L7_uint32 countActId,
                        L7_DIFFSERV_COUNT_ACT_TABLE_OBJECT_t objectId,
                                  void *pValue)
{
  L7_RC_t                     rc = L7_SUCCESS;
  dsStdMibCountActEntryCtrl_t *pRow; 
  dsStdMibCountActKey_t       keys;
  L7_ulong64                  octetsCtr, pktsCtr;
  
  if(DSSTDMIB_COUNT_ACT_INDEX_OUT_OF_RANGE(countActId) == L7_TRUE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pValue);

  keys.countActId = countActId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_COUNT_ACT],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
    return L7_ERROR;
  
  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  switch(objectId)
  {
  case L7_DIFFSERV_COUNT_ACT_OCTETS:
  case L7_DIFFSERV_COUNT_ACT_PKTS:
    octetsCtr = pRow->mib.octets;
    pktsCtr = pRow->mib.pkts;
    if (pRow->ctrSource.octetsCtrId != DSSTDMIB_PRVTMIB_CTR_ID_NONE)
    {
      /* refresh by reading latest counter value from private MIB */
      if ((*(pRow->ctrSource.fpPerfObjectGet))(pRow->ctrSource.policyIndex, 
              pRow->ctrSource.policyInstIndex, pRow->ctrSource.intIfNum,
              pRow->ctrSource.octetsCtrId, (void *)&octetsCtr) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
        break;
      }
    }
    if (pRow->ctrSource.pktsCtrId != DSSTDMIB_PRVTMIB_CTR_ID_NONE)
    {
      if ((*(pRow->ctrSource.fpPerfObjectGet))(pRow->ctrSource.policyIndex, 
              pRow->ctrSource.policyInstIndex, pRow->ctrSource.intIfNum,
              pRow->ctrSource.pktsCtrId, (void *)&pktsCtr) != L7_SUCCESS)
      {
        rc = L7_FAILURE;
        break;
      }
    }
    pRow->mib.octets = octetsCtr;
    pRow->mib.pkts = pktsCtr;
    if (objectId == L7_DIFFSERV_COUNT_ACT_OCTETS)
      *((L7_ulong64 *)pValue) = pRow->mib.octets;
    else
      *((L7_ulong64 *)pValue) = pRow->mib.pkts;
    break;

  case L7_DIFFSERV_COUNT_ACT_STORAGE:
    *(dsStdMibStorageType_t *)pValue = pRow->mib.storage;
    break;

  case L7_DIFFSERV_COUNT_ACT_ROW_STATUS:
    *(dsStdMibRowStatus_t *)pValue = pRow->mib.status;
    break;
  
  default:
    rc = L7_FAILURE;
    break;

  } /* endswitch */
  
  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
  return rc; 
}


/*
=============================
=============================
=============================

   ASSIGN QUEUE TABLE APIs

=============================
=============================
=============================
*/

/*********************************************************************
* @purpose  Initialize draft MIB Assign Queue support
*
* @param    void
*
* @returns  void
*
* @notes
*       
* @end
*********************************************************************/
void dsStdMibAssignQueueInit(void)
{
  /* initialize index in-use bitmask */
  memset(&assignQueueIdInUseMask, 0, sizeof(assignQueueIdInUseMask));

  /* establish initial index next values */
  dsStdMibAssignQueueIdNextUpdate();
}

/*********************************************************************
* @purpose  Obtain the next available Id value to be used when
*           creating a new row in the Assign Queue Table
*
* @param    pAssignId   @b{(output)} Pointer to the Assign Queue
*                                      Id value
*
* @returns  void
*
* @notes    Only outputs a value if the pAssignId parm is non-null.
*
* @notes    An output *pAssignId value of 0 means the table is full.
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
void dsStdMibAssignQueueIdNext(L7_uint32 *pAssignId)
{
  if (pAssignId != L7_NULLPTR)
  {
    if (DSSTDMIB_ASSIGN_QUEUE_INDEX_OUT_OF_RANGE(assignQueueIdNextFree) == L7_TRUE)
    {
      *pAssignId = 0;
    }
    else
    {
      *pAssignId = assignQueueIdNextFree;
    }
  }
}

/*********************************************************************
* @purpose  Determine next available Assign Queue Id value and update
*           its value
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
void dsStdMibAssignQueueIdNextUpdate(void)
{
  L7_uint32    i;

  for (i = 1; i <= DSSTDMIB_ASSIGN_QUEUE_INDEX_MAX; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(assignQueueIdInUseMask, i) == L7_FALSE)
      break;
  }

  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > DSSTDMIB_ASSIGN_QUEUE_INDEX_MAX)
    i = 0;

  assignQueueIdNextFree = i;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Assign Queue row element
*
* @param    assignId    @b{(input)} Assign Queue Id
*
* @returns  pRow        Pointer to row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsStdMibAssignQueueEntryCtrl_t *dsStdMibAssignQueueRowFind(L7_uint32 assignId)
{
  dsStdMibAssignQueueEntryCtrl_t  *pRow;
  dsStdMibAssignQueueKey_t        keys;

  keys.assignId = assignId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ASSIGN_QUEUE], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Create a new row in the Assign Queue Table 
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrIndex  @b{(input)} Policy Attr Index
* @param    pAssignId        @b{(output)}Pointer to Assign Queue Id value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    No more index values available (table full)
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServAssignQueueCreate(L7_uint32 policyIndex, 
                                  L7_uint32 policyInstIndex, 
                                  L7_uint32 policyAttrIndex, 
                                  L7_uint32 *pAssignId)
{
  L7_RC_t                         rc = L7_FAILURE;
  dsStdMibTableId_t               tableId = DSSTDMIB_TABLE_ID_ASSIGN_QUEUE;
  dsStdMibAssignQueueEntryCtrl_t  assignRow, *pAssignRow = L7_NULLPTR;
  L7_uint32                       assignId;
  L7_uint32                       qid;

  DIFFSERV_NULLPTR_CHECK(pAssignId);

  do
  {
    /* get the next available assign queue id */
    dsStdMibAssignQueueIdNext(pAssignId);

    /* set up a local working copy of the id */
    assignId = *pAssignId;

    if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
    {
      DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                               assignId, 0, 0, 
                               (L7_uchar8)L7_TRUE);
    }

    /* assign queue id 0 means the table is full */
    if (assignId == 0)
    {
      rc = L7_ERROR;
      break;
    }

    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                    L7_DIFFSERV_POLICY_ATTR_STMT_ASSIGN_QUEUE_QID,
                                    &qid) != L7_SUCCESS)
      break;

    pAssignRow = &assignRow;
    memset(pAssignRow, 0, sizeof(*pAssignRow));

    pAssignRow->key.assignId = assignId;

    pAssignRow->tableId = tableId;

    pAssignRow->mib.id = assignId;
    pAssignRow->mib.qNum = qid;
    pAssignRow->mib.storage = DSSTDMIB_DEFAULT_STORAGE_TYPE; 
    pAssignRow->mib.status = L7_DIFFSERV_ROW_STATUS_ACTIVE;

    /* Insert Assign Queue Entry in AVL */
    if (dsmibRowInsert(&dsStdMibAvlTree[tableId],
                       (void *)pAssignRow) != L7_SUCCESS)
      break;

    /* successful row creation -- update next free index value*/
    L7_INTF_SETMASKBIT(assignQueueIdInUseMask, assignId);
    dsStdMibAssignQueueIdNextUpdate();

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            assignId, 0, 0,
                            (L7_uint32)pAssignRow, (L7_uchar8)rc,
                            (L7_uchar8)L7_TRUE);
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete a row from the Assign Queue Table for the specified
*           index
*
* @param    assignId    @b{(input)} Assign Queue Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServAssignQueueDelete(L7_uint32 assignId)
{
  L7_RC_t                         rc = L7_FAILURE;
  dsStdMibTableId_t               tableId = DSSTDMIB_TABLE_ID_ASSIGN_QUEUE;
  dsStdMibAssignQueueEntryCtrl_t  *pRow = L7_NULLPTR;

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                             assignId, 0, 0);
  }

  do
  {
    if(DSSTDMIB_ASSIGN_QUEUE_INDEX_OUT_OF_RANGE(assignId) == L7_TRUE)
       break;

    pRow = dsStdMibAssignQueueRowFind(assignId);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_SUCCESS;          /* consider this a success if row not found */
      break;
    }

    if (dsmibRowDelete(&dsStdMibAvlTree[tableId], pRow) != L7_SUCCESS)
      break;

    /* successful row deletion -- update next free index value*/
    L7_INTF_CLRMASKBIT(assignQueueIdInUseMask, assignId);
    dsStdMibAssignQueueIdNextUpdate();

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            assignId, 0, 0,                                                
                            (L7_uint32)pRow, (L7_uchar8)rc);
  }

  return rc;
}
                                                                            
/*********************************************************************
 NOTE: ASSIGN QUEUE TABLE EXTERNAL APIs
*********************************************************************/
 
/*************************************************************************
* @purpose  Obtain the current value of assign queue index next variable
*
* @param    pAssignIndex  @b{(output)} Pointer to assign queue index value
*
* @returns  void
*
* @notes    Only outputs a value if the pAssignIndex parm is non-null.
*
* @notes    A *pAssignIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServAssignQueueIndexNext(L7_uint32 *pAssignIndex)
{
  if (pAssignIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  dsStdMibAssignQueueIdNext(pAssignIndex);

  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
}

/*********************************************************************
* @purpose  Verify that an Assign Queue Table row exists for the 
*           specified Assign Queue Id
*
* @param    assignId  @b{(input)} Assign Queue Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServAssignQueueGet(L7_uint32 assignId)
{
  dsStdMibAssignQueueEntryCtrl_t  *pRow; 
  dsStdMibAssignQueueKey_t        keys;
                           
  if(DSSTDMIB_ASSIGN_QUEUE_INDEX_OUT_OF_RANGE(assignId) == L7_TRUE)
    return L7_FAILURE;

  keys.assignId = assignId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ASSIGN_QUEUE],
                      &keys, AVL_EXACT);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Assign Queue Table
*
* @param    prevAssignId  @b{(input)}  Assign Queue Id to begin
*                                        search
* @param    pAssignId     @b{(output)} Pointer to next sequential
*                                        Assign Queue Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevAssignId value of 0 is used to find the first
*           Assign Queue Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServAssignQueueGetNext(L7_uint32 prevAssignId,
                                   L7_uint32 *pAssignId)
{
  dsStdMibAssignQueueEntryCtrl_t  *pRow; 
  dsStdMibAssignQueueKey_t        keys;
                           
  DIFFSERV_NULLPTR_CHECK(pAssignId);

  keys.assignId = prevAssignId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ASSIGN_QUEUE],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
  {
    *pAssignId = 0;
    return L7_FAILURE;
  }

  *pAssignId = pRow->key.assignId;
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get the value of an object in the Assign Queue Table based
*           on Object Id
*
* @param    assignId    @b{(input)}  Assign Queue Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to object value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServAssignQueueObjectGet(L7_uint32 assignId,
                        L7_DIFFSERV_ASSIGN_QUEUE_TABLE_OBJECT_t objectId,
                                     void *pValue)
{
  L7_RC_t                         rc = L7_SUCCESS;
  dsStdMibAssignQueueEntryCtrl_t  *pRow; 
  dsStdMibAssignQueueKey_t        keys;
  
  if(DSSTDMIB_ASSIGN_QUEUE_INDEX_OUT_OF_RANGE(assignId) == L7_TRUE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pValue);

  keys.assignId = assignId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ASSIGN_QUEUE],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
    return L7_ERROR;
  
  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  switch(objectId)
  {
  case L7_DIFFSERV_ASSIGN_QUEUE_QNUM:
    *(L7_uint32 *)pValue = pRow->mib.qNum;
    break;

  case L7_DIFFSERV_ASSIGN_QUEUE_STORAGE:
    *(dsStdMibStorageType_t *)pValue = pRow->mib.storage;
    break;

  case L7_DIFFSERV_ASSIGN_QUEUE_ROW_STATUS:
    *(dsStdMibRowStatus_t *)pValue = pRow->mib.status;
    break;
  
  default:
    rc = L7_FAILURE;
    break;

  } /* endswitch */
  
  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
  return rc; 
}


/*
=============================
=============================
=============================

     REDIRECT TABLE APIs

=============================
=============================
=============================
*/

/*********************************************************************
* @purpose  Initialize draft MIB Redirect support
*
* @param    void
*
* @returns  void
*
* @notes
*       
* @end
*********************************************************************/
void dsStdMibRedirectInit(void)
{
  /* initialize index in-use bitmask */
  memset(&redirectIdInUseMask, 0, sizeof(redirectIdInUseMask));

  /* establish initial index next values */
  dsStdMibRedirectIdNextUpdate();
}

/*********************************************************************
* @purpose  Obtain the next available Id value to be used when
*           creating a new row in the Redirect Table
*
* @param    pRedirectId @b{(output)} Pointer to the Redirect Id value
*
* @returns  void
*
* @notes    Only outputs a value if the pRedirectId parm is non-null.
*
* @notes    An output *pRedirectId value of 0 means the table is full.
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
void dsStdMibRedirectIdNext(L7_uint32 *pRedirectId)
{
  if (pRedirectId != L7_NULLPTR)
  {
    if (DSSTDMIB_REDIRECT_INDEX_OUT_OF_RANGE(redirectIdNextFree) == L7_TRUE)
    {
      *pRedirectId = 0;
    }
    else
    {
      *pRedirectId = redirectIdNextFree;
    }
  }
}

/*********************************************************************
* @purpose  Determine next available Redirect Id value and update
*           its value
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
void dsStdMibRedirectIdNextUpdate(void)
{
  L7_uint32    i;

  for (i = 1; i <= DSSTDMIB_REDIRECT_INDEX_MAX; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(redirectIdInUseMask, i) == L7_FALSE)
      break;
  }

  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > DSSTDMIB_REDIRECT_INDEX_MAX)
    i = 0;

  redirectIdNextFree = i;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Redirect row element
*
* @param    redirectId  @b{(input)} Redirect Id
*
* @returns  pRow        Pointer to row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsStdMibRedirectEntryCtrl_t *dsStdMibRedirectRowFind(L7_uint32 redirectId)
{
  dsStdMibRedirectEntryCtrl_t *pRow;
  dsStdMibRedirectKey_t       keys;

  keys.redirectId = redirectId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_REDIRECT], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Create a new row in the Redirect Table 
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrIndex  @b{(input)} Policy Attr Index
* @param    pRedirectId      @b{(output)}Pointer to Redirect Id value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    No more index values available (table full)
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServRedirectCreate(L7_uint32 policyIndex, 
                               L7_uint32 policyInstIndex, 
                               L7_uint32 policyAttrIndex, 
                               L7_uint32 *pRedirectId)
{
  L7_RC_t                         rc = L7_FAILURE;
  dsStdMibTableId_t               tableId = DSSTDMIB_TABLE_ID_REDIRECT;
  dsStdMibRedirectEntryCtrl_t     redirectRow, *pRedirectRow = L7_NULLPTR;
  L7_uint32                       redirectId;
  L7_uint32                       intIfNum;
  nimConfigID_t                   configId;

  DIFFSERV_NULLPTR_CHECK(pRedirectId);

  do
  {
    /* get the next available assign queue id */
    dsStdMibRedirectIdNext(pRedirectId);

    /* set up a local working copy of the id */
    redirectId = *pRedirectId;

    if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
    {
      DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                               redirectId, 0, 0, 
                               (L7_uchar8)L7_TRUE);
    }

    /* redirect id 0 means the table is full */
    if (redirectId == 0)
    {
      rc = L7_ERROR;
      break;
    }

    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                    L7_DIFFSERV_POLICY_ATTR_STMT_REDIRECT_INTF,
                                    &configId) != L7_SUCCESS)
      break;

    if (nimIntIfFromConfigIDGet(&configId, &intIfNum) != L7_SUCCESS)
      break;

    pRedirectRow = &redirectRow;
    memset(pRedirectRow, 0, sizeof(*pRedirectRow));

    pRedirectRow->key.redirectId = redirectId;

    pRedirectRow->tableId = tableId;

    pRedirectRow->mib.id = redirectId;
    pRedirectRow->mib.intIfNum = intIfNum;
    pRedirectRow->mib.storage = DSSTDMIB_DEFAULT_STORAGE_TYPE; 
    pRedirectRow->mib.status = L7_DIFFSERV_ROW_STATUS_ACTIVE;

    /* Insert Redirect Entry in AVL */
    if (dsmibRowInsert(&dsStdMibAvlTree[tableId],
                       (void *)pRedirectRow) != L7_SUCCESS)
      break;

    /* successful row creation -- update next free index value*/
    L7_INTF_SETMASKBIT(redirectIdInUseMask, redirectId);
    dsStdMibRedirectIdNextUpdate();

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            redirectId, 0, 0,
                            (L7_uint32)pRedirectRow, (L7_uchar8)rc,
                            (L7_uchar8)L7_TRUE);
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete a row from the Redirect Table for the specified
*           index
*
* @param    redirectId    @b{(input)} Redirect Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServRedirectDelete(L7_uint32 redirectId)
{
  L7_RC_t                         rc = L7_FAILURE;
  dsStdMibTableId_t               tableId = DSSTDMIB_TABLE_ID_REDIRECT;
  dsStdMibRedirectEntryCtrl_t     *pRow = L7_NULLPTR;

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                             redirectId, 0, 0);
  }

  do
  {
    if(DSSTDMIB_REDIRECT_INDEX_OUT_OF_RANGE(redirectId) == L7_TRUE)
       break;

    pRow = dsStdMibRedirectRowFind(redirectId);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_SUCCESS;          /* consider this a success if row not found */
      break;
    }

    if (dsmibRowDelete(&dsStdMibAvlTree[tableId], pRow) != L7_SUCCESS)
      break;

    /* successful row deletion -- update next free index value*/
    L7_INTF_CLRMASKBIT(redirectIdInUseMask, redirectId);
    dsStdMibRedirectIdNextUpdate();

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            redirectId, 0, 0,                                                
                            (L7_uint32)pRow, (L7_uchar8)rc);
  }

  return rc;
}
                                                                            
/*********************************************************************
 NOTE: REDIRECT TABLE EXTERNAL APIs
*********************************************************************/
 
/*************************************************************************
* @purpose  Obtain the current value of redirect index next variable
*
* @param    pRedirectIndex  @b{(output)} Pointer to redirect index value
*
* @returns  void
*
* @notes    Only outputs a value if the pRedirectIndex parm is non-null.
*
* @notes    A *pRedirectIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServRedirectIndexNext(L7_uint32 *pRedirectIndex)
{
  if (pRedirectIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  dsStdMibRedirectIdNext(pRedirectIndex);

  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
}

/*********************************************************************
* @purpose  Verify that a Redirect Table row exists for the 
*           specified Redirect Id
*
* @param    redirectId  @b{(input)} Redirect Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServRedirectGet(L7_uint32 redirectId)
{
  dsStdMibRedirectEntryCtrl_t *pRow; 
  dsStdMibRedirectKey_t       keys;
                           
  if(DSSTDMIB_REDIRECT_INDEX_OUT_OF_RANGE(redirectId) == L7_TRUE)
    return L7_FAILURE;

  keys.redirectId = redirectId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_REDIRECT],
                      &keys, AVL_EXACT);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Redirect Table
*
* @param    prevRedirectId  @b{(input)}  Redirect Id to begin search
* @param    pRedirectId     @b{(output)} Pointer to next sequential
*                                          Redirect Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevRedirectId value of 0 is used to find the first
*           Redirect Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServRedirectGetNext(L7_uint32 prevRedirectId,
                                L7_uint32 *pRedirectId)
{
  dsStdMibRedirectEntryCtrl_t *pRow; 
  dsStdMibRedirectKey_t       keys;
                           
  DIFFSERV_NULLPTR_CHECK(pRedirectId);

  keys.redirectId = prevRedirectId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_REDIRECT],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
  {
    *pRedirectId = 0;
    return L7_FAILURE;
  }

  *pRedirectId = pRow->key.redirectId;
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get the value of an object in the Redirect Table based
*           on Object Id
*
* @param    redirectId  @b{(input)}  Redirect Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to object value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServRedirectObjectGet(L7_uint32 redirectId,
                                  L7_DIFFSERV_REDIRECT_TABLE_OBJECT_t objectId,
                                  void *pValue)
{
  L7_RC_t                     rc = L7_SUCCESS;
  dsStdMibRedirectEntryCtrl_t *pRow; 
  dsStdMibRedirectKey_t       keys;
  
  if(DSSTDMIB_REDIRECT_INDEX_OUT_OF_RANGE(redirectId) == L7_TRUE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pValue);

  keys.redirectId = redirectId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_REDIRECT],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
    return L7_ERROR;
  
  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  switch(objectId)
  {
  case L7_DIFFSERV_REDIRECT_INTF:
    *(L7_uint32 *)pValue = pRow->mib.intIfNum;
    break;

  case L7_DIFFSERV_REDIRECT_STORAGE:
    *(dsStdMibStorageType_t *)pValue = pRow->mib.storage;
    break;

  case L7_DIFFSERV_REDIRECT_ROW_STATUS:
    *(dsStdMibRowStatus_t *)pValue = pRow->mib.status;
    break;
  
  default:
    rc = L7_FAILURE;
    break;

  } /* endswitch */
  
  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
  return rc; 
}


/*
===========================
===========================
===========================

     MIRROR TABLE APIs

===========================
===========================
===========================
*/

/*********************************************************************
* @purpose  Initialize draft MIB Mirror support
*
* @param    void
*
* @returns  void
*
* @notes
*       
* @end
*********************************************************************/
void dsStdMibMirrorInit(void)
{
  /* initialize index in-use bitmask */
  memset(&mirrorIdInUseMask, 0, sizeof(mirrorIdInUseMask));

  /* establish initial index next values */
  dsStdMibMirrorIdNextUpdate();
}

/*********************************************************************
* @purpose  Obtain the next available Id value to be used when
*           creating a new row in the Mirror Table
*
* @param    pMirrorId   @b{(output)} Pointer to the Mirror Id value
*
* @returns  void
*
* @notes    Only outputs a value if the pMirrorId parm is non-null.
*
* @notes    An output *pMirrorId value of 0 means the table is full.
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
void dsStdMibMirrorIdNext(L7_uint32 *pMirrorId)
{
  if (pMirrorId != L7_NULLPTR)
  {
    if (DSSTDMIB_MIRROR_INDEX_OUT_OF_RANGE(mirrorIdNextFree) == L7_TRUE)
    {
      *pMirrorId = 0;
    }
    else
    {
      *pMirrorId = mirrorIdNextFree;
    }
  }
}

/*********************************************************************
* @purpose  Determine next available Mirror Id value and update
*           its value
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
void dsStdMibMirrorIdNextUpdate(void)
{
  L7_uint32    i;

  for (i = 1; i <= DSSTDMIB_MIRROR_INDEX_MAX; i++)
  {
    /* Look for first entry not in use.  Do not mark it as in use until
     * a row using this index value is successfully created.
     */
    if (L7_INTF_ISMASKBITSET(mirrorIdInUseMask, i) == L7_FALSE)
      break;
  }

  /* Use a value of 0 to indicate all indexes currently in use */
  if (i > DSSTDMIB_MIRROR_INDEX_MAX)
    i = 0;

  mirrorIdNextFree = i;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Mirror row element
*
* @param    mirrorId    @b{(input)} Mirror Id
*
* @returns  pRow        Pointer to row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*       
* @end
*********************************************************************/
dsStdMibMirrorEntryCtrl_t *dsStdMibMirrorRowFind(L7_uint32 mirrorId)
{
  dsStdMibMirrorEntryCtrl_t   *pRow;
  dsStdMibMirrorKey_t         keys;

  keys.mirrorId = mirrorId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_MIRROR], 
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Create a new row in the Mirror Table 
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrIndex  @b{(input)} Policy Attr Index
* @param    pMirrorId        @b{(output)}Pointer to Mirror Id value
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    No more index values available (table full)
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServMirrorCreate(L7_uint32 policyIndex, 
                             L7_uint32 policyInstIndex, 
                             L7_uint32 policyAttrIndex, 
                             L7_uint32 *pMirrorId)
{
  L7_RC_t                         rc = L7_FAILURE;
  dsStdMibTableId_t               tableId = DSSTDMIB_TABLE_ID_MIRROR;
  dsStdMibMirrorEntryCtrl_t       mirrorRow, *pMirrorRow = L7_NULLPTR;
  L7_uint32                       mirrorId;
  L7_uint32                       intIfNum;
  nimConfigID_t                   configId;

  DIFFSERV_NULLPTR_CHECK(pMirrorId);

  do
  {
    /* get the next available mirror id */
    dsStdMibMirrorIdNext(pMirrorId);

    /* set up a local working copy of the id */
    mirrorId = *pMirrorId;

    if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
    {
      DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                               mirrorId, 0, 0, 
                               (L7_uchar8)L7_TRUE);
    }

    /* mirror id 0 means the table is full */
    if (mirrorId == 0)
    {
      rc = L7_ERROR;
      break;
    }

    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                    L7_DIFFSERV_POLICY_ATTR_STMT_MIRROR_INTF,
                                    &configId) != L7_SUCCESS)
      break;

    if (nimIntIfFromConfigIDGet(&configId, &intIfNum) != L7_SUCCESS)
      break;

    pMirrorRow = &mirrorRow;
    memset(pMirrorRow, 0, sizeof(*pMirrorRow));

    pMirrorRow->key.mirrorId = mirrorId;

    pMirrorRow->tableId = tableId;

    pMirrorRow->mib.id = mirrorId;
    pMirrorRow->mib.intIfNum = intIfNum;
    pMirrorRow->mib.storage = DSSTDMIB_DEFAULT_STORAGE_TYPE; 
    pMirrorRow->mib.status = L7_DIFFSERV_ROW_STATUS_ACTIVE;

    /* Insert Mirror Entry in AVL */
    if (dsmibRowInsert(&dsStdMibAvlTree[tableId],
                       (void *)pMirrorRow) != L7_SUCCESS)
      break;

    /* successful row creation -- update next free index value*/
    L7_INTF_SETMASKBIT(mirrorIdInUseMask, mirrorId);
    dsStdMibMirrorIdNextUpdate();

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            mirrorId, 0, 0,
                            (L7_uint32)pMirrorRow, (L7_uchar8)rc,
                            (L7_uchar8)L7_TRUE);
  }

  return rc;
}

/*********************************************************************
* @purpose  Delete a row from the Mirror Table for the specified
*           index
*
* @param    mirrorId      @b{(input)} Mirror Id
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServMirrorDelete(L7_uint32 mirrorId)
{
  L7_RC_t                         rc = L7_FAILURE;
  dsStdMibTableId_t               tableId = DSSTDMIB_TABLE_ID_MIRROR;
  dsStdMibMirrorEntryCtrl_t       *pRow = L7_NULLPTR;

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                             mirrorId, 0, 0);
  }

  do
  {
    if(DSSTDMIB_MIRROR_INDEX_OUT_OF_RANGE(mirrorId) == L7_TRUE)
       break;

    pRow = dsStdMibMirrorRowFind(mirrorId);
    if (pRow == L7_NULLPTR)
    {
      rc = L7_SUCCESS;          /* consider this a success if row not found */
      break;
    }

    if (dsmibRowDelete(&dsStdMibAvlTree[tableId], pRow) != L7_SUCCESS)
      break;

    /* successful row deletion -- update next free index value*/
    L7_INTF_CLRMASKBIT(mirrorIdInUseMask, mirrorId);
    dsStdMibMirrorIdNextUpdate();

    rc = L7_SUCCESS;

  } while (0);

  if (DS_TRACE_MODE_ENHANCED() == L7_TRUE)
  {
    DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId, 
                            mirrorId, 0, 0,                                                
                            (L7_uint32)pRow, (L7_uchar8)rc);
  }

  return rc;
}
                                                                            
/*********************************************************************
 NOTE: MIRROR TABLE EXTERNAL APIs
*********************************************************************/
 
/*************************************************************************
* @purpose  Obtain the current value of mirror index next variable
*
* @param    pMirrorIndex    @b{(output)} Pointer to mirror index value
*
* @returns  void
*
* @notes    Only outputs a value if the pMirrorIndex parm is non-null.
*
* @notes    A *pMirrorIndex output value of 0 means the table is full.
*
* @end
*********************************************************************/
void diffServMirrorIndexNext(L7_uint32 *pMirrorIndex)
{
  if (pMirrorIndex == L7_NULLPTR)
    return;

  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  dsStdMibMirrorIdNext(pMirrorIndex);

  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
}

/*********************************************************************
* @purpose  Verify that a Mirror Table row exists for the 
*           specified Mirror Id
*
* @param    mirrorId    @b{(input)} Mirror Id
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServMirrorGet(L7_uint32 mirrorId)
{
  dsStdMibMirrorEntryCtrl_t   *pRow; 
  dsStdMibMirrorKey_t         keys;
                           
  if(DSSTDMIB_MIRROR_INDEX_OUT_OF_RANGE(mirrorId) == L7_TRUE)
    return L7_FAILURE;

  keys.mirrorId = mirrorId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_MIRROR],
                      &keys, AVL_EXACT);

  return (pRow != L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Determine next sequential row in the Mirror Table
*
* @param    prevMirrorId    @b{(input)}  Mirror Id to begin search
* @param    pMirrorId       @b{(output)} Pointer to next sequential
*                                          Mirror Id value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @notes    A prevMirrorId value of 0 is used to find the first
*           Mirror Entry in the table
*
* @end
*********************************************************************/
L7_RC_t diffServMirrorGetNext(L7_uint32 prevMirrorId,
                              L7_uint32 *pMirrorId)
{
  dsStdMibMirrorEntryCtrl_t   *pRow; 
  dsStdMibMirrorKey_t         keys;
                           
  DIFFSERV_NULLPTR_CHECK(pMirrorId);

  keys.mirrorId = prevMirrorId;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_MIRROR],
                      &keys, AVL_NEXT);

  if (pRow == L7_NULLPTR)
  {
    *pMirrorId = 0;
    return L7_FAILURE;
  }

  *pMirrorId = pRow->key.mirrorId;
  return L7_SUCCESS; 
}

/*********************************************************************
* @purpose  Get the value of an object in the Mirror Table based
*           on Object Id
*
* @param    mirrorId    @b{(input)}  Mirror Id
* @param    objectId    @b{(input)}  Object Id
* @param    pValue      @b{(output)} Pointer to object value 
*
* @returns  L7_SUCCESS  
* @returns  L7_ERROR    Row does not exist for the specified index
* @returns  L7_FAILURE  
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t diffServMirrorObjectGet(L7_uint32 mirrorId,
                                L7_DIFFSERV_MIRROR_TABLE_OBJECT_t objectId,
                                void *pValue)
{
  L7_RC_t                     rc = L7_SUCCESS;
  dsStdMibMirrorEntryCtrl_t   *pRow; 
  dsStdMibMirrorKey_t         keys;
  
  if(DSSTDMIB_MIRROR_INDEX_OUT_OF_RANGE(mirrorId) == L7_TRUE)
    return L7_FAILURE;

  DIFFSERV_NULLPTR_CHECK(pValue);

  keys.mirrorId = mirrorId;
  
  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_MIRROR],
                      &keys, AVL_EXACT);
  if (pRow == L7_NULLPTR)
    return L7_ERROR;
  
  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  switch(objectId)
  {
  case L7_DIFFSERV_MIRROR_INTF:
    *(L7_uint32 *)pValue = pRow->mib.intIfNum;
    break;

  case L7_DIFFSERV_MIRROR_STORAGE:
    *(dsStdMibStorageType_t *)pValue = pRow->mib.storage;
    break;

  case L7_DIFFSERV_MIRROR_ROW_STATUS:
    *(dsStdMibRowStatus_t *)pValue = pRow->mib.status;
    break;
  
  default:
    rc = L7_FAILURE;
    break;

  } /* endswitch */
  
  DIFFSERV_SEMA_GIVE(dsStdMibSemId);
  return rc; 
}

