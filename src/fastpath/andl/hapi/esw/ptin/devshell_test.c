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

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS || hapi_ptin_bcmPort_get(port,&bcm_port)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_port_control_set(0,bcm_port,type,arg);

  printf("bcm_port_control_set(0,%d,%d,%d) => %d (\"%s\")\r\n",bcm_port,type,arg,error,bcm_errmsg(error));

  return error;
}


int ptin_port_control_get(int port, bcm_port_control_t type)
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

  return(0);;
}

