/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename   diffserv_stdmib_debug.c
*
* @purpose    DiffServ Standard MIB table debug 'show' APIs
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
**********************************************************************/
#include <string.h>

#include "l7_diffserv_include.h"

/* external declaration for AVL tree data array */
extern avlTree_t   dsmibAvlTree[];

/* external Aux MF Clfr reuse matrix */
extern dsStdMibAuxMFClfrReuse_t   dsAuxReuseMatrix_g[L7_DIFFSERV_CLASS_LIM+1]
                                                    [L7_DIFFSERV_RULE_PER_CLASS_LIM+1];

char *dsStdMibTableStr[] = 
{
  "--", "Data Path", "Clfr", "Clfr Elem", "MF Clfr","Aux MF Clfr", 
  "Meter", "TB Param", "Color Aware", 
  "Action", "Mark Cos", "Mark Cos2",  "Mark Dscp", "Mark Prec", 
  "Count Act", "Assign Queue", "Redirect", "Mirror", 
  "Alg Drop", "Random Drop", 
  "Queue", "Scheduler", "Min Rate", "Max Rate"
};
char *dsStdMibRowPtrStr[] = 
{
  "--", "DataPath", "Clfr", "ClfrElem", "MFClfr","AuxMFClfr", 
  "Meter", "TBParam", "ColorAware", 
  "Action", "MarkCos", "MarkCos2",  "MarkDscp", "MarkPrec", 
  "CountAct", "AssignQ", "Redirect", "Mirror", 
  "AlgDrop", "RandDrop", 
  "Queue", "Sched", "MinRate", "MaxRate"
};
char *dsStdMibIfDirStr[]  = 
{
  "--", "In", "Out"
};
char *dsStdMibTBMeterTypeStr[] = 
{
  "--", "Simple", "AvgRate", "SrTcmBlind",
  "SrTcmAware", "TrTcmBlind", "TrTcmAware",
  "TswTcm"
};
char *dsStdMibColorLevelStr[] = 
{
  "none   ", "conform", "exceed "
};
char *dsStdMibColorModeStr[] = 
{
  "none  ", "blind ", "cos   ", "cos2  ", "ipdscp", "ipprec", "unused"
};
char *dsStdMibAlgDropTypeStr[] = 
{
  "--", "Other", "Tail", "Head", "Random","Always"
};
                                                                                          
char *dsStdMibSchedulerTypeStr[] = 
{
  "--", "SP ", "WRR", "WFQ"
};

/* always want to display output for 'show' functions */
static  L7_uint32 msgLvlReqd = DIFFSERV_MSGLVL_ON;


/*********************************************************************
* @purpose  Display max capacity of all AVL trees used by DiffServ 
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
void dsStdMibAvlTableSizeShow(void)
{
  L7_uint32     tableId, count, max = 0;
  char          *pStr;
  avlTree_t     *pTree;
  L7_BOOL       ignore;

  for (tableId = (L7_uint32)(DSSTDMIB_TABLE_ID_NONE+1); 
       tableId < DSSTDMIB_TABLE_ID_TOTAL; 
       tableId++)
  {
    ignore = L7_FALSE;

    switch (tableId)
    {
    case DSSTDMIB_TABLE_ID_DATA_PATH:
      max = (L7_uint32)DSSTDMIB_DATAPATH_TABLE_SIZE_MAX;
      break;

    case DSSTDMIB_TABLE_ID_CLFR:
      max = (L7_uint32)DSSTDMIB_CLFR_TABLE_SIZE_MAX;
      break;

    case DSSTDMIB_TABLE_ID_CLFR_ELEMENT:
      max = (L7_uint32)DSSTDMIB_CLFR_ELEMENT_TABLE_SIZE_MAX;
      break;

    case DSSTDMIB_TABLE_ID_MULTI_FIELD_CLFR:
      max = (L7_uint32)DSSTDMIB_MULTI_FIELD_CLFR_TABLE_SIZE_MAX;
      break;

    case DSSTDMIB_TABLE_ID_AUX_MF_CLFR:
      max = (L7_uint32)DSSTDMIB_AUX_MF_CLFR_TABLE_SIZE_MAX;
      break;

    case DSSTDMIB_TABLE_ID_METER:
      max = (L7_uint32)DSSTDMIB_METER_TABLE_SIZE_MAX;
      break;

    case DSSTDMIB_TABLE_ID_TB_PARAM:
      max = (L7_uint32)DSSTDMIB_TBPARAM_TABLE_SIZE_MAX;
      break;

    case DSSTDMIB_TABLE_ID_COLOR_AWARE:
      /* uses Meter table AVL tree */
      ignore = L7_TRUE;
      break;

    case DSSTDMIB_TABLE_ID_ACTION:
      max = (L7_uint32)DSSTDMIB_ACTION_TABLE_SIZE_MAX;
      break;

    case DSSTDMIB_TABLE_ID_MARK_COS_ACT:
    case DSSTDMIB_TABLE_ID_MARK_COS_AS_COS2_ACT:    
    case DSSTDMIB_TABLE_ID_MARK_COS2_ACT:
    case DSSTDMIB_TABLE_ID_MARK_IPDSCP_ACT:
    case DSSTDMIB_TABLE_ID_MARK_IPPREC_ACT:
      /* no AVL tree used for these tables */
      ignore = L7_TRUE;
      break;

    case DSSTDMIB_TABLE_ID_COUNT_ACT:
      max = (L7_uint32)DSSTDMIB_COUNT_ACT_TABLE_SIZE_MAX;
      break;

    case DSSTDMIB_TABLE_ID_ASSIGN_QUEUE:
      max = (L7_uint32)DSSTDMIB_ASSIGN_QUEUE_TABLE_SIZE_MAX;
      break;

    case DSSTDMIB_TABLE_ID_REDIRECT:
      max = (L7_uint32)DSSTDMIB_REDIRECT_TABLE_SIZE_MAX;
      break;

    case DSSTDMIB_TABLE_ID_MIRROR:
      max = (L7_uint32)DSSTDMIB_MIRROR_TABLE_SIZE_MAX;
      break;

    case DSSTDMIB_TABLE_ID_ALG_DROP:
      max = (L7_uint32)DSSTDMIB_ALG_DROP_TABLE_SIZE_MAX;
      break;

    default:
      ignore = L7_TRUE;
      break;

    } /* endswitch */

    if (ignore == L7_TRUE)
      continue;

    pStr = dsStdMibTableStr[tableId];
    pTree = &dsStdMibAvlTree[tableId];

    DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);
    count = avlTreeCount(pTree);
    DIFFSERV_SEMA_GIVE(pTree->semId);

    DIFFSERV_PRT(msgLvlReqd, 
                 "%-15.15s Table size curr/max.............  %u/%u\n",
                 pStr, count, max);

  } /* endfor tableId */
}

/*********************************************************************
* @purpose  Display the current DiffServ Classifier Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibDataPathTableShow(void)
{
  dsStdMibDataPathEntryCtrl_t *pRow;
  dsStdMibDataPathKey_t       keys;
  avlTree_t     *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_DATA_PATH];
  L7_uint32     count;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Data Path Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Data Path Table (contains %u entries):\n\n", count);

  DIFFSERV_PRT(msgLvlReqd, "Indexes   Start Element(Clfr)Id\n");
  DIFFSERV_PRT(msgLvlReqd, "-------   --------------------\n");

  keys.ifIndex = 0;
  keys.ifDirection = 0;
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    DIFFSERV_PRT(msgLvlReqd, "%2u %-3s            %4u \n", pRow->key.ifIndex, 
                 dsStdMibIfDirStr[pRow->key.ifDirection], pRow->mib.start.rowIndex1);
    /* update search keys for next pass */
    keys = pRow->key;
  }
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}


/*********************************************************************
* @purpose  Display the current DiffServ Classifier Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibClfrTableShow(void)
{
  dsStdMibClfrEntryCtrl_t *pRow;
  dsStdMibClfrKey_t       keys;
  avlTree_t     *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_CLFR];
  L7_uint32     count;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Classifier Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Classifier Table (contains %u entries):\n\n", 
              count);

  DIFFSERV_PRT(msgLvlReqd, "Clfr Id PrecAllExcl PrecAll  PrecAny \n");
  DIFFSERV_PRT(msgLvlReqd, "------- ----------- -------- --------\n");

  keys.clfrId = 0;
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    DIFFSERV_PRT(msgLvlReqd, " %4u     %8u  %8u %8u\n",
                 pRow->mib.id, pRow->precedenceAllExcl, pRow->precedenceAll,
                 pRow->precedenceAny);

    /* update search keys for next pass */
    keys = pRow->key;
  }
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
  return;
}
                             
/*********************************************************************
* @purpose  Display the current DiffServ Classifier Element Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibClfrElementTableShow(void)
{
  dsStdMibClfrElementEntryCtrl_t *pRow;
  dsStdMibClfrElementKey_t       keys;
  avlTree_t     *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_CLFR_ELEMENT];
  L7_uint32     count;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Classifier Element Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Classifier Element Table (contains %u entries):\n\n", 
              count);
  
  DIFFSERV_PRT(msgLvlReqd, "Indexes Precedence      Next        auxMFClfrId\n");
  DIFFSERV_PRT(msgLvlReqd, "------- ---------- --------------   ------------\n");

  keys.clfrId = 0;                          /* start with first entry */
  keys.clfrElemId = 0;                      /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    DIFFSERV_PRT(msgLvlReqd, " %2u %2u %8u    %9s(%3u)        %3u\n", 
                 pRow->key.clfrId, 
                 pRow->key.clfrElemId,
                 pRow->mib.precedence,
                 dsStdMibRowPtrStr[pRow->mib.next.tableId],
                 pRow->mib.next.rowIndex1,
                 pRow->mib.specific.rowIndex1);
    
    /* update search keys for next pass */
    keys = pRow->key;
  }
  
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

/*********************************************************************
* @purpose  Display the current DiffServ Auxiliary Multi Field Classifier 
*           Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibAuxMFClfrTableShow(void)
{
  dsStdMibAuxMFClfrEntryCtrl_t  *pRow;
  dsStdMibAuxMFClfrKey_t        keys;
  dsStdMibAuxMFClfrReuse_t      *pMatrix;
  avlTree_t     *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_AUX_MF_CLFR];
  L7_uint32     count;
  L7_uint32     i;
  L7_uchar8     ipv6AddrString[64];

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Auxiliary MF Classifier Table is empty\n\n");
    return;
  }
  
  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Auxiliary MF Classifier Table (contains %u entries):\n\n", count);

  keys.auxMFClfrId = 0;
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    pMatrix = &dsAuxReuseMatrix_g[pRow->classIndex][pRow->classRuleIndex];

    DIFFSERV_PRT(msgLvlReqd, "Id=%u   (maps to classIndex=%u classRuleIndex=%u, refCount=%u)\n", 
                   pRow->key.auxMFClfrId, pRow->classIndex, pRow->classRuleIndex, 
                   pMatrix->refCount);
    DIFFSERV_PRT(msgLvlReqd, "------\n");


    DIFFSERV_PRT(msgLvlReqd, "cos          :  %d\n", pRow->mib.cos); 

    DIFFSERV_PRT(msgLvlReqd, "cos2         :  %d\n", pRow->mib.cos2); 

    DIFFSERV_PRT(msgLvlReqd, "dstip        :  addr=0x%8.8x  mask=0x%8.8x\n",
                 pRow->mib.dstAddr, pRow->mib.dstMask);

    osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&pRow->mib.ipv6DstAddr, ipv6AddrString, sizeof(ipv6AddrString));
    DIFFSERV_PRT(msgLvlReqd, "dstipv6      :  addr=%s   pref_len=0x%8.8x\n",
                 ipv6AddrString, pRow->mib.ipv6DstPlen);

    DIFFSERV_PRT(msgLvlReqd, "ipv6 flowlbl :  %u\n", pRow->mib.ipv6FlowLabel);

    DIFFSERV_PRT(msgLvlReqd, "dstl4port    :  start=%u  end=%u\n",
                 pRow->mib.dstL4PortMin, pRow->mib.dstL4PortMax);

    DIFFSERV_PRT(msgLvlReqd, "dstmac       :  addr=");
    for (i = 0; i < L7_MAC_ADDR_LEN; i++)
    {
      DIFFSERV_PRT(msgLvlReqd, "%s%2.2x", 
                   (i == 0) ? "" : ":", pRow->mib.dstMac[i]);
    }
    DIFFSERV_PRT(msgLvlReqd, "  mask=");
    for (i = 0; i < L7_MAC_ADDR_LEN; i++)
    {
      DIFFSERV_PRT(msgLvlReqd, "%s%2.2x", 
                   (i == 0) ? "" : ":", pRow->mib.dstMacMask[i]);
    }
    DIFFSERV_PRT(msgLvlReqd, "\n");

    DIFFSERV_PRT(msgLvlReqd, "etype        :  val1=0x%4.4x  val2=0x%4.4x\n",
                 pRow->mib.etypeVal1, pRow->mib.etypeVal2);

    DIFFSERV_PRT(msgLvlReqd, "protocol     :  %u\n", pRow->mib.protocol); 

    DIFFSERV_PRT(msgLvlReqd, "tos          :  val=0x%2.2x  mask=0x%2.2x\n", 
                 pRow->mib.tos, pRow->mib.tosMask);

    DIFFSERV_PRT(msgLvlReqd, "srcip        :  addr=0x%8.8x  mask=0x%8.8x\n",
                 pRow->mib.srcAddr, pRow->mib.srcMask);

    osapiInetNtop(L7_AF_INET6, (L7_uchar8 *)&pRow->mib.ipv6SrcAddr, ipv6AddrString, sizeof(ipv6AddrString));
    DIFFSERV_PRT(msgLvlReqd, "srcipv6      :  addr=%s   pref_len=0x%8.8x\n",
                 ipv6AddrString, pRow->mib.ipv6SrcPlen);

    DIFFSERV_PRT(msgLvlReqd, "srcl4port    :  start=%u  end=%u\n",
                 pRow->mib.srcL4PortMin, pRow->mib.srcL4PortMax);

    DIFFSERV_PRT(msgLvlReqd, "srcmac       :  addr=");
    for (i = 0; i < L7_MAC_ADDR_LEN; i++)
    {
      DIFFSERV_PRT(msgLvlReqd, "%s%2.2x", 
                   (i == 0) ? "" : ":", pRow->mib.srcMac[i]);
    }
    DIFFSERV_PRT(msgLvlReqd, "  mask=");
    for (i = 0; i < L7_MAC_ADDR_LEN; i++)
    {
      DIFFSERV_PRT(msgLvlReqd, "%s%2.2x", 
                   (i == 0) ? "" : ":", pRow->mib.srcMacMask[i]);
    }
    DIFFSERV_PRT(msgLvlReqd, "\n");

    DIFFSERV_PRT(msgLvlReqd, "vlanid       :  start=%u  end=%u\n",
                 pRow->mib.vlanIdMin, pRow->mib.vlanIdMax); 

    DIFFSERV_PRT(msgLvlReqd, "vlanid2      :  start=%u  end=%u\n",
                 pRow->mib.vlanId2Min, pRow->mib.vlanId2Max); 

    DIFFSERV_PRT(msgLvlReqd, "\n");

    /* update search keys for next pass */
    keys = pRow->key;
  }

  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

/*********************************************************************
* @purpose  Display the current DiffServ Auxiliary Multi Field Classifier 
*           reuse matrix contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibAuxReuseMatrixShow(void)
{
  avlTree_t                 *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_AUX_MF_CLFR];
  dsStdMibAuxMFClfrReuse_t  *pMatrix;
  L7_uint32                 i, j;
  L7_BOOL                   needHdr;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Aux MF Clfr Reuse Matrix (only in-use entries shown):\n\n");

  DIFFSERV_PRT(msgLvlReqd, 
              "Matrix start address:  0x%8.8x\n\n", dsAuxReuseMatrix_g);

  for (i = 1; i <= L7_DIFFSERV_CLASS_LIM; i++)
  {
    needHdr = L7_TRUE;

    for (j = 1; j <= L7_DIFFSERV_RULE_PER_CLASS_LIM; j++)
    {
      pMatrix = &dsAuxReuseMatrix_g[i][j];

      if (pMatrix->auxMFClfrId != 0)
      {
        if (needHdr == L7_TRUE)
        {
          needHdr = L7_FALSE;
          DIFFSERV_PRT(msgLvlReqd, "\nClass index %u\n", i);
        }

        DIFFSERV_PRT(msgLvlReqd, 
                     "  Class rule index %2u:  auxMFClfrId=%4u  refCount=%4u\n", 
                     j, pMatrix->auxMFClfrId, pMatrix->refCount);
      }
    } /* endfor j */
  } /* endfor i */

  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

/*********************************************************************
* @purpose  Display the current DiffServ Meter Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibMeterTableShow(void)
{
  dsStdMibMeterEntryCtrl_t *pRow;
  dsStdMibMeterKey_t       keys;
  avlTree_t     *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_METER];
  L7_uint32     count;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Meter Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Meter Table (contains %u entries):\n\n", 
              count);
  
  DIFFSERV_PRT(msgLvlReqd, "Index     SucceedNext         FailNext           Specific   \n");
  DIFFSERV_PRT(msgLvlReqd, "-----    --------------    ---------------    --------------\n");

  keys.meterId = 0;                      /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    DIFFSERV_PRT(msgLvlReqd, " %2u      %9s(%3u)     %9s(%3u)     %9s(%3u)\n", 
                 pRow->key.meterId,
                 dsStdMibRowPtrStr[pRow->mib.succeedNext.tableId],
                 pRow->mib.succeedNext.rowIndex1,
                 dsStdMibRowPtrStr[pRow->mib.failNext.tableId],
                 pRow->mib.failNext.rowIndex1,
                 dsStdMibRowPtrStr[pRow->mib.specific.tableId],
                 pRow->mib.specific.rowIndex1);
    
    /* update search keys for next pass */
    keys = pRow->key;
  }
  
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

/*********************************************************************
* @purpose  Display the current DiffServ Token Bucket Param Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibTBParamTableShow(void)
{
  dsStdMibTBParamEntryCtrl_t *pRow;
  dsStdMibTBParamKey_t       keys;
  avlTree_t                  *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_TB_PARAM];
  L7_uint32                  count;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Token Bucket Param Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Token Bucket Param Table (contains %u entries):\n\n", 
              count);
  
  DIFFSERV_PRT(msgLvlReqd, "Index    Type             Rate    Burst Size  Interval\n");
  DIFFSERV_PRT(msgLvlReqd, "-----  ---------------  --------  ----------  --------\n");

  keys.tbParamId = 0;                      /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    DIFFSERV_PRT(msgLvlReqd, " %2u   %12s(%1u)  %8u   %7u     %5u  \n", 
                 pRow->key.tbParamId,
                 dsStdMibTBMeterTypeStr[pRow->mib.type], pRow->mib.type,
                 pRow->mib.rate, pRow->mib.burstSize,
                 pRow->mib.interval);
    
    /* update search keys for next pass */
    keys = pRow->key;
  }
  
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

/*********************************************************************
* @purpose  Display the current DiffServ Color Aware Table contents
*
* @param    void
*
* @returns  void
*
* @notes    Augments the Meter Table
*
* @end
*********************************************************************/
void dsStdMibColorAwareTableShow(void)
{
  dsStdMibMeterEntryCtrl_t  *pRow;
  dsStdMibMeterKey_t        keys;
  avlTree_t                 *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_METER];
  L7_uint32                 count;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Color Aware Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Color Aware Table (contains %u entries, uses Meter index):\n\n", 
              count);
  
  DIFFSERV_PRT(msgLvlReqd, "Index     Level       Mode    Value\n");
  DIFFSERV_PRT(msgLvlReqd, "-----  ----------  ---------  -----\n");

  keys.meterId = 0;                             /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    DIFFSERV_PRT(msgLvlReqd, " %2u    %7s(%1u)  %6s(%1u)   %2u  \n", 
                 pRow->key.meterId,
                 dsStdMibColorLevelStr[pRow->mibColor.level], pRow->mibColor.level,
                 dsStdMibColorModeStr[pRow->mibColor.mode], pRow->mibColor.mode,
                 pRow->mibColor.value);
    
    /* update search keys for next pass */
    keys = pRow->key;
  }
  
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

/*********************************************************************
* @purpose  Display the current DiffServ Action Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibActionTableShow(void)
{
  dsStdMibActionEntryCtrl_t  *pRow;
  dsStdMibActionKey_t        keys;
  avlTree_t                  *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ACTION];
  L7_uint32                  count;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Action Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Action Table (contains %u entries):\n\n", 
              count);
  
  DIFFSERV_PRT(msgLvlReqd, "Index   Iface             Next             Specific    \n");
  DIFFSERV_PRT(msgLvlReqd, "-----   -----       ----------------   ----------------\n");

  keys.actionId = 0;                      /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {

    DIFFSERV_PRT(msgLvlReqd, " %2u    %5u         %9s(%3u)     %9s(%3u)\n", 
                 pRow->key.actionId,
                 pRow->mib.interface,
                 dsStdMibRowPtrStr[pRow->mib.next.tableId],
                 pRow->mib.next.rowIndex1,
                 dsStdMibRowPtrStr[pRow->mib.specific.tableId],
                 pRow->mib.specific.rowIndex1);
    
    /* update search keys for next pass */
    keys = pRow->key;
  }
  
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

/*********************************************************************
* @purpose  Display the current DiffServ CountAction Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibCountActTableShow(void)
{
  dsStdMibCountActEntryCtrl_t  *pRow;
  dsStdMibCountActKey_t        keys;
  avlTree_t                    *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_COUNT_ACT];
  L7_uint32                    count;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Count Action Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Count Action Table (contains %u entries):\n\n", 
              count);
  
  DIFFSERV_PRT(msgLvlReqd, "Index       Octets             Packets      PolIdx Intf\n");
  DIFFSERV_PRT(msgLvlReqd, "----- ------------------ ------------------ ------ ----\n");

  
  keys.countActId = 0;                      /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {

    DIFFSERV_PRT(msgLvlReqd, " %2u   0x%8.8x%8.8x 0x%8.8x%8.8x %2u,%-2u  %3u \n", 
                 pRow->key.countActId,
                 pRow->mib.octets.high,
                 pRow->mib.octets.low,
                 pRow->mib.pkts.high,
                 pRow->mib.pkts.low,
                 pRow->ctrSource.policyIndex,
                 pRow->ctrSource.policyInstIndex,
                 pRow->ctrSource.intIfNum);
    
    /* update search keys for next pass */
    keys = pRow->key;
  }
  
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

/*********************************************************************
* @purpose  Display the current DiffServ Assign Queue Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibAssignQueueTableShow(void)
{
  dsStdMibAssignQueueEntryCtrl_t  *pRow;
  dsStdMibAssignQueueKey_t        keys;
  avlTree_t                       *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ASSIGN_QUEUE];
  L7_uint32                       count;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Assign Queue is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Assign Queue Table (contains %u entries):\n\n", 
              count);
  
  DIFFSERV_PRT(msgLvlReqd, "Index  Qnum\n");
  DIFFSERV_PRT(msgLvlReqd, "-----  ----\n");

  keys.assignId = 0;                            /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {

    DIFFSERV_PRT(msgLvlReqd, " %2u      %1u \n", 
                 pRow->key.assignId, 
                 pRow->mib.qNum);
    
    /* update search keys for next pass */
    keys = pRow->key;
  }
  
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

/*********************************************************************
* @purpose  Display the current DiffServ Redirect Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibRedirectTableShow(void)
{
  dsStdMibRedirectEntryCtrl_t *pRow;
  dsStdMibRedirectKey_t       keys;
  avlTree_t                   *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_REDIRECT];
  L7_uint32                   count;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Redirect is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Redirect Table (contains %u entries):\n\n", 
              count);
  
  DIFFSERV_PRT(msgLvlReqd, "Index  Intf\n");
  DIFFSERV_PRT(msgLvlReqd, "-----  ----\n");

  keys.redirectId = 0;                          /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {

    DIFFSERV_PRT(msgLvlReqd, " %2u    %3u \n", 
                 pRow->key.redirectId, 
                 pRow->mib.intIfNum);
    
    /* update search keys for next pass */
    keys = pRow->key;
  }
  
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

/*********************************************************************
* @purpose  Display the current DiffServ Mirror Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibMirrorTableShow(void)
{
  dsStdMibMirrorEntryCtrl_t   *pRow;
  dsStdMibMirrorKey_t         keys;
  avlTree_t                   *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_MIRROR];
  L7_uint32                   count;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Mirror is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Mirror Table (contains %u entries):\n\n", 
              count);
  
  DIFFSERV_PRT(msgLvlReqd, "Index  Intf\n");
  DIFFSERV_PRT(msgLvlReqd, "-----  ----\n");

  keys.mirrorId = 0;                            /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {

    DIFFSERV_PRT(msgLvlReqd, " %2u    %3u \n", 
                 pRow->key.mirrorId, 
                 pRow->mib.intIfNum);
    
    /* update search keys for next pass */
    keys = pRow->key;
  }
  
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

/*********************************************************************
* @purpose  Display the current DiffServ Algorithmic Drop Table contents
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsStdMibAlgDropTableShow(void)
{
  dsStdMibAlgDropEntryCtrl_t   *pRow;
  dsStdMibAlgDropKey_t         keys;
  avlTree_t                    *pTree = &dsStdMibAvlTree[DSSTDMIB_TABLE_ID_ALG_DROP];
  L7_uint32                    count;

  DIFFSERV_SEMA_TAKE(pTree->semId, L7_WAIT_FOREVER);

  if ((count = avlTreeCount(pTree)) == 0)
  {
    DIFFSERV_SEMA_GIVE(pTree->semId);
    DIFFSERV_PRT(msgLvlReqd, "\nDiffServ Algorithmic Drop Table is empty\n\n");
    return;
  }

  DIFFSERV_PRT(msgLvlReqd, 
              "\nDiffServ Algorithmic Drop Table (contains %u entries):\n\n", count);
  
  DIFFSERV_PRT(msgLvlReqd, "Index   Type           Next           Qmeasure     Thres     Specific    ");  
  DIFFSERV_PRT(msgLvlReqd, "Ctr:        Drop           Random Drop    \n");
  DIFFSERV_PRT(msgLvlReqd, "-----  ---------  --------------   --------------  -----  -------------- "); 
  DIFFSERV_PRT(msgLvlReqd, "---- ------------------ ------------------\n");
  
  keys.algDropId = 0;                      /* start with first entry */
  while ((pRow = avlSearchLVL7(pTree, &keys, AVL_NEXT)) != L7_NULLPTR)
  {
    /* part A */
    DIFFSERV_PRT(msgLvlReqd, " %2u    %6s(%1u)  %9s(%3u)   %9s(%3u)  %5u  %9s(%3u) ", 
                 pRow->key.algDropId, 
                 dsStdMibAlgDropTypeStr[pRow->mib.type], pRow->mib.type,
                 dsStdMibRowPtrStr[pRow->mib.next.tableId], 
                 pRow->mib.next.rowIndex1,
                 dsStdMibRowPtrStr[pRow->mib.qMeasure.tableId],
                 pRow->mib.qMeasure.rowIndex1,
                 pRow->mib.qThreshold,
                 dsStdMibRowPtrStr[pRow->mib.specific.tableId],
                 pRow->mib.specific.rowIndex1);

    /* part B -- octet counts */
    DIFFSERV_PRT(msgLvlReqd, "oct: 0x%8.8x%8.8x 0x%8.8x%8.8x\n", 
                 pRow->mib.octets.high, pRow->mib.octets.low,
                 pRow->mib.randomDropOctets.high, pRow->mib.randomDropOctets.low);

    /* part C -- packet counts */
    DIFFSERV_PRT(msgLvlReqd, "                                                                         "); /* spacer */
    DIFFSERV_PRT(msgLvlReqd, "pkt: 0x%8.8x%8.8x 0x%8.8x%8.8x\n", 
                 pRow->mib.pkts.high, pRow->mib.pkts.low,
                 pRow->mib.randomDropPkts.high, pRow->mib.randomDropPkts.low);

    /* update search keys for next pass */
    keys = pRow->key;
  }
  
  DIFFSERV_PRT(msgLvlReqd, "\n\n");

  DIFFSERV_SEMA_GIVE(pTree->semId);
}

