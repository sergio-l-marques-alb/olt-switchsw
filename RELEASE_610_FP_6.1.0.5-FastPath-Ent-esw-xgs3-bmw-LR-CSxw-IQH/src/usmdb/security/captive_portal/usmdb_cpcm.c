/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename  usmdb_cpcm.c
*
* @purpose      Captive Portal Client Credential Manager (CPCM)
*
* @component    CPCM
*
* @comments     none
*
* @create       08/05/2007
*
* @author       darsenault
*
* @end
*
*********************************************************************/

#include "l7_common.h"
#include "usmdb_cpcm_api.h"
#include "intf_cb_api.h"
#include "cpcm_api.h"

/*********************************************************************
*
* @purpose  Authenticate CP user credentials (local or radius)
*
* @param    L7_uint32               intfId   @b{(input)} interface id
* @param    time_t                  cTime    @b{(input)} connection time
* @param    L7_char8                *uid     @b{(input)} user id
* @param    L7_char8                *pwd     @b{(input)} password
* @param    CP_AUTH_STATUS_FLAG_t   *flag    @b{(output)} status flag
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpcmUserAuthRequest(cpUserAuth_t *user, CP_AUTH_STATUS_FLAG_t *flag)
{
  return cpcmUserAuthRequest(user,flag);
}

/*********************************************************************
*
* @purpose  Retrieve interface ID for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_enetMacAddr_t * intfId  @b{(output)} I/F ID
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpcmAIPStatusIntfIdGet(L7_IP_ADDR_t ipAddr, L7_uint32 * pIntfId)
{
  return cpcmAIPStatusIntfIdGet(ipAddr,pIntfId);
}

/*********************************************************************
*
* @purpose  Set client connection port for existing IP address
*           with authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     port    @b{(output)} client port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpcmAIPStatusPortSet(L7_IP_ADDR_t ipAddr, L7_uint32 port)
{
  return cpcmAIPStatusPortSet(ipAddr, port);
}

/*********************************************************************
*
* @purpose  Retrieve client connection port for existing IP address
*           with authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_uint32     * port  @b{(output)} client port
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpcmAIPStatusPortGet(L7_IP_ADDR_t ipAddr, L7_uint32 * port)
{
  return cpcmAIPStatusPortGet(ipAddr, port);
}

/*********************************************************************
*
* @purpose  Check for existing IP address with authorization in 
*           progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpcmAIPStatusGet(L7_IP_ADDR_t ipAddr)
{
  return cpcmAIPStatusGet(ipAddr);
}

/*********************************************************************
*
* @purpose  Check for NEXT IP address with authorization in 
*           progress, given a current IP address
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    L7_IP_ADDR_t  *nextIpAddr  @b{(output)} IP addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpcmAIPStatusNextGet(L7_IP_ADDR_t ipAddr, L7_IP_ADDR_t * nextIpAddr)
{
  return cpcmAIPStatusNextGet(ipAddr,nextIpAddr);
}

/*********************************************************************
*
* @purpose  Delete  authorization in progress table entry
*
* @param    L7_IP_ADDR_t      ipAddr  @b{(input)} IP addr
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpcmAIPStatusDelete(L7_IP_ADDR_t ipAddr)
{
  return cpcmAIPStatusDelete(ipAddr);
}

/*********************************************************************
*
* @purpose  Retrieve connection time for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr  @b{(input)} IP addr
* @param    time_t   *  connTime  @b{(output)} connection time
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpcmAIPStatusConnTimeGet(L7_IP_ADDR_t ipAddr, time_t * connTime)
{
  return cpcmAIPStatusConnTimeGet(ipAddr, connTime);
}

/*********************************************************************
*
* @purpose  Retrieve MAC address for existing IP address with 
*           authorization in progress
*
* @param    L7_IP_ADDR_t  ipAddr   @b{(input)} IP addr
* @param    L7_enetMacAddr_t *mac  @b{(output)} MAC address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @comments none
*
* @end
*
*********************************************************************/
L7_RC_t usmDbCpcmAIPStatusMacAddrGet(L7_IP_ADDR_t ipAddr, L7_enetMacAddr_t * mac)
{
  return cpcmAIPStatusMacAddrGet(ipAddr, mac);
}
