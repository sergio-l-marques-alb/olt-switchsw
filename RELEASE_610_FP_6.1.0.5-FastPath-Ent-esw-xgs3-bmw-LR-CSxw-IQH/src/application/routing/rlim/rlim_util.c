/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
*
* @filename rlim.c
*
* @purpose Utility functions for to support routing logical interfaces
*
* @component Routing Logical Interface Manager
*
* @comments
*
* @create 02/23/2005
*
* @author eberge
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "rlim.h"


/*********************************************************************
* @purpose  Get the interface name from a type/id pair
*
* @param    intfType    Type of the interface
* @param    intfId      0-based interface ID
* @param    pIntfName   Pointer to buffer for name
*
* @returns  void
*
* @notes    Assumes pIntName array is at least
*           RLIM_INTF_MAX_NAME_LENGTH bytes.
*
* @end
*********************************************************************/
void rlimIntfNameFromTypeIdGet(L7_uint32 intfType, L7_uint32 intfId,
                               L7_uchar8 *pIntName)
{
  L7_uchar8 *namePrefix = "unknown";

  switch (intfType)
  {
    case L7_LOOPBACK_INTF:
      namePrefix = RLIM_INTF_LOOPBACK_NAME_PREFIX;
      break;

    case L7_TUNNEL_INTF:
      namePrefix = RLIM_INTF_TUNNEL_NAME_PREFIX;
      break;

    default:
      break;
  }

  sprintf(pIntName, "%s%u", namePrefix, intfId);
}

/*********************************************************************
* @purpose  Log a message including the interface name
*
* @param    fmtString   Format string
* @param    intIfNum    Internal interface identifier
*
* @returns  void
*
* @notes    Assumes fmtString includes a %s for the interface name.
*
* @end
*********************************************************************/
void rlimIntfLogMsg(L7_uchar8 *fmtString, L7_uint32 intIfNum)
{
  L7_uint32 ifType, ifId;
  L7_uchar8 nameBuf[RLIM_INTF_MAX_NAME_LENGTH];

  if (rlimIntfTypeIdGet(intIfNum, &ifType, &ifId) != L7_SUCCESS)
  {
    LOG_MSG(fmtString, "unknown");
    return;
  }

  rlimIntfNameFromTypeIdGet(ifType, ifId, nameBuf);
  LOG_MSG(fmtString, nameBuf);
}

/*********************************************************************
* @purpose  Get the type and ID of a given interface
*
* @param    intIfNum    Internal interface identifier
* @param    pIfType     Pointer to type of interface
* @param    pIfId       Pointer to RLIM ID of interface
*
* @returns  L7_SUCCESS  If a valid RLIM interface
* @returns  L7_FAILURE  Otherwise
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t rlimIntfTypeIdGet(L7_uint32 intIfNum, L7_uint32 *pIfType,
                          L7_uint32 *pIfId)
{
  L7_RC_t rc;
  nimConfigID_t configId;

  rc = nimConfigIdGet(intIfNum, &configId);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  switch (configId.type)
  {
    case L7_LOOPBACK_INTF:
      *pIfId = configId.configSpecifier.loopbackId;
      break;

    case L7_TUNNEL_INTF:
      *pIfId = configId.configSpecifier.tunnelId;
      break;

    default:
      return L7_FAILURE;
  }

  *pIfType = configId.type;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Convert an internal interace number to a loopbackId
*
* @param    intIfNum     internal interface number
* @param    pLoopbackId  pointer to Loopback ID return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    For internal use, so locks are not acquired.
*
* @end
*********************************************************************/
L7_RC_t rlimLoopbackIdGetNoLock(L7_uint32 intIfNum, L7_uint32 *pLoopbackId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 ifType, ifId;

  rc = rlimIntfTypeIdGet(intIfNum, &ifType, &ifId);
  if (rc == L7_SUCCESS && ifType == L7_LOOPBACK_INTF)
  {
    *pLoopbackId = ifId;
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Convert an internal interace number to a tunnelId
*
* @param    intIfNum     internal interface number
* @param    pTunnelId    pointer to Tunnel ID return value
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    For internal use, so locks are not acquired.
*
* @end
*********************************************************************/
L7_RC_t rlimTunnelIdGetNoLock(L7_uint32 intIfNum, L7_uint32 *pTunnelId)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 ifType, ifId;

  rc = rlimIntfTypeIdGet(intIfNum, &ifType, &ifId);
  if (rc == L7_SUCCESS && ifType == L7_TUNNEL_INTF)
  {
    *pTunnelId = ifId;
  }
  else
  {
    rc = L7_FAILURE;
  }

  return rc;
}

/*********************************************************************
* @purpose  Determine if two addresses are equal
*
* @param    pAddr1
* @param    pAddr2
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @notes    none
*
* @end
*********************************************************************/
L7_BOOL rlimAddrIsEqual(rlimAddr_t *pAddr1, rlimAddr_t *pAddr2)
{
  if (pAddr1->addrType != pAddr2->addrType)
    return L7_FALSE;

  switch (pAddr1->addrType)
  {
    case RLIM_ADDRTYPE_IP4:
      return (pAddr1->un.ip4addr == pAddr2->un.ip4addr);

    case RLIM_ADDRTYPE_IP6:
      return memcmp(&pAddr1->un.ip6addr, &pAddr2->un.ip6addr,
                    sizeof(pAddr1->un.ip6addr)) == 0;

    case RLIM_ADDRTYPE_INTERFACE:
      return NIM_CONFIG_ID_IS_EQUAL(&pAddr1->un.intfConfigId,
                                    &pAddr2->un.intfConfigId);
    default:
      break;
  }

  return L7_FALSE;
}
