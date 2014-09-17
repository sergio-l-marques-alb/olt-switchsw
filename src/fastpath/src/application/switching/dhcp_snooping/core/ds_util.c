/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2005-2007
*
**********************************************************************
* @filename  ds_util.c
*
* @purpose   DHCP snooping APIs
*
* @component DHCP snooping
*
* @comments none
*
* @create 3/15/2007
*
* @author Rob Rice (rrice)
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "usmdb_unitmgr_api.h"
#include "usmdb_util_api.h"
#include "osapi.h"
#include "osapi_support.h"
#include "log.h"
#include "nimapi.h"
#include "comm_mask.h"
#include "sysnet_api_ipv4.h"
#include "sysnet_api_ipv6.h"
#include <stdlib.h>

#include "ds_util.h"
#include "ds_cfg.h"
#include "ds_cnfgr.h"

#ifdef L7_IPSG_PACKAGE
#include "ds_ipsg.h"
#endif

#ifdef L7_NSF_PACKAGE
#include "ds_ckpt.h"
#endif

#include "dot1q_api.h"
#include "dhcp_snooping_api.h"

#include "usmdb_sim_api.h"
#include "nvstoreapi.h"
#include "unitmgr_api.h"

extern dsCfgData_t *dsCfgData;
extern dsDbCfgData_t dsDbCfgData;

extern dsCnfgrState_t dsCnfgrState;
extern void *Ds_Event_Queue;
extern void *Ds_Packet_Queue;
extern void *dsMsgQSema;
extern L7_BOOL downloadFlag;

/* Global status data for DHCP snooping*/
dsInfo_t *dsInfo = NULL;

#ifdef L7_IPSG_PACKAGE
/* Global status data for IPSG entries */
extern ipsgInfo_t *ipsgInfo;
#endif

/* Array of per interface working data. Indexed on internal interface number */
dsIntfInfo_t *dsIntfInfo = NULL;

L7_BOOL txtBasedConfigComplete = L7_FALSE;

extern L7_uchar8 *dsInitStateNames[];

/*to check whether it's DHCP or BootP Packet  */
L7_uchar8 DS_DHCP_OPTIONS_COOKIE[]= {99, 130, 83, 99, 255};

#define DS_TRACE_LEN_MAX 255

extern L7_RC_t dsCheckpointCallback(dsCkptEventType_t dsEvent, L7_enetMacAddr_t *macAddr);

/*********************************************************************
* @purpose  Note that configuration data has changed
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsConfigDataChange(void)
{
  dsInfo->cfgDataChanged = L7_TRUE;
}

/*********************************************************************
* @purpose  Note that configuration data is now in sync with persistent
*           storage.
*
* @param    void
*
* @returns  void
*
* @notes    none
*
* @end
*********************************************************************/
void dsCfgDataNotChanged(void)
{
  dsInfo->cfgDataChanged = L7_FALSE;
  dsInfo->dsDbDataChanged = L7_FALSE;
}

/*********************************************************************
* @purpose  Apply a change to the global DHCP config state
*
* @param    adminMode   @b((input)) DHCP snooping admin mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsAdminModeApply(L7_uint32 adminMode)
{
  if (adminMode == L7_ENABLE)
    return dsAdminModeEnable();
  else
    return dsAdminModeDisable(L7_TRUE);
}
#ifdef L7_DHCP_L2_RELAY_PACKAGE
/*********************************************************************
* @purpose  Apply a change to the global DHCP L2Relay config state
*
* @param    adminMode   @b((input)) DHCP L2 Relay admin mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    PTIN - We need to register for both unicast and multicast
*           messages, as DHCPv6 is multicast in upstream and unicast
*           in downstream.
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayAdminModeApply(L7_uint32 adminMode)
{
  L7_uint32 i;
  sysnetPduIntercept_t sysnetPduIntercept, unicastdhcpv6SysnetPduIntercept, multicastdhcpv6SysnetPduIntercept;

  /* PTin modified - DHCPv6 */
  if (dsCfgData->dsGlobalAdminMode == L7_DISABLE)
  {

    /* Intercept IP packets at DTL layer. This is before DHCP relay or
     * DHCP snooping get a crack at the packet in a routing build. */

     /*IPv4*/
    sysnetPduIntercept.addressFamily = L7_AF_INET;
    sysnetPduIntercept.hookId = SYSNET_INET_RECV_IN;
    sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_DHCP_SNOOPING_PRECEDENCE;
    sysnetPduIntercept.interceptFunc = dsPacketIntercept;
    osapiStrncpy(sysnetPduIntercept.interceptFuncName, "dsPacketIntercept", SYSNET_MAX_FUNC_NAME);

    /*IPv6 unicast*/
    unicastdhcpv6SysnetPduIntercept.addressFamily = L7_AF_INET6;
    unicastdhcpv6SysnetPduIntercept.hookId = SYSNET_INET6_RECV_IN;
    unicastdhcpv6SysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_DHCP_SNOOPING_PRECEDENCE;
    unicastdhcpv6SysnetPduIntercept.interceptFunc = dsv6PacketIntercept;
    osapiStrncpy(unicastdhcpv6SysnetPduIntercept.interceptFuncName, "dsv6PacketIntercept", SYSNET_MAX_FUNC_NAME);

    /*IPv6 multicast*/
    unicastdhcpv6SysnetPduIntercept.addressFamily = L7_AF_INET6;
    unicastdhcpv6SysnetPduIntercept.hookId = SYSNET_INET6_MCAST_IN;
    unicastdhcpv6SysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_DHCP_SNOOPING_PRECEDENCE;
    unicastdhcpv6SysnetPduIntercept.interceptFunc = dsv6PacketIntercept;
    osapiStrncpy(unicastdhcpv6SysnetPduIntercept.interceptFuncName, "dsv6PacketIntercept", SYSNET_MAX_FUNC_NAME);

    if (adminMode == L7_ENABLE)
    {
      if ((sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS) &&
          (sysNetPduInterceptRegister(&unicastdhcpv6SysnetPduIntercept) != L7_SUCCESS) &&
          (sysNetPduInterceptRegister(&multicastdhcpv6SysnetPduIntercept) != L7_SUCCESS))
      {
        if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_EXTERNAL_CALLS)
        {
          L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
          osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                        "Failed to register with Sysnet when L2relay is enabled.");
          dsTraceWrite(traceMsg);
        }
        return L7_FAILURE;
      }
    }
    else
    {
      (void)sysNetPduInterceptDeregister(&sysnetPduIntercept);
      (void)sysNetPduInterceptDeregister(&unicastdhcpv6SysnetPduIntercept);
      (void)sysNetPduInterceptDeregister(&multicastdhcpv6SysnetPduIntercept);
    }
  }

  for (i = 1; i < DS_MAX_INTF_COUNT; i++)
  {
    if (_dsIntfL2RelayGet(i))
    {
      dsL2RelayIntfModeApply(i, adminMode);
    }
    dsIntfRateLimitApply(i);
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Apply a change to the global DHCP L2Relay config state
*
* @param    intIfNum   @b((input)) internal interface number
* @param    intfMode   @b((input)) DHCP L2 Relay interface mode
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    PTIN - We need to register for both unicast and multicast
*           messages, as DHCPv6 is multicast in upstream and unicast
*           in downstream.
*
* @end
*********************************************************************/
L7_RC_t dsL2RelayIntfModeApply(L7_uint32 intIfNum, L7_uint32 intfMode)
{
  if (dsIntfIsSnooping(intIfNum) == L7_TRUE)
  {
    return L7_SUCCESS;
  }
  if (intfMode == L7_ENABLE)
  {
    return dsIntfEnable(intIfNum);
  }
  else if (intfMode == L7_DISABLE)
  {
    return dsIntfDisable(intIfNum);
  }
  else
  {
    return L7_FAILURE;
  }
}
#endif
/*********************************************************************
* @purpose  Enable DHCP globally
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    PTIN - We need to register for both unicast and multicast
*           messages, as DHCPv6 is multicast in upstream and unicast
*           in downstream.
*
* @end
*********************************************************************/
L7_RC_t dsAdminModeEnable(void)
{
  L7_uint32 i;
  sysnetPduIntercept_t sysnetPduIntercept, unicastdhcpv6SysnetPduIntercept, multicastdhcpv6SysnetPduIntercept;
#ifdef L7_DHCP_L2_RELAY_PACKAGE
  /* PTin modified - DHCPv6 */
  if (dsCfgData->dsL2RelayAdminMode != L7_ENABLE)
  {
#endif
    /* Intercept IP packets at DTL layer. This is before DHCP relay or
     * DHCP snooping get a crack at the packet in a routing build. */

     /*IPv4*/
     sysnetPduIntercept.addressFamily = L7_AF_INET; //L7_AF_INET;
     sysnetPduIntercept.hookId = SYSNET_INET_RECV_IN; //SYSNET_INET_RECV_IN;
     sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_DHCP_SNOOPING_PRECEDENCE; /*FD_SYSNET_HOOK_DHCP_SNOOPING_PRECEDENCE*/;
     sysnetPduIntercept.interceptFunc = dsPacketIntercept;
     strcpy(sysnetPduIntercept.interceptFuncName, "dsPacketIntercept");

     /*IPv6 unicast*/
     unicastdhcpv6SysnetPduIntercept.addressFamily = L7_AF_INET6; //L7_AF_INET;
     unicastdhcpv6SysnetPduIntercept.hookId = SYSNET_INET6_RECV_IN; //SYSNET_INET6_MCAST_IN;
     unicastdhcpv6SysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_DHCP_SNOOPING_PRECEDENCE; /*FD_SYSNET_HOOK_DHCP_SNOOPING_PRECEDENCE*/;
     unicastdhcpv6SysnetPduIntercept.interceptFunc = dsv6PacketIntercept;
     strcpy(unicastdhcpv6SysnetPduIntercept.interceptFuncName, "dsv6PacketIntercept");

     /*IPv6 multicast*/
     multicastdhcpv6SysnetPduIntercept.addressFamily = L7_AF_INET6; //L7_AF_INET;
     multicastdhcpv6SysnetPduIntercept.hookId = SYSNET_INET6_MCAST_IN; //SYSNET_INET6_MCAST_IN;
     multicastdhcpv6SysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_DHCP_SNOOPING_PRECEDENCE; /*FD_SYSNET_HOOK_DHCP_SNOOPING_PRECEDENCE*/;
     multicastdhcpv6SysnetPduIntercept.interceptFunc = dsv6PacketIntercept;
     strcpy(multicastdhcpv6SysnetPduIntercept.interceptFuncName, "dsv6PacketIntercept");

     if (sysNetPduInterceptRegister(&sysnetPduIntercept) != L7_SUCCESS ||
           sysNetPduInterceptRegister(&unicastdhcpv6SysnetPduIntercept) != L7_SUCCESS ||
           sysNetPduInterceptRegister(&multicastdhcpv6SysnetPduIntercept) != L7_SUCCESS)
      return L7_FAILURE;
#ifdef L7_DHCP_L2_RELAY_PACKAGE
  }
#endif
  for (i = 0; i < DS_MAX_INTF_COUNT; i++)
  {
    if (dsIntfIsSnooping(i))
    {
      dsIntfSnoopingApply(i, L7_ENABLE);
      dsIntfRateLimitApply(i);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Disable DHCP snooping globally.
*
* @param    clearBindings @b{(input)} Flag to indicate clear action
*                                     on bindings
*
* @returns  L7_SUCCESS
*
* @notes    none  clearBindings would be L7_FALSE during warm Restart
*
* @end
*********************************************************************/
L7_RC_t dsAdminModeDisable(L7_BOOL clearBindings)
{
  L7_uint32 i;
  sysnetPduIntercept_t sysnetPduIntercept, unicastdhcpv6SysnetPduIntercept, multicastdhcpv6SysnetPduIntercept;

#ifdef L7_DHCP_L2_RELAY_PACKAGE
  /* PTin modified - DHCPv6 */
  if (dsCfgData->dsL2RelayAdminMode == L7_DISABLE)
  {
#endif

     /*IPv4*/
     sysnetPduIntercept.addressFamily = L7_AF_INET;
     sysnetPduIntercept.hookId = SYSNET_INET_RECV_IN;
     sysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_DHCP_SNOOPING_PRECEDENCE;
     sysnetPduIntercept.interceptFunc = dsPacketIntercept;
     strcpy(sysnetPduIntercept.interceptFuncName, "dsPacketIntercept");
     (void)sysNetPduInterceptDeregister(&sysnetPduIntercept);

     /*IPv6 unicast*/
     unicastdhcpv6SysnetPduIntercept.addressFamily = L7_AF_INET6;
     unicastdhcpv6SysnetPduIntercept.hookId = SYSNET_INET6_RECV_IN;
     unicastdhcpv6SysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_DHCP_SNOOPING_PRECEDENCE;
     unicastdhcpv6SysnetPduIntercept.interceptFunc = dsv6PacketIntercept;
     strcpy(unicastdhcpv6SysnetPduIntercept.interceptFuncName, "dsv6PacketIntercept");
     (void)sysNetPduInterceptDeregister(&unicastdhcpv6SysnetPduIntercept);

     /*IPv6 multicast*/
     multicastdhcpv6SysnetPduIntercept.addressFamily = L7_AF_INET6;
     multicastdhcpv6SysnetPduIntercept.hookId = SYSNET_INET6_MCAST_IN;
     multicastdhcpv6SysnetPduIntercept.hookPrecedence = FD_SYSNET_HOOK_DHCP_SNOOPING_PRECEDENCE;
     multicastdhcpv6SysnetPduIntercept.interceptFunc = dsv6PacketIntercept;
     strcpy(multicastdhcpv6SysnetPduIntercept.interceptFuncName, "dsv6PacketIntercept");
     (void)sysNetPduInterceptDeregister(&multicastdhcpv6SysnetPduIntercept);

#ifdef L7_DHCP_L2_RELAY_PACKAGE
  }
#endif

  if(clearBindings)
  {
  /* Clear the all dynamic bindings for all interfaces */
  _dsBindingClear(0,L7_FALSE);
  }

  /* Clear statistics */
  _dsIntfStatsClear();

  for (i = 0; i < DS_MAX_INTF_COUNT; i++)
  {
    if (dsIntfIsSnooping(i))
    {
      dsIntfRateLimitApply(i);
      dsIntfSnoopingApply(i, L7_DISABLE);
    }
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Clear DHCP snooping bindings.
*
* @param    intIfNum  @b((input))  If non-zero, only bindings associated
*                                  with a specific interface are cleared.
*           staticBindingsClrFlag   @b((input))
*                                  If set , static entries also will be
*                                  cleared
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t _dsBindingClear(L7_uint32 intIfNum,
                        L7_BOOL staticBindingsClrFlag)
{
  L7_RC_t rc = L7_SUCCESS;
  L7_RC_t retval = L7_SUCCESS;
  L7_enetMacAddr_t prevMac;
  L7_uint32 prevIntf;
  dsBindingType_t prevType;
  dhcpSnoopBinding_t dsBinding;

  memset(&prevMac, 0, sizeof(prevMac));
  memset(&dsBinding, 0, sizeof(dsBinding));

#ifdef L7_NSF_PACKAGE
  if ((cnfgrIsFeaturePresent(L7_FLEX_STACKING_COMPONENT_ID, L7_STACKING_NSF_FEATURE_ID)) &&
      (intIfNum == 0))
  {
    /* Send a flush all checkpoint event */
    (void)dsCheckpointCallback(DS_ENTRY_FLUSH_ALL, &prevMac);
  }
#endif

  rc = dsBindingFind(&dsBinding, L7_MATCH_GETNEXT);
  while (rc == L7_SUCCESS)
  {
    memcpy(&prevMac, &dsBinding.key.macAddr, sizeof(L7_enetMacAddr_t));
    prevIntf = dsBinding.intIfNum;
    prevType = dsBinding.bindingType;

    /* Get next before deleting previous */
    rc = dsBindingFind(&dsBinding, L7_MATCH_GETNEXT);


    if (!intIfNum || (intIfNum == prevIntf))
    {

     if ( (staticBindingsClrFlag == L7_FALSE) &&
         (prevType == DS_BINDING_STATIC) )
     {
        continue;
     }
      /* Delete this binding */
      dsBindingTreeKey_t key;
      memset(&key, 0x00, sizeof(key));
      memcpy(&key.macAddr.addr, &prevMac.addr, L7_ENET_MAC_ADDR_LEN);
      if (dsBindingRemove(&key) != L7_SUCCESS)
      retval = L7_FAILURE;
      dsInfo->debugStats.bindingsRemoved++;
    }
  }

  return retval;
}

/*********************************************************************
* @purpose  Clear DHCP snooping interface statistics for all interfaces.
*
* @param    void
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t _dsIntfStatsClear(void)
{
  L7_uint32 i;

  for (i = 1; i < DS_MAX_INTF_COUNT; i++)
  {
    dsIntfInfo[i].dsIntfStats.macVerify = 0;
    dsIntfInfo[i].dsIntfStats.intfMismatch = 0;
    dsIntfInfo[i].dsIntfStats.untrustedSvrMsg = 0;
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Apply a change to a VLAN's enable status for DHCP snooping
*
* @param    vlanId    @b((input)) VLAN ID
* @param    vlanCfg   @b((input)) L7_ENABLE if DHCP snooping enabled on VLAN
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsVlanEnableApply(L7_uint32 vlanId, L7_uint32 vlanCfg)
{
  NIM_INTF_MASK_t portMask;
  L7_uint32 i;

  /* For each port in the VLAN, update the VLAN enabled count. */
  if (dot1qVlanEgressPortsGet(vlanId, &portMask) == L7_SUCCESS)
  {
    for (i = 1; i < DS_MAX_INTF_COUNT; i++)
    {
      if (L7_INTF_ISMASKBITSET(portMask, i))
      {
        L7_BOOL intfIsSnooping = dsIntfIsSnooping(i);
        if (vlanCfg == L7_ENABLE)
        {
          L7_VLAN_SETMASKBIT (dsIntfInfo[i].dsVlans, vlanId);
          dsIntfInfo[i].dsNumVlansEnabled++;
          if ((dsCfgData->dsGlobalAdminMode == L7_ENABLE) &&
              !intfIsSnooping && dsIntfIsSnooping(i))
          {
            dsIntfSnoopingApply(i, L7_ENABLE);
          }
        }
        else
        {
          dsIntfInfo[i].dsNumVlansEnabled--;
          L7_VLAN_CLRMASKBIT (dsIntfInfo[i].dsVlans, vlanId);
          if (intfIsSnooping && !dsIntfIsSnooping(i))
          {
            dsIntfSnoopingApply(i, L7_DISABLE);
          }
        }
      }
    }
    return L7_SUCCESS;
  }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Apply a change to an interface's trust for DHCP snooping
*
* @param    intIfNum   @b((input)) internal interface number
* @param    trust      @b((input)) L7_TRUE if interface is trusted for DHCP snooping
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsIntfTrustApply(L7_uint32 intIfNum, L7_BOOL trust)
{
  /* If port has become trusted, clear all dynamic bindings on port */
  if (trust)
  {
    _dsBindingClear(intIfNum, L7_FALSE);
  }

  /* This is code is commented as :
     With the new support for L2 relay, all the DHCP messages from trusted
     or untrusted are trapped to CPU and switched in application. Earlier the
     packets for trusted interfaces are switched in hardware, which is removed
     now. */
  #if 0
  if (dsIntfIsSnooping(intIfNum))
  {
    /* Enable DHCP snooping on port in hardware. */
    if (dtlDhcpSnoopingConfig(intIfNum, L7_TRUE, trust) != L7_SUCCESS)
    {
      L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
      nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
      L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
              "DHCP snooping failed to set interface %s trust in hardware.",
              ifName);
      return L7_FAILURE;
    }
  }
  #endif
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine whether DHCP snooping is a member of a VLAN
*           where DHCP snooping is enabled.
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_SUCCESS
*
* @notes    DHCP snooping is considered enabled on a port if the port
*           is a member of one or more VLANs where DHCP snooping is enabled.
*
* @end
*********************************************************************/
L7_BOOL dsIntfIsSnooping(L7_uint32 intIfNum)
{
  if (intIfNum > DS_MAX_INTF_COUNT)
    return L7_FALSE;
  if (dsIntfInfo[intIfNum].dsNumVlansEnabled > 0)
    return L7_TRUE;

    return L7_FALSE;
}

/*********************************************************************
* @purpose  Enable DHCP snooping on a port.
*
* @param    intIfNum   @b((input)) internal interface number
* @param    intfMode   @b((input)) DHCP Snooping interface mode
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsIntfSnoopingApply(L7_uint32 intIfNum, L7_uint32 intfMode)
{
#ifdef L7_DHCP_L2_RELAY_PACKAGE
  if (_dsIntfL2RelayGet(intIfNum) == L7_TRUE)
  {
    return L7_SUCCESS;
  }
#endif
  if (intfMode == L7_ENABLE)
  {
    return dsIntfEnable(intIfNum);
  }
  else if (intfMode == L7_DISABLE)
  {
    return dsIntfDisable(intIfNum);
  }
  else
  {
    return L7_FAILURE;
  }
}
/*********************************************************************
* @purpose  Enables DHCP packet reception on a port.
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsIntfEnable(L7_uint32 intIfNum)
{
  if (dsCfgData->dsTraceFlags & DS_TRACE_PORT_STATE)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "Enabling DHCP snooping on %s port.",
                  ifName);
    dsTraceWrite(traceMsg);
  }
  /* Enable DHCP snooping on port in hardware. */
  if (dtlDhcpSnoopingConfig(intIfNum, L7_TRUE) != L7_SUCCESS)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
            "Failed to enable DHCP snooping in hardware on interface %s (intIfNum=%u)",
            ifName,intIfNum);
    return L7_FAILURE;
  }
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Disables DHCP packet reception on a port.
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsIntfDisable(L7_uint32 intIfNum)
{
  if (dsCfgData->dsTraceFlags & DS_TRACE_PORT_STATE)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "Disabling DHCP snooping on port %s.",
                  ifName);
    dsTraceWrite(traceMsg);
  }

  /* Remove all dynamic bindings associated with this interface. */
  _dsBindingClear(intIfNum,L7_FALSE );

  if (DHCP_SNOOP_IS_READY)
  {
    /* Disable DHCP snooping on port in hardware. */
    if (dtlDhcpSnoopingConfig(intIfNum, L7_FALSE) != L7_SUCCESS)
    {
     L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
     nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
     L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
            "Failed to disable DHCP snooping in hardware on interface (%d) %s",
            intIfNum, ifName);
     return L7_FAILURE;
   }
  }
  return L7_SUCCESS;
}


#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
#ifdef L7_DHCP_L2_RELAY_PACKAGE
/*********************************************************************
* @purpose  Gets the subscription entry on an interface
*
* @param    intIfNum           @b((input)) internal interface number
* @param    subscriptionName   @b((input)) subscription name
* @param    exactFlag          @b((input)) if true, returns success only if the entry exists,
*                                          if false, returns suceess either if the entry exists
*                                          or any free entry exists.
* @param    subsCfg            @b((output)) subscription onfig entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsL2RelaySubscriptionEntryGet(L7_uint32 intIfNum,
                                      L7_uchar8 *subscriptionName,
                                      L7_BOOL exactFlag,
                                      dsL2RelaySubscriptionCfg_t **subsCfg)
{
  dsL2RelaySubscriptionCfg_t *subsCfgTemp, *subsCfgFreeSlot = L7_NULLPTR;
  L7_uint32 i = 0;
  L7_BOOL freeFound = L7_FALSE;
  L7_uchar8 eoStr = L7_EOS;

  if (dsCfgData->dsTraceFlags & DS_TRACE_OPTION82_CONFIG)
  {
    L7_uchar8 traceMsg[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceMsg, DS_MAX_TRACE_LEN,
                  "Entered with intIfNum = %d , subscName = %s, exactFlag = %d",
                  intIfNum, subscriptionName, exactFlag);
    dsTraceWrite(traceMsg);
  }

  for (i = 0; i < L7_DOT1AD_SERVICES_MAX_PER_INTF; i++)
  {
    subsCfgTemp = &(dsCfgData->dsIntfCfg[intIfNum].dsL2RelaySubscrptionCfg[i]);
    if (exactFlag == L7_FALSE)
    {
      if (freeFound == L7_FALSE)
      {
        if(osapiStrncmp(subsCfgTemp->subscriptionName, &eoStr,
                        sizeof(L7_uchar8)) == L7_NULL)
        {
          freeFound = L7_TRUE;
          subsCfgFreeSlot = subsCfgTemp;
        }
      }
    }
    if (exactFlag == L7_TRUE &&
        (osapiStrncmp(subsCfgTemp->subscriptionName,
                   &(eoStr), sizeof(eoStr)) == L7_NULL))
    {
      continue;
    }
    if(osapiStrncmp(subscriptionName, subsCfgTemp->subscriptionName,
                    L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX) == L7_NULL)
    {
      *subsCfg = subsCfgTemp;
      return L7_SUCCESS;
    }
  }

  if ((exactFlag == L7_FALSE) && (freeFound == L7_FALSE))
  {
    return L7_TABLE_IS_FULL;
  }
  else if (exactFlag == L7_TRUE)
  {
    return L7_FAILURE;
  }
  *subsCfg = subsCfgFreeSlot;
  return L7_SUCCESS;
}
/*********************************************************************
* @purpose  Frees the subscription entry on an interface
*
* @param    subsCfg            @b((output)) subscription onfig entry
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    The entry is made to be a free slot if all the configuration
*           parameters under the entry are default. Making free slot means
*           erasing the subscription name.
*
* @end
*********************************************************************/
L7_RC_t dsL2RelaySubscriptionEntryFree(dsL2RelaySubscriptionCfg_t *subsCfg)
{
  L7_uchar8 eoStr = L7_EOS;

  if ((subsCfg->l2relay == FD_DHCP_L2RELAY_SUBSCRIPTION_MODE) &&
      (subsCfg->circuitId == FD_DHCP_L2RELAY_CIRCUIT_ID_SUBSCRIPTION_MODE) &&
      (osapiStrncmp(subsCfg->remoteId, FD_DHCP_L2RELAY_REMOTE_ID_SUBSCRIPTION_MODE,
                    DS_MAX_REMOTE_ID_STRING-1) == L7_NULL))
  {
    memset(subsCfg->subscriptionName, 0 , L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
    osapiStrncpy(subsCfg->subscriptionName, &eoStr, L7_DOT1AD_SUBSCRIPTION_NAME_LEN_MAX);
  }
  return L7_SUCCESS;
}
#endif
#endif

/*********************************************************************
* @purpose  Handle NIM startup callback
*
* @param    startupPhase     create or activate
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dsIntfStartupCallback(NIM_STARTUP_PHASE_t startupPhase)
{
  dsEventMsg_t   msg;
  L7_RC_t rc;

  if (dsCfgData->dsTraceFlags & DS_TRACE_NSF)
  {
    L7_uchar8 traceBuf[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceBuf, DS_MAX_TRACE_LEN,
                  "DS received NIM %s startup event",
                  startupPhase == NIM_INTERFACE_CREATE_STARTUP ? "CREATE" : "ACTIVATE");
    dsTraceWrite(traceBuf);
  }

  if (dsCnfgrState != DHCP_SNOOP_PHASE_EXECUTE)
  {
    nimStartupEventDone(L7_DHCP_SNOOPING_COMPONENT_ID);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
            "Received NIM startup %d callback while in init state %s",
            startupPhase, dsInitStateNames[dsCnfgrState]);
    return;
  }

  memset(&msg, 0, sizeof(dsEventMsg_t));
  msg.msgType = DS_NIM_STARTUP;
  msg.dsMsgData.nimStartupEvent.startupPhase = startupPhase;

  rc = osapiMessageSend(Ds_Event_Queue, &msg, sizeof(dsEventMsg_t),
                        L7_WAIT_FOREVER, L7_MSG_PRIORITY_NORM);
  if(rc == L7_SUCCESS)
  {
    osapiSemaGive(dsMsgQSema);
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_ALERT, L7_DHCP_SNOOPING_COMPONENT_ID,
            "Failed to send NIM startup phase %d to DHCP Snooping thread",
            startupPhase);
  }
}

/*********************************************************************
* @purpose  Do NIM create startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes    DHCP Snooping has no configuration at this point.
*
* @end
*********************************************************************/
L7_RC_t dsNimCreateStartup(void)
{
  L7_RC_t rc;
  L7_uint32 intIfNum;

  rc = nimFirstValidIntfNumber(&intIfNum);
  while (rc == L7_SUCCESS)
  {
    if (dsIntfIsValid(intIfNum))
    {
      dsIntfCreate(intIfNum);
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* Register with NIM to receive port CREATE and DELETE events */
  PORTEVENT_SETMASKBIT(dsInfo->nimEventMask, L7_CREATE);
  PORTEVENT_SETMASKBIT(dsInfo->nimEventMask, L7_DELETE);

  nimRegisterIntfEvents(L7_DHCP_SNOOPING_COMPONENT_ID, dsInfo->nimEventMask);

  nimStartupEventDone(L7_DHCP_SNOOPING_COMPONENT_ID);

  if (dsCfgData->dsTraceFlags & DS_TRACE_NSF)
  {
    L7_uchar8 traceBuf[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceBuf, DS_MAX_TRACE_LEN,
                  "DS has processed NIM create startup callback");
    dsTraceWrite(traceBuf);
  }

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Do NIM activate startup
*
* @param    void
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
L7_RC_t dsNimActivateStartup(void)
{
  L7_RC_t          rc;
  L7_uint32        intIfNum;
  L7_INTF_STATES_t intIfState;

  if (dsCfgData->dsTraceFlags & DS_TRACE_NSF)
  {
    L7_uchar8 traceBuf[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceBuf, DS_MAX_TRACE_LEN,
                  "DS beginning NIM activate startup");
    dsTraceWrite(traceBuf);
  }

  /* If this is a warm restart, apply checkpointed data first. */

  if (dsInfo->warmRestart)
  {
#ifdef L7_NSF_PACKAGE
    /* don't read the bindings from remote file or from local database.
     * Apply the checkpointed data */
    dsCheckpointDataApply();
#endif
  }
  else
  {
    /* In case of cold restart only, try to get it from remote file
     * or local file. */
    if(cnfgrIsFeaturePresent(L7_DHCP_SNOOPING_COMPONENT_ID,
                             L7_DHCP_SNOOPING_DB_PERSISTENCY) == L7_TRUE)
    {
      if (dsCfgData->dsDbIpAddr)
      {
        downloadFlag = L7_TRUE;
      }
      else
      {
        dsDbLocalRestore();
      }
    }
  }

  /* Now attach the valid interfaces. That will push down the
   * IPSG entries in the IPSG AVL tree into hardware */
  rc = nimFirstValidIntfNumber(&intIfNum);
  while (rc == L7_SUCCESS)
  {
    if (dsIntfIsValid(intIfNum))
    {
      intIfState = nimGetIntfState(intIfNum);

      switch (intIfState)
      {
        case L7_INTF_ATTACHED:
          dsIntfAttach(intIfNum);
          break;

        default:
          break;
      }
    }
    rc = nimNextValidIntfNumber(intIfNum, &intIfNum);
  }

  /* Set the flag that the text based configuration apply is complete.
   * This flag is used in dsDbLocalSave() */
  txtBasedConfigComplete = L7_TRUE;

  /* Now ask NIM to send any future changes for these event types */
  PORTEVENT_SETMASKBIT(dsInfo->nimEventMask, L7_CREATE);
  PORTEVENT_SETMASKBIT(dsInfo->nimEventMask, L7_DELETE);
  PORTEVENT_SETMASKBIT(dsInfo->nimEventMask, L7_ATTACH);
  PORTEVENT_SETMASKBIT(dsInfo->nimEventMask, L7_DETACH);

  nimRegisterIntfEvents(L7_DHCP_SNOOPING_COMPONENT_ID, dsInfo->nimEventMask);

  nimStartupEventDone(L7_DHCP_SNOOPING_COMPONENT_ID);

  if (dsCfgData->dsTraceFlags & DS_TRACE_NSF)
  {
    L7_uchar8 traceBuf[DS_MAX_TRACE_LEN];
    osapiSnprintf(traceBuf, DS_MAX_TRACE_LEN,
                  "DS completed processing NIM activate startup");
    dsTraceWrite(traceBuf);
  }

  /* Tell Configurator that hardware update by DHCP Snooping component is done */
  cnfgrApiComponentHwUpdateDone(L7_DHCP_SNOOPING_COMPONENT_ID, L7_CNFGR_HW_APPLY_CONFIG);

  dsInfo->l2HwUpdateComplete = L7_TRUE;

  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Update the current state of a given interface.
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsIntfChangeCallback(L7_uint32 intIfNum,
                             L7_uint32 event,
                             NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;
  dsEventMsg_t msg;

  status.intIfNum = intIfNum;
  status.component = L7_DHCP_SNOOPING_COMPONENT_ID;
  status.event = event;
  status.correlator = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED;

  if (nimPhaseStatusCheck() != L7_TRUE)
  {
    L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
            "DHCP snooping received an interface change callback for event %s"
            " on interface %s during invalid initialization phase.",
            nimGetIntfEvent(event), ifName);
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  if (  (event != L7_CREATE) &&
         (event != L7_DELETE) &&
          (event != L7_ATTACH)&&
         (event != L7_DETACH)
       ) /* No need to process any other NIM event than these  */
  {
    rc = L7_SUCCESS;
    status.response.rc = rc;
    nimEventStatusCallback(status);
    return rc;
  }

  memset(&msg, 0, sizeof(dsEventMsg_t) );

  msg.dsMsgData.intfEvent.intIfNum = intIfNum;
  msg.dsMsgData.intfEvent.event = event;
  msg.dsMsgData.intfEvent.correlator = correlator;
  msg.msgType    = DS_INTF_EVENT;

  if (Ds_Event_Queue == L7_NULLPTR)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
           "DS event queue is NULL when processing Interface change.");
    return L7_FAILURE;
  }

  if ( (rc = osapiMessageSend(Ds_Event_Queue, &msg, sizeof(dsEventMsg_t),
                        L7_NO_WAIT,
                        L7_MSG_PRIORITY_NORM )) != L7_SUCCESS)
  {
     L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_SNOOPING_COMPONENT_ID,
             "DS: NIM event Enqueue failed.\n");
  }
  if (rc == L7_SUCCESS)
  {
    osapiSemaGive(dsMsgQSema);
  }
  else
  {
    static L7_uint32 lastMsg = 0;
    dsInfo->debugStats.eventMsgTxError++;
    if (osapiUpTimeRaw() > lastMsg)
    {
      lastMsg = osapiUpTimeRaw();
    }
  }

  return rc;
}

/*********************************************************************
* @purpose  Initialize the interface data structures for DHCP Snooping
*
* @param    intIfnum   @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes
*
* @end
*********************************************************************/
void dsIntfCreate(L7_uint32 intIfNum)
{
      memset(&dsIntfInfo[intIfNum], 0,  sizeof(dsIntfInfo_t));
      dsIntfInfo[intIfNum].consecutiveInterval = 1;
      if (FD_DHCP_SNOOP_TRUST == L7_TRUE)
      {
        _dsIntfTrust(intIfNum);
      }
      else
      {
        _dsIntfDistrust(intIfNum);
      }
      if (FD_DHCP_SNOOP_LOG_INVALID_MSG == L7_TRUE)
      {
        _dsIntfLogInvalid(intIfNum);
      }
      else
      {
        _dsIntfDontLogInvalid(intIfNum);
      }
#ifdef  L7_DHCP_L2_RELAY_PACKAGE
      /* Update the L2 Relay config also.*/
  /* Update the L2 Relay config also.*/
      if (FD_DHCP_L2RELAY_INTERFACE_MODE == L7_TRUE)
      {
        _dsIntfL2RelayEnable(intIfNum);
      }
      else
      {
        _dsIntfL2RelayDisable(intIfNum);
      }
      if (FD_DHCP_L2RELAY_INTERFACE_TRUST_MODE == L7_TRUE)
      {
        _dsIntfL2RelayTrust(intIfNum);
      }
      else
      {
        _dsIntfL2RelayDistrust(intIfNum);
      }
#endif
#ifdef L7_IPSG_PACKAGE
      dsCfgData->ipsgIntfCfg[intIfNum].verifyIp = FD_IPSG_VERIFY_IP;
      dsCfgData->ipsgIntfCfg[intIfNum].verifyMac = FD_IPSG_VERIFY_MAC;
#endif
      dsCfgData->dsIntfCfg[intIfNum].rateLimitCfg.rate_limit     = FD_DHCP_SNOOPING_RATE_LIMIT;
      dsCfgData->dsIntfCfg[intIfNum].rateLimitCfg.burst_interval = FD_DHCP_SNOOPING_BURST_INTERVAL;
}

/*********************************************************************
* @purpose  Attach the interface for DHCP Snooping
*
* @param    intIfnum   @b{(input)} internal interface number
*
* @returns  L7_SUCCESS
*
* @notes
*
* @end
*********************************************************************/
void dsIntfAttach(L7_uint32 intIfNum)
{
#ifdef L7_IPSG_PACKAGE
      if (ipsgIsEnabled(intIfNum))
      {
        ipsgVerifySourceApply (intIfNum, dsCfgData->ipsgIntfCfg[intIfNum].verifyIp &&
                               dsCfgData->ipsgIntfCfg[intIfNum].verifyMac);
      }
#endif
}

/*********************************************************************
* @purpose  Process Intf event caught in dsIntfChangeCallback().
*
* @param    intIfnum   @b{(input)}   internal interface whose state has changed
* @param    event      @b{(input)}   Intf event (see L7_PORT for list)
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    Called on DS thread context.
*
* @end
*********************************************************************/
L7_RC_t dsIntfChangeProcess(L7_uint32 intIfNum, L7_uint32 event, NIM_CORRELATOR_t correlator)
{
  L7_RC_t rc = L7_SUCCESS;
  NIM_EVENT_COMPLETE_INFO_t status;

  status.intIfNum     = intIfNum;
  status.component    = L7_DHCP_SNOOPING_COMPONENT_ID;
  status.response.rc  = L7_SUCCESS;
  status.event        = event;
  status.correlator   = correlator;
  status.response.reason = NIM_ERR_RC_UNUSED;

  if (dsIntfIsValid(intIfNum) != L7_TRUE)
  {
    nimEventStatusCallback(status);
    return L7_SUCCESS;
  }
  if (DHCP_SNOOP_IS_READY)
  {
    switch (event)
    {
    case L7_CREATE:
      dsIntfCreate(intIfNum);
      break;

    case L7_DELETE:
      /* Remove all the Binding entries including Static */
      _dsBindingClear(intIfNum, L7_TRUE);
#ifdef L7_IPSG_PACKAGE
      _ipsgEntryClear (intIfNum);
#endif
      break;
    case L7_ATTACH:
      dsIntfAttach(intIfNum);
      break;

    case L7_DETACH:
      (void)dsIntfDisable(intIfNum);
#ifdef L7_IPSG_PACKAGE
      if (ipsgIsEnabled(intIfNum))
      {
        ipsgIntfDisableApply(intIfNum);
      }
#endif
      break;

    default:
      rc  = L7_SUCCESS; /* If its an event we dont care about, return SUCCESS */
      break;
    }
  }
  else
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_SNOOPING_COMPONENT_ID,
            "%s: %d: dsIntfChangeProcess(): Received a interface callback while outside the EXECUTE state", __FILE__, __LINE__);
    rc = L7_FAILURE;
  }
  status.response.rc = rc;
  nimEventStatusCallback(status);
  return rc;

}

/*********************************************************************
* @purpose  Apply the rate limit params on an interface
*
* @param    intIfNum   @b((input)) internal interface number
*
* @returns  L7_SUCCESS
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t dsIntfRateLimitApply(L7_uint32 intIfNum)
{
  /* reset the received packet count per second and number of
   * consecutive intervals with pps > rate limit,
   * to initial values */
  dsIntfInfo[intIfNum].pktRxCount = 0;
  dsIntfInfo[intIfNum].consecutiveInterval = 1;

  return L7_SUCCESS;
}


/*********************************************************************
* @purpose  Convert a MAC address to a string.
*
* @param    macAddr    @b((input)) MAC address to be converted
* @param    macAddrStr @b((output)) In string format
*
* @returns  void
*
* @notes    macAddrStr must be at least DS_MAC_STR_LEN.
*
* @end
*********************************************************************/
void dsMacToString(L7_uchar8 *macAddr, L7_uchar8 *macAddrStr)
{
  osapiSnprintf(macAddrStr, DS_MAC_STR_LEN, "%02X:%02X:%02X:%02X:%02X:%02X",
                macAddr[0], macAddr[1], macAddr[2],
                macAddr[3], macAddr[4], macAddr[5]);
}

/*********************************************************************
* @purpose  Convert a MAC address in string format to hex.
*
* @param    macAddr  @b((input)) MAC string to be converted
* @param    mac      @b((output)) In hex format
*
* @returns  L7_SUCCESS or L7_FAILURE
*
* @notes    stolen from cliConvertMac(). It's a little lame (requires
*           string to contain 2 characters between each colon).
*
* @end
*********************************************************************/
L7_RC_t dsStringToMac(L7_uchar8 *macStr, L7_enetMacAddr_t *macAddr)
{
  L7_uint32 i, j, digit_count = 0;
  L7_uchar8 mac_address[20];   /* more than enough */

  if (strlen(macStr) != 17)
  {      /* test string length */
    return L7_FAILURE;
  }

  for ( i=0,j=0; i<17; i++,j++ )
  {
    digit_count++;
    switch ( macStr[i] )
    {
    case '0':
      mac_address[j]=0x0;
      break;
    case '1':
      mac_address[j]=0x1;
      break;
    case '2':
      mac_address[j]=0x2;
      break;
    case '3':
      mac_address[j]=0x3;
      break;
    case '4':
      mac_address[j]=0x4;
      break;
    case '5':
      mac_address[j]=0x5;
      break;
    case '6':
      mac_address[j]=0x6;
      break;
    case '7':
      mac_address[j]=0x7;
      break;
    case '8':
      mac_address[j]=0x8;
      break;
    case '9':
      mac_address[j]=0x9;
      break;
    case 'a':
    case 'A':
      mac_address[j]=0xA;
      break;
    case 'b':
    case 'B':
      mac_address[j]=0xB;
      break;
    case 'c':
    case 'C':
      mac_address[j]=0xC;
      break;
    case 'd':
    case 'D':
      mac_address[j]=0xD;
      break;
    case 'e':
    case 'E':
      mac_address[j]=0xE;
      break;
    case 'f':
    case 'F':
      mac_address[j]=0xF;
      break;
    case ':':
      if (digit_count != 3)
      {    /* if more or less than 2 digits return false */
        return L7_FAILURE;
      }
      j--;
      digit_count=0;
      break;
    default:
      return L7_FAILURE;
      break;
    }
  }

  for ( i = 0; i < 6; i++ )
  {
    macAddr->addr[i] = ( (mac_address[(i*2)] << 4) + mac_address[(i*2)+1] );
  }
  return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Determine the type of a DHCP packet.
*
* @param    packet  @b{(input)} Pointer to Dhcp Packet
* @param    pktLen  @b{(input)} length of the Dhcp Packet
*
* @returns  DHCP packet type
*
* @notes    finds packet type option and returns its value
*
* @end
*********************************************************************/
L7_dhcp_pkt_type_t dsPacketType(L7_dhcp_packet_t *packet, L7_uint32 pktLen)
{
  L7_uchar8 *dhcpOption;
  L7_uchar8 *pktEnd;
  L7_uchar8 *optionStart;
  L7_BOOL stop_processing = L7_FALSE;
  const L7_uint32 optLenIndex = 1;            /* offset of length field in an option */
  const L7_uint32 optValIndex = 2;            /* offset of value field in an option */

  /* If there's no cookie, it's a bootp packet, so we should just
     forward it unchanged. */
  optionStart = (L7_uchar8*) (packet + 1);
  if (memcmp(optionStart, DS_DHCP_OPTIONS_COOKIE, L7_DHCP_MAGIC_COOKIE_LEN))
    return L7_DHCP_UNKNOWN;

  pktEnd = (L7_uchar8*)(packet) + pktLen;

  dhcpOption = optionStart + L7_DHCP_MAGIC_COOKIE_LEN;
  while ((dhcpOption < pktEnd) && (!stop_processing))
  {
    switch (*dhcpOption)
    {
      case DHO_PAD:
        ++dhcpOption;
        break;

      case DHO_DHCP_MESSAGE_TYPE:
        return (L7_dhcp_pkt_type_t) dhcpOption[optValIndex];

        /* Quit immediately if we come across End option. */
      case DHO_END:
        stop_processing = L7_TRUE; /* while loop breaks */
        break;

      default:
        /* skip code and length bytes plus value bytes */
        dhcpOption += (optValIndex + dhcpOption[optLenIndex]);
        break;
    }
  }

  return L7_DHCP_UNKNOWN;
}

/*********************************************************************
* @purpose  Extract the duration of a lease from a DHCP packet.
*
* @param    packet  @b{(input)} Pointer to DHCP Packet
* @param    pktLen  @b{(input)} length of the DHCP Packet
*
* @returns  lease time in minutes
*
* @notes    finds lease time option and returns its value
*
* @end
*********************************************************************/
L7_uint32 dsLeaseTimeGet(L7_dhcp_packet_t *packet, L7_uint32 pktLen)
{
  L7_uchar8 *dhcpOption;
  L7_uchar8 *pktEnd;
  L7_uchar8 *optionStart;
  L7_BOOL stop_processing = L7_FALSE;
  const L7_uint32 optLenIndex = 1;            /* offset of length field in an option */
  const L7_uint32 optValIndex = 2;            /* offset of value field in an option */
  L7_uint32 lt;                               /* lease time. network byte order */

  optionStart = (L7_uchar8*) (packet + 1);
  pktEnd = (L7_uchar8*)(packet) + pktLen;
  dhcpOption = optionStart + L7_DHCP_MAGIC_COOKIE_LEN;
  while ((dhcpOption < pktEnd) && (!stop_processing))
  {
    switch (*dhcpOption)
    {
      case DHO_PAD:
        ++dhcpOption;
        break;

      case DHO_DHCP_LEASE_TIME:
        memcpy(&lt, &dhcpOption[optValIndex], 4);
        return (L7_uint32) osapiNtohl(lt);

        /* Quit immediately if we come across End option. */
      case DHO_END:
        stop_processing = L7_TRUE; /* while loop breaks */
        break;

      default:
        /* skip code and length bytes plus value bytes */
        dhcpOption += (optValIndex + dhcpOption[optLenIndex]);
        break;
    }
  }

  return 0;
}

/*********************************************************************
* @purpose  Returns the length of an IP header in bytes.
*
* @param    ipHeader  @b{(input)} IP header
*
* @returns  length of IP header in bytes
*
* @notes
*
* @end
*********************************************************************/
L7_ushort16 dsIpHdrLen(L7_ipHeader_t *ipHeader)
{
  return 4 * (0x0F & *((L7_uchar8*)ipHeader));       /* bytes */
}

/*********************************************************************
* @purpose  Log an Ethernet header.
*
* @param    frame      @b{(input)} start of ethernet frame
* @param    traceDest  @b{(input)} whether to send output to log or console
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dsLogEthernetHeader(L7_enetHeader_t *ethHdr, dsTraceDest_t traceDest)
{
  L7_uchar8 traceBuf[DS_TRACE_LEN_MAX];
  L7_uchar8 srcMacStr[DS_MAC_STR_LEN];
  L7_uchar8 dstMacStr[DS_MAC_STR_LEN];
  L7_enet_encaps_t *tol;
  L7_ushort16 typeOrLen;

  dsMacToString(ethHdr->dest.addr, dstMacStr);
  dsMacToString(ethHdr->src.addr, srcMacStr);
  tol = (L7_enet_encaps_t*) ((L7_uchar8*)ethHdr + sizeof(L7_enetHeader_t));
  typeOrLen = osapiNtohs(tol->type);
  osapiSnprintf(traceBuf, DS_TRACE_LEN_MAX,
                "Ethernet header: dest %s, src %s, type/len %#x.",
                dstMacStr, srcMacStr, typeOrLen);
  if (traceDest == DS_TRACE_LOG)
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID, traceBuf);
  else if (traceDest == DS_TRACE_CONSOLE)
  {
    printf("\n\n%s", traceBuf);
  }
}

/*********************************************************************
* @purpose  Log an IP packet.
*
* @param    ipHeader  @b{(input)} IP header
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dsLogIpHeader(L7_ipHeader_t *ipHdr, dsTraceDest_t traceDest)
{
  L7_uchar8 traceBuf[DS_TRACE_LEN_MAX];
  L7_uchar8 srcStr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 destStr[OSAPI_INET_NTOA_BUF_SIZE];

  osapiInetNtoa(osapiNtohl(ipHdr->iph_src), srcStr);
  osapiInetNtoa(osapiNtohl(ipHdr->iph_dst), destStr);
  osapiSnprintf(traceBuf, DS_TRACE_LEN_MAX,
                "IP packet: ver/hlen %#4.2x, tos %d, len %d, id %d, "
                "flags/offset %#4.2x, ttl %d, proto %d, src %s, dst %s.",
                ipHdr->iph_versLen, ipHdr->iph_tos, osapiNtohs(ipHdr->iph_len),
                osapiNtohs(ipHdr->iph_ident), ipHdr->iph_flags_frag,
                ipHdr->iph_ttl, ipHdr->iph_prot, srcStr, destStr);
  if (traceDest == DS_TRACE_LOG)
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID, traceBuf);
  else if (traceDest == DS_TRACE_CONSOLE)
  {
    printf("\n\n%s", traceBuf);
  }
}

/*********************************************************************
* @purpose  Log a DHCP packet
*
* @param    ipHeader  @b{(input)} IP header
*
* @returns  void
*
* @notes
*
* @end
*********************************************************************/
void dsLogDhcpPacket(L7_dhcp_packet_t *dhcpPacket, dsTraceDest_t traceDest)
{
  L7_uchar8 traceBuf[DS_TRACE_LEN_MAX];
  L7_uchar8 ciaddr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 yiaddr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 siaddr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 giaddr[OSAPI_INET_NTOA_BUF_SIZE];
  L7_uchar8 chaddr[DS_MAC_STR_LEN];

  osapiInetNtoa(osapiNtohl(dhcpPacket->ciaddr), ciaddr);
  osapiInetNtoa(osapiNtohl(dhcpPacket->yiaddr), yiaddr);
  osapiInetNtoa(osapiNtohl(dhcpPacket->siaddr), siaddr);
  osapiInetNtoa(osapiNtohl(dhcpPacket->giaddr), giaddr);
  dsMacToString(dhcpPacket->chaddr, chaddr);

  osapiSnprintf(traceBuf, DS_TRACE_LEN_MAX,
                "DHCP packet: op %s, htype %d, hlen %d, hops %d, xid %u, "
                "secs %u, ciaddr %s, yiaddr %s, server %s, giaddr %s, chaddr %s.",
                ((dhcpPacket->op == L7_DHCP_BOOTP_REQUEST) ? "Request" : "Reply"),
                dhcpPacket->htype, dhcpPacket->hlen, dhcpPacket->hops,
                osapiNtohl(dhcpPacket->xid), dhcpPacket->secs, ciaddr, yiaddr,
                siaddr, giaddr, chaddr);

  if (traceDest == DS_TRACE_LOG)
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID, traceBuf);
  else if (traceDest == DS_TRACE_CONSOLE)
  {
    printf("\n\n%s", traceBuf);
  }
}

#if 0
void
dsL2RelayDetailLog(L7_uchar8 *str, L7_uint32 intIfNum, L7_ushort16 vlanId,
                   L7_enetHeader_t *ethHdr,L7_ipHeader_t *ipHdr,
                   L7_dhcp_packet_t *dhcpPacket, dsTraceDest_t traceDest)
{
  static L7_uint32 lastMsg = 0;
  L7_uchar8 ifName[L7_NIM_IFNAME_SIZE + 1];
  L7_uchar8 ifInfo[DS_IFACE_INFO_STR_SIZE];

  if (osapiUpTimeRaw() > lastMsg)
  {
    lastMsg = osapiUpTimeRaw();

    nimGetIntfName(intIfNum, L7_SYSNAME, ifName);
    osapiSnprintf(ifInfo, DS_IFACE_INFO_STR_SIZE, "interface %s vlanId %d",
                  ifName, vlanId);
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID, str);
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DHCP_SNOOPING_COMPONENT_ID, ifInfo);
    dsLogEthernetHeader(ethHdr, DS_TRACE_LOG);
    dsLogIpHeader(ipHdr, DS_TRACE_LOG);
    dsLogDhcpPacket(dhcpPacket, DS_TRACE_LOG);
  }
}

#endif

static L7_RC_t dsConvertTo32BitUnsignedInteger(const L7_char8 *buf, L7_uint32 *pVal)
{

  L7_int32 f;
  L7_int32 j;
  L7_uint32 tempval;
#define MAXUINT 4294967295U
#define MAXMSB 4

  if (buf == L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  if ( buf[0] == '-' )
    return L7_FAILURE;

  j = strlen(buf);

  if ( j > 10 )
    return L7_FAILURE;

  if( (strlen(buf)== 10))
  {
    tempval = (L7_uint32)(buf[0] -'0');
    if (tempval > MAXMSB)
    {
      return L7_FAILURE;
    }
  }

  for ( *pVal = 0, f=1, j=j-1; j >= 0; j--, f=f*10)
  {
    tempval = (L7_uint32)(buf[j] -'0');

    if ( ( tempval > 9 ) )
      return L7_FAILURE;

    tempval = tempval * f;

    if ( (MAXUINT - tempval) < *pVal )
      return L7_FAILURE;
    else
      *pVal = *pVal + tempval;
  }

  return L7_SUCCESS;
}


static L7_BOOL dsConvertMac(L7_uchar8 * buf, L7_uchar8 * mac)
{
  L7_uint32 i,j, digit_count=0;
  L7_uchar8 mac_address[L7_CLI_MAX_STRING_LENGTH];

  if (strlen(buf) != 17)
  {      /* test string length */
    return L7_FALSE;
  }

  for ( i=0,j=0; i<17; i++,j++ )
  {
    digit_count++;
    switch ( buf[i] )
    {
    case '0':
      mac_address[j]=0x0;
      break;
    case '1':
      mac_address[j]=0x1;
      break;
    case '2':
      mac_address[j]=0x2;
      break;
    case '3':
      mac_address[j]=0x3;
      break;
    case '4':
      mac_address[j]=0x4;
      break;
    case '5':
      mac_address[j]=0x5;
      break;
    case '6':
      mac_address[j]=0x6;
      break;
    case '7':
      mac_address[j]=0x7;
      break;
    case '8':
      mac_address[j]=0x8;
      break;
    case '9':
      mac_address[j]=0x9;
      break;
    case 'a':
    case 'A':
      mac_address[j]=0xA;
      break;
    case 'b':
    case 'B':
      mac_address[j]=0xB;
      break;
    case 'c':
    case 'C':
      mac_address[j]=0xC;
      break;
    case 'd':
    case 'D':
      mac_address[j]=0xD;
      break;
    case 'e':
    case 'E':
      mac_address[j]=0xE;
      break;
    case 'f':
    case 'F':
      mac_address[j]=0xF;
      break;
    case ':':
      if (digit_count != 3)
      {    /* if more or less than 2 digits return false */
        return L7_FALSE;
      }
      j--;
      digit_count=0;
      break;
    default:
      return L7_FALSE;
      break;
    }
  }

  for ( i = 0; i < 6; i++ )
  {
    mac[i] = ( (mac_address[(i*2)] << 4) + mac_address[(i*2)+1] );
  }
  return L7_TRUE;
}

static L7_BOOL dsIsStackingSupported(void)
{
#ifdef L7_STACKING_PACKAGE
  return L7_TRUE;
#else
  return L7_FALSE;
#endif
}

static L7_char8 *dsStringSeparateHelp(L7_char8 * * input, const L7_char8 * delim)
{
  const L7_char8 * tmp_delim;
  L7_char8 * tmp_input, * tok;
        L7_int32 i, j;

        if ((tmp_input = *input) == NULL)
  {
    return NULL;
  }

        for (tok = tmp_input;;)
  {
    i = *tmp_input++;
    tmp_delim = delim;
    do
    {
      if ((j = *tmp_delim++) == i)
      {
        if (i == 0)
        {
          tmp_input = NULL;
        }
        else
        {
          *(tmp_input-1) = 0;
        }

        *input = tmp_input;
        return tok;
      }
    } while (j != 0);
  }
}

static L7_uint32 dsGetUnitId()
{
  L7_uint32 unit;

  /* get switch ID based on presence/absence of STACKING package */
  if (dsIsStackingSupported() == L7_TRUE)
  {
    if (usmDbUnitMgrMgrNumberGet(&unit) != L7_SUCCESS)
    {
      /* This is a temporary fix until the CLI changes are made to handle a unit of 0 */
      unit = L7_MAX_UNITS_PER_STACK + 1;
  }
  }
  else
  {
   /* Needs to use this macro */
   /*  unit = (L7_uint32)U_IDX; */
    unit = 1;
  }

  return unit;
}

static L7_RC_t dsValidSpecificUSPCheck(const L7_char8 * buf, L7_uint32 * unit,
    L7_uint32 * slot, L7_uint32 * port)
{
  L7_uint32 buf_len;
  L7_char8  tmp_buf[L7_CLI_MAX_STRING_LENGTH];
  L7_char8 *u, *s, *p;
  L7_char8 *input;
  const L7_char8 * slash = "/";
  const L7_char8 * end = "";


  buf_len = (L7_uint32)strlen(buf);
  if (strlen(buf) >= sizeof(tmp_buf))
  {
    return L7_FAILURE;
  }

  memset (tmp_buf, 0, (L7_int32)sizeof(tmp_buf));
  memcpy(tmp_buf, buf, buf_len);

  if (strcmp(tmp_buf, "") == 0)
  {
    return L7_FAILURE;
  }

  /* NOTE: Input of type <a/b/c> only is valid.
   *
   * Check for erroneous inputs, i.e, of type <a>, <a/b>, <a/b/c/d> etc.
   */
  input = tmp_buf;
  if (dsIsStackingSupported() == L7_TRUE)
  {
  u = dsStringSeparateHelp(&input, slash);
    if ((u == L7_NULLPTR) || (dsConvertTo32BitUnsignedInteger(u, unit) != L7_SUCCESS))
    {
      return L7_FAILURE;
    }
    if (*unit > L7_UNITMGR_MAX_UNIT_NUMBER)
    {
      return L7_ERROR;
    }
  }
  else
  {
    *unit = dsGetUnitId();
  }

  s = dsStringSeparateHelp(&input, slash);
  if(s != L7_NULLPTR)
  {
     if(strlen(s)== 0)
     {
      return L7_FAILURE;
     }
  }
  p = dsStringSeparateHelp(&input, end);
  if ((s == L7_NULLPTR) || (dsConvertTo32BitUnsignedInteger(s, slot) != L7_SUCCESS) ||
      (p == L7_NULLPTR) || (dsConvertTo32BitUnsignedInteger(p, port) != L7_SUCCESS))
  {
    return L7_FAILURE;
  }

#ifdef L7_CHASSIS
  {
  if ((*slot > L7_MAX_SLOTS_PER_UNIT) ||
      (*port == 0) ||
      (*port > L7_MAX_PORTS_PER_SLOT))
        (*slot > (L7_MAX_SLOTS_PER_CHASSIS+
                  L7_MAX_LOGICAL_SLOTS_PER_UNIT+L7_MAX_CPU_SLOTS_PER_UNIT)) ||
        (*port == 0))
    {
      return L7_ERROR;
    }
  }
#else
  {
    if ((*slot > L7_MAX_SLOTS_PER_UNIT) ||
        (*port == 0))
    {
      return L7_ERROR;
    }
  }
#endif

  return L7_SUCCESS;

}

static L7_RC_t dsSlotPortToInterface(const L7_char8 * unit_slot_port,
    L7_uint32 * unit,
    L7_uint32 * slot,
    L7_uint32 * port,
    L7_uint32 * intIfNum)
{
  L7_char8 * now;
  L7_char8 * charSlot;
  L7_char8 * charPort;
  L7_char8 tempSlotPort[L7_CLI_MAX_STRING_LENGTH];
  L7_uint32 val;

  OSAPI_STRNCPY_SAFE(tempSlotPort, unit_slot_port);

  now = strchr(tempSlotPort, '/');

  if ((now != NULL) && ((now+1) != NULL))
  {
    charPort = now+1;
    now[0]='\0';
    charSlot = tempSlotPort;
    if ((dsConvertTo32BitUnsignedInteger(charPort, &val) == L7_SUCCESS) &&
        (dsConvertTo32BitUnsignedInteger(charSlot, &val) == L7_SUCCESS) &&
        (strcmp(now, tempSlotPort) != 0))
    {
      *slot = atoi(charSlot);
      *port = atoi(charPort);
      *unit = dsGetUnitId();

      if (*slot <= L7_MAX_SLOTS_PER_UNIT)
      {
        return usmDbIntIfNumFromUSPGet(*unit, *slot, *port, intIfNum);
      }
    }
  }
  return L7_FAILURE;
}



static L7_RC_t dsSlotPortToIntNum(const L7_char8 * slot_port,
    L7_uint32 * slot,
    L7_uint32 * port,
    L7_uint32 * intIfNum)

{
  L7_uint32 unit;

  unit = dsGetUnitId();
  return dsSlotPortToInterface(slot_port, &unit, slot, port, intIfNum);

}

/*********************************************************************
* @purpose  CLI helper routine for interface display format
*
* @param  unit  @b{(input)) the unit index
* @param  slot  @b{(input)) the slot index
* @param  port  @b{(input)) internal interace number
*
* @returns  Interface display string
*
* @comments Returned string reflects STACKING component support
*
* @end
*********************************************************************/
static L7_char8 *dsDisplayInterfaceHelp(L7_uint32 unit, L7_uint32 slot, L7_uint32 port)
{
  static L7_char8 tmp_string[L7_CLI_MAX_STRING_LENGTH];
  static L7_char8 intf_string[L7_CLI_MAX_STRING_LENGTH];

#ifdef L7_STACKING_PACKAGE
  osapiSnprintf(tmp_string, sizeof(tmp_string), "%u/%u/%u", unit, slot, port);
#else
  osapiSnprintf(tmp_string, sizeof(tmp_string), "%u/%u", slot, port);
#endif

  osapiSnprintf(intf_string, sizeof(intf_string), tmp_string);
  return intf_string;
}





/*********************************************************************
* @purpose  Store the DHCP Snooping database into a local file
*           to upload
*
* @param    pduInfo  @b{(input)} pointer to pdu info structure
*                                which stores intIfNum and vlanId
*
* @returns  L7_TRUE  - If file got stored successfully.
*
* @returns  L7_FALSE  - If there is a problem in storing the file.
*
* @notes    none
*
* @end
********************************************************************/
L7_RC_t dsDbRemoteSave()
{
 L7_int32 filedesc;
 dhcpSnoopBinding_t binding;
 L7_uchar8 macStr[L7_CLI_MAX_STRING_LENGTH];
 L7_uchar8 ipAddrStr[L7_CLI_MAX_STRING_LENGTH];
 L7_uchar8 ifName[L7_CLI_MAX_STRING_LENGTH];
 L7_char8 buf[80];
 L7_char8 tmpBuf[80];
 L7_uint32 unit, intSlot,intPort;

 if (osapiFsFileCreate(DHCP_SNOOPING_UPLOAD_FILE_NAME, &filedesc) == L7_SUCCESS)
 {
   memset (buf, '\0', sizeof(buf));
   memset(&binding.key.macAddr, 0, L7_MAC_ADDR_LEN);
   while(dsBindingFind(&binding,L7_MATCH_GETNEXT) == L7_SUCCESS)
   {
     if (binding.bindingType != DS_BINDING_DYNAMIC)
      continue;

     /* MAC address */
     osapiSnprintf(macStr, sizeof(macStr),"%02X:%02X:%02X:%02X:%02X:%02X",
                  binding.key.macAddr[0], binding.key.macAddr[1], binding.key.macAddr[2],
                  binding.key.macAddr[3], binding.key.macAddr[4], binding.key.macAddr[5]);
     /* IP address */
     usmDbInetNtoa(binding.ipAddr, ipAddrStr);

     /* Interface */
     if (usmDbUnitSlotPortGet(binding.intIfNum, &unit, &intSlot, &intPort) == L7_SUCCESS)
     {
       osapiSnprintf(ifName, sizeof(ifName),
                    dsDisplayInterfaceHelp(unit, intSlot, intPort));
     }
     else
     {
       /* This case may be because of either the lags or the stack members */
       continue;
     }
     if (binding.bindingType == DS_BINDING_DYNAMIC)
     {
       memset (buf, '\0', sizeof(buf));
       memset (tmpBuf, '\0', sizeof(buf));

       osapiSnprintf(tmpBuf, sizeof(tmpBuf), "%s %s %u %s %u \n",
                    macStr, ipAddrStr, binding.vlanId, ifName,
                     binding.remLease);

       osapiSnprintf(buf, sizeof(buf), "%u %s",
                         nvStoreCrc32(tmpBuf,strlen(tmpBuf)),
                         tmpBuf
                     );
       /* call save NVStore routine */
       if (osapiFsWriteNoClose(filedesc,buf,sizeof(buf)) == L7_ERROR)
       {
         L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_SNOOPING_COMPONENT_ID,
                 " Problem occured while writing DHCP Snooping database into NVRAM");
         osapiFsClose(filedesc);
         osapiFsDeleteFile(DHCP_SNOOPING_UPLOAD_FILE_NAME);
         return L7_FAILURE;
       }
     }
  }
  osapiFsClose(filedesc);
  return L7_SUCCESS;
 }
 else
 {
  L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_SNOOPING_COMPONENT_ID,
          " Problem with file creation while writing DHCP Snooping database");
  return L7_FAILURE;
 }
 return L7_SUCCESS;
}

/*********************************************************************
* @purpose  Read the DHCP Snooping database from a local file
*
*
* @returns  L7_TRUE  - If file got read successfully.
*
* @returns  L7_FALSE  - If there is a problem in reading the file.
*
* @notes    none
*
* @end
********************************************************************/
L7_RC_t dsDbRemoteRestore()
{

  L7_int32 filedesc;
  L7_uint32 readCnt =0,fileSize;
  L7_char8 buf[80];
  L7_char8 *tmpBuf = L7_NULLPTR;
  L7_int32 res=0;
  L7_uint32 checkSum;
  L7_uint32 rowCheckSum;
  dhcpSnoopBinding_t dsBinding;
  L7_enetMacAddr_t macAddr;
  int count=0;
  L7_uint32 unit,slot, port;
  L7_uint32 vlanId;
  L7_RC_t rc;


  if((osapiFsFileSizeGet(DHCP_SNOOPING_DOWNLOAD_FILE_NAME,&fileSize)) != L7_SUCCESS)
  {
    return L7_ERROR;
  }
 if (osapiFsOpen(DHCP_SNOOPING_DOWNLOAD_FILE_NAME, &filedesc) != L7_ERROR)
 {
   while (readCnt < fileSize)
   {
     /* call save NVStore routine */
     res=osapiFileRead(filedesc,buf,sizeof(buf));
     if (res == L7_ERROR)
     {
       osapiFsClose(filedesc);
       osapiFsDeleteFile(DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
       return L7_FAILURE;
     }
     tmpBuf = strtok (buf, " ");

     /* Evalute the check sum */
     if (dsConvertTo32BitUnsignedInteger(tmpBuf,&checkSum) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Check sum failure occured while reading the snooping database.");
        osapiFsClose(filedesc);
        osapiFsDeleteFile(DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
        return L7_FAILURE;
     }
     rowCheckSum = nvStoreCrc32(buf+strlen(tmpBuf)+1,strlen(buf+strlen(tmpBuf)+1));
     if ( checkSum != rowCheckSum)
     {
        L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Checksum mismatch occured while reading the snooping database.");
        osapiFsClose(filedesc);
        osapiFsDeleteFile(DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
        return L7_FAILURE;
     }
     /* Popolate the Binding Attributes */
     count = 0;
     tmpBuf = strtok((L7_char8 *)'\0', " ");
     memset(&dsBinding, 0, sizeof(dsBinding));
     while (tmpBuf != NULL)
     {
       if ( count == 0) /* MAC-Address population */
       {
         if (dsConvertMac(tmpBuf, dsBinding.key.macAddr) != L7_TRUE)
         {
           L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Mac address parsing problem occured while reading the snooping database.");
           osapiFsClose(filedesc);
           osapiFsDeleteFile(DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
           return L7_FAILURE;
         }
         /* Find if this binidng already exists ot not */
         rc = dsBindingFind(&dsBinding, L7_MATCH_EXACT);
         if ( rc == L7_SUCCESS)
         {
           break;
         }
         count++;
         memcpy(macAddr.addr, dsBinding.key.macAddr, L7_ENET_MAC_ADDR_LEN);
         tmpBuf = strtok((L7_char8 *)'\0', " ");
       }
       if ( count == 1) /* IP-Address */
       {
         if (
               (tmpBuf == NULL)||
               (usmDbInetAton(tmpBuf,&dsBinding.ipAddr) != L7_SUCCESS)
            )
         {
           L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Ip address parsing problem occured while reading the snooping database.");
           osapiFsClose(filedesc);
           osapiFsDeleteFile(DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
           return L7_FAILURE;
         }
         count++;
         tmpBuf = strtok((L7_char8 *)'\0', " ");
       }
       if ( count == 2) /* VLAN */
       {
         if( (tmpBuf == NULL) ||
              (dsConvertTo32BitUnsignedInteger(tmpBuf, &vlanId) != L7_SUCCESS) )
         {
           L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Vlan parsing problem occured while reading the snooping database.");
           osapiFsClose(filedesc);
           osapiFsDeleteFile(DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
           return L7_FAILURE;
         }
         dsBinding.vlanId = vlanId;
         count++;
         tmpBuf = strtok((L7_char8 *)'\0', " ");
       }
       if ( count == 3) /* parse the interface */
       {
         if ( tmpBuf == NULL )
         {
           L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
             "Interface parsing problem occured while reading the snooping database.");
           osapiFsClose(filedesc);
           osapiFsDeleteFile(DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
           return L7_FAILURE;
         }
         if (dsIsStackingSupported() == L7_TRUE)
         {
           if (dsValidSpecificUSPCheck(tmpBuf, &unit, &slot, &port) != L7_SUCCESS)
           {
               L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                  "Interface parsing problem occured while reading the snooping database.");
                osapiFsClose(filedesc);
                 osapiFsDeleteFile(DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
                 return L7_FAILURE;
           }

           /* Get interface and check its validity */
           if (usmDbIntIfNumFromUSPGet(unit, slot, port, &dsBinding.intIfNum) != L7_SUCCESS)
           {
               L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                  "Interface parsing problem occured while reading the snooping database.");
                osapiFsClose(filedesc);
                 osapiFsDeleteFile(DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
                 return L7_FAILURE;

           }

         }
         else  /* It's not stacking code */
         {
           if ( dsSlotPortToIntNum(tmpBuf,&slot,&port, &dsBinding.intIfNum)!=L7_SUCCESS )
           {
               L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                  "Interface parsing problem occured while reading the snooping database.");
                osapiFsClose(filedesc);
                 osapiFsDeleteFile(DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
                 return L7_FAILURE;
           }
         }
         count++;
         tmpBuf = strtok((L7_char8 *)'\0', " ");
       }
       if (count == 4) /* It's lease parsing.. */
       {
         if( (tmpBuf == NULL) ||
              (dsConvertTo32BitUnsignedInteger(tmpBuf, &dsBinding.remLease) != L7_SUCCESS) )
         {
               L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                  "Lease time parsing problem occured while reading the snooping database.");
                osapiFsClose(filedesc);
                 osapiFsDeleteFile(DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
                 return L7_FAILURE;

         }
       }
      /* Add in to the binding database*/
       rc = dsBindingAdd(DS_BINDING_DYNAMIC,
                         &macAddr,
                         dsBinding.ipAddr,
                         dsBinding.vlanId, 0 /* PTin modified: DHCP */,
                        dsBinding.intIfNum);
       if ( rc == L7_FAILURE)
       {
               L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DHCP_SNOOPING_COMPONENT_ID,
                  "Lease time parsing problem occured while reading the snooping database.");
                osapiFsClose(filedesc);
                 osapiFsDeleteFile(DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
                 return L7_FAILURE;
       }
        dsBindingLeaseSet(&macAddr, dsBinding.remLease);
       break;
     }
     readCnt = readCnt+sizeof(buf);
   }
   osapiFsClose(filedesc);
   osapiFsDeleteFile(DHCP_SNOOPING_DOWNLOAD_FILE_NAME);
   return L7_SUCCESS;
 }
  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Save the DHCP Snooping database locally.
*
*
*
* @end
********************************************************************/
void dsDbLocalSave()
{
 dhcpSnoopBinding_t binding;
 L7_uint32 dbIndex=0, dbEntries;

 if(! txtBasedConfigComplete)
 {
   /* Till the text based configuration apply is complete, don't
    * write the bindings db, else we might end up overwriting the
    * bindings db with zero entries */
   return;
 }

 memset(&binding.key.macAddr, 0, L7_MAC_ADDR_LEN);
 memset(dsDbCfgData.dsBindingDb, 0, sizeof(dsDbCfgData.dsBindingDb));

 dbEntries = (sizeof(dsDbCfgData.dsBindingDb) / sizeof(dsDbBindingTreeNode_t));
 while ((dsBindingFind(&binding,L7_MATCH_GETNEXT) == L7_SUCCESS) &&
        (dbIndex < dbEntries))
 {
   if (binding.bindingType != DS_BINDING_DYNAMIC)
     continue;

   memcpy((void *)&dsDbCfgData.dsBindingDb[dbIndex].macAddr.addr,
            (void *) binding.key.macAddr, L7_ENET_MAC_ADDR_LEN);
   dsDbCfgData.dsBindingDb[dbIndex].ipAddr = binding.ipAddr;
   dsDbCfgData.dsBindingDb[dbIndex].vlanId = binding.vlanId;
   dsDbCfgData.dsBindingDb[dbIndex].intIfNum = binding.intIfNum;
   dsDbCfgData.dsBindingDb[dbIndex].remLease = binding.remLease;
   dbIndex++;
 }
 dsDbCfgData.checkSum = nvStoreCrc32((L7_char8 *)&dsDbCfgData,
                 sizeof(dsDbCfgData_t) - sizeof(dsDbCfgData.checkSum));

 /* call config file save routine */
 if (sysapiBinaryCfgFileWrite(L7_DHCP_SNOOPING_COMPONENT_ID,
                              L7_DHCP_SNOOPING_CFG_FILENAME,
                              (L7_char8 *)&dsDbCfgData,
                              sizeof(dsDbCfgData_t)) == L7_ERROR)
 {
   L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_SNOOPING_COMPONENT_ID,
           "dsDbSaveCfg: Error during sysapiCfgFileWrite %s\n",
             L7_DHCP_SNOOPING_CFG_FILENAME);
 }

}

/*********************************************************************
* @purpose  Restore the DHCP Snooping database locally.
*
*
*
* @end
********************************************************************/

void dsDbLocalRestore()
{

  L7_RC_t rc;
  L7_uint32 dbIndex, dbEntries;
  dhcpSnoopBinding_t dsBinding;

  if (dsCfgData->dsGlobalAdminMode == L7_ENABLE)
  {
    if (sysapiCfgFileGet( L7_DHCP_SNOOPING_COMPONENT_ID,
                           L7_DHCP_SNOOPING_CFG_FILENAME ,
                           (L7_char8 *)&dsDbCfgData,
                           sizeof(dsDbCfgData_t),
                           &dsDbCfgData.checkSum,
                            L7_DHCP_SNOOPING_VER_CURRENT ,
                  NULL, NULL) != L7_SUCCESS)
     {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_SNOOPING_COMPONENT_ID,
                "%s: %d: dsDb Restore problem: DHCP Sboop unable to read \
                snooping config file (%s)\n", __FILE__, __LINE__,
                L7_DHCP_SNOOPING_CFG_FILENAME);
        return;
     }
   dbEntries = (sizeof(dsDbCfgData.dsBindingDb) / sizeof(dsDbBindingTreeNode_t));
   for (dbIndex = 0; dbIndex < dbEntries; dbIndex ++)
   {
      if ( dsDbCfgData.dsBindingDb[dbIndex].ipAddr > 0)
      {
         memset(&dsBinding, 0, sizeof(dsBinding));
         memcpy(dsBinding.key.macAddr,
              (void *)&dsDbCfgData.dsBindingDb[dbIndex].macAddr,
              L7_ENET_MAC_ADDR_LEN);
         rc = dsBindingFind(&dsBinding, L7_MATCH_EXACT);
         if ( rc == L7_SUCCESS)
         {
          continue;
         }
         rc = dsBindingAdd(DS_BINDING_DYNAMIC,
                       &dsDbCfgData.dsBindingDb[dbIndex]. macAddr,
                      dsDbCfgData.dsBindingDb[dbIndex].ipAddr,
                      dsDbCfgData.dsBindingDb[dbIndex].vlanId, 0 /* PTin modified: DHCP */,
                      dsDbCfgData.dsBindingDb[dbIndex].intIfNum);
         dsBindingLeaseSet(&dsDbCfgData.dsBindingDb[dbIndex].macAddr,
                       dsDbCfgData.dsBindingDb[dbIndex].remLease);
      }
   }
  }

  /* apply snooping database */

}

/*********************************************************************
* @purpose  Initialize the tftp parameters and upload the DHCP
*           Snooping database.
*
* @notes   Success or failure of this TFTP transfer will be
*          asynchronously notified to the dhcp snooping application.
*
*
* @end
********************************************************************/
void dsTftpUploadInit()
{

  L7_int32 unit = 1;
  L7_inet_addr_t inetAddr;
  L7_RC_t rc;
  L7_char8 ipStr[128];

  if(! txtBasedConfigComplete)
  {
   /* Till the text based configuration apply is complete, don't
    * write the bindings db, else we might end up overwriting the
    * bindings db with zero entries */
    return;
  }

  if ( usmDbTransferInProgressGet(unit) == L7_TRUE )
  {
    return;
  }

  if ( dsDbRemoteSave() !=L7_SUCCESS)
  {
    L7_LOGF(L7_LOG_SEVERITY_INFO, L7_DHCP_SNOOPING_COMPONENT_ID,
            "DHCP snooping database saving on nvram failed");
    return;
  }

  usmDbTransferContextSet(unit, NULL);

  usmDbTransferModeSet(unit, L7_TRANSFER_TFTP);
  usmDbTransferUploadFileTypeSet (unit, L7_FILE_TYPE_DHCP_SNOOPING_BINDINGS);

  usmDbTransferDownloadFileTypeSet (unit,L7_FILE_TYPE_DHCP_SNOOPING_BINDINGS);

  usmDbTransferFilePathRemoteSet (unit,"./");

  usmDbTransferFileNameRemoteSet (unit, dsCfgData->dsDbFileName);

  /* usmDbTransferFileNameLocalSet  (unit, "ds.txt"); */

  osapiInetNtoa((L7_uint32) dsCfgData->dsDbIpAddr,ipStr);

  if(usmDbParseInetAddrFromIPAddrHostNameStr(ipStr, &inetAddr) != L7_SUCCESS)
  {
    return;
  }

  /* setting the address type of the tftp server */
  rc = usmDbTransferServerAddressTypeSet(unit, (L7_uint32)inetAddr.family);
  if(rc == L7_SUCCESS)
  {
    /* setting the tftp server's address */
    rc = usmDbTransferServerAddressSet(unit, (L7_uchar8 *)(&(inetAddr.addr)));
  }

  rc = usmDbTransferUpStartSet(unit);

}

/*********************************************************************
* @purpose  Initialize the tftp parameters and download the DHCP
*           Snooping database.
*
* @notes   Success or failure of this TFTP transfer will be
*          asynchronously notified to the dhcp snooping application.
*
*
* @end
********************************************************************/
void dsTftpDownloadInit()
{

  L7_int32 unit = 1;
  L7_inet_addr_t inetAddr;
  L7_RC_t rc;
  L7_char8 ipStr[128];

  if ( usmDbTransferInProgressGet(unit) == L7_TRUE )
  {
    return;
  }

  usmDbTransferContextSet(unit, NULL);

  usmDbTransferModeSet(unit, L7_TRANSFER_TFTP);

  usmDbTransferUploadFileTypeSet (unit, L7_FILE_TYPE_DHCP_SNOOPING_BINDINGS);

  usmDbTransferDownloadFileTypeSet (unit,L7_FILE_TYPE_DHCP_SNOOPING_BINDINGS);

  usmDbTransferFilePathRemoteSet (unit,"./");

  usmDbTransferFileNameRemoteSet (unit, dsCfgData->dsDbFileName);

  usmDbTransferFileNameLocalSet  (unit, DHCP_SNOOPING_DOWNLOAD_FILE_NAME);

  osapiInetNtoa((L7_uint32) dsCfgData->dsDbIpAddr,ipStr);

  if(usmDbParseInetAddrFromIPAddrHostNameStr(ipStr, &inetAddr) != L7_SUCCESS)
  {
    return;
  }

  /* setting the address type of the tftp server */
  rc = usmDbTransferServerAddressTypeSet(unit, (L7_uint32)inetAddr.family);
  if(rc == L7_SUCCESS)
  {
    /* setting the tftp server's address */
    rc = usmDbTransferServerAddressSet(unit, (L7_uchar8 *)(&(inetAddr.addr)));
  }
  rc = usmDbTransferDownStartSet(unit);
}
