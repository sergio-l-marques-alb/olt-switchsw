/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename pml_api.h
*
* @purpose Externs for Port MAC Locking
*
* @component Port MAC Locking
*
* @comments none
*
* @create 05/21/2004
*
* @author colinw
* @end
*
**********************************************************************/
/*********************************************************************
 *                   
 ********************************************************************/

#ifndef PML_API_H
#define PML_API_H


/*********************************************************************
*
* @purpose  Gets the Port MAC Locking Admin mode
*
* @param    L7_uint32  *adminMode   @b((output)) Port MAC Locking admin mode
*                            
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlAdminModeGet(L7_uint32 *adminMode);

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking Admin mode
*
* @param    L7_uint32  adminMode   @b((input)) Port MAC Locking admin mode
*
* @returns  L7_SUCCESS, if mode was set
* @returns  L7_FAILURE, if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlAdminModeSet(L7_uint32 adminMode);

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((output)) Internal interface number
* @param    L7_uint32  mode      @b((input))  Port MAC Locking intf mode
*
* @returns  L7_SUCCESS, if mode was set successfully
* @returns  L7_FAILURE, if interface out of range or if invalid mode was specified
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfModeSet(L7_uint32 intIfNum, L7_uint32 mode);

/*********************************************************************
*
* @purpose  Gets the configured Port MAC Locking mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uint32  *mode     @b((output)) Configured Port MAC Locking intf mode
*                            
*
* @returns  L7_SUCCESS, if mode was retrieved successfully
* @returns  L7_FAILURE, if interface out of range
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfModeGet(L7_uint32 intIfNum, L7_uint32 *mode);

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking dynamic limit for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *limit    @b((output)) Dynamic Limit
*                            
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfDynamicLimitGet(L7_uint32 intIfNum, L7_uint32 *limit);

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking maximum dynamic limit for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *limit    @b((output)) Maximum Dynamic Limit
*                            
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfDynamicMaxLimitGet(L7_uint32 intIfNum, L7_uint32 *limit);

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking dynamic limit for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uchar8  limit     @b((input))  Dynamic Limit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfDynamicLimitSet(L7_uint32 intIfNum, L7_uint32 limit);

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking static limit for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *limit    @b((output)) Static Limit
*                            
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticLimitGet(L7_uint32 intIfNum, L7_uint32 *limit);

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking maximum static limit for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *limit    @b((output)) Maximum Static Limit
*                            
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticMaxLimitGet(L7_uint32 intIfNum, L7_uint32 *limit);

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking current static entries used count
*           for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *count    @b((output)) Count
*                            
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticCountGet(L7_uint32 intIfNum, L7_uint32 *count);

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking static limit for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uchar8  limit     @b((input)) Static Limit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticLimitSet(L7_uint32 intIfNum, L7_uint32 limit);

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking violation trap mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *trapMode @b((output)) Violation Trap Mode
*                            
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfViolationTrapModeGet(L7_uint32 intIfNum, L7_uint32 *trapMode);

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking violation trap mode for the specified interface
*
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uchar8  trapMode  @b((input)) Violation Trap Mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfViolationTrapModeSet(L7_uint32 intIfNum, L7_uint32 trapMode);

/*********************************************************************
* @purpose Gets the Frequency interval between successive SNMP traps
* 
* @param L7_uint32 intIfNum   @b((input)) internal interface number
* @param L7_uint32 seconds    @b((input)) Time interval in seconds
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
* 
* @notes none
* 
* @end
* 
*********************************************************************/
L7_RC_t pmlIntfTrapFrequencyGet(L7_uint32 intIfNum, L7_uint32 *seconds);

/*********************************************************************
* @purpose Sets the Frequency interval between successive SNMP traps
* 
* @param L7_uint32 intIfNum   @b((input)) internal interface number
* @param L7_uint32 seconds    @b((input)) Time interval in seconds
* 
* @returns L7_SUCCESS
* @returns L7_FAILURE
* 
* @notes none
* 
* @end
* 
*********************************************************************/
L7_RC_t pmlIntfTrapFrequencySet(L7_uint32 intIfNum, L7_uint32 seconds);

/*********************************************************************
*
* @purpose  Adds a statically locked entry to an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((input)) MAC address
* @param    L7_ushort16      vlanId    @b((input)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticEntryAdd(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId);

/*********************************************************************
*
* @purpose  Deletes a statically locked entry from an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((input)) MAC address
* @param    L7_ushort16      vlanId    @b((input)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticEntryDelete(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId);

/*********************************************************************
*
* @purpose  Gets the first static entry for an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticEntryGetFirst(L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId);

/*********************************************************************
*
* @purpose  Gets the first static entry for an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfStaticEntryGetNext(L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId);

/*********************************************************************
*
* @purpose  Gets the first dynamic entry for an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfDynamicEntryGetFirst(L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId);

/*********************************************************************
*
* @purpose  Gets the first dynamic entry for an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfDynamicEntryGetNext(L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId);

/*********************************************************************
*
* @purpose  Converts dynamically locked entries to statically locked entries 
*           on this interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfDynamicToStaticMove(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Gets the last violation entry for an interface.
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlIntfLastViolationAddrGet(L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId);

/*********************************************************************
* @purpose  Determine if the interface type is valid in Port MAC Locking
*
* @param    sysIntfType  @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*
* @end
*********************************************************************/
L7_BOOL pmlIsValidIntfType(L7_uint32 sysIntfType);

/*********************************************************************
* @purpose  Determine if the interface is valid in Port MAC Locking
*
* @param    intIfNum  @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments none
*       
* @end
*********************************************************************/
L7_BOOL pmlIsValidIntf(L7_uint32 intIfNum);

/*********************************************************************
*
* @purpose  Callback from DTL informing about learned address
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlLearnEntryCallBack(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId);

/*********************************************************************
*
* @purpose  Callback from DTL informing about aged address
*
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlAgeEntryCallBack(L7_enetMacAddr_t macAddr, L7_ushort16 vlanId);

/*********************************************************************
*
* @purpose  Callback from DTL informing about an unknown address
*
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
* @param    L7_enetMacAddr_t macAddr   @b((output)) MAC address
* @param    L7_ushort16      vlanId    @b((output)) VLAN ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlUnknownAddrCallBack(L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId);

/*********************************************************************
*
* @purpose  Returns the count of dynamcially learned Mac Addresses
*
* @param    L7_uint32        interface @b((input)) Interface for which learned addresses 
*                            needs to  be determined
* @param    L7_uint32        learnedCount  @b((output)) Learned Dynamic addresses count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlGetDynamicLearnedAddrCount(L7_uint32 interface, L7_uint32 *learnedCount);
/*********************************************************************
*
* @purpose  Returns the count of static learned Mac Addresses
*
* @param    L7_uint32        interface @b((input)) The interface for which count
*                            is to be determined
* @param    L7_uint32        learnedCount  @b((output)) static addresses count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlGetStaticLearnedAddrCount(L7_uint32 interface, L7_uint32 *learnedCount);

/*********************************************************************
*
* @purpose  Returns the count of static learned Mac Addresses
*
* @param    L7_uint32        vlanId @b((input)) 
* @param    L7_uint32        count  @b((output)) static addresses count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlGetStaticAddrCountByVlan(L7_uint32 vlanId,L7_uint32 *count);

/*********************************************************************
*
* @purpose  Returns the count of static learned Mac Addresses
*
* @param    L7_uint32        count  @b((output)) static addresses count
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t pmlGetStaticAddrCountGlobal(L7_uint32 *count);


#endif /* PML_API_H */
