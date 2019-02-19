/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename dvmrp_igmp.c
*
* @purpose Contains IGMP related definitions to support the DVMRP protocol
*
* @component
*
* @comments
*
* @create 03/03/2006
*
* @author Prakash/Shashidhar
* @end
*
**********************************************************************/
#include "dvmrp_igmp.h"
#include "dvmrp_mfc.h"
#include "dvmrp_cache_table.h"
#include "dvmrp_routing.h"
#include "dvmrp_igmp_group_table.h"
#include "dvmrp_protocol.h"
#include "dvmrp_util.h"

/*********************************************************************
* @purpose  This function is used to update the cache table when IGMP group 
*           membership info is recieved
*           
* @param       dvmrpcb   - @b{(input)} Pointer to the dvmrp control block 
*                    grp_info   -  @b{(input)} Pointer toIGMP membership info
*
* @returns  None
*
* @notes    None
* @end
*********************************************************************/
void dvmrpCacheTableUpdateForGrpUpdateEvent(dvmrp_t *dvmrpcb, mgmdMrpEventInfo_t *grp_info)
{
  dvmrp_cache_entry_t tmpEntry,*entry = L7_NULLPTR;   
  dvmrp_Group_entry_t *grpEntry = L7_NULLPTR;
  dvmrp_Group_entry_t tempGrpEntry;
  L7_char8 grp[DVMRP_STRING_SIZE];
  L7_uint32 srcIndex = 0;
  mrp_source_record_t *src_add = L7_NULLPTR;
  L7_char8 srcbuf[IPV6_DISP_ADDR_LEN],grpbuf[IPV6_DISP_ADDR_LEN];
  interface_bitset_t oList;
  interface_bitset_t tempList;

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);

  if ((L7_NULLPTR == dvmrpcb) || (L7_NULLPTR == grp_info))
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"\n\nINVALID : dvmrpcb/grp_info is NULL!\n\n");
    return;
  }

  if (grp_info->numSrcs == 0) /* IGMPv1/v2 Processing */
  {
    if (inetAddrHtop(&grp_info->group, grp) != L7_SUCCESS)
    {
      DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "\nInetAddress to ASCII conversion Failed.\n");
      return;   
    }
  
    if (grp_info->mode == MGMD_FILTER_MODE_INCLUDE)
    {
      DVMRP_DEBUG(DVMRP_DEBUG_GROUP,
                  "%s:%d INCLUDE mode event received for grp=%s, No_Srcs=%d",
                  __FUNCTION__,__LINE__, grp, grp_info->numSrcs);
      if (grp_info->numSrcs == 0)
      {
        memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
        if (L7_SUCCESS != 
            dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d No Entry found \n",
                      __FUNCTION__,__LINE__);
          DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d No Entry found \n",
                      __FUNCTION__,__LINE__);
          return;
        }
        while (L7_NULLPTR != entry)
        {
          if (L7_INET_ADDR_COMPARE(&entry->group,&grp_info->group) ==0)
          {
            DVMRP_DEBUG(DVMRP_DEBUG_GROUP, "%s: call dvmrp_update_fn_call for"
                        "DVMRP_EVENT_MFC_DEL_MEMBER for grp=%s on index=%d\n",
                        __FUNCTION__,grp,grp_info->rtrIfNum);
  
            dvmrp_update_fn_call(DVMRP_EVENT_MFC_DEL_MEMBER, entry, 
                                 grp_info->rtrIfNum);
            dvmrpApplyAdminScope(dvmrpcb,entry);          
          }
          memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));
          entry = L7_NULLPTR;
          if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d No more Entry found \n",
                        __FUNCTION__,__LINE__);
            DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d No more Entry found \n",
                        __FUNCTION__,__LINE__);
            break;
          }
        }
      }
      else
      {
        DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d IGMPV3 Records with Source List are not supported.\n",
                    __FUNCTION__,__LINE__);
      }
    }
    else /*  EXCLUDE MODE */
    {
      memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
      if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
      {
        DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d No Entry found \n",
                    __FUNCTION__,__LINE__);
        DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d No Entry found \n",
                    __FUNCTION__,__LINE__);
      }
      while (L7_NULLPTR != entry)
      {
        if (L7_INET_ADDR_COMPARE(&entry->group,&grp_info->group) ==0)
        {
          if (grp_info->numSrcs == 0)
          {
            DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: call dvmrp_update_fn_call for"
                        " DVMRP_EVENT_MFC_ADD_MEMBER for grp=%s on index=%d\n",
                        __FUNCTION__,grp,grp_info->rtrIfNum);
  
            dvmrp_update_fn_call(DVMRP_EVENT_MFC_ADD_MEMBER, entry, 
                                 grp_info->rtrIfNum);
            dvmrpApplyAdminScope(dvmrpcb,entry);                    
          }
          else
          {
            DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d IGMPV3 Records with Source List are not supported.\n",
                        __FUNCTION__,__LINE__);
          }
        }
        memcpy(&tmpEntry, entry, sizeof(dvmrp_cache_entry_t));    
        entry = L7_NULLPTR;
        if (L7_SUCCESS != dvmrpCacheTableCacheNextGet(dvmrpcb,&tmpEntry, &entry))
        {
          DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s:%d No more Entry found \n",
                      __FUNCTION__,__LINE__);
          DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s:%d No more Entry found \n",
                      __FUNCTION__,__LINE__);
          break;
        }
      }
    }
  }
  else /* IGMPv3 Processing */
  {
    for (srcIndex = 0; srcIndex < grp_info->numSrcs; srcIndex++)
    {
      if ((src_add = grp_info->sourceList[srcIndex]) == L7_NULLPTR)
      {
        continue;
      }
      memset (&tempGrpEntry,0,sizeof(dvmrp_Group_entry_t));
      inetCopy (&tempGrpEntry.source, &src_add->sourceAddr);
      inetCopy (&tempGrpEntry.group, &grp_info->group);
      if (dvmrpGroupTableGroupSearch (dvmrpcb,&tempGrpEntry,&grpEntry) != L7_SUCCESS)
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "[%s-%d]: (%s,%s) Group Entry Not Found.\n", __FUNCTION__, __LINE__,
                inetAddrPrint(&tempGrpEntry.source,srcbuf), inetAddrPrint(&tempGrpEntry.group,grpbuf));
        continue;
      }

      BITX_RESET_ALL (&oList);
      BITX_RESET_ALL (&tempList);
      BITX_SUBTRACT (&grpEntry->includeStarG, &grpEntry->excludeSG, &tempList);
      BITX_ADD (&oList, &tempList, &oList);
      BITX_ADD (&oList, &grpEntry->includeSG, &oList);

      memset(&tmpEntry, 0, sizeof(dvmrp_cache_entry_t));
      inetCopy(&tmpEntry.source, &src_add->sourceAddr);
      inetCopy(&tmpEntry.group, &grp_info->group);

      if (dvmrpCacheTableCacheGet (dvmrpcb,&tmpEntry, &entry) != L7_SUCCESS)
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "[%s-%d]: (%s,%s) Cache Entry Not Found.\n", __FUNCTION__, __LINE__,
                inetAddrPrint(&tmpEntry.source,srcbuf), inetAddrPrint(&tmpEntry.group,grpbuf));
        continue;
      }

      if (BITX_TEST (&oList, grp_info->rtrIfNum) != 0)
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "[%s-%d]: (%s,%s) - Setting %d in OIF List.\n", __FUNCTION__, __LINE__,
                inetAddrPrint(&tmpEntry.source,srcbuf), inetAddrPrint(&tmpEntry.group,grpbuf), grp_info->rtrIfNum);
        dvmrp_update_fn_call (DVMRP_EVENT_MFC_ADD_MEMBER, entry, grp_info->rtrIfNum);
        dvmrpApplyAdminScope (dvmrpcb, entry);
      }
      else
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "[%s-%d]: (%s,%s) - Resetting %d in OIF List.\n", __FUNCTION__, __LINE__,
                inetAddrPrint(&tmpEntry.source,srcbuf), inetAddrPrint(&tmpEntry.group,grpbuf), grp_info->rtrIfNum);
        dvmrp_update_fn_call (DVMRP_EVENT_MFC_DEL_MEMBER, entry, grp_info->rtrIfNum);
        dvmrpApplyAdminScope (dvmrpcb, entry);
      }
    }
  }

  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
}


/*********************************************************************
* @purpose  This function is to test the igmp group membership
*
* @param     dvmrpcb   - @b{(input)} Pointer to the dvmrp control block 
* @param     group  -  @b{(input)}  group address
* @param     source -  @b{(input)}  source address
* @param     index  -  @b{(input)}  interfac index
* 
* @returns    L7_SUCCESS/L7_FAILURE
*
* @notes   None
* @end
*********************************************************************/
L7_RC_t  dvmrpMemberShipTest(dvmrp_t *dvmrpcb,L7_inet_addr_t *group, L7_inet_addr_t *source, 
                             L7_int32 index)
{
  dvmrp_Group_entry_t cacheentry, *entry = L7_NULLPTR;
  L7_char8 src[DVMRP_STRING_SIZE], grp[DVMRP_STRING_SIZE];
  L7_char8 srcbuf[IPV6_DISP_ADDR_LEN],grpbuf[IPV6_DISP_ADDR_LEN];
  interface_bitset_t oList;
  interface_bitset_t tempList;

  inetAddrHtop(group, grp);
  inetAddrHtop(source, src);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: ENTERED \n",__FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_GROUP,
              "%s:%d called for src=%s,grp=%s,on RtrIfNum=%d \n",
              __FUNCTION__,__LINE__,src,grp,index);

  if (L7_NULLPTR == dvmrpcb)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_FAILURES, "%s:%d INVALID : dvmrpcb is NULL!\n\n",
                __FUNCTION__, __LINE__);
    return L7_FAILURE;
  }

  memset(&cacheentry,0,sizeof(dvmrp_Group_entry_t));
  inetAddressZeroSet (L7_AF_INET, &cacheentry.group);
  inetAddressZeroSet (L7_AF_INET, &cacheentry.source);
  inetCopy(&cacheentry.source, source);
  inetCopy(&cacheentry.group, group);
  if (L7_SUCCESS != dvmrpGroupTableGroupSearch(dvmrpcb,&cacheentry,&entry))
  {
    memset(&cacheentry,0,sizeof(dvmrp_Group_entry_t));
    inetAddressZeroSet (L7_AF_INET, &cacheentry.group);
    inetAddressZeroSet (L7_AF_INET, &cacheentry.source);
    inetCopy(&cacheentry.group, group);

    if (L7_SUCCESS != dvmrpGroupTableGroupSearch(dvmrpcb,&cacheentry,&entry))
    {
      DVMRP_DEBUG(DVMRP_DEBUG_FAILURES,"%s: Failed to get the entry \n",__FUNCTION__);
      DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: Failed to get the entry \n",__FUNCTION__);
    }
  }
  if (entry != L7_NULLPTR)
  {
    DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: Found the entry\n", __FUNCTION__);

    if (entry->numSrcs == 0) /* IGMPv2 */
    {
      if (MCAST_BITX_TEST(entry->grpIntfBitMask, index))
      {
        if (entry->numSrcs == L7_NULL)
        {
          if (!MCAST_BITX_TEST(entry->modeIntfBitMask, index))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: returning success\n", __FUNCTION__);
            return L7_SUCCESS;
          }
        }
        else
        {
          if (MCAST_BITX_TEST(entry->modeIntfBitMask, index) && 
              MCAST_BITX_TEST(entry->srcIntfBitMask, index))
          {
            DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: returning success\n", __FUNCTION__);
            return L7_SUCCESS;
          }
        }
      }
    }
    else /* IGMPv3 */
    {
      BITX_RESET_ALL (&oList);
      BITX_RESET_ALL (&tempList);
      BITX_SUBTRACT (&entry->includeStarG, &entry->excludeSG, &tempList);
      BITX_ADD (&oList, &tempList, &oList);
      BITX_ADD (&oList, &entry->includeSG, &oList);
      if (BITX_TEST (&oList, index) != 0)
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "[%s-%d]: (%s,%s) - Membership Present on rtrIfNum - %d.\n", __FUNCTION__, __LINE__,
                inetAddrPrint(source,srcbuf), inetAddrPrint(group,grpbuf), index);
        return L7_SUCCESS;
      }
      else
      {
        DVMRP_DEBUG (DVMRP_DEBUG_FAILURES, "[%s-%d]: (%s,%s) - No Members on rtrIfNum - %d.\n", __FUNCTION__, __LINE__,
                inetAddrPrint(source,srcbuf), inetAddrPrint(group,grpbuf), index);
        return L7_FAILURE;
      }
    }
  }

  DVMRP_DEBUG(DVMRP_DEBUG_GROUP,"%s: returning L7_FAILURE\n", __FUNCTION__);
  DVMRP_DEBUG(DVMRP_DEBUG_APIS,"%s: LEAVING \n",__FUNCTION__);
  return L7_FAILURE;  
}

