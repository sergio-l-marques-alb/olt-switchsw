/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename src\l7public\porting\usmdb_filter_api.h
*
* @purpose externs for USMDB layer
*
* @component unitmgr
*
* @comments none
*
* @create 1/7/2002
*
* @author skalyanam
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/


#ifndef USMDB_FILTER_API_H
#define USMDB_FILTER_API_H
#include "filter_exports.h"

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
extern L7_RC_t usmDbFilterCreate(L7_uint32 UnitIndex, 
                                 L7_uchar8 *macAddr,
                                 L7_uint32 vlanId);
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
                                     L7_INTF_MASK_t *srcIntfMask);
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
extern L7_RC_t usmdbFilterSrcIntfAdd(L7_uint32 UnitIndex, 
                                     L7_uchar8* macAddr,
                                     L7_uint32 vlanId, 
                                     L7_uint32 intIfNum);

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
extern L7_RC_t usmdbFilterSrcIntfDelete(L7_uint32 UnitIndex,
                                        L7_uchar8* macAddr,
                                        L7_uint32 vlanId,
                                        L7_uint32 intIfNum);

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
extern L7_RC_t usmdbFilterDstIntfAdd(L7_uint32 UnitIndex, 
                                     L7_uchar8* macAddr,
                                     L7_uint32 vlanId, 
                                     L7_uint32 intIfNum);

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
extern L7_RC_t usmdbFilterDstIntfDelete(L7_uint32 UnitIndex,
                                        L7_uchar8* macAddr,
                                        L7_uint32 vlanId,
                                        L7_uint32 intIfNum);

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
extern L7_uint32 usmDbFilterRemove(L7_uint32 UnitIndex,
                                   L7_uchar8* macAddr,
                                   L7_uint32 vlanId);


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
extern L7_RC_t usmDbFilterFirstGet(L7_uint32 UnitIndex,
                                   L7_uchar8* macAddr,
                                   L7_uint32* vlanId);

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
extern L7_RC_t usmDbFilterNextGet(L7_uint32 UnitIndex,
                                  L7_uchar8* macAddr,
                                  L7_uint32 vlanId,
                                  L7_uchar8* nextAddr,
                                  L7_uint32* nextVlanId);

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
* @notes    srcIntfList must be of size FILTER_MAX_INTF
*           Only the first numSrcIntf entries in the srcIntfList have valid 
*           interfaces, rest are garbage
*
* @end
*********************************************************************/
extern L7_RC_t usmDbFilterSrcIntfListGet(L7_uint32 UnitIndex,
                                         L7_uchar8* macaddr,
                                         L7_uint32 vlanId,
                                         L7_uint32* numSrcIntf,
                                         L7_uint32 srcIntfList[]);

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
* @notes    dstIntfList must be of size FILTER_MAX_INTF
*           Only the first numDstIntf entries in the dstIntfList have valid 
*           interfaces, rest are garbage
*
* @end
*********************************************************************/
extern L7_RC_t usmDbFilterDstIntfListGet(L7_uint32 UnitIndex,
                                         L7_uchar8* macaddr,
                                         L7_uint32 vlanId,
                                         L7_uint32* numDstIntf,
                                         L7_uint32 dstIntfList[]);

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
                                  L7_uint32 dstFiltIntfList[]);
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
extern L7_RC_t usmdbFilterCountGet(L7_uint32 UnitIndex,
                                   L7_uint32* count);


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
*                                               Buffer size must be FILTER_INTF_INDICES
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
extern L7_RC_t usmDbFilterSrcIntfMaskSet(L7_uint32 UnitIndex,
                                         L7_uchar8* macAddr,
                                         L7_uint32 vlanId,
                                         L7_uchar8* maskOctets);
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
*                                               Buffer size must be FILTER_INTF_INDICES
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    Mask is written into and is returned with bits set to represent interfaces that are 
*           in the src list, in the manner SNMP wants. MSb represents interface 1           
*
* @end
*********************************************************************/
extern L7_RC_t usmDbFilterSrcIntfMaskGet(L7_uint32 UnitIndex,
                                         L7_uchar8* macAddr,
                                         L7_uint32 vlanId,
                                         L7_uchar8* mask);
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
*                                               Buffer size must be FILTER_INTF_INDICES
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes               
*           
*
* @end
*********************************************************************/
extern L7_RC_t usmDbFilterDstIntfMaskSet(L7_uint32 UnitIndex,
                                         L7_uchar8* macAddr,
                                         L7_uint32 vlanId,
                                         L7_uchar8* maskOctets);
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
*                                               previously there.
*                                               Buffer size must be FILTER_INTF_INDICES
*
* @returns  L7_SUCCESS, if success
* @returns  L7_FAILURE, if there is no MAC Filter
*
* @notes    Mask is written into and is returned with bits set to represent interfaces that are 
*           in the dst list, in the manner SNMP wants. MSb represents interface 1           
*
* @end
*********************************************************************/
extern L7_RC_t usmDbFilterDstIntfMaskGet(L7_uint32 UnitIndex,
                                         L7_uchar8* macAddr,
                                         L7_uint32 vlanId,
                                         L7_uchar8* mask);
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
extern L7_RC_t usmDbFilterIsConfigured(L7_uint32 UnitIndex,
                                       L7_uchar8* macAddr,
                                       L7_uint32 vlanId);
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
extern L7_RC_t usmDbFilterSizeGet(L7_uint32 UnitIndex, 
                                  L7_uint32* bufferSize);
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
extern L7_BOOL usmDbFilterIsIntfInAnyFilter(L7_uint32 UnitIndex,
                                            L7_uint32 intIfNum);

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
L7_RC_t usmDbFilterPortParmCanSet(L7_uint32 unitIndex, L7_uint32 intIfNum);


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
extern L7_BOOL usmDbFilterIsValidIntf(L7_uint32 unitIndex, L7_uint32 intIfNum);


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
L7_BOOL usmDbFilterIsRestrictedFilterMac(L7_uchar8 *macaddr);       

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
L7_BOOL usmDbFilterCheckMacAddrTypeMulticast(L7_uchar8 *macAddr);

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
L7_RC_t usmdbFilterVlanFilteringModeSet(L7_uint32 unit, L7_uint32 vlanId, L7_FILTER_VLAN_FILTER_MODE_t mode);

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
L7_RC_t usmdbFilterVlanFilteringModeGet(L7_uint32 unit, L7_uint32 vlanId, L7_FILTER_VLAN_FILTER_MODE_t *mode);

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
L7_RC_t usmdbFilterDstIntfMaskAdd(L7_uint32 UnitIndex, 
                              L7_uint32 vlanId, 
                              L7_enetMacAddr_t macAddr,
                              L7_INTF_MASK_t fwdMask,
                              L7_INTF_MASK_t filterMask);

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
                              L7_INTF_MASK_t filterMask);

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
L7_RC_t usmDbFilterAdminModeSet(L7_uint32 UnitIndex, L7_BOOL enable);

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
L7_RC_t usmDbFilterAdminModeGet(L7_uint32 UnitIndex, L7_BOOL *enable);


#endif /* USMDB_FILTER_API_H*/

