/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    filter.c
* @purpose     Static MAC Filtering functions
* @component   Filter
* @comments    none
* @create      1/7/2001
* @author      skalyanam
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/


#include <string.h>
#include "l7_common.h"
#include "comm_mask.h"
#include "nimapi.h"
#include "sysapi.h"
#include "registry.h"
#include "log.h"
#include "defaultconfig.h"
#include "dtlapi.h"
#include "filter_api.h"
#include "filter.h"
#include "avl_api.h"
#include "dot3ad_api.h"
#include "mfdb_api.h"
#include "dot1q_api.h"

extern filterCfgData_t *filterCfgData;
extern avlTree_t filterAvlTree;
static const L7_uchar8 restrictedMacAddr[FILTER_MAX_RESTRICTED_ADDR][L7_MAC_ADDR_LEN] =
{
  {0x00,0x00,0x00,0x00,0x00,0x00},
  {0x01,0x80,0xC2,0x00,0x00,0x00},
  {0x01,0x80,0xC2,0x00,0x00,0x01},
  {0x01,0x80,0xC2,0x00,0x00,0x02},
  {0x01,0x80,0xC2,0x00,0x00,0x03},
  {0x01,0x80,0xC2,0x00,0x00,0x04},
  {0x01,0x80,0xC2,0x00,0x00,0x05},
  {0x01,0x80,0xC2,0x00,0x00,0x06},
  {0x01,0x80,0xC2,0x00,0x00,0x07},
  {0x01,0x80,0xC2,0x00,0x00,0x08},
  {0x01,0x80,0xC2,0x00,0x00,0x09},
  {0x01,0x80,0xC2,0x00,0x00,0x0A},
  {0x01,0x80,0xC2,0x00,0x00,0x0B},
  {0x01,0x80,0xC2,0x00,0x00,0x0C},
  {0x01,0x80,0xC2,0x00,0x00,0x0D},
  {0x01,0x80,0xC2,0x00,0x00,0x0E},
  {0x01,0x80,0xC2,0x00,0x00,0x0F},
  {0x01,0x80,0xC2,0x00,0x00,0x20},
  {0x01,0x80,0xC2,0x00,0x00,0x21},
  {0x01,0x80,0xC2,0x00,0x00,0x22},
  {0x01,0x80,0xC2,0x00,0x00,0x23},
  {0x01,0x80,0xC2,0x00,0x00,0x24},
  {0x01,0x80,0xC2,0x00,0x00,0x25},
  {0x01,0x80,0xC2,0x00,0x00,0x26},
  {0x01,0x80,0xC2,0x00,0x00,0x27},
  {0x01,0x80,0xC2,0x00,0x00,0x28},
  {0x01,0x80,0xC2,0x00,0x00,0x29},
  {0x01,0x80,0xC2,0x00,0x00,0x2A},
  {0x01,0x80,0xC2,0x00,0x00,0x2B},
  {0x01,0x80,0xC2,0x00,0x00,0x2C},
  {0x01,0x80,0xC2,0x00,0x00,0x2D},
  {0x01,0x80,0xC2,0x00,0x00,0x2E},
  {0x01,0x80,0xC2,0x00,0x00,0x2F},
  {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}
};

L7_uint32 numNonMfdbEntries=0;
L7_uint32 numMfdbEntries=0;
L7_uint32 numFilterEntries=0;

/* This macro would return num of bits set in the interface mask.*/
#define L7_INTF_NUM_MASKBITS(j, num)          \
{                                             \
  L7_uchar8 temp = 0, x,y;                    \
  *num = 0;                                   \
                                              \
  for (x = 0; x < L7_INTF_INDICES ; x++)   {   \
    temp = j.value[x];                        \
    for (y = 0; y < 8; y++)  {                 \
      if (temp & 1)                           \
      {                                   \
        (*num)++;                        \
      }                        \
      temp = temp>>1;                        \
    }                        \
  }                        \
}
/*********************************************************************
*
* @purpose  Get the number of interfaces set in a mask.
*
* @param   
* @param    
* @param    
*          
*
* @returns  L7_SUCCESS, if success
* @returns  
* @returns  
*
* @notes    
*           
*           
*
* @end
*********************************************************************/
static L7_RC_t filterIntfListGet(L7_INTF_MASK_t* mask, L7_BOOL checkNimState, L7_uint32* numIntf, L7_uint32 intfList[])
{
  L7_INTF_MASK_t tempMask;
  L7_uint32 index;
  L7_NIM_QUERY_DATA_t       nimQueryData;
  L7_uint32 numIntfTemp = L7_NULL;

  *numIntf = 0;
  memset((void *)&nimQueryData,0,sizeof(nimQueryData));
  nimQueryData.request = L7_NIM_QRY_RQST_STATE;
  memcpy(&tempMask, mask, sizeof(tempMask));

  L7_INTF_FHMASKBIT(tempMask, index);
  L7_INTF_NUM_MASKBITS(tempMask, numIntf);
  numIntfTemp = (*numIntf)-1;
  while (index != 0)
  {

    nimQueryData.intIfNum = index;
    if ((checkNimState == L7_TRUE) && nimIntfQuery(&nimQueryData) != L7_SUCCESS)
    {
        /* should never get here */
        LOG_MSG(" nimIntfQuery failed for intIfNum %d\n", index);
        continue;
    }
    /* constructing a list of interfaces that are ATTACHED or ATTACHING for purposes of
     * passing this to the dtl call*/
    if  ( (checkNimState == L7_FALSE) ||(nimQueryData.data.state == L7_INTF_ATTACHED || 
             nimQueryData.data.state == L7_INTF_ATTACHING))
    {
      /* This is to return the interfaces in sorted ascending order.
         This is because the the indices are considered from most significant bits.*/
      intfList[numIntfTemp] = index;
      numIntfTemp--;
    }
    else
    {
      if (*numIntf != 0)
      {
        *numIntf = *numIntf - 1;
      }
    }


    L7_INTF_CLRMASKBIT(tempMask,index);
	/* Get the next valid interface */
    L7_INTF_FHMASKBIT(tempMask, index);
  }

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Create a MAC Static Filter if it is not already present.
*
* @param    L7_uchar8*  macAddr   @b((input)) Static MAC Address for the filter
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the Filter  
*                            
*
* @returns  the filter entry created on success
* @returns  L7_NULLPTR on failure
*
* @notes  This function assumes that the caller has acquired 
*         the appropriate semaphore. 
*
* @end
*********************************************************************/
static filterInfoData_t *filterCreateIfNotPresent(L7_uchar8* macAddr,
                     L7_uint32 vlanId)
{
  L7_RC_t rc;
  L7_BOOL entryMulticast;
  filterInfoData_t *entry = L7_NULLPTR;
  
 
    /*check for illegal mac addresses*/
  rc = filterMacAddrCheck(macAddr);
  if (rc==L7_FAILURE)
    /*illegal MAC address*/
    return entry;

  /*does a filter with the same mac addr and vlan id already exist*/
  entry = filterEntryFind(macAddr,vlanId,L7_MATCH_EXACT);
  if (entry != L7_NULLPTR)
  {/*entry exists return already configured*/
    return entry;
  }

  /* make sure the vlan is created first 
    - this is required for configuration migration, 
     where vlan create message can still be in queue*/

  if (dot1qVlanQueueSyncCheckValid(vlanId) != L7_SUCCESS)
  { 
    /* vlan does not exist */
    LOG_MSG( "Vlan %d does not exist . Cannot create static filter.",vlanId);
    return entry;
  }

  /*check for the mac address type - unicast or multicast*/
  entryMulticast = filterIsMacAddrTypeMulticast(macAddr);


  if(entryMulticast==L7_FALSE) 
  {
    return entry;
  }
  
  
  if (numMfdbEntries < L7_MAX_GROUP_REGISTRATION_ENTRIES)
  {
    /* first add this entry to the fiter database*/
    rc = filterEntryAdd(macAddr, vlanId);
    if (rc == L7_SUCCESS)
    {
        filterCfgData->cfgHdr.dataChanged = L7_TRUE;
        return filterEntryFind(macAddr,vlanId,L7_MATCH_EXACT);

    }
      
  } /* numMfdbEntries < MAX */

      
  
  return L7_NULLPTR;
}

/************************************************************************/
/*                           APIs                                       */
/************************************************************************/

/*********************************************************************
* @purpose  Set multicast address filtering configuration.
*
* @param    L7_BOOL        enable    @{{input}}  enable/disable filtering
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t filterAdminModeSet(L7_BOOL enable)
{
  L7_RC_t rc = L7_FAILURE;

  if (enable != L7_TRUE &&
      enable != L7_FALSE)
  {
    return L7_FAILURE;
  }
 /* osapiSemaTake(mfdbSemaphore, L7_WAIT_FOREVER);*/
  osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  if (enable == filterCfgData->filteringEnabled)
  {
    /*osapiSemaGive(mfdbSemaphore);*/
    osapiSemaGive(filterAvlTree.semId);
    return L7_SUCCESS;
  }

  filterCfgData->filteringEnabled = enable;
  filterCfgData->cfgHdr.dataChanged = L7_TRUE;

  rc = filterAdminModeApply(filterCfgData->filteringEnabled);
  /*osapiSemaGive(mfdbSemaphore);*/
  osapiSemaGive(filterAvlTree.semId);

  return rc;
}

/*********************************************************************
* @purpose  Get multicast address filtering configuration.
*
* @param    L7_BOOL        *enable    @{{output}}  enable/disable filtering
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t filterAdminModeGet(L7_BOOL *enable)
{
  if (enable == L7_NULLPTR)
  {
    return L7_FAILURE;
  }
  /*osapiSemaTake(mfdbSemaphore, L7_WAIT_FOREVER);*/
  *enable = filterCfgData->filteringEnabled;
  /*osapiSemaGive(mfdbSemaphore);*/

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Create a MAC Static Filter
*
* @param    L7_uchar8*  macAddr   @b((input)) Static MAC Address for the filter
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the Filter  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ALREADY_CONFIGURED, if the filter already exists
* @returns  L7_FAILURE, if other failure
*
* @notes    This section of code was modified. 
*			The design philosophy now is to let mfdb tell dtl/driver about 
* 			all dst interfaces related to multicast addresses irrespective whether it is 
* 			static (filter) or dynamic (IGMP Snoop or GMRP) in origination
* 			The filter component will continue to tell dtl/driver about
* 			static multicast src interfaces, unicast src and unicast dst interfaces
*
* @end
*********************************************************************/
L7_RC_t filterCreate(L7_uchar8* macAddr,
                     L7_uint32 vlanId)
{
  L7_RC_t rc,rc1;
  L7_BOOL entryMulticast;
  L7_BOOL srcPresent;
  
 
    /*check for illegal mac addresses*/
  rc = filterMacAddrCheck(macAddr);
  if (rc==L7_FAILURE)
    /*illegal MAC address*/
    return L7_FAILURE;

  /* make sure the vlan is created first 
    - this is required for configuration migration, 
     where vlan create message can still be in queue*/

  if (dot1qVlanQueueSyncCheckValid(vlanId) != L7_SUCCESS)
  { 
    /* vlan does not exist */
    LOG_MSG( "Vlan %d does not exist . Cannot create static filter.",vlanId);
    return L7_FAILURE;
  }

  /*check for the mac address type - unicast or multicast*/
  entryMulticast = filterIsMacAddrTypeMulticast(macAddr);

  srcPresent = cnfgrIsFeaturePresent(L7_FILTER_COMPONENT_ID, 
									 L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID);


  rc = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  /*has a filter with the same mac addr and vlan id already exist*/
  if (filterEntryFind(macAddr,vlanId,L7_MATCH_EXACT)!=L7_NULLPTR)
  {/*entry exists return already configured*/
    rc = osapiSemaGive(filterAvlTree.semId);
    return L7_ALREADY_CONFIGURED;
  }

  if(entryMulticast==L7_FALSE) 
  {
    printf("[PTin] %s(%d) I was here\r\n", __FUNCTION__, __LINE__);

    if (numNonMfdbEntries >= L7_MAX_FDB_STATIC_FILTER_ENTRIES )
    {
      rc = osapiSemaGive(filterAvlTree.semId);
      return L7_TABLE_IS_FULL;
    }
    else
    {
      
      rc = filterEntryAdd(macAddr, vlanId);
      numFilterEntries++;
      if (rc== L7_SUCCESS)
      {
        /*call dtl routine to create a filter*/
        rc1 = dtlFilterAdd(macAddr, vlanId, 0, L7_NULLPTR, 0, L7_NULLPTR);
        if (rc1==L7_SUCCESS)
        {
           numNonMfdbEntries++;
        }
       
      }
      else
      {
        rc1 = filterEntryDelete(macAddr,vlanId);
        if (rc1==L7_SUCCESS)
        {
          numFilterEntries--;
        }

      }/* filterEntryAdd returned a failure*/
    }
  }
  
  filterCfgData->cfgHdr.dataChanged = L7_TRUE;
  if (entryMulticast == L7_TRUE)
  {
    printf("[PTin] %s(%d) I was here\r\n", __FUNCTION__, __LINE__);
   /* NOTE: This entry is added to the mfdb only if the 
    *       destination port feature is supported, else not.
    */
   /* NOTE: Destination ports setting is applicable for static multicast addresses, 
    *      for all platforms */
    if (numMfdbEntries < L7_MAX_GROUP_REGISTRATION_ENTRIES)
    {
      /* first add this entry to the fiter database*/
      rc = filterEntryAdd(macAddr, vlanId);
			
      printf("[PTin] %s(%d) I was here\r\n", __FUNCTION__, __LINE__);

      /*Now add the entry to mfdb*/		
      if(rc == L7_SUCCESS && filterCfgData->filteringEnabled == L7_TRUE)
      {
        printf("[PTin] %s(%d) I was here\r\n", __FUNCTION__, __LINE__);
          if(filterMfdbEntryAdd(vlanId,macAddr,L7_NULL,L7_NULL)!= L7_SUCCESS)
          {
            rc = osapiSemaGive(filterAvlTree.semId);
            return L7_FAILURE;
          }
          numMfdbEntries++; 
      }
      printf("[PTin] %s(%d) I was here\r\n", __FUNCTION__, __LINE__);

    } /* numMfdbEntries < MAX */

      
  }
  rc = osapiSemaGive(filterAvlTree.semId);
  return rc;
}


/*********************************************************************
*
* @purpose  Make dtl calls for adding/removing interfaces from mac filters
*
* @param    L7_uchar8*  macAddr   @b((input)) Static MAC Address for the filter
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the Filter  
* @param    filterInfoData_t   filterEntry  @b{(input)} Filter entry structure
*               containing list of source and destination ports that are part 
*               of the filter
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_ALREADY_CONFIGURED, if the filter already exists
* @returns  L7_FAILURE, if other failure
*
* @notes    This sole purpose of this function is to parse the list of ALL 
*           interfaces that are assigned to the mac filter and make a dtl call 
*           with only the SUBSET of this list which comprises of ATTACHED or 
*           ATTACHING interfaces
*
* @end
*********************************************************************/

L7_RC_t filterIntfModify(L7_uchar8* macAddr, L7_uint32 vlanId, 
                              filterInfoData_t  *filterEntry)
{
    L7_uint32 srcIntfList[L7_FILTER_MAX_INTF];
    L7_uint32 dstIntfList[L7_FILTER_MAX_INTF];               
    L7_uint32 numDstIntf, numSrcIntf;
    L7_RC_t rc;

    numSrcIntf = 0;
    numDstIntf = 0;
    
   
    filterIntfListGet(&filterEntry->srcIntfMask, L7_TRUE, &numSrcIntf, srcIntfList);
    filterIntfListGet(&filterEntry->dstIntfMask, L7_TRUE, &numDstIntf, dstIntfList);
   
    /* making the dtl call with only attached and attaching 
     * interfaces (no pre-configured interfaces*/
    rc = dtlFilterAdd(macAddr, vlanId, 
                      numSrcIntf, srcIntfList, 
                      numDstIntf, dstIntfList);

    return rc;
}


/*********************************************************************
*
* @purpose  Adds a source interface to the static MAC Filter 
*
* @param    L7_uchar8*  macAddr   @b((input)) MAC Address of the Filter
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the Filter
* @param    L7_uint32   srcIntf   @b{(input)} Internal Interface number of the source port  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    adds src interface at the end of densely packed list
*           This section of code was modified. 
*			The design philosophy now is to let mfdb tell dtl/driver about 
* 			all dst interfaces related to multicast addresses irrespective whether it is 
* 			static (filter) or dynamic (IGMP Snoop or GMRP) in origination
* 			The filter component will continue to tell dtl/driver about
* 			static multicast src interfaces, unicast src and unicast dst interfaces
*
*
* @end
*********************************************************************/
L7_RC_t filterSrcIntfAdd(L7_uchar8* macAddr,
                         L7_uint32 vlanId, 
                         L7_uint32 srcIntf)
{
  L7_RC_t rc,rc1;
  filterInfoData_t* filterEntry;
  filterInfoData_t  backupEntry;
  L7_uint32 index;
  L7_BOOL srcPresent;
  L7_BOOL entryMulticast;

  /*check for the mac address type - unicast or multicast*/
  entryMulticast = filterIsMacAddrTypeMulticast(macAddr);

  srcPresent = cnfgrIsFeaturePresent(L7_FILTER_COMPONENT_ID, 
									 L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID);

  /*check if we have an acceptable srcIntf*/
  rc = filterIntfCheck(srcIntf);
  if (rc==L7_FAILURE)
  {
    return L7_FAILURE;
  }

  rc = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  filterEntry = filterEntryFind(macAddr,vlanId,L7_MATCH_EXACT);
  if (filterEntry == L7_NULLPTR)
  {
    /*entry not found*/
    rc = osapiSemaGive(filterAvlTree.semId);
    return L7_FAILURE;
  }

   /* this filter was created . Check if this filter exceeds the maximum number of static
     multicast entries allowed */
  if ((entryMulticast == L7_TRUE) && (srcPresent==L7_TRUE) && (filterEntry->numSrc==0))
  {
     if (numNonMfdbEntries <= L7_MAX_FDB_STATIC_FILTER_ENTRIES)
     {
        /*call dtl routine to create a filter*/
        rc = dtlFilterAdd(macAddr, vlanId, 0, L7_NULLPTR, 0, L7_NULLPTR);  
        if (rc==L7_SUCCESS)
        {
          numNonMfdbEntries++;
        }
        
     }
     else
     {
       rc = osapiSemaGive(filterAvlTree.semId);
       return L7_TABLE_IS_FULL;
     }
  }


  /*this filter was created so proceed to add src interface*/
  /*is there space in the list?*/
  if (filterEntry->numSrc >= L7_FILTER_MAX_INTF)
  {
    rc = osapiSemaGive(filterAvlTree.semId);
    return L7_FAILURE;
  }
  /*copy the filterEntry into backup entry*/
  memcpy((void *)&backupEntry,(void *)filterEntry,sizeof(filterInfoData_t));

  if (L7_INTF_ISMASKBITSET(backupEntry.srcIntfMask, srcIntf) == L7_FALSE)
  {
    index = backupEntry.numSrc;
    L7_INTF_SETMASKBIT(backupEntry.srcIntfMask, srcIntf);
    backupEntry.numSrc++;
  }

  /* if the interface to be added is in attached state, call dtl with this 
   * latest information. Check is done to make sure interface is not detached 
   * to avoid making dtl and subsequent dapi calls for interfaces on 
   * pre-configured units. This is done in the filterIntfModify function. */
                  
  if (entryMulticast == L7_FALSE || srcPresent == L7_TRUE)
  {
    rc = filterIntfModify(macAddr, vlanId, &backupEntry);
  }

  if (rc==L7_SUCCESS)
  {
    /*update the filter entry only on successful dtl execution or for 
    non-attached interfaces*/ 
    memcpy((void *)filterEntry,(void *)&backupEntry,sizeof(filterInfoData_t));
    filterCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  rc1 = osapiSemaGive(filterAvlTree.semId);
  return rc;
}

/*********************************************************************
*
* @purpose  Deletes a source port from the source port list of the MAC Filter 
*
* @param    L7_uchar8*  macAddr   @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the Filter
* @param    L7_uint32   srcIntf   @b{(input)} Internal Interface number of the source port  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    deletes src from list and reorders list densely
* 		    This section of code was modified. 
*			The design philosophy now is to let mfdb tell dtl/driver about 
* 			all dst interfaces related to multicast addresses irrespective whether it is 
* 			static (filter) or dynamic (IGMP Snoop or GMRP) in origination
* 			The filter component will continue to tell dtl/driver about
* 			static multicast src interfaces, unicast src and unicast dst interfaces
*
*
* @end
*********************************************************************/
L7_RC_t filterSrcIntfDelete(L7_uchar8* macAddr,
                            L7_uint32 vlanId,
                            L7_uint32 srcIntf)
{
  L7_RC_t rc,rc1;
  filterInfoData_t* filterEntry;
  filterInfoData_t  backupEntry;
  L7_BOOL srcPresent;
  L7_BOOL entryMulticast;

  /*check for the mac address type - unicast or multicast*/
  entryMulticast = filterIsMacAddrTypeMulticast(macAddr);

  srcPresent = cnfgrIsFeaturePresent(L7_FILTER_COMPONENT_ID, 
									 L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID);

  rc = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  filterEntry = filterEntryFind(macAddr,vlanId,L7_MATCH_EXACT);
  if (filterEntry == L7_NULLPTR)
  {
    rc = osapiSemaGive(filterAvlTree.semId);
    return L7_FAILURE;
  }
  /*copy the filterEntry into backup entry*/
  memcpy((void *)&backupEntry,(void *)filterEntry,sizeof(filterInfoData_t));

  /*entry is found proceed to delete*/
   if (L7_INTF_ISMASKBITSET(backupEntry.srcIntfMask, srcIntf))
  {
      backupEntry.numSrc--;
      L7_INTF_CLRMASKBIT(backupEntry.srcIntfMask,srcIntf);

      /* if the interface to be added is in attached state, call dtl with this 
       * latest information. Check is done to make sure interface is not detached 
       * to avoid making dtl and subsequent dapi calls for interfaces on 
       * pre-configured units. This is done in the filterIntfModify function. */

      if (entryMulticast == L7_FALSE || srcPresent == L7_TRUE)
	  {
        rc = filterIntfModify(macAddr, vlanId, &backupEntry);
	  }
      
      if (rc==L7_SUCCESS)
      {
          
          /* if src list is empty for a multicast mac address remove from dtl entry for that mac+vid*/
          if ((entryMulticast==L7_TRUE) && (srcPresent==L7_TRUE) && (backupEntry.numSrc == 0))
          {
             /*tell dtl to delete this filter*/
            rc = dtlFilterDelete(macAddr,vlanId);
            if (rc==L7_SUCCESS)
            {
               numNonMfdbEntries--;
            }
          }
         /*operation successfull copy the backup into the filter entry*/
          memcpy((void *)filterEntry,(void *)&backupEntry,sizeof(filterInfoData_t));
          filterCfgData->cfgHdr.dataChanged = L7_TRUE;
      }
  }
  else
    rc = L7_FAILURE;
  
  rc1 = osapiSemaGive(filterAvlTree.semId);
  return rc;
}

/*********************************************************************
*
* @purpose  Adds a destination interface to the static MAC Filter 
*
* @param    L7_uchar8* macAddr   @b((input)) MAC Address of the Filter
* @param    L7_uint32  vlanId    @b{(input)} Vlan ID for the Filter
* @param    L7_uint32  dstIntf   @b{(input)} Internal Interface number of the destination port  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    This section of code was modified. 
*			The design philosophy now is to let mfdb tell dtl/driver about 
* 			all dst interfaces related to multicast addresses irrespective whether it is 
* 			static (filter) or dynamic (IGMP Snoop or GMRP) in origination
* 			The filter component will continue to tell dtl/driver about
* 			static multicast src interfaces, unicast src and unicast dst interfaces
*
* @end
*********************************************************************/
L7_RC_t filterDstIntfAdd(L7_uchar8* macAddr,
                         L7_uint32 vlanId, 
                         L7_uint32 dstIntf)
{
  L7_RC_t rc,rc1;
  L7_BOOL entryMulticast;
  filterInfoData_t* filterEntry;
  filterInfoData_t  backupEntry;
  L7_INTF_MASK_t tmp;
  L7_BOOL srcPresent;
  L7_NIM_QUERY_DATA_t nimQueryData;
 

  /*check if we have an acceptable dstIntf*/
  rc = filterIntfCheck(dstIntf);
  if (rc==L7_FAILURE)
  {
    return L7_FAILURE;
  }

  /*check for the mac address type - unicast or multicast*/
  entryMulticast = filterIsMacAddrTypeMulticast(macAddr);

  srcPresent = cnfgrIsFeaturePresent(L7_FILTER_COMPONENT_ID, 
									 L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID); 

  rc1 = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  filterEntry = filterEntryFind(macAddr,vlanId,L7_MATCH_EXACT);
  if (filterEntry == L7_NULLPTR)
  {
    /*entry not found*/
    rc1 = osapiSemaGive(filterAvlTree.semId);
    return L7_FAILURE;
  }                                                          
  
 
  /*copy the filterEntry into backup entry*/
  memcpy((void *)&backupEntry,(void *)filterEntry,sizeof(filterInfoData_t));
  
  if (L7_INTF_ISMASKBITSET(backupEntry.dstIntfMask, dstIntf) == L7_FALSE)
  {
	/* Only update the filter entry if this is unique. Allowing duplicate 
	 * ports to go through so that they can be added in mfdb if they come from intfApply
	 * code path
	 */
    L7_INTF_SETMASKBIT(backupEntry.dstIntfMask,dstIntf);
  }

  if (filterCfgData->filteringEnabled == L7_TRUE)
  {
    
      /* NOTE: Destination ports setting is applicable for static multicast addresses, 
             *      for all platforms */
      if (entryMulticast == L7_TRUE)
      {
        nimQueryData.intIfNum = dstIntf;
        nimQueryData.request = L7_NIM_QRY_RQST_STATE;
    
        if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
        {
          return L7_FAILURE;/* should never get here*/
        }
        /* only talk to the hardware when the hardware is valid 
           MFDB is not making any checks to see if the ports are valid. 
           MFDB needs to be told only when the entries can actually
           be sent to the hardware. Hence if the interface is not attached
           or attaching then do not inform mfdb.
         */
        if ((nimQueryData.data.state == L7_INTF_ATTACHING) ||
            (nimQueryData.data.state == L7_INTF_ATTACHED))
        {
          memset(&tmp, 0, sizeof(L7_INTF_MASK_t));
          L7_INTF_SETMASKBIT(tmp, dstIntf);
          rc = filterMfdbEntryAdd(vlanId,macAddr, &tmp, L7_NULL);
        
          if (rc == L7_SUCCESS)
          {
            numMfdbEntries++;
          }
        }
        
      }
      else
      {
          /* if the interface to be added is in attached state, call dtl with this 
           * latest information. Check is done to make sure interface is not detached 
           * to avoid making dtl and subsequent dapi calls for interfaces on 
           * pre-configured units */
          rc = filterIntfModify(macAddr, vlanId, &backupEntry);
      }
  }/* If filtering is enabled */

  if (rc == L7_SUCCESS)
  {
    /* update the filter entry only on successful dtl execution*/
    memcpy((void *)filterEntry,(void *)&backupEntry,sizeof(filterInfoData_t));
    filterCfgData->cfgHdr.dataChanged = L7_TRUE;
  }
  rc1 = osapiSemaGive(filterAvlTree.semId);
  return rc;
}

/*********************************************************************
*
* @purpose  Deletes a destination port from the destination port list of the MAC Filter 
*
* @param    L7_uchar8*  macAddr   @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the Filter
* @param    L7_uint32   srcIntf   @b{(input)} Internal Interface number of the destination port  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    This section of code was modified. 
*			The design philosophy now is to let mfdb tell dtl/driver about 
* 			all dst interfaces related to multicast addresses irrespective whether it is 
* 			static (filter) or dynamic (IGMP Snoop or GMRP) in origination
* 			The filter component will continue to tell dtl/driver about
* 			static multicast src interfaces, unicast src and unicast dst interfaces
*
* @end
*********************************************************************/
L7_RC_t filterDstIntfDelete(L7_uchar8* macAddr,
                            L7_uint32 vlanId,
                            L7_uint32 dstIntf)
{
  L7_BOOL entryMulticast;
  filterInfoData_t* filterEntry;
  filterInfoData_t  backupEntry;
  mfdbMemberInfo_t mfdbInfo;
  L7_NIM_QUERY_DATA_t nimQueryData;
  L7_INTF_MASK_t tmp;
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t rc1 = L7_SUCCESS;
  L7_BOOL srcPresent;
  L7_BOOL dstIntfsPresent = L7_TRUE;

  memset(&backupEntry, 0x00, sizeof(filterInfoData_t));
  memset(&nimQueryData, 0x00, sizeof(L7_NIM_QUERY_DATA_t));
  memset(&mfdbInfo,  0x00,  sizeof(mfdbMemberInfo_t));
  memset(&tmp, 0x00, sizeof(L7_INTF_MASK_t));

  /*check for the mac address type - unicast or multicast*/
  entryMulticast = filterIsMacAddrTypeMulticast(macAddr);

  srcPresent = cnfgrIsFeaturePresent(L7_FILTER_COMPONENT_ID, 
									 L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID);

  rc1 = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  filterEntry = filterEntryFind(macAddr,vlanId,L7_MATCH_EXACT);
  if (filterEntry == L7_NULLPTR)
  {
    rc1 = osapiSemaGive(filterAvlTree.semId);
    return L7_FAILURE;
  }
  /*copy the filterEntry into backup entry*/
  memcpy((void *)&backupEntry,(void *)filterEntry,sizeof(filterInfoData_t));

  /*entry is found proceed to delete*/
  if (L7_INTF_ISMASKBITSET(backupEntry.dstIntfMask, dstIntf))
  {
    L7_INTF_CLRMASKBIT(backupEntry.dstIntfMask,dstIntf);
    

    /*check for the mac address type - unicast or multicast*/
    entryMulticast = filterIsMacAddrTypeMulticast(macAddr);
    
    /* NOTE: Port(s) can only be deleted from the multicast VLANID-MAC 
     *       entries in the MFDB table.
     */
    if (filterCfgData->filteringEnabled == L7_TRUE)
    {
    
        /*delete port only for multicast mac addresses*/
        if (entryMulticast == L7_TRUE)
        {
          nimQueryData.intIfNum = dstIntf;
          nimQueryData.request = L7_NIM_QRY_RQST_STATE;
    
          if (nimIntfQuery(&nimQueryData) != L7_SUCCESS)
          {
            return L7_FAILURE;/* should never get here*/
          }
          /* only talk to the hardware when the hardware is valid 
            MFDB is not making any checks to see if the ports are valid. 
            MFDB needs to be told only when the entries can actually
            be sent to the hardware. Hence if the interface is not attached
            or attaching then do not inform mfdb.
          */
          
          if ((nimQueryData.data.state == L7_INTF_ATTACHING) ||
              (nimQueryData.data.state == L7_INTF_ATTACHED) ||
              (nimQueryData.data.state == L7_INTF_DETACHING))
          {
            memset(&tmp, 0, sizeof(L7_INTF_MASK_t));
            L7_INTF_SETMASKBIT(tmp, dstIntf);
            rc = filterMfdbEntryPortsDelete(vlanId,macAddr,&tmp, L7_NULLPTR);
    
          }
    
          /* if this is the last destination port in the list, then remove the corresponding mfdb entry 
           * if the a source list has been configured for this port , else allow it to be a 
           * filter entry with zero destination ports */
          L7_INTF_NONZEROMASK(backupEntry.dstIntfMask, dstIntfsPresent)
          if (dstIntfsPresent == L7_FALSE) 
          {
            rc1 = filterMfdbEntryDelete(vlanId,macAddr);
    
            if (rc1==L7_SUCCESS)
            {
              numMfdbEntries--;
            }
          }
         /*if there is no port in the dst list, we will delete the 
           entry from the hardware.we need to add it again as we need 
           to make sure if that traffic is blocked if there are no ports 
           in the dest list
         */
         if((rc1 == L7_SUCCESS) && (mfdbEntryExist(macAddr,vlanId) == L7_NOT_EXIST))
         {
           if(filterMfdbEntryAdd(vlanId,macAddr, L7_NULLPTR, L7_NULLPTR) != L7_SUCCESS)
	   {
	     rc1 = osapiSemaGive(filterAvlTree.semId);
	     return L7_FAILURE;
	   }
         }
          	
          
       }
       else
       {
         /* if the interface to be added is in attached state, call dtl with this 
         * latest information. Check is done to make sure interface is not detached 
         * to avoid making dtl and subsequent dapi calls for interfaces on 
         * pre-configured units */
        
        rc = filterIntfModify(macAddr, vlanId, &backupEntry);
      }
    }/* if filtering enabled*/

    if (rc == L7_SUCCESS)
    {
      /* update the filter entry only on successful dtl execution*/
      memcpy((void *)filterEntry,(void *)&backupEntry,sizeof(filterInfoData_t));
      filterCfgData->cfgHdr.dataChanged = L7_TRUE;
    }
  }
  else
    rc = L7_FAILURE;

  rc1 = osapiSemaGive(filterAvlTree.semId);
  return rc;
}

static void filterInterfacesCompare(L7_INTF_MASK_t *actorMask,
                              L7_INTF_MASK_t *refMask)
{
  L7_INTF_MASK_t tempMask;
  L7_BOOL isMaskNonZero = L7_TRUE;
  
  /* */
  memcpy(&tempMask,&actorMask,sizeof(tempMask));
  L7_INTF_MASKANDEQ(tempMask,*refMask);
  L7_INTF_NONZEROMASK(tempMask,isMaskNonZero);
  if (isMaskNonZero == L7_TRUE)
  {                     
     /* Clean out the offending bits*/
     L7_INTF_MASKANDEQINV(*actorMask,tempMask);

   /* TBD */
    /* This is an error condition we need to clear both the masks*/
  }
  return;
}

/*********************************************************************
*
* @purpose  Adds destination interfaces to the static MAC Filter 
*
* @param    L7_ushort16             vlanId     @{{input}}  VLAN ID
* @param    L7_enetMacAddr_t        macAddr    @{{input}}  MAC Address
* @param    L7_INTF_MASK_t          forward    @{{input}}  ports to forward
* @param    L7_INTF_MASK_t          filter     @{{input}}  ports to forbid
*
* @returns  L7_SUCCESS, entry added or exact entry exists
* @returns  L7_FAILURE
*
* @notes  If the entry already exists, this will add the indicated ports.
*         This function is only for destination /multicast entries and 
*         hence no processing for unicast addresses.
* @end
*
* @end
*********************************************************************/
L7_RC_t filterDstIntfMaskAdd( L7_uint32 vlanId, 
                              L7_enetMacAddr_t macAddr,
                              L7_INTF_MASK_t fwdMask,
                              L7_INTF_MASK_t filterMask)
{
  L7_RC_t rc= L7_FAILURE;
  L7_BOOL entryMulticast;
  filterInfoData_t* filterEntry;
  L7_BOOL fwdMaskNonZero= L7_FALSE, filterMaskNonZero = L7_FALSE;

  /*check if we have an acceptable dstIntf*/
 
  /*check for the mac address type - unicast or multicast*/
  entryMulticast = filterIsMacAddrTypeMulticast(macAddr.addr);
  if (entryMulticast != L7_TRUE)
  {
      /* This function only acts on multicast addresses*/
      return rc;
  }

  rc = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  filterEntry = filterCreateIfNotPresent(macAddr.addr,vlanId);
  if (filterEntry == L7_NULLPTR)
  {
    /*entry not found*/
    rc = osapiSemaGive(filterAvlTree.semId);
    return L7_FAILURE;
  }
  
  /* check If both incoming forward ports and filter ports are same*/
  filterInterfacesCompare(&fwdMask,&filterMask);

  /* Should check if the incoming forward ports are not already filtered*/
  filterInterfacesCompare(&fwdMask, &filterEntry->dstIntfFilterMask);
  L7_INTF_NONZEROMASK(fwdMask,fwdMaskNonZero);


  /* Check if the incoming filter ports are already in the forward mask
     The user needs to delete that forwarding entry before adding a filtering entry
  */
  filterInterfacesCompare(&filterMask, &filterEntry->dstIntfMask);
  L7_INTF_NONZEROMASK(filterMask, filterMaskNonZero);

  /* Update the filtering entry*/
  if (fwdMaskNonZero)
  {
      L7_INTF_MASKOREQ(filterEntry->dstIntfMask, fwdMask);
  }                 

  if (filterMaskNonZero)
  {
      L7_INTF_MASKOREQ(filterEntry->dstIntfFilterMask, filterMask);
  }

  /*  Only add the entry to mfdb if there is any processing needed */
  if (filterCfgData->filteringEnabled == L7_TRUE)
  {
	rc = filterMfdbEntryAdd(vlanId, macAddr.addr, &fwdMask, &filterMask);
    
    if (rc == L7_SUCCESS)
    {
      numMfdbEntries++;
    }
  }

  if (rc == L7_SUCCESS)
  {
    /* update the filter entry only on successful dtl execution*/
    filterCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  osapiSemaGive(filterAvlTree.semId);
  return rc;
}

/*********************************************************************
*
* @purpose  Removes destination interfaces from the static MAC Filter 
*
* @param    L7_ushort16             vlanId     @{{input}}  VLAN ID
* @param    L7_enetMacAddr_t        macAddr    @{{input}}  MAC Address
* @param    L7_INTF_MASK_t          forward    @{{input}}  ports to forward
* @param    L7_INTF_MASK_t          filter     @{{input}}  ports to forbid
*
* @returns  L7_SUCCESS, entry added or exact entry exists
* @returns  L7_FAILURE
*
* @notes  This function is only for destination /multicast entries and 
*         hence no processing for unicast addresses.
* @end
*
* @end
*********************************************************************/
L7_RC_t filterDstIntfMaskDelete( L7_uint32 vlanId, 
                              L7_enetMacAddr_t macAddr,
                              L7_INTF_MASK_t fwdMask,
                              L7_INTF_MASK_t filterMask)
{
  L7_RC_t rc= L7_FAILURE;
  L7_BOOL entryMulticast;
  filterInfoData_t* filterEntry;
  L7_BOOL fwdMaskNonZero= L7_FALSE, filterMaskNonZero = L7_FALSE;

  /*check if we have an acceptable dstIntf*/
 /* rc = filterIntfCheck(dstIntf);
  if (rc==L7_FAILURE)
  {
    return L7_FAILURE;
  } Need to clear out any other masks
 */
  /*check for the mac address type - unicast or multicast*/
  entryMulticast = filterIsMacAddrTypeMulticast(macAddr.addr);
  if (entryMulticast != L7_TRUE)
  {
      /* This function only acts on multicast addresses*/
      return rc;
  }


  rc = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  filterEntry = filterEntryFind(macAddr.addr, vlanId, L7_MATCH_EXACT);
  if (filterEntry == L7_NULLPTR)
  {
    /*entry not found*/
    rc = osapiSemaGive(filterAvlTree.semId);
    return L7_FAILURE;
  }
  
  /* check If both incoming forward ports and filter ports are same*/
  filterInterfacesCompare(&fwdMask,&filterMask);

  /* Should check if the incoming forward ports are not already filtered*/
  filterInterfacesCompare(&fwdMask, &filterEntry->dstIntfFilterMask);
  L7_INTF_NONZEROMASK(fwdMask,fwdMaskNonZero);


  /* Check if the incoming filter ports are already in the forward mask
     The user needs to delete that forwarding entry before adding a filtering entry
  */
  filterInterfacesCompare(&filterMask, &filterEntry->dstIntfMask);
  L7_INTF_NONZEROMASK(filterMask,filterMaskNonZero);

  /* Update the filtering entry*/
  if (fwdMaskNonZero)
  {
      L7_INTF_MASKANDEQINV(filterEntry->dstIntfMask,fwdMask);
  }                 

  if (filterMaskNonZero)
  {
      L7_INTF_MASKANDEQINV(filterEntry->dstIntfFilterMask,filterMask);
  }
  
  /*  Only add the entry to mfdb if there is any processing needed */
  if (((fwdMaskNonZero == L7_TRUE) || (filterMaskNonZero == L7_TRUE)) &&
      (filterCfgData->filteringEnabled == L7_TRUE))
  {
	rc = filterMfdbEntryPortsDelete(vlanId, macAddr.addr, &fwdMask, &filterMask);
  }

  if (rc == L7_SUCCESS)
  {
    /* update the filter entry only on successful dtl execution*/
    filterCfgData->cfgHdr.dataChanged = L7_TRUE;
  }

  osapiSemaGive(filterAvlTree.semId);
  return rc;
}

/*********************************************************************
*
* @purpose  Removes a configured MAC static Filter 
*
* @param    L7_uchar8*  macAddr   @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the Filter
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    This section of code was modified. 
*			The design philosophy now is to let mfdb tell dtl/driver about 
* 			all dst interfaces related to multicast addresses irrespective whether it is 
* 			static (filter) or dynamic (IGMP Snoop or GMRP) in origination
* 			The filter component will continue to tell dtl/driver about
* 			static multicast src interfaces, unicast src and unicast dst interfaces
*
* @end
*********************************************************************/
L7_uint32 filterRemove(L7_uchar8* macAddr, L7_uint32 vlanId)
{
  L7_RC_t rc,rc1;
  L7_BOOL entryMulticast;
  filterInfoData_t* filterEntry;
  filterInfoData_t  backupEntry;
  L7_BOOL srcPresent;
   
  /*check for the mac address type - unicast or multicast*/
  entryMulticast = filterIsMacAddrTypeMulticast(macAddr);

  srcPresent = cnfgrIsFeaturePresent(L7_FILTER_COMPONENT_ID, 
									 L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID);

  rc1 = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  filterEntry = filterEntryFind(macAddr,vlanId,L7_MATCH_EXACT);

  if (filterEntry == L7_NULLPTR)
  {
    rc1 = osapiSemaGive(filterAvlTree.semId);
    return L7_FAILURE;
  }

  /*copy the filterEntry into backup entry*/
  memcpy((void *)&backupEntry,(void *)filterEntry,sizeof(filterInfoData_t));

  rc = L7_SUCCESS;
  /*entry is found*/
  if (entryMulticast == L7_FALSE || (srcPresent == L7_TRUE))
  {
    if (((entryMulticast == L7_TRUE) && (srcPresent==L7_TRUE) && (numNonMfdbEntries > 0) && (backupEntry.numSrc > 0)) ||
        ((entryMulticast == L7_FALSE) && (numNonMfdbEntries > 0)))
    {
      /*tell dtl to delete this filter*/
      rc = dtlFilterDelete(macAddr,vlanId);
      if (rc==L7_SUCCESS)
      {
         numNonMfdbEntries--;
      }
    }
  }

  if (rc == L7_SUCCESS)
  {
    if (entryMulticast==L7_TRUE)
    {
      if (numMfdbEntries > 0)
      {
        rc1 = filterMfdbEntryDelete(vlanId,macAddr);
        if (rc1==L7_SUCCESS)
        {
          numMfdbEntries--;
        }
      }
      
    }
    else
    {
      /* Since we are not adding the unicast entry to the FDB there
	     is no reason to delete it from the FDB	as it is not there.
	  */
	  rc = L7_SUCCESS;
    }

    /*delete this entry from filter database*/
    if (rc == L7_SUCCESS)
    {
      rc = filterEntryDelete(macAddr, vlanId);
      filterCfgData->cfgHdr.dataChanged = L7_TRUE;
      if (rc==L7_SUCCESS)
      {
         numFilterEntries--;
      }
    }
  }
  else
  {
    LOG_MSG("FILTER: dtl filter delete failure for vlanid %u "
            "mac %02X:%02X:%02X:%02X:%02X:%02X\n", vlanId,
            macAddr[0], macAddr[1], macAddr[2], 
            macAddr[3], macAddr[4], macAddr[5]);
  }
   
  rc1 = osapiSemaGive(filterAvlTree.semId);
  return rc;
}

/*********************************************************************
*
* @purpose  Gets the first configured MAC static Filter 
*
* @param    L7_uchar8*  macAddr   @b((input)) MAC Address of the filter
* @param    L7_uint32*  vlanId    @b{(input)} Vlan ID for the Filter
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    macAddr is returned as L7_NULL if there are no configured MAC Filters
*
* @end
*********************************************************************/
L7_RC_t filterFirstGet(L7_uchar8* macAddr,
                       L7_uint32* vlanId)
{
  filterInfoData_t* filterEntry;
  L7_uchar8 addr[L7_MAC_ADDR_LEN];
  L7_ushort16 vid;
  L7_RC_t rc;

  vid = 1;
  memset((void *)addr, 0x00, L7_MAC_ADDR_LEN);

  rc = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  filterEntry = filterEntryFind(addr,1,L7_MATCH_GETNEXT);

  if (filterEntry)
  {
    /*first 2 bytes are vlan id*/
    memcpy((void *)&vid, (void *)&filterEntry->vlanidmacAddr[0], 2);
    *vlanId = (L7_uint32)vid;
    /*next six bytes are mac address*/
    memcpy((void *)macAddr, (void *)&filterEntry->vlanidmacAddr[2], L7_MAC_ADDR_LEN);
    rc = osapiSemaGive(filterAvlTree.semId);
    return L7_SUCCESS;
  }
  else
  {
    rc = osapiSemaGive(filterAvlTree.semId);
    memset((void *)macAddr, 0x00, L7_MAC_ADDR_LEN);
    *vlanId = 0;
    return L7_FAILURE;
  }
}

/*********************************************************************
*
* @purpose  Gets the next configured MAC static Filter 
*
* @param    L7_uchar8*  macAddr     @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId      @b{(input)} Vlan ID for the filter
* @param    L7_uchar8*  nextAddr    @b((output) MAC Address of the next filter
* @param    L7_uint32*  nextVlanId  @b{(input)} Vlan ID for the next filter
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there are no further MAC Filters
*
* @notes    nextAddr is returned as L7_NULL if there is no further MAC filters
*
* @end
*********************************************************************/
L7_RC_t filterNextGet(L7_uchar8* macAddr,
                      L7_uint32 vlanId,
                      L7_uchar8* nextAddr,
                      L7_uint32* nextVlanId)
{
  filterInfoData_t* nextFilterEntry;
  L7_ushort16 nextVid = 0;
  L7_RC_t rc;

  rc = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  nextFilterEntry = filterEntryFind(macAddr,vlanId,L7_MATCH_GETNEXT);
  if (nextFilterEntry == L7_NULLPTR)
  {
    rc = osapiSemaGive(filterAvlTree.semId);
    memset((void *)nextAddr,0,L7_MAC_ADDR_LEN);
    *nextVlanId = 0;
    return L7_FAILURE;
  }
  /*get the next entries values*/
  /*next Entry is valid*/
  /*first 2 bytes are vlan id*/
  memcpy((void *)&nextVid, (void *)&nextFilterEntry->vlanidmacAddr[0], 2);
  *nextVlanId = (L7_uint32)nextVid;
  /*next six bytes are mac address*/
  memcpy((void *)nextAddr, (void *)&nextFilterEntry->vlanidmacAddr[2], L7_MAC_ADDR_LEN);
  rc = osapiSemaGive(filterAvlTree.semId);
  return L7_SUCCESS; 


}



/*********************************************************************
*
* @purpose  Gets list source interfaces for a MAC static Filter 
*
* @param    L7_uchar8*  macAddr      @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId       @b{(input)}  Vlan ID for the Filter
* @param    L7_uint32*  numSrcIntf   @b((output)) Number of source interfaces
* @param    L7_uint32*  srcIntfList  @b((output)) Densely packed array of source interfaces                           
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    numSrcIntf will be returned as zero if there is no src port for the MAC Filter
*           Only the first numSrcIntf entries in the srcIntfList have valid 
*           interfaces, rest are garbage
*
* @end
*********************************************************************/
L7_RC_t filterSrcIntfListGet(L7_uchar8* macaddr,
                             L7_uint32 vlanId,
                             L7_uint32* numSrcIntf,
                             L7_uint32 srcIntfList[])
{
  filterInfoData_t* filterEntry;
  L7_RC_t rc;

  rc = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  filterEntry = filterEntryFind(macaddr,vlanId,L7_MATCH_EXACT);
  if (filterEntry == L7_NULLPTR)
  {
    rc = osapiSemaGive(filterAvlTree.semId);
    return L7_FAILURE;
  }
  filterIntfListGet(&filterEntry->srcIntfMask, L7_FALSE, numSrcIntf, srcIntfList);

  rc = osapiSemaGive(filterAvlTree.semId);
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets list destination interfaces for a MAC static Filter 
*
* @param    L7_uchar8*  macAddr      @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId       @b{(input)}  Vlan ID for the Filter
* @param    L7_uint32*  numDstIntf   @b((output)) Number of destination interfaces
* @param    L7_uint32*  dstIntfList  @b((output)) Densely packed array of destination interfaces                           
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    numDstIntf will be returned as zero if there is no dst port for the MAC Filter
*           Only the first numDstIntf entries in the dstIntfList have valid 
*           interfaces, rest are garbage
*
* @end
*********************************************************************/
L7_RC_t filterDstIntfListGet(L7_uchar8* macaddr,
                             L7_uint32 vlanId,
                             L7_uint32* numDstIntf,
                             L7_uint32 dstIntfList[])
{
  filterInfoData_t* filterEntry;
  L7_RC_t rc;

  rc = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  filterEntry = filterEntryFind(macaddr,vlanId,L7_MATCH_EXACT);
  if (filterEntry == L7_NULLPTR)
  {
    rc = osapiSemaGive(filterAvlTree.semId);
    return L7_FAILURE;
  }

  filterIntfListGet(&filterEntry->dstIntfMask, L7_FALSE, numDstIntf, dstIntfList);

  rc = osapiSemaGive(filterAvlTree.semId);
  return L7_SUCCESS;


}

/*********************************************************************
*
* @purpose  Gets list destination filter interfaces  for a MAC static Filter 
*
* @param    L7_uchar8*  macAddr      @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId       @b{(input)}  Vlan ID for the Filter
* @param    L7_uint32*  numDstIntf   @b((output)) Number of destination interfaces
* @param    L7_uint32*  dstIntfList  @b((output)) Densely packed array of destination interfaces                           
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    numDstIntf will be returned as zero if there is no dst port for the MAC Filter
*           Only the first numDstIntf entries in the dstIntfList have valid 
*           interfaces, rest are garbage
*
* @end
*********************************************************************/
L7_RC_t filterDstFiltIntfListGet(L7_uchar8* macaddr,
                             L7_uint32 vlanId,
                             L7_uint32* numFiltIntf,
                             L7_uint32 dstFiltIntfList[])
{
  filterInfoData_t* filterEntry;
  L7_RC_t rc;

  rc = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  filterEntry = filterEntryFind(macaddr,vlanId,L7_MATCH_EXACT);
  if (filterEntry == L7_NULLPTR)
  {
    rc = osapiSemaGive(filterAvlTree.semId);
    return L7_FAILURE;
  }

  filterIntfListGet(&filterEntry->dstIntfFilterMask, L7_FALSE, numFiltIntf, dstFiltIntfList);

  rc = osapiSemaGive(filterAvlTree.semId);
  return L7_SUCCESS;


}

/*********************************************************************
*
* @purpose  Is a particular MAC Filter configured 
*
* @param    L7_uchar8*  macAddr  @b((input)) MAC Address of the filter
* @param    L7_uint32   vlanId   @b{(input)} Vlan ID for the Filter
*
* @returns  L7_TRUE, if a MAC filter exists
* @returns  L7_FALSE, if there is no MAC Filter
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL filterIsConfigured(L7_uchar8* macAddr, L7_uint32 vlanId)
{
  filterInfoData_t* filterEntry;
  L7_RC_t rc;

  rc = osapiSemaTake(filterAvlTree.semId,L7_WAIT_FOREVER);
  filterEntry = filterEntryFind(macAddr,vlanId,L7_MATCH_EXACT);
  rc = osapiSemaGive(filterAvlTree.semId);
  if (filterEntry == L7_NULLPTR)
    return L7_FALSE;
  else
    return L7_TRUE;

}
/*********************************************************************
*
* @purpose  Checks if a particular interface is present in any MAC filter 
*
* @param    L7_uint32  intIfNum  @b((input)) Internal Interface Number
*
* @returns  L7_TRUE, if the interface is in any filter
* @returns  L7_FALSE, if the interface is not associated with any MAC filter
*
* @notes    An interface is associated with a MAC filter if it is in the filter's
*           source list or destination list. If an interface is found on any list
*           of any configured MAC filter then this routine will return a L7_TRUE
*           Only if the interface is NOT found on ANY list of ANY filter, a 
*           L7_FALSE is returned
*           This routine also return L7_FALSE when this routine is called 
*           before the filterSysInit has fully executed. That is the avl tree has
*           not yet been created. It is safe to assume that who ever quries the filter 
*           membership does so because the interface is a part of the calling application.
*           By design anything that is part of a filter cannot be a part of that 
*           calling application and vice versa. Hence if it is already a part of 
*           calling application then it is safe to say that it is not a part of 
*           filter and it return a L7_FALSE  
*           
*
* @end
*********************************************************************/
L7_BOOL filterIsIntfInAnyFilter(L7_uint32 intIfNum)
{
  L7_RC_t rc;
  L7_BOOL rcBool = L7_FALSE;
  L7_BOOL rcBool1 = L7_FALSE;
  L7_uchar8 macAddr[L7_MAC_ADDR_LEN];
  L7_uint32 vlanId;
  L7_uint32 srcList[L7_FILTER_MAX_INTF];
  L7_uint32 numSrc;
  L7_uint32 dstList[L7_FILTER_MAX_INTF];
  L7_uint32 numDst;
  L7_uint32 index;

  memset((void *)macAddr,0x00,L7_MAC_ADDR_LEN);
  /*filter has not yet initialized return a L7_FALSE*/
  if (filterAvlTree.semId == L7_NULL)
    return L7_FALSE;
  rc = filterFirstGet(macAddr,&vlanId);
  while (rc == L7_SUCCESS)
  {
    rc = filterSrcIntfListGet(macAddr,vlanId,&numSrc,srcList);
    rcBool1 = filterIsIntfPresent(srcList,numSrc,intIfNum,&index);
    if (rcBool1 == L7_TRUE)
    {
      rcBool = L7_TRUE;
      break;
    }
    rc = filterDstIntfListGet(macAddr,vlanId,&numDst,dstList);
    rcBool1 = filterIsIntfPresent(dstList,numDst,intIfNum,&index);
    if (rcBool1 == L7_TRUE)
    {
      rcBool = L7_TRUE;
      break;
    }

    rc = filterNextGet(macAddr,vlanId,macAddr,&vlanId);

  }

  return rcBool;

}

/*********************************************************************
*
* @purpose  To check the mac address type, unicast or multicast.
*
* @param    L7_uchar8  *macAddr   @b((input)) MAC Address
*
* @returns  L7_TRUE, if MAC address is of type multicast
* @returns  L7_FALSE, if MAC address is of type unicast
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL filterCheckMacAddrTypeMulticast(L7_uchar8 *macAddr)
{
  return filterIsMacAddrTypeMulticast(macAddr);
}

/*********************************************************************
* @purpose  Set multicast address filtering configuration.
*
* @param    L7_uint32           vlanId  @{{intput}} VLAN
* @param    L7_MFDB_VLAN_FILTER_MODE_t mode    @{{intput}}  MFDB_FORWARD_ALL,
*                                                    MFDB_FORWARD_UNREGISTERED,
*                                                    MFDB_FILTER_UNREGISTERED.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE, vlan is not configurable
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t filterVlanFilteringModeSet(L7_uint32 vlanId, L7_FILTER_VLAN_FILTER_MODE_t mode)
{
  filterVlanCfgData_t *vlanCfg = L7_NULLPTR;


  if ((vlanId <= 0) ||
      (vlanId > platVlanVlanIdMaxGet()))
  {
    return L7_FAILURE;
  }

  if ((mode != L7_FILTER_FORWARD_ALL) &&
      (mode != L7_FILTER_FORWARD_UNREGISTERED) &&
      (mode != L7_FILTER_FILTER_UNREGISTERED))
  {
    return L7_FAILURE;
  }

  /*osapiSemaTake(mfdbSemaphore, L7_WAIT_FOREVER);*/
  if (filterVlanCfgEntryGet(vlanId, &vlanCfg) != L7_SUCCESS)
  {
    /*osapiSemaGive(mfdbSemaphore);*/
    return L7_FAILURE;
  }
  if (vlanCfg->mode == mode)
  {
    /*osapiSemaGive(mfdbSemaphore); */
    return L7_SUCCESS;
  }
  if (filterCfgData->filteringEnabled == L7_TRUE)
  {
    if (dtlL2McastVlanFloodModeSet(vlanId, mode) != L7_SUCCESS)
    {
      /*osapiSemaGive(mfdbSemaphore);*/
      return L7_FAILURE;
    }
  }
  vlanCfg->mode = mode;
  filterCfgData->cfgHdr.dataChanged = L7_TRUE;
  /*osapiSemaGive(mfdbSemaphore); */
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Get multicast address filtering configuration.
*
* @param    L7_uint32           vlanId  @{{intput}} VLAN
* @param    L7_MFDB_VLAN_FILTER_MODE_t *mode   @{{output}}  MFDB_FORWARD_ALL,
*                                                    MFDB_FORWARD_UNREGISTERED,
*                                                    MFDB_FILTER_UNREGISTERED.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t filterVlanFilteringModeGet(L7_uint32 vlanId, L7_FILTER_VLAN_FILTER_MODE_t *mode)
{
  filterVlanCfgData_t *vlanCfg = L7_NULLPTR;

  if ((vlanId <= 0) ||
      (vlanId > platVlanVlanIdMaxGet()))
  {
    return L7_FAILURE;
  }

  /*osapiSemaTake(mfdbSemaphore, L7_WAIT_FOREVER);*/
  if (filterVlanCfgEntryGet(vlanId, &vlanCfg) != L7_SUCCESS)
  {
    /*osapiSemaGive(mfdbSemaphore);*/
    return L7_FAILURE;
  }


  *mode = vlanCfg->mode;
  /*osapiSemaGive(mfdbSemaphore); */
  return L7_SUCCESS;
}

/************************************************************************/
/*                        PRIVATE ROUTINES                              */
/************************************************************************/

/*********************************************************************
*
* @purpose  Is a particular interface present in the list of interfaces 
*
* @param    L7_uint32   list[]  @b((input))  List of interface numbers
* @param    L7_uint32   listSize@b{(input)}  Number of valid entries in the list
* @param    L7_uint32   intf    @b{(input)}  The interface number to search for
* @param    L7_uint32*  index   @b{(output)} The index into the List provided if interface found
*
* @returns  L7_TRUE, if the interface exists in the list
* @returns  L7_FALSE, if the interface in not in the list
*
* @notes    
*
* @end
*********************************************************************/
L7_BOOL filterIsIntfPresent(L7_uint32 list[], 
                            L7_uint32 listSize,
                            L7_uint32 intf, 
                            L7_uint32* index)
{
  L7_uint32 i;
  for (i=0;i<listSize;i++)
  {
    if (list[i] == intf)
    {
      *index = i;
      return L7_TRUE;
    }
  }
  return L7_FALSE;
}

/*********************************************************************
*
* @purpose  Checks if the MAC Address is not within a set restricted MAC Addresses 
*
* @param    L7_uchar8*  macAddr      @b((input))  MAC Address of the filter
*
* @returns  L7_SUCCESS, if mac address in not restricted
* @returns  L7_FAILURE, if mac address is one of the restricted addresses
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t filterMacAddrCheck(L7_uchar8* macAddr)
{
  L7_uint32 r;

  for (r=0;r<FILTER_MAX_RESTRICTED_ADDR;r++)
  {
    if (memcmp((void *)macAddr,(void *)&restrictedMacAddr[r][0],L7_MAC_ADDR_LEN)==0)
      return L7_FAILURE;
  }
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  To check the mac address type, unicast or multicast.
*
* @param    L7_uchar8  *macAddr   @b((input)) MAC Address
*
* @returns  L7_TRUE, if MAC address is of type multicast
* @returns  L7_FALSE, if MAC address is of type unicast
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL filterIsMacAddrTypeMulticast(L7_uchar8 *macAddr)
{
  if ((macAddr[0] & 0x01) != 0x01)
    return L7_FALSE;

  return L7_TRUE;
} 

/*********************************************************************
*
* @purpose  Extracts the vlan Id and Mac address from the 8 byte combination
*
* @param    L7_uchar8*  combined  @b{(input)}  Combined MAC and vlanId 
* @param    L7_uchar8*  macAddr   @b((output)) MAC Address of the filter
* @param    L7_uint32*  vlanId    @b{(output)} Vlan ID for the Filter
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    First 2 bytes are the vlanId and the next 6 bytes the MAC address
*
* @end
*********************************************************************/
L7_RC_t filterMacVlanIdExtract(L7_uchar8* combined, L7_uchar8* macAddr, L7_uint32* vlanId)
{
  L7_ushort16 vid;
  /*tbd are we writing into the right two bytes of the vlanId????*/
  /*first 2 bytes are vlan id*/
  memcpy((void *)&vid, (void *)combined, 2);
  *vlanId = (L7_uint32)vid;
  /*next six bytes are mac address*/
  memcpy((void *)macAddr, (void *)&combined[2], L7_MAC_ADDR_LEN);
  return L7_SUCCESS;

}

/*********************************************************************
* @purpose  Check if the interface is valid for filtering
*
* @param 	intIfNum			Internal interface number
*
* @returns L7_TRUE if valid
* @returns L7_FALSE otherwise
*
* @end
*
*********************************************************************/
L7_BOOL filterIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t intfType;

  if (nimGetIntfType(intIfNum, &intfType) != L7_SUCCESS)
  {
    return L7_FALSE;
  }

  return filterIsValidIntfType(intfType);
}

/*********************************************************************
*
* @purpose  Checks if the Interface type is valid for filters 
*
* @param    L7_uint32 intfType  @b((input))  Interface Type
*
* @returns  L7_TRUE, if interface type is acceptable
* @returns  L7_FALSE, if interface type is not acceptable
*
* @end
*********************************************************************/
L7_BOOL filterIsValidIntfType(L7_INTF_TYPES_t intfType)
{
  if (intfType != L7_PHYSICAL_INTF)
    if (intfType != L7_LAG_INTF)
      return L7_FALSE;
  return L7_TRUE;
}

/*********************************************************************
*
* @purpose  Checks if the Interface is valid for filters 
*
* @param    L7_uint32 intIfNum  @b((input))  Internal Interface Number
*
* @returns  L7_SUCCESS, if interface is acceptable
* @returns  L7_FAILURE, if interface is not acceptable
*
* @notes    Acceptability criteria
*           1. Must be a valid configured interface number &&
*           2. Must be a physical or LAG interface         &&
*           3. Must not be a LAG member
*           
*
* @end
*********************************************************************/
L7_RC_t filterIntfCheck(L7_uint32 intIfNum)
{
  L7_RC_t rc;
  L7_INTF_TYPES_t intfType;
  L7_BOOL rcBool;

  /*check for valid interface number*/
  rc = nimCheckIfNumber(intIfNum);
  if (L7_ERROR==rc)
    return L7_FAILURE;

  /*check for physical interface or LAG interface*/
  rc = nimGetIntfType(intIfNum,&intfType);
  if (rc==L7_SUCCESS)
  {
    if (intfType != L7_PHYSICAL_INTF)
      if (intfType != L7_LAG_INTF)
        return L7_FAILURE;
  }
  else
    /*case when rc is returned as a failure*/
    /*hence not able to determine the type of interface*/
    return L7_FAILURE;

  /*check for lag membership*/
  rcBool = dot3adIsLagMember(intIfNum);

  if (rcBool == L7_TRUE)
    return L7_FAILURE;

  /*If it has not yet returned with failure then it must be good*/
  /*We can now say that this interface is acceptable for filtering*/
  return L7_SUCCESS;

}


