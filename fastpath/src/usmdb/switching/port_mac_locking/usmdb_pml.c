/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename usmdb_pml.c
*
* @purpose Provide interface to hardware API's for unitmgr components
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

#include <string.h>
#include "l7_common.h"
#include "usmdb_pml_api.h"

#include "pml_api.h"
#include "nimapi.h"

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking Admin mode
*
* @param    L7_uint32  UnitIndex    @b((input)) The unit for this operation
* @param    L7_uchar8  *adminMode   @b((output)) Port MAC Locking admin mode
*                            
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPmlAdminModeGet(L7_uint32 UnitIndex, L7_uint32 *adminMode)
{
  return pmlAdminModeGet(adminMode);
}

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking Admin mode
*
* @param    L7_uint32  UnitIndex   @b((input)) The unit for this operation
* @param    L7_uchar8  adminMode   @b((input)) Port MAC Locking admin mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPmlAdminModeSet(L7_uint32 UnitIndex, L7_uint32 adminMode)
{
  return pmlAdminModeSet(adminMode);
}

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking mode for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
* @param    L7_uint32  intIfNum  @b((input))  Internal interface number
* @param    L7_uchar8  *mode     @b((output)) Port MAC Locking intf mode
*                            
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPmlIntfModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *mode)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfModeGet(intIfNum, mode);
}

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking mode for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
* @param    L7_uint32  intIfNum  @b((input)) Internal interface number
* @param    L7_uchar8  mode      @b((input))  Port MAC Locking intf mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPmlIntfModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 mode)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfModeSet(intIfNum, mode);
}

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking dynamic limit for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfDynamicLimitGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *limit)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfDynamicLimitGet(intIfNum, limit);
}

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking maximum dynamic limit for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfDynamicMaxLimitGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *limit)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfDynamicMaxLimitGet(intIfNum, limit);
}

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking dynamic limit for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfDynamicLimitSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 limit)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfDynamicLimitSet(intIfNum, limit);
}

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking maximum static limit for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfStaticMaxLimitGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *limit)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfStaticMaxLimitGet(intIfNum, limit);
}

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking static limit for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfStaticLimitGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *limit)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfStaticLimitGet(intIfNum, limit);
}

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking current static entries used count
*           for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfStaticCountGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *count)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfStaticCountGet(intIfNum, count);
}

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking static limit for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfStaticLimitSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 limit)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfStaticLimitSet(intIfNum, limit);
}

/*********************************************************************
*
* @purpose  Gets the Port MAC Locking violation trap mode for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfViolationTrapModeGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 *trapMode)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfViolationTrapModeGet(intIfNum, trapMode);
}

/*********************************************************************
*
* @purpose  Sets the Port MAC Locking violation trap mode for the specified interface
*
* @param    L7_uint32  UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfViolationTrapModeSet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_uint32 trapMode)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfViolationTrapModeSet(intIfNum, trapMode);
}

/*********************************************************************
* @purpose gets the Frequency interval between successive SNMP traps
*
* @param L7_uint32 intIfNum   @b((input)) internal interface number
* @param L7_uint32 interval   @b((input)) Time interval in seconds
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPmlIntfTrapFrequencyGet(L7_uint32 intIfNum, L7_uint32 *interval)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfTrapFrequencyGet(intIfNum, interval);
}

/*********************************************************************
*
* @purpose Sets the Frequency interval between successive SNMP traps
*
* @param L7_uint32 intIfNum   @b((input)) internal interface number
* @param L7_uint32 interval   @b((input)) Time interval in seconds
*
* @returns L7_SUCCESS
* @returns L7_FAILURE
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbPmlIntfTrapFrequencySet(L7_uint32 intIfNum, L7_uint32 interval)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfTrapFrequencySet(intIfNum, interval);
}

/*********************************************************************
*
* @purpose  Adds a statically locked entry to an interface.
*
* @param    L7_uint32        UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfStaticEntryAdd(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfStaticEntryAdd(intIfNum, macAddr, vlanId);
}

/*********************************************************************
*
* @purpose  Deletes a statically locked entry from an interface.
*
* @param    L7_uint32        UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfStaticEntryDelete(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_enetMacAddr_t macAddr, L7_ushort16 vlanId)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfStaticEntryDelete(intIfNum, macAddr, vlanId);
}

/*********************************************************************
*
* @purpose  Gets the first static entry for an interface.
*
* @param    L7_uint32        UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfStaticEntryGetFirst(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfStaticEntryGetFirst(intIfNum, macAddr, vlanId);
}

/*********************************************************************
*
* @purpose  Gets the first static entry for an interface.
*
* @param    L7_uint32        UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfStaticEntryGetNext(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfStaticEntryGetNext(intIfNum, macAddr, vlanId);
}

/*********************************************************************
*
* @purpose  Gets the first dynamic entry for an interface.
*
* @param    L7_uint32        UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfDynamicEntryGetFirst(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfDynamicEntryGetFirst(intIfNum, macAddr, vlanId);
}

/*********************************************************************
*
* @purpose  Gets the first dynamic entry for an interface.
*
* @param    L7_uint32        UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfDynamicEntryGetNext(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfDynamicEntryGetNext(intIfNum, macAddr, vlanId);
}

/*********************************************************************
*
* @purpose  Converts dynamically locked entries to statically locked entries 
*           on this interface.
*
* @param    L7_uint32        UnitIndex @b((input)) The unit for this operation
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbPmlIntfDynamicToStaticMove(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfDynamicToStaticMove(intIfNum);
}

/*********************************************************************
*
* @purpose  Gets the last violation entry for an interface.
*
* @param    L7_uint32        UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmDbPmlIntfLastViolationAddrGet(L7_uint32 UnitIndex, L7_uint32 intIfNum, L7_enetMacAddr_t *macAddr, L7_ushort16 *vlanId)
{
  if (pmlIsValidIntf(intIfNum) != L7_TRUE)
    return L7_FAILURE;

  return pmlIntfLastViolationAddrGet(intIfNum, macAddr, vlanId);
}

/*********************************************************************
*
* @purpose check to see if intIfNum is a valid PML Interface
*
* @param    L7_uint32        UnitIndex @b((input)) The unit for this operation
* @param    L7_uint32        intIfNum  @b((input)) Internal interface number
*
* @returns L7_TRUE     If valid interface
* @returns L7_FALSE    If not valid interface 
*
* @notes none
*       
* @end
*
*********************************************************************/
L7_BOOL usmDbPmlIsValidIntf(L7_uint32 UnitIndex, L7_uint32 intIfNum)
{
  return pmlIsValidIntf(intIfNum);
}

/*********************************************************************
*
* @purpose gets the first valid PML interface
*
* @param L7_uint32 intf       @b((input)) Interface Number
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @notes none
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbPmlFirstValidIntfGet(L7_uint32 *intIfNum)
{
  L7_uint32 tempIntIfNum;
  L7_RC_t   rc;

  rc = nimFirstValidIntfNumber(&tempIntIfNum);

  while (rc == L7_SUCCESS)
  {
    if (pmlIsValidIntf(tempIntIfNum))
    {
      *intIfNum = tempIntIfNum;
      return L7_SUCCESS;
    }

    rc = nimNextValidIntfNumber(tempIntIfNum, &tempIntIfNum);
  }

  return L7_FAILURE;
}

/*********************************************************************
*
* @purpose gets the first valid PML interface
*
* @param L7_uint32 intf       @b((input)) Interface Number
*
* @returns L7_SUCCESS  
* @returns L7_FAILURE
*
* @notes none
*       
* @end
*
*********************************************************************/
L7_RC_t usmDbPmlNextValidIntfGet(L7_uint32 intIfNum, L7_uint32 *nextIntIfNum)
{
  L7_uint32 tempIntIfNum;
  L7_RC_t   rc;

  rc = nimNextValidIntfNumber(intIfNum, &tempIntIfNum);

  while (rc == L7_SUCCESS)
  {
    if (pmlIsValidIntf(tempIntIfNum))
    {
      *nextIntIfNum = tempIntIfNum;
      return L7_SUCCESS;
    }

    rc = nimNextValidIntfNumber(tempIntIfNum, &tempIntIfNum);
  }

  return L7_FAILURE;
}
/*********************************************************************
*
* @purpose  Returns the count of dynamcially learned Mac Addresses 
*
* @param    L7_uint32        UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmdbPmlGetDynamicLearnedAddrCount(L7_uint32 UnitIndex, L7_uint32 interface,L7_uint32 *learnedCount)
{
  return pmlGetDynamicLearnedAddrCount(interface,learnedCount);
}
/*********************************************************************
*
* @purpose  Returns the count of static learned Mac Addresses
*
* @param    L7_uint32        UnitIndex @b((input)) The unit for this operation
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
L7_RC_t usmdbPmlGetStaticLearnedAddrCount(L7_uint32 UnitIndex, L7_uint32 interface,L7_uint32 *learnedCount)
{
  return pmlGetStaticLearnedAddrCount(interface,learnedCount);
}

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
L7_RC_t usmdbPmlGetStaticAddrCountByVlan(L7_uint32 vlanId,L7_uint32 *count)
{
  return pmlGetStaticAddrCountByVlan(vlanId, count);
}

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
L7_RC_t usmdbPmlGetStaticAddrCountGlobal(L7_uint32 *count)
{
  return pmlGetStaticAddrCountGlobal(count);
}

/********************************************************************
*
* Debug Functions
*
*********************************************************************/
void usmDbPmlDebugDump(L7_uint32 intIfNum)
{
  L7_uint32        globalMode;
  L7_uint32        intfMode;
  L7_uint32        dynamicLimit;
  L7_uint32        staticLimit;
  L7_uint32        staticCount;
  L7_uint32        trapMode;
  L7_enetMacAddr_t macAddr;
  L7_ushort16      vlanId;
  L7_RC_t          rc;

  (void)usmDbPmlAdminModeGet(0,&globalMode);
  (void)usmDbPmlIntfModeGet(0, intIfNum, &intfMode);
  (void)usmDbPmlIntfDynamicLimitGet(0, intIfNum, &dynamicLimit);
  (void)usmDbPmlIntfStaticLimitGet(0, intIfNum, &staticLimit);
  (void)usmDbPmlIntfStaticCountGet(0, intIfNum, &staticCount);
  (void)usmDbPmlIntfViolationTrapModeGet(0, intIfNum, &trapMode);

  printf("Intf %d\n", intIfNum);
  printf("globalMode %d\n", globalMode);
  printf("intfMode %d\n", intfMode);
  printf("dynamicLimit %d\n", dynamicLimit);
  printf("staticLimit %d\n", staticLimit);
  printf("staticCount %d\n", staticCount);
  printf("trapMode %d\n", trapMode);

  printf("Static Entries:\n");
  rc = usmDbPmlIntfStaticEntryGetFirst(0, intIfNum, &macAddr, &vlanId);
  while (rc == L7_SUCCESS)
  {
    printf("Vlan %d, %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
           vlanId,
           macAddr.addr[0],
           macAddr.addr[1],
           macAddr.addr[2],
           macAddr.addr[3],
           macAddr.addr[4],
           macAddr.addr[5]);

    rc = usmDbPmlIntfStaticEntryGetNext(0, intIfNum, &macAddr, &vlanId);
  }

  printf("Dynamic Entries:\n");
  rc = usmDbPmlIntfDynamicEntryGetFirst(0, intIfNum, &macAddr, &vlanId);
  while (rc == L7_SUCCESS)
  {
    printf("Vlan %d, %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
           vlanId,
           macAddr.addr[0],
           macAddr.addr[1],
           macAddr.addr[2],
           macAddr.addr[3],
           macAddr.addr[4],
           macAddr.addr[5]);

    rc = usmDbPmlIntfDynamicEntryGetNext(0, intIfNum, &macAddr, &vlanId);
  }

  printf("Last Violation Address:\n");
  rc = usmDbPmlIntfLastViolationAddrGet(0, intIfNum, &macAddr, &vlanId);
  if (rc == L7_SUCCESS)
  {
    printf("Vlan %d, %2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x\n",
           vlanId,
           macAddr.addr[0],
           macAddr.addr[1],
           macAddr.addr[2],
           macAddr.addr[3],
           macAddr.addr[4],
           macAddr.addr[5]);
  }
}

void usmDbPmlDebugStaticAdd(L7_uint32 intIfNum, L7_uint32 vidMacHi, L7_uint32 macLo)
{
  L7_enetMacAddr_t macAddr;
  L7_ushort16      vlanId;

  vlanId = vidMacHi >> 16;
  *(L7_ushort16 *)&macAddr.addr[0] = vidMacHi & 0x0000ffff;
  memcpy(&macAddr.addr[2], &macLo, 4);
  usmDbPmlIntfStaticEntryAdd(0, intIfNum, macAddr, vlanId);
}

void usmDbPmlDebugStaticDelete(L7_uint32 intIfNum, L7_uint32 vidMacHi, L7_uint32 macLo)
{
  L7_enetMacAddr_t macAddr;
  L7_ushort16      vlanId;

  vlanId = vidMacHi >> 16;
  *(L7_ushort16 *)&macAddr.addr[0] = vidMacHi & 0x0000ffff;
  memcpy(&macAddr.addr[2], &macLo, 4);
  usmDbPmlIntfStaticEntryDelete(0, intIfNum, macAddr, vlanId);
}
