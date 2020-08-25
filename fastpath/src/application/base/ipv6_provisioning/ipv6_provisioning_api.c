/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename ipv6_provisioning_api.c
*
* @purpose Contains API definitions to support IPv6 provisioning
*
* @component ipv6_provisioning
*
* @comments
*
* @create 09/02/2003
*
* @author nshrivastav
* @end
*
**********************************************************************/



#include "l7_common.h"
#include "osapi.h"
#include "sysapi.h"
#include "nvstoreapi.h"
#include "nimapi.h"
#include "defaultconfig.h"
#include "log.h"
#include "dtlapi.h"
#include "ipv6_provisioning_config.h"
#include "ipv6_provisioning_api.h"
#include "ipv6_provisioning_debug.h"

extern ipv6ProvCfgData_t *ipv6ProvCfgData;
extern L7_uint32 *ipv6ProvMapTbl;
extern L7_uint32 numIPv6Pkts;



/*********************************************************************
* @purpose To enable/disable the bridging of packets between two ports
*          or copying the packets to user application
*
* @param   srcIfNum         @b{(input)} The internal interface number from
*                                       which the IPv6 packet must be copied
*          dstIfNum         @b{(input)} The internal interface number to which
*                                       IPv6 packets must be copied to
*          mode             @b{(input)} L7_ENABLE or L7_DISABLE
*
*          copyToCpu        @b{(input)} L7_ENABLE - To enable copying of packets
*                                       to CPU when bridging between ports
*                                       L7_DISABLE - Copying of packets to CPU
*                                       will not be enabled while bridging
*                                       between ports
*
*
* @returns  L7_SUCCESS
*           L7_FAILURE
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipv6ProvBridgeSrcIfNumDstIfNum(L7_uint32 srcIfNum, L7_uint32 dstIfNum,
                                       L7_uint32 mode, L7_uint32 copyToCpu)
{
  L7_uint32 srcIntfType, dstIntfType, rc, cfgIndex;

  if ((nimGetIntfType(srcIfNum, &srcIntfType) != L7_SUCCESS) ||
      (srcIntfType != L7_PHYSICAL_INTF))
    return L7_FAILURE;

  if ((nimGetIntfType(dstIfNum, &dstIntfType) != L7_SUCCESS) ||
     ((dstIntfType != L7_PHYSICAL_INTF) && (dstIntfType != L7_CPU_INTF)))
    return L7_FAILURE;

  if (((mode != L7_ENABLE) && (mode != L7_DISABLE)) ||
      ((copyToCpu != L7_ENABLE) && (copyToCpu != L7_DISABLE)))
    return L7_FAILURE;

  cfgIndex = ipv6ProvMapTbl[srcIfNum];

  if (nimConfigIdGet(dstIfNum, &(ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6DstId)) != L7_SUCCESS)
    return L7_FAILURE;

  ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6Mode = mode;
  ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6copyToCpu = copyToCpu;
  ipv6ProvCfgData->cfgHdr.dataChanged = L7_TRUE;

  rc = dtlIpv6ProvConfig(srcIfNum, dstIfNum, mode, copyToCpu);
  if (rc == L7_FAILURE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(srcIfNum, L7_SYSNAME, ifName);

    L7_LOGF(L7_LOG_SEVERITY_NOTICE, L7_IPV6_PROVISIONING_COMPONENT_ID,
            " Failure setting ipv6 provisioning mode on interface %s\n", ifName);
  }
  return rc;
}

/*********************************************************************
* @purpose Return the internal interface number to which IPv6 packets are
*          being forwarded from an interface
*
*
* @param    srcIfNum    @b{(input)}  The source interface number
*           dstIfNum    @b{(output)} The destination interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t ipv6ProvGetDstIfNum(L7_uint32 srcIfNum, L7_uint32 *dstIfNum)
{
  L7_uint32 srcIntfType;
  L7_uint32 cfgIndex;

  if ((nimGetIntfType(srcIfNum, &srcIntfType) == L7_SUCCESS) &&
      (srcIntfType == L7_PHYSICAL_INTF))
  {
    cfgIndex = ipv6ProvMapTbl[srcIfNum];

    if (nimIntIfFromConfigIDGet(&(ipv6ProvCfgData->ipv6PortMappings[cfgIndex].ipv6DstId),
                                dstIfNum) == L7_SUCCESS)
    {
      return L7_SUCCESS;
    }
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose Returns the total number of IPv6 packets passed to application
*
*
* @param    none
*
* @returns  Number of IPv6 packets passed to application component
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 ipv6ProvGetStats(void)
{
  return numIPv6Pkts;
}

/*********************************************************************
* @purpose clears the IPv6 packet statistics
*
*
* @param    none
*
* @returns
*
* @notes    none
*
* @end
*********************************************************************/
void ipv6ProvClearStats(void)
{
  numIPv6Pkts = 0;
}

/*********************************************************************
* @purpose  Determine if the interface type is valid to participate in IPv^ Provisioning
*
* @param    sysIntfType              @b{(input)} interface type
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL ipv6ProvIsValidIntfType(L7_uint32 sysIntfType)
{
  if (sysIntfType != L7_PHYSICAL_INTF)
    return L7_FALSE;

  return L7_TRUE;
}
/*********************************************************************
* @purpose  Determine if the interface is valid to participate in IPv^ Provisioning
*
* @param    intIfNum              @b{(input)} internal interface number
*
* @returns  L7_TRUE
* @returns  L7_FALSE
*
* @comments
*
* @end
*********************************************************************/
L7_BOOL ipv6ProvIsValidIntf(L7_uint32 intIfNum)
{
  L7_INTF_TYPES_t sysIntfType;

  if (nimGetIntfType(intIfNum, &sysIntfType) != L7_SUCCESS)
    return L7_FALSE;

  return ipv6ProvIsValidIntfType(sysIntfType);
}
