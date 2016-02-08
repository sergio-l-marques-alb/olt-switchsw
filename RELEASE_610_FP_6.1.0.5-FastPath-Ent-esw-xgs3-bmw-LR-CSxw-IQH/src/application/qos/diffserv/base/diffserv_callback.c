/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   diffserv_callback.c
*
* @purpose    DiffServ Component callback support
*
* @component  DiffServ
*
* @comments   none
*
* @create     12/21/2007
*
* @author     gpaussa
* @end
*
**********************************************************************/

#include "l7_diffserv_include.h"
#include "l7_cnfgr_api.h"
#include "usmdb_mib_diffserv_private_api.h"

/* global variables */
diffServCallbackInfo_t *pDiffServCallbackInfo_g = L7_NULLPTR;


/*********************************************************************
* @purpose  Initialize DiffServ component callback support
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments 
*       
* @end
*********************************************************************/
L7_RC_t diffServCallbackInit(void)
{
  L7_uint32               maxEnt, memSize;
  void                    *pMem;
  diffServCallbackTable_t *pTable;

  /* malloc space for DiffServ callback info structure  */
  pDiffServCallbackInfo_g = 
    (diffServCallbackInfo_t *)osapiMalloc(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, sizeof(diffServCallbackInfo_t));
  if (pDiffServCallbackInfo_g == L7_NULLPTR)
  {
    LOG_MSG("%s: Unable to allocate DiffServ callback info structure\n", __FUNCTION__);
    return L7_FAILURE;
  }
  memset(pDiffServCallbackInfo_g, 0, sizeof(diffServCallbackInfo_t));

  /* create a semaphore for callback table(s) mutual exclusion */
  pDiffServCallbackInfo_g->semId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (pDiffServCallbackInfo_g->semId == L7_NULLPTR)
  {
    LOG_MSG("%s: Unable to create callback table semaphore\n", __FUNCTION__);
    return L7_FAILURE;
  }

  /* malloc space for the DiffServ policy Notify Event callback registration table */
  maxEnt = DIFFSERV_CALLBACK_REGISTRANTS_MAX_NOTIFY_EVENT;
  memSize = maxEnt * sizeof(diffServCallbackEntry_t); 
  pMem = osapiMalloc(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, memSize);
  if (pMem == L7_NULLPTR)
  {
    LOG_MSG("%s: Unable to allocate policy Notify Event callback table\n", __FUNCTION__);
    return L7_FAILURE;
  }
  memset(pMem, 0, memSize);
  pTable = &pDiffServCallbackInfo_g->table[DIFFSERV_CALLBACK_TABLE_ID_NOTIFY_EVENT];
  pTable->pStart = (diffServCallbackEntry_t *)pMem;
  pTable->maxEntries = maxEnt;

  /* malloc space for the DiffServ policy Delete Approval callback registration table */
  maxEnt = DIFFSERV_CALLBACK_REGISTRANTS_MAX_DELETE_APPROVAL;
  memSize = maxEnt * sizeof(diffServCallbackEntry_t); 
  pMem = osapiMalloc(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, memSize);
  if (pMem == L7_NULLPTR)
  {
    LOG_MSG("%s: Unable to allocate policy Delete Approval callback table\n", __FUNCTION__);
    return L7_FAILURE;
  }
  memset(pMem, 0, memSize);
  pTable = &pDiffServCallbackInfo_g->table[DIFFSERV_CALLBACK_TABLE_ID_DELETE_APPROVAL];
  pTable->pStart = (diffServCallbackEntry_t *)pMem;
  pTable->maxEntries = maxEnt;

  /* set flag indicating callback registration tables are ready for use */
  pDiffServCallbackInfo_g->isInitialized = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clean up DiffServ component callback support after a failed init
*
* @param    void
*
* @returns  void
*
* @comments 
*       
* @end
*********************************************************************/
void diffServCallbackFini(void)
{
  diffServCallbackTable_t     *pTable;

  if (pDiffServCallbackInfo_g != L7_NULLPTR)
  {
    pDiffServCallbackInfo_g->isInitialized = L7_FALSE;

    /* NOTE: None of the pStart ptrs are set to L7_NULLPTR since the 
     *       entire callback info struct will be freed as well.
     */

    pTable = &pDiffServCallbackInfo_g->table[DIFFSERV_CALLBACK_TABLE_ID_DELETE_APPROVAL];
    if (pTable->pStart != L7_NULLPTR)
    {
      osapiFree(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pTable->pStart);
    }

    pTable = &pDiffServCallbackInfo_g->table[DIFFSERV_CALLBACK_TABLE_ID_NOTIFY_EVENT];
    if (pTable->pStart != L7_NULLPTR)
    {
      osapiFree(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pTable->pStart);
    }

    if (pDiffServCallbackInfo_g->semId != L7_NULLPTR)
    {
      if (osapiSemaDelete(pDiffServCallbackInfo_g->semId) != L7_SUCCESS)
      {
        LOG_MSG("%s: Error freeing callback table semaphore (0x%8.8x)\n",
                __FUNCTION__, (L7_uint32)pDiffServCallbackInfo_g->semId);
      }
    }

    osapiFree(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pDiffServCallbackInfo_g);
    pDiffServCallbackInfo_g = L7_NULLPTR;
  }
}

/*********************************************************************
*
* @purpose  Indicates whether the DiffServ component can accept callback registrations
*
* @param    void
*
* @returns  L7_TRUE
*           L7_FALSE
*
* @comments An L7_FALSE return from this routine usually means the caller
*           is attempting to register too soon, such as during phase1 init
*           instead of phase2.
*
* @end
*
*********************************************************************/
L7_BOOL diffServCallbackRegisterIsReady(void)
{
  return pDiffServCallbackInfo_g->isInitialized;
}

/*************************************************************************
* @purpose  Send callback notify event for all policies
*
* @param    void
*
* @returns  void
*
* @comments This is called during DiffServ initialization, after 
*           "special config mode" is turned off.
*
* @end
*********************************************************************/
void diffServCallbackPolicyAllSend(void)
{
  L7_uint32     policyIndex;

  /* walk through entire private MIB Policy Table */
  policyIndex = 0;                              /* start with first policy */
  while (diffServPolicyGetNext(policyIndex, &policyIndex) == L7_SUCCESS)
  {
    diffServChangeNotify(policyIndex, DIFFSERV_EVENT_CREATE, L7_NULLPTR);
  }
}

/*********************************************************************
*
* @purpose  Indicates whether the DiffServ component can accept callback registrations
*
* @param    policyIndex @b{(input)} policy index
*
* @returns  L7_TRUE
*           L7_FALSE
*
* @comments Policy is 'ready' if and only if:  the policy row is active
*           and all instances within this policy are active (at least one
*           such instance must exist).
*
* @comments This differs from dsDistillerPolicyIsReady() in that the determination
*           is based solely on the readiness of the policy instances and not on
*           any other factors such as the DiffServ admin mode, whether the policy
*           is assigned to a service interface, or the interface link state.
*
* @end
*
*********************************************************************/
L7_BOOL diffServCallbackPolicyIsReady(L7_uint32 policyIndex)
{
  L7_BOOL           rc = L7_FALSE;
  dsmibRowStatus_t  rowStatus;
  L7_uint32         policyInstIndex, nextPolicy;

  /* check for active policy row status */
  if ((diffServPolicyObjectGet(policyIndex, L7_DIFFSERV_POLICY_ROW_STATUS,
                              &rowStatus) != L7_SUCCESS) ||
      (rowStatus != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE))
  {
    return L7_FALSE;
  }

  /* check if each policy row instance is ready
   *
   * at least one instance must be defined, and all defined instances must
   * be in a ready state for the policy to be considered ready
   */
  policyInstIndex = 0;                          /* start with first instance */
  while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex,
                                    &nextPolicy, &policyInstIndex) == L7_SUCCESS) &&
         (nextPolicy == policyIndex))
  {
    rc = dsDistillerPolicyInstIsReady(policyIndex, policyInstIndex);
    if (rc == L7_FALSE)
      break;
  }

  return rc;
}

/*************************************************************************
* @purpose  Generic callback registration function for signaling DiffServ policy config changes.
*
* @param    tableId     @b{(input)} callback table identifier
* @param    funcPtr     @b{(input)} generic pointer to callback function
* @param    compId      @b{(input)} component ID
* @param    displayStr  @b{(input)} display string (for debugging), or L7_NULLPTR
*
* @returns  L7_SUCCESS
*           L7_REQUEST_DENIED DiffServ component is not yet initialized
*           L7_FAILURE        Bad function ptr, or maximum callbacks already registered
*
* @comments The displayStr parm can be used to specify information to be used
*           for debugging, such as the name of the callback function being registered
*           bu the caller.  This is optional and is set to L7_NULLPTR if not specified.
*
* @end
*********************************************************************/
static L7_RC_t dsCommonCallbackRegister(diffServCallbackTableId_t tableId,
                                        L7_VOIDFUNCPTR_t funcPtr,
                                        L7_COMPONENT_IDS_t compId,
                                        L7_uchar8 *displayStr)
{
  L7_uint32                   i, maxEnt;
  diffServCallbackEntry_t     *pTableStart, *pCbEntry;
  L7_RC_t                     rc = L7_FAILURE;

  if (diffServCallbackRegisterIsReady() != L7_TRUE)
    return L7_REQUEST_DENIED;

  if ((tableId < DIFFSERV_CALLBACK_TABLE_ID_FIRST) ||
      (tableId >= DIFFSERV_CALLBACK_TABLE_ID_TOTAL))
  {
    LOG_MSG("Invalid DiffServ callback registration table ID %d for component %u\n", 
            tableId, compId);
    return L7_FAILURE;
  }

  if ((funcPtr == L7_NULLPTR) ||
      ((compId <= L7_FIRST_COMPONENT_ID) || (compId >= L7_LAST_COMPONENT_ID)))
  {
    LOG_MSG("Invalid DiffServ callback registration for component %u\n", compId);
    return L7_FAILURE;
  }

  DIFFSERV_SEMA_TAKE(pDiffServCallbackInfo_g->semId, L7_WAIT_FOREVER);

  /* get table info */
  pTableStart = pDiffServCallbackInfo_g->table[tableId].pStart;
  maxEnt      = pDiffServCallbackInfo_g->table[tableId].maxEntries;

  /* prevent duplicate registration */
  for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
  {
    if (pCbEntry->funcPtr == funcPtr)
    {
      DIFFSERV_SEMA_GIVE(pDiffServCallbackInfo_g->semId);
      return L7_SUCCESS;
    }
  }

  for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
  {
    if (pCbEntry->funcPtr == L7_NULLPTR)
    {
      /* this index available */
      pCbEntry->funcPtr = funcPtr;
      pCbEntry->compId = compId;
      if (displayStr != L7_NULLPTR)
      {
        osapiStrncpySafe(pCbEntry->displayStr, displayStr, sizeof(pCbEntry->displayStr));
      }
      rc = L7_SUCCESS;
      break;
    }   
  }

  DIFFSERV_SEMA_GIVE(pDiffServCallbackInfo_g->semId);

  return rc;
}

/*************************************************************************
* @purpose  Generic callback unregistration function for signaling DiffServ policy config changes.
*
* @param    tableId           @b{(input)} callback table identifier
* @param    funcPtr           @b{(input)} generic pointer to callback function
*
* @returns  L7_SUCCESS
*           L7_REQUEST_DENIED DiffServ component is not yet initialized
*           L7_FAILURE        Function pointer is NULL, or is not found in the table
*
* @comments 
*
* @end
*********************************************************************/
static L7_RC_t dsCommonCallbackUnregister(diffServCallbackTableId_t tableId,
                                          L7_VOIDFUNCPTR_t funcPtr)
{
  L7_VOIDFUNCPTR_t            voidFuncPtr = L7_NULLPTR;
  L7_uint32                   i, maxEnt;
  diffServCallbackEntry_t     *pTableStart, *pCbEntry;
  L7_RC_t                     rc = L7_FAILURE;

  if (diffServCallbackRegisterIsReady() != L7_TRUE)
    return L7_REQUEST_DENIED;

  if ((tableId < DIFFSERV_CALLBACK_TABLE_ID_FIRST) ||
      (tableId >= DIFFSERV_CALLBACK_TABLE_ID_TOTAL) ||
      (funcPtr == L7_NULLPTR))
  {
    LOG_MSG("Invalid DiffServ callback de-registration for table ID %d\n", tableId);
    return L7_FAILURE;
  }

  DIFFSERV_SEMA_TAKE(pDiffServCallbackInfo_g->semId, L7_WAIT_FOREVER);

  /* get table info */
  pTableStart = pDiffServCallbackInfo_g->table[tableId].pStart;
  maxEnt      = pDiffServCallbackInfo_g->table[tableId].maxEntries;

  for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
  {
    if (pCbEntry->funcPtr == funcPtr)
    {
      pCbEntry->funcPtr = voidFuncPtr;
      pCbEntry->compId = 0;
      memset(pCbEntry->displayStr, 0, sizeof(pCbEntry->displayStr));
      rc = L7_SUCCESS;
      break;
    }
  }

  DIFFSERV_SEMA_GIVE(pDiffServCallbackInfo_g->semId);

  return rc;
}

/*************************************************************************
* @purpose  Generic function to indicate whether a callback routine is already registered
*           for DiffServ policy changes.
*
* @param    tableId           @b{(input)} callback table identifier
* @param    funcPtr           @b{(input)} generic pointer to callback function
*
* @returns  L7_TRUE
*           L7_FALSE
*
* @comments 
*
* @end
*********************************************************************/
static L7_BOOL dsCommonCallbackIsRegistered(diffServCallbackTableId_t tableId,
                                            L7_VOIDFUNCPTR_t funcPtr)
{
  L7_uint32                   i, maxEnt;
  diffServCallbackEntry_t     *pTableStart, *pCbEntry;
  L7_BOOL                     rc = L7_FALSE;

  if (diffServCallbackRegisterIsReady() != L7_TRUE)
    return L7_FALSE;

  if ((tableId < DIFFSERV_CALLBACK_TABLE_ID_FIRST) ||
      (tableId >= DIFFSERV_CALLBACK_TABLE_ID_TOTAL) ||
      (funcPtr == L7_NULLPTR))
  {
    return L7_FALSE;
  }

  DIFFSERV_SEMA_TAKE(pDiffServCallbackInfo_g->semId, L7_WAIT_FOREVER);

  /* get table info */
  pTableStart = pDiffServCallbackInfo_g->table[tableId].pStart;
  maxEnt      = pDiffServCallbackInfo_g->table[tableId].maxEntries;

  for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
  {
    if (pCbEntry->funcPtr == funcPtr)
    {
      rc = L7_TRUE;
      break;
    }
  }

  DIFFSERV_SEMA_GIVE(pDiffServCallbackInfo_g->semId);

  return rc;
}

/*************************************************************************
* @purpose  Generic function to display contents of the specified DiffServ callback table.
*
* @param    tableId           @b{(input)} callback table identifier
* @param    tableNameStr      @b{(input)} table name display string
*
* @returns  L7_SUCCESS
*           L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
static L7_RC_t dsCommonCallbackTableShow(diffServCallbackTableId_t tableId,
                                         L7_uchar8 *tableNameStr)
{
  L7_uint32                   i, maxEnt;
  diffServCallbackEntry_t     *pTableStart, *pCbEntry;
  L7_char8                    compIdStr[L7_COMPONENT_MNEMONIC_MAX_LEN];
  L7_BOOL                     displayHdr = L7_TRUE;
  L7_uint32                   msgLvlReqd = DIFFSERV_MSGLVL_ON;

  if (diffServCallbackRegisterIsReady() != L7_TRUE)
    return L7_REQUEST_DENIED;

  if ((tableId < DIFFSERV_CALLBACK_TABLE_ID_FIRST) ||
      (tableId >= DIFFSERV_CALLBACK_TABLE_ID_TOTAL))
  {
    return L7_FAILURE;
  }

  if (tableNameStr == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  DIFFSERV_SEMA_TAKE(pDiffServCallbackInfo_g->semId, L7_WAIT_FOREVER);

  /* get table info */
  pTableStart = pDiffServCallbackInfo_g->table[tableId].pStart;
  maxEnt      = pDiffServCallbackInfo_g->table[tableId].maxEntries;

  for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
  {
    if (pCbEntry->funcPtr != L7_NULLPTR)
    {
      /* only display the header output prior to the first valid entry */
      if (displayHdr == L7_TRUE)
      {
        DIFFSERV_PRT(msgLvlReqd, "\nContents of %s table:\n", tableNameStr);
        displayHdr = L7_FALSE;
      }

      if (cnfgrApiComponentMnemonicGet(pCbEntry->compId, compIdStr) != L7_SUCCESS)
      {
        osapiSnprintf(compIdStr, sizeof(compIdStr), "<unknown>");
      }
      DIFFSERV_PRT(msgLvlReqd,
              " [%2u] funcPtr=0x%8.8x comp=%-*s descr=%s\n",
              i, (L7_uint32)pCbEntry->funcPtr, sizeof(compIdStr), compIdStr, pCbEntry->displayStr);
    }
  } /* endfor */

  DIFFSERV_PRT(msgLvlReqd, "\n");

  DIFFSERV_SEMA_GIVE(pDiffServCallbackInfo_g->semId);

  return L7_SUCCESS;
}

/*************************************************************************
* @purpose  Register a notify event callback function to be called 
*           whenever there is a configuration change to a DiffServ policy. 
*
* @param    funcPtr     @b{(input)} pointer to the callback function
* @param    compId      @b{(input)} component ID
* @param    displayStr  @b{(input)} display string (for debugging), or L7_NULLPTR
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer bad, or maximum callbacks already registered
*
* @comments The displayStr parm can be used to specify information to be used
*           for debugging, such as the name of the callback function being registered
*           by the caller.  This is optional and is set to L7_NULLPTR if not specified.
*
*
* @end
*********************************************************************/
L7_RC_t diffServNotifyEventCallbackRegister(L7_DIFFSERV_NOTIFY_EVENT_FUNCPTR_t funcPtr,
                                            L7_COMPONENT_IDS_t compId,
                                            L7_uchar8 *displayStr)
{
  return dsCommonCallbackRegister(DIFFSERV_CALLBACK_TABLE_ID_NOTIFY_EVENT,
                                  (L7_VOIDFUNCPTR_t)funcPtr, compId, displayStr);
}

/*************************************************************************
* @purpose  Remove a notify event callback function from the registered list.
*
* @param    funcPtr  @b{(input)} pointer to callback function to unregister
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer NULL or not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t diffServNotifyEventCallbackUnregister(L7_DIFFSERV_NOTIFY_EVENT_FUNCPTR_t funcPtr)
{
  return dsCommonCallbackUnregister(DIFFSERV_CALLBACK_TABLE_ID_NOTIFY_EVENT, 
                                    (L7_VOIDFUNCPTR_t)funcPtr);
}

/*************************************************************************
* @purpose  Indicate whether a notify event callback function is already 
*           registered for DiffServ policy changes.
*
* @param    funcPtr     @b{(input)} pointer to the callback function
*
* @returns  L7_TRUE     already registered
* @returns  L7_FALSE    not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t diffServNotifyEventCallbackIsRegistered(L7_DIFFSERV_NOTIFY_EVENT_FUNCPTR_t funcPtr)
{
  return dsCommonCallbackIsRegistered(DIFFSERV_CALLBACK_TABLE_ID_NOTIFY_EVENT,
                                      (L7_VOIDFUNCPTR_t)funcPtr);
}

/*************************************************************************
* @purpose  Display the contents of the notify event callback table.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t diffServNotifyEventCallbackTableShow(void)
{
  return dsCommonCallbackTableShow(DIFFSERV_CALLBACK_TABLE_ID_NOTIFY_EVENT,
                                   "Notify Event Callback");
}

/*************************************************************************
* @purpose  Register a delete approval callback function to be called 
*           prior to deleting a DiffServ policy.
*
* @param    funcPtr     @b{(input)} pointer to the callback function
* @param    compId      @b{(input)} component ID
* @param    displayStr  @b{(input)} display string (for debugging), or L7_NULLPTR
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer bad, or maximum callbacks already registered
*
* @comments The displayStr parm can be used to specify information to be used
*           for debugging, such as the name of the callback function being registered
*           by the caller.  This is optional and is set to L7_NULLPTR if not specified.
*
*
* @end
*********************************************************************/
L7_RC_t diffServDeleteApprovalCallbackRegister(L7_DIFFSERV_DELETE_APPROVAL_FUNCPTR_t funcPtr,
                                               L7_COMPONENT_IDS_t compId,
                                               L7_uchar8 *displayStr)
{
  return dsCommonCallbackRegister(DIFFSERV_CALLBACK_TABLE_ID_DELETE_APPROVAL,
                                  (L7_VOIDFUNCPTR_t)funcPtr, compId, displayStr);
}

/*************************************************************************
* @purpose  Remove a delete approval callback function from the registered list.
*
* @param    funcPtr  @b{(input)} pointer to callback function to unregister
*
* @returns  L7_SUCCESS          registration succeeded
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE          function pointer NULL or not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t diffServDeleteApprovalCallbackUnregister(L7_DIFFSERV_DELETE_APPROVAL_FUNCPTR_t funcPtr)
{
  return dsCommonCallbackUnregister(DIFFSERV_CALLBACK_TABLE_ID_DELETE_APPROVAL,
                                    (L7_VOIDFUNCPTR_t)funcPtr);
}

/*************************************************************************
* @purpose  Indicate whether a delete approval callback function is already 
*           registered for DiffServ policy changes.
*
* @param    funcPtr     @b{(input)} pointer to the callback function
*
* @returns  L7_TRUE     already registered
* @returns  L7_FALSE    not registered
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t diffServDeleteApprovalCallbackIsRegistered(L7_DIFFSERV_DELETE_APPROVAL_FUNCPTR_t funcPtr)
{
  return dsCommonCallbackIsRegistered(DIFFSERV_CALLBACK_TABLE_ID_DELETE_APPROVAL,
                                      (L7_VOIDFUNCPTR_t)funcPtr);
}

/*************************************************************************
* @purpose  Display the contents of the delete approval callback table.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t diffServDeleteApprovalCallbackTableShow(void)
{
  return dsCommonCallbackTableShow(DIFFSERV_CALLBACK_TABLE_ID_DELETE_APPROVAL,
                                   "Delete Approval Callback");
}

/*************************************************************************
* @purpose  Display the contents of all supported DiffServ callback tables.
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_REQUEST_DENIED   component is not yet initialized
* @returns  L7_FAILURE
*
* @comments 
*
* @end
*********************************************************************/
L7_RC_t diffServCallbackTableShow(void)
{
  L7_RC_t rc = L7_SUCCESS;

  if (diffServCallbackRegisterIsReady() != L7_TRUE)
    return L7_REQUEST_DENIED;

  if (diffServNotifyEventCallbackTableShow() != L7_SUCCESS)
    rc = L7_FAILURE;

  if (diffServDeleteApprovalCallbackTableShow() != L7_SUCCESS)
    rc = L7_FAILURE;

  return rc;
}

/*********************************************************************
*
* @purpose  Notify registered users of a change to a DiffServ policy
*
* @param    policyIndex   @b{(input)} Policy index
* @param    event         @b{(input)} Event indication
* @param    policyOldName @b{(input)} Previous policy name, or L7_NULLPTR
*
* @returns  void
*
* @comments This function is not used for a delete event. Use diffServDeleteNotify()
*           instead.
*
* @comments The policyOldName parm is used for a rename event only,
*           and is set to L7_NULLPTR otherwise.
*
* @comments If the policy is being named for the first time, the policyOldName
*           parameter is non-null and points to an empty string "".
*
* @end
*
*********************************************************************/
void diffServChangeNotify(L7_uint32 policyIndex, diffServEvent_t event, L7_uchar8 *policyOldName)
{
  dsmibPolicyType_t               policyType;
  L7_uchar8                       policyName[L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1];
  L7_DIFFSERV_NOTIFY_EVENT_t      callbackEvent = L7_DIFFSERV_NOTIFY_EVENT_CREATE;
  L7_BOOL                         sendNotify = L7_TRUE;

  /* this function is not used for delete events */
  if (event == DIFFSERV_EVENT_DELETE)
  {
    LOG_MSG("%s: Improper usage for DiffServ policy delete event\n", __FUNCTION__);
    return;
  }

  /* suppress notifications while the policy is in a not-ready state
   * (this usually implies that a series of row object updates is in progress)
   */
  if (diffServCallbackPolicyIsReady(policyIndex) != L7_TRUE)
    return;

  /* get the policy type and name */
  if ((diffServPolicyObjectGet(policyIndex, L7_DIFFSERV_POLICY_TYPE, &policyType) != L7_SUCCESS) ||
      (diffServPolicyObjectGet(policyIndex, L7_DIFFSERV_POLICY_NAME, policyName) != L7_SUCCESS))
    return;

  /* translate supported internal event value to public callback event value */
  switch (event)
  {
    case DIFFSERV_EVENT_CREATE:
      callbackEvent = L7_DIFFSERV_NOTIFY_EVENT_CREATE;
      break;

    case DIFFSERV_EVENT_MODIFY:
      callbackEvent = L7_DIFFSERV_NOTIFY_EVENT_MODIFY;
      break;

    case DIFFSERV_EVENT_RENAME:
      callbackEvent = L7_DIFFSERV_NOTIFY_EVENT_RENAME;
      break;

    default:
      sendNotify = L7_FALSE;
      break;

  } /* endswitch */

  /* send policy notification events */
  if (sendNotify == L7_TRUE)
  {
    diffServNotifyEventSend(callbackEvent, policyType, policyIndex, policyName, policyOldName);
  }
}

/*********************************************************************
*
* @purpose  Notify registered users of a deleted DiffServ policy
*
* @param    policyIndex   @b{(input)} Policy index
* @param    policyType    @b{(input)} Type of policy
* @param    policyName    @b{(input)} Policy name
*
* @returns  void
*
* @comments The policy no longer exists when this function is called, so any
*           attempts to get information from the MIB row will fail.
*
* @end
*
*********************************************************************/
void diffServDeleteNotify(L7_uint32 policyIndex, dsmibPolicyType_t policyType, L7_uchar8 *policyName)
{
  /* deleted policy name must be supplied by caller */
  if (policyName == L7_NULLPTR)
    return;

  diffServNotifyEventSend(L7_DIFFSERV_NOTIFY_EVENT_DELETE, policyType, policyIndex,
                          policyName, L7_NULLPTR);
}

/*********************************************************************
*
* @purpose  Notify registered users of a change to a DiffServ policy
*
* @param    callbackEvent @b{(input)} Callback event
* @param    policyType    @b{(input)} Type of policy
* @param    policyIndex   @b{(input)} Policy index
* @param    policyName    @b{(input)} Policy name
* @param    policyOldName @b{(input)} Previous policy name, or L7_NULLPTR
*
* @returns  void
*
* @comments The policyOldName parm is used for a rename event only,
*           and is set to L7_NULLPTR otherwise.
*
* @end
*
*********************************************************************/
void diffServNotifyEventSend(L7_DIFFSERV_NOTIFY_EVENT_t callbackEvent, 
                             dsmibPolicyType_t policyType, 
                             L7_uint32 policyIndex, 
                             L7_uchar8 *policyName, 
                             L7_uchar8 *policyOldName)
{
  L7_uint32                       i, maxEnt;
  diffServCallbackEntry_t         *pTableStart, *pCbEntry;

  /* send policy notification events */
  pTableStart = pDiffServCallbackInfo_g->table[DIFFSERV_CALLBACK_TABLE_ID_NOTIFY_EVENT].pStart;
  maxEnt      = pDiffServCallbackInfo_g->table[DIFFSERV_CALLBACK_TABLE_ID_NOTIFY_EVENT].maxEntries;

  for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
  {
    if (pCbEntry->funcPtr != L7_NULLPTR) 
    {
      /* The policyOldName parm is L7_NULLPTR except for a name change event. If the
       * DiffServ policy is being named for the first time, the policyOldName points to
       * the empty string "".
       */
      (pCbEntry->funcPtr)(callbackEvent, policyType, policyIndex, policyName, policyOldName);
    }
  } /* endfor */
}

/*********************************************************************
*
* @purpose  Ask registered applications if it is okay to delete a DiffServ policy
*
* @param    policyIndex   @b{(input)} Policy index
*
* @returns  L7_SUCCESS    OK to delete DiffServ policy
* @returns  L7_FAILURE    Do not delete DiffServ policy (it is currently in use)
*
* @comments Gives registered apps the chance to block an ACL from 
*           being deleted by the user when it is being used elsewhere
*           in the switch configuration.
*
* @end
*
*********************************************************************/
L7_RC_t diffServDeleteApprovalQuery(L7_uint32 policyIndex)
{
  L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_t   policyType;
  L7_uchar8                             policyName[L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1];
  L7_FUNCPTR_t                          funcPtr;
  L7_uint32                             i, maxEnt;
  diffServCallbackEntry_t               *pTableStart, *pCbEntry;
  L7_RC_t                               rc = L7_FAILURE;

  /* get the policy type and name
   *
   * NOTE: If we cannot get the policy type or name then there is something
   *       strange about this policy, so allow it to be deleted without
   *       querying the registrants.
   */
  if ((diffServPolicyObjectGet(policyIndex, L7_DIFFSERV_POLICY_TYPE, &policyType) != L7_SUCCESS) ||
      (diffServPolicyObjectGet(policyIndex, L7_DIFFSERV_POLICY_NAME, policyName) != L7_SUCCESS))
  {
    return L7_SUCCESS;                  /* see note above! */
  }

  /* initiate DiffServ policy delete approval callbacks */
  pTableStart = pDiffServCallbackInfo_g->table[DIFFSERV_CALLBACK_TABLE_ID_DELETE_APPROVAL].pStart;
  maxEnt      = pDiffServCallbackInfo_g->table[DIFFSERV_CALLBACK_TABLE_ID_DELETE_APPROVAL].maxEntries;

  for (i = 0, pCbEntry = pTableStart; i < maxEnt; i++, pCbEntry++)
  {
    if (pCbEntry->funcPtr != L7_NULLPTR) 
    {
      /* Can stop after the first negative response since that is enough to 
       * prevent the policy from being deleted.
       */
      funcPtr = (L7_FUNCPTR_t) pCbEntry->funcPtr;
      rc = (funcPtr)(policyType, policyIndex, policyName);
      if (rc != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
  } /* endfor */

  return L7_SUCCESS;
}

