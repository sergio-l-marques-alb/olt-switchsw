/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    filter_util.c
* @purpose     Static MAC Filtering utilities functions
* @component   Filter
* @comments    contains dll routine for filtering
* @create      1/7/2001
* @author      skalyanam
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include <string.h>
#include "flex.h"
#include "l7_common.h"
#include "nimapi.h"
#include "sysapi.h"
#include "osapi.h"
#include "log.h"
#include "filter_api.h"
#include "filter.h"
#include "avl_api.h"
#include "fdb_api.h"
#include "mfdb_api.h"

extern avlTree_t filterAvlTree;

extern L7_uint32 numMfdbEntries;
/*********************************************************************
*
* @purpose  Apply a change to global filtering configuration
*
* @param    L7_BOOL  enable
*
* @returns  L7_SUCCESS,
* @returns  L7_FAILURE,
*
* @end
*
*********************************************************************/
L7_RC_t filterAdminModeApply(L7_BOOL enable)
{
   filterInfoData_t  *filterEntry = L7_NULLPTR; 
   L7_uchar8         key[L7_FDB_KEY_SIZE];
   L7_uchar8         macAddr[L7_FDB_MAC_ADDR_LEN];
   L7_uint32         ivlLength = 0;
   L7_ushort16       vid;
   L7_RC_t rc = L7_FAILURE;

   L7_FDB_TYPE_t     fdbType;

   memset((void *)key,0x00,L7_FDB_KEY_SIZE);

   filterEntry = avlSearchLVL7(&filterAvlTree,key,AVL_NEXT);
   if (filterEntry == L7_NULLPTR)
   {
     /* There are no filtering entries for which global mode has to be applied.*/
     return L7_SUCCESS;
   }

   (void)fdbGetTypeOfVL(&fdbType);
   if (fdbType == L7_IVL)
   {
     ivlLength = L7_FDB_IVL_ID_LEN;
   }


    /* walk through the filtering entry database*/
   while (filterEntry != L7_NULLPTR)
   {
     /* For each entry that is a multicast and that has 
        destination interface or filtering interface configured
        if mode is enabled add that interface to the mdfb table
        else delete that interface from mfdb
     */
     memcpy(key, filterEntry->vlanidmacAddr, L7_FDB_KEY_SIZE);
     memcpy(&vid, key, ivlLength);
     memcpy(macAddr, &key[ivlLength], L7_FDB_MAC_ADDR_LEN);

     if (filterIsMacAddrTypeMulticast(macAddr) == L7_TRUE)
     {
         rc= L7_SUCCESS;
         
             if (enable == L7_TRUE)
             {
                 rc = filterMfdbEntryAdd(vid,macAddr, &filterEntry->dstIntfMask, &filterEntry->dstIntfFilterMask);
                 numMfdbEntries++;
             }
             else
             {
                rc = filterMfdbEntryDelete(vid,macAddr);
                numMfdbEntries--;
             }

         }


     filterEntry = avlSearchLVL7(&filterAvlTree,key,AVL_NEXT);

   }

   return rc;


}

/*********************************************************************
* @purpose  Removes a node entry from the registry
*          
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
*	
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t filterEntryDelete(L7_uchar8* macAddr, 
                          L7_uint32 vlanId)
{
  filterInfoData_t  *pData;
  filterInfoData_t  *filterEntry;

  pData = filterEntryFind(macAddr,vlanId,L7_MATCH_EXACT);
  if (pData==L7_NULLPTR)
    return L7_FAILURE;

  filterEntry = pData;
  pData = avlDeleteEntry(&filterAvlTree, pData);

  if (pData == L7_NULL)
  {
    /*entry does not exist*/
    return L7_FAILURE;
  }
  if (pData == filterEntry)
  {
    /*entry deleted*/
    return L7_SUCCESS;
  }
  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  adds a node entry from the registry
*          
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
*	
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t filterEntryAdd(L7_uchar8* macAddr,
                       L7_uint32 vlanId)
{
  filterInfoData_t  filterEntry;
  filterInfoData_t  *pData;
  L7_ushort16       vid;
  L7_uint32         ivlLength = 0;
  L7_FDB_TYPE_t     fdbType;
  L7_RC_t rc;

  vid = (L7_ushort16)vlanId;

  memset(&filterEntry,0x00,sizeof(filterInfoData_t));

  rc = fdbGetTypeOfVL(&fdbType);

  if (fdbType == L7_IVL)
  {
    ivlLength = L7_FDB_IVL_ID_LEN;
    memcpy(filterEntry.vlanidmacAddr,&vid,ivlLength);
  }
  memcpy(&filterEntry.vlanidmacAddr[ivlLength], macAddr, L7_FDB_MAC_ADDR_LEN);

  pData = avlInsertEntry(&filterAvlTree, &filterEntry);

  if (pData == L7_NULL)
  {
    /*entry was added into the avl tree*/
    return L7_SUCCESS;
  }
  if (pData == &filterEntry)
  {
    /*some error in avl tree addition*/
    return L7_FAILURE;
  }
  if (pData)
  {
    /*entry already exists*/
    return L7_FAILURE;
  }


  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Finds a node entry from the registry
*          
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
* @param    L7_uint32   flag     @b{(input)} Flag type for search
*                                            L7_MATCH_EXACT   - Exact match
*                                            L7_MATCH_GETNEXT - Next entry greater than this one
*	
* @returns  filter entry pointer on success
* @returns  null pointer on failure
*
* @notes none
*
* @end
*
*********************************************************************/
filterInfoData_t *filterEntryFind(L7_uchar8* macAddr,
                                  L7_uint32 vlanId,
                                  L7_uint32 flag)
{
  filterInfoData_t  *filterEntry;
  L7_uchar8         key[L7_FDB_KEY_SIZE];
  L7_ushort16       vid;
  L7_uint32         ivlLength = 0;
  L7_FDB_TYPE_t     fdbType;
  L7_RC_t rc;

  vid = (L7_ushort16)vlanId;

  memset((void *)key,0x00,L7_FDB_KEY_SIZE);

  rc = fdbGetTypeOfVL(&fdbType);

  if (fdbType == L7_IVL)
  {
    ivlLength = L7_FDB_IVL_ID_LEN;
    memcpy(key,&vid,ivlLength);
  }
  memcpy(&key[ivlLength], macAddr, L7_FDB_MAC_ADDR_LEN);
  filterEntry = avlSearchLVL7(&filterAvlTree,key,flag);
  if (filterEntry == L7_NULL)
    return L7_NULLPTR;
  else
    return filterEntry;
}

/*********************************************************************
* @purpose  Adds an entry to mfdb . If the entry has already been
*           added, add the passed set of ports to the existing entry.
*          
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_INTF_MASK_t   dstIntfMask     @b{(input)} The list of ports
*                                            configured for the entry
*	
* @returns  L7_SUCCESS on succes
* @returns  L7_FAILURE otherwise
*
* @notes The filterIntfMask is optional and could be null. 
*
* @end
*
*********************************************************************/
L7_RC_t filterMfdbEntryAdd(L7_uint32 vlanId,L7_uchar8* macAddr, 
                           L7_INTF_MASK_t *dstIntfMask, L7_INTF_MASK_t *filterIntfMask)
{
  mfdbMemberInfo_t mfdbInfo;
  L7_BOOL entryPresent;
  L7_RC_t rc; 

  printf("[PTin] %s(%d) I was here\r\n", __FUNCTION__, __LINE__);

  bzero((char*)&mfdbInfo, (L7_int32)sizeof(mfdbMemberInfo_t));

  mfdbInfo.vlanId = vlanId;
  memcpy((void *)mfdbInfo.macAddr, macAddr, L7_MAC_ADDR_LEN);
  mfdbInfo.user.componentId = L7_MFDB_PROTOCOL_STATIC;
  mfdbInfo.user.type = L7_MFDB_TYPE_STATIC;
  memcpy((void *)mfdbInfo.user.description, 
         (void *)L7_MFDB_MGMT_CONFIGURED, L7_MFDB_COMPONENT_DESCR_STRING_LEN);

  if(dstIntfMask != L7_NULLPTR)
  {
    memcpy(&mfdbInfo.user.fwdPorts, dstIntfMask, sizeof(L7_INTF_MASK_t));
  }

  if (filterIntfMask != L7_NULLPTR)
  {
      memcpy(&mfdbInfo.user.fltPorts , filterIntfMask, sizeof(L7_INTF_MASK_t));
  }

  rc = mfdbEntryExist(macAddr,vlanId);
  if (rc == L7_NOT_EXIST)
  {
    printf("[PTin] %s(%d) I was here\r\n", __FUNCTION__, __LINE__);
     entryPresent = L7_FALSE;
  }
  else
  {
    printf("[PTin] %s(%d) I was here\r\n", __FUNCTION__, __LINE__);
    entryPresent = L7_TRUE;
  }

  /* add this new VLANID-MAC entry to the MFDB table */
  if (entryPresent == L7_TRUE)
  {
    printf("[PTin] %s(%d) I was here\r\n", __FUNCTION__, __LINE__);
      return mfdbEntryPortsAdd(&mfdbInfo);
  }                          
  else
  {
    printf("[PTin] %s(%d) I was here\r\n", __FUNCTION__, __LINE__);
      return mfdbEntryAdd(&mfdbInfo);
  }
}
/*********************************************************************
* @purpose  Deletes an entry from the mfdb. 
*          
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_INTF_MASK_t   dstIntfMask     @b{(input)} The list of ports
*                                            configured for the entry
*           L7_BOOL  entryPresent  @b((input)) True if the entry has already 
*                                             been added to the database
*	
* @returns  L7_SUCCESS on succes
* @returns  L7_FAILURE otherwise
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t filterMfdbEntryDelete(L7_uint32 vlanId,L7_uchar8* macAddr)
{
  mfdbMemberInfo_t mfdbInfo;
  /*delete this entry from mfdb*/
  bzero((char*)&mfdbInfo, (L7_int32)sizeof(mfdbMemberInfo_t));

  mfdbInfo.vlanId = vlanId;
  memcpy((void *)mfdbInfo.macAddr, macAddr, L7_MAC_ADDR_LEN);
  mfdbInfo.user.componentId = L7_MFDB_PROTOCOL_STATIC;
  mfdbInfo.user.type = L7_MFDB_TYPE_STATIC;

  /* delete this static filter VLANID-MAC entry from the MFDB table, if it exists */
  return mfdbEntryDelete(&mfdbInfo);

}
/*********************************************************************
* @purpose  Deletes the set of ports from an mfdb entry
*          
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_INTF_MASK_t   dstIntfMask     @b{(input)} The list of ports
*                                            configured for the entry
*	
* @returns  L7_SUCCESS on succes
* @returns  L7_FAILURE otherwise
*
* @notes 
*
* @end
*
*********************************************************************/
L7_RC_t filterMfdbEntryPortsDelete(L7_uint32 vlanId,L7_uchar8* macAddr, L7_INTF_MASK_t *dstIntfMask,
                                   L7_INTF_MASK_t *dstIntfFilterMask)
{
  mfdbMemberInfo_t mfdbInfo;

  /*delete dstIntf from this entry in mfdb*/
  bzero((char*)&mfdbInfo, (L7_int32)sizeof(mfdbMemberInfo_t));
  

  mfdbInfo.vlanId = vlanId;
  memcpy((void *)mfdbInfo.macAddr, macAddr, L7_MAC_ADDR_LEN);
  mfdbInfo.user.componentId = L7_MFDB_PROTOCOL_STATIC;
  mfdbInfo.user.type = L7_MFDB_TYPE_STATIC;
  memcpy((void *)mfdbInfo.user.description, 
         (void *)L7_MFDB_MGMT_CONFIGURED, L7_MFDB_COMPONENT_DESCR_STRING_LEN);

  if (dstIntfMask != L7_NULLPTR)
  {
    memcpy(&mfdbInfo.user.fwdPorts, dstIntfMask, sizeof(L7_INTF_MASK_t));
  }

  if (dstIntfFilterMask != L7_NULLPTR)
  {
    memcpy(&mfdbInfo.user.fltPorts, dstIntfFilterMask, sizeof(L7_INTF_MASK_t));
  }

  /* delete port(s) from this static filter VLANID-MAC entry in the MFDB table */
  return mfdbEntryPortsDelete(&mfdbInfo);
}
/*********************************************************************
*
* @purpose  Gets the number of MAC static Filters configured 
*
* @param    L7_uint32* count      @b((output))  NumberOf Filters configured
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is MAC Filter
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t filterCountGet(L7_uint32* count)
{
  *count = filterAvlTree.count;
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert filter bitmasks to SNMP octet-strings 
*
* @param    FILTER_INTF_MASK_t  in  @b((input))  mask to be converted  
* @param    FILTER_INTF_MASK_t* out @b((output)) pointer to storage to hold the converted mask
*
* @returns  L7_SUCCESS
*
* @notes    Works both ways, basically converts lsb to msb
*       
* @end
*********************************************************************/
L7_RC_t filterReverseMask(FILTER_INTF_MASK_t in, FILTER_INTF_MASK_t* out)
{
  L7_uint32 j,k;
  FILTER_INTF_MASK_t temp;
  memset((void *)&temp, 0, sizeof(temp));   

  for (k=0;k<FILTER_INTF_INDICES;k++)  /* bytes 0 to max-1 */
  {
    for (j=1;j<=8;j++)  /* interfaces 1 to 8 within a byte */
    {
      if (FILTER_INTF_ISMASKBITSET(in, 8*k + j)) /* kth byte, jth interface */
        FILTER_INTF_SETMASKBIT( temp,8*k + (8-j+1) ); /* reverse msb-lsb */
      else
        FILTER_INTF_CLRMASKBIT(temp, 8*k + (8-j+1) );
    }
  }

  memcpy(out, &temp, FILTER_INTF_INDICES);
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Convert a mask to a densely packed list of internal interface numbers 
*
* @param    FILTER_INTF_MASK_t*  mask     @b((input))  mask to be converted  
* @param    L7_uint32            list[]   @b{(output)} densely packed list 
* @param    L7_uint32*           numList  @b{(output)} number of interfaces in the above list
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*       
* @end
*********************************************************************/
L7_RC_t filterMaskToList(FILTER_INTF_MASK_t *mask, 
                         L7_uint32 list[], 
                         L7_uint32 *numList)
{
  L7_uint32 i;
  L7_uint32 count = 0;
  FILTER_INTF_MASK_t temp;

  memcpy((void *)&temp, (void *)mask, FILTER_INTF_INDICES);
  memset((void *)list,0x00,L7_FILTER_MAX_INTF);

  for (i=1;i<=FILTER_MAX_INTF_NUM;i++)
    if (FILTER_INTF_ISMASKBITSET(temp,i))
    {
      list[count] = i;
      count++;
    }
  *numList = count;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Convert a densely packed list of internal interface numbers to a mask
*
* @param    L7_uint32            list[]   @b{(input)} densely packed list 
* @param    L7_uint32            numList  @b{(input)} number of interfaces in the above list
* @param    FILTER_INTF_MASK_t*  mask     @b((output))  mask to be generated  
*
* @returns  L7_SUCCESS
*
* @notes    mask is filter oriented i.e. lsb/rightmost bit represents interafce 1
*       
* @end
*********************************************************************/
L7_RC_t filterListToMask(L7_uint32 list[], 
                         L7_uint32 numList, 
                         FILTER_INTF_MASK_t *mask)
{
  FILTER_INTF_MASK_t temp;
  L7_uint32 i;

  memset((void *)&temp,0x00,sizeof(FILTER_INTF_MASK_t));
  for (i=0;i<numList;i++)
  {
    FILTER_INTF_SETMASKBIT(temp,list[i]);
  }
  memcpy((void *)mask,(void *)&temp,sizeof(FILTER_INTF_MASK_t));
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose To get convert a Multicast MAC Addr into ip format
*
* @param macBuf  Array of 6 bytes containing the mac address
*
*
* @returns L7_char8
*
* @end
*
*********************************************************************/
L7_uchar8 *filterMulticastMacToIpFormat(L7_uchar8 *macBuf)
{
  static L7_uchar8 buf[32];

  if (macBuf == L7_NULLPTR)
  {
    sprintf(buf, "error");
  }
  else
  {
    if (macBuf[0] == 0x01 && macBuf[1] == 0x00 && macBuf[2] == 0x5E
        && macBuf[3] < 0x80)
      sprintf(buf, "224-239.%d|%d.%d.%d", (128 + macBuf[3]), macBuf[3],
              macBuf[4], macBuf[5]);
    else
      sprintf(buf, "%02x:%02x:%02x:%02x:%02x:%02x", macBuf[0], macBuf[1], macBuf[2],
              macBuf[3], macBuf[4], macBuf[5]);
  }
  return buf;
}


