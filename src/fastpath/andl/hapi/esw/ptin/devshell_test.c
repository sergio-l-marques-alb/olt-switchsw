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

#include "ptin_fpga_api.h"
#include "hpc_db.h"
#include "dapi_db.h"
#include "logger.h"

extern DAPI_t *dapi_g;


int ptin_port_stat(bcm_port_t bcm_port)
{
  bcm_port_info_t info;
  bcm_error_t rv;

  bcm_port_info_t_init(&info);
  info.action_mask  = 0xffffffff;
  info.action_mask2 = 0xffffffff;

  rv = bcm_port_selective_get(0, bcm_port, &info);

  if (rv != L7_SUCCESS)
  {
    printf("Error reading port status\r\n");
    return -1;
  }

  printf("bcm_port %u:\r\n",  bcm_port);
  printf(" enable             = %u\r\n", info.enable             );
  printf(" linkstatus         = %u\r\n", info.linkstatus         );
  printf(" autoneg            = %u\r\n", info.autoneg            );
  printf(" speed              = %u\r\n", info.speed              );
  printf(" duplex             = %u\r\n", info.duplex             );
  printf(" linkscan           = %u\r\n", info.linkscan           );
  printf(" learn              = %u\r\n", info.learn              );
  printf(" discard            = %u\r\n", info.discard            );
  printf(" vlanfilter         = %u\r\n", info.vlanfilter         );
  printf(" untagged_priority  = %u\r\n", info.untagged_priority  );
  printf(" untagged_vlan      = %u\r\n", info.untagged_vlan      );
  printf(" stp_state          = %u\r\n", info.stp_state          );
  printf(" pfm                = %u\r\n", info.pfm                );
  printf(" loopback           = %u\r\n", info.loopback           );
  printf(" phy_master         = %u\r\n", info.phy_master         );
  printf(" interface          = %u\r\n", info.interface          );
  printf(" pause_tx           = %u\r\n", info.pause_tx           );
  printf(" pause_rx           = %u\r\n", info.pause_rx           );
  printf(" encap_mode         = %u\r\n", info.encap_mode         );
  printf(" pause_mac          = %02u:%02u:%02u:%02u:%02u:%02u\r\n", info.pause_mac[0],info.pause_mac[1],info.pause_mac[2],info.pause_mac[3],info.pause_mac[4],info.pause_mac[5]);
  printf(" local_advert       = %u\r\n", info.local_advert       );
  printf(" local_ability      > speedHD=0x%x speedFD=0x%x pause=%u if=%u med=%u lb=%u flg=0x%x eee=%u enc=%u\r\n",
         info.local_ability.speed_half_duplex,
         info.local_ability.speed_full_duplex,
         info.local_ability.pause,
         info.local_ability.interface,
         info.local_ability.medium,
         info.local_ability.loopback,
         info.local_ability.flags,
         info.local_ability.eee,
         info.local_ability.encap);
  printf(" remote_advert_valid= %u\r\n", info.remote_advert_valid);
  printf(" remote_advert      = %u\r\n", info.remote_advert      );
  printf(" remote_ability     > speedHD=0x%x speedFD=0x%x pause=%u if=%u med=%u lb=%u flg=0x%x eee=%u enc=%u\r\n",
         info.remote_ability.speed_half_duplex,
         info.remote_ability.speed_full_duplex,
         info.remote_ability.pause,
         info.remote_ability.interface,
         info.remote_ability.medium,
         info.remote_ability.loopback,
         info.remote_ability.flags,
         info.remote_ability.eee,
         info.remote_ability.encap);
  printf(" mcast_limit        = %u\r\n", info.mcast_limit        );
  printf(" mcast_limit_enable = %u\r\n", info.mcast_limit_enable );
  printf(" bcast_limit        = %u\r\n", info.bcast_limit        );
  printf(" bcast_limit_enable = %u\r\n", info.bcast_limit_enable );
  printf(" dlfbc_limit        = %u\r\n", info.dlfbc_limit        );
  printf(" dlfbc_limit_enable = %u\r\n", info.dlfbc_limit_enable );
  printf(" speed_max          = %u\r\n", info.speed_max          );
  printf(" ability            = 0x%x\r\n", info.ability            );
  printf(" port_ability       > speedHD=0x%x speedFD=0x%x pause=%u if=%u med=%u lb=%u flg=0x%x eee=%u enc=%u\r\n",
         info.port_ability.speed_half_duplex,
         info.port_ability.speed_full_duplex,
         info.port_ability.pause,
         info.port_ability.interface,
         info.port_ability.medium,
         info.port_ability.loopback,
         info.port_ability.flags,
         info.port_ability.eee,
         info.port_ability.encap);
  printf(" frame_max          = %u\r\n", info.frame_max          );
  printf(" mdix               = %u\r\n", info.mdix               );
  printf(" mdix_status        = %u\r\n", info.mdix_status        );
  printf(" medium             = %u\r\n", info.medium             );
  printf(" fault              = 0x%x\r\n", info.fault            );
  printf(" action_mask        = 0x%x\r\n", info.action_mask);
  printf(" action_mask2       = 0x%x\r\n", info.action_mask2);
  printf("Done!");

  return 0;
}


int ptin_cosq_discard_set(unsigned int gport, unsigned int queueIndex, unsigned int flags, unsigned int minthresh, unsigned int maxthresh, unsigned int dropprob, unsigned int wredexp)
{
  int precIndex;
  bcm_cosq_gport_discard_t discardParams;
  bcm_error_t rv;

  memset(&discardParams,  0x00,  sizeof(bcm_cosq_gport_discard_t));
  //discardParams.flags             = flags;
  discardParams.min_thresh        = minthresh;
  discardParams.max_thresh        = maxthresh;
  discardParams.drop_probability  = dropprob;
  discardParams.gain              = wredexp;

  for (precIndex = 0; precIndex < 6; precIndex++)
  {
    /* Flags */
    switch (precIndex) 
    {
      case 0:
        discardParams.flags = flags | (BCM_COSQ_DISCARD_COLOR_GREEN | BCM_COSQ_DISCARD_TCP);
        break;
      case 1:
        discardParams.flags = flags | (BCM_COSQ_DISCARD_COLOR_YELLOW | BCM_COSQ_DISCARD_TCP);
        break;
      case 2:
        discardParams.flags = flags | (BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_TCP);
        break;
      case 3:
        discardParams.flags = flags | (BCM_COSQ_DISCARD_COLOR_GREEN | BCM_COSQ_DISCARD_NONTCP);
        break;
      case 4:
        discardParams.flags = flags | (BCM_COSQ_DISCARD_COLOR_YELLOW | BCM_COSQ_DISCARD_NONTCP);
        break;
      case 5:
        discardParams.flags = flags | (BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_NONTCP);
        break;
      default:
        return(BCM_E_PARAM);
    }

    rv = bcm_cosq_gport_discard_set(0, gport, queueIndex, &discardParams);

    printf("precIndex=%u: rv = %d (%s)\r\n", precIndex, rv, bcm_errmsg(rv));
  }

  return rv;
}

int ptin_cosq_discard_get(unsigned int gport, unsigned int queueIndex, unsigned int flags)
{
  int precIndex;
  bcm_cosq_gport_discard_t discardParams;
  bcm_error_t rv;

  for (precIndex = 0; precIndex < 6; precIndex++)
  {
    memset(&discardParams,  0x00,  sizeof(bcm_cosq_gport_discard_t));

    switch (precIndex) 
    {
      case 0:
        discardParams.flags = flags | (BCM_COSQ_DISCARD_COLOR_GREEN | BCM_COSQ_DISCARD_TCP);
        break;
      case 1:
        discardParams.flags = flags | (BCM_COSQ_DISCARD_COLOR_YELLOW | BCM_COSQ_DISCARD_TCP);
        break;
      case 2:
        discardParams.flags = flags | (BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_TCP);
        break;
      case 3:
        discardParams.flags = flags | (BCM_COSQ_DISCARD_COLOR_GREEN | BCM_COSQ_DISCARD_NONTCP);
        break;
      case 4:
        discardParams.flags = flags | (BCM_COSQ_DISCARD_COLOR_YELLOW | BCM_COSQ_DISCARD_NONTCP);
        break;
      case 5:
        discardParams.flags = flags | (BCM_COSQ_DISCARD_COLOR_RED | BCM_COSQ_DISCARD_NONTCP);
        break;
      default:
        return(BCM_E_PARAM);
    }

    rv = bcm_cosq_gport_discard_get(0, gport, queueIndex, &discardParams);

    printf("precIndex=%u: rv = %d (%s)\r\n", precIndex, rv, bcm_errmsg(rv));
    printf("   flags=0x%08x gain=%d min_thresh=%d max_thresh=%d drop_prob=%d evc_thresh=%d refresh_time=%d\r\n",
           discardParams.flags, discardParams.gain, discardParams.min_thresh, discardParams.max_thresh,
           discardParams.drop_probability, discardParams.ecn_thresh, discardParams.refresh_time);
  }

  return rv;
}


int kt2_hqos_set(bcm_port_t port)
{
  int unit = 0;
  int cos;
  bcm_gport_t gport,port_sched, l0_sched, l1_sched;
  bcm_gport_t queue[8];
  int rv;

  /***** PART-1: HQOS creation ******/
  /* Delete the default hierarchy of given port */ 
  rv = bcm_port_gport_get(unit, port, &gport); 
  printf("%s(%d) rv=%d port=%d gport=0x%x\r\n",  __FUNCTION__,  __LINE__,  rv, port, gport);

  rv = bcm_cosq_gport_delete(unit, gport);
  printf("%s(%d) rv=%d\r\n",  __FUNCTION__,  __LINE__,  rv);

  /* Create port scheduler. specify port scheduler with 1 input */ 
  rv = bcm_cosq_gport_add(unit, port, 1, 0, &port_sched);
  printf("%s(%d) rv=%d port_sched=0x%x\r\n",  __FUNCTION__,  __LINE__,  rv, port_sched);

  /* Create L0 scheduler gport object with 1 input */ 
  rv = bcm_cosq_gport_add(unit, port,1, BCM_COSQ_GPORT_SCHEDULER, &l0_sched);
  printf("%s(%d) rv=%d l0_sched=0x%x\r\n",  __FUNCTION__,  __LINE__,  rv, l0_sched);

  /* Create L1 scheduler gport object with 1 input */ 
  rv = bcm_cosq_gport_add(unit, port,8, BCM_COSQ_GPORT_SCHEDULER, &l1_sched);
  printf("%s(%d) rv=%d l1_sched=0x%x\r\n",  __FUNCTION__,  __LINE__,  rv, l1_sched);

  /* Attach L0 gport to port scheduler at input 0 */ 
  rv = bcm_cosq_gport_attach(unit, l0_sched, port_sched, 0);
  printf("%s(%d) rv=%d l0 attached to port\r\n",  __FUNCTION__,  __LINE__,  rv);

  /* Attach L1 gport to L0 at input 0 */
  rv = bcm_cosq_gport_attach(unit, l1_sched, l0_sched,0);
  printf("%s(%d) rv=%d l1 attached to l0\r\n",  __FUNCTION__,  __LINE__,  rv);

  /* Create 8 cosq's */
  for (cos = 0; cos < 8; cos++)
  {
    rv = bcm_cosq_gport_add(unit, port, 1,BCM_COSQ_GPORT_UCAST_QUEUE_GROUP, &queue[cos]);
    printf("%s(%d) rv=%d cos=%d/gportid=0x%x created\r\n",  __FUNCTION__,  __LINE__,  rv, cos, queue[cos]);
    rv = bcm_cosq_gport_attach(unit, queue[cos], l1_sched, cos);
    printf("%s(%d) rv=%d cos=%d/gportid=0x%x attached to l1\r\n",  __FUNCTION__,  __LINE__, rv, cos, queue[cos]);
  }

  printf("Done!\r\n");

  return 0;
}

int ptin_cosq_control_get(L7_uint port, L7_uint cos, L7_uint prop)
{
  int rv, arg;

  rv = bcm_cosq_control_get(0, port, cos, prop, &arg);

  printf("bcm_cosq_control_get(0, %u, %u, %u, &arg) => arg=%u (rv=%d)\r\n", port, cos, prop, arg, rv);

  return rv;
}

int ptin_cosq_control_set(L7_uint port, L7_uint cos, L7_uint prop, L7_int val)
{
  int rv;

  rv = bcm_cosq_control_set(0, port, cos, prop, val);

  printf("bcm_cosq_control_set(0, %u, %u, %u, %d) (rv=%d)\r\n", port, cos, prop, val, rv);

  return rv;
}

int ptin_reg_get(int bcm_port, unsigned int ctr_reg, unsigned int index)
{
  uint64 value = 0;
  int rv;

     //soc_counter_get(unit, port, ctr_reg, ar_idx, &val)
  rv = soc_reg64_get(0, ctr_reg, bcm_port, index, &value);

  if ( rv != SOC_E_NONE)
  {
    printf("Error reading register: rv=%d\r\n", rv);
    //return -1;
  }

  printf("Counter = %llu\r\n",  value);

  return 0;
}

int ptin_counter_get(int bcm_port, unsigned int ctr_reg, unsigned int index)
{
  uint64 value = 0;
  int rv;

  //soc_counter_get(unit, port, ctr_reg, ar_idx, &val)
  rv = soc_counter_get(0, bcm_port, ctr_reg, index, &value);

  if ( rv != SOC_E_NONE)
  {
    printf("Error reading register: rv=%d\r\n", rv);
    //return -1;
  }

  printf("Counter = %llu\r\n",  value);

  return 0;
}

void ptin_mem_mod(uint32 mem, uint32 field, uint32 i, uint32 val)
{
  uint32    entry[SOC_MAX_MEM_WORDS];
  int rv;

  rv = soc_mem_read(0, mem, MEM_BLOCK_ANY, i, &entry);
  if (rv != 0)
  {
    printf("Error reading MMU_THDO_QCONFIG_CELL[%u] contents: rv=%d\r\n", i, rv);
    return;
  }
  //soc_mem_field32_set(0, MMU_THDO_QCONFIG_CELLm, &entry, Q_MIN_CELLf, val);/*c366, 382*/
  soc_mem_field32_set(0, mem, &entry, field, val);
}

void ptin_temperature_monitor(int number_of_sensors)
{
  bcm_error_t rv;
  int i, count=0, max_temp=-1000;
  bcm_switch_temperature_monitor_t temp_array[10];

  if (number_of_sensors == 0)
  {
    number_of_sensors = 4;
  }
  else if (number_of_sensors > 10)
  {
    number_of_sensors = 10;
  }

  printf("Going to read %u sensors...\r\n", number_of_sensors);

  rv = bcm_switch_temperature_monitor_get(0, number_of_sensors, temp_array, &count);

  printf("Obtained data from %u sensors (rv=%d)\r\n", count, rv);

  if (count > 0)
  {
    printf("Temperature (curr/peak): ");
    for (i=0; i<count; i++)
    {
      printf("[%d/%d] ", temp_array[i].curr, temp_array[i].peak);
      if (temp_array[i].curr > max_temp)  max_temp = temp_array[i].curr;
    }
    printf("units\r\n");
    printf("Temperature (curr/peak): ");
    for (i=0; i<count; i++)
    {
      printf("[%d.%d/%d.%d] ", temp_array[i].curr/10, abs(temp_array[i].curr)%10, temp_array[i].peak/10, abs(temp_array[i].peak)%10);
    }
    printf("degC\r\n");
    printf("Max. Temperature = %d.%d degC\r\n", max_temp/10, abs(max_temp)%10);
  }
}


L7_RC_t ptin_maclimit_setmax(bcm_port_t bcm_port, L7_uint16 vlanId, L7_uint32 flags, L7_int max)
{
  bcm_l2_learn_limit_t  limit;
  bcm_error_t           rv;

  bcm_l2_learn_limit_t_init(&limit);

  limit.flags = flags;

  limit.port = bcm_port;
  limit.vlan = vlanId;
  limit.limit = max;

  printf("flags  0x%.4X\r\n", limit.flags);
  printf("trunk  %u\r\n", limit.trunk);
  printf("port   %u\r\n", limit.port);
  printf("vlan   %u\r\n", limit.vlan);
  printf("limit  %u\r\n", limit.limit);

  rv=bcm_l2_learn_limit_set(0, &limit);

  if (rv != BCM_E_NONE)
  {
    printf("Error (%d) setting L2 learn limit to %u\r\n", rv, max);
  }
  else
  {
    printf("Sucessfully set L2 learn limit to %u\r\n", max);
  }

  return rv;
}


static BROAD_POLICY_t       policyId_counter[10]  = {[0 ... 9] = BROAD_POLICY_INVALID};
static BROAD_POLICY_RULE_t  ruleId_counter[10]    = {[0 ... 9] = BROAD_POLICY_INVALID};

int ptin_lookup_counter_read(L7_int index)
{
  BROAD_POLICY_STATS_t stat;
  L7_RC_t rc;

  if (index >= 10)
  {
    return L7_FAILURE;
  }

  if (policyId_counter[index] == BROAD_POLICY_INVALID || 
      ruleId_counter[index] == BROAD_POLICY_INVALID)
  {
    printf("No counter defined\r\n");
    fflush(stdout);
    return L7_FAILURE;
  }

  printf("Going to read counter\r\n");
  fflush(stdout);

  if ((rc=hapiBroadPolicyStatsGet(policyId_counter[index], ruleId_counter[index], &stat))!=L7_SUCCESS)
  {
    printf("Error reading counters\r\n");
    fflush(stdout);
    return L7_FAILURE;
  }

  printf("Packets: %llu\r\n",stat.statMode.counter.count);
  fflush(stdout);

  return L7_SUCCESS;
}

int ptin_lookup_counter_clear(L7_int index)
{
  if (index >= 10)
  {
    return L7_FAILURE;
  }

  if (policyId_counter[index] != BROAD_POLICY_INVALID)
  {
    hapiBroadPolicyDelete(policyId_counter[index]);
    policyId_counter[index] = BROAD_POLICY_INVALID;
    ruleId_counter[index] = BROAD_POLICY_INVALID;

    printf("%s(%d) Counter removed\r\n",__FUNCTION__,__LINE__);
  }

  return L7_SUCCESS;
}

int ptin_lookup_counter_set(L7_int type, L7_int stage, L7_int index, L7_int port, L7_uint16 outerVlan, L7_uint16 innerVlan)
{
  L7_uint32           bcm_unit, bcm_port, gport;
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId;
  L7_uint8  dmac[]       = {0x00, 0x00, 0xc0, 0x01, 0x01, 0x02};
  L7_uint8  mask[]       = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  L7_RC_t rc = L7_SUCCESS;

  if (stage >= BROAD_POLICY_STAGE_COUNT || index >= 10)
  {
    return L7_FAILURE;
  }

  /* Apply this rule only to GE48 port */
  if (hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, &gport) != L7_SUCCESS)
  {
    printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
    fflush(stdout);
    hapiBroadPolicyDelete(policyId);
    return L7_FAILURE;
  }

  ptin_lookup_counter_clear(index);

  hapiBroadPolicyCreate(type);
  hapiBroadPolicyStageSet(stage);
  hapiBroadPolicyRuleAdd(&ruleId);

  if (stage == BROAD_POLICY_STAGE_EGRESS)
  {
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OUTPORT, (L7_uchar8 * ) &bcm_port, mask);
    if (rc != L7_SUCCESS)
    {
      printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
      fflush(stdout);
      hapiBroadPolicyCreateCancel();
      return rc;
    }
    printf("%s(%d) Outport qualifier added\r\n",__FUNCTION__,__LINE__);
  }

  /* Outer Vlan qualifier */
  if (outerVlan >= 1 && outerVlan <=4095)
  {
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 * ) &outerVlan, mask);
    if (rc != L7_SUCCESS)
    {
      printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
      fflush(stdout);
      hapiBroadPolicyCreateCancel();
      return rc;
    }
    printf("%s(%d) Outer vlan qualifier added\r\n",__FUNCTION__,__LINE__);
  }

  /* Inner Vlan qualifier */
  if (innerVlan >= 1 && innerVlan <=4095)
  {
    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IVID, (L7_uchar8 * ) &innerVlan, mask);
    if (rc != L7_SUCCESS)
    {
      printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
      fflush(stdout);
      hapiBroadPolicyCreateCancel();
      return rc;
    }
    printf("%s(%d) Inner vlan qualifier added\r\n",__FUNCTION__,__LINE__);
  }

  rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, dmac, mask);
  if (rc != L7_SUCCESS)
  {
    printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
    fflush(stdout);
    hapiBroadPolicyCreateCancel();
    return rc;
  }
  printf("%s(%d) DMAC qualifier added\r\n",__FUNCTION__,__LINE__);

  /* Actions */
  /* Add counter */
  rc = hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
  if (rc != L7_SUCCESS)
  {
    printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
    fflush(stdout);
    hapiBroadPolicyCreateCancel();
    return rc;
  }
  
  /* Commit */
  rc = hapiBroadPolicyCommit(&policyId);
  if (rc != L7_SUCCESS)
  {
    printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
    fflush(stdout);
    hapiBroadPolicyCreateCancel();
    return rc;
  }

  if (stage == BROAD_POLICY_STAGE_LOOKUP ||
      stage == BROAD_POLICY_STAGE_INGRESS)
  {
    rc = hapiBroadPolicyApplyToIface(policyId, gport);
    if (L7_SUCCESS != rc)
    {
      printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
      fflush(stdout);
      hapiBroadPolicyDelete(policyId);
      return rc;
    }
  }

  /* Save policy id */
  policyId_counter[index] = policyId;
  ruleId_counter[index] = ruleId;

  printf("Counter applied successfully\r\n");
  fflush(stdout);

  return L7_SUCCESS;
}


static BROAD_POLICY_t policyId_pvid[PTIN_SYSTEM_N_PORTS]  = {[0 ... PTIN_SYSTEM_N_PORTS-1] = BROAD_POLICY_INVALID};

int ptin_lookup_pvid_set(L7_int port, L7_uint16 outerVlan, L7_uint16 innerVlan, L7_uint8 vlan_format)
{
  L7_uint32           bcm_unit, bcm_port, gport;
  BROAD_POLICY_t      policyId;
  BROAD_POLICY_RULE_t ruleId;
  L7_uint8  mask[]       = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  L7_RC_t rc = L7_SUCCESS;

  if (port >= PTIN_SYSTEM_N_PORTS)
  {
    return L7_FAILURE;
  }

  if (policyId_pvid[port] != BROAD_POLICY_INVALID)
  {
    hapiBroadPolicyDelete(policyId_pvid[port]);
    policyId_pvid[port] = BROAD_POLICY_INVALID;

    printf("PVID cleared successfully\r\n");
  }

  if (outerVlan >= 1 && outerVlan <= 4095)
  {
    /* Apply this rule only to GE48 port */
    if (hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, &gport) != L7_SUCCESS)
    {
      printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
      fflush(stdout);
      return L7_FAILURE;
    }

    hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PORT);
    hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_LOOKUP);
    hapiBroadPolicyRuleAdd(&ruleId);

    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_VLAN_FORMAT, (L7_uchar8 * ) &vlan_format, mask);
    if (rc != L7_SUCCESS)
    {
      printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
      fflush(stdout);
      hapiBroadPolicyCreateCancel();
      return rc;
    }
    printf("%s(%d) Vlan format qualifier added\r\n",__FUNCTION__,__LINE__);

    /* Actions */
    /* Outer Vlan qualifier */
    rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_ADD_OUTER_VID, outerVlan, 0, 0);
    if (rc != L7_SUCCESS)
    {
      printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
      fflush(stdout);
      hapiBroadPolicyCreateCancel();
      return rc;
    }
    printf("%s(%d) Outer vlan add action added\r\n",__FUNCTION__,__LINE__);

    /* Inner Vlan qualifier */
    if (innerVlan >= 1 && innerVlan <=4095)
    {
      rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_ADD_INNER_VID, innerVlan, 0, 0);
      if (rc != L7_SUCCESS)
      {
        printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
        fflush(stdout);
        hapiBroadPolicyCreateCancel();
        return rc;
      }
      printf("%s(%d) Inner vlan add action added\r\n",__FUNCTION__,__LINE__);
    }
    
    /* Commit */
    rc = hapiBroadPolicyCommit(&policyId);
    if (rc != L7_SUCCESS)
    {
      printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
      fflush(stdout);
      hapiBroadPolicyCreateCancel();
      return rc;
    }

    rc = hapiBroadPolicyApplyToIface(policyId, gport);
    if (L7_SUCCESS != rc)
    {
      printf("%s(%d) Error\r\n",__FUNCTION__,__LINE__);
      fflush(stdout);
      hapiBroadPolicyDelete(policyId);
      return rc;
    }

    /* Save policy id */
    policyId_pvid[port] = policyId;

    printf("PVID set successfully\r\n");
    fflush(stdout);
  }

  return L7_SUCCESS;
}

int ptin_l2_replace_port(bcm_port_t bcm_port_old, bcm_port_t bcm_port_new)
{
  bcm_l2_addr_t bcm_l2_addr;
  bcm_error_t rv;

  memset(&bcm_l2_addr, 0x00, sizeof(bcm_l2_addr));

  bcm_l2_addr.flags = 0;
  bcm_l2_addr.modid = 0;
  bcm_l2_addr.port  = bcm_port_old;
  bcm_l2_addr.tgid  = -1;

  if ((rv = bcm_l2_replace(0, BCM_L2_REPLACE_MATCH_DEST, &bcm_l2_addr, 0, bcm_port_new, -1)) != BCM_E_NONE)
  {
    printf("Error applying bcm_l2_replace\r\n");
    return -1;
  }

  printf("Success applying bcm_l2_replace\r\n");
  return 0;
}

int ptin_l2_replace_trunk(bcm_trunk_t tgid_old, bcm_trunk_t tgid_new)
{
  bcm_l2_addr_t bcm_l2_addr;
  bcm_error_t rv;

  memset(&bcm_l2_addr, 0x00, sizeof(bcm_l2_addr));

  bcm_l2_addr.flags = BCM_L2_TRUNK_MEMBER;
  bcm_l2_addr.modid = 0;
  bcm_l2_addr.port  = 0;
  bcm_l2_addr.tgid  = tgid_old;

  if ((rv = bcm_l2_replace(0, BCM_L2_REPLACE_MATCH_DEST | BCM_L2_REPLACE_NEW_TRUNK, &bcm_l2_addr, 0, 0, tgid_new)) != BCM_E_NONE)
  {
    printf("Error applying bcm_l2_replace to trunk\r\n");
    return -1;
  }

  printf("Success applying bcm_l2_replace to trunk\r\n");
  return 0;
}


int ptin_link_notify(bcm_port_t bcm_port)
{
  int link_status;
  bcm_gport_t gport;
  bcm_port_info_t info;
  bcm_error_t rv;

  /* FIXME: Only applied to unit 0 */
  if (bcmy_lut_unit_port_to_gport_get(0 /*unit*/, bcm_port, &gport) != BCMY_E_NONE)
  {
    printf("Error with unit %d, port %d", 0, bcm_port);
    return -1;
  }
  if (gport < 0)
  {
    printf("%s(%d) Invalid bcm_port %d\r\n", __FUNCTION__, __LINE__, bcm_port);
    return -1;
  }

  rv = bcm_port_link_status_get(0 /*unit*/, bcm_port, &link_status);
  if (rv != BCM_E_NONE)
  {
    printf("%s(%d) bcm_port_link_status_get: rv=%u (\"%s\")\r\n", __FUNCTION__, __LINE__, rv, bcm_errmsg(rv));
    return rv;
  }

  info.linkstatus = link_status;
  hapiBroadPortLinkStatusChange(0 /*unit*/, bcm_port, &info);

  printf("%s(%d) Notification sent: gport=0x%08x -> link=%d\r\n", __FUNCTION__, __LINE__, gport, link_status);

  return 0;
}

bcm_error_t ptin_linkscan_update(bcm_port_t bcm_port)
{
  bcm_error_t rv;
  bcm_pbmp_t pbmp;

  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, bcm_port);
    
  rv = bcm_linkscan_update(0, pbmp);

  printf("Executed linkscan to port %u: rv=%d -> \"%s\")\r\n", bcm_port, rv, bcm_errmsg(rv));

  return rv;
}

bcm_error_t ptin_link_change(bcm_port_t bcm_port)
{
  bcm_error_t rv;
  bcm_pbmp_t pbmp;

  BCM_PBMP_CLEAR(pbmp);
  BCM_PBMP_PORT_ADD(pbmp, bcm_port);
    
  rv = bcm_link_change(0, pbmp);

  printf("Executed bcm_link_change to port %u: rv=%d -> \"%s\")\r\n", bcm_port, rv, bcm_errmsg(rv));

  return rv;
}

bcm_error_t ptin_port_link_status_get(bcm_port_t bcm_port)
{
  int status;
  bcm_error_t rv;
    
  rv = bcm_port_link_status_get(0, bcm_port, &status);

  printf("status=%d   (rv=%d -> \"%s\")\r\n", status, rv, bcm_errmsg(rv));

  return rv;
}

bcm_error_t ptin_port_link_state_get(bcm_port_t bcm_port, uint32 flags)
{
  bcm_port_link_state_t link_state;
  bcm_error_t rv;
    
  rv = bcm_port_link_state_get(0, bcm_port, flags, &link_state);

  printf("status=%d, latch_down=%d   (rv=%d -> \"%s\")\r\n", link_state.status, link_state.latch_down, rv, bcm_errmsg(rv));

  return rv;
}

bcm_error_t ptin_port_fault_get(bcm_port_t bcm_port)
{
  uint32 flags;
  bcm_error_t rv;

  rv = bcm_port_fault_get(0, bcm_port, &flags);

  printf("bcm_port_fault_get: flags=%u   (rv=%d -> \"%s\")\r\n", flags, rv, bcm_errmsg(rv));

  return rv;
}

#include "bcm_int/esw/link.h"
#if (SDK_VERSION_IS < SDK_VERSION(6,5,18,0))
bcm_error_t ptin_link_fault_get(bcm_port_t bcm_port)
{
  uint32 flags;
  bcm_error_t rv;

  rv = _ptin_esw_link_fault_get(0, bcm_port, &flags);

  printf("_bcm_esw_link_fault_get: flags=%u   (rv=%d -> \"%s\")\r\n", flags, rv, bcm_errmsg(rv));

  return rv;
}

#endif 
bcm_error_t ptin_linkscan(bcm_port_t bcm_port)
{
  int status;
  bcm_error_t rv;
  L7_uint64 time_start, time_end[3];

  time_start = osapiTimeMillisecondsGet64();

  rv = bcm_port_link_status_get(0, bcm_port, &status);
  if (rv != BCM_E_NONE)
  {
    printf("%s(%d) bcm_port_link_status_get: rv=%u (\"%s\")\r\n", __FUNCTION__, __LINE__, rv, bcm_errmsg(rv));
    return rv;
  }

  time_end[0] = osapiTimeMillisecondsGet64();

  rv = bcm_port_update(0, bcm_port, status);
  if (rv != BCM_E_NONE)
  {
    printf("%s(%d) bcm_port_update: rv=%u (\"%s\")\r\n", __FUNCTION__, __LINE__, rv, bcm_errmsg(rv));
    return rv;
  }

  time_end[1] = osapiTimeMillisecondsGet64();

  #if 0
  rv = _soc_link_update(0);
  if (rv != SOC_E_NONE)
  {
    printf("%s(%d) bcm_port_update: rv=%u\r\n", __FUNCTION__, __LINE__, rv);
    return rv;
  }
  #endif

  time_end[2] = osapiTimeMillisecondsGet64();

  printf("%s(%d) link=%d -> Time taken: t1=%llu, t2=%llu, t3=%llu, total=%llu\r\n", __FUNCTION__, __LINE__,
         status,
         time_end[0]-time_start, time_end[1]-time_end[0], time_end[2]-time_end[1],
         time_end[2]-time_start);

  return rv;
}

void teste(bcm_port_t bcm_port, int linkscan_mode)
{
  bcm_port_info_t info_apply;
  bcm_error_t rv;

  memset(&info_apply, 0x00, sizeof(bcm_port_info_t));

  info_apply.action_mask = BCM_PORT_ATTR_LINKSCAN_MASK /*| BCM_PORT_ATTR_LINKSTAT_MASK*/;

  if ((rv = bcm_port_selective_get(0, bcm_port, &info_apply)) < 0) {
    printf("%s(%d) error: rv=%d!\r\n",__FUNCTION__,__LINE__, rv);
    return;
  }

  info_apply.linkscan = linkscan_mode;

  if ((rv = bcm_port_selective_set(0, bcm_port, &info_apply)) < 0) {
    printf("%s(%d) error: rv=%d!\r\n",__FUNCTION__,__LINE__, rv);
    return;
  }

  printf("%s(%d) Done (V2)!\r\n",__FUNCTION__,__LINE__);
  return;
}

void ptin_port_selective_get_all(bcm_port_t bcm_port)
{
  bcm_port_info_t info;
  bcm_error_t rv;

  memset(&info, 0x00, sizeof(bcm_port_info_t));

  info.action_mask = BCM_PORT_ATTR_ALL_MASK;

  if ((rv = bcm_port_selective_get(0, bcm_port, &info)) < 0) {
    printf("%s(%d) error: rv=%d!\r\n",__FUNCTION__,__LINE__, rv);
    return;
  }

  printf("%s(%d) Done!\r\n",__FUNCTION__,__LINE__);
}

void ptin_port_selective_get_link(bcm_port_t bcm_port)
{
  bcm_port_info_t info;
  bcm_error_t rv;

  memset(&info, 0x00, sizeof(bcm_port_info_t));

  info.action_mask = BCM_PORT_ATTR_LINKSTAT_MASK;

  if ((rv = bcm_port_selective_get(0, bcm_port, &info)) < 0) {
    printf("%s(%d) error: rv=%d!\r\n",__FUNCTION__,__LINE__, rv);
    return;
  }

  printf("%s(%d) link=%u!\r\n",__FUNCTION__,__LINE__, info.linkstatus);
}

void ptin_port_selective_set(bcm_port_t bcm_port, int linkscan_mode)
{
  bcm_port_info_t info_apply;
  bcm_error_t rv;

  memset(&info_apply, 0x00, sizeof(bcm_port_info_t));

  info_apply.action_mask = BCM_PORT_ATTR_LINKSCAN_MASK /*| BCM_PORT_ATTR_LINKSTAT_MASK*/;
  info_apply.linkscan = linkscan_mode;

  if ((rv = bcm_port_selective_set(0, bcm_port, &info_apply)) < 0) {
    printf("%s(%d) error: rv=%d!\r\n",__FUNCTION__,__LINE__, rv);
    return;
  }

  printf("%s(%d) ptin_port_selective_set executed!\r\n",__FUNCTION__,__LINE__);
}

bcm_error_t ptin_phyctrl_link_get(bcm_port_t bcm_port)
{
  int status;
  bcm_error_t rv;
    
  rv = soc_phyctrl_link_get(0, bcm_port, &status);

  printf("status=%d   (rv=%d -> \"%s\")\r\n", status, rv, bcm_errmsg(rv));

  return rv;
}

// Ingress Translations (single tagged packets)

int ptin_vlan_single_translate_action_add(int port, bcm_vlan_t oVlanId, bcm_vlan_t newOVlanId)
{
  int error;
  L7_uint32 bcm_unit, bcm_port, gport;
  bcm_vlan_translate_key_t keyType;
  bcm_vlan_action_set_t action;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, &gport) !=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

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

  error = bcm_vlan_translate_action_add(bcm_unit,gport,keyType,oVlanId,0,&action);

  printf("bcm_vlan_translate_action_add(%u,%u[%d],%u,%u,%u,&action) => %d (\"%s\")\r\n",
         bcm_unit,gport,bcm_port,keyType,oVlanId,0,error,bcm_errmsg(error));

  return error;
}

int ptin_vlan_single_translate_action_delete(int port, bcm_vlan_t oVlanId)
{
  int error;
  L7_uint32 bcm_unit, bcm_port, gport;
  bcm_vlan_translate_key_t keyType;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, &gport)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  keyType = bcmVlanTranslateKeyPortOuter;

  error = bcm_vlan_translate_action_delete(bcm_unit,gport,keyType,oVlanId,0);

  printf("bcm_vlan_translate_action_delete(%u,%u[%d],%u,%u,%u) => %d (\"%s\")\r\n",
         bcm_unit,gport,bcm_port,keyType,oVlanId,0,error,bcm_errmsg(error));

  return error;
}

// Ingress Translations (double tagged packets)

int ptin_vlan_double_translate_action_add(int port, bcm_vlan_t oVlanId, bcm_vlan_t iVlanId, bcm_vlan_t newOVlanId)
{
  int error;
  L7_uint32 bcm_unit, bcm_port, gport;
  bcm_vlan_translate_key_t keyType;
  bcm_vlan_action_set_t action;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, &gport)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

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

  error = bcm_vlan_translate_action_add(bcm_unit,gport,keyType,oVlanId,iVlanId,&action);

  printf("bcm_vlan_translate_action_add(%u,%u[%d],%u,%u,%u,&action) => %d (\"%s\")\r\n",
         bcm_unit,gport,bcm_port,keyType,oVlanId,iVlanId,error,bcm_errmsg(error));

  return error;
}

int ptin_vlan_double_translate_action_delete(int port, bcm_vlan_t oVlanId, bcm_vlan_t iVlanId)
{
  int error;
  L7_uint32 bcm_unit, bcm_port, gport;
  bcm_vlan_translate_key_t keyType;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, &gport)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  keyType = bcmVlanTranslateKeyPortDouble;

  error = bcm_vlan_translate_action_delete(bcm_unit,gport,keyType,oVlanId,iVlanId);

  printf("bcm_vlan_translate_action_delete(%u,%u[%d],%u,%u,%u) => %d (\"%s\")\r\n",
         bcm_unit,gport,bcm_port,keyType,oVlanId,iVlanId,error,bcm_errmsg(error));

  return error;
}

// Egress translations (single+double tagged packets)

int ptin_vlan_egress_translate_action_add(int classId, bcm_vlan_t oVlanId, bcm_vlan_t iVlanId, bcm_vlan_t newOVlanId, bcm_vlan_t newIVlanId)
{
  int error;
  bcm_vlan_action_set_t action;

  // Validate port, and get bcm_port reference
  if (classId<=0)
  {
    printf("Port/Class is invalid\r\n");
    return -1;
  }

  bcm_vlan_action_set_t_init(&action);
  action.dt_outer      = bcmVlanActionReplace;
  action.dt_inner      = bcmVlanActionReplace;
  action.dt_outer_prio = bcmVlanActionNone;
  action.dt_inner_prio = bcmVlanActionNone;

  action.ot_outer      = bcmVlanActionReplace;
  action.ot_inner      = bcmVlanActionAdd;
  action.ot_outer_prio = bcmVlanActionNone;

  action.new_outer_vlan = newOVlanId;
  action.new_inner_vlan = newIVlanId;

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

int vport_create(int bcm_port, int ovid, int new_ovid)
{
  bcm_vlan_port_t vlan_port;
  int rc;
  
  bcm_vlan_port_t_init(&vlan_port);

  vlan_port.flags = BCM_VLAN_PORT_INNER_VLAN_ADD;

  vlan_port.match_vlan = ovid;
  vlan_port.match_inner_vlan = 1000;
  vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
  vlan_port.egress_vlan = new_ovid;
  vlan_port.egress_inner_vlan = 1000;
  BCM_GPORT_LOCAL_SET(vlan_port.port, bcm_port);

  rc=bcm_vlan_port_create(0, &vlan_port);
  printf("vlan_port.vlan_port_id=0x%x, rc=%d (%s)\r\n",vlan_port.vlan_port_id,rc,bcm_errmsg(rc));

  if (rc == BCM_E_NONE)
  {
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t_init(&action);

    /* for outer tagged packet => outer tag replaced with gem_id */
    action.ot_outer = bcmVlanActionReplace;
    action.dt_outer = bcmVlanActionReplace;
    action.new_outer_vlan = ovid;

    /* for outer tagged packet => inner tag added with cvid */
    action.ot_inner = bcmVlanActionAdd;
    action.dt_inner = bcmVlanActionReplace;
    action.new_inner_vlan = 1000;

    rc=bcm_vlan_translate_egress_action_add(0, vlan_port.vlan_port_id, ovid, 0, &action);
    printf("vlan_port.vlan_port_id=0x%x, new_ovid=%u, rc=%d (%s)\r\n",vlan_port.vlan_port_id,new_ovid,rc,bcm_errmsg(rc));
  }

  return rc;
}

int vport_create_iterative(int bcm_port, int vlan_start, int vlan_end)
{
  int i, j, rc=0;
    
  for (i=vlan_start; i<=vlan_end; i++)
  {
    for (j=0; j<1000; j++);

    rc = vport_create(bcm_port, i, i);
    if (rc != BCM_E_NONE)
    {
      printf("Error rc=%d at vlan %u (%s)\r\n",rc,i,bcm_errmsg(rc));
      break;
    }
  }
  
  printf("End of function\r\n");
  return rc;
}


// Cross connections (single tagged packets)

int ptin_vlan_single_cross_connect_add(int port1, int port2, bcm_vlan_t oVlanId)
{
  int error;
  bcm_port_t  bcm_port1, bcm_port2;
  bcm_gport_t gport1, gport2;

  // Validate port, and get bcm_port reference
  if (port1>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port1, dapi_g, L7_NULLPTR, &bcm_port1, &gport1)!=L7_SUCCESS ||
      port2>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port2, dapi_g, L7_NULLPTR, &bcm_port2, &gport2)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_vlan_cross_connect_add(0,oVlanId,BCM_VLAN_INVALID,gport1,gport2);

  printf("bcm_vlan_cross_connect_add(0,%u,%u,%u[%d],%u[%d]) => %d (\"%s\")\r\n",
         oVlanId,BCM_VLAN_INVALID,gport1,bcm_port1,gport2,bcm_port2,error,bcm_errmsg(error));

  return error;
}

int ptin_vlan_single_cross_connect_delete(bcm_vlan_t oVlanId)
{
  int error;

  error = bcm_vlan_cross_connect_delete(0,oVlanId,BCM_VLAN_INVALID);

  printf("bcm_vlan_cross_connect_delete(0,%u,%u) => %d (\"%s\")\r\n",
         oVlanId,BCM_VLAN_INVALID,error,bcm_errmsg(error));

  return error;
}

// Cross connections (double tagged packets)

int ptin_vlan_double_cross_connect_add(int port1, int port2, bcm_vlan_t oVlanId, bcm_vlan_t iVlanId)
{
  int error;
  bcm_port_t  bcm_port1, bcm_port2;
  bcm_gport_t gport1, gport2;

  // Validate port, and get bcm_port reference
  if (port1>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port1, dapi_g, L7_NULLPTR, &bcm_port1, &gport1)!=L7_SUCCESS ||
      port2>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port2, dapi_g, L7_NULLPTR, &bcm_port2, &gport2)!=L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_vlan_cross_connect_add(0,oVlanId,iVlanId,gport1,gport2);

  printf("bcm_vlan_cross_connect_add(0,%u,%u,%u[%d],%u[%d]) => %d (\"%s\")\r\n",
         oVlanId,iVlanId,gport1,bcm_port1,gport2,bcm_port2,error,bcm_errmsg(error));

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
  L7_uint32 bcm_unit, bcm_port;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, L7_NULLPTR) != L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_switch_control_port_set(bcm_unit,bcm_port,type,arg);

  printf("bcm_switch_control_port_set(%u,%u,%d,%d) => %d (\"%s\")\r\n",
         bcm_unit,bcm_port,type,arg,error,bcm_errmsg(error));

  return error;
}


int ptin_switch_control_port_get(int port, bcm_switch_control_t type)
{
  int error;
  L7_uint32 bcm_unit, bcm_port;
  int arg;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, L7_NULLPTR) != L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_switch_control_port_get(bcm_unit,bcm_port,type,&arg);

  printf("bcm_switch_control_port_get(%u,%u,%d,&arg) => %d (\"%s\") arg=%d\r\n",
         bcm_unit,bcm_port,type,error,bcm_errmsg(error),arg);

  return error;
}


int ptin_port_control_set(int port, bcm_port_control_t type, int arg)
{
  int error;
  L7_uint32 bcm_unit, bcm_port;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, L7_NULLPTR) != L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_port_control_set(bcm_unit,bcm_port,type,arg);

  printf("bcm_port_control_set(%d,%d,%d,%d) => %d (\"%s\")\r\n",
         bcm_unit,bcm_port,type,arg,error,bcm_errmsg(error));

  return error;
}


int ptin_port_control_get(int port, bcm_port_control_t type)
{
  int error;
  L7_uint32 bcm_unit, bcm_port;
  int arg;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, L7_NULLPTR) != L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_port_control_get(bcm_unit,bcm_port,type,&arg);

  printf("bcm_port_control_get(%d,%d,%d,&arg) => %d (\"%s\") arg=%d\r\n",
         bcm_unit,bcm_port,type,error,bcm_errmsg(error),arg);

  return error;
}


// Control Port settings (to allow ingress translations)

int ptin_vlan_control_port_set(int port, bcm_vlan_control_port_t type, int arg)
{
  int error;
  L7_uint32 bcm_unit, bcm_port;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, L7_NULLPTR) != L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_vlan_control_port_set(bcm_unit,bcm_port,type,arg);

  printf("bcm_vlan_control_port_set(%u,%u,%d,%d) => %d (\"%s\")\r\n",
         bcm_unit,bcm_port,type,arg,error,bcm_errmsg(error));

  return error;
}


int ptin_vlan_control_port_get(int port, bcm_vlan_control_port_t type)
{
  int error;
  L7_uint32 bcm_unit, bcm_port;
  int arg;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, L7_NULLPTR) != L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_vlan_control_port_get(bcm_unit,bcm_port,type,&arg);

  printf("bcm_vlan_control_port_get(%u,%u,%d,&arg) => %d (\"%s\") arg=%d\r\n",
         bcm_unit,bcm_port,type,error,bcm_errmsg(error),arg);

  return error;
}


int ptin_port_phy_control_set(int port, bcm_port_phy_control_t type, int arg)
{
  int error;
  L7_uint32 bcm_unit, bcm_port;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, L7_NULLPTR) != L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_port_phy_control_set(bcm_unit, bcm_port, type,arg);

  printf("bcm_port_phy_control_set(%u,%u,%d,%d) => %d (\"%s\")\r\n",
         bcm_unit,bcm_port,type,arg,error,bcm_errmsg(error));

  return error;
}


int ptin_port_phy_control_get(int port, bcm_port_phy_control_t type)
{
  int error;
  L7_uint32 bcm_unit, bcm_port;
  int arg;

  // Validate port, and get bcm_port reference
  if (port>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port, dapi_g, &bcm_unit, &bcm_port, L7_NULLPTR) != L7_SUCCESS)
  {
    printf("Port is invalid\r\n");
    return -1;
  }

  error = bcm_port_phy_control_get(bcm_unit, bcm_port, type, &arg);

  printf("bcm_port_phy_control_get(%u,%u,%d,&arg) => %d (\"%s\") arg=%d\r\n",
         bcm_unit,bcm_port,type,error,bcm_errmsg(error),arg);

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

int ptin_vp_gpon(L7_uint32 pon_port, L7_uint32 network_port, L7_int s_vid, L7_int c_vid)
{
  int unit = 0;
  bcm_gport_t network_gport;
  int gemid[] = {101, 102, 103};
  int cvid[]  = {25, 35, 45};
  bcm_vlan_port_t vlan_port[3];
  bcm_vlan_action_set_t action;
  bcm_multicast_t mcast_group;
  bcm_multicast_t encap_id;
  bcm_pbmp_t pbmp, ubmp;
  bcm_vlan_control_vlan_t vlan_control;
  int i;
  bcm_error_t error;

  /* enable L3 egress mode... needed for the virtual port APIs to work */
  if ((error=bcm_switch_control_set(unit, bcmSwitchL3EgressMode, 1)) != BCM_E_NONE)
  {
    printf("Error setting on bcmSwitchL3EgressModecreating policer: error=%d (\"%s\")\r\n",error,bcm_errmsg(error));
    return error;
  }

  /* create the virtual ports */
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
  if (port_nni>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port_nni, dapi_g, L7_NULLPTR, &bcm_port_nni, L7_NULLPTR) != L7_SUCCESS)
  {
    printf("Port %u is invalid\r\n", port_nni);
    return -1;
  }
  if (port_uni>=PTIN_SYSTEM_N_PORTS ||
      hapi_ptin_get_bcmdata_from_uspport(port_uni, dapi_g, L7_NULLPTR, &bcm_port_uni, &gport_uni) != L7_SUCCESS)
  {
    printf("Port %u is invalid\r\n", port_uni);
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
  if ((error = bcm_vlan_control_vlan_get(0 /*unit*/, vid_nni, &control))!=BCM_E_NONE)
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error getting vlan control structure! error=%d (%s)\r\n", error, bcm_errmsg(error));
    return L7_FAILURE;
  }
  control.forwarding_vlan = vid_nni;
  control.flags &= ~((uint32) BCM_VLAN_LEARN_DISABLE);
  control.forwarding_mode = bcmVlanForwardBridging;

  /* Apply new control definitions to this vlan */
  if ( (error = bcm_vlan_control_vlan_set(0 /*unit*/, vid_nni, control)) != BCM_E_NONE )
  {
    PT_LOG_ERR(LOG_CTX_HAPI, "Error with bcm_vlan_control_vlan_set: error=%d (%s)", error, bcm_errmsg(error));
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

BROAD_POLICY_t cpu_policyId = BROAD_POLICY_INVALID;

L7_RC_t ptin_cpu_traffic_limit( L7_uint16 cir )
{
  BROAD_POLICY_t      policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t ruleId = BROAD_POLICY_RULE_INVALID;
  BROAD_METER_ENTRY_t meterInfo;
  bcm_port_t          bcm_port = 0;
  L7_uint32           mask = 0xffffffff;
  //L7_uint8            drop = 1;
  L7_RC_t rc = L7_SUCCESS;

  meterInfo.cir       = cir;
  meterInfo.cbs       = 128;
  meterInfo.pir       = cir;
  meterInfo.pbs       = 128;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;

  if (cpu_policyId != BROAD_POLICY_INVALID)
  {
    hapiBroadPolicyDelete(cpu_policyId);
    cpu_policyId = BROAD_POLICY_INVALID;

    printf("CPU policy removed\r\n");
    fflush(stdout);
  }

  /* Nothing to be done */
  if (cir == (L7_uint16) -1)
  {
    return L7_SUCCESS;
  }

  /* Create policy */
  rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PTIN);
  if (rc != L7_SUCCESS)
  {
    printf("Cannot create trap policy\r\n");
    return L7_FAILURE;
  }
  printf("tRAP Policy created\r\n");

  /* Egress stage */
  if (hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_EGRESS) != L7_SUCCESS)
  {
    printf("Error creating a egress policy\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  /* Create rule */
  rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding rule\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }

  printf("Adding port qualifier (bcm_port=%d)\r\n",bcm_port);
  rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OUTPORT, (L7_uchar8 *)&bcm_port, (L7_uchar8 *)&mask);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding port qualifier (bcm_port=%d)\r\n",bcm_port);
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("Port qualifier (bcm_port=%d) added\r\n",bcm_port);

  #if 0
  /* Add drop qualifer */
  printf("Adding Drop qualifier (drop=%u)\r\n",drop);
  rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DROP, (L7_uchar8 *)&drop, (L7_uchar8 *)&mask);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding drop qualifier (drop=%u/0x%02x)\r\n",drop,mask);
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("Drop qualifier added (drop=%u/0x%02x)\r\n",drop,mask);
  #endif

  /* Drop all packets */
  rc = hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding hard_drop action\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("hard_drop action added\r\n");

  /* Define meter action, to rate limit packets */
  rc = hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
  if (rc != L7_SUCCESS)
  {
    printf("Error adding rate limit\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("Rate limit added\r\n");

  printf("Commiting trap policy\r\n");
  if ((rc=hapiBroadPolicyCommit(&policyId)) != L7_SUCCESS)
  {
    printf("Error commiting trap policy\r\n");
    hapiBroadPolicyCreateCancel();
    return L7_FAILURE;
  }
  printf("Trap policy commited successfully (policyId=%u)\r\n",policyId);

  /* Save policy id */
  cpu_policyId = policyId;

  fflush(stdout);

  return L7_SUCCESS;
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
#if (PTIN_BOARD == PTIN_BOARD_CXO640G || PTIN_BOARD == PTIN_BOARD_CXO160G)

#include <bcm/port.h>
#include <unistd.h>
//#include <application/ptin/utils/include/ipc_lib.h>
//#include "soc/trident.h"
//
//int
//soc_trident_port_cbl_table_parity_set(int unit, int enable)
//{
//  uint32 rval;
//  soc_reg_t enable_reg = PORT_CBL_TABLE_PARITY_CONTROLr;
//  soc_field_t enable_field = PARITY_ENf;
//
//  soc_trident_pipe_select(unit, 0, 0);
//
//  soc_reg32_get(unit, enable_reg, REG_PORT_ANY, 0, &rval);
//  soc_reg_field_set(unit, enable_reg, &rval, enable_field, enable);
//  soc_reg32_set(unit, enable_reg, REG_PORT_ANY, 0, rval);
//
//  soc_trident_pipe_select(unit, 0, 1);
//  soc_reg32_get(unit, enable_reg, REG_PORT_ANY, 0, &rval);
//  soc_reg_field_set(unit, enable_reg, &rval, enable_field, enable);
//  soc_reg32_set(unit, enable_reg, REG_PORT_ANY, 0, rval);
//
//  soc_trident_pipe_select(unit, 0, 0);
//
//  printf("soc_trident_port_cbl_table_parity_set executed!\r\n");
//
//  return SOC_E_NONE;
//}
//

// *****************************************************************************
//      definicao da estrutura que suporta o protocolo de comunicacao          *
// *****************************************************************************
#include "ipc.h"
#include "ipc_lib.h"

#define N_SLOTS_MAX   (PTIN_SYSTEM_MAX_N_FULLSLOTS-2)
#define N_GROUPS_MAX  7
#define N_LANES_MAX   PTIN_SYS_INTFS_PER_SLOT_MAX

int bcm_slot_map[2][PTIN_SYS_SLOTS_MAX+1][PTIN_SYS_INTFS_PER_SLOT_MAX];

/* To save BER results */
struct ber_t {
  unsigned int pre;
  unsigned int main;
  unsigned int post;
  unsigned int reg;
  unsigned int ber;
};

#define MAX_MEASUREMENTS  8192

struct ber_t results_tx[N_SLOTS_MAX][N_LANES_MAX][MAX_MEASUREMENTS]; /* slot, port, results */
struct ber_t results_rx[N_SLOTS_MAX][N_LANES_MAX][MAX_MEASUREMENTS]; /* slot, port, results */

unsigned int ip_addr_slot[PTIN_SYSTEM_MAX_N_FULLSLOTS + 1];

struct params {
  char file[128];
  int optimize_lc;
  int n_iter;
  int reset_delay;
  int start_delay;
  int test_time;
  int mode;
  int pre_start;    //Just
  int pre_end;
  int pre_step;     //_rx_
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

typedef struct {
    L7_uint8 pre;   /* pre cursor - default 0x0F */
#if 0
/* OLD */
    L7_uint8 main;  /* main cursor */
#else
/* NEW */
    L7_uint16 main; /* main cursor */
#endif
    L7_uint8 post;  /* post cursor */
    L7_uint8 slew;  /* slew control - default 0x0A */
    L7_uint8 mx;    /* source matrix */
} __attribute__((packed)) rx_ber_txmsg_t;
/* Please check twin msg_ptin_tap_settings */

struct params p_tx, p_rx;

/* Semaphore to synchronize PTin task execution */
void *ptin_ber_tx_sem = L7_NULLPTR;
void *ptin_ber_rx_sem = L7_NULLPTR;

int ber_tx_running = 0;
int ber_rx_running = 0;

static int canal_ipc = -1;

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
  int tap_main, tap_post, tap_direction;
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
  txmsg.dstId        = IPC_HW_PORTO_MSG_CXP;
  txmsg.flags        = 0;
  txmsg.counter      = rand ();
  txmsg.msgId        = 0x531;
  txmsg.infoDim      = 2;
  txmsg.info[0]      = mx;    /* First byte will contain the source matrix */
  txmsg.info[1]      = 0xff;  /* All 4 ports */

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
                  " reset delay     = %u\n"
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
              p_tx.reset_delay, p_tx.start_delay, p_tx.test_time, p_tx.mode,
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
//      for (tap_main=p_tx.main_start; tap_main>=p_tx.main_end; tap_main-=p_tx.main_step) {
//        for (tap_post=p_tx.post_start; tap_post<=p_tx.post_end; tap_post+=p_tx.post_step, idx++) {

      tap_main = p_tx.main_start;
      tap_post = p_tx.post_start;
      tap_direction = p_tx.post_step;

      while (tap_direction != 0 && p_tx.main_step != 0 && p_tx.post_step != 0 && idx < MAX_MEASUREMENTS) 
      {
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
            /* Update tap settings */
            for (slot=0; slot<p_tx.n_slots; slot++)
            {
              for (port_idx = 0; port_idx < N_LANES_MAX; port_idx++)
              {
                port = p_tx.port_list[slot][port_idx];
                if ( port < 0 )  continue;
              
                bcm_port_phy_control_set(0, port, BCM_PORT_PHY_CONTROL_PREEMPHASIS, reg);
              }
            }
            fprintf(fd, "=> Main=%2u Post=%2u Reg=0x%04X\n\n", tap_main, tap_post, reg);
          }
          else
          {
            fprintf(fd, "=> Transmission tap settings not changed!\n\n");
          }

          fflush(fd);

          /* Reset delay */
          sleep(p_tx.reset_delay);

          /* Initialization type 3 */
          if (p_tx.optimize_lc & 0x02 )
          {
            fprintf(fd, "  Applying init procedure Copper->SFI, SFI->Copper\n");
            for (slot=0; slot<p_tx.n_slots; slot++)
            {
              /* Set SFI mode */
              if (remote_var_write(p_tx.ip_addr[slot], -1, 1, 0x94, 2)!=0)
              {
                fprintf(fd, "   [ERROR] Failed setting SFI mode for slot %u\n", p_tx.slot[slot]);
              }
              sleep(1);
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

            fprintf(fd, "  Applying init procedure 0x8000->1x8036, 0->0xB0\n");

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
            ret = send_data (canal_ipc, IPC_HW_PORTO_MSG_CXP, p_tx.ip_addr[slot], &txmsg, &rxmsg);
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

          if ( p_tx.optimize_lc & 0x04 )
          {
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
          }

          fflush(fd);

          /* Wait the integration time... */
          sleep(p_tx.test_time);

          tx_ber_sum = 0;
          memset(results_iter,0xff,sizeof(results_iter));

          /* For each slot, get the final values (4 ports at once) */
          for (slot=0; slot<p_tx.n_slots; slot++)
          {
            memset(rxmsg.info, 0xFF, 4*sizeof(int));
            ret = send_data (canal_ipc, IPC_HW_PORTO_MSG_CXP, p_tx.ip_addr[slot], &txmsg, &rxmsg);
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
                results_tx[slot][port_idx][idx].pre = -1;
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

              fprintf(fd, " bcm:%-2d=%-5u", port, results_iter[slot][port_idx]);
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

        /* Update main tap at the end of each post cycle? */
        if ( (p_tx.mode & 1) ) {
          tap_main -= p_tx.main_step;
        }

        /* Bidirectional running */
        if (p_tx.mode & 0x04)
        {
          /* Invert direction ? */
          if (((tap_direction > 0) && (tap_post + p_tx.post_step > p_tx.post_end)) ||
              ((tap_direction < 0) && (tap_post - p_tx.post_step < p_tx.post_start)))
          {
            tap_direction *= -1;

            /* Update main tap ? */
            if ( !(p_tx.mode & 1) )
              tap_main -= p_tx.main_step;
          }
          /* Only update post cursor */
          else
          {
            tap_post += (tap_direction * p_tx.post_step);
          }
        }
        /* Normal running procedure */
        else
        {
          if ((tap_post + p_tx.post_step) > p_tx.post_end)
          {
            /* Reset post cursor */
            tap_post = p_tx.post_start;

            /* Update main tap ? */
            if ( !(p_tx.mode & 1) )
              tap_main -= p_tx.main_step;
          }
          else
          {
            tap_post += p_tx.post_step;
          }
        }

        /* If main tap out of range, stop cycle */
        if (tap_main < p_tx.main_end)
        {
          tap_direction = 0;
        }

        /* Update index */
        idx++;

        if (stop)
          break;
      }

//      if (stop)
//        break;
//    }

      if (stop) {
        fprintf(fd, "\nBER tx task forced to stop!\n");
        printf("\nBER tx task forced to stop!\n");
      }

      fprintf(fd, "\n\nFinal Report:\n");

      /* Print a first table with the port mapping */
      fprintf(fd, "+-------+-------------+---------+-----------------+\n");
      fprintf(fd, "| SLOT  | BCM PORT    | LC PORT | LC IP           |\n");
      fprintf(fd, "+-------+-------------+---------+-----------------+\n");
      //           | i=N  | xeXX (lane=K) | port=P  | 192.168.200.xxx |

      for (slot=0; slot<p_tx.n_slots; slot++) {
        for (port_idx=0; port_idx<N_LANES_MAX; port_idx++) {
          port = p_tx.port_list[slot][port_idx];
          if ( port < 0 )  continue;

          fprintf(fd, "| i=%-2u  | %-2d (lane=%d) | port=%d  | 192.168.200.%-3u |\n",
                  p_tx.slot[slot], port, port_idx, port_idx, p_tx.ip_addr[slot] & 0xFF);
        }
      }
      fprintf(fd, "+-------+-------------+---------+-----------------+\n");

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
  int tap_pre, tap_main, tap_post, tap_direction;
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
  txmsg.dstId        = IPC_HW_PORTO_MSG_CXP;
  txmsg.flags        = 0;
  txmsg.counter      = rand ();
  txmsg.msgId        = 0x532;
  txmsg.infoDim      = sizeof(rx_ber_txmsg_t);
  memset(txmsg.info, 0, sizeof(rx_ber_txmsg_t));

  /* Do stuff... */
  do {          //while (1)
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
                  " reset delay     = %u\n"
                  " start delay     = %u\n"
                  " test time       = %u\n"
                  " mode            = %u\n"
                  " pre start       = %u\n"
                  " pre end         = %u\n"
                  " pre step        = %u\n"
                  " main start      = %u\n"
                  " main end        = %u\n"
                  " main step       = %u\n"
                  " post start      = %u\n"
                  " post end        = %u\n"
                  " post step       = %u\n"
                  " n_slots         = %u\n",
              iter, p_rx.n_iter,
              p_rx.reset_delay, p_rx.start_delay, p_rx.test_time, p_rx.mode,
              p_rx.pre_start, p_rx.pre_end, p_rx.pre_step,
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
      //for (tap_main=p_rx.main_start; tap_main>=p_rx.main_end; tap_main-=p_rx.main_step) {
      //  for (tap_post=p_rx.post_start; tap_post<=p_rx.post_end; tap_post+=p_rx.post_step, idx++) {

      for ( tap_pre = p_rx.pre_start;
            tap_pre <= p_rx.pre_end;
            tap_pre += p_rx.pre_step)
      {
       tap_main = p_rx.main_start;
       tap_post = p_rx.post_start;
       tap_direction = 1;

       while (tap_direction != 0 && p_rx.main_step != 0 && p_rx.post_step != 0 && idx < MAX_MEASUREMENTS)
       {
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
              {
                  rx_ber_txmsg_t *p;

                  p = (rx_ber_txmsg_t *) txmsg.info; // &txmsg.info[0];

                  p->pre = tap_pre;   /* pre cursor - default 0x0F */
                  p->main = htons(tap_main); /* main cursor */
                  p->post = tap_post; /* post cursor */
                  p->slew = 0x0A; /* Slew control - default 0x0A */
                  p->mx = mx;     /* Source matrix */
              }

              ret = send_data (canal_ipc, IPC_HW_PORTO_MSG_CXP, p_rx.ip_addr[slot], &txmsg, &rxmsg);
              if (ret != 0) {
                fprintf(fd, "Error setting remote tap values slot %u\n", p_rx.slot[slot]);
                goto next_tap_settings_vector;  //continue;
              }
              if (rxmsg.flags != IPCLIB_FLAGS_ACK
                  || *((L7_uint32 *)rxmsg.info) != ENDIAN_SWAP32(0))
              {
                fprintf(fd,
                        "Slot %u  refused setting remote tap values "
                        "pre=%-2u main=%-2u post=%-2u: reg=0x%04X\n\n",
                        p_rx.slot[slot],
                        tap_pre, tap_main, tap_post, reg);
                goto next_tap_settings_vector;  //continue;
              }
              usleep(1*1000);
            }//for (slot=0...
            fprintf(fd,
                    "=> Remote tap settings updated to "
                    "pre=%-2u main=%-2u post=%-2u: reg=0x%04X\n\n",
                    tap_pre, tap_main, tap_post, reg);
          }
          else
          {
            fprintf(fd, "=> Vitesse tap settings not changed!\n\n");
          }
          fflush(fd);

          /* Reset delay */
          sleep(p_rx.reset_delay);

          /* Reset PRBS readings */
          for (slot=0; slot<p_rx.n_slots; slot++)
          {
            for (port_idx=0; port_idx<N_LANES_MAX; port_idx++)
            {
              port = p_rx.port_list[slot][port_idx];
              if ( port < 0 )  continue;

              rc = bcm_port_control_get(0, port, bcmPortControlPrbsRxStatus, &rx_ber);
              if (rc != L7_SUCCESS)
              {
                fprintf(fd, "ERROR reading rx status from port %u\n", port);
                continue;
              }
            }
          }

          /* Let local partner stabilize... */
          sleep(p_rx.start_delay);

          /* For each slot and for each port, read ber error to reset to zero */
          for (slot=0; slot<p_rx.n_slots; slot++)
          {
            for (port_idx=0; port_idx<N_LANES_MAX; port_idx++)
            {
              port = p_rx.port_list[slot][port_idx];
              if ( port < 0 )  continue;

              rc = bcm_port_control_get(0, port, bcmPortControlPrbsRxStatus, &rx_ber);
              if (rc != L7_SUCCESS)
              {
                fprintf(fd, "ERROR reading rx status from port %u\n", port);
                continue;
              }
            }
          }
        }//if ( iter == 1...

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
                results_rx[slot][port_idx][idx].pre  = tap_pre;
                results_rx[slot][port_idx][idx].main = tap_main;
                results_rx[slot][port_idx][idx].post = tap_post;
                results_rx[slot][port_idx][idx].reg  = reg;
              }

              /* Only valid lanes will continue */
              port = p_rx.port_list[slot][port_idx];
              if ( port < 0 )  continue;

              rc = bcm_port_control_get(0, port, bcmPortControlPrbsRxStatus, &rx_ber);
              if (rc != L7_SUCCESS) {
                fprintf(fd, "ERROR reading rx status from port %u\n", port);
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

            fprintf(fd, "Slot %-2d -> main=%-2u post=%-2u reg=0x%04X BER:", slot, tap_main, tap_post, reg);
            for (port_idx=0; port_idx<N_LANES_MAX; port_idx++)
            {
              port = p_rx.port_list[slot][port_idx];
              if ( port < 0 )  continue;

              fprintf(fd, " bcm:%-2d=%-5u", port, results_iter[slot][port_idx]);
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
        }//for (count=0...

        fprintf(fd, "--------------------------------------------------------------------------------------\n");
        fflush(fd);

        /* Update index */
        idx++;  //Moved to here

next_tap_settings_vector:
        /* Update main tap at the end of each post cycle? */
        if ( (p_rx.mode & 1) ) {
          tap_main -= p_rx.main_step;
        }

        /* Bidirectional post cursor running */
        if (p_rx.mode & 0x04)
        {
          /* Invert direction ? */
          if (((tap_direction > 0) && (tap_post + p_rx.post_step > p_rx.post_end)) ||
              ((tap_direction < 0) && (tap_post - p_rx.post_step < p_rx.post_start)))
          {
            tap_direction *= -1;

            /* Update main tap ? */
            if ( !(p_rx.mode & 1) )
              tap_main -= p_rx.main_step;
          }
          /* Only update post cursor */
          else
          {
            tap_post += (tap_direction * p_rx.post_step);
          }
        }
        /* Normal running procedure */
        else
        {
          if ((tap_post + p_rx.post_step) > p_rx.post_end)
          {
            /* Reset post cursor */
            tap_post = p_rx.post_start;

            /* Update main tap ? */
            if ( !(p_rx.mode & 1) )
              tap_main -= p_rx.main_step;
          }
          else
          {
            tap_post += p_rx.post_step;
          }
        }

        /* If main tap out of range, stop cycle */
        if (tap_main < p_rx.main_end)
        {
          tap_direction = 0;
        }

        /* Update index */
        //idx++;    Moved upwards

        if (stop)
          break;
       }//while
      }//for ( tap_pre = p_rx.pre_start;

      if (stop) {
        fprintf(fd, "\nBER rx task forced to stop!\n");
        printf("\nBER rx task forced to stop!\n");
      }

      fprintf(fd, "\n\nFinal Report:\n");

      /* Print a first table with the port mapping */
      fprintf(fd, "+-------+-------------+---------+-----------------+\n");
      fprintf(fd, "| SLOT  | BCM PORT    | LC PORT | LC IP           |\n");
      fprintf(fd, "+-------+-------------+---------+-----------------+\n");
      //           | i=NN  | xeXX (lane=K) | port=P  | 192.168.200.xxx |

      for (slot=0; slot<p_rx.n_slots; slot++) {
        for (port_idx=0; port_idx<N_LANES_MAX; port_idx++) {
          port = p_rx.port_list[slot][port_idx];
          if ( port < 0 )  continue;

          fprintf(fd, "|  %-2u  | %-2d (lane=%d) | port=%d  | 192.168.200.%-3u |\n",
                  p_rx.slot[slot], port, port_idx, port_idx, p_rx.ip_addr[slot] & 0xFF);
        }
      }
      fprintf(fd, "+-------+-------------+---------+-----------------+\n");

      /* Print a second table with all results from all slots */

      /* The heading of this table varies with the number of slots being tested */
      char str1[768], str2[768], tmp[768];
      strcpy(str1, "+------+------+------+--------+");
      strcpy(str2, "| PRE  | MAIN | POST |  REG   |");
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
        fprintf(fd, "|  %-2u  |  %-2u  |  %-2u  | 0x%04X |",
               results_rx[0][0][i].pre,
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
    }//for (iter=1...

    printf("\nBER @rx done!\n");

  } while (1);

  return;
}


int ber_init(void)
{
  int slot, lane;
  int ret;
  int matrix;
  int port, bcm_port;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardPtr;
  HAPI_CARD_SLOT_MAP_t         *hapiSlotMapPtr;
  HAPI_WC_PORT_MAP_t           *hapiWCMapPtr;

  /* Open IPC channel */
  if (canal_ipc >= 0)
  {
    if (close_ipc(canal_ipc) != 0)
    {
      printf("Closed IPC channel!\n");
      canal_ipc = -1; 
    }
    else
    {
      printf("Error closing IPC channel!\n");
      return -1;
    }
  }

  ret = open_ipc(0, 0, NULL, 5, &canal_ipc);
  if (ret != 0) {
    printf("Error opening IPC channel! (ret=%d)\n", ret);
    return -1;
  }
  printf("IPC channel OK\n");

  if (ptin_ber_tx_sem != L7_NULLPTR) {
    printf("BER has already been initialized!\n");
    return 0;
  }

  /* Read slot id: 0->Working; 1->Protection */
  matrix = CPLD_SLOT_ID_GET();

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
  memset(bcm_slot_map, 0xff, sizeof(bcm_slot_map));   /* -1 for all values */

  for (port=0; port<L7_MAX_PHYSICAL_PORTS_PER_UNIT; port++)
  {
    slot = hapiWCMapPtr[port].slotNum;
    lane = hapiWCMapPtr[port].wcLane;
    bcm_port = hapiSlotMapPtr[port].bcm_port;

    /* Update xe port map (only 10/40/100Gbps) */
    if ( hapiWCMapPtr[port].wcSpeedG > 1 )
    {
      if (slot<=PTIN_SYS_SLOTS_MAX && lane<PTIN_SYS_INTFS_PER_SLOT_MAX)
        bcm_slot_map[mx][slot][lane] = bcm_port;
    }
  }

  /* Clear IP addresses */
  memset(ip_addr_slot, 0x00, sizeof(ip_addr_slot));

  printf("bcm_slot_map:");
  for (slot=1; slot<=PTIN_SYS_SLOTS_MAX; slot++)
  {
    printf("\n Slot %02u: ",slot);
    /* BCM port table */
    for (lane=0; lane<PTIN_SYS_INTFS_PER_SLOT_MAX; lane++)
    {
      if (bcm_slot_map[mx][slot][lane] >= 0)
        printf("  %2d", bcm_slot_map[mx][slot][lane]);
      else
        printf("  --");
    }

    /* IP list */
    if (slot == PTIN_SYS_MX1_SLOT)
    {
      ip_addr_slot[slot] = (192<<24) + (168<<16) + (200<<8) + ((PTIN_BOARD == PTIN_BOARD_CXO160G) ? 101 : 1);
    }
    else if (slot == PTIN_SYS_MX2_SLOT)
    {
      ip_addr_slot[slot] = (192<<24) + (168<<16) + (200<<8) + ((PTIN_BOARD == PTIN_BOARD_CXO160G) ? 102 : 2);
    }
    else
    {
      /* OLT1T1 has IP addresses inverted */
    #if (PTIN_BOARD == PTIN_BOARD_CXO160G)
      ip_addr_slot[slot] = (192<<24)+(168<<16)+(200<<8) + PTIN_SYS_SLOTS_MAX + 1 - slot + 1;
    #else
      ip_addr_slot[slot] = (192<<24)+(168<<16)+(200<<8) + slot + 1;
    #endif
    }
    printf("\t\tIP=%d.%d.%d.%d", ip_addr_slot[slot]>>24 & 0xff, ip_addr_slot[slot]>>16 & 0xff, ip_addr_slot[slot]>>8 & 0xff, ip_addr_slot[slot] & 0xff);
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
         "Please wait until each cycle ends.\n");
  stop = 1;

  return 0;
}

#define MAX_TX_MAIN_START   ((1<<6)-1)
#define MAX_TX_POST_END     ((1<<5)-1)
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
               int reset_delay, int start_delay, int test_time, int mode,
               int main_start , int main_end, int main_step,
               int post_start , int post_end, int post_step,
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
  if (main_start > MAX_TX_MAIN_START || post_end > MAX_TX_POST_END) {
    printf("main_start or post_end out of range! Max %u,%u\n",
           MAX_TX_MAIN_START, MAX_TX_POST_END);
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
  p_tx.reset_delay = reset_delay;
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
      p_tx.port_list[i][j] = bcm_slot_map[mx][p_tx.slot[i]][j];
    }
    /* Check if this slot has a valid  mapping */
    for (j=0; j<N_LANES_MAX && p_tx.port_list[i][j] < 0; j++);
    if ( j >= N_LANES_MAX ) {
      printf("Slot %u is not mapped internally!\n", p_tx.slot[i]);
      return -1;
    }

    p_tx.ip_addr[i] = ip_addr_slot[p_tx.slot[i]];
  }

  ber_tx_running = 1;
  osapiSemaGive(ptin_ber_tx_sem);

  return 0;
}




#define MAX_RX_PRE_END      ((1<<5)-1)
/*Originally, PRE had no range (default value); 
  TG16GF/Cortina PHY needed a 6bit range.*/

//#define MAX_RX_MAIN_START   (31)
/* Original value; TG16GF/Cortina PHY needed 5=>9bits */
#define MAX_RX_MAIN_START   ((1<<9)-1)
#define MAX_RX_POST_END     ((1<<6)-1)
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
               int reset_delay, int start_delay, int test_time, int mode,
               int pre_start , int pre_end, int pre_step,
               int main_start , int main_end, int main_step,
               int post_start , int post_end, int post_step,
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
  if (pre_start < 0 || pre_start > pre_end || pre_end > MAX_RX_PRE_END ||
      pre_step <= 0)
  {
    printf("Mandatory 0 <= pre_start <= pre_end <= %u and pre_step > 0\n",
           MAX_RX_PRE_END);
    return -1;
  }
  if (main_start > MAX_RX_MAIN_START || post_end > MAX_RX_POST_END) {
    printf("main_start or post_end out of range! Max %u,%u\n",
           MAX_RX_MAIN_START, MAX_RX_POST_END);
    return -1;
  }
  if (main_step <= 0 || post_step <= 0) {
    printf("main_step and post_step must be greater than zero!\n");
    return -1;
  }
  if (n_iter < 1 || n_iter > 1000) {
    printf("Number of iterations must be in the range of [1..999]\n");
    return -1;
  }

  strcpy(p_rx.file, file);
  p_rx.optimize_lc = optimize_lc;
  p_rx.n_iter      = n_iter;
  p_rx.reset_delay = reset_delay;
  p_rx.start_delay = start_delay;
  p_rx.test_time   = test_time;
  p_rx.mode        = mode;
  p_rx.pre_start   = pre_start;
  p_rx.pre_end     = pre_end;
  p_rx.pre_step    = pre_step;
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
      p_rx.port_list[i][j] = bcm_slot_map[mx][p_rx.slot[i]][j];
    }
    for ( j=0; j<N_LANES_MAX && p_rx.port_list[i][j]<0; j++);
    if ( j >= N_LANES_MAX ) {
      printf("Slot %u is not mapped internally!\n", p_rx.slot[i]);
      return -1;
    }

    p_rx.ip_addr[i] = ip_addr_slot[p_rx.slot[i]];
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

  for (i=0; i<n_slots; i++)
  {
    ip_addr[i] = ip_addr_slot[slot[i]];
  }

  /* Compose TX message */
  txmsg.protocolId   = 1;
  txmsg.srcId        = 7100;
  txmsg.dstId        = IPC_HW_PORTO_MSG_CXP;
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
    ret = send_data (canal_ipc, IPC_HW_PORTO_MSG_CXP, ip_addr[i], &txmsg, &rxmsg);
    if (ret != 0) {
      printf("Error initing remote BER in slot %u\n", slot[i]);
      return -1;
    }
    if (rxmsg.flags != 0x01)
    {
      printf("Request not acknowledged in slot %u\n", slot[i]);
      continue;
    }
    printf("Success %s BER tx/rx in slot %u (IP %u.%u.%u.%u)\n",
           ((enable) ? "enabling" : "disabling"),
           slot[i],
           (ip_addr[i]>>24) & 0xff, (ip_addr[i]>>16) & 0xff, (ip_addr[i]>>8) & 0xff, ip_addr[i] & 0xff);
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
 * Configure tap settings in a specific slot
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
int set_remote_tapcursors( int tap_pre, int tap_main, int tap_post,
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

  for (i=0; i<n_slots; i++)
  {
    ip_addr[i] = ip_addr_slot[slot[i]];
  }

  /* Compose TX message */
  txmsg.protocolId   = 1;
  txmsg.srcId        = 7100;
  txmsg.dstId        = IPC_HW_PORTO_MSG_CXP;
  txmsg.flags        = 0;
  txmsg.counter      = rand ();
  txmsg.msgId        = 0x532;
  txmsg.infoDim      = sizeof(rx_ber_txmsg_t);
  memset(txmsg.info, 0, sizeof(rx_ber_txmsg_t));

  /* For each slot, get remote values (just to reset them!) (4 ports at once) */
  for (i=0; i<n_slots; i++)
  {
    {
        rx_ber_txmsg_t *p;

        p = (rx_ber_txmsg_t *) txmsg.info; // &txmsg.info[0];

        p->pre = tap_pre;   /* pre cursor - default 0x0F */
        p->main = htons(tap_main); /* main cursor */
        p->post = tap_post; /* post cursor */
        p->slew = 0x0A;     /* Slew control - default 0x0A */
        p->mx = mx;         /* Source matrix */
    }


    ret = send_data (canal_ipc, IPC_HW_PORTO_MSG_CXP, ip_addr[i], &txmsg, &rxmsg);
    if (ret != 0) {
      printf("Error setting remote BER in slot %u\n", slot[i]);
      return -1;
    }
    if (rxmsg.flags != 0x01)
    {
      printf("Request not acknowledged in slot %u\n", slot[i]);
      continue;
    }
    printf("Success configuring tap settings in slot %u (IP %u.%u.%u.%u)\n",
           slot[i],
           (ip_addr[i]>>24) & 0xff, (ip_addr[i]>>16) & 0xff, (ip_addr[i]>>8) & 0xff, ip_addr[i] & 0xff);
    usleep(1*1000);
  }

  printf("\nDone!\n");

  return 0;
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

  for (i=0; i<n_slots; i++)
  {
    ip_addr[i] = ip_addr_slot[slot[i]];
  }

  /* Compose TX message */
  txmsg.protocolId   = 1;
  txmsg.srcId        = 7100;
  txmsg.dstId        = IPC_HW_PORTO_MSG_CXP;
  txmsg.flags        = 0;
  txmsg.counter      = rand ();
  txmsg.msgId        = 0x531;
  txmsg.infoDim      = 2;
  txmsg.info[0]      = mx;
  txmsg.info[1]      = 0xff;  /* All 4 ports */

  memset(results, 0xff, sizeof(results));

  printf("Reading BER results in remote linecards (n_slots=%u)\n",n_slots);

  for (i=0; i<n_slots; i++)
  {
    printf(" slot = %-2u  IP = 0x%08X\n", slot[i], ip_addr[i]);
  }

  /* For each slot, get remote values (just to reset them!) (4 ports at once) */
  for (i=0; i<n_slots; i++) {
    memset(rxmsg.info, 0xFF, 4*sizeof(int));
    ret = send_data (canal_ipc, IPC_HW_PORTO_MSG_CXP, ip_addr[i], &txmsg, &rxmsg);
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
  txmsg.dstId        = IPC_HW_PORTO_MSG_CXP;
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
  ret = send_data (canal_ipc, IPC_HW_PORTO_MSG_CXP, ip_addr, &txmsg, &rxmsg);

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
  txmsg.dstId        = IPC_HW_PORTO_MSG_CXP;
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
  ret = send_data (canal_ipc, IPC_HW_PORTO_MSG_CXP, ip_addr, &txmsg, &rxmsg);

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
  txmsg.dstId        = IPC_HW_PORTO_MSG_CXP;
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
  ret = send_data (canal_ipc, IPC_HW_PORTO_MSG_CXP, ip_addr, &txmsg, &rxmsg);

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
  txmsg.dstId        = IPC_HW_PORTO_MSG_CXP;
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
  ret = send_data (canal_ipc, IPC_HW_PORTO_MSG_CXP, ip_addr, &txmsg, &rxmsg);

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

  for (i=0; i<n_slots; i++)
  {
    ip_addr[i] = ip_addr_slot[slot[i]];
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

  for (i=0; i<n_slots; i++)
  {
    ip_addr[i] = ip_addr_slot[slot[i]];
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

  for (i=0; i<n_slots; i++)
  {
    ip_addr[i] = ip_addr_slot[slot[i]];
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

  for (i=0; i<n_slots; i++)
  {
    ip_addr[i] = ip_addr_slot[slot[i]];
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
         "   fp.cli m 1006 <en/dis: 0/1> <port#1> <port#2> <...>\n"
         "\n"
         " Read BER counter for a lists of ports\n"
         "   fp.cli m 1007 <port#1> <port#2>\n"
         "\n"
         " Initialize BER meter functions (-1 to let fw to guess)\n"
         "   fp.shell dev ber_init <mx_slot>\n"
         "\n"
         " Stop running BER threads\n"
         "   fp.shell dev ber_stop\n"
         "\n"
         " Read current tap settings applied to a port:\n"
         "   fp.shell dev ptin_tapsettings_dump <port>\n"
         " Apply new tap settings to a port:\n"
         "   fp.shell dev ptin_tapsettings_set <port> <pre> <main> <post> <force>\n"
         "\n"
         " Run BER meter at RX BCM\n"
         "   fp.shell dev \"get_rx_ber('<file>', <optimize_lc>, <n_iters>,\n"
         "                              <reset_delay>, <start delay>, <test time>, <mode>,\n"
         "                              <pre_start>, <pre_end>, <pre_step>,\n"
         "                              <main_start>, <main_end>, <main_step>,\n"
         "                              <post_start>, <post_end>, <port_step>,\n"
         "                              <n_groups>,\n"
         "                              <n_slots0>, <start_slot0>,\n"
         "                              <n_slots1>, <start_slot1>,\n"
         "                              <...>)\"\n"
         "NOTE: PRE aplicable just for line cards with Cortina PHY (like TG16GF)."
         "Otherwise use (pre_start, pre_end, pre_step)=(15,15,1), the default PRE value."
         " Run BER meter at RX Line Card\n"
         "   fp.shell dev \"get_tx_ber('<file>', <optimize_lc>, <n_iters>,\n"
         "                              <reset_delay>, <start delay>, <test time>, <mode>,\n"
         "                              <main_start>, <main_end>, <main_step>,\n"
         "                              <post_start>, <post_end>, <port_step>,\n"
         "                              <n_groups>,\n"
         "                              <n_slots0>, <start_slot0>,\n"
         "                              <n_slots1>, <start_slot1>,\n"
         "                              <...>)\"\n"
         " Input parameters for get_rx_ber and get_tx_ber routines:\n"
         "   lc_optimize   : 0x00 -> No optimization\n"
         "                   0x01 -> Applying init procedure 0x8000->1x8036, 0->0xB0\n"
         "                   0x02 -> Reset Copper mode on LC for each iteration\n"
         "                   0x04 -> Make aditional register readings (applied only for Vitess devices - TX test)\n"
         "   n_iters       : Number of test repetitions\n"
         "   reset_delay   : Time delay in seconds before reseting reception device\n"
         "   start_delay   : Time delay in seconds before start a PRBS test\n"
         "   test_delay    : PRBS test duration\n"
         "   mode[bit 0]   : 0 -> Only decrement main cursor, after running all post cursors range;\n"
         "                   1 -> Increment post cursor and decrement main cursor simultaneously\n"
         "   mode[bit 1]   : 0 -> Only apply start_delay for the first iteration\n"
         "                   1 -> Apply start_delay for all iterations\n"
         "   mode[bit 2]   : 0 -> Make a normal post cursor running from start to final value\n"
         "                   1 -> Make a bidirection post cursor running to avoid high jumps\n"
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
         "   fp.shell dev init_remote_ber <enable>\n"
         "                                      <n_groups>\n"
         "                                      <n_slots0> <start_slot0>\n"
         "                                      <n_slots1> <start_slot1>\n"
         "                                      <...>\n"
         "\n"
         " Enable BER in remote linecards\n"
         "   fp.shell dev enable_remote_ber <n_groups>\n"
         "                                        <n_slots0> <start_slot0>\n"
         "                                        <n_slots1> <start_slot1>\n"
         "                                        <...>\n"
         "\n"
         " Disable BER in remote linecards\n"
         "   fp.shell dev disable_remote_ber <n_groups>\n"
         "                                         <n_slots0> <start_slot0>\n"
         "                                         <n_slots1> <start_slot1>\n"
         "                                         <...>\n"
         "\n"
         " Configure tap settings in remote linecards\n"
         "   fp.shell dev set_remote_tapcursors <precursor> <maincursor> <postcursor>"
         "                                      <n_groups>\n"
         "                                      <n_slots0> <start_slot0>\n"
         "                                      <n_slots1> <start_slot1>\n"
         "                                      <...>\n"
         "\n"
         " Get remote BER measurements in remote linecards\n"
         "   fp.shell dev get_remote_ber <n_groups>\n"
         "                                     <n_slots0> <start_slot0>\n"
         "                                     <n_slots1> <start_slot1>\n"
         "                                     <...>\n"
         "\n"
         " Read remote Vitesse register on a Line Card\n"
         " (if mmd[8] bit is set - 0x1?? - channel referencing is insensitive to failover mode)\n"
         "   fp.shell dev get_remote_reg <mmd> <addr>\n"
         "                                     <n_groups>\n"
         "                                     <n_slots0> <start_slot0>\n"
         "                                     <n_slots1> <start_slot1>\n"
         "                                     <...>\n"
         "\n"
         " Write remote Vitesse register on a Line Card\n"
         " (if mmd[8] bit is set - 0x1?? - channel referencing is insensitive to failover mode)\n"
         "   fp.shell dev set_remote_reg <mmd> <addr> <value>\n"
         "                                     <n_groups>\n"
         "                                     <n_slots0> <start_slot0>\n"
         "                                     <n_slots1> <start_slot1>\n"
         "                                     <...>\n"
         "\n"
         " Read remote Vitesse firmware variable on a Line Card\n"
         "(mmd=0: XAUI channels read; mmd=1: PMA channels read)\n"
         "   fp.shell dev get_remote_var <mmd> <addr>\n"
         "                                     <n_groups>\n"
         "                                     <n_slots0> <start_slot0>\n"
         "                                     <n_slots1> <start_slot1>\n"
         "                                     <...>\n"
         "\n"
         " Write remote Vitesse firmware variable on a Line Card\n"
         "(mmd=0: XAUI channels written; mmd=1: PMA channels written; mmd=2: both channels pair written)\n"
         "   fp.shell dev set_remote_var <mmd> <addr> <value>\n"
         "                                     <n_groups>\n"
         "                                     <n_slots0> <start_slot0>\n"
         "                                     <n_slots1> <start_slot1>\n"
         "                                     <...>\n"
         "\n"
         " Example commands:\n"
         "  fp.shell dev \"get_remote_reg('test_reg.txt', 1,0x8037, 1,1,3)\"\n"
         "  fp.shell dev \"get_tx_ber('test_tx.log', 0x02,1,5,20,60, 0x01, 63,32,2, 0,31,2, 1,1,3)\"\n"
         "  fp.shell dev \"get_rx_ber('test_rx.log', 0x00,1,5,20,5,  0x02, 15,16,1, 63,32,2, 0,31,2, 2, 8,2, 8,12)\"\n"
         "  fp.cli m 1007 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63\n"
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

void ptin_crash(void)
{
  int *p = L7_NULLPTR;

  *p = 1;
}

#if 1

void ptin_ring_set(int unit, uint32 flags, uint16 vlan, bcm_port_t port0, bcm_port_t port1)
{
#if 0
  typedef struct bcm_failover_element_s {
      uint32 flags;               /* flags */
      bcm_gport_t port;           /* MPLS/MiM/VLAN gport */
      bcm_failover_t failover_id; /* Failover Object Index. */
      bcm_if_t intf;              /* Egress Object index. */
  } bcm_failover_element_t;

  /* Failover Ring Structure. */
  typedef struct bcm_failover_ring_s {
      uint32 flags;                   /* flags */
      bcm_vlan_vector_t vlan_vector;  /* List of Vlans. */
      bcm_gport_t port0;              /* VLAN gport */
      bcm_gport_t port1;              /* VLAN gport */
  } bcm_failover_ring_t;


  /* L2 ring structure. */
  typedef struct bcm_l2_ring_s {
      uint32 flags;                   /* flags */
      bcm_vlan_vector_t vlan_vector;  /* List of Vlans. */
      bcm_gport_t port0;              /* VLAN gport */
      bcm_gport_t port1;              /* VLAN gport */
  } bcm_l2_ring_t;

  bcm_gport_t gport;

  // Calculate gport
  BCM_GPORT_LOCAL_SET(gport,53);  //xe3
  BCM_GPORT_LOCAL_SET(gport,50);  //xe0



  bcm_failover_element_t ring_elem;
  bcm_failover_ring_t_init(&ring_elem);

  ring_elem.flags = BCM_FAILOVER_CLEAR | BCM_FAILOVER_LEARN_DISABLE;

  ring_elem.port = ;

  typedef uint32 bcm_vlan_vector_t[_SHR_BITDCLSIZE(BCM_VLAN_COUNT)];

  #define BCM_VLAN_VEC_GET(vec, n)  SHR_BITGET(vec, n)
  #define BCM_VLAN_VEC_SET(vec, n)  SHR_BITSET(vec, n)
  #define BCM_VLAN_VEC_CLR(vec, n)  SHR_BITCLR(vec, n)
  #define BCM_VLAN_VEC_ZERO(vec)

#endif

  bcm_failover_ring_t failover_ring;
  bcm_gport_t gport0;
  bcm_gport_t gport1;
  bcm_error_t rv;

  bcm_failover_ring_t_init(&failover_ring);

  failover_ring.flags = flags;

  // Calculate gport
  BCM_GPORT_LOCAL_SET(gport0, port0);
  BCM_GPORT_LOCAL_SET(gport1, port1);

  failover_ring.port0 = gport0;
  failover_ring.port1 = gport1;

  BCM_VLAN_VEC_SET(failover_ring.vlan_vector, vlan);

  rv = bcm_failover_ring_config_set(unit, &failover_ring);
  printf("%s(%d) rv=%u (\"%s\")\r\n", __FUNCTION__, __LINE__, rv, bcm_errmsg(rv));
//bcm_failover_ring_t_init(&failover_ring);
//rv = bcm_failover_ring_config_set(unit, &failover_ring);
//printf("%s(%d) rv=%u (\"%s\")\r\n", __FUNCTION__, __LINE__, rv, bcm_errmsg(rv));

#if 0
bcm_failover_ring_t fr;
bcm_failover_ring_t_init(&fr);
fr.flags = BCM_FAILOVER_LOOKUP_DISABLE | BCM_FAILOVER_LEARN_DISABLE;
fr.vlan_vector[0] = 4;
print bcm_port_gport_get(0, 0, &fr.port0);
print bcm_port_gport_get(0, 5, &fr.port1);
print bcm_failover_ring_config_set(0, &fr);
bcm_failover_ring_t_init(&fr);
print bcm_failover_ring_config_set(0, &fr);
#endif

}

void ptin_ring_get(int unit)
{
#if 0
  typedef struct bcm_failover_element_s {
      uint32 flags;               /* flags */
      bcm_gport_t port;           /* MPLS/MiM/VLAN gport */
      bcm_failover_t failover_id; /* Failover Object Index. */
      bcm_if_t intf;              /* Egress Object index. */
  } bcm_failover_element_t;

  /* Failover Ring Structure. */
  typedef struct bcm_failover_ring_s {
      uint32 flags;                   /* flags */
      bcm_vlan_vector_t vlan_vector;  /* List of Vlans. */
      bcm_gport_t port0;              /* VLAN gport */
      bcm_gport_t port1;              /* VLAN gport */
  } bcm_failover_ring_t;


  /* L2 ring structure. */
  typedef struct bcm_l2_ring_s {
      uint32 flags;                   /* flags */
      bcm_vlan_vector_t vlan_vector;  /* List of Vlans. */
      bcm_gport_t port0;              /* VLAN gport */
      bcm_gport_t port1;              /* VLAN gport */
  } bcm_l2_ring_t;

  bcm_gport_t gport;

  // Calculate gport
  BCM_GPORT_LOCAL_SET(gport,53);  //xe3
  BCM_GPORT_LOCAL_SET(gport,50);  //xe0



  bcm_failover_element_t ring_elem;
  bcm_failover_ring_t_init(&ring_elem);

  ring_elem.flags = BCM_FAILOVER_CLEAR | BCM_FAILOVER_LEARN_DISABLE;

  ring_elem.port = ;

  typedef uint32 bcm_vlan_vector_t[_SHR_BITDCLSIZE(BCM_VLAN_COUNT)];

  #define BCM_VLAN_VEC_GET(vec, n)  SHR_BITGET(vec, n)
  #define BCM_VLAN_VEC_SET(vec, n)  SHR_BITSET(vec, n)
  #define BCM_VLAN_VEC_CLR(vec, n)  SHR_BITCLR(vec, n)
  #define BCM_VLAN_VEC_ZERO(vec)

#endif

  bcm_failover_ring_t failover_ring;
  bcm_error_t rv;

  rv = bcm_failover_ring_config_get(unit, &failover_ring);
  printf("%s(%d) rv=%u (\"%s\")\r\n", __FUNCTION__, __LINE__, rv, bcm_errmsg(rv));

  printf("\nflags %Xh, gport0 %d, gport1 %d", failover_ring.flags, failover_ring.port0, failover_ring.port1);

}

void ptin_ring_replace(int unit, uint32 flags, uint16 vlan, bcm_port_t port0, bcm_port_t port1)
{
#if 0
  /* L2 ring structure. */
  typedef struct bcm_l2_ring_s {
      uint32 flags;                   /* flags */
      bcm_vlan_vector_t vlan_vector;  /* List of Vlans. */
      bcm_gport_t port0;              /* VLAN gport */
      bcm_gport_t port1;              /* VLAN gport */
  } bcm_l2_ring_t;
#endif

  bcm_l2_ring_t l2_ring;
  bcm_gport_t gport0;
  bcm_gport_t gport1;
  bcm_error_t rv;


  bcm_l2_ring_t_init(&l2_ring);

   // Calculate gport
  BCM_GPORT_LOCAL_SET(gport0, port0);
  BCM_GPORT_LOCAL_SET(gport1, port1);

  l2_ring.port0 = gport0;
  l2_ring.port1 = gport1;

  l2_ring.flags = flags; //BCM_L2_REPLACE_PROTECTION_RING;

  BCM_VLAN_VEC_SET(l2_ring.vlan_vector, vlan);

  rv = bcm_l2_ring_replace(unit, &l2_ring);

  printf("%s(%d) rv=%u (\"%s\")\r\n", __FUNCTION__, __LINE__, rv, bcm_errmsg(rv));

#if 0
bcm_l2_ring_t l2r;
bcm_l2_ring_t_init(&l2r);
l2r.flags = BCM_L2_REPLACE_PROTECTION_RING;
l2r.vlan_vector[0] = 4;
print bcm_port_gport_get(0, 0, &l2r.port0);
print bcm_port_gport_get(0, 5, &l2r.port1);
print bcm_l2_ring_replace(0, &l2r);
#endif

}

#endif
