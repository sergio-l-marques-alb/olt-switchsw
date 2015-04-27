/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   diffserv_stdmib.c
*
* @purpose    DiffServ standard MIB Internal functions
*
* @component  DiffServ
*
* @comments   none
*
* @create     05/07/2002
*
* @author     vbhaskar
* @end
*
*********************************************************************/
#include "l7_diffserv_include.h"
#include "usmdb_mib_diffserv_common.h"

/* global structure to represent ZERO_DOT_ZERO Row Pointer */
dsStdMibRowPtr_t dsStdMibZeroDotZero = { DSSTDMIB_TABLE_ID_NONE, 0, 0, 0 };

/* all necessary AVL tree anchors and mutex semaphores */
avlTree_t         dsStdMibAvlTree[DSSTDMIB_TABLE_ID_TOTAL]; /* AVL tree anchor nodes */
dsStdMibAvlCtrl_t *pDsStdMibAvlCtrl = L7_NULLPTR; /* AVL tree control data array */
void *            dsStdMibSemId = L7_NULLPTR;     /* std MIB tables index semaphore */

extern char *dsmibStdTableIdStr[];


/*********************************************************************
* @purpose  Initialize DIFFSERV Mib Support parameters
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t diffServStandardMibInit(void)
{
  /* create a semaphore for all standard MIB tables */
  dsStdMibSemId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (dsStdMibSemId == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: Error creating standard MIB semaphore\n", __FUNCTION__);
    return L7_FAILURE;
  }

#ifdef L7_INCLUDE_DIFFSERV_STD_MIB_SUPPORT
  /* initialize all of the AVL resources used for private MIB tables */
  if (dsStdMibAvlCreate() != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: Error creating standard MIB AVL trees\n", __FUNCTION__);
    return L7_FAILURE;
  }
#endif

  /* initialize each major MIB group */
  dsStdMibGroupInit();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Fini DIFFSERV Mib Support parameters
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
void diffServStandardMibFini(void)
{
  /*
   * there is nothing to do for each major MIB group
   * because it is just global data being initialized
   * which is the same as "un"initializing it
   */

  /* clear contents of all standard MIB table AVL trees */
  dsStdMibAvlDelete();

  /* delete the semaphore for all standard MIB tables */
  if (dsStdMibSemId != L7_NULLPTR)
  {
    if (osapiSemaDelete(dsStdMibSemId) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
              "%s: Error freeing standard MIB index table semaphore (0x%8.8x)\n",
              __FUNCTION__, (L7_uint32)dsStdMibSemId);
    }
  }
}

/*********************************************************************
* @purpose  Reset all DiffServ standard MIB contents to initial state
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Improper use of this function will adversely affect DiffServ
*           operation.
*
* @end
*********************************************************************/
L7_RC_t diffServStandardMibReset(void)
{
  /* clear contents of all standard MIB table AVL trees */
  dsStdMibAvlPurge();

  /* re-initialize all major standard MIB groups */
  dsStdMibGroupInit();

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize all AVL resources used by DiffServ standard MIB
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
L7_RC_t dsStdMibAvlCreate(void)
{
  L7_RC_t           rc = L7_SUCCESS;
  dsStdMibAvlCtrl_t *pCtrl;
  L7_uint32         i;

  /* allocate the AVL control data structure */
  pDsStdMibAvlCtrl = osapiMalloc(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, (L7_uint32)sizeof(*pDsStdMibAvlCtrl) * DSSTDMIB_TABLE_ID_TOTAL);
  if (pDsStdMibAvlCtrl == L7_NULLPTR)
    return L7_FAILURE;

  /* data to support Data Path Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_DATA_PATH;
  pCtrl->entryMaxNum = DSSTDMIB_DATAPATH_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsStdMibDataPathEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsStdMibDataPathKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* data to support Classifier Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_CLFR;
  pCtrl->entryMaxNum = DSSTDMIB_CLFR_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsStdMibClfrEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsStdMibClfrKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* data to support Classifier Element Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_CLFR_ELEMENT;
  pCtrl->entryMaxNum = DSSTDMIB_CLFR_ELEMENT_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsStdMibClfrElementEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsStdMibClfrElementKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* data to support Multi Field Classifier Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_MULTI_FIELD_CLFR;
  pCtrl->entryMaxNum = DSSTDMIB_MULTI_FIELD_CLFR_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsStdMibMultiFieldClfrEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsStdMibMultiFieldClfrKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* data to support Auxiliary Multi Field Classifier Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_AUX_MF_CLFR;
  pCtrl->entryMaxNum = DSSTDMIB_AUX_MF_CLFR_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsStdMibAuxMFClfrEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsStdMibAuxMFClfrKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* data to support Meter Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_METER;
  pCtrl->entryMaxNum = DSSTDMIB_METER_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsStdMibMeterEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsStdMibMeterKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* data to support Token Bucket Parameter Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_TB_PARAM;
  pCtrl->entryMaxNum = DSSTDMIB_TBPARAM_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsStdMibTBParamEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsStdMibTBParamKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* NOTE: Color Aware Table uses Meter Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_COLOR_AWARE;
  pCtrl->entryMaxNum = 0;                       /* no AVL tree needed */

  /* data to support Action Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_ACTION;
  pCtrl->entryMaxNum = DSSTDMIB_ACTION_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsStdMibActionEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsStdMibActionKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* NOTE: Mark COS Action Table does not use an AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_MARK_COS_ACT;
  pCtrl->entryMaxNum = 0;                       /* no AVL tree needed */

  /* NOTE: Mark COS as Secondary Cos (COS2) Action Table does not use an AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_MARK_COS_AS_COS2_ACT;
  pCtrl->entryMaxNum = 0;                       /* no AVL tree needed */

  /* NOTE: Mark COS2 Action Table does not use an AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_MARK_COS2_ACT;
  pCtrl->entryMaxNum = 0;                       /* no AVL tree needed */

  /* NOTE: Mark IP DSCP Action Table does not use an AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_MARK_IPDSCP_ACT;
  pCtrl->entryMaxNum = 0;                       /* no AVL tree needed */

  /* NOTE: Mark IP Precedence Action Table does not use an AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_MARK_IPPREC_ACT;
  pCtrl->entryMaxNum = 0;                       /* no AVL tree needed */

  /* data to support Count Action Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_COUNT_ACT;
  pCtrl->entryMaxNum = DSSTDMIB_COUNT_ACT_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsStdMibCountActEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsStdMibCountActKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* data to support Assign Queue Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_ASSIGN_QUEUE;
  pCtrl->entryMaxNum = DSSTDMIB_ASSIGN_QUEUE_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsStdMibAssignQueueEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsStdMibAssignQueueKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* data to support Redirect Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_REDIRECT;
  pCtrl->entryMaxNum = DSSTDMIB_REDIRECT_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsStdMibRedirectEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsStdMibRedirectKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* data to support Mirror Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_MIRROR;
  pCtrl->entryMaxNum = DSSTDMIB_MIRROR_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsStdMibMirrorEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsStdMibMirrorKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* data to support Algorithmic Drop Table AVL tree */
  pCtrl = pDsStdMibAvlCtrl + DSSTDMIB_TABLE_ID_ALG_DROP;
  pCtrl->entryMaxNum = DSSTDMIB_ALG_DROP_TABLE_SIZE_MAX;
  pCtrl->entrySize = (L7_uint32)sizeof(dsStdMibAlgDropEntryCtrl_t);
  pCtrl->entryKeySize = (L7_uint32)sizeof(dsStdMibAlgDropKey_t);
  pCtrl->pTreeHeap = L7_NULLPTR;
  pCtrl->pDataHeap = L7_NULLPTR;

  /* initialize the AVL tree table */
  memset(dsStdMibAvlTree, 0, sizeof(dsStdMibAvlTree));

  /* create each AVL tree */
  for (i = (L7_uint32)(DSSTDMIB_TABLE_ID_NONE+1); i < DSSTDMIB_TABLE_ID_TOTAL; i++)
  {
    if (dsStdMibAvlTreeCreate(&dsStdMibAvlTree[i], pDsStdMibAvlCtrl+i) != L7_SUCCESS)
    {
      rc = L7_FAILURE;
      break;
    }
  }

  /* clean up any partial allocations if failure occured during tree creation */
  if (rc != L7_SUCCESS)
  {
    dsStdMibAvlDelete();
  }

  return rc;
}

/*********************************************************************
* @purpose  Initialize all AVL resources used by DiffServ standard MIB
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
L7_RC_t dsStdMibAvlTreeCreate(avlTree_t *pTree, dsStdMibAvlCtrl_t *pCtrl)
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
*           standard MIB
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dsStdMibAvlDelete(void)
{
  L7_uint32     i;

  if (pDsStdMibAvlCtrl != L7_NULLPTR)
  {
    /* delete each AVL tree */
    for (i = (L7_uint32)(DSSTDMIB_TABLE_ID_NONE+1); i < DSSTDMIB_TABLE_ID_TOTAL; i++)
    {
      dsStdMibAvlTreeDelete(&dsStdMibAvlTree[i], pDsStdMibAvlCtrl+i);
    }

    /* deallocate the AVL control data structure */
    osapiFree(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pDsStdMibAvlCtrl);
    pDsStdMibAvlCtrl = L7_NULLPTR;
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
void dsStdMibAvlTreeDelete(avlTree_t *pTree, dsStdMibAvlCtrl_t *pCtrl)
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
* @purpose  Purge all AVL trees used by DiffServ standard MIB
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
void dsStdMibAvlPurge(void)
{
  dsStdMibAvlCtrl_t *pCtrl;
  L7_uint32         i;

  if (pDsStdMibAvlCtrl != L7_NULLPTR)
  {
    /* purge each AVL tree */
    for (i = (L7_uint32)(DSSTDMIB_TABLE_ID_NONE+1); i < DSSTDMIB_TABLE_ID_TOTAL; i++)
    {
      pCtrl = pDsStdMibAvlCtrl + i;

      if (pCtrl->entryMaxNum != 0)
      {
        avlPurgeAvlTree(&dsStdMibAvlTree[i], pCtrl->entryMaxNum);
      }
    }
  }
}

/*********************************************************************
* @purpose  Initialize all DiffServ standard MIB groups
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibGroupInit(void)
{
  /* initialize each major standard MIB group */

  dsStdMibClfrInit();

  dsStdMibClfrElemInit();

  dsStdMibMFClfrInit();

  dsStdMibAuxMFClfrInit();

  dsStdMibMeterInit();

  dsStdMibTBParamInit();

  dsStdMibColorAwareInit();

  dsStdMibActionInit();

  dsStdMibCountActInit();

  dsStdMibAssignQueueInit();

  dsStdMibRedirectInit();

  dsStdMibMirrorInit();

  dsStdMibAlgDropInit();

}


/*
==========================
==========================
==========================

   DATA PATH TABLE APIs

==========================
==========================
==========================
*/

/*********************************************************************
* @purpose  Create a Data Path for a given interface in a specific
*           direction
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Index values already in use
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServDataPathCreate(L7_uint32 intIfNum,
                               L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
  L7_RC_t           rc = L7_FAILURE;
  dsStdMibTableId_t tableId = DSSTDMIB_TABLE_ID_DATA_PATH;
  dsStdMibRowPtr_t  pathNext;
  dsStdMibRowPtr_t  pathNext2;
  L7_uint32         clfrId;
  L7_uint32         policyIndex;
  L7_uint32         policyInstIndex;
  L7_uint32         nextPolicyIndex;

  DS_TRACE_PT_CREATE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId,
                           intIfNum, ifDirection, 0,
                           (L7_uchar8)L7_TRUE);

  do
  {
    /* check whether data path is already created on the given interface and direction */
    if(diffServDataPathGet(intIfNum, ifDirection) == L7_SUCCESS)
    {
      rc = L7_ERROR;
      break;
    }

    /* acquire the semaphore */
    DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

    pathNext = DSSTDMIB_ZERO_DOT_ZERO;

    /* create one classifier for all class instances attached to a policy */
    if (diffServClfrCreate(&pathNext) == L7_SUCCESS)
    {
      clfrId = pathNext.rowIndex1;

      /* get policy index */
      if (diffServServiceObjectGet(intIfNum, ifDirection,
                                   L7_DIFFSERV_SERVICE_POLICY_INDEX,
                                   &policyIndex) == L7_SUCCESS)
      {

        /* for all policy-class instances attached to this policy */
        policyInstIndex = 0;
        while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex,
                                          &nextPolicyIndex, &policyInstIndex)
                == L7_SUCCESS) &&
               (nextPolicyIndex == policyIndex))
        {
          /* this is the working row ptr to the next data path element that is used
           * to fill in the 'next' pointer in an element being created
           *
           * each instance starts with a null pathNext row ptr
           */
          pathNext2 = DSSTDMIB_ZERO_DOT_ZERO;

          if ((rc = dsStdMibPathInstanceCreate(policyIndex, policyInstIndex, intIfNum, ifDirection, clfrId, &pathNext2))
              != L7_SUCCESS)
          {
            break;
          }
        } /* endwhile - instIndex */

        /* Data Path element */
        if (rc == L7_SUCCESS)
        {
          rc = dsStdMibDataPathElemCreate(intIfNum, ifDirection, &pathNext);
        }
      }
    }

    /* release the semaphore */
    DIFFSERV_SEMA_GIVE(dsStdMibSemId);

  } while (0);

  DS_TRACE_PT_CREATE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId,
                          intIfNum, ifDirection, 0,
                          0, (L7_uchar8)rc,
                          (L7_uchar8)L7_TRUE);

  return rc;
}

/*********************************************************************
* @purpose  Create Data Path for a data path instance
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrIndex  @b{(input)} Policy Attr Index
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    ifDirection      @b{(input)}  Interface direction
* @param    clfrId           @b{(input)} Classifier identifier index
* @param    pPathNext        @b{(inout)} Ptr to next elem in data path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibPathInstanceCreate(L7_uint32 policyIndex,
                                   L7_uint32 policyInstIndex,
                                   L7_uint32 intIfNum,
                                   L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                   L7_uint32 clfrId,
                                   dsStdMibRowPtr_t *pPathNext)
{
  L7_uint32                            policyAttrIndex;


  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* Datapath elements:
   *
   *   Data Path ::= Classifier + Counter + Dropper
   * -or-
   *   Data Path ::= Classifier + Counter [+ (Policer | Marker) [+AssignQueue] [+ (Mirror | Redirect)]
   *
   *   Classifier ::= Clfr + ClfrElements + AuxMfClfr
   *   Counter ::= OfferedCtr
   *   Policer ::= Simple | SingleRate | TwoRate
   *   Marker ::= MarkCos | MarkCos2 | MarkDscp | MarkPrec
   *
   *
   * NOTE:  Must look for data path elements in REVERSE ORDER of the sequence
   *        shown so that the pPathNext pointer can be filled in properly as
   *        each element is created.  The common Clfr was already created.
   */

  /* Redirect element */
  if (dsStdMibPathAttrExists(policyIndex, policyInstIndex,
                             L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_REDIRECT,
                             &policyAttrIndex) == L7_SUCCESS)
  {
    if (diffServPolicyAttrTargetIntfIsUsable(policyIndex, policyInstIndex, policyAttrIndex,
                                             L7_DIFFSERV_POLICY_ATTR_STMT_REDIRECT_INTF)
        == L7_TRUE)
    {
      if (dsStdMibRedirectElemCreate(policyIndex, policyInstIndex,
                                     policyAttrIndex, intIfNum,
                                     pPathNext) != L7_SUCCESS)
        return L7_FAILURE;
    }
  }

  /* Mirror element */
  if (dsStdMibPathAttrExists(policyIndex, policyInstIndex,
                             L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MIRROR,
                             &policyAttrIndex) == L7_SUCCESS)
  {
    if (diffServPolicyAttrTargetIntfIsUsable(policyIndex, policyInstIndex, policyAttrIndex,
                                             L7_DIFFSERV_POLICY_ATTR_STMT_MIRROR_INTF)
        == L7_TRUE)
    {
      if (dsStdMibMirrorElemCreate(policyIndex, policyInstIndex,
                                   policyAttrIndex, intIfNum,
                                   pPathNext) != L7_SUCCESS)
        return L7_FAILURE;
    }
  }

  /* Assign Queue element */
  if (dsStdMibPathAttrExists(policyIndex, policyInstIndex,
                             L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_ASSIGN_QUEUE,
                             &policyAttrIndex) == L7_SUCCESS)
  {
    if (dsStdMibAssignQueueElemCreate(policyIndex, policyInstIndex,
                                      policyAttrIndex, intIfNum,
                                      pPathNext) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* NOTE: only one of the following is allowed: dropper, marker, policer */

  /* Drop element */
  if (dsStdMibPathAttrExists(policyIndex, policyInstIndex,
                             L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_DROP,
                             &policyAttrIndex) == L7_SUCCESS)
  {
    if (dsStdMibInDropElemCreate(policyIndex, policyInstIndex,
                                 policyAttrIndex, intIfNum,
                                 pPathNext) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* Marker element (any type) */
  else if (dsStdMibMarkerAttrExists(policyIndex, policyInstIndex,
                                    &policyAttrIndex) == L7_SUCCESS)
  {
    if (dsStdMibMarkerElemCreate(policyIndex, policyInstIndex,
                                 policyAttrIndex, intIfNum,
                                 pPathNext) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* Policer element (any type) */
  else if (dsStdMibPolicerAttrExists(policyIndex, policyInstIndex,
                                     &policyAttrIndex) == L7_SUCCESS)
  {
    if (dsStdMibPolicerElemCreate(policyIndex, policyInstIndex,
                                  policyAttrIndex, intIfNum,
                                  pPathNext) != L7_SUCCESS)
      return L7_FAILURE;
  }

  /* Counter element (always exists) */
  switch (ifDirection)
  {
  case L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN:
    if (dsStdMibCounterElemCreate(policyIndex, policyInstIndex, intIfNum,
                                  DSSTDMIB_COUNT_TYPE_IN_OFFERED,
                                  pPathNext) != L7_SUCCESS)
      return L7_FAILURE;
    break;

  case L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT:
    if (dsStdMibCounterElemCreate(policyIndex, policyInstIndex, intIfNum,
                                  DSSTDMIB_COUNT_TYPE_OUT_OFFERED,
                                  pPathNext) != L7_SUCCESS)
      return L7_FAILURE;
    break;

  default:
    return L7_FAILURE;
    /*PASSTHRU*/
  }

  /* Classifier element
   *
   * NOTE:  This in turn creates Aux. MFClfr element(s)
   */
  if (dsStdMibClfrElemCreate(policyIndex, policyInstIndex, clfrId,
                             pPathNext) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Check if any marking attribute exists for this policy instance
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    pPolicyAttrIndex @b{(output)} Pointer to Policy Attr Index
*
* @returns  L7_SUCCESS    Marker attr exists
* @returns  L7_NOT_EXIST  Marker attr not found
* @returns  L7_FAILURE    Processing error
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibMarkerAttrExists(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 *pPolicyAttrIndex)
{
  L7_RC_t               rc = L7_NOT_EXIST;
  L7_uint32             policyAttrIndex;

  DIFFSERV_NULLPTR_CHECK(pPolicyAttrIndex);

  *pPolicyAttrIndex = 0;                        /* init output value */

  /* NOTE: At most one marking attribute type is contained in an instance,
   *       so just return with the first one found.
   */

  do
  {
    /* Mark COS */
    if (dsStdMibPathAttrExists(policyIndex, policyInstIndex,
                               L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL,
                               &policyAttrIndex) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }

    /* Mark COS as Secondary Cos (Cos2) */
    if (dsStdMibPathAttrExists(policyIndex, policyInstIndex,
                               L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2,
                               &policyAttrIndex) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }

    /* Mark COS2 */
    if (dsStdMibPathAttrExists(policyIndex, policyInstIndex,
                               L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL,
                               &policyAttrIndex) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }

    /* Mark IP DSCP */
    if (dsStdMibPathAttrExists(policyIndex, policyInstIndex,
                               L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL,
                               &policyAttrIndex) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }

    /* Mark IP Precedence */
    if (dsStdMibPathAttrExists(policyIndex, policyInstIndex,
                               L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL,
                               &policyAttrIndex) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }

  } while (0);

  if (rc == L7_SUCCESS)
    *pPolicyAttrIndex = policyAttrIndex;

  return rc;
}

/*********************************************************************
* @purpose  Check if any policing attribute exists for this policy instance
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    pPolicyAttrIndex @b{(output)} Pointer to Policy Attr Index
*
* @returns  L7_SUCCESS    Policer attr exists
* @returns  L7_NOT_EXIST  Policer attr not found
* @returns  L7_FAILURE    Processing error
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibPolicerAttrExists(L7_uint32 policyIndex,
                                  L7_uint32 policyInstIndex,
                                  L7_uint32 *pPolicyAttrIndex)
{
  L7_RC_t       rc = L7_NOT_EXIST;
  L7_uint32     policyAttrIndex;

  DIFFSERV_NULLPTR_CHECK(pPolicyAttrIndex);

  *pPolicyAttrIndex = 0;                        /* init output value */

  /* NOTE: At most one policing attribute type is contained in an instance,
   *       so just return with the first one found.
   */

  do
  {
    /* Police Simple */
    if (dsStdMibPathAttrExists(policyIndex, policyInstIndex,
                               L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE,
                               &policyAttrIndex) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }

    /* Police Single-Rate */
    if (dsStdMibPathAttrExists(policyIndex, policyInstIndex,
                               L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE,
                               &policyAttrIndex) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }

    /* Police Two-Rate */
    if (dsStdMibPathAttrExists(policyIndex, policyInstIndex,
                               L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE,
                               &policyAttrIndex) == L7_SUCCESS)
    {
      rc = L7_SUCCESS;
      break;
    }

  } while (0);

  if (rc == L7_SUCCESS)
    *pPolicyAttrIndex = policyAttrIndex;

  return rc;
}

/*********************************************************************
* @purpose  Check if the specified attribute exists for this policy instance
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrType   @b{(input)} Policy Attr Type
* @param    pPolicyAttrIndex @b{(output)} Pointer to Policy Attr Index
*
* @returns  L7_SUCCESS    Specified policy attr exists
* @returns  L7_NOT_EXIST  Specified policy attr not found
* @returns  L7_FAILURE    Processing error
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibPathAttrExists(L7_uint32 policyIndex,
                               L7_uint32 policyInstIndex,
                               dsmibPolicyAttrType_t policyAttrType,
                               L7_uint32 *pPolicyAttrIndex)
{
  dsmibPolicyAttrType_t attrType;
  L7_uint32             policyAttrIndex;
  L7_uint32             nextPolicyIndex, nextPolicyInstIndex;

  DIFFSERV_NULLPTR_CHECK(pPolicyAttrIndex);

  *pPolicyAttrIndex = 0;                        /* init output value */

  /* scan through all attributes attached to this instance */
  policyAttrIndex = 0;
  while ((diffServPolicyAttrGetNext(policyIndex, policyInstIndex, policyAttrIndex,
                                    &nextPolicyIndex, &nextPolicyInstIndex,
                                    &policyAttrIndex) == L7_SUCCESS)
         && (nextPolicyIndex == policyIndex)
         && (nextPolicyInstIndex == policyInstIndex))
  {
    if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                    L7_DIFFSERV_POLICY_ATTR_STMT_ENTRY_TYPE,
                                    &attrType) != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    if (attrType == policyAttrType)
    {
      *pPolicyAttrIndex = policyAttrIndex;
      return L7_SUCCESS;
    }

  } /* endwhile */

  return L7_NOT_EXIST;
}

/*********************************************************************
* @purpose  Create a Redirect data path element
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrIndex  @b{(input)} Policy Attr Index
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    pPathNext        @b{(inout)} Ptr to next elem in data path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibRedirectElemCreate(L7_uint32 policyIndex,
                                   L7_uint32 policyInstIndex,
                                   L7_uint32 policyAttrIndex,
                                   L7_uint32 intIfNum,
                                   dsStdMibRowPtr_t *pPathNext)
{
  L7_uint32         elemId;
  dsStdMibRowPtr_t  specific;                   /* specific data path element */

  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* get redirect interface number from private MIB */
  if (diffServRedirectCreate(policyIndex, policyInstIndex, policyAttrIndex,
                             &elemId) != L7_SUCCESS)
  {
    return  L7_FAILURE;
  }

  /* specific row ptr is to individual table entry */
  DSSTDMIB_ROW_PTR_SET(&specific, DSSTDMIB_TABLE_ID_REDIRECT,
                       elemId, 0, 0);

  /* insert in Action Avl tree and update path next ptr */
  if (diffServActionCreate(intIfNum, &specific, pPathNext) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a Mirror data path element
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrIndex  @b{(input)} Policy Attr Index
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    pPathNext        @b{(inout)} Ptr to next elem in data path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibMirrorElemCreate(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex,
                                 L7_uint32 intIfNum,
                                 dsStdMibRowPtr_t *pPathNext)
{
  L7_uint32         elemId;
  dsStdMibRowPtr_t  specific;                   /* specific data path element */

  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* get mirror interface number from private MIB */
  if (diffServMirrorCreate(policyIndex, policyInstIndex, policyAttrIndex,
                           &elemId) != L7_SUCCESS)
  {
    return  L7_FAILURE;
  }

  /* specific row ptr is to individual table entry */
  DSSTDMIB_ROW_PTR_SET(&specific, DSSTDMIB_TABLE_ID_MIRROR,
                       elemId, 0, 0);

  /* insert in Action Avl tree and update path next ptr */
  if (diffServActionCreate(intIfNum, &specific, pPathNext) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create an Assign Queue data path element
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrIndex  @b{(input)} Policy Attr Index
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    pPathNext        @b{(inout)} Ptr to next elem in data path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibAssignQueueElemCreate(L7_uint32 policyIndex,
                                      L7_uint32 policyInstIndex,
                                      L7_uint32 policyAttrIndex,
                                      L7_uint32 intIfNum,
                                      dsStdMibRowPtr_t *pPathNext)
{
  L7_uint32         elemId;
  dsStdMibRowPtr_t  specific;                   /* specific data path element */

  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* get assign queue id from private MIB */
  if (diffServAssignQueueCreate(policyIndex, policyInstIndex, policyAttrIndex,
                                &elemId) != L7_SUCCESS)
  {
    return  L7_FAILURE;
  }

  /* specific row ptr is to individual table entry */
  DSSTDMIB_ROW_PTR_SET(&specific, DSSTDMIB_TABLE_ID_ASSIGN_QUEUE,
                       elemId, 0, 0);

  /* insert in Action Avl tree and update path next ptr */
  if (diffServActionCreate(intIfNum, &specific, pPathNext) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create an inbound Dropper data path element
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrIndex  @b{(input)} Policy Attr Index
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    pPathNext        @b{(inout)} Ptr to next elem in data path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibInDropElemCreate(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex,
                                 L7_uint32 intIfNum,
                                 dsStdMibRowPtr_t *pPathNext)
{
  dsStdMibRowPtr_t  qMeasure, specific;

  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* there is nothing to get from private MIB */

  qMeasure= DSSTDMIB_ZERO_DOT_ZERO;
  specific= DSSTDMIB_ZERO_DOT_ZERO;

  if (diffServAlgDropCreate(policyIndex, policyInstIndex,
                            policyAttrIndex, intIfNum,
                            L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_ALWAYS_DROP,
                            &qMeasure, &specific,
                            pPathNext) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a Marker data path element
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrIndex  @b{(input)} Policy Attr Index
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    pPathNext        @b{(inout)} Ptr to next elem in data path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibMarkerElemCreate(L7_uint32 policyIndex,
                                 L7_uint32 policyInstIndex,
                                 L7_uint32 policyAttrIndex,
                                 L7_uint32 intIfNum,
                                 dsStdMibRowPtr_t *pPathNext)
{
  dsmibPolicyAttrType_t                   attrType;
  L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t  objectId;
  dsStdMibTableId_t                       tableId;
  L7_uint32                               markVal;
  dsStdMibRowPtr_t                        specific;

  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* get marker type from private MIB policy attribute */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                  L7_DIFFSERV_POLICY_ATTR_STMT_ENTRY_TYPE,
                                  &attrType) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  switch (attrType)
  {
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COSVAL:
    objectId = L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COSVAL;
    tableId = DSSTDMIB_TABLE_ID_MARK_COS_ACT;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS_AS_COS2:
    objectId = L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COS_AS_COS2;
    tableId = DSSTDMIB_TABLE_ID_MARK_COS_AS_COS2_ACT;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_COS2VAL:
    objectId = L7_DIFFSERV_POLICY_ATTR_STMT_MARK_COS2VAL;
    tableId = DSSTDMIB_TABLE_ID_MARK_COS2_ACT;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPDSCPVAL:
    objectId = L7_DIFFSERV_POLICY_ATTR_STMT_MARK_IPDSCPVAL;
    tableId = DSSTDMIB_TABLE_ID_MARK_IPDSCP_ACT;
    break;

  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_MARK_IPPRECEDENCEVAL:
    objectId = L7_DIFFSERV_POLICY_ATTR_STMT_MARK_IPPRECEDENCEVAL;
    tableId = DSSTDMIB_TABLE_ID_MARK_IPPREC_ACT;
    break;

  default:
    return L7_FAILURE;
    /*PASSTHRU*/
  }

  /* get mark value from private MIB */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex,
                                  policyAttrIndex, objectId,
                                  &markVal) != L7_SUCCESS)
  {
    return  L7_FAILURE;
  }

  /* marker specific row ptr is to individual mark table entry */
  DSSTDMIB_PATH_NEXT_UPDATE(&specific, tableId, markVal, 0, 0);

  /* insert in Action Avl tree */
  if (diffServActionCreate(intIfNum, &specific, pPathNext) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a Policer data path element
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrIndex  @b{(input)} Policy Attr Index
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    pPathNext        @b{(inout)} Ptr to next elem in data path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The pPathNext parm represents the next ptr used by the
*           policer action element created here (e.g. marker).  As
*           an output value, pPathNext is updated with the row ptr
*           of the policer's first meter element.
*
* @end
*********************************************************************/
L7_RC_t dsStdMibPolicerElemCreate(L7_uint32 policyIndex,
                                  L7_uint32 policyInstIndex,
                                  L7_uint32 policyAttrIndex,
                                  L7_uint32 intIfNum,
                                  dsStdMibRowPtr_t *pPathNext)
{
  dsmibPolicyAttrType_t                   attrType;
  DSSTDMIB_ACTION_TYPE_t                  policingType;
  L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t  objectAct, objectVal;
  dsStdMibRowPtr_t                        succeedNext, failNext, specific;
  dsStdMibRowPtr_t                        meterNext;
  L7_uint32                               meterId;


  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* get marker type from private MIB policy attribute */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex, policyAttrIndex,
                                  L7_DIFFSERV_POLICY_ATTR_STMT_ENTRY_TYPE,
                                  &attrType) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  switch (attrType)
  {
  /* create simple policer:
   *
   *        |---SucceedNext-->ConformAct
   * Meter1-|---FailNext----->NonconformAct
   *      | |---Specific->TBParam
   *      |
   *      +->ColorAware1
   *
   *   ConformAct-->(pathNextElem)
   *   NonconformAct-->(pathNextElem)
   *
   */
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SIMPLE:
    {
      policingType = DSSTDMIB_ACTION_TYPE_POLICE_SIMPLE;

      /* get conform next element */
      objectAct = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CONFORM_ACT;
      objectVal = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_CONFORM_VAL;
      succeedNext = *pPathNext;
      if(dsStdMibPoliceNextElementCreate(policyIndex, policyInstIndex,
                                         policyAttrIndex, intIfNum,
                                         objectAct, objectVal,
                                         &succeedNext) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* get non-conform next element */
      objectAct = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_NONCONFORM_ACT;
      objectVal = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SIMPLE_NONCONFORM_VAL;
      failNext = *pPathNext;
      if(dsStdMibPoliceNextElementCreate(policyIndex, policyInstIndex,
                                         policyAttrIndex, intIfNum,
                                         objectAct, objectVal,
                                         &failNext) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* create TB param entry */
      specific = DSSTDMIB_ZERO_DOT_ZERO;
      if (diffServTBParamCreate(policyIndex, policyInstIndex, policyAttrIndex,
                                policingType,
                                DSSTDMIB_RATE_TYPE_NONE,
                                &specific) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
    break;


    /* Create Single Rate policer:
     *
     *        |---SucceedNext-->ConformAct  |---SucceedNext-->ExceedAct
     * Meter1-|---FailNext----->Meter2------|---FailNext----->NonconformAct
     *      | |---Specific->TBParam1 |      |---Specific->TBParam2
     *      |                        |
     *      +->ColorAware1           +->ColorAware2
     *
     *   ConformAct-->(pathNextElem)
     *   ExceedAct-->(pathNextElem)
     *   NonconformAct-->(pathNextElem)
     *
     */
    case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_SINGLERATE:
    {
      policingType = DSSTDMIB_ACTION_TYPE_POLICE_SINGLERATE;

      /* get exceed next element (for meter2) */
      objectAct = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EXCEED_ACT;
      objectVal = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_EXCEED_VAL;
      succeedNext = *pPathNext;
      if(dsStdMibPoliceNextElementCreate(policyIndex, policyInstIndex,
                                         policyAttrIndex, intIfNum,
                                         objectAct, objectVal,
                                         &succeedNext) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* get non-conform next element (for meter2) */
      objectAct = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_NONCONFORM_ACT;
      objectVal = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_NONCONFORM_VAL;
      failNext = *pPathNext;
      if(dsStdMibPoliceNextElementCreate(policyIndex, policyInstIndex,
                                         policyAttrIndex, intIfNum,
                                         objectAct, objectVal,
                                         &failNext) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* create TB param2 entry (for meter2) */
      specific = DSSTDMIB_ZERO_DOT_ZERO;
      if (diffServTBParamCreate(policyIndex, policyInstIndex,
                                policyAttrIndex,
                                policingType,
                                DSSTDMIB_RATE_TYPE_EXCESS,
                                &specific) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* create meter2 */
      meterNext = DSSTDMIB_ZERO_DOT_ZERO;       /* use temp var for path next */
      if (diffServMeterCreate(&succeedNext, &failNext, &specific,
                              &meterNext) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* create color aware2 (augments meter2) */
      meterId = meterNext.rowIndex1;
      if (diffServColorAwareCreate(policyIndex, policyInstIndex,
                                   policyAttrIndex,
                                   policingType,
                                   L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_EXCEED,
                                   meterId) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* get conform next element (for meter1) */
      objectAct = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CONFORM_ACT;
      objectVal = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_SINGLERATE_CONFORM_VAL;
      succeedNext = *pPathNext;
      if(dsStdMibPoliceNextElementCreate(policyIndex, policyInstIndex,
                                         policyAttrIndex, intIfNum,
                                         objectAct, objectVal,
                                         &succeedNext) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* set failNext element (for meter1)
       *
       * NOTE:  Using meter2 row ptr obtained above.
       */
      failNext = meterNext;

      /* create TB param1 entry (for meter1) */
      specific = DSSTDMIB_ZERO_DOT_ZERO;
      if (diffServTBParamCreate(policyIndex, policyInstIndex,
                                policyAttrIndex,
                                policingType,
                                DSSTDMIB_RATE_TYPE_COMMITTED,
                                &specific) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
    break;

    /* Create Two Rate policer:
     *
     *        |---SucceedNext-->ConformAct  |---SucceedNext-->ExceedAct
     * Meter1-|---FailNext----->Meter2------|---FailNext----->NonconformAct
     *      | |---Specific->TBParam1 |      |---Specific->TBParam2
     *      |                        |
     *      +->ColorAware1           +->ColorAware2
     *
     *   ConformAct-->(pathNextElem)
     *   ExceedAct-->(pathNextElem)
     *   NonconformAct-->(pathNextElem)
     *
     */
  case L7_USMDB_MIB_DIFFSERV_POLICY_ATTR_TYPE_POLICE_TWORATE:
    {
      policingType = DSSTDMIB_ACTION_TYPE_POLICE_TWORATE;

      /* get exceed next element (for meter2) */
      objectAct = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_EXCEED_ACT;
      objectVal = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_EXCEED_VAL;
      succeedNext = *pPathNext;
      if(dsStdMibPoliceNextElementCreate(policyIndex, policyInstIndex,
                                         policyAttrIndex, intIfNum,
                                         objectAct, objectVal,
                                         &succeedNext) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* get non-conform next element (for meter2) */
      objectAct = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_NONCONFORM_ACT;
      objectVal = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_NONCONFORM_VAL;
      failNext = *pPathNext;
      if(dsStdMibPoliceNextElementCreate(policyIndex, policyInstIndex,
                                         policyAttrIndex, intIfNum,
                                         objectAct, objectVal,
                                         &failNext) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* create TB param2 entry (for meter2) */
      specific = DSSTDMIB_ZERO_DOT_ZERO;
      if (diffServTBParamCreate(policyIndex, policyInstIndex,
                                policyAttrIndex,
                                policingType,
                                DSSTDMIB_RATE_TYPE_PEAK,
                                &specific) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* create meter2 */
      meterNext = DSSTDMIB_ZERO_DOT_ZERO;       /* use temp var for path next */
      if (diffServMeterCreate(&succeedNext, &failNext, &specific,
                              &meterNext) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* create color aware2 (augments meter2) */
      meterId = meterNext.rowIndex1;
      if (diffServColorAwareCreate(policyIndex, policyInstIndex,
                                   policyAttrIndex,
                                   policingType,
                                   L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_EXCEED,
                                   meterId) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* get conform next element (for meter1) */
      objectAct = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CONFORM_ACT;
      objectVal = L7_DIFFSERV_POLICY_ATTR_STMT_POLICE_TWORATE_CONFORM_VAL;
      succeedNext = *pPathNext;
      if(dsStdMibPoliceNextElementCreate(policyIndex, policyInstIndex,
                                         policyAttrIndex, intIfNum,
                                         objectAct, objectVal,
                                         &succeedNext) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* set failNext element (for meter1)
       *
       * NOTE:  Using meter2 row ptr obtained above.
       */
      failNext = meterNext;

      /* create TB param1 entry (for meter1) */
      specific = DSSTDMIB_ZERO_DOT_ZERO;
      if (diffServTBParamCreate(policyIndex, policyInstIndex,
                                policyAttrIndex,
                                policingType,
                                DSSTDMIB_RATE_TYPE_COMMITTED,
                                &specific) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
    break;

  default:
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch attr type */

  /* create meter1 */
  if (diffServMeterCreate(&succeedNext, &failNext, &specific,
                          pPathNext) != L7_SUCCESS)
  {
    return  L7_FAILURE;
  }

  /* create color aware1 (augments meter1) */
  meterId = pPathNext->rowIndex1;
  if (diffServColorAwareCreate(policyIndex, policyInstIndex,
                               policyAttrIndex,
                               policingType,
                               L7_USMDB_MIB_DIFFSERV_COLOR_LEVEL_CONFORM,
                               meterId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create and get police next element
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    policyAttrIndex  @b{(input)} Policy Attr Index
* @param    intIfNum         @b{(input)} Internal Interface Number
* @param    actionOid        @b{(input)} Police action Object Id
* @param    valueOid         @b{(input)} conform/exceed/nonconform
*                                          value Object Id
* @param    pMeterNext       @b{(inout)} Ptr to meter next elem location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The pMeterNext parm as an input value represents the next
*           path element for the policing action element created here
*           (but is ignored for 'drop' action).  As an output parm, it
*           is set to the new action element row ptr.
*
* @end
*********************************************************************/
L7_RC_t dsStdMibPoliceNextElementCreate(L7_uint32 policyIndex,
                                        L7_uint32 policyInstIndex,
                                        L7_uint32 policyAttrIndex,
                                        L7_uint32 intIfNum,
                                        L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t actionOid,
                                        L7_DIFFSERV_POLICY_ATTR_TABLE_OBJECT_t valueOid,
                                        dsStdMibRowPtr_t *pMeterNext)
{
  dsmibPoliceAct_t  policeAct;
  dsStdMibRowPtr_t  qMeasure, specific, tempNext;
  dsStdMibTableId_t tableId;
  L7_uint32         markVal;

  /* get policing action */
  if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex,
                                  policyAttrIndex, actionOid,
                                  &policeAct) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  switch (policeAct)
  {
  /* create a dropper */
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_DROP:
    {
      qMeasure = DSSTDMIB_ZERO_DOT_ZERO;
      specific = DSSTDMIB_ZERO_DOT_ZERO;
      tempNext = DSSTDMIB_ZERO_DOT_ZERO;

      /* NOTE:  Need to pass in a null meter next row ptr for the always
       *        drop case, but then copy the algorithmic dropper output
       *        value to the caller's pMeterNext.  The input path next value
       *        is ignored in this case.
       */
      if (diffServAlgDropCreate(policyIndex, policyInstIndex,
                                policyAttrIndex, intIfNum,
                                L7_USMDB_MIB_DIFFSERV_ALG_DROP_TYPE_ALWAYS_DROP,
                                &qMeasure, &specific,
                                &tempNext) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }

      /* output the Alg Dropper row ptr as the new next data path elem */
      *pMeterNext = tempNext;
    }
    break;

  /* create a marker */
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
    {
      switch (policeAct)
      {
      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS:
        tableId = DSSTDMIB_TABLE_ID_MARK_COS_ACT;
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS_AS_COS2:
        tableId = DSSTDMIB_TABLE_ID_MARK_COS_AS_COS2_ACT;
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKCOS2:
        tableId = DSSTDMIB_TABLE_ID_MARK_COS2_ACT;
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKDSCP:
        tableId = DSSTDMIB_TABLE_ID_MARK_IPDSCP_ACT;
        break;

      case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_MARKPREC:
        tableId = DSSTDMIB_TABLE_ID_MARK_IPPREC_ACT;
        break;

      default:
        return L7_FAILURE;
        /*PASSTHRU*/
      }

      /* get mark value from private MIB */
      if (diffServPolicyAttrObjectGet(policyIndex, policyInstIndex,
                                      policyAttrIndex, valueOid,
                                      &markVal) != L7_SUCCESS)
      {
        return  L7_FAILURE;
      }

      /* marker specific row ptr is to individual mark table entry */
      DSSTDMIB_PATH_NEXT_UPDATE(&specific, tableId, markVal, 0, 0);

      /* insert in Action Avl tree */
      if (diffServActionCreate(intIfNum, &specific, pMeterNext) != L7_SUCCESS)
      {
        return L7_FAILURE;
      }
    }
    break;

  /* a `send' action creates no new element */
  case L7_USMDB_MIB_DIFFSERV_POLICE_ACT_SEND:
    /* NOTE:  The caller sees the input pMeterNext element as the output */
    break;

  default:
    return L7_FAILURE;
    /*PASSTHRU*/
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a counter element
*
* @param    policyIndex      @b{(input)}  Policy Index
* @param    policyInstIndex  @b{(input)}  Policy Inst Index
* @param    intIfNum         @b{(input)}  Internal Interface Number
* @param    countType        @b{(input)}  Count Type
* @param    pPathNext        @b{(inout)}  Ptr to next elem in data path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The interface direction is implied by the name of the countType.
*
* @end
*********************************************************************/
L7_RC_t dsStdMibCounterElemCreate(L7_uint32 policyIndex, L7_uint32 policyInstIndex,
                                  L7_uint32 intIfNum,
                                  DSSTDMIB_COUNT_TYPE_t countType,
                                  dsStdMibRowPtr_t *pPathNext)
{
  L7_uint32           countActId;
  dsStdMibRowPtr_t    specific;


  DIFFSERV_NULLPTR_CHECK(pPathNext);

  if (diffServCountActCreate(policyIndex, policyInstIndex,
                             intIfNum, countType,
                             &countActId) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  /* specific row ptr is to individual table entry */
  DSSTDMIB_ROW_PTR_SET(&specific, DSSTDMIB_TABLE_ID_COUNT_ACT,
                       countActId, 0, 0);

  /* insert in Action Avl tree and update path next ptr */
  if (diffServActionCreate(intIfNum, &specific, pPathNext) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create classifier elements for a given class
*
* @param    policyIndex      @b{(input)} Policy Index
* @param    policyInstIndex  @b{(input)} Policy Inst Index
* @param    clfrId           @b{(input)} Classifier identifier
* @param    pPathNext        @b{(inout)} Ptr to next elem in data path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibClfrElemCreate(L7_uint32 policyIndex, L7_uint32 policyInstIndex,
                               L7_uint32 clfrId, dsStdMibRowPtr_t *pPathNext)
{
  L7_uint32         classIndex;


  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* get classIndex */
  if (diffServPolicyInstObjectGet(policyIndex, policyInstIndex,
                                  L7_DIFFSERV_POLICY_INST_CLASS_INDEX,
                                  (void *)&classIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

   /* Create entries in Classifier Element Table*/
  if (dsStdMibClfrElementsCreate(classIndex, clfrId, pPathNext) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create new rows in the Classifier Element Table and
*           Aux MF Clfr table based on class type
*
* @param    classIndex  @b{(input)} Class Index
* @param    clfrId      @b{(input)} Classifier identifier index
* @param    pPathNext   @b{(input)} Ptr to next elem in data path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Creates multiple rows based on class Type and Exclude Flag
*
* @end
*********************************************************************/
L7_RC_t dsStdMibClfrElementsCreate(L7_uint32 classIndex,
                                   L7_uint32 clfrId,
                                   dsStdMibRowPtr_t *pPathNext)
{
  dsmibClassType_t      classType;
  L7_uint32             refNestLvl;
  L7_uint32             nextClassIndex;
  L7_uint32             classRuleIndex;
  dsmibClassRuleType_t  classRuleType;
  dsmibTruthValue_t     excludeFlag;
  L7_uint32             refClassIndex;
  dsStdMibRowPtr_t      tempNext;
  L7_uint32             prevArid, arid;
  L7_BOOL               needElemCreate, aridChanged;
  struct
  {
    L7_uint32           classIndex;
    L7_uint32           classRuleIndex;
  } refSaveStack[DSMIB_CLASS_REF_CHAIN_DEPTH_MAX];

  L7_BOOL                      generalInUse;
  dsmibTruthValue_t            generalExcludeFlag;
  dsStdMibAuxMFClfrEntryCtrl_t auxMFClfrGeneral;
  dsStdMibAuxMFClfrEntryCtrl_t auxMFClfrLocal;
  dsStdMibAuxMFClfrEntryCtrl_t *pAuxInfo;


  /* clear out the save stack */
  memset(refSaveStack, 0, sizeof(refSaveStack));

  /* initialize a "general" working row structure that is used to assemble
   * fields from multiple DiffServ class rules (i.e, AND condition)
   */
  dsStdMibAuxMFClfSetToDefaults(&auxMFClfrGeneral);
  generalInUse = L7_FALSE;
  generalExcludeFlag = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE;

  /* get the class type */
  if (diffServClassObjectGet(classIndex, L7_DIFFSERV_CLASS_TYPE,
                             (void *)&classType) != L7_SUCCESS)
    return L7_FAILURE;

  refNestLvl = 1;                               /* init for proper loop entry */
  classRuleIndex = 0;                           /* start with first class rule*/
  prevArid = arid = DSMIB_CLASS_RULE_ARID_NONE;

  do
  {
    refNestLvl--;

    /* scan all rules belonging to the current class */
    while ((diffServClassRuleGetNext(classIndex, classRuleIndex, &nextClassIndex,
                                     &classRuleIndex) == L7_SUCCESS) &&
           (nextClassIndex == classIndex))
    {

      /* (re)initialize logic flags to default state */
      needElemCreate = L7_TRUE;
      aridChanged = L7_FALSE;

      /* initialize a "local" working row structure for use during this pass */
      dsStdMibAuxMFClfSetToDefaults(&auxMFClfrLocal);

      /* set working aux ptr to the local structure */
      pAuxInfo = &auxMFClfrLocal;

      /* save class index, class rule index for possible aux elem reuse */
      pAuxInfo->classIndex = classIndex;
      pAuxInfo->classRuleIndex = classRuleIndex;

      /* get entry type and exclude flag for this class rule */
      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE,
                                     (void *)&classRuleType) != L7_SUCCESS)
        return L7_FAILURE;

      if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                     L7_DIFFSERV_CLASS_RULE_MATCH_EXCLUDE_FLAG,
                                     (void *)&excludeFlag) != L7_SUCCESS)
        return L7_FAILURE;

      /* depending on the class type and other factors, it may be necessary
       * to change from the local to the general aux structure while processing
       * this class rule (the general aux collects multiple rules of a related
       * group that are evaluated as an AND condition)
       *
       * this is done for the following cases:
       *   - type 'all': non-excluded rule
       *   - type 'acl': when ARID has not changed (or this is first ARID rule)
       */
      if (classType == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL)
      {
        if (excludeFlag != L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE)
        {
          pAuxInfo = &auxMFClfrGeneral;
          if (generalInUse != L7_TRUE)
          {
            generalInUse = L7_TRUE;
            generalExcludeFlag = L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_FALSE;

            /* save initial class index, class rule index for possible aux elem reuse */
            pAuxInfo->classIndex = classIndex;
            pAuxInfo->classRuleIndex = classRuleIndex;
          }
          needElemCreate = L7_FALSE;
        }
      }

      else if (classType == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ACL)
      {
        /* get the access list rule id for this class rule */
        if (diffServClassRuleAclRuleIdGet(classIndex, classRuleIndex,
                                          &arid) != L7_SUCCESS)
          return L7_FAILURE;

        if ((arid == prevArid) || (prevArid == DSMIB_CLASS_RULE_ARID_NONE))
        {
          pAuxInfo = &auxMFClfrGeneral;
          if (generalInUse != L7_TRUE)
          {
            generalInUse = L7_TRUE;
            generalExcludeFlag = excludeFlag;     /* all same w/in ARID group */

            /* save initial class index, class rule index for possible aux elem reuse */
            pAuxInfo->classIndex = classIndex;
            pAuxInfo->classRuleIndex = classRuleIndex;
          }
          needElemCreate = L7_FALSE;
        }
        else
        {
          aridChanged = L7_TRUE;

          /* NOTE: The current 'acl' rule information is stored in the
           *       aux local structure for this case.  Once the general
           *       aux struct containing the previous ARID group info
           *       is processed, the current rule info is copied from
           *       the local to general aux struct for the new ARID group.
           */
        }

        prevArid = arid;

      } /* endelseif class type 'acl' */

      /* if this is a refclass rule, save current indexes in the reference
       * class nesting "stack" and immediately begin processing rules from
       * the referenced class
       *
       * NOTE: The remaining rules for the current class are processed after
       *       all of the referenced class' rules.
       *
       * NOTE: Only occurs for class type 'all' or 'any'.  All classes
       *       in the reference chain are of the same class type.
       */
      if (classRuleType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS)
      {
        /* get the index of the referenced class
         * NOTE: if unsuccessful, assume the reference is invalid and ignore it
         */
        if (diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                       L7_DIFFSERV_CLASS_RULE_MATCH_REFCLASS_INDEX,
                                       &refClassIndex) == L7_SUCCESS)
        {
          refSaveStack[refNestLvl].classIndex = classIndex;
          refSaveStack[refNestLvl].classRuleIndex = classRuleIndex;
          refNestLvl++;

          /* change working index values to begin processing referenced class */
          classIndex = refClassIndex;
          classRuleIndex = 0;

        } /* endif refclass index object get */

        /* there is nothing else to do for this refclass rule */
        continue;                               /* at top of while loop */

      } /* endif refclass rule type */


      /* set the class rule object field(s) in the relevant aux working struct*/
      if (dsStdMibAuxMFClfrObjectSet(classIndex, classRuleIndex,
                                     pAuxInfo) != L7_SUCCESS)
        return L7_FAILURE;


      /* conditionally create the std MIB elements for the classifier */
      if (needElemCreate == L7_TRUE)
      {
        dsmibTruthValue_t   tempExcludeFlag = excludeFlag;

        /* for class type 'acl' with an ARID change, we want to use the
         * class rule fields stored in the general aux struct from the
         * previous ARID group that is now complete
         *
         * use saved excludeFlag value of previous ARID group as well
         */
        if (aridChanged == L7_TRUE)
        {
          pAuxInfo = &auxMFClfrGeneral;
          tempExcludeFlag = generalExcludeFlag;
        }

        /* save a temp copy of the pPathNext value to be used when creating
         * each classifier element; there is no need to pass back the updated
         * path next row ptr from the classifier element(s), since the common
         * parent Clfr entry does not use it
         */
        tempNext = *pPathNext;

        if (dsStdMibClfrElementsBuild(pAuxInfo, classType, tempExcludeFlag,
                                      clfrId, &tempNext) != L7_SUCCESS)
          return L7_FAILURE;
      }

      /* if the ARID changed (only possible for class type 'acl'), replace
       * the general aux structure with the current local aux struct contents
       * (contains the first class rule field for the new ARID group)
       */
      if (aridChanged == L7_TRUE)
      {
        memcpy(&auxMFClfrGeneral, &auxMFClfrLocal, sizeof(auxMFClfrGeneral));
        generalInUse = L7_TRUE;
        generalExcludeFlag = excludeFlag;
        /* NOTE: The saved classIndex, classRuleIndex values were copied from local */
      }

    } /* endwhile scan all class rules */


    /* if not currently at refNestLvl 0, then we must have just finished
     * processing a reference class (there can be a series of them)
     */
    if (refNestLvl > 0)
    {
      /* restore the class and class rule indexes for the interrupted level */
      classIndex = refSaveStack[refNestLvl-1].classIndex;
      classRuleIndex = refSaveStack[refNestLvl-1].classRuleIndex;
    }

  } while (refNestLvl > 0);

  /* handle any remnants from the general aux classifier working structure */
  if (generalInUse == L7_TRUE)
  {
    pAuxInfo = &auxMFClfrGeneral;
    tempNext = *pPathNext;
    if (dsStdMibClfrElementsBuild(pAuxInfo, classType, generalExcludeFlag,
                                  clfrId, &tempNext) != L7_SUCCESS)
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Build the Clasifier Element along with its specific
*           Auxiliary Multi-field Classifier row element
*
* @param    pAuxInfo    @b{(input)} Ptr to Auxiliary Multi Field
*                                   Classifier info
* @param    classType   @b{(input)} DiffServ class type
* @param    excludeFlag @b{(input)} DiffServ class rule exclude flag
*                                   value
* @param    clfrId      @b{(input)} Classifier Id
* @param    pPathNext   @b{(input)} Ptr to next elem in data path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibClfrElementsBuild(dsStdMibAuxMFClfrEntryCtrl_t *pAuxInfo,
                                  dsmibClassType_t classType,
                                  dsmibTruthValue_t excludeFlag,
                                  L7_uint32 clfrId,
                                  dsStdMibRowPtr_t *pPathNext)
{
  L7_uint32         auxMFClfrId;
  dsStdMibRowPtr_t  next;
  dsStdMibRowPtr_t  specific;
  L7_uint32         precedence;

  /* create Aux Multi-Field Classifier row element from the specified info */
  if (diffServAuxMFClfrCreate(pAuxInfo, &auxMFClfrId) != L7_SUCCESS)
    return L7_FAILURE;

  /* create Classifier Element row, referencing the Aux MFClfr as its
   * 'specific' element
   */
  if (excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE)
    next = DSSTDMIB_ZERO_DOT_ZERO;
  else
    next = *pPathNext;
  DSSTDMIB_ROW_PTR_SET(&specific, DSSTDMIB_TABLE_ID_AUX_MF_CLFR,
                       auxMFClfrId, 0, 0);
  precedence = DSSTDMIB_CLFR_ELEM_PRECEDENCE_ANY;

  /* override precedence setting for class type 'all' per exclude flag value */
  if (classType == L7_USMDB_MIB_DIFFSERV_CLASS_TYPE_ALL)
  {
    if (excludeFlag == L7_USMDB_MIB_DIFFSERV_TRUTH_VALUE_TRUE)
      precedence = DSSTDMIB_CLFR_ELEM_PRECEDENCE_ALLEXCL;
    else
      precedence = DSSTDMIB_CLFR_ELEM_PRECEDENCE_ALL;
  }

  if (diffServClfrElemCreate(clfrId, &specific, precedence,
                             &next) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Create a Data Path element for a given interface in a specific
*           direction
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pStart      @b{(input)} Pointer to first Data Path element
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibDataPathElemCreate(L7_uint32 intIfNum,
                                   L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                                   dsStdMibRowPtr_t *pStart)
{
  dsStdMibDataPathEntryCtrl_t  row, *pRow;
  L7_uint32                    ifIndex;

  /* get ifIndex from intIfNum */
  if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  pRow = &row;
  DIFFSERV_NULLPTR_CHECK(pRow);
  memset(pRow, 0, sizeof(*pRow));

  /* initialize row index keys */
  pRow->key.ifIndex = ifIndex;
  pRow->key.ifDirection = ifDirection;

  pRow->tableId = DSSTDMIB_TABLE_ID_DATA_PATH;

  /* Set other objects */
  pRow->mib.ifDirection = ifDirection;
  pRow->mib.start = *pStart;
  pRow->mib.storage = DSSTDMIB_DEFAULT_STORAGE_TYPE;
  pRow->mib.status = L7_DIFFSERV_ROW_STATUS_ACTIVE;

  /* Insert Data Path Entry in AVL */
  if (dsmibRowInsert(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_DATA_PATH],
                     (void *)pRow) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete a Data Path element for a given interface in a specific
*           direction
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
* @param    pStart      @b{(input)} Pointer to first Data Path element
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibDataPathElemDelete(L7_uint32 intIfNum,
                                   L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
  dsStdMibDataPathEntryCtrl_t *pDataPathRow;

  /* get data path row*/
  pDataPathRow = dsStdMibDataPathRowFind(intIfNum, ifDirection);
  if (pDataPathRow == L7_NULLPTR)
    return L7_SUCCESS;        /* consider this a success if row not found */

  /* datapath delete */
  if (dsmibRowDelete(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_DATA_PATH],
                     pDataPathRow) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Delete a Data Path for a given interface in a specific
*           direction
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t diffServDataPathDelete(L7_uint32 intIfNum,
                               L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
  dsStdMibTableId_t           tableId = DSSTDMIB_TABLE_ID_DATA_PATH;
  L7_RC_t                     rc;
  dsStdMibRowPtr_t            pathNext;
  dsStdMibDataPathEntryCtrl_t *pDataPathRow;

  DS_TRACE_PT_DELETE_ENTER(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId,
                           intIfNum, ifDirection, 0);

  /* acquire the semaphore */
  DIFFSERV_SEMA_TAKE(dsStdMibSemId, L7_WAIT_FOREVER);

  /* get data path row*/
  pDataPathRow = dsStdMibDataPathRowFind(intIfNum, ifDirection);
  if (pDataPathRow != L7_NULLPTR)
  {
    /* this is the working path next value that is updated as each successive
     * element in the data path is deleted, starting with the classifier entry
     */
    pathNext = pDataPathRow->mib.start;

    rc = dsStdMibDataPathDelete(intIfNum, ifDirection, &pathNext);
  }
  else
  {
    rc = L7_SUCCESS;          /* consider this a success if row not found */
  }

  /* release the semaphore */
  DIFFSERV_SEMA_GIVE(dsStdMibSemId);

  DS_TRACE_PT_DELETE_EXIT(DS_TRACE_SUBID_STDMIB, (L7_uchar8)tableId,
                          intIfNum, ifDirection, 0,
                          0, (L7_uchar8)rc);

  return rc;
}

/*********************************************************************
* @purpose  Delete a Data Path for a given interface in an inbound or
*           outbound direction
*
* @param    intIfNum    @b{(input)}  Internal interface number
* @param    ifDirection @b{(input)}  Interface direction
* @param    pPathNext   @b{(inout)}  Ptr to next elem in data path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsStdMibDataPathDelete(L7_uint32 intIfNum,
                               L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection,
                               dsStdMibRowPtr_t *pPathNext)
{
  dsStdMibRowPtr_t                     clfrElemRow;
  L7_uint32                            clfrId, clfrElemId, nextClfrId;


  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* always expecting the initial pPathNext to refer to a Classifier entry */
  if (pPathNext->tableId != DSSTDMIB_TABLE_ID_CLFR)
    return L7_FAILURE;

  /* get clfr id */
  clfrId = pPathNext->rowIndex1;


  /* Datapath elements on inbound interface:
   *
   *   Data Path ::= Classifier + Counter + Dropper
   * -or-
   *   Data Path ::= Classifier + Counter [+ (Policer | Marker) [+AssignQueue] [+ (Mirror | Redirect)]
   *
   *   Classifier ::= Clfr + ClfrElements + AuxMfClfr
   *   Counter ::= OfferedCtr
   *   Dropper ::= AlwaysDrop
   *   Policer ::= Simple | SingleRate | TwoRate
   *   Marker ::= MarkCos | MarkCos2 | MarkDscp | MarkPrec
   *
   *
   * Datapath elements on outbound interface:
   *
   *   Data Path ::= Classifier + Dropper + Counter + Allocator [+ Shaper] +Scheduler
   *
   *   Classifier ::= Clfr + ClfrElements + AuxMfClfr
   *   Dropper ::= (Tail | Random)
   *   Counter ::= (SentCtr | (Meter, SentCtr, ShapeDelayCtr))
   *   Allocator ::= (Bandwidth | Expedite | BestEffort)
   *   Shaper ::= (Average | Peak)
   *   Scheduler ::= (Scheduler1 | Scheduler2 | (Scheduler1, Scheduler2))
   *
   *
   * NOTE:  The data path elements are deleted from beginning to end of the
   *        sequence, with the pPathNext pointer updated with each element's
   *        next pointer.  Some recursion may be needed for elements with
   *        multiple 'next' data paths (e.g. meter).
   */

  /* scan all clfr elements*/
  clfrElemId = 0;
  while ((diffServClfrElemGetNext(clfrId, clfrElemId, &nextClfrId,
                                  &clfrElemId) == L7_SUCCESS) &&
         (clfrId == nextClfrId))
  {
    /* build a row ptr for the classifier element path instance to be deleted */
    DSSTDMIB_ROW_PTR_SET(&clfrElemRow, DSSTDMIB_TABLE_ID_CLFR_ELEMENT,
                         clfrId, clfrElemId, 0);

    /* delete entire set of path elements for this classifier instance */
    if (dsStdMibDataPathInstanceDelete(&clfrElemRow) != L7_SUCCESS)
      return L7_FAILURE;

  } /* endwhile scan all clfr elements */

  /* Clfr delete */
  if(diffServClfrDelete(pPathNext) != L7_SUCCESS)
    return L7_FAILURE;

  /* Data Path elem delete */
  if (dsStdMibDataPathElemDelete(intIfNum, ifDirection) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Delete each successive element in a data path instance
*
* @param    pPathNext   @b{(inout)}  Ptr to next elem in data path
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Can be called recursively to delete parallel data paths
*           originated from a particular element (e.g. meter succeed
*           next and fail next paths).
*
* @end
*********************************************************************/
L7_RC_t dsStdMibDataPathInstanceDelete(dsStdMibRowPtr_t *pPathNext)
{
  L7_RC_t       rc = L7_SUCCESS;

  DIFFSERV_NULLPTR_CHECK(pPathNext);

  /* successively delete data path elements until pPathNext is empty */
  while (DSSTDMIB_ROW_PTR_IS_NULL(pPathNext) != L7_TRUE)
  {
    /* only process table IDs for elements in the data path that contain
     * a 'next' pointer (all other tables IDs are cleaned up internally)
     */
    switch (pPathNext->tableId)
    {
    case DSSTDMIB_TABLE_ID_CLFR_ELEMENT:
      rc = diffServClfrElemDelete(pPathNext);
      break;

    case DSSTDMIB_TABLE_ID_METER:
      rc = diffServMeterDelete(pPathNext);
      break;

    case DSSTDMIB_TABLE_ID_ACTION:
      rc = diffServActionDelete(pPathNext);
      break;

    case DSSTDMIB_TABLE_ID_ALG_DROP:
      rc = diffServAlgDropDelete(pPathNext);
      break;

    default:
      /* table ID not handled by this function */
      rc = L7_FAILURE;
      break;
    } /* endswitch */

    if (rc != L7_SUCCESS)
      break;

  } /* endwhile */

  return rc;
}

/*********************************************************************
* @purpose  Delete a specific primitive element
*
* @param    pRow        @b{(inout)}  Ptr to primitive elem row ptr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Primitive elements do not contain a 'next' pointer and are
*           referenced by chaining elements.
*
* @end
*********************************************************************/
L7_RC_t dsStdMibPrimitiveElemDelete(dsStdMibRowPtr_t *pRow)
{
  L7_RC_t       rc;

  DIFFSERV_NULLPTR_CHECK(pRow);

  switch (pRow->tableId)
  {
  case DSSTDMIB_TABLE_ID_NONE:
    /* empty row (0.0) */
    rc = L7_SUCCESS;
    break;

  case DSSTDMIB_TABLE_ID_MULTI_FIELD_CLFR:
    rc = L7_FAILURE;                    /* this table not used */
    break;

  case DSSTDMIB_TABLE_ID_AUX_MF_CLFR:
    rc = diffServAuxMFClfrDelete(pRow->rowIndex1);
    break;

  case DSSTDMIB_TABLE_ID_TB_PARAM:
    rc = diffServTBParamDelete(pRow->rowIndex1);
    break;

  case DSSTDMIB_TABLE_ID_COLOR_AWARE:
    rc = diffServColorAwareDelete(pRow->rowIndex1);
    break;

  case DSSTDMIB_TABLE_ID_MARK_COS_ACT:
  case DSSTDMIB_TABLE_ID_MARK_COS_AS_COS2_ACT:
  case DSSTDMIB_TABLE_ID_MARK_COS2_ACT:
  case DSSTDMIB_TABLE_ID_MARK_IPDSCP_ACT:
  case DSSTDMIB_TABLE_ID_MARK_IPPREC_ACT:
    /* nothing to delete in these tables */
    rc = L7_SUCCESS;
    break;

  case DSSTDMIB_TABLE_ID_COUNT_ACT:
    rc = diffServCountActDelete(pRow->rowIndex1);
    break;

  case DSSTDMIB_TABLE_ID_ASSIGN_QUEUE:
    rc = diffServAssignQueueDelete(pRow->rowIndex1);
    break;

  case DSSTDMIB_TABLE_ID_REDIRECT:
    rc = diffServRedirectDelete(pRow->rowIndex1);
    break;

  case DSSTDMIB_TABLE_ID_MIRROR:
    rc = diffServMirrorDelete(pRow->rowIndex1);
    break;

  default:
    /* table ID not handled by this function */
    rc = L7_FAILURE;
    break;

  } /* endswitch */

  /* clear out row ptr if elem successfully deleted */
  if (rc == L7_SUCCESS)
    *pRow = DSSTDMIB_ZERO_DOT_ZERO;

  return rc;
}

/*********************************************************************
* @purpose  Obtain pointer to specified Data Path row element
*
* @param    intIfNum    @b{(input)} Internal interface number
* @param    ifDirection @b{(input)} Interface direction
*
* @returns  pRow        Pointer to row
* @returns  L7_NULLPTR  Row not found
*
* @notes    Assumes caller manages MIB index semaphore, if needed.
*
* @end
*********************************************************************/
dsStdMibDataPathEntryCtrl_t *dsStdMibDataPathRowFind(L7_uint32 intIfNum,
                              L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection)
{
  dsStdMibDataPathEntryCtrl_t  *pRow;
  dsStdMibDataPathKey_t        keys;
  L7_uint32                    ifIndex;

  /* get ifIndex from intIfNum */
  if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
    return L7_NULLPTR;

  /* get access to the data path row */
  keys.ifIndex = ifIndex;
  keys.ifDirection = ifDirection;

  pRow = dsmibRowFind(&dsStdMibAvlTree[DSSTDMIB_TABLE_ID_DATA_PATH],
                      &keys, AVL_EXACT);

  return pRow;
}

/*********************************************************************
* @purpose  Translate standard MIB internal row ptr to external API row ptr
*
* @param    stdRowPtr   @b{(input)} Standard MIB row ptr
*
* @returns  L7_USMDB_MIB_DIFFSERV_ROW_PTR_t   external API table ID row ptr
*
* @notes    All standard MIB table IDs are stored internally using a
*           local numbering scheme.  At USMDB, both private and standard
*           MIB table IDs are contained in a single list, so the numbers
*           differ.
*
* @end
*********************************************************************/
L7_USMDB_MIB_DIFFSERV_ROW_PTR_t dsStdMibRowPtrXlateToExt(dsStdMibRowPtr_t stdRowPtr)
{
  L7_USMDB_MIB_DIFFSERV_ROW_PTR_t extRowPtr;
  L7_USMDB_MIB_DIFFSERV_TABLE_ID_t extTableId;

  switch (stdRowPtr.tableId)
  {
  case DSSTDMIB_TABLE_ID_DATA_PATH:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_DATA_PATH;
    break;
  case DSSTDMIB_TABLE_ID_CLFR:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_CLFR;
    break;
  case DSSTDMIB_TABLE_ID_CLFR_ELEMENT:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_CLFR_ELEMENT;
    break;
  case DSSTDMIB_TABLE_ID_MULTI_FIELD_CLFR:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_MULTI_FIELD_CLFR;
    break;
  case DSSTDMIB_TABLE_ID_AUX_MF_CLFR:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_AUX_MF_CLFR;
    break;
  case DSSTDMIB_TABLE_ID_METER:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_METER;
    break;
  case DSSTDMIB_TABLE_ID_TB_PARAM:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_TB_PARAM;
    break;
  case DSSTDMIB_TABLE_ID_COLOR_AWARE:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_COLOR_AWARE;
    break;
  case DSSTDMIB_TABLE_ID_ACTION:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_ACTION;
    break;
  case DSSTDMIB_TABLE_ID_MARK_COS_ACT:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_COS_ACT;
    break;
  case DSSTDMIB_TABLE_ID_MARK_COS_AS_COS2_ACT:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_COS_AS_COS2_ACT;
    break;
  case DSSTDMIB_TABLE_ID_MARK_COS2_ACT:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_COS2_ACT;
    break;
  case DSSTDMIB_TABLE_ID_MARK_IPDSCP_ACT:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_IPDSCP_ACT;
    break;
  case DSSTDMIB_TABLE_ID_MARK_IPPREC_ACT:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_MARK_IPPREC_ACT;
    break;
  case DSSTDMIB_TABLE_ID_COUNT_ACT:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_COUNT_ACT;
    break;
  case DSSTDMIB_TABLE_ID_ASSIGN_QUEUE:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_ASSIGN_QUEUE;
    break;
  case DSSTDMIB_TABLE_ID_REDIRECT:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_REDIRECT;
    break;
  case DSSTDMIB_TABLE_ID_MIRROR:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_MIRROR;
    break;
  case DSSTDMIB_TABLE_ID_ALG_DROP:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_ALG_DROP;
    break;

  case DSSTDMIB_TABLE_ID_NONE:
  default:
    extTableId = L7_USMDB_MIB_DIFFSERV_TABLE_ID_NONE;
    break;

  } /* endswitch */

  extRowPtr.tableId = extTableId;
  extRowPtr.rowIndex1 = stdRowPtr.rowIndex1;
  extRowPtr.rowIndex2 = stdRowPtr.rowIndex2;
  extRowPtr.rowIndex3 = stdRowPtr.rowIndex3;

  return extRowPtr;
}

/*********************************************************************
* @purpose  Display contents of DiffServ standard MIB AVL control structures
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dsStdMibAvlCtrlShow(void)
{
  L7_uint32         treeHeapElemSize = (L7_uint32)sizeof(avlTreeTables_t);
  L7_uint32         allocBytes, totalBytes = 0;
  dsStdMibAvlCtrl_t *pCtrl;
  L7_uint32         i;
  L7_uint32         msgLvlReqd;

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  if (pDsStdMibAvlCtrl == L7_NULLPTR)
  {
    DIFFSERV_PRT(msgLvlReqd, "\nStandard MIB AVL control structure not allocated\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, "\nAll AVL Tree Heaps:\n");
  DIFFSERV_PRT(msgLvlReqd, "  entSize=%u\n", treeHeapElemSize);

  /*  each AVL tree */
  for (i = (L7_uint32)(DSSTDMIB_TABLE_ID_NONE+1); i < DSSTDMIB_TABLE_ID_TOTAL; i++)
  {
    /* NOTE: Display values for all tables, even if entryMaxNum is 0 */
    pCtrl = pDsStdMibAvlCtrl + i;

    /* calculate memory allocation amount */
    allocBytes = (treeHeapElemSize + pCtrl->entrySize) * pCtrl->entryMaxNum;
    totalBytes += allocBytes;

    DIFFSERV_PRT(msgLvlReqd, "\n%s Table:\n", dsmibStdTableIdStr[i]);
    DIFFSERV_PRT(msgLvlReqd, "  maxEnt=%u entSize=%u keySize=%u ",
                 pCtrl->entryMaxNum, pCtrl->entrySize, pCtrl->entryKeySize);
    DIFFSERV_PRT(msgLvlReqd, "pTreeHeap=0x%8.8x pDataHeap=0x%8.8x ",
                 (L7_uint32)pCtrl->pTreeHeap, (L7_uint32)pCtrl->pDataHeap);
    DIFFSERV_PRT(msgLvlReqd, "allocBytes=%u\n", allocBytes);
  }
  DIFFSERV_PRT(msgLvlReqd, "\nTotal standard MIB AVL resource byte allocation:  %u\n\n", totalBytes);
}

/*********************************************************************
* @purpose  Display all standard MIB tables
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsmibStdShowAll(void)
{
  dsStdMibAvlTableSizeShow();
  dsStdMibDataPathTableShow();
  dsStdMibClfrTableShow();
  dsStdMibClfrElementTableShow();
  dsStdMibAuxMFClfrTableShow();
  dsStdMibMeterTableShow();
  dsStdMibTBParamTableShow();
  dsStdMibColorAwareTableShow();
  dsStdMibActionTableShow();
  dsStdMibCountActTableShow();
  dsStdMibAssignQueueTableShow();
  dsStdMibRedirectTableShow();
  dsStdMibMirrorTableShow();
  dsStdMibAlgDropTableShow();
}

