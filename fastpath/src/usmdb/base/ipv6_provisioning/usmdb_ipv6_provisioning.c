/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename usmdb_ipv6_provisioning.c
*
* @purpose Contains LVL7 ipv6 provisioning function usmdb APIs
*
* @component 
*
* @comments 
*
* @create 09/16/2003
*
* @author nshrivastav
* @end
*
**********************************************************************/


#include "l7_common.h"
#include "usmdb_ipv6_provisioning.h"

#include "osapi.h"
#include "nimapi.h"
#include "ipv6_provisioning_api.h"



/*********************************************************************
* @purpose To enable/disable the bridging of packets between two ports
*          or copying the packets to user application
*
* @param   UnitIndex   @b((input)) The unit for this operation
*          srcIfNum    @b{(input)} The internal interface number from which
*                                  the IPv6 packet must be copied
*          dstIfNum    @b{(input)} The internal interface number to which
*                                  IPv6 packets must be copied to
*          mode        @b{(input)} L7_ENABLE or L7_DISABLE
*
*          copyToCpu   @b{(input)} L7_ENABLE - To enable copying of packets
*                                  to CPU when bridging between ports
*                                  L7_DISABLE - Copying of packets to CPU
*                                  will not be enabled while bridging between
*                                  ports
*
*
* @returns  L7_SUCCESS
*           L7_FAILURE
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbipv6ProvBridgeSrcIfNumDstIfNum(L7_uint32 UnitIndex, L7_uint32 srcIfNum, 
                                            L7_uint32 dstIfNum, L7_uint32 mode, L7_uint32 copyToCpu)
{
  return ipv6ProvBridgeSrcIfNumDstIfNum(srcIfNum, dstIfNum, mode, copyToCpu);
}

/*********************************************************************
* @purpose Return the internal interface number to which IPv6 packets are
*          being bridged from an interface
*
*
* @param    UnitIndex   @b((input)) The unit for this operation
*           srcIfNum    @b{(input)}  The source interface number
*           dstIfNum    @b{(output)} The destination interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t usmDbipv6ProvGetDstIfNum(L7_uint32 UnitIndex, L7_uint32 srcIfNum, L7_uint32 *dstIfNum)
{
  return ipv6ProvGetDstIfNum(srcIfNum, dstIfNum); 
}

/*********************************************************************
* @purpose Returns the total number of IPv6 packets passed to application
*
*
* @param    UnitIndex   @b((input)) The unit for this operation
*
* @returns  Number of IPv6 packets passed to application component
*
* @notes    none
*
* @end
*********************************************************************/
L7_uint32 usmDbipv6ProvGetStats(void)
{
  return ipv6ProvGetStats();
}

