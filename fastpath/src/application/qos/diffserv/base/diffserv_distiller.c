/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   diffserv_distiller.c
*
* @purpose    DiffServ component Distiller implementation
*
* @component  DiffServ
*
* @comments   none
*
* @create     05/03/2002
*
* @author     gpaussa
* @end
*
**********************************************************************/
#include "l7_diffserv_include.h"
#include "diffserv_tlv.h"
#include "usmdb_mib_diffserv_common.h"
#include "usmdb_mib_diffserv_private_api.h"

/* external references */

/* Distiller globals */
dsDstlCtrl_t    *pDsDstlCtrl;                   /*principal control struct ptr*/

/* Data to support Distiller interface node AVL tree */
static L7_uint32          dsDstlIntfTreeEntryMax = L7_DIFFSERV_SERVICE_INTF_LIM;
static avlTreeTables_t    dsDstlIntfTreeHeap[L7_DIFFSERV_SERVICE_INTF_LIM];
static dsDstlIntfNode_t   dsDstlIntfDataHeap[L7_DIFFSERV_SERVICE_INTF_LIM];
static avlTree_t          dsDstlIntfAvlTree;

/* local function prototypes */
static L7_RC_t dsDistillerCtrlInit(void);

static int dsDstlIntfNodeKeyCompare(const void *a, const void *b, size_t len)
{
  static L7_uint32    keyLengthErrors = 0;
  dsDstlIntfNodeKey_t *key_a = (dsDstlIntfNodeKey_t *)a;
  dsDstlIntfNodeKey_t *key_b = (dsDstlIntfNodeKey_t *)b;
  L7_int32            val;

  if (len != sizeof(dsDstlIntfNodeKey_t))
    keyLengthErrors++;                  /* might be handy for debugging */

  if (key_a->policyIndex < key_b->policyIndex)
    val = -1;
  else if (key_a->policyIndex > key_b->policyIndex)
    val = 1;
  else if (key_a->ifIndex < key_b->ifIndex)
    val = -1;
  else if (key_a->ifIndex > key_b->ifIndex)
    val = 1;
  else
    val = 0;

  return val;
}

/*********************************************************************
* @purpose  Initialize DiffServ Distiller
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
L7_RC_t dsDistillerInit(void)
{
  static const char *routine_name = "dsDistillerInit()";

  /* allocate the Distiller control data structure */
  if ((pDsDstlCtrl = osapiMalloc(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, (L7_uint32)sizeof(*pDsDstlCtrl))) == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: Unable to allocate Distiller control memory\n", routine_name);
    return L7_FAILURE;
  }

  /* initialize specific Distiller control fields */
  if (dsDistillerCtrlInit() != L7_SUCCESS)
    return L7_FAILURE;

  /* create a semaphore to control mutual exclusion access to OLL AVL tree */
  pDsDstlCtrl->ollSemId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (pDsDstlCtrl->ollSemId == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: Unable to create OLL AVL tree semaphore\n", routine_name);
    return L7_FAILURE;
  }

  /* create Distiller service intf ordered linked list AVL tree */
  memset(dsDstlIntfTreeHeap, 0, sizeof(dsDstlIntfTreeHeap));
  memset(dsDstlIntfDataHeap, 0, sizeof(dsDstlIntfDataHeap));
  memset(&dsDstlIntfAvlTree, 0, sizeof(dsDstlIntfAvlTree));
  avlCreateAvlTree(&dsDstlIntfAvlTree, dsDstlIntfTreeHeap, dsDstlIntfDataHeap,
                   dsDstlIntfTreeEntryMax, (L7_uint32)sizeof(dsDstlIntfNode_t),
                   0x10, (L7_uint32)sizeof(dsDstlIntfNodeKey_t));
  (void)avlSetAvlTreeComparator(&dsDstlIntfAvlTree, dsDstlIntfNodeKeyCompare);

  pDsDstlCtrl->pOLL = &dsDstlIntfAvlTree;

  /* initialize Distiller TLV code */
  if (dsDstlTlvInit() != L7_SUCCESS)
    return L7_FAILURE;


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize DiffServ Distiller
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
L7_RC_t dsDistillerInitPhase1Process(void)
{
  static const char *routine_name = "dsDistillerInitPhase1Process()";

  /* allocate the Distiller control data structure */
  if ((pDsDstlCtrl = osapiMalloc(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, (L7_uint32)sizeof(*pDsDstlCtrl))) == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: Unable to allocate Distiller control memory\n", routine_name);
    return L7_FAILURE;
  }

  /* initialize specific Distiller control fields */
  if (dsDistillerCtrlInit() != L7_SUCCESS)
    return L7_FAILURE;

  /* create a semaphore to control mutual exclusion access to OLL AVL tree */
  pDsDstlCtrl->ollSemId = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
  if (pDsDstlCtrl->ollSemId == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
            "%s: Unable to create OLL AVL tree semaphore\n", routine_name);
    return L7_FAILURE;
  }

  /* create Distiller service intf ordered linked list AVL tree */
  memset(dsDstlIntfTreeHeap, 0, sizeof(dsDstlIntfTreeHeap));
  memset(dsDstlIntfDataHeap, 0, sizeof(dsDstlIntfDataHeap));
  memset(&dsDstlIntfAvlTree, 0, sizeof(dsDstlIntfAvlTree));
  avlCreateAvlTree(&dsDstlIntfAvlTree, dsDstlIntfTreeHeap, dsDstlIntfDataHeap,
                   dsDstlIntfTreeEntryMax, (L7_uint32)sizeof(dsDstlIntfNode_t),
                   0x10, (L7_uint32)sizeof(dsDstlIntfNodeKey_t));
  (void)avlSetAvlTreeComparator(&dsDstlIntfAvlTree, dsDstlIntfNodeKeyCompare);

  pDsDstlCtrl->pOLL = &dsDstlIntfAvlTree;


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize DiffServ Distiller
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
L7_RC_t dsDistillerInitPhase2Process(void)
{
  /* initialize Distiller TLV code */
  if (dsDstlTlvInit() != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Initialize DiffServ Distiller
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
L7_RC_t dsDistillerInitPhase3Process(void)
{
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Fini DiffServ Distiller
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dsDistillerFiniPhase1Process(void)
{
  if (pDsDstlCtrl->ollSemId != L7_NULLPTR)
  {
    (void)osapiSemaDelete(pDsDstlCtrl->ollSemId);
    pDsDstlCtrl->ollSemId = L7_NULLPTR;
  }

  if (pDsDstlCtrl != L7_NULLPTR)
  {
    osapiFree(L7_FLEX_QOS_DIFFSERV_COMPONENT_ID, pDsDstlCtrl);
    pDsDstlCtrl = L7_NULLPTR;
  }

  /* this semaphore was created in avlCreateAvlTree */
  if (dsDstlIntfAvlTree.semId != L7_NULLPTR)
    (void)osapiSemaDelete(dsDstlIntfAvlTree.semId);

  memset(dsDstlIntfTreeHeap, 0, sizeof(dsDstlIntfTreeHeap));
  memset(dsDstlIntfDataHeap, 0, sizeof(dsDstlIntfDataHeap));
  memset(&dsDstlIntfAvlTree, 0, sizeof(dsDstlIntfAvlTree));
}

/*********************************************************************
* @purpose  Fini DiffServ Distiller
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dsDistillerFiniPhase2Process(void)
{
  dsDstlTlvFini();
}

/*********************************************************************
* @purpose  Fini DiffServ Distiller
*
* @param    void
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dsDistillerFiniPhase3Process(void)
{
  return;
}

/*********************************************************************
* @purpose  Reset DiffServ Distiller to an initial state
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
L7_RC_t dsDistillerReset(void)
{
  L7_RC_t           rc = L7_SUCCESS;
  dsDstlIntfNode_t  node, *pNode;

  /* delete all policy instance TLVs that have been issued to the low-level code
   *
   * use a brute-force approach here and do not worry about related clean up
   * (like is done in the policy tear down function) -- this function is
   * typically invoked when the TLVs must be retracted, so the focus is on
   * making that happen.
   *
   * NOTE:  This must be done first in the Distiller reset sequence.
   */

  /* set up OLL search node */
  memset(&node, 0, sizeof(node));
  node.key.policyIndex = 0;                     /* start w/ first policy      */
  node.key.ifIndex = 0;                         /* start w/ first interface   */
  pNode = &node;

  /* walk entire OLL (all policies) */
  while (((pNode = (dsDstlIntfNode_t *)
            avlSearchLVL7(pDsDstlCtrl->pOLL, pNode, AVL_NEXT)) != L7_NULLPTR))
  {
    if (dsDstlTlvPolicyDel(pNode->key.policyIndex, pNode->intIfNum,
                           pNode->ifDirection) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(pNode->intIfNum, L7_SYSNAME, ifName);

      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
              "dsDistillerReset: Unable to delete TLV instances for policy %u, "
              "intf %s\n", pNode->key.policyIndex, ifName);
      rc = L7_FAILURE;                          /* change rc and keep going   */
    }
  } /* endwhile */

  /* re-initialize specific Distiller control fields */
  if (dsDistillerCtrlInit() != L7_SUCCESS)
    rc = L7_FAILURE;

  /* purge Distiller service intf ordered linked list AVL tree */
  avlPurgeAvlTree(&dsDstlIntfAvlTree, dsDstlIntfTreeEntryMax);

  return rc;
}

/*********************************************************************
* @purpose  Initialize Distiller control info structure
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
static L7_RC_t dsDistillerCtrlInit(void)
{
  L7_uint32           i, j;
  L7_uint32           key, keymax;
  dsDstlPolicyInfo_t  *pPolInfo;

  /* initialize specific Distiller control fields */
  keymax = L7_DIFFSERV_POLICY_INST_LIM;
  for (i = 1, key = 1; i <= L7_DIFFSERV_POLICY_LIM; i++)
  {
    pPolInfo = &pDsDstlCtrl->policy[i];

    for (j = 1; j <= L7_DIFFSERV_INST_PER_POLICY_LIM; j++)
    {
      pPolInfo->instIsReady[j] = L7_FALSE;

      /* sanity check bounds of key-to-index table */
      if (key > keymax)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "dsDistillerInit: Key-to-index table exceeds maximum (%u)\n",
                keymax);
        return L7_FAILURE;
      }

      /* instance-to-key table */
      pPolInfo->inst2KeyTable[j] = key;

      /* key-to-instance table */
      pDsDstlCtrl->key2InstTable[key].policyIndex = i;
      pDsDstlCtrl->key2InstTable[key].policyInstIndex = j;

      key++;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handle DiffServ administrative mode changes from private
*           MIB component
*
* @param    mode        @{(input)} Operating mode (L7_ENABLE, L7_DISABLE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function is concerned with major mode transitions between
*           the disabled and enabled states.  The focus is to process all
*           attached service policies and take the appropriate action per
*           the mode.
*
* @notes    The mode value is passed in as a parameter since it is possible
*           that the actual configuration value has not been updated yet,
*           as is typically the case when disabling DiffServ.
*
* @end
*********************************************************************/
L7_RC_t dsDistillerAdminModeChange(L7_uint32 mode)
{
  L7_uint32     policyIndex;

  if ((mode != L7_ENABLE) && (mode != L7_DISABLE))
    return L7_FAILURE;

  DS_TRACE_PT_ADMIN_MODE_CHNG(mode);

  /* walk through entire private MIB Policy Table and evaluate each policy
   * row by row.
   */
  policyIndex = 0;                              /* start with first policy */
  while (diffServPolicyGetNext(policyIndex, &policyIndex) == L7_SUCCESS)
  {
    /* evaluate possible actions for this policy based on the new mode */
    if (dsDistillerPolicyEvaluate(policyIndex, mode, DSMIB_POLICY_EVAL_ALL_INTF,
                                  L7_FALSE) != L7_SUCCESS)
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handle interface state changes as they pertain to Distiller
*           operation
*
* @param    intIfNum    @{(input)} Internal interface number
* @param    event       @{(input)} Interface change event
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function assumes NIM has already updated its link state
*           prior to initiating callbacks to its registrants.  The Distiller
*           only cares about certain interface events and ignores
*           all other events.
*
* @notes    Changes in link state affect service interface operational
*           status.  A link down condition causes deactivation of DiffServ
*           on the service interface.  A link up may cause an attached
*           DiffServ policy to become active on the service interface
*           (provided other conditions are satisfied).
*
* @end
*********************************************************************/
L7_RC_t dsDistillerIntfEventNotify(L7_uint32 intIfNum, L7_uint32 event)
{
  L7_uint32     adminMode;
  L7_uint32     nextIntf;
  L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_t ifDirection;
  L7_uint32     policyIndex;
  L7_RC_t       rc, rc2;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

  switch (event)
  {
  case L7_ACTIVE:
  case L7_INACTIVE:
  case L7_LAG_ACQUIRE:
  case L7_PROBE_SETUP:
  case L7_LAG_RELEASE:
  case L7_PROBE_TEARDOWN:
  case L7_SPEED_CHANGE:
  case L7_DETACH:
    /* proceed to evaluate policy for these events */
    break;

  default:
    /* ignore all other interface events */
    return L7_SUCCESS;
    /*PASSTHRU*/
  }

  /* get current DiffServ admin mode */
  diffServAdminModeGet(&adminMode);

  DS_TRACE_PT_INTF_EVENT(event, adminMode, intIfNum);

  /* check private MIB Service Table for all occurrences of this interface
   * and evaluate the attached policy, if any
   *
   * NOTE: Do not stop for a policy evaluation error.  Remember that an error
   *       occurred, but process all Service Table rows belonging to this
   *       interface.
   */
  rc = L7_SUCCESS;
  ifDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE;  /* start with first */
  while ((diffServServiceGetNext(intIfNum, ifDirection,
                                 &nextIntf, &ifDirection) == L7_SUCCESS) &&
         (nextIntf == intIfNum))
  {
    if (diffServServiceObjectGet(intIfNum, ifDirection,
                                 L7_DIFFSERV_SERVICE_POLICY_INDEX,
                                 &policyIndex) == L7_SUCCESS)
    {
      /* evaluate possible actions for this policy due to intf state change */
      if ((rc2 = dsDistillerPolicyEvaluate(policyIndex, adminMode, intIfNum,
                                           L7_FALSE)) != L7_SUCCESS)
      {
        /* log error message and remember failure rc, but keep processing */
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "dsDistillerIntfEventNotify: error (rc=%u) evaluating policy "
                "index %u, intf %s, direction %u\n", rc2, policyIndex,
                ifName, ifDirection);
        rc = L7_FAILURE;
      }
    }
  } /* endwhile */

  return rc;
}

/*********************************************************************
* @purpose  Evaluate current conditions to determine if any policy
*           activity needs to occur at the low-level
*
* @param    void
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This function must be invoked immediately after the deletion
*           of **any** DiffServ private MIB table row (except Service table).
*
* @notes    The policy evaluations occur for all interfaces, since this
*           function is used when there is a change in overall class or
*           policy definition.
*
* @end
*********************************************************************/
L7_RC_t dsDistillerEvaluateAll(void)
{
  L7_uint32     adminMode;
  L7_uint32     policyIndex, policyInstIndex;

  /* bypass all policy evaluation while in DiffServ "special config mode"
   *
   * NOTE:  This is done for performance reasons, since there is no need
   *        to do this work at this time.  The instance 'is ready' flag
   *        is updated as each policy instance and attribute row becomes
   *        active.  When the DiffServ config processing completes
   *        its work, it exits "special config mode" and sets the
   *        DiffServ admin mode to enable, at which time all policies
   *        are evaluated.
   */
  if (dsmibSpecialConfigModeGet() != L7_FALSE)
    return L7_SUCCESS;

  /* get current DiffServ admin mode */
  diffServAdminModeGet(&adminMode);

  DS_TRACE_PT_EVAL_ALL(adminMode);

  /* walk through entire private MIB Policy Instance Table and update the
   * Distiller's policy instance 'isReady' flag based on the latest information
   */
  policyIndex = policyInstIndex = 0;            /* start with first instance */
  while (diffServPolicyInstGetNext(policyIndex, policyInstIndex,
                          &policyIndex, &policyInstIndex) == L7_SUCCESS)
  {
    /* only evaluate policies that are referenced by services */
    if (diffServIsPolicyReferenced(policyIndex) == L7_TRUE)
    {
      pDsDstlCtrl->policy[policyIndex].instIsReady[policyInstIndex] =
        dsDistillerPolicyInstIsReady(policyIndex, policyInstIndex);
    }
    else
    {
      /* move on to next policy if current one is not being used in a service */
      policyIndex = policyIndex+1;
      policyInstIndex = 0;
    }
  }

  /* walk through entire private MIB Policy Table and evaluate each policy
   * row by row.
   */
  policyIndex = 0;                              /* start with first policy */

  while (diffServPolicyGetNext(policyIndex, &policyIndex) == L7_SUCCESS)
  {
    if (diffServIsPolicyReferenced(policyIndex) == L7_TRUE)
    {
      /* evaluate possible actions for this policy based on the admin mode */
      if (dsDistillerPolicyEvaluate(policyIndex, adminMode, DSMIB_POLICY_EVAL_ALL_INTF,
                                    L7_FALSE) != L7_SUCCESS)
        return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Handle event notifications from the private MIB component
*
* @param    tableId     @{(input)} MIB table identifier
* @param    pRow        @{(input)} Row pointer (generic) within MIB table
* @param    event       @{(input)} Event indication (L7_ACTIVE, L7_INACTIVE)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Only important row status transitions are signaled here.
*           These are investigated to determine if any interaction
*           with the low-level code is required.  This occurs on a
*           row basis for any row regardless of current operating mode.
*
* @end
*********************************************************************/
L7_RC_t dsDistillerRowEventNotify(dsmibTableId_t tableId, void *pRow,
                                  L7_uint32 event)
{
  L7_uint32     evalIntIfNum = DSMIB_POLICY_EVAL_ALL_INTF;
  L7_uint32     adminMode, mode;
  L7_uint32     policyIndex, policyInstIndex;

  /* check inputs */
  if ((tableId == DSMIB_TABLE_ID_NONE) || (tableId >= DSMIB_TABLE_ID_TOTAL))
    return L7_FAILURE;
  if (pRow == L7_NULLPTR)
    return L7_FAILURE;
  if ((event != L7_ACTIVE) && (event != L7_INACTIVE) && (event != L7_DELETE))
    return L7_FAILURE;

  policyIndex = policyInstIndex = 0;            /* initialize for all cases */

  /* get current DiffServ admin mode */
  diffServAdminModeGet(&adminMode);

  /* derive an effective mode based on the admin mode and the incoming event */
  if ((adminMode == L7_ENABLE) && (event == L7_ACTIVE))
    mode = L7_ENABLE;
  else
    mode = L7_DISABLE;

  DS_TRACE_PT_ROW_EVENT(tableId, event, adminMode, pRow);

  /* perform specific update activity based on table type */
  switch (tableId)
  {
  case DSMIB_TABLE_ID_CLASS:
  case DSMIB_TABLE_ID_CLASS_RULE:
    {
      dsmibClassEntryCtrl_t   *pClass, *pRefClass;
      L7_uint32               tempClassIndex;
      L7_uint32               prevPolicyIndex;
      L7_BOOL                 found;

      /* set up Class Table row ptr */
      if (tableId == DSMIB_TABLE_ID_CLASS)
        pClass = (dsmibClassEntryCtrl_t *)pRow;
      else
        pClass = ((dsmibClassRuleEntryCtrl_t *)pRow)->pClass;

      /* walk through entire private MIB Policy Instance Table, updating the
       * 'isReady' state for any instance that uses this class
       */
      policyIndex = policyInstIndex = 0;        /* start with first instance */
      prevPolicyIndex = policyIndex;            /* init to watch transitions */
      while (diffServPolicyInstGetNext(policyIndex, policyInstIndex,
                              &policyIndex, &policyInstIndex) == L7_SUCCESS)
      {
        if (diffServPolicyInstObjectGet(policyIndex, policyInstIndex,
              L7_DIFFSERV_POLICY_INST_CLASS_INDEX, &tempClassIndex)
              != L7_SUCCESS)
          continue;

        /* starting with the policy class, scan the class reference chain,
         * if any, looking for a match against the class that changed
         */
        found = L7_FALSE;
        while (tempClassIndex != DSMIB_CLASS_REF_INDEX_NONE)
        {
          if ((pRefClass = dsmibClassRowFind(tempClassIndex)) == L7_NULLPTR)
            break;

          if (pRefClass == pClass)
          {
            found = L7_TRUE;
            break;
          }

          tempClassIndex = pRefClass->refClassIndex;
        }
        if (found != L7_TRUE)
          continue;

        pDsDstlCtrl->policy[policyIndex].instIsReady[policyInstIndex] =
          dsDistillerPolicyInstIsReady(policyIndex, policyInstIndex);

        /* a change in class or class rule row status can affect instances
         * belonging to different policies, so do a policy evaluation each
         * time the policyIndex changes for an instance containing this class
         *
         * NOTE: the last (or only) policy is evaluated at the end of this
         *       function, just like the other 'case' blocks
         */
        if (prevPolicyIndex != policyIndex)
        {
          if (prevPolicyIndex != 0)
          {
            if (dsDistillerPolicyEvaluate(prevPolicyIndex, mode, evalIntIfNum,
                                          L7_FALSE) != L7_SUCCESS)
              return L7_FAILURE;
          }
          prevPolicyIndex = policyIndex;
        }
      } /* endwhile */
      /* set index to last affected policy in order to evaluate it below */
      policyIndex = prevPolicyIndex;
    }
    break;

  case DSMIB_TABLE_ID_POLICY:
    {
      dsmibPolicyEntryCtrl_t    *pPolicy;

      pPolicy = (dsmibPolicyEntryCtrl_t *)pRow;
      policyIndex = pPolicy->key.policyIndex;

      /* (nothing special to do here) */
    }
    break;

  case DSMIB_TABLE_ID_POLICY_INST:
  case DSMIB_TABLE_ID_POLICY_ATTR:
    {
      dsmibPolicyInstEntryCtrl_t  *pPolicyInst;

      /* set up Policy Instance Table row ptr */
      if (tableId == DSMIB_TABLE_ID_POLICY_INST)
        pPolicyInst = (dsmibPolicyInstEntryCtrl_t *)pRow;
      else
        pPolicyInst = ((dsmibPolicyAttrEntryCtrl_t *)pRow)->pPolicyInst;

      /* update 'isReady' state for this policy instance */
      policyIndex = pPolicyInst->key.policyIndex;
      policyInstIndex = pPolicyInst->key.policyInstIndex;
      pDsDstlCtrl->policy[policyIndex].instIsReady[policyInstIndex] =
        dsDistillerPolicyInstIsReady(policyIndex, policyInstIndex);
    }
    break;

  case DSMIB_TABLE_ID_POLICY_PERF_IN:
  case DSMIB_TABLE_ID_POLICY_PERF_OUT:
    /* (ignore all row status changes in these tables) */
    return L7_SUCCESS;
    /*PASSTHRU*/

  case DSMIB_TABLE_ID_SERVICE:
    {
      dsmibServiceEntryCtrl_t   *pService;

      pService = (dsmibServiceEntryCtrl_t *)pRow;

      /* obtain the policyIndex assigned to this service interface
       *
       * NOTE:  Proper procedure for deactivating a Service Table row
       *        mandates that the row status be set to 'not in service'
       *        before the policyIndex object value is altered.  Thus,
       *        a valid service row cannot contain a policy index of 0.
       */
      if ((diffServServiceObjectGet(pService->intIfNum, pService->key.ifDirection,
            L7_DIFFSERV_SERVICE_POLICY_INDEX, &policyIndex) != L7_SUCCESS) ||
          (policyIndex == 0))
        return L7_FAILURE;

      if (event == L7_ACTIVE)
      {
        /* if the row event is 'active' it means a new/same/diff policyIndex
         * has been assigned to the service interface, so insert a node for
         * the interface in the OLL for this policy (the policy is evaluated
         * below)
         */
        if (dsDistillerPolicyOLLInsert(policyIndex, pService->intIfNum)
             != L7_SUCCESS)
          return L7_FAILURE;
      }

      else
      {
        /* remove the interface node from the policy OLL per the policyIndex,
         * since it is no longer active on this service interface
         *
         * NOTE:  Must evaluate the policy before removing the OLL node so
         *        that all outstanding policy instances are cleared out of
         *        the low-level code.  The OLL node information is used for
         *        that processing.  Only evaluate the policy for this interface.
         */
        evalIntIfNum = pService->intIfNum;
        if (dsDistillerPolicyEvaluate(policyIndex, mode, evalIntIfNum, L7_FALSE)
              != L7_SUCCESS)
          return L7_FAILURE;

        if (dsDistillerPolicyOLLRemove(policyIndex, pService->intIfNum)
             != L7_SUCCESS)
          return L7_FAILURE;

        /* set the policy index to 0 to avoid re-evaluating the policy below */
        policyIndex = 0;
      }
    }
    break;

  default:
    return L7_FAILURE;
    /*PASSTHRU*/

  } /* endswitch */

  /* evaluate possible actions for this policy */
  if (policyIndex != 0)
  {
    if (dsDistillerPolicyEvaluate(policyIndex, mode, evalIntIfNum, L7_FALSE)
          != L7_SUCCESS)
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Obtain a policy instance key value for specified instance indexes
*
* @param    policyIndex     @{(input)}  Policy index
* @param    policyInstIndex @{(input)}  Policy instance index
* @param    pKey            @{(output)} Pointer to key output location
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    For Distiller internal use only.
*
* @end
*********************************************************************/
L7_RC_t dsDistillerInstanceKeyGet(L7_uint32 policyIndex,
                                  L7_uint32 policyInstIndex,
                                  L7_uint32 *pKey)
{
  /* check inputs */
  if (DSMIB_POLICY_INDEX_OUT_OF_RANGE(policyIndex))
    return L7_FAILURE;
  if (DSMIB_POLICY_INST_INDEX_OUT_OF_RANGE(policyInstIndex))
    return L7_FAILURE;
  if (pKey == L7_NULLPTR)
    return L7_FAILURE;

  dsDstlTlvInstToKey(policyIndex, policyInstIndex, pKey);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine if the policy is considered 'ready' for
*           operation based on readiness of its policy instances
*
* @param    policyIndex     @{(input)} Policy index
*
* @returns  L7_TRUE     Is ready for operation
* @returns  L7_FALSE    Not ready
*
* @notes    Policy is 'ready' if and only if:  the policy row is active
*           and all instances within this policy are active (at least one
*           such instance must exist).
*
* @end
*********************************************************************/
L7_BOOL dsDistillerPolicyIsReady(L7_uint32 policyIndex)
{
  dsmibRowStatus_t  rowStatus;
  L7_uint32         numInst;
  L7_uint32         policyInstIndex, nextPolicy;

  /* get policy row status */
  if (diffServPolicyObjectGet(policyIndex, L7_DIFFSERV_POLICY_ROW_STATUS,
                              &rowStatus) != L7_SUCCESS)
    return L7_FALSE;

  if (rowStatus != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE)
    return L7_FALSE;

  /* walk the Policy Instance Table and for each instance belonging to
   * this policy, check its 'isReady' flag in the Distiller control struct
   *
   * at least one instance must be defined, and all defined instances must
   * have their 'isReady' flag set to True for this policy to be considered
   * 'ready'
   */
  numInst = 0;
  policyInstIndex = 0;                          /* start with first instance */
  while ((diffServPolicyInstGetNext(policyIndex, policyInstIndex,
                          &nextPolicy, &policyInstIndex) == L7_SUCCESS) &&
         (nextPolicy == policyIndex))
  {
    numInst++;
    if (pDsDstlCtrl->policy[policyIndex].instIsReady[policyInstIndex] !=
          L7_TRUE)
      return L7_FALSE;
  }
  if (numInst == 0)
    return L7_FALSE;

  /* the policy is considered 'ready' */
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Determine if the policy instance is considered 'ready' for
*           operation based on the status of its composite rows
*
* @param    policyIndex     @{(input)} Policy index
* @param    policyInstIndex @{(input)} Policy instance index
*
* @returns  L7_TRUE     Is ready for operation
* @returns  L7_FALSE    Not ready
*
* @notes    Policy instance is 'ready' if and only if:  the instance is
*           active, all policy attributes are active (if any exist), the
*           class is active and all class rules are active (at least one
*           class rule must exist).
*
* @end
*********************************************************************/
L7_BOOL dsDistillerPolicyInstIsReady(L7_uint32 policyIndex,
                                     L7_uint32 policyInstIndex)
{
  dsmibRowStatus_t      rowStatus;
  L7_uint32             policyAttrIndex;
  L7_uint32             nextPolicy, nextInst;
  L7_uint32             classIndex, classRuleIndex;
  L7_uint32             prevClassIndex;
  L7_BOOL               foundFirstRule;
  dsmibClassRuleType_t  ruleEntryType;

  /* check policy instance row status */
  if ((diffServPolicyInstObjectGet(policyIndex, policyInstIndex,
                                   L7_DIFFSERV_POLICY_INST_ROW_STATUS,
                                   &rowStatus) != L7_SUCCESS) ||
      (rowStatus != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE))
    return L7_FALSE;

  /* check all policy attribute row statuses for this instance
   *
   * NOTE: an absence of any attributes implies a 'best-effort' policy,
   *       which is okay here
   */
  policyAttrIndex = 0;                          /* start with first attribute */
  while ((diffServPolicyAttrGetNext(policyIndex, policyInstIndex,
           policyAttrIndex, &nextPolicy, &nextInst, &policyAttrIndex)
           == L7_SUCCESS) &&
         (nextPolicy == policyIndex) &&
         (nextInst == policyInstIndex))
  {
    if ((diffServPolicyAttrObjectGet(policyIndex, policyInstIndex,
                                     policyAttrIndex,
                                     L7_DIFFSERV_POLICY_ATTR_ROW_STATUS,
                                     &rowStatus) != L7_SUCCESS) ||
        (rowStatus != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE))
      return L7_FALSE;
  }

  /* get policy instance class index */
  if (diffServPolicyInstObjectGet(policyIndex, policyInstIndex,
                                  L7_DIFFSERV_POLICY_INST_CLASS_INDEX,
                                  &classIndex) != L7_SUCCESS)
    return L7_FALSE;

  /* check all class rule row statuses for this class (and its reference chain)
   *
   * NOTE: a valid class definition requires the existence of at least
   *       one class rule (i.e., even a 'match all' or 'match none'
   *       classifier definition needs a rule)
   */
  foundFirstRule = L7_FALSE;

  prevClassIndex = 0;                           /* for detecting class changes*/
  classRuleIndex = 0;                           /* start with first rule */
  while (dsmibClassRuleRefChainGetNext(classIndex, classRuleIndex, &classIndex,
                                       &classRuleIndex) == L7_SUCCESS)
  {
    /* check the class row status each time a different class is detected */
    if (classIndex != prevClassIndex)
    {
      if ((diffServClassObjectGet(classIndex,
                                  L7_DIFFSERV_CLASS_ROW_STATUS,
                                  &rowStatus) != L7_SUCCESS) ||
          (rowStatus != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE))
        return L7_FALSE;

      prevClassIndex = classIndex;
    }

    if ((diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                    L7_DIFFSERV_CLASS_RULE_ROW_STATUS,
                                    &rowStatus) != L7_SUCCESS) ||
        (rowStatus != L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE))
      return L7_FALSE;

    /* don't count a refclass rule as a valid classifier rule here (what it
     * references will be processed later in this loop)
     */
    if ((diffServClassRuleObjectGet(classIndex, classRuleIndex,
                                    L7_DIFFSERV_CLASS_RULE_MATCH_ENTRY_TYPE,
                                    &ruleEntryType) != L7_SUCCESS) ||
        (ruleEntryType == L7_USMDB_MIB_DIFFSERV_CLASS_RULE_TYPE_REFCLASS))
      continue;

    foundFirstRule = L7_TRUE;

  } /* endwhile */

  if (foundFirstRule == L7_FALSE)
    return L7_FALSE;

  /* reaching this point means all composite rows are 'active' for this
   * instance, so the instance is considered 'ready'
   */
  return L7_TRUE;
}

/*********************************************************************
* @purpose  Determine if a TLV is currently issued for a policy instance
*           on the specified interface
*
* @param    policyIndex     @{(input)} Policy index
* @param    policyInstIndex @{(input)} Policy instance index
* @param    intIfNum        @{(input)} Internal interface number
*
* @returns  L7_TRUE     TLV is currently issued
* @returns  L7_FALSE    TLV not currently issued
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL dsDistillerPolicyInstIsIssued(L7_uint32 policyIndex,
                                      L7_uint32 policyInstIndex,
                                      L7_uint32 intIfNum)
{
  L7_BOOL           rc = L7_FALSE;
  L7_uint32         ifIndex, instKey;
  dsDstlIntfNode_t  node, *pNode;
  L7_uint32         i;

  /* check if the policy instance exists */
  if (diffServPolicyInstGet(policyIndex, policyInstIndex) != L7_SUCCESS)
    return L7_FALSE;

  /* convert intIfNum to ifIndex */
  if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
    return L7_FALSE;

  /* translate the policy instance index to its instance key */
  dsDstlTlvInstToKey(policyIndex, policyInstIndex, &instKey);

  /* find the OLL node, if any, and check the 'TLV issued' array for this
   * instance key on this interface
   */
  memset(&node, 0, sizeof(node));
  node.key.policyIndex = policyIndex;
  node.key.ifIndex = ifIndex;

  DIFFSERV_SEMA_TAKE(pDsDstlCtrl->ollSemId, L7_WAIT_FOREVER);

  pNode = (dsDstlIntfNode_t *)avlSearchLVL7(pDsDstlCtrl->pOLL, &node,
                                            AVL_EXACT);

  if (pNode != L7_NULLPTR)
  {
    /* NOTE: entry 0 not used */
    for (i = 1; i <= L7_DIFFSERV_INST_PER_POLICY_LIM; i++)
    {
      if (pNode->tlvIssued[i] == instKey)
      {
        /* found it -- instance is currently issued on this interface */
        rc = L7_TRUE;
        break;
      }
    }
  }

  DIFFSERV_SEMA_GIVE(pDsDstlCtrl->ollSemId);

  return rc;
}

/*********************************************************************
* @purpose  Get policy direction from its index
*
* @param    policyIndex   @{(input)}  Policy index
* @param    pIfDirection  @{(output)} Pointer to interface direction output
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsDistillerPolicyDirGet(L7_uint32 policyIndex,
                                dsmibServiceIfDir_t *pIfDirection)
{
  dsmibPolicyType_t     policyType;

  /* derive ifDirection from policy type */
  if (diffServPolicyObjectGet(policyIndex, L7_DIFFSERV_POLICY_TYPE,
                              &policyType) != L7_SUCCESS)
    return L7_FAILURE;

  *pIfDirection = (policyType == L7_USMDB_MIB_DIFFSERV_POLICY_TYPE_IN) ?
                   L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_IN :
                   L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_OUT;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Insert an interface node into the ordered linked list for
*           the specified policy
*
* @param    policyIndex @{(input)} Policy index
* @param    intIfNum    @{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsDistillerPolicyOLLInsert(L7_uint32 policyIndex, L7_uint32 intIfNum)
{
  L7_uint32             ifIndex;
  dsmibServiceIfDir_t   ifDirection;
  dsDstlIntfNode_t      node, *pNode;
  L7_uint32             i;

  /* convert intIfNum to ifIndex */
  if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
    return L7_FAILURE;

  /* derive ifDirection from policy */
  if (dsDistillerPolicyDirGet(policyIndex, &ifDirection) != L7_SUCCESS)
    return L7_FAILURE;

  /* set up the node information for this policy and interface */
  memset(&node, 0, sizeof(node));
  node.key.policyIndex = policyIndex;
  node.key.ifIndex = ifIndex;
  node.intIfNum = intIfNum;
  node.ifDirection = ifDirection;
  node.intfLinkRate = 0;                        /* updated during policy eval */
  for (i = 1; i <= L7_DIFFSERV_INST_PER_POLICY_LIM; i++)
    node.tlvIssued[i] = L7_FALSE;

  DIFFSERV_SEMA_TAKE(pDsDstlCtrl->ollSemId, L7_WAIT_FOREVER);
  pNode = (dsDstlIntfNode_t *)avlInsertEntry(pDsDstlCtrl->pOLL, &node);
  DIFFSERV_SEMA_GIVE(pDsDstlCtrl->ollSemId);

  /* a non-null pNode means the node already exists or there an error occurred*/
  return (pNode == L7_NULLPTR) ? L7_SUCCESS : L7_FAILURE;
}

/*********************************************************************
* @purpose  Remove the service interface node from the specified policy OLL
*
* @param    policyIndex @{(input)} Policy index
* @param    intIfNum    @{(input)} Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Requires all still-issued TLV instance information be deleted
*           from the low-level code prior to calling this function.
*
* @end
*********************************************************************/
L7_RC_t dsDistillerPolicyOLLRemove(L7_uint32 policyIndex, L7_uint32 intIfNum)
{
  L7_uint32             ifIndex;
  dsDstlIntfNode_t      node, *pNode;
  L7_uint32             i;
  L7_RC_t               rc = L7_SUCCESS;

  /* convert intIfNum to ifIndex */
  if (nimGetIntfIfIndex(intIfNum, &ifIndex) != L7_SUCCESS)
    return L7_FAILURE;

  /* set up a search node for this policy and interface */
  memset(&node, 0, sizeof(node));
  node.key.policyIndex = policyIndex;
  node.key.ifIndex = ifIndex;

  DIFFSERV_SEMA_TAKE(pDsDstlCtrl->ollSemId, L7_WAIT_FOREVER);

  pNode = (dsDstlIntfNode_t *)avlSearchLVL7(pDsDstlCtrl->pOLL, &node, AVL_EXACT);

  /* delete the OLL node if it was found
   * NOTE: if node not found, treat the node deletion operation as a success
   */
  if (pNode != L7_NULLPTR)
  {
    /* there should not be any outstanding TLVs for this node */
    for (i = 1; i <= L7_DIFFSERV_INST_PER_POLICY_LIM; i++)
    {
      L7_assert(pNode->tlvIssued[i] != DSDSTL_TLV_INST_KEY_NOT_ISSUED);
    }

    /* delete the policy OLL node entry for the specified interface
     * NOTE: a successful deletion returns a pointer to the item deleted
     */
    if (avlDeleteEntry(pDsDstlCtrl->pOLL, pNode) != pNode)
      rc = L7_FAILURE;
  }

  DIFFSERV_SEMA_GIVE(pDsDstlCtrl->ollSemId);

  return rc;
}

/*********************************************************************
* @purpose  Evaluate the current readiness state of a policy and decide
*           whether any TLVs need to be sent to the low-level code
*
* @param    policyIndex   @{(input)} Policy index
* @param    mode          @{(input)} DiffServ admin mode (L7_ENABLE, L7_DISABLE)
* @param    evalIntIfNum  @{(input)} Internal interface number to evaluate
* @param    reissueTlv    @{(input)} Flag to force TLV to be reissued
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    Be careful not to query the actual DiffServ administrative
*           mode here, since for the 'disable' case, the configuration
*           value is not updated in the MIB until AFTER all disable
*           processing occurs.  Base all decisions on the mode parm instead.
*
* @notes    This function is analogous to the 'apply' functions used in
*           other components.
*
* @notes    An evalIntIfNum parm value of DSMIB_POLICY_EVAL_ALL_INTF means
*           the policy is to be evaluated for all interfaces (i.e., all
*           interface OLL node).  Any other value implies evaluation is only
*           performed for the specified interface number.
*
* @end
*********************************************************************/
L7_RC_t dsDistillerPolicyEvaluate(L7_uint32 policyIndex, L7_uint32 mode,
                                  L7_uint32 evalIntIfNum, L7_BOOL reissueTlv)
{
  L7_uint32             intIfNum = 0;
  dsmibServiceIfDir_t   ifDirection = L7_USMDB_MIB_DIFFSERV_IF_DIRECTION_NONE;
  dsmibRowStatus_t      serviceRowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
  L7_BOOL               needTearDown = L7_FALSE;
  L7_BOOL               needBuildUp = L7_FALSE;
  L7_RC_t               rc, rc2;
  L7_BOOL               policyIsReady;
  L7_uint32             linkState;
  L7_uint32             linkRate;
  dsDstlPolicyInfo_t    *pPolInfo;
  dsDstlIntfNode_t      node, *pNode;
  L7_INTF_STATES_t      intfState;

  /* to support pre-configuration, return w/success if component not ready */
  if (diffServIsReady() != L7_TRUE)
    return L7_SUCCESS;

  /* bypass all policy evaluation while in DiffServ "special config mode"
   *
   * NOTE:  This is done for performance reasons.  When the DiffServ config
   *        processing has completed its work, it exits the special config
   *        mode before setting the admin mode to enable.
   */
  if (dsmibSpecialConfigModeGet() != L7_FALSE)
    return L7_SUCCESS;

  pPolInfo = &pDsDstlCtrl->policy[policyIndex];

  policyIsReady = (mode == L7_ENABLE) ?
                    dsDistillerPolicyIsReady(policyIndex) : L7_FALSE;

  /* now that the preliminaries are out of the way....
   *
   * walk the policy's OLL to determine which service interfaces may need
   * updating
   */
  memset(&node, 0, sizeof(node));
  node.key.policyIndex = policyIndex;
  node.key.ifIndex = 0;                         /* start with first interface */
  pNode = &node;

  rc = rc2 = L7_SUCCESS;                        /* default init for while loop*/

  /* walk entire OLL for this policy */
  while (1)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];

    /* init for each pass through the loop */
    linkState = L7_INACTIVE;
    linkRate = 0;

    /* get the OLL node */
    DIFFSERV_SEMA_TAKE(pDsDstlCtrl->ollSemId, L7_WAIT_FOREVER);
    pNode = (dsDstlIntfNode_t *)avlSearchLVL7(pDsDstlCtrl->pOLL, pNode, AVL_NEXT);
    DIFFSERV_SEMA_GIVE(pDsDstlCtrl->ollSemId);

    /* If there are no more OLL for this policy, we're done */
    if ((pNode == L7_NULLPTR) || (pNode->key.policyIndex != policyIndex))
      break;

    /* get intf number and direction from the node element */
    intIfNum    = pNode->intIfNum;
    ifDirection = pNode->ifDirection;
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);

    /* Check the interface state */
    if (diffServIsIntfAttached(intIfNum, &intfState) != L7_TRUE)
    {
      /*
       * this interface is not in a state that should be handling the
       * policies. Get the next OLL row.
       */
       continue; /* goto while(1) */
    }

    /* process this interface if evaluating all or it matches the eval intf */
    if ((evalIntIfNum == DSMIB_POLICY_EVAL_ALL_INTF) ||
        (evalIntIfNum == intIfNum))
    {
      /* get the service interface row status */
      if (diffServServiceObjectGet(intIfNum, ifDirection,
                                   L7_DIFFSERV_SERVICE_ROW_STATUS,
                                   &serviceRowStatus) != L7_SUCCESS)
      {
        /* don't return with a failure here -- this could be an indication that
         * the service interface is no longer configured -- just substitute a
         * row status value that is not 'active'
         */
        serviceRowStatus = L7_USMDB_MIB_DIFFSERV_ROW_STATUS_NONE;
      }

      /* get the current interface link state and data rate
       * (if data rate get fails, just use a rate of zero)
       *
       * NOTE:  If interface is currently acquired by a LAG, leave the
       *        link state inactive so that the operational status is
       *        treated as 'down'.
       */
      if (diffServIsIntfAcquired(intIfNum) == L7_FALSE)
      {
        if (nimGetIntfActiveState(intIfNum, &linkState) != L7_SUCCESS)
        {
          linkState = L7_INACTIVE;
          break;                                    /* failed */
        }

        if (nimGetIntfSpeedSimplexDataRate(intIfNum, &linkRate) != L7_SUCCESS)
          linkRate = 0;
      }

      DS_TRACE_PT_POL_EVAL_INST(policyIndex, intIfNum, ifDirection, linkState,
                                policyIsReady, serviceRowStatus, mode, reissueTlv);

      /* initialize policy evaluation TLV action flags */
      needTearDown = needBuildUp = L7_FALSE;

      /* based on all the available information, determine the action to
       * perform on behalf of the service policy
       */
      if ((policyIsReady == L7_TRUE) &&
          (serviceRowStatus == L7_USMDB_MIB_DIFFSERV_ROW_STATUS_ACTIVE) &&
          (intfState != L7_INTF_DETACHING))
      {
        L7_BOOL       attachIsValid;
        L7_uint32     i;

        /* only worry about issuing TLVs when the link is active
         *
         * NOTE:  For a link not-active scenario, the intent is to not "churn"
         *        the low-level code by issuing TLV deletes and adds just because
         *        the link cycles down and then up again.  If the service policy
         *        configuration is otherwise unchanged, defer any TLV activity
         *        until the link is active, at which time old TLVs will be
         *        deleted, if necessary.
         *
         * NOTE:  Likewise, an interface that is acquired and released by a LAG
         *        acts the same as when the link for that interface goes down
         *        and up, respectively.
         */
        if (linkState == L7_ACTIVE)
        {
          /* check for any constraints within the policy that could prevent
           * proper operation on this particular interface
           */
          if ((dsmibPolicyAttachConstraintCheck(policyIndex, intIfNum,
                                                ifDirection, &attachIsValid)
                != L7_SUCCESS) ||
              (attachIsValid != L7_TRUE))
          {
            L7_uchar8  nameStr[L7_USMDB_MIB_DIFFSERV_POLICY_NAME_MAX+1];

            if (diffServPolicyObjectGet(policyIndex, L7_DIFFSERV_POLICY_NAME,
                                        &nameStr) != L7_SUCCESS)
            {
              sprintf((char *)nameStr, "index %u", policyIndex);
            }
            L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                    "Policy invalid for service intf: "
                    "policy \'%s\', intf %s, direction %u."
                    " The DiffServ policy definition is not compatible"
                    " with the capabilities of the interface specified.  "
                    "Check the platform release notes for information on "
                    "configuration limitations.",
                    (char *)nameStr, intIfNum, ifDirection);

            /* only want tear down without a corresponding build up */
            needTearDown = L7_TRUE;

            /* remember deferred return code for function exit */
            rc2 = L7_FAILURE;
          }

          else  /* policy attachment valid */
          {
            /* look for an interface link rate change, which indicates that a
             * valid policy needs to be re-issued since some of its attributes
             * may depend on the link rate
             *
             * NOTE: The linkRate was set to zero if the NIM call failed above,
             *       although this should not happen when the link is up.
             */
            if (linkRate != pNode->intfLinkRate)
            {
              pNode->intfLinkRate = linkRate;

              needTearDown = L7_TRUE;

              if (linkRate != 0)
                needBuildUp = L7_TRUE;
            }

            else  /* link rate unchanged */
            {
              /* Build Up Case */

              /* with one exception, the latest policy definition must be provided
               * to the low-level code, and preceeded by a tear down operation if
               * there are any leftover, still-issued instance keys
               *
               * the exception:  ALL instance 'isReady' flag settings EXACTLY
               * match their corresponding TLV issued status (i.e.,
               * 'isReady'=False when tlvIssued[inst]==0, or 'isReady'=True
               * when tlvIssued[inst]!=0), meaning nothing has changed;
               * neither tear down nor build up actions are necessary in this
               * case (unless 'reissueTlv' flag is set)
               */

              for (i = 1; i <= L7_DIFFSERV_INST_PER_POLICY_LIM; i++)
              {
                if (pNode->tlvIssued[i] == DSDSTL_TLV_INST_KEY_NOT_ISSUED)
                {
                  /* TLV has not been issued, but instance is 'ready */
                  if (pPolInfo->instIsReady[i] == L7_TRUE)
                    needBuildUp = L7_TRUE;
                }

                else
                {
                  needTearDown = L7_TRUE;

                  /* TLV has been issued, but instance is 'not ready'
                   * or 'reissueTlv' flag is set
                   */
                  if ((pPolInfo->instIsReady[i] == L7_FALSE) ||
                      (reissueTlv == L7_TRUE))
                    needBuildUp = L7_TRUE;
                }
              } /* endfor */

              /* to handle the exception case, cancel a tear down if no
               * build up is needed
               */
              if ((needTearDown == L7_TRUE) && (needBuildUp == L7_FALSE))
              {
                /* NOTE:  This is the special exception case, so don't
                 *        do the tear down operation.  Also, since this
                 *        means one or more TLVs are active in the device,
                 *        signal the service interface operational status
                 *        as now 'up'.
                 */
                needTearDown = L7_FALSE;
                if (dsmibServiceIfOperStatusUp(intIfNum, ifDirection) != L7_SUCCESS)
                  return L7_FAILURE;
              }

            } /* endelse link rate unchanged */

          } /* endelse policy attachment valid */

        } /* endif link is active */

        else  /* link is inactive */
        {
          if (reissueTlv == L7_TRUE)
          {
            /* force a changed policy to be torn down now even though the link
             * is currently inactive (otherwise, this change might be missed
             * later when the link becomes active)
             */
            needTearDown = L7_TRUE;
          }

          /* make sure service interface operational status is 'down' whenever
           * link is down
           */
          if (dsmibServiceIfOperStatusDown(intIfNum, ifDirection) != L7_SUCCESS)
            return L7_FAILURE;

        } /* endelse link is inactive */

      } /* endif service policy ready */

      else
      {
        /* Tear Down Cases */

        /* tear down all still-issued instance keys for this service policy */
        needTearDown = L7_TRUE;
      }

      DIFFSERV_PRT(DIFFSERV_MSGLVL_LO_2,
                   "DiffServ: Eval policy=%u intf=%u, %s reissue=%u tearDown=%u buildUp=%u\n",
                   policyIndex, intIfNum, ifName, reissueTlv, needTearDown, needBuildUp);

      /* now check the result of the two local flags from above
       * (if a tear down is needed, do it before the build up)
       */
      rc = L7_SUCCESS;                          /* re-init for this sequence */
      if (needTearDown == L7_TRUE)
        rc = dsDistillerPolicyTearDown(policyIndex, intIfNum, ifDirection);

      if (rc == L7_SUCCESS)
        if (needBuildUp == L7_TRUE)
          rc = dsDistillerPolicyBuildUp(policyIndex, intIfNum, ifDirection);

      /* check result of any build up or tear down operation during this pass */
      if (rc != L7_SUCCESS)
      {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_FLEX_QOS_DIFFSERV_COMPONENT_ID,
                "%s: build up or tear down error for "
                "policy %u, intf %s, direction %u\n",
                __FUNCTION__, policyIndex, ifName, ifDirection);
        break;
      }

      /* check for non-successful deferred rc */
      if (rc2 != L7_SUCCESS)
      {
        rc = rc2;
        break;
      }

    } /* endif eval all or matching interface */

  } /* endwhile AVL search */

  DS_TRACE_PT_POL_EVAL_EXIT(policyIndex, intIfNum, ifDirection, linkState,
                            policyIsReady, serviceRowStatus, mode, reissueTlv,
                            rc, needBuildUp, needTearDown);

  return rc;
}

/*********************************************************************
* @purpose  Build up the service policy information in the low-level
*           code and construct the standard MIB data path
*
* @param    policyIndex @{(input)} Policy index
* @param    intIfNum    @{(input)} Internal interface number
* @param    ifDirection @{(input)} Interface direction
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes    A separate TLV is used for each policy-class instance
*           within the specified policy.
*
* @end
*********************************************************************/
L7_RC_t dsDistillerPolicyBuildUp(L7_uint32 policyIndex, L7_uint32 intIfNum,
                                 dsmibServiceIfDir_t ifDirection)
{
  L7_RC_t             rc;

  DS_TRACE_PT_POL_BUILD_UP(policyIndex, intIfNum, ifDirection);

#ifdef L7_INCLUDE_DIFFSERV_STD_MIB_SUPPORT
  /* construct the data path model in the standard MIB */
  if (diffServDataPathCreate(intIfNum, ifDirection) != L7_SUCCESS)
    return L7_FAILURE;
#endif

  /* inform the low-level code about all instances of this policy */
  if ((rc = dsDstlTlvPolicyAdd(policyIndex, intIfNum, ifDirection))
       != L7_SUCCESS)
  {
    /* undo any partial buildup that may have occurred
     * NOTE: ignore the return code from tear down
     */
    (void)dsDistillerPolicyTearDown(policyIndex, intIfNum, ifDirection);
    /* use return code from policy add operation */
    return rc;
  }

  /* set the service interface operational status to 'up' */
  if (dsmibServiceIfOperStatusUp(intIfNum, ifDirection) != L7_SUCCESS)
    return L7_FAILURE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Tear down the service policy information in the low-level
*           code and destroy the standard MIB data path
*
* @param    policyIndex @{(input)} Policy index
* @param    intIfNum    @{(input)} Internal interface number
* @param    ifDirection @{(input)} Interface direction
*
* @returns  L7_SUCCESS
* @returns  L7_ERROR    Problem with TLV
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsDistillerPolicyTearDown(L7_uint32 policyIndex, L7_uint32 intIfNum,
                                  dsmibServiceIfDir_t ifDirection)
{
  L7_RC_t                 rc;

  DS_TRACE_PT_POL_TEAR_DOWN(policyIndex, intIfNum, ifDirection);

  /* delete all instances of this policy from the low-level code */
  if ((rc = dsDstlTlvPolicyDel(policyIndex, intIfNum, ifDirection))
       != L7_SUCCESS)
    return rc;

  /* set the service interface operational status to 'down' */
  if (dsmibServiceIfOperStatusDown(intIfNum, ifDirection) != L7_SUCCESS)
    return L7_FAILURE;

#ifdef L7_INCLUDE_DIFFSERV_STD_MIB_SUPPORT
  /* destroy the data path model in the standard MIB
   *
   * NOTE: Expects called function to successfully handle case where
   *       data path is already torn down from a prior invocation.
   */
  rc = diffServDataPathDelete(intIfNum, ifDirection);
#endif

  return rc;
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
L7_RC_t dsDistillerPolicyTlvGet(L7_uint32 policyIndex, L7_uint32 intIfNum,
                                dsmibServiceIfDir_t ifDirection, L7_tlvHandle_t tlvHandle)
{
  return(dsDstlTlvPolicyCombinedTlvGet(policyIndex, intIfNum, ifDirection, tlvHandle));
}

/*********************************************************************
* @purpose  Display contents of DiffServ Distiller control structure
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
L7_RC_t dsDistillerCtrlShow(void)
{
  L7_uint32           msgLvlReqd;
  dsDstlCtrl_t        *p = pDsDstlCtrl;
  L7_uint32           i, j;
  dsDstlPolicyInfo_t  *pPolInfo;
  dsDstlIntfNode_t    node, *pNode;
  L7_BOOL             showEntry;
  L7_BOOL             policyReady;
  char                *pInstReadyStr;

  #define LIST_COLS   10                        /* number of colums to print */

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  if (p == L7_NULLPTR)
  {
    DIFFSERV_PRT(msgLvlReqd,
                 "\n\nDiffServ Distiller Ctrl Struct does not exist.\n");
    return L7_FAILURE;
  }


  DIFFSERV_PRT(msgLvlReqd,
               "\nDisplaying Distiller Ctrl struct from location 0x%8.8x:\n",
               (L7_uint32)p);

  /* general fields */
  DIFFSERV_PRT(msgLvlReqd,
               "  Service intf OLL tree anchor: 0x%8.8x:\n",
               (L7_uint32)p->pOLL);
  DIFFSERV_PRT(msgLvlReqd,
               "  OLL mutex semaphore id      : 0x%8.8x:\n",
               (L7_uint32)p->ollSemId);
  DIFFSERV_PRT(msgLvlReqd,
               "  TLV block handle            : 0x%8.8x:\n",
               (L7_uint32)p->tlvHandle);

  /* policy information */
  for (i = 1; i <= L7_DIFFSERV_POLICY_LIM; i++)
  {
    pPolInfo = &p->policy[i];

    /* only display a policy index entry if there is at least one ready
     * instance, or the policy is attached to at least one service intf
     */
    showEntry = L7_FALSE;

    for (j = 1; j <= L7_DIFFSERV_INST_PER_POLICY_LIM; j++)
    {
      if (pPolInfo->instIsReady[j] == L7_TRUE)
      {
        showEntry = L7_TRUE;
        break;
      }
    } /* endfor j */

    if (showEntry != L7_TRUE)
    {
      memset(&node, 0, sizeof(node));
      node.key.policyIndex = i;
      node.key.ifIndex = 0;

      pNode = (dsDstlIntfNode_t *)avlSearchLVL7(pDsDstlCtrl->pOLL, &node,
                                                AVL_NEXT);
      if ((pNode != L7_NULLPTR) && (pNode->key.policyIndex == i))
        showEntry = L7_TRUE;
    }

    if (showEntry == L7_TRUE)
    {
      policyReady = dsDistillerPolicyIsReady(i);

      DIFFSERV_PRT(msgLvlReqd,
                   "  Information for policy index %u:\n", i);

      DIFFSERV_PRT(msgLvlReqd,
                   "    policy ready    : %1s\n",
                   (policyReady == L7_TRUE) ? "Y" : "n");

      /* display each instIsReady flag for the policy */
      DIFFSERV_PRT(msgLvlReqd,
                   "    instIsReady list: ")
      for (j = 1; j <= L7_DIFFSERV_INST_PER_POLICY_LIM; j++)
      {
        if (diffServPolicyInstGet(i, j) == L7_SUCCESS)
          pInstReadyStr = (pPolInfo->instIsReady[j] == L7_TRUE) ? "Y" : "n";
        else
          pInstReadyStr = "-";
        DIFFSERV_PRT(msgLvlReqd, "%1s ", pInstReadyStr);
        if (((j % LIST_COLS) == 0) && (j != L7_DIFFSERV_INST_PER_POLICY_LIM))
        {
          DIFFSERV_PRT(msgLvlReqd, "\n"
                   "                      ");
        }
      } /* endfor j */

      DIFFSERV_PRT(msgLvlReqd, "\n");

      /* walk entire OLL for this policy */
      DIFFSERV_SEMA_TAKE(pDsDstlCtrl->ollSemId, L7_WAIT_FOREVER);

      memset(&node, 0, sizeof(node));
      node.key.policyIndex = i;
      node.key.ifIndex = 0;
      pNode = &node;

      while (((pNode = (dsDstlIntfNode_t *)
                avlSearchLVL7(pDsDstlCtrl->pOLL, pNode, AVL_NEXT)) != L7_NULLPTR)
             && (pNode->key.policyIndex == i))
      {
        /* display the tlvIssued list for this interface */
        DIFFSERV_PRT(msgLvlReqd,
                     "    Intf %u instance key issued list: \n\t", pNode->intIfNum);
        for (j = 1; j <= L7_DIFFSERV_INST_PER_POLICY_LIM; j++)
        {
          DIFFSERV_PRT(msgLvlReqd, "%4u ", pNode->tlvIssued[j]);
          if (((j % LIST_COLS) == 0) && (j != L7_DIFFSERV_INST_PER_POLICY_LIM))
          {
            DIFFSERV_PRT(msgLvlReqd, "\n\t");
          }
        } /* endfor j */

        DIFFSERV_PRT(msgLvlReqd, "\n");

      } /* endwhile */

      DIFFSERV_SEMA_GIVE(pDsDstlCtrl->ollSemId);

    } /* endif showEntry is true */

  } /* endfor i */

  DIFFSERV_PRT(msgLvlReqd, "\n");

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Display contents of DiffServ Distiller Policy OLL tree
*
* @param    policyIndex @{(input)} Policy index (use 0 to display all)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    A policyIndex parameter value of 0 is used to display the
*           entire contents of the OLL tree.
*
* @end
*********************************************************************/
L7_RC_t dsDistillerOLLShow(L7_uint32 policyIndex)
{
  L7_uint32         msgLvlReqd;
  dsDstlIntfNode_t  node, *pNode;
  L7_uint32         prevPolicy;

  /* always display output for this function */
  msgLvlReqd = DIFFSERV_MSGLVL_ON;

  DIFFSERV_SEMA_TAKE(pDsDstlCtrl->ollSemId, L7_WAIT_FOREVER);

  DIFFSERV_PRT(msgLvlReqd,
               "\nDistiller OLL Contents: \n");
  DIFFSERV_PRT(msgLvlReqd,
               "  Policy  Service Interface(s)\n");
  DIFFSERV_PRT(msgLvlReqd,
               "  ------  -------------------------------------------");

  memset(&node, 0, sizeof(node));
  node.key.policyIndex = policyIndex;           /* start w/ specified policy  */
  node.key.ifIndex = 0;                         /* start w/ first interface   */
  pNode = &node;

  /* walk entire OLL */
  prevPolicy = 0;
  while (((pNode = (dsDstlIntfNode_t *)
            avlSearchLVL7(pDsDstlCtrl->pOLL, pNode, AVL_NEXT)) != L7_NULLPTR))
  {
    /* restrict output to a specific policy if the policyIndex is nonzero */
    if ((policyIndex != 0) &&
        (pNode->key.policyIndex != policyIndex))
      break;                                    /* no more for this policy */

    /* display list of interfaces for each policy in OLL */
    if (pNode->key.policyIndex != prevPolicy)
    {
      /* print policy header line */
      DIFFSERV_PRT(msgLvlReqd, "\n");           /* terminate previous row */
      DIFFSERV_PRT(msgLvlReqd, "    %3u   ", pNode->key.policyIndex);
      prevPolicy = pNode->key.policyIndex;
    }
    DIFFSERV_PRT(msgLvlReqd, "%u ", pNode->intIfNum);

  } /* endwhile */

  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pDsDstlCtrl->ollSemId);

  return L7_SUCCESS;
}

