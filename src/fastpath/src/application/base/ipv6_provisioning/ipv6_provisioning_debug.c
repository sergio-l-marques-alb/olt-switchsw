/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename ipv6_provisioning_debug.c
*
* @purpose IPv6 Provisioning feature debug utilities
*
* @component ipv6_provisioning
*
* @comments 
*
* @create 09/04/2003
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




/*********************************************************************
* @purpose  List the Debug commands available
*
* @param   
*
* @returns  L7_SUCCESS
*
* @notes    devshell command
*       
* @end
*********************************************************************/
L7_RC_t ipv6ProvDebugHelp(void)
{
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "Available IPv6 Debug commands:\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "------------------------------\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "L7_RC_t ipv6ProvDebugInit()\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "L7_RC_t ipv6ProvDebugCopyToCpu(L7_uint32 unit, L7_uint32 slot, L7_uint32 port, L7_uint32 mode)\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "L7_RC_t ipv6ProvDebugBridgeSrcUspDstUsp(L7_uint32 sunit, L7_uin32 sslot, L7_uint32 sport, L7_uint32 dunit, L7_uint32 dslot, L7_uint32 dport, L7_uint32 mode, L7_uint32 copyToCpu)\r\n"); 
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "L7_uint32 ipv6ProvDebugGetStats()\r\n");
  SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                "void ipv6ProvDebugClearStats()\r\n");

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Enable/Disable copying of packets to CPU from a USP
*
* @param    unit  @b{(input)} unit of the box
*           slot  @b{(input)} slot of the box
*           port  @b{(input)} port of the box
*
*
* @returns  L7_SUCCESS  
* @returns  L7_FAILURE
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ipv6ProvDebugCopyToCpu(L7_uint32 unit, L7_uint32 slot, L7_uint32 port, L7_uint32 mode)
{
  L7_uint32 rc,i, cpuIfNum, srcIfNum, maxIfNum;
  L7_uint32 dstIntfType;
  nimUSP_t nUsp;

  maxIfNum = platIntfTotalMaxCountGet();

  for (i = 1; i <= maxIfNum; i++)
  {
    if ((nimGetIntfType(i, &dstIntfType) == L7_SUCCESS) &&
        (dstIntfType == L7_CPU_INTF))
      break;
  }
  if (i > maxIfNum)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      " Could not find a CPU interface\n");
    return L7_FAILURE;
  }

  cpuIfNum = i;
  nUsp.unit = unit;
  nUsp.slot = slot;
  nUsp.port = port;

  if (nimGetIntIfNumFromUSP(&nUsp, &srcIfNum) == L7_SUCCESS)
  {
    rc = ipv6ProvBridgeSrcIfNumDstIfNum(srcIfNum, cpuIfNum, mode, 0);
    return rc;
  }
  else
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      " Could not find Source interface number from usp\n");
    return L7_FAILURE;
  }

}

/*********************************************************************
* @purpose  Enable/Disable bridging of packets between srcUsp and dstUsp
*
* @param    sunit       @b{(input)} source unit of the box
*           sslot       @b{(input)} source slot of the box
*           sport       @b{(input)} source port of the box
*           dunit       @b{(input)} dest unit of the box
*           dslot       @b{(input)} dest slot of the box
*           dport       @b{(input)} dest port of the box
*           mode        @b{(input)} L7_ENABLE or L7_DISABLE
*           copyToCpu   @b{(input)} L7_ENABLE or L7_DISABLE
*
*
* @returns  L7_SUCCESS 
* @returns  L7_FAILURE 
*
* @notes    none
*       
* @end
*********************************************************************/
L7_RC_t ipv6ProvDebugBridgeSrcUspDstUsp(L7_uint32 sunit, L7_uint32 sslot, L7_uint32 sport, 
                                        L7_uint32 dunit, L7_uint32 dslot, L7_uint32 dport, 
                                        L7_uint32 mode, L7_uint32 copyToCpu)
{
  L7_uint32 rc, srcIfNum, dstIfNum ;
  nimUSP_t nUsp;

  nUsp.unit = sunit;
  nUsp.slot = sslot;
  nUsp.port = sport;

  if (nimGetIntIfNumFromUSP(&nUsp, &srcIfNum) != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      " Could not find Source interface number from usp\n");
    return L7_FAILURE;
  }
  nUsp.unit = dunit;
  nUsp.slot = dslot;
  nUsp.port = dport;

  if (nimGetIntIfNumFromUSP(&nUsp, &dstIfNum) != L7_SUCCESS)
  {
    SYSAPI_PRINTF(SYSAPI_APPLICATION_LOGGING_ALWAYS,
                      " Could not find Dst interface number from usp\n");
    return L7_FAILURE;
  }

  rc = ipv6ProvBridgeSrcIfNumDstIfNum(srcIfNum, dstIfNum, mode, copyToCpu);
  return rc;
}


/*********************************************************************
* @purpose  Initialize debug data structures
*
* @param    
*
*
* @returns  L7_SUCCESS 
*
* @notes    none
*       
* @end
*********************************************************************/
void ipv6ProvDebugInit()
{
  return ;
}

/*********************************************************************
* @purpose   Get the IPv6 statistics
*
* @param    
*
*
* @returns  Number of IPv6 packets received
*
* @notes    none
*       
* @end
*********************************************************************/
L7_uint32 ipv6ProvDebugGetStats(void)
{
  return ipv6ProvGetStats();
}

/*********************************************************************
* @purpose   Clear the IPv6 statistics
*
* @param    
*
*
* @returns  
*
* @notes    none
*       
* @end
*********************************************************************/
void ipv6ProvDebugClearStats(void)
{
  ipv6ProvClearStats();
  return;
}
