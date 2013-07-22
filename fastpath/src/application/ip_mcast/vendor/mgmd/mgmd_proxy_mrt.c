/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   mgmd_proxy_mrt.c
*
* @purpose    MGMD Proxy MRT Tree Database Maintenance Routines
*
* @component  MGMD
*
* @comments   none
*
* @create     
*
* @author     Kamlesh Agrawal
* @end
*
**********************************************************************/

/*******************************************************************************
**                             Includes                                       **
*******************************************************************************/

#include "l7_common.h"
#include "l3_addrdefs.h"
#include "l7apptimer_api.h"
#include "l3_mcast_defaultconfig.h"
#include "mcast_api.h"
#include "mgmd.h"
#include "mgmd_proxy.h"
#include "mgmd_proxy_debug.h"

static L7_RC_t 
mgmdProxyAdminScopeIntfBitSetGet (mgmd_cb_t *mgmdCB,
                                  L7_inet_addr_t *grpAddr,
                                  L7_uchar8      *oList);

static void mgmdProxyMRTCacheEntryUpdate(mgmd_cb_t         *mgmdCB, 
                                  L7_inet_addr_t    *srcAddr,
                                  mgmd_host_group_t *mgmdHostGroup,
                                  mgmdProxyCacheEntry_t *mrtEntry);

/*********************************************************************
*
* @purpose  Update the MFC Entry
*
* @param    mgmdCB  @b{ (input) } Pointer MGMD Control Block
*           srcAddr  @b{ (input) } Pointer Source Address
*           grpAddr  @b{ (input) } Pointer Group Address
*           rtrIfNum @b{ (input) } Incoming Interface
*           oList    @b{ (input) } Outgoing Interface List.
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None   
*
* @end
*********************************************************************/
static L7_RC_t mgmdProxyMfcEntryUpdate (mgmd_cb_t *mgmdCb,
                                        L7_inet_addr_t *srcAddr,
                                        L7_inet_addr_t *grpAddr,
                                        L7_uint32 rtrIfNum,
                                        L7_uchar8 *oList)
{
  mfcEntry_t mfcEntry;
  L7_RC_t retVal = L7_FAILURE; 

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");
  /* Basic Validations */
  if (L7_NULLPTR == mgmdCb)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Invalid MGMD Control Block\n");
    return L7_FAILURE;
  }

  memset (&mfcEntry,0,sizeof (mfcEntry));
  inetCopy (&mfcEntry.group,grpAddr);
  inetCopy (&mfcEntry.source,srcAddr);
  mfcEntry.iif = rtrIfNum;
  memcpy (&(mfcEntry.oif),oList,MGMD_INTF_BIT_SIZE);
  mfcEntry.mcastProtocol = L7_MRP_MGMD_PROXY;

  retVal = mfcMessagePost (MFC_UPDATE_ENTRY, (void *) &mfcEntry, 
                           sizeof(mfcEntry_t));

  return retVal;
} 


/*********************************************************************
*
* @purpose  Delete the MFC Entry
*
* @param    mgmdCB  @b{ (input) } Pointer MGMD Control Block
*           srcAddr  @b{ (input) } Pointer Source Address
*           grpAddr  @b{ (input) } Pointer Group Address
*           rtrIfNum @b{ (input) } Incoming Interface
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
static L7_RC_t mgmdProxyMfcEntryDelete (mgmd_cb_t *mgmdCb,
                                        L7_inet_addr_t *srcAddr,
                                        L7_inet_addr_t *grpAddr,
                                        L7_uint32 rtrIfNum)
{
  mfcEntry_t mfcEntry;
  L7_RC_t retVal = L7_FAILURE;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  memset (&mfcEntry,0,sizeof (mfcEntry));
  inetCopy (&mfcEntry.group,grpAddr);
  inetCopy (&mfcEntry.source,srcAddr);
  mfcEntry.iif = rtrIfNum;
  mfcEntry.mcastProtocol = L7_MRP_MGMD_PROXY;

  retVal = mfcMessagePost (MFC_DELETE_ENTRY, (void *) &mfcEntry, 
                           sizeof(mfcEntry_t));

  return retVal;
}

/*****************************************************************
* @purpose  It Updates the Multi cast forwarding cache 
*
* @param    mgmdCB           @b{ (input) }   MGMD Control Block
* @param    srcAddr          @b{ (input) }   Soource Address
* @param    mgmd_host_group  @b{ (input) }   MGMD Host Group
*
* @returns  None
*
* @notes  it update the MFC cache table
*
* @end
*********************************************************************/
void mgmdProxyMRTCacheEntryUpdate(mgmd_cb_t         *mgmdCB, 
                                  L7_inet_addr_t    *srcAddr,
                                  mgmd_host_group_t *mgmdHostGroup,
                                  mgmdProxyCacheEntry_t *mrtEntry)
{
  L7_uchar8                   oifList[MGMD_INTF_BIT_SIZE];
  L7_uchar8                   adminList[MGMD_INTF_BIT_SIZE];
  mgmd_host_source_record_t   *host_src_rec = L7_NULL;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  if (mgmdCB == L7_NULLPTR ||
      mgmdHostGroup == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Invalid MGMD Control Block\n");
    return ;
  }

  if (mrtEntry == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "MGMD proxy MRT entry not found\n");
    return ;
  }


  MCAST_BITX_RESET_ALL(oifList);
  MCAST_BITX_RESET_ALL(adminList);

  /* 1. Add (grp+src) interested Querier-IFs to children.
   * 2. Reset Proxy_If in children, 
   *         if ((PROXYfwdDISABLED) or (parent_if == Proxy_If)).
   * 3. Reset parent_If in children, if (parent_if == Proxy_If).
   */     

  mgmd_proxy_src_rec_get(&(mgmdHostGroup->sourceRecords),
                         *srcAddr, &host_src_rec);

  if (host_src_rec != L7_NULLPTR && host_src_rec->createFlag == L7_FALSE)
  {
    /* There exists a src record for the given source -
       need to use the SPL list to get the outgoing list */
    MCAST_BITX_COPY(host_src_rec->srcPortList.bits, oifList);
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, " Considering SPL for Oif ");
  }
  else
  {
    /* There exists no src record for the given source -
       need to use the GPL list to get the outgoing list */
    MCAST_BITX_COPY(mgmdHostGroup->grpPortList.bits, oifList);
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, " Considering GPL for Oif");
  }

  /* Apply the IGMP/MLD Querier Filter list */
  MCAST_BITX_AND(oifList,mgmdCB->mgmd_querier_if_mask.bits,oifList);

  /* Remove the incoming interface from the outgoing list */
  MCAST_BITX_RESET(oifList,mrtEntry->upstrmRtrIfNum);

  /* If (the upstream forwarding is enabled) and (Incoming interface is 
     not Proxy-Index) then Add proxy-index to outgoing list. */
  if ((mgmdCB->mgmd_host_info->mgmdProxyUpStreamDisable == L7_FALSE)
      && (mrtEntry->upstrmRtrIfNum != mgmdCB->mgmd_proxy_ifIndex))
  {
    MCAST_BITX_SET(oifList, mgmdCB->mgmd_proxy_ifIndex);
  }

  /* Apply - AdminScope boundary  */
  MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS, " admin scope grp = ",&mgmdHostGroup->group);
  mgmdProxyAdminScopeIntfBitSetGet(mgmdCB,&mgmdHostGroup->group,adminList);
  
  if (MCAST_BITX_TEST(adminList, mrtEntry->upstrmRtrIfNum))
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, " resetting Oif");
    MCAST_BITX_RESET_ALL(oifList);
  }
  else
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS, " subtracting admin scope oif");
    MCAST_BITX_SUBTRACT(oifList,adminList,oifList);
  }

  MCAST_BITX_COPY(oifList, mrtEntry->oifList);

  /* Send to MFC */
  mgmdProxyMfcEntryUpdate(mgmdCB,&mrtEntry->srcAddr,
                          &mrtEntry->grpAddr,
                          mrtEntry->upstrmRtrIfNum,mrtEntry->oifList);
  return ;
}


/*********************************************************************
*
* @purpose  Add the MRT entry into the tree if not found
*
* @param    MGMDCB @b{ (input) } Pointer PIMDM Control Block
*           srcAddr  @b{ (input) } Pointer Source Address
*           grpAddr  @b{ (input) } Pointer Group Address
*           rtrIfNum @b{ (input) } Router Interface Number  
*                            
* @returns  Pointer to the MRT Entry if found else return NULL.
*
* @comments None
*
* @end
*********************************************************************/
mgmdProxyCacheEntry_t* mgmdProxyCacheEntryAdd (mgmd_cb_t *mgmdCb,
                                               L7_inet_addr_t *srcAddr,
                                               L7_inet_addr_t *grpAddr,
                                               L7_uint32 rtrIfNum,
                                               mgmd_host_group_t *mgmdHostGroup)
{
  mgmdProxyCacheEntry_t *mrtEntry =L7_NULLPTR;
  mgmdProxyCacheEntry_t tempMrtEntry;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  /* Validations */
  if ((srcAddr == L7_NULLPTR) || (grpAddr == L7_NULLPTR) ||
      (mgmdCb == L7_NULLPTR))
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Invalid Input Parameters\n");
    return L7_NULLPTR;
  }

  /* Update the srcAddr and grpAddr information */
  memset (&tempMrtEntry, 0, sizeof (mgmdProxyCacheEntry_t));
  inetCopy (&(tempMrtEntry.srcAddr), srcAddr);
  inetCopy (&(tempMrtEntry.grpAddr), grpAddr);

  /* Add the entry to the MRT AVL Tree. 
   * Returns NULL if entry Add is Success.
   */
  if (osapiSemaTake (mgmdCb->mgmd_host_info->mgmdProxyCache.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to Acquire MGMD MRT Semaphore.\n");
    return L7_NULLPTR;
  }

  mrtEntry = avlInsertEntry (&mgmdCb->mgmd_host_info->mgmdProxyCache, (void*)&tempMrtEntry);

  osapiSemaGive (mgmdCb->mgmd_host_info->mgmdProxyCache.semId);

  if (mrtEntry == L7_NULLPTR) /* Success */
  {
    mrtEntry = mgmdProxyMRTCacheEntryGet(mgmdCb,srcAddr,grpAddr);
    if (mrtEntry == L7_NULLPTR)
    {
      return L7_NULLPTR;
    }
    /* TBD: do RPF check */

    /* Update the values */
    mrtEntry->upstrmRtrIfNum = rtrIfNum;
    mrtEntry->entryUpTime = osapiUpTimeRaw();

    /* Update the OIF list and MFC */
    mgmdProxyMRTCacheEntryUpdate(mgmdCb,srcAddr,mgmdHostGroup, mrtEntry);
  }
  return mrtEntry;
}

/*********************************************************************
*
* @purpose  Delete the MRT Entry
*
* @param    MGMDCB @b{ (input) } Pointer PIMDM Control Block
*           srcAddr  @b{ (input) } Pointer Source Address
*           grpAddr  @b{ (input) } Pointer Group Address
*           flag     @b{ (input) } If True - send msg to MFC
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments None
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyCacheEntryDelete(mgmd_cb_t *mgmdCb,
                                  L7_inet_addr_t *srcAddr,
                                  L7_inet_addr_t *grpAddr,
                                  L7_BOOL        flag)
{
  L7_RC_t   rc = L7_SUCCESS;
  mgmdProxyCacheEntry_t *mrtEntry =L7_NULLPTR;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  /* Validations */
  if ((srcAddr == L7_NULLPTR) || (grpAddr == L7_NULLPTR) ||
      (mgmdCb == L7_NULLPTR))
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Invalid Input Parameters\n");
    return L7_FAILURE;
  }

  mrtEntry =  mgmdProxyMRTCacheEntryGet(mgmdCb,srcAddr,grpAddr);

  if (mrtEntry != L7_NULLPTR) /* Success */
  {
    /* delete it and send the notification to MFC */
    if (flag == L7_TRUE)
    {
      mgmdProxyMfcEntryDelete(mgmdCb,srcAddr,grpAddr,mrtEntry->upstrmRtrIfNum);
    }

    if (osapiSemaTake (mgmdCb->mgmd_host_info->mgmdProxyCache.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to Acquire MGMD MRT Semaphore.\n");
      return L7_FAILURE;
    }
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Deleting found entry");
    if (avlDeleteEntry (&mgmdCb->mgmd_host_info->mgmdProxyCache, (void*) mrtEntry) == L7_NULLPTR)
    {
      rc = L7_FAILURE;
    }
    osapiSemaGive (mgmdCb->mgmd_host_info->mgmdProxyCache.semId);
  }
  return rc;
}

/*********************************************************************
*
* @purpose  Search the MRT tree to get the  entry 
*
* @param    MGMDCB @b{ (input) } Pointer PIMDM Control Block
*           srcAddr @b{ (input) } Pointer Source Address
*           grpAddr @b{ (input) } Pointer Group Address
*                            
* @returns  Pointer to the MRT Entry if found else return NULL.
*
* @comments None
*
* @end
*********************************************************************/
mgmdProxyCacheEntry_t* mgmdProxyMRTCacheEntryGet (mgmd_cb_t *mgmdCb,
                                                  L7_inet_addr_t *srcAddr,
                                                  L7_inet_addr_t *grpAddr)
{
  mgmdProxyCacheEntry_t *mrtEntry = L7_NULLPTR;
  mgmdProxyCacheEntry_t tempMrtEntry;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  if (mgmdCb == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Invalid MGMD Control Block\n");
    return mrtEntry;
  }

  /* Update the srcAddr and grpAddr information */
  memset (&tempMrtEntry, 0, sizeof (mgmdProxyCacheEntry_t));

  if (srcAddr != L7_NULLPTR)
  {
    inetCopy (&(tempMrtEntry.srcAddr), srcAddr);
  }

  if (grpAddr != L7_NULLPTR)
  {
    inetCopy (&(tempMrtEntry.grpAddr), grpAddr);
  }

  /* Lookup for the matching srcAddr and grpAddr entry in the Tree */
  mrtEntry = (mgmdProxyCacheEntry_t *)
             avlSearchLVL7 (&mgmdCb->mgmd_host_info->mgmdProxyCache, (void*)(&tempMrtEntry),
                            AVL_EXACT);

  return mrtEntry;
}


/*********************************************************************
*
* @purpose  Search the MRT tree to get the  next entry 
*
* @param    MGMDCB @b{ (input) } Pointer PIMDM Control Block
*           srcAddr @b{ (input) } Pointer Source Address
*           grpAddr @b{ (input) } Pointer Group Address
*                            
* @returns  Pointer to the MRT Entry if found else return NULL.
*
* @comments None
*
* @end
*********************************************************************/
mgmdProxyCacheEntry_t* mgmdProxyMRTCacheEntryNextGet (mgmd_cb_t *mgmdCb,
                                                      L7_inet_addr_t *srcAddr,
                                                      L7_inet_addr_t *grpAddr)
{
  mgmdProxyCacheEntry_t *mrtEntry = L7_NULLPTR;
  mgmdProxyCacheEntry_t tempMrtEntry;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  if (mgmdCb == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Invalid MGMD Control Block\n");
    return mrtEntry;
  }

  /* Update the srcAddr and grpAddr information */
  memset (&tempMrtEntry, 0, sizeof (mgmdProxyCacheEntry_t));

  if (srcAddr != L7_NULLPTR)
  {
    inetCopy (&(tempMrtEntry.srcAddr), srcAddr);
  }

  if (grpAddr != L7_NULLPTR)
  {
    inetCopy (&(tempMrtEntry.grpAddr), grpAddr);
  }

  /* Lookup for the matching srcAddr and grpAddr entry in the Tree */
  mrtEntry = (mgmdProxyCacheEntry_t*)
             avlSearchLVL7 (&mgmdCb->mgmd_host_info->mgmdProxyCache, (void*) (&tempMrtEntry),
                            AVL_NEXT);
  return mrtEntry;
}



/*****************************************************************
* @purpose To update MFC entries
*
* @param    mgmdCB   @b{ (input) }   MGMD Control Block
* @param    srcAddr  @b{ (input) } Pointer to Source address
* @param    grpAddr  @b{ (input) } Pointer Group address 
* @param    rtrIfNum @b{ (input) } incoming Interface Number  
*
* @returns  None
*
* @notes  it update the MFC cache table
*
* @end
*********************************************************************/
static L7_RC_t mgmdProxyMrtMfcNoCacheEventProcess(mgmd_cb_t *mgmdCB,
                                                  L7_inet_addr_t *srcAddr,
                                                  L7_inet_addr_t *grpAddr,
                                                  L7_uint32 rtrIfNum) 
{
  mgmdProxyCacheEntry_t   *mrtEntry;
  mgmd_host_group_t       *mgmdHostGroup;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  /*check to see if the entry exists */
  mrtEntry =  mgmdProxyMRTCacheEntryGet(mgmdCB,srcAddr,grpAddr);

  if (mrtEntry != L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," MRT Entry already present");
    /* update the MFC with the entry if the incoming interface is correct */
    return mgmdProxyMfcEntryUpdate(mgmdCB,srcAddr,grpAddr,rtrIfNum,mrtEntry->oifList);
  }
  else
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," MRT Entry not present");

    /* Defect 91259.
     * Removing this intentionally as IGMP-Proxy do not support the dynamic
     * route changes and doesn't make sense to have this check.
     */

    /* Check the Host info table for the S,G  */
    mgmd_proxy_host_group_get(mgmdCB,*grpAddr,&mgmdHostGroup);
    if (mgmdHostGroup != L7_NULLPTR)
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Group Entry present");
      /* create the new mrt entry and update the MFC */
      mrtEntry = mgmdProxyCacheEntryAdd(mgmdCB,srcAddr,grpAddr,rtrIfNum,mgmdHostGroup);
      if (mrtEntry == L7_NULLPTR)
      {
        L7_uchar8 grp[IPV6_DISP_ADDR_LEN];
        L7_uchar8 src[IPV6_DISP_ADDR_LEN];

        L7_LOGF ( L7_LOG_SEVERITY_NOTICE, L7_FLEX_MGMD_MAP_COMPONENT_ID,
                 "MGMD Proxy Route Entry Creation Failied; grpAddr - %s, "
                 "srcAddr - %s, rtrIfNum - %d"
                 "Registration of the Multicast Forwarding entry for the"
                 " specified Source and Group Address Failed when MGMD-Proxy is used.",
                 inetAddrPrint(grpAddr,grp), inetAddrPrint(srcAddr,src), rtrIfNum);
        return L7_FAILURE;
      }
      else
      {
        /* update the oif list and update the mfc */
        mgmdProxyMRTCacheEntryUpdate(mgmdCB,srcAddr,mgmdHostGroup, mrtEntry);
      }
    }
    else
    {
      /* If no ports are interested in the rx'ed group, 
         forward the packets to proxy interface unless the 
         incoming interface is proxy interface ,
         else data packets are flooded to CPU when 
         packets are received continuosly              */
      mgmdProxyCacheEntry_t tempMrtEntry;

      MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS,
                       " No port is interested in this group = ",grpAddr);

      memset (&tempMrtEntry, 0, sizeof (mgmdProxyCacheEntry_t));
      inetCopy (&(tempMrtEntry.srcAddr), srcAddr);
      inetCopy (&(tempMrtEntry.grpAddr), grpAddr);
      tempMrtEntry.upstrmRtrIfNum = rtrIfNum;
      tempMrtEntry.entryUpTime = osapiUpTimeRaw();
      MCAST_BITX_RESET_ALL(tempMrtEntry.oifList);
      /* Add the entry to the MRT AVL Tree. 
       * Returns NULL if entry Add is Success.
       */

      if ((rtrIfNum != mgmdCB->mgmd_proxy_ifIndex) &&
          (mgmdCB->mgmd_host_info->mgmdProxyUpStreamDisable == L7_FALSE))
      {
        MCAST_BITX_SET(tempMrtEntry.oifList, mgmdCB->mgmd_proxy_ifIndex);
      }
      if (osapiSemaTake (mgmdCB->mgmd_host_info->mgmdProxyCache.semId, L7_WAIT_FOREVER) != L7_SUCCESS)
      {
        MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Failed to Acquire MGMD MRT Semaphore.\n");
        return L7_FAILURE;
      }

      mrtEntry = avlInsertEntry (&mgmdCB->mgmd_host_info->mgmdProxyCache, (void*)&tempMrtEntry);

      osapiSemaGive (mgmdCB->mgmd_host_info->mgmdProxyCache.semId);
      mgmdProxyMfcEntryUpdate(mgmdCB,srcAddr ,grpAddr ,rtrIfNum, tempMrtEntry.oifList);

    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Handler for MFC Component Events
*
* @param    mgmdCB       @b{ (input) }   MGMD Control Block
*           mfcEventType  @b{ (input) } MFC Event ID
*           mfcInfo       @b{ (input) } MFC Event Info 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyMFCEventHandler (mgmd_cb_t *mgmdCB,
                                  L7_uint32 mfcEventType,
                                  mfcEntry_t *mfcInfo)
{
  L7_RC_t retVal = L7_FAILURE;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  if (mgmdCB == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Invalid MGMD Control Block\n");
    return L7_FAILURE;
  }

  if (mgmdCB->mgmd_proxy_status != L7_ENABLE)
  {
    MGMD_PROXY_DEBUG (MGMD_PROXY_DEBUG_FAILURE, "MGMD Proxy is Not Operational");
    return L7_FAILURE;
  }

  if (mfcInfo == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Invalid MFC Info");
    return L7_FAILURE;
  }

  switch (mfcEventType)
  {
    case MCAST_MFC_NOCACHE_EVENT:
      {
        retVal = mgmdProxyMrtMfcNoCacheEventProcess (mgmdCB,
                                                     &mfcInfo->source,
                                                     &mfcInfo->group,
                                                     mfcInfo->iif);
        break;
      }

    case MCAST_MFC_ENTRY_EXPIRE_EVENT:
      {
        retVal = mgmdProxyCacheEntryDelete(mgmdCB,&mfcInfo->source,&mfcInfo->group,L7_FALSE); 
        break;
      }

    default:
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Invalid MFC Event - %d received", mfcEventType);
  }
  return retVal;
}


/*********************************************************************
*
* @purpose  Get the Adminscope Boundary Node for a Group
*
* @param    mgmdCB         @b{ (input) }   MGMD Control Block
*           grpAddr        @b{ (input) } Address of the Multicast Group
*           grpMask        @b{ (input) } Mask of the Multicast Group
*
* @returns  Pointer to the node.
*
* @comments
*
* @end
*********************************************************************/
static mgmdProxyASBNode_t* 
mgmdProxyAdminScopeNodeGet (mgmd_cb_t *mgmdCB,
                            L7_inet_addr_t *grpAddr,
                            L7_inet_addr_t *grpMask)
{
  mgmdProxyASBNode_t tempCurrNode;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  memset(&tempCurrNode,0,sizeof(mgmdProxyASBNode_t));
  inetCopy (&(tempCurrNode.grpAddr), grpAddr);
  inetCopy (&(tempCurrNode.grpMask), grpMask);

  return(mgmdProxyASBNode_t*)SLLFind(&mgmdCB->mgmd_host_info->asbList,(L7_sll_member_t*)&tempCurrNode);
}


/*********************************************************************
*
* @purpose  Get the Adminscope Boundary interface bitset for a Group
*
* @param    mgmdCB         @b{ (input) }   MGMD Control Block
*           grpAddr        @b{ (input) } Address of the Multicast Group
*           intfSet        @b{ (output) } interface mask for the Group/mask. 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t 
mgmdProxyAdminScopeIntfBitSetGet (mgmd_cb_t *mgmdCB,
                                  L7_inet_addr_t *grpAddr,
                                  L7_uchar8      *oList)
{
  mgmdProxyASBNode_t *curNode;
  L7_uchar8          maskLen = 0; 

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");
  MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS," Given Grp = ", grpAddr);


  curNode = (mgmdProxyASBNode_t*)SLLNextGet(&mgmdCB->mgmd_host_info->asbList,(L7_sll_member_t*)L7_NULLPTR);
  MCAST_BITX_RESET_ALL(oList);

  while (curNode != L7_NULLPTR)
  {
    inetMaskToMaskLen(&curNode->grpMask,&maskLen);

    if(inetAddrCompareAddrWithMask(&curNode->grpAddr,maskLen,
                                   grpAddr, maskLen) == L7_SUCCESS)
    {
      /* Add the interface list to the oList */
      MCAST_BITX_ADD(oList, curNode->intfMask,oList);
    }
    curNode =  (mgmdProxyASBNode_t*)SLLNextGet(&mgmdCB->mgmd_host_info->asbList,(L7_sll_member_t*)curNode);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Compute the Outgoing Interface List and update the MFC
*
* @param    mgmdCB         @b{ (input) }   MGMD Control Block
*           srcAddr @b{ (input) } Pointer Source Address
*           grpAddr @b{ (input) } Pointer Group Address
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments 
*
* @end
*********************************************************************/
static
L7_RC_t  mgdmProxyAdminScopeMrtCacheEntriesUpdate (mgmd_cb_t *mgmdCB,
                                                   L7_inet_addr_t *grpAddr,
                                                   L7_inet_addr_t *grpMask)
{
  mgmdProxyCacheEntry_t  *mrtEntry;
  mgmd_host_group_t      *mgmdHostGroup;
  L7_uchar8              maskLen;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  inetMaskToMaskLen(grpMask,&maskLen);
  mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,L7_NULLPTR,L7_NULLPTR);

  while (mrtEntry != L7_NULLPTR)
  {
    if(inetAddrCompareAddrWithMask(&mrtEntry->grpAddr,maskLen,
                                   grpAddr, maskLen) == L7_SUCCESS)
    {
      /* Check the Host info table for the S,G  */
      mgmd_proxy_host_group_get(mgmdCB, mrtEntry->grpAddr, &mgmdHostGroup);
      if (mgmdHostGroup == L7_NULLPTR)
      {
        MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Failed to get group info ");
        return L7_FAILURE;
      }
      /* Re-Calculate new oif  and update in MFC */
      mgmdProxyMRTCacheEntryUpdate(mgmdCB, &mrtEntry->srcAddr,mgmdHostGroup, mrtEntry);

    }
    mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,&mrtEntry->srcAddr,&mrtEntry->grpAddr);
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Set the Adminscope Boundary for a Group on an Interface
*
* @param    mgmdCB         @b{ (input) }   MGMD Control Block
*           grpAddr        @b{ (input) } Address of the Multicast Group
*           grpMask        @b{ (input) } Mask of the Multicast Group
*           rtrIfNum       @b{ (input) } Index of the Router Interface
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
mgmdproxyAdminScopeBoundarySet (mgmd_cb_t *mgmdCB,
                                L7_inet_addr_t *grpAddr,
                                L7_inet_addr_t *grpMask,
                                L7_uint32 rtrIfNum)
{
  mgmdProxyASBNode_t *asbNode = L7_NULLPTR;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  /* Get the ASB Node for the Group. */
  if ((asbNode = mgmdProxyAdminScopeNodeGet (mgmdCB, grpAddr, grpMask))
      == L7_NULLPTR)
  {
    /* Node doesn't exist.  Create one.
     */
    if ( (asbNode = (mgmdProxyASBNode_t *)MGMD_PROXY_ALLOC(mgmdCB->proto, sizeof (mgmdProxyASBNode_t))) 
          == L7_NULLPTR)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Admin Scope Node Alloc Failed ");
      return L7_FAILURE;
    }
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Creating new entry");
    /* Update the Node members and Add to the List. */
    memset(asbNode,0,sizeof(mgmdProxyASBNode_t));
    inetCopy (&(asbNode->grpAddr), grpAddr);
    inetCopy (&(asbNode->grpMask), grpMask);
    MCAST_BITX_SET (asbNode->intfMask, rtrIfNum);
    asbNode->family = mgmdCB->proto;
    if (SLLAdd (&(mgmdCB->mgmd_host_info->asbList), (L7_sll_member_t*) asbNode)
             != L7_SUCCESS)
    {
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Admin Scope Node Addition Failed ");
      MGMD_PROXY_FREE (mgmdCB->proto, asbNode);
      return L7_FAILURE;
    }
  }
  else
  {
    /* Verify if the Interface is already SET for this Group.
     */
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS,"Entry already present");
    if (MCAST_BITX_TEST (asbNode->intfMask, rtrIfNum))
    {
      /* log error */
      return L7_SUCCESS;
    }

    /* Set the Interface as Scoped for this Group.*/
    MCAST_BITX_SET (asbNode->intfMask, rtrIfNum);
  }

  /* Compute the OIF List. for all (*,G) entries */
  mgdmProxyAdminScopeMrtCacheEntriesUpdate(mgmdCB,grpAddr,grpMask);

  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Reset the Adminscope Boundary for a Group on an Interface
*
* @param    mgmdCB         @b{ (input) }   MGMD Control Block
*           grpAddr        @b{ (input) } Address of the Multicast Group
*           grpMask        @b{ (input) } Mask of the Multicast Group
*           rtrIfIndex     @b{ (input) } Index of the Router Interface
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
static L7_RC_t
mgmdproxyAdminScopeBoundaryReset (mgmd_cb_t *mgmdCB,
                                  L7_inet_addr_t *grpAddr,
                                  L7_inet_addr_t *grpMask,
                                  L7_uint32 rtrIfIndex)
{
  mgmdProxyASBNode_t *asbNode = L7_NULLPTR;
  L7_BOOL isEmpty = L7_FALSE;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  /* Get the ASB Node for the Group. */
  if ((asbNode = mgmdProxyAdminScopeNodeGet (mgmdCB, grpAddr, grpMask))
      == L7_NULLPTR)
  {
    /* log error */
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Failed to get admin scope node");
    return L7_FAILURE;
  }

  /* Verify if the Interface is SET for this Group. */
  if (!(MCAST_BITX_TEST (asbNode->intfMask, rtrIfIndex)))
  {
    /* log error */
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Given inface = %d is not set for this admin scope",
                      rtrIfIndex);
    return L7_FAILURE;
  }

  /* Reset the Interface as Scoped for this Group. */
  MCAST_BITX_RESET (asbNode->intfMask, rtrIfIndex);

  /* If there are no other Interfaces set for this Group,
   * Delete it. */
  MCAST_BITX_IS_EMPTY (asbNode->intfMask, isEmpty);
  if (isEmpty == L7_TRUE)
  {
    if (SLLDelete (&mgmdCB->mgmd_host_info->asbList, (L7_sll_member_t*) asbNode) != L7_SUCCESS)
    {
      /* log error */
      return L7_FAILURE;
    }
  }

  /* Compute the OIF List. */
  mgdmProxyAdminScopeMrtCacheEntriesUpdate(mgmdCB,grpAddr,grpMask);

  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Handler for Admin Scope Boundary Events
*
* @param    mgmdCB       @b{ (input) }   MGMD Control Block
*           asbEventType @b{ (input) } Admin Scope Event ID
*           asbInfo      @b{ (input) } Admin Scope Boundary Event Info 
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyAdminScopeBoundaryEventHandler (mgmd_cb_t *mgmdCB,
                                                 L7_uint32 asbEventType,
                                                 mcastAdminMsgInfo_t *asbInfo)
{
  L7_RC_t retVal = L7_FAILURE;
  L7_uint32 rtrIfNum;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  if (asbInfo == L7_NULLPTR)
  {
    MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Invalid Admin Scope Boundary Info");
    return L7_FAILURE;
  }

  /* By this time from mapping layer to vendor Proxy code,
     the intIfNum in event structure contains router interface number.*/
  rtrIfNum = asbInfo->intIfNum;

  switch (asbInfo->mode)
  {
    case L7_ENABLE:
      {
        retVal = mgmdproxyAdminScopeBoundarySet (mgmdCB,
                                                 &(asbInfo->groupAddress),
                                                 &(asbInfo->groupMask),
                                                 rtrIfNum);
        break;
      }

    case L7_DISABLE:
      {

        retVal = mgmdproxyAdminScopeBoundaryReset (mgmdCB,
                                                   &(asbInfo->groupAddress),
                                                   &(asbInfo->groupMask),
                                                   rtrIfNum);
        break;
      }

    default:
      MGMD_DEBUG (MGMD_DEBUG_FAILURE, "Invalid Admin Scope Mode - %d received", asbInfo->mode);
  }
  return retVal;
}
/*********************************************************************
*
* @purpose  Handler for Querier state change Events
*
* @param    mgmdCB       @b{ (input) } MGMD Control Block
*           rtrIfNum     @b{ (input) } Interface on which querier state changed
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @comments
*
* @end
*********************************************************************/
L7_RC_t mgmdProxyQuerierStateChangeEventHandler (mgmd_cb_t *mgmdCB)
{
  mgmdProxyCacheEntry_t     *mrtEntry;
  mgmd_host_group_t         *mgmdHostGroup;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,L7_NULLPTR,L7_NULLPTR);

  while (mrtEntry != L7_NULLPTR)
  {
    /* Check the Host info table for the S,G  */
    mgmd_proxy_host_group_get(mgmdCB, mrtEntry->grpAddr, &mgmdHostGroup);
    if (mgmdHostGroup == L7_NULLPTR)
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Failed to get group info ");
      return L7_FAILURE;
    }
    /* Calculate new oif  and update in MFC */
    mgmdProxyMRTCacheEntryUpdate(mgmdCB, &mrtEntry->srcAddr,mgmdHostGroup, mrtEntry);

    /* Get the next MRT entry */
    mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,&mrtEntry->srcAddr,&mrtEntry->grpAddr);
  }
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Updates MRT table when group record in proxy database is deleted
*
* @param    mgmdCB     @b{ (input) } MGMD Control Block
* @param    group      @b{ (input) } group address
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void mgmdProxyMRTGroupRecordDeleteHandler(mgmd_cb_t *mgmdCB, L7_inet_addr_t group)
{
  mgmdProxyCacheEntry_t  *mrtEntry = L7_NULLPTR;
  mgmd_host_group_t      *mgmdHostGroup = L7_NULLPTR;

  MGMD_PROXY_DEBUG_ADDR(MGMD_PROXY_DEBUG_APIS," Entered, grp = ", &group);

  mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,L7_NULLPTR,L7_NULLPTR);

  while (mrtEntry != L7_NULLPTR)
  {
    if (L7_INET_ADDR_COMPARE(&group, &mrtEntry->grpAddr) == 0)
    {
      /* Check the Host info table for the S,G  */
      mgmd_proxy_host_group_get(mgmdCB, group,&mgmdHostGroup);
      if (mgmdHostGroup != L7_NULLPTR)
      {
        MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Obtained group info ");
        /* Calculate new oif  and update in MFC */
        mgmdProxyMRTCacheEntryUpdate(mgmdCB, &mrtEntry->srcAddr,mgmdHostGroup, mrtEntry);
      }
      else
      {
        MCAST_BITX_RESET_ALL(mrtEntry->oifList);
        if ((mrtEntry->upstrmRtrIfNum != mgmdCB->mgmd_proxy_ifIndex) &&
          (mgmdCB->mgmd_host_info->mgmdProxyUpStreamDisable == L7_FALSE))
        {
          MCAST_BITX_SET(mrtEntry->oifList, mgmdCB->mgmd_proxy_ifIndex);
        }
        mgmdProxyMfcEntryUpdate(mgmdCB, &mrtEntry->srcAddr,
                                &mrtEntry->grpAddr,
                                mrtEntry->upstrmRtrIfNum,
                                mrtEntry->oifList);
      }
    }
    /* Get the next MRT entry */
    mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,&mrtEntry->srcAddr,&mrtEntry->grpAddr);
  }
}
/*********************************************************************
*
* @purpose  Updates MRT table when group record in proxy database is deleted
*
* @param    mgmdCB     @b{ (input) } MGMD Control Block
* @param    rtIfNum    @b{ (input) } router interface number
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void mgmdProxyMRTIntfDownHandler(mgmd_cb_t *mgmdCB, L7_uint32 rtrIfNum)
{
  mgmdProxyCacheEntry_t  *mrtEntry = L7_NULLPTR, *mrtDeleteEntry = L7_NULLPTR;
 
  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered, rtrIfNum = ", rtrIfNum);

  mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,L7_NULLPTR,L7_NULLPTR);

  while (mrtEntry != L7_NULLPTR)
  {
    mrtDeleteEntry = mrtEntry;
    mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,&mrtEntry->srcAddr,&mrtEntry->grpAddr);
    if (mrtDeleteEntry->upstrmRtrIfNum == rtrIfNum)
    {
      mgmdProxyCacheEntryDelete(mgmdCB, &mrtDeleteEntry->srcAddr,
                                &mrtDeleteEntry->grpAddr, L7_TRUE);      
    }
  }
}
/*********************************************************************
*
* @purpose  Purges proxy MRT table 
*
* @param    mgmdCB     @b{ (input) } MGMD Control Block
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void mgmdProxyMRTPurge(mgmd_cb_t *mgmdCB)
{
  mgmdProxyCacheEntry_t     *mrtEntry, *mrtDeleteEntry;

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered");

  mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,L7_NULLPTR,L7_NULLPTR);

  while (mrtEntry != L7_NULLPTR)
  {
    mrtDeleteEntry =  mrtEntry;
    /* Get the next MRT entry */
    mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,&mrtEntry->srcAddr,&mrtEntry->grpAddr);

    mgmdProxyCacheEntryDelete(mgmdCB, &mrtDeleteEntry->srcAddr,
                                &mrtDeleteEntry->grpAddr, L7_TRUE);
  }
  /*Also purge the admin scope info for MRT entries */
   SLLPurge(L7_FLEX_MGMD_MAP_COMPONENT_ID, &(mgmdCB->mgmd_host_info->asbList)); 
}

/*********************************************************************
*
* @purpose  Purges proxy MRT table 
*
* @param    mgmdCB     @b{ (input) } MGMD Control Block
*
* @returns  none
*
* @comments
*
* @end
*********************************************************************/
void mgmdProxyMRTUpdate(mgmd_cb_t *mgmdCB, L7_inet_addr_t *srcAddr,
                        L7_inet_addr_t *grpAddr)
{
  mgmdProxyCacheEntry_t *mrtEntry = L7_NULLPTR;
  mgmd_host_group_t     *mgmdHostGroup = L7_NULLPTR;
  L7_inet_addr_t         group;

  inetCopy(&group, grpAddr);
  /* Check the Host info table for the S,G  */
  mgmd_proxy_host_group_get(mgmdCB, group, &mgmdHostGroup);
  if (mgmdHostGroup == L7_NULLPTR)
  {
    MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Failed to get group info ");
    return ;
  }

  if (srcAddr != L7_NULLPTR)
  {
    mrtEntry = mgmdProxyMRTCacheEntryGet(mgmdCB,srcAddr,&mgmdHostGroup->group);
    if (mrtEntry == L7_NULLPTR)
    {
      MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Failed to get MRT entry ");
      return ;
    }
    mgmdProxyMRTCacheEntryUpdate(mgmdCB,srcAddr,mgmdHostGroup, mrtEntry);
  }
  else
  {
    mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,L7_NULLPTR,L7_NULLPTR);

    while (mrtEntry != L7_NULLPTR)
    {
      if (L7_INET_ADDR_COMPARE(grpAddr, &mrtEntry->grpAddr) == 0)
      {
        mgmdProxyMRTCacheEntryUpdate(mgmdCB, &mrtEntry->srcAddr,mgmdHostGroup, mrtEntry);
      }
      /* Get the next MRT entry */
      mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,&mrtEntry->srcAddr,&mrtEntry->grpAddr);
    }
  }
}
/*********************************************************************
* @purpose  Handles the interface routing change
*
* @param    mgmdCB            @b{ (input) }  MGMD Control Block
* @param    rtrIfNum          @b{ (input) }  the disabled router interface
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
void mgmdProxyIntfDownMRTUpdate(MCAST_CB_HNDL_t mgmdCBHndl, L7_uint32 rtrIfNum)
{
  mgmdProxyCacheEntry_t     *mrtEntry, *mrtDeleteEntry;
  mgmd_cb_t *mgmdCB = mgmdCBHndl; 

  MGMD_PROXY_DEBUG(MGMD_PROXY_DEBUG_APIS," Entered,rtrIfNum = ", rtrIfNum);

  mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,L7_NULLPTR,L7_NULLPTR);

  while (mrtEntry != L7_NULLPTR)
  {
    mrtDeleteEntry =  mrtEntry;
    /* Get the next MRT entry */
    mrtEntry = mgmdProxyMRTCacheEntryNextGet(mgmdCB,&mrtEntry->srcAddr,&mrtEntry->grpAddr);

    if (mrtDeleteEntry->upstrmRtrIfNum == rtrIfNum)
    {
      mgmdProxyCacheEntryDelete(mgmdCB, &mrtDeleteEntry->srcAddr,
                                &mrtDeleteEntry->grpAddr, L7_TRUE);
    }
  }
}
