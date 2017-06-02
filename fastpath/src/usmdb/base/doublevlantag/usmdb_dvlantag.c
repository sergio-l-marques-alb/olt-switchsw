/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  usmdb_dvlantag_api.c
*
* @purpose   USMDB API's for Double Vlan Tagging
*
* @component dvlantag
*
* @component unitmgr
*
* @create    08/20/2003
*
* @author    skalyanam
*
* @end
**********************************************************************/

#include "dvlantag_api.h"
#include "usmdb_dvlantag_api.h"

/*********************************************************************
* @purpose  Set the double vlan tag mode on the interface
*          
* @param    unitIndex  @b{(input)} Unit of this device
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    mode       @b{(input)} Mode for this interface
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments mode: L7_ENABLE or L7_DISABLE
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDvlantagIntfModeSet(L7_uint32 unit,
								 L7_uint32 intIfNum,
								 L7_uint32 mode)
{

  return dvlantagIntfModeSet(intIfNum, mode);
}
/*********************************************************************
* @purpose  Get the double vlan tag mode on the interface
*          
* @param    unitIndex  @b{(input)} Unit of this device
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    *mode      @b{(input)} Mode for this interface
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments mode: L7_ENABLE or L7_DISABLE
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDvlantagIntfModeGet(L7_uint32 unit,
								 L7_uint32 intIfNum,
								 L7_uint32 *mode)
{

  return dvlantagIntfModeGet(intIfNum, mode);
}
/*********************************************************************
* @purpose  Set the ethertype for the double vlan tagg for this interface
*          
* @param    unitIndex  @b{(input)} Unit of this device
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    etherType  @b{(input)} ethertype for this interface
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* @returns  L7_NOT_EXIST  if the specified etherType is not yet configured
*                         globally.
*
* @comments etherType: 0x0001 - 0xFFFF
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDvlantagIntfEthertypeSet(L7_uint32 unit,
								      L7_uint32 intIfNum,
								      L7_uint32 etherType,
								      L7_BOOL etherTypeSet)
{
  
  
  return dvlantagIntfEthertypeSet(intIfNum, etherType, etherTypeSet,L7_TRUE);
}
/*********************************************************************
* @purpose  Get the ethertype for the double vlan tagg for this interface
*          
* @param    unitIndex  @b{(input)} Unit of this device
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    *etherType @b{(input)} ethertype for this interface
* @param    tpidIdx       @b{(input)} TPID Index
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments etherType: 0x0000 - 0xFFFF
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDvlantagIntfEthertypeGet(L7_uint32 unit,
								      L7_uint32 intIfNum,
								      L7_uint32 *etherType,
								      L7_uint32 tpidIdx)
{
  
  
  return dvlantagIntfEthertypeGet(intIfNum, etherType, tpidIdx);
}

/*********************************************************************
* @purpose  Set/Clear the global ethertype
*          
* @param    unitIndex     @b{(input)} Unit of this device
* @param    etherType     @b{(input)} ethertype for this interface
* @param    primaryTpid   @b{(input)} Is this the primary TPID
* @param    etherTypeSet  @b{(input)} Set /Clear the TPID value
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments etherType: 0x0000 - 0xFFFF
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDvlantagEthertypeSet(L7_uint32 unit,
								      L7_uint32 etherType,
								      L7_BOOL primaryTpid,
								      L7_BOOL etherTypeSet)
{
  
  
  return dvlantagEthertypeSet(etherType, primaryTpid, etherTypeSet);
}
/*********************************************************************
* @purpose  Get the global ethertype
*          
* @param    unitIndex     @b{(input)} Unit of this device
* @param    etherType     @b{(input)} ethertype for this interface
* @param    defaultTpid   @b{(input)} Is this the default TPID
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments etherType: 0x0000 - 0xFFFF
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDvlantagEthertypeGet(L7_uint32 unit,
								      L7_uint32 *etherType,
								      L7_uint32 tpidIdx)
{
  
  
  return dvlantagEthertypeGet(etherType, tpidIdx);
}

/*********************************************************************
* @purpose  Set the customer ID for double vlan tagging for this interface
*          
* @param    unitIndex  @b{(input)} Unit of this device
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    custId     @b{(input)} Customer ID for this interface
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments custId: 0 - 4095
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDvlantagIntfCustIdSet(L7_uint32 unit,
								   L7_uint32 intIfNum,
								   L7_uint32 custId)
{
  
  
  return dvlantagIntfCustIdSet(intIfNum, custId);
}
/*********************************************************************
* @purpose  Get the customer ID for double vlan tagging for this interface
*          
* @param    unitIndex  @b{(input)} Unit of this device
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    *custId    @b{(input)} Customer ID for this interface
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments custId: 0 - 4095
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDvlantagIntfCustIdGet(L7_uint32 unit,
								   L7_uint32 intIfNum,
								   L7_uint32 *custId)
{
  
  
  return dvlantagIntfCustIdGet(intIfNum, custId);
}
/*********************************************************************
* @purpose  Checks is dvlan configuration can be configued to a given interface 
*          
* @param    intIfNum  @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  On a valid interface
* @returns  L7_FAILURE  On an invalid interface
*
* @comments Does not mean that this configuration will be applied.
*           For configuration to be applied the interface in addition 
*           to this check, must not an active lag member, probe intf, 
*           router port etc.
*
*
* @end
*********************************************************************/
L7_RC_t usmDbDvlantagIntfValidCheck(L7_uint32 intIfNum)
{
  return dvlantagIntfValidCheck(intIfNum);
}
