/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: snmp_trap_api_l3.c
*
* Purpose: API interface for SNMP Agent Traps
*
* Created by:
*
* Component: SNMP
*
*********************************************************************/
/********************************************************************
 *                    
 *******************************************************************/

#include "snmp_trap_api_mcast_l3.h"
#ifdef L7_MCAST_PACKAGE
#include "privatetrap_dvmrp.h"
#include "privatetrap_pim.h"
#endif /* L7_MCAST_PACKAGE */
#include "snmp_api.h"
#include "usmdb_util_api.h"
#include "dtlapi.h"
#include "osapi.h"


typedef struct
{
  union
  {
    L7_uint32 pimNeighborIfIndex;
    L7_uint32 pimNeighborUpTime;
  }object;
}
L7_SNMP_TRAP_IP_MCAST_PimNeighborLoss_t;

typedef struct
{
  L7_uint32 dvmrpInfLocalAddress;
  L7_uint32 dvmrpNeighborCapabilities;
}
L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborNotPruning_t;

typedef struct
{
  L7_uint32 dvmrpInfLocalAddress;
  L7_int32  dvmrpNeighborState;
}
L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborLoss_t;

typedef enum
{
  L7_SNMP_TRAP_IP_MCAST_PimNeighborLoss = 0,
  L7_SNMP_TRAP_IP_MCAST_PimSmNeighborLoss,
  L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborNotPruning,
  L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborLoss,
} L7_SNMP_TRAP_TYPE_IP_MCAST_t;

/*********************************************************************
*
* @purpose  Handles calling the trap code to send traps
*
* @param    trap_id   enumeration of the trap to send
* @param    data      data structure holding trap information
*
* @returns  void
*
* @notes    The datastructure is determined by the trap_id indicated.
*
* @end
*********************************************************************/
void SnmpTrapSendCallback_ip_mcast(L7_uint32 trap_id, void *data)
{
  switch (trap_id)
  {
  case L7_SNMP_TRAP_IP_MCAST_PimNeighborLoss:
    (void)snmp_pimNeighborLossTrapSend(((L7_SNMP_TRAP_IP_MCAST_PimNeighborLoss_t*)data)->object.pimNeighborIfIndex);
    break;
  case L7_SNMP_TRAP_IP_MCAST_PimSmNeighborLoss:
    (void)snmp_pimSmNeighborLossTrapSend(((L7_SNMP_TRAP_IP_MCAST_PimNeighborLoss_t*)data)->object.pimNeighborUpTime);
    break;
  case L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborNotPruning:
    (void)snmp_dvmrpNeighborNotPruningTrapSend(((L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborNotPruning_t*)data)->dvmrpInfLocalAddress,
                                               ((L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborNotPruning_t*)data)->dvmrpNeighborCapabilities);
    break;

  case L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborLoss:
    (void)snmp_dvmrpNeighborLossTrapSend(((L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborLoss_t*)data)->dvmrpInfLocalAddress,
                                        ((L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborLoss_t*)data)->dvmrpNeighborState);
    break;

  default:
    /* unknown trap */
    break;
  }
}

/* Begin Function Declarations: snmp_trap_api_l3.h */
/*********************************************************************
*
* @purpose  Send a trap when PIM neighbor adjacency is lost
*
*
* @param    pimNeighborLossIfIndex       The interface on which neighbor is lost
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t SnmpPimNeighborLossTrapSend(L7_uint32 pimNeighborIfIndex, L7_uint32 pimNeighborUpTime, L7_BOOL isPimSm)
{
  L7_uint32 ifIndex;
  L7_SNMP_TRAP_IP_MCAST_PimNeighborLoss_t *trap = L7_NULLPTR;

  if (!isPimSm)
  {
    if (usmDbExtIfNumFromIntIfNum(pimNeighborIfIndex,&ifIndex) != L7_SUCCESS)
     return L7_FAILURE;
  }

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_IP_MCAST_PimNeighborLoss_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  if (!isPimSm)
  {
    trap->object.pimNeighborIfIndex = ifIndex;
    return snmpTrapSend(L7_SNMP_TRAP_IP_MCAST_PimNeighborLoss, (void*)trap, &SnmpTrapSendCallback_ip_mcast);
  }
  else
  {
    trap->object.pimNeighborUpTime = pimNeighborUpTime;
     return snmpTrapSend(L7_SNMP_TRAP_IP_MCAST_PimSmNeighborLoss, (void*)trap, &SnmpTrapSendCallback_ip_mcast);
  }

}

/*********************************************************************
*
* @purpose  Send a trap when DVMRP neighbor is not pruning
*
*
* @param    dvmrpInfLocalAddress       The interface address on which neighbor is lost
* @param    dvmrpNeighborCapabilities  The Capabilities of the neighbor
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t SnmpDvmrpNeighborNotPruningTrapSend(L7_uint32 dvmrpInfLocalAddress, L7_uint32 dvmrpNeighborCapabilities)
{
  L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborNotPruning_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborNotPruning_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->dvmrpInfLocalAddress = dvmrpInfLocalAddress;
  trap->dvmrpNeighborCapabilities = dvmrpNeighborCapabilities;

  return snmpTrapSend(L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborNotPruning, (void*)trap, &SnmpTrapSendCallback_ip_mcast);
}

/*********************************************************************
*
* @purpose  Send a trap when DVMRP neighbor adjacency is lost
*
*
* @param    dvmrpInfLocalAddress       The interface address on which neighbor is lost
* @param    dvmrpNeighborState       The state of the neighbor
*
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes   none
*
* @end
*********************************************************************/
L7_RC_t SnmpDvmrpNeighborLossTrapSend(L7_uint32 dvmrpInfLocalAddress, L7_int32 dvmrpNeighborState)
{
  L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborLoss_t *trap = L7_NULLPTR;

  trap = osapiMalloc(L7_SNMP_COMPONENT_ID, sizeof(L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborLoss_t));

  if (trap == L7_NULLPTR)
    return L7_ERROR;

  trap->dvmrpInfLocalAddress = dvmrpInfLocalAddress;
  trap->dvmrpNeighborState = dvmrpNeighborState;

  return snmpTrapSend(L7_SNMP_TRAP_IP_MCAST_DvmrpNeighborLoss, (void*)trap, &SnmpTrapSendCallback_ip_mcast);
}

void SnmpTestTraps_ip_mcast()
{
  L7_RC_t rc;

  rc = SnmpPimNeighborLossTrapSend(1, 0, L7_FALSE);
  sysapiPrintf("SnmpPimNeighborLossTrapSend -> %d\n", rc);

  rc = SnmpDvmrpNeighborNotPruningTrapSend(2, 3);
  sysapiPrintf("SnmpDvmrpNeighborNotPruningTrapSend -> %d\n", rc);

  rc = SnmpDvmrpNeighborLossTrapSend(4, 5);
  sysapiPrintf("SnmpDvmrpNeighborLossTrapSend -> %d\n", rc);

  return;
}

/* End Function Declarations */

