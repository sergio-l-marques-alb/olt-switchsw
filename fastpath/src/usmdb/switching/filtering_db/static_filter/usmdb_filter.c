/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename     src\application\unitmgr\usmdb\usmdb_filter.c
*
* @purpose      Provide interface to hardware API's for unitmgr components
*
* @component    Usmdb
*
* @comments 
*
* @create       1/7/2002
*
* @author       skalyanam
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_common.h"
#include "usmdb_util_api.h"
#include "nimapi.h"

#include "usmdb_filter_api.h"
#include "filter_api.h"
#include <string.h>

/*********************************************************************
* @purpose  Get multicast address filtering configuration.
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
L7_RC_t usmDbFilterAdminModeSet(L7_uint32 UnitIndex, L7_BOOL enable)
{
    return filterAdminModeSet(enable);
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
L7_RC_t usmDbFilterAdminModeGet(L7_uint32 UnitIndex, L7_BOOL *enable)
{
    return filterAdminModeGet(enable);
}

/*********************************************************************
*
* @purpose  Create a MAC Static Filter
*
* @param    L7_uint32   UnitIndex @b((input)) The unit for this operation
* @param    L7_uchar8*  macAddr   @b((input)) Staic MAC Address for the filter  
* @param    L7_uint32   vlanId    @b{(input)} Vlan ID for the filter
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterCreate(L7_uint32 UnitIndex, 
                          L7_uchar8 *macAddr,
                          L7_uint32 vlanId)
{
  return filterCreate(macAddr,vlanId);
}
/*********************************************************************
*
* @purpose  Apply a source interface mask to the static MAC Filter
*
* @param    L7_uchar8*  macAddr     @b((input)) MAC Address of the Filter
* @param    L7_uint32   vlanId      @b{(input)} Vlan ID for the Filter
* @param    L7_uint32   srcIntfMask @b{(input)} List of interfaces specifying
*                                               source ports
*
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
* @returns  L7_TABLE_IS_FULL Configuration failed due to resource constraints
* @returns  L7_NOT_SUPPORTED Feature not supported
*
* @notes                A 1 in srcIntfMask signifies addition of interface
*                       as a source interface. 0 signifies deletion of interface
*                       from the source interface list. Calling function should
*                       ensure that srcIntfMask contains the final list of
*                       intended interfaces.
*
*                       The design philosophy now is to let mfdb tell dtl/driver about
*                       all dst interfaces related to multicast addresses irrespective whether it is
*                       static (filter) or dynamic (IGMP Snoop or GMRP) in origination
*                       The filter component will continue to tell dtl/driver about
*                       static multicast src interfaces, unicast src and unicast dst interfaces
*
*
* @end
*********************************************************************/
L7_RC_t usmdbFilterSrcIntfMaskModify(L7_uchar8* macAddr, L7_uint32 vlanId,
                                     L7_INTF_MASK_t *srcIntfMask)
{
  if (usmDbFeaturePresentCheck(L7_NULL, L7_FILTER_COMPONENT_ID, L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return filterSrcIntfMaskModify(macAddr, vlanId, srcIntfMask);
}
/*********************************************************************
*
* @purpose  Adds a source interface to the static MAC Filter 
*
* @param    L7_uint32  UnitIndex  @b((input)) The unit for this operation
* @param    L7_uchar8* macAddr    @b((input)) MAC Address of the Filter
* @param    L7_uint32  vlanId     @b{(input)} Vlan ID for the filter
* @param    L7_uint32  intIfNum   @b{(input)} Internal Interface number of the source port  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbFilterSrcIntfAdd(L7_uint32 UnitIndex, 
                              L7_uchar8* macAddr,
                              L7_uint32 vlanId, 
                              L7_uint32 intIfNum)
{
  L7_RC_t rc;

  if (usmDbFeaturePresentCheck(UnitIndex, L7_FILTER_COMPONENT_ID, L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;
  
  rc = usmDbFilterPortParmCanSet(UnitIndex, intIfNum);
  if (rc != L7_SUCCESS)
	return rc;
  return filterSrcIntfAdd(macAddr,vlanId,intIfNum, L7_TRUE);
}
/*********************************************************************
*
* @purpose  Deletes a source port from the source port list of the MAC Filter 
*
* @param    L7_uint32   UnitIndex @b((input))  The unit for this operation
* @param    L7_uchar8*  macAddr   @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId    @b{(input)}  Vlan ID for the filter
* @param    L7_uint32   intIfNum  @b{(input)}  Internal Interface number of the source port  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbFilterSrcIntfDelete(L7_uint32 UnitIndex,
                                 L7_uchar8* macAddr,
                                 L7_uint32 vlanId,
                                 L7_uint32 intIfNum)
{
  L7_RC_t rc;

  if (usmDbFeaturePresentCheck(UnitIndex, L7_FILTER_COMPONENT_ID, L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;
  
  rc = usmDbFilterPortParmCanSet(UnitIndex, intIfNum);
  if (rc != L7_SUCCESS)
	return rc;
  return filterSrcIntfDelete(macAddr,vlanId,intIfNum); 

}
/*********************************************************************
*
* @purpose  Adds a destination interface to the static MAC Filter 
*
* @param    L7_uint32  UnitIndex  @b((input)) The unit for this operation
* @param    L7_uchar8* macAddr    @b((input)) MAC Address of the Filter
* @param    L7_uint32  vlanId     @b{(input)} Vlan ID for the filter
* @param    L7_uint32  intIfNum   @b{(input)} Internal Interface number of the destination port  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_IMPLEMENTED_YET , if feature is not supported on current platform
* @returns  L7_NOT_SUPPORTED, if configuration is not supported on specified interface
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbFilterDstIntfAdd(L7_uint32 UnitIndex, 
                              L7_uchar8* macAddr,
                              L7_uint32 vlanId, 
                              L7_uint32 intIfNum)
{
  L7_RC_t rc;
  L7_BOOL entryMulticast;

  /* check if destination port configuration is supported for unicast MAC addresses*/
  /* Destination port configuration is supoorted for multicast MAC addresses for all platforms*/
  entryMulticast=L7_FALSE;
  entryMulticast = filterCheckMacAddrTypeMulticast(macAddr);

  if ((entryMulticast == L7_FALSE) &&
      (usmDbFeaturePresentCheck(UnitIndex, L7_FILTER_COMPONENT_ID, L7_FILTER_UCAST_DEST_PORT_FILTERING_FEATURE_ID) != L7_TRUE))
    return L7_NOT_IMPLEMENTED_YET;

  rc = usmDbFilterPortParmCanSet(UnitIndex, intIfNum);
  if (rc != L7_SUCCESS)
	return rc;
  return filterDstIntfAdd(macAddr,vlanId,intIfNum);
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
*       
* @end
*
* @end
*********************************************************************/
L7_RC_t usmdbFilterDstIntfMaskAdd(L7_uint32 UnitIndex, 
                              L7_uint32 vlanId, 
                              L7_enetMacAddr_t macAddr,
                              L7_INTF_MASK_t fwdMask,
                              L7_INTF_MASK_t filterMask)
{

    return filterDstIntfMaskAdd(vlanId, macAddr, fwdMask, filterMask);
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
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE
*
* @notes  
*       
* @end
*
* @end
*********************************************************************/
L7_RC_t usmdbFilterDstIntfMaskDelete(L7_uint32 UnitIndex, 
                              L7_uint32 vlanId, 
                              L7_enetMacAddr_t macAddr,
                              L7_INTF_MASK_t fwdMask,
                              L7_INTF_MASK_t filterMask)
{

    return filterDstIntfMaskDelete(vlanId, macAddr, fwdMask, filterMask);
}

/*********************************************************************
*
* @purpose  Deletes a destination port from the destination port list of the MAC Filter 
*
* @param    L7_uint32   UnitIndex @b((input))  The unit for this operation
* @param    L7_uchar8*  macAddr   @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId    @b{(input)}  Vlan ID for the filter
* @param    L7_uint32   intIfNum  @b{(input)}  Internal Interface number of the destination port  
*                            
*
* @returns  L7_SUCCESS, if success
* @returns  L7_NOT_IMPLEMENTED_YET , if feature is not supported on current platform
* @returns  L7_NOT_SUPPORTED, if configuration is not supported on specified interface
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmdbFilterDstIntfDelete(L7_uint32 UnitIndex,
                                 L7_uchar8* macAddr,
                                 L7_uint32 vlanId,
                                 L7_uint32 intIfNum)
{  
  L7_RC_t rc;

  L7_BOOL entryMulticast;

  /* check if destination port configuration is supported for unicast MAC addresses*/
  /* Destination port configuration is supoorted for multicast MAC addresses for all platforms*/
  entryMulticast=L7_FALSE;
  entryMulticast = filterCheckMacAddrTypeMulticast(macAddr);

  if ((entryMulticast == L7_FALSE) &&
      (usmDbFeaturePresentCheck(UnitIndex, L7_FILTER_COMPONENT_ID, L7_FILTER_UCAST_DEST_PORT_FILTERING_FEATURE_ID) != L7_TRUE))
    return L7_NOT_IMPLEMENTED_YET;

 
  rc = usmDbFilterPortParmCanSet(UnitIndex, intIfNum);
  if (rc != L7_SUCCESS)
	return rc;
  return filterDstIntfDelete(macAddr,vlanId,intIfNum);
}
/*********************************************************************
*
* @purpose  Removes a configured MAC static Filter 
*
* @param    L7_uint32   UnitIndex @b((input))  The unit for this operation
* @param    L7_uchar8*  macAddr   @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId    @b{(input)}  Vlan ID for the filter
*                            
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if other failure
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usmDbFilterRemove(L7_uint32 UnitIndex,
                            L7_uchar8* macAddr,
                            L7_uint32 vlanId)
{
  return filterRemove(macAddr,vlanId);
}

/*********************************************************************
*
* @purpose  Gets the first configured MAC static Filter 
*
* @param    L7_uint32   UnitIndex  @b((input))  The unit for this operation
* @param    L7_uchar8*  macAddr    @b((output))  MAC Address of the filter
* @param    L7_uint32*  vlanId     @b{(output)}  Vlan ID for the filter
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    macAddr is returned as L7_NULL if there are no configured MAC Filters
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterFirstGet(L7_uint32 UnitIndex,
                            L7_uchar8* macAddr,
                            L7_uint32* vlanId)
{
  return filterFirstGet(macAddr,vlanId);
}

/*********************************************************************
*
* @purpose  Gets the next configured MAC static Filter 
*
* @param    L7_uint32   UnitIndex  @b((input))  The unit for this operation
* @param    L7_uchar8*  macAddr    @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId     @b{(input)}  Vlan ID for the filter
* @param    L7_uchar8*  nextAddr   @b((output)} MAC Address of the next filter
* @param    L7_uint32*  nextVlanId @b{(output)} Vlan ID for the next filter
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there are no further MAC Filters
*
* @notes    nextAddr is returned as L7_NULL if there is no further MAC filters
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterNextGet(L7_uint32 UnitIndex,
                           L7_uchar8* macAddr,
                           L7_uint32 vlanId,
                           L7_uchar8* nextAddr,
                           L7_uint32* nextVlanId)
{
  return filterNextGet(macAddr,vlanId,nextAddr,nextVlanId);
}
/*********************************************************************
*
* @purpose  Gets list source interfaces for a MAC static Filter 
*
* @param    L7_uint32   UnitIndex    @b((input))  The unit for this operation
* @param    L7_uchar8*  macAddr      @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId       @b{(input)}  Vlan ID for the filter
* @param    L7_uint32*  numSrcIntf   @b((output)) Number of source interfaces
* @param    L7_uint32*  srcIntfList  @b((output)) Densely packed array of source interfaces                           
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    numSrcIntf will be returned as zero if there is no src port for the MAC Filter
* @notes    srcIntfList must be of size L7_FILTER_MAX_INTF
*           Only the first numSrcIntf entries in the srcIntfList have valid 
*           interfaces, rest are garbage
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterSrcIntfListGet(L7_uint32 UnitIndex,
                                  L7_uchar8* macaddr,
                                  L7_uint32 vlanId,
                                  L7_uint32* numSrcIntf,
                                  L7_uint32 srcIntfList[])
{
  if (usmDbFeaturePresentCheck(UnitIndex, L7_FILTER_COMPONENT_ID, L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  return filterSrcIntfListGet(macaddr,vlanId,numSrcIntf,srcIntfList);

}

/*********************************************************************
*
* @purpose  Gets list destination interfaces for a MAC static Filter 
*
* @param    L7_uint32   UnitIndex    @b((input))  The unit for this operation
* @param    L7_uchar8*  macAddr      @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId       @b{(input)}  Vlan ID for the filter
* @param    L7_uint32*  numDstIntf   @b((output)) Number of destination interfaces
* @param    L7_uint32*  dstIntfList  @b((output)) Densely packed array of destination interfaces                           
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    numDstIntf will be returned as zero if there is no dst port for the MAC Filter
* @notes    dstIntfList must be of size L7_FILTER_MAX_INTF
*           Only the first numDstIntf entries in the dstIntfList have valid 
*           interfaces, rest are garbage
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterDstIntfListGet(L7_uint32 UnitIndex,
                                  L7_uchar8* macaddr,
                                  L7_uint32 vlanId,
                                  L7_uint32* numDstIntf,
                                  L7_uint32 dstIntfList[])
{

    L7_BOOL entryMulticast;

    /* check if destination port configuration is supported for unicast MAC addresses*/
    /* Destination port configuration is supoorted for multicast MAC addresses for all platforms*/
    entryMulticast=L7_FALSE;
    entryMulticast = filterCheckMacAddrTypeMulticast(macaddr);

    if ((entryMulticast == L7_FALSE) &&
        (usmDbFeaturePresentCheck(UnitIndex, L7_FILTER_COMPONENT_ID, L7_FILTER_UCAST_DEST_PORT_FILTERING_FEATURE_ID) != L7_TRUE))
      {
        *numDstIntf = 0;
        return L7_NOT_SUPPORTED;
      }
   
    return filterDstIntfListGet(macaddr,vlanId,numDstIntf,dstIntfList);

}

/*********************************************************************
*
* @purpose  Gets list of destination filter interfaces for a MAC static Filter 
*
* @param    L7_uint32   UnitIndex    @b((input))  The unit for this operation
* @param    L7_uchar8*  macAddr      @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId       @b{(input)}  Vlan ID for the filter
* @param    L7_uint32*  numDstIntf   @b((output)) Number of destination interfaces
* @param    L7_uint32*  dstIntfList  @b((output)) Densely packed array of destination interfaces                           
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    numDstIntf will be returned as zero if there is no dst port for the MAC Filter
* @notes    dstIntfList must be of size L7_FILTER_MAX_INTF
*           Only the first numDstIntf entries in the dstIntfList have valid 
*           interfaces, rest are garbage
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterDstFiltIntfListGet(L7_uint32 UnitIndex,
                                  L7_uchar8* macaddr,
                                  L7_uint32 vlanId,
                                  L7_uint32* numFiltIntf,
                                  L7_uint32 dstFiltIntfList[])
{

    L7_BOOL entryMulticast;

    /* check if destination port configuration is supported for unicast MAC addresses*/
    /* Destination port configuration is supoorted for multicast MAC addresses for all platforms*/
    entryMulticast=L7_FALSE;
    entryMulticast = filterCheckMacAddrTypeMulticast(macaddr);

    if (entryMulticast == L7_FALSE)
      {
        *numFiltIntf = 0;
        return L7_NOT_SUPPORTED;
      }
   
    return filterDstFiltIntfListGet(macaddr,vlanId,numFiltIntf,dstFiltIntfList);

}

/*********************************************************************
*
* @purpose  Gets the number of MAC static Filters configured 
*
* @param    L7_uint32  UnitIndex  @b((input))  The unit for this operation
* @param    L7_uint32* count      @b((output)) NumberOf Filters configured
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmdbFilterCountGet(L7_uint32 UnitIndex,
                            L7_uint32* count)
{

  return filterCountGet(count);
}

/*********************************************************************
*
* @purpose  Sets src interface in the MAC filter  
*
* @param    L7_uint32   UnitIndex  @b((input))  The unit for this operation
* @param    L7_uchar8*  macAddr    @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId     @b{(input)}  Vlan ID for the filter
* @param    L7_uchar8*  maskOctets @b{(input)}  Bit Mask of the interfaces
*                                               1 or Set indicates include this intf in src
*                                               0 indicates remove this interface if it was 
*                                               previously there
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    This routine is as SNMP requires. Bits are interpreted in the 
*           following manner. MSB MSB-1 .......LSB+1 LSB, left to right. 
*           MSB will represent Internal interface 1, MSB-1 will represent 
*           Internal Interface Number 2 and so on.
*           The size of this mask buffer will be FD_CNFGR_NIM_MAX_LAG_INTF_NUM bits long
*           (This is to accomodate all the physical interface plus the LAG interfaces
*           Assuming physical interface numbers will be less that those assigned to LAGS)
*           Interms of Bytes it is FD_CNFGR_NIM_MAX_LAG_INTF_NUM % 8 +1
*           Internally we reverse this lsb to msb and vice versa.
* @notes    NOTE: this mask represents the src interface that should be configured, all zeros
*           means remove all configured interfaces. similarly any changes will be
*           reflected. i.e. in the final standing only those interfaces will be 
*           configured which are set, others will be removed
*           
*           
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterSrcIntfMaskSet(L7_uint32 UnitIndex,
                                  L7_uchar8* macAddr,
                                  L7_uint32 vlanId,
                                  L7_uchar8* maskOctets)
{
  /*take care of the fact that this mask may include addition
    and deletion of src interfaces. So need to compare this mask with
    the old list of src interfaces*/
  FILTER_INTF_MASK_t* mask;
  FILTER_INTF_MASK_t oldMask;
  FILTER_INTF_MASK_t newMask;
  FILTER_INTF_MASK_t delMask;
  FILTER_INTF_MASK_t addMask;

  L7_uint32 oldList[L7_FILTER_MAX_INTF];
  L7_uint32 oldNum;
  L7_uint32 newList[L7_FILTER_MAX_INTF];
  L7_uint32 newNum;
  L7_uint32 addList[L7_FILTER_MAX_INTF];
  L7_uint32 addNum;
  L7_uint32 delList[L7_FILTER_MAX_INTF];
  L7_uint32 delNum;

  L7_RC_t rc;
  L7_uint32 i;

  if (usmDbFeaturePresentCheck(UnitIndex, L7_FILTER_COMPONENT_ID, L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  mask = (FILTER_INTF_MASK_t *)maskOctets;

  /*reverse the mask*/
  rc = filterReverseMask(*mask,&newMask);
  /*generate the new src list*/
  rc = filterMaskToList(&newMask,newList,&newNum);
  /*get the old list of src interfaces*/
  rc = filterSrcIntfListGet(macAddr,vlanId,&oldNum,oldList);
  if (rc==L7_FAILURE)
    return L7_FAILURE;
  /*generate old mask*/
  rc = filterListToMask(oldList,oldNum,&oldMask);
  /*calculate 1s to 0s i.e. those bits that were set in old but now deleted*/
  /*algorithm is all bitwise operation
    old exor new -> result
    result and old -> a mask of 1s to 0s w.r.t. old mask
    i.e.mask contains list of src interface to be deleted*/
  /*copy newmask into delmask*/
  FILTER_INTF_MASKEQ(delMask,newMask);
  /*do the exor with oldMask  and write back into delMask*/
  FILTER_INTF_MASKEXOREQ(delMask,oldMask);
  /*do the and with oldMask and write back to delMask*/
  FILTER_INTF_MASKANDEQ(delMask,oldMask);
  /*convert the mask into the delete list*/
  filterMaskToList(&delMask,delList,&delNum);
  /*apply this delete list to the filter database*/
  for (i=0;i<delNum;i++)
  {
	rc = usmDbFilterPortParmCanSet(UnitIndex, delList[i]);
	if (rc != L7_SUCCESS)
	  return L7_FAILURE;
    rc = filterSrcIntfDelete(macAddr,vlanId,delList[i]);
    if (rc==L7_FAILURE)
      return rc;
  }


  /*calculate 0s to 1s i.e. those bits that were set to 0 in old but now set or added*/
  /*algorithm is all bitwise operation 
    old exor new -> result
    result and new -> a mask of 0s to 1s w.r.t. old mask
    i.e. mask contains list of src interfaces to be added*/
  /*copy newmask into addmask*/
  FILTER_INTF_MASKEQ(addMask,newMask);
  /*do the exor with oldMask and write back into addMask*/
  FILTER_INTF_MASKEXOREQ(addMask,oldMask);
  /*do the and with the newmask and write back to addMask*/
  FILTER_INTF_MASKANDEQ(addMask,newMask);
  /*convert the mask into the delete list*/
  filterMaskToList(&addMask,addList,&addNum);
  /*apply this add list to the filter database*/
  for (i=0;i<addNum;i++)
  {
	rc = usmDbFilterPortParmCanSet(UnitIndex, addList[i]);
	if (rc != L7_SUCCESS)
	  return L7_FAILURE;
    rc = filterSrcIntfAdd(macAddr,vlanId,addList[i], L7_TRUE);
    if (rc==L7_FAILURE)
      return rc;
  }
  /*if we reach here all is good and we can return a success*/
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Gets src interface in the MAC filter  
*
* @param    L7_uint32   UnitIndex  @b((input))  The unit for this operation
* @param    L7_uchar8*  macAddr    @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId     @b{(input)}  Vlan ID for the filter
* @param    L7_uchar8*  mask       @b{(output)}  Bit Mask of the interfaces
*                                               1 or Set indicates include this intf in src
*                                               0 indicates remove this interface if it was 
*                                               previously there
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    Mask is written into and is returned with bits set to represent interfaces that are 
*           in the src list, in the manner SNMP wants. MSb represents interface 1           
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterSrcIntfMaskGet(L7_uint32 UnitIndex,
                                  L7_uchar8* macAddr,
                                  L7_uint32 vlanId,
                                  L7_uchar8* mask)
{
  FILTER_INTF_MASK_t currentMask;

  L7_uint32 currentList[L7_FILTER_MAX_INTF];
  L7_uint32 numList;

  L7_RC_t rc;

  if (usmDbFeaturePresentCheck(UnitIndex, L7_FILTER_COMPONENT_ID, L7_FILTER_SOURCE_PORT_FILTERING_FEATURE_ID) != L7_TRUE)
    return L7_NOT_SUPPORTED;

  rc = filterSrcIntfListGet(macAddr,vlanId,&numList,currentList);
  if (rc==L7_FAILURE)
    return L7_FAILURE;
  filterListToMask(currentList,numList,&currentMask);
  return filterReverseMask(currentMask,(FILTER_INTF_MASK_t *)mask);
}
/*********************************************************************
*
* @purpose  Sets destination interface in the MAC filter  
*
* @param    L7_uint32   UnitIndex  @b((input))  The unit for this operation
* @param    L7_uchar8*  macAddr    @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId     @b{(input)}  Vlan ID for the filter
* @param    L7_uchar8*  maskOctets @b{(input)}  Bit Mask of the interfaces
*                                               1 or Set indicates include this intf in src
*                                               0 indicates remove this interface if it was 
*                                               previously there
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes               
*           
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterDstIntfMaskSet(L7_uint32 UnitIndex,
                                  L7_uchar8* macAddr,
                                  L7_uint32 vlanId,
                                  L7_uchar8* maskOctets)
{
  /*take care of the fact that this mask may include addition
    and deletion of dst interfaces. So need to compare this mask with
    the old list of dst interfaces*/
  FILTER_INTF_MASK_t* mask;
  FILTER_INTF_MASK_t oldMask;
  FILTER_INTF_MASK_t newMask;
  FILTER_INTF_MASK_t delMask;
  FILTER_INTF_MASK_t addMask;

  L7_uint32 oldList[L7_FILTER_MAX_INTF];
  L7_uint32 oldNum;
  L7_uint32 newList[L7_FILTER_MAX_INTF];
  L7_uint32 newNum;
  L7_uint32 addList[L7_FILTER_MAX_INTF];
  L7_uint32 addNum;
  L7_uint32 delList[L7_FILTER_MAX_INTF];
  L7_uint32 delNum;

  L7_RC_t rc;
  L7_uint32 i;

  L7_BOOL entryMulticast;

  /* check if destination port configuration is supported for unicast MAC addresses*/
  /* Destination port configuration is supoorted for multicast MAC addresses for all platforms*/
  entryMulticast=L7_FALSE;
  entryMulticast = filterCheckMacAddrTypeMulticast(macAddr);

  if ((entryMulticast == L7_FALSE) &&
      (usmDbFeaturePresentCheck(UnitIndex, L7_FILTER_COMPONENT_ID, L7_FILTER_UCAST_DEST_PORT_FILTERING_FEATURE_ID) != L7_TRUE))
        return L7_NOT_SUPPORTED;

  mask = (FILTER_INTF_MASK_t *)maskOctets;

  /*reverse the mask*/
  rc = filterReverseMask(*mask,&newMask);
  /*generate the new dst list*/
  rc = filterMaskToList(&newMask,newList,&newNum);
  /*get the old list of src interfaces*/
  rc = filterDstIntfListGet(macAddr,vlanId,&oldNum,oldList);
  if (rc==L7_FAILURE)
    return L7_FAILURE;
  /*generate old mask*/
  rc = filterListToMask(oldList,oldNum,&oldMask);
  /*calculate 1s to 0s i.e. those bits that were set in old but now deleted*/
  /*algorithm is all bitwise operation
    old exor new -> result
    result and old -> a mask of 1s to 0s w.r.t. old mask
    i.e.mask contains list of dst interface to be deleted*/
  /*copy newmask into delmask*/
  FILTER_INTF_MASKEQ(delMask,newMask);
  /*do the exor with oldMask  and write back into delMask*/
  FILTER_INTF_MASKEXOREQ(delMask,oldMask);
  /*do the and with oldMask and write back to delMask*/
  FILTER_INTF_MASKANDEQ(delMask,oldMask);
  /*convert the mask into the delete list*/
  filterMaskToList(&delMask,delList,&delNum);
  /*apply this delete list to the filter database*/
  for (i=0;i<delNum;i++)
  {
	rc = usmDbFilterPortParmCanSet(UnitIndex, delList[i]);
	if (rc != L7_SUCCESS)
	  return L7_FAILURE;
    rc = filterDstIntfDelete(macAddr,vlanId,delList[i]);
    if (rc==L7_FAILURE)
      return rc;
  }


  /*calculate 0s to 1s i.e. those bits that were set to 0 in old but now set or added*/
  /*algorithm is all bitwise operation 
    old exor new -> result
    result and new -> a mask of 0s to 1s w.r.t. old mask
    i.e. mask contains list of dst interfaces to be added*/
  /*copy newmask into addmask*/
  FILTER_INTF_MASKEQ(addMask,newMask);
  /*do the exor with oldMask and write back into addMask*/
  FILTER_INTF_MASKEXOREQ(addMask,oldMask);
  /*do the and with the newmask and write back to addMask*/
  FILTER_INTF_MASKANDEQ(addMask,newMask);
  /*convert the mask into the delete list*/
  filterMaskToList(&addMask,addList,&addNum);
  /*apply this add list to the filter database*/
  for (i=0;i<addNum;i++)
  {
	rc = usmDbFilterPortParmCanSet(UnitIndex, addList[i]);
	if (rc != L7_SUCCESS)
	  return L7_FAILURE;
    rc = filterDstIntfAdd(macAddr,vlanId,addList[i]);
    if (rc==L7_FAILURE)
      return rc;
  }
  /*if we reach here all is good and we can return a success*/
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Gets Destination interface in the MAC filter  
*
* @param    L7_uint32   UnitIndex  @b((input))  The unit for this operation
* @param    L7_uchar8*  macAddr    @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId     @b{(input)}  Vlan ID for the filter
* @param    L7_uchar8*  mask       @b{(output)}  Bit Mask of the interfaces
*                                               1 or Set indicates include this intf in src
*                                               0 indicates remove this interface if it was 
*                                               previously there
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    Mask is written into and is returned with bits set to represent interfaces that are 
*           in the dst list, in the manner SNMP wants. MSb represents interface 1           
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterDstIntfMaskGet(L7_uint32 UnitIndex,
                                  L7_uchar8* macAddr,
                                  L7_uint32 vlanId,
                                  L7_uchar8* mask)
{
  FILTER_INTF_MASK_t currentMask;

  L7_uint32 currentList[L7_FILTER_MAX_INTF];
  L7_uint32 numList;

  L7_RC_t rc;

  L7_BOOL entryMulticast;
  
  /* check if destination port configuration is supported for unicast MAC addresses*/
  /* Destination port configuration is supoorted for multicast MAC addresses for all platforms*/
 
  entryMulticast=L7_FALSE;
  entryMulticast = filterCheckMacAddrTypeMulticast(macAddr);
 
  if ((entryMulticast == L7_FALSE) &&
      (usmDbFeaturePresentCheck(UnitIndex, L7_FILTER_COMPONENT_ID, L7_FILTER_UCAST_DEST_PORT_FILTERING_FEATURE_ID) != L7_TRUE))
        return L7_NOT_SUPPORTED;  

  rc = filterDstIntfListGet(macAddr,vlanId,&numList,currentList);
  if (rc==L7_FAILURE)
    return L7_FAILURE;
  filterListToMask(currentList,numList,&currentMask);
  return filterReverseMask(currentMask,(FILTER_INTF_MASK_t *)mask);
}

/*********************************************************************
*
* @purpose  Checks to see if a MAC filter is configured or not  
*
* @param    L7_uint32   UnitIndex  @b((input))  The unit for this operation
* @param    L7_uchar8*  macAddr    @b((input))  MAC Address of the filter
* @param    L7_uint32   vlanId     @b{(input)}  Vlan ID for the filter
*
* @returns  L7_SUCCESS, if the MAC filter is configured
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterIsConfigured(L7_uint32 UnitIndex,
                                L7_uchar8* macAddr,
                                L7_uint32 vlanId)
{
  L7_BOOL rcBool;

  rcBool = filterIsConfigured(macAddr,vlanId);
  if (rcBool == L7_TRUE)
    return L7_SUCCESS;
  else
    return L7_FAILURE;

}
/*********************************************************************
*
* @purpose  Gets the buffer size used by filtering component for the bit mask  
*
* @param    L7_uint32   UnitIndex  @b((input))  The unit for this operation
* @param    L7_uint32*  bufferSize @b{(output)} Buffer size for the bit mask
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if failure
*
* @notes    size is passed back in terms of bytes
*
* @end
*********************************************************************/
L7_RC_t usmDbFilterSizeGet(L7_uint32 UnitIndex, 
                           L7_uint32* bufferSize)
{
  *bufferSize = FILTER_INTF_INDICES;
  return L7_SUCCESS;
}
/*********************************************************************
*
* @purpose  Checks if a particular interface is present in any MAC filter 
*
* @param    L7_uint32   UnitIndex  @b((input)) The unit for this operation
* @param    L7_uint32   intIfNum   @b((input)) Internal Interface Number
*
* @returns  L7_TRUE, if the interface is in any filter
* @returns  L7_FALSE, if the interface is not associated with any MAC filter
*
* @notes    An interface is associated with a MAC filter if it is in the filter's
*           source list or destination list. If an interface is found on any list
*           of any configured MAC filter then this routine will return a L7_TRUE
*           Only if the interface is NOT found on ANY list of ANY filter, a 
*           L7_FALSE is returned
*           
*
* @end
*********************************************************************/
L7_BOOL usmDbFilterIsIntfInAnyFilter(L7_uint32 UnitIndex,
                                     L7_uint32 intIfNum)
{
  return filterIsIntfInAnyFilter(intIfNum);
}

/*********************************************************************
* @purpose  Check to see if the port is the appropriate type on which
*			the parm can be set
*
* @param    unitIndex           Unit Index
* @param 	intIfNum			Internal interface number
*
* @returns  L7_SUCCESS			The parm can be set on this port type
* @returns  L7_NOT_SUPPORTED	The port does not support this parm to be set
*
* @notes    presently this function tests only for logical vlan intf.
*                                 
* @end
*********************************************************************/
L7_RC_t usmDbFilterPortParmCanSet(L7_uint32 unitIndex, L7_uint32 intIfNum)
{
  if (filterIsValidIntf(intIfNum) != L7_TRUE)
  {
    return L7_NOT_SUPPORTED;
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
L7_BOOL usmDbFilterCheckMacAddrTypeMulticast(L7_uchar8 *macAddr)
{
  return filterCheckMacAddrTypeMulticast(macAddr);
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
L7_RC_t usmdbFilterVlanFilteringModeSet(L7_uint32 unit, L7_uint32 vlanId, L7_FILTER_VLAN_FILTER_MODE_t mode)
{
    return filterVlanFilteringModeSet(vlanId,mode);
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
L7_RC_t usmdbFilterVlanFilteringModeGet(L7_uint32 unit, L7_uint32 vlanId, L7_FILTER_VLAN_FILTER_MODE_t *mode)
{
    return filterVlanFilteringModeGet(vlanId,mode);
}

/*********************************************************************************/
/***************************************TEST ROUTINES*****************************/
/*********************************************************************************/
void usmdbFilterTest(L7_uint32 unitIndex)
{
  L7_uchar8 macAddr[6];
  L7_uint32 srcList[L7_FILTER_MAX_INTF];
  L7_uint32 dstList[L7_FILTER_MAX_INTF];
  L7_uint32 intf,numSrc,numDst;
  L7_uchar8 srcMask[FILTER_INTF_INDICES];
  L7_uchar8 dstMask[FILTER_INTF_INDICES];
  L7_uchar8 testMask[FILTER_INTF_INDICES];
  FILTER_INTF_MASK_t mask;
  L7_uint32 testList[L7_FILTER_MAX_INTF];
  L7_uint32 numTest;

  macAddr[0] = 0x01;
  macAddr[1] = 0x70;
  macAddr[2] = 0xC2;
  macAddr[3] = 0x00;
  macAddr[4] = 0x00;
  macAddr[5] = 0x04;
  filterCreate(macAddr,1);
  intf = 2;

  srcList[0] = 1;
  srcList[1] = 2;
  srcList[2] = 3;
  numSrc = 3;
  dstList[0] = 1;
  dstList[1] = 2;
  dstList[2] = 3;
  numDst = 3;

  filterListToMask(srcList,numSrc,(FILTER_INTF_MASK_t *)testMask);
  filterMaskToList((FILTER_INTF_MASK_t *)testMask,testList,&numTest);

  filterListToMask(srcList,numSrc,(FILTER_INTF_MASK_t *)testMask);
  memcpy((void *)&mask,testMask,FILTER_INTF_INDICES);
  filterReverseMask(mask,(FILTER_INTF_MASK_t *)srcMask);
  filterListToMask(dstList,numDst,(FILTER_INTF_MASK_t *)testMask);
  memcpy((void *)&mask,testMask,FILTER_INTF_INDICES);
  filterReverseMask(mask,(FILTER_INTF_MASK_t *)dstMask);




  usmDbFilterSrcIntfMaskSet(1,macAddr,1,srcMask);
  usmDbFilterDstIntfMaskSet(1,macAddr,1,dstMask);


  memset((void *)srcList,0x00,sizeof(L7_uint32)*L7_FILTER_MAX_INTF);
  memset((void *)dstList,0x00,sizeof(L7_uint32)*L7_FILTER_MAX_INTF);
  memset((void *)srcMask,0x00,sizeof(L7_uchar8)*FILTER_INTF_INDICES);
  memset((void *)dstMask,0x00,sizeof(L7_uchar8)*FILTER_INTF_INDICES);

  usmDbFilterSrcIntfMaskGet(1,macAddr,1,srcMask);
  memcpy((void *)&mask,srcMask,FILTER_INTF_INDICES);
  filterReverseMask(mask,(FILTER_INTF_MASK_t *)srcMask);
  usmDbFilterDstIntfMaskGet(1,macAddr,1,dstMask);
  memcpy((void *)&mask,dstMask,FILTER_INTF_INDICES);
  filterReverseMask(mask,(FILTER_INTF_MASK_t *)dstMask);

  filterMaskToList((FILTER_INTF_MASK_t *)srcMask,srcList,&numSrc);
  filterMaskToList((FILTER_INTF_MASK_t *)dstMask,dstList,&numDst);


  filterSrcIntfDelete(macAddr,1,intf);
  filterDstIntfDelete(macAddr,1,intf);


}

#if 0
/*********************************************************************
* @purpose  Determine if the specified MAC address is a member of the restricted set.
*
* @param  *macaddr - pointer to mac address 
*
* @returns L7_TRUE if restricted mac, L7_FALSE otherwise
*
* @end
*
*********************************************************************/
L7_BOOL usmDbFilterIsRestrictedFilterMac(L7_uchar8 *macaddr)
{
    L7_BOOL    rc;
    L7_uint32  i;

    /* --------------------------- */
    /* Check for 00:00:00:00:00:00 */
    /* --------------------------- */
    rc = L7_TRUE;
    for (i=0; i<6; i++)
    {
      if (macaddr[i] != 0x00)
      {
        rc = L7_FALSE;
        break;
      }
    }
    if (rc == L7_TRUE)
      return L7_TRUE;


    /* --------------------------- */
    /* Check for FF:FF:FF:FF:FF:FF */
    /* --------------------------- */
    rc = L7_TRUE;
    for (i=0; i<6; i++)
    {
      if (macaddr[i] != 0xFF)
      {
        rc = L7_FALSE;
        break;
      }
    }
    if (rc == L7_TRUE)
      return L7_TRUE;


    /* ------------------------------------------------ */
    /* Check for 01:80:C2:00:00:00 to 01:80:C2:00:00:10 */
    /* ------------------------------------------------ */
    rc = L7_FALSE;
    if (macaddr[0] == 0x01)
    {
      if (macaddr[1] == 0x80)
      {
        if (macaddr[2] == 0xC2)
        {
          if (macaddr[3] == 0x00)
          {
            if (macaddr[4] == 0x00)
            {
              if ((macaddr[5] == 0x10) || (macaddr[5] & 0xF0) == 0x00)
              {
                rc = L7_TRUE;
              }
            }
          }
        }
      }
    }
    if (rc == L7_TRUE)
      return L7_TRUE;


    /* ------------------------------------------------ */
    /* Check for 01:80:C2:00:00:20 to 01:80:C2:00:00:2F */
    /* ------------------------------------------------ */
    rc = L7_FALSE;
    if (macaddr[0] == 0x01)
    {
      if (macaddr[1] == 0x80)
      {
        if (macaddr[2] == 0xC2)
        {
          if (macaddr[3] == 0x00)
          {
            if (macaddr[4] == 0x00)
            {
              if ((macaddr[5] & 0xF0) == 0x20) 
              {
                rc = L7_TRUE;
              }
            }
          }
        }
      }
    }
    if (rc == L7_TRUE)
      return L7_TRUE;

    return L7_FALSE;

}

#endif
/*********************************************************************
* @purpose  Check if the interface is valid for filtering
*
* @param  unitIndex     Unit Index
* @param 	intIfNum			Internal interface number
*
* @returns L7_TRUE if valid
* @returns L7_FALSE otherwise
*
* @end
*
*********************************************************************/
L7_BOOL usmDbFilterIsValidIntf(L7_uint32 unitIndex, L7_uint32 intIfNum)
{
  return filterIsValidIntf(intIfNum);
}
