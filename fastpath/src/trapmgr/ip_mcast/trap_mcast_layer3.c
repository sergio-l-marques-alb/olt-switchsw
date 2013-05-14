/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
* @filename trap_mcast_layer3.c
*
* @purpose Trap Manager Layer 3 functions
*
* @component trap_layer3.c
*
* @comments none
*
* @created
*
* @author
*
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include <stdio.h>
#include <l7_common.h>
#include <l3_commdefs.h>
#include <trapapi.h>
#include <trap.h>
#include <trap_layer3_api.h>
#include <trapstr_layer3.h>
#include <trapstr_mcast_layer3.h>
#include <usmdb_trap_layer3_api.h>
#include <usmdb_snmp_trap_api_l3.h>
#include <usmdb_snmp_trap_mcast_api_l3.h>
#include <usmdb_snmp_api.h>
#include <usmdb_util_api.h>
#include <l7_ospf_api.h>
#include "l3_mcast_commdefs.h"
#include <trap_layer3_mcast_api.h>
#include "usmdb_sim_api.h"
#include "usmdb_snmp_api.h"
#include "usmdb_snmp_trap_mcast_api_l3.h"
#include "usmdb_util_api.h"

extern trapMgrCfgData_t trapMgrCfgData;
extern trapMgrTrapData_t trapMgrTrapData;

/*********************************************************************
*
* @purpose  A pimNeighborLoss trap signifies the loss of an adjacency
*           with a neighbor.  This trap should be generated when the
*           neighbor timer expires, and the router has no other
*           neighbors on the same interface with a lower IP address than
*           itself.
*
* @param    pimNeighborLossIfIndex       The IfIndex on which neighbor is lost.
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/

L7_RC_t trapMgrPimTrapNeighborLoss(L7_uint32 pimNeighborLossIfIndex, L7_uint32 pimNeighborUpTime, L7_BOOL isPimSm)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  if (trapMgrCfgData.trapPim & L7_PIM_TRAP_NEIGHBOR_LOSS)
  {
    sprintf(trapStringBuf, pimNeighborLoss_str, pimNeighborUpTime);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_PIM_NEIGHBOR_LOSS;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc =  usmDbSnmpPimNeighborLossTrapSend(unit, pimNeighborLossIfIndex, pimNeighborUpTime, isPimSm);
  }

  return rc;
}
/*********************************************************************
*
* @purpose  Allows a user to know whether the specified trap
*           is enabled or disabled
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  PIM trap type specified in L3_PIMDM_COMMDEFS_H
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t trapMgrPimTrapModeGet(L7_uint32 *mode, L7_uint32 trapType)
{
  if(trapMgrCfgData.trapPim & trapType)
  {
    *mode = L7_ENABLE;
  }
  else
    *mode = L7_DISABLE;
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose  Allows a user to enable or disable traps based on the
*           specified mode.
*
* @param    mode      L7_ENABLE/ L7_DISABLE
* @param    trapType  PIM trap type specified in L3_PIMDM_COMMDEFS_H
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t trapMgrPimTrapModeSet(L7_uint32 mode, L7_uint32 trapType)
{
    if(mode == L7_ENABLE)
    {
        trapMgrCfgData.trapPim |= trapType;
    }
    else if(mode == L7_DISABLE)
    {
        trapMgrCfgData.trapPim &= ~(trapType);
    }
    else
        return L7_FAILURE;
    trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose   A dvmrpNeighborLoss trap signifies the loss of a 2-way
*            adjacency with a neighbor.  This trap should be generated
*            when the neighbor state changes from active to one-way,
*            ignoring, or down.  The trap should be generated only if the
*            router has no other neighbors on the same interface with a
*            lower IP address than itself.
*
* @param    dvmrpInterfaceLocalAddress  The interface address on which neighbor is lost
* @param    dvmrpNeighborState          The state of the neighbor
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t trapMgrDvmrpTrapNeighborLoss(L7_uint32 dvmrpInterfaceLocalAddress, L7_int32 dvmrpNeighborState)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 dvmrpInterfaceLocalAddressBuf[16];
  L7_char8 dvmrpNeighborStateBuf[16];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)dvmrpInterfaceLocalAddressBuf, 16);
  bzero((L7_char8 *)dvmrpNeighborStateBuf, 16);

  if (trapMgrCfgData.trapDvmrp & L7_DVMRP_TRAP_NEIGHBOR_LOSS)
  {
    (void) usmDbInetNtoa(dvmrpInterfaceLocalAddress, dvmrpInterfaceLocalAddressBuf);

    switch (dvmrpNeighborState)
    {
    case L7_DVMRP_NEIGHBOR_STATE_ONEWAY:
      strcpy(dvmrpNeighborStateBuf, dvmrpNeighborStateOneway);
      break;
    case L7_DVMRP_NEIGHBOR_STATE_ACTIVE:
      strcpy(dvmrpNeighborStateBuf, dvmrpNeighborStateActive);
      break;
    case L7_DVMRP_NEIGHBOR_STATE_IGNORING:
      strcpy(dvmrpNeighborStateBuf, dvmrpNeighborStateIgnoring);
      break;
    case L7_DVMRP_NEIGHBOR_STATE_DOWN:
      strcpy(dvmrpNeighborStateBuf, dvmrpNeighborStateDown);
      break;
    default:
      strcpy(dvmrpNeighborStateBuf, defaultvalue);
      break;
    }

    sprintf(trapStringBuf, dvmrpNeighborLoss_str, dvmrpInterfaceLocalAddressBuf, dvmrpNeighborStateBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_DVMRP_NEIGHBOR_LOSS;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc =  usmDbSnmpDvmrpNeighborLossTrapSend(unit, dvmrpInterfaceLocalAddress, dvmrpNeighborState);
  }

  return rc;
}

/*********************************************************************
*
* @purpose   A dvmrpNeighborNotPruning trap signifies that a non-pruning
*            neighbor has been detected.  This trap should be generated at
*            most once per generation ID of the neighbor.  The trap
*            should be generated only if the router has no other neighbors on
*            the same interface with a lower IP address than itself.
*
* @param    dvmrpInterfaceLocalAddress  The interface address on which neighbor is lost
* @param    dvmrpNeighborCapabilities   The Capabilities of the neighbor
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t trapMgrDvmrpTrapNeighborNotPruning(L7_uint32 dvmrpInterfaceLocalAddress,
                                           L7_uint32 dvmrpNeighborCapabilities)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_char8 trapStringBuf[TRAPMGR_MSG_SIZE];
  L7_char8 dvmrpInterfaceLocalAddressBuf[16];
  L7_char8 dvmrpNeighborCapabilitiesBuf[32];
  L7_uint32 unit;

  unit = usmDbThisUnitGet();

  bzero((L7_char8 *)trapStringBuf, TRAPMGR_MSG_SIZE);
  bzero((L7_char8 *)dvmrpInterfaceLocalAddressBuf, 16);
  bzero((L7_char8 *)dvmrpNeighborCapabilitiesBuf, 32);

  if (trapMgrCfgData.trapDvmrp & L7_DVMRP_TRAP_NEIGHBOR_NOT_PRUNING)
  {
    (void) usmDbInetNtoa(dvmrpInterfaceLocalAddress, dvmrpInterfaceLocalAddressBuf);

    if (dvmrpNeighborCapabilities & (0x1 << L7_DVMRP_CAPABILITIES_LEAF))
      osapiSnprintfcat (dvmrpNeighborCapabilitiesBuf, strlen(dvmrpCapabilitiesLeaf),
                        dvmrpCapabilitiesLeaf);

    if (dvmrpNeighborCapabilities & (0x1 << L7_DVMRP_CAPABILITIES_PRUNE))
      osapiSnprintfcat (dvmrpNeighborCapabilitiesBuf, strlen(dvmrpCapabilitiesPrune),
                        dvmrpCapabilitiesPrune);

    if (dvmrpNeighborCapabilities & (0x1 << L7_DVMRP_CAPABILITIES_GENID))
      osapiSnprintfcat (dvmrpNeighborCapabilitiesBuf, strlen(dvmrpCapabilitiesGenId),
                        dvmrpCapabilitiesGenId);

    if (dvmrpNeighborCapabilities & (0x1 << L7_DVMRP_CAPABILITIES_MTRACE))
      osapiSnprintfcat (dvmrpNeighborCapabilitiesBuf, strlen(dvmrpCapabilitiesMTrace),
                        dvmrpCapabilitiesMTrace);
    osapiSnprintf(trapStringBuf,TRAPMGR_MSG_SIZE, dvmrpNeighborNotPruning_str, dvmrpInterfaceLocalAddressBuf, dvmrpNeighborCapabilitiesBuf);
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].gen = TRAPMGR_GENERIC_NONE;
    trapMgrTrapData.trapLog[trapMgrTrapData.lastReceivedTrap].spec = TRAPMGR_SPECIFIC_DVMRP_NEIGHBOR_NOT_PRUNING;

    trapMgrLogTrapToLocalLog(trapStringBuf);

    if (usmDbSnmpStatusGet(unit) == L7_ENABLE)
      rc =  usmDbSnmpDvmrpNeighborNotPruningTrapSend(unit, dvmrpInterfaceLocalAddress, dvmrpNeighborCapabilities);
  }

  return rc;
}

/*********************************************************************
* @purpose  Returns Trap Manager's trap DVMRP state  
*
* @param    *mode     L7_ENABLE/L7_DISABLE
* @param    trapType  DVMRP trap type specified
*
* @returns  L7_SUCCESS
*           
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t trapMgrDvmrpTrapModeGet(L7_uint32 *mode, L7_uint32 trapType)
{
  if(trapMgrCfgData.trapDvmrp & trapType)
  {
    *mode = L7_ENABLE;
  }
  else
    *mode = L7_DISABLE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Sets Trap Manager's DVMRP state  
*
*
* @param    mode      L7_ENABLE/L7_DISABLE
* @param    trapType  DVMRP trap type specified
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none 
*       
* @end
*********************************************************************/
L7_RC_t trapMgrDvmrpTrapModeSet(L7_uint32 mode, L7_uint32 trapType)
{
  if(mode == L7_ENABLE)
  {
    trapMgrCfgData.trapDvmrp |= trapType;
  }
  else if(mode == L7_DISABLE)
  {
    trapMgrCfgData.trapDvmrp &= ~trapType;
  }
  else 
    return L7_FAILURE;

  trapMgrCfgData.cfgHdr.dataChanged = L7_TRUE;

  return L7_SUCCESS;
}
