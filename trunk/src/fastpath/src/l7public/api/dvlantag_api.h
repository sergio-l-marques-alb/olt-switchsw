/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dvlantag_api.h
*
* @purpose   API's for Double Vlan Tagging
*
* @component dvlantag
*
* @component switching
*
* @create    08/20/2003
*
* @author    skalyanam
*
* @end
**********************************************************************/   

#ifndef DVLANTAG_API_H
#define DVLANTAG_API_H

#include "l7_common.h"

/*********************************************************************
* @purpose  Set the double vlan tag mode on the interface
*          
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
extern L7_RC_t dvlantagIntfModeSet(L7_uint32 intIfNum, L7_uint32 mode);
/*********************************************************************
* @purpose  Get the double vlan tag mode on the interface
*          
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
extern L7_RC_t dvlantagIntfModeGet(L7_uint32 intIfNum, L7_uint32 *mode);
/*********************************************************************
* @purpose  Set/Clear the GLOBAL ethertype
*          
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
L7_RC_t dvlantagEthertypeSet(L7_uint32 etherType, L7_BOOL primaryTpid, L7_BOOL etherTypeSet);

/*********************************************************************
* @purpose  Get the GLOBAL ethertype based on TPID Index
*          
* @param    etherType     @b{(input)} ethertype for this interface
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
L7_RC_t dvlantagEthertypeGet(L7_uint32 *etherType, L7_uint32 tpidIdx);

/*********************************************************************
* @purpose  Set the ethertype for the double vlan tagg for this interface
*          
* @param    intIfNum   @b{(input)} Internal Interface Number
* @param    etherType  @b{(input)} ethertype for this interface
* @param    etherTypeSet  @b{(input)} ethertype Enable/Disable Flag
* @param    ownLock       @b{(input)} Own the semaphore
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE  
* @returns  L7_NOT_EXIST  if the specified etherType is not yet configured
*                         globally.
*
* @comments etherType: 0x0001 - 0xFFFF
* @Note: This function is called from UI routines and internal functions.
*        For internal function calls we should not take the semaphore.
*        ownLock flag is FALSE for internal functions.  
*
*
* @end
*********************************************************************/
extern L7_RC_t dvlantagIntfEthertypeSet(L7_uint32 intIfNum, L7_uint32 etherType, 
                                        L7_BOOL etherTypeSet,
                                        L7_BOOL ownLock);
/*********************************************************************
* @purpose  Get the ethertype for the double vlan tagg for this interface
*          
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
extern L7_RC_t dvlantagIntfEthertypeGet(L7_uint32 intIfNum, L7_uint32 *etherType, L7_uint32 tpidIdx);
/*********************************************************************
* @purpose  Set the customer ID for double vlan tagging for this interface
*          
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
extern L7_RC_t dvlantagIntfCustIdSet(L7_uint32 intIfNum, L7_uint32 custId);
/*********************************************************************
* @purpose  Get the customer ID for double vlan tagging for this interface
*          
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
extern L7_RC_t dvlantagIntfCustIdGet(L7_uint32 intIfNum, L7_uint32 *custId);

/*********************************************************************
* @purpose  Checks is dvlan configuration can be configued to a given interface type
*
* @param    intfType  @b{(input)} interface type
*
* @returns  L7_SUCCESS  On a valid interface
* @returns  L7_FAILURE  On an invalid interface
*
* @comments Does not mean that this configuration will be applied.
*           For configuration to be applied the interface must not
*           active lag member, probe intf, router port etc.
*
* @end
*********************************************************************/
extern L7_RC_t dvlantagIsValidIntfType(L7_uint32 intfType);

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
extern L7_RC_t dvlantagIntfValidCheck(L7_uint32 intIfNum);

#endif/*DVLANTAG_API_H*/
