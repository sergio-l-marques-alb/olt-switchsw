/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_prvtmib.c
*
* @purpose    DiffServ component Private MIB internal functions.
*
* @component  DiffServ
*
* @comments   none
*
* @create     04/22/2002
*
* @author     gpaussa
* @author     rjindal
* @end
*
**********************************************************************/
#include <string.h>
#include "l7_diffserv_include.h"
#include "nimapi.h"

#ifdef L7_QOS_FLEX_PACKAGE_ACL
  #include "acl_api.h"
#endif

/* all necessary AVL tree anchors and mutex semaphores */
avlTree_t       dsmibAvlTree[DSMIB_TABLE_ID_TOTAL];  /* AVL tree anchor nodes */
dsmibAvlCtrl_t  *pDsmibAvlCtrl = L7_NULLPTR;    /* AVL tree control data array*/
void *          dsmibIndexSemId = L7_NULLPTR;   /* MIB tables index semaphore */

/* global special configuration control */
static L7_BOOL  dsmibSpecialConfigMode;

/* MIB instantiations */
dsmibGenStatusGroup_t   genStatus;      /* General Status Group */

extern char *dsmibPrvtTableIdStr[];

/* local function prototypes */
static L7_RC_t dsmibAvlCreate(void);
static L7_RC_t dsmibAvlTreeCreate(avlTree_t *pTree, dsmibAvlCtrl_t *pCtrl);
static void    dsmibAvlDelete(void);
static void    dsmibAvlTreeDelete(avlTree_t *pTree, dsmibAvlCtrl_t *pCtrl);
static void    dsmibAvlPurge(void);
static void    dsmibGroupInit(void);
static void    dsmibGenStatusInit(void);
static void    dsmibClassInit(void);
static void    dsmibPolicyInit(void);
static void    dsmibServiceInit(void);


/*********************************************************************
* @purpose  Initialize DiffServ private MIB support
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServPrivateMibInit(void)
{
  /* create a semaphore to control mutual exclusion access when updating
   * any MIB table index value (same semaphore used for all private MIB tables)
   */
  dsmibIndexSemId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dsmibIndexSemId == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: Unable to create private MIB index semaphore\n", __FUNCTION__);
    return L7_FAILURE;
  }

  /* initialize all of the AVL resources used for private MIB tables */
  if (dsmibAvlCreate() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: Unable to create private MIB AVL trees\n", __FUNCTION__);
    return L7_FAILURE;
  }

  /* initialize all MIB groups */
  dsmibGroupInit();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize DiffServ private MIB support
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
void diffServPrivateMibFini(void)
{
  /* Uninitializing all MIB groups is the same as
   * initializing them so we will not do anything
   * with them here
   */

  /* clear contents of all private MIB table AVL trees */
  dsmibAvlDelete();

  /* delete the semaphore to control mutual exclusion access when updating
   * any MIB table index value (same semaphore used for all private MIB tables)
   */

  if (dsmibIndexSemId != L7_NULLPTR)
  {
    if (osapiSemaDelete(dsmibIndexSemId) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
              "%s: Error freeing MIB index table semaphore (0x%8.8x)\n",
              __FUNCTION__, (L7_uint32)dsmibIndexSemId);
    }
    dsmibIndexSemId = L7_NULLPTR;
  }
}


/*********************************************************************
* @purpose  Reset all DiffServ private MIB contents to initial state
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Improper use of this function will adversely affect DiffServ
*           operation.
*
* @end
*********************************************************************/
L7_RC_t diffServPrivateMibReset(void)
{
  /* clear contents of all private MIB table AVL trees */
  dsmibAvlPurge();

  /* re-initialize all private MIB groups */
  dsmibGroupInit();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize AVL control data and create private MIB AVL trees
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t dsmibAvlCreate(void)
{
  L7_RC_t         rc = L7_SUCCESS;
  dsmibAvlCtrl_t  *pCtrl;
  L7_uint32       i;

  /* allocate the AVL control data structure */
  pDsmibAvlCtrl = osapiMalloc(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, (L7_uint32)sizeof(*pDsmibAvlCtrl) * DSMIB_TABLE_ID_TOTAL);
  if (pDsmibAvlCtrl == L7_NULLPTR)
    return L7_FAILURE;

  /* Data to support Class Table AVL tree */
  pCtrl = pDsmibAvlCtrl + DSMIB_TABLE_ID_CLASS;
  pCtrl->entryMaxNum = DSMIB_CLASS_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsmibClassEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsmibClassKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* Data to support Class Rule Table AVL tree */
  pCtrl = pDsmibAvlCtrl + DSMIB_TABLE_ID_CLASS_RULE;
  pCtrl->entryMaxNum = DSMIB_CLASS_RULE_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsmibClassRuleEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsmibClassRuleKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* Data to support Policy Table AVL tree */
  pCtrl = pDsmibAvlCtrl + DSMIB_TABLE_ID_POLICY;
  pCtrl->entryMaxNum = DSMIB_POLICY_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsmibPolicyEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsmibPolicyKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* Data to support Policy Instance Table AVL tree */
  pCtrl = pDsmibAvlCtrl + DSMIB_TABLE_ID_POLICY_INST;
  pCtrl->entryMaxNum = DSMIB_POLICY_INST_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsmibPolicyInstEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsmibPolicyInstKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* Data to support Policy Attribute Table AVL tree */
  pCtrl = pDsmibAvlCtrl + DSMIB_TABLE_ID_POLICY_ATTR;
  pCtrl->entryMaxNum = DSMIB_POLICY_ATTR_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsmibPolicyAttrEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsmibPolicyAttrKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* Data to support Policy Inbound Performance Table AVL tree */
  pCtrl = pDsmibAvlCtrl + DSMIB_TABLE_ID_POLICY_PERF_IN;
  pCtrl->entryMaxNum = DSMIB_POLICY_PERF_IN_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsmibPolicyPerfInCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsmibPolicyPerfInKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* Data to support Policy Outbound Performance Table AVL tree */
  pCtrl = pDsmibAvlCtrl + DSMIB_TABLE_ID_POLICY_PERF_OUT;
  pCtrl->entryMaxNum = DSMIB_POLICY_PERF_OUT_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsmibPolicyPerfOutCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsmibPolicyPerfOutKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* Data to support Service Table AVL tree (includes Service Performance Table) */
  pCtrl = pDsmibAvlCtrl + DSMIB_TABLE_ID_SERVICE;
  pCtrl->entryMaxNum = DSMIB_SERVICE_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsmibServiceEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsmibServiceKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* initialize the AVL tree table */
  memset(dsmibAvlTree, 0, sizeof(dsmibAvlTree));

  /* create each AVL tree */
  for (i = (L7_uint32)(DSMIB_TABLE_ID_NONE+1); i < DSMIB_TABLE_ID_TOTAL; i++)
  {
    if (dsmibAvlTreeCreate(&dsmibAvlTree[i], pDsmibAvlCtrl+i) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }
  }

  /* clean up any partial allocations if failure occured during tree creation */
  if (rc != L7_SUCCESS)
  {
    dsmibAvlDelete();
  }

  return rc;
}

/*********************************************************************
* @purpose  Initialize all AVL resources used by DiffServ private MIB
*
* @param    pTree       @b{(input)}  AVL tree anchor ptr
* @param    pCtrl       @b{(input)}  AVL tree control ptr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
static L7_RC_t dsmibAvlTreeCreate(avlTree_t *pTree, dsmibAvlCtrl_t *pCtrl)
{
  L7_uint32     avlType = 0x10;                 /* common constant for all */

  if ((pTree == L7_NULLPTR) || (pCtrl == L7_NULLPTR))
    return L7_FAILURE;

  /* an entryMaxNum of 0 means no AVL tree resources are needed */
  if (pCtrl->entryMaxNum == 0)
    return L7_SUCCESS;

  pCtrl->pTreeHeap = osapiMalloc(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, (L7_uint32)sizeof(avlTreeTables_t) * pCtrl->entryMaxNum);
  pCtrl->pDataHeap = osapiMalloc(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pCtrl->entrySize * pCtrl->entryMaxNum);
  if ((pCtrl->pTreeHeap == L7_NULLPTR) || (pCtrl->pDataHeap == L7_NULLPTR))
    return L7_FAILURE;

  memset(pTree, 0, sizeof(avlTree_t));
  avlCreateAvlTree(pTree, pCtrl->pTreeHeap, pCtrl->pDataHeap,
                   pCtrl->entryMaxNum, pCtrl->entrySize, avlType,
                   pCtrl->entryKeySize);

  /* check AVL semaphore ID to estimate if create succeeded */
  if (pTree->semId == L7_NULLPTR)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete and deallocate all AVL resources used by DiffServ
*           private MIB
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void dsmibAvlDelete(void)
{
  L7_uint32     i;

  if (pDsmibAvlCtrl != L7_NULLPTR)
  {
    /* delete each AVL tree */
    for (i = (L7_uint32)(DSMIB_TABLE_ID_NONE+1); i < DSMIB_TABLE_ID_TOTAL; i++)
    {
      dsmibAvlTreeDelete(&dsmibAvlTree[i], pDsmibAvlCtrl+i);
    }

    /* deallocate the AVL control data structure */
    osapiFree(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pDsmibAvlCtrl);
    pDsmibAvlCtrl = L7_NULLPTR;
  }
}

/*********************************************************************
* @purpose  Deletes all resources used by a specific AVL tree
*
* @param    pTree       @b{(input)}  AVL tree anchor ptr
* @param    pCtrl       @b{(input)}  AVL tree control ptr
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void dsmibAvlTreeDelete(avlTree_t *pTree, dsmibAvlCtrl_t *pCtrl)
{
  if (pCtrl == L7_NULLPTR)
    return;

  /* an entryMaxNum of 0 means no AVL tree resources were used */
  if (pCtrl->entryMaxNum == 0)
    return;

  if (pTree != L7_NULLPTR)
  {
    /* using the AVL tree semaphore handle as an estimation of whether
     * the tree was created or not
     */
    if (pTree->semId != L7_NULLPTR)
    {
      (void)avlDeleteAvlTree(pTree);            /* ignore rc here */
    }

    memset(pTree, 0, sizeof(avlTree_t));
  }

  if (pCtrl->pTreeHeap != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pCtrl->pTreeHeap);
    pCtrl->pTreeHeap = L7_NULLPTR;
  }

  if (pCtrl->pDataHeap != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pCtrl->pDataHeap);
    pCtrl->pDataHeap = L7_NULLPTR;
  }

  return;
}

/*********************************************************************
* @purpose  Purge all AVL trees used by DiffServ private MIB
*
* @param    void
*
* @returns  void
*
* @notes    Purging clears the tree contents, but does not de-allocate
*           the tree.
*
* @end
*********************************************************************/
static void dsmibAvlPurge(void)
{
  dsmibAvlCtrl_t *pCtrl;
  L7_uint32      i;

  if (pDsmibAvlCtrl != L7_NULLPTR)
  {
    /* purge each AVL tree */
    for (i = (L7_uint32)(DSMIB_TABLE_ID_NONE+1); i < DSMIB_TABLE_ID_TOTAL; i++)
    {
      pCtrl = pDsmibAvlCtrl + i;

      if (pCtrl->entryMaxNum != 0)
      {
        avlPurgeAvlTree(&dsmibAvlTree[i], pCtrl->entryMaxNum);
      }
    }
  }
}

/*********************************************************************
* @purpose  Initialize all DiffServ private MIB Groups
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void dsmibGroupInit(void)
{
  /* initialize special configuration mode flag */
  dsmibSpecialConfigMode = L7_FALSE;

  /* initialize each major private MIB group
   * (called function is responsible for its own failure log messages)
   */
  dsmibGenStatusInit();
  dsmibClassInit();
  dsmibPolicyInit();
  dsmibServiceInit();
}

/*********************************************************************
* @purpose  Initialize private MIB General Status Group
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void dsmibGenStatusInit(void)
{
  /* NOTE:  The table size and max values are initialized within their own
   *        section init routines.
   */
  memset(&genStatus, 0, sizeof(genStatus));
  genStatus.adminMode = L7_DISABLE;     /*will be updated later from saved cfg*/
}

/*********************************************************************
* @purpose  Initialize private MIB Class support
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void dsmibClassInit(void)
{
  /* establish initial index next values */
  dsmibClassIndexNextInit();

  /* initialize size values of various Class tables */
  genStatus.tableSize[DSMIB_TABLE_ID_CLASS] = 0;
  genStatus.tableMax[DSMIB_TABLE_ID_CLASS] = DSMIB_CLASS_TABLE_SIZE_MAX;

  genStatus.tableSize[DSMIB_TABLE_ID_CLASS_RULE] = 0;
  genStatus.tableMax[DSMIB_TABLE_ID_CLASS_RULE] =
                                        DSMIB_CLASS_RULE_TABLE_SIZE_MAX;

  /* NOTE: If a "default" class is ever needed, create it here.  Use a
   *       class type of 'any' and a single class rule of match 'every'.
   *       No one is allowed to change the default class definition or
   *       delete it.
   */
}

/*********************************************************************
* @purpose  Initialize private MIB Policy support
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void dsmibPolicyInit(void)
{
  /* establish initial index next values */
  dsmibPolicyIndexNextInit();

  /* initialize size of various Policy tables */
  genStatus.tableSize[DSMIB_TABLE_ID_POLICY] = 0;
  genStatus.tableMax[DSMIB_TABLE_ID_POLICY] = DSMIB_POLICY_TABLE_SIZE_MAX;

  genStatus.tableSize[DSMIB_TABLE_ID_POLICY_INST] = 0;
  genStatus.tableMax[DSMIB_TABLE_ID_POLICY_INST] =
                                        DSMIB_POLICY_INST_TABLE_SIZE_MAX;

  genStatus.tableSize[DSMIB_TABLE_ID_POLICY_ATTR] = 0;
  genStatus.tableMax[DSMIB_TABLE_ID_POLICY_ATTR] =
                                        DSMIB_POLICY_ATTR_TABLE_SIZE_MAX;

  genStatus.tableSize[DSMIB_TABLE_ID_POLICY_PERF_IN] = 0;
  genStatus.tableMax[DSMIB_TABLE_ID_POLICY_PERF_IN] =
                                        DSMIB_POLICY_PERF_IN_TABLE_SIZE_MAX;

  genStatus.tableSize[DSMIB_TABLE_ID_POLICY_PERF_OUT] = 0;
  genStatus.tableMax[DSMIB_TABLE_ID_POLICY_PERF_OUT] =
                                        DSMIB_POLICY_PERF_OUT_TABLE_SIZE_MAX;

  /* establish initial ipdscp value support */
  dsmibPolicyAttrIpDscpSupportInit();
}

/*********************************************************************
* @purpose  Initialize private MIB Service support
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
static void dsmibServiceInit(void)
{
  /* initialize size of Service table */
  genStatus.tableSize[DSMIB_TABLE_ID_SERVICE] = 0;
  genStatus.tableMax[DSMIB_TABLE_ID_SERVICE] = DSMIB_SERVICE_TABLE_SIZE_MAX;
}

/*********************************************************************
* @purpose  Get private MIB special config mode flag value
*
* @param    void
*
* @returns  Special config mode flag (L7_TRUE, L7_FALSE)
*
* @notes    This is an access function used within the DiffServ component,
*           but whose usage is limited to the private MIB support and the
*           DiffServ configuration handler.  It is not part of the private
*           MIB API.
*
* @end
*********************************************************************/
L7_BOOL dsmibSpecialConfigModeGet(void)
{
  return dsmibSpecialConfigMode;
}

/*********************************************************************
* @purpose  Activates private MIB special configuration mode
*
* @param    void
*
* @returns  void
*
* @notes    For DiffServ internal component use only.
*
* @end
*********************************************************************/
void dsmibSpecialConfigModeOn(void)
{
  dsmibSpecialConfigMode = L7_TRUE;
}

/*********************************************************************
* @purpose  Deactivates private MIB special configuration mode
*
* @param    void
*
* @returns  void
*
* @notes    For DiffServ internal component use only.
*
* @end
*********************************************************************/
void dsmibSpecialConfigModeOff(void)
{
  dsmibSpecialConfigMode = L7_FALSE;
}

/*********************************************************************
* @purpose  Checks if the specified MIB table size is at its maximum
*
* @param    tableId     @b{(input)} MIB table identifier
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Table is full
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsmibMibTableSizeCheck(dsmibTableId_t tableId)
{
  /* check inputs */
  if ((tableId <= DSMIB_TABLE_ID_NONE) ||
      (tableId >= DSMIB_TABLE_ID_TOTAL))
    return L7_FAILURE;

  /* the table size should never exceed its maximum */
  if (genStatus.tableSize[tableId] > genStatus.tableMax[tableId])
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "dsmibMibTableSizeCheck: table ID %u size %u exceeds maximum %u\n",
            tableId, genStatus.tableSize[tableId], genStatus.tableMax[tableId]);
    return L7_FAILURE;
  }

  return (genStatus.tableSize[tableId] < genStatus.tableMax[tableId]) ?
         L7_SUCCESS : L7_ERROR;
}

/*********************************************************************
* @purpose  Insert a row in the specified MIB table
*
* @param    pTree       @b{(input)} AVL tree pointer
* @param    pElem       @b{(output)} Pointer to new element content
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row element already exists
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsmibRowInsert(avlTree_t *pTree, void *pElem)
{
  void          *pNode;
  L7_RC_t       rc;

  /* check inputs */
  if (pTree == L7_NULLPTR)
    return L7_FAILURE;
  if (pElem == L7_NULLPTR)
    return L7_FAILURE;

  /* create node in AVL tree */
  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);
  pNode = avlInsertEntry(pTree, pElem);
  DIFFSERV_SEMA_GIVE(pTree->semId);

  if (pNode == L7_NULLPTR)
    rc = L7_SUCCESS;
  else if (pNode == pElem)
    rc = L7_FAILURE;
  else
    rc = L7_ERROR;                              /* duplicate exists in tree */

  return rc;
}

/*********************************************************************
* @purpose  Delete a row from the specified MIB table
*
* @param    pTree       @b{(input)} AVL tree pointer
* @param    pElem       @b{(output)} Pointer to deletion element
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Row element does not exist
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsmibRowDelete(avlTree_t *pTree, void *pElem)
{
  void          *pNode;

  /* check inputs */
  if (pTree == L7_NULLPTR)
    return L7_FAILURE;
  if (pElem == L7_NULLPTR)
    return L7_FAILURE;

  /* create node in AVL tree */
  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);
  pNode = avlDeleteEntry(pTree, pElem);
  DIFFSERV_SEMA_GIVE(pTree->semId);

  return (pNode != L7_NULLPTR) ? L7_SUCCESS : L7_ERROR;
}

/*********************************************************************
* @purpose  Find a row in the specified MIB table
*
* @param    pTree       @b{(input)} AVL tree pointer
* @param    pKeys       @b{(input)} Search key list pointer
* @param    matchMode   @b{(input)} Type of search (AVL_EXACT, AVL_NEXT)
* @param    pRowPtr     @b{(output)} Pointer to row pointer value
*
* @returns  rowPtr      Table row pointer as specified by search key list
* @returns  L7_NULLPTR  Row not found, or other failure occurred
*
* @notes    This function can be used to find the current or next entry
*           in an AVL tree.
*
* @end
*********************************************************************/
void *dsmibRowFind(avlTree_t *pTree, void *pKeys, L7_uint32 matchMode)
{
  void          *rowPtr;

  /* check inputs */
  if (pTree == L7_NULLPTR)
    return L7_NULLPTR;
  if (pKeys == L7_NULLPTR)
    return L7_NULLPTR;
  if ((matchMode != AVL_EXACT) && (matchMode != AVL_NEXT))
    return L7_NULLPTR;

  /* find node in AVL tree */
  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);
  rowPtr = avlSearchLVL7(pTree, pKeys, matchMode);
  DIFFSERV_SEMA_GIVE(pTree->semId);

  return rowPtr;
}

/*********************************************************************
* @purpose  Check if a storage type set value is valid
*
* @param    val         @b{(input)} Storage type set value
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    This check is common to all MIB table 'SetTest' APIs.
*
* @end
*********************************************************************/
L7_BOOL dsmibStorageTypeIsValid(dsmibStorageType_t val)
{
  if ((val > L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_NONE) &&
      (val < L7_USMDB_MIB_DIFFSERV_STORAGE_TYPE_TOTAL))
    return L7_TRUE;

  return L7_FALSE;
}

/*********************************************************************
* @purpose  Check if a row status set value is valid
*
* @param    val         @b{(input)} Row status set value
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    This check is common to all MIB table 'SetTest' APIs.
*
* @end
*********************************************************************/
L7_BOOL dsmibRowStatusIsValid(dsmibRowStatus_t val)
{
  L7_BOOL       rc = L7_FALSE;

  /* only certain row status values can be 'set' directly through the API
   *
   * NOTE:  create-and-go and create-and-wait are not set directly,
   *        but are implied via 'create' API.
   */
  switch (val)
  {
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
    rc = L7_TRUE;
    break;

  default:
    break;
  }

  return rc;
}

/*********************************************************************
* @purpose  Output the externally-observable row status value
*
* @param    val             @b{(input)}  Actual row status value
* @param    rowInvalidFlags @b{(input)}  Row invalid flags
* @param    pOut            @b{(output)} Pointer to output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function translates the actual row status into a valid
*           'read' value.
*
* @end
*********************************************************************/
L7_RC_t dsmibRowStatusObservable(dsmibRowStatus_t val,
                                 L7_uint32 rowInvalidFlags,
                                 dsmibRowStatus_t *pOut)
{
  dsmibRowStatus_t  outVal;

  /* In most cases, the visible output value is the same as the actual
   * row status value.  Specific overrides are made below.
   */
  outVal = val;

  switch (val)
  {
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
    /* the 'notReady' value is never a valid actual value */
    *pOut = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
    return L7_FAILURE;

  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
    /* the 'create' values are always presented externally as 'notReady' */
    outVal = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY;
    break;

  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
    /* an incomplete row is presented externally as 'notReady' */
    if (rowInvalidFlags != 0)
      outVal = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY;
    break;

  default:
    break;
  }

  *pOut = outVal;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check for a MIB table row completion and generate the
*           new row status value
*
* @param    oldVal          @b{(input)}  Existing row status value
* @param    flags           @b{(input)}  Row invalid flags
* @param    pNewVal         @b{(output)} Pointer to new status output value
* @param    pJustCompleted  @b{(output)} Pointer to boolean output value
*
* @returns  void
*
* @notes    Only outputs values if the pNewVal and pJustCompleted parms
*           are non-null, respectively.
*
* @notes    The oldVal parameter must be an ACTUAL row status value, not
*           an "externally-observable" value produced by
*           dsmibRowStatusObservable().
*
* @notes    The caller must check the *pNewVal result to determine if
*           there is a change in row status, since L7_SUCCESS only
*           indicates there was no malfunction.
*
* @end
*********************************************************************/
void dsmibRowCompleteCheck(dsmibRowStatus_t oldVal, L7_uint32 flags,
                           dsmibRowStatus_t *pNewVal, L7_BOOL *pJustCompleted)
{
  dsmibRowStatus_t  newVal;
  L7_BOOL           justCompleted;

  newVal = oldVal;
  justCompleted = L7_FALSE;

  /* check if row is complete */
  if (flags == 0)
  {
    /* row is complete, but need to determine if this is new news */
    switch (oldVal)
    {
    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
      newVal = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE;
      justCompleted = L7_TRUE;
      break;

    case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
      newVal = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE;
      justCompleted = L7_TRUE;
      break;

    default:
      break;
    }
  } /* endif */

  if (pNewVal != L7_NULLPTR)
    *pNewVal = newVal;

  if (pJustCompleted != L7_NULLPTR)
    *pJustCompleted = justCompleted;
}

/*********************************************************************
* @purpose  Process a row status update for a MIB table row
*
* @param    oldVal      @b{(input)}  Existing row status value
* @param    reqVal      @b{(input)}  Requested row status value
* @param    flags       @b{(input)}  Row invalid flags
* @param    pRowStatus  @b{(output)} Pointer to row status location to update
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function is used internally by the DiffServ private MIB
*           component and does not impose the same restrictions as the API
*           function.
*
* @end
*********************************************************************/
L7_RC_t dsmibRowStatusUpdate(dsmibRowStatus_t oldVal, dsmibRowStatus_t reqVal,
                             L7_uint32 flags, dsmibRowStatus_t *pRowStatus)
{
  /* check inputs */
  if (pRowStatus == L7_NULLPTR)
    return L7_FAILURE;

  /* only certain requested row status values are permitted here */
  switch (reqVal)
  {
  /* values allowed */
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
    break;

  /* values not allowed */
  /* NOTE: createAndGo and createAndWait are set automatically via the 'create'
   *       API, so are not allowed as directly-settable row status values
   */
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_GO:
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_CREATE_AND_WAIT:
  default:  /* invalid value */
    return L7_FAILURE;
    /*PASSTHRU*/
  }

  /* do not change an existing row status of destroy */
  if (oldVal == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY)
  {
    *pRowStatus = oldVal;
    return (reqVal == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY) ?
             L7_SUCCESS : L7_FAILURE;
  }

  /* evaluate the allowed row status request values */
  switch (reqVal)
  {
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
    /* use requested value, unless row is incomplete */
    if (flags != 0)           /* row is not ready */
      return L7_FAILURE;
    break;

  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
    /* always accept a destroy value */
    break;

  default:
    return L7_FAILURE;
    /*PASSTHRU*/
  }

  *pRowStatus = reqVal;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if a MIB table row has been completed and generate the
*           new row status value
*
* @param    tableId     @b{(input)}  MIB table identifier
* @param    pRow        @b{(input)}  Row pointer of changed table row
* @param    oldStatus   @b{(input)}  Previous row status value
* @param    newStatus   @b{(input)}  Current row status value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Only "important" row status transitions are noted here, namely
*           moving into and out of the 'active' state and going into the
*           'destroy' state.
*
* @notes    This function assumes only valid transitions are signaled
*           here, so it does not verify that the newStatus is a proper
*           transition from the oldStatus.
*
* @notes    The oldStatus and newStatus parameters are permitted to be
*           "conditioned" externally-observable row status values.
*
* @end
*********************************************************************/
L7_RC_t dsmibRowEventCheck(dsmibTableId_t tableId, void *pRow,
                           dsmibRowStatus_t oldStatus,
                           dsmibRowStatus_t newStatus)
{
  L7_BOOL       signalEvent;
  L7_uint32     event = L7_INACTIVE;

  /* check inputs */
  if ((tableId == DSMIB_TABLE_ID_NONE) || (tableId >= DSMIB_TABLE_ID_TOTAL))
    return L7_FAILURE;
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  if (newStatus == oldStatus)                   /* not a transition */
    return L7_FAILURE;

  signalEvent = L7_FALSE;

  /* look at the new status to decide what to do */
  switch (newStatus)
  {
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE:
    signalEvent = L7_TRUE;
    event = L7_ACTIVE;
    break;

  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_IN_SERVICE:
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NOT_READY:
  case L7_USMDB_MIB_DIFFSERV_ROW_STATUS_DESTROY:
    if (oldStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    {
      signalEvent = L7_TRUE;
      event = L7_INACTIVE;
    }
    break;

  default:
    /* don't care about any other states */
    break;
  }

  /* check if the distiller needs to be signaled */
  if (signalEvent == L7_TRUE)
  {
    if (dsDistillerRowEventNotify(tableId, pRow, event) != L7_SUCCESS)
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display contents of DiffServ private MIB AVL control structures
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dsmibAvlCtrlShow(void)
{
  L7_uint32         treeHeapElemSize = (L7_uint32)sizeof(avlTreeTables_t);
  L7_uint32         allocBytes, totalBytes = 0;
  dsmibAvlCtrl_t    *pCtrl;
  L7_uint32         i;
  L7_uint32         msgLvlReqd;

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  if (pDsmibAvlCtrl == L7_NULLPTR)
  {
    DIFFSERV_PRT(msgLvlReqd, "\nPrivate MIB AVL control structure not allocated\n\n");
    return;
  }

  /*  each AVL tree */
  for (i = (L7_uint32)(DSMIB_TABLE_ID_NONE+1); i < DSMIB_TABLE_ID_TOTAL; i++)
  {
    /* NOTE: Display values for all tables, even if entryMaxNum is 0 */
    pCtrl = pDsmibAvlCtrl + i;

    /* calculate memory allocation amount */
    allocBytes = (treeHeapElemSize + pCtrl->entrySize) * pCtrl->entryMaxNum;
    totalBytes += allocBytes;

    DIFFSERV_PRT(msgLvlReqd, "\n%s Table:\n", dsmibPrvtTableIdStr[i]);
    DIFFSERV_PRT(msgLvlReqd, "  maxEnt=%u entSize=%u keySize=%u ",
                 pCtrl->entryMaxNum, pCtrl->entrySize, pCtrl->entryKeySize);
    DIFFSERV_PRT(msgLvlReqd, "pTreeHeap=0x%8.8x pDataHeap=0x%8.8x ",
                 (L7_uint32)pCtrl->pTreeHeap, (L7_uint32)pCtrl->pDataHeap);
    DIFFSERV_PRT(msgLvlReqd, "allocBytes=%u\n", allocBytes);
  }
  DIFFSERV_PRT(msgLvlReqd, "\nTotal private MIB AVL resource byte allocation:  %u\n\n", totalBytes);
}

/*********************************************************************
* @purpose  Display all private MIB tables
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsmibPrvtShowAll(void)
{
  dsmibGenStatusShow();
  dsmibClassTableShow();
  dsmibClassRuleTableShow();
  dsmibPolicyTableShow();
  dsmibPolicyInstTableShow();
  dsmibPolicyAttrTableShow();
  dsmibPolicyPerfInTableShow();
  dsmibPolicyPerfOutTableShow();
  dsmibServiceTableShow();
}

