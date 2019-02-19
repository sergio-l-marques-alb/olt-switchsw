#include "ptin_translate_api.h"
#include "dtlinclude.h"
#include "usmdb_util_api.h"
#include "ptin_flow.h"
#include "usmdb_dot3ad_api.h"

#define PTIN_INGRESS   0
#define PTIN_EGRESS    1

// Snooping translations
L7_uint32 snoop_mcast_vlan[4096];       /* UniVlan to MCVlan*/
L7_uint32 snoop_mcast_vlan_inv[4096];   /* MCVlan to UniVlan*/

// Snooping translations
L7_uint32 vlan_xlate_snoop[4096];
L7_uint32 vlan_xlate_dot1q[4096];


// Lag references
L7_uint32 lag_interfaces[L7_SYSTEM_N_LAGS];

L7_uint16 newVlan_xlate_table[L7_SYSTEM_N_INTERF][4096];
L7_uint16 oldVlan_xlate_table[L7_SYSTEM_N_INTERF][4096];

L7_uint16 egress_newVlan_xlate_table[L7_SYSTEM_N_INTERF][4096];
L7_uint16 egress_oldVlan_xlate_table[L7_SYSTEM_N_INTERF][4096];


typedef struct {
  L7_int port;
  L7_uint16 oldVlan;
  L7_uint16 newVlan;
} struct_newVlan;

struct_newVlan vlan_xlate_flush_table[4096];
L7_uint16  vlan_xlate_flush_number=0;

//L7_RC_t ptin_translate_get_realVlans(L7_uint32 intf, L7_uint16 oldvlan, L7_uint16 *realVlans)
//{
//  L7_uint32 unit, slot, port;
//
//  if (usmDbUnitSlotPortGet(intif,&unit,&slot,&port)!=L7_SUCCESS || unit!=1 || slot!=0)
//    return L7_FAILURE;
//
//  return ptin_flow_get_realVlans(port-1, oldvlan, realVlans);
//}


void ptin_vlan_translate_init(void)
{
  L7_uint16 vid;

  memset(vlan_xlate_snoop,0xFF,sizeof(L7_uint16)*4096);
  memset(vlan_xlate_dot1q,0xFF,sizeof(L7_uint16)*4096);

  memset(egress_newVlan_xlate_table,0x00,sizeof(L7_uint16)*4096*(L7_SYSTEM_N_INTERF));
  memset(egress_oldVlan_xlate_table,0x00,sizeof(L7_uint16)*4096*(L7_SYSTEM_N_INTERF));

  memset(newVlan_xlate_table,0x00,sizeof(L7_uint16)*4096*(L7_SYSTEM_N_INTERF));
  memset(oldVlan_xlate_table,0x00,sizeof(L7_uint16)*4096*(L7_SYSTEM_N_INTERF));
  memset(vlan_xlate_flush_table,0x00,sizeof(struct_newVlan)*4096);
  vlan_xlate_flush_number = 0;

  // Initialize snoop_mcast_vlan table in snooping.c file
  for (vid=0; vid<4096; vid++)
  {
    snoop_mcast_vlan[vid]=vid;
    snoop_mcast_vlan_inv[vid]=vid;
  }

  // Clear LAG interfaces table
  memset(lag_interfaces,0x00,sizeof(L7_uint32)*L7_SYSTEM_N_LAGS);
}


L7_RC_t ptin_intif_oldvlan_translate_get(L7_uint32 intif, L7_uint16 *vlan_old, L7_uint16 vlan_new)
{
  L7_int port;

  if (ptin_translate_intf2port(intif,&port)!=L7_SUCCESS)
    return L7_FAILURE;

  return ptin_oldvlan_translate_get(port, vlan_old, vlan_new);
}


L7_RC_t ptin_oldvlan_translate_get(L7_int port, L7_uint16 *vlan_old, L7_uint16 vlan_new)
{
  L7_uint16 vid;

  if (vlan_new>4095 || port<0 || port>=L7_SYSTEM_N_INTERF)  return L7_FAILURE;

  vid = oldVlan_xlate_table[port][vlan_new];
  if (vid==0 || vid>4095)  return L7_FAILURE;

  if (vlan_old!=L7_NULLPTR)  *vlan_old = vid;

  return L7_SUCCESS;
}


L7_RC_t ptin_intif_vlan_translate_get(L7_uint32 intif, L7_uint16 vlan_old, L7_uint16 *vlan_new)
{
  L7_uint32 port;

  if (ptin_translate_intf2port(intif,&port)!=L7_SUCCESS)
    return L7_FAILURE;

  return ptin_vlan_translate_get(port,vlan_old,vlan_new);
}


L7_RC_t ptin_vlan_translate_get(L7_int port, L7_uint16 vlan_old, L7_uint16 *vlan_new)
{
  L7_uint16 vid;

  if (vlan_old>4095 || port<0 || port>=L7_SYSTEM_N_INTERF)  return L7_FAILURE;

  vid = newVlan_xlate_table[port][vlan_old];
  if (vid==0 || vid>4095)  return L7_FAILURE;

  if (vlan_new!=L7_NULLPTR)  *vlan_new = vid;

  return L7_SUCCESS;
}


L7_RC_t ptin_intif_vlan_translate_egress_get(L7_uint32 intif, L7_uint16 vlan_old, L7_uint16 *vlan_new)
{
  L7_int port;

  if (ptin_translate_intf2port(intif,&port)!=L7_SUCCESS)
    return L7_FAILURE;

  return ptin_vlan_translate_egress_get(port,vlan_old,vlan_new);
}


L7_RC_t ptin_vlan_translate_egress_get(L7_int port, L7_uint16 vlan_old, L7_uint16 *vlan_new)
{
  L7_uint16 vid;

  if (vlan_old>4095 || port<0 || port>=L7_SYSTEM_N_INTERF)  return L7_FAILURE;

  vid = egress_newVlan_xlate_table[port][vlan_old];
  if (vid==0 || vid>4095)  return L7_FAILURE;

  if (vlan_new!=L7_NULLPTR)  *vlan_new = vid;

  return L7_SUCCESS;
}


L7_RC_t ptin_vlan_translate_add(L7_int port, L7_uint16 vlan_old, L7_uint16 vlan_new)
{
  L7_uint32 nPorts, i;
  L7_uint32 member_list[L7_SYSTEM_N_PORTS];
  L7_uint8  port_list[L7_SYSTEM_N_PORTS];
  L7_uint32 intf;
  L7_int    p;
  //L7_uint16 vlan;
  L7_RC_t   rc;
  L7_vlan_translation xlate;

  if (port<0 || port>=L7_SYSTEM_N_INTERF || vlan_old>4095 || vlan_new>4095)  {
    return L7_FAILURE;
  }

  if (port<L7_SYSTEM_N_PORTS) {
    xlate.operation = L7_VLAN_XLATE_OPER_ADD_INGRESS_ST;
    xlate.port      = port;
    xlate.vlan_old  = vlan_old;
    xlate.vlan_new  =&vlan_new;
    
    if ((rc=dtlPtinVlanXlate(&xlate))!=L7_SUCCESS)  {
      return rc;
    }
  }
  else {
    // Get interface number
    if (ptin_translate_port2intf(port,&intf)!=L7_SUCCESS || intf==0)  {
      return L7_FAILURE;
    }
    // Get list of ports, of this LAG
    nPorts = L7_SYSTEM_N_PORTS;
    if (usmDbDot3adMemberListGet(1,intf,&nPorts,member_list)!=L7_SUCCESS)  {
      return L7_FAILURE;
    }
    memset(port_list,0x00,sizeof(L7_uint8)*L7_SYSTEM_N_PORTS);
    for (i=0; i<nPorts; i++) {
      if (member_list[i]!=0 &&
          ptin_translate_intf2port(member_list[i],&p)==L7_SUCCESS &&
          p>=0 && p<L7_SYSTEM_N_PORTS)  {
        port_list[p]=L7_TRUE;
      }
    }

    for (i=igmp_firstRootPort; i<L7_SYSTEM_N_PORTS; i++) {

      //rc=ptin_vlan_translate_get(i, vlan_old, &vlan);
      // Validate interface number
      if (port_list[i])  {
        if ((rc=ptin_vlan_translate_add(i, vlan_old, vlan_new))!=L7_SUCCESS)  {
          return rc;
        }
      }
//    else if (rc==L7_SUCCESS && vlan==vlan_new) {
//      if (ptin_vlan_translate_delete(i, vlan_old)!=L7_SUCCESS)  {
//        return L7_FAILURE;
//      }
//    }
      else  {
        continue;
      }
    }
  }

  newVlan_xlate_table[port][vlan_old] = vlan_new;
  oldVlan_xlate_table[port][vlan_new] = vlan_old;

  return L7_SUCCESS;
}

L7_RC_t ptin_vlan_translate_egress_add(L7_int port, L7_uint16 vlan_old, L7_uint16 vlan_new)
{
  L7_uint32 nPorts, i;
  L7_uint32 member_list[L7_SYSTEM_N_PORTS];
  L7_uint8  port_list[L7_SYSTEM_N_PORTS];
  L7_uint32 intf;
  L7_int    p;
  //L7_uint16 vlan;
  L7_RC_t   rc;
  L7_vlan_translation xlate;

  if (port<0 || port>=L7_SYSTEM_N_INTERF || vlan_old>4095 || vlan_new>4095)  return L7_FAILURE;

  if (port<L7_SYSTEM_N_PORTS) {
    xlate.operation = L7_VLAN_XLATE_OPER_ADD_EGRESS_ST;
    xlate.port      = port;
    xlate.vlan_old  = vlan_old;
    xlate.vlan_new  =&vlan_new;
    
    if ((rc=dtlPtinVlanXlate(&xlate))!=L7_SUCCESS)  {
      return rc;
    }
  }
  else {
    // Get interface number
    if (ptin_translate_port2intf(port,&intf)!=L7_SUCCESS || intf==0)  {
      return L7_FAILURE;
    }

    // Get list of ports, of this LAG
    nPorts = L7_SYSTEM_N_PORTS;
    if (usmDbDot3adMemberListGet(1,intf,&nPorts,member_list)!=L7_SUCCESS)  {
      return L7_FAILURE;
    }
    memset(port_list,0x00,sizeof(L7_uint8)*L7_SYSTEM_N_PORTS);
    for (i=0; i<nPorts; i++) {
      if (member_list[i]!=0 &&
          ptin_translate_intf2port(member_list[i],&p)==L7_SUCCESS &&
          p>=0 && p<L7_SYSTEM_N_PORTS)  {
        port_list[p]=L7_TRUE;
      }
    }

    for (i=igmp_firstRootPort; i<L7_SYSTEM_N_PORTS; i++) {

      //rc=ptin_vlan_translate_egress_get(i, vlan_old, &vlan);

      // Validate interface number
      if (port_list[i])  {
        if ((rc=ptin_vlan_translate_egress_add(i, vlan_old, vlan_new))!=L7_SUCCESS)  {
          return rc;
        }
      }
//    else if (rc==L7_SUCCESS && vlan==vlan_new) {
//      if (ptin_vlan_translate_egress_delete(i, vlan_old)!=L7_SUCCESS)  {
//        return L7_FAILURE;
//      }
//    }
      else  {
        continue;
      }
    }
  }

  egress_newVlan_xlate_table[port][vlan_old] = vlan_new;
  egress_oldVlan_xlate_table[port][vlan_new] = vlan_old;

  return L7_SUCCESS;
}

L7_RC_t ptin_vlan_translate_delete(L7_int port, L7_uint16 vlan_old)
{
  L7_uint32 nPorts, i;
  L7_uint32 member_list[L7_MAX_MEMBERS_PER_LAG];
  L7_uint32 intf;
  L7_int    p;
  L7_vlan_translation xlate;
  L7_uint16 vlan_new=0;

  if (port<0 || port>=L7_SYSTEM_N_INTERF || vlan_old>4095)  return L7_FAILURE;

  if (ptin_vlan_translate_get(port,vlan_old,&vlan_new)!=L7_SUCCESS)  vlan_new=0xFFFF;

  if (port<L7_SYSTEM_N_PORTS) {
    xlate.operation = L7_VLAN_XLATE_OPER_DEL_INGRESS_ST;
    xlate.port      = port;
    xlate.vlan_old  = vlan_old;
    xlate.vlan_new  = &vlan_new;
  
    dtlPtinVlanXlate(&xlate);
  }
  else {
    // Get interface number
    if (ptin_translate_port2intf(port,&intf)!=L7_SUCCESS || intf==0)
      return L7_FAILURE;

    // Get list of ports, of this LAG
    nPorts = L7_SYSTEM_N_PORTS;
    if (usmDbDot3adMemberListGet(1,intf,&nPorts,member_list)!=L7_SUCCESS)  {
      return L7_FAILURE;
    }
    for (i=0; i<nPorts; i++) {
      // Validate interface number
      if (member_list[i]==0 || ptin_translate_intf2port(member_list[i],&p)!=L7_SUCCESS || p<0 || p>=L7_SYSTEM_N_PORTS)
        continue;
      // Add translation entry to this port
      if (ptin_vlan_translate_delete(p,vlan_old)!=L7_SUCCESS)
        return L7_FAILURE;
    }
  }

  if (vlan_new<4096 && vlan_xlate_flush_number<4096)  {
    vlan_xlate_flush_table[vlan_xlate_flush_number].port    = port;
    vlan_xlate_flush_table[vlan_xlate_flush_number].oldVlan = vlan_old;
    vlan_xlate_flush_table[vlan_xlate_flush_number].newVlan = vlan_new;
    vlan_xlate_flush_number++;
  }

  return L7_SUCCESS;
}

void ptin_vlan_translate_delete_flush(void)
{
  L7_int port;
  L7_uint16 index, newVlan, oldVlan;

  if (vlan_xlate_flush_number>4096)  vlan_xlate_flush_number=4096;

  for (index=0; index<vlan_xlate_flush_number; index++) {
    if ((port=vlan_xlate_flush_table[index].port)<L7_SYSTEM_N_INTERF) {
      if ((oldVlan=vlan_xlate_flush_table[index].oldVlan)<4096)  {
        newVlan_xlate_table[port][oldVlan]=0;
      }
      if ((newVlan=vlan_xlate_flush_table[index].newVlan)<4096)
      {
        oldVlan_xlate_table[port][newVlan]=0;
      }
    }
  }
  vlan_xlate_flush_number = 0;
}


L7_RC_t ptin_vlan_translate_egress_delete(L7_int port, L7_uint16 vlan_old)
{
  L7_uint32 nPorts, i;
  L7_uint32 member_list[L7_MAX_MEMBERS_PER_LAG];
  L7_uint32 intf;
  L7_int    p;
  L7_vlan_translation xlate;
  L7_uint16 vlan_new=0;

  if (port<0 || port>=L7_SYSTEM_N_INTERF || vlan_old>4095)  return L7_FAILURE;

  if (ptin_vlan_translate_get(port,vlan_old,&vlan_new)!=L7_SUCCESS)  vlan_new=0xFFFF;

  if (port<L7_SYSTEM_N_PORTS) {
    xlate.operation = L7_VLAN_XLATE_OPER_DEL_EGRESS_ST;
    xlate.port      = port;
    xlate.vlan_old  = vlan_old;
    xlate.vlan_new  = &vlan_new;
  
    dtlPtinVlanXlate(&xlate);
  }
  else {
    // Get interface number
    if (ptin_translate_port2intf(port,&intf)!=L7_SUCCESS || intf==0)
      return L7_FAILURE;

    // Get list of ports, of this LAG
    nPorts = L7_SYSTEM_N_PORTS;
    if (usmDbDot3adMemberListGet(1,intf,&nPorts,member_list)!=L7_SUCCESS)  {
      return L7_FAILURE;
    }
    for (i=0; i<nPorts; i++) {
      // Validate interface number
      if (member_list[i]==0 || ptin_translate_intf2port(member_list[i],&p)!=L7_SUCCESS || p<0 || p>=L7_SYSTEM_N_PORTS)
        continue;
      // Get reference to physical port
      if (ptin_vlan_translate_egress_delete(p, vlan_old)!=L7_SUCCESS)
        return L7_FAILURE;
    }
  }

  if (vlan_old<4096)   egress_newVlan_xlate_table[port][vlan_old]=0;
  if (vlan_new<4096)   egress_oldVlan_xlate_table[port][vlan_new]=0;

  return L7_SUCCESS;
}

//static L7_RC_t ptin_vlan_translate_egress_delete_all(L7_int port, L7_uint16 vlan_old)
//{
//  L7_vlan_translation xlate;
//
//  xlate.operation = L7_VLAN_XLATE_OPER_DELETE_ALL;
//  xlate.port      = L7_NULL;
//  xlate.vlan_old  = L7_NULL;
//  xlate.vlan_new  = L7_NULL;
//
//  return dtlPtinVlanXlate(&xlate);
//}


// PTin added
L7_RC_t usmDbSnoopQuerierVlanXlateSet(L7_uint32 vlanId, L7_uint32 xlateVlanId)
{
  // Validate arguments
  if (vlanId==0 || vlanId>4095 ||
      xlateVlanId==0 || xlateVlanId>4095)
    return L7_FAILURE;

  snoop_mcast_vlan[vlanId] = xlateVlanId;
  snoop_mcast_vlan_inv[xlateVlanId] = vlanId;

  printf("%s(%d) snoop_mcast_vlan[%u]=%u, snoop_mcast_vlan_inv[%u]=%u\r\n", __FUNCTION__, __LINE__,
         vlanId,snoop_mcast_vlan[vlanId],xlateVlanId,snoop_mcast_vlan_inv[xlateVlanId]);
  
  return L7_SUCCESS;
}

// PTin added
L7_RC_t usmDbSnoopQuerierVlanXlateGet(L7_uint32 vlanId, L7_uint32 *xlateVlanId)
{
  if (vlanId>4095)  return L7_FAILURE;

  if (xlateVlanId!=L7_NULLPTR)
  {
    *xlateVlanId = snoop_mcast_vlan[vlanId];
  }

  return L7_SUCCESS;
}


// PTin added
L7_RC_t ptin_translate_dhcpVlanGet(L7_uint32 src_intf, L7_uint16 src_vlanId, L7_uint32 dst_intf, L7_uint16 *dst_vlanId)
{
  L7_int src_port, dst_port;
  L7_uint16 new_vlan;
  L7_uint16 flowId;

  // Validate arguments
  if (src_vlanId>4095)
    return L7_FAILURE;

  if (ptin_translate_intf2port(src_intf, &src_port)!=L7_SUCCESS || src_port<0 || src_port>=L7_SYSTEM_N_INTERF ||
      ptin_translate_intf2port(dst_intf, &dst_port)!=L7_SUCCESS || dst_port<0 || dst_port>=L7_SYSTEM_N_INTERF)
    return L7_FAILURE;

  // Get flow_id reference, related to this vlan and source port
  if (ptin_flow_getId(src_port, src_vlanId, &flowId)!=L7_SUCCESS)
    return L7_FAILURE;

  // Get the vlan associated to the destination port
  if (ptin_flow_getVlan(dst_port, flowId, &new_vlan)!=L7_SUCCESS) 
    return L7_FAILURE;

  // Validate new vlan
  if (new_vlan<=1 && new_vlan>4095) 
    return L7_FAILURE;

  // ... and return it
  if (dst_vlanId!=L7_NULLPTR)
  {
    *dst_vlanId = new_vlan;
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_lag_memberList_get(L7_int port, L7_uint32 *port_pbmp)
{
  L7_uint32 intf;
  L7_int    p;
  L7_uint32 nPorts, i, pbmp;
  L7_uint32 member_list[L7_SYSTEM_N_PORTS];

  // Validate port
  if (port<0 || port>=L7_SYSTEM_N_INTERF)
    return L7_FAILURE;

  // Physical port? Return the port itself
  if (port<L7_SYSTEM_N_PORTS)  {
    if (port_pbmp!=L7_NULLPTR) *port_pbmp = (L7_uint32) 1<<port;
    return L7_SUCCESS;
  }
  // Here, we are dealing with virtual ports
  intf = lag_interfaces[port-L7_SYSTEM_N_PORTS];
  // Is defined this lag? If not, error
  if (intf==0)  {
    return L7_FAILURE;
  }

  // Get list of ports, of this LAG
  nPorts = L7_SYSTEM_N_PORTS;
  if (usmDbDot3adMemberListGet(1,intf,&nPorts,member_list)!=L7_SUCCESS)  {
    return L7_FAILURE;
  }

  pbmp = 0;
  for (i=0; i<nPorts; i++) {
    // Validate intf reference
    if (member_list[i]==0 || ptin_translate_intf2port(member_list[i],&p)!=L7_SUCCESS || p<0 || p>=L7_SYSTEM_N_PORTS)
      continue;
    // Mark port bit
    pbmp |= (L7_uint32) 1<<p;
  }
  // Return
  if (port_pbmp!=L7_NULLPTR)  *port_pbmp=pbmp;

  return L7_SUCCESS;
}

L7_RC_t ptin_lag_activeMemberList_get(L7_int port, L7_uint32 *port_pbmp)
{
  L7_uint32 intf;
  L7_int    p;
  L7_uint32 nPorts, i, pbmp;
  L7_uint32 member_list[L7_SYSTEM_N_PORTS];

  // Validate port
  if (port<0 || port>=L7_SYSTEM_N_INTERF)
    return L7_FAILURE;

  // Physical port? Return the port itself
  if (port<L7_SYSTEM_N_PORTS)  {
    if (port_pbmp!=L7_NULLPTR) *port_pbmp = (L7_uint32) 1<<port;
    return L7_SUCCESS;
  }
  // Here, we are dealing with virtual ports
  intf = lag_interfaces[port-L7_SYSTEM_N_PORTS];
  // Is defined this lag? If not, error
  if (intf==0)  {
    return L7_FAILURE;
  }

  // Get list of ports, of this LAG
  nPorts = L7_SYSTEM_N_PORTS;
  if (usmDbDot3adActiveMemberListGet(1,intf,&nPorts,member_list)!=L7_SUCCESS)  {
    return L7_FAILURE;
  }

  pbmp = 0;
  for (i=0; i<nPorts; i++) {
    // Validate intf reference
    if (member_list[i]==0 || ptin_translate_intf2port(member_list[i],&p)!=L7_SUCCESS || p<0 || p>=L7_SYSTEM_N_PORTS)
      continue;
    // Mark port bit
    pbmp |= (L7_uint32) 1<<p;
  }
  // Return
  if (port_pbmp!=L7_NULLPTR)  *port_pbmp=pbmp;

  return L7_SUCCESS;
}


L7_RC_t ptin_translate_intf2port(L7_uint32 intf, L7_int *port_ref)
{
  L7_uint8 i;
  L7_uint32 unit, slot, port;

  if (intf==0)  return L7_FAILURE;

  // Get reference to slot and port
  if (usmDbUnitSlotPortGet(intf,&unit,&slot,&port)!=L7_SUCCESS || unit!=1 || (slot!=0 && slot!=1) || port==0 || port>L7_SYSTEM_N_PORTS)
    return L7_FAILURE;
  
  // Physical port
  if (slot==0)  {
    port--;
  }
  // Lag port
  else  {
    // Check if this LAG exists and if it is configured
    if (!usmDbDot3adValidIntfCheck(1,intf) || !usmDbLagIsConfigured(1,intf))
      return L7_FAILURE;

    // Find lag id
    for (i=0; i<L7_SYSTEM_N_LAGS && lag_interfaces[i]!=intf; i++);
    // If not found, error
    if (i>=L7_SYSTEM_N_LAGS)
      return L7_FAILURE;
    // Found port reference
    port = L7_SYSTEM_N_PORTS + i;
  }

  // Value to return
  if (port_ref!=L7_NULLPTR)  *port_ref = port;

  return L7_SUCCESS;
}

L7_RC_t ptin_translate_port2intf(L7_int port, L7_uint32 *intf_ref)
{
  L7_uint32 intf;

  if (port<0 || port>=L7_SYSTEM_N_INTERF)  {
    return L7_FAILURE;
  }

  // LAG port
  if (port>=L7_SYSTEM_N_PORTS)  {
    intf = lag_interfaces[port-L7_SYSTEM_N_PORTS];
    if (intf==0)  {
      return L7_FAILURE;
    }
  }
  // Physical port
  else {
    if (usmDbIntIfNumFromUSPGet(1,0,port+1,&intf)!=L7_SUCCESS || intf==0)
      return L7_FAILURE;
  }

  // interface values to return
  if (intf_ref!=L7_NULLPTR)  *intf_ref = intf;

  return L7_SUCCESS;
}

L7_RC_t ptin_translate_LagPortIntfSet(L7_int port, L7_uint32 intf_ref)
{
  if (port<0 || port>=L7_SYSTEM_N_INTERF || intf_ref==0)
    return L7_FAILURE;

  if (port<L7_SYSTEM_N_PORTS)
    return L7_SUCCESS;

  lag_interfaces[port-L7_SYSTEM_N_PORTS] = intf_ref;

  return L7_SUCCESS;
}

L7_RC_t ptin_translate_LagPortIntfClear(L7_int port)
{
  if (port<0 || port>=L7_SYSTEM_N_INTERF)
    return L7_FAILURE;

  if (port<L7_SYSTEM_N_PORTS)
    return L7_SUCCESS;

  lag_interfaces[port-L7_SYSTEM_N_PORTS] = 0;

  return L7_SUCCESS;
}



L7_RC_t ptin_mcflow_translate_save(L7_uint16 vlan_snoop, L7_uint16 vlan_dot1q)
{
  if (vlan_dot1q>4095 || vlan_snoop>4095)
    return L7_FAILURE;

  vlan_xlate_snoop[vlan_dot1q]=vlan_snoop;
  vlan_xlate_dot1q[vlan_snoop]=vlan_dot1q;

  return L7_SUCCESS;
}

L7_RC_t ptin_mcflow_translate_clear(L7_uint16 vlan_snoop)
{
  L7_uint16 vlan_dot1q;

  if (vlan_snoop>4095)  return L7_FAILURE;

  vlan_dot1q = vlan_xlate_dot1q[vlan_snoop];
  if (vlan_dot1q<4096)  {
    vlan_xlate_snoop[vlan_dot1q]=(L7_uint16)-1;
  }
  vlan_xlate_dot1q[vlan_snoop]=(L7_uint16)-1;

  return L7_SUCCESS;
}

L7_RC_t ptin_mcflow_translate_dot1qvlan_get(L7_uint16 vlan_snoop, L7_uint16 *vlan_dot1q)
{
  L7_uint16 vlanId;

  if (vlan_snoop>4095)
    return L7_FAILURE;

  vlanId = vlan_xlate_dot1q[vlan_snoop];
  if (vlanId>4095)  vlanId = vlan_snoop;

  if (vlan_dot1q!=L7_NULLPTR)  {
    *vlan_dot1q = vlanId;
  }

  return L7_SUCCESS;
}

L7_RC_t ptin_mcflow_translate_snoopvlan_get(L7_uint16 vlan_dot1q, L7_uint16 *vlan_snoop)
{
  L7_uint16 vlanId;

  if (vlan_dot1q>4095)
    return L7_FAILURE;

  vlanId = vlan_xlate_snoop[vlan_dot1q];
  if (vlanId>4095)  vlanId = vlan_dot1q;

  if (vlan_snoop!=L7_NULLPTR)  {
    *vlan_snoop = vlanId;
  }

  return L7_SUCCESS;
}

