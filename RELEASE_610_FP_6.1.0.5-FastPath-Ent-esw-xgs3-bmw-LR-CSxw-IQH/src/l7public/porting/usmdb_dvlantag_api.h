/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  usmdb_dvlantag_api.h
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

#ifndef USMDB_DVLANTAG_API_H
#define USMDB_DVLANTAG_API_H
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
extern L7_RC_t usmDbDvlantagIntfModeSet(L7_uint32 unit,
								        L7_uint32 intIfNum,
								        L7_uint32 mode);
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
extern L7_RC_t usmDbDvlantagIntfModeGet(L7_uint32 unit,
								        L7_uint32 intIfNum,
								        L7_uint32 *mode);
/*********************************************************************
* @purpose  Set the ethertype for the double vlan tagg for this interface
*          
* @param    unitIndex  @b{(input)} Unit of this device
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    etherType  @b{(input)} ethertype for this interface
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments etherType: 0x0000 - 0xFFFF
*
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDvlantagIntfEthertypeSet(L7_uint32 unit,
								             L7_uint32 intIfNum,
								             L7_uint32 etherType,
								             L7_BOOL etherTypeSet);
/*********************************************************************
* @purpose  Get the ethertype for the double vlan tagg for this interface
*          
* @param    unitIndex  @b{(input)} Unit of this device
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    *etherType @b{(input)} ethertype for this interface
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
*
* @comments etherType: 0x0000 - 0xFFFF
*
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDvlantagIntfEthertypeGet(L7_uint32 unit,
								             L7_uint32 intIfNum,
								             L7_uint32 *etherType,
        							       L7_uint32 tpidIdx);

/*********************************************************************
* @purpose  Set the ethertype for the double vlan tagg for this interface
*          
* @param    unitIndex     @b{(input)} Unit of this device
* @param    etherType     @b{(input)} ethertype for this interface
* @param    defaultTpid   @b{(input)} Is this the default TPID
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
								      L7_BOOL defaultTpid,
								      L7_BOOL etherTypeSet);
								      
/*********************************************************************
* @purpose  Get the ethertype for the double vlan tagg for this interface
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
								      L7_uint32 tpidIdx);
								             
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
extern L7_RC_t usmDbDvlantagIntfCustIdSet(L7_uint32 unit,
								          L7_uint32 intIfNum,
								          L7_uint32 custId);
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
extern L7_RC_t usmDbDvlantagIntfCustIdGet(L7_uint32 unit,
								          L7_uint32 intIfNum,
								          L7_uint32 *custId);
/*********************************************************************
* @purpose  Checks is dvlan configuration can be configued to a given interface 
*          
* @param    intIfNum  @b{(input)} Internal Interface Number
*
* @returns  L7_SUCCESS  On a valid interface
* @returns  L7_FAILURE  On an invalid interface
*
* @comments Does not mean that this configuration will be applied.
*           For configuration to be applied the interface must not
*           active lag member, probe intf, router port etc.
*
*
* @end
*********************************************************************/
extern L7_RC_t usmDbDvlantagIntfValidCheck(L7_uint32 intIfNum);


#endif/*USMDB_DVLANTAG_API_H*/
