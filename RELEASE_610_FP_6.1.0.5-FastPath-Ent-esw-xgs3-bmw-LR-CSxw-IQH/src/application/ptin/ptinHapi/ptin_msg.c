#include <unistd.h>
#include <math.h>
#include <string.h>
#include "ptin_msg.h"
#include "ptin_interface.h"
#include "ptin_flow.h"
#include "ptin_snoop_stats.h"
#include "comm_mask.h"
#include "tty_ptin.h"
#include "osapi.h"
#include "usmdb_nim_api.h"
#include "usmdb_dot1s_api.h"
#include "usmdb_dot1q_api.h"
#include "usmdb_dot3ad_api.h"
#include "usmdb_isdp_api.h"
#include "dot3ad_exports.h"
#include "usmdb_util_api.h"
#include "usmdb_snooping_api.h"
#include "snooping_api.h"
#include "traces.h"
#include "ptin_dhcp_database.h"
#include "usmdb_ip_base_api.h"
#include "usmdb_sim_api.h"
#include "usmdb_mib_vlan_api.h"
#include "usmdb_qos_cos_api.h"
#include "usmdb_dhcp_snooping.h"
#include "ptinHapi_cnfgr.h"
#include "IPC.h"

#include "ptin/logger.h"

typedef struct _st_ptin_monitor
{
  L7_HWEthPhyConf           PhyConf;
  //st_HWEthPhyState  PhyState;
  //st_HWEthSFPstatus_Block SfpStatus;
  //st_HWSFFInfo_Block      SfpInfo;
  //st_HWEthRFC2819_PortStatistics stats;
} st_ptin_monitor;

st_ptin_monitor ptin_monitor[L7_INTF_INDICES];


// IGMP querier, IP address
st_snooping snooping_conf={L7_DISABLE,L7_DISABLE,0xC0A80001,60,60,2,2};


L7_RC_t ptin_msg_HwResources(st_msg_ptin_hw_resources *rsc)
{
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d)",__FILE__,__LINE__);

  rsc->err_code = 0;
  return dtlPtinHwResources(&rsc->resources);
}


L7_RC_t ptin_msg_showNtwConnectivity(L7_NtwConnectivity *connection)
{
  L7_uint32 value;

  // Validate arguments
  if (connection==L7_NULLPTR) {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid parameters",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  if (usmDbAgentIpIfAddressGet(1, &value)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error with usmDbAgentIpIfAddressGet",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  connection->ipaddr = value;

  if (usmDbAgentIpIfNetMaskGet(1, &value)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error with usmDbAgentIpIfNetMaskGet",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  connection->netmask = value;

  if (usmDbAgentIpIfDefaultRouterGet(1, &value)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error with usmDbAgentIpIfDefaultRouterGet",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  connection->defGateway = value;

  if (usmDbMgmtVlanIdGet(1,&value)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error with usmDbMgmtVlanIdGet",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  connection->MngmVlanId = value;

  connection->err_code = 0;
  usmDbSwDevCtrlBurnedInMacAddrGet(1,connection->localMacAddr);
  //memset(connection->localMacAddr,0x00,sizeof(uint8)*6);
  connection->ntwConfProtocol = 0;

  return L7_SUCCESS;
}


L7_RC_t ptin_msg_ntwConnectivity(L7_NtwConnectivity *connection)
{
  L7_uint32 errorNum;
  L7_uint32 oldIpAddr, oldNetMask, oldGateway;
  L7_HwEthernetMef10EvcBundling evc;
  L7_uint8 p;
  L7_RC_t rc;

  // Validate arguments
  if (connection==L7_NULLPTR) {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid parameters",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  /* Get the old ip address & subnet mask, and gateway in case something fails */
  if (usmDbAgentIpIfAddressGet(1, &oldIpAddr)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error with usmDbAgentIpIfAddressGet",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  if (usmDbAgentIpIfNetMaskGet(1, &oldNetMask)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error with usmDbAgentIpIfNetMaskGet",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  if (usmDbAgentIpIfDefaultRouterGet(1, &oldGateway)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error with usmDbAgentIpIfDefaultRouterGet",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  /* Verify the validity of the ip address, subnet mask & gateway together */
//if (usmDbIpInfoValidate(connection->ipaddr, connection->netmask, connection->defGateway) != L7_SUCCESS)
//{
//  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Not valid addresses",__FUNCTION__,__LINE__);
//  return L7_FAILURE;
//}

  /* If the triad is valid reset gateway, ip address, and netmask */
  if (usmDbAgentIpIfDefaultRouterSet(1, 0)!=L7_SUCCESS)            /* CLEAR GATEWAY */
  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error with usmDbAgentIpIfDefaultRouterSet",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Configure ip and netmask, only if they are different
  if ( connection->ipaddr!=oldIpAddr || connection->netmask!=oldNetMask )
  {
    if (usmDbSystemIPAndNetMaskSet(1,connection->ipaddr,connection->netmask,&errorNum)!=L7_SUCCESS)
    {
      usmDbAgentIpIfDefaultRouterSet(1, oldGateway);
      return L7_FAILURE;
    }
  }

  // Only change the default gateway, if not null
  if ( connection->defGateway!=0 )
  {
    if (usmDbAgentIpIfDefaultRouterSet(1, connection->defGateway) != L7_SUCCESS) /* SET GATEWAY */
    {
      /* Revert back changes and display reason for failure, does not check validity */
      usmDbAgentIpIfDefaultRouterSet(1, oldGateway);                              /* RESET GATEWAY */
  
      /* only if ip or netmask changed */
      if ( connection->ipaddr!=oldIpAddr || connection->netmask!=oldNetMask )
      {
        usmDbAgentIpIfAddressSet(1, oldIpAddr);                                     /* RESET IP */
        usmDbAgentIpIfNetMaskSet(1, oldNetMask);                                    /* RESET NETMASK */
      }
      return L7_FAILURE;
    }
  }
  else {
    // If Default Gateway is null, revert to the old value
    usmDbAgentIpIfDefaultRouterSet(1, oldGateway);
  }

  if (connection->MngmVlanId<L7_DOT1Q_MIN_VLAN_ID || connection->MngmVlanId>L7_DOT1Q_MAX_VLAN_ID ||
      usmDbMgmtVlanIdSet(1,L7_DOT1Q_MAX_VLAN_ID)!=L7_SUCCESS)
  {
    // Revert Gateway IP
    if ( connection->defGateway!=0 )
    {
      usmDbAgentIpIfDefaultRouterSet(1, oldGateway);                              /* RESET GATEWAY */
    }
    // only if ip or netmask changed
    if ( connection->ipaddr!=oldIpAddr || connection->netmask!=oldNetMask )
    {
      usmDbAgentIpIfAddressSet(1, oldIpAddr);                                     /* RESET IP */
      usmDbAgentIpIfNetMaskSet(1, oldNetMask);                                    /* RESET NETMASK */
    }
  }

  // Remove EVC 0
  if (ptin_flow_kill(0)!=L7_SUCCESS)  {
    // Revert Gateway IP
    if ( connection->defGateway!=0 )
    {
      usmDbAgentIpIfDefaultRouterSet(1, oldGateway);                              /* RESET GATEWAY */
    }
    // only if ip or netmask changed
    if ( connection->ipaddr!=oldIpAddr || connection->netmask!=oldNetMask )
    {
      usmDbAgentIpIfAddressSet(1, oldIpAddr);                                     /* RESET IP */
      usmDbAgentIpIfNetMaskSet(1, oldNetMask);                                    /* RESET NETMASK */
    }
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error destroying EVC 0",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Only recreate the inband EVC, if ipaddr is not null
  if (connection->ipaddr!=0)
  {
    // Add EVC 0
    evc.err_code = 0;
    evc.index    = 0;      // EVC 0
    evc.admin    = L7_TRUE;
    evc.type     = FLOW_TYPE_UNICAST;
    evc.bundling = 0;
    evc.outerTag = 0;
    // PON ports are not to be used
    for (p=0; p<L7_SYSTEM_N_PORTS; p++) {
      evc.uni[p].type  = MEF10_EVC_UNITYPE_UNUSED;
      evc.uni[p].ceVid = (L7_uint16) -1;
    }
    // All external Eth ports to be grouped
    for (p=igmp_firstRootPort; p<L7_SYSTEM_N_PORTS; p++) {
      evc.uni[p].type  = MEF10_EVC_UNITYPE_ROOT;
      evc.uni[p].ceVid = connection->MngmVlanId;
    }
    if ((rc=ptin_flow_create(&evc))!=L7_SUCCESS)
    {
      // Revert Gateway IP
      if ( connection->defGateway!=0 )
      {
        usmDbAgentIpIfDefaultRouterSet(1, oldGateway);                              /* RESET GATEWAY */
      }
      // only if ip or netmask changed
      if ( connection->ipaddr!=oldIpAddr || connection->netmask!=oldNetMask )
      {
        usmDbAgentIpIfAddressSet(1, oldIpAddr);                                     /* RESET IP */
        usmDbAgentIpIfNetMaskSet(1, oldNetMask);                                    /* RESET NETMASK */
      }
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error creating EVC 0",__FUNCTION__,__LINE__);
      return rc;
    }
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_routeConnectivity(L7_RouteConnectivity *connection)
{
  L7_uint32 oldGateway;

    // Validate arguments
  if (connection==L7_NULLPTR) {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid parameters",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  /* Get the old gateway in case something fails */
  if (usmDbAgentIpIfDefaultRouterGet(1, &oldGateway)!=L7_SUCCESS)
  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error with usmDbAgentIpIfDefaultRouterGet",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Add default gateway (0), or normal route (1)
  if (connection->routeOp==0 || connection->routeOp==1)
  {
    if (usmDbAgentIpIfDefaultRouterSet(1, connection->ipaddr) != L7_SUCCESS) /* SET GATEWAY */
    {
      /* Revert back changes and display reason for failure, does not check validity */
      usmDbAgentIpIfDefaultRouterSet(1, oldGateway);                              /* RESET GATEWAY */
      return L7_FAILURE;
    }
  }
  // Remove (2) or remove all (3) routes
  else if (connection->routeOp==2 || connection->routeOp==3)
  {
    if (usmDbAgentIpIfDefaultRouterSet(1, 0) != L7_SUCCESS) /* SET GATEWAY */
    {
      /* Revert back changes and display reason for failure, does not check validity */
      usmDbAgentIpIfDefaultRouterSet(1, oldGateway);                              /* RESET GATEWAY */
      return L7_FAILURE;
    }
  }
  else
  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Route operation not supported",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


L7_RC_t ptin_msg_dhcpOpt82Get(L7_HwEthernetDhcpOpt82Profile *profile)
{
  L7_uint16 port, evcId;
  L7_uint32 intIfNum;
  L7_uint16 vlanId, innerVlanId;

  if (profile==L7_NULLPTR)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Invalid parameters");
    return L7_FAILURE;
  }

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Going to read DHCP profile");

  port  = (L7_uint16) (profile->index & 0xFF);
  if ((port & 0xFF)!=0xFF && (port & 0x80) && (port & 0x7F)>0)  port = L7_SYSTEM_N_PORTS-1+(port & 0x7F);
  evcId = (L7_uint16) ((profile->index>>48) & 0xFFFF);
  innerVlanId = (L7_uint16) ((profile->index>>32) & 0xFFFF);

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "evcId=%u",evcId);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "port=%u",port);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "innerVlanId=%u",innerVlanId);

  // Validate innervlanId
  if (port>=L7_SYSTEM_N_INTERF || evcId==0 || evcId>=FLOWS_MAX || innerVlanId>4095) {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Invalid arguments");
    return L7_FAILURE;
  }

  // Get associated vlan (port, evcId and vlanId are validated)
  if (ptin_flow_getVlan(port, evcId, &vlanId)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with ptin_MCflow_getVlan");
    return L7_FAILURE;
  }
  // Get interface internal reference
  if (ptin_translate_port2intf(port, &intIfNum)!=L7_SUCCESS || intIfNum==0) {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with ptin_translate_port2intf(%u,&intIfNum)",port);
    return L7_FAILURE;
  }
  if (ptin_dhcp_database_get(intIfNum, vlanId, innerVlanId, profile->circuitId, profile->remoteId)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error adding strings to dhcp database");
    return L7_FAILURE;
  }

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "circuitId=\"%s\"",profile->circuitId);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "remoteId =\"%s\"",profile->remoteId);

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_dhcpOpt82AccessNodeUpdate(L7_HwEthernetAccessNodeId *accessnode_msg)
{
  if (accessnode_msg==L7_NULLPTR)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Invalid parameters");
    return L7_FAILURE;
  }

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Going to set a new Access Node Identifier");

  if (ptin_dhcp_database_accessNodeId_update(accessnode_msg->access_node_id, accessnode_msg->L2Type, accessnode_msg->slot)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error updating access node identifier");
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_dhcpOpt82Config(L7_HwEthernetDhcpOpt82Profile *profile)
{
  L7_uint16 p,port, evcId;
  L7_uint32 intIfNum;
  L7_uint16 vlanId, innerVlanId;

  if (profile==L7_NULLPTR)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Invalid parameters");
    return L7_FAILURE;
  }
  
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Going to add DHCP profile");
    
  port  = (L7_uint16) (profile->index & 0xFF);
  if ((port & 0xFF)!=0xFF && (port & 0x80) && (port & 0x7F)>0)  port = L7_SYSTEM_N_PORTS-1+(port & 0x7F);
  evcId = (L7_uint16) ((profile->index>>48) & 0xFFFF);
  innerVlanId = (L7_uint16) ((profile->index>>32) & 0xFFFF);

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "evcId=%u",evcId);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "port=%u",port);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "innerVlanId=%u",innerVlanId);

  // Validate innervlanId
  if (innerVlanId>4095) {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Invalid innerVlanId (%u)",innerVlanId);
    return L7_FAILURE;
  }
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "circuitId=\"%s\"",profile->circuitId);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "remoteId =\"%s\"",profile->remoteId);

  if (port<L7_SYSTEM_N_INTERF) {
    // Get associated vlan (port, evcId and vlanId are validated)
    if (ptin_flow_getVlan(port, evcId, &vlanId)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with ptin_MCflow_getVlan");
      return L7_FAILURE;
    }
    // Get interface internal reference
    if (ptin_translate_port2intf(port, &intIfNum)!=L7_SUCCESS || intIfNum==0) {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with ptin_translate_port2intf(%u,&intIfNum)",port);
      return L7_FAILURE;
    }
    if (ptin_dhcp_database_config(intIfNum, vlanId, innerVlanId, profile->circuitId, profile->remoteId)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error adding strings to dhcp database");
      return L7_FAILURE;
    }
  }
  else  {
    for (p=0; p<L7_SYSTEM_N_INTERF; p++) {
      // Get associated vlan (port, evcId and vlanId are validated)
      if (ptin_flow_getVlan(p, evcId, &vlanId)!=L7_SUCCESS)
        continue;
      // Get interface internal reference
      if (ptin_translate_port2intf(p, &intIfNum)!=L7_SUCCESS || intIfNum==0) {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with ptin_translate_port2intf(%u,&intIfNum)",port);
        return L7_FAILURE;
      }
      if (ptin_dhcp_database_config(intIfNum, vlanId, innerVlanId, profile->circuitId, profile->remoteId)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error adding strings to dhcp database");
        return L7_FAILURE;
      }
    }
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_dhcpOpt82Remove(L7_HwEthernetDhcpOpt82Profile *profile)
{
  L7_uint16 p, port, evcId;
  L7_uint32 intIfNum;
  L7_uint16 vlanId, innerVlanId;

  if (profile==L7_NULLPTR)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Invalid parameters");
    return L7_FAILURE;
  }

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Going to remove DHCP profile");

  port  = (L7_uint16) (profile->index & 0xFF);
  if ((port & 0xFF)!=0xFF && (port & 0x80) && (port & 0x7F)>0)  port = L7_SYSTEM_N_PORTS-1+(port & 0x7F);
  evcId = (L7_uint16) ((profile->index>>48) & 0xFFFF);
  innerVlanId = (L7_uint16) ((profile->index>>32) & 0xFFFF);

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "evcId=%u",evcId);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "port=%u",port);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "innerVlanId=%u",innerVlanId);

    // Validate innervlanId
  if (innerVlanId>4095) {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Invalid innerVlanId (%u)",innerVlanId);
    return L7_FAILURE;
  }

  if (port<L7_SYSTEM_N_INTERF) {
    // Get associated vlan (port, evcId and vlanId are validated)
    if (ptin_flow_getVlan(port, evcId, &vlanId)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with ptin_MCflow_getVlan");
      return L7_FAILURE;
    }
    // Get interface internal reference
    if (ptin_translate_port2intf(port, &intIfNum)!=L7_SUCCESS || intIfNum==0) {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with ptin_translate_port2intf(%u,&intIfNum)",port);
      return L7_FAILURE;
    }
    
    if (ptin_dhcp_database_remove(intIfNum, vlanId, innerVlanId)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error removing dhcp database entry");
      return L7_FAILURE;
    }
  }
  else  {
    for (p=0; p<L7_SYSTEM_N_INTERF; p++) {
      // Get associated vlan (port, evcId and vlanId are validated)
      if (ptin_flow_getVlan(p, evcId, &vlanId)!=L7_SUCCESS)
        continue;

      // Get interface internal reference
      if (ptin_translate_port2intf(p, &intIfNum)!=L7_SUCCESS || intIfNum==0) {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with ptin_translate_port2intf(%u,&intIfNum)",port);
        return L7_FAILURE;
      }
      if (ptin_dhcp_database_remove(intIfNum, vlanId, innerVlanId)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error removing dhcp database entries");
        return L7_FAILURE;
      }
    }
  }

  return L7_SUCCESS;
}


L7_RC_t ptin_msg_LAGGet(L7_LACPLagInfo *lag_info, L7_uint8 *nLags)
{
  L7_uint32 member_list[L7_SYSTEM_N_PORTS]; /* Internal interface numbers of portChannel members */
  L7_uint32 lag_intf, value;
  L7_int    i, port, lag_port;
  L7_uint8  lag_index, lag_i, number_of_lags=0;
  L7_uint32 member_pbmp, nPorts;

  // Validate arguments
  if (lag_info==L7_NULLPTR || nLags==L7_NULLPTR) {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Null pointers");
    return L7_FAILURE;
  }

  // Lag index
  lag_index = (L7_uint8) (lag_info->index & 0xFF);

  // Validate lag index
  if (lag_index>L7_SYSTEM_N_LAGS && lag_index<(L7_uint8)-1)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Invalid parameters");
    return L7_FAILURE;
  }

  // Read only one lag
  if (lag_index>0 && lag_index<=L7_SYSTEM_N_LAGS)  {

    lag_port = L7_SYSTEM_N_PORTS+lag_index-1;
  
    if (ptin_translate_port2intf(lag_port,&lag_intf)!=L7_SUCCESS || lag_intf==0) {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "This LAG does not exist");
      return L7_FAILURE;
    }
  
    // Check if is a lag interface
    if (!usmDbDot3adValidIntfCheck(1,lag_intf)) {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "This is not a LAG interface");
      return L7_FAILURE;
    }
  
    // LAG admin
    if (usmDbDot3adAdminModeGet(1,lag_intf,&value)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error reading admin (%u)",lag_port);
      return L7_FAILURE;
    }
    lag_info->admin = value;
  
    // LAG STP Mode
    if (usmDbDot1sPortStateGet(1,lag_intf,&value)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot1sPortStateGet");
      return L7_FAILURE;
    }
    lag_info->stp_enable = value;
  
    // LAG Static Mode
    if (usmDbDot3adIsStaticLag(1,lag_intf,&value)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot3adIsStaticLag");
      return L7_FAILURE;
    }
    lag_info->static_enable = value;
  
    // LAG Balance Mode
    if (usmDbDot3adLagHashModeGet(1,lag_intf,&value)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot3adLagHashModeGet");
      return L7_FAILURE;
    }
    lag_info->loadBalance_mode = value;
  
    member_pbmp = 0;
    nPorts = L7_SYSTEM_N_PORTS;
    if (usmDbDot3adMemberListGet(1,lag_intf,&nPorts,member_list)!=L7_SUCCESS)  {
      return L7_FAILURE;
    }
    for (i=0; i<nPorts; i++) {
      // Validate interface number
      if (member_list[i]==0 || ptin_translate_intf2port(member_list[i],&port)!=L7_SUCCESS || port<0 || port>=L7_SYSTEM_N_PORTS)
        continue;
      member_pbmp |= (L7_uint32) 1<<port;
    }
    lag_info->members_pbmp = member_pbmp;

    number_of_lags = 1;
  }
  // Real all lags
  else  {
    number_of_lags = 0;
    for (lag_i=1; lag_i<=L7_SYSTEM_N_LAGS; lag_i++) {

      lag_port = L7_SYSTEM_N_PORTS+lag_i-1;

      // If this lag does not exist, skip to the next one
      if (ptin_translate_port2intf(lag_port,&lag_intf)!=L7_SUCCESS || lag_intf==0) {
        //DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "This LAG does not exist");
        continue;
      }

      // Check if is a lag interface
      if (!usmDbDot3adValidIntfCheck(1,lag_intf)) {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "This is not a LAG interface");
        return L7_FAILURE;
      }

      // Initialize some variables
      lag_info[number_of_lags].err_code = 0;
      lag_info[number_of_lags].index    = lag_i;

      // LAG admin
      if (usmDbDot3adAdminModeGet(1,lag_intf,&value)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error reading admin (%u)",lag_port);
        return L7_FAILURE;
      }
      lag_info[number_of_lags].admin = value;

      // LAG STP Mode
      if (usmDbDot1sPortStateGet(1,lag_intf,&value)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot1sPortStateGet");
        return L7_FAILURE;
      }
      lag_info[number_of_lags].stp_enable = value;

      // LAG Static Mode
      if (usmDbDot3adIsStaticLag(1,lag_intf,&value)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot3adIsStaticLag");
        return L7_FAILURE;
      }
      lag_info[number_of_lags].static_enable = value;

      // LAG Balance Mode
      if (usmDbDot3adLagHashModeGet(1,lag_intf,&value)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot3adLagHashModeGet");
        return L7_FAILURE;
      }
      lag_info[number_of_lags].loadBalance_mode = value;

      member_pbmp = 0;
      nPorts = L7_SYSTEM_N_PORTS;
      if (usmDbDot3adMemberListGet(1,lag_intf,&nPorts,member_list)!=L7_SUCCESS)  {
        return L7_FAILURE;
      }
      for (i=0; i<nPorts; i++) {
        // Validate interface number
        if (member_list[i]==0 || ptin_translate_intf2port(member_list[i],&port)!=L7_SUCCESS || port<0 || port>=L7_SYSTEM_N_PORTS)
          continue;
        member_pbmp |= (L7_uint32) 1<<port;
      }
      lag_info[number_of_lags].members_pbmp = member_pbmp;

      // Increment number of lags
      number_of_lags++;
    }
  }
 
  *nLags = number_of_lags;

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LACP read successfully");
  return L7_SUCCESS;
}


L7_RC_t ptin_msg_LAGCreate(L7_LACPLagInfo *lag_info)
{
  char lag_name[21];
  L7_uint32 member[L7_MAX_MEMBERS_PER_LAG]; /* Internal interface numbers of portChannel members */
  L7_uint32 lag_intf, lag_intf_tmp, intf;
  L7_uint8  port, lag_port;
  L7_uint32 member_pbmp, new_member_pbmp;
  L7_uint32 mode, val;
  L7_BOOL bool_val;
  L7_uint16 inBand_vlanId=0;
  L7_RC_t rc = L7_SUCCESS;

  // Validate lag index
  if (lag_info->index==0 || lag_info->index>L7_SYSTEM_N_LAGS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Invalid parameters (%u)",lag_info->index);
    return L7_FAILURE;
  }

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Admin=%u",lag_info->admin);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "index=%u",lag_info->index);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "loadbalance=%u",lag_info->loadBalance_mode);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "index=0x%X",lag_info->members_pbmp);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "static=%u",lag_info->static_enable);
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "stp=%u",lag_info->stp_enable);

  // Lag port index
  lag_port = L7_SYSTEM_N_PORTS+lag_info->index-1;

  // Check if this lag exists (lag_intf is null)
  if (ptin_translate_port2intf(lag_port,&lag_intf)!=L7_SUCCESS) {
    lag_intf = 0;
  }

  // Get list of lag members (if lag exists... otherwise is null)
  if (lag_intf==0 || ptin_lag_memberList_get(lag_port,&member_pbmp)!=L7_SUCCESS)
  {
    member_pbmp = 0;
  }
  // Final list with new ports and old
  new_member_pbmp = (member_pbmp | lag_info->members_pbmp);

  // This list must not be null
  if (new_member_pbmp==0)
  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "List of ports is null");
    return L7_FAILURE;
  }

  // Check if these ports exist in any EVC... if so, we have an error
  for (port=0; port<L7_SYSTEM_N_INTERF && new_member_pbmp!=0; port++,new_member_pbmp>>=1)
  {
    // Next
    if (!(new_member_pbmp & 1))  continue;

    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Checking if port %u is being used in any EVC",port);

    if (ptin_flow_port_exists(port))
    {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Port %u is being used in one EVC",port);
      return L7_FAILURE;
    }
  }

  // Does inBand flow exists?
  if (!ptin_flow_inBand_exists(&inBand_vlanId))  inBand_vlanId=0;

  // if inBand is configured, destroy (temporarily) this EVC
  if (inBand_vlanId!=0)
  {
    if (ptin_flow_destroy_inBand()!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error destroying inBand EVC (vlan=%u)",inBand_vlanId);
      return L7_FAILURE;
    }
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "inBand EVC destroyed (vlan=%u)",inBand_vlanId);
  }

  // If Lag doesn't exist, create it
  if (lag_intf==0) {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Going to create lag %u",lag_port);
    // LAG name
    sprintf(lag_name,"lag%02u",(L7_uint16) lag_info->index);
  
    /* Set all member values to zero, allowing us to create a portChannel without any members */
    /* Members will be created at a later stage using the add portChannel members command */
    memset ((L7_char8 *)member, 0, sizeof(L7_uint32)*L7_MAX_MEMBERS_PER_LAG);
    /* Verify that we haven't already created the maximum number of port-channels (lags). */
  
    if (usmDbDot3adCreateSet(1,lag_name,FD_DOT3AD_ADMIN_MODE,FD_DOT3AD_LINK_TRAP_MODE,FD_DOT3AD_MEMBER_UNIT,FD_DOT3AD_HASH_MODE,member,&lag_intf)!=L7_SUCCESS) {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot3adCreateSet");
      ptin_flow_create_inBand(inBand_vlanId);
      return L7_FAILURE;
    }

    // Wait until new lag exists
    do {
      usleep(10000);
      if (usmDbIntfStatusGet(lag_intf,&mode)!=L7_SUCCESS)  {
        mode = L7_INTF_UNINITIALIZED;
      }
    } while (mode!=L7_INTF_ATTACHED);
    
    // Some initializations
    if (usmDbIfConfigMaxFrameSizeSet(lag_intf, 2048)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error setting maxframe in lag_port %u",lag_port);
      usmDbDot3adRemoveSet(1, lag_intf);
      // Wait while lag still exists
      do {
        usleep(10000);
        if (usmDbIntfStatusGet(lag_intf,&mode)!=L7_SUCCESS)  {
          mode = L7_INTF_UNINITIALIZED;
        }
      } while (mode!=L7_INTF_UNINITIALIZED);
      ptin_flow_create_inBand(inBand_vlanId);
      return L7_FAILURE;
    }

    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) New LAG intIfNum is %u",__FUNCTION__,__LINE__,lag_intf);
  }

  // Now, starts the configuration

  // LAG STP Mode
  if (usmDbDot1sPortStateGet(1,lag_intf,&bool_val)!=L7_SUCCESS || bool_val!=(lag_info->stp_enable & 1))
  {
    if (usmDbDot1sPortStateSet(1,lag_intf,lag_info->stp_enable & 1)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot1sPortStateSet");
      rc = L7_FAILURE;
    }
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "usmDbDot1sPortStateSet executed");
  }

  // LAG Balance Mode
  if (usmDbDot3adLagHashModeGet(1,lag_intf,&mode)!=L7_SUCCESS || mode!=lag_info->loadBalance_mode)
  {
    if (usmDbDot3adLagHashModeSet(1,lag_intf,lag_info->loadBalance_mode)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot3adLagHashModeSet");
      rc = L7_FAILURE;
    }
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "usmDbDot3adLagHashModeSet executed");
  }

  // LAG Static Mode
  if (usmDbDot3adIsStaticLag(1,lag_intf,&bool_val)!=L7_SUCCESS || bool_val!=(lag_info->static_enable & 1))
  {
    if (usmDbDot3adLagStaticModeSet(1,lag_intf,lag_info->static_enable & 1)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot3adLagStaticModeSet");
      rc = L7_FAILURE;
    }
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "usmDbDot3adLagStaticModeSet executed");
  }

  // LAG Collector max delay
  if (usmDbDot3adAggCollectorMaxDelayGet(1,lag_intf,&val)!=L7_SUCCESS || val!=0)
  {
    if (usmDbDot3adAggCollectorMaxDelaySet(1,lag_intf,0)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot3adAggCollectorMaxDelaySet");
      rc = L7_FAILURE;
    }
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "usmDbDot3adAggCollectorMaxDelaySet executed");
  }

  // ISDP mode
  if (usmdbIsdpIntfModeGet(lag_intf,&mode)!=L7_SUCCESS || mode!=L7_DISABLE)
  {
    if (usmdbIsdpIntfModeSet(lag_intf,L7_DISABLE)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbIsdpIntfModeSet");
      rc = L7_FAILURE;
    }
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "usmdbIsdpIntfModeSet executed");
  }

  member_pbmp = lag_info->members_pbmp;
  // Do not allow client interafaces, as part of the LAG
  member_pbmp = (member_pbmp>>igmp_firstRootPort) << igmp_firstRootPort;

  for (port=0; port<L7_SYSTEM_N_PORTS; port++, member_pbmp>>=1) {

    // Initialize admin array
    //admin[port] = (L7_uint32)-1;

    // Calculate ionterface id
    if (ptin_translate_port2intf(port,&intf)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with ptin_translate_port2intf(%u,&intf)",port);
      rc = L7_FAILURE;
      continue;
    }

    // Is LACP enabled in this interface?
    if (usmDbDot3adAggPortLacpModeGet(1,intf,&mode)!=L7_SUCCESS || mode!=L7_ENABLE) {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error: This is not an LACP interface (port=%u, intf=%u)",port,intf);
      continue;
    }

    // Port member
    if ((member_pbmp & 1))
    {
      // Check if this interface already belongs to a LAG
      if (dot3adAggGet(intf,&lag_intf_tmp)==L7_SUCCESS && lag_intf_tmp!=0)  {
        if (lag_intf!=lag_intf_tmp) {
          DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error: Port %u already belongs to another lag",port);
        }
        continue;
      }

      // Disable auto-negotiation
      if (usmDbIfAutoNegoStatusCapabilitiesGet(intf,&val)!=L7_SUCCESS || val!=L7_DISABLE)  {
        if (usmDbIfAutoNegoStatusCapabilitiesSet(intf,L7_DISABLE)!=L7_SUCCESS) {
          DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error with usmDbIfAutoNegoStatusCapabilitiesSet(1,%u,L7_DISABLE)",__FUNCTION__,__LINE__,intf);
          rc = L7_FAILURE;
          continue;
        }
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "usmDbIfAutoNegoStatusCapabilitiesSet executed");
      }
      
      // Actor Admin Key Set
      if (usmDbDot3adAggPortActorAdminKeyGet(1,intf,&val)!=L7_SUCCESS || val!=lag_intf) {
        if (usmDbDot3adAggPortActorAdminKeySet(1,intf,lag_intf /*54+lag_info->index-1*/)!=L7_SUCCESS) {
          DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error with usmDbDot3adAggPortActorAdminKeySet(1,%u,%u)",__FUNCTION__,__LINE__,intf,lag_intf);
          rc = L7_FAILURE;
          continue;
        }
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "usmDbDot3adAggPortActorAdminKeySet executed");
      }

      // Add member
      if (usmDbDot3adMemberAddSet(1,lag_intf,intf)!=L7_SUCCESS) {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot3adMemberAddSet(1,%u,%u)",lag_intf,intf);
        rc = L7_FAILURE;
        continue;
      }

      // Add translation entries to EVCs using this LAG
      if (ptin_flow_lag_port_add(lag_port,port)!=L7_SUCCESS) {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with ptin_flow_lag_port_add(%u,%u)",lag_port,port);
        usmDbDot3adMemberDeleteSet(1,lag_intf,intf);
        rc = L7_FAILURE;
        continue;
      }

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Port %u added",port);
    }
    // Port not member
    else  {
      // If this interface does not belong to any lag, do nothing
      if (dot3adAggGet(intf,&lag_intf_tmp)!=L7_SUCCESS || lag_intf_tmp==0)  {
        continue;
      }
      // If belongs to a lag, but a different one, do nothing
      if (lag_intf_tmp!=lag_intf) {
        continue;
      }

      // Remove translation entries to EVCs using this LAG
      if (ptin_flow_lag_port_delete(lag_port,port)!=L7_SUCCESS) {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with ptin_flow_lag_port_delete(%u,%u)",lag_port,port);
        rc = L7_FAILURE;
        continue;
      }

      // At this point, we know this interface belongs to this lag. Just remove it.
      if (usmDbDot3adMemberDeleteSet(1,lag_intf,intf)!=L7_SUCCESS) {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error with usmDbDot3adMemberDeleteSet(1,%u,%u)",__FUNCTION__,__LINE__,lag_intf,intf);
        rc = L7_FAILURE;
        continue;
      }

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Port %u removed",port);
    }
  }

  // Remove this interface from vlan 1
  if (usmDbVlanMemberSet(1,1,lag_intf,L7_DOT1Q_FORBIDDEN,DOT1Q_SWPORT_MODE_NONE)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s:%s:%d\n",__FILE__,__FUNCTION__,__LINE__);
    rc = L7_FAILURE;
  }
 
  // Save the lag_intf reference
  if (ptin_translate_LagPortIntfSet(lag_port, lag_intf)!=L7_SUCCESS)  {
    rc = L7_FAILURE;
  }

  do {
    usleep(10000);
    if (usmDbIntfStatusGet(lag_intf,&mode)!=L7_SUCCESS)  {
      mode = L7_INTF_UNINITIALIZED;
    }
  } while (mode!=L7_INTF_ATTACHED);

  // Wait for configurations be applied
  do {
    usleep(10000);
  } while (!dot3adQueueIsEmpty());
  
  // Wait for members to be active
  do  {
    usleep(10000);
  } while (dot3adLagPendingConfigsOnHW()>0);
  
  // Wait for members to be active
  do  {
    usleep(10000);
  } while (!usmDbDot3adIsConfigured(1,lag_intf));
    
  // Wait some time
  usleep(100000);

  // Recreate inband EVC
  if (inBand_vlanId!=0) {
    if ((rc=ptin_flow_create_inBand(inBand_vlanId))!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error recreating inBand EVC (vlanId=%u)",inBand_vlanId);
      return rc;
    }
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "inBand EVC recreated (vlanId=%u)",inBand_vlanId);
  }

  // Success
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LAg created/modified successfully");
  return rc;
}


L7_RC_t ptin_msg_LAGDestroy(L7_LACPLagInfo *lag_info)
{
  L7_uint32 state;
  L7_uint32 lag_intf, lag_port;
  L7_uint16 inBand_vlanId=0;
  L7_RC_t   rc;

  // Validate LAG index
  if (lag_info->index==0 || lag_info->index>L7_SYSTEM_N_LAGS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Invalid parameters (%u)",lag_info->index);
    return L7_FAILURE;
  }

  // Lag port index
  lag_port = L7_SYSTEM_N_PORTS+lag_info->index-1;

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Going to destroy lag %u",lag_port);

  // If Lag does not exist, return success
  if (ptin_translate_port2intf(lag_port,&lag_intf)!=L7_SUCCESS || lag_intf==0)
  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Lag %u doesn't exist... return success",lag_port);
    return L7_SUCCESS;
  }

  // Check if is a lag interface
  if (!usmDbDot3adValidIntfCheck(1,lag_intf)) {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "This is not a LAG interface");
    ptin_translate_LagPortIntfClear(lag_port);
    return L7_FAILURE;
  }

  // If this LAG is configured in some EVC, we have an error
  if (ptin_flow_port_exists(lag_port))
  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Lag %u is defined in one EVC... error!",lag_port);
    return L7_FAILURE;
  }

  // Does inBand flow exists?
  if (!ptin_flow_inBand_exists(&inBand_vlanId))  inBand_vlanId=0;

  // if inBand is configured, destroy (temporarily) this EVClag_info->index
  if (inBand_vlanId!=0)
  {
    if (ptin_flow_destroy_inBand()!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error destroying inBand EVC (vlan=%u)",inBand_vlanId);
      return L7_FAILURE;
    }
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "inBand EVC destroyed (vlan=%u)",inBand_vlanId);
  }

  // Destroy LAG
  if (usmDbDot3adRemoveSet(1, lag_intf)!=L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error removing LAG");
    ptin_flow_create_inBand(inBand_vlanId);
    return L7_FAILURE;
  }

  // Wait while lag still exists
  do {
    usleep(10000);
    if (usmDbIntfStatusGet(lag_intf,&state)!=L7_SUCCESS)  {
      state = L7_INTF_UNINITIALIZED;
    }
  } while (state!=L7_INTF_UNINITIALIZED);

  // Clear LAg reference
  ptin_translate_LagPortIntfClear(lag_port);

  // Wait for configurations be applied
  do {
    usleep(1000);
  } while (!dot3adQueueIsEmpty());

  // Recreate inband EVC
  if (inBand_vlanId!=0) {
    if ((rc=ptin_flow_create_inBand(inBand_vlanId))!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error recreating inBand EVC (vlanId=%u)",inBand_vlanId);
      return rc;
    }
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "inBand EVC recreated (vlanId=%u)",inBand_vlanId);
  }

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LACP removed successfully");
  return L7_SUCCESS;
}


L7_RC_t ptin_msg_LAGStatus(L7_LACPLagStatus *lag_status, L7_uint8 *nLags)
{
  L7_uint32 member_list[L7_SYSTEM_N_PORTS]; /* Internal interface numbers of portChannel members */
  L7_uint32 lag_intf, value;
  L7_int    i, port, lag_port;
  L7_uint8  lag_index, lag_i, number_of_lags=0;
  L7_uint32 member_pbmp, nPorts;

  // Validate arguments
  if (lag_status==L7_NULLPTR || nLags==L7_NULLPTR) {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Null pointers");
    return L7_FAILURE;
  }

  // Lag index
  lag_index = (L7_uint8) (lag_status->index & 0xFF);

  // Validate lag index
  if (lag_index>L7_SYSTEM_N_LAGS && lag_index<(L7_uint8)-1)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Invalid parameters");
    return L7_FAILURE;
  }

  // Only consulting one single lag
  if (lag_index>0 && lag_index<=L7_SYSTEM_N_LAGS) {

    lag_port = L7_SYSTEM_N_PORTS+lag_index-1;
  
    if (ptin_translate_port2intf(lag_port,&lag_intf)!=L7_SUCCESS || lag_intf==0) {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "This LAG does not exist");
      return L7_FAILURE;
    }
  
    // Check if is a lag interface
    if (!usmDbDot3adValidIntfCheck(1,lag_intf)) {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "This is not a LAG interface");
      return L7_FAILURE;
    }
  
    // LAG admin
    if (usmDbDot3adAdminModeGet(1,lag_intf,&value)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error reading admin (%u)",lag_port);
      return L7_FAILURE;
    }
    lag_status->admin = value;
  
    // LAG Static Mode
    if (usmDbDot3adIsStaticLag(1,lag_intf,&value)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot3adIsStaticLag");
      return L7_FAILURE;
    }
    lag_status->port_channel_type = value;
  
    // Link status
    if (nimGetIntfLinkState(lag_intf, &value)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with nimGetIntfLinkState");
      return L7_FAILURE;
    }
    lag_status->link_status = (value==L7_UP);
  
    // Get list of ports  
    member_pbmp = 0;
    nPorts = L7_SYSTEM_N_PORTS;
    if (usmDbDot3adMemberListGet(1,lag_intf,&nPorts,member_list)!=L7_SUCCESS)  {
      return L7_FAILURE;
    }
    for (i=0; i<nPorts; i++) {
      // Validate interface number
      if (member_list[i]==0 || ptin_translate_intf2port(member_list[i],&port)!=L7_SUCCESS || port<0 || port>=L7_SYSTEM_N_PORTS)
        continue;
      member_pbmp |= (L7_uint32) 1<<port;
    }
    lag_status->members_pbmp = member_pbmp;

    // Get list of active ports
    member_pbmp = 0;
    nPorts = L7_SYSTEM_N_PORTS;
    if (usmDbDot3adActiveMemberListGet(1,lag_intf,&nPorts,member_list)!=L7_SUCCESS) {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot3adActiveMemberListGet");
      return L7_FAILURE;
    }
    for (i=0; i<nPorts; i++) {
      // Validate interface number
      if (member_list[i]==0 || ptin_translate_intf2port(member_list[i],&port)!=L7_SUCCESS || port<0 || port>=L7_SYSTEM_N_PORTS)
        continue;
      member_pbmp |= (L7_uint32) 1<<port;
    }
    lag_status->active_members_pbmp = member_pbmp;

    number_of_lags = 1;
  }
  // Read all lags
  else  {
    number_of_lags = 0;
    for (lag_i=1; lag_i<=L7_SYSTEM_N_LAGS; lag_i++) {

      lag_port = L7_SYSTEM_N_PORTS+lag_i-1;
  
      // Check if lag exists... if not, skip to the next one
      if (ptin_translate_port2intf(lag_port,&lag_intf)!=L7_SUCCESS || lag_intf==0) {
        //DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "This LAG does not exist");
        continue;
      }

      // Check if is a lag interface
      if (!usmDbDot3adValidIntfCheck(1,lag_intf)) {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "This is not a LAG interface");
        return L7_FAILURE;
      }
  
      // Initialize some variables
      lag_status[number_of_lags].err_code = 0;
      lag_status[number_of_lags].index    = lag_i;

      // LAG admin
      if (usmDbDot3adAdminModeGet(1,lag_intf,&value)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error reading admin (%u)",lag_port);
        return L7_FAILURE;
      }
      lag_status[number_of_lags].admin = value;
  
      // LAG Static Mode
      if (usmDbDot3adIsStaticLag(1,lag_intf,&value)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot3adIsStaticLag");
        return L7_FAILURE;
      }
      lag_status[number_of_lags].port_channel_type = value;
  
      // Link status
      if (nimGetIntfLinkState(lag_intf, &value)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with nimGetIntfLinkState");
        return L7_FAILURE;
      }
      lag_status[number_of_lags].link_status = (value==L7_UP);
  
      // Get list of ports  
      member_pbmp = 0;
      nPorts = L7_SYSTEM_N_PORTS;
      if (usmDbDot3adMemberListGet(1,lag_intf,&nPorts,member_list)!=L7_SUCCESS)  {
        return L7_FAILURE;
      }
      for (i=0; i<nPorts; i++) {
        // Validate interface number
        if (member_list[i]==0 || ptin_translate_intf2port(member_list[i],&port)!=L7_SUCCESS || port<0 || port>=L7_SYSTEM_N_PORTS)
          continue;
        member_pbmp |= (L7_uint32) 1<<port;
      }
      lag_status[number_of_lags].members_pbmp = member_pbmp;

      // Get list of active ports
      member_pbmp = 0;
      nPorts = L7_SYSTEM_N_PORTS;
      if (usmDbDot3adActiveMemberListGet(1,lag_intf,&nPorts,member_list)!=L7_SUCCESS) {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "Error with usmDbDot3adActiveMemberListGet");
        return L7_FAILURE;
      }
      for (i=0; i<nPorts; i++) {
        // Validate interface number
        if (member_list[i]==0 || ptin_translate_intf2port(member_list[i],&port)!=L7_SUCCESS || port<0 || port>=L7_SYSTEM_N_PORTS)
          continue;
        member_pbmp |= (L7_uint32) 1<<port;
      }
      lag_status[number_of_lags].active_members_pbmp = member_pbmp;

      // Increment number of lags
      number_of_lags++;
    }
  }

  *nLags = number_of_lags;

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_LAGAdminStateSet( L7_LACPAdminState *adminState, L7_uint8 nStructs )
{
  L7_uint8 i, actor_state, partner_state;
  L7_uint32 intf;
  L7_LACPAdminState *ptr;

  if (adminState==L7_NULLPTR || nStructs==0)
    return L7_FAILURE;

  for (i=0; i<nStructs; i++)
  {
    ptr = &adminState[i];

    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Index             = %llu",ptr->index);
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "State aggregation = %u",ptr->state_aggregation);
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LACP Activity     = %u",ptr->lacp_activity);
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LACP Timeout      = %u",ptr->lacp_timeout);

    // Validate port number, ang get interface id
    if (ptr->index>=L7_SYSTEM_N_PORTS || ptin_translate_port2intf((L7_int) ptr->index, &intf)!=L7_SUCCESS || intf==0)
      return L7_FAILURE;

    // Get Current Admin State
    if (usmDbDot3adAggPortActorAdminStateGet(1, intf, &actor_state)!=L7_SUCCESS ||
        usmDbDot3adAggPortPartnerAdminStateGet(1, intf, &partner_state)!=L7_SUCCESS)
      return L7_FAILURE;

    if (ptr->state_aggregation)  {
      actor_state   |=   (L7_uchar8) DOT3AD_STATE_AGGREGATION;
      partner_state |=   (L7_uchar8) DOT3AD_STATE_AGGREGATION;
    }
    else {
      actor_state   &= ~((L7_uchar8) DOT3AD_STATE_AGGREGATION);
      partner_state &= ~((L7_uchar8) DOT3AD_STATE_AGGREGATION);
    }

    if (ptr->lacp_activity) {
      actor_state   |=   (L7_uchar8) DOT3AD_STATE_LACP_ACTIVITY;
      partner_state |=   (L7_uchar8) DOT3AD_STATE_LACP_ACTIVITY;
    }
    else {
      actor_state   &= ~((L7_uchar8) DOT3AD_STATE_LACP_ACTIVITY);
      partner_state &= ~((L7_uchar8) DOT3AD_STATE_LACP_ACTIVITY);
    }

    if (ptr->lacp_timeout) {
      actor_state   |=   (L7_uchar8) DOT3AD_STATE_LACP_TIMEOUT;
      partner_state |=   (L7_uchar8) DOT3AD_STATE_LACP_TIMEOUT;
    }
    else {
      actor_state   &= ~((L7_uchar8) DOT3AD_STATE_LACP_TIMEOUT);
      partner_state &= ~((L7_uchar8) DOT3AD_STATE_LACP_TIMEOUT);
    }

    // Apply new state
    if (usmDbDot3adAggPortActorAdminStateSet(1,intf,&actor_state)!=L7_SUCCESS ||
        usmDbDot3adAggPortPartnerAdminStateSet(1,intf,&partner_state)!=L7_SUCCESS)
      return L7_FAILURE;
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_LAGAdminStateGet( L7_int port, L7_LACPAdminState *adminState, L7_uint8 *nStructs )
{
  L7_uint8 p, state;
  L7_uint32 intf;
  L7_LACPAdminState *ptr;

  if ((port!=-1 && port>=L7_SYSTEM_N_PORTS) || adminState==L7_NULLPTR || nStructs==L7_NULLPTR)
    return L7_FAILURE;

  if (port==-1)
  {
    for (p=0; p<L7_SYSTEM_N_PORTS; p++)
    {
      ptr = &adminState[p];
    
      // get interface id
      if (ptin_translate_port2intf(p, &intf)!=L7_SUCCESS || intf==0)
        return L7_FAILURE;

      // Get Current Admin State
      if (usmDbDot3adAggPortActorAdminStateGet(1, intf, &state)!=L7_SUCCESS)
        return L7_FAILURE;
  
      ptr->index = p;
      ptr->state_aggregation = (state & DOT3AD_STATE_AGGREGATION)   ? L7_TRUE : L7_FALSE;
      ptr->lacp_activity     = (state & DOT3AD_STATE_LACP_ACTIVITY) ? L7_TRUE : L7_FALSE;
      ptr->lacp_timeout      = (state & DOT3AD_STATE_LACP_TIMEOUT)  ? L7_TRUE : L7_FALSE;

      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Index             = %u",ptr->index);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "State aggregation = %u",ptr->state_aggregation);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LACP Activity     = %u",ptr->lacp_activity);
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LACP Timeout      = %u",ptr->lacp_timeout);
    }
    *nStructs = L7_SYSTEM_N_PORTS;
  }
  else  {
    // get interface id
    if (ptin_translate_port2intf(port, &intf)!=L7_SUCCESS || intf==0)
      return L7_FAILURE;

    // Get Current Admin State
    if (usmDbDot3adAggPortActorAdminStateGet(1, intf, &state)!=L7_SUCCESS)
      return L7_FAILURE;

    adminState->index = port;
    adminState->state_aggregation = (state & DOT3AD_STATE_AGGREGATION)   ? L7_TRUE : L7_FALSE;
    adminState->lacp_activity     = (state & DOT3AD_STATE_LACP_ACTIVITY) ? L7_TRUE : L7_FALSE;
    adminState->lacp_timeout      = (state & DOT3AD_STATE_LACP_TIMEOUT)  ? L7_TRUE : L7_FALSE;

    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Index             = %u",adminState->index);
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "State aggregation = %u",adminState->state_aggregation);
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LACP Activity     = %u",adminState->lacp_activity);
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "LACP Timeout      = %u",adminState->lacp_timeout);

    *nStructs = 1;
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_LACPduStat_get( L7_int port, L7_LACPStats *stat, L7_uint8 *nStructs )
{
  L7_uint8 p;
  L7_uint32 rxStat, txStat, intf;

  // Validate pointer
  if ( stat==L7_NULLPTR || nStructs==L7_NULLPTR )
  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Null arguments",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Adapt for lag reference
  if ((port & 0xFF)!=0xFF && (port & 0x80) && (port & 0x7F)>0)  port = L7_SYSTEM_N_PORTS-1+(port & 0x7F);
  // Validate port index
  if ( port!=-1 && port>=L7_SYSTEM_N_PORTS )
  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid arguments",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  if (port==-1)
  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) All ports",__FUNCTION__,__LINE__);
    for (p=0; p<L7_SYSTEM_N_PORTS; p++)
    {
      if (ptin_translate_port2intf(p, &intf)!=L7_SUCCESS || intf==0)  {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error getting interface of port %u",__FUNCTION__,__LINE__,p);
        return L7_FAILURE;
      }
      if (usmDbDot3adAggPortStatsLACPDUsRxGet(1,intf, &rxStat)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error",__FUNCTION__,__LINE__);
        return L7_FAILURE;
      }
      if (usmDbDot3adAggPortStatsLACPDUsTxGet(1, intf, &txStat)!=L7_SUCCESS)  {
        DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error",__FUNCTION__,__LINE__);
        return L7_FAILURE;
      }

      stat[p].err_code = 0;
      stat[p].index = (p>=L7_SYSTEM_N_PORTS) ? ((L7_uint64) 0x80+p-L7_SYSTEM_N_PORTS+1) : (L7_uint64) p;
      stat[p].LACPdus_rx = rxStat;
      stat[p].LACPdus_tx = txStat;

      *nStructs = L7_SYSTEM_N_PORTS;
    }
  }
  else {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) Port %d",__FUNCTION__,__LINE__,port);

    if (ptin_translate_port2intf(port, &intf)!=L7_SUCCESS || intf==0)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error getting interface of port %u",__FUNCTION__,__LINE__,port);
      return L7_FAILURE;
    }
    if (usmDbDot3adAggPortStatsLACPDUsRxGet(1,intf, &rxStat)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
    if (usmDbDot3adAggPortStatsLACPDUsTxGet(1, intf, &txStat)!=L7_SUCCESS)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }

    stat->err_code = 0;
    stat->index = (port>=L7_SYSTEM_N_PORTS) ? ((L7_uint64) 0x80 | (port-L7_SYSTEM_N_PORTS+1)) : (L7_uint64) port;
    stat->LACPdus_rx = rxStat;
    stat->LACPdus_tx = txStat;

    *nStructs = 1;
  }

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) Success",__FUNCTION__,__LINE__);
  return L7_SUCCESS;
}

L7_RC_t ptin_msg_LACPduStat_clr( L7_LACPStats *stat )
{
  L7_uint8 port;
  L7_uint32 intf;

  // Validate pointer
  if (stat==L7_NULLPTR)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid pointer",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  port = (L7_uint8) (stat->index & 0xFF);
  if ((port & 0xFF)!=0xFF && (port & 0x80) && (port & 0x7F)>0)  port = L7_SYSTEM_N_PORTS-1+(port & 0x7F);

  // Validate port index
  if (port!=0xFF && port>=L7_SYSTEM_N_PORTS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid port (port=%u)",__FUNCTION__,__LINE__,port);
    return L7_FAILURE;
  }

  if (ptin_translate_port2intf(port, &intf)!=L7_SUCCESS || intf==0)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error getting interface of port %u",__FUNCTION__,__LINE__,port);
    return L7_FAILURE;
  }
  if (usmDbDot3adPortStatsClear(1,intf)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  return L7_SUCCESS;
}


L7_RC_t ptin_msg_snooping_querier_config(st_snooping *snoop)
{
  L7_RC_t rc = L7_SUCCESS;

  // Wait until snoop Queue is empty
  while (!snoopQueueEmpty())  usleep(10000);

  // Snooping global activation
  if (usmDbSnoopPrioModeSet(1, /*SNOOP_IGMP_DEFAULT_PRIO*/ snoop->igmp_packet_cos, L7_AF_INET)!=L7_SUCCESS)  {
    rc = L7_FAILURE;
  }

  // Snooping global activation
  if (usmDbSnoopAdminModeSet( 1, snoop->snooping_admin, 0, 0, L7_AF_INET)!=L7_SUCCESS)  {
    rc = L7_FAILURE;
  }

  // Querier admin
  if (usmDbSnoopQuerierAdminModeSet( snoop->querier_admin, L7_AF_INET)!=L7_SUCCESS)  {
    rc = L7_FAILURE;
  }

  if (snoop->querier_admin)
  {
    // Querier IP address
    if (usmDbSnoopQuerierAddressSet( (void *) &snoop->querier_ipaddress, L7_AF_INET)!=L7_SUCCESS)  {
      rc = L7_FAILURE;
    }
    // IGMP version
    if (usmDbSnoopQuerierVersionSet( snoop->igmp_version, L7_AF_INET)!=L7_SUCCESS)  {
      rc = L7_FAILURE;
    }
    // Querier interval
    if (usmDbSnoopQuerierQueryIntervalSet( snoop->querier_interval, L7_AF_INET)!=L7_SUCCESS)  {
      rc = L7_FAILURE;
    }
    // Querier Expiry interval
//  if (usmDbSnoopQuerierExpiryIntervalSet( snoop->querier_expiry_interval, L7_AF_INET)!=L7_SUCCESS)  {
//    rc = L7_FAILURE;
//  }
  }

  if (rc==L7_SUCCESS) {
    // Save general snooping configurations
    snooping_conf = *snoop;
    // Update all vlan queriers, with the new ipaddr
    rc = ptin_flow_update_snooping(&snooping_conf);
  }

  return rc;
}

L7_RC_t ptin_msg_snooping_intf_config_single(L7_uint8 port, enum_intf_type type, st_snooping_cfg *cfg)
{
  L7_uint32 intf;
  L7_RC_t rc = L7_SUCCESS;

  // Do not touch this interface
  if (type==PTIN_SNOOP_INTF_NONE)
    return L7_SUCCESS;

  // Wait until snoop Queue is empty
  while (!snoopQueueEmpty())  usleep(10000);

  // Adapt for lag reference
  if ((port & 0xFF)!=0xFF && (port & 0x80) && (port & 0x7F)>0)  port = L7_SYSTEM_N_PORTS-1+(port & 0x7F);
  // Calculate ionterface id
  if (ptin_translate_port2intf(port,&intf)!=L7_SUCCESS)  {
    rc = L7_FAILURE;
    return L7_FAILURE;
  }

  // If port is client...
  if (type==PTIN_SNOOP_INTF_CLIENT)
  {
    // Activate interface as client
    if (usmDbSnoopIntfModeSet( 1, intf, cfg->admin ,L7_AF_INET)!=L7_SUCCESS)  {
      rc = L7_FAILURE;
    }

    if (cfg->admin)
    {
      // Group-membership interval
      if (usmDbSnoopIntfGroupMembershipIntervalSet( 1, intf, cfg->group_membership_interval, L7_AF_INET )!=L7_SUCCESS)  {
        rc = L7_FAILURE;
      }
      // Response time
      if (usmDbSnoopIntfResponseTimeSet( 1, intf, cfg->max_response_time, L7_AF_INET )!=L7_SUCCESS)  {
        rc = L7_FAILURE;
      }
      // Multicast router Expiry time
      if (usmDbSnoopIntfMcastRtrExpiryTimeSet( 1, intf, cfg->mrouter_present_expiration_time, L7_AF_INET )!=L7_SUCCESS)  {
        rc = L7_FAILURE;
      }
      // Fast-leave mode
      if (usmDbSnoopIntfFastLeaveAdminModeSet( 1, intf, cfg->fastleave_admin_mode, L7_AF_INET )!=L7_SUCCESS)  {
        rc = L7_FAILURE;
      }
    }
  }
  // If port is router
  else if (type==PTIN_SNOOP_INTF_ROUTER)
  {
    // Activate interface as a router
    if (usmDbSnoopIntfMrouterSet( 1, intf, cfg->admin, L7_AF_INET)!=L7_SUCCESS)  {
      rc = L7_FAILURE;
    }
  }
  else if (type==PTIN_SNOOP_INTF_DISABLED)
  {
    // Deactivate interface as a router
    if (usmDbSnoopIntfMrouterSet(1,intf,L7_DISABLE,L7_AF_INET)!=L7_SUCCESS)  {
      rc = L7_FAILURE;
    }
    // Deactivate interface as client
    if (usmDbSnoopIntfModeSet(1,intf,L7_DISABLE,L7_AF_INET)!=L7_SUCCESS)  {
      rc = L7_FAILURE;
    }
  }

  return rc;
}

L7_RC_t ptin_msg_snooping_intf_config(st_snooping_intf *snoop)
{
  L7_uint8 port;
  L7_RC_t rc = L7_SUCCESS;

  for (port=0; port<L7_SYSTEM_N_INTERF; port++)
  {
    if (ptin_msg_snooping_intf_config_single(port, snoop->intf_type[port], &snoop->intf_config[port])!=L7_SUCCESS)
      rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t ptin_msg_snooping_vlan_config(st_snooping_vlan *snoop, L7_uint16 number_of_vlans)
{
  L7_uint16 i;
  L7_uint32 intfNum;
  st_snooping_vlan *ptr;
  st_snooping_cfg  *cfg;
  L7_RC_t rc = L7_SUCCESS;

  // Wait until snoop Queue is empty
  while (!snoopQueueEmpty())  usleep(10000);

  for (i=0; i<number_of_vlans; i++)
  {
    ptr = &snoop[i];
    cfg = &ptr->snoop_vlan_config;

    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) snoop_vlan=%u, snoop_vlan_xlate=%u, admin=%u, ptr->port.intf=%u ptr->port.type=%u",__FUNCTION__,__LINE__,ptr->snoop_vlan,ptr->snoop_vlan_xlate,cfg->admin,ptr->port.intf,ptr->port.type);

    // Validate vlans
    if (ptr->snoop_vlan<=L7_DOT1Q_MIN_VLAN_ID || ptr->snoop_vlan>L7_DOT1Q_MAX_VLAN_ID)
    {
      rc = L7_FAILURE;
      continue;
    }

    if (ptr->port.type == PTIN_SNOOP_INTF_CLIENT)
    {
      // Admin
      if (usmDbSnoopVlanModeSet( 1, ptr->snoop_vlan, cfg->admin, L7_AF_INET)!=L7_SUCCESS)  {
        rc = L7_FAILURE;
      }
  
      if (cfg->admin)
      {
        // Only for PON ports the snoop_vlan corresponds to the UNIVLAN,
        // and so the xlate array should only be updated for these ports
        if (ptr->port.intf<igmp_firstRootPort) {
          // Vlan translation from client to router ports
          if (usmDbSnoopQuerierVlanXlateSet( ptr->snoop_vlan, ptr->snoop_vlan_xlate)!=L7_SUCCESS)
            rc = L7_FAILURE;
        }
        // Fast-leave mode
        if (usmDbSnoopVlanFastLeaveModeSet( 1, ptr->snoop_vlan, cfg->fastleave_admin_mode, L7_AF_INET)!=L7_SUCCESS)
          rc = L7_FAILURE;
        // Group-membership interval
        if (usmDbSnoopVlanGroupMembershipIntervalSet(1, ptr->snoop_vlan, cfg->group_membership_interval, L7_AF_INET)!=L7_SUCCESS)
          rc = L7_FAILURE;
        // Response time
        if (usmDbSnoopVlanMaximumResponseTimeSet(1, ptr->snoop_vlan, cfg->max_response_time, L7_AF_INET)!=L7_SUCCESS)
          rc = L7_FAILURE;
        // Multicast router Expiry time
        if (usmDbSnoopVlanMcastRtrExpiryTimeSet(1, ptr->snoop_vlan, cfg->mrouter_present_expiration_time, L7_AF_INET)!=L7_SUCCESS)
          rc = L7_FAILURE;
      }
      else
      {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) usmDbSnoopQuerierVlanXlateSet was supposed to be reset here!",__FUNCTION__,__LINE__);
        // No translation from client to router ports
//      if (usmDbSnoopQuerierVlanXlateSet( ptr->snoop_vlan, ptr->snoop_vlan)!=L7_SUCCESS)  {
//        rc = L7_FAILURE;
//      }
      }
    }
    else if (ptr->port.type == PTIN_SNOOP_INTF_ROUTER)  {
      // Calculate ionterface id
      if (snoop->port.intf<L7_SYSTEM_N_INTERF && ptin_translate_port2intf(ptr->port.intf,&intfNum)==L7_SUCCESS)
      {
        // Router vlan
        if (usmDbsnoopIntfApiVlanStaticMcastRtrSet( 1, intfNum, ptr->snoop_vlan, cfg->admin, L7_AF_INET)!=L7_SUCCESS)  {
          rc = L7_FAILURE;
        }
      }
      else  {
        rc = L7_FAILURE;
      }
    }
  }

  return rc;
}

L7_RC_t ptin_msg_snooping_querier_vlan_config(st_snooping_querier_vlan *snoop, L7_uint16 number_of_vlans)
{
  L7_uint16 i;
  st_snooping_querier_vlan *ptr;
  L7_RC_t rc = L7_SUCCESS;

  // Wait until snoop Queue is empty
  while (!snoopQueueEmpty())  usleep(10000);

  for (i=0; i<number_of_vlans; i++)
  {
    ptr = &snoop[i];

    // Update global settings
    ptr->querier_ipaddress = snooping_conf.querier_ipaddress;

    // Validate vlans
    if (ptr->querier_vlan<=L7_DOT1Q_MIN_VLAN_ID || ptr->querier_vlan>L7_DOT1Q_MAX_VLAN_ID)
    {
      rc = L7_FAILURE;
      continue;
    }
    // Admin of this vlans
    if (usmDbSnoopQuerierVlanModeSet( ptr->querier_vlan, ptr->querier_vlan_admin, L7_AF_INET)!=L7_SUCCESS)
      rc = L7_FAILURE;

    if (ptr->querier_vlan_admin)
    {
      // IP address related to this vlan
      if (usmDbSnoopQuerierVlanAddressSet( ptr->querier_vlan, (void *) &ptr->querier_ipaddress, L7_AF_INET)!=L7_SUCCESS)
        rc = L7_FAILURE;
      // Election participate mode
      if (usmDbSnoopQuerierVlanElectionModeSet( ptr->querier_vlan, ptr->querier_election_participate_mode, L7_AF_INET)!=L7_SUCCESS)
        rc = L7_FAILURE;
    }
  }

  // Wait until snoop Queue is empty
  while (!snoopQueueEmpty())  usleep(10000);

  return rc;
}


L7_RC_t ptin_msg_snooping_querier_vlan_getconfig(st_snooping_querier_vlan *snoop, L7_uint16 number_of_vlans)
{
  L7_uint16 i;
  st_snooping_querier_vlan *ptr;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32 mode;

  // Wait until snoop Queue is empty
  while (!snoopQueueEmpty())  usleep(10000);

  for (i=0; i<number_of_vlans; i++)
  {
    ptr = &snoop[i];

    // Validate vlans
    if (ptr->querier_vlan<=L7_DOT1Q_MIN_VLAN_ID || ptr->querier_vlan>L7_DOT1Q_MAX_VLAN_ID)
    {
      rc = L7_FAILURE;
      continue;
    }
    // Admin of this vlans
    if (usmDbSnoopQuerierVlanModeGet( ptr->querier_vlan, &mode, L7_AF_INET)!=L7_SUCCESS)
      rc = L7_FAILURE;
    ptr->querier_vlan_admin = mode;

    if (ptr->querier_vlan_admin)
    {
      // IP address related to this vlan
      if (usmDbSnoopQuerierVlanAddressGet( ptr->querier_vlan, (void *) &ptr->querier_ipaddress, L7_AF_INET)!=L7_SUCCESS)
        rc = L7_FAILURE;
      // Election participate mode
      if (usmDbSnoopQuerierVlanElectionModeGet( ptr->querier_vlan, &mode, L7_AF_INET)!=L7_SUCCESS)
        rc = L7_FAILURE;
      ptr->querier_election_participate_mode = mode;
    }
  }

  return rc;
}

#ifdef IGMP_WHITELIST_FEATURE
/**
 * Add channels to White list
 * 
 * @param channel_list : Channel list array
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_WList_add(msg_MCChannelWList_t *channel_list, L7_uint16 n_channels)
{
  L7_uint16 i;
  L7_inet_addr_t groupAddr;
  L7_RC_t rc = L7_SUCCESS;

  if (channel_list==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  for (i=0; i<n_channels; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Adding channel index %u:",i);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Entry_idx = %llu", channel_list[i].entry_idx);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," EVC_idx   = %d", channel_list[i].evc_idx);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," DstIP_Channel = 0x%08x / %u",channel_list[i].grpAddr, channel_list[i].grpAddrmask);

    /* Prepare group address */
    memset(&groupAddr, 0x00, sizeof(L7_inet_addr_t));
    groupAddr.family = L7_AF_INET;
    groupAddr.addr.ipv4.s_addr = channel_list[i].grpAddr;

    if (igmp_wlist_channel_add(channel_list[i].evc_idx, &groupAddr, channel_list[i].grpAddrmask) != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error adding group address 0x%08x/%u (evc %u)",
              channel_list[i].grpAddr, channel_list[i].grpAddrmask, channel_list[i].evc_idx);
      return L7_FAILURE;
    }
  }

  return rc;
}

/**
 * Remove channels from white list
 * 
 * @param channel_list : Channel list array
 * @param n_channels : Number of channels returned
 * 
 * @return L7_RC_t : L7_SUCCESS / L7_FAILURE
 */
L7_RC_t ptin_msg_IGMP_WList_remove(msg_MCChannelWList_t *channel_list, L7_uint16 n_channels)
{
  L7_uint16 i;
  L7_inet_addr_t groupAddr;
  L7_RC_t rc = L7_SUCCESS;

  if (channel_list==L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_MSG, "Invalid arguments");
    return L7_FAILURE;
  }

  for (i=0; i<n_channels; i++)
  {
    LOG_DEBUG(LOG_CTX_PTIN_MSG,"Removing channel index %u:",i);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," Entry_idx = %d",channel_list[i].entry_idx);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," EVC_idx   = %d", channel_list[i].evc_idx);
    LOG_DEBUG(LOG_CTX_PTIN_MSG," DstIP_Channel = 0x%08x / %u",channel_list[i].grpAddr, channel_list[i].grpAddrmask);

    /* Prepare group address */
    memset(&groupAddr, 0x00, sizeof(L7_inet_addr_t));
    groupAddr.family = L7_AF_INET;
    groupAddr.addr.ipv4.s_addr = channel_list[i].grpAddr;

    if (igmp_wlist_channel_remove(channel_list[i].evc_idx, &groupAddr, channel_list[i].grpAddrmask ) != L7_SUCCESS )
    {
      LOG_ERR(LOG_CTX_PTIN_MSG, "Error removing group address 0x%08x/%u (evc %u)",
              channel_list[i].grpAddr, channel_list[i].grpAddrmask, channel_list[i].evc_idx);
      return L7_FAILURE;
    }
  }

  return rc;
}
#endif

L7_RC_t ptin_msg_MCStaticChannel_add(L7_MCStaticChannel *channel, L7_uint8 nStructs)
{
  L7_BOOL   in_use;
  L7_uint8  i, port, evc_type;
  L7_uint16 vlanId, evcId;
  L7_uint32 channelIP;

  // Validate arguments
  if (channel==L7_NULLPTR || nStructs==0)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Null Argument",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  for (i=0; i<nStructs; i++)
  {
    // Validate flow id
    if (channel[i].index==0 || channel[i].index>=FLOWS_MAX)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid EVCid",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
  
    // Get flow id and channel IP
    evcId = (L7_uint16) (channel[i].index & 0xFFFF);
    channelIP = channel[i].channelIp;
  
    // Check if flow exists
    if (ptin_flow_inUse(evcId, &in_use)!=L7_SUCCESS || !in_use)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC does not exist (evc=%u)",__FUNCTION__,__LINE__,evcId);
      return L7_FAILURE;
    }
  
    // Confirm that is a MC EVC
    if (ptin_flow_getType(evcId, &evc_type)!=L7_SUCCESS || evc_type!=FLOW_TYPE_MULTICAST)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Not a multicast EVC (type=%u)",__FUNCTION__,__LINE__,evc_type);
      return L7_FAILURE;
    }
  
    // Extract vlan correspondent to this flow. The vlan is the same for all ports in MC flows.
    vlanId = 0;
    for (port=0; port<igmp_firstRootPort; port++)
    {
      if (ptin_flow_get_originalVlan(port, evcId, &vlanId)==L7_SUCCESS && vlanId>=2 && vlanId<=4093)
        break;
    }
    // No Vlan found
    if (vlanId==0)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) No Vlan found",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
    // At this point, vlan was found...

    // Add static IP
    if (snoopStaticIpAdd(vlanId, channelIP)!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_MCStaticChannel_del(L7_MCStaticChannel *channel, L7_uint8 nStructs)
{
  L7_BOOL   in_use;
  L7_uint8  i, port, evc_type;
  L7_uint16 vlanId, evcId;
  L7_uint32 channelIP;

  // Validate arguments
  if (channel==L7_NULLPTR || nStructs==0)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Null Argument",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  for (i=0; i<nStructs; i++)
  {
    // Validate flow id
    if (channel[i].index==0 || channel[i].index>=FLOWS_MAX)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid EVCid",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
  
    // Get flow id and channel IP
    evcId = (L7_uint16) (channel[i].index & 0xFFFF);
    channelIP = channel[i].channelIp;

    // Check if flow exists
    if (ptin_flow_inUse(evcId, &in_use)!=L7_SUCCESS || !in_use)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC does not exist (evc=%u)",__FUNCTION__,__LINE__,evcId);
      return L7_FAILURE;
    }
  
    // Confirm that is a MC EVC
    if (ptin_flow_getType(evcId, &evc_type)!=L7_SUCCESS || evc_type!=FLOW_TYPE_MULTICAST)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Not a multicast EVC (type=%u)",__FUNCTION__,__LINE__,evc_type);
      return L7_FAILURE;
    }
  
    // Extract vlan correspondent to this flow. The vlan is the same for all ports in MC flows.
    for (port=0; port<igmp_firstRootPort; port++)
    {
      if (ptin_flow_get_originalVlan(port, evcId, &vlanId)==L7_SUCCESS && vlanId>=2 && vlanId<=4093)  break;
    }
    // No Vlan found
    if (port>=L7_SYSTEM_N_INTERF)  {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) No Vlan found",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
    // At this point, vlan was found...

    // Remove static IP
    if (snoopStaticIpRemove(vlanId, channelIP)!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error",__FUNCTION__,__LINE__);  
      return L7_FAILURE;
    }
  }

  return L7_SUCCESS;
}


L7_RC_t ptin_msg_MCActiveChannelsList_get(L7_MCActiveChannels *channels)
{
  L7_BOOL   in_use;
  L7_uint8  port, evc_type;
  L7_uint16 i, vlanId, client_vlanId, evcId;
  L7_uint16 page, pages_max, start, entries;
  L7_inet_addr_t *active_channels_ip;
  L7_uint16 number_of_channels;

  // Validate arguments
  if (channels==L7_NULLPTR)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Null Argument",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Get flow id and client vlanId
  evcId = (L7_uint16) (channels->index & 0xFFFF);
  client_vlanId = (L7_uint16) ((channels->index>>16) & 0xFFFF);
  if (client_vlanId==0 || client_vlanId>=4096)  client_vlanId = (L7_uint16)-1;

  // Validate flow id
  if (evcId==0 || evcId>=FLOWS_MAX)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid EVCid",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Check if flow exists
  if (ptin_flow_inUse(evcId, &in_use)!=L7_SUCCESS || !in_use)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC does not exist (evc=%u)",__FUNCTION__,__LINE__,evcId);
    return L7_FAILURE;
  }

  // Confirm that is a MC EVC
  if (ptin_flow_getType(evcId, &evc_type)!=L7_SUCCESS || evc_type!=FLOW_TYPE_MULTICAST)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Not a multicast EVC (type=%u)",__FUNCTION__,__LINE__,evc_type);
    return L7_FAILURE;
  }

  // Extract vlan correspondent to this flow. The vlan is the same for all ports in MC flows.
  for (port=0; port<igmp_firstRootPort; port++)
  {
    if (ptin_flow_get_originalVlan(port, evcId, &vlanId)==L7_SUCCESS && vlanId>=2 && vlanId<=4093)  break;
  }
  // No Vlan found
  if (port>=igmp_firstRootPort)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) No Vlan found",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) Vlan found: %u",__FUNCTION__,__LINE__,vlanId);
  // At this point, vlan was found...

  // Extract list of channels
  if (ptin_snooping_activeChannels_get(vlanId, client_vlanId, &active_channels_ip, &number_of_channels)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error getting list of channels",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Calculate max number of pages (messages necessary to transport all channels)
  pages_max = number_of_channels/1024;
  if (number_of_channels==0 || (number_of_channels%1024)!=0)  pages_max++;

  // Extract and correct page index, from source message
  page = channels->page_index;
  if (page>=pages_max)  page=0;

  // Determine first index to read, and number of entries
  start   = page*1024;
  entries = ((number_of_channels-start)<=1024) ? number_of_channels : 1024;

  // Fullfil message structure
  for (i=0; i<entries; i++)
  {
    channels->channelsIp_list[i] = active_channels_ip[start+i].addr.ipv4.s_addr;
  }
  channels->n_channels_total = number_of_channels;
  channels->n_channels_msg   = entries;
  channels->n_pages_total    = pages_max;
  channels->page_index       = page;

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) Success",__FUNCTION__,__LINE__);

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_MCChannelClientsList_get(L7_MCActiveChannelClients *clients)
{
  L7_BOOL   in_use;
  L7_uint8  port, evc_type;
  L7_uint16 vlanId, evcId;

  // Validate arguments
  if (clients==L7_NULLPTR)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Null Argument",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Validate flow id
  if (clients->index==0 || clients->index>=FLOWS_MAX || clients->channelIp==0)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid Arguments",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Get flow id
  evcId = (L7_uint16) (clients->index & 0xFFFF);

  // Check if flow exists
  if (ptin_flow_inUse(evcId, &in_use)!=L7_SUCCESS || !in_use)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC does not exist (evc=%u)",__FUNCTION__,__LINE__,evcId);
    return L7_FAILURE;
  }

  // Confirm that is a MC EVC
  if (ptin_flow_getType(evcId, &evc_type)!=L7_SUCCESS || evc_type!=FLOW_TYPE_MULTICAST)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Not a MC EVC (type=%u)",__FUNCTION__,__LINE__,evc_type);
    return L7_FAILURE;
  }

  // Extract vlan correspondent to this flow. The vlan is the same for all ports in MC flows.
  for (port=0; port<igmp_firstRootPort; port++)
  {
    if (ptin_flow_get_originalVlan(port, evcId, &vlanId)==L7_SUCCESS && vlanId>=2 && vlanId<=4093)  break;
  }
  // No Vlan found
  if (port>=L7_SYSTEM_N_INTERF)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) No Vlan found",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }
  // At this point, vlan was found...
  
  // Get list and number of clients
  if (ptin_snooping_clientsList_get(clients->channelIp, vlanId, clients->clients_list_bmp, &clients->n_clients)!=L7_SUCCESS)  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error getting list of clients",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) Success",__FUNCTION__,__LINE__);

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_IgmpFlowAdd(L7_ClientIgmp *flow, L7_uint16 number_of_flows)
{
  L7_uint16 evcId;
  L7_uint16 i;
  L7_RC_t rc = L7_SUCCESS;

  if (flow==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  for (i=0; i<number_of_flows; i++)
  {
    evcId = (L7_uint16) (flow[i].index & 0xFFFF);

    if (ptin_igmp_flow_add(evcId)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t ptin_msg_IgmpFlowRemove(L7_ClientIgmp *flow, L7_uint16 number_of_flows)
{
  L7_uint16 evcId;
  L7_uint16 i;
  L7_RC_t rc = L7_SUCCESS;

  if (flow==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  for (i=0; i<number_of_flows; i++)
  {
    evcId = (L7_uint16) (flow[i].index & 0xFFFF);

    if (ptin_igmp_flow_remove(evcId)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t ptin_msg_IgmpClientAdd(L7_ClientIgmp *client, L7_uint16 number_of_clients)
{
  L7_uint16 evcId;
  L7_uint16 cvid;
  L7_uint16 i;
  L7_RC_t rc = L7_SUCCESS;

  if (client==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  for (i=0; i<number_of_clients; i++)
  {
    evcId = (L7_uint16) (client[i].index & 0xFFFF);
    cvid  = client[i].cvid;

    if (ptin_igmp_evcClient_add(evcId,cvid)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t ptin_msg_IgmpClientRemove(L7_ClientIgmp *client, L7_uint16 number_of_clients)
{
  L7_uint16 evcId;
  L7_uint16 cvid;
  L7_uint16 i;
  L7_RC_t rc = L7_SUCCESS;

  if (client==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  for (i=0; i<number_of_clients; i++)
  {
    evcId = (L7_uint16) (client->index & 0xFFFF);
    cvid  = client->cvid;

    // Specific client remotion
    if (ptin_igmp_evcClient_remove(evcId,cvid)!=L7_SUCCESS)
      rc = L7_FAILURE;
  }

  return rc;
}

L7_RC_t ptin_msg_IgmpClientStatsGet(L7_ClientIgmpStatistics *stats)
{
  L7_uint16 evcId;
  L7_uint16 cvid;
  L7_IGMP_Statistics_t *ptr;

  if (stats==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  evcId = (L7_uint16) (stats->index & 0xFFFF);
  cvid  = stats->cvid;
  ptr   = &stats->stats;

  return ptin_igmp_stat_get(evcId,cvid,L7_NULL,ptr,L7_NULLPTR,L7_NULLPTR);
}

L7_RC_t ptin_msg_IgmpClientStatsClear(L7_ClientIgmpStatistics *stats)
{
  L7_uint16 evcId;
  L7_uint16 cvid;

  if (stats==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  evcId = (L7_uint16) (stats->index & 0xFFFF);
  cvid  = stats->cvid;

  return ptin_igmp_stat_client_clear(evcId,cvid);
}

L7_RC_t ptin_msg_IgmpPortStatsGet(L7_ClientIgmpStatistics *stats)
{
  L7_uint16 evcId;
  L7_uint16 port;
  L7_IGMP_Statistics_t *ptr;
  L7_RC_t rc = L7_SUCCESS;

  if (stats==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  evcId = (L7_uint16) (stats->index & 0xFFFF);
  port  = stats->port;
  if ((port & 0xFF)!=0xFF && (port & 0x80) && (port & 0x7F)>0)  port = L7_SYSTEM_N_PORTS-1+(port & 0x7F);

  ptr   = &stats->stats;

  // Read general port statistics
  if (evcId==0)
    rc = ptin_igmp_stat_get(evcId,L7_NULL,port,L7_NULLPTR,L7_NULLPTR,ptr);
  // Read evc port statistics
  else
    rc = ptin_igmp_stat_get(evcId,L7_NULL,port,L7_NULLPTR,ptr,L7_NULLPTR);

  return rc;
}

L7_RC_t ptin_msg_IgmpPortStatsClear(L7_ClientIgmpStatistics *stats)
{
  L7_uint16 evcId;
  L7_uint16 port;

  evcId = (L7_uint16) (stats->index & 0xFFFF);
  port  = stats->port;
  if ((port & 0xFF)!=0xFF && (port & 0x80) && (port & 0x7F)>0)  port = L7_SYSTEM_N_PORTS-1+(port & 0x7F);

  return ptin_igmp_stat_port_clear(evcId,port);
}

L7_RC_t ptin_msg_DhcpClientStatsGet(L7_ClientDhcpStatistics *stats)
{
  L7_uint16 evcId;
  L7_uint16 cvid;
  L7_DHCP_Statistics_t *ptr;

  if (stats==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  evcId = (L7_uint16) (stats->index & 0xFFFF);
  cvid  = stats->cvid;
  ptr   = &stats->stats;

  return ptin_dhcp_stat_get(evcId,cvid,L7_NULL,ptr,L7_NULLPTR,L7_NULLPTR);
}

L7_RC_t ptin_msg_DhcpClientStatsClear(L7_ClientDhcpStatistics *stats)
{
  L7_uint16 evcId;
  L7_uint16 cvid;

  if (stats==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  evcId = (L7_uint16) (stats->index & 0xFFFF);
  cvid  = stats->cvid;

  return ptin_dhcp_stat_client_clear(evcId,cvid);
}

L7_RC_t ptin_msg_DhcpPortStatsGet(L7_ClientDhcpStatistics *stats)
{
  L7_uint16 evcId;
  L7_uint16 port;
  L7_DHCP_Statistics_t *ptr;
  L7_RC_t rc = L7_SUCCESS;

  if (stats==L7_NULLPTR)
  {
    return L7_FAILURE;
  }

  evcId = (L7_uint16) (stats->index & 0xFFFF);
  port  = stats->port;
  if ((port & 0xFF)!=0xFF && (port & 0x80) && (port & 0x7F)>0)  port = L7_SYSTEM_N_PORTS-1+(port & 0x7F);

  ptr   = &stats->stats;

  // Read general port statistics
  if (evcId==0)
    rc = ptin_dhcp_stat_get(evcId,L7_NULL,port,L7_NULLPTR,L7_NULLPTR,ptr);
  // Read evc port statistics
  else
    rc = ptin_dhcp_stat_get(evcId,L7_NULL,port,L7_NULLPTR,ptr,L7_NULLPTR);

  return rc;
}

L7_RC_t ptin_msg_DhcpPortStatsClear(L7_ClientDhcpStatistics *stats)
{
  L7_uint16 evcId;
  L7_uint16 port;

  evcId = (L7_uint16) (stats->index & 0xFFFF);
  port  = stats->port;
  if ((port & 0xFF)!=0xFF && (port & 0x80) && (port & 0x7F)>0)  port = L7_SYSTEM_N_PORTS-1+(port & 0x7F);

  return ptin_dhcp_stat_port_clear(evcId,port);
}


L7_RC_t ptin_msg_EVCread(L7_HwEthernetMef10EvcBundling *evcCfg)
{
  if (evcCfg==L7_NULLPTR)  return L7_FAILURE;

  // Create EVC
  return ptin_flow_read(evcCfg);
}


L7_RC_t ptin_msg_EVCcreate(L7_HwEthernetMef10EvcBundling *evcCfg)
{
  // Do not allow EVC 0 to be used from outside
  if ((evcCfg->index & 0xFFFF)==0)
  {
    DEBUGTRACE (TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) EVC 0 index not allowed to be used",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Create EVC
  return ptin_flow_create(evcCfg);
}

L7_RC_t ptin_msg_EVCdestroy(L7_HwEthernetMef10p1EvcRemove *evcCfg)
{
  L7_uint16 evcId;

  if (evcCfg==L7_NULLPTR)  return L7_FAILURE;

  // Extract EVC ID
  evcId = evcCfg->index & 0xFFFF;

  // Create EVC
  return ptin_flow_kill(evcId);
}


L7_RC_t ptin_msg_GetProfile(L7_HwEthernetProfile *dapiCmd, L7_uint16 *nProfiles)
{
  L7_bw_profile profile[SNOOP_MAX_NUMBER_OF_CLIENTS];
  L7_uint16 flow_id, n, i;
  L7_uint8  port;
  L7_RC_t rc;

  // EVC id
  flow_id = (L7_uint16) ((dapiCmd->index>>48) & 0xFFFF);
  // Port bitmap
  profile[0].port_bmp = 0;
  // Client tag
  profile[0].ctag.vid_value = (L7_uint16) ((dapiCmd->index>>32) & 0xFFFF);
  profile[0].ctag.vid_mask  = 0xFFF;
  // Service tag
  profile[0].stag.vid_value = (L7_uint16) ((dapiCmd->index>>16) & 0xFFFF);
  profile[0].stag.vid_mask  = 0xFFF;
  // Port id
  port = (L7_uint8) (dapiCmd->index & 0xFF);
  if ((port & 0xFF)!=0xFF && (port & 0x80) && (port & 0x7F)>0)  port = L7_SYSTEM_N_PORTS-1+(port & 0x7F);

  if ((rc=ptin_flow_get_profile(flow_id, port, profile, &n))!=L7_SUCCESS)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: failure in ptin_flow_get_profile\n",__FUNCTION__);
    return rc;
  }

  for (i=0; i<n; i++)
  {
    // Clear all element
    memset(&dapiCmd[i],0x00,sizeof(L7_HwEthernetProfile));
    // Index
    dapiCmd[i].index = 0;
    dapiCmd[i].index |= (port>=L7_SYSTEM_N_PORTS) ? ((L7_uint64) 0x80 | (port-L7_SYSTEM_N_PORTS+1)) : ((L7_uint64) port);  // Port
    dapiCmd[i].index |= ((L7_uint64) profile[i].stag.vid_value & 0xFFFF)<<14;     // SVid
    dapiCmd[i].index |= ((L7_uint64) profile[i].stag.vid_value & 0xFFFF)<<16;     // SVid
    dapiCmd[i].index |= ((L7_uint64) profile[i].ctag.vid_value & 0xFFFF)<<32;     // CVid
    dapiCmd[i].index |= ((L7_uint64) flow_id                   & 0xFFFF)<<48;     // Flow id

    // Profile parameters
    if (profile[i].cir==(L7_uint32)-1)
    {
      dapiCmd[i].CIR = (L7_uint64) -1;
      dapiCmd[i].EIR = (L7_uint64) -1;
      dapiCmd[i].CBS = (L7_uint64) -1;
      dapiCmd[i].EBS = (L7_uint64) -1;
    }
    else
    {
      dapiCmd[i].CIR = (L7_uint64) profile[i].cir*1000;   // in bps
      dapiCmd[i].EIR = (L7_uint64) profile[i].eir*1000;   // in bps
      dapiCmd[i].CBS = (L7_uint64) profile[i].cbs;           // in bytes
      dapiCmd[i].EBS = (L7_uint64) profile[i].ebs;           // in bytes
    }
  }
  if (nProfiles!=L7_NULLPTR)  *nProfiles = n;

  return L7_SUCCESS;
}


L7_RC_t ptin_msg_SetProfile(L7_HwEthernetProfile *dapiCmd)
{
  L7_bw_profile  profile;
  L7_uint16 flow_id;
  L7_uint8  port;
  L7_RC_t rc;

  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CIR=%llu",dapiCmd->CIR);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EIR=%llu",dapiCmd->EIR);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CBS=%llu",dapiCmd->CBS);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EBS=%llu",dapiCmd->EBS);

  // Profile parameters
  if ( ( dapiCmd->CIR ) >= ( (L7_uint64) pow(2,32) * 1000 ) )
  {
    DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Estou aqui=%llu",( (L7_uint64) pow(2,32) * 1000 ));
    profile.cir = (L7_uint32) -1;
    profile.eir = (L7_uint32) -1;
    profile.cbs = (L7_uint32) -1;
    profile.ebs = (L7_uint32) -1;
  }
  else
  {
    profile.cir = (L7_uint32) (dapiCmd->CIR/1000);    // in kbps
    profile.eir = (L7_uint32) (dapiCmd->EIR/1000);    // in kbps
    profile.cbs = (L7_uint32) dapiCmd->CBS;         // in bytes
    profile.ebs = (L7_uint32) dapiCmd->EBS;         // in bytes
  }
  // EVC id
  flow_id = (L7_uint16) ((dapiCmd->index>>48) & 0xFFFF);
  // Client tag
  profile.ctag.vid_value = (L7_uint16) ((dapiCmd->index>>32) & 0xFFFF);
  profile.ctag.vid_mask  = 0xFFF;
  // Service tag
  profile.stag.vid_value = (L7_uint16) ((dapiCmd->index>>16) & 0xFFFF);
  profile.stag.vid_mask  = 0xFFF;
  // Destination port (used for bitsrtream flows)
  profile.port_output    = (L7_uint8)  ((dapiCmd->index>>8) & 0xFF);
  // Port id
  port = (L7_uint8) (dapiCmd->index & 0xFF);
  if ((port & 0xFF)!=0xFF && (port & 0x80) && (port & 0x7F)>0)  port = L7_SYSTEM_N_PORTS-1+(port & 0x7F);
  if ( port>=L7_SYSTEM_N_INTERF )  port=(L7_uint8)-1;

  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "flow_id=%u",flow_id);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "srcPort=%u",port);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "dstPort=%u",profile.port_output);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "svid_value=%u",profile.stag.vid_value);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "cvid_value=%u",profile.ctag.vid_value);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CIR=%lu",profile.cir);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EIR=%lu",profile.eir);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "CBS=%lu",profile.cbs);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "EBS=%lu",profile.ebs);

  if ((rc=ptin_flow_set_profile(flow_id, port, &profile))!=L7_SUCCESS)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: failure in ptin_flow_set_profile\n",__FUNCTION__);
    return rc;
  }
  return L7_SUCCESS;
}

L7_RC_t ptin_msg_RemoveProfile(L7_HwEthernetProfile *dapiCmd)
{
  L7_bw_profile  profile;
  L7_uint16 flow_id;
  L7_uint8  port;
  L7_RC_t rc;

  // Profile parameters
  profile.cir = 0;
  profile.eir = 0;
  profile.cbs = 0;
  profile.ebs = 0;
  // EVC id
  flow_id = (L7_uint16) ((dapiCmd->index>>48) & 0xFFFF);
  // Client tag
  profile.ctag.vid_value = (L7_uint16) ((dapiCmd->index>>32) & 0xFFFF);
  profile.ctag.vid_mask  = 0xFFF;
  // Service tag
  profile.stag.vid_value = (L7_uint16) ((dapiCmd->index>>16) & 0xFFFF);
  profile.stag.vid_mask  = 0xFFF;
  // Port id
  port = (L7_uint8) (dapiCmd->index & 0xFF);
  if ((port & 0xFF)!=0xFF && (port & 0x80) && (port & 0x7F)>0)  port = L7_SYSTEM_N_PORTS-1+(port & 0x7F);
  if ( port>=L7_SYSTEM_N_INTERF )  port=(L7_uint8)-1;

  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "flow_id=%u",flow_id);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "port=%u",port);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "svid_value=%u",profile.stag.vid_value);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "cvid_value=%u",profile.ctag.vid_value);

  if ((rc=ptin_flow_remove_profile(flow_id, port, &profile))!=L7_SUCCESS)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: failure in ptin_flow_remove_profile\n",__FUNCTION__);
    return rc;
  }
  return L7_SUCCESS;
}


L7_RC_t ptin_msg_setPhyConfig(L7_HWEthPhyConf *dapiCmd)
{
  L7_uint16 tmp;
  L7_uint8  port = dapiCmd->Port;

  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Port=%u", port);
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Enable   = %u",dapiCmd->PortEnable );
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Speed    = %u",dapiCmd->Speed );
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "AutoNeg  = %u",0 );
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Duplex   = %u",dapiCmd->Duplex );
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Media    = %u",dapiCmd->Media );
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "MaxFrame = %u",dapiCmd->MaxFrame );
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Loopback = %u",dapiCmd->LoopBack );
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Learning = %u",dapiCmd->MacLearning );
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "AutoMDI  = %u",dapiCmd->AutoMDI );
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "FlowCtrl = %u",dapiCmd->FlowControl );
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "VlanAware= %u",dapiCmd->VlanAwareness );
  DEBUGTRACE(TRACE_MODULE_SYSTEM | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "Mask = 0x%04X",dapiCmd->Mask );

  if ( ptin_phy_config_set( port,
                            ((dapiCmd->Mask & 0x0020) ? dapiCmd->PortEnable : ((L7_uint8) -1)),
                            ((dapiCmd->Mask & 0x0001) ? dapiCmd->Speed : ((L7_uint8) -1)),
                            ((dapiCmd->Mask & 0x0001) ? 0 : ((L7_uint8) -1)),
                            ((dapiCmd->Mask & 0x0004) ? dapiCmd->Duplex : ((L7_uint8) -1)),
                            ((dapiCmd->Mask & 0x0040) ? dapiCmd->MaxFrame : ((L7_uint16) -1)),
                            ((dapiCmd->Mask & 0x0008) ? dapiCmd->LoopBack : ((L7_uint8) -1)),
                            ((dapiCmd->Mask & 0x0100) ? dapiCmd->MacLearning : ((L7_uint8) -1)) ) )
  {
    tmp = dapiCmd->Mask;  // Save mask
    memset(dapiCmd,0x00,sizeof(L7_HWEthPhyConf));
    dapiCmd->Mask = tmp;  // Restore mask
    dapiCmd->Port = port;
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error applying configurations\n", __FUNCTION__);
    return L7_FAILURE;
  } else
  {
    // Media
    if ( dapiCmd->Mask & 0x0002 )
    {
      if ( port<8 )   dapiCmd->Media = PHY_PORT_MEDIA_INTERNAL;
      else            dapiCmd->Media = PHY_PORT_MEDIA_OPTICAL;
    }
  }

//// Copy configurations to database
  //osapiSemaTake(sem_monitor_access,L7_WAIT_FOREVER);
  ptin_monitor[port].PhyConf.Port = port;
  ptin_monitor[port].PhyConf.Mask = 0xFFFF;
  if (dapiCmd->Mask & 0x0001)     ptin_monitor[port].PhyConf.Speed         = dapiCmd->Speed;
  if (dapiCmd->Mask & 0x0002)     ptin_monitor[port].PhyConf.Media         = dapiCmd->Media;
  if (dapiCmd->Mask & 0x0004)     ptin_monitor[port].PhyConf.Duplex        = dapiCmd->Duplex;
  if (dapiCmd->Mask & 0x0008)     ptin_monitor[port].PhyConf.LoopBack      = dapiCmd->LoopBack;
  if (dapiCmd->Mask & 0x0010)     ptin_monitor[port].PhyConf.FlowControl   = dapiCmd->FlowControl;
  if (dapiCmd->Mask & 0x0020)     ptin_monitor[port].PhyConf.PortEnable    = dapiCmd->PortEnable;
  if (dapiCmd->Mask & 0x0040)     ptin_monitor[port].PhyConf.MaxFrame      = dapiCmd->MaxFrame;
  if (dapiCmd->Mask & 0x0080)     ptin_monitor[port].PhyConf.VlanAwareness = dapiCmd->VlanAwareness;
  if (dapiCmd->Mask & 0x0100)     ptin_monitor[port].PhyConf.MacLearning   = dapiCmd->MacLearning;
  if (dapiCmd->Mask & 0x0200)     ptin_monitor[port].PhyConf.AutoMDI       = dapiCmd->AutoMDI;
  //osapiSemaGive(sem_monitor_access);

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_getPhyConfig(L7_HWEthPhyConf *phy_conf)
{
  L7_uint16 mask;
  L7_uint8 port;

  port = phy_conf->Port;

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: going to read configurations of port %u\n", __FUNCTION__,port);

  // Copy applied configurations
  mask = phy_conf->Mask;
  //osapiSemaTake(sem_monitor_access,L7_WAIT_FOREVER);
  memcpy(phy_conf,&ptin_monitor[port].PhyConf,sizeof(L7_HWEthPhyConf));
  //osapiSemaGive(sem_monitor_access);
  phy_conf->Port = port;
  phy_conf->Mask = mask;

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s: configurations read from port %u\n", __FUNCTION__,port);
  return L7_SUCCESS;
}


L7_RC_t ptin_msg_getPhyState(L7_HWEthPhyState *phy_state)
{
  L7_RC_t error=L7_SUCCESS;
  L7_uint16 frame_max;
  L7_uint16 mask;
  L7_uint8 port;
  L7_HWEthRFC2819_PortStatistics stats;

  port = phy_state->Port;

  // Clear structure
  mask = phy_state->Mask;
  memset(phy_state,0x00,sizeof(L7_HWEthPhyState));
  phy_state->Port = port;
  phy_state->Mask = mask;
  stats.Port = port;
  stats.Mask = 0xFF;
  stats.RxMask = 0xFFFF;
  stats.TxMask = 0xFFFF;

  // Read some configurations: Speed and full-duplex
  if ( ptin_phy_config_get(port, NULL, &phy_state->Speed, NULL, &phy_state->Duplex, &frame_max, NULL, NULL) )
  {
    error = L7_FAILURE;
    memset(phy_state,0x00,sizeof(L7_HWEthPhyState));
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error with bcm56580_config_phy_get in port %u\n", __FUNCTION__,port);
  } else
  {
    // Read some state parameters: link up and autoneg complete
    if ( ptin_phy_state_get(port, &phy_state->LinkUp,&phy_state->AutoNegComplete) )
    {
      error = L7_FAILURE;
      memset(phy_state,0x00,sizeof(L7_HWEthPhyState));
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: error with bcm56580_phy_state in port %u\n", __FUNCTION__,port);
    } else
    {
      // Read statistics
      if (ptin_msg_getCounters(&stats)!=L7_SUCCESS)  return L7_FAILURE;

      //osapiSemaTake(sem_monitor_access,L7_WAIT_FOREVER);
      phy_state->Collisions = stats.Tx.etherStatsCollisions>0;
      phy_state->RxActivity = stats.Rx.Throughput>0;
      phy_state->TxActivity = stats.Tx.Throughput>0;
      // Media and MTU parameters
      phy_state->Media = ptin_monitor[port].PhyConf.Media;
      phy_state->MTU_mismatch = (frame_max>PHY_MAX_MAXFRAME);
      //osapiSemaGive(sem_monitor_access);

      phy_state->Suported_MaxFrame = PHY_MAX_MAXFRAME;

      // Unclear parameters
      phy_state->LOS = 0;         // FALSE
      phy_state->TxFault = 0;     // FALSE
      phy_state->RemoteFault = 0; // FALSE
    }
  }

  return L7_SUCCESS;
}

static L7_uint16 dhcp_bindtable_entries = 0;
static L7_DHCP_bind_entry dhcp_bindtable[PLAT_MAX_FDB_MAC_ENTRIES];

L7_RC_t ptin_msg_dhcp_bindtable_get(L7_DHCP_bind_table *ptr)
{
  dhcpSnoopBinding_t dsBinding;
  L7_ushort16        page, first, entries;
  L7_uint16          index, i;
  L7_int port;
  L7_uint16 flow_id;

  page = ptr->index & 0xFFFF;
    
  // For index null, read all mac entries
  if (page==0) {
    memset(&dsBinding,0x00,sizeof(dhcpSnoopBinding_t));
    for (i=0,index=0; i<PLAT_MAX_FDB_MAC_ENTRIES && usmDbDsBindingGetNext(&dsBinding)==L7_SUCCESS; i++)
    {
      // Calculate port reference and validate it
      if (ptin_translate_intf2port(dsBinding.intIfNum,&port)!=L7_SUCCESS || port>=L7_SYSTEM_N_INTERF)
        continue;
      // Extract vlan and validate it
      if (dsBinding.vlanId==0 || dsBinding.vlanId>4095)
        continue;
      // Calculate flow id and validate it
      if (ptin_flow_getId(port,dsBinding.vlanId,&flow_id)!=L7_SUCCESS || flow_id>=FLOWS_MAX)
        flow_id = (L7_uint16)-1;

      // Fill mac-table entry
      dhcp_bindtable[index].entry_index  = index;
      dhcp_bindtable[index].flow_id      = flow_id;
      dhcp_bindtable[index].service_vlan = dsBinding.vlanId;
      dhcp_bindtable[index].client_vlan  = dsBinding.innerVlanId;
      dhcp_bindtable[index].port         = port;
      memcpy(dhcp_bindtable[index].macAddr,dsBinding.macAddr,sizeof(uint8)*L7_MAC_ADDR_LEN);
      dhcp_bindtable[index].ipAddr       = dsBinding.ipAddr;
      dhcp_bindtable[index].remLeave     = dsBinding.remLease;
      dhcp_bindtable[index].bindingType  = dsBinding.bindingType;
      index++;
    }
    // Total number of entries
    dhcp_bindtable_entries = index;
  }

  // Validate page index
  if ((page*128)>dhcp_bindtable_entries)
    return L7_FAILURE;

  first   = page*128;
  entries = dhcp_bindtable_entries-first;   // Calculate remaining entries to be read
  if (entries>128)  entries = 128;          // Overgoes 256? If so, limit to 256

  ptr->bind_table_msg_size      = entries;
  ptr->bind_table_total_entries = dhcp_bindtable_entries;

  // Copy mac table entries
  memcpy(ptr->bind_table,&dhcp_bindtable[first],sizeof(L7_DHCP_bind_entry)*entries);

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_dhcp_bindtable_remove(L7_DHCP_bind_table *ptr)
{
  dhcpSnoopBinding_t dsBinding;
  L7_enetMacAddr_t   macAddr;
  L7_uint16          i, i_max;
  L7_RC_t            result = L7_SUCCESS;

  i_max = ptr->bind_table_msg_size;
  if (i_max>128)  i_max = 128;

  // For index null, read all mac entries
  for (i=0; i<i_max ; i++)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): FlowId =%u",__FUNCTION__,__LINE__,ptr->bind_table[i].flow_id);
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Port   =%u",__FUNCTION__,__LINE__,ptr->bind_table[i].port);
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): VlanId =%u",__FUNCTION__,__LINE__,ptr->bind_table[i].service_vlan);
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): CVlanId=%u",__FUNCTION__,__LINE__,ptr->bind_table[i].client_vlan);
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): MacAddr=%02X:%02X:%02X:%02X:%02X:%02X",__FUNCTION__,__LINE__,
                ptr->bind_table[i].macAddr[0],
                ptr->bind_table[i].macAddr[1],
                ptr->bind_table[i].macAddr[2],
                ptr->bind_table[i].macAddr[3],
                ptr->bind_table[i].macAddr[4],
                ptr->bind_table[i].macAddr[5]);

    // Find This entry
    memset(&dsBinding,0x00,sizeof(dhcpSnoopBinding_t));
    memcpy(dsBinding.macAddr,ptr->bind_table[i].macAddr,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
    if (usmDbDsBindingGet(&dsBinding)!=L7_SUCCESS) {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) This entry does not exist\n",__FUNCTION__,__LINE__);
      continue;
    }

    // Remove this entry
    memcpy(macAddr.addr,ptr->bind_table[i].macAddr,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
    if (usmDbDsBindingRemove(&macAddr)!=L7_SUCCESS) {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error removing entry\n",__FUNCTION__,__LINE__);
      result = L7_FAILURE;
      continue;
    }
  }

  return result;
}


static L7_uint16 mac_table_entries = 0;
static L7_switch_mac_entry mac_table[PLAT_MAX_FDB_MAC_ENTRIES];

L7_RC_t ptin_msg_mac_table_get(L7_switch_mac_table *ptr, L7_BOOL static_entry)
{
  L7_uchar8 keyNext[L7_FDB_KEY_SIZE];
  dot1dTpFdbData_t   fdbEntry;
  L7_ushort16        page, first, entries;
  L7_uint16          index, i;

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) index=%llu",__FUNCTION__,__LINE__,ptr->index);

  page = ptr->index & 0xFFFF;
  
  // For index null, read all mac entries
  if (page==0) {
    osapiSemaTake(ptin_module_l2_sem,L7_WAIT_FOREVER);
    memset(keyNext,0x00,sizeof(L7_uchar8)*L7_FDB_KEY_SIZE);
    for (i=0,index=0; i<PLAT_MAX_FDB_MAC_ENTRIES && fdbFind(keyNext,L7_MATCH_GETNEXT,&fdbEntry)==L7_SUCCESS; i++)
    {
      memcpy(keyNext, fdbEntry.dot1dTpFdbAddress, L7_FDB_KEY_SIZE);
      // Only save dynamic entries
      if (( static_entry && fdbEntry.dot1dTpFdbEntryType==L7_FDB_ADDR_FLAG_STATIC) ||
          (!static_entry && fdbEntry.dot1dTpFdbEntryType!=L7_FDB_ADDR_FLAG_STATIC)) {
        L7_int port;
        L7_uint16 vlan, flow_id;

        //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d) Entry found",__FUNCTION__,__LINE__);

        // Calculate port reference and validate it
        if (ptin_translate_intf2port(fdbEntry.dot1dTpFdbPort,&port)!=L7_SUCCESS || port>=L7_SYSTEM_N_INTERF) {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid port",__FUNCTION__,__LINE__);
          continue;
        }
        // Extract vlan and validate it
        vlan = (L7_uint16) *((L7_uint16 *) &fdbEntry.dot1dTpFdbAddress[0]);
        if (vlan==0 || vlan>4095) {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid vlanid (%u)",__FUNCTION__,__LINE__,vlan);
          continue;
        }
        // Calculate flow id and validate it
        if (ptin_flow_getId_fromInternalVlan(port,vlan,&flow_id)!=L7_SUCCESS || flow_id>=FLOWS_MAX) {
          //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) No EVC found for port %u and vlan %u",__FUNCTION__,__LINE__,port,vlan);
          flow_id = (L7_uint16)-1;
        }

        // Fill mac-table entry
        mac_table[index].mac_index      = index;
        mac_table[index].flow_id        = flow_id;
        mac_table[index].vlan_id        = vlan;
        mac_table[index].port           = port;
        mac_table[index].static_address = static_entry;
        memcpy(mac_table[index].mac_address, &fdbEntry.dot1dTpFdbAddress[2], 6);
        index++;
      }
    }
    osapiSemaGive(ptin_module_l2_sem);
    // Total number of entries
    mac_table_entries = index;
  }

  // Validate page index
  if ((page*256)>mac_table_entries)
    return L7_FAILURE;

  first   = page*256;
  entries = mac_table_entries-first;    // Calculate remaining entries to be read
  if (entries>256)  entries = 256;      // Overgoes 256? If so, limit to 256

  ptr->mac_table_msg_size      = entries;
  ptr->mac_table_total_entries = mac_table_entries;

  // Copy mac table entries
  memcpy(ptr->mac_table,&mac_table[first],sizeof(L7_switch_mac_entry)*entries);

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_mac_table_entry_remove(L7_switch_mac_table *ptr)
{
  L7_uchar8 keyNext[L7_FDB_KEY_SIZE];
  dot1dTpFdbData_t   fdbEntry;
  fdbMeberInfo_t     entry;
  L7_uint8           *mac_address;
  L7_uint16          i, i_max, vlanId;
  L7_RC_t            rc, result = L7_SUCCESS;

  i_max = ptr->mac_table_msg_size;
  if (i_max>256)  i_max = 256;

  // For index null, read all mac entries
  for (i=0; i<i_max ; i++)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): FlowId =%u",__FUNCTION__,__LINE__,ptr->mac_table[i].flow_id);
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Port   =%u",__FUNCTION__,__LINE__,ptr->mac_table[i].port);
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): VlanId =%u",__FUNCTION__,__LINE__,ptr->mac_table[i].vlan_id);
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): MacAddr=%02X:%02X:%02X:%02X:%02X:%02X",__FUNCTION__,__LINE__,
                ptr->mac_table[i].mac_address[0],
                ptr->mac_table[i].mac_address[1],
                ptr->mac_table[i].mac_address[2],
                ptr->mac_table[i].mac_address[3],
                ptr->mac_table[i].mac_address[4],
                ptr->mac_table[i].mac_address[5]);

    // Extract Internal VlanId
    if (ptr->mac_table[i].flow_id==(L7_uint16)-1) {
      vlanId      = ptr->mac_table[i].vlan_id;
      mac_address = ptr->mac_table[i].mac_address;

      // Validate Vlan
      if (vlanId<1 || vlanId>4095)  {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid vlan (%u)\n",__FUNCTION__,__LINE__,vlanId);
        result = L7_FAILURE;
        continue;
      }
  
      // Prepare key
      memcpy(&keyNext[0],&vlanId,sizeof(L7_uint16));
      memcpy(&keyNext[2],mac_address,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
  
      // Find This entry
      if (fdbFind(keyNext,L7_MATCH_EXACT,&fdbEntry)==L7_SUCCESS)
      {            
        // Remove this entry
        memcpy(entry.macAddr,&fdbEntry.dot1dTpFdbAddress[2],sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
        entry.vlanId    = (L7_uint16) *((L7_uint16 *) &fdbEntry.dot1dTpFdbAddress[0]);
        entry.intIfNum  = fdbEntry.dot1dTpFdbPort;
        entry.entryType = fdbEntry.dot1dTpFdbEntryType;
            
        if (fdbDelEntry(&entry)!=L7_SUCCESS) {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error removing entry\n",__FUNCTION__,__LINE__);
          result = L7_FAILURE;
          continue;
        }
      }
      else
      {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) This entry does not exist\n",__FUNCTION__,__LINE__);
      }
    }
    else
    {
      if ((rc=ptin_flow_L2StaticEntry_delete(ptr->mac_table[i].flow_id ,ptr->mac_table[i].port, ptr->mac_table[i].mac_address))!=L7_SUCCESS)
      {
        result = rc;
        continue;
      }
    }
  }

  return result;
}


L7_RC_t ptin_msg_mac_table_entry_add(L7_switch_mac_operation *ptr)
{
  L7_uchar8 keyNext[L7_FDB_KEY_SIZE];
  dot1dTpFdbData_t   fdbEntry;
  fdbMeberInfo_t     entry;
  L7_uint32          intIfNum;
  L7_uint16          vlanId;
  L7_uint8           port, *mac_address;
  L7_RC_t            rc;

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): FlowId =%u",__FUNCTION__,__LINE__,ptr->flow_id);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Port   =%u",__FUNCTION__,__LINE__,ptr->port);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): VlanId =%u",__FUNCTION__,__LINE__,ptr->vlan_id);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): MacAddr=%02X:%02X:%02X:%02X:%02X:%02X",__FUNCTION__,__LINE__,
              ptr->mac_address[0],
              ptr->mac_address[1],
              ptr->mac_address[2],
              ptr->mac_address[3],
              ptr->mac_address[4],
              ptr->mac_address[5]);

  // If flowId is not provided, use directly the vlanId
  if (ptr->flow_id==(L7_uint16)-1) {
    // Inputs
    port        = ptr->port;
    vlanId      = ptr->vlan_id;
    mac_address = ptr->mac_address;

    // Calculate and validate Interface
    if (ptin_translate_port2intf(port,&intIfNum)!=L7_SUCCESS || intIfNum==0)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid port ()\n",__FUNCTION__,__LINE__,port);
      return L7_FAILURE;
    }
    // Validate Vlan
    if (vlanId<1 || vlanId>4095)  {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Invalid vlan (%u)\n",__FUNCTION__,__LINE__,vlanId);
      return L7_FAILURE;
    }
  
    // Find This entry
    // If this entry exists, remove it
    memcpy(&keyNext[0],&vlanId,sizeof(L7_uint16));
    memcpy(&keyNext[2],mac_address,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
    if (fdbFind(keyNext,L7_MATCH_EXACT,&fdbEntry)==L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) This entry exists... going to remove it\n",__FUNCTION__,__LINE__);
      
      // Remove this entry
      memcpy(entry.macAddr,&fdbEntry.dot1dTpFdbAddress[2],sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
      entry.vlanId    = (L7_uint16) *((L7_uint16 *) &fdbEntry.dot1dTpFdbAddress[0]);
      entry.intIfNum  = fdbEntry.dot1dTpFdbPort;
      entry.entryType = fdbEntry.dot1dTpFdbEntryType;
      if (fdbDelEntry(&entry)!=L7_SUCCESS) {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error removing entry\n",__FUNCTION__,__LINE__);
      }
    }
  
    // Add this entry
    // Prepare entry remotion
    memcpy(entry.macAddr,mac_address,sizeof(L7_uint8)*L7_MAC_ADDR_LEN);
    entry.vlanId    = vlanId;
    entry.intIfNum  = intIfNum;
    entry.entryType = L7_FDB_ADDR_FLAG_STATIC;  
    if (fdbAddEntry(&entry)!=L7_SUCCESS) {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error adding entry\n",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
  }
  else
  {
    if ((rc=ptin_flow_L2StaticEntry_add(ptr->flow_id,ptr->port,ptr->mac_address))!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d) Error calling ptin_flow_L2StaticEntry_add\n",__FUNCTION__,__LINE__);
      return rc;
    }
  }

  return L7_SUCCESS;
}

 
L7_RC_t ptin_msg_getCounters(L7_HWEthRFC2819_PortStatistics *stat)
{
  return ptin_readCounters(stat);
}

L7_RC_t ptin_msg_getVlanCounters(L7_HWEth_VlanStatistics *stat, L7_uint16 *nClients)
{
  L7_HWEth_VlanStat counters[SNOOP_MAX_NUMBER_OF_CLIENTS];
  L7_uint16 flow_id, svid, cvid, n, i;
  L7_RC_t rc;

  // EVC id
  flow_id = stat->FlowId;
  // Service tag
  svid = stat->SVid;
  // Client tag
  cvid = stat->stat.CVid;

  memset(counters,0x00,sizeof(L7_HWEth_VlanStat)*SNOOP_MAX_NUMBER_OF_CLIENTS);
  counters[0].CVid = cvid;

  if ((rc=ptin_readVlanCounters(flow_id, svid, counters, &n))!=L7_SUCCESS)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s: failure in ptin_readVlanCounters\n",__FUNCTION__);
    return rc;
  }

  for (i=0; i<n; i++)
  {
    stat[i].FlowId = flow_id;
    stat[i].SVid   = svid;
    memcpy(&stat[i].stat,&counters[i],sizeof(L7_HWEth_VlanStat));
  }
  if (nClients!=L7_NULLPTR)  *nClients = n;

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_clearCounters(L7_HWEthRFC2819_PortStatistics *stat)
{
  return ptin_clearCounters( stat->Port );
}

L7_RC_t ptin_msg_getFlowCounters(st_ptin_flow_counters *ptr)
{
  st_ptin_fcounters_oper data;
  L7_BOOL   flow_in_use;
  L7_uint16 flow_id;
  L7_uint8  flow_type;

  flow_id = (L7_uint16) (ptr->index & 0xFFFF);

  // Validate flow
  if (ptin_flow_inUse(flow_id, &flow_in_use)!=L7_SUCCESS || !flow_in_use) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,"%s: flowId not in use",__FUNCTION__);
    return L7_FAILURE;
  }
  // Extract flow_type
  if (ptin_flow_getType(flow_id, &flow_type)!=L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,"%s: Error getting flow type",__FUNCTION__);
    return L7_FAILURE;
  }

  memset(&data,0x00,sizeof(st_ptin_fcounters_oper));

  // Operation is REMOVE COUNTERS
  data.oper = PTIN_READ_FLOW_ABS_COUNTERS;
  // Flow id
  data.flow_id = flow_id;

  // Flow type and client/channel
  switch (flow_type) {
  case FLOW_TYPE_UNICAST:
    data.flow_type = PTIN_FLOW_TYPE_UNICAST;
    data.client_channel = ptr->c_id.client_vlan;
    break;
  case FLOW_TYPE_MULTICAST:
    data.flow_type = PTIN_FLOW_TYPE_MULTICAST;
    data.client_channel = ptr->c_id.channel_ip;
    break;
  case FLOW_TYPE_BITSTREAM:
  case FLOW_TYPE_BITSTREAM_WITH_MACLEARN:
    data.flow_type = PTIN_FLOW_TYPE_UNICAST;
    data.client_channel = ptr->c_id.client_vlan;
    break;
  case FLOW_TYPE_UNIVOIP:
    data.flow_type = PTIN_FLOW_TYPE_UNICAST;
    data.client_channel = ptr->c_id.client_vlan;
    break;
  default:
    data.flow_type = PTIN_FLOW_TYPE_NONE;
    data.client_channel = 0;
    break;
  }

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) oper      = %u",__FUNCTION__,__LINE__,data.oper);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) flow_id   = %u",__FUNCTION__,__LINE__,data.flow_id);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) flow_type = %u",__FUNCTION__,__LINE__,data.flow_type);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) PON vlan  = %u",__FUNCTION__,__LINE__,data.pon_ports.vlan);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) PON ports = 0x%08X",__FUNCTION__,__LINE__,data.pon_ports.port_bmp);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) ETH vlan  = %u",__FUNCTION__,__LINE__,data.eth_ports.vlan);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) ETH ports = 0x%08X",__FUNCTION__,__LINE__,data.eth_ports.port_bmp);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) Client    = %u",__FUNCTION__,__LINE__,data.client_channel);

  // Send request
  if (dtlPtinFlowCounters( &data )!=L7_SUCCESS)  {
    //DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,"%s: Error with dtlPtinFlowCounters",__FUNCTION__);
    ptr->countersExist = L7_FALSE;
  }
  else {
    ptr->countersExist = L7_TRUE;
  }

  // Copy stat values
  memcpy(&ptr->stats,&data.stats,sizeof(st_ptin_flow_stats));

  return L7_SUCCESS;
}


L7_RC_t ptin_msg_addFlowCounters(st_ptin_flow_counters *ptr)
{
  L7_HwEthernetMef10CeVidMap plist[L7_SYSTEM_N_INTERF];
  st_ptin_fcounters_oper data;
  L7_BOOL   flow_in_use;
  L7_uint16 flow_id;
  L7_uint8  flow_type;
  L7_uint8  p;
  L7_uint32 p_bmp;
  L7_RC_t   rc;

  flow_id = (L7_uint16) (ptr->index & 0xFFFF);

  // Validate flow
  if (ptin_flow_inUse(flow_id, &flow_in_use)!=L7_SUCCESS || !flow_in_use) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,"%s: flowId not in use",__FUNCTION__);
    return L7_FAILURE;
  }
  // Extract flow_type
  if (ptin_flow_getType(flow_id, &flow_type)!=L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,"%s: Error getting flow type",__FUNCTION__);
    return L7_FAILURE;
  }
  // Extract list of ports
  if (ptin_flow_getPortVlanList(flow_id, plist)!=L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,"%s: Error getting port list",__FUNCTION__);
    return L7_FAILURE;
  }

  // Operation is ADD COUNTERS
  data.oper = PTIN_ADD_FLOW_COUNTERS;
  // Flow id
  data.flow_id = flow_id;

  // Flow type and client/channel
  switch (flow_type) {
  case FLOW_TYPE_UNICAST:
    data.flow_type = PTIN_FLOW_TYPE_UNICAST;
    data.client_channel = ptr->c_id.client_vlan;
    break;
  case FLOW_TYPE_MULTICAST:
    data.flow_type = PTIN_FLOW_TYPE_MULTICAST;
    data.client_channel = ptr->c_id.channel_ip;
    break;
  case FLOW_TYPE_BITSTREAM:
  case FLOW_TYPE_BITSTREAM_WITH_MACLEARN:
    data.flow_type = PTIN_FLOW_TYPE_UNICAST;
    data.client_channel = ptr->c_id.client_vlan;
    break;
  case FLOW_TYPE_UNIVOIP:
    data.flow_type = PTIN_FLOW_TYPE_UNICAST;
    data.client_channel = ptr->c_id.client_vlan;
    break;
  default:
    data.flow_type = PTIN_FLOW_TYPE_NONE;
    data.client_channel = 0;
    break;
  }
 
  // PON vlan and ports 
  data.pon_ports.vlan = 0;
  data.pon_ports.port_bmp = 0;
  for (p=0; p<igmp_firstRootPort; p++) {
    if (plist[p].type==MEF10_EVC_UNITYPE_UNUSED || plist[p].ceVid==0)
      continue;
    if (data.pon_ports.vlan==0) {
      data.pon_ports.vlan = plist[p].ceVid;
    }
    data.pon_ports.port_bmp |= (L7_uint32) 1<<p;
  }
  // Ethernet vlan and ports
  data.eth_ports.vlan = 0;
  data.eth_ports.port_bmp = 0;
  for (p=igmp_firstRootPort; p<L7_SYSTEM_N_INTERF; p++) {
    if (plist[p].type==MEF10_EVC_UNITYPE_UNUSED || plist[p].ceVid==0)
      continue;
    if (data.eth_ports.vlan==0) {
      data.eth_ports.vlan = plist[p].ceVid;
    }
    if (p<L7_SYSTEM_N_PORTS)  {
      data.eth_ports.port_bmp |= (L7_uint32) 1<<p;
    }
    else if (ptin_lag_memberList_get(p, &p_bmp)==L7_SUCCESS)  {
      data.eth_ports.port_bmp |= p_bmp;
    }
  }

  // Clear statistics
  memset(&data.stats,0x00,sizeof(st_ptin_flow_stats));

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) oper      = %u",__FUNCTION__,__LINE__,data.oper);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) flow_id   = %u",__FUNCTION__,__LINE__,data.flow_id);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) flow_type = %u",__FUNCTION__,__LINE__,data.flow_type);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) PON vlan  = %u",__FUNCTION__,__LINE__,data.pon_ports.vlan);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) PON ports = 0x%08X",__FUNCTION__,__LINE__,data.pon_ports.port_bmp);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) ETH vlan  = %u",__FUNCTION__,__LINE__,data.eth_ports.vlan);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) ETH ports = 0x%08X",__FUNCTION__,__LINE__,data.eth_ports.port_bmp);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) Client    = %u",__FUNCTION__,__LINE__,data.client_channel);

  // Send request
  if ((rc=dtlPtinFlowCounters( &data ))!=L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,"%s: Error with dtlPtinFlowCounters",__FUNCTION__);
    return rc;
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_msg_removeFlowCounters(st_ptin_flow_counters *ptr)
{
  st_ptin_fcounters_oper data;
  L7_BOOL   flow_in_use;
  L7_uint16 flow_id;
  L7_uint8  flow_type;
  L7_RC_t   rc;

  flow_id = (L7_uint16) (ptr->index & 0xFFFF);

  // Validate flow
  if (ptin_flow_inUse(flow_id, &flow_in_use)!=L7_SUCCESS || !flow_in_use) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,"%s: flowId not in use",__FUNCTION__);
    return L7_FAILURE;
  }
  // Extract flow_type
  if (ptin_flow_getType(flow_id, &flow_type)!=L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,"%s: Error getting flow type",__FUNCTION__);
    return L7_FAILURE;
  }

  memset(&data,0x00,sizeof(st_ptin_fcounters_oper));

  // Operation is REMOVE COUNTERS
  data.oper = PTIN_REMOVE_FLOW_COUNTERS;
  // Flow id
  data.flow_id = flow_id;

  // Flow type and client/channel
  switch (flow_type) {
  case FLOW_TYPE_UNICAST:
    data.flow_type = PTIN_FLOW_TYPE_UNICAST;
    data.client_channel = ptr->c_id.client_vlan;
    break;
  case FLOW_TYPE_MULTICAST:
    data.flow_type = PTIN_FLOW_TYPE_MULTICAST;
    data.client_channel = ptr->c_id.channel_ip;
    break;
  case FLOW_TYPE_BITSTREAM:
  case FLOW_TYPE_BITSTREAM_WITH_MACLEARN:
    data.flow_type = PTIN_FLOW_TYPE_UNICAST;
    data.client_channel = ptr->c_id.client_vlan;
    break;
  case FLOW_TYPE_UNIVOIP:
    data.flow_type = PTIN_FLOW_TYPE_UNICAST;
    data.client_channel = ptr->c_id.client_vlan;
    break;
  default:
    data.flow_type = PTIN_FLOW_TYPE_NONE;
    data.client_channel = 0;
    break;
  }

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) oper      = %u",__FUNCTION__,__LINE__,data.oper);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) flow_id   = %u",__FUNCTION__,__LINE__,data.flow_id);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) flow_type = %u",__FUNCTION__,__LINE__,data.flow_type);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) PON vlan  = %u",__FUNCTION__,__LINE__,data.pon_ports.vlan);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) PON ports = 0x%08X",__FUNCTION__,__LINE__,data.pon_ports.port_bmp);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) ETH vlan  = %u",__FUNCTION__,__LINE__,data.eth_ports.vlan);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) ETH ports = 0x%08X",__FUNCTION__,__LINE__,data.eth_ports.port_bmp);
  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL,"%s(%d) Client    = %u",__FUNCTION__,__LINE__,data.client_channel);

  // Send request
  if ((rc=dtlPtinFlowCounters( &data ))!=L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR,"%s: Error with dtlPtinFlowCounters",__FUNCTION__);
    return rc;
  }

  return L7_SUCCESS;
}


#define PTIN_NO_CONFIG          0

#define PTIN_QOS_UNTRUST_MARKS  1
#define PTIN_QOS_802_1P_MARKS   2
#define PTIN_QOS_IP_PREC_MARKS  3
#define PTIN_QOS_IP_DSCP_MARKS  4

#define PTIN_QOS_SCHEDULER_STRICT   1
#define PTIN_QOS_SCHEDULER_WEIGHTED 2

#define PTIN_GEN_MASK_TRUST_MODE      0x01
#define PTIN_GEN_MASK_BANDWIDTH_UNIT  0x02
#define PTIN_GEN_MASK_SHAPING_RATE    0x04
#define PTIN_GEN_MASK_PACKET_PRIO     0x08
#define PTIN_GEN_MASK_COS_CONFIG      0x10

#define PTIN_COS_MASK_SCHEDULER_TYPE  0x01
#define PTIN_COS_MASK_MIN_BANDWIDTH   0x02
#define PTIN_COS_MASK_MAX_BANDWIDTH   0x04

static L7_RC_t ptin_msg_qos_config_set_one(L7_QoSConfiguration *qos_cell);

L7_RC_t ptin_msg_qos_config_set(L7_QoSConfiguration *qos, L7_uint8 n_cells)
{
  L7_uint8 i;
  L7_RC_t rc, rc_global = L7_SUCCESS;

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Start",__FUNCTION__,__LINE__);

  // Validate arguments
  if (qos==L7_NULLPTR)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid arguments",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Run all configuration elements
  for (i=0; i<n_cells; i++)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Processing cell %u",__FUNCTION__,__LINE__,i);

    if ((rc=ptin_msg_qos_config_set_one(&qos[i]))==L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Success",__FUNCTION__,__LINE__);
    }
    else {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Error!",__FUNCTION__,__LINE__);
      rc_global = rc;
    }
  }

  return rc_global;
}

static L7_RC_t ptin_msg_qos_config_set_one(L7_QoSConfiguration *qos_cell)
{
  L7_int    lag_port;
  L7_uint8  port, lagid, pbit, pbit2;
  L7_uint32 cos;
  L7_uint32 intf, lag_intf=0;
  L7_QOS_COS_MAP_INTF_MODE_t trust_mode;
  L7_qosCosQueueSchedTypeList_t schedTypeList;
  L7_qosCosQueueBwList_t minBwList, maxBwList;
  L7_RC_t rc = L7_SUCCESS;
  //L7_uint32 new_intf_prio_matrix[L7_SYSTEM_N_INTERF][8];

  // Validate arguments
  if (qos_cell==L7_NULLPTR)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid arguments",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Calculate interface
  port = qos_cell->index & 0xFF;
  if (port!=0xFF)
  {
    // Is port a lag reference?
    if (port & 0x80)
    {
      // Calculate lag id
      lagid = port & 0x7F;
      if (lagid==0)
      {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Lag id is null",__FUNCTION__,__LINE__);
        return L7_FAILURE;
      }
      // Convert to port reference
      port = L7_SYSTEM_N_PORTS+lagid-1;
    }
    // Calculate interface
    if (ptin_translate_port2intf(port, &intf)!=L7_SUCCESS || intf==0)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error calculating interface for port %u",__FUNCTION__,__LINE__,port);
      return L7_FAILURE;
    }

    // Physical port
    if (port<L7_SYSTEM_N_PORTS)
    {
       if (usmDbDot3adValidIntfCheck(1,intf))
       {
         DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): This port (%u) is not suppoed to be a lag",__FUNCTION__,__LINE__,port);
         return L7_FAILURE;
       }
       // If this interface is associated to a lag, and that lag is configured,
       // replace the interf reference for that lag
       if (usmDbDot3adIntfIsMemberGet(1,intf,&lag_intf)==L7_SUCCESS && lag_intf!=0 && usmDbLagIsConfigured(1,lag_intf))
       {
         // Calculate correpondent port reference
         if (ptin_translate_intf2port(lag_intf,&lag_port)==L7_SUCCESS && lag_port>=L7_SYSTEM_N_PORTS && lag_port<L7_SYSTEM_N_INTERF)
         {
           //port = (L7_uint8) port_lag;
           //intf = lag_intf;
         }
         else
         {
           DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): There is no port associated to this interf (%u)",__FUNCTION__,__LINE__,intf);
           return L7_FAILURE;
         }
       }
    }
    // LAG port
    else
    {
      // Validate lag interface (if it is a lag)
      if (!usmDbDot3adValidIntfCheck(1,intf) || !usmDbLagIsConfigured(1,intf))
      {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid Lag",__FUNCTION__,__LINE__);
        return L7_FAILURE;
      }
    }
  }
  else
  {
    intf = L7_ALL_INTERFACES;
  }

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Going to configure port %u, interface=%u",__FUNCTION__,__LINE__,port,intf);

  // Is a valid interface for QoS operation?
  if (!usmDbQosCosMapIntfIsValid(1,intf) ||
      (lag_intf!=0 && !usmDbQosCosMapIntfIsValid(1,lag_intf)))
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid interface por QoS operation",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Get Trust mode
  if (usmDbQosCosMapTrustModeGet( 1, ((lag_intf!=0) ? lag_intf : intf), &trust_mode)!=L7_SUCCESS)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosMapTrustModeGet",__FUNCTION__,__LINE__);
    rc = L7_FAILURE;
  }
  // Validate trust mode
  else if (trust_mode==PTIN_NO_CONFIG || trust_mode>L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid trust mode (%u)",__FUNCTION__,__LINE__,trust_mode);
    rc = L7_FAILURE;
  }
  // Trust mode was correctly read
  else
  {
    // Mask to change trust mode
    if ((qos_cell->mask & PTIN_GEN_MASK_TRUST_MODE) && 
        qos_cell->trust_mode!=PTIN_NO_CONFIG)
    {
      // Validate trust mode value
      if (qos_cell->trust_mode<=L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
      {
        // Define trust mode
        if (usmDbQosCosMapTrustModeSet(1,intf,qos_cell->trust_mode)!=L7_SUCCESS ||
            (lag_intf!=0 && usmDbQosCosMapTrustModeSet(1,lag_intf,qos_cell->trust_mode)!=L7_SUCCESS))
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosMapTrustModeSet",__FUNCTION__,__LINE__);
          rc = L7_FAILURE;
        }
        else
        {
          // Configuration successfull => change trust mode value
          trust_mode = qos_cell->trust_mode;
        }
      }
      else
      {
        rc = L7_FAILURE;
      }
    }

    // Mask to change shaping rate
    if ((qos_cell->mask & PTIN_GEN_MASK_SHAPING_RATE))
    {
      // Shaping rate
      if (usmDbQosCosQueueIntfShapingRateSet(1,intf,qos_cell->shaping_rate)!=L7_SUCCESS ||
          (lag_intf!=0 && usmDbQosCosQueueIntfShapingRateSet(1,lag_intf,qos_cell->shaping_rate)!=L7_SUCCESS))
      {  
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosQueueIntfShapingRateSet",__FUNCTION__,__LINE__);
        rc = L7_FAILURE;
      }
    }

    // Only define priority map, if mode is not untrusted, na dif mask is open
    if ((qos_cell->mask & PTIN_GEN_MASK_PACKET_PRIO) &&
        trust_mode!=L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED)
    {
      // For 802.1p trust mode, get qos mapping to be applied, and check if there are enough resources
      if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
      {
        // Run all 8 priorities
        for (pbit=0; pbit<8; pbit++)
        {
          // Check prio mask to check if this priority should be processed
          if ( !((qos_cell->pktprio.mask>>pbit) & 1) )
          {
            /* Get qos map, and fill qos list */
            if (usmDbDot1dTrafficClassGet(1,intf,pbit,&cos)==L7_SUCCESS)
            {
              qos_cell->pktprio.cos[pbit] = cos & 0x0f;
            }
            else
            {
              DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbDot1dTrafficClassGet",__FUNCTION__,__LINE__);
              return L7_TABLE_IS_FULL;
            }
          }
        }

        /* Check if we have enough resources for this operation */
        if (!ptin_interface_qos_valid(intf,qos_cell->pktprio.cos))
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Not enough resources for this operation!",__FUNCTION__,__LINE__);
          return L7_TABLE_IS_FULL;
        }
      }

      // Run all 8 priorities
      for (pbit=0; pbit<8; pbit++)
      {
        // Check prio mask to check if this priority should be processed
        if ( !((qos_cell->pktprio.mask>>pbit) & 1) )  continue;

        // 802.1p trust mode
        if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
        {
          if ((cos=(qos_cell->pktprio.cos[pbit] & 0xF))>7)  continue;

          if (usmDbDot1dTrafficClassSet(1,intf,pbit,cos)!=L7_SUCCESS ||
              (lag_intf!=0 && usmDbDot1dTrafficClassSet(1,lag_intf,pbit,cos)!=L7_SUCCESS))
          {
            DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosQueueIntfShapingRateSet",__FUNCTION__,__LINE__);
            rc = L7_FAILURE;
          }
        }
        // IP-precedence trust mode
        else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
        {
          if ((cos=(qos_cell->pktprio.cos[pbit] & 0xF))>7)  continue;

          if (usmDbQosCosMapIpPrecTrafficClassSet(1, intf, pbit, cos)!=L7_SUCCESS ||
              (lag_intf!=0 && usmDbQosCosMapIpPrecTrafficClassSet(1, lag_intf, pbit, cos)!=L7_SUCCESS))
          { 
            DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosQueueIntfShapingRateSet",__FUNCTION__,__LINE__); 
            rc = L7_FAILURE;
          }
        }
        // DSCP trust mode
        else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
        {
          // Run all 8 sub-priorities (8*8=64 possiblle priorities)
          for (pbit2=0; pbit2<8; pbit2++)
          {
            if ((cos=(((qos_cell->pktprio.cos[pbit])>>(4*pbit2)) & 0xF))>7)  continue;

            if (usmDbQosCosMapIpDscpTrafficClassSet(1, intf, pbit*8+pbit2, cos)!=L7_SUCCESS ||
                (lag_intf!=0 && usmDbQosCosMapIpDscpTrafficClassSet(1, lag_intf, pbit*8+pbit2, cos)!=L7_SUCCESS))
            {
              DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosMapIpDscpTrafficClassSet",__FUNCTION__,__LINE__);
              rc = L7_FAILURE;
            }
          } // Run all 8 sub-priorities (8*8=64 possiblle priorities)
        }
      } // Run all 8 priorities
    } // Only define priority map, if mode is not untrusted, na dif mask is open
  } // Get trust mode
  
  // Mask to change CoS configurations
  if ((qos_cell->mask & PTIN_GEN_MASK_COS_CONFIG))
  {
    // Get scheduler type definition for all queues, min and max bw
    if (usmDbQosCosQueueSchedulerTypeListGet( 1, ((lag_intf!=0) ? lag_intf : intf), &schedTypeList)!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error reading CoS' configurations",__FUNCTION__,__LINE__);
      rc = L7_FAILURE;
    }
    // Reading successfull
    else
    {
      // Run all 8 CoS queues
      for (cos=0; cos<L7_MAX_CFG_QUEUES_PER_PORT; cos++)
      {
        // Check if scheduler shoud be configured for this CoS
        if ( !((qos_cell->cos_config.mask>>cos) & 1) ||
             !(qos_cell->cos_config.cos[cos].mask & PTIN_COS_MASK_SCHEDULER_TYPE) ||
              (qos_cell->cos_config.cos[cos].scheduler==PTIN_NO_CONFIG) )
          continue;

        // Validate scheduler type
        if (qos_cell->cos_config.cos[cos].scheduler>L7_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED)
        {
          rc = L7_FAILURE;  
          continue;
        }

        // Scheduler type
        schedTypeList.schedType[cos] = qos_cell->cos_config.cos[cos].scheduler;       
      } // Run all 8 CoS queues

      // Scheduler type definition for all queues, min and max bw
      if (usmDbQosCosQueueSchedulerTypeListSet(1,intf,&schedTypeList)!=L7_SUCCESS ||
          (lag_intf!=0 && usmDbQosCosQueueSchedulerTypeListSet(1,lag_intf,&schedTypeList)!=L7_SUCCESS))
      {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error setting CoS' configurations",__FUNCTION__,__LINE__);
        rc = L7_FAILURE;
      }
    } // Reading successfull

    // Get scheduler type definition for all queues, min and max bw
    if (usmDbQosCosQueueMinBandwidthListGet( 1, ((lag_intf!=0) ? lag_intf : intf), &minBwList)!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error reading CoS' configurations",__FUNCTION__,__LINE__);
      rc = L7_FAILURE;
    }
    else if (usmDbQosCosQueueMaxBandwidthListGet( 1, ((lag_intf!=0) ? lag_intf : intf), &maxBwList)!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error reading CoS' configurations",__FUNCTION__,__LINE__);
      rc = L7_FAILURE;
    }
    // Reading successfull
    else
    {
      // Run all 8 CoS queues
      for (cos=0; cos<L7_MAX_CFG_QUEUES_PER_PORT; cos++)
      {
        // Check if min. bandwidth shoud be configured for this CoS
        if ( !((qos_cell->cos_config.mask>>cos) & 1) ||
             !(qos_cell->cos_config.cos[cos].mask & PTIN_COS_MASK_MIN_BANDWIDTH) )
          continue;

        // Minimum and maximum bandwidth (already given in kbps)
        minBwList.bandwidth[cos] = qos_cell->cos_config.cos[cos].min_bandwidth;
        maxBwList.bandwidth[cos] = qos_cell->cos_config.cos[cos].max_bandwidth;
      } // Run all 8 CoS queues
  
      // Scheduler type definition for all queues, min and max bw
      if (usmDbQosCosQueueMinBandwidthListSet(1,intf,&minBwList)!=L7_SUCCESS ||
          (lag_intf!=0 && usmDbQosCosQueueMinBandwidthListSet(1,lag_intf,&minBwList)!=L7_SUCCESS))
      {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error setting CoS' configurations",__FUNCTION__,__LINE__);
        rc = L7_NOT_SUPPORTED;
      }
      if (usmDbQosCosQueueMaxBandwidthListSet(1,intf,&maxBwList)!=L7_SUCCESS ||
          (lag_intf!=0 && usmDbQosCosQueueMaxBandwidthListSet(1,lag_intf,&maxBwList)!=L7_SUCCESS))
      {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error setting CoS' configurations",__FUNCTION__,__LINE__);
        rc = L7_NOT_SUPPORTED;
      }
    } // Reading successfull
  } // Mask to change CoS configurations

  return rc;
}

/*
L7_RC_t ptin_msg_qos_config_set(L7_QoSConfiguration *qos, L7_uint8 n_cells)
{
  L7_uint8 i, port, lagid, pbit, pbit2, cos;
  L7_uint32 intf;
  L7_QoSConfiguration *qos_cell;
  L7_QOS_COS_MAP_INTF_MODE_t trust_mode;
  L7_qosCosQueueSchedTypeList_t schedTypeList;
  L7_qosCosQueueBwList_t minBwList, maxBwList;
  L7_RC_t rc = L7_SUCCESS;

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Start",__FUNCTION__,__LINE__);

  // Validate arguments
  if (qos==L7_NULLPTR)  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid arguments",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Run all configuration elements
  for (i=0; i<n_cells; i++)
  {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Processing cell %u",__FUNCTION__,__LINE__,i);

    qos_cell = &qos[i];

    // Calculate interface
    port = qos_cell->index & 0xFF;
    if (port!=0xFF)
    {
      // Is port a lag reference?
      if (port & 0x80)
      {
        // Calculate lag id
        lagid = port & 0x7F;
        if (lagid==0)
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Lag id is null",__FUNCTION__,__LINE__);
          return L7_FAILURE;
        }
        // Convert to port reference
        port = L7_SYSTEM_N_PORTS+lagid-1;
      }
      // Calculate interface
      if (ptin_translate_port2intf(port, &intf)!=L7_SUCCESS || intf==0)
      {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error calculating interface for port %u",__FUNCTION__,__LINE__,port);
        return L7_FAILURE;
      }
      // Validate lag interface (if it is a lag)
      if (port>=L7_SYSTEM_N_PORTS && (!usmDbDot3adValidIntfCheck(1,intf) || !usmDbLagIsConfigured(1,intf)))
      {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid Lag",__FUNCTION__,__LINE__);
        return L7_FAILURE;
      }
    }
    else
    {
      intf = L7_ALL_INTERFACES;
    }

    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Going to configure port %u, interface=%u",__FUNCTION__,__LINE__,port,intf);

    // Is a valid interface for QoS operation?
    if (!usmDbQosCosMapIntfIsValid(1,intf))
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid interface por QoS operation",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }

    // Get Trust mode
    if (usmDbQosCosMapTrustModeGet(1,intf,&trust_mode)!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosMapTrustModeGet",__FUNCTION__,__LINE__);
      rc = L7_FAILURE;
    }
    // Validate trust mode
    else if (trust_mode==PTIN_NO_CONFIG || trust_mode>L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid trust mode (%u)",__FUNCTION__,__LINE__,trust_mode);
      rc = L7_FAILURE;
    }
    // Trust mode was correctly read
    else
    {
      // Mask to change trust mode
      if ((qos_cell->mask & PTIN_GEN_MASK_TRUST_MODE) && 
          qos_cell->trust_mode!=PTIN_NO_CONFIG)
      {
        // Validate trust mode value
        if (qos_cell->trust_mode<=L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
        {
          // Define trust mode
          if (usmDbQosCosMapTrustModeSet(1,intf,qos_cell->trust_mode)!=L7_SUCCESS)
          {
            DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosMapTrustModeSet",__FUNCTION__,__LINE__);
            rc = L7_FAILURE;
          }
          else
          {
            // Configuration successfull => change trust mode value
            trust_mode = qos_cell->trust_mode;
          }
        }
        else
        {
          rc = L7_FAILURE;
        }
      }

      // Mask to change shaping rate
      if ((qos_cell->mask & PTIN_GEN_MASK_SHAPING_RATE))
      {
        // Shaping rate
        if (usmDbQosCosQueueIntfShapingRateSet(1,intf,qos_cell->shaping_rate)!=L7_SUCCESS)
        {  
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosQueueIntfShapingRateSet",__FUNCTION__,__LINE__);
          rc = L7_FAILURE;
        }
      }
  
      // Only define priority map, if mode is not untrusted, na dif mask is open
      if ((qos_cell->mask & PTIN_GEN_MASK_PACKET_PRIO) &&
          trust_mode!=L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED)
      {
        // Run all 8 priorities
        for (pbit=0; pbit<8; pbit++)
        {
          // Check prio mask to check if this priority should be processed
          if ( !((qos_cell->pktprio.mask>>pbit) & 1) )  continue;

          // 802.1p trust mode
          if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
          {
            if ((cos=(qos_cell->pktprio.cos[pbit] & 0xF))>7)  continue;
  
            if (usmDbDot1dTrafficClassSet(1,intf,pbit,cos)!=L7_SUCCESS)
            {
              DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosQueueIntfShapingRateSet",__FUNCTION__,__LINE__);
              rc = L7_FAILURE;
            }
          }
          // IP-precedence trust mode
          else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
          {
            if ((cos=(qos_cell->pktprio.cos[pbit] & 0xF))>7)  continue;
  
            if (usmDbQosCosMapIpPrecTrafficClassSet(1, intf, pbit, cos)!=L7_SUCCESS)
            { 
              DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosQueueIntfShapingRateSet",__FUNCTION__,__LINE__); 
              rc = L7_FAILURE;
            }
          }
          // DSCP trust mode
          else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
          {
            // Run all 8 sub-priorities (8*8=64 possiblle priorities)
            for (pbit2=0; pbit2<8; pbit2++)
            {
              if ((cos=(((qos_cell->pktprio.cos[pbit])>>(4*pbit2)) & 0xF))>7)  continue;
  
              if (usmDbQosCosMapIpDscpTrafficClassSet(1, intf, pbit*8+pbit2, cos)!=L7_SUCCESS)
              {
                DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosMapIpDscpTrafficClassSet",__FUNCTION__,__LINE__);
                rc = L7_FAILURE;
              }
            } // Run all 8 sub-priorities (8*8=64 possiblle priorities)
          }
        } // Run all 8 priorities
      } // Only define priority map, if mode is not untrusted, na dif mask is open
    } // Get trust mode
    
    // Mask to change CoS configurations
    if ((qos_cell->mask & PTIN_GEN_MASK_COS_CONFIG))
    {
      // Get scheduler type definition for all queues, min and max bw
      if (usmDbQosCosQueueSchedulerTypeListGet(1,intf,&schedTypeList)!=L7_SUCCESS)
      {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error reading CoS' configurations",__FUNCTION__,__LINE__);
        rc = L7_FAILURE;
      }
      // Reading successfull
      else
      {
        // Run all 8 CoS queues
        for (cos=0; cos<L7_MAX_CFG_QUEUES_PER_PORT; cos++)
        {
          // Check if scheduler shoud be configured for this CoS
          if ( !((qos_cell->cos_config.mask>>cos) & 1) ||
               !(qos_cell->cos_config.cos[cos].mask & PTIN_COS_MASK_SCHEDULER_TYPE) ||
                (qos_cell->cos_config.cos[cos].scheduler==PTIN_NO_CONFIG) )
            continue;

          // Validate scheduler type
          if (qos_cell->cos_config.cos[cos].scheduler>L7_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED)
          {
            rc = L7_FAILURE;  
            continue;
          }

          // Scheduler type
          schedTypeList.schedType[cos] = qos_cell->cos_config.cos[cos].scheduler;       
        } // Run all 8 CoS queues
  
        // Scheduler type definition for all queues, min and max bw
        if (usmDbQosCosQueueSchedulerTypeListSet(1,intf,&schedTypeList)!=L7_SUCCESS)
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error setting CoS' configurations",__FUNCTION__,__LINE__);
          rc = L7_FAILURE;
        }
      } // Reading successfull
  
      // Get scheduler type definition for all queues, min and max bw
      if (usmDbQosCosQueueMinBandwidthListGet(1,intf,&minBwList)!=L7_SUCCESS)
      {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error reading CoS' configurations",__FUNCTION__,__LINE__);
        rc = L7_FAILURE;
      }
      else if (usmDbQosCosQueueMaxBandwidthListGet(1,intf,&maxBwList)!=L7_SUCCESS)
      {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error reading CoS' configurations",__FUNCTION__,__LINE__);
        rc = L7_FAILURE;
      }
      // Reading successfull
      else
      {
        // Run all 8 CoS queues
        for (cos=0; cos<L7_MAX_CFG_QUEUES_PER_PORT; cos++)
        {
          // Check if min. bandwidth shoud be configured for this CoS
          if ( !((qos_cell->cos_config.mask>>cos) & 1) ||
               !(qos_cell->cos_config.cos[cos].mask & PTIN_COS_MASK_MIN_BANDWIDTH) )
            continue;

          // Minimum and maximum bandwidth (already given in kbps)
          minBwList.bandwidth[cos] = qos_cell->cos_config.cos[cos].min_bandwidth;
          maxBwList.bandwidth[cos] = qos_cell->cos_config.cos[cos].max_bandwidth;
        } // Run all 8 CoS queues
    
        // Scheduler type definition for all queues, min and max bw
        if (usmDbQosCosQueueMinBandwidthListSet(1,intf,&minBwList)!=L7_SUCCESS)
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error setting CoS' configurations",__FUNCTION__,__LINE__);
          rc = L7_NOT_SUPPORTED;
        }
        if (usmDbQosCosQueueMaxBandwidthListSet(1,intf,&maxBwList)!=L7_SUCCESS)
        {
          DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error setting CoS' configurations",__FUNCTION__,__LINE__);
          rc = L7_NOT_SUPPORTED;
        }
      } // Reading successfull
    } // Mask to change CoS configurations
  } // Run all configuration elements

  if (rc==L7_SUCCESS) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Success",__FUNCTION__,__LINE__);
  }
  else {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Error!",__FUNCTION__,__LINE__);
  }
  return rc;
}
*/

L7_RC_t ptin_msg_qos_config_get(L7_QoSConfiguration *qos, L7_uint8 *n_cells)
{
  L7_uint8 p, port, lagid, pbit, pbit2, cell_index;
  L7_uint32 cos, intf;
  L7_QoSConfiguration *qos_cell;
  L7_QOS_COS_MAP_INTF_MODE_t trust_mode;
  L7_qosCosQueueSchedTypeList_t schedTypeList;
  L7_qosCosQueueBwList_t minBwList, maxBwList;

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Start",__FUNCTION__,__LINE__);

  // Validate arguments
  if (qos==L7_NULLPTR || n_cells==L7_NULLPTR) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid arguments",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  // Initialize n_cells to zero
  *n_cells = 0;

  // Correct port reference
  port = qos->index & 0xFF;
  if (port!=0xFF)
  {
    // Is port a lag reference?
    if (port & 0x80)
    {
      // Calculate lag id
      lagid = port & 0x7F;
      if (lagid==0)
      {
        DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid lag id (0)",__FUNCTION__,__LINE__);
        return L7_FAILURE;
      }
      // Convert to port reference
      port = L7_SYSTEM_N_PORTS+lagid-1;
    }
  }

  // Run all ports
  cell_index = 0;
  for (p=0; p<L7_SYSTEM_N_INTERF; p++)
  {
    // All ports
    if (port==0xFF)
      qos_cell = &qos[cell_index];
    // Selective port reading
    else if (port==p)
      qos_cell = &qos[0];
    // Skip to the next port
    else
      continue;

    // Calculate interface, related to this port
    if (ptin_translate_port2intf(p, &intf)!=L7_SUCCESS || intf==0)
      continue;
    // Validate lag interface (if it is a lag)
    if (p>=L7_SYSTEM_N_PORTS && (!usmDbDot3adValidIntfCheck(1,intf) || !usmDbLagIsConfigured(1,intf)))
      continue;
    // Is a valid interface for QoS operation?
    if (!usmDbQosCosMapIntfIsValid(1,intf))
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Interface %u (port %u) is not valid for QoS operation",__FUNCTION__,__LINE__,intf,port);
      return L7_FAILURE;
    }

    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Processing port=%u",__FUNCTION__,__LINE__,port);

    // Increment number of cells
    cell_index++;

    // Clear all structure
    memset(qos_cell,0x00,sizeof(L7_QoSConfiguration));

    // No error (yet)
    qos_cell->err_code = 0;
    // Port reference
    qos_cell->index = (p<L7_SYSTEM_N_PORTS) ? p : (0x80 | (p-L7_SYSTEM_N_INTERF+1));

    // Get Trust mode
    if (usmDbQosCosMapTrustModeGet(1,intf,&trust_mode)!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosMapTrustModeGet",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
    // Validate trust mode
    if (trust_mode==PTIN_NO_CONFIG || trust_mode>L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP) {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Invalid trust mode (%u)",__FUNCTION__,__LINE__,trust_mode);
      return L7_FAILURE;
    }
    // trust mode
    qos_cell->trust_mode = trust_mode;
    qos_cell->mask |= PTIN_GEN_MASK_TRUST_MODE;

    // Percentage unit
    qos_cell->bandwidth_unit = 0;
    qos_cell->mask |= PTIN_GEN_MASK_BANDWIDTH_UNIT;

    // Shaping rate
    if (usmDbQosCosQueueIntfShapingRateGet(1,intf,&qos_cell->shaping_rate)!=L7_SUCCESS)
    {  
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosQueueIntfShapingRateGet",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
    qos_cell->mask |= PTIN_GEN_MASK_SHAPING_RATE;

    // Only read priority map, if trust mode is not untrusted
    if (trust_mode!=L7_QOS_COS_MAP_INTF_MODE_UNTRUSTED)
    {
      // Run all 8 priorities, to get priority map
      for (pbit=0; pbit<8; pbit++)
      {
        // 802.1p trust mode
        if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_DOT1P)
        {
          if (usmDbDot1dTrafficClassGet(1,intf,pbit,&cos)!=L7_SUCCESS)
          {
            DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbDot1dTrafficClassGet",__FUNCTION__,__LINE__);
            return L7_FAILURE;
          }
          qos_cell->pktprio.cos[pbit] = cos;
        }
        // IP-precedence trust mode
        else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPPREC)
        {
          if (usmDbQosCosMapIpPrecTrafficClassGet(1, intf, pbit, &cos)!=L7_SUCCESS)
          {  
            DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosMapIpPrecTrafficClassGet",__FUNCTION__,__LINE__);
            return L7_FAILURE;
          }
          qos_cell->pktprio.cos[pbit] = cos;
        }
        // DSCP trust mode
        else if (trust_mode==L7_QOS_COS_MAP_INTF_MODE_TRUST_IPDSCP)
        {
          // Run all 8 sub-priorities (8*8=64 possiblle priorities)
          for (pbit2=0; pbit2<8; pbit2++) {
            if (usmDbQosCosMapIpDscpTrafficClassGet(1, intf, pbit*8+pbit2, &cos)!=L7_SUCCESS)
            {  
              DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosMapIpDscpTrafficClassGet",__FUNCTION__,__LINE__);
              return L7_FAILURE;
            }
            qos_cell->pktprio.cos[pbit] |= (cos & 0xF)<<(4*pbit2);
          } // Run all 8 sub-priorities (8*8=64 possiblle priorities)
        }
      } // Run all 8 priorities
      qos_cell->pktprio.mask = 0xFF;
      qos_cell->mask |= PTIN_GEN_MASK_PACKET_PRIO;
    }
  
    // Get scheduler type definition for all queues, min and max bw
    if (usmDbQosCosQueueSchedulerTypeListGet(1,intf,&schedTypeList)!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosQueueSchedulerTypeListGet",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
    // Run all 8 CoS queues
    for (cos=0; cos<L7_MAX_CFG_QUEUES_PER_PORT; cos++)
    {
      // Validate scheduler type
      if (schedTypeList.schedType[cos]==PTIN_NO_CONFIG ||
          schedTypeList.schedType[cos]>L7_QOS_COS_QUEUE_SCHED_TYPE_WEIGHTED) {
        return L7_FAILURE;
      }

      // Scheduler type
      qos_cell->cos_config.cos[cos].scheduler = schedTypeList.schedType[cos];
      qos_cell->cos_config.cos[cos].mask |= PTIN_COS_MASK_SCHEDULER_TYPE;
    } // Run all 8 CoS queues

    // Get min and max bandwidth for all queues
    if (usmDbQosCosQueueMinBandwidthListGet(1,intf,&minBwList)!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosQueueMinBandwidthListGet",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
    if (usmDbQosCosQueueMaxBandwidthListGet(1,intf,&maxBwList)!=L7_SUCCESS)
    {
      DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Error with usmDbQosCosQueueMaxBandwidthListGet",__FUNCTION__,__LINE__);
      return L7_FAILURE;
    }
    // Run all 8 CoS queues
    for (cos=0; cos<L7_MAX_CFG_QUEUES_PER_PORT; cos++)
    {
      // Minimum bandwidth (other side expecting kbps)
      qos_cell->cos_config.cos[cos].min_bandwidth = minBwList.bandwidth[cos];
      qos_cell->cos_config.cos[cos].mask |= PTIN_COS_MASK_MIN_BANDWIDTH;
      // Maximum bandwidth (other side expecting kbps)
      qos_cell->cos_config.cos[cos].max_bandwidth = maxBwList.bandwidth[cos];
      qos_cell->cos_config.cos[cos].mask |= PTIN_COS_MASK_MAX_BANDWIDTH;
    } // Run all 8 CoS queues

    qos_cell->cos_config.mask = 0xFF;
  } // Run all ports

  // Return number of cells
  *n_cells = cell_index;

  if (cell_index==0) {
    DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_ERROR, "%s(%d): Null number of elements read",__FUNCTION__,__LINE__);
    return L7_FAILURE;
  }

  DEBUGTRACE (TRACE_MODULE_ETHERNET | TRACE_LAYER_CAPI, TRACE_SEVERITY_INFORMATIONAL, "%s(%d): Success",__FUNCTION__,__LINE__);

  return L7_SUCCESS;
}

// Remove all flows, lags, and static channels
L7_RC_t ptin_msg_reset_alarms(void)
{
  L7_int port;

  for (port=0; port<L7_SYSTEM_N_INTERF; port++) {
    linkStatus[port] = L7_TRUE;
  }
  for (port=0; port<L7_SYSTEM_N_PORTS; port++) {
    lagActiveMembers[port] = L7_TRUE;
  }

  return L7_SUCCESS;
}

// Remove all flows, lags, and static channels
L7_RC_t ptin_msg_set_defaults(L7_uint32 param)
{
  L7_RC_t rc = L7_SUCCESS, status;
  L7_LACPLagInfo lag_info;
  L7_uint8 lag_port;

  // Flows Reset
  if ((status=ptin_flow_reset())!=L7_SUCCESS)
    rc = status;

  param &= 0xFF;

  // Remove all lags
  memset(&lag_info,0x00,sizeof(L7_LACPLagInfo));
  for (lag_port=L7_SYSTEM_N_PORTS; lag_port<L7_SYSTEM_N_INTERF; lag_port++) {

    lag_info.index = lag_port-L7_SYSTEM_N_PORTS+1;

    // Condition to preserve lags
    if (  (param == 0xFF) ||
          ( (param & 0x80) && ((param & 0x7F)==lag_info.index) ) || 
          (!(param & 0x80) && (param==lag_port) ) )
      continue;
    
    if ((status=ptin_msg_LAGDestroy(&lag_info))!=L7_SUCCESS)
      rc = status;
  }

  // Remove all static channels
  if ((status=snoopStaticIpRemoveAll())!=L7_SUCCESS)
    rc = status;

  return rc;
}


static L7_RC_t ptin_Shell_Command(L7_char8 *tty, L7_char8 *type, L7_char8 *cmd);

L7_RC_t ptin_msg_Shell_Command(L7_char8 *str)
{
  L7_char8 *tty, *type, *cmd;
  L7_uint16 i=0;
  L7_uint16 len=strlen(str);

  if (len>200)  len=200;

  // TTY
  tty = str;
  // Validate command
  if ( *tty=='\0' || i>=len )  return(0);

  // TYPE
  // Search for a space
  for ( type=str; i<len && *type!='\0' && *type!=' '; type++,i++ );
  if ( *type==' ' )
  {
    *(type++)='\0';  i++;
  }
  // Validate command
  if ( *type=='\0' || i>=len )  return(0);

  // COMMAND
  // Search for a space
  for ( cmd=type; i<len && *cmd!='\0' && *cmd!=' '; cmd++,i++ );
  if ( *cmd==' ' )
  {
    *(cmd++)='\0';  i++;
  }
  // Validate command
  if ( *cmd=='\0' || i>=len )  return(0);

  return ptin_Shell_Command(tty,type,cmd);
}


static L7_RC_t ptin_Shell_Command(L7_char8 *tty, L7_char8 *type, L7_char8 *cmd)
{
  L7_RC_t   rc=L7_SUCCESS;
  L7_char8 *prevtty=ttyname(1);

  ptin_PitHandler(tty);

  if (strcmp(type,"driv")==0)
  {
    dtlDriverShell(cmd);
  } else if (strcmp(type,"dev")==0)
  {
    if (osapiDevShellExec(cmd)!=0)  rc=L7_FAILURE;
  }

  ptin_PitHandler(prevtty);

  return L7_SUCCESS;
}

