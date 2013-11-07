/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* @filename   nim_api.c
*
* @purpose    Interface management API for all other components
*
* @component  NIM
*
* @comments   none
*
* @create     08/01/2009
*
* @author     krevathi
* @end
*
**********************************************************************/

#include "nimapi.h"
/*********************************************************************
*
* @purpose  Define the system ifName,ifNameLong and ifDescr for the specified interface
*
* @param    configId        @b{(input)} NIM configID for the interface
* @param    configId        @b{(input)} NIM configID for the interface
* @param    *ifName         @b{(output)}  Ptr to buffer to contain name
* @param    *ifNameDescr    @b{(output)}  Ptr to buffer to contain description
* @param    *ifNameLong     @b{(output)}  Ptr to buffer to contain long format name
*
* @returns  L7_SUCCESS  if success
* @returns  L7_ERROR    if interface does not exist
* @returns  L7_FAILURE  if other failure
*
* @notes    Buffer must be of size L7_NIM_IFNAME_SIZE
*           and L7_NIM_INTF_DESCR_SIZE, respectively
*
* @notes    "This routine provides a hook for customization of the ifName and ifDescr 
*            of a specific interface type. A valid ifName and ifDescr are expected to 
*            be passed in by the component.If nimIfDescrInfoSet() is overridden by a 
*            replacement routine,the replacement routine must be used to modify these
*            parameters for interfaces which must be customized for the product."
*
*         
*
* @end
*********************************************************************/
void nimIfDescrInfoSet(nimConfigID_t *configId, L7_IANA_INTF_TYPE_t ianaType,
                          L7_uchar8 *ifName,L7_uchar8 *ifDescr,L7_uchar8 *ifNameLong)
{
  return;
}

