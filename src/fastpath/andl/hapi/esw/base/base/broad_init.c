/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_init.c
*
* @purpose   This file contains the broad hapi interface and other common broad
*            routines
*
* @component hapi
*
* @comments
*
* @create    2/22/2001
*
* @author    nsummers
*
* @end
*
**********************************************************************/

#define GLOBAL_HAPI_DEBUG_PRINTF

/* broad hapi */
#include "dapi_db.h"
#include "broad_common.h"
#include "broad_port.h"
#include "broad_stats.h"
#include "broad_debug.h"
#include "broad_l2_std.h"
#include "broad_l3.h"
#include "broad_l2_vlan.h"
#include "broad_l2_mcast.h"
#include "broad_l2_lag.h"
#include "broad_ptin.h"     /* PTin added: startup configuration */
#include "ibde.h"

#include "osapi.h"
#include "osapi_support.h"
#include "sysapi.h"
#include "platform_config.h"
#include "registry.h"

#include "broad_policy.h"
#include "broad_cos_util.h"
#include "broad_l2_ipsg.h"

#ifdef L7_QOS_PACKAGE
#include "broad_qos.h"
#endif

#ifdef L7_MCAST_PACKAGE
#include "broad_l3_mcast.h"
#include "l7_mcast_api.h"
#include "l7_usl_bcmx_ipmcast.h"
#endif

#ifdef L7_WIRELESS_PACKAGE
#include "broad_l2_tunnel.h"
#endif

#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
#include "broad_dot1ad.h"
#include "broad_l2_dot1ad.h"
#endif
#include "broad_l2_dot3ah.h"
#endif

/* broad hw */
#include "phy_hapi.h"
#include "default_cnfgr.h"

#include "sal/core/boot.h"
#include "sal/appl/sal.h"
#include "bcm/init.h"
#include "bcm/link.h"
#include "bcm/port.h"
#include "bcm/cosq.h"
#include "bcm/ipmc.h"
#include "bcm/stack.h"
#include "bcmx/lport.h"
#include "bcmx/link.h"
/* PTin modified: SDK 6.3.0 */
#include "ptin_globaldefs.h"
#if (SDK_VERSION_IS >= SDK_VERSION(6,0,0,0))
/* No include */
#else
#include "bcmx/filter.h"
#endif
#include "bcmx/switch.h"
#include "ibde.h"
#include "l7_usl_api.h"
#include "l7_usl_sm.h"
#include "l7_usl_port_db.h"
#include "l7_usl_bcmx_l2.h"
#include "spm_api.h"
#include "unitmgr_api.h"
#include "cardmgr_api.h"

#include <bcmx/bcmx.h>
#include <bcmx/bcmx_int.h>
/*#include <bcmx/boards.h>*/
#include <bcmx/port.h>
#include <bcmx/ipmc.h>
#include <bcmx/cosq.h>
#include "sysbrds.h"
#include <soc/debug.h>
#include <soc/drv.h>
#include "appl/stktask/stktask.h"
#include "appl/stktask/attach.h"
#include "bcm_int/rpc/rlink.h"

#include "l7_usl_bcmx_port.h"
#include "pfc_exports.h"

#ifdef L7_STACKING_PACKAGE
#include "broad_stack_mgr.h"

extern void dapiTraceShow(L7_uint32 count,L7_uchar8 *db,L7_BOOL unformat);
extern void fp_stk_mgr_system_db_dump();
extern void ut(L7_uint32 i);
extern void osapiDebugStackTrace (L7_uint32 task_id, FILE *filePtr);

#endif

/* PTin added: packet trap */
#include "logger.h"
#include "ptin_hapi.h"

/* Max time to wait for a unit to be discovered by lower stacking layer */
#define BROAD_UNIT_DISCOVER_TIMEOUT   (7000) /* 5 sec */
#define BROAD_UNIT_DISCOVER_RETRIES   (3)
#define BROAD_MGMT_UNIT_DISCOVER_RETRIES (30)

extern void *hapiLinkStatusQueue;

L7_RC_t broadDriverStart(void *data);

static int hapi_broad_modid_modport_to_lport [BCM_UNITS_MAX][HAPI_BROAD_MAX_PORTS_PER_CPU_UNIT];

void hapiBroadModidModportToLportSet (int mod_id, int mod_port, int lport)
{
  hapi_broad_modid_modport_to_lport[mod_id][mod_port] = lport;
}

void hapiBroadModidModportToLportGet (int mod_id, int mod_port, int *lport)
{
  *lport = hapi_broad_modid_modport_to_lport[mod_id][mod_port];
}


/*********************************************************************
*
* @purpose Return the number of MAC addresses in the shadow table.
*
* @param   none
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_uint32 hapiBroadLocalMacCountGet(void)
{
  int                  i;
  soc_control_t        *soc;
  int                  max_count = 0;
  int                  count;

  for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
  {
    if (SOC_IS_HERCULES(i))
    {
      continue;
    }
    soc = SOC_CONTROL (i);

    count = shr_avl_count (soc->arlShadow);
    if (count > max_count)
    {
      max_count = count;
    }
  }

  return max_count;
}

#if defined(FEAT_METRO_CPE_V1_0)
L7_RC_t hapiBroadSystemIpv6PacketsToCpuPolicyInstall(DAPI_t *dapi_g)
{
  L7_RC_t                 rc = L7_SUCCESS;
  BROAD_POLICY_RULE_t     ruleId       = BROAD_POLICY_RULE_INVALID;

  L7_uchar8               ipV6_format  = BCM_FIELD_PKT_FMT_IPV6;
  L7_ushort16             ipV6_ethtype = L7_ETYPE_IPV6;
  L7_uchar8               ip_icmpv6    = IP_PROT_ICMPV6;
  L7_uchar8               mldSnoop_macda[]      = {0x33, 0x33, 0x00, 0x00, 0x00, 0x00};
  L7_uchar8               mldSnoop_macda_mask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_ALL,
                                                   FIELD_MASK_ALL,  FIELD_MASK_ALL,  FIELD_MASK_ALL};

  L7_uchar8               exact_match[]         = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                                   FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  BROAD_SYSTEM_t     *hapiSystem;

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;


  /*On bcm53115 with DOT1AD package, dot1ad related rules in TCAM will be
    hit first for MLD packets (as DOT1AD rules are only based on VLAN tags).
    As a result MLD packets will not be trapped to CPU.
    To fix this problem, we need to install protocol snooping rules at higher
    priority compared to DOT1AD rules.*/

  rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_DOT1AD_SNOOP);
    do
    {
    /* give MDL frames high priority and trap to the CPU. */
    if ((rc = hapiBroadPolicyRuleAdd(&ruleId)) != L7_SUCCESS)
    {
         break;
       }
    if ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                             BROAD_FIELD_MACDA,
                                             mldSnoop_macda, mldSnoop_macda_mask)) !=
                                             L7_SUCCESS)
    {
         break;
       }
    if ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                             BROAD_FIELD_ETHTYPE,
                                             (L7_uchar8 *)&ipV6_ethtype, exact_match)) !=
                                             L7_SUCCESS)
    {
        break;
    }

    if ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId,
                                             BROAD_FIELD_IP6_NEXTHEADER,
                                             (L7_uchar8 *)&ip_icmpv6, exact_match)) !=
                                             L7_SUCCESS)
    {
        break;
    }

    if ((rc = hapiBroadPolicyRuleQualifierAdd(ruleId,
             BROAD_FIELD_PKT_FORMAT,
             (L7_uchar8 *)&ipV6_format, exact_match)) !=
             L7_SUCCESS)
    { 
       break;
     }
    if ((rc = hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ,
                                          HAPI_BROAD_INGRESS_MED_PRIORITY_COS-1, 0, 0)) !=
                                          L7_SUCCESS)
    {
      break;
    }
    rc = hapiBroadPolicyRuleActionAdd(ruleId,
                                      BROAD_ACTION_SET_REASON_CODE,
                                      8, 0, 0);
     
    if ((rc = hapiBroadPolicyRuleActionAdd(ruleId,
                                          BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0)) !=
                                          L7_SUCCESS)
    {
      break;
    }

  }while (0);

  if (rc == L7_SUCCESS)
  {
    /* Commit the policy */
    rc = hapiBroadPolicyCommit(&hapiSystem->ipv6SnoopId);
  }
  else
  {
    hapiBroadPolicyCreateCancel();
  }
  if ( rc == L7_FAILURE)
  {
    hapiSystem->ipv6SnoopId = BROAD_POLICY_INVALID;
  }

  return rc;

}
#endif


/*********************************************************************
*
* @purpose Install system policies for prioritizing traffic on Raptor.
*
* @param   DAPI_t *
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemPolicyInstallRaptor(DAPI_t *dapi_g)
{
  BROAD_SYSTEM_t     *hapiSystem = L7_NULLPTR;
  L7_RC_t             result;
  L7_ushort16         arp_ethtype = L7_ETYPE_ARP;
  L7_ushort16         eap_ethtype = L7_ETYPE_EAPOL;
  L7_uchar8           res_macda[]   = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x00};
  L7_uchar8           bcast_macda[] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#ifdef L7_ISDP_PACKAGE
  L7_uchar8           isdp_macda[]  = {0x01, 0x00, 0x0c, 0xcc, 0xcc, 0xcc};
#endif
  L7_uchar8           res_macmask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, 0xC0};
  L7_uchar8           res_mac_drop_mask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                             FIELD_MASK_ALL, FIELD_MASK_ALL, FIELD_MASK_ALL};
  L7_uchar8           exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  BROAD_POLICY_t      sysId1 = BROAD_POLICY_INVALID, sysId2 = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t ruleId;
#ifdef BCM_ROBO_SUPPORT
  L7_ushort16       sntp_dport    = 123;
  L7_uchar8         ip_udp_proto[]   = {IP_PROT_UDP}; 
#endif

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  /* Create policy for L2 specific information, e.g. BPDUs */
  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);   /* Policy 1 */

  /* give dot1x EAPOL packets high priority so they reach the cpu */
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&eap_ethtype, exact_match);
#ifdef BCM_ROBO_SUPPORT
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_REASON_CODE,
                                       BCM_ROBO_RX_REASON_PROTOCOL_TERMINATION, 0, 0);
#else
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
#endif
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);

  /* Drop BPDUs which are within the reserved range but not used. Add rule
     with least priority for the valid address range to trap to CPU */
  hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_LOWEST);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, res_macda, res_mac_drop_mask);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);

  /* give BPDUs high priority */
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, res_macda, res_macmask);
#ifdef BCM_ROBO_SUPPORT
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_REASON_CODE,
                                       BCM_ROBO_RX_REASON_PROTOCOL_TERMINATION, 0, 0);
#else
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_BPDU_COS, 0, 0);
#endif
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);

#ifdef L7_ISDP_PACKAGE
  /* trap cdp/dtp/vtp/pagp traffic */
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, isdp_macda, exact_match);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
#endif /* L7_ISDP_PACKAGE */

  /* give ARP requests high priority, so if the CPU is slammed w/ data traffic, it will
  ** be more likely to get ARPs. ARP replies to switch MAC address will reach CPU as a result
  ** of L2 entry. Also, the system MAC filter assigns higher priority too. ARP replies to
  ** router/VRRP MAC address are handled in L3 code. Idea is to avoid tunneling ARP replies.
  */
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&arp_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, bcast_macda, exact_match);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
#ifdef BCM_ROBO_SUPPORT
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_REASON_CODE,
                                            BCM_ROBO_RX_REASON_PROTOCOL_SNOOP, 0, 0);
#endif

  result = hapiBroadPolicyCommit(&sysId1);

  if (L7_SUCCESS != result)
      return result;

#ifdef BCM_ROBO_SUPPORT /* ADDED for SNTP Broad packet */
   /* Create policy for L4 specific information, e.g. SNTPs */
  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);   /* Policy 2 */
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO, ip_udp_proto, exact_match);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT, (L7_uchar8 *)&sntp_dport, exact_match);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);

  result = hapiBroadPolicyCommit(&sysId2);
  if (L7_SUCCESS != result)
    return result;
#endif /* SNTP Broad packet */

#ifdef L7_STACKING_PACKAGE
  /* If this device is using front panel stacking then create a filter to
  ** block all traffic on the management vlan for non-stack ports.
  */
  if (cnfgrIsFeaturePresent (L7_FLEX_STACKING_COMPONENT_ID,
                             L7_FPS_FEATURE_ID) == L7_TRUE)
  {
     if (hapiSystem->fpsSysId == BROAD_POLICY_INVALID)
     {
       BROAD_POLICY_t      fpsId;
       BROAD_POLICY_RULE_t fpsRuleId;
       L7_ushort16         vlanId = HPC_STACKING_VLAN_ID;

       hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
       hapiBroadPolicyRuleAdd(&fpsRuleId);
       hapiBroadPolicyRuleQualifierAdd(fpsRuleId, BROAD_FIELD_OVID,
                                       (L7_uchar8*)&vlanId, exact_match);
       hapiBroadPolicyRuleActionAdd(fpsRuleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
       result = hapiBroadPolicyCommit(&fpsId);
       if (result == L7_SUCCESS)
       {
         hapiSystem->fpsSysId = fpsId;
       }
    }
  }
#endif

  if (L7_SUCCESS == result)
  {
    hapiSystem->sysId1 = sysId1;
    hapiSystem->sysId2 = sysId2;
  }



#if !(defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE))
  /* DHCP packets on ports must go to the CPU and be rate limited to 64 kbps */
  result = hapiBroadDhcpPolicyInstall(dapi_g);
  if (L7_SUCCESS != result)
    return result;
#endif
#if defined(L7_METRO_PACKAGE) && defined(L7_DOT1AD_PACKAGE)
  hapiSystem->dhcpSnoopingPolicyId = BROAD_POLICY_INVALID;
#endif

#if defined(FEAT_METRO_CPE_V1_0)
  hapiBroadSystemIpv6PacketsToCpuPolicyInstall(dapi_g);
#endif

  if(hapiBroadRoboVariantCheck() != __BROADCOM_53115_ID)
  {
    result = hapiBroadDot1xViolationPolicyCreate(dapi_g);
  }

  return result;
}

/* PTin added: Only allow packets trap and copy to CPU */
#define PTIN_BROAD_INIT_TRAP_TO_CPU       0
#define PTIN_BROAD_INIT_ALLOW_COS_CHANGE  0


/*********************************************************************
*
* @purpose Install system policies for prioritizing traffic.
*
* @param   DAPI_t *
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemPolicyInstall(DAPI_t *dapi_g)
{
  BROAD_SYSTEM_t     *hapiSystem;
  L7_RC_t             result;

  L7_ushort16         arp_ethtype = L7_ETYPE_ARP;
#if PTIN_BROAD_INIT_TRAP_TO_CPU
  L7_ushort16         ip_ethtype  = L7_ETYPE_IP;
  L7_ushort16         eap_ethtype = L7_ETYPE_EAPOL;
  L7_uchar8           vrrp_proto[]  = {IP_PROT_VRRP};
  L7_uchar8           udp_proto[]   = {IP_PROT_UDP};
  L7_uchar8           dhcp_serverPort[] = {0x00, 0x43}; /* DHCP Port 67 */
  L7_uchar8           dhcp_clientPort[] = {0x00, 0x44}; /* DHCP Port 68 */
  L7_uchar8           res_macda[]   = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x00};
#ifdef L7_ISDP_PACKAGE
  L7_uchar8           isdp_macda[]  = {0x01, 0x00, 0x0c, 0xcc, 0xcc, 0xcc};
  BROAD_POLICY_t      isdpId;
  BROAD_POLICY_RULE_t isdpRuleId;
#endif
  L7_uchar8           res_macmask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, 0xC0};
  L7_uchar8           res_mac_drop_mask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                             FIELD_MASK_ALL, FIELD_MASK_ALL, FIELD_MASK_ALL};
#endif
  BROAD_POLICY_RULE_t ruleId;
  BROAD_METER_ENTRY_t meterInfo;
  L7_uchar8           exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};

#if PTIN_BROAD_INIT_ALLOW_COS_CHANGE
#ifdef L7_IPV6_PACKAGE
  L7_ushort16         ipV6_ethtype = L7_ETYPE_IPV6;
#endif
  L7_uchar8           ospf_proto[]  = {IP_PROT_OSPFIGP};
  L7_uchar8           icmp_proto[]        = {IP_PROT_ICMP};
  L7_ushort16         icmp_rtr_solicit = 0x0a00;
  L7_ushort16         icmp_type_match  = 0xff00;
  L7_uchar8           tcp_proto[]   = {IP_PROT_TCP};
  L7_ushort16         rip_dport   = 0x0208;
  L7_ushort16         bgp_dport   = TCP_PORT_BGP;
  L7_uchar8           bcast_macda[]   = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif

  BROAD_POLICY_t      sysId1=0, sysId2=0, arpPolicyId=0;
  bcm_chip_family_t board_family;

  if ((hapiBroadRaptorCheck() == L7_TRUE) ||
      (hapiBroadRoboCheck()== L7_TRUE) || (hapiBroadHawkeyeCheck() == L7_TRUE) )
  {
    return hapiBroadSystemPolicyInstallRaptor(dapi_g);
  }

  hapiBroadGetSystemBoardFamily(&board_family);

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  /* PTin removed: Packets priority not modified */
  #if PTIN_BROAD_INIT_TRAP_TO_CPU
  /* Install the L2 system policies that trap PDUs to the CPU first. We need these to have
     higher priority than the dot1x violation policy so that the link layer protocols
     function correctly even if a port is unauthorized. */
  /* Create policy for L2 specific information, e.g. BPDUs */
  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);   /* Policy 2 of 2 */

  /* give dot1x EAPOL packets high priority so they reach the cpu */
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&eap_ethtype, exact_match);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  LOG_TRACE(LOG_CTX_STARTUP,"DOT1X EAPOL rule added");

  /* Drop BPDUs which are within the reserved range but not used. Add rule
     with least priority for the valid address range to trap to CPU */
  hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_LOWEST);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, res_macda, res_mac_drop_mask);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  LOG_TRACE(LOG_CTX_STARTUP,"BPDUs drop rule added");

  /* give BPDUs high priority */
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, res_macda, res_macmask);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_BPDU_COS, 0, 0);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  LOG_TRACE(LOG_CTX_STARTUP,"High priority BPDU rule added");

#ifdef L7_STACKING_PACKAGE
  hapiBroadPolicyEnableFPS(); /* Enable on FPS ports, if applicable */
#endif
  result = hapiBroadPolicyCommit(&sysId2);
  if (L7_SUCCESS != result)
    return result;
#endif

#if (0 /*PTIN_BOARD != PTIN_BOARD_OLT1T0*/)
  /* Install dot1x violation policy next, as it needs to have higher priority than other system policies. For
     example, we need to ensure that DHCP packets received on an unauthorized port get trapped to the CPU, rather
     than have their priority elevated. */
  result = hapiBroadDot1xViolationPolicyCreate(dapi_g);
  if (L7_SUCCESS != result)
    return result;
#else
  LOG_WARNING(LOG_CTX_STARTUP,"dot1x violation policy not installed!");
#endif

  /* DHCP packets on ports must go to the CPU and be rate limited to 64 kbps */
  result = hapiBroadDhcpPolicyInstall(dapi_g);
  if (L7_SUCCESS != result)
    return result;

  /* Install IP Source Guard default policy */
  result = hapiBroadIpsgDefaultPolicyInstall(dapi_g);
  if (L7_SUCCESS != result)
    return result;

  /* PTin removed: Packets priority not modified */
  #if PTIN_BROAD_INIT_TRAP_TO_CPU
  /* Create system policy to apply to all Ethernet ports (FE/GE/XG) - not stack ports.
  ** Add rules with highest precedence first. Note that until Rule Chaining is
  ** implemented on XGS3 it is not possible to combine all L2/3/4 fields into a single
  ** policy. Therefore, it is necessary to allocate two policies, one for MAC DA and
  ** the other for the remaining policies. It should not affect the operation.
  */
  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);   /* Policy 1 of 2 */

#ifdef L7_IPV6_PACKAGE
  /*
   * Add a rule to ensure that all tunnel packets that
   * miss L3 lookups are copied to the CPU fully encapsulated.
   * Otherwise the packets are delivered to the CPU decapsulated, 
   * and the software cannot determine the source tunnel interface
   * to process the packet correctly. 
   */
  {
  L7_ushort16 tunnel_hit    = osapiHtons(BROAD_LOOKUPSTATUS_TUNNEL_HIT);
  L7_ushort16 tunnel_l3_hit = osapiHtons((BROAD_LOOKUPSTATUS_TUNNEL_HIT | BROAD_LOOKUPSTATUS_L3_DST_HIT | BROAD_LOOKUPSTATUS_LPM_HIT));

  /* Note: This rule will send OSPFv3 packets coming over a tunnel to CPU 
     fully encapsulated and on higher priority.
   */
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_LOOKUP_STATUS, (L7_uchar8 *)&tunnel_hit, (L7_uchar8 *)&tunnel_l3_hit);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_COPY_TO_CPU, 0, 0, 0);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
  LOG_TRACE(LOG_CTX_STARTUP,"OSPFv3 rule added");
  }
#endif /* L7_IPV6_PACKAGE */

  /* give RIP frames high priority. We give multicast frames
  ** high priority in hapiBroadRoutingIntfLocalMcastAdd(), but RIPv1 and RIPv1c
  ** also use broadcast frames for establishing adjacency, so we need to
  ** guarantee that these get to the CPU if the CPU is getting slammed w/
  ** other data traffic.
  */
  #if PTIN_BROAD_INIT_ALLOW_COS_CHANGE
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO,   udp_proto,  exact_match);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT,   (L7_uchar8 *)&rip_dport,  exact_match);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
  LOG_TRACE(LOG_CTX_STARTUP,"RIP frames rule added");
  #endif

  /* give OSPF frames high priority. We give multicast frames
  ** high priority in hapiBroadRoutingIntfLocalMcastAdd(), but OSPF
  ** also uses unicast frames for establishing adjacency, so we need to
  ** guarantee that these get to the CPU if the CPU is getting slammed w/
  ** other data traffic.
  */
  #if PTIN_BROAD_INIT_ALLOW_COS_CHANGE
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO,   ospf_proto, exact_match);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
  LOG_TRACE(LOG_CTX_STARTUP,"OSPF frames rule added");
  #endif

  /* give BGP frames high priority. We need to gaurantee that these get to the CPU if the CPU
   * is slammed with other data traffic.
   */
  #if PTIN_BROAD_INIT_ALLOW_COS_CHANGE
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO,   tcp_proto,  exact_match);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT,   (L7_uchar8 *)&bgp_dport,  exact_match);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
  LOG_TRACE(LOG_CTX_STARTUP,"BGP frames rule added");
  #endif

  /* Copy VRRP frames to cpu */
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO,   vrrp_proto, exact_match);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_COPY_TO_CPU, 0, 0, 0);
  LOG_TRACE(LOG_CTX_STARTUP,"VRRP rule added");
  
  /* give Router solicitation frames medium priority and copy to the CPU.
   * Restricted to XGS3 due to resource constraints on XGS2.
   */
  if ((board_family != BCM_FAMILY_DRACO)  &&
      (board_family != BCM_FAMILY_TUCANA))
  {
    #if PTIN_BROAD_INIT_ALLOW_COS_CHANGE
    hapiBroadPolicyRuleAdd(&ruleId);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO,   icmp_proto, exact_match);
    /* The L4 src port and ICMP type share the same offset in the packet,
     * so just re-use the mechanism for qualifying on an L4 src port.
     * Note the mask only cares about 1 byte instead of 2.
     */
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_SPORT,   (L7_uchar8 *)&icmp_rtr_solicit, (L7_uchar8 *)&icmp_type_match);
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
    LOG_TRACE(LOG_CTX_STARTUP,"Router solicitation frames rule added");
    #endif
  }

  result = hapiBroadPolicyCommit(&sysId1);
  if (L7_SUCCESS != result)
      return result;
  #endif

#ifdef L7_IPV6_PACKAGE
  /* all link-local icmpv6 and OSPFv3 to cpu */
  {
    /* PTin removed: Packets priority not modified */
    #if PTIN_BROAD_INIT_ALLOW_COS_CHANGE
    BROAD_POLICY_t      Ip6SysId;
    L7_uchar8  hoplim = 255;
    L7_uchar8  icmp_prot = 58;

    hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
    hapiBroadPolicyRuleAdd(&ruleId);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ipV6_ethtype, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IP6_HOPLIMIT,   &hoplim, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IP6_NEXTHEADER,   &icmp_prot, exact_match);
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0,0);
    LOG_TRACE(LOG_CTX_STARTUP,"ICMPv6 frames rule added");

    /* Note that the rule above for OSPF packets will generally work for both IPv4 and IPv6. However, 
       in some cases (e.g. Helix), we use a UDF to qualify on some packet fields and in those cases
      the rule will only work for IPv4 packets. The rule below will ensure that in these cases that
      there is a rule to catch IPv6 OSPF packets as well. */
    hapiBroadPolicyRuleAdd(&ruleId);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ipV6_ethtype, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IP6_NEXTHEADER, (L7_uchar8 *)&ospf_proto, exact_match);
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0,0);
    LOG_TRACE(LOG_CTX_STARTUP,"OSPFv3 rule added");
    
    result = hapiBroadPolicyCommit(&Ip6SysId);
    if (result != L7_SUCCESS)
    {
       return result;
    }
    hapiSystem->Ip6SysId = Ip6SysId;
    #endif
  }
#endif /* L7_IPV6_PACKAGE */

  /* Dynamic ARP Inspection: ARP packets on untrusted ports must go to the CPU and be rate limited to 64 kbps */
  meterInfo.cir       = 512;
  meterInfo.cbs       = 64;
  meterInfo.pir       = 512;
  meterInfo.pbs       = 64;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;

  /* ARP packets on untrusted ports must go to the CPU and be rate limited to 64 kbps */
  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);

  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&arp_ethtype, exact_match);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS);
  hapiBroadPolicyRuleMeterAdd(ruleId, &meterInfo);
  LOG_TRACE(LOG_CTX_STARTUP,"ARP rule added");

  result = hapiBroadPolicyCommit(&hapiSystem->dynamicArpInspectUntrustedPolicyId);
  if (L7_SUCCESS != result)
      return result;

  result = hapiBroadPolicyRemoveFromAll(hapiSystem->dynamicArpInspectUntrustedPolicyId);
  if (L7_SUCCESS != result)
      return result;

  /* PTin removed: Packets priority not modified */
  #if PTIN_BROAD_INIT_ALLOW_COS_CHANGE
  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);

  /* give ARP requests high priority, so if the CPU is slammed w/ data traffic, it will
  ** be more likely to get ARPs. ARP replies to switch MAC address will reach CPU as a result
  ** of L2 entry. Also, the system MAC filter assigns higher priority too. ARP replies to
  ** router/VRRP MAC address are handled in L3 code. Idea is to avoid tunneling ARP replies.
  */
  /* PTin removed: Packets priority not modified */
  hapiBroadPolicyRuleAdd(&ruleId);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&arp_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, bcast_macda, exact_match);
  hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
  LOG_TRACE(LOG_CTX_STARTUP,"High priority ARP rule added");

  result = hapiBroadPolicyCommit(&arpPolicyId);
  if (L7_SUCCESS != result)
      return result;
  #endif

#ifdef L7_STACKING_PACKAGE
  /* If this device is using front panel stacking then create a filter to
  ** block all traffic on the management vlan for non-stack ports.
  */
  if (cnfgrIsFeaturePresent (L7_FLEX_STACKING_COMPONENT_ID,
                             L7_FPS_FEATURE_ID) == L7_TRUE)
  {
     if (hapiSystem->fpsSysId == BROAD_POLICY_INVALID)
     {
       /* PTin removed: Packets priority not modified */
       #if PTIN_BROAD_INIT_ALLOW_COS_CHANGE
       BROAD_POLICY_t      fpsId = BROAD_POLICY_INVALID;
       BROAD_POLICY_RULE_t fpsRuleId;
       L7_ushort16         vlanId = HPC_STACKING_VLAN_ID;

       hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
       hapiBroadPolicyRuleAdd(&fpsRuleId);
       hapiBroadPolicyRuleQualifierAdd(fpsRuleId, BROAD_FIELD_OVID,
                                       (L7_uchar8*)&vlanId, exact_match);
       hapiBroadPolicyRuleActionAdd(fpsRuleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
       LOG_TRACE(LOG_CTX_STARTUP,"Rule added");

       result = hapiBroadPolicyCommit(&fpsId);
       if (result == L7_SUCCESS)
       {
         hapiSystem->fpsSysId = fpsId;
       }
       LOG_TRACE(LOG_CTX_STARTUP,"RIP frames rule added");
       #endif
    }
  }
#endif

#ifdef L7_ISDP_PACKAGE
   /* Only install this policy if LLPF is not present. Otherwise the LLPF code manages ISDP policies. */
   if(cnfgrIsFeaturePresent(L7_LLPF_COMPONENT_ID, L7_LLPF_BLOCK_ISDP_FEATURE_ID) == L7_FALSE)
   {
     /* PTin removed: packet trap */
     #if PTIN_BROAD_INIT_TRAP_TO_CPU
     /* trap cdp/udld/dtp/vtp/pagp traffic */
     if (L7_SUCCESS == hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM_PORT))
     {
       if ((L7_SUCCESS == hapiBroadPolicyPriorityRuleAdd(&isdpRuleId, BROAD_POLICY_RULE_PRIORITY_HIGHEST)) &&
           (L7_SUCCESS == hapiBroadPolicyRuleQualifierAdd(isdpRuleId, BROAD_FIELD_MACDA, isdp_macda, exact_match)) &&
           (L7_SUCCESS == hapiBroadPolicyRuleActionAdd(isdpRuleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0)) &&
           (L7_SUCCESS == hapiBroadPolicyRuleActionAdd(isdpRuleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0)))
       {
         LOG_TRACE(LOG_CTX_STARTUP,"cdp/udld/dtp/vtp/pagp frames rule added");
         if (L7_SUCCESS == hapiBroadPolicyCommit(&isdpId))
         {
           hapiSystem->isdpSysId = isdpId;
         }
       }
       else
       {
         hapiBroadPolicyCreateCancel();
       }
     }
     #endif
   }
#endif

  /* Create policy to receive DHCP packets at CPU for voice VLAN. */
  if ((L7_TRUE == cnfgrIsComponentPresent(L7_VOICE_VLAN_COMPONENT_ID)) &&
      (BROAD_POLICY_INVALID == hapiSystem->voiceDhcpSysId))
  {
    /* PTin removed: Packets priority not modified */
    #if PTIN_BROAD_INIT_TRAP_TO_CPU
    BROAD_POLICY_t      voiceId;
    BROAD_POLICY_RULE_t voiceRuleId;

    do
    {
      if (L7_SUCCESS == hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM_PORT))
      {
        if (L7_SUCCESS != hapiBroadPolicyPriorityRuleAdd(&voiceRuleId, BROAD_POLICY_RULE_PRIORITY_HIGHEST))
        {
          hapiBroadPolicyCreateCancel();
          break;
        }
        if (L7_SUCCESS != hapiBroadPolicyRuleQualifierAdd(voiceRuleId, BROAD_FIELD_ETHTYPE,(L7_uchar8 *)&ip_ethtype, exact_match))
        {
          hapiBroadPolicyCreateCancel();
          break;
        }
        if (L7_SUCCESS != hapiBroadPolicyRuleQualifierAdd(voiceRuleId, BROAD_FIELD_PROTO,   udp_proto,  exact_match))
        {
          hapiBroadPolicyCreateCancel();
          break;
        }
        if (L7_SUCCESS != hapiBroadPolicyRuleQualifierAdd(voiceRuleId, BROAD_FIELD_DPORT, dhcp_serverPort, exact_match))
        {
          hapiBroadPolicyCreateCancel();
          break;
        }
        if (L7_SUCCESS != hapiBroadPolicyRuleQualifierAdd(voiceRuleId, BROAD_FIELD_SPORT, dhcp_clientPort, exact_match))
        {
          hapiBroadPolicyCreateCancel();
          break;
        }
        if (L7_SUCCESS != hapiBroadPolicyRuleActionAdd(voiceRuleId, BROAD_ACTION_COPY_TO_CPU, 0, 0, 0))
        {
          hapiBroadPolicyCreateCancel();
          break;
        }
        LOG_TRACE(LOG_CTX_STARTUP,"DHCP frames rule added");

        if (L7_SUCCESS == hapiBroadPolicyCommit(&voiceId))
        {
          hapiSystem->voiceDhcpSysId = voiceId;
        }
      }
    } while (0);
    #endif
  }

  if (L7_SUCCESS == result)
  {
    hapiSystem->sysId1 = sysId1;
    hapiSystem->sysId2 = sysId2;
    hapiSystem->arpPolicyId = arpPolicyId;
  }

#ifdef L7_MCAST_PACKAGE
  hapiBroadConfigL3McastFilter(L7_TRUE);
  LOG_TRACE(LOG_CTX_STARTUP,"Multicast package");
#endif  
#ifdef L7_LLPF_PACKAGE
  /*Drop rules to discard LLPF PDU's like cdp/dtp/vtp/pagp/sstp*/
  result = hapiBroadLlpfPolicyInstall(dapi_g);
  LOG_TRACE(LOG_CTX_STARTUP,"LLPF rules added");
  if(L7_SUCCESS != result)
  {
    return result;
  }
#endif

  /* PTin added */
  #if 1
  result = hapiBroadSystemInstallPtin_preInit();
  if(L7_SUCCESS != result)
  {
    return result;
  }
  #endif

  LOG_INFO(LOG_CTX_STARTUP,"Default rules initialization finished! (result=%d)",result);

  return result;
}
/*********************************************************************
*
* @purpose Remove system policies for prioritizing traffic.
*
* @param   DAPI_t *
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadSystemPolicyRemove(DAPI_t *dapi_g)
{
    BROAD_SYSTEM_t     *hapiSystem;
    L7_RC_t             result = L7_SUCCESS;

    hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

    /* Remove IP Source Guard default policy */
    hapiBroadIpsgDefaultPolicyRemove(dapi_g);

    if (BROAD_POLICY_INVALID != hapiSystem->dhcpSnoopingPolicyId) {
        if (hapiBroadPolicyDelete(hapiSystem->dhcpSnoopingPolicyId) != L7_SUCCESS)
            result = L7_FAILURE;
        
        hapiSystem->dhcpSnoopingPolicyId = BROAD_POLICY_INVALID;
    }

    if (BROAD_POLICY_INVALID != hapiSystem->dhcpDefaultPolicyId) {
        if (hapiBroadPolicyDelete(hapiSystem->dhcpDefaultPolicyId) != L7_SUCCESS)
            result = L7_FAILURE;

        hapiSystem->dhcpDefaultPolicyId = BROAD_POLICY_INVALID;
    }

    if (BROAD_POLICY_INVALID != hapiSystem->dot1xViolationPolicyId) {
        if (hapiBroadPolicyDelete(hapiSystem->dot1xViolationPolicyId) != L7_SUCCESS)
            result = L7_FAILURE;

        hapiSystem->dot1xViolationPolicyId = BROAD_POLICY_INVALID;
    }

    if (BROAD_POLICY_INVALID != hapiSystem->dynamicArpInspectUntrustedPolicyId) {
        if (hapiBroadPolicyDelete(hapiSystem->dynamicArpInspectUntrustedPolicyId) != L7_SUCCESS)
            result = L7_FAILURE;

        hapiSystem->dynamicArpInspectUntrustedPolicyId = BROAD_POLICY_INVALID;
    }

    if (BROAD_POLICY_INVALID != hapiSystem->sysId1) {
        if (hapiBroadPolicyDelete(hapiSystem->sysId1) != L7_SUCCESS)
            result = L7_FAILURE;

        hapiSystem->sysId1 = BROAD_POLICY_INVALID;
    }

    if (BROAD_POLICY_INVALID != hapiSystem->sysId2) {
        if (hapiBroadPolicyDelete(hapiSystem->sysId2) != L7_SUCCESS)
            result = L7_FAILURE;

        hapiSystem->sysId2 = BROAD_POLICY_INVALID;
    }

    if (BROAD_POLICY_INVALID != hapiSystem->arpPolicyId) {
        if (hapiBroadPolicyDelete(hapiSystem->arpPolicyId) != L7_SUCCESS)
            result = L7_FAILURE;

        hapiSystem->arpPolicyId = BROAD_POLICY_INVALID;
    }
#ifdef L7_STACKING_PACKAGE
    if (BROAD_POLICY_INVALID != hapiSystem->fpsSysId) {
        if (hapiBroadPolicyDelete(hapiSystem->fpsSysId) != L7_SUCCESS)
            result = L7_FAILURE;

        hapiSystem->fpsSysId = BROAD_POLICY_INVALID;
    }
#endif
#ifdef L7_DOT3AH_PACKAGE
    if (BROAD_POLICY_INVALID != hapiSystem->dot3ahPolicyId) {
        if (hapiBroadPolicyDelete(hapiSystem->dot3ahPolicyId) != L7_SUCCESS)
            result = L7_FAILURE;

        hapiSystem->dot3ahPolicyId = BROAD_POLICY_INVALID;
    }
#endif /* L7_DOT3AH_PACKAGE */


#ifdef L7_IPV6_PACKAGE
    /* Remove all IPV6 Package related policies */
    if (BROAD_POLICY_INVALID != hapiSystem->Ip6SysId) {
        if (hapiBroadPolicyDelete(hapiSystem->Ip6SysId) != L7_SUCCESS)
            result = L7_FAILURE;

        hapiSystem->Ip6SysId = BROAD_POLICY_INVALID;
    }
#endif /* L7_IPV6_PACKAGE */

#if defined(FEAT_METRO_CPE_V1_0)
    if (BROAD_POLICY_INVALID != hapiSystem->ipv6SnoopId) {
        if (hapiBroadPolicyDelete(hapiSystem->ipv6SnoopId) != L7_SUCCESS)
        {
            result = L7_FAILURE;
        }
      hapiSystem->ipv6SnoopId = BROAD_POLICY_INVALID;
    }
#endif

#ifdef L7_ISDP_PACKAGE
  if (BROAD_POLICY_INVALID != hapiSystem->isdpSysId)
  {
    if (L7_SUCCESS != hapiBroadPolicyDelete(hapiSystem->isdpSysId))
    {
      result = L7_FAILURE;
    }

    hapiSystem->isdpSysId= BROAD_POLICY_INVALID;
  }
#endif


  if (BROAD_POLICY_INVALID != hapiSystem->voiceDhcpSysId)
  {
    if (L7_SUCCESS != hapiBroadPolicyDelete(hapiSystem->voiceDhcpSysId))
    {
      result = L7_FAILURE;
    }

    hapiSystem->voiceDhcpSysId = BROAD_POLICY_INVALID;
  }

#ifdef L7_MCAST_PACKAGE
  hapiBroadConfigL3McastFilter(L7_FALSE);
#endif

#ifdef L7_LLPF_PACKAGE
  result = hapiBroadLlpfPolicyRemove(dapi_g);
  if(L7_SUCCESS != result)
  {
    return result;
  }
#endif

  return result;
}


/*********************************************************************
*
* @purpose Initialize any non card specific stuff
*
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadInit(L7_ulong32 cardId, void *handle)
{
  L7_RC_t             result       = L7_SUCCESS;
  DAPI_t             *dapi_g       = (DAPI_t*)handle;

  dapi_g->system->driverStart = broadDriverStart;


  /* allocate the hapiSystem pointer */
  if (hapiBroadMemInit(dapi_g) != L7_SUCCESS)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadMemInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  /* always initialize the Debug routines, This insures that the routines
   are linked into the image */
  hapiBroadDebugInit(dapi_g);

  if (hapiBroadCpuBoardInit(dapi_g) == L7_FAILURE)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadCpuBoardInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  if (hapiBroadDrvInit(cardId, dapi_g) != L7_SUCCESS)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadDrvInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  /* Policy Manager initialization - must be called before any filters
   * are installed.
   */
  if (hapiBroadPolicyInit() != L7_SUCCESS)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadPolicyInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return L7_FAILURE;
  }

  /* Either start the phy polling task or hook interrupts */
  else if (hapiBroadPhyInit(dapi_g) != L7_SUCCESS)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadPhyInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  /* create the statistics task */
  else if (hapiBroadStatisticsInit(dapi_g) != L7_SUCCESS)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadStatisticsInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  /* initialize any layer 2 system level stuff, like tables ... */
  else if (hapiBroadL2Init(dapi_g) != L7_SUCCESS)
  {

    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadL2Init'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }

  /* initialize any layer 3 system level stuff, like tables ... */
  else if (hapiBroadL3Init(dapi_g) != L7_SUCCESS)
  {

    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadL3Init'\n",
                   __FILE__, __LINE__, __FUNCTION__);
  }
#ifdef L7_MCAST_PACKAGE
  /* initialize IP Multicast package */
  else if (hapiBroadL3McastInit(dapi_g) != L7_SUCCESS)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadL3McastInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    result = L7_FAILURE;
    return result;
  }
#endif
#ifdef L7_QOS_PACKAGE
  /* initialize QOS package */
  else if (hapiBroadQosInit(dapi_g) != L7_SUCCESS)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadQosInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    result = L7_FAILURE;
    return result;
  }
#endif
#ifdef L7_METRO_FLEX_PACKAGE
#ifdef L7_DOT1AD_PACKAGE
  /* initialize DOT1AD package */
  else if (hapiBroadDot1adInit(dapi_g) != L7_SUCCESS)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadDot1adInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    result = L7_FAILURE;
    return result;
  }
#endif
#ifdef L7_DOT3AH_PACKAGE
  else if (hapiBroadDot3ahInit(dapi_g) != L7_SUCCESS)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadDot3ahInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    result = L7_FAILURE;
    return result;
  }
#endif
#endif

#ifdef L7_WIRELESS_PACKAGE
  else if (hapiBroadL2TunnelInit(dapi_g) != L7_SUCCESS)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'hapiBroadL2TunnelInit'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    result = L7_FAILURE;
  }
#endif
  else
  {
    result = L7_SUCCESS;
  }

  return result;

}

/*********************************************************************
*
* @purpose Allocate any system memory needed, BROAD_SYSTEM_t and anything else
*
* @param   DAPI_t *dapi_g - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadMemInit(DAPI_t *dapi_g)
{
  BROAD_SYSTEM_t             *hapiSystem;
  L7_ushort16                 result = L7_SUCCESS;
  L7_uchar8                   counter;


  /*
   * BROAD System Allocated
   */
  if ((dapi_g->system->hapiSystem = (void *)osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(BROAD_SYSTEM_t))) == L7_NULL)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\n%s %d: In %s call to 'osapiMalloc'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  bzero(dapi_g->system->hapiSystem, sizeof(BROAD_SYSTEM_t));
  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  for (counter=0; counter<BROAD_POLICY_STAGE_COUNT; counter++)
  {
    hapiSystem->mgmtPolicy[counter] = BROAD_POLICY_INVALID; 
  }
  hapiSystem->mgmtVlanId = 0;

  hapiSystem->sysId1 = BROAD_POLICY_INVALID;
  hapiSystem->sysId2 = BROAD_POLICY_INVALID;
  hapiSystem->arpPolicyId               = BROAD_POLICY_INVALID;
  hapiSystem->dhcpSnoopingPolicyId      = BROAD_POLICY_INVALID;
  hapiSystem->dhcpDefaultPolicyId       = BROAD_POLICY_INVALID;
  hapiSystem->dot1xViolationPolicyId    = BROAD_POLICY_INVALID;
  hapiSystem->dynamicArpInspectUntrustedPolicyId = BROAD_POLICY_INVALID;
  hapiSystem->ipsgPolicyId                       = BROAD_POLICY_INVALID;

#ifdef L7_IPV6_PACKAGE
  hapiSystem->Ip6SysId = BROAD_POLICY_INVALID;
#endif /* L7_IPV6_PACKAGE */

#ifdef L7_DOT3AH_PACKAGE
  hapiSystem->dot3ahPolicyId = BROAD_POLICY_INVALID;
#endif /* L7_DOT3AH_PACKAGE */

#ifdef L7_STACKING_PACKAGE
  hapiSystem->fpsSysId = BROAD_POLICY_INVALID;
#endif

#if defined(FEAT_METRO_CPE_V1_0)
  hapiSystem->ipv6SnoopId = BROAD_POLICY_INVALID;
#endif

#ifdef L7_DOT1AG_PACKAGE
  hapiSystem->dot1agPolicy = BROAD_POLICY_INVALID;
#endif

  hapiSystem->voiceDhcpSysId = BROAD_POLICY_INVALID;

  hapiSystem->isdpSysId = BROAD_POLICY_INVALID;

  return result;
}

/*********************************************************************
*
* @functions
*
* @purpose   Performs BCM specific initialization. This includes both ASIC
*            init and BCM software init.
*
* @param     DAPI_t   *dapi_g
*
* @returns   L7_RC_t result
*
* @notes     none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDrvInit(L7_ulong32 cardId, DAPI_t *dapi_g)
{
  L7_RC_t                      result = L7_SUCCESS;


  return result;
}

/*********************************************************************
*
* @purpose Alloc packet memory
*
* @param   L7_int32   unit
* @param   L7_int32   size
* @param   L7_uint32  flags
*
* @returns void*
*
* @notes   none
*
* @end
*
*********************************************************************/
void* hapiBroadPacketAlloc(L7_int32 unit, L7_int32 size, L7_uint32 flags)
{
  COMPILER_REFERENCE(unit);
  COMPILER_REFERENCE(flags);

  return sal_dma_alloc(size,"hapiPktBuf");
}

/*********************************************************************
*
* @purpose Free packet memory
*
* @param   L7_int32   unit
* @param   void*      ptr
*
* @returns NONE
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadPacketFree(L7_int32 unit, void* ptr)
{
  COMPILER_REFERENCE(unit);
  sal_dma_free(ptr);
}

/*********************************************************************
*
* @purpose Initializes the frame handling pointers and calls the
*          to create a cpu board
*
* @param   DAPI_t *dapi_g - system information
*
* @returns L7_RC_t result
*
* @notes   Handles setting up the board support
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCpuBoardInit(DAPI_t *dapi_g)
{

  L7_RC_t  result = L7_SUCCESS;

  hapiBroadCpuTxRxInit(dapi_g);

#ifndef L7_PRODUCT_SMARTPATH

  /* create link status queue.
  */
  hapiLinkStatusQueue = (void *)osapiMsgQueueCreate("hapiLinkStatusQueue",
                                                    L7_MAX_INTERFACE_COUNT * 2,
                                                    sizeof(portLinkStatus_t));

  if (hapiLinkStatusQueue == L7_NULL)
  {
    L7_LOG_ERROR(0);
  }

  /* Start link status task.
  */
  if (osapiTaskCreate("hapiLinkStatusTask",hapiBroadPortLinkStatusTask,0,0, L7_DEFAULT_STACK_SIZE,
                      FD_CNFGR_SIM_DEFAULT_STATS_TASK_PRI,L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    L7_LOG_ERROR(0);
  }
#endif /* L7_PRODUCT_SMARTPATH */

  return result;
}

/*********************************************************************
*
* @purpose Initializes Physical Line card
*
* @param   L7_ushort16  unitNum - the unit being initialized
* @param   L7_ushort16  slotNum - the slot being initialized
* @param   DAPI        *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPhysicalCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data, void *handle)
{
  L7_RC_t                 result       = L7_SUCCESS;
  DAPI_USP_t              usp;
  DAPI_t                 *dapi_g = (DAPI_t*)handle;
  bcmx_lplist_t           tmpLpList, portLpList;
  BROAD_PORT_t           *hapiPortPtr;
  L7_BOOL                 newBcmUnitInserted = L7_TRUE;
  L7_uint32               idx;
  L7_BOOL                 dbGroups[USL_LAST_DB_GROUP];
  L7_enetMacAddr_t        unitKey;

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  LOG_TRACE(LOG_CTX_STARTUP,"PhysicalCardInsert starting...");

  /*
   * Retrieve the Database Info Pointers
   */
  do 
  {

    if (sysapiHpcUnitIdentifierKeyGet(usp.unit, &unitKey) != L7_SUCCESS)
    {
      result = L7_FAILURE;  
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Card insertion for unit %d, slot %d failed. Wait for retry.\n",
              usp.unit, usp.slot);
      break;
    }

    LOG_TRACE(LOG_CTX_STARTUP,"unitKey=%02x:%02x:%02x:%02x:%02x:%02x",
              unitKey.addr[0],unitKey.addr[1],unitKey.addr[2],unitKey.addr[3],unitKey.addr[4],unitKey.addr[5]);
#ifdef L7_STACKING_PACKAGE

    dapiTraceStackEvent("Starting to wait for unit %d to be discovered\n",
                    usp.unit);

    if (fp_stk_mgr_wait_for_key_discovery(unitKey.addr, 
                                          BROAD_UNIT_DISCOVER_TIMEOUT, 0) != BCM_E_NONE)
    {
      dapiTraceStackEvent("\nUnit %d not discovered. Wait for retry.\n",
              usp.unit);
      result = L7_FAILURE;
      /* The card insertion is failing probably because Broadcom discovery algorithm 
      ** didn't find all the boxes.
      ** Restart Broadcom discovery and hope that all will go well this time.
      */
      fp_stk_mgr_disc_start_request(FP_STK_MGR_DISC_EVENT_APP_SOFT_REQ); 
      break;
    }
    

    dapiTraceStackEvent("Finished waiting for unit %d to be discovered\n",
                     usp.unit);
#endif

    for (idx = USL_FIRST_DB_GROUP; idx < USL_LAST_DB_GROUP; idx++)
    {
      dbGroups[idx] = L7_TRUE;    
    }

    usl_bcmx_suspend(dbGroups);
    /* sync the new unit/slot */
    if (usl_card_insert(usp.unit,usp.slot, &newBcmUnitInserted) != L7_SUCCESS)
    {
      result = L7_FAILURE;
      dapiTraceStackEvent("Card Insertion is incomplete for unit %d, slot %d, usl_card_insert failed.\n",
                     usp.unit, usp.slot);
      usl_bcmx_resume(dbGroups);

#ifdef L7_STACKING_PACKAGE
      /* The card insertion is failing probably because Broadcom discovery algorithm 
      ** didn't find all the boxes.
      ** Restart Broadcom discovery and hope that all will go well this time.
      */
     fp_stk_mgr_disc_start_request(FP_STK_MGR_DISC_EVENT_APP_SOFT_REQ); 
#endif

      break;
    }

    /* allocate all of the ports on the card */
    if (hapiBroadPortMemAlloc(usp.unit, usp.slot, dapi_g) != L7_SUCCESS)
    {
      result = L7_FAILURE;
      usl_bcmx_resume(dbGroups);
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Card insertion for unit %d, slot %d failed. Wait for retry.\n",
               usp.unit, usp.slot);
      break;
    }

    /* Attach the card at dispatch/bcmx */
    if (hapiBroadPhysicalPortMapGet(usp.unit, usp.slot, dapi_g) != L7_SUCCESS)
    {
      result = L7_FAILURE;
      usl_bcmx_resume(dbGroups);
      dapiTraceStackEvent("Card Insertion is incomplete for unit %d, slot %d."
                          "Bcmx/Dispatch attach failed\n",
                          usp.unit, usp.slot);

      break;
    }

    /* obtain a list of lport for the usl call */
    bcmx_lplist_init(&tmpLpList,0,0);

    for (usp.port = 0; usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;usp.port++)
    {
      hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);
      bcmx_lplist_add(&tmpLpList,hapiPortPtr->bcmx_lport);
    }

    /* Synchronize the new units */
    if ((result = usl_db_bcm_unit_populate(usp.unit,usp.slot,&tmpLpList)) != L7_SUCCESS)
    {
      result = L7_FAILURE;
      /* we must free the list now */
      bcmx_lplist_free(&tmpLpList);

      usl_bcmx_resume(dbGroups);
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Card insertion for unit %d, slot %d failed. Wait for retry.\n",
              usp.unit, usp.slot);

      break;
    }

    /* we must free the list now */
    bcmx_lplist_free(&tmpLpList);

    /* Create entry for the card ports in the USL port-database */
    result = L7_SUCCESS;
   if ((cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)  ||
       (cnfgrIsFeaturePresent(L7_PFC_COMPONENT_ID, 
                              L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) == L7_TRUE))
   {

     bcmx_lplist_init(&portLpList,0,0); 

     for (usp.port = 0; usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;usp.port++)
     {
       /* Skip Stacking ports*/
       if (spmFpsPortStackingModeCheck (usp.unit, usp.slot, 
                                        (usp.port + 1)) == L7_FALSE)
       {
         hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);
         bcmx_lplist_add(&portLpList,hapiPortPtr->bcmx_lport);
       }
     }

      result = usl_portdb_update(L7_TRUE, usp.unit, &portLpList);

      bcmx_lplist_free(&portLpList);

    }
 
    usl_bcmx_resume(dbGroups);

    if (result != L7_SUCCESS)
    {
      result = L7_FAILURE;
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Card insertion for unit %d, slot %d failed. Wait for retry.\n",
              usp.unit, usp.slot);

      break;
    }

    /* hook in the Function pointers to the ports function table */
    if (hapiBroadSlotCtlInit(usp.unit, usp.slot, dapi_g) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Card insertion for unit %d, slot %d failed. Wait for retry.\n",
              usp.unit, usp.slot);
      result = L7_FAILURE;
      break;
    }

    /* setup the PHYs and get the initial link state */
    if (hapiBroadPortPhyInit(usp.unit, usp.slot, dapi_g) != L7_SUCCESS)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Card insertion for unit %d, slot %d failed. Wait for retry.\n",
              usp.unit, usp.slot);
      result = L7_FAILURE;
      break;
    }

    if (newBcmUnitInserted == L7_TRUE) 
    {
      if (hapiBroadBcmxRegisterUnit(usp.unit, usp.slot, dapi_g) == L7_FAILURE)
      {
        L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
                "Driver: Card insertion for unit %d, slot %d failed. Wait for retry.\n",
                usp.unit, usp.slot);
        result = L7_FAILURE;
        break;
      }
    }

    if (hapiBroadDefaultConfigInit(usp.unit, usp.slot, dapi_g) == L7_FAILURE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Card insertion for unit %d, slot %d failed. Wait for retry.\n",
              usp.unit, usp.slot);
      result = L7_FAILURE;
      break;
    }


    /* Initialize the Routing package for all ports on this card */
    if (hapiBroadL3CardInit(usp.unit,usp.slot,dapi_g) == L7_FAILURE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Card insertion for unit %d, slot %d failed. Wait for retry.\n",
              usp.unit, usp.slot);

      result = L7_FAILURE;
      break;
    }

#ifdef L7_MCAST_PACKAGE
    /* Initialize the IP Mcast package for all ports on this card */
    if (hapiBroadL3McastCardInit(usp.unit,usp.slot,dapi_g) == L7_FAILURE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Card insertion for unit %d, slot %d failed. Wait for retry.\n",
              usp.unit, usp.slot);

      result = L7_FAILURE;
      break;
    }
#endif
#ifdef L7_QOS_PACKAGE
    /* Initialize the QOS package for all ports on this card */
    if (hapiBroadQosCardInit(usp.unit,usp.slot,dapi_g) == L7_FAILURE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Card insertion for unit %d, slot %d failed. Wait for retry.\n",
              usp.unit, usp.slot);

      result = L7_FAILURE;
      break;
    }
#endif

  } while (0);
  
  if (result != L7_SUCCESS)
  {
    LOG_TRACE(LOG_CTX_STARTUP,"Error inserting card!");
    usl_card_remove(usp.unit, usp.slot);     

    L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_DRIVER_COMPONENT_ID,
            "Driver: Card insertion for unit %d, slot %d failed. Wait for retry.\n",
            usp.unit, usp.slot);
  }

  LOG_NOTICE(LOG_CTX_STARTUP,"Complete: result=%d",result);

  return result;
}

/*********************************************************************
*
* @purpose This function maps the slots to the physical ports by reading
*          the card database.  By filling in these parameters the system
*          can be mapped out.
*
* @param   SYSAPI_CARD_ENTRY_t *cardInfoPtr - card database entry for this slot
* @param   L7_ushort16  unitNum     - the unit being initialized
* @param   L7_ushort16  slotNum     - the slot being initialized
* @param   DAPI_t      *dapi_g      - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPortMemAlloc(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g)
{
  L7_RC_t           result       = L7_SUCCESS;
  BROAD_PORT_t           *hapiPortPtr;
  DAPI_PORT_t            *dapiPortPtr;
  DAPI_USP_t              usp;
  L7_uint32 i;

  usp.unit = unitNum;
  usp.slot = slotNum;
  usp.port = 0;

  /***********************************************************************
   * traverse the array of switch ports and create a port for each port on
   *  this unit/slot
   ***********************************************************************/
  for (usp.port=0; usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot; usp.port++)
  {
    if (IS_PORT_TYPE_PHYSICAL(DAPI_PORT_GET(&usp, dapi_g)) == L7_TRUE)
    {
      /* allocate the BROAD_PORT_t */
      if ((dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]->hapiPort =
           (void *)osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(BROAD_PORT_t))) == L7_NULL)
      {
        result = L7_FAILURE;
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'osapiMalloc'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
      }

      hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);

      /* initialize the data to zero */
      bzero((L7_char8*)hapiPortPtr,sizeof(BROAD_PORT_t));

      hapiPortPtr->dot1pPolicy = BROAD_POLICY_INVALID;

      hapiPortPtr->dhcpSnoopingEnabled         = L7_TRUE; //L7_FALSE; /* PTin modified: DHCP */
      hapiPortPtr->dynamicArpInspectionTrusted = L7_TRUE;
      hapiPortPtr->ipsgEnabled                 = L7_FALSE;

      hapiPortPtr->dot1x.dot1xStatus          = L7_DOT1X_PORT_STATUS_AUTHORIZED;
      for (i = 0; i < L7_DOT1X_PORT_MAX_MAC_USERS; i++)
      {
        hapiPortPtr->dot1x.client[i].vlanAssignmentPolicyId = BROAD_POLICY_INVALID;
        hapiPortPtr->dot1x.client[i].policyId               = BROAD_POLICY_INVALID;
      }

      /* initialize voice vlan port values */
      hapiPortPtr->voiceVlanPort.deviceCount =0;
      hapiPortPtr->voiceVlanPort.voiceVlanPolicy = BROAD_POLICY_INVALID;
      hapiPortPtr->voiceVlanPort.voiceVlanUnauthMode = L7_DISABLE;

      /* Set up the initial dot1p to COS map to undefined so we
       * can tell which values have been specified.
       */
      for (i = 0; i < L7_DOT1P_MAX_PRIORITY+1; i++)
        hapiPortPtr->dot1pMap[i] = L7_DOT1P_MAX_PRIORITY+1;

      /* PTin added: + default prio */
      hapiPortPtr->dot1pMap[(L7_DOT1P_MAX_PRIORITY+1)] = 0;

      /* If this port is used for front panel stacking then set a flag.
      */
      if (spmFpsPortStackingModeCheck (usp.unit, usp.slot, usp.port + 1) == L7_TRUE)
      {
        hapiPortPtr->front_panel_stack = L7_TRUE;
      }

      hapiPortPtr->hapiModeparm.physical.phySemaphore = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

      if (hapiPortPtr->hapiModeparm.physical.phySemaphore == L7_NULL)
      {
        L7_LOG_ERROR(usp.port);
      }

      /* Initialize speed and duplex */
      dapiPortPtr = DAPI_PORT_GET(&usp, dapi_g);
      /* PTin added: Speed 100G */
      if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_FULL_100G)
      {
        hapiPortPtr->speed  = DAPI_PORT_SPEED_GE_100GBPS;
        hapiPortPtr->duplex = DAPI_PORT_DUPLEX_FULL;
      }
      /* PTin added: Speed 40G */
      else if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_FULL_40G)
      {
        hapiPortPtr->speed  = DAPI_PORT_SPEED_GE_40GBPS;
        hapiPortPtr->duplex = DAPI_PORT_DUPLEX_FULL;
      }
      /* PTin end */
      else if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_FULL_10G)
      {
        hapiPortPtr->speed  = DAPI_PORT_SPEED_GE_10GBPS;
        hapiPortPtr->duplex = DAPI_PORT_DUPLEX_FULL;
      }
      /* PTin added: Speed 2.5G */
      else if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_FULL_2500)
      {
        hapiPortPtr->speed  = DAPI_PORT_SPEED_GE_2G5BPS;
        hapiPortPtr->duplex = DAPI_PORT_DUPLEX_FULL;
      }
      /* PTin end */
      else if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_FULL_1000)
      {
        hapiPortPtr->speed  = DAPI_PORT_SPEED_GE_1GBPS;
        hapiPortPtr->duplex = DAPI_PORT_DUPLEX_FULL;
      }
      else if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_HALF_1000)
      {
        hapiPortPtr->speed  = DAPI_PORT_SPEED_GE_1GBPS;
        hapiPortPtr->duplex = DAPI_PORT_DUPLEX_HALF;
      }
      else if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_FULL_100)
      {
        hapiPortPtr->speed  = DAPI_PORT_SPEED_FE_100MBPS;
        hapiPortPtr->duplex = DAPI_PORT_DUPLEX_FULL;
      }
      else if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_HALF_100)
      {
        hapiPortPtr->speed  = DAPI_PORT_SPEED_FE_100MBPS;
        hapiPortPtr->duplex = DAPI_PORT_DUPLEX_HALF;
      }
      else if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_FULL_10)
      {
        hapiPortPtr->speed  = DAPI_PORT_SPEED_FE_10MBPS;
        hapiPortPtr->duplex = DAPI_PORT_DUPLEX_FULL;
      }
      else if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_HALF_10)
      {
        hapiPortPtr->speed  = DAPI_PORT_SPEED_FE_10MBPS;
        hapiPortPtr->duplex = DAPI_PORT_DUPLEX_HALF;
      }
    }
    else if (IS_PORT_TYPE_CPU(DAPI_PORT_GET(&usp, dapi_g)) == L7_TRUE)
    {
      /* allocate the BROAD_PORT_t */
      if ((dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]->hapiPort =
           (void *)osapiMalloc(L7_DRIVER_COMPONENT_ID, sizeof(BROAD_PORT_t))) == L7_NULL)
      {
        result = L7_FAILURE;
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'osapiMalloc'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        return result;
      }

      hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);

      /* initialize the data to zero */
      bzero((L7_char8*)hapiPortPtr,sizeof(BROAD_PORT_t));
    }
    else
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "%s %d: In %s unknown port mode\n",
                     __FILE__, __LINE__, __FUNCTION__);
      return result;
    }
  }

  return result;

}



/*********************************************************************
*
* @purpose This function maps the slots to the physical ports by reading
*          the card database.  By filling in these parameters the system
*          can be mapped out. Highly dependent on the Card Database for the
*          hardware.
*
* @param   cardInfoPtr - card database entry for this slot
* @param   unitNum     - the unit being initialized
* @param   slotNum     - the slot being initialized
* @param   dapi_g      - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPhysicalPortMapGet(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g)
{
  L7_RC_t                       result = L7_SUCCESS;
  L7_ushort16                   slotMapIndex;
  L7_ushort16                   portMapIndex;
  BROAD_PORT_t                 *hapiPortPtr;
  DAPI_USP_t                    usp;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardInfoPtr;
  L7_uint32                     portCount;
  cpudb_key_t                   cpuKey;
  bcmx_uport_t                  uport;
  bcm_port_t                    mod_port;
#ifdef L7_STACKING_PACKAGE
  int                           rv;
#endif

  usp.unit = unitNum;
  usp.slot = slotNum;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(dapi_g->unit[usp.unit]->slot[usp.slot]->cardId);
  if (sysapiHpcCardInfoPtr == L7_NULLPTR)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'sysapiHpcCardDbEntryGet'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  if (sysapiHpcCardInfoPtr->type != SYSAPI_CARD_TYPE_LINE)
  {
    /* This function only support physical line cards */
    result = L7_FAILURE;
    return result;
  }

  dapiCardInfoPtr = sysapiHpcCardInfoPtr->dapiCardInfo;

  /* Obtain the key from hpc. Logical units always use the key of the local unit.
  */
  if (usp.unit == L7_LOGICAL_UNIT)
  {
    result = sysapiHpcLocalUnitIdentifierMacGet((L7_enetMacAddr_t *)&cpuKey.key);
  }
  else
  {
    result = sysapiHpcUnitIdentifierKeyGet(usp.unit, (L7_enetMacAddr_t *)&cpuKey.key);
  }

  if (result != L7_SUCCESS)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'sysapiHpcUnitIdentifierKeyGet'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return L7_FAILURE;
  }

  /* First call the hapiBroadMapDbEntryGet with known unmatched BCM unit. This forces
  ** BCMX attachment of all known units.
  */
  {
    int ignore_unit;
    bcmx_lport_t ignore_port;

    hapiBroadMapDbEntryGet (&cpuKey, 0xffff, 0xffff, &ignore_unit, &ignore_port);
  }

  /*
   * scan slot database by port for needed data; switch core, switch core module
   * port index as seen from module, usp
   */
  for (slotMapIndex=0, portCount = 0;
       (slotMapIndex < dapiCardInfoPtr->numOfSlotMapEntries) &&
       (portCount < sysapiHpcCardInfoPtr->numOfNiPorts); slotMapIndex++)
  {
    if (dapiCardInfoPtr->slotMap[slotMapIndex].slotNum == usp.slot)
    {
      usp.port = dapiCardInfoPtr->slotMap[slotMapIndex].portNum;
      hapiPortPtr  = (BROAD_PORT_t *)dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]->hapiPort;
      hapiPortPtr->bcm_port =  dapiCardInfoPtr->slotMap[slotMapIndex].bcm_port;

      if (hapiBroadMapDbEntryGet(&cpuKey,
                                 dapiCardInfoPtr->slotMap[slotMapIndex].bcm_cpuunit,
                                 dapiCardInfoPtr->slotMap[slotMapIndex].bcm_port,
                                 &hapiPortPtr->bcm_unit,
                                 &hapiPortPtr->bcmx_lport) != L7_SUCCESS)
      {
#ifdef L7_STACKING_PACKAGE
        /* We may have failed because the remote units are not attached yet.
        ** Attach them now and try again.
        */
        sysapiPrintf("\nTrying to attach more units.....\n");
        /*if (bcm_st_cur_db != 0)*/
        {
          {
             rv = fp_stk_mgr_key_attach(cpuKey.key);
             if (rv != BCM_E_NONE)
             {
               dapiTraceStackEvent("Cpu Key %x:%x:%x:%x:%x:%x attach failed\n",
                                cpuKey.key[0], cpuKey.key[1], cpuKey.key[2],
                                cpuKey.key[3], cpuKey.key[4], cpuKey.key[5]); 
               result = L7_FAILURE;
               return result;
             }
             else
             {
                 dapiTraceStackEvent("Cpu Key %x:%x:%x:%x:%x:%x attach success\n",
                                  cpuKey.key[0], cpuKey.key[1], cpuKey.key[2],
                                  cpuKey.key[3], cpuKey.key[4], cpuKey.key[5]); 

             }

          }
        }
#endif

        if (hapiBroadMapDbEntryGet(&cpuKey,
                                   dapiCardInfoPtr->slotMap[slotMapIndex].bcm_cpuunit,
                                   dapiCardInfoPtr->slotMap[slotMapIndex].bcm_port,
                                   &hapiPortPtr->bcm_unit,
                                   &hapiPortPtr->bcmx_lport) != L7_SUCCESS)
        {
          printf("Physical card insert failed due to missing BCMX ports. Unit = %d, Slot = %d.\n",
                 usp.unit,
                 usp.slot);
          printf("Use 'devshell hapiBroadDebugBcmxMapDump()' to see existing ports.\n\n");
          result = L7_FAILURE;
          return result;
        }
      }

      /* Configure the uport in bcmx */
      HAPI_BROAD_USP_TO_UPORT(&usp,uport);
      bcmx_uport_set(hapiPortPtr->bcmx_lport, uport);

      hapiPortPtr->bcm_modid = BCM_GPORT_MODPORT_MODID_GET (hapiPortPtr->bcmx_lport);
      mod_port               = BCM_GPORT_MODPORT_PORT_GET(hapiPortPtr->bcmx_lport);

      if ((hapiPortPtr->bcm_modid == HAPI_BROAD_INVALID_MODID) ||
          (mod_port == HAPI_BROAD_INVALID_MODPORT))
      {
        L7_LOG_ERROR(0);
      }

      hapiBroadModidModportToLportSet (hapiPortPtr->bcm_modid,
                                       mod_port,
                                       hapiPortPtr->bcmx_lport);
    }
  }

  /* scan card database for needed data, phy address and media */
  for (portMapIndex=0; portMapIndex < dapiCardInfoPtr->numOfPortMapEntries; portMapIndex++)
  {
    usp.port = dapiCardInfoPtr->portMap[portMapIndex].portNum;
    hapiPortPtr  = HAPI_PORT_GET(&usp, dapi_g);
  }

  return result;
}

/*********************************************************************
*
* @purpose Initializes Power Supply card
*
* @param  unitNum         unit number for this logical card
* @param  slotNum         slot number for this logical card
* @param *dapi_g          system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPowerSupplyCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd,
                                       void *data, DAPI_t *dapi_g)
{
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Uninitializes Power Supply card
*
* @param  unitNum         unit number for this logical card
* @param  slotNum         slot number for this logical card
* @param *dapi_g          system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPowerSupplyCardRemove(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd,
                                       void *data, void *handle)
{
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Initializes Fan card
*
* @param  unitNum         unit number for this logical card
* @param  slotNum         slot number for this logical card
* @param *dapi_g          system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadFanCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd,
                               void *data, DAPI_t *dapi_g)
{
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Uninitializes Fan card
*
* @param  unitNum         unit number for this logical card
* @param  slotNum         slot number for this logical card
* @param *dapi_g          system information
*
* @returns L7_RC_t result
*
* @notes none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadFanCardRemove(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd,
                               void *data, void *handle)
{
  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose This function maps the slots to the cpu ports by reading
*          the card database.  By filling in these parameters the system
*          can be mapped out. Highly dependent on the Card Database for the
*          hardware.
*
* @param   cardInfoPtr - card database entry for this slot
* @param   unitNum     - the unit being initialized
* @param   slotNum     - the slot being initialized
* @param   dapi_g      - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCpuPortMapGet(L7_ushort16 unitNum, L7_ushort16 slotNum, DAPI_t *dapi_g)
{
  L7_RC_t                       result = L7_SUCCESS;
  L7_ushort16                   slotMapIndex;
  L7_ushort16                   portMapIndex;
  BROAD_PORT_t                 *hapiPortPtr;
  DAPI_USP_t                    usp;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  DAPI_CARD_ENTRY_t            *dapiCardInfoPtr;
  cpudb_key_t                   cpuKey;
  bcmx_uport_t                  uport;

  usp.unit = unitNum;
  usp.slot = slotNum;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(dapi_g->unit[usp.unit]->slot[usp.slot]->cardId);
  if (sysapiHpcCardInfoPtr == L7_NULLPTR)
  {
    result = L7_FAILURE;
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'sysapiHpcCardDbEntryGet'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return result;
  }

  if (sysapiHpcCardInfoPtr->type != SYSAPI_CARD_TYPE_LOGICAL_CPU)
  {
    /* This function only support logical cpu cards */
    result = L7_FAILURE;
    return result;
  }

  dapiCardInfoPtr = sysapiHpcCardInfoPtr->dapiCardInfo;

  /* Obtain the key from hpc */
  /* Add one to HPC because it is one base and the andl driver is zero based */
  if ((result = sysapiHpcLocalUnitIdentifierMacGet((L7_enetMacAddr_t *)&cpuKey.key)) != L7_SUCCESS)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'sysapiHpcUnitIdentifierKeyGet'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return L7_FAILURE;
  }


#ifdef L7_STACKING_PACKAGE
  /* We need to call this function at least one time in order to attach the
  ** local CPU.
  */
  /* Check whether there are any active stacking ports */
  {
    int rv = BCM_E_NONE;
    /* bcm_stack_attach_fn (bcm_st_cur_db, 1, 0); */
    /* Attempt to attach the local board */
    rv = fp_stk_mgr_key_attach( cpuKey.key );
    dapiTraceStackEvent("Mgr cpu key attach rv %d\n",rv);
  }
#endif

  /*
   * scan slot database by port for needed data; switch core, switch core module
   * port index as seen from module, usp
   */
  for (slotMapIndex=0, usp.port=0; slotMapIndex < dapiCardInfoPtr->numOfSlotMapEntries; slotMapIndex++, usp.port++)
  {
    hapiPortPtr  = (BROAD_PORT_t *)dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]->hapiPort;

#ifdef L7_ROBO_SUPPORT
    hapiPortPtr->bcm_port =  dapiCardInfoPtr->slotMap[slotMapIndex].bcm_port;
    if ((hapiBroadRoboCheck() == L7_TRUE))
    {
      int i;
      for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
      {
        bcm_port_enable_set(i,hapiPortPtr->bcm_port,1);
      }
    }
#endif

    if (hapiBroadMapDbEntryGet(&cpuKey,
                               dapiCardInfoPtr->slotMap[slotMapIndex].bcm_cpuunit,
                               dapiCardInfoPtr->slotMap[slotMapIndex].bcm_port,
                               &hapiPortPtr->bcm_unit,
                               &hapiPortPtr->bcmx_lport) != L7_SUCCESS)
    {
      hapiBroadDebugBcmxMapDump();
      result = L7_FAILURE;
      return result;
    }

    /* Configure the uport in bcmx */
    HAPI_BROAD_USP_TO_UPORT(&usp,uport);
    bcmx_uport_set(hapiPortPtr->bcmx_lport, uport);
  }

  /* scan card database for needed data, phy address and media */
  for (portMapIndex=0; portMapIndex < dapiCardInfoPtr->numOfPortMapEntries; portMapIndex++)
  {
    usp.port = dapiCardInfoPtr->portMap[portMapIndex].portNum;
    hapiPortPtr  = HAPI_PORT_GET(&usp, dapi_g);
  }

  return result;
}
#ifdef BCM_ROBO_SUPPORT
extern int
drv_bcm5395_queue_rx_reason_set(int unit, uint8 reason, uint32 queue);
#endif
/*********************************************************************
*
* @purpose On a new bcm unit insert, refresh the l2/linkscan/rx registrations.
*
* @param   L7_ushort16  unitNum - the unit being initialized
* @param   L7_ushort16  slotNum - the slot being initialized
* @param   DAPI_t      *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadBcmxRegisterUnit(L7_ushort16 unitNum,L7_ushort16 slotNum, DAPI_t *dapi_g)
{
  L7_RC_t               result      = L7_SUCCESS;
  int                   rv;
  int                   bcm_unit, i;

#ifdef L7_STACKING_PACKAGE
  /* clear the remote event reporting infrastructure 
  */
  bcm_rlink_client_clear();
#endif

  /* refresh any linkscan registrations in the system to assure remote events are sent here */
  rv = bcmx_linkscan_enable_set(-1);
  BCMX_UNIT_ITER(bcm_unit, i)
  {
    rv = bcmx_linkscan_device_add(bcm_unit);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Failed to attach card %s/%d at linkscan, error code %d\n", 
              unitNum, slotNum, rv);
      result = L7_FAILURE;
      return result;
    }

    rv = bcmx_rx_device_add (bcm_unit);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Failed to attach card %s/%d in rx subsystem, error code %d\n", 
              unitNum, slotNum, rv);
      result = L7_FAILURE;
      return result;
    }
  }

  /* Register for L2 notifications on standalone package */
#ifndef L7_STACKING_PACKAGE
  {
    rv = bcmx_l2_notify_register((bcmx_l2_notify_f) hapiBroadAddrMacUpdate,dapi_g);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Failed to register for L2 notifications, error code %d\n", 
              rv);
      result = L7_FAILURE;
      return result;
    }

    if ( hapiBroadRoboCheck() == L7_TRUE)
    {
      bcmx_l2_notify_stop();
    }

    rv = bcmx_l2_notify_start();
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
              "Driver: Failed to start L2 notifications, error code %d\n", 
              rv);
      result = L7_FAILURE;
      return result;
    }
  }
#endif

  rv = bcmx_linkscan_register(hapiBroadPortLinkStatusChange);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
            "Driver: Failed to register with linscan, error code %d\n", 
            rv);
    result = L7_FAILURE;
    return result;
  }

#ifdef BCM_ROBO_SUPPORT
/*need to change with SOC_IS_53115*/
 drv_bcm5395_queue_rx_reason_set(0,DRV_RX_REASON_SWITCHING,3);
 drv_bcm5395_queue_rx_reason_set(0,DRV_RX_REASON_PROTO_TERM,3);
 drv_bcm5395_queue_rx_reason_set(0,DRV_RX_REASON_PROTO_SNOOP,3);
#endif

  rv = bcmx_rx_register("hapiBroadReceive",
                        hapiBroadReceive,
                        10,
                        dapi_g,
                        BCM_RCO_F_ALL_COS);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
            "Driver: Failed to register callback with rx, error code %d\n", 
            rv);
    result = L7_FAILURE;
    return result;
  }

  rv = bcmx_rx_start();
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_DRIVER_COMPONENT_ID,
            "Driver: Failed to start rx subsystem, error code %d\n", 
            rv);
    result = L7_FAILURE;
    return result;
  }

  return result;
}
/*********************************************************************
*
* @purpose help
*
* @param   L7_ushort16  unitNum - the unit being initialized
* @param   L7_ushort16  slotNum - the slot being initialized
* @param   DAPI_t      *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDefaultConfigInit(L7_ushort16 unitNum,L7_ushort16 slotNum, DAPI_t *dapi_g)
{
  L7_RC_t               result      = L7_SUCCESS;
  DAPI_USP_t            usp;
  BROAD_PORT_t         *hapiPortPtr;
  DAPI_PORT_t          *dapiPortPtr;

  usp.unit    = unitNum;
  usp.slot    = slotNum;

  for (usp.port=0;usp.port<dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;usp.port++)
  {
    dapiPortPtr = DAPI_PORT_GET(&usp, dapi_g);

    if (IS_PORT_TYPE_PHYSICAL(dapiPortPtr) == L7_TRUE)
    {
      dapiPortPtr->statsEnable = L7_TRUE;

      hapiPortPtr = HAPI_PORT_GET(&usp, dapi_g);

      /* Don't touch front panel stacking ports.
      */
      if (hapiPortPtr->front_panel_stack == L7_TRUE)
      {
        continue;
      }

      hapiPortPtr->l2FlushSyncSem = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);
      if (hapiPortPtr->l2FlushSyncSem == L7_NULL)
      {
        L7_LOG_ERROR(usp.port);
      }
    }
  }

  return result;
}




void hapiBroadUspGet(L7_uint32 cpuId, L7_int32 unit, L7_int32 port, DAPI_t *dapi_g, DAPI_USP_t *usp)
{
  BROAD_SYSTEM_t             *hapiSystem;

  /* return the Physical USP associated w/ the broadcom port */
  hapiSystem = (BROAD_SYSTEM_t*)dapi_g->system->hapiSystem;

  usp->unit = hapiSystem->uspMap[cpuId][unit][port].unit;
  usp->slot = hapiSystem->uspMap[cpuId][unit][port].slot;
  usp->port = hapiSystem->uspMap[cpuId][unit][port].port;
}

void hapiBroadLogicalUspGet(L7_int32 unit, L7_int32 port, DAPI_t *dapi_g, DAPI_USP_t *usp)
{
  BROAD_SYSTEM_t             *hapiSystem;
  BROAD_PORT_t               *hapiPortPtr;
  L7_uint32                   cpuId = 0;

  /* return the logical USP associated w/ the broadcom port */
  hapiSystem = (BROAD_SYSTEM_t*)dapi_g->system->hapiSystem;

  usp->unit = hapiSystem->uspMap[cpuId][unit][port].unit;
  usp->slot = hapiSystem->uspMap[cpuId][unit][port].slot;
  usp->port = hapiSystem->uspMap[cpuId][unit][port].port;

  hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
  if (hapiPortPtr->hapiModeparm.physical.isMemberOfLag == L7_TRUE)
  {
    *usp = hapiPortPtr->hapiModeparm.physical.lagUsp;
  }
}


/*********************************************************************
*
* @purpose callback function to get the logical port on src unit that
*          is used to reach the destination port
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
bcmx_lport_t
hapiBroadDestRouteGet(bcmx_lport_t src, bcmx_lport_t dest)
{
  /* Currently under non-stacking dest is the port */
  /* FIX ME - when stacking is enabled */

  return(dest);


}

/* Meter for inband traffic */
BROAD_METER_ENTRY_t inband_meterInfo = {RATE_LIMIT_INBAND, 128, RATE_LIMIT_INBAND, 128, BROAD_METER_COLOR_BLIND};

/**
 * Reconfigure inband rule with new meter
 * 
 * @param cir 
 * @param pir 
 * @param cbs 
 * @param pbs 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadReconfigInbandMeter(L7_uint16 cir, L7_uint16 cbs)
{
  BROAD_SYSTEM_t *hapiSystemPtr;
  extern DAPI_t *dapi_g;

  if (dapi_g == L7_NULLPTR || dapi_g->system == L7_NULLPTR || dapi_g->system->hapiSystem == L7_NULLPTR)
  {
    LOG_ERR(LOG_CTX_PTIN_HAPI, "dapi_g pointer is NULL");
    return L7_FAILURE;
  }

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  /* Update meter data */
  if (cbs == 0)  cbs = 128;

  LOG_INFO(LOG_CTX_PTIN_HAPI, "Going to apply CIR=%u CBS=%u", cir, cbs);

  inband_meterInfo.cir = cir;
  inband_meterInfo.pir = cir;
  inband_meterInfo.cbs = cbs;
  inband_meterInfo.pbs = cbs;

  if (hapiSystemPtr->mgmtVlanId == 0 ||
      hapiSystemPtr->mgmtPolicy[BROAD_POLICY_STAGE_INGRESS] == 0 ||
      hapiSystemPtr->mgmtPolicy[BROAD_POLICY_STAGE_INGRESS] == BROAD_POLICY_INVALID)
  {
    LOG_WARNING(LOG_CTX_PTIN_HAPI, "No inband rule is configured, but meter info was updated successfully");
    return L7_SUCCESS;
  }

  /* Reconfigure inband rule */
  hapiBroadFfpSysMacInstall(dapi_g, hapiSystemPtr->mgmtVlanId, hapiSystemPtr->bridgeMacAddr.addr);
  
  return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Install the MAC filter for the system MAC address.
*
* @param   dapi_g   - DAPI driver.
* @param   new_vlan_id - VLAN ID for the new MAC address.
*                        If set to zero then new filter is not installed.
*
* @param   new_mac_addr - MAC Address for this filter.
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void hapiBroadFfpSysMacInstall (DAPI_t      *dapi_g,
                                L7_ushort16  new_vlan_id,
                                L7_uchar8   *new_mac_addr)
{
  BROAD_SYSTEM_t *hapiSystemPtr;
  /* PTin modified: inband -> Only consider bytes identifying PTIn related packets */
  L7_uchar8       exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                   FIELD_MASK_ALL,  FIELD_MASK_ALL,  FIELD_MASK_ALL };
  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  LOG_INFO(LOG_CTX_MISC,"Going to configure Inband Trap rule...");

  /* Process the Ingress Stage */

  /* If we already have an old MAC address for the network interface
  ** then remove it.
  */
  if (hapiSystemPtr->mgmtPolicy[BROAD_POLICY_STAGE_INGRESS] != BROAD_POLICY_INVALID)
  {
    (void)hapiBroadPolicyDelete(hapiSystemPtr->mgmtPolicy[BROAD_POLICY_STAGE_INGRESS]);
    hapiSystemPtr->mgmtPolicy[BROAD_POLICY_STAGE_INGRESS] = BROAD_POLICY_INVALID;
  }

  /* Create new system mac filter, if specified. */
  if ((0 != new_vlan_id) && (L7_NULLPTR != new_mac_addr))
  {
    BROAD_POLICY_t      mgmtId;
    BROAD_POLICY_RULE_t ruleId;

    hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
    hapiBroadPolicyRuleAdd(&ruleId);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, new_mac_addr, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID,  (L7_uchar8*)&new_vlan_id, exact_match);
#ifdef BCM_ROBO_SUPPORT
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_REASON_CODE, 
                                      BCM_ROBO_RX_REASON_SWITCHING, 0, 0);
#else
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, CPU_TRAPPED_PACKETS_COS_INBAND, 0, 0);
#endif

    /* PTin added: inband */
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
    hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
    hapiBroadPolicyRuleMeterAdd(ruleId, &inband_meterInfo);
    /* PTin end */

#ifdef L7_STACKING_PACKAGE
    hapiBroadPolicyEnableFPS(); /* Enable on FPS ports too, if applicable */
#endif

    if (hapiBroadPolicyCommit(&mgmtId) == L7_SUCCESS)
      hapiSystemPtr->mgmtPolicy[BROAD_POLICY_STAGE_INGRESS] = mgmtId;
  }

#if 0
  /* Process the Egress Stage */

  /* If we already have an old MAC address for the network interface
  ** then remove it.
  */
  if (hapiSystemPtr->mgmtPolicy[BROAD_POLICY_STAGE_EGRESS] != BROAD_POLICY_INVALID)
  {
    (void)hapiBroadPolicyDelete(hapiSystemPtr->mgmtPolicy[BROAD_POLICY_STAGE_EGRESS]);
    hapiSystemPtr->mgmtPolicy[BROAD_POLICY_STAGE_EGRESS] = BROAD_POLICY_INVALID;
  }

  /* Create new system mac filter, if specified. */
  if ((0 != new_vlan_id) && (L7_NULLPTR != new_mac_addr))
  {
    BROAD_POLICY_t      mgmtId;
    BROAD_POLICY_RULE_t ruleId;
    bcmx_lport_t  lport;
    bcm_port_t    bcm_port;
    bcm_port_t    bcm_port_mask = (bcm_port_t) -1;

    /* CPU port */
    if (bcmx_lport_local_cpu_get(0, &lport) != BCM_E_NONE)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with bcmx_lport_local_cpu_get");
      return;
    }
    bcm_port = bcmx_lport_bcm_port(lport);
    if (bcm_port < 0)
    {
      LOG_ERR(LOG_CTX_PTIN_HAPI,"Error with bcmx_lport_bcm_port");
      return;
    }

    /* Create policy */
    if (hapiBroadPolicyCreate(BROAD_POLICY_TYPE_PTIN) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_STARTUP, "Cannot create trap policy\r\n");
      return;
    }
    /* Egress stage */
    if (hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_EGRESS) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_STARTUP, "Error creating a egress policy\r\n");
      hapiBroadPolicyCreateCancel();
      return;
    }
    /* Create rule */    
    if (hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_HIGH) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_STARTUP, "Error adding rule\r\n");
      hapiBroadPolicyCreateCancel();
      return;
    }
    /* CPU port */
    if (hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OUTPORT, (L7_uchar8 *)&bcm_port, (L7_uchar8 *)&bcm_port_mask) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_STARTUP, "Error adding port qualifier (bcm_port=%d)\r\n",bcm_port);
      hapiBroadPolicyCreateCancel();
      return;
    }
    /* Inband type packets */
    /* Odd behavior when added: default rules are not applied! */
#if 0
    if (hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, new_mac_addr, exact_match) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_STARTUP, "Error adding BROAD_FIELD_MACDA qualifier");
      hapiBroadPolicyCreateCancel();
      return;
    }
    if (hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID,  (L7_uchar8*)&new_vlan_id, exact_match) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_STARTUP, "Error adding BROAD_FIELD_OVID qualifier");
      hapiBroadPolicyCreateCancel();
      return;
    }
#endif
    /* Drop red packets */    
    if (hapiBroadPolicyRuleNonConfActionAdd(ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_STARTUP, "Error adding hard_drop action\r\n");
      hapiBroadPolicyCreateCancel();
      return;
    }
    /* Define meter action, to rate limit packets */   
    if (hapiBroadPolicyRuleMeterAdd(ruleId, &inband_meterInfo) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_STARTUP, "Error adding rate limit\r\n");
      hapiBroadPolicyCreateCancel();
      return;
    }
#if 0
    /* Add counter */
    if (hapiBroadPolicyRuleCounterAdd(ruleId, BROAD_COUNT_PACKETS) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_STARTUP,"Error with hapiBroadPolicyRuleCounterAdd");
      hapiBroadPolicyCreateCancel();
      return;
    }
#endif
    /* Commit rule */
    if ((hapiBroadPolicyCommit(&mgmtId)) != L7_SUCCESS)
    {
      LOG_ERR(LOG_CTX_STARTUP, "Error commiting trap policy\r\n");
      hapiBroadPolicyCreateCancel();
      return;
    }
    LOG_TRACE(LOG_CTX_STARTUP, "Trap policy commited successfully (policyId=%u)\r\n",mgmtId);

    /* Store policyId */
    hapiSystemPtr->mgmtPolicy[BROAD_POLICY_STAGE_EGRESS] = mgmtId;
  }
#endif

  LOG_INFO(LOG_CTX_MISC,"Inband Trap rule configured for VLAN %u", new_vlan_id);
}

#ifdef L7_DOT1AG_PACKAGE
/*********************************************************************
*
* @purpose Install DOT1AG filter to trap cfm packets with system MAC address.
*
* @param   dapi_g   - DAPI driver.
*
* @param   new_mac_addr - MAC Address for this filter.
*
* @returns void
*
* @notes
*
* @end
*
*********************************************************************/
void hapiBroadDot1agPolicyInstall (DAPI_t      *dapi_g,
                                   L7_uchar8   *new_mac_addr)
{
  BROAD_SYSTEM_t *hapiSystemPtr;
  L7_ushort16    cfm_ethtype  = L7_ETYPE_CFM;
  L7_uchar8      exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                   FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};


  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  /* If we already have an old MAC address for the network interface
  ** then remove it.
  */
  if (hapiSystemPtr->dot1agPolicy != BROAD_POLICY_INVALID)
  {
    (void)hapiBroadPolicyDelete(hapiSystemPtr->dot1agPolicy);
    hapiSystemPtr->dot1agPolicy = BROAD_POLICY_INVALID;
  }

  /* Create new system mac filter, if specified. */
  if (L7_NULLPTR != new_mac_addr)
  {
    BROAD_POLICY_t      mgmtId;
    BROAD_POLICY_RULE_t ruleId;

    hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM);
    hapiBroadPolicyRuleAdd(&ruleId);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&cfm_ethtype, exact_match);
    hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_MACDA, new_mac_addr, exact_match);
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
    hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);

    if (hapiBroadPolicyCommit(&mgmtId) == L7_SUCCESS)
      hapiSystemPtr->dot1agPolicy = mgmtId;
  }
}
#endif /* L7_DOT1AG_PACKAGE */

/*********************************************************************
*
* @purpose Initializes Logical CPU card
*
* @param   L7_ushort16  unitNum - the unit being initialized
* @param   L7_ushort16  slotNum - the slot being initialized
* @param   DAPI        *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCpuCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data, void *handle)
{
  L7_RC_t                       result = L7_SUCCESS;
  DAPI_USP_t                    usp;
  DAPI_t                       *dapi_g = (DAPI_t*)handle;
  SYSAPI_HPC_CARD_DESCRIPTOR_t *sysapiHpcCardInfoPtr;
  L7_uint32                     mgmtUnit;
  L7_enetMacAddr_t              mgrKey;

  LOG_TRACE(LOG_CTX_STARTUP,"CpuCardInsert starting (slot=%d)...", dapiUsp->slot);

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;
  /* There is only one CPU port on the system and it is always 0 on the cpu slot */
  usp.port = 0;

  sysapiHpcCardInfoPtr = sysapiHpcCardDbEntryGet(dapi_g->unit[usp.unit]->slot[usp.slot]->cardId);
  if (sysapiHpcCardInfoPtr == L7_NULLPTR)
  {
    result = L7_FAILURE;
    return result;
  }

  if (hapiBroadPortMemAlloc(usp.unit, usp.slot, dapi_g) == L7_FAILURE)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadPortMemAlloc'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return L7_FAILURE;
  }

  if (unitMgrNumberGet(&mgmtUnit) != L7_SUCCESS)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'unitMgrNumberGet'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return L7_FAILURE;
      
  }

  LOG_TRACE(LOG_CTX_STARTUP,"mgmtUnit=%u",mgmtUnit);

  if (sysapiHpcUnitIdentifierKeyGet(mgmtUnit, &mgrKey) != L7_SUCCESS)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'sysapiHpcUnitIdentifierKeyGet'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return L7_FAILURE;
  }

  LOG_TRACE(LOG_CTX_STARTUP,"mgrKey=%02x:%02x:%02x:%02x:%02x:%02x",
         mgrKey.addr[0],mgrKey.addr[1],mgrKey.addr[2],mgrKey.addr[3],mgrKey.addr[4],mgrKey.addr[5]);

#ifdef L7_STACKING_PACKAGE
  /* Wait for modid assignment of the manager */
  dapiTraceStackEvent("Starting to wait for mgr unit to be discovered\n");

  if (fp_stk_mgr_wait_for_key_discovery(mgrKey.addr, 
                                        BROAD_UNIT_DISCOVER_TIMEOUT, 
                                        BROAD_MGMT_UNIT_DISCOVER_RETRIES) != BCM_E_NONE)
  {
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Error: Cpu card insert failed as there are no modids for local key \n");
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Driver Stacking trace\n\n");
    dapiTraceShow(0, 0, 0);
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Stack topology db\n\n");
    fp_stk_mgr_system_db_dump();
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Unit Manager trace\n\n");
    ut(255);
    SYSAPI_PRINTF(SYSAPI_LOGGING_ALWAYS,
                  "Stack trace of all the tasks\n\n");
    osapiDebugStackTrace(0, 0);
       
    return L7_FAILURE;

  }

  dapiTraceStackEvent("Finished waiting for mgr key to be discovered \n");
#endif


  if (hapiBroadCpuPortMapGet(usp.unit, usp.slot, dapi_g) == L7_FAILURE)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                   "%s %d: In %s call to 'hapiBroadCpuPortMapGet'\n",
                   __FILE__, __LINE__, __FUNCTION__);
    return L7_FAILURE;
  }

  /* Initialize all the ports for the given unit/slot combination */
  if (hapiBroadSlotCtlInit(usp.unit, usp.slot, dapi_g) == L7_FAILURE)
  {
    result = L7_FAILURE;
    return result;
  }

#ifdef L7_QOS_PACKAGE
  /* Initialize the QOS package for all ports on this card */
  if (hapiBroadQosCardInit(usp.unit, usp.slot, dapi_g) == L7_FAILURE)
  {
    result = L7_FAILURE;
    return result;
  }
#endif
#ifdef L7_ROUTING_PACKAGE
  if (hapiBroadL3CardInit(usp.unit, usp.slot, dapi_g) == L7_FAILURE)
  {
    result = L7_FAILURE;
    return result;
  }
#endif
#ifdef L7_MCAST_PACKAGE
  /* Initialize the QOS package for all ports on this card */
  if (hapiBroadL3McastCardInit(usp.unit, usp.slot, dapi_g) == L7_FAILURE)
  {
    result = L7_FAILURE;
    return result;
  }
#endif
  if (hapiBroadSystemPolicyInstall(dapi_g) != L7_SUCCESS)
  {
    return L7_FAILURE;
  }

  LOG_NOTICE(LOG_CTX_STARTUP,"Complete!");

  return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Initializes generic logical cards
*
* @param  unitNum         unit number for this logical card
* @param  slotNum         slot number for this logical card
* @param *dapi_g          system information
*
* @returns L7_RC_t result
*
* @notes used for interfaces that do not have direct hardware support
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadGenericCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd,
                                   void *data, void *handle)
{
  DAPI_USP_t            usp;
  BROAD_PORT_t         *hapiPortPtr;
  DAPI_t               *dapi_g = (DAPI_t*)handle;
  L7_RC_t               rc;

  rc = dapiGenericCardInsert(dapiUsp, cmd, data, handle);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  for (usp.port = 0;
       usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;
       usp.port++)
  {
    hapiPortPtr = (BROAD_PORT_t*)osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                             sizeof(BROAD_PORT_t));
    dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]->hapiPort = (void *)hapiPortPtr;
    bzero((L7_char8*)hapiPortPtr, sizeof(BROAD_PORT_t));
  }

  return rc;
}

/* PTin added: virtual ports */
#if 1
/**
 * Card insert function for VLAN_PORT card
 * 
 * @author mruas (1/9/2015)
 * 
 * @param dapiUsp 
 * @param cmd 
 * @param data 
 * @param handle 
 * 
 * @return L7_RC_t 
 */
L7_RC_t hapiBroadVlanPortCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd,
                                   void *data, void *handle)
{
  DAPI_USP_t            usp;
  BROAD_PORT_t         *hapiPortPtr;
  DAPI_PORT_t          *dapiPortPtr;
  DAPI_t               *dapi_g = (DAPI_t*)handle;
  L7_RC_t               rc;

  rc = dapiGenericCardInsert(dapiUsp, cmd, data, handle);
  if (rc != L7_SUCCESS)
  {
    return rc;
  }

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  for (usp.port = 0;
       usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot;
       usp.port++)
  {
    hapiPortPtr = (BROAD_PORT_t*)osapiMalloc(L7_DRIVER_COMPONENT_ID,
                                             sizeof(BROAD_PORT_t));
    dapiPortPtr = DAPI_PORT_GET(&usp, dapi_g);

    dapi_g->unit[usp.unit]->slot[usp.slot]->port[usp.port]->hapiPort = (void *)hapiPortPtr;
    bzero((L7_char8*)hapiPortPtr, sizeof(BROAD_PORT_t));

    /*
     * Override hardware-supported operations
     */
    hapiBroadL2StdPortInit(dapiPortPtr);
  }

  return rc;
}
#endif

/*********************************************************************
*
* @purpose Uninitializes ALL card types physical, cpu, lag, router
*
* @param   L7_ushort16  unitNum - the unit being initialized
* @param   L7_ushort16  slotNum - the slot being initialized
* @param   DAPI        *dapi_g  - the driver object
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCardRemove(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data, void *handle)
{
  L7_RC_t       result = L7_SUCCESS;
  DAPI_USP_t    usp;
  BROAD_PORT_t *hapiPortPtr;
  DAPI_t       *dapi_g = (DAPI_t*)handle;
  L7_RC_t       rc;
  L7_uint32     i;
  bcmx_lplist_t tmpLpList, portLpList;

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  /* tmpLpList freed at the end of the function */
  bcmx_lplist_init(&tmpLpList,0,0);
  bcmx_lplist_init(&portLpList,0,0);

  switch (dapi_g->unit[usp.unit]->slot[usp.slot]->cardType)
  {

    case SYSAPI_CARD_TYPE_LINE:
      /* Wait for the asynchronous Dot1s operations to finish.
      */
      hapiBroadDot1sAsyncDoneWait ();
      (void) hapiBroadLagAsyncDoneWait (HAPI_BROAD_LAG_ASYNC_DONE_WAIT_TIMEOUT);
      break;

    case SYSAPI_CARD_TYPE_LAG:
       /* Wait for the asynchronous LAG operations to finish.
       */
       (void)hapiBroadLagAsyncDoneWait (HAPI_BROAD_LAG_ASYNC_DONE_WAIT_TIMEOUT);
       break;

    default:
      break;
  }

  for (usp.port = 0; usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot; usp.port++)
  {
    hapiPortPtr = HAPI_PORT_GET(&usp,dapi_g);

    if (hapiPortPtr == L7_NULLPTR)
    {
      /* Free the lplist */
      bcmx_lplist_free(&tmpLpList);
      bcmx_lplist_free(&portLpList);
      return result;
    }

    if (IS_PORT_TYPE_PHYSICAL(DAPI_PORT_GET(&usp, dapi_g)) == L7_TRUE)
    {
      bcmx_lplist_add(&tmpLpList, hapiPortPtr->bcmx_lport);
    }

    /* Skip Stacking ports for portdb list */
    if (spmFpsPortStackingModeCheck (usp.unit, usp.slot, 
                                     (usp.port + 1)) == L7_FALSE)
    {
      bcmx_lplist_add(&portLpList,hapiPortPtr->bcmx_lport);
    }

    if (BCMX_LPORT_TO_UPORT(hapiPortPtr->bcmx_lport) != _bcmx_uport_invalid)
    {
      bcmx_uport_set(hapiPortPtr->bcmx_lport, _bcmx_uport_invalid);
    }

    if (hapiPortPtr->vrrp_interface_id)
      osapiFree(L7_DRIVER_COMPONENT_ID, hapiPortPtr->vrrp_interface_id);

    if (hapiPortPtr->qos)
      osapiFree(L7_DRIVER_COMPONENT_ID, hapiPortPtr->qos);

    if (dapi_g->unit[usp.unit]->slot[usp.slot]->cardType == SYSAPI_CARD_TYPE_VLAN_ROUTER)
    {
      if (hapiPortPtr->routing_if_sync_sem != NULL)
      {
        rc = osapiSemaDelete (hapiPortPtr->routing_if_sync_sem);
        if (rc != L7_SUCCESS)
        {
          L7_LOG_ERROR(rc);
        }
        hapiPortPtr->routing_if_sync_sem = NULL;
      }
    }

    if (IS_PORT_TYPE_PHYSICAL(DAPI_PORT_GET(&usp, dapi_g)) == L7_TRUE)
    {
       osapiSemaDelete (hapiPortPtr->hapiModeparm.physical.phySemaphore);
    }

    if ((IS_PORT_TYPE_PHYSICAL(DAPI_PORT_GET(&usp, dapi_g)) == L7_TRUE) ||
        (IS_PORT_TYPE_LOGICAL_LAG(DAPI_PORT_GET(&usp, dapi_g)) == L7_TRUE))
    {
       if (hapiPortPtr->l2FlushSyncSem != NULL)
       {
          osapiSemaDelete(hapiPortPtr->l2FlushSyncSem);
       }
    }

    if (BROAD_POLICY_INVALID != hapiPortPtr->dot1pPolicy)
        hapiBroadCosPolicyUtilRemove(hapiPortPtr->dot1pPolicy, hapiPortPtr->bcmx_lport);

    for (i = 0; i < L7_DOT1X_PORT_MAX_MAC_USERS; i++)
    {
      if (hapiPortPtr->dot1x.client[i].inUse == L7_TRUE)
      {
        if (BROAD_POLICY_INVALID != hapiPortPtr->dot1x.client[i].policyId)
        {
          hapiBroadPolicyDelete(hapiPortPtr->dot1x.client[i].policyId);
        }
        if (BROAD_POLICY_INVALID != hapiPortPtr->dot1x.client[i].vlanAssignmentPolicyId)
        {
          hapiBroadPolicyDelete(hapiPortPtr->dot1x.client[i].vlanAssignmentPolicyId);
        }
      }
    }

    if (hapiPortPtr)
      osapiFree(L7_DRIVER_COMPONENT_ID, hapiPortPtr);
  }

  switch (dapi_g->unit[usp.unit]->slot[usp.slot]->cardType)
  {
    case SYSAPI_CARD_TYPE_LOGICAL_CPU:
      hapiBroadSystemPolicyRemove(dapi_g);
      break;

    case SYSAPI_CARD_TYPE_LINE:


      if ((cnfgrIsComponentPresent(L7_FLEX_STACKING_COMPONENT_ID) == L7_TRUE)  ||
          (cnfgrIsFeaturePresent(L7_PFC_COMPONENT_ID, 
                                 L7_PFC_PORT_PRIORITY_FLOW_CONTROL_FEATURE_ID) == L7_TRUE))
      { 

        /* Delete entry for the card ports from the USL port-database */
        (void) usl_portdb_update(L7_FALSE, usp.unit, &portLpList);
      }

#ifdef L7_STACKING_PACKAGE
      {
        L7_int32           rv;
        cpudb_key_t        cpuKey;

       
        /* Can't rely on unit-manager to get the unit's key as it may
        ** have already removed the unit record 
        */
        if (usl_db_unit_key_get(usp.unit, (L7_enetMacAddr_t *)&cpuKey.key) == L7_SUCCESS)
        {
          /* Device is detached from bcmx in usl_card_remove */
      usl_card_remove(usp.unit,usp.slot);
    
          dapiTraceStackEvent("Cpu Key: %x:%x:%x:%x:%x:%x  ",
                              cpuKey.key[0], cpuKey.key[1], cpuKey.key[2],
                              cpuKey.key[3], cpuKey.key[4], cpuKey.key[5]); 

          rv = fp_stk_mgr_key_detach(cpuKey.key);
          if ((rv != BCM_E_NONE) && (rv != BCM_E_NOT_FOUND))
          { 
            dapiTraceStackEvent("Detach failed, rv %d\n", rv);
          }
          else
          {
            dapiTraceStackEvent("Detached \n");
          }
        }
        else 
        {
          dapiTraceStackEvent("Unit %d detach failed as key is not available\n", usp.unit);
        }
      }
#endif

      
      break;

    case SYSAPI_CARD_TYPE_LAG:
       break;

    case SYSAPI_CARD_TYPE_VLAN_ROUTER:
      /*
       * Tell routing that the card is being removed so that
       * the ASYNC operations can be flushed until the card is
       * re-inserted.
       */
      hapiBroadL3RouterCardRemove(dapiUsp, cmd, data, handle);
      break;

    default:
      break;
  }

  /* Free the lplist */
  bcmx_lplist_free(&tmpLpList);
  bcmx_lplist_free(&portLpList);

  return result;
}



/*********************************************************************
*
* @purpose Replays configuration that is system based in our application
*          but interface based in the hardware
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadApplyConfig(L7_uint32 unit,L7_uint32 slot, DAPI_t *dapi_g)
{
  L7_RC_t                 result    = L7_SUCCESS;
  DAPI_USP_t              usp = {-1,-1,-1};  /* NA for these commands */
  L7_uchar8 mac[6];
#ifdef L7_MCAST_PACKAGE
  int rv;
#endif

  usp.unit = unit;
  usp.slot = slot;

  if(IS_SLOT_TYPE_PHYSICAL(&usp, dapi_g))
  {
    /* Set the flow control state since it is a global */
    result = sysapiHpcIfaceMacGet(L7_CPU_INTF, L7_CPU_SLOT_NUM, 0,
                                  (L7_uchar8 *)mac, L7_NULLPTR);
    if (result != L7_SUCCESS)
    {
      return result;    
    }

    usl_bcmx_flow_control_set(unit, dapi_g->system->flowControlEnable, mac);
  }

#ifdef L7_ROUTING_PACKAGE
  result = hapiBroadL3ApplyConfig(dapi_g);

  if (result != L7_SUCCESS)
  {
    return L7_FAILURE;
  }
#endif /* L7_ROUTING_PACKAGE */

  hapiBroadAddrMacFilterConfigApply(dapi_g);


#ifdef L7_MCAST_PACKAGE
  /* If ip multicast is not enabled, then start with IPMC disabled.
   * SDK enables IPMC by default
   */
  if (dapi_g->system->mcastEnable == L7_TRUE)
  {
    rv = usl_bcmx_ipmc_enable(L7_TRUE);
  }
  else
  {
    rv = usl_bcmx_ipmc_enable(L7_FALSE);
  }

  if (L7_BCMX_OK(rv) != L7_TRUE)
    return L7_FAILURE;
        
#endif

  /* Send the IGMP filter if igmp snooping is configured */
  if (dapi_g->system->igmpSnoopingEnable)
    hapiBroadConfigIgmpFilter(L7_TRUE, L7_NULL, L7_FALSE,dapi_g);         /* PTin modified: IGMP snooping */

#ifndef FEAT_METRO_CPE_V1_0
  /* Set the MLD filter if either l3 mld or mld snooping is configured */
  if (dapi_g->system->mldSnoopingEnable || dapi_g->system->mldEnable)
    hapiBroadConfigMldFilter(L7_TRUE,dapi_g);
#endif 

  return result;
}

#ifdef L7_STACKING_PACKAGE
/*******************************************************************************
*
* @purpose Routine to do processing after all card-plugins in the stack are done
*
*
* @returns none
* @notes   On Stackabales, the admin mode of the ports is not configured in the
*          hardware until all the cards of the known connected units are plugged-in.
*          This is done to do the system initialization faster. If we enable the ports
*          as they are attached then it is possible that these ports can slam the cpu
*          with traffic and that would delay the attach processing of remaining cards.
*
*          This routine would normally be called when the last known card is plugged-in.
*          It can also be called when the HAPI_BROAD_STACK_CARD_PLUGIN_TIMER which
*          should happen only in case of errors.
*
*
* @end
*
*******************************************************************************/
void hapiBroadStackCardPluginsPostProcess(L7_uint32 args, DAPI_t *dapi_g)
{
  L7_uint32             unitIdx, slotIdx;
  L7_RC_t               result;
  unitMgrStatus_t       unitStatus;  

  dapiCardRemovalReadLockTake();

  for (unitIdx = 0; unitIdx <= L7_MAX_UNITS_PER_STACK; unitIdx++) 
  {
    /* Make sure unit is present */
    result = unitMgrUnitStatusGet(unitIdx, &unitStatus);
    if ((result != L7_SUCCESS) || (unitStatus != L7_UNITMGR_UNIT_OK) ||
        (usl_unit_sync_check(unitIdx) == L7_FALSE))
    {
      continue;
    }

    for (slotIdx = 0; slotIdx < L7_MAX_SLOTS_PER_UNIT; slotIdx++) 
    {
      (void)dapiCardConfigWriteLockTake(unitIdx, slotIdx);

      /* Check if this is a line card and adminMode config is pending */
      if ((dapi_g->unit[unitIdx]->slot[slotIdx]->cardPresent == L7_TRUE) &&
          (dapi_g->unit[unitIdx]->slot[slotIdx]->cardType == SYSAPI_CARD_TYPE_LINE) &&
          (dapi_g->unit[unitIdx]->slot[slotIdx]->pendingAdminModeConfig == L7_TRUE))
      {
        /* Apply the admin mode for all the ports in this card */
        /*printf("Applying admin mode for unit/slot %d/%d\n", unitIdx, slotIdx);*/
        hapiBroadSystemCardPortsAdminModeSet(unitIdx, slotIdx, L7_FALSE, L7_FALSE, dapi_g);
        dapi_g->unit[unitIdx]->slot[slotIdx]->pendingAdminModeConfig = L7_FALSE;
      }
     
      (void)dapiCardConfigWriteLockGive(unitIdx, slotIdx);
    }
  }

  dapiCardRemovalReadLockGive();

  return;
}

/*******************************************************************************
*
* @purpose Timer handler for HAPI_BROAD_STACK_CARD_PLUGIN_TIMER
*
* @param   arg1
* @param   arg2
*
* @returns none
*
* @notes  Under normal circumstances the timer would not pop and the post card-plugin 
*         processing would be done after the last card has been plugged-in. 
*         This timer is a fail-safe mechanism to ensure cases where driver
*         is still waiting for more cards to be plugged-in and for "some"
*         reasons that never happens. 
*
* @end
*
*******************************************************************************/
void hapiBroadStackCardPluginsFinishTimerProcess(L7_uint32 arg1, L7_uint32 arg2)
{
  /* Move the post init processing to another task */
  if (osapiTaskCreate ("HAPI_BROAD_STACK_CARD_PLUGIN_TASK", hapiBroadStackCardPluginsPostProcess,1,(void *)arg1,
                       L7_DEFAULT_STACK_SIZE,
                       L7_DEFAULT_TASK_PRIORITY,
                       L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    L7_LOG_ERROR(0);
  }

  return;
}

#endif

/*********************************************************************
*
* @purpose Notification from CMGR to the driver about a card plug-in
*
*
* @returns L7_RC_t result
*
* @notes   CARD_CMD_PLUGIN_START is issued before the L7_ATTACH for the card is issued
*          to NIM.
*          CARD_CMD_PLUGIN_FINISH is issued after NIM/Application has finished L7_ATTACH
*          processing.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCardPluginNotify (DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                     result = L7_SUCCESS;
 
  hapiBroadSystemCardPortLinkupGenerate(usp->unit, usp->slot, dapi_g);
  
  if (hapiBroadApplyConfig(usp->unit,usp->slot,dapi_g) == L7_FAILURE)
  {
    L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_DRIVER_COMPONENT_ID, 
            "Call to hapiBroadApplyConfig failed\n");
  }

  return result;
}

/*********************************************************************
*
* @purpose Notification from CMGR to the driver about a card plug-in
*
*
* @returns L7_RC_t result
*
* @notes   CARD_CMD_UNPLUG_START is issued before the L7_DETACH for the card is issued
*          to NIM/Application.
*          CARD_CMD_UNPLUG_FINISH is issued after NIM/Application has finished L7_DETACH
*          processing.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCardUnplugNotify (DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
    return L7_SUCCESS;
  }

/* PTin modified: startup configuration */
L7_RC_t hapiBroadStart(L7_LAST_STARTUP_REASON_t startupReason)
{
  L7_RC_t rc;

  rc = hapiBroadPtinDataInit();

  return rc;
}
/* PTin end */

/*********************************************************************
*
* @purpose Start the driver in cold or warm mode
*
*
* @returns L7_RC_t result
*
* @notes  If warm start could not be done, then failure is returned and
*         cold start is initiated.
*
* @end
*
*********************************************************************/
L7_RC_t broadDriverStart(void *data)
{
  L7_RC_t                        rc = L7_SUCCESS;
  DAPI_CONTROL_GENERIC_CMD_t    *dapiCmd = (DAPI_CONTROL_GENERIC_CMD_t *)data;
  L7_LAST_STARTUP_REASON_t       startupReason;

  startupReason  = dapiCmd->cmdData.startup.startupReason;

  do
  {

    /* Start the USL layer first. If warm mode fails, then usl starts in cold mode */
    rc = uslStart(startupReason, 
                  dapiCmd->cmdData.startup.mgrFailover,
                  dapiCmd->cmdData.startup.lastMgrUnitId,
                  dapiCmd->cmdData.startup.lastMgrKey);  

    if (rc != L7_SUCCESS)
    {
      /* request denied indicates warm start could not be initiated, so revert to cold start */
      if (rc == L7_REQUEST_DENIED)
      {
        startupReason = L7_STARTUP_AUTO_COLD;  
      }
      else /* Fatal error */
        L7_LOG_ERROR(rc);
    }

    /* Start hapi layer */
    if (hapiBroadStart(startupReason) != L7_SUCCESS)
    {
      /* Fatal error */
      L7_LOG_ERROR(0);
    }

  } while (0);
  
  return rc;
}

/*********************************************************************
*
* @purpose Notification from CNFGR that application hw updates are done
*          for a phase
*
*
* @returns L7_RC_t result
*
* @notes   
* @end
*
*********************************************************************/
L7_RC_t hapiBroadControlHwApplyNotify (DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                        rc;
  DAPI_CONTROL_GENERIC_CMD_t    *dapiCmd = (DAPI_CONTROL_GENERIC_CMD_t *)data;

  rc = usl_hw_apply_notify(dapiCmd->cmdData.hwApplyNotification.phase);

  return rc;
}


/*********************************************************************
*
* @purpose Notification about a unit stack status update (disconnected only)
*
*
* @returns L7_RC_t result
*
* @notes   
* @end
*
*********************************************************************/
L7_RC_t hapiBroadControlUnitStatusNotify (DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, DAPI_t *dapi_g)
{
  L7_RC_t                        rc = L7_SUCCESS;

#ifdef L7_STACKING_PACKAGE
  DAPI_CONTROL_GENERIC_CMD_t    *dapiCmd = (DAPI_CONTROL_GENERIC_CMD_t *)data;


  rc = usl_unit_status_notify(dapiCmd->cmdData.unitStatus.unit, 
                              dapiCmd->cmdData.unitStatus.flag);
#endif

  return rc;
}


/*********************************************************************
*
* @purpose Notification from USL that warm start is complete
*
*
* @returns L7_RC_t result
*
* @notes   Triggers hardware retries of entries that could not be
*          inserted in the hardware due to temporary table full 
*          condition.
*
* @end
*
*********************************************************************/
void hapiBroadWarmStartCompleteCb(void)
{

  hapiBroadL2McastRetryFailures();

#ifdef L7_ROUTING_PACKAGE
  hapiBroadL3WarmStartRetryFailures();
#endif

  return;
}
