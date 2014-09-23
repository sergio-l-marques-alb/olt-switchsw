/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename trap_layer3.c
*
* @purpose Trap Manager Layer 3 functions
*
* @component trap_layer3.c
*
* @comments none
*
* @created 04/17/2001
*
* @author kdesai
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include <stdio.h>
#include "l7_common.h"
#include "l3_commdefs.h"
#include "trapapi.h"
#include "trap.h"
#include "trap_layer3_api.h"
#include "trapstr_layer3.h"
#include "usmdb_trap_layer3_api.h"
#include "usmdb_snmp_trap_api_l3.h"
#include "usmdb_snmp_api.h"
#include "usmdb_util_api.h"
#include "usmdb_sim_api.h"
#include "l7_ospf_api.h"

#ifdef L7_IPV6_PACKAGE
#include "l7_ospfv3_api.h"
#include "osapi_support.h"
#include "usmdb_sim_api.h"
#include "usmdb_snmp_api.h"
#include "usmdb_snmp_trap_api_l3.h"
#include "usmdb_trap_layer3_api.h"
#include "usmdb_util_api.h"
#endif

extern trapMgrCfgData_t trapMgrCfgData;
extern trapMgrTrapData_t trapMgrTrapData;

/* Max length of the name of an interface, either as u/s/p or s/p */
#define TM_IF_NAME_LEN 20

/*********************************************************************
* @purpose  Returns Trap Manager's trap VRRP state  
*
* @param    *mode     L7_ENABLE/L7_DISABLE
* @param    trapType  VRRP trap type specified
*
* @returns  L7_SUCCESS
*           
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t trapMgrGetTrapVrrp(L7_uint32 *mode, L7_uint32 trapType)
{
  if(trapMgrCfgData.trapVrrp & trapType)
  {
    *mode = L7_ENABLE;
  }
  else
    *mode = L7_DISABLE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets Trap Manager's VRRP state  
*
*
* @param    mode      L7_ENABLE/L7_DISABLE
* @param    trapType  VRRP trap type specified
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t trapMgrSetTrapVrrp(L7_uint32 mode, L7_uint32 trapType)
{
  if(mode == L7_ENABLE)
  {
    trapMgrCfgData.trapVrrp |= trapType;
  }
  else if(mode == L7_DISABLE)
  {
    trapMgrCfgData.trapVrrp &= ~trapType;
  }
  else 
    return L7_FAILURE;

  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}

#if defined(L7_OSPF_PACKAGE) || defined (L7_IPV6_PACKAGE)
/*********************************************************************
*
* @purpose  Get the name of an interface in the form u/s/p or s/p. The
*           first format is used if stacking is included in the build.
*
* @param    usp {(@binput}) The usp of the interface
* @param    ifName {(@boutput)} The interface name 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE if ifName is NULL
*
* @notes    ifName must point to a buffer at least TM_IF_NAME_LEN long.
*
* @end
*********************************************************************/
static L7_RC_t trapMgrIfNameGet(nimUSP_t *usp, L7_uchar8 *ifName, L7_uint32 ifNameLen)
{
    if (ifName == L7_NULLPTR)
    {
        return L7_FAILURE;
    }
#ifdef L7_STACKING_PACKAGE
    osapiSnprintf(ifName, ifNameLen, "%u/%u/%u", (L7_uint32) usp->unit, 
            (L7_uint32) usp->slot, (L7_uint32) usp->port);
#else
    osapiSnprintf(ifName, ifNameLen, "%u/%u", (L7_uint32) usp->slot, (L7_uint32) usp->port);
#endif

    return L7_SUCCESS;
}
#endif

#ifdef L7_OSPF_PACKAGE              

/*********************************************************************
*
* @purpose  Allows a user to enable or disable traps based on the
*           specified mode.
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  ospf trap type specified in L7_OSPF_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfTrapModeSet(L7_uint32 mode, L7_uint32 trapType)
{
  if(mode == L7_ENABLE)
  {
    trapMgrCfgData.trapOspf |= trapType;
  }
  else if(mode == L7_DISABLE)
  {
    trapMgrCfgData.trapOspf &= ~(trapType);
  }
  else 
    return L7_FAILURE;

/* @p1117 start */
  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
/* @p1117 end */

  return ospfMapTrapModeSet(trapMgrCfgData.trapOspf);
}

/*********************************************************************
*
* @purpose  Allows a user to determine whether the specified trap
*           is enabled or disabled
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  ospf trap type specified in L7_OSPF_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfTrapModeGet(L7_uint32 *mode, L7_uint32 trapType)
{
  if(trapMgrCfgData.trapOspf & trapType)
  {
    *mode = L7_ENABLE;
  }
  else
    *mode = L7_DISABLE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Allows a user to determine configured ospf trap flags
*
* @param    trapFlags      bit mask indicating ospf traps that are 
*                          enabled
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfTrapFlagsGet(L7_uint32 *trapFlags)
{
  *trapFlags = trapMgrCfgData.trapOspf;
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose  Signifies a change in the state of an OSPF virtual interface.
*
* @param  Routerid        The ip address of the router originating the trap.
*         VirtIfAreaId    The area ID of the Virtual Interface to which
*                         this RTO is attached.
*         VirtIfNeighbor  The Neighbor Router ID to which this interface
*                         is connected.
*         VirtIfState     The Virtual Interface Hello protocol State machine state.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This trap is generated when the interface state regresses or 
*         progresses to a terminal state. 
*
* @end
*********************************************************************/
L7_RC_t trapMgrVirtIfStateChange(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor, L7_uint32 virtIfState)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 nbrBuf[16];
  L7_char8 routerIDBuf[16];
  L7_char8 areaIDBuf[16];
  L7_char8 stateBuf[28];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)nbrBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)areaIDBuf, 16);
  bzero((L7_char8 *)stateBuf, 28);

  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(virtIfAreaId, areaIDBuf);
    usmDbInetNtoa(virtIfNeighbor, nbrBuf);

    switch (virtIfState)
    {
    case L7_OSPF_INTF_DOWN:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_down);
      break;
    case L7_OSPF_INTF_LOOPBACK:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_loopback);
      break;
    case L7_OSPF_INTF_WAITING:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_waiting);
      break;
    case L7_OSPF_INTF_POINTTOPOINT:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_pointtopoint);
      break;
    case L7_OSPF_INTF_DESIGNATEDROUTER:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_desig_rtr);
      break;
    case L7_OSPF_INTF_BACKUPDESIGNATEDROUTER:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_bkup_desig_rtr);
      break;
    case L7_OSPF_INTF_OTHERDESIGNATEDROUTER:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_other_desig_rtr);
      break;
    default:  
      osapiSnprintf(stateBuf, sizeof(stateBuf), defaultvalue);
      break;
    }

    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), virtIfStateChange_str, routerIDBuf, areaIDBuf, nbrBuf, stateBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_VIRTUAL_INTERFACE_CHANGE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfVirtIfStateChangeTrapSend(unit, routerId, virtIfAreaId, 
                                                  virtIfNeighbor, virtIfState);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies a change in the state of a non-virtual OSPF neighbor.
*
* @param    routerId    The ip address of the router originating the trap.
*           nbrIpAddr   The IP address this neighbor is using as this 
*                       IP Source Address.
*           intIfNum    On an interface having and IP Address, "0".  
*                                On an interface without an address, the 
*                                corresponding value of ifIndex for this
*                                interface in the Internet Standard MIB 
*                                ifTable.  This index is usually used in 
*                                an unnumbered interface as a reference to
*                                the local router's control application 
*                                interface table.
*           nbrRtrId     A 32-bit integer uniquely indentifying the neighboring
*                        router in the AS (Autonomous System).
*           nbrState     The new NBR state.  NBR states are defined in the 
*                        OSPF MIB.(pg. 126)
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This trap is generated when the neighbor state regresses or 
*         progresses to a terminal state.  When a neighbor transitions 
*         from or to Full on non-broadcast multi-access and broadcast 
*         networks, the trap should be generated by the designated router.
*         A designated router transitioning to Down will be noted by 
*         ospfIfStateChange. 
*
* @end
*********************************************************************/
L7_RC_t trapMgrNbrStateChange(L7_uint32 routerId, L7_uint32 nbrIpAddr,
                              L7_uint32 intIfNum,
                              L7_uint32 nbrRtrId, L7_uint32 nbrState)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_char8 nbrBuf[16];
  L7_char8 routerIDBuf[16];
  L7_char8 nbrRtrBuf[16];
  L7_char8 stateBuf[28];
  L7_char8 ifName[TM_IF_NAME_LEN];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)nbrBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)nbrRtrBuf, 16);
  bzero((L7_char8 *)stateBuf, 28);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_NBR_STATE_CHANGE)
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    if (trapMgrIfNameGet(&usp, ifName, sizeof(ifName)) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(nbrRtrId, nbrRtrBuf);
    usmDbInetNtoa(nbrIpAddr, nbrBuf);
    
    switch (nbrState)
    {
    case L7_OSPF_DOWN:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_down);
      break;
    case L7_OSPF_ATTEMPT:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_attempt);
      break;
    case L7_OSPF_INIT:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_init);
      break;
    case L7_OSPF_TWOWAY:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_twoway);
      break;
    case L7_OSPF_EXCHANGESTART:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_exchangestart);
      break;
    case L7_OSPF_EXCHANGE:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_exchange);
      break;
    case L7_OSPF_LOADING:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_loading);
      break;
    case L7_OSPF_FULL:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_full);
      break;
    default:  
      osapiSnprintf(stateBuf, sizeof(stateBuf), defaultvalue);
      break;
    }

    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), nbrStateChange_str, ifName, routerIDBuf,nbrBuf,nbrRtrBuf,stateBuf );
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NEIGHBOR_STATE_CHANGE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfNbrStateChangeTrapSend(unit, routerId, nbrIpAddr, 
                                               intIfNum, nbrRtrId, nbrState);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that there  has been a change in the state of a 
*           virtual OSPF neighbor.
*
* @param    routerId      The originator of the trap
*           virtNbrArea
*           virtNbrRtrId
*           virtNbrState  The new state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   This  trap  should  be generated  when  the  neighbor
*          state regresses (e.g., goes from Attempt or Full 
*          to  1-Way  or Down)  or progresses to a terminal 
*          state (e.g., 2-Way or Full).  When an  neighbor 
*          transitions from  or  to Full on non-broadcast
*          multi-access and broadcast networks, the trap should
*          be generated  by the designated router.  A designated      
*          router transitioning to Down will be  noted  by      
*          ospfIfStateChange." 
*
* @end
*********************************************************************/
L7_RC_t trapMgrVirtNbrStateChange(L7_uint32 routerId, L7_uint32 virtNbrArea,
                                  L7_uint32 virtNbrRtrId, L7_uint32 virtNbrState)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 nbrBuf[16];
  L7_char8 nbrRtrBuf[16];
  L7_char8 stateBuf[28];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)nbrBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)nbrRtrBuf, 16);
  bzero((L7_char8 *)stateBuf, 28);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE)
  {
    
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(virtNbrRtrId, nbrRtrBuf);
    usmDbInetNtoa(virtNbrArea, nbrBuf);

    switch (virtNbrState)
    {
    case L7_OSPF_VIRT_NBR_STATE_DOWN:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_down);
      break;
    case L7_OSPF_VIRT_NBR_STATE_ATTEMPT:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_attempt);
      break;
    case L7_OSPF_VIRT_NBR_STATE_INIT:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_init);
      break;
    case L7_OSPF_VIRT_NBR_STATE_TWOWAY:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_twoway);
      break;
    case L7_OSPF_VIRT_NBR_STATE_EXCHANGE_START:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_exchangestart);
      break;
    case L7_OSPF_VIRT_NBR_STATE_EXCHANGE:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_exchange);
      break;
    case L7_OSPF_VIRT_NBR_STATE_LOADING:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_loading);
      break;
    case L7_OSPF_VIRT_NBR_STATE_FULL:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_full);
      break;
    default:  
      osapiSnprintf(stateBuf, sizeof(stateBuf), defaultvalue);
      break;
    }
    
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), virtNbrStateChange_str, routerIDBuf, nbrBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NEIGHBOR_STATE_CHANGE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfVirtNbrStateChangeTrapSend(unit, routerId, virtNbrArea, 
                                                   virtNbrRtrId, virtNbrState);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that a packet has been received on a non-virtual 
*           interface from a router whose configuration parameters  conflict  
*           with this router's configuration parameters.
*
* @param    routerId          The originator of the trap.
*           ifIpAddress
*           intIfNum          internal interface number
*           packetSrc         The source IP Address
*           configErrorType   Type of error
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Note that the event optionMismatch should cause a trap only
*          if it prevents an adjacency from forming. 
*
* @end
*********************************************************************/
L7_RC_t trapMgrIfConfigError(L7_uint32 routerId, L7_uint32 ifIpAddress,
                             L7_uint32 intIfNum, L7_uint32 packetSrc,
                             L7_uint32 configErrorType, L7_uint32 packetType)
{
  L7_RC_t rc = L7_SUCCESS;
  nimUSP_t usp;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 ifIpAddrBuf[16];
  L7_char8 packetSrcBuf[16];
  L7_char8 routerIDBuf[16];
  L7_char8 errorTypeBuf[28];
  L7_char8 pckTypeBuf[28];
  L7_char8 ifName[TM_IF_NAME_LEN];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)ifIpAddrBuf, 16);
  bzero((L7_char8 *)packetSrcBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)errorTypeBuf, 28);
  bzero((L7_char8 *)pckTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_IF_CONFIG_ERROR)
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    if (trapMgrIfNameGet(&usp, ifName, sizeof(ifName)) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(ifIpAddress, ifIpAddrBuf);
    usmDbInetNtoa(packetSrc, packetSrcBuf);

    switch (configErrorType)
    {
    case L7_BAD_VERSION:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), bad_version);
      break;
    case L7_AREA_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), area_mismatch);
      break;
    case L7_UNKNOWN_NBMA_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_nbma_nbr);
      break;
    case L7_UNKNOWN_VIRTUAL_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_virtual_nbr);
      break;
    case L7_AUTH_TYPE_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_type_mismatch);
      break;
    case L7_AUTH_FAILURE:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_failure);
      break;
    case L7_NET_MASK_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), net_mask_mismatch);
      break;
    case L7_HELLO_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), hello_interval_mismatch);
      break;
    case L7_DEAD_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), dead_interval_mismatch);
      break;
    case L7_OPTION_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), option_mismatch);
      break;
    default:  
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), defaultvalue);
      break;
    }

    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }
    
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), ifConfigError_str, ifName, routerIDBuf, ifIpAddrBuf, packetSrcBuf,errorTypeBuf,pckTypeBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_INTERFACE_CONFIG_ERROR;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfIfConfigErrorTrapSend(unit, routerId, ifIpAddress, 
                                              intIfNum, packetSrc, configErrorType, packetType);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that a packet has been received on a virtual 
*           interface from a router whose configuration parameters  conflict  
*           with this router's configuration parameters.
*
* @param    routerId        The originator of the Trap
*           virtIfAreaId
*           virtIfNeighbor
*           configErrorType Type of error
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Note that the event optionMismatch should cause a trap only
*          if it prevents an adjacency from forming. 
*
* @end
*********************************************************************/
L7_RC_t trapMgrVirtIfConfigError(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor,
                                 L7_uint32 configErrorType,
                                 L7_uint32 packetType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 virtIfAreaIdBuf[16];
  L7_char8 virtIfNeighborBuf[16];
  L7_char8 errorTypeBuf[28];
  L7_char8 pckTypeBuf[28];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)virtIfAreaIdBuf, 16);
  bzero((L7_char8 *)virtIfNeighborBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)errorTypeBuf, 28);
  bzero((L7_char8 *)pckTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(virtIfAreaId, virtIfAreaIdBuf);
    usmDbInetNtoa(virtIfNeighbor, virtIfNeighborBuf);

    switch (configErrorType)
    {
    case L7_BAD_VERSION:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), bad_version);
      break;
    case L7_AREA_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), area_mismatch);
      break;
    case L7_UNKNOWN_NBMA_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_nbma_nbr);
      break;
    case L7_UNKNOWN_VIRTUAL_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_virtual_nbr);
      break;
    case L7_AUTH_TYPE_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_type_mismatch);
      break;
    case L7_AUTH_FAILURE:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_failure);
      break;
    case L7_NET_MASK_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), net_mask_mismatch);
      break;
    case L7_HELLO_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), hello_interval_mismatch);
      break;
    case L7_DEAD_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), dead_interval_mismatch);
      break;
    case L7_OPTION_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), option_mismatch);
      break;
    default:  
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), defaultvalue);
      break;
    }

    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), virtIfConfigError_str, routerIDBuf,virtIfAreaIdBuf,virtIfNeighborBuf,errorTypeBuf, pckTypeBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_VIRTUAL_INTERFACE_CONFIG_ERROR;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfVirtIfConfigErrorTrapSend(unit, routerId, virtIfAreaId, 
                                                  virtIfNeighbor, configErrorType, packetType);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Signifies that a packet has been received on a non-virtual
*          interface from a router whose authentication key or 
*          authentication type conflicts with this router's 
*          authentication key or authentication type.
*
* @param   routerId         The originator of the trap.
*          ifIpAddress
*          intIfNum         internal interface number
*          packetSrc        The source IP Address
*          configErrorType  authType mismatch or auth failure
*          packetType 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t trapMgrIfAuthFailure(L7_uint32 routerId, L7_uint32 ifIpAddress,
                             L7_uint32 intIfNum, L7_uint32 packetSrc,
                             L7_uint32 configErrorType, L7_uint32 packetType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_char8 ifIpAddrBuf[16];
  L7_char8 packetSrcBuf[16];
  L7_char8 routerIDBuf[16];
  L7_char8 errorTypeBuf[28];
  L7_char8 pckTypeBuf[28];
  L7_char8 ifName[TM_IF_NAME_LEN];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)ifIpAddrBuf, 16);
  bzero((L7_char8 *)packetSrcBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)errorTypeBuf, 28);
  bzero((L7_char8 *)pckTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_IF_AUTH_FAILURE)
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    if (trapMgrIfNameGet(&usp, ifName, sizeof(ifName)) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(ifIpAddress, ifIpAddrBuf);
    usmDbInetNtoa(packetSrc, packetSrcBuf);
    
    switch (configErrorType)
    {
    case L7_BAD_VERSION:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), bad_version);
      break;
    case L7_AREA_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), area_mismatch);
      break;
    case L7_UNKNOWN_NBMA_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_nbma_nbr);
      break;
    case L7_UNKNOWN_VIRTUAL_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_virtual_nbr);
      break;
    case L7_AUTH_TYPE_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_type_mismatch);
      break;
    case L7_AUTH_FAILURE:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_failure);
      break;
    case L7_NET_MASK_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), net_mask_mismatch);
      break;
    case L7_HELLO_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), hello_interval_mismatch);
      break;
    case L7_DEAD_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), dead_interval_mismatch);
      break;
    case L7_OPTION_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), option_mismatch);
      break;
    default:  
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), defaultvalue);
      break;
    }

    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), ifAuthFailure_str, ifName, routerIDBuf, ifIpAddrBuf, 
            packetSrcBuf, errorTypeBuf, pckTypeBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = 
        TRAPMGR_SPECIFIC_INTERFACE_AUTHFAILURE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfIfAuthFailureTrapSend(unit, routerId, ifIpAddress,
                                              intIfNum, packetSrc, configErrorType, 
                                              packetType);
  }

  return rc;
}

/*********************************************************************
*
* @purpose Signifies that a packet has been received on a virtual
*          interface from a router whose authentication key or 
*          authentication type conflicts with this router's 
*          authentication key or authentication type.
*
* @param   routerId         The originator of the trap.
*          virtIfAreaId
*          virtIfNeighbor
*          configErrorType  Auth type mismatch or auth failure
*          packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t trapMgrVirtIfAuthFailure(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor,
                                 L7_uint32 configErrorType,
                                 L7_uint32 packetType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 virtIfAreaIdBuf[16];
  L7_char8 virtIfNeighborBuf[16];
  L7_char8 errorTypeBuf[28];
  L7_char8 pckTypeBuf[28];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)virtIfAreaIdBuf, 16);
  bzero((L7_char8 *)virtIfNeighborBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)errorTypeBuf, 28);
  bzero((L7_char8 *)pckTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE)
  {  
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(virtIfAreaId, virtIfAreaIdBuf);
    usmDbInetNtoa(virtIfNeighbor, virtIfNeighborBuf);

    switch (configErrorType)
    {
    case L7_BAD_VERSION:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), bad_version);
      break;
    case L7_AREA_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), area_mismatch);
      break;
    case L7_UNKNOWN_NBMA_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_nbma_nbr);
      break;
    case L7_UNKNOWN_VIRTUAL_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_virtual_nbr);
      break;
    case L7_AUTH_TYPE_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_type_mismatch);
      break;
    case L7_AUTH_FAILURE:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_failure);
      break;
    case L7_NET_MASK_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), net_mask_mismatch);
      break;
    case L7_HELLO_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), hello_interval_mismatch);
      break;
    case L7_DEAD_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), dead_interval_mismatch);
      break;
    case L7_OPTION_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), option_mismatch);
      break;
    default:  
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), defaultvalue);
      break;
    }

    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), virtIfAuthFailure_str, routerIDBuf,virtIfAreaIdBuf,virtIfNeighborBuf,errorTypeBuf, pckTypeBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_VIRTUAL_INTERFACE_AUTHFAILURE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfVirtIfAuthFailureTrapSend(unit, routerId, virtIfAreaId, virtIfNeighbor, configErrorType, packetType);

  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that an OSPF packet has been received on a 
*           non-virtual interface that cannot be parsed.
*
* @param    routerId        The originator of the Trap
*           ifIpAddress
*           intIfNum        internal interface number
*           packetSrc       The source ip address
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrIfRxBadPacket(L7_uint32 routerId, L7_uint32 ifIpAddress,
                             L7_uint32 intIfNum, L7_uint32 packetSrc,
                             L7_uint32 packetType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_char8 ifIpAddrBuf[16];
  L7_char8 packetSrcBuf[16];
  L7_char8 routerIDBuf[16];
  L7_char8 pckTypeBuf[28];
  L7_char8 ifName[TM_IF_NAME_LEN];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)ifIpAddrBuf, 16);
  bzero((L7_char8 *)packetSrcBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)pckTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_RX_BAD_PACKET)
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    if (trapMgrIfNameGet(&usp, ifName, sizeof(ifName)) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(ifIpAddress, ifIpAddrBuf);
    usmDbInetNtoa(packetSrc, packetSrcBuf);
    
    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), ifRxBadPacket_str, ifName, routerIDBuf, ifIpAddrBuf, 
            packetSrcBuf, pckTypeBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = 
        TRAPMGR_SPECIFIC_RECEIVED_BADPACKET;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfIfRxBadPacketTrapSend(unit, routerId, ifIpAddress, 
                                              intIfNum, packetSrc, packetType);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that an OSPF packet has been received on a 
*           virtual interface that cannot be parsed.
*
* @param    routerId        The originator of the trap
*           virtIfAreaId
*           virtIfNeighbor
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrVirtIfRxBadPacket(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor, L7_uint32 packetType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 virtIfAreaIdBuf[16];
  L7_char8 virtIfNeighborBuf[16];
  L7_char8 pckTypeBuf[28];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)virtIfAreaIdBuf, 16);
  bzero((L7_char8 *)virtIfNeighborBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)pckTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(virtIfAreaId, virtIfAreaIdBuf);
    usmDbInetNtoa(virtIfNeighbor, virtIfNeighborBuf);

    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), virtIfRxBadPacket_str, routerIDBuf, virtIfAreaIdBuf, virtIfNeighborBuf, pckTypeBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_VIRTUAL_INTERFACE_RECEIVED_BADPACKET;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfVirtIfRxBadPacketTrapSend(unit, routerId, virtIfAreaId,virtIfNeighbor,packetType);
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies than an OSPF packet has been retransmitted on a 
*           non-virtual interface.
*
* @param    routerId        The originator of the trap
*           ifIpAddress
*           intIfNum        internal interface number
*           nbrRtrId        Destination
*           packetType
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All packets that may be re-transmitted  are associated 
*           with an LSDB entry.  The LS type, LS ID, and Router ID 
*           are used to identify the LSDB entry.
*
* @end
*********************************************************************/
L7_RC_t trapMgrTxRetransmit(L7_uint32 routerId, L7_uint32 ifIpAddress,
                            L7_uint32 intIfNum, L7_uint32 nbrRtrId,
                            L7_uint32 packetType, L7_uint32 lsdbType,
                            L7_uint32 lsdbLsid, L7_uint32 lsdbRouterId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_char8 ifIpAddrBuf[16];
  L7_char8 nbrRtrIdBuf[16];
  L7_char8 routerIDBuf[16];
  L7_char8 pckTypeBuf[28];
  L7_char8 lsdbTypeBuf[28];
  L7_char8 lsdbLsidBuf[16];
  L7_char8 lsdbRouterIdBuf[16];
  L7_char8 ifName[TM_IF_NAME_LEN];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)ifIpAddrBuf, 16);
  bzero((L7_char8 *)nbrRtrIdBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)lsdbLsidBuf, 16);
  bzero((L7_char8 *)lsdbRouterIdBuf, 16);
  bzero((L7_char8 *)pckTypeBuf, 28);
  bzero((L7_char8 *)lsdbTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_TX_RETRANSMIT)
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    if (trapMgrIfNameGet(&usp, ifName, sizeof(ifName)) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(ifIpAddress, ifIpAddrBuf);
    usmDbInetNtoa(nbrRtrId, nbrRtrIdBuf);
    usmDbInetNtoa(lsdbLsid, lsdbLsidBuf);
    usmDbInetNtoa(lsdbRouterId, lsdbRouterIdBuf);
    
    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }

    switch (lsdbType)
    {
    case L7_S_ILLEGAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_illegal_lsa);
      break;
    case L7_S_ROUTER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_router_lsa);
      break;
    case L7_S_NETWORK_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_network_lsa);
      break;
    case L7_S_IPNET_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_ipnet_summary_lsa);
      break;
    case L7_S_ASBR_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_asbr_summary_lsa);
      break;
    case L7_S_AS_EXTERNAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_external_lsa);
      break;
    case L7_S_GROUP_MEMBER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_group_member_lsa);
      break;
    case L7_S_NSSA_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp1_lsa);
      break;
    case L7_S_TMP2_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp2_lsa);
      break;
    case L7_S_LINK_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AREA_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AS_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_opaque_lsa);
      break;
    case L7_S_LAST_LSA_TYPE:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_last_lsa_type);
      break;
    default:  
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), defaultvalue);
      break;
    }
    /*may have buffer problem here */
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), txRetransmit_str, ifName, routerIDBuf, ifIpAddrBuf, 
            nbrRtrIdBuf,pckTypeBuf, lsdbTypeBuf,lsdbLsidBuf, lsdbRouterIdBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = 
        TRAPMGR_SPECIFIC_RETRANSMIT;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfTxRetransmitTrapSend(unit, routerId,ifIpAddress,intIfNum,nbrRtrId,
                           packetType,lsdbType,lsdbLsid,lsdbRouterId);
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  Signifies than an OSPF packet has been retransmitted on a 
*           virtual interface.
*
* @param    routerId        The originator of the trap
*           virtIfAreaId
*           virtIfNeighbor
*           packetType
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All packets that may be re-transmitted  are associated 
*           with an LSDB entry.  The LS type, LS ID, and Router ID 
*           are used to identify the LSDB entry.
*
* @end
*********************************************************************/
L7_RC_t trapMgrVirtTxRetransmit(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                L7_uint32 virtIfNeighbor, L7_uint32 packetType,
                                L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                                L7_uint32 lsdbRouterId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 virtIfAreaIdBuf[16];
  L7_char8 virtIfNeighborBuf[16];
  L7_char8 pckTypeBuf[28];
  L7_char8 lsdbTypeBuf[28];
  L7_char8 lsdbLsidBuf[16];
  L7_char8 lsdbRouterIdBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)virtIfAreaIdBuf, 16);
  bzero((L7_char8 *)virtIfNeighborBuf, 16);
  bzero((L7_char8 *)pckTypeBuf, 28);
  bzero((L7_char8 *)lsdbTypeBuf, 28);
  bzero((L7_char8 *)lsdbLsidBuf, 16);
  bzero((L7_char8 *)lsdbRouterIdBuf, 16);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(virtIfAreaId, virtIfAreaIdBuf);
    usmDbInetNtoa(virtIfNeighbor, virtIfNeighborBuf);
    usmDbInetNtoa(lsdbLsid, lsdbLsidBuf);
    usmDbInetNtoa(lsdbRouterId, lsdbRouterIdBuf);
    
    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }

    switch (lsdbType)
    {
    case L7_S_ILLEGAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_illegal_lsa);
      break;
    case L7_S_ROUTER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_router_lsa);
      break;
    case L7_S_NETWORK_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_network_lsa);
      break;
    case L7_S_IPNET_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_ipnet_summary_lsa);
      break;
    case L7_S_ASBR_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_asbr_summary_lsa);
      break;
    case L7_S_AS_EXTERNAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_external_lsa);
      break;
    case L7_S_GROUP_MEMBER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_group_member_lsa);
      break;
    case L7_S_NSSA_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp1_lsa);
      break;
    case L7_S_TMP2_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp2_lsa);
      break;
    case L7_S_LINK_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AREA_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AS_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_opaque_lsa);
      break;
    case L7_S_LAST_LSA_TYPE:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_last_lsa_type);
      break;
    default:  
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), defaultvalue);
      break;
    }

    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), virtTxRetransmit_str, routerIDBuf, virtIfAreaIdBuf, virtIfNeighborBuf, pckTypeBuf,lsdbTypeBuf,lsdbLsidBuf, lsdbRouterIdBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_VIRTUAL_RETRANSMIT;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfVirtTxRetransmitTrapSend(unit, routerId,virtIfAreaId,virtIfNeighbor,
                               packetType,lsdbType,lsdbLsid,lsdbRouterId);
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that a  new LSA  has  been originated by this router.
*
* @param    routerId        The originator of the trap.
*           lsdbAreaId      0.0.0.0 for AS Externals
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This trap should not be invoked for simple refreshes of  
*           LSAs  (which happesn every 30 minutes), but       
*           instead will only be invoked  when  an  LSA  is       
*           (re)originated due to a topology change.  Addi-       
*           tionally, this trap does not include LSAs  that       
*           are  being  flushed  because  they have reached       
*           MaxAge.
*
* @end      
*********************************************************************/
L7_RC_t trapMgrOriginateLsa(L7_uint32 routerId, L7_uint32 lsdbAreaId,
                            L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                            L7_uint32 lsdbRouterId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 lsdbAreaIdBuf[16];
  L7_char8 lsdbTypeBuf[28];
  L7_char8 lsdbLsidBuf[16];
  L7_char8 lsdbRouterIdBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)lsdbAreaIdBuf, 16);
  bzero((L7_char8 *)lsdbTypeBuf, 28);
  bzero((L7_char8 *)lsdbLsidBuf, 16);
  bzero((L7_char8 *)lsdbRouterIdBuf, 16);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_ORIGINATE_LSA)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(lsdbAreaId, lsdbAreaIdBuf);
    usmDbInetNtoa(lsdbLsid, lsdbLsidBuf);
    usmDbInetNtoa(lsdbRouterId, lsdbRouterIdBuf);
    
    switch (lsdbType)
    {
    case L7_S_ILLEGAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_illegal_lsa);
      break;
    case L7_S_ROUTER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_router_lsa);
      break;
    case L7_S_NETWORK_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_network_lsa);
      break;
    case L7_S_IPNET_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_ipnet_summary_lsa);
      break;
    case L7_S_ASBR_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_asbr_summary_lsa);
      break;
    case L7_S_AS_EXTERNAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_external_lsa);
      break;
    case L7_S_GROUP_MEMBER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_group_member_lsa);
      break;
    case L7_S_NSSA_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp1_lsa);
      break;
    case L7_S_TMP2_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp2_lsa);
      break;
    case L7_S_LINK_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AREA_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AS_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_opaque_lsa);
      break;
    case L7_S_LAST_LSA_TYPE:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_last_lsa_type);
      break;
    default:  
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), defaultvalue);
      break;
    }

    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), originateLsa_str, routerIDBuf, lsdbAreaIdBuf, lsdbTypeBuf, lsdbLsidBuf,lsdbRouterIdBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_ORIGINATE_LSA;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfOriginateLsaTrapSend(unit, routerId,lsdbAreaId,lsdbType,lsdbLsid,lsdbRouterId);
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that one of the LSA in the router's link-state 
*           database has aged to MaxAge.
*
* @param    routerId         The originator of the trap
*           lsdbAreaId       0.0.0.0 for AS Externals
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrMaxAgeLsa(L7_uint32 routerId, L7_uint32 lsdbAreaId,
                         L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                         L7_uint32 lsdbRouterId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 lsdbAreaIdBuf[16];
  L7_char8 lsdbTypeBuf[28];
  L7_char8 lsdbLsidBuf[16];
  L7_char8 lsdbRouterIdBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)lsdbAreaIdBuf, 16);
  bzero((L7_char8 *)lsdbTypeBuf, 28);
  bzero((L7_char8 *)lsdbLsidBuf, 16);
  bzero((L7_char8 *)lsdbRouterIdBuf, 16);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_MAX_AGE_LSA)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(lsdbAreaId, lsdbAreaIdBuf);
    usmDbInetNtoa(lsdbLsid, lsdbLsidBuf);
    usmDbInetNtoa(lsdbRouterId, lsdbRouterIdBuf);
    
    switch (lsdbType)
    {
    case L7_S_ILLEGAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_illegal_lsa);
      break;
    case L7_S_ROUTER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_router_lsa);
      break;
    case L7_S_NETWORK_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_network_lsa);
      break;
    case L7_S_IPNET_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_ipnet_summary_lsa);
      break;
    case L7_S_ASBR_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_asbr_summary_lsa);
      break;
    case L7_S_AS_EXTERNAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_external_lsa);
      break;
    case L7_S_GROUP_MEMBER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_group_member_lsa);
      break;
    case L7_S_NSSA_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp1_lsa);
      break;
    case L7_S_TMP2_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp2_lsa);
      break;
    case L7_S_LINK_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AREA_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AS_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_opaque_lsa);
      break;
    case L7_S_LAST_LSA_TYPE:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_last_lsa_type);
      break;
    default:  
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), defaultvalue);
      break;
    }

    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), maxAgeLsa_str, routerIDBuf, lsdbAreaIdBuf, lsdbTypeBuf, lsdbLsidBuf,lsdbRouterIdBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_MAX_AGE_LSA;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfMaxAgeLsaTrapSend(unit, routerId,lsdbAreaId,lsdbType,lsdbLsid,lsdbRouterId);
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that the number of LSAs in the router's link-state 
*           data-base has exceeded ospfExtLsdbLimit.
*
* @param    routerId      The originator of the trap
*           extLsdbLimit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrLsdbOverflow(L7_uint32 routerId, L7_uint32 extLsdbLimit)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)routerIDBuf, 16);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_LS_DB_OVERFLOW)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), lsdbOverflow_str, routerIDBuf, extLsdbLimit);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_LSDB_OVERFLOW;

    trapMgrLogTrapToLocalLog(trapStringBuf);
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfLsdbOverflowTrapSend(unit, routerId,extLsdbLimit);
  }
  
  return rc;
}


/*********************************************************************
*
* @purpose  Signifies that  the  number of LSAs in the router's link-
*           state database has exceeded ninety  percent  of      
*           ospfExtLsdbLimit.
*
* @param    routerId      The originator of the trap
*           extLsdbLimit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrLsdbApproachingOverflow(L7_uint32 routerId,
                                       L7_uint32 extLsdbLimit)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)routerIDBuf, 16);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), lsdbApproachingOverflow_str, routerIDBuf, extLsdbLimit);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_LSDB_APPROACHING_OVERFLOW;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfLsdbApproachingOverflowTrapSend(unit, routerId,extLsdbLimit);
  }
  
  return rc;
}


/*********************************************************************
*
* @purpose  Signifies that there has been a change in the state of a 
*           non-virtual OSPF interface.
*
* @param    routerId      The originator of the trap
*           ifIpAddress
*           intIfNum      internal interface number
*           ifState       New state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   This trap should  be  generated when  the interface state 
*          regresses (e.g., goes from Dr to Down) or progresses  to  a  terminal      
*          state  (i.e.,  Point-to-Point, DR Other, Dr, or Backup).
*
* @end
*********************************************************************/
L7_RC_t trapMgrIfStateChange(L7_uint32 routerId, L7_uint32 ifIpAddress,
                             L7_uint32 intIfNum, L7_uint32 ifState)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_char8 ifIpAddrBuf[16];
  L7_char8 routerIDBuf[16];
  L7_char8 stateBuf[28];
  L7_char8 ifName[TM_IF_NAME_LEN];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)ifIpAddrBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)stateBuf, 28);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_IF_STATE_CHANGE)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(ifIpAddress, ifIpAddrBuf);
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    if (trapMgrIfNameGet(&usp, ifName, sizeof(ifName)) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    switch (ifState)
    {
    case L7_OSPF_INTF_DOWN:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_down);
      break;
    case L7_OSPF_INTF_LOOPBACK:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_loopback);
      break;
    case L7_OSPF_INTF_WAITING:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_waiting);
      break;
    case L7_OSPF_INTF_POINTTOPOINT:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_pointtopoint);
      break;
    case L7_OSPF_INTF_DESIGNATEDROUTER:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_desig_rtr);
      break;
    case L7_OSPF_INTF_BACKUPDESIGNATEDROUTER:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_bkup_desig_rtr);
      break;
    case L7_OSPF_INTF_OTHERDESIGNATEDROUTER:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_other_desig_rtr);
      break;
    default:  
      osapiSnprintf(stateBuf, sizeof(stateBuf), defaultvalue);
      break;
    }

    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), ifStateChange_str, ifName, routerIDBuf, ifIpAddrBuf, stateBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = 
        TRAPMGR_SPECIFIC_INTERFACE_STATE_CHANGE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfIfStateChangeTrapSend(unit, routerId,ifIpAddress,intIfNum,ifState);
  }
  
  return rc;
}


/*********************************************************************
*
* @purpose  Signifies that an OSPF packet was received on a non-virtual interface. 
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t trapMgrIfRxPacket()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  
  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_IF_RX_PACKET)
  {
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), ifRxPacket_str);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_INTERFACE_RECEIVED_PACKET;

    trapMgrLogTrapToLocalLog(trapStringBuf);
/*    rc = usmDbIfRxPacket();*/
    rc = L7_FAILURE;
  }
  
  return rc;
}


/*********************************************************************
*
* @purpose  Signifies that an entry has been made in the OSPF routing table. 
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ATIC's proprietary Routing Table Entry Information trap.
*
* @end
*********************************************************************/
L7_RC_t trapMgrRtbEntryInfo()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);

  if (trapMgrCfgData.trapOspf & L7_OSPF_TRAP_RTB_ENTRY_INFO)
  {
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), rtbEntryInfo_str);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_ROUNDTABLE_ENTRY_INFO;

    trapMgrLogTrapToLocalLog(trapStringBuf);
/*    rc = usmDbRtbEntryInfo();*/
    rc = L7_FAILURE;
  }
  
  return rc;
}
#endif

#ifdef L7_VRRP_PACKAGE
/*********************************************************************
*
* @purpose  Signifies that the sending agent has transitioned to the 
*           'Master' state.
*
* @param    vrrpOperMasterIpAddr   The master router's real (primary) IP address
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t trapMgrVrrpTrapNewMasterTrap(L7_uint32 vrrpOperMasterIpAddr)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 ipAddressBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)ipAddressBuf, 16);

  if (trapMgrCfgData.trapVrrp & L7_VRRP_TRAP_NEW_MASTER)
  {
    usmDbInetNtoa(vrrpOperMasterIpAddr, ipAddressBuf);
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), vrrp_new_master_str, TRAPMGR_UNIT_INDEX, ipAddressBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_VRRP_NEW_MASTER;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc =  usmDbSnmpVrrpTrapNewMasterTrapSend(unit, vrrpOperMasterIpAddr);

  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that a packet has been received from a router
*           whose authentication key or authentication type conflicts
*           with this router's authentication key or authentication type.
*
* @param    vrrpTrapPacketSrc       The address of an inbound VRRP packet
* @param    vrrpTrapAuthErrorType   The type of configuration conflict
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t trapMgrVrrpTrapAuthFailureTrap(L7_uint32 vrrpTrapPacketSrc, L7_int32 vrrpTrapAuthErrorType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 ipAddressBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)ipAddressBuf, 16);

  if (trapMgrCfgData.trapVrrp & L7_VRRP_TRAP_AUTH_FAILURE)
  {
    usmDbInetNtoa(vrrpTrapPacketSrc, ipAddressBuf);
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), vrrp_auth_fail_str, TRAPMGR_UNIT_INDEX, ipAddressBuf, vrrpTrapAuthErrorType);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_VRRP_AUTH_FAILURE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc =  usmDbSnmpVrrpTrapAuthFailureTrapSend(unit, vrrpTrapPacketSrc, vrrpTrapAuthErrorType);

  }

  return rc;
}
#endif

#ifdef L7_IPV6_PACKAGE
/*********************************************************************
*
* @purpose  Signifies a change in the state of an OSPF virtual interface.
*
* @param  Routerid        The ip address of the router originating the trap.
*         VirtIfAreaId    The area ID of the Virtual Interface to which
*                         this RTO is attached.
*         VirtIfNeighbor  The Neighbor Router ID to which this interface
*                         is connected.
*         VirtIfState     The Virtual Interface Hello protocol State machine state.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This trap is generated when the interface state regresses or 
*         progresses to a terminal state. 
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3VirtIfStateChange(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor, L7_uint32 virtIfState)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 nbrBuf[16];
  L7_char8 routerIDBuf[16];
  L7_char8 areaIDBuf[16];
  L7_char8 stateBuf[28];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)nbrBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)areaIDBuf, 16);
  bzero((L7_char8 *)stateBuf, 28);

  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_VIRT_IF_STATE_CHANGE)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(virtIfAreaId, areaIDBuf);
    usmDbInetNtoa(virtIfNeighbor, nbrBuf);

    switch (virtIfState)
    {
    case L7_OSPF_INTF_DOWN:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_down);
      break;
    case L7_OSPF_INTF_LOOPBACK:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_loopback);
      break;
    case L7_OSPF_INTF_WAITING:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_waiting);
      break;
    case L7_OSPF_INTF_POINTTOPOINT:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_pointtopoint);
      break;
    case L7_OSPF_INTF_DESIGNATEDROUTER:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_desig_rtr);
      break;
    case L7_OSPF_INTF_BACKUPDESIGNATEDROUTER:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_bkup_desig_rtr);
      break;
    case L7_OSPF_INTF_OTHERDESIGNATEDROUTER:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_other_desig_rtr);
      break;
    default:  
      osapiSnprintf(stateBuf, sizeof(stateBuf), defaultvalue);
      break;
    }

    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), virtIfStateChange_str, routerIDBuf, areaIDBuf, nbrBuf, stateBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_VIRTUAL_INTERFACE_CHANGE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3VirtIfStateChangeTrapSend(unit, routerId, virtIfAreaId, 
                                                  virtIfNeighbor, virtIfState);
#endif
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies a change in the state of a non-virtual OSPF neighbor.
*
* @param    routerId    The ip address of the router originating the trap.
*           nbrIpAddr   The IP address this neighbor is using as this 
*                       IP Source Address.
*           intIfNum    On an interface having and IP Address, "0".  
*                                On an interface without an address, the 
*                                corresponding value of ifIndex for this
*                                interface in the Internet Standard MIB 
*                                ifTable.  This index is usually used in 
*                                an unnumbered interface as a reference to
*                                the local router's control application 
*                                interface table.
*           nbrRtrId     A 32-bit integer uniquely indentifying the neighboring
*                        router in the AS (Autonomous System).
*           nbrState     The new NBR state.  NBR states are defined in the 
*                        OSPF MIB.(pg. 126)
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes  This trap is generated when the neighbor state regresses or 
*         progresses to a terminal state.  When a neighbor transitions 
*         from or to Full on non-broadcast multi-access and broadcast 
*         networks, the trap should be generated by the designated router.
*         A designated router transitioning to Down will be noted by 
*         ospfIfStateChange. 
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3NbrStateChange(L7_uint32 routerId, L7_in6_addr_t nbrIpAddr,
                              L7_uint32 intIfNum,
                              L7_uint32 nbrRtrId, L7_uint32 nbrState)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_char8 nbrBuf[40];
  L7_char8 routerIDBuf[16];
  L7_char8 nbrRtrBuf[16];
  L7_char8 stateBuf[28];
  L7_char8 ifName[TM_IF_NAME_LEN];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)nbrBuf, 40);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)nbrRtrBuf, 16);
  bzero((L7_char8 *)stateBuf, 28);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_NBR_STATE_CHANGE)
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    if (trapMgrIfNameGet(&usp, ifName, sizeof(ifName)) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(nbrRtrId, nbrRtrBuf);
    osapiInetNtop(L7_AF_INET6, (L7_char8 *)&nbrIpAddr, nbrBuf, sizeof(nbrBuf));
    
    switch (nbrState)
    {
    case L7_OSPF_DOWN:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_down);
      break;
    case L7_OSPF_ATTEMPT:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_attempt);
      break;
    case L7_OSPF_INIT:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_init);
      break;
    case L7_OSPF_TWOWAY:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_twoway);
      break;
    case L7_OSPF_EXCHANGESTART:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_exchangestart);
      break;
    case L7_OSPF_EXCHANGE:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_exchange);
      break;
    case L7_OSPF_LOADING:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_loading);
      break;
    case L7_OSPF_FULL:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_full);
      break;
    default:  
      osapiSnprintf(stateBuf, sizeof(stateBuf), defaultvalue);
      break;
    }

    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), nbrStateChange_str, ifName, routerIDBuf,nbrBuf,nbrRtrBuf,stateBuf );
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NEIGHBOR_STATE_CHANGE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3NbrStateChangeTrapSend(unit, routerId, nbrIpAddr, 
                                               intIfNum, nbrRtrId, nbrState);
#endif
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that there  has been a change in the state of a 
*           virtual OSPF neighbor.
*
* @param    routerId      The originator of the trap
*           virtNbrArea
*           virtNbrRtrId
*           virtNbrState  The new state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   This  trap  should  be generated  when  the  neighbor
*          state regresses (e.g., goes from Attempt or Full 
*          to  1-Way  or Down)  or progresses to a terminal 
*          state (e.g., 2-Way or Full).  When an  neighbor 
*          transitions from  or  to Full on non-broadcast
*          multi-access and broadcast networks, the trap should
*          be generated  by the designated router.  A designated      
*          router transitioning to Down will be  noted  by      
*          ospfIfStateChange." 
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3VirtNbrStateChange(L7_uint32 routerId, L7_uint32 virtNbrArea,
                                  L7_uint32 virtNbrRtrId, L7_uint32 virtNbrState)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 nbrBuf[16];
  L7_char8 nbrRtrBuf[16];
  L7_char8 stateBuf[28];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)nbrBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)nbrRtrBuf, 16);
  bzero((L7_char8 *)stateBuf, 28);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_VIRT_NBR_STATE_CHANGE)
  {
    
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(virtNbrRtrId, nbrRtrBuf);
    usmDbInetNtoa(virtNbrArea, nbrBuf);

    switch (virtNbrState)
    {
    case L7_OSPF_VIRT_NBR_STATE_DOWN:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_down);
      break;
    case L7_OSPF_VIRT_NBR_STATE_ATTEMPT:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_attempt);
      break;
    case L7_OSPF_VIRT_NBR_STATE_INIT:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_init);
      break;
    case L7_OSPF_VIRT_NBR_STATE_TWOWAY:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_twoway);
      break;
    case L7_OSPF_VIRT_NBR_STATE_EXCHANGE_START:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_exchangestart);
      break;
    case L7_OSPF_VIRT_NBR_STATE_EXCHANGE:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_exchange);
      break;
    case L7_OSPF_VIRT_NBR_STATE_LOADING:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_loading);
      break;
    case L7_OSPF_VIRT_NBR_STATE_FULL:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_full);
      break;
    default:  
      osapiSnprintf(stateBuf, sizeof(stateBuf), defaultvalue);
      break;
    }
    
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), virtNbrStateChange_str, routerIDBuf, nbrBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_NEIGHBOR_STATE_CHANGE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3VirtNbrStateChangeTrapSend(unit, routerId, virtNbrArea, 
                                                   virtNbrRtrId, virtNbrState);
#endif
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that a packet has been received on a non-virtual 
*           interface from a router whose configuration parameters  conflict  
*           with this router's configuration parameters.
*
* @param    routerId          The originator of the trap.
*           ifIpAddress
*           intIfNum          internal interface number
*           packetSrc         The source IP Address
*           configErrorType   Type of error
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Note that the event optionMismatch should cause a trap only
*          if it prevents an adjacency from forming. 
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3IfConfigError(L7_uint32 routerId, L7_in6_addr_t ifIpAddress,
                             L7_uint32 intIfNum, L7_in6_addr_t packetSrc,
                             L7_uint32 configErrorType, L7_uint32 packetType)
{
  L7_RC_t rc = L7_SUCCESS;
  nimUSP_t usp;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 ifIpAddrBuf[40];
  L7_char8 packetSrcBuf[40];
  L7_char8 routerIDBuf[16];
  L7_char8 errorTypeBuf[28];
  L7_char8 pckTypeBuf[28];
  L7_char8 ifName[TM_IF_NAME_LEN];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)ifIpAddrBuf, 40);
  bzero((L7_char8 *)packetSrcBuf, 40);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)errorTypeBuf, 28);
  bzero((L7_char8 *)pckTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_IF_CONFIG_ERROR)
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    if (trapMgrIfNameGet(&usp, ifName, sizeof(ifName)) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    usmDbInetNtoa(routerId, routerIDBuf);
    osapiInetNtop(L7_AF_INET6, (L7_char8 *)&ifIpAddress, ifIpAddrBuf, sizeof(ifIpAddrBuf));
    osapiInetNtop(L7_AF_INET6, (L7_char8 *)&packetSrc, packetSrcBuf, sizeof(packetSrcBuf));

    switch (configErrorType)
    {
    case L7_BAD_VERSION:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), bad_version);
      break;
    case L7_AREA_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), area_mismatch);
      break;
    case L7_UNKNOWN_NBMA_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_nbma_nbr);
      break;
    case L7_UNKNOWN_VIRTUAL_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_virtual_nbr);
      break;
    case L7_AUTH_TYPE_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_type_mismatch);
      break;
    case L7_AUTH_FAILURE:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_failure);
      break;
    case L7_NET_MASK_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), net_mask_mismatch);
      break;
    case L7_HELLO_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), hello_interval_mismatch);
      break;
    case L7_DEAD_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), dead_interval_mismatch);
      break;
    case L7_OPTION_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), option_mismatch);
      break;
    default:  
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), defaultvalue);
      break;
    }

    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }
    
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), ifConfigError_str, ifName, routerIDBuf, ifIpAddrBuf, packetSrcBuf,errorTypeBuf,pckTypeBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_INTERFACE_CONFIG_ERROR;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3IfConfigErrorTrapSend(unit, routerId, ifIpAddress, 
                                              intIfNum, packetSrc, configErrorType, packetType);
#endif
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that a packet has been received on a virtual 
*           interface from a router whose configuration parameters  conflict  
*           with this router's configuration parameters.
*
* @param    routerId        The originator of the Trap
*           virtIfAreaId
*           virtIfNeighbor
*           configErrorType Type of error
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   Note that the event optionMismatch should cause a trap only
*          if it prevents an adjacency from forming. 
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3VirtIfConfigError(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor,
                                 L7_uint32 configErrorType,
                                 L7_uint32 packetType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 virtIfAreaIdBuf[16];
  L7_char8 virtIfNeighborBuf[16];
  L7_char8 errorTypeBuf[28];
  L7_char8 pckTypeBuf[28];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)virtIfAreaIdBuf, 16);
  bzero((L7_char8 *)virtIfNeighborBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)errorTypeBuf, 28);
  bzero((L7_char8 *)pckTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_VIRT_IF_CONFIG_ERROR)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(virtIfAreaId, virtIfAreaIdBuf);
    usmDbInetNtoa(virtIfNeighbor, virtIfNeighborBuf);

    switch (configErrorType)
    {
    case L7_BAD_VERSION:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), bad_version);
      break;
    case L7_AREA_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), area_mismatch);
      break;
    case L7_UNKNOWN_NBMA_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_nbma_nbr);
      break;
    case L7_UNKNOWN_VIRTUAL_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_virtual_nbr);
      break;
    case L7_AUTH_TYPE_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_type_mismatch);
      break;
    case L7_AUTH_FAILURE:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_failure);
      break;
    case L7_NET_MASK_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), net_mask_mismatch);
      break;
    case L7_HELLO_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), hello_interval_mismatch);
      break;
    case L7_DEAD_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), dead_interval_mismatch);
      break;
    case L7_OPTION_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), option_mismatch);
      break;
    default:  
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), defaultvalue);
      break;
    }

    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), virtIfConfigError_str, routerIDBuf,virtIfAreaIdBuf,virtIfNeighborBuf,errorTypeBuf, pckTypeBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_VIRTUAL_INTERFACE_CONFIG_ERROR;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3VirtIfConfigErrorTrapSend(unit, routerId, virtIfAreaId, 
                                                  virtIfNeighbor, configErrorType, packetType);
#endif
  }

  return rc;
}

/*********************************************************************
*
* @purpose Signifies that a packet has been received on a non-virtual
*          interface from a router whose authentication key or 
*          authentication type conflicts with this router's 
*          authentication key or authentication type.
*
* @param   routerId         The originator of the trap.
*          ifIpAddress
*          intIfNum         internal interface number
*          packetSrc        The source IP Address
*          configErrorType  authType mismatch or auth failure
*          packetType 
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3IfAuthFailure(L7_uint32 routerId, L7_in6_addr_t ifIpAddress,
                             L7_uint32 intIfNum, L7_in6_addr_t packetSrc,
                             L7_uint32 configErrorType, L7_uint32 packetType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_char8 ifIpAddrBuf[40];
  L7_char8 packetSrcBuf[40];
  L7_char8 routerIDBuf[16];
  L7_char8 errorTypeBuf[28];
  L7_char8 pckTypeBuf[28];
  L7_char8 ifName[TM_IF_NAME_LEN];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)ifIpAddrBuf, 40);
  bzero((L7_char8 *)packetSrcBuf, 40);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)errorTypeBuf, 28);
  bzero((L7_char8 *)pckTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_IF_AUTH_FAILURE)
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    if (trapMgrIfNameGet(&usp, ifName, sizeof(ifName)) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    usmDbInetNtoa(routerId, routerIDBuf);
    osapiInetNtop(L7_AF_INET6, (L7_char8 *)&ifIpAddress, ifIpAddrBuf, sizeof(ifIpAddrBuf));
    osapiInetNtop(L7_AF_INET6, (L7_char8 *)&packetSrc,packetSrcBuf,sizeof(packetSrcBuf));
    
    switch (configErrorType)
    {
    case L7_BAD_VERSION:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), bad_version);
      break;
    case L7_AREA_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), area_mismatch);
      break;
    case L7_UNKNOWN_NBMA_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_nbma_nbr);
      break;
    case L7_UNKNOWN_VIRTUAL_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_virtual_nbr);
      break;
    case L7_AUTH_TYPE_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_type_mismatch);
      break;
    case L7_AUTH_FAILURE:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_failure);
      break;
    case L7_NET_MASK_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), net_mask_mismatch);
      break;
    case L7_HELLO_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), hello_interval_mismatch);
      break;
    case L7_DEAD_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), dead_interval_mismatch);
      break;
    case L7_OPTION_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), option_mismatch);
      break;
    default:  
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), defaultvalue);
      break;
    }

    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), ifAuthFailure_str, ifName, routerIDBuf, ifIpAddrBuf, 
            packetSrcBuf, errorTypeBuf, pckTypeBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = 
        TRAPMGR_SPECIFIC_INTERFACE_AUTHFAILURE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3IfAuthFailureTrapSend(unit, routerId, ifIpAddress,
                                              intIfNum, packetSrc, configErrorType, 
                                              packetType);
#endif
  }

  return rc;
}

/*********************************************************************
*
* @purpose Signifies that a packet has been received on a virtual
*          interface from a router whose authentication key or 
*          authentication type conflicts with this router's 
*          authentication key or authentication type.
*
* @param   routerId         The originator of the trap.
*          virtIfAreaId
*          virtIfNeighbor
*          configErrorType  Auth type mismatch or auth failure
*          packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3VirtIfAuthFailure(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor,
                                 L7_uint32 configErrorType,
                                 L7_uint32 packetType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 virtIfAreaIdBuf[16];
  L7_char8 virtIfNeighborBuf[16];
  L7_char8 errorTypeBuf[28];
  L7_char8 pckTypeBuf[28];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)virtIfAreaIdBuf, 16);
  bzero((L7_char8 *)virtIfNeighborBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)errorTypeBuf, 28);
  bzero((L7_char8 *)pckTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_VIRT_IF_AUTH_FAILURE)
  {  
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(virtIfAreaId, virtIfAreaIdBuf);
    usmDbInetNtoa(virtIfNeighbor, virtIfNeighborBuf);

    switch (configErrorType)
    {
    case L7_BAD_VERSION:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), bad_version);
      break;
    case L7_AREA_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), area_mismatch);
      break;
    case L7_UNKNOWN_NBMA_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_nbma_nbr);
      break;
    case L7_UNKNOWN_VIRTUAL_NBR:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), unknown_virtual_nbr);
      break;
    case L7_AUTH_TYPE_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_type_mismatch);
      break;
    case L7_AUTH_FAILURE:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), auth_failure);
      break;
    case L7_NET_MASK_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), net_mask_mismatch);
      break;
    case L7_HELLO_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), hello_interval_mismatch);
      break;
    case L7_DEAD_INTERVAL_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), dead_interval_mismatch);
      break;
    case L7_OPTION_MISMATCH:
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), option_mismatch);
      break;
    default:  
      osapiSnprintf(errorTypeBuf, sizeof(errorTypeBuf), defaultvalue);
      break;
    }

    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), virtIfAuthFailure_str, routerIDBuf,virtIfAreaIdBuf,virtIfNeighborBuf,errorTypeBuf, pckTypeBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_VIRTUAL_INTERFACE_AUTHFAILURE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3VirtIfAuthFailureTrapSend(unit, routerId, virtIfAreaId, virtIfNeighbor, configErrorType, packetType);

#endif
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that an OSPF packet has been received on a 
*           non-virtual interface that cannot be parsed.
*
* @param    routerId        The originator of the Trap
*           ifIpAddress
*           intIfNum        internal interface number
*           packetSrc       The source ip address
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3IfRxBadPacket(L7_uint32 routerId, L7_in6_addr_t ifIpAddress,
                             L7_uint32 intIfNum, L7_in6_addr_t packetSrc,
                             L7_uint32 packetType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_char8 ifIpAddrBuf[40];
  L7_char8 packetSrcBuf[40];
  L7_char8 routerIDBuf[16];
  L7_char8 pckTypeBuf[28];
  L7_char8 ifName[TM_IF_NAME_LEN];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)ifIpAddrBuf, 40);
  bzero((L7_char8 *)packetSrcBuf, 40);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)pckTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_RX_BAD_PACKET)
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    if (trapMgrIfNameGet(&usp, ifName, sizeof(ifName)) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    usmDbInetNtoa(routerId, routerIDBuf);
    osapiInetNtop(L7_AF_INET6, (L7_char8 *)&ifIpAddress, ifIpAddrBuf, sizeof(ifIpAddrBuf));
    osapiInetNtop(L7_AF_INET6, (L7_char8 *)&packetSrc, packetSrcBuf, sizeof(packetSrcBuf));
    
    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), ifRxBadPacket_str, ifName, routerIDBuf, ifIpAddrBuf, 
            packetSrcBuf, pckTypeBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = 
        TRAPMGR_SPECIFIC_RECEIVED_BADPACKET;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3IfRxBadPacketTrapSend(unit, routerId, ifIpAddress, 
                                              intIfNum, packetSrc, packetType);
#endif
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that an OSPF packet has been received on a 
*           virtual interface that cannot be parsed.
*
* @param    routerId        The originator of the trap
*           virtIfAreaId
*           virtIfNeighbor
*           packetType
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3VirtIfRxBadPacket(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                 L7_uint32 virtIfNeighbor, L7_uint32 packetType)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 virtIfAreaIdBuf[16];
  L7_char8 virtIfNeighborBuf[16];
  L7_char8 pckTypeBuf[28];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)virtIfAreaIdBuf, 16);
  bzero((L7_char8 *)virtIfNeighborBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)pckTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_VIRT_IF_RX_BAD_PACKET)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(virtIfAreaId, virtIfAreaIdBuf);
    usmDbInetNtoa(virtIfNeighbor, virtIfNeighborBuf);

    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), virtIfRxBadPacket_str, routerIDBuf, virtIfAreaIdBuf, virtIfNeighborBuf, pckTypeBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_VIRTUAL_INTERFACE_RECEIVED_BADPACKET;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3VirtIfRxBadPacketTrapSend(unit, routerId, virtIfAreaId,virtIfNeighbor,packetType);
#endif
  }

  return rc;
}

/*********************************************************************
*
* @purpose  Signifies than an OSPF packet has been retransmitted on a 
*           non-virtual interface.
*
* @param    routerId        The originator of the trap
*           ifIpAddress
*           intIfNum        internal interface number
*           nbrRtrId        Destination
*           packetType
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All packets that may be re-transmitted  are associated 
*           with an LSDB entry.  The LS type, LS ID, and Router ID 
*           are used to identify the LSDB entry.
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3TxRetransmit(L7_uint32 routerId, L7_in6_addr_t ifIpAddress,
                            L7_uint32 intIfNum, L7_uint32 nbrRtrId,
                            L7_uint32 packetType, L7_uint32 lsdbType,
                            L7_uint32 lsdbLsid, L7_uint32 lsdbRouterId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_char8 ifIpAddrBuf[40];
  L7_char8 nbrRtrIdBuf[16];
  L7_char8 routerIDBuf[16];
  L7_char8 pckTypeBuf[28];
  L7_char8 lsdbTypeBuf[28];
  L7_char8 lsdbLsidBuf[16];
  L7_char8 lsdbRouterIdBuf[16];
  L7_char8 ifName[TM_IF_NAME_LEN];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)ifIpAddrBuf, 40);
  bzero((L7_char8 *)nbrRtrIdBuf, 16);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)lsdbLsidBuf, 16);
  bzero((L7_char8 *)lsdbRouterIdBuf, 16);
  bzero((L7_char8 *)pckTypeBuf, 28);
  bzero((L7_char8 *)lsdbTypeBuf, 28);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_TX_RETRANSMIT)
  {
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    if (trapMgrIfNameGet(&usp, ifName, sizeof(ifName)) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    usmDbInetNtoa(routerId, routerIDBuf);
    osapiInetNtop(L7_AF_INET6, (L7_char8 *)&ifIpAddress, ifIpAddrBuf, sizeof(ifIpAddrBuf));
    usmDbInetNtoa(nbrRtrId, nbrRtrIdBuf);
    usmDbInetNtoa(lsdbLsid, lsdbLsidBuf);
    usmDbInetNtoa(lsdbRouterId, lsdbRouterIdBuf);
    
    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }

    switch (lsdbType)
    {
    case L7_S_ILLEGAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_illegal_lsa);
      break;
    case L7_S_ROUTER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_router_lsa);
      break;
    case L7_S_NETWORK_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_network_lsa);
      break;
    case L7_S_IPNET_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_ipnet_summary_lsa);
      break;
    case L7_S_ASBR_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_asbr_summary_lsa);
      break;
    case L7_S_AS_EXTERNAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_external_lsa);
      break;
    case L7_S_GROUP_MEMBER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_group_member_lsa);
      break;
    case L7_S_NSSA_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp1_lsa);
      break;
    case L7_S_TMP2_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp2_lsa);
      break;
    case L7_S_LINK_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AREA_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AS_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_opaque_lsa);
      break;
    case L7_S_LAST_LSA_TYPE:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_last_lsa_type);
      break;
    default:  
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), defaultvalue);
      break;
    }
    /*may have buffer problem here */
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), txRetransmit_str, ifName, routerIDBuf, ifIpAddrBuf, 
            nbrRtrIdBuf,pckTypeBuf, lsdbTypeBuf,lsdbLsidBuf, lsdbRouterIdBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = 
        TRAPMGR_SPECIFIC_RETRANSMIT;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3TxRetransmitTrapSend(unit, routerId,ifIpAddress,intIfNum,nbrRtrId,
                           packetType,lsdbType,lsdbLsid,lsdbRouterId);
#endif
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  Signifies than an OSPF packet has been retransmitted on a 
*           virtual interface.
*
* @param    routerId        The originator of the trap
*           virtIfAreaId
*           virtIfNeighbor
*           packetType
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    All packets that may be re-transmitted  are associated 
*           with an LSDB entry.  The LS type, LS ID, and Router ID 
*           are used to identify the LSDB entry.
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3VirtTxRetransmit(L7_uint32 routerId, L7_uint32 virtIfAreaId,
                                L7_uint32 virtIfNeighbor, L7_uint32 packetType,
                                L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                                L7_uint32 lsdbRouterId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 virtIfAreaIdBuf[16];
  L7_char8 virtIfNeighborBuf[16];
  L7_char8 pckTypeBuf[28];
  L7_char8 lsdbTypeBuf[28];
  L7_char8 lsdbLsidBuf[16];
  L7_char8 lsdbRouterIdBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)virtIfAreaIdBuf, 16);
  bzero((L7_char8 *)virtIfNeighborBuf, 16);
  bzero((L7_char8 *)pckTypeBuf, 28);
  bzero((L7_char8 *)lsdbTypeBuf, 28);
  bzero((L7_char8 *)lsdbLsidBuf, 16);
  bzero((L7_char8 *)lsdbRouterIdBuf, 16);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_VIRT_IF_TX_RETRANSMIT)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(virtIfAreaId, virtIfAreaIdBuf);
    usmDbInetNtoa(virtIfNeighbor, virtIfNeighborBuf);
    usmDbInetNtoa(lsdbLsid, lsdbLsidBuf);
    usmDbInetNtoa(lsdbRouterId, lsdbRouterIdBuf);
    
    switch (packetType)
    {
    case L7_S_HELLO:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_hello);
      break;
    case L7_S_DB_DESCR:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_db_descr);
      break;
    case L7_S_LS_REQUEST:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_request);
      break;
    case L7_S_LS_UPDATE:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_update);
      break;
    case L7_S_LS_ACK:
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), packet_ls_ack);
      break;
    default:  
      osapiSnprintf(pckTypeBuf, sizeof(pckTypeBuf), defaultvalue);
      break;
    }

    switch (lsdbType)
    {
    case L7_S_ILLEGAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_illegal_lsa);
      break;
    case L7_S_ROUTER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_router_lsa);
      break;
    case L7_S_NETWORK_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_network_lsa);
      break;
    case L7_S_IPNET_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_ipnet_summary_lsa);
      break;
    case L7_S_ASBR_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_asbr_summary_lsa);
      break;
    case L7_S_AS_EXTERNAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_external_lsa);
      break;
    case L7_S_GROUP_MEMBER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_group_member_lsa);
      break;
    case L7_S_NSSA_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp1_lsa);
      break;
    case L7_S_TMP2_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp2_lsa);
      break;
    case L7_S_LINK_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AREA_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AS_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_opaque_lsa);
      break;
    case L7_S_LAST_LSA_TYPE:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_last_lsa_type);
      break;
    default:  
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), defaultvalue);
      break;
    }

    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), virtTxRetransmit_str, routerIDBuf, virtIfAreaIdBuf, virtIfNeighborBuf, pckTypeBuf,lsdbTypeBuf,lsdbLsidBuf, lsdbRouterIdBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_VIRTUAL_RETRANSMIT;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3VirtTxRetransmitTrapSend(unit, routerId,virtIfAreaId,virtIfNeighbor,
                               packetType,lsdbType,lsdbLsid,lsdbRouterId);
#endif
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that a  new LSA  has  been originated by this router.
*
* @param    routerId        The originator of the trap.
*           lsdbAreaId      0.0.0.0 for AS Externals
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    This trap should not be invoked for simple refreshes of  
*           LSAs  (which happesn every 30 minutes), but       
*           instead will only be invoked  when  an  LSA  is       
*           (re)originated due to a topology change.  Addi-       
*           tionally, this trap does not include LSAs  that       
*           are  being  flushed  because  they have reached       
*           MaxAge.
*
* @end      
*********************************************************************/
L7_RC_t trapMgrOspfv3OriginateLsa(L7_uint32 routerId, L7_uint32 lsdbAreaId,
                            L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                            L7_uint32 lsdbRouterId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 lsdbAreaIdBuf[16];
  L7_char8 lsdbTypeBuf[28];
  L7_char8 lsdbLsidBuf[16];
  L7_char8 lsdbRouterIdBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)lsdbAreaIdBuf, 16);
  bzero((L7_char8 *)lsdbTypeBuf, 28);
  bzero((L7_char8 *)lsdbLsidBuf, 16);
  bzero((L7_char8 *)lsdbRouterIdBuf, 16);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_ORIGINATE_LSA)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(lsdbAreaId, lsdbAreaIdBuf);
    usmDbInetNtoa(lsdbLsid, lsdbLsidBuf);
    usmDbInetNtoa(lsdbRouterId, lsdbRouterIdBuf);
    
    switch (lsdbType)
    {
    case L7_S_ILLEGAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_illegal_lsa);
      break;
    case L7_S_ROUTER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_router_lsa);
      break;
    case L7_S_NETWORK_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_network_lsa);
      break;
    case L7_S_IPNET_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_ipnet_summary_lsa);
      break;
    case L7_S_ASBR_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_asbr_summary_lsa);
      break;
    case L7_S_AS_EXTERNAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_external_lsa);
      break;
    case L7_S_GROUP_MEMBER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_group_member_lsa);
      break;
    case L7_S_NSSA_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp1_lsa);
      break;
    case L7_S_TMP2_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp2_lsa);
      break;
    case L7_S_LINK_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AREA_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AS_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_opaque_lsa);
      break;
    case L7_S_LAST_LSA_TYPE:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_last_lsa_type);
      break;
    default:  
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), defaultvalue);
      break;
    }

    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), originateLsa_str, routerIDBuf, lsdbAreaIdBuf, lsdbTypeBuf, lsdbLsidBuf,lsdbRouterIdBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_ORIGINATE_LSA;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3OriginateLsaTrapSend(unit, routerId,lsdbAreaId,lsdbType,lsdbLsid,lsdbRouterId);
#endif
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that one of the LSA in the router's link-state 
*           database has aged to MaxAge.
*
* @param    routerId         The originator of the trap
*           lsdbAreaId       0.0.0.0 for AS Externals
*           lsdbType
*           lsdbLsid
*           lsdbRouterId
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3MaxAgeLsa(L7_uint32 routerId, L7_uint32 lsdbAreaId,
                         L7_uint32 lsdbType, L7_uint32 lsdbLsid,
                         L7_uint32 lsdbRouterId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_char8 lsdbAreaIdBuf[16];
  L7_char8 lsdbTypeBuf[28];
  L7_char8 lsdbLsidBuf[16];
  L7_char8 lsdbRouterIdBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)lsdbAreaIdBuf, 16);
  bzero((L7_char8 *)lsdbTypeBuf, 28);
  bzero((L7_char8 *)lsdbLsidBuf, 16);
  bzero((L7_char8 *)lsdbRouterIdBuf, 16);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_MAX_AGE_LSA)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    usmDbInetNtoa(lsdbAreaId, lsdbAreaIdBuf);
    usmDbInetNtoa(lsdbLsid, lsdbLsidBuf);
    usmDbInetNtoa(lsdbRouterId, lsdbRouterIdBuf);
    
    switch (lsdbType)
    {
    case L7_S_ILLEGAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_illegal_lsa);
      break;
    case L7_S_ROUTER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_router_lsa);
      break;
    case L7_S_NETWORK_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_network_lsa);
      break;
    case L7_S_IPNET_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_ipnet_summary_lsa);
      break;
    case L7_S_ASBR_SUMMARY_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_asbr_summary_lsa);
      break;
    case L7_S_AS_EXTERNAL_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_external_lsa);
      break;
    case L7_S_GROUP_MEMBER_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_group_member_lsa);
      break;
    case L7_S_NSSA_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp1_lsa);
      break;
    case L7_S_TMP2_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_tmp2_lsa);
      break;
    case L7_S_LINK_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AREA_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_link_opaque_lsa);
      break;
    case L7_S_AS_OPAQUE_LSA:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_as_opaque_lsa);
      break;
    case L7_S_LAST_LSA_TYPE:
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), lsdb_last_lsa_type);
      break;
    default:  
      osapiSnprintf(lsdbTypeBuf, sizeof(lsdbTypeBuf), defaultvalue);
      break;
    }

    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), maxAgeLsa_str, routerIDBuf, lsdbAreaIdBuf, lsdbTypeBuf, lsdbLsidBuf,lsdbRouterIdBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_MAX_AGE_LSA;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3MaxAgeLsaTrapSend(unit, routerId,lsdbAreaId,lsdbType,lsdbLsid,lsdbRouterId);
#endif
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  Signifies that the number of LSAs in the router's link-state 
*           data-base has exceeded ospfExtLsdbLimit.
*
* @param    routerId      The originator of the trap
*           extLsdbLimit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3LsdbOverflow(L7_uint32 routerId, L7_uint32 extLsdbLimit)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)routerIDBuf, 16);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_LS_DB_OVERFLOW)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), lsdbOverflow_str, routerIDBuf, extLsdbLimit);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_LSDB_OVERFLOW;

    trapMgrLogTrapToLocalLog(trapStringBuf);
#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3LsdbOverflowTrapSend(unit, routerId,extLsdbLimit);
#endif
  }
  
  return rc;
}


/*********************************************************************
*
* @purpose  Signifies that  the  number of LSAs in the router's link-
*           state database has exceeded ninety  percent  of      
*           ospfExtLsdbLimit.
*
* @param    routerId      The originator of the trap
*           extLsdbLimit
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3LsdbApproachingOverflow(L7_uint32 routerId,
                                       L7_uint32 extLsdbLimit)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 routerIDBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)routerIDBuf, 16);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_LS_DB_APPROACHING_OVERFLOW)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), lsdbApproachingOverflow_str, routerIDBuf, extLsdbLimit);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_LSDB_APPROACHING_OVERFLOW;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef L7_MIB_OSPFV3 
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3LsdbApproachingOverflowTrapSend(unit, routerId,extLsdbLimit);
#endif
  }
  
  return rc;
}


/*********************************************************************
*
* @purpose  Signifies that there has been a change in the state of a 
*           non-virtual OSPF interface.
*
* @param    routerId      The originator of the trap
*           ifIpAddress
*           intIfNum      internal interface number
*           ifState       New state
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   This trap should  be  generated when  the interface state 
*          regresses (e.g., goes from Dr to Down) or progresses  to  a  terminal      
*          state  (i.e.,  Point-to-Point, DR Other, Dr, or Backup).
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3IfStateChange(L7_uint32 routerId, L7_in6_addr_t ifIpAddress,
                             L7_uint32 intIfNum, L7_uint32 ifState)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  nimUSP_t usp;
  L7_char8 ifIpAddrBuf[40];
  L7_char8 routerIDBuf[16];
  L7_char8 stateBuf[28];
  L7_char8 ifName[TM_IF_NAME_LEN];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)ifIpAddrBuf, 40);
  bzero((L7_char8 *)routerIDBuf, 16);
  bzero((L7_char8 *)stateBuf, 28);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_IF_STATE_CHANGE)
  {
    usmDbInetNtoa(routerId, routerIDBuf);
    osapiInetNtop(L7_AF_INET6, (L7_char8 *)&ifIpAddress, ifIpAddrBuf, sizeof(ifIpAddrBuf));
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }
    if (trapMgrIfNameGet(&usp, ifName, sizeof(ifName)) != L7_SUCCESS)
    {
        return L7_FAILURE;
    }

    switch (ifState)
    {
    case L7_OSPF_INTF_DOWN:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_down);
      break;
    case L7_OSPF_INTF_LOOPBACK:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_loopback);
      break;
    case L7_OSPF_INTF_WAITING:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_waiting);
      break;
    case L7_OSPF_INTF_POINTTOPOINT:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_pointtopoint);
      break;
    case L7_OSPF_INTF_DESIGNATEDROUTER:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_desig_rtr);
      break;
    case L7_OSPF_INTF_BACKUPDESIGNATEDROUTER:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_bkup_desig_rtr);
      break;
    case L7_OSPF_INTF_OTHERDESIGNATEDROUTER:
      osapiSnprintf(stateBuf, sizeof(stateBuf), ospf_intf_other_desig_rtr);
      break;
    default:  
      osapiSnprintf(stateBuf, sizeof(stateBuf), defaultvalue);
      break;
    }

    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), ifStateChange_str, ifName, routerIDBuf, ifIpAddrBuf, stateBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = 
        TRAPMGR_SPECIFIC_INTERFACE_STATE_CHANGE;

    trapMgrLogTrapToLocalLog(trapStringBuf);

#ifdef  L7_MIB_OSPFV3
    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc = usmDbSnmpOspfv3IfStateChangeTrapSend(unit, routerId,ifIpAddress,intIfNum,ifState);
#endif
  }
  
  return rc;
}


/*********************************************************************
*
* @purpose  Signifies that an OSPF packet was received on a non-virtual interface. 
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3IfRxPacket()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  
  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  
  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_IF_RX_PACKET)
  {
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), ifRxPacket_str);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_INTERFACE_RECEIVED_PACKET;

    trapMgrLogTrapToLocalLog(trapStringBuf);
/*    rc = usmDbIfRxPacket();*/
    rc = L7_FAILURE;
  }
  
  return rc;
}


/*********************************************************************
*
* @purpose  Signifies that an entry has been made in the OSPF routing table. 
*
* @param    
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    ATIC's proprietary Routing Table Entry Information trap.
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3RtbEntryInfo()
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);

  if (trapMgrCfgData.trapOspfv3 & L7_OSPF_TRAP_RTB_ENTRY_INFO)
  {
    osapiSnprintf(trapStringBuf, sizeof(trapStringBuf), rtbEntryInfo_str);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_ROUNDTABLE_ENTRY_INFO;

    trapMgrLogTrapToLocalLog(trapStringBuf);
/*    rc = usmDbRtbEntryInfo();*/
    rc = L7_FAILURE;
  }
  
  return rc;
}

/*********************************************************************
*
* @purpose  Allows a user to enable or disable traps based on the
*           specified mode.
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  ospf trap type specified in L7_OSPF_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3TrapModeSet(L7_uint32 mode, L7_uint32 trapType)
{
  if(mode == L7_ENABLE)
  {
    trapMgrCfgData.trapOspfv3 |= trapType;
  }
  else if(mode == L7_DISABLE)
  {
    trapMgrCfgData.trapOspfv3 &= ~(trapType);
  }
  else 
    return L7_FAILURE;

/* @p1117 start */
  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
/* @p1117 end */

  return ospfv3MapTrapModeSet(trapMgrCfgData.trapOspfv3);
}

/*********************************************************************
*
* @purpose  Allows a user to determine whether the specified trap
*           is enabled or disabled
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  ospf trap type specified in L7_OSPF_TRAP_t
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3TrapModeGet(L7_uint32 *mode, L7_uint32 trapType)
{
  if(trapMgrCfgData.trapOspfv3 & trapType)
  {
    *mode = L7_ENABLE;
  }
  else
    *mode = L7_DISABLE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Allows a user to determine configured ospf trap flags
*
* @param    trapFlags      bit mask indicating ospf traps that are 
*                          enabled
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    
*
* @end
*********************************************************************/
L7_RC_t trapMgrOspfv3TrapFlagsGet(L7_uint32 *trapFlags)
{
  *trapFlags = trapMgrCfgData.trapOspfv3;
  return L7_SUCCESS;
}
#endif
