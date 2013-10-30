/*
 * This module is used to test functionalities
 * It is a testing module, whose functions are called using devshell
*/

#include <bcm/vlan.h>
#include <bcm/error.h>
#include <stdio.h>

#include "l7_common.h"
#include "ptin_hapi.h"
#include "broad_policy.h"
#include <bcm_int/esw_dispatch.h>

#include "hpc_db.h"
#include "dapi_db.h"
#include "bcmx/vlan.h"
#include "logger.h"

// Ingress Translations (single tagged packets)

int ptin_vlan_single_translate_action_add(int port, bcm_vlan_t oVlanId, bcm_vlan_t newOVlanId)
{
  int error;
  bcm_port_t  bcm_port;
  bcm_gport_t gport;
  bcm_vlan_translate_key_t keyType;
  bcm_vlan_action_set_t action;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  // Calculate gport
  BCM_GPORT_LOCAL_SET(gport,bcm_port);

  keyType = bcmVlanTranslateKeyPortOuter;

  bcm_vlan_action_set_t_init(&action);
  action.dt_outer      = bcmVlanActionReplace;
  action.dt_inner      = bcmVlanActionNone;
  action.dt_outer_prio = bcmVlanActionNone;
  action.dt_inner_prio = bcmVlanActionNone;

  action.ot_outer      = bcmVlanActionReplace;
  action.ot_inner      = bcmVlanActionNone;
  action.ot_outer_prio = bcmVlanActionNone;

  action.new_outer_vlan = newOVlanId;

  error = bcm_vlan_translate_action_add(0,gport,keyType,oVlanId,0,&action);

  printf("bcm_vlan_translate_action_add(0,%u[%d],%u,%u,%u,&action) => %d (\"%s\")\r\n",gport,bcm_port,keyType,oVlanId,0,error,bcm_errmsg(error));

  return error;
}

int ptin_vlan_single_translate_action_delete(int port, bcm_vlan_t oVlanId)
{
  int error;
  bcm_port_t  bcm_port;
  bcm_gport_t gport;
  bcm_vlan_translate_key_t keyType;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  // Calculate gport
  BCM_GPORT_LOCAL_SET(gport,bcm_port);

  keyType = bcmVlanTranslateKeyPortOuter;

  error = bcm_vlan_translate_action_delete(0,gport,keyType,oVlanId,0);

  printf("bcm_vlan_translate_action_delete(0,%u[%d],%u,%u,%u) => %d (\"%s\")\r\n",gport,bcm_port,keyType,oVlanId,0,error,bcm_errmsg(error));

  return error;
}

// Ingress Translations (double tagged packets)

int ptin_vlan_double_translate_action_add(int port, bcm_vlan_t oVlanId, bcm_vlan_t iVlanId, bcm_vlan_t newOVlanId)
{
  int error;
  bcm_port_t  bcm_port;
  bcm_gport_t gport;
  bcm_vlan_translate_key_t keyType;
  bcm_vlan_action_set_t action;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  // Calculate gport
  BCM_GPORT_LOCAL_SET(gport,bcm_port);

  keyType = bcmVlanTranslateKeyPortDouble;

  bcm_vlan_action_set_t_init(&action);
  action.dt_outer      = bcmVlanActionReplace;
  action.dt_inner      = bcmVlanActionNone;
  action.dt_outer_prio = bcmVlanActionNone;
  action.dt_inner_prio = bcmVlanActionNone;

  action.ot_outer      = bcmVlanActionReplace;
  action.ot_inner      = bcmVlanActionNone;
  action.ot_outer_prio = bcmVlanActionNone;

  action.new_outer_vlan = newOVlanId;

  error = bcm_vlan_translate_action_add(0,gport,keyType,oVlanId,iVlanId,&action);

  printf("bcm_vlan_translate_action_add(0,%u[%d],%u,%u,%u,&action) => %d (\"%s\")\r\n",gport,bcm_port,keyType,oVlanId,iVlanId,error,bcm_errmsg(error));

  return error;
}

int ptin_vlan_double_translate_action_delete(int port, bcm_vlan_t oVlanId, bcm_vlan_t iVlanId)
{
  int error;
  bcm_port_t  bcm_port;
  bcm_gport_t gport;
  bcm_vlan_translate_key_t keyType;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  // Calculate gport
  BCM_GPORT_LOCAL_SET(gport,bcm_port);

  keyType = bcmVlanTranslateKeyPortDouble;

  error = bcm_vlan_translate_action_delete(0,gport,keyType,oVlanId,iVlanId);

  printf("bcm_vlan_translate_action_delete(0,%u[%d],%u,%u,%u) => %d (\"%s\")\r\n",gport,bcm_port,keyType,oVlanId,iVlanId,error,bcm_errmsg(error));

  return error;
}

// Egress translations (single+double tagged packets)

int ptin_vlan_egress_translate_action_add(int port, int classId, bcm_vlan_t oVlanId, bcm_vlan_t iVlanId, bcm_vlan_t newOVlanId)
{
  int error;
  bcm_port_t bcm_port;
  bcm_vlan_action_set_t action;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS || classId<=0 || hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)
  {
    printf("Port/Class is invalid\r\n");
    return -1;
  }

  // Class
  if (bcm_port_class_set(0,bcm_port,bcmPortClassVlanTranslateEgress,classId)!=BCM_E_NONE)
  {
    printf("Error setting class %d with port %d\r\n",classId,port);
    return -1;
  }

  bcm_vlan_action_set_t_init(&action);
  action.dt_outer      = bcmVlanActionReplace;
  action.dt_inner      = bcmVlanActionNone;
  action.dt_outer_prio = bcmVlanActionNone;
  action.dt_inner_prio = bcmVlanActionNone;

  action.ot_outer      = bcmVlanActionReplace;
  action.ot_inner      = bcmVlanActionNone;
  action.ot_outer_prio = bcmVlanActionNone;

  action.new_outer_vlan = newOVlanId;

  error = bcm_vlan_translate_egress_action_add(0,classId,oVlanId,iVlanId,&action);

  printf("bcm_vlan_translate_egress_action_add(0,%d,%u,%u,&action) => %d (\"%s\")\r\n",classId,oVlanId,iVlanId,error,bcm_errmsg(error));

  return error;
}

int ptin_vlan_egress_translate_action_delete(int classId, bcm_vlan_t oVlanId, bcm_vlan_t iVlanId)
{
  int error;

  // Validate port, and get bcm_port reference
  if (classId<=0)
  {
    printf("ClassId is invalid\r\n");
    return -1;
  }

  error = bcm_vlan_translate_egress_action_delete(0,classId,oVlanId,iVlanId);

  printf("bcm_vlan_translate_egress_action_delete(0,%d,%u,%u) => %d (\"%s\")\r\n",classId,oVlanId,iVlanId,error,bcm_errmsg(error));

  return error;
}

// Cross connections (single tagged packets)

int ptin_vlan_single_cross_connect_add(int port1, int port2, bcm_vlan_t oVlanId)
{
  int error;
  bcm_port_t  bcm_port1, bcm_port2;
  bcm_gport_t gport1, gport2;

  // Validate port, and get bcm_port reference
  if (port1>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port1,&bcm_port1)!=L7_SUCCESS ||
      port2>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port2,&bcm_port2)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  // Calculate gport
  BCM_GPORT_LOCAL_SET(gport1,bcm_port1);
  BCM_GPORT_LOCAL_SET(gport2,bcm_port2);

  error = bcm_vlan_cross_connect_add(0,oVlanId,BCM_VLAN_INVALID,gport1,gport2);

  printf("bcm_vlan_cross_connect_add(0,%u,%u,%u[%d],%u[%d]) => %d (\"%s\")\r\n",oVlanId,BCM_VLAN_INVALID,gport1,bcm_port1,gport2,bcm_port2,error,bcm_errmsg(error));

  return error;
}

int ptin_vlan_single_cross_connect_delete(bcm_vlan_t oVlanId)
{
  int error;

  error = bcm_vlan_cross_connect_delete(0,oVlanId,BCM_VLAN_INVALID);

  printf("bcm_vlan_cross_connect_delete(0,%u,%u) => %d (\"%s\")\r\n",oVlanId,BCM_VLAN_INVALID,error,bcm_errmsg(error));

  return error;
}

// Cross connections (double tagged packets)

int ptin_vlan_double_cross_connect_add(int port1, int port2, bcm_vlan_t oVlanId, bcm_vlan_t iVlanId)
{
  int error;
  bcm_port_t  bcm_port1, bcm_port2;
  bcm_gport_t gport1, gport2;

  // Validate port, and get bcm_port reference
  if (port1>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port1,&bcm_port1)!=L7_SUCCESS ||
      port2>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port2,&bcm_port2)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  // Calculate gport
  BCM_GPORT_LOCAL_SET(gport1,bcm_port1);
  BCM_GPORT_LOCAL_SET(gport2,bcm_port2);

  error = bcm_vlan_cross_connect_add(0,oVlanId,iVlanId,gport1,gport2);

  printf("bcm_vlan_cross_connect_add(0,%u,%u,%u[%d],%u[%d]) => %d (\"%s\")\r\n",oVlanId,iVlanId,gport1,bcm_port1,gport2,bcm_port2,error,bcm_errmsg(error));

  return error;
}

int ptin_vlan_double_cross_connect_delete(bcm_vlan_t oVlanId, bcm_vlan_t iVlanId)
{
  int error;

  error = bcm_vlan_cross_connect_delete(0,oVlanId,iVlanId);

  printf("bcm_vlan_cross_connect_delete(0,%u,%u) => %d (\"%s\")\r\n",oVlanId,iVlanId,error,bcm_errmsg(error));

  return error;
}


int ptin_switch_control_set(bcm_switch_control_t type, int arg)
{
  int error;

  error = bcm_switch_control_set(0,type,arg);

  printf("bcm_switch_control_set(0,%d,%d) => %d (\"%s\")\r\n",type,arg,error,bcm_errmsg(error));

  return error;
}


int ptin_switch_control_get(bcm_switch_control_t type)
{
  int error;
  int arg;

  error = bcm_switch_control_get(0,type,&arg);

  printf("bcm_switch_control_get(0,%d,&arg) => %d (\"%s\") arg=%d\r\n",type,error,bcm_errmsg(error),arg);

  return error;
}


int ptin_switch_control_port_set(int port, bcm_switch_control_t type, int arg)
{
  int error;
  bcm_port_t  bcm_port;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_switch_control_port_set(0,bcm_port,type,arg);

  printf("bcm_switch_control_port_set(0,%d,%d,%d) => %d (\"%s\")\r\n",bcm_port,type,arg,error,bcm_errmsg(error));

  return error;
}


int ptin_switch_control_port_get(int port, bcm_switch_control_t type)
{
  int error;
  bcm_port_t  bcm_port;
  int arg;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_switch_control_port_get(0,bcm_port,type,&arg);

  printf("bcm_switch_control_port_get(0,%d,%d,&arg) => %d (\"%s\") arg=%d\r\n",bcm_port,type,error,bcm_errmsg(error),arg);

  return error;
}


int ptin_port_control_set(int port, bcm_port_control_t type, int arg)
{
  int error;
  bcm_port_t  bcm_port;

  #if 0
  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }
  #else
  bcm_port = port;
  #endif

  error = bcm_port_control_set(0,bcm_port,type,arg);

  printf("bcm_port_control_set(0,%d,%d,%d) => %d (\"%s\")\r\n",bcm_port,type,arg,error,bcm_errmsg(error));

  return error;
}


int ptin_port_control_get(int port, bcm_port_control_t type)
{
  int error;
  bcm_port_t  bcm_port;
  int arg;

  #if 0
  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }
  #else
  bcm_port = port;
  #endif

  error = bcm_port_control_get(0,bcm_port,type,&arg);

  printf("bcm_port_control_get(0,%d,%d,&arg) => %d (\"%s\") arg=%d\r\n",bcm_port,type,error,bcm_errmsg(error),arg);

  return error;
}


// Control Port settings (to allow ingress translations)

int ptin_vlan_control_port_set(int port, bcm_vlan_control_port_t type, int arg)
{
  int error;
  bcm_port_t  bcm_port;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_vlan_control_port_set(0,bcm_port,type,arg);

  printf("bcm_vlan_control_port_set(0,%d,%d,%d) => %d (\"%s\")\r\n",bcm_port,type,arg,error,bcm_errmsg(error));

  return error;
}


int ptin_vlan_control_port_get(int port, bcm_vlan_control_port_t type)
{
  int error;
  bcm_port_t  bcm_port;
  int arg;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_vlan_control_port_get(0,bcm_port,type,&arg);

  printf("bcm_vlan_control_port_get(0,%d,%d,&arg) => %d (\"%s\") arg=%d\r\n",bcm_port,type,error,bcm_errmsg(error),arg);

  return error;
}

// Control Vlan settings (to allow modifiy Learning on a per-vlan basis and single/double cross-connections)

int ptin_vlan_control_vlan_fwdMode_set(bcm_vlan_t vlanId, bcm_vlan_forward_t fwd_mode, uint32 learn)
{
  int error;
  bcm_vlan_control_vlan_t control;

  bcm_vlan_control_vlan_t_init(&control);

  if ((error=bcm_vlan_control_vlan_get(0,vlanId,&control))!=BCM_E_NONE)
  {
    printf("Error getting vlan control structure! error=%d (%s)\r\n",error,bcm_errmsg(error));
    return -1;
  }

  // Learn on this vlan
  if (!learn)
  {
    control.flags |= BCM_VLAN_LEARN_DISABLE;
  }
  else
  {
    control.flags &= ~((uint32) BCM_VLAN_LEARN_DISABLE);
  }
  // Forwarding mode
  control.forwarding_mode = fwd_mode;

  error = bcm_vlan_control_vlan_set(0,vlanId,control);

  printf("bcm_vlan_control_vlan_set(0,%d,&control) => %d (\"%s\")\r\n",vlanId,error,bcm_errmsg(error));

  return error;
}

int ptin_vlan_control_vlan_fwdVlan_set(bcm_vlan_t vlanId, bcm_vlan_t fwdVlanId)
{
  int error;
  bcm_vlan_control_vlan_t control;

  bcm_vlan_control_vlan_t_init(&control);

  if ((error=bcm_vlan_control_vlan_get(0,vlanId,&control))!=BCM_E_NONE)
  {
    printf("Error getting vlan control structure! error=%d (%s)\r\n",error,bcm_errmsg(error));
    return -1;
  }

  // Forwarding mode
  control.forwarding_vlan = fwdVlanId;
  control.outer_tpid = 0x8100;

  error = bcm_vlan_control_vlan_set(0,vlanId,control);

  printf("bcm_vlan_control_vlan_set(0,%d,&control) => %d (\"%s\")\r\n",vlanId,error,bcm_errmsg(error));

  return error;
}

int ptin_vlan_control_vlan_get(bcm_vlan_t vlanId)
{
  int error;
  bcm_vlan_control_vlan_t control;

  bcm_vlan_control_vlan_t_init(&control);

  if ((error=bcm_vlan_control_vlan_get(0,vlanId,&control))!=BCM_E_NONE)
  {
    printf("Error getting vlan control structure! error=%d (%s)\r\n",error,bcm_errmsg(error));
    return -1;
  }

  printf("flags=%u\r\n",control.flags);
  printf("Forwarding_vlan=%u\r\n",control.forwarding_vlan);
  printf("TPID=0x%04X\r\n",control.outer_tpid);
  printf("Fowarding_mode =%u\r\n",control.forwarding_mode);

  return 0;
}

int ptin_L2StationMove_to_CPU(void)
{
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId  = BROAD_POLICY_RULE_INVALID;
  BROAD_METER_ENTRY_t meterInfo;
  BROAD_POLICY_TYPE_t policyType = BROAD_POLICY_TYPE_SYSTEM;
  //uint8               L2StationMove = 1;
  //uint8               mask[6]  = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  L7_RC_t             result;

  // Rate limiter
  meterInfo.cir       = 64;
  meterInfo.cbs       = 64;
  meterInfo.pir       = 64;
  meterInfo.pbs       = 64;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;

  if ((result=hapiBroadPolicyCreate(policyType))!=L7_SUCCESS)
    return result;

  hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  //hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_L2_STATION_MOVE, (uint8 *)&L2StationMove, (uint8 *) mask);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_LOW_PRIORITY_COS, 0, 0);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);

  result=hapiBroadPolicyCommit(&policyId);

  printf("%s(%d) Rate limiter for L2StationMove: policyId=%d (result=%u)\r\n",__FUNCTION__,__LINE__,policyId,result);

  return result;
}

// Contadores 'a VLAN: feature unavailable!

int ptin_vlan_stat_get(bcm_vlan_t vlanId, bcm_cos_t cos, bcm_vlan_stat_t stat)
{
  int error;
  uint64 val;

  error = bcm_vlan_stat_get(0,vlanId,cos,stat,&val);

  printf("bcm_vlan_stat_get => %d (\"%s\") val=%llu\r\n",error,bcm_errmsg(error),val);

  return error;
}

// Perfis 'a VLAN: feature unavailable!

int ptin_vlan_policer_policer_set(int port, bcm_vlan_t vlanId, uint32 cir, uint32 eir)
{
  int error;
  bcm_port_t  bcm_port;
  bcm_policer_t policer_id;
  bcm_policer_config_t policer_cfg;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  bcm_policer_config_t_init(&policer_cfg);
  policer_cfg.flags       = 0; //BCM_POLICER_DROP_RED;
  policer_cfg.mode        = bcmPolicerModeTrTcm;
  policer_cfg.ckbits_sec  = cir*1000;
  policer_cfg.ckbits_burst= 100;
  policer_cfg.pkbits_sec  = (cir+eir)*1000;
  policer_cfg.pkbits_burst= 100;

  if ((error=bcm_policer_create(0,&policer_cfg,&policer_id))!=BCM_E_NONE || policer_id<=0)
  {
    printf("Error creating policer: error=%d(\"%s\") policer_id=%d\r\n",error,bcm_errmsg(error),policer_id);
  }

  error = bcm_vlan_port_policer_set(0,vlanId,bcm_port,policer_id);

  printf("bcm_vlan_port_policer_set(0,%d,%d,%d) => %d (\"%s\")\r\n",vlanId,bcm_port,policer_id,error,bcm_errmsg(error));

  return error;
}


int ptin_vp_gpon(L7_uint32 pon_port, L7_uint32 network_port, L7_int s_vid, L7_int c_vid)
{
  int unit = 0;
  bcm_gport_t network_gport;
  int gemid[] = {101, 102, 103};
  int cvid[]  = {25, 35, 45};
  int i;
  bcm_error_t error;

  /* enable L3 egress mode... needed for the virtual port APIs to work */
  if ((error=bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1)) != BCM_E_NONE)
  {
    printf("Error setting on bcmSwitchL3EgressModecreating policer: error=%d (\"%s\")\r\n",error,bcm_errmsg(error));
    return error;
  }

  /* create the virtual ports */
  bcm_vlan_port_t vlan_port[3];
  bcm_vlan_port_t_init(&vlan_port[0]);
  bcm_vlan_port_t_init(&vlan_port[1]);
  bcm_vlan_port_t_init(&vlan_port[2]);

  /* in direction PON -> network, match on stacked VLAN, translate to client ID on ingress */
  for (i = 0; i < 3; i++) {
      vlan_port[i].flags = BCM_VLAN_PORT_INNER_VLAN_ADD | BCM_VLAN_PORT_INNER_VLAN_REPLACE;
      vlan_port[i].match_vlan = gemid[i];
      vlan_port[i].match_inner_vlan = cvid[i];
      vlan_port[i].criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
      vlan_port[i].egress_vlan = s_vid;
      vlan_port[i].egress_inner_vlan = c_vid;
      BCM_GPORT_LOCAL_SET(vlan_port[i].port, pon_port);

      if ((error=bcm_vlan_port_create(unit, &vlan_port[i]))!=BCM_E_NONE)
      {
        printf("Error with bcm_vlan_port_create: error=%d (\"%s\")\r\n", error, bcm_errmsg(error));
        return error;
      }

      printf("Vlan idx %d created!\r\n",i);
  }

  /* create egress translation entries for virtual ports to do VLAN tag manipulation 
   * i.e. client -> gem_id + some_c_vlan */
  bcm_vlan_action_set_t action;
  for (i = 0; i < 3; i++) {
      bcm_vlan_action_set_t_init(&action);
      
      /* for outer tagged packet => outer tag replaced with gem_id */
      action.ot_outer = bcmVlanActionReplace;
      action.dt_outer = bcmVlanActionReplace;
      action.new_outer_vlan = gemid[i];
      
      /* for outer tagged packet => inner tag added with cvid */
      action.ot_inner = bcmVlanActionAdd;
      action.dt_inner = bcmVlanActionReplace;
      action.new_inner_vlan = cvid[i];
      
      if ((error=bcm_vlan_translate_egress_action_add(unit, vlan_port[i].vlan_port_id, s_vid, 0, &action))!=BCM_E_NONE)
      {
        printf("Error with bcm_vlan_translate_egress_action_add(%d, %d, %d, %d, &action): error=%d (\"%s\")\r\n",
               unit, vlan_port[i].vlan_port_id, s_vid, 0, error, bcm_errmsg(error));
        return error;
      }
  }

  /* create multicast group, and add virtual ports to it */
  bcm_multicast_t mcast_group;
  bcm_multicast_t encap_id;

  if ((error=bcm_multicast_create(unit, BCM_MULTICAST_TYPE_VLAN, &mcast_group))!=BCM_E_NONE)
  {
    printf("Error with bcm_multicast_create(%d, %d, &mcast_group): error=%d (\"%s\")\r\n",
           unit, BCM_MULTICAST_TYPE_VLAN, error, bcm_errmsg(error));
    return error;
  }
  for (i = 0; i < 3; i++) {
      if ((error=bcm_multicast_vlan_encap_get(unit, mcast_group, vlan_port[i].port, vlan_port[i].vlan_port_id, &encap_id))!=BCM_E_NONE)
      {
        printf("Error with bcm_multicast_vlan_encap_get: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
        return error;
      }
      if ((error=bcm_multicast_egress_add(unit, mcast_group, vlan_port[i].port, encap_id))!=BCM_E_NONE)
      {
        printf("Error with bcm_multicast_egress_add: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
        return error;
      }
  }

  /* add network port to multicast group as L2 member */
  BCM_GPORT_LOCAL_SET(network_gport, network_port);
  if ((error=bcm_multicast_l2_encap_get(unit, mcast_group, network_gport, -1, &encap_id))!=BCM_E_NONE)
  {
    printf("Error with bcm_multicast_l2_encap_get: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
    return error;
  }
  if ((error=bcm_multicast_egress_add(unit, mcast_group, network_gport, encap_id))!=BCM_E_NONE)
  {
    printf("Error with bcm_multicast_egress_add: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
    return error;
  }

  /* configure vlan membership */
  bcm_pbmp_t pbmp, ubmp;
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_CLEAR(ubmp);
  BCM_PBMP_PORT_ADD(pbmp, network_port);
  BCM_PBMP_PORT_ADD(pbmp, pon_port);

  printf("pon_port=%d, network_port=%d\r\n",pon_port, network_port);
  for (i=0; i<_SHR_PBMP_WORD_MAX; i++)
  {
    printf("0x%08x ",pbmp.pbits[i]);
  }
  printf("\r\n");

  if ((error=bcm_vlan_create(unit, s_vid))!=BCM_E_NONE)
  {
    printf("Error with bcm_vlan_create: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
    return error;
  }
  if ((error=bcm_vlan_port_add(unit, s_vid, pbmp, ubmp))!=BCM_E_NONE)
  {
    printf("Error with bcm_vlan_port_add: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
    return error;
  }

  /* configure the VLAN to enable flooding towards virtual ports, this overrides the regular VLAN flooding */
  bcm_vlan_control_vlan_t vlan_control;
  if ((error=bcm_vlan_control_vlan_get(unit, s_vid, &vlan_control))!=BCM_E_NONE)
  {
    printf("Error with bcm_vlan_control_vlan_get: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
    return error;
  }
  vlan_control.broadcast_group = mcast_group;
  vlan_control.unknown_multicast_group = mcast_group;
  vlan_control.unknown_unicast_group = mcast_group;

  /* if using SVL, configure fid_id */
  vlan_control.forwarding_vlan = s_vid;
  if ((error=bcm_vlan_control_vlan_set(unit, s_vid, vlan_control))!=BCM_E_NONE)
  {
    printf("Error with bcm_vlan_control_vlan_set: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
    return error;
  }

  /* enable VLAN translation & configure ingress VLAN translation key for GPON port */
  if ((error=bcm_vlan_control_set(unit, bcmVlanTranslate, 1))!=BCM_E_NONE)
  {
    printf("Error with bcm_vlan_control_set: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
    return error;
  }

  /* device will do 2 lookups in VLAN_XLATE, configure the keys here */
  if ((error=bcm_vlan_control_port_set(unit, pon_port, bcmVlanPortTranslateKeyFirst, bcmVlanTranslateKeyPortDouble))!=BCM_E_NONE)
  {
    printf("Error with bcm_vlan_control_port_set: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
    return error;
  }
  if ((error=bcm_vlan_control_port_set(unit, pon_port, bcmVlanPortTranslateKeySecond, bcmVlanTranslateKeyPortOuter))!=BCM_E_NONE)
  {
    printf("Error with bcm_vlan_control_port_set: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
    return error;
  }

  /* if you need stacked VLAN for network port, you need to use the 
   * bcm_vlan_translate_action_add() API and 
   * bcm_vlan_translate_egress_action_add() API and 
   * for instance, translate stacked VLANs S=500/C=501 -> 4000 for network port
   */
  #if 0
  bcm_vlan_t s_vlan = 500;
  bcm_vlan_t c_vlan = 501;
  bcm_vlan_action_set_t network_ing_action;
  bcm_vlan_action_set_t network_egr_action;

  bcm_vlan_action_set_t_init(&network_ing_action);
  /* for double tagged packets => replace outer tag & delete inner tag */
  network_ing_action.dt_outer = bcmVlanActionReplace;
  network_ing_action.new_outer_vlan = s_vid;
  network_ing_action.dt_inner = bcmVlanActionDelete;
  if ((error=bcm_vlan_translate_action_add(unit, network_gport, bcmVlanTranslateKeyPortDouble, s_vlan, c_vlan, &network_ing_action))!=BCM_E_NONE)
  {
    printf("Error with bcm_vlan_translate_action_add: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
    return error;
  }
  if ((error=bcm_vlan_control_port_set(unit, network_gport, bcmVlanPortTranslateKeyFirst, bcmVlanTranslateKeyPortDouble))!=BCM_E_NONE)
  {
    printf("Error with bcm_vlan_control_port_set: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
    return error;
  }
  if ((error=bcm_vlan_control_port_set(unit, network_gport, bcmVlanPortTranslateKeySecond, bcmVlanTranslateKeyPortOuter))!=BCM_E_NONE)
  {
    printf("Error with bcm_vlan_control_port_set: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
    return error;
  }

  bcm_vlan_action_set_t_init(&network_egr_action);
  /* for single tagged packets (client 4000) => replace outer tag & add inner tag */
  network_egr_action.ot_outer = bcmVlanActionReplace;
  network_egr_action.new_outer_vlan = s_vlan;
  network_egr_action.ot_inner = bcmVlanActionAdd;
  network_egr_action.new_inner_vlan = c_vlan;
  if ((error=bcm_vlan_translate_egress_action_add(unit, network_gport, s_vid, 0, &network_egr_action))!=BCM_E_NONE)
  {
    printf("Error with bcm_vlan_translate_egress_action_add: error=%d (\"%s\")\r\n",error, bcm_errmsg(error));
    return error;
  }
  #endif

  return BCM_E_NONE;
}

int ptin_vp_group_create(L7_uint32 port_nni, L7_uint32 port_uni, L7_uint16 vid_nni,
                         L7_uint16 vid_uni0, L7_uint16 vid_uni1, L7_uint16 vid_uni2, L7_uint16 vid_uni3, L7_uint16 vid_uni4, L7_uint16 vid_uni5, L7_uint16 vid_uni6, L7_uint16 vid_uni7)
{
  bcm_vlan_t def_vlan, vlan_uni;
  bcm_pbmp_t pbmp, upbmp;
  bcm_port_t bcm_port_nni, bcm_port_uni;
  bcm_gport_t gport_uni;
  bcm_subport_config_t port_config;
  bcm_subport_group_config_t group_config;
  int pri;
  bcm_error_t error;
  static bcm_gport_t group = -1;
  bcm_vlan_control_vlan_t control;

  // Get bcm_port_t values
  if (port_nni>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port_nni, &bcm_port_nni)!=L7_SUCCESS)
  {
    printf("Port %u is invalid\r\n", port_nni);
    return -1;
  }
  if (port_uni>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port_uni, &bcm_port_uni)!=L7_SUCCESS)
  {
    printf("Port %u is invalid\r\n", port_uni);
    return -1;
  }

  /* Initialize gport values */
  if (bcm_port_gport_get(0, bcm_port_uni, &gport_uni) != BCM_E_NONE)
  {
    printf("bcm_port %u is invalid\r\n", bcm_port_uni);
    return -1;
  }

  // Create vlan     
  if (bcm_vlan_create(0, vid_nni) != BCM_E_NONE)
  {
    printf("%s(%d) ERROR!\r\n", __FUNCTION__,__LINE__);
    //return -1;
  }

  /* Get current control definitions for this vlan */
  bcm_vlan_control_vlan_t_init(&control);
  if ((error = bcmx_vlan_control_vlan_get(vid_nni, &control))!=BCM_E_NONE)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error getting vlan control structure! error=%d (%s)\r\n", error, bcm_errmsg(error));
    return L7_FAILURE;
  }
  control.forwarding_vlan = vid_nni;
  control.flags &= ~((uint32) BCM_VLAN_LEARN_DISABLE);
  control.forwarding_mode = bcmVlanForwardBridging;

  /* Apply new control definitions to this vlan */
  if ( (error = bcmx_vlan_control_vlan_set(vid_nni, control)) != BCM_E_NONE )
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
    return L7_FAILURE;
  }

  // Add ports to vid    
  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, bcm_port_nni);
  BCM_PBMP_PORT_ADD(pbmp, bcm_port_uni);
  BCM_PBMP_CLEAR(upbmp);
  if (bcm_vlan_port_add (0, vid_nni, pbmp, upbmp) != BCM_E_NONE)
  {
    printf("%s(%d) ERROR!\r\n", __FUNCTION__,__LINE__);
    //return -1;
  }

  // Remove both ports from default vlan    
  bcm_vlan_default_get (0, &def_vlan);
  bcm_vlan_port_remove (0, def_vlan, pbmp);


  // Set CUST port's vlan translation KEY type    
  bcm_vlan_control_port_set(0, bcm_port_nni, bcmVlanPortTranslateKeyFirst, bcmVlanTranslateKeyPortOuterTag);
  bcm_vlan_control_port_set(0, bcm_port_uni, bcmVlanPortTranslateKeyFirst, bcmVlanTranslateKeyPortOuterTag);

  // Disable ipmc packet modifications for the ports     
  bcm_port_ipmc_modify_set(0, bcm_port_nni, BCM_PORT_IPMC_MODIFY_NO_TTL | BCM_PORT_IPMC_MODIFY_NO_SRCMAC);
  bcm_port_ipmc_modify_set(0, bcm_port_uni, BCM_PORT_IPMC_MODIFY_NO_TTL | BCM_PORT_IPMC_MODIFY_NO_SRCMAC);

  // Enable vlan translation    
  bcm_vlan_control_port_set (0, bcm_port_nni, bcmVlanTranslateIngressEnable, 0);
  bcm_vlan_control_port_set (0, bcm_port_nni, bcmVlanTranslateEgressEnable, 0);
  bcm_vlan_control_port_set (0, bcm_port_nni, bcmVlanTranslateIngressMissDrop, 0);
  bcm_vlan_control_port_set (0, bcm_port_nni, bcmVlanTranslateEgressMissDrop, 0);

  bcm_vlan_control_port_set (0, bcm_port_uni, bcmVlanTranslateIngressEnable, 1);    
  bcm_vlan_control_port_set (0, bcm_port_uni, bcmVlanTranslateEgressEnable, 1);
  bcm_vlan_control_port_set (0, bcm_port_uni, bcmVlanTranslateIngressMissDrop, 1);
  bcm_vlan_control_port_set (0, bcm_port_uni, bcmVlanTranslateEgressMissDrop, 1);

  // Create a subport group    
  bcm_subport_group_config_t_init(&group_config);
  group_config.port  = gport_uni;
  group_config.vlan  = vid_nni;

  if (group>=0)
  {
    bcm_subport_port_delete(0, gport_uni);
    printf("%s(%d) Destroying group %d...\r\n", __FUNCTION__,__LINE__,group);
    if ((error=bcm_subport_group_destroy(0, group))!=BCM_E_NONE)
    {
      printf("%s(%d) ERROR (\"%s\")!\r\n", __FUNCTION__,__LINE__,bcm_errmsg(error));
      return -1;
    }
    group = -1;
  }

  if ((error=bcm_subport_group_create (0, &group_config, &group))!=BCM_E_NONE)
  {
    printf("%s(%d) ERROR (\"%s\")!\r\n", __FUNCTION__,__LINE__,bcm_errmsg(error));
    group = -1;
    return -1;
  }

  // Add subport ports to the subport group
  printf("%s(%d) Created group %d!\r\n", __FUNCTION__,__LINE__,group);

  for( pri=0; pri<8; ++pri )
  {
    if (pri==0)
      vlan_uni = vid_uni0;
    else if (pri==1)
      vlan_uni = vid_uni1;
    else if (pri==2)
      vlan_uni = vid_uni2;
    else if (pri==3)
      vlan_uni = vid_uni3;
    else if (pri==4)
      vlan_uni = vid_uni4;
    else if (pri==5)
      vlan_uni = vid_uni5;
    else if (pri==6)
      vlan_uni = vid_uni6;
    else if (pri==7)
      vlan_uni = vid_uni7;
    else
      break;

    /* Validate vlan */
    if (vlan_uni<1 || vlan_uni>4095)
    {
      break;
    }

    printf("%s(%d) pri=%u: Adding subport port with vlan %u\r\n", __FUNCTION__,__LINE__, pri, vlan_uni);

    bcm_subport_config_t_init(&port_config);
    port_config.group = group;
    port_config.pkt_vlan = vlan_uni;
    port_config.int_pri = pri;
    if ((error=bcm_subport_port_add(0, &port_config, &gport_uni)) != BCM_E_NONE)
    {
     printf("%s(%d) ERROR (pri=%u:\"%s\")!\r\n", __FUNCTION__,__LINE__,pri,bcm_errmsg(error));
     return -1;
    }
  }

  printf("DONE!\r\n");

  return 0;
}

#include <soc/xaui.h>
//#include <soc/phyctrl.h>
//#include <soc/phyreg.h>

/**
* configure_equalizer
* @param port
* @param equalizer
* 
 * @return int
*/
int configure_equalizer(unsigned char port, unsigned char equalizer)
{
  soc_xaui_config_t config;

  //printf("config: port=%d, equalizer=%d\n\r",port, equalizer);
  soc_xaui_config_get(0, port, &config);
  printf("Antes: idriver=%d, ipredriver=%d, preemphasis=%d, equalizer_ctrl=%d\n\r",
         config.idriver,
         config.ipredriver,
         config.preemphasis,
         config.equalizer_ctrl
         );

  config.equalizer_ctrl = equalizer;
  soc_xaui_config_set(0, port, &config);
  printf("Depois: idriver=%d, ipredriver=%d, preemphasis=%d, equalizer_ctrl=%d\n\r",
         config.idriver,
         config.ipredriver,
         config.preemphasis,
         config.equalizer_ctrl
         );

  return(0);
}

/* BER tests only for the CXO640G */
#if (PTIN_BOARD == PTIN_BOARD_CXO640G)

#include <bcm/port.h>
#include <unistd.h>
//#include <application/ptin/utils/include/ipc_lib.h>
#include "soc/trident.h"

int
soc_trident_port_cbl_table_parity_set(int unit, int enable)
{
  uint32 rval;
  soc_reg_t enable_reg = PORT_CBL_TABLE_PARITY_CONTROLr;
  soc_field_t enable_field = PARITY_ENf;

  soc_trident_pipe_select(unit, 0, 0);

  soc_reg32_get(unit, enable_reg, REG_PORT_ANY, 0, &rval);
  soc_reg_field_set(unit, enable_reg, &rval, enable_field, enable);
  soc_reg32_set(unit, enable_reg, REG_PORT_ANY, 0, rval);

  soc_trident_pipe_select(unit, 0, 1);
  soc_reg32_get(unit, enable_reg, REG_PORT_ANY, 0, &rval);
  soc_reg_field_set(unit, enable_reg, &rval, enable_field, enable);
  soc_reg32_set(unit, enable_reg, REG_PORT_ANY, 0, rval);

  soc_trident_pipe_select(unit, 0, 0);

  printf("soc_trident_port_cbl_table_parity_set executed!\r\n");

  return SOC_E_NONE;
}


// *****************************************************************************
//      definicao da estrutura que suporta o protocolo de comunicacao          *
// *****************************************************************************
typedef struct
{
   unsigned int   protocolId;               //identificador do tipo de protocolo
   unsigned int   srcId;                    //endereco fonte   (nao obrigatorio)
   //UWORD  pageSize;                 //indica o tamanho da pagina de leitura
   unsigned int   dstId;                    //endereco destino (nao obrigatorio)
   unsigned int   flags;                    //flags (bit0-0:comando, 1:resposta)
   unsigned int   counter;                  //contador usado para identificar as msg
   unsigned int   msgId;                    //codigo da mensagem a enviar
   unsigned int   infoDim;                  //dimensao em bytes validos  do buffer info
   unsigned char  info[6144]; //buffer de dados
} ipc_msg;

extern int  open_ipc        (int porto_rx,
                               unsigned int ipaddr,
                               int  (*MessageHandler)(ipc_msg *inbuffer, ipc_msg *outbuffer),
                               int timeout,
                               int *handlerid);

extern int  close_ipc       (int handler);

extern int  send_data       (int canal_id,
                               int porto_destino,
                               unsigned int ipdest,
                               ipc_msg *sendbuffer,
                               ipc_msg *receivebuffer);

#define N_SLOTS_MAX   18
#define N_GROUPS_MAX  7
#define N_LANES_MAX   PTIN_SYS_INTFS_PER_SLOT_MAX

int xe_slot_map[2][PTIN_SYS_SLOTS_MAX+1][PTIN_SYS_INTFS_PER_SLOT_MAX];

/* To save BER results */
struct ber_t {
  unsigned int main;
  unsigned int post;
  unsigned int reg;
  unsigned int ber;
};

struct ber_t results_tx[N_SLOTS_MAX][N_LANES_MAX][1024]; /* slot, port, results */
struct ber_t results_rx[N_SLOTS_MAX][N_LANES_MAX][1024]; /* slot, port, results */

struct params {
  char file[128];
  int optimize_lc;
  int n_iter;
  int start_delay;
  int test_time;
  int mode;
  int main_start;
  int main_end;
  int main_step;
  int post_start;
  int post_end;
  int post_step;
  int n_slots;
  int slot[N_SLOTS_MAX];
  unsigned int ip_addr[N_SLOTS_MAX];
  int port_list[N_SLOTS_MAX][N_LANES_MAX];
};

struct params p_tx, p_rx;

/* Semaphore to synchronize PTin task execution */
void *ptin_ber_tx_sem = L7_NULLPTR;
void *ptin_ber_rx_sem = L7_NULLPTR;

int ber_tx_running = 0;
int ber_rx_running = 0;

static int canal_ipc;

static int ber_init_done = 0;
static int mx = 0;
static int stop = 0;

int remote_reg_read(unsigned int ip_addr, int port, int mmd, int addr, unsigned int *values, unsigned int *n_values);
int remote_reg_write(unsigned int ip_addr, int port, int mmd, int addr, int value);
int remote_var_read(unsigned int ip_addr, int port, int mmd, int addr, unsigned int *values, unsigned int *n_values);
int remote_var_write(unsigned int ip_addr, int port, int mmd, int addr, int value);

#define TEST_TIME 10

void ptin_ber_tx_task(L7_uint32 numArgs, void *unit)
{
  int tap_main, tap_post;
  int reg;
  ipc_msg txmsg, rxmsg;
  L7_RC_t rc;
  int ret;
  int i, idx, slot, port, port_idx, iter;
  char iter_str[10];
  char file[128];
  FILE *fd;
  unsigned int tx_ber, tx_ber_sum, vitesse_res[N_LANES_MAX], n_res;
  unsigned int max_count, count;
  unsigned int results_iter[N_SLOTS_MAX][N_LANES_MAX];

  rc = osapiTaskInitDone(L7_BER_TX_SYNC);

  /* Compose TX message */
  txmsg.protocolId   = 1;
  txmsg.srcId        = 7100;
  txmsg.dstId        = 6100;
  txmsg.flags        = 0;
  txmsg.counter      = rand ();
  txmsg.msgId        = 0x531;
  txmsg.infoDim      = 1;
  txmsg.info[0]      = mx;  /* First byte will contain the source matrix */

  /* Do stuff... */
  do {
    ret = 0;
    ber_tx_running = 0;

    /* Wait for a signal indicating that all other modules
     * configurations were executed */
    printf("BER TX task waiting for commands...\n");
    rc = osapiSemaTake(ptin_ber_tx_sem, L7_WAIT_FOREVER);
    printf("\nBER TX task will now start!\n");

    for (iter=1; iter<=p_tx.n_iter; iter++)
    {
      memset(results_tx, 0x00, sizeof(results_tx));

      strcpy(file, p_tx.file);
      /* Only apply multiple iteration mode if n_iter!=0 */
      if (p_tx.n_iter > 1) {
        printf("\nRunning iteration %u of %u\n", iter, p_tx.n_iter);
        sprintf(iter_str, "_iter%04u", iter);
        strcat(file, iter_str);
      }

      fd = fopen(file, "w+");
      if (fd == NULL) {
        printf("Error opening file %.128s for writing!\n", file);
        continue;
      }
      printf("File %.128s successfully opened\n", file);

      fprintf(fd, "Running BER analysis @ TX...\n");
      fprintf(fd, " iteration       = %u of %u\n"
                  " start delay     = %u\n"
                  " test time       = %u\n"
                  " mode            = %u\n"
                  " main start      = %u\n"
                  " main end        = %u\n"
                  " main step       = %u\n"
                  " post start      = %u\n"
                  " post end        = %u\n"
                  " post step       = %u\n"
                  " n_slots         = %u\n",
              iter, p_tx.n_iter,
              p_tx.start_delay, p_tx.test_time, p_tx.mode,
              p_tx.main_start, p_tx.main_end, p_tx.main_step,
              p_tx.post_start, p_tx.post_end, p_tx.post_step,
              p_tx.n_slots);

      for (i=0; i<p_tx.n_slots; i++) {
        fprintf(fd, " slot = %-2u  port list = {%-2d,%-2d,%-2d,%-2d}  IP = 0x%08X\n",
                p_tx.slot[i], p_tx.port_list[i][0], p_tx.port_list[i][1], p_tx.port_list[i][2], p_tx.port_list[i][3], p_tx.ip_addr[i]);
      }
      fprintf(fd, "\n");
      fflush(fd);

      /* For each TAP setting... */
      idx = 0;
      for (tap_main=p_tx.main_start; tap_main>=p_tx.main_end; tap_main-=p_tx.main_step) {
        for (tap_post=p_tx.post_start; tap_post<=p_tx.post_end; tap_post+=p_tx.post_step, idx++) {

          reg = 0x0000;

          if ( !(p_tx.mode & 0x08) )
          {
            reg = 0x8000 | (tap_post<<10) | (tap_main<<4);
          }

          /* Only apply tap settings and reset BER in the first iteration
           * For the next ones, just monitor BER values after each test_time
           */
          if (iter == 1 || (p_tx.mode & 0x2) )
          {
            if ( !(p_tx.mode & 0x08) )
            {
              fprintf(fd, "=> Main=%2u Post=%2u Reg=0x%04X\n\n", tap_main, tap_post, reg);

              /* Update tap settings */
              for (slot=0; slot<p_tx.n_slots; slot++)
              {
                for (port_idx = 0; port_idx < N_LANES_MAX; port_idx++)
                {
                  port = p_tx.port_list[slot][port_idx];
                  if ( port < 0 )  continue;
                
                  bcm_port_phy_control_set(0, port+1, BCM_PORT_PHY_CONTROL_PREEMPHASIS, reg);
                }
              }
            }
            else
            {
              fprintf(fd, "=> Transmission tap settings not changed!\n\n");
            }

            /* Initialization type 3 */
            if (p_tx.optimize_lc & 0x02 )
            {
              fprintf(fd, "   Applying init procedure Copper->SFI, SFI->Copper\n");
              for (slot=0; slot<p_tx.n_slots; slot++)
              {
                /* Set SFI mode */
                if (remote_var_write(p_tx.ip_addr[slot], -1, 1, 0x94, 2)!=0)
                {
                  fprintf(fd, "   [ERROR] Failed setting SFI mode for slot %u\n", p_tx.slot[slot]);
                }
                /* Get back to Copper mode */
                if (remote_var_write(p_tx.ip_addr[slot], -1, 1, 0x94, 3)!=0)
                {
                  fprintf(fd, "   [ERROR] Failed setting Copper mode for slot %u\n", p_tx.slot[slot]);
                }
              }
            }

            if (p_tx.optimize_lc & 0x01 )
            {
              ret = 0;

              fprintf(fd, "   Applying init procedure 0x8000->1x8036, 0->0xB0\n");

              /* Pre-processing for Linecards */
              for (slot=0; slot<p_tx.n_slots; slot++)
              {
                /* Set standard library on remote vitesses: 0 */
                if ((ret=remote_reg_write(p_tx.ip_addr[slot], -1, 0x101, 0x8036, 0x8000)) != 0)
                {
                  fprintf(fd, "   [ERROR] Failed setting new value for 1x8036 in slot %u\n",p_tx.slot[slot]);
                  continue;
                }
                /* Reset counter to start aggresive track phase */
                if ((ret=remote_var_write(p_tx.ip_addr[slot], -1, 1, 0xb0, 0)) != 0)
                {
                  fprintf(fd, "   [ERROR] Failed aggressive track phase activation in slot %u\n",p_tx.slot[slot]);
                  continue;
                }
              }
            }

            fflush(fd);

            /* Let remote partner stabilize... */
            sleep(p_tx.start_delay);

            /* For each slot, get remote values (just to reset them!) (4 ports at once) */
            for (slot=0; slot<p_tx.n_slots; slot++)
            {
              ret = send_data (canal_ipc, 6100, p_tx.ip_addr[slot], &txmsg, &rxmsg);
              if (ret != 0) {
                fprintf(fd, "   [ERROR] Failed remote values extraction from slot %u\n", p_tx.slot[slot]);
                //fclose(fd);
                continue;
              }

              usleep(1*1000);
            }
          }

          /* Maximum number of readings for one iteration */
          max_count = (p_tx.mode>>8) & 0xff;
          if (max_count==0)  max_count=1;

          for (count=0; count<max_count; count++)
          {
            fprintf(fd, "  Time Instant: t = %u s\n", count*p_tx.test_time);

            #if 1
            /* Read register 1x8036 */
            for (slot=0; slot<p_tx.n_slots; slot++)
            {
              /* Read status of Vitesse firmware */
              if (remote_reg_read(p_tx.ip_addr[slot], -1, 0x101, 0x8036, vitesse_res, &n_res)==0 && n_res==4)
              {
                fprintf(fd, "   Slot=%-2u: 1x8036 = { ", p_tx.slot[slot]);
                for (port_idx = 0; port_idx < N_LANES_MAX; port_idx++)
                {
                  if ( p_tx.port_list[slot][port_idx] < 0 )
                    continue;
                  fprintf(fd, " 0x%04x ", vitesse_res[port_idx]);
                }
                fprintf(fd, " }\n");
              }
              else
              {
                fprintf(fd, "   [ERROR] Failed reading 1x8036 register in slot %u\n", p_tx.slot[slot]);
              }
            }
            fprintf(fd, "\n");
            /* Read register 1x8037 */
            for (slot=0; slot<p_tx.n_slots; slot++)
            {
              /* Read status of Vitesse firmware */
              if (remote_reg_read(p_tx.ip_addr[slot], -1, 0x101, 0x8037, vitesse_res, &n_res)==0 && n_res==4)
              {
                fprintf(fd, "   Slot=%-2u: 1x8037 = { ", p_tx.slot[slot]);
                for (port_idx = 0; port_idx < N_LANES_MAX; port_idx++)
                {
                  if ( p_tx.port_list[slot][port_idx] < 0 )
                    continue;
                  fprintf(fd, " 0x%04x ", vitesse_res[port_idx]);
                }
                fprintf(fd, " }\n");
              }
              else
              {
                fprintf(fd, "   [ERROR] reading 1x8037 register in slot %u\n", p_tx.slot[slot]);
              }
            }
            fprintf(fd, "\n");
            /* Read register 1x8034 */
            for (slot=0; slot<p_tx.n_slots; slot++)
            {
              /* Read status of Vitesse firmware */
              if (remote_reg_read(p_tx.ip_addr[slot], -1, 0x101, 0x8034, vitesse_res, &n_res)==0 && n_res==4)
              {
                fprintf(fd, "   Slot=%-2u: 1x8034 = { ", p_tx.slot[slot]);
                for (port_idx = 0; port_idx < N_LANES_MAX; port_idx++)
                {
                  if ( p_tx.port_list[slot][port_idx] < 0 )
                    continue;
                  fprintf(fd, " 0x%04x ", vitesse_res[port_idx]);
                }
                fprintf(fd, " }\n");
              }
              else
              {
                fprintf(fd, "   [ERROR] reading 1x8034 register in slot %u\n", p_tx.slot[slot]);
              }
            }
            fprintf(fd, "\n");
            #endif

            fflush(fd);

            /* Wait the integration time... */
            sleep(p_tx.test_time);

            tx_ber_sum = 0;
            memset(results_iter,0xff,sizeof(results_iter));

            /* For each slot, get the final values (4 ports at once) */
            for (slot=0; slot<p_tx.n_slots; slot++)
            {
              memset(rxmsg.info, 0xFF, 4*sizeof(int));
              ret = send_data (canal_ipc, 6100, p_tx.ip_addr[slot], &txmsg, &rxmsg);
              if (ret != 0) {
                fprintf(fd, "   [ERROR] Failed remote values extraction from slot %u\n", p_tx.slot[slot]);
                //fclose(fd);
                continue;
              }
              usleep(1*1000);

              for (port_idx = 0; port_idx<4; port_idx++) {

                tx_ber = ((unsigned int *)rxmsg.info)[port_idx];

                /* Only consider valid lanes */
                if (p_tx.port_list[slot][port_idx]>=0)
                {
                  tx_ber_sum += tx_ber;
                }

                /* Save results */
                if (count==0)
                {
                  results_tx[slot][port_idx][idx].main = tap_main;
                  results_tx[slot][port_idx][idx].post = tap_post;
                  results_tx[slot][port_idx][idx].reg  = reg;
                }

                results_tx[slot][port_idx][idx].ber += tx_ber;
                if (results_tx[slot][port_idx][idx].ber > 0xFFFF)
                  results_tx[slot][port_idx][idx].ber = 0xFFFF;

                results_iter[slot][port_idx] = tx_ber;
              }

              fprintf(fd, "   Slot %-2u: BER -> ", p_tx.slot[slot]);
              for ( port_idx=0; port_idx < N_LANES_MAX; port_idx++)
              {
                port = p_tx.port_list[slot][port_idx];
                if ( port < 0 )  continue;

                fprintf(fd, " xe%-2d=%-5u", port, results_iter[slot][port_idx]);
              }
              fprintf(fd, "\n");
            }

            /* Stop test, if we have errors */
            if ( !(p_tx.mode & 0xff00) ||
                  ( (p_tx.mode & 0xf0)==0x10 && tx_ber_sum == 0   ) ||
                  ( (p_tx.mode & 0xf0)==0x20 && tx_ber_sum < 3    ) ||
                  ( (p_tx.mode & 0xf0)==0x30 && tx_ber_sum < 5    ) ||
                  ( (p_tx.mode & 0xf0)==0x40 && tx_ber_sum < 10   ) ||
                  ( (p_tx.mode & 0xf0)==0x50 && tx_ber_sum < 20   ) ||
                  ( (p_tx.mode & 0xf0)==0x60 && tx_ber_sum < 50   ) ||
                  ( (p_tx.mode & 0xf0)==0x70 && tx_ber_sum < 100  ) ||
                  ( (p_tx.mode & 0xf0)==0x80 && tx_ber_sum < 200  ) ||
                  ( (p_tx.mode & 0xf0)==0x90 && tx_ber_sum < 500  ) ||
                  ( (p_tx.mode & 0xf0)==0xa0 && tx_ber_sum < 1000 ) ||
                  ( (p_tx.mode & 0xf0)==0xb0 && tx_ber_sum < 2000 ) ||
                  ( (p_tx.mode & 0xf0)==0xc0 && tx_ber_sum < 10000) ||
                  ( (p_tx.mode & 0xf0)==0xd0 && tx_ber_sum < 20000) ||
                  ( (p_tx.mode & 0xf0)==0xe0 && tx_ber_sum < 50000) ||
                  ( (p_tx.mode & 0xf0)==0xf0 && tx_ber_sum < 65535) )   break;

            if (stop)
              break;
          }

          fprintf(fd, "--------------------------------------------------------------------------------------\n");
          fflush(fd);

          if ( (p_tx.mode & 1) == 1) {
            tap_main -= p_tx.main_step;
          }

          if (stop)
            break;
        }

        if (stop)
          break;
      }

      if (stop) {
        fprintf(fd, "\nBER tx task forced to stop!\n");
        printf("\nBER tx task forced to stop!\n");
      }

      fprintf(fd, "\n\nFinal Report:\n");

      /* Print a first table with the port mapping */
      fprintf(fd, "+-------+---------------+---------+-----------------+\n");
      fprintf(fd, "| SLOT  | BCM PORT      | LC PORT | LC IP           |\n");
      fprintf(fd, "+-------+---------------+---------+-----------------+\n");
      //           | i=N  | xeXX (lane=K) | port=P  | 192.168.200.xxx |

      for (slot=0; slot<p_tx.n_slots; slot++) {
        for (port_idx=0; port_idx<N_LANES_MAX; port_idx++) {
          port = p_tx.port_list[slot][port_idx];
          if ( port < 0 )  continue;

          fprintf(fd, "| i=%-2u  | xe%-2d (lane=%d) | port=%d  | 192.168.200.%-3u |\n",
                  p_tx.slot[slot], port, port_idx, port_idx, p_tx.ip_addr[slot] & 0xFF);
        }
      }
      fprintf(fd, "+-------+---------------+---------+-----------------+\n");

      /* Print a second table with all results from all slots */

      /* The heading of this table varies with the number of slots being tested */
      char str1[768], str2[768], tmp[768];
      strcpy(str1, "+------+------+--------+");
      strcpy(str2, "| MAIN | POST |  REG   |");
      for (slot=0; slot<p_tx.n_slots; slot++) {
        for (port_idx=0; port_idx<N_LANES_MAX; port_idx++) {
          port = p_tx.port_list[slot][port_idx];
          if ( port < 0 )  continue;

          strcat(str1, "-------+");
          sprintf(tmp, " s%02d:%d |", p_tx.slot[slot], port_idx+1);
          strcat(str2, tmp);
        }
      }
      strcat(str1, "\n");
      strcat(str2, "\n");
      fprintf(fd, str1);
      fprintf(fd, str2);
      fprintf(fd, str1);

      /* Print columns with the BER results */
      for (i=0; i<idx; i++) { /* idx represents the number of main+post values */
        fprintf(fd, "|  %-2u  |  %-2u  | 0x%04X |",
               results_tx[0][0][i].main,
               results_tx[0][0][i].post,
               results_tx[0][0][i].reg);

        for (slot=0; slot<p_tx.n_slots; slot++) {
          for (port_idx=0; port_idx<N_LANES_MAX; port_idx++) {
            port = p_tx.port_list[slot][port_idx];
            if ( port < 0 )  continue;

            fprintf(fd, " %5u |", results_tx[slot][port_idx][i].ber);
          }
        }
        fprintf(fd, "\n");
      }
      fprintf(fd, str1);

      fflush(fd);
      fclose(fd);

      if (stop)
        break;
    }

    printf("\nBER @tx done!\n");

  } while (1);

  return;
}

void ptin_ber_rx_task(L7_uint32 numArgs, void *unit)
{
  int tap_main, tap_post;
  int reg;
  ipc_msg txmsg, rxmsg;
  L7_RC_t rc;
  int ret;
  int i, idx, slot, port, port_idx, iter;
  char iter_str[10];
  char file[128];
  FILE *fd;
  int rx_ber, rx_ber_sum;
  unsigned int max_count, count;
  unsigned int results_iter[N_SLOTS_MAX][N_LANES_MAX];

  rc = osapiTaskInitDone(L7_BER_RX_SYNC);

  /* Compose TX message */
  txmsg.protocolId   = 1;
  txmsg.srcId        = 7100;
  txmsg.dstId        = 6100;
  txmsg.flags        = 0;
  txmsg.counter      = rand ();
  txmsg.msgId        = 0x532;
  txmsg.infoDim      = 5;
  txmsg.info[0] = txmsg.info[1] = txmsg.info[2] = txmsg.info[3] = txmsg.info[4] = 0;

  /* Do stuff... */
  do {
    ret = 0;
    ber_rx_running = 0;

    /* Wait for a signal indicating that all other modules
     * configurations were executed */
    printf("BER RX task waiting for commands...\n");
    rc = osapiSemaTake(ptin_ber_rx_sem, L7_WAIT_FOREVER);
    printf("\nBER RX task will now start!\n");

    for (iter=1; iter<=p_rx.n_iter; iter++)
    {
      memset(results_rx, 0x00, sizeof(results_rx));

      strcpy(file, p_rx.file);
      /* Only apply multiple iteration mode if n_iter!=0 */
      if (p_rx.n_iter > 1) {
        printf("\nRunning iteration %u of %u\n", iter, p_rx.n_iter);
        sprintf(iter_str, "_iter%04u", iter);
        strcat(file, iter_str);
      }

      fd = fopen(file, "w+");
      if (fd == NULL) {
        printf("Error opening file %.128s for writing!\n", file);
        continue;
      }
      printf("File %.128s successfully opened\n", file);

      fprintf(fd, "Running BER analysis @ RX...\n");
      fprintf(fd, " iteration       = %u of %u\n"
                  " start delay     = %u\n"
                  " test time       = %u\n"
                  " mode            = %u\n"
                  " main start      = %u\n"
                  " main end        = %u\n"
                  " main step       = %u\n"
                  " post start      = %u\n"
                  " post end        = %u\n"
                  " post step       = %u\n"
                  " n_slots         = %u\n",
              iter, p_rx.n_iter,
              p_rx.start_delay, p_rx.test_time, p_rx.mode,
              p_rx.main_start, p_rx.main_end, p_rx.main_step,
              p_rx.post_start, p_rx.post_end, p_rx.post_step,
              p_rx.n_slots);

      for (i=0; i<p_rx.n_slots; i++) {
        fprintf(fd, " slot = %-2u  port list = {%-2d,%-2d,%-2d,%-2d}  IP = 0x%08X\n",
                p_rx.slot[i], p_rx.port_list[i][0], p_rx.port_list[i][1], p_rx.port_list[i][2], p_rx.port_list[i][3], p_rx.ip_addr[i]);
      }

      fflush(fd);

      /* For each TAP setting... */
      idx = 0;
      for (tap_main=p_rx.main_start; tap_main>=p_rx.main_end; tap_main-=p_rx.main_step) {
        for (tap_post=p_rx.post_start; tap_post<=p_rx.post_end; tap_post+=p_rx.post_step, idx++) {

          reg = 0x0000;

          if ( !(p_rx.mode & 0x08) )
          {
            reg = (tap_main<<8) | tap_post;
          }

          /* Only apply tap settings and reset BER in the first iteration
           * For the next ones, just monitor BER values after each test_time
           */
          if ( iter == 1 || (p_rx.mode & 0x02) ) {

            if ( !(p_rx.mode & 0x08) )
            {
              /* For each slot, set remote tap values (4 ports at once) */
              for (slot=0; slot<p_rx.n_slots; slot++) {
                txmsg.info[0] = 0x0F; /* pre cursor - default 0x0F */
                txmsg.info[1] = tap_main; /* main cursor */
                txmsg.info[2] = tap_post; /* post cursor */
                txmsg.info[3] = 0x0A; /* Slew control - default 0x0A */
                txmsg.info[4] = mx;   /* Source matrix */

                ret = send_data (canal_ipc, 6100, p_rx.ip_addr[slot], &txmsg, &rxmsg);
                if (ret != 0) {
                  fprintf(fd, "Error setting remote tap values slot %u\n", p_rx.slot[slot]);
                  //fclose(fd);
                  continue;
                }
                usleep(1*1000);
              }
            }
            else
            {
              fprintf(fd, "=> Vitesse tap settings not changed!\n\n");
            }

            /* Let local partner stabilize... */
            sleep(p_rx.start_delay);

            /* For each slot and for each port, read ber error to reset to zero */
            for (slot=0; slot<p_rx.n_slots; slot++) {

              for (port_idx=0; port_idx<N_LANES_MAX; port_idx++) {
                port = p_rx.port_list[slot][port_idx];
                if ( port < 0 )  continue;

                rc = bcm_port_control_get(0, port+1, bcmPortControlPrbsRxStatus, &rx_ber);
                if (rc != L7_SUCCESS) {
                  fprintf(fd, "ERROR reading rx status from port %u\n", port);
                  //fclose(fd);
                  continue;
                }
              }
            }
          }

          /* Maximum number of readings for one iteration */
          max_count = (p_rx.mode>>8) & 0xff;
          if (max_count==0)  max_count=1;

          for (count=0; count<max_count; count++)
          {
            /* Wait the integration time... */
            sleep(p_rx.test_time);

            rx_ber_sum = 0;
            memset(results_iter,0xff,sizeof(results_iter));

            fprintf(fd, "  Time Instant: t = %u s\n", count*p_rx.test_time);

            /* For each slot, get the final values (4 ports at once) */
            for (slot=0; slot<p_rx.n_slots; slot++) {
              for (port_idx=0; port_idx<N_LANES_MAX; port_idx++) {

                /* Save tap settings and register address into results_rx */
                if (count==0)
                {
                  results_rx[slot][port_idx][idx].main = tap_main;
                  results_rx[slot][port_idx][idx].post = tap_post;
                  results_rx[slot][port_idx][idx].reg  = reg;
                }

                /* Only valid lanes will continue */
                port = p_rx.port_list[slot][port_idx];
                if ( port < 0 )  continue;

                rc = bcm_port_control_get(0, port+1, bcmPortControlPrbsRxStatus, &rx_ber);
                if (rc != L7_SUCCESS) {
                  fprintf(fd, "ERROR reading rx status from port %u\n", port);
                  //fclose(fd);
                  continue;
                }

                /* Sum errors */
                rx_ber_sum += rx_ber;

                /* Save results */
                results_rx[slot][port_idx][idx].ber += rx_ber;
                if (results_rx[slot][port_idx][idx].ber > 0xFFFF)
                  results_rx[slot][port_idx][idx].ber = 0xFFFF;

                results_iter[slot][port_idx] = rx_ber;
              }

              fprintf(fd, "Slot %-2d -> main=%2u post=%2u reg=0x%04X BER:", slot, tap_main, tap_post, reg);
              for (port_idx=0; port_idx<N_LANES_MAX; port_idx++)
              {
                port = p_rx.port_list[slot][port_idx];
                if ( port < 0 )  continue;

                fprintf(fd, " xe%-2d=%-5u", port, results_iter[slot][port_idx]);
              }
              fprintf(fd, "\n");
            }

            /* One more test, if we have errors */
            if ( !(p_rx.mode & 0xff00) ||
                  ( (p_rx.mode & 0xf0)==0x10 && rx_ber_sum == 0   ) ||
                  ( (p_rx.mode & 0xf0)==0x20 && rx_ber_sum < 3    ) ||
                  ( (p_rx.mode & 0xf0)==0x30 && rx_ber_sum < 5    ) ||
                  ( (p_rx.mode & 0xf0)==0x40 && rx_ber_sum < 10   ) ||
                  ( (p_rx.mode & 0xf0)==0x50 && rx_ber_sum < 20   ) ||
                  ( (p_rx.mode & 0xf0)==0x60 && rx_ber_sum < 50   ) ||
                  ( (p_rx.mode & 0xf0)==0x70 && rx_ber_sum < 100  ) ||
                  ( (p_rx.mode & 0xf0)==0x80 && rx_ber_sum < 200  ) ||
                  ( (p_rx.mode & 0xf0)==0x90 && rx_ber_sum < 500  ) ||
                  ( (p_rx.mode & 0xf0)==0xa0 && rx_ber_sum < 1000 ) ||
                  ( (p_rx.mode & 0xf0)==0xb0 && rx_ber_sum < 2000 ) ||
                  ( (p_rx.mode & 0xf0)==0xc0 && rx_ber_sum < 10000) ||
                  ( (p_rx.mode & 0xf0)==0xd0 && rx_ber_sum < 20000) ||
                  ( (p_rx.mode & 0xf0)==0xe0 && rx_ber_sum < 50000) ||
                  ( (p_rx.mode & 0xf0)==0xf0 && rx_ber_sum < 65535) )   break;

            if (stop)
              break;
          }

          fprintf(fd, "--------------------------------------------------------------------------------------\n");
          fflush(fd);

          if ( (p_rx.mode & 1) == 1) {
            tap_main -= p_rx.main_step;
          }

          if (stop)
            break;
        }

        if (stop)
          break;
      }

      if (stop) {
        fprintf(fd, "\nBER rx task forced to stop!\n");
        printf("\nBER rx task forced to stop!\n");
      }

      fprintf(fd, "\n\nFinal Report:\n");

      /* Print a first table with the port mapping */
      fprintf(fd, "+-------+---------------+---------+-----------------+\n");
      fprintf(fd, "| SLOT  | BCM PORT      | LC PORT | LC IP           |\n");
      fprintf(fd, "+-------+---------------+---------+-----------------+\n");
      //           | i=NN  | xeXX (lane=K) | port=P  | 192.168.200.xxx |

      for (slot=0; slot<p_rx.n_slots; slot++) {
        for (port_idx=0; port_idx<N_LANES_MAX; port_idx++) {
          port = p_rx.port_list[slot][port_idx];
          if ( port < 0 )  continue;

          fprintf(fd, "|  %-2u  | xe%-2d (lane=%d) | port=%d  | 192.168.200.%-3u |\n",
                  p_rx.slot[slot], port, port_idx, port_idx, p_rx.ip_addr[slot] & 0xFF);
        }
      }
      fprintf(fd, "+-------+---------------+---------+-----------------+\n");

      /* Print a second table with all results from all slots */

      /* The heading of this table varies with the number of slots being tested */
      char str1[768], str2[768], tmp[768];
      strcpy(str1, "+------+------+--------+");
      strcpy(str2, "| MAIN | POST |  REG   |");
      for (slot=0; slot<p_rx.n_slots; slot++) {
        for (port_idx=0; port_idx<N_LANES_MAX; port_idx++) {
          port = p_rx.port_list[slot][port_idx];
          if ( port < 0 )  continue;

          strcat(str1, "-------+");
          sprintf(tmp, " s%02d:%d |", p_rx.slot[slot], port_idx+1);
          strcat(str2, tmp);
        }
      }
      strcat(str1, "\n");
      strcat(str2, "\n");
      fprintf(fd, str1);
      fprintf(fd, str2);
      fprintf(fd, str1);

      /* Print columns with the BER results */
      for (i=0; i<idx; i++) { /* idx represents the number of main+post values */
        fprintf(fd, "|  %-2u  |  %-2u  | 0x%04X |",
               results_rx[0][0][i].main,
               results_rx[0][0][i].post,
               results_rx[0][0][i].reg);

        for (slot=0; slot<p_rx.n_slots; slot++) {
          for (port_idx=0; port_idx<N_LANES_MAX; port_idx++) {
            port = p_rx.port_list[slot][port_idx];
            if ( port < 0 )  continue;

            fprintf(fd, " %5u |", results_rx[slot][port_idx][i].ber);
          }
        }
        fprintf(fd, "\n");
      }
      fprintf(fd, str1);

      fflush(fd);
      fclose(fd);

      if (stop)
        break;
    }

    printf("\nBER @rx done!\n");

  } while (1);

  return;
}


int ber_init(void)
{
  int slot, lane;
  int ret;
  int matrix;
  int port, xe_port;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;
  HAPI_WC_PORT_MAP_t           *hapiWCMapPtr;

  if (ptin_ber_tx_sem != L7_NULLPTR) {
    printf("BER has already been initialized!\n");
    return -1;
  }

  /* Read slot id: 0->Working; 1->Protection */
  matrix = cpld_map->reg.slot_id;

  if (matrix != 0 && matrix != 1) {
    printf("Unknown matrix: %u\n",matrix);
    return -1;
  }
  printf("We are in matrix: %s\n",((matrix) ? "Protection" : "Working"));

  mx = matrix & 1;

  /* Ports information about the system */
  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(hpcLocalCardIdGet(0));
  dapiCardPtr          = sysapiHpcCardInfoPtr->dapiCardInfo;
  hapiSlotMapPtr       = dapiCardPtr->slotMap;
  hapiWCMapPtr         = dapiCardPtr->wcPortMap;

  /* Fill xe_slot_map */
  memset(xe_slot_map, 0xff, sizeof(xe_slot_map));   /* -1 for all values */

  for (port=0; port<L7_MAX_PHYSICAL_PORTS_PER_UNIT; port++)
  {
    slot = hapiWCMapPtr[port].slotNum;
    lane = hapiWCMapPtr[port].wcLane;
    xe_port = hapiSlotMapPtr[port].bcm_port-1;

    /* Update xe port map (only 10/40/100Gbps) */
    if ( hapiWCMapPtr[port].wcSpeedG > 1 )
    {
      if (slot<=PTIN_SYS_SLOTS_MAX && lane<PTIN_SYS_INTFS_PER_SLOT_MAX)
        xe_slot_map[mx][slot][lane] = xe_port;
    }
  }

  printf("xe_slot_map:");
  for (slot=1; slot<=PTIN_SYS_SLOTS_MAX; slot++)
  {
    printf("\n Slot %02u: ",slot);
    for (lane=0; lane<PTIN_SYS_INTFS_PER_SLOT_MAX; lane++)
    {
      if (xe_slot_map[mx][slot][lane] >= 0)
        printf(" xe%-2d", xe_slot_map[mx][slot][lane]);
      else
        printf("  -- ");
    }
  }
  printf("\n");

  ptin_ber_tx_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
  if (ptin_ber_tx_sem == L7_NULLPTR) {
    printf("Failed to create ptin_ber_tx_sem semaphore!\n");
    return -1;
  }

  ptin_ber_rx_sem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_EMPTY);
  if (ptin_ber_rx_sem == L7_NULLPTR) {
    printf("Failed to create ptin_ber_rx_sem semaphore!\n");
    return -1;
  }

  /* Create BER tx task */
  if (osapiTaskCreate("PTIN BER TX task", ptin_ber_tx_task, 0, 0,
                      L7_DEFAULT_STACK_SIZE,
                      L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    printf("Failed to create BER TX task!\n");
    return -1;
  }

  /* Create BER rx task */
  if (osapiTaskCreate("PTIN BER RX task", ptin_ber_rx_task, 0, 0,
                      L7_DEFAULT_STACK_SIZE,
                      L7_DEFAULT_TASK_PRIORITY,
                      L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    printf("Failed to create BER RX task!\n");
    return -1;
  }

  /* Wait for task to be launched */
  if (osapiWaitForTaskInit (L7_BER_TX_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    printf("Failed to start BER TX task!\n");
    return -1;
  }
  /* Wait for task to be launched */
  if (osapiWaitForTaskInit (L7_BER_RX_SYNC, L7_WAIT_FOREVER) != L7_SUCCESS)
  {
    printf("Failed to start BER RX task!\n");
    return -1;
  }

  printf("BER tasks launch OK\n");

  /* Open IPC channel */
  ret = open_ipc(0, 0, NULL, 5, &canal_ipc);
  if (ret != 0) {
    printf("Error opening IPC channel!\n");
    return -1;
  }
  printf("IPC channel OK\n");

  ber_init_done = 1;

  return 0;
}


int ber_stop(void)
{
  if (ber_init_done == 0) {
    printf("BER needs to be initialized. Please run 'ber_init()'\n");
    return -1;
  }

  printf("Stop signal sent to working threads...\n"
         "Please wait until each cylce ends.\n");
  stop = 1;

  return 0;
}

/**
 * 
 * 
 * @author alex (1/14/2013)
 * 
 * @param start_delay Delay before PRBS sampling
 * @param test_time   PRBS sampling time
 * @param n_slots     Number of slots being testes simultaneously
 * @param bp1         Base port #1 (set of 4 consecutive ports mapped to a single slot)
 * @param ip1         Remote IP address #1
 * @param bp2         Base port #2
 * @param ip2         Remote IP address #2
 * (...)
 * 
 * @return int 
 */
int get_tx_ber(char *file,
               int optimize_lc,
               int n_iter,
               int start_delay, int test_time, int mode,
               int main_start, int main_end, int main_step,
               int post_start, int post_end, int post_step,
               int n_groups,
               int n_slots0, int slot0,
               int n_slots1, int slot1,
               int n_slots2, int slot2,
               int n_slots3, int slot3,
               int n_slots4, int slot4,
               int n_slots5, int slot5,
               int n_slots6, int slot6)
{
  int i, j;
  struct groups {
    int n_slots;
    int slot;
  } group[N_GROUPS_MAX];

  stop = 0;

  if (ber_init_done == 0) {
    printf("BER needs to be initialized. Please run 'ber_init()'\n");
    return -1;
  }
  if (ber_tx_running) {
    printf("BER TX thread is already running!\n");
    return -1;
  }
  if (n_groups > N_GROUPS_MAX) {
    printf("Number of groups is limited to %u!\n", N_GROUPS_MAX);
    return -1;
  }
  if (file == NULL) {
    printf("Invalid file name: %.20s\n", file);
    return -1;
  }
  if (main_start > 63 || post_end > 31) {
    printf("main_start or post_end out of range! Max 63,31\n");
    return -1;
  }
  if (main_step <= 0 || post_step <= 0) {
    printf("main_step or post_step must be greater than zero!\n");
    return -1;
  }
  if (n_iter < 1 || n_iter > 9999 ) {
    printf("Number of iterations must be in the range of [1..9999]\n");
    return -1;
  }

  strcpy(p_tx.file, file);
  p_tx.optimize_lc = optimize_lc;
  p_tx.n_iter      = n_iter;
  p_tx.start_delay = start_delay;
  p_tx.test_time   = test_time;
  p_tx.mode        = mode;
  p_tx.main_start  = main_start;
  p_tx.main_end    = main_end;
  p_tx.main_step   = main_step;
  p_tx.post_start  = post_start;
  p_tx.post_end    = post_end;
  p_tx.post_step   = post_step;

  group[0].n_slots = n_slots0;
  group[0].slot    = slot0;
  group[1].n_slots = n_slots1;
  group[1].slot    = slot1;
  group[2].n_slots = n_slots2;
  group[2].slot    = slot2;
  group[3].n_slots = n_slots3;
  group[3].slot    = slot3;
  group[4].n_slots = n_slots4;
  group[4].slot    = slot4;
  group[5].n_slots = n_slots5;
  group[5].slot    = slot5;
  group[6].n_slots = n_slots6;
  group[6].slot    = slot6;

  p_tx.n_slots = 0;
  for (i=0; i<n_groups; i++) {
    for (j=0; j<group[i].n_slots; j++) {
      p_tx.slot[p_tx.n_slots] = group[i].slot + j;
      p_tx.n_slots++;
    }
  }

  if (p_tx.n_slots > N_SLOTS_MAX) {
    printf("Number of slots is limited to %u!\n", N_SLOTS_MAX);
    return -1;
  }

  
  for (i=0; i<p_tx.n_slots; i++) {
    for (j=0; j<N_LANES_MAX; j++)
    {
      p_tx.port_list[i][j] = xe_slot_map[mx][p_tx.slot[i]][j];
    }
    /* Check if this slot has a valid  mapping */
    for (j=0; j<N_LANES_MAX && p_tx.port_list[i][j] < 0; j++);
    if ( j >= N_LANES_MAX ) {
      printf("Slot %u is not mapped internally!\n", p_tx.slot[i]);
      return -1;
    }
    p_tx.ip_addr[i]   = (192<<24)+(168<<16)+(200<<8) + p_tx.slot[i] + 1;
  }

  ber_tx_running = 1;
  osapiSemaGive(ptin_ber_tx_sem);

  return 0;
}


/**
 *
 *
 * @author alex (1/14/2013)
 *
 * @param start_delay Delay before PRBS sampling
 * @param test_time   PRBS sampling time
 * @param n_slots     Number of slots being testes simultaneously
 * @param bp1         Base port #1 (set of 4 consecutive ports mapped to a single slot)
 * @param ip1         Remote IP address #1
 * @param bp2         Base port #2
 * @param ip2         Remote IP address #2
 * (...)
 *
 * @return int
 */
int get_rx_ber(char *file,
               int optimize_lc,
               int n_iter,
               int start_delay, int test_time, int mode,
               int main_start, int main_end, int main_step,
               int post_start, int post_end, int post_step,
               int n_groups,
               int n_slots0, int slot0,
               int n_slots1, int slot1,
               int n_slots2, int slot2,
               int n_slots3, int slot3,
               int n_slots4, int slot4,
               int n_slots5, int slot5,
               int n_slots6, int slot6)
{
  int i, j;
  struct groups {
    int n_slots;
    int slot;
  } group[N_GROUPS_MAX];

  stop = 0;

  if (ber_init_done == 0) {
    printf("BER needs to be initialized. Please run 'ber_init()'\n");
    return -1;
  }
  if (ber_rx_running) {
    printf("BER TX thread is already running!\n");
    return -1;
  }
  if (n_groups > N_GROUPS_MAX) {
    printf("Number of groups is limited to %u!\n", N_GROUPS_MAX);
    return -1;
  }
  if (file == NULL) {
    printf("Invalid file name: %.20s\n", file);
    return -1;
  }
  if (main_start > 31 || post_end > 63) {
    printf("main_start or post_end out of range! Max 31,63\n");
    return -1;
  }
  if (main_step <= 0 || post_step <= 0) {
    printf("main_step or post_step must be greater than zero!\n");
    return -1;
  }
  if (n_iter < 1 || n_iter > 1000) {
    printf("Number of iterations must be in the range of [1..999]\n");
    return -1;
  }

  strcpy(p_rx.file, file);
  p_rx.optimize_lc = optimize_lc;
  p_rx.n_iter      = n_iter;
  p_rx.start_delay = start_delay;
  p_rx.test_time   = test_time;
  p_rx.mode        = mode;
  p_rx.main_start  = main_start;
  p_rx.main_end    = main_end;
  p_rx.main_step   = main_step;
  p_rx.post_start  = post_start;
  p_rx.post_end    = post_end;
  p_rx.post_step   = post_step;

  group[0].n_slots = n_slots0;
  group[0].slot    = slot0;
  group[1].n_slots = n_slots1;
  group[1].slot    = slot1;
  group[2].n_slots = n_slots2;
  group[2].slot    = slot2;
  group[3].n_slots = n_slots3;
  group[3].slot    = slot3;
  group[4].n_slots = n_slots4;
  group[4].slot    = slot4;
  group[5].n_slots = n_slots5;
  group[5].slot    = slot5;
  group[6].n_slots = n_slots6;
  group[6].slot    = slot6;

  p_rx.n_slots = 0;
  for (i=0; i<n_groups; i++) {
    for (j=0; j<group[i].n_slots; j++) {
      p_rx.slot[p_rx.n_slots] = group[i].slot + j;
      p_rx.n_slots++;
    }
  }

  if (p_rx.n_slots > N_SLOTS_MAX) {
    printf("Number of slots is limited to %u!\n", N_SLOTS_MAX);
    return -1;
  }

  for (i=0; i<p_rx.n_slots; i++) {
    for ( j=0; j<N_LANES_MAX; j++)
    {
      p_rx.port_list[i][j] = xe_slot_map[mx][p_rx.slot[i]][j];
    }
    for ( j=0; j<N_LANES_MAX && p_rx.port_list[i][j]<0; j++);
    if ( j >= N_LANES_MAX ) {
      printf("Slot %u is not mapped internally!\n", p_rx.slot[i]);
      return -1;
    }
    p_rx.ip_addr[i]   = (192<<24)+(168<<16)+(200<<8) + p_rx.slot[i] + 1;
  }

  ber_rx_running = 1;
  osapiSemaGive(ptin_ber_rx_sem);

  return 0;
}

/**
 * Init remote ber in a specific slot
 * 
 * @param n_groups 
 * @param n_slots0 
 * @param slot0 
 * @param n_slots1 
 * @param slot1 
 * @param n_slots2 
 * @param slot2 
 * @param n_slots3 
 * @param slot3 
 * @param n_slots4 
 * @param slot4 
 * @param n_slots5 
 * @param slot5 
 * @param n_slots6 
 * @param slot6 
 * 
 * @return int 
 */
int init_remote_ber(int enable,
                    int n_groups,
                    int n_slots0, int slot0,
                    int n_slots1, int slot1,
                    int n_slots2, int slot2,
                    int n_slots3, int slot3,
                    int n_slots4, int slot4,
                    int n_slots5, int slot5,
                    int n_slots6, int slot6)
{
  int i, j;
  struct groups {
    int n_slots;
    int slot;
  } group[N_GROUPS_MAX];
  int n_slots;
  int slot[N_SLOTS_MAX];
  unsigned int ip_addr[N_SLOTS_MAX];
  ipc_msg txmsg, rxmsg;
  int ret;

//if (ber_init_done == 0) {
//  printf("BER needs to be initialized. Please run 'ber_init()'\n");
//  return -1;
//}
  if (n_groups > N_GROUPS_MAX) {
    printf("Number of groups is limited to %u!\n", N_GROUPS_MAX);
    return -1;
  }

  group[0].n_slots = n_slots0;
  group[0].slot    = slot0;
  group[1].n_slots = n_slots1;
  group[1].slot    = slot1;
  group[2].n_slots = n_slots2;
  group[2].slot    = slot2;
  group[3].n_slots = n_slots3;
  group[3].slot    = slot3;
  group[4].n_slots = n_slots4;
  group[4].slot    = slot4;
  group[5].n_slots = n_slots5;
  group[5].slot    = slot5;
  group[6].n_slots = n_slots6;
  group[6].slot    = slot6;

  n_slots = 0;
  for (i=0; i<n_groups; i++) {
    for (j=0; j<group[i].n_slots; j++) {
      slot[n_slots] = group[i].slot + j;
      n_slots++;
    }
  }

  if (n_slots > 18) {
    printf("Number of slots is limited to %u!\n", 18);
    return -1;
  }

  for (i=0; i<n_slots; i++) {
    ip_addr[i] = (192<<24)+(168<<16)+(200<<8) + slot[i] + 1;
  }

  /* Compose TX message */
  txmsg.protocolId   = 1;
  txmsg.srcId        = 7100;
  txmsg.dstId        = 6100;
  txmsg.flags        = 0;
  txmsg.counter      = rand ();
  txmsg.msgId        = 0x530;
  txmsg.infoDim      = 3;
  txmsg.info[0] = mx;
  txmsg.info[1] = 0;
  txmsg.info[2] = 0;

  /* For each slot, get remote values (just to reset them!) (4 ports at once) */
  for (i=0; i<n_slots; i++)
  {
    txmsg.info[1] = 0xff;   /* All 4 lanes */
    txmsg.info[2] = enable & 1;
    ret = send_data (canal_ipc, 6100, ip_addr[i], &txmsg, &rxmsg);
    if (ret != 0) {
      printf("Error initing remote BER in slot %u\n", slot[i]);
      return -1;
    }
    if (rxmsg.flags != 0x01)
    {
      printf("Request not acknowledged in slot %u\n", slot[i]);
      continue;
    }
    printf("Success %s BER tx/rx in slot %u\n",
           ((enable) ? "enabling" : "disabling"),
           slot[i]);
    usleep(1*1000);
  }

  printf("\nDone!\n");

  return 0;
}

/**
 * Enable remote ber in a specific slot
 * 
 * @param n_groups 
 * @param n_slots0 
 * @param slot0 
 * @param n_slots1 
 * @param slot1 
 * @param n_slots2 
 * @param slot2 
 * @param n_slots3 
 * @param slot3 
 * @param n_slots4 
 * @param slot4 
 * @param n_slots5 
 * @param slot5 
 * @param n_slots6 
 * @param slot6 
 * 
 * @return int 
 */
int enable_remote_ber(int n_groups,
                      int n_slots0, int slot0,
                      int n_slots1, int slot1,
                      int n_slots2, int slot2,
                      int n_slots3, int slot3,
                      int n_slots4, int slot4,
                      int n_slots5, int slot5,
                      int n_slots6, int slot6)
{
  return init_remote_ber( L7_TRUE,
                          n_groups,
                          n_slots0, slot0,
                          n_slots1, slot1,
                          n_slots2, slot2,
                          n_slots3, slot3,
                          n_slots4, slot4,
                          n_slots5, slot5,
                          n_slots6, slot6);
}

/**
 * Disable remote ber in a specific slot
 * 
 * @param n_groups 
 * @param n_slots0 
 * @param slot0 
 * @param n_slots1 
 * @param slot1 
 * @param n_slots2 
 * @param slot2 
 * @param n_slots3 
 * @param slot3 
 * @param n_slots4 
 * @param slot4 
 * @param n_slots5 
 * @param slot5 
 * @param n_slots6 
 * @param slot6 
 * 
 * @return int 
 */
int disable_remote_ber(int n_groups,
                       int n_slots0, int slot0,
                       int n_slots1, int slot1,
                       int n_slots2, int slot2,
                       int n_slots3, int slot3,
                       int n_slots4, int slot4,
                       int n_slots5, int slot5,
                       int n_slots6, int slot6)
{
  return init_remote_ber( L7_FALSE,
                          n_groups,
                          n_slots0, slot0,
                          n_slots1, slot1,
                          n_slots2, slot2,
                          n_slots3, slot3,
                          n_slots4, slot4,
                          n_slots5, slot5,
                          n_slots6, slot6);
}

/**
 * Read ber errors in remote linecards
 * 
 * @param n_groups 
 * @param n_slots0 
 * @param slot0 
 * @param n_slots1 
 * @param slot1 
 * @param n_slots2 
 * @param slot2 
 * @param n_slots3 
 * @param slot3 
 * @param n_slots4 
 * @param slot4 
 * @param n_slots5 
 * @param slot5 
 * @param n_slots6 
 * @param slot6 
 * 
 * @return int 
 */
int get_remote_ber( int n_groups,
                    int n_slots0, int slot0,
                    int n_slots1, int slot1,
                    int n_slots2, int slot2,
                    int n_slots3, int slot3,
                    int n_slots4, int slot4,
                    int n_slots5, int slot5,
                    int n_slots6, int slot6)
{
  int i, j;
  struct groups {
    int n_slots;
    int slot;
  } group[N_GROUPS_MAX];
  int n_slots;
  int slot[N_SLOTS_MAX];
  unsigned int ip_addr[N_SLOTS_MAX];
  unsigned int results[N_SLOTS_MAX][N_LANES_MAX];
  ipc_msg txmsg, rxmsg;
  int ret;

//if (ber_init_done == 0) {
//  printf("BER needs to be initialized. Please run 'ber_init()'\n");
//  return -1;
//}
  if (n_groups > N_GROUPS_MAX) {
    printf("Number of groups is limited to %u!\n", N_GROUPS_MAX);
    return -1;
  }

  group[0].n_slots = n_slots0;
  group[0].slot    = slot0;
  group[1].n_slots = n_slots1;
  group[1].slot    = slot1;
  group[2].n_slots = n_slots2;
  group[2].slot    = slot2;
  group[3].n_slots = n_slots3;
  group[3].slot    = slot3;
  group[4].n_slots = n_slots4;
  group[4].slot    = slot4;
  group[5].n_slots = n_slots5;
  group[5].slot    = slot5;
  group[6].n_slots = n_slots6;
  group[6].slot    = slot6;

  n_slots = 0;
  for (i=0; i<n_groups; i++) {
    for (j=0; j<group[i].n_slots; j++) {
      slot[n_slots] = group[i].slot + j;
      n_slots++;
    }
  }

  if (n_slots > 18) {
    printf("Number of slots is limited to %u!\n", 18);
    return -1;
  }

  for (i=0; i<n_slots; i++) {
    ip_addr[i] = (192<<24)+(168<<16)+(200<<8) + slot[i] + 1;
  }

  /* Compose TX message */
  txmsg.protocolId   = 1;
  txmsg.srcId        = 7100;
  txmsg.dstId        = 6100;
  txmsg.flags        = 0;
  txmsg.counter      = rand ();
  txmsg.msgId        = 0x531;
  txmsg.infoDim      = 1;
  txmsg.info[0]      = mx;

  memset(results, 0xff, sizeof(results));

  printf("Reading BER results in remote linecards (n_slots=%u)\n",n_slots);

  for (i=0; i<n_slots; i++) {
    printf(" slot = %-2u  IP = 0x%08X\n", slot[i], ip_addr[i]);
  }

  /* For each slot, get remote values (just to reset them!) (4 ports at once) */
  for (i=0; i<n_slots; i++) {
    memset(rxmsg.info, 0xFF, 4*sizeof(int));
    ret = send_data (canal_ipc, 6100, ip_addr[i], &txmsg, &rxmsg);
    if (ret != 0) {
      printf("Error getting remote values from slot %u\n", slot[i]);
      return -1;
    }
    if (rxmsg.flags != 0x01)
    {
      printf("Request not acknowledged in slot %u\n", slot[i]);
      continue;
    }

    results[i][0] = ((unsigned int *)rxmsg.info)[0];
    results[i][1] = ((unsigned int *)rxmsg.info)[1];
    results[i][2] = ((unsigned int *)rxmsg.info)[2];
    results[i][3] = ((unsigned int *)rxmsg.info)[3];

    usleep(1*1000);
  }

  printf("+------+--------+--------+--------+--------+\n");
  printf("| slot | lane 0 | lane 1 | lane 2 | lane 3 |\n");
  printf("+------+--------+--------+--------+--------+\n");
  for (i=0; i<n_slots; i++) {
    printf("|  %-2u  | %6u | %6u | %6u | %6u |\n",
            slot[i], results[i][0], results[i][1], results[i][2], results[i][3]);
  }
  printf("+------+--------+--------+--------+--------+\n");

  printf("\nDone!\n");

  return 0;
}


/**
 * Read from a Vitesse register in a linarcard
 * 
 * @param ip_addr : IP address of the linecard
 * @param port : Vitesse port (0xff for all)
 * @param mmd  : MMD block
 * @param addr : Register address
 * @param values   : Values read back (output)
 * @param n_values : Number of values read back (output)
 * 
 * @return int : 0=Success; -1=NAK; -2:Transmission error
 */
int remote_reg_read(unsigned int ip_addr, int port, int mmd, int addr, unsigned int *values, unsigned int *n_values)
{
  ipc_msg txmsg, rxmsg;
  int ret;
  struct message {
    unsigned char mx;
    unsigned char mmd;
    unsigned short addr;
    unsigned char port;
  } __attribute__((packed)) *pmsg;

  /* Compose TX message */
  txmsg.protocolId   = 1;
  txmsg.srcId        = 7100;
  txmsg.dstId        = 6100;
  txmsg.flags        = 0;
  txmsg.counter      = rand ();
  txmsg.msgId        = 0x53A;
  txmsg.infoDim      = sizeof(struct message);
  pmsg = (struct message *)txmsg.info;
  pmsg->mx   = mx & 1;
  pmsg->mmd  = mmd & 0xff;
  pmsg->addr = addr;
  pmsg->port = (port>=0 && port<=3) ? port : 0xff;

  /* If mmd has active the 9th bit, make direct channel referencing */
  if ( (mmd & 0x100) )
  {
    pmsg->mx = (mx & 1) + 2;
  }

  /* Send request */
  ret = send_data (canal_ipc, 6100, ip_addr, &txmsg, &rxmsg);

  usleep(1*1000);

  /* Check success */
  if (ret != 0) {
    return -2;
  }
  if (rxmsg.flags != 0x01)
  {
    return -1;
  }

  /* Copy values to output */
  if (values!=L7_NULLPTR)
  {
    memcpy(values, &rxmsg.info[0], rxmsg.infoDim);
  }
  if (n_values!=L7_NULLPTR)
  {
    *n_values = rxmsg.infoDim / sizeof(unsigned int);
  }

  /* Success */
  return 0;
}

/**
 * Write to a Vitesse register in a linarcard
 * 
 * @param ip_addr : IP address of the linecard
 * @param port : Vitesse port (0xff for all)
 * @param mmd  : MMD block
 * @param addr : Register address
 * @param value: Values to be written
 * 
 * @return int : 0=Success; -1=NAK; -2:Transmission error
 */
int remote_reg_write(unsigned int ip_addr, int port, int mmd, int addr, int value)
{
  ipc_msg txmsg, rxmsg;
  int ret;
  struct message {
    unsigned char mx;
    unsigned char mmd;
    unsigned short addr;
    unsigned short value;
    unsigned char port;
  } __attribute__((packed)) *pmsg;

  /* Compose TX message */
  txmsg.protocolId   = 1;
  txmsg.srcId        = 7100;
  txmsg.dstId        = 6100;
  txmsg.flags        = 0;
  txmsg.counter      = rand ();
  txmsg.msgId        = 0x53B;
  txmsg.infoDim      = sizeof(struct message);
  pmsg = (struct message *)txmsg.info;
  pmsg->mx   = mx & 1;
  pmsg->mmd  = mmd & 0xff;
  pmsg->addr = addr;
  pmsg->value= value;
  pmsg->port = (port>=0 && port<=3) ? port : 0xff;

  /* If mmd has active the 9th bit, make direct channel referencing */
  if ( (mmd & 0x100) )
  {
    pmsg->mx = (mx & 1) + 2;
  }

  /* Send message */
  ret = send_data (canal_ipc, 6100, ip_addr, &txmsg, &rxmsg);

  usleep(1*1000);

  if (ret != 0)
  {
    return -2;
  }
  if (rxmsg.flags != 0x01)
  {
    return -1;
  }

  /* Success */
  return 0;
}

/**
 * Read from a Vitesse firmware variable in a linecard
 * 
 * @param ip_addr : IP address of the linecard
 * @param port : Vitesse port (0xff for all)
 * @param mmd  : 0 to read current matrix ports; 
 *               1 to read the other matrix ports.
 * @param addr : Register address
 * @param values   : Values read back (output)
 * @param n_values : Number of values read back (output)
 * 
 * @return int : 0=Success; -1=NAK; -2:Transmission error
 */
int remote_var_read(unsigned int ip_addr, int port, int mmd, int addr, unsigned int *values, unsigned int *n_values)
{
  ipc_msg txmsg, rxmsg;
  int ret;
  struct message {
    unsigned char mx;
    unsigned char mmd;
    unsigned short addr;
    unsigned char port;
  } __attribute__((packed)) *pmsg;

  /* Compose TX message */
  txmsg.protocolId   = 1;
  txmsg.srcId        = 7100;
  txmsg.dstId        = 6100;
  txmsg.flags        = 0;
  txmsg.counter      = rand ();
  txmsg.msgId        = 0x53C;
  txmsg.infoDim      = sizeof(struct message);
  pmsg = (struct message *)txmsg.info;
  pmsg->mx   = mx & 1;
  pmsg->mmd  = 0;
  pmsg->addr = addr;
  pmsg->port = (port>=0 && port<=3) ? port : 0xff;

  /* If mmd is not null, make direct channel referencing */
  if ( mmd )
  {
    pmsg->mx = (mx & 1) + 2;
  }

  /* Send request */
  ret = send_data (canal_ipc, 6100, ip_addr, &txmsg, &rxmsg);

  usleep(1*1000);

  /* Check success */
  if (ret != 0) {
    return -2;
  }
  if (rxmsg.flags != 0x01)
  {
    return -1;
  }

  /* Copy values to output */
  if (values!=L7_NULLPTR)
  {
    memcpy(values, &rxmsg.info[0], rxmsg.infoDim);
  }
  if (n_values!=L7_NULLPTR)
  {
    *n_values = rxmsg.infoDim / sizeof(unsigned int);
  }

  /* Success */
  return 0;
}

/**
 * Write to a Vitesse firmware variable in a linecard
 * 
 * @param ip_addr : IP address of the linecard
 * @param port : Vitesse port (0xff for all)
 * @param mmd  : 0 to read current matrix ports; 
 *               1 to read the other matrix ports.
 * @param addr : Register address
 * @param value: Value to be written
 * 
 * @return int : 0=Success; -1=NAK; -2:Transmission error
 */
int remote_var_write(unsigned int ip_addr, int port, int mmd, int addr, int value)
{
  ipc_msg txmsg, rxmsg;
  int ret;
  struct message {
    unsigned char mx;
    unsigned char mmd;
    unsigned short addr;
    unsigned short value;
    unsigned char port;
  } __attribute__((packed)) *pmsg;

  /* Compose TX message */
  txmsg.protocolId   = 1;
  txmsg.srcId        = 7100;
  txmsg.dstId        = 6100;
  txmsg.flags        = 0;
  txmsg.counter      = rand ();
  txmsg.msgId        = 0x53D;
  txmsg.infoDim      = sizeof(struct message);
  pmsg = (struct message *)txmsg.info;
  pmsg->mx   = mx & 1;
  pmsg->mmd  = 0;
  pmsg->addr = addr;
  pmsg->value= value;
  pmsg->port = (port>=0 && port<=3) ? port : 0xff;

  /* If mmd is not null, make direct channel referencing */
  if ( mmd == 1 )
  {
    pmsg->mx = (mx & 1) + 2;
  }
  else if ( mmd >= 2 )
  {
    pmsg->mx = 0xff;
  }

  /* Send message */
  ret = send_data (canal_ipc, 6100, ip_addr, &txmsg, &rxmsg);

  usleep(1*1000);

  if (ret != 0)
  {
    return -2;
  }
  if (rxmsg.flags != 0x01)
  {
    return -1;
  }

  /* Success */
  return 0;
}

/**
 * Read register on Remote linecard (Vitesse device)
 * 
 * @param file 
 * @param mmd 
 * @param addr 
 * @param n_groups 
 * @param n_slots0 
 * @param slot0 
 * @param n_slots1 
 * @param slot1 
 * @param n_slots2 
 * @param slot2 
 * @param n_slots3 
 * @param slot3 
 * @param n_slots4 
 * @param slot4 
 * @param n_slots5 
 * @param slot5 
 * @param n_slots6 
 * @param slot6 
 * 
 * @return int 
 */
int get_remote_reg(int mmd, int addr,
                   int n_groups,
                   int n_slots0, int slot0,
                   int n_slots1, int slot1,
                   int n_slots2, int slot2,
                   int n_slots3, int slot3,
                   int n_slots4, int slot4,
                   int n_slots5, int slot5,
                   int n_slots6, int slot6)
{
  int i, j;
  struct groups {
    int n_slots;
    int slot;
  } group[N_GROUPS_MAX];
  int n_slots;
  int slot[N_SLOTS_MAX];
  unsigned int ip_addr[N_SLOTS_MAX];
  unsigned int results[N_SLOTS_MAX][N_LANES_MAX], n_results;
  int ret;

//if (ber_init_done == 0) {
//  printf("BER needs to be initialized. Please run 'ber_init()'\n");
//  return -1;
//}
  if (n_groups > N_GROUPS_MAX) {
    printf("Number of groups is limited to %u!\n", N_GROUPS_MAX);
    return -1;
  }

  group[0].n_slots = n_slots0;
  group[0].slot    = slot0;
  group[1].n_slots = n_slots1;
  group[1].slot    = slot1;
  group[2].n_slots = n_slots2;
  group[2].slot    = slot2;
  group[3].n_slots = n_slots3;
  group[3].slot    = slot3;
  group[4].n_slots = n_slots4;
  group[4].slot    = slot4;
  group[5].n_slots = n_slots5;
  group[5].slot    = slot5;
  group[6].n_slots = n_slots6;
  group[6].slot    = slot6;

  n_slots = 0;
  for (i=0; i<n_groups; i++) {
    for (j=0; j<group[i].n_slots; j++) {
      slot[n_slots] = group[i].slot + j;
      n_slots++;
    }
  }

  if (n_slots > 18) {
    printf("Number of slots is limited to %u!\n", 18);
    return -1;
  }

  for (i=0; i<n_slots; i++) {
    ip_addr[i] = (192<<24)+(168<<16)+(200<<8) + slot[i] + 1;
  }

  printf("Reading remote register\n");
  printf(" mmd     = %u\n"
         " addr    = 0x%04X\n"
         " n_slots = %u\n",
         mmd & 0xff, addr, n_slots);

  for (i=0; i<n_slots; i++) {
    printf(" slot = %-2u  IP = 0x%08X\n",
            slot[i], ip_addr[i]);
  }

  /* Reset results */
  memset(results, 0xff, sizeof(results));

  /* For each slot, get remote values (just to reset them!) (4 ports at once) */
  for (i=0; i<n_slots; i++) {

    ret = remote_reg_read(ip_addr[i], -1, mmd, addr, results[i], &n_results);

    if (ret != 0 || n_results != 4) {
      printf("Error getting remote values from slot %u\n", slot[i]);
      return -1;
    }

    usleep(1*1000);
  }

  printf("+------+--------+--------+--------+--------+\n");
  printf("| slot | lane 0 | lane 1 | lane 2 | lane 3 |\n");
  printf("+------+--------+--------+--------+--------+\n");
  for (i=0; i<n_slots; i++) {
    printf("|  %-2u  | 0x%04X | 0x%04X | 0x%04X | 0x%04X |\n",
            slot[i], results[i][0], results[i][1], results[i][2], results[i][3]);
  }
  printf("+------+--------+--------+--------+--------+\n");

  printf("\nDone!\n");

  return 0;
}

/**
 * Write register on remote linecard (Vitesse device)
 * 
 * @author mruas (2/12/2013)
 * 
 * @param mmd 
 * @param addr 
 * @param value 
 * @param n_groups 
 * @param n_slots0 
 * @param slot0 
 * @param n_slots1 
 * @param slot1 
 * @param n_slots2 
 * @param slot2 
 * @param n_slots3 
 * @param slot3 
 * @param n_slots4 
 * @param slot4 
 * @param n_slots5 
 * @param slot5 
 * @param n_slots6 
 * @param slot6 
 * 
 * @return int 
 */
int set_remote_reg(int mmd, int addr, int value,
                   int n_groups,
                   int n_slots0, int slot0,
                   int n_slots1, int slot1,
                   int n_slots2, int slot2,
                   int n_slots3, int slot3,
                   int n_slots4, int slot4,
                   int n_slots5, int slot5,
                   int n_slots6, int slot6)
{
  int i, j;
  struct groups {
    int n_slots;
    int slot;
  } group[N_GROUPS_MAX];
  int n_slots;
  int slot[N_SLOTS_MAX];
  unsigned int ip_addr[N_SLOTS_MAX];
  int ret;


//if (ber_init_done == 0) {
//  printf("BER needs to be initialized. Please run 'ber_init()'\n");
//  return -1;
//}
  if (n_groups > N_GROUPS_MAX) {
    printf("Number of groups is limited to %u!\n", N_GROUPS_MAX);
    return -1;
  }

  group[0].n_slots = n_slots0;
  group[0].slot    = slot0;
  group[1].n_slots = n_slots1;
  group[1].slot    = slot1;
  group[2].n_slots = n_slots2;
  group[2].slot    = slot2;
  group[3].n_slots = n_slots3;
  group[3].slot    = slot3;
  group[4].n_slots = n_slots4;
  group[4].slot    = slot4;
  group[5].n_slots = n_slots5;
  group[5].slot    = slot5;
  group[6].n_slots = n_slots6;
  group[6].slot    = slot6;

  n_slots = 0;
  for (i=0; i<n_groups; i++) {
    for (j=0; j<group[i].n_slots; j++) {
      slot[n_slots] = group[i].slot + j;
      n_slots++;
    }
  }

  if (n_slots > 18) {
    printf("Number of slots is limited to %u!\n", 18);
    return -1;
  }

  for (i=0; i<n_slots; i++) {
    ip_addr[i] = (192<<24)+(168<<16)+(200<<8) + slot[i] + 1;
  }

  printf("Writing remote register\n");
  printf(" mmd     = %u\n"
         " addr    = 0x%04X\n"
         " value   = 0x%04X\n"
         " n_slots = %u\n",
         mmd & 0xff, addr, value, n_slots);

  for (i=0; i<n_slots; i++) {
    printf(" slot = %-2u  IP = 0x%08X\n",
           slot[i], ip_addr[i]);
  }

  /* For each slot, get remote values (just to reset them!) (4 ports at once) */
  for (i=0; i<n_slots; i++) {

    ret = remote_reg_write(ip_addr[i], -1, mmd, addr, value);

    if (ret != 0) {
      printf("Error getting remote values from slot %u\n", slot[i]);
      return -1;
    }

    usleep(1*1000);
  }

  printf("\nDone!\n");

  return 0;
}

/**
 * Read fw variable on Remote linecard (Vitesse device)
 * 
 * @param file 
 * @param addr 
 * @param n_groups 
 * @param n_slots0 
 * @param slot0 
 * @param n_slots1 
 * @param slot1 
 * @param n_slots2 
 * @param slot2 
 * @param n_slots3 
 * @param slot3 
 * @param n_slots4 
 * @param slot4 
 * @param n_slots5 
 * @param slot5 
 * @param n_slots6 
 * @param slot6 
 * 
 * @return int 
 */
int get_remote_var(int mmd, int addr,
                   int n_groups,
                   int n_slots0, int slot0,
                   int n_slots1, int slot1,
                   int n_slots2, int slot2,
                   int n_slots3, int slot3,
                   int n_slots4, int slot4,
                   int n_slots5, int slot5,
                   int n_slots6, int slot6)
{
  int i, j;
  struct groups {
    int n_slots;
    int slot;
  } group[N_GROUPS_MAX];
  int n_slots;
  int slot[N_SLOTS_MAX];
  unsigned int ip_addr[N_SLOTS_MAX];
  unsigned int results[N_SLOTS_MAX][N_LANES_MAX], n_results;
  int ret;

//if (ber_init_done == 0) {
//  printf("BER needs to be initialized. Please run 'ber_init()'\n");
//  return -1;
//}
  if (n_groups > N_GROUPS_MAX) {
    printf("Number of groups is limited to %u!\n", N_GROUPS_MAX);
    return -1;
  }

  group[0].n_slots = n_slots0;
  group[0].slot    = slot0;
  group[1].n_slots = n_slots1;
  group[1].slot    = slot1;
  group[2].n_slots = n_slots2;
  group[2].slot    = slot2;
  group[3].n_slots = n_slots3;
  group[3].slot    = slot3;
  group[4].n_slots = n_slots4;
  group[4].slot    = slot4;
  group[5].n_slots = n_slots5;
  group[5].slot    = slot5;
  group[6].n_slots = n_slots6;
  group[6].slot    = slot6;

  n_slots = 0;
  for (i=0; i<n_groups; i++) {
    for (j=0; j<group[i].n_slots; j++) {
      slot[n_slots] = group[i].slot + j;
      n_slots++;
    }
  }

  if (n_slots > 18) {
    printf("Number of slots is limited to %u!\n", 18);
    return -1;
  }

  for (i=0; i<n_slots; i++) {
    ip_addr[i] = (192<<24)+(168<<16)+(200<<8) + slot[i] + 1;
  }

  printf("Reading remote fw variable\n");
  printf(" addr    = 0x%02X\n"
         " n_slots = %u\n",
          addr, n_slots);

  for (i=0; i<n_slots; i++) {
    printf(" slot = %-2u  IP = 0x%08X\n",
            slot[i], ip_addr[i]);
  }

  /* Reset results */
  memset(results, 0xff, sizeof(results));

  /* For each slot, get remote values (just to reset them!) (4 ports at once) */
  for (i=0; i<n_slots; i++) {

    ret = remote_var_read(ip_addr[i], -1, mmd, addr, results[i], &n_results);

    if (ret != 0 || n_results != 4)
    {
      printf("Error getting remote values from slot %u\n", slot[i]);
      return -1;
    }

    usleep(1*1000);
  }

  printf("+------+--------+--------+--------+--------+\n");
  printf("| slot | lane 0 | lane 1 | lane 2 | lane 3 |\n");
  printf("+------+--------+--------+--------+--------+\n");
  for (i=0; i<n_slots; i++) {
    printf("|  %-2u  | 0x%04X | 0x%04X | 0x%04X | 0x%04X |\n",
            slot[i], results[i][0], results[i][1], results[i][2], results[i][3]);
  }
  printf("+------+--------+--------+--------+--------+\n");

  printf("\nDone!\n");

  return 0;
}

/**
 * Write to fw variable on remote linecard (Vitesse device)
 * 
 * @param addr 
 * @param value 
 * @param n_groups 
 * @param n_slots0 
 * @param slot0 
 * @param n_slots1 
 * @param slot1 
 * @param n_slots2 
 * @param slot2 
 * @param n_slots3 
 * @param slot3 
 * @param n_slots4 
 * @param slot4 
 * @param n_slots5 
 * @param slot5 
 * @param n_slots6 
 * @param slot6 
 * 
 * @return int 
 */
int set_remote_var(int mmd, int addr, int value,
                   int n_groups,
                   int n_slots0, int slot0,
                   int n_slots1, int slot1,
                   int n_slots2, int slot2,
                   int n_slots3, int slot3,
                   int n_slots4, int slot4,
                   int n_slots5, int slot5,
                   int n_slots6, int slot6)
{
  int i, j;
  struct groups {
    int n_slots;
    int slot;
  } group[N_GROUPS_MAX];
  int n_slots;
  int slot[N_SLOTS_MAX];
  unsigned int ip_addr[N_SLOTS_MAX];
  int ret;

//if (ber_init_done == 0) {
//  printf("BER needs to be initialized. Please run 'ber_init()'\n");
//  return -1;
//}
  if (n_groups > N_GROUPS_MAX) {
    printf("Number of groups is limited to %u!\n", N_GROUPS_MAX);
    return -1;
  }

  group[0].n_slots = n_slots0;
  group[0].slot    = slot0;
  group[1].n_slots = n_slots1;
  group[1].slot    = slot1;
  group[2].n_slots = n_slots2;
  group[2].slot    = slot2;
  group[3].n_slots = n_slots3;
  group[3].slot    = slot3;
  group[4].n_slots = n_slots4;
  group[4].slot    = slot4;
  group[5].n_slots = n_slots5;
  group[5].slot    = slot5;
  group[6].n_slots = n_slots6;
  group[6].slot    = slot6;

  n_slots = 0;
  for (i=0; i<n_groups; i++) {
    for (j=0; j<group[i].n_slots; j++) {
      slot[n_slots] = group[i].slot + j;
      n_slots++;
    }
  }

  if (n_slots > 18) {
    printf("Number of slots is limited to %u!\n", 18);
    return -1;
  }

  for (i=0; i<n_slots; i++) {
    ip_addr[i] = (192<<24)+(168<<16)+(200<<8) + slot[i] + 1;
  }

  printf("Writing remote fw variable\n");
  printf(" addr    = 0x%04X\n"
         " value   = 0x%04X\n"
         " n_slots = %u\n",
         addr, value, n_slots);

  for (i=0; i<n_slots; i++) {
    printf(" slot = %-2u  IP = 0x%08X\n",
           slot[i], ip_addr[i]);
  }

  /* For each slot, get remote values (just to reset them!) (4 ports at once) */
  for (i=0; i<n_slots; i++) {

    ret = remote_var_write(ip_addr[i], -1, mmd, addr, value);

    if (ret != 0) {
      printf("Error getting remote values from slot %u\n", slot[i]);
      return -1;
    }

    usleep(1*1000);
  }

  printf("\nDone!\n");

  return 0;
}


int ptin_ber_help(void)
{
  printf("PTIN PRBS/BER help\n"
         "\n"
         " Enable PRBS generator for a lists of ports\n"
         "   fastpath.cli m 1006 <en/dis: 0/1> <port#1> <port#2> <...>\n"
         "\n"
         " Read BER counter for a lists of ports\n"
         "   fastpath.cli m 1007 <port#1> <port#2>\n"
         "\n"
         " Initialize BER meter functions (-1 to let fw to guess)\n"
         "   fastpath.shell dev ber_init <mx_slot>\n"
         "\n"
         " Stop running BER threads\n"
         "   fastpath.shell dev ber_stop\n"
         "\n"
         " Run BER meter at RX BCM\n"
         "   fastpath.shell dev \"get_rx_ber('<file>', <optimize_lc>, <n_iters>,\n"
         "                                   <start delay>, <test time>, <mode>,\n"
         "                                   <main_start>, <main_end>, <main_step>,\n"
         "                                   <post_start>, <post_end>, <port_step>,\n"
         "                                   <n_groups>,\n"
         "                                   <n_slots0>, <start_slot0>,\n"
         "                                   <n_slots1>, <start_slot1>,\n"
         "                                   <...>)\"\n"
         " Run BER meter at RX Line Card\n"
         "   fastpath.shell dev \"get_tx_ber('<file>', <optimize_lc>, <n_iters>,\n"
         "                                   <start delay>, <test time>, <mode>,\n"
         "                                   <main_start>, <main_end>, <main_step>,\n"
         "                                   <post_start>, <post_end>, <port_step>,\n"
         "                                   <n_groups>,\n"
         "                                   <n_slots0>, <start_slot0>,\n"
         "                                   <n_slots1>, <start_slot1>,\n"
         "                                   <...>)\"\n"
         " Input parameters for get_rx_ber and get_tx_ber routines:\n"
         "   lc_optimize   : 0x00 -> No optimization\n"
         "                   0x02 -> Reset Copper mode on LC for each iteration\n"
         "   n_iters       : Number of test repetitions\n"
         "   start_delay   : Time delay in seconds before start a PRBS test\n"
         "   test_delay    : PRBS test duration\n"
         "   mode[bit 0]   : 0 -> Only decrement main cursor, after running all post cursors range;\n"
         "                   1 -> Increment post cursor and decrement main cursor simultaneously\n"
         "   mode[bit 1]   : 0 -> When <n_iters> is not null, only apply start_delay for the first iteration\n"
         "                   1 -> When <n_iters> is not null, apply start_delay for all iterations\n"
         "   mode[bit 3]   : 0 -> Update main and post cursors\n"
         "                   1 -> Do not touch in tap settings\n"
         "   mode[bit 4-7] : Minimum number of errors, to initiatiate suplementary PRBS readings (only for mode[bit 8-15] > 0)\n"
         "                   0x0: no limiar -> always repeat readings\n"
         "                   0x1: 1 error\n"
         "                   0x2: 3 errors\n"
         "                   0x3: 5 errors\n"
         "                   0x4: 10 errors\n"
         "                   0x5: 20 errors\n"
         "                   0x6: 50 errors\n"
         "                   0x7: 100 errors\n"
         "                   0x8: 200 errors\n"
         "                   0x9: 500 errors\n"
         "                   0xa: 1000 errors\n"
         "                   0xb: 2000 errors\n"
         "                   0xc: 10000 errors\n"
         "                   0xd: 20000 errors\n"
         "                   0xe: 50000 errors\n"
         "                   0xf: 65535 errors\n"
         "   mode[bit 8-15]: (Maximum) number of suplementary PRBS readings\n"
         "   main_start, main_end, main_step : Maximum and minimum main cusor (in this order), and step unit\n"
         "   post_start, post_end, post_step : Minimum and maximum post cusor (in this order), and step unit\n"
         "\n"
         " Init BER in remote linecards\n"
         "   fastpath.shell dev init_remote_ber <enable>\n"
         "                                      <n_groups>\n"
         "                                      <n_slots0> <start_slot0>\n"
         "                                      <n_slots1> <start_slot1>\n"
         "                                      <...>\n"
         "\n"
         " Enable BER in remote linecards\n"
         "   fastpath.shell dev enable_remote_ber <n_groups>\n"
         "                                        <n_slots0> <start_slot0>\n"
         "                                        <n_slots1> <start_slot1>\n"
         "                                        <...>\n"
         "\n"
         " Disable BER in remote linecards\n"
         "   fastpath.shell dev disable_remote_ber <n_groups>\n"
         "                                         <n_slots0> <start_slot0>\n"
         "                                         <n_slots1> <start_slot1>\n"
         "                                         <...>\n"
         "\n"
         " Get remote BER measurements in remote linecards\n"
         "   fastpath.shell dev get_remote_ber <n_groups>\n"
         "                                     <n_slots0> <start_slot0>\n"
         "                                     <n_slots1> <start_slot1>\n"
         "                                     <...>\n"
         "\n"
         " Read remote Vitesse register on a Line Card\n"
         " (if mmd[8] bit is set - 0x1?? - channel referencing is insensitive to failover mode)\n"
         "   fastpath.shell dev get_remote_reg <mmd> <addr>\n"
         "                                     <n_groups>\n"
         "                                     <n_slots0> <start_slot0>\n"
         "                                     <n_slots1> <start_slot1>\n"
         "                                     <...>\n"
         "\n"
         " Write remote Vitesse register on a Line Card\n"
         " (if mmd[8] bit is set - 0x1?? - channel referencing is insensitive to failover mode)\n"
         "   fastpath.shell dev set_remote_reg <mmd> <addr> <value>\n"
         "                                     <n_groups>\n"
         "                                     <n_slots0> <start_slot0>\n"
         "                                     <n_slots1> <start_slot1>\n"
         "                                     <...>\n"
         "\n"
         " Read remote Vitesse firmware variable on a Line Card\n"
         "(mmd=0: XAUI channels read; mmd=1: PMA channels read)\n"
         "   fastpath.shell dev get_remote_var <mmd> <addr>\n"
         "                                     <n_groups>\n"
         "                                     <n_slots0> <start_slot0>\n"
         "                                     <n_slots1> <start_slot1>\n"
         "                                     <...>\n"
         "\n"
         " Write remote Vitesse firmware variable on a Line Card\n"
         "(mmd=0: XAUI channels written; mmd=1: PMA channels written; mmd=2: both channels pair written)\n"
         "   fastpath.shell dev set_remote_var <mmd> <addr> <value>\n"
         "                                     <n_groups>\n"
         "                                     <n_slots0> <start_slot0>\n"
         "                                     <n_slots1> <start_slot1>\n"
         "                                     <...>\n"
         "\n"
         " Example commands:\n"
         "  fastpath.shell dev \"get_remote_reg('test_reg.txt',1,0x8037,2,5,2,11,8)\"\n"
         "  fastpath.shell dev \"get_tx_ber('test_tx.log',0,0,1,63,32,1,0,31,1,2,11,3,5,15)\"\n"
         "  fastpath.shell dev \"get_rx_ber('test_rx.log',0,0,0,31,31,1,0,31,1,2,11,3,5,15)\"\n"
         "  fastpath.cli m 1007 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63\n"
         "\n"
         );

  return 0;
}

#else

int ptin_ber_help(void)
{
  printf("\nPRBS/BER tests are not supported for this board!\n");

  return 0;
}

int ber_init(void)
{
  return ptin_ber_help();
}

int init_remote_ber(void)
{
  return ptin_ber_help();
}

#endif

int ber_help(void)
{
  return ptin_ber_help();
}

