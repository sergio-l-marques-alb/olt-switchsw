/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_mac_db.c
*
* @purpose   dot1x mac address data store file
*
* @component dot1x
*
* @comments  none
*
* @create    11/12/2007
*
* @author    SNK
*
* @end
*
**********************************************************************/

#include "dot1x_include.h"
/*#include "l7_sll_api.h"
#include "buff_api.h"*/


/* Global parameters */
typedef struct dot1xMacAddrInfo_s{
    L7_sll_member_t          *next;
    L7_enetMacAddr_t         suppMacAddr;
    L7_uint32                lIntIfNum;
}dot1xMacAddrInfo_t;

static L7_uint32     dot1xMacAddrBufferPoolId =0;
static L7_sll_t      dot1xMacAddrSLL;
static osapiRWLock_t dot1xMacAddrDBRWLock;


/*************************************************************************
* @purpose  API to destroy the Mac Addr Info data node
*
* @param    ll_member  @b{(input)}  Linked list node containing the
*                                   Mac Addr Info to be destroyed
*
* @returns  L7_SUCCESS
*
* @comments This is called by SLL library when a node is being deleted
*
* @end
*************************************************************************/
L7_RC_t dot1xMacAddrDataDestroy (L7_sll_member_t *ll_member)
{
  dot1xMacAddrInfo_t *pMacAddrInfo ;

  pMacAddrInfo = ( dot1xMacAddrInfo_t *)ll_member;

  pMacAddrInfo->lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;

  bufferPoolFree(dot1xMacAddrBufferPoolId,(L7_uchar8 *)pMacAddrInfo);

  return L7_SUCCESS;
}
/*************************************************************************
* @purpose  Helper API to compare two Mac Addr Info nodes  and
*           returns the result
*
* @param     p  @b{(input)}  Pointer to Candidate 1 for comparison
* @param     q  @b{(input)}  Pointer to Candidate 2 for comparison
*
* @returns   0   p = q
* @returns  -1   p < q
* @returns  +1   p > q
*
* @comments This is called by SLL library when a nodes are compared
*
* @end
*************************************************************************/
L7_int32 dot1xMacAddrDataCmp(void *p, void *q, L7_uint32 key)
{
    L7_int32 cmp;
    cmp = memcmp(((dot1xMacAddrInfo_t *)p)->suppMacAddr.addr,((dot1xMacAddrInfo_t *)q)->suppMacAddr.addr,L7_ENET_MAC_ADDR_LEN);
    if (cmp == 0)
      return 0;
  else if (cmp < 0)
      return -1;
  else
      return 1;

}


/*********************************************************************
* @purpose  Initialize Mac Address Info Database
*
* @param    nodeCount    @b{(input)} The number of nodes to be created.
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dot1xMacAddrInfoDBInit(L7_uint32 nodeCount)
{
  /* Allocate the buffer pool */
  if (bufferPoolInit(nodeCount, sizeof(dot1xMacAddrInfo_t), "Dot1x Mac Addr Bufs",
                     &(dot1xMacAddrBufferPoolId)) != L7_SUCCESS)
  {
    L7_LOGF( L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
        "\n%s: Error allocating buffers for supplicant mac address database."
        " Could not allocate buffer pool for Mac address link list. Insufficient memory."
        ,__FUNCTION__);
    DOT1X_EVENT_TRACE(DOT1X_TRACE_MAC_ADDR_DB,0,"%s: Error allocating buffers for supplicant mac address database\n",__FUNCTION__);
    return L7_FAILURE;
  }

  /* Create the SLL*/
  if (SLLCreate(L7_DOT1X_COMPONENT_ID, L7_SLL_ASCEND_ORDER,
               sizeof(L7_enetMacAddr_t),dot1xMacAddrDataCmp ,dot1xMacAddrDataDestroy ,
               &dot1xMacAddrSLL) != L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
            "\n%s: Failed to create supplicant mac address linked list \n",__FUNCTION__);
    DOT1X_EVENT_TRACE(DOT1X_TRACE_MAC_ADDR_DB,0,"%s: Failed to create supplicant mac address linked list \n",__FUNCTION__);
    return L7_FAILURE;
  }

  /* Create Mac Address DB Semaphore*/
  /* Read write lock for controlling Mac Addr Info additions and Deletions */
  if (osapiRWLockCreate(&dot1xMacAddrDBRWLock,
                        OSAPI_RWLOCK_Q_PRIORITY) == L7_FAILURE)
  {
      L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
              "Error creating dot1qCfgRWlock semaphore \n");
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  DeInitialize Mac Address Info Database
*
* @param    none
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dot1xMacAddrInfoDBDeInit(void)
{



  /* Destroy the SLL */
  if (SLLDestroy(L7_DOT1X_COMPONENT_ID, &dot1xMacAddrSLL)!= L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DOT1X_COMPONENT_ID,
            "\n%s: Failed to destroy the supplicant mac address linked list \n",__FUNCTION__);
    DOT1X_EVENT_TRACE(DOT1X_TRACE_MAC_ADDR_DB,0,"\n%s: Failed to destroy the supplicant mac address linked list \n",__FUNCTION__);
  }

  /* Deallocate the buffer pool */

  if (dot1xMacAddrBufferPoolId  != 0)
  {
    bufferPoolDelete(dot1xMacAddrBufferPoolId );
    dot1xMacAddrBufferPoolId  = 0;
  }

  /* Delete the Mac Address DB Semaphore */
  (void)osapiRWLockDelete(dot1xMacAddrDBRWLock);


  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Add a node Mac Address Info Database
*
* @param    mac_addr  supplicant mac address
* @param    lIntIfNum corresponding logical interface
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dot1xMacAddrInfoAdd(L7_enetMacAddr_t *mac_addr,L7_uint32 lIntIfNum)
{
   dot1xMacAddrInfo_t *pMacAddrInfo,*pMacAddrFind,macAddrInfo;
   L7_enetMacAddr_t    nullMacAddr;

   memset(&(nullMacAddr.addr),0,L7_ENET_MAC_ADDR_LEN);


   if (mac_addr==L7_NULL)
   {
       DOT1X_EVENT_TRACE(DOT1X_TRACE_MAC_ADDR_DB,dot1xPhysPortGet(lIntIfNum),"\n%s: Could not add supplicant mac address. Mac address is NULL. Input error. \n",
               __FUNCTION__);
       return L7_FAILURE;
   }
   if((lIntIfNum == DOT1X_LOGICAL_PORT_ITERATE)||
        (memcmp(mac_addr->addr,nullMacAddr.addr,L7_ENET_MAC_ADDR_LEN)==0))
   {
       DOT1X_EVENT_TRACE(DOT1X_TRACE_MAC_ADDR_DB,dot1xPhysPortGet(lIntIfNum),"\n%s: Could not add supplicant mac address(%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x) logical Interface: %d . Input error. \n",
               __FUNCTION__, mac_addr->addr[0],mac_addr->addr[1],mac_addr->addr[2],mac_addr->addr[3],mac_addr->addr[4],mac_addr->addr[5],lIntIfNum);
       return L7_FAILURE;
   }

   /* In order to handle client roaming , check if the mac address already exists*/
   /* copy info to node*/
    memcpy(macAddrInfo.suppMacAddr.addr,mac_addr->addr,L7_ENET_MAC_ADDR_LEN);
   /* take Mac address DB semaphore*/
   (void)osapiWriteLockTake(dot1xMacAddrDBRWLock, L7_WAIT_FOREVER);

   if ((pMacAddrFind=(dot1xMacAddrInfo_t *)SLLFind(&dot1xMacAddrSLL,(L7_sll_member_t *)&macAddrInfo)) != L7_NULLPTR)
   {
       pMacAddrFind->lIntIfNum = lIntIfNum;
      (void) osapiWriteLockGive(dot1xMacAddrDBRWLock);
       DOT1X_EVENT_TRACE(DOT1X_TRACE_MAC_ADDR_DB,lIntIfNum,"\n%s: Found supplicant mac address(%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x) Changed logical Interface to: %d .\n",
               __FUNCTION__, mac_addr->addr[0],mac_addr->addr[1],mac_addr->addr[2],mac_addr->addr[3],mac_addr->addr[4],mac_addr->addr[5],lIntIfNum);
       return L7_SUCCESS;
   }

   /* Allocate memory from the buffer pool*/
   if (bufferPoolAllocate(dot1xMacAddrBufferPoolId,
                            (L7_uchar8 **)&pMacAddrInfo) != L7_SUCCESS)
     {
       /* release semaphore*/
       (void)osapiWriteLockGive(dot1xMacAddrDBRWLock);

       L7_LOGF( L7_LOG_SEVERITY_NOTICE, L7_DOT1X_COMPONENT_ID,
           "\n%s: Could not add supplicant mac address(%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x) logical Interface: %d . Insufficient memory. \n",
               __FUNCTION__, mac_addr->addr[0],mac_addr->addr[1],mac_addr->addr[2],mac_addr->addr[3],mac_addr->addr[4],mac_addr->addr[5],lIntIfNum);

       DOT1X_EVENT_TRACE(DOT1X_TRACE_MAC_ADDR_DB,dot1xPhysPortGet(lIntIfNum),"\n%s: Could not add supplicant mac address(%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x) logical Interface: %d . Insufficient memory. \n",
               __FUNCTION__, mac_addr->addr[0],mac_addr->addr[1],mac_addr->addr[2],mac_addr->addr[3],mac_addr->addr[4],mac_addr->addr[5],lIntIfNum);

       return L7_FAILURE;
     }

   /* copy details into allocated node*/
   memset(pMacAddrInfo,0,sizeof(dot1xMacAddrInfo_t));
   memcpy(pMacAddrInfo->suppMacAddr.addr,mac_addr->addr,L7_ENET_MAC_ADDR_LEN);
   pMacAddrInfo->lIntIfNum = lIntIfNum;


   /* Add node to the SLL*/
   if (SLLAdd(&dot1xMacAddrSLL, (L7_sll_member_t *)pMacAddrInfo)
         != L7_SUCCESS)
   {
       DOT1X_EVENT_TRACE(DOT1X_TRACE_MAC_ADDR_DB,dot1xPhysPortGet(lIntIfNum),"\n%s: Could not add supplicant mac address(%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x) logical Interface: %d . \n",
               __FUNCTION__, mac_addr->addr[0],mac_addr->addr[1],mac_addr->addr[2],mac_addr->addr[3],mac_addr->addr[4],mac_addr->addr[5],lIntIfNum);
       /* Free the previously allocated bufferpool */
       bufferPoolFree(dot1xMacAddrBufferPoolId, (L7_uchar8 *)pMacAddrInfo);
       /* release semaphore*/
       (void)osapiWriteLockGive(dot1xMacAddrDBRWLock);
       return L7_FAILURE;
   }

    /* release semaphore*/
    (void)osapiWriteLockGive(dot1xMacAddrDBRWLock);
    return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Remove a node from the Mac Address Info Database
*
* @param    mac_addr  supplicant mac address
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dot1xMacAddrInfoRemove(L7_enetMacAddr_t *mac_addr)
{
   dot1xMacAddrInfo_t macAddrInfo;
   L7_enetMacAddr_t    nullMacAddr;

   memset(&nullMacAddr.addr,0,L7_ENET_MAC_ADDR_LEN);
   /*input check*/
   if ((mac_addr == L7_NULLPTR) ||
        (memcmp(mac_addr->addr,nullMacAddr.addr,L7_ENET_MAC_ADDR_LEN)==0))
   {
       return L7_FAILURE;
   }
   /* copy info to node*/
   memcpy(macAddrInfo.suppMacAddr.addr,mac_addr->addr,L7_ENET_MAC_ADDR_LEN);
   macAddrInfo.lIntIfNum = 0;

   /* take Mac address DB semaphore*/
   (void)osapiWriteLockTake(dot1xMacAddrDBRWLock, L7_WAIT_FOREVER);

    /* delete from SLL*/
    if (SLLDelete(&dot1xMacAddrSLL, (L7_sll_member_t *)&macAddrInfo)
                  != L7_SUCCESS)
    {
        /* release semaphore*/
       (void)osapiWriteLockGive(dot1xMacAddrDBRWLock);

        DOT1X_EVENT_TRACE(DOT1X_TRACE_MAC_ADDR_DB,0,"\n%s: Could not delete supplicant mac address(%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x) from the SLL . \n",
               __FUNCTION__, mac_addr->addr[0],mac_addr->addr[1],mac_addr->addr[2],mac_addr->addr[3],mac_addr->addr[4],mac_addr->addr[5]);
       return L7_FAILURE;
    }

  /* release semaphore*/
  (void)osapiWriteLockGive(dot1xMacAddrDBRWLock);
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Find a node in the Mac Address Info Database
*
* @param    mac_addr            supplicant mac address
* @param   lIntIfnum{(output)} logical Port Number
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dot1xMacAddrInfoFind(L7_enetMacAddr_t *mac_addr,L7_uint32 *lIntIfNum)
{
  dot1xMacAddrInfo_t macAddrInfo,*pMacAddrInfo;
  L7_enetMacAddr_t    nullMacAddr;

  memset(&nullMacAddr.addr,0,L7_ENET_MAC_ADDR_LEN);
  /*input check*/
  if ((mac_addr == L7_NULLPTR) ||
      (memcmp(mac_addr->addr,nullMacAddr.addr,L7_ENET_MAC_ADDR_LEN)==0))
  {
      return L7_FAILURE;
  }
  /* copy info to node*/
  memcpy(macAddrInfo.suppMacAddr.addr,mac_addr->addr,L7_ENET_MAC_ADDR_LEN);

  /* take Mac address DB semaphore*/
   (void)osapiWriteLockTake(dot1xMacAddrDBRWLock, L7_WAIT_FOREVER);

  if ((pMacAddrInfo=(dot1xMacAddrInfo_t *)SLLFind(&dot1xMacAddrSLL,(L7_sll_member_t *)&macAddrInfo)) == L7_NULLPTR)
  {
      /* release semaphore*/
     (void)osapiWriteLockGive(dot1xMacAddrDBRWLock);
      DOT1X_EVENT_TRACE(DOT1X_TRACE_MAC_ADDR_DB,0,"\n%s: Could not find supplicant mac address(%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x). \n",
               __FUNCTION__, mac_addr->addr[0],mac_addr->addr[1],mac_addr->addr[2],mac_addr->addr[3],mac_addr->addr[4],mac_addr->addr[5]);
      *lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
      return L7_FAILURE;
  }
  *lIntIfNum = pMacAddrInfo->lIntIfNum;
  /* release semaphore*/
  (void)osapiWriteLockGive(dot1xMacAddrDBRWLock);
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Find the next node in the Mac Address Info Database
*
* @param    mac_addr            supplicant mac address
* @param   lIntIfnum{(output)}  logical Port Number
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dot1xMacAddrInfoFindNext(L7_enetMacAddr_t *mac_addr,L7_uint32 *lIntIfNum)
{
  dot1xMacAddrInfo_t macAddrInfo,*pMacAddrInfo;

  /*input check*/
  if (mac_addr == L7_NULLPTR)
  {
      return L7_FAILURE;
  }

  /* copy info to node*/
  memcpy(macAddrInfo.suppMacAddr.addr,mac_addr->addr,L7_ENET_MAC_ADDR_LEN);

   /* take Mac address DB semaphore*/
   (void)osapiWriteLockTake(dot1xMacAddrDBRWLock, L7_WAIT_FOREVER);

  if ((pMacAddrInfo=(dot1xMacAddrInfo_t *)SLLFindNext(&dot1xMacAddrSLL,(L7_sll_member_t *)&macAddrInfo)) == L7_NULLPTR)
  {
      /* release semaphore*/
      (void)osapiWriteLockGive(dot1xMacAddrDBRWLock);

      DOT1X_EVENT_TRACE(DOT1X_TRACE_MAC_ADDR_DB,0,"\n%s: Could not find next node for supplicant mac address(%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x). \n",
               __FUNCTION__, mac_addr->addr[0],mac_addr->addr[1],mac_addr->addr[2],mac_addr->addr[3],mac_addr->addr[4],mac_addr->addr[5]);
      *lIntIfNum = DOT1X_LOGICAL_PORT_ITERATE;
      return L7_FAILURE;
  }
  memcpy(mac_addr->addr,pMacAddrInfo->suppMacAddr.addr,L7_ENET_MAC_ADDR_LEN);
  *lIntIfNum = pMacAddrInfo->lIntIfNum;

  /* release semaphore*/
  (void)osapiWriteLockGive(dot1xMacAddrDBRWLock);

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Print the information in the Mac Address Database
*
* @param
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @comments none
*
* @end
*********************************************************************/
L7_RC_t dot1xDebugMacAddrDBList()
{
    L7_enetMacAddr_t macAddr,nullMacAddr;
    L7_uint32 lIntIfNum;

    memset(&macAddr,0,sizeof(L7_enetMacAddr_t));

    while(dot1xMacAddrInfoFindNext(&macAddr,&lIntIfNum)== L7_SUCCESS)
    {
        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n Mac Address: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
                      macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);

        SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n Logical Port :%u",lIntIfNum);

    }

     memset(&nullMacAddr,0,sizeof(L7_enetMacAddr_t));
     if (memcmp(macAddr.addr,nullMacAddr.addr,L7_ENET_MAC_ADDR_LEN)!=0)
     {
         if (dot1xMacAddrInfoFind(&macAddr,&lIntIfNum)==L7_SUCCESS)
         {
             SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n Testing dot1xMacAddrInfoFind.Found \n");
             SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n Mac Address: %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
                      macAddr.addr[0],macAddr.addr[1],macAddr.addr[2],macAddr.addr[3],macAddr.addr[4],macAddr.addr[5]);

            SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,"\n Logical Port :%u",lIntIfNum);
         }
     }
    return L7_SUCCESS;
}
