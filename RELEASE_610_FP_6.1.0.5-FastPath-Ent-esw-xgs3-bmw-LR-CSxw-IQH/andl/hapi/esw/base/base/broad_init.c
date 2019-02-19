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
#include "broad_l2_lag.h"
#include "ibde.h"

#include "osapi.h"
#include "osapi_support.h"
#include "sysapi.h"
#include "platform_config.h"
#include "registry.h"

#include "broad_policy.h"
#include "broad_policy_util.h"
#include "broad_l2_ipsg.h"

#ifdef L7_QOS_PACKAGE
#include "broad_qos.h"
#endif

#ifdef L7_MCAST_PACKAGE
#include "broad_l3_mcast.h"
#include "l7_mcast_api.h"
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
#include "bcmx/filter.h"
#include "bcmx/switch.h"
#include "ibde.h"
#include "l7_usl_api.h"
#include "l7_usl_bcm.h"
#include "l7_usl_port_cache.h"
#include "l7_usl_bcm_l2.h"
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
#include "broad_ptin.h"
#include "ptin_hapi.h"

#include "broad_utils.h"
extern void *hapiLinkStatusQueue;
extern L7_BOOL hpcBroadNoStackPorts;

osapiTimerDescr_t *hapiBroadStackCardPluginsFinishTimer = L7_NULL;
void hapiBroadStackCardPluginsPostProcess(L7_uint32 args, DAPI_t *dapi_g);
void hapiBroadStackCardPluginsFinishTimerProcess(L7_uint32 arg1, L7_uint32 arg2);

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

#ifdef L7_ROBO_SUPPORT
  for (i = 0; i < bde->num_devices(BDE_SWITCH_DEVICES); i++)
#else
  for (i = 0; i < bde->num_devices(BDE_ALL_DEVICES); i++)
#endif
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

    rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_DOT1AD_SNOOP, &hapiSystem->ipv6SnoopId);
    do
    {
    /* give MDL frames high priority and trap to the CPU. */
       if ((rc = hapiBroadPolicyRuleAdd(hapiSystem->ipv6SnoopId, &ruleId)) != L7_SUCCESS)
    {
         break;
       }
       if((rc = hapiBroadPolicyRuleQualifierAdd(hapiSystem->ipv6SnoopId, ruleId,
                                             BROAD_FIELD_MACDA,
                                             mldSnoop_macda, mldSnoop_macda_mask)) !=
                                             L7_SUCCESS)
    {
         break;
       }
       if((rc = hapiBroadPolicyRuleQualifierAdd(hapiSystem->ipv6SnoopId, ruleId,
                                             BROAD_FIELD_ETHTYPE,
                                             (L7_uchar8 *)&ipV6_ethtype, exact_match)) !=
                                             L7_SUCCESS)
    {
        break;
    }

      if((rc = hapiBroadPolicyRuleQualifierAdd(hapiSystem->ipv6SnoopId, ruleId,
                                             BROAD_FIELD_IP6_NEXTHEADER,
                                             (L7_uchar8 *)&ip_icmpv6, exact_match)) !=
                                             L7_SUCCESS)
    {
        break;
    }

      if((rc = hapiBroadPolicyRuleQualifierAdd(hapiSystem->ipv6SnoopId, ruleId,
             BROAD_FIELD_PKT_FORMAT,
             (L7_uchar8 *)&ipV6_format, exact_match)) !=
             L7_SUCCESS)
    { 
       break;
     }
     if((rc = hapiBroadPolicyRuleActionAdd(hapiSystem->ipv6SnoopId, ruleId, BROAD_ACTION_SET_COSQ,
                                          HAPI_BROAD_INGRESS_MED_PRIORITY_COS-1, 0, 0)) !=
                                          L7_SUCCESS)
    {
      break;
    }
     rc = hapiBroadPolicyRuleActionAdd(hapiSystem->ipv6SnoopId, ruleId,
                                      BROAD_ACTION_SET_REASON_CODE,
                                      8, 0, 0);
     
    if((rc = hapiBroadPolicyRuleActionAdd(hapiSystem->ipv6SnoopId, ruleId,
                                          BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0)) !=
                                          L7_SUCCESS)
    {
      break;
    }
    /* Commit the policy */
    if ((rc = hapiBroadPolicyCommit(hapiSystem->ipv6SnoopId)) != L7_SUCCESS)
    {
      break;
    }
   }while(0);

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
  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM, &sysId1);   /* Policy 1 */

  /* give dot1x EAPOL packets high priority so they reach the cpu */
  hapiBroadPolicyRuleAdd(sysId1, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&eap_ethtype, exact_match);
#ifdef BCM_ROBO_SUPPORT
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_SET_REASON_CODE,
                                       BCM_ROBO_RX_REASON_PROTOCOL_TERMINATION, 0, 0);
#else
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
#endif
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);

  /* Drop BPDUs which are within the reserved range but not used. Add rule
     with least priority for the valid address range to trap to CPU */
  hapiBroadPolicyPriorityRuleAdd(sysId2, &ruleId, BROAD_POLICY_RULE_PRIORITY_LOWEST);
  hapiBroadPolicyRuleQualifierAdd(sysId2, ruleId, BROAD_FIELD_MACDA, res_macda, res_mac_drop_mask);
  hapiBroadPolicyRuleActionAdd(sysId2, ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);

  /* give BPDUs high priority */
  hapiBroadPolicyRuleAdd(sysId1, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_MACDA, res_macda, res_macmask);
#ifdef BCM_ROBO_SUPPORT
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_SET_REASON_CODE,
                                       BCM_ROBO_RX_REASON_PROTOCOL_TERMINATION, 0, 0);
#else
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_BPDU_COS, 0, 0);
#endif
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);

#ifdef L7_ISDP_PACKAGE
  /* trap cdp/dtp/vtp/pagp traffic */
  hapiBroadPolicyRuleAdd(sysId1, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_MACDA, isdp_macda, exact_match);
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
#endif /* L7_ISDP_PACKAGE */

  /* give ARP requests high priority, so if the CPU is slammed w/ data traffic, it will
  ** be more likely to get ARPs. ARP replies to switch MAC address will reach CPU as a result
  ** of L2 entry. Also, the system MAC filter assigns higher priority too. ARP replies to
  ** router/VRRP MAC address are handled in L3 code. Idea is to avoid tunneling ARP replies.
  */
  hapiBroadPolicyRuleAdd(sysId1, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&arp_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_MACDA, bcast_macda, exact_match);
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
#ifdef BCM_ROBO_SUPPORT
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_SET_REASON_CODE,
                                            BCM_ROBO_RX_REASON_PROTOCOL_SNOOP, 0, 0);
#endif

  result = hapiBroadPolicyCommit(sysId1);

  if (L7_SUCCESS != result)
      return result;

#ifdef L7_DOT3AH_PACKAGE
  hapiBroadDot3ahInit(dapi_g);
#endif

#ifdef BCM_ROBO_SUPPORT /* ADDED for SNTP Broad packet */
   /* Create policy for L4 specific information, e.g. SNTPs */
  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM, &sysId2);   /* Policy 2 */
  hapiBroadPolicyRuleAdd(sysId2, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(sysId2, ruleId, BROAD_FIELD_PROTO, ip_udp_proto, exact_match);
  hapiBroadPolicyRuleQualifierAdd(sysId2, ruleId, BROAD_FIELD_DPORT, (L7_uchar8 *)&sntp_dport, exact_match);
  hapiBroadPolicyRuleActionAdd(sysId2, ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);

  result = hapiBroadPolicyCommit(sysId2);
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

       hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM, &fpsId);
       hapiBroadPolicyRuleAdd(fpsId, &fpsRuleId);
       hapiBroadPolicyRuleQualifierAdd(fpsId, fpsRuleId, BROAD_FIELD_OVID,
                                       (L7_uchar8*)&vlanId, exact_match);
       hapiBroadPolicyRuleActionAdd(fpsId, fpsRuleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
       result = hapiBroadPolicyCommit(fpsId);
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
  hapiSystem->dhcpSnoopingPolicyId[0][0] = BROAD_POLICY_INVALID;
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
// PTin added
#define PTIN_COSQ_MARKING   0
L7_RC_t hapiBroadSystemPolicyInstall(DAPI_t *dapi_g)
{
  BROAD_SYSTEM_t     *hapiSystem;
  L7_RC_t             result;
  L7_ushort16         arp_ethtype = L7_ETYPE_ARP;
  L7_ushort16         ip_ethtype  = L7_ETYPE_IP;
#ifdef L7_IPV6_PACKAGE
  L7_ushort16         ipV6_ethtype = L7_ETYPE_IPV6;
#endif
  L7_ushort16         eap_ethtype = L7_ETYPE_EAPOL;
  L7_uchar8           udp_proto[]   = {IP_PROT_UDP};
#if (PTIN_COSQ_MARKING)
  L7_uchar8           vrrp_proto[]  = {IP_PROT_VRRP};
  L7_uchar8           ospf_proto[]  = {IP_PROT_OSPFIGP};
  L7_uchar8           icmp_proto[]        = {IP_PROT_ICMP};
  L7_ushort16         icmp_rtr_solicit = 0x0a00;
  L7_ushort16         icmp_type_match  = 0xff00;
  L7_uchar8           tcp_proto[]   = {IP_PROT_TCP};
  L7_ushort16         rip_dport   = 0x0208;
  L7_ushort16         bgp_dport   = TCP_PORT_BGP;
  L7_uchar8           bcast_macda[]   = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
#endif
  L7_uchar8           cdpDstMac[] = {0x01,0x00,0x0c,0xcc,0xcc,0xcc};

  L7_uchar8           dhcp_serverPort[] = {0x00, 0x43}; /* DHCP Port 67 */
  L7_uchar8           dhcp_clientPort[] = {0x00, 0x44}; /* DHCP Port 68 */
#ifdef L7_ISDP_PACKAGE
  // PTin removed
  //L7_uchar8           isdp_macda[]  = {0x01, 0x00, 0x0c, 0xcc, 0xcc, 0xcc};
#endif
#if 0
  L7_uchar8           res_macda[]   = {0x01, 0x80, 0xC2, 0x00, 0x00, 0x00};
  L7_uchar8           res_macmask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, 0xC0};
  L7_uchar8           res_mac_drop_mask[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                             FIELD_MASK_ALL, FIELD_MASK_ALL, FIELD_MASK_ALL};
#endif
  L7_uchar8           exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                       FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};
  BROAD_POLICY_t      sysId1, sysId2;
  BROAD_POLICY_RULE_t ruleId;
  bcm_chip_family_t board_family;
  BROAD_METER_ENTRY_t meterInfo;
#ifdef L7_IPV6_PACKAGE
  const bcm_sys_board_t *board_info;
#endif

  if ((hapiBroadRaptorCheck() == L7_TRUE) ||
      (hapiBroadRoboCheck()== L7_TRUE) || (hapiBroadHawkeyeCheck() == L7_TRUE) )
  {
    return hapiBroadSystemPolicyInstallRaptor(dapi_g);
  }

  hapiBroadGetSystemBoardFamily(&board_family);

  hapiSystem = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

#if (PTIN_COSQ_MARKING)
  /* DHCP packets on ports must go to the CPU and be rate limited to 64 kbps */
  result = hapiBroadDhcpPolicyInstall(dapi_g);
  if (L7_SUCCESS != result)
    return result;
#endif

  /* Install IP Source Guard default policy */
  result = hapiBroadIpsgDefaultPolicyInstall(dapi_g);
  if (L7_SUCCESS != result)
    return result;

  /* Create system policy to apply to all Ethernet ports (FE/GE/XG) - not stack ports.
  ** Add rules with highest precedence first. Note that until Rule Chaining is
  ** implemented on XGS3 it is not possible to combine all L2/3/4 fields into a single
  ** policy. Therefore, it is necessary to allocate two policies, one for MAC DA and
  ** the other for the remaining policies. It should not affect the operation.
  */
  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM, &sysId1);   /* Policy 1 of 2 */

#ifdef L7_IPV6_PACKAGE
  /*
   * Add a rule to ensure that all tunnel packets that
   * miss L3 lookups are copied to the CPU fully encapsulated.
   * Otherwise the packets are delivered to the CPU decapsulated, 
   * and the software cannot determine the source tunnel interface
   * to process the packet correctly. 
   */
  board_info = hpcBoardGet();
  if (board_info != L7_NULL)
  {
    switch (board_info->npd_id)
    {
      /* The following platforms need this policy in order to ensure that these
         tunnel terminated packets get to the CPU fully encapsulated. Newer
         T2 chips have issues with Tunnel Hit qualifiers. For T2, we workaround
         using the BCM_VLAN_UNKNOWN_IP6_MCAST_TOCPU bit */
    case __BROADCOM_56304_ID:
    case __BROADCOM_56314_ID:
    case __BROADCOM_56504_ID:
    case __BROADCOM_56800_ID:
    case __BROADCOM_56514_ID:
    case __BROADCOM_56224_ID:
    case __BROADCOM_56228_ID:
    case __BROADCOM_56624_ID:
    case __BROADCOM_56820_ID:
      {
      L7_ushort16 tunnel_hit    = osapiHtons(BROAD_LOOKUPSTATUS_TUNNEL_HIT);
      L7_ushort16 tunnel_l3_hit = osapiHtons((BROAD_LOOKUPSTATUS_TUNNEL_HIT | BROAD_LOOKUPSTATUS_L3_DST_HIT | BROAD_LOOKUPSTATUS_LPM_HIT));

      /* Note: This rule will send OSPFv3 packets coming over a tunnel to CPU 
         fully encapsulated and on higher priority.
       */
      hapiBroadPolicyPriorityRuleAdd(sysId1, &ruleId,BROAD_POLICY_RULE_PRIORITY_DEFAULT);
      //hapiBroadPolicyRuleAdd(sysId1, &ruleId);
      hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_LOOKUP_STATUS, (L7_uchar8 *)&tunnel_hit, (L7_uchar8 *)&tunnel_l3_hit);
      hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_COPY_TO_CPU, 0, 0, 0);
      hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
      }
      break;

    default:
      break;
    }
  }
#endif /* L7_IPV6_PACKAGE */

#if (PTIN_COSQ_MARKING)
  /* give RIP frames high priority. We give multicast frames
  ** high priority in hapiBroadRoutingIntfLocalMcastAdd(), but RIPv1 and RIPv1c
  ** also use broadcast frames for establishing adjacency, so we need to
  ** guarantee that these get to the CPU if the CPU is getting slammed w/
  ** other data traffic.
  */
  hapiBroadPolicyPriorityRuleAdd(sysId1, &ruleId,BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  //hapiBroadPolicyRuleAdd(sysId1, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_PROTO,   udp_proto,  exact_match);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_DPORT,   (L7_uchar8 *)&rip_dport,  exact_match);
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);

  /* give OSPF frames high priority. We give multicast frames
  ** high priority in hapiBroadRoutingIntfLocalMcastAdd(), but OSPF
  ** also uses unicast frames for establishing adjacency, so we need to
  ** guarantee that these get to the CPU if the CPU is getting slammed w/
  ** other data traffic.
  */
  hapiBroadPolicyPriorityRuleAdd(sysId1, &ruleId,BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  //hapiBroadPolicyRuleAdd(sysId1, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_PROTO,   ospf_proto, exact_match);
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);

  /* give BGP frames high priority. We need to gaurantee that these get to the CPU if the CPU
   * is slammed with other data traffic.
   */

  hapiBroadPolicyPriorityRuleAdd(sysId1, &ruleId,BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  //hapiBroadPolicyRuleAdd(sysId1, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_PROTO,   tcp_proto,  exact_match);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_DPORT,   (L7_uchar8 *)&bgp_dport,  exact_match);
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);

  /* Copy VRRP frames to cpu */
  hapiBroadPolicyPriorityRuleAdd(sysId1, &ruleId,BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  //hapiBroadPolicyRuleAdd(sysId1, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_PROTO,   vrrp_proto, exact_match);
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_COPY_TO_CPU, 0, 0, 0);
  hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
  
  /* give Router solicitation frames medium priority and copy to the CPU.
   * Restricted to XGS3 due to resource constraints on XGS2.
   */
  if ((board_family != BCM_FAMILY_DRACO)  &&
      (board_family != BCM_FAMILY_TUCANA))
  {
    hapiBroadPolicyPriorityRuleAdd(sysId1, &ruleId,BROAD_POLICY_RULE_PRIORITY_DEFAULT);
    //hapiBroadPolicyRuleAdd(sysId1, &ruleId);
    hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ip_ethtype, exact_match);
    hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_PROTO,   icmp_proto, exact_match);
    /* The L4 src port and ICMP type share the same offset in the packet,
     * so just re-use the mechanism for qualifying on an L4 src port.
     * Note the mask only cares about 1 byte instead of 2.
     */
    hapiBroadPolicyRuleQualifierAdd(sysId1, ruleId, BROAD_FIELD_SPORT,   (L7_uchar8 *)&icmp_rtr_solicit, (L7_uchar8 *)&icmp_type_match);
    hapiBroadPolicyRuleActionAdd(sysId1, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
  }

  result = hapiBroadPolicyCommit(sysId1);
  if (L7_SUCCESS != result)
      return result;

#ifdef L7_IPV6_PACKAGE
  /* all link-local icmpv6 and OSPVv3 to cpu */
  {
  BROAD_POLICY_t      Ip6SysId;
  L7_uchar8  hoplim = 255;
  L7_uchar8  icmp_prot = 58;

  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM, &Ip6SysId);
  hapiBroadPolicyPriorityRuleAdd(Ip6SysId, &ruleId,BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  //hapiBroadPolicyRuleAdd(Ip6SysId, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(Ip6SysId, ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ipV6_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(Ip6SysId, ruleId, BROAD_FIELD_IP6_HOPLIMIT,   &hoplim, exact_match);
  hapiBroadPolicyRuleQualifierAdd(Ip6SysId, ruleId, BROAD_FIELD_IP6_NEXTHEADER,   &icmp_prot, exact_match);
  hapiBroadPolicyRuleActionAdd(Ip6SysId, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0,0);

  /* Note that the rule above for OSPF packets will generally work for both IPv4 and IPv6. However, 
     in some cases (e.g. Helix), we use a UDF to qualify on some packet fields and in those cases
    the rule will only work for IPv4 packets. The rule below will ensure that in these cases that
    there is a rule to catch IPv6 OSPF packets as well. */
  hapiBroadPolicyPriorityRuleAdd(Ip6SysId, &ruleId,BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  //hapiBroadPolicyRuleAdd(Ip6SysId, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(Ip6SysId, ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&ipV6_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(Ip6SysId, ruleId, BROAD_FIELD_IP6_NEXTHEADER, (L7_uchar8 *)&ospf_proto, exact_match);
  hapiBroadPolicyRuleActionAdd(Ip6SysId, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0,0);
  result = hapiBroadPolicyCommit(Ip6SysId);
  if (result != L7_SUCCESS)
  {
     return result;
  }
  hapiSystem->Ip6SysId = Ip6SysId;
  }
#endif /* L7_IPV6_PACKAGE */
#endif

  /* Dynamic ARP Inspection: ARP packets on untrusted ports must go to the CPU and be rate limited to 64 kbps */
  meterInfo.cir       = 64;
  meterInfo.cbs       = 64;
  meterInfo.pir       = 64;
  meterInfo.pbs       = 64;
  meterInfo.colorMode = BROAD_METER_COLOR_BLIND;
  meterInfo.ffpmtr_id = 0; /* Not used for XGS3 */
  meterInfo.port      = 0; /* Not used for XGS3 */

  /* ARP packets on untrusted ports must go to the CPU and be rate limited to 64 kbps */
  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM, &hapiSystem->dynamicArpInspectUntrustedPolicyId);

  hapiBroadPolicyPriorityRuleAdd(hapiSystem->dynamicArpInspectUntrustedPolicyId, &ruleId,BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  //hapiBroadPolicyRuleAdd(hapiSystem->dynamicArpInspectUntrustedPolicyId, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(hapiSystem->dynamicArpInspectUntrustedPolicyId, ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&arp_ethtype, exact_match);
  hapiBroadPolicyRuleActionAdd(hapiSystem->dynamicArpInspectUntrustedPolicyId, ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  hapiBroadPolicyRuleNonConfActionAdd(hapiSystem->dynamicArpInspectUntrustedPolicyId, ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
  hapiBroadPolicyRuleMeterAdd(hapiSystem->dynamicArpInspectUntrustedPolicyId, ruleId, &meterInfo);

  result = hapiBroadPolicyCommit(hapiSystem->dynamicArpInspectUntrustedPolicyId);
  if (L7_SUCCESS != result)
      return result;

  result = hapiBroadPolicyRemoveFromAll(hapiSystem->dynamicArpInspectUntrustedPolicyId);
  if (L7_SUCCESS != result)
      return result;

  /* Create policy for L2 specific information, e.g. BPDUs */
  hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM, &sysId2);   /* Policy 2 of 2 */

  /* give dot1x EAPOL packets high priority so they reach the cpu */
  hapiBroadPolicyPriorityRuleAdd(sysId2, &ruleId,BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  //hapiBroadPolicyRuleAdd(sysId2, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(sysId2, ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&eap_ethtype, exact_match);
  hapiBroadPolicyRuleActionAdd(sysId2, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
  hapiBroadPolicyRuleActionAdd(sysId2, ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  hapiBroadPolicyRuleNonConfActionAdd(sysId2, ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0); // PTin added
  hapiBroadPolicyRuleMeterAdd(sysId2, ruleId, &meterInfo);                              // PTin added

  // PTin removed : Let BPDUs not be dropped
  #if 0
  /* Drop BPDUs which are within the reserved range but not used. Add rule
     with least priority for the valid address range to trap to CPU */
  hapiBroadPolicyPriorityRuleAdd(sysId2, &ruleId, BROAD_POLICY_RULE_PRIORITY_LOWEST);
  hapiBroadPolicyRuleQualifierAdd(sysId2, ruleId, BROAD_FIELD_MACDA, res_macda, res_mac_drop_mask);
  hapiBroadPolicyRuleActionAdd(sysId2, ruleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);

  /* give BPDUs high priority */
  hapiBroadPolicyRuleAdd(sysId2, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(sysId2, ruleId, BROAD_FIELD_MACDA, res_macda, res_macmask);
  hapiBroadPolicyRuleActionAdd(sysId2, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_BPDU_COS, 0, 0);
  hapiBroadPolicyRuleActionAdd(sysId2, ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);
  #endif

#ifdef L7_ISDP_PACKAGE
  // PTin removed
  /* trap cdp/udld/dtp/vtp/pagp traffic */
  /*hapiBroadPolicyRuleAdd(sysId2, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(sysId2, ruleId, BROAD_FIELD_MACDA, isdp_macda, exact_match);
  hapiBroadPolicyRuleActionAdd(sysId2, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_HIGH_PRIORITY_COS, 0, 0);
  hapiBroadPolicyRuleActionAdd(sysId2, ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);*/
#endif /* L7_ISDP_PACKAGE */

#if (PTIN_COSQ_MARKING)
  /* give ARP requests high priority, so if the CPU is slammed w/ data traffic, it will
  ** be more likely to get ARPs. ARP replies to switch MAC address will reach CPU as a result
  ** of L2 entry. Also, the system MAC filter assigns higher priority too. ARP replies to
  ** router/VRRP MAC address are handled in L3 code. Idea is to avoid tunneling ARP replies.
  */
  // PTin removed
  hapiBroadPolicyPriorityRuleAdd(sysId2, &ruleId,BROAD_POLICY_RULE_PRIORITY_DEFAULT);
  //hapiBroadPolicyRuleAdd(sysId2, &ruleId);
  hapiBroadPolicyRuleQualifierAdd(sysId2, ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&arp_ethtype, exact_match);
  hapiBroadPolicyRuleQualifierAdd(sysId2, ruleId, BROAD_FIELD_MACDA, bcast_macda, exact_match);
  hapiBroadPolicyRuleActionAdd(sysId2, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);

#ifdef L7_STACKING_PACKAGE
  hapiBroadPolicyEnableFPS(sysId2); /* Enable on FPS ports, if applicable */
#endif
  result = hapiBroadPolicyCommit(sysId2);

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
       BROAD_POLICY_t      fpsId;
       BROAD_POLICY_RULE_t fpsRuleId;
       L7_ushort16         vlanId = HPC_STACKING_VLAN_ID;

       hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM, &fpsId);
       hapiBroadPolicyPriorityRuleAdd(fpsId, &ruleId,BROAD_POLICY_RULE_PRIORITY_DEFAULT);
       //hapiBroadPolicyRuleAdd(fpsId, &fpsRuleId);
       hapiBroadPolicyRuleQualifierAdd(fpsId, fpsRuleId, BROAD_FIELD_OVID,
                                       (L7_uchar8*)&vlanId, exact_match);
       hapiBroadPolicyRuleActionAdd(fpsId, fpsRuleId, BROAD_ACTION_HARD_DROP, 0, 0, 0);
       result = hapiBroadPolicyCommit(fpsId);
       if (result == L7_SUCCESS)
       {
         hapiSystem->fpsSysId = fpsId;
       }
    }
  }
#endif

  /* Create policy to receive CDP packets at CPU. */
   
  if ((L7_TRUE == cnfgrIsComponentPresent(L7_VOICE_VLAN_COMPONENT_ID)) &&
      (BROAD_POLICY_INVALID == hapiSystem->voiceCdpSysId))
  {
    BROAD_POLICY_t      voiceId;
    BROAD_POLICY_RULE_t voiceRuleId;

    if (L7_SUCCESS == hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM_PORT, &voiceId))
    {
      if ((L7_SUCCESS == hapiBroadPolicyPriorityRuleAdd(voiceId, &voiceRuleId, BROAD_POLICY_RULE_PRIORITY_HIGHEST)) &&
          (L7_SUCCESS == hapiBroadPolicyRuleQualifierAdd(voiceId, voiceRuleId, BROAD_FIELD_MACDA, cdpDstMac, exact_match)) &&
          (L7_SUCCESS == hapiBroadPolicyRuleActionAdd(voiceId, voiceRuleId, BROAD_ACTION_COPY_TO_CPU, 0, 0, 0)) &&
          (L7_SUCCESS == hapiBroadPolicyCommit(voiceId)))

      {
        hapiSystem->voiceCdpSysId = voiceId;
      }
      else
      {
        hapiBroadPolicyRuleDelete(voiceId, voiceRuleId);
        hapiBroadPolicyDelete(voiceId);
      }
    }
  }

  /* Create policy to receive DHCP packets at CPU for voice VLAN. */
  if ((L7_TRUE == cnfgrIsComponentPresent(L7_VOICE_VLAN_COMPONENT_ID)) &&
      (BROAD_POLICY_INVALID == hapiSystem->voiceDhcpSysId))
  {
    BROAD_POLICY_t      voiceId;
    BROAD_POLICY_RULE_t voiceRuleId;

    if (L7_SUCCESS == hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM_PORT, &voiceId))
    {
      if ((L7_SUCCESS == hapiBroadPolicyPriorityRuleAdd(voiceId, &voiceRuleId, BROAD_POLICY_RULE_PRIORITY_HIGHEST)) &&
          (L7_SUCCESS == hapiBroadPolicyRuleQualifierAdd(voiceId, voiceRuleId, BROAD_FIELD_ETHTYPE,(L7_uchar8 *)&ip_ethtype, exact_match)) &&
          (L7_SUCCESS == hapiBroadPolicyRuleQualifierAdd(voiceId, voiceRuleId, BROAD_FIELD_PROTO,   udp_proto,  exact_match)) &&
          (L7_SUCCESS == hapiBroadPolicyRuleQualifierAdd(voiceId, voiceRuleId, BROAD_FIELD_DPORT, dhcp_serverPort, exact_match)) &&
          (L7_SUCCESS == hapiBroadPolicyRuleQualifierAdd(voiceId, voiceRuleId, BROAD_FIELD_SPORT, dhcp_clientPort, exact_match)) &&
          (L7_SUCCESS == hapiBroadPolicyRuleActionAdd(voiceId, voiceRuleId, BROAD_ACTION_COPY_TO_CPU, 0, 0, 0)) &&
          (L7_SUCCESS == hapiBroadPolicyCommit(voiceId)))
      {
        hapiSystem->voiceDhcpSysId = voiceId;
      }
      else
      {
        hapiBroadPolicyRuleDelete(voiceId, voiceRuleId);
        hapiBroadPolicyDelete(voiceId);
      }
    }
  }
  if (L7_SUCCESS == result)
  {
    hapiSystem->sysId1 = sysId1;
    hapiSystem->sysId2 = sysId2;
  }
  result = hapiBroadDot1xViolationPolicyCreate(dapi_g);

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

    if (BROAD_POLICY_INVALID != hapiSystem->dhcpSnoopingPolicyId[0][0]) {
        if (hapiBroadPolicyDelete(hapiSystem->dhcpSnoopingPolicyId[0][0]) != L7_SUCCESS)
            result = L7_FAILURE;
        
        hapiSystem->dhcpSnoopingPolicyId[0][0] = BROAD_POLICY_INVALID;
    }

    if (BROAD_POLICY_INVALID != hapiSystem->dhcpDefaultPolicyId[0]) {
        if (hapiBroadPolicyDelete(hapiSystem->dhcpDefaultPolicyId[0]) != L7_SUCCESS)
            result = L7_FAILURE;

        hapiSystem->dhcpDefaultPolicyId[0] = BROAD_POLICY_INVALID;
    }
    if (BROAD_POLICY_INVALID != hapiSystem->dhcpDefaultPolicyId[1]) {
        if (hapiBroadPolicyDelete(hapiSystem->dhcpDefaultPolicyId[1]) != L7_SUCCESS)
            result = L7_FAILURE;

        hapiSystem->dhcpDefaultPolicyId[1] = BROAD_POLICY_INVALID;
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

  if (BROAD_POLICY_INVALID != hapiSystem->voiceCdpSysId)
  {
    if (L7_SUCCESS != hapiBroadPolicyDelete(hapiSystem->voiceCdpSysId))
    {
      result = L7_FAILURE;
    }

    hapiSystem->voiceCdpSysId = BROAD_POLICY_INVALID;
  }

  if (BROAD_POLICY_INVALID != hapiSystem->voiceDhcpSysId)
  {
    if (L7_SUCCESS != hapiBroadPolicyDelete(hapiSystem->voiceDhcpSysId))
    {
      result = L7_FAILURE;
    }

    hapiSystem->voiceDhcpSysId = BROAD_POLICY_INVALID;
  }
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
                   "\n%s %d: In %s call to 'hapiBroadQosInit'\n",
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

  hapiSystem->mgmtPolicy = BROAD_POLICY_INVALID;
  hapiSystem->mgmtVlanId = 0;

  hapiSystem->sysId1 = BROAD_POLICY_INVALID;
  hapiSystem->sysId2 = BROAD_POLICY_INVALID;
  memset(hapiSystem->dhcpSnoopingPolicyId,0xFF,sizeof(BROAD_POLICY_t)*120*2);     /* PTin modified */
  //hapiSystem->dhcpSnoopingPolicyId[0]   = BROAD_POLICY_INVALID;
  hapiSystem->dhcpDefaultPolicyId[0]    = BROAD_POLICY_INVALID;   // PTin modified
  hapiSystem->dhcpDefaultPolicyId[1]    = BROAD_POLICY_INVALID;   // PTin modified
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

  hapiSystem->voiceCdpSysId  = BROAD_POLICY_INVALID;
  hapiSystem->voiceDhcpSysId = BROAD_POLICY_INVALID;
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
    LOG_ERROR(0);
  }

  /* Start link status task.
  */
  if (osapiTaskCreate("hapiLinkStatusTask",hapiBroadPortLinkStatusTask,0,0, L7_DEFAULT_STACK_SIZE,
                      FD_CNFGR_SIM_DEFAULT_STATS_TASK_PRI,L7_DEFAULT_TASK_SLICE) == L7_ERROR)
  {
    LOG_ERROR(0);
  }
#endif /* L7_PRODUCT_SMARTPATH */

  return result;
}

/*********************************************************************
*
* @purpose Initializes Fabric Only Line card
*
* @returns L7_RC_t result
*
* @notes   This was done for 5675 based Chassis Control Module.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadPhysicalCfmFabricCardInsert(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data, void *handle)
{
  L7_RC_t                 result       = L7_SUCCESS;
  DAPI_USP_t                    usp;
  L7_BOOL                 newBcmUnitInserted = L7_TRUE;
  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

#ifdef L7_STACKING_PACKAGE
  {
    L7_RC_t rc;
    L7_uint32 unit_num;

    /* If physical card is on the local unit then don't attach anything.
    ** The local unit should be already attached when the CPU card is inserted.
    */
    rc = unitMgrMgrNumberGet (&unit_num);
    if ((rc == L7_SUCCESS) &&
        (unit_num == dapiUsp->unit))
    {
    /* If physical card is on the local unit then don't attach anything.
       Do nothing for local units. */
    }
    else
    {
      /* Check if more units need to be attached.
      */
      bcm_stack_attach_fn (bcm_st_cur_db, 0, 0);
    }

  }
#endif
  {
    cpudb_key_t                   cpuKey;
    result = sysapiHpcUnitIdentifierKeyGet(usp.unit, (L7_enetMacAddr_t *)&cpuKey.key);

    if (result != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    /* First call the hapiBroadMapDbEntryGet with known unmatched BCM unit. This forces
     *   ** BCMX attachment of all known units.
     *     */
    {
      int ignore_unit;
      bcmx_lport_t ignore_port;

      hapiBroadMapDbEntryGet (&cpuKey, 0xffff, 0xffff, &ignore_unit, &ignore_port);
    }
  }

 /*
 * Retrieve the Database Info Pointers
 */
  /* suspend all usl_bcmx_*** api calls before syncing */
  usl_bcmx_suspend();

  /* sync the new unit/slot */
  if (usl_card_insert(usp.unit,usp.slot, &newBcmUnitInserted) != L7_SUCCESS)
  {
    result = L7_FAILURE;

    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "\nCard Insertion is incomplete for unit %d, slot %d. Wait for retry.\n",
                   usp.unit, usp.slot);

    /* allow the usl_bcmx_*** calls to happen now */
    usl_bcmx_resume();

#ifdef L7_STACKING_PACKAGE
    /* The card insertion is failing probably because Broadcom discovery algorithm
	 * didn't find all the boxes.
	 *  Restart Broadcom discovery and hope that all will go well this time.
	 */
    hpcBroadRediscover(-1);

#endif

    return result;
  }
  /* allow the usl_bcmx_*** calls to happen now */
  usl_bcmx_resume();

  bcm_stack_attach_fn (bcm_st_cur_db, 0, 0);

  {
    cpudb_key_t                   cpuKey;
    result = sysapiHpcUnitIdentifierKeyGet(usp.unit, (L7_enetMacAddr_t *)&cpuKey.key);

    if (result != L7_SUCCESS)
    {
      return L7_FAILURE;
    }

    /* First call the hapiBroadMapDbEntryGet with known unmatched BCM unit. This forces
     *   ** BCMX attachment of all known units.
     *     */
    {
      int ignore_unit;
      bcmx_lport_t ignore_port;

      hapiBroadMapDbEntryGet (&cpuKey, 0xffff, 0xffff, &ignore_unit, &ignore_port);
    }
  }

#if 0 /* Redo the patch */
  {
    DAPI_t                       *dapi_g = (DAPI_t*)handle;
    L7_uint32 rc;
    L7_uint32 mgmt_unit,this_unit;

   /* Mirroring configuration is not applied to the standby CFM after save and reboot.*/
   /* The stacking ports on stand by CFM have to be mirror enabled if mirrored is configured */
   /* Check if it is standby CFM */
   rc = unitMgrNumberGet(&this_unit);
   if (rc == L7_SUCCESS)
     rc = unitMgrMgrNumberGet (&mgmt_unit);
   if (rc == L7_SUCCESS &&
       (mgmt_unit == this_unit))
   {
     L7_uint32 otherCFMSlot, otherCfmUnit;

     sysapiHpcOtherCfmSlotGet(&otherCFMSlot);
     if (L7_SUCCESS == sysapiHpcSlotMapUSUnitNumGet(otherCFMSlot, &otherCfmUnit))
     {
       if (usp.unit == otherCfmUnit) /* Attaching unit is standby CFM */
       {
         /* Check mirroring configured */
         if (dapi_g->system->mirrorEnable)
         {
           BROAD_PORT_t          *hapiMirrorToPortPtr;

           hapiBroadMirrorEnable(); /* Enable mirroring on stack ports */

           /* Tell any new units in the stack about the probe port.
           */
           hapiMirrorToPortPtr = HAPI_PORT_GET(&dapi_g->system->probeUsp, dapi_g);
           if (hapiMirrorToPortPtr != L7_NULLPTR)
           {
             int rv=0;
             rv = bcmx_mirror_to_set(hapiMirrorToPortPtr->bcmx_lport);
             if (L7_BCMX_OK(rv) != L7_TRUE)
             {
               LOG_ERROR(rv);
             }
           }
         }
       }
     }
   }
  }
#endif


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
  bcmx_lplist_t           tmpLpList;
  BROAD_PORT_t           *hapiPortPtr;
  L7_BOOL                 newBcmUnitInserted = L7_TRUE;

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

#ifdef L7_STACKING_PACKAGE
  
  {
    L7_RC_t rc;
    L7_uint32 unit_num;

    /* Stop the card plugin timer if it is running */
    if (hapiBroadStackCardPluginsFinishTimer != L7_NULL)
    {
      osapiTimerFree(hapiBroadStackCardPluginsFinishTimer);
      hapiBroadStackCardPluginsFinishTimer = L7_NULL;
    }

    /* If physical card is on the local unit then don't attach anything.
    ** The local unit should be already attached when the CPU card is inserted.
    */
    rc = unitMgrMgrNumberGet (&unit_num);
    if ((rc == L7_SUCCESS) &&
        (unit_num == dapiUsp->unit))
    {
      /* Do nothing for local units. */
    } else
    {
      /* Check if more units need to be attached.
      */
      if (hpcBroadNoStackPorts == L7_FALSE)
      {
         bcm_stack_attach_fn (bcm_st_cur_db, 0, 0);
      }
    }

  }
#endif

  /*
   * Retrieve the Database Info Pointers
   */
  do 
  {

    /* suspend all usl_bcmx_*** api calls before syncing */
    usl_bcmx_suspend();

    /* sync the new unit/slot */
    if (usl_card_insert(usp.unit,usp.slot, &newBcmUnitInserted) != L7_SUCCESS)
    {
      result = L7_FAILURE;

      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "\nCard Insertion is incomplete for unit %d, slot %d. Wait for retry.\n",
                     usp.unit, usp.slot);

      /* allow the usl_bcmx_*** calls to happen now */
      usl_bcmx_resume();

#ifdef L7_STACKING_PACKAGE
      /* The card insertion is failing probably because Broadcom discovery algorithm 
      ** didn't find all the boxes.
      ** Restart Broadcom discovery and hope that all will go well this time.
      */
      hpcBroadRediscover(-1);
#endif

      break;
    }

    /* allocate all of the ports on the card */
    if (hapiBroadPortMemAlloc(usp.unit, usp.slot, dapi_g) != L7_SUCCESS)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "%s %d: In %s call to 'hapiBroadPortMemAlloc'\n",
                     __FILE__, __LINE__, __FUNCTION__);

      /* allow the usl_bcmx_*** calls to happen now */
      usl_bcmx_resume();

      break;
    }

    /* Populate the BROAD_PORT_t info from the Card database */
    if (hapiBroadPhysicalPortMapGet(usp.unit, usp.slot, dapi_g) != L7_SUCCESS)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "%s %d: In %s call to 'hapiBroadPhysicalPortMapGet'\n",
                     __FILE__, __LINE__, __FUNCTION__);

      /* allow the usl_bcmx_*** calls to happen now */
      usl_bcmx_resume();

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
    if ((result = usl_lplist_attach(usp.unit,usp.slot,&tmpLpList)) != L7_SUCCESS)
    {
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "\n%s %d: In %s call to 'usl_card_insert'\n",
                     __FILE__, __LINE__, __FUNCTION__);

      /* allow the usl_bcmx_*** calls to happen now */
      usl_bcmx_resume();

      break;
    }

    /* allow the usl_bcmx_*** calls to happen now */
    usl_bcmx_resume();

    /* we must free the list now */
    bcmx_lplist_free(&tmpLpList);

    /* hook in the Function pointers to the ports function table */
    if (hapiBroadSlotCtlInit(usp.unit, usp.slot, dapi_g) != L7_SUCCESS)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "%s %d: In %s call to 'hapiBroadPortCtlInit'\n",
                     __FILE__, __LINE__, __FUNCTION__);
      break;
    }

    /* setup the PHYs and get the initial link state */
    if (hapiBroadPortPhyInit(usp.unit, usp.slot, dapi_g) != L7_SUCCESS)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "%s %d: In %s call to 'hapiBroadPortPhyInit'\n",
                     __FILE__, __LINE__, __FUNCTION__);
      break;
    }

    if (newBcmUnitInserted == L7_TRUE) 
    {
      if (hapiBroadBcmxRegisterUnit(usp.unit, usp.slot, dapi_g) == L7_FAILURE)
      {
        result = L7_FAILURE;
        SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                       "\n%s %d: In %s call to 'hapiBroadDefaultConfigInit'\n",
                       __FILE__, __LINE__, __FUNCTION__);
        break;
      }
    }

    if (hapiBroadDefaultConfigInit(usp.unit, usp.slot, dapi_g) == L7_FAILURE)
    {
	  result = L7_FAILURE;
	  SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
		  		   "\n%s %d: In %s call to 'hapiBroadDefaultConfigInit'\n",
			  	   __FILE__, __LINE__, __FUNCTION__);
	  break;
    }


    /* Initialize the Routing package for all ports on this card */
    if (hapiBroadL3CardInit(usp.unit,usp.slot,dapi_g) == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "\n%s %d: In %s call to 'hapiBroadL3CardInit'\n",
                     __FILE__, __LINE__, __FUNCTION__);
      break;
    }

#ifdef L7_MCAST_PACKAGE
    /* Initialize the IP Mcast package for all ports on this card */
    if (hapiBroadL3McastCardInit(usp.unit,usp.slot,dapi_g) == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "\n%s %d: In %s call to 'hapiBroadL3McastCardInit'\n",
                     __FILE__, __LINE__, __FUNCTION__);
      break;
    }
#endif
#ifdef L7_QOS_PACKAGE
    /* Initialize the QOS package for all ports on this card */
    if (hapiBroadQosCardInit(usp.unit,usp.slot,dapi_g) == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "\n%s %d: In %s call to 'hapiBroadQosCardInit'\n",
                     __FILE__, __LINE__, __FUNCTION__);
      break;
    }
#endif

    /* PTIN: setup the PHYs and get the initial link state */
    if (hapiBroadPtinStart() != L7_SUCCESS)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "%s %d: In %s call to 'hapi_ptin_flow_init'\n",
                     __FILE__, __LINE__, __FUNCTION__);
      break;
    }
  } while (0);
  
  
  /* If the card insert failed then check if the card plugin finish post processing should
  **  be done or wait for another card plugin.
  */
#ifdef L7_STACKING_PACKAGE
  {
    L7_uint32 nextUnit, nextSlot;

    if ((result == L7_FAILURE) && (hapiBroadStackCardPluginsFinishTimer == L7_NULL))
    {
      if (cmgrCardPendingPluginCheck(usp.unit, usp.slot, &nextUnit, &nextSlot) == L7_FALSE)
      {
        hapiBroadStackCardPluginsPostProcess(1, dapi_g);
      }
      else
      {
        /* Start the timer */
        if (hapiBroadStackCardPluginsFinishTimer == L7_NULL)
        {
          osapiTimerAdd((void*)hapiBroadStackCardPluginsFinishTimerProcess, (L7_uint32)dapi_g, 0,
                         HAPI_BROAD_STACK_CARD_PLUGINS_FINISH_TIMER, &hapiBroadStackCardPluginsFinishTimer);
        }
      }

    }
  }
#endif

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

      hapiPortPtr->dhcpSnoopingEnabled         = L7_FALSE;
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

      /* If this port is used for front panel stacking then set a flag.
      */
      if (spmFpsPortStackingModeCheck (usp.unit, usp.slot, usp.port + 1) == L7_TRUE)
      {
        hapiPortPtr->front_panel_stack = L7_TRUE;
      }

      hapiPortPtr->hapiModeparm.physical.phySemaphore = osapiSemaBCreate(OSAPI_SEM_Q_FIFO, OSAPI_SEM_FULL);

      if (hapiPortPtr->hapiModeparm.physical.phySemaphore == L7_NULL)
      {
        LOG_ERROR(usp.port);
      }

      /* Initialize speed and duplex */
      dapiPortPtr = DAPI_PORT_GET(&usp, dapi_g);
      if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_FULL_10G)
      {
        hapiPortPtr->speed  = DAPI_PORT_SPEED_GE_10GBPS;
        hapiPortPtr->duplex = DAPI_PORT_DUPLEX_FULL;
      }
      else if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_FULL_2500)
      {
        hapiPortPtr->speed  = DAPI_PORT_SPEED_GE_2G5BPS;
        hapiPortPtr->duplex = DAPI_PORT_DUPLEX_FULL;
      }
      else if (dapiPortPtr->phyCapabilities & L7_PHY_CAP_PORTSPEED_HALF_2500)
      {
        hapiPortPtr->speed  = DAPI_PORT_SPEED_GE_2G5BPS;
        hapiPortPtr->duplex = DAPI_PORT_DUPLEX_HALF;
      }
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
  int                           rv;

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
        printf("Trying to attach more units.....\n");
        if (bcm_st_cur_db != 0)
        {
          if (hpcBroadNoStackPorts == L7_FALSE)
          {
             bcm_stack_attach_fn (bcm_st_cur_db, 0, 0);
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

      rv = bcmx_lport_to_modid_port (hapiPortPtr->bcmx_lport, &hapiPortPtr->bcm_modid,
                                     &mod_port);

      if (rv != BCM_E_NONE)
      {
        LOG_ERROR (rv);
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
  if (bcm_st_cur_db == 0)
  {
    LOG_ERROR (0);
  }

  /* Check whether there are any active stacking ports */
  if (hpcBroadNoStackPorts == L7_FALSE)
  {
     bcm_stack_attach_fn (bcm_st_cur_db, 1, 0);
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
      LOG_ERROR (rv);
    }

    rv = bcmx_rx_device_add (bcm_unit);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      LOG_ERROR (rv);
    }
  }

  /* Register for L2 notifications */
  if (usl_is_present() == L7_FALSE)
  {
    rv = bcmx_l2_notify_register((bcmx_l2_notify_f) hapiBroadAddrMacUpdate,dapi_g);
    if (L7_BCMX_OK(rv) != L7_TRUE)
    {
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "%s %d: call to 'bcm_l2_addr_register'\n",
                     __FILE__, __LINE__);
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
      SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                     "%s %d: call to 'bcmx_l2_notify_start'\n",
                     __FILE__, __LINE__);
      result = L7_FAILURE;
      return result;
    }
  }

  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: call to 'bcmx_linkscan_enable_set'\n",
                   __FILE__, __LINE__);
    result = L7_FAILURE;
    return result;
  }

  rv = bcmx_linkscan_register(hapiBroadPortLinkStatusChange);
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: call to 'bcmx_linkscan_register'\n",
                   __FILE__, __LINE__);
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
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: call to 'bcmx_rx_register'\n",
                   __FILE__, __LINE__);
    result = L7_FAILURE;
    return result;
  }

  // PTin added
  #if 0
  BCMX_UNIT_ITER(bcm_unit, i)
  {
    if ((rv=bcm_rx_rate_set(bcm_unit,32))!=BCM_E_NONE)
    {
      printf("%s(%d) Error applying rate limiter: error=%d (%s)\r\n",__FUNCTION__,__LINE__,rv,bcm_errmsg(rv));
    }
    else
    {
      printf("%s(%d) Success applying rate limiter\r\n",__FUNCTION__,__LINE__);
    }
  }
  #endif

  // PTin end

  rv = bcmx_rx_start();
  if (L7_BCMX_OK(rv) != L7_TRUE)
  {
    SYSAPI_PRINTF( SYSAPI_LOGGING_HAPI_ERROR,
                   "%s %d: call to 'bcmx_rx_start'\n",
                   __FILE__, __LINE__);
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
        LOG_ERROR(usp.port);
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
  L7_uchar8       exact_match[] = {FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE,
                                   FIELD_MASK_NONE, FIELD_MASK_NONE, FIELD_MASK_NONE};

  hapiSystemPtr = (BROAD_SYSTEM_t *)dapi_g->system->hapiSystem;

  /* If we already have an old MAC address for the network interface
  ** then remove it.
  */
  if (hapiSystemPtr->mgmtPolicy != BROAD_POLICY_INVALID)
  {
    (void)hapiBroadPolicyDelete(hapiSystemPtr->mgmtPolicy);
    hapiSystemPtr->mgmtPolicy = BROAD_POLICY_INVALID;
  }

  /* Create new system mac filter, if specified. */
  if ((0 != new_vlan_id) && (L7_NULLPTR != new_mac_addr))
  {
    BROAD_POLICY_t      mgmtId;
    BROAD_POLICY_RULE_t ruleId;

    hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM, &mgmtId);
    hapiBroadPolicyRuleAdd(mgmtId, &ruleId);
    hapiBroadPolicyRuleQualifierAdd(mgmtId, ruleId, BROAD_FIELD_MACDA, new_mac_addr, exact_match);
    hapiBroadPolicyRuleQualifierAdd(mgmtId, ruleId, BROAD_FIELD_OVID,  (L7_uchar8*)&new_vlan_id, exact_match);
#ifdef BCM_ROBO_SUPPORT
    hapiBroadPolicyRuleActionAdd(mgmtId, ruleId, BROAD_ACTION_SET_REASON_CODE, 
                                      BCM_ROBO_RX_REASON_SWITCHING, 0, 0);
#else
    hapiBroadPolicyRuleActionAdd(mgmtId, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
#endif

#ifdef L7_STACKING_PACKAGE
    hapiBroadPolicyEnableFPS(mgmtId); /* Enable on FPS ports too, if applicable */
#endif

    if (hapiBroadPolicyCommit(mgmtId) == L7_SUCCESS)
      hapiSystemPtr->mgmtPolicy = mgmtId;
  }
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

    hapiBroadPolicyCreate(BROAD_POLICY_TYPE_SYSTEM, &mgmtId);
    hapiBroadPolicyRuleAdd(mgmtId, &ruleId);
    hapiBroadPolicyRuleQualifierAdd(mgmtId, ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *)&cfm_ethtype, exact_match);
    hapiBroadPolicyRuleQualifierAdd(mgmtId, ruleId, BROAD_FIELD_MACDA, new_mac_addr, exact_match);
    hapiBroadPolicyRuleActionAdd(mgmtId, ruleId, BROAD_ACTION_SET_COSQ, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, 0, 0);
    hapiBroadPolicyRuleActionAdd(mgmtId, ruleId, BROAD_ACTION_TRAP_TO_CPU, 0, 0, 0);

    if (hapiBroadPolicyCommit(mgmtId) == L7_SUCCESS)
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
  L7_uint32 mac_count;
  L7_uint32 delay_count = 0;

  /* During the clear-config operation we need to wait until all MAC addresses are
  ** aged out of the device driver table before restarting the code. Otherwise we
  ** may not get some address lear notifications.
  ** This works on stacking and stand-alone builds.
  ** We will not wait more than 30 seconds.
  ** There may be some permanent control MACs in the MAC table, so the table will
  ** probably never reach zero.
  */
  mac_count = hapiBroadLocalMacCountGet();
  while ((mac_count > 16) && (delay_count < 30))
  {
    /* printf("MACs in table = %d, loop = %d\n", mac_count, delay_count); */
    osapiSleep (1);
    delay_count++;
    mac_count = hapiBroadLocalMacCountGet();

  }

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

/*********************************************************************
*
* @purpose Uninitializes fabric Card Types
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadCfmFabricCardRemove(DAPI_USP_t *dapiUsp, DAPI_CMD_t cmd, void *data, void *handle)
{
  L7_RC_t       result = L7_SUCCESS;
#ifdef L7_STACKING_PACKAGE
      bcm_stack_attach_fn (bcm_st_cur_db, 0, 0);
#endif

  usl_card_remove(dapiUsp->unit,dapiUsp->slot);

 return result;

}

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

  usp.unit = dapiUsp->unit;
  usp.slot = dapiUsp->slot;

  switch (dapi_g->unit[usp.unit]->slot[usp.slot]->cardType)
  {

    case SYSAPI_CARD_TYPE_LINE:
      /* Wait for the asynchronous Dot1s operations to finish.
      */
      hapiBroadDot1sAsyncDoneWait ();
      hapiBroadLagAsyncDoneWait ();
      break;

    case SYSAPI_CARD_TYPE_LAG:
       /* Wait for the asynchronous LAG operations to finish.
       */
       hapiBroadLagAsyncDoneWait ();
       break;

    default:
      break;
  }

  for (usp.port = 0; usp.port < dapi_g->unit[usp.unit]->slot[usp.slot]->numOfPortsInSlot; usp.port++)
  {
    hapiPortPtr = HAPI_PORT_GET(&usp,dapi_g);

    if (hapiPortPtr == L7_NULLPTR)
      return result;

    if (BCMX_LPORT_TO_UPORT(hapiPortPtr->bcmx_lport) != _bcmx_uport_invalid)
      bcmx_uport_set(hapiPortPtr->bcmx_lport, _bcmx_uport_invalid);

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
          LOG_ERROR (rc);
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
        hapiBroadPolicyUtilRemove(hapiPortPtr->dot1pPolicy, hapiPortPtr->bcmx_lport);

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
      // PTin removed
      //hapiBroadSystemPolicyRemove(dapi_g);
      break;

    case SYSAPI_CARD_TYPE_LINE:
#ifdef L7_STACKING_PACKAGE
      if (hpcBroadNoStackPorts == L7_FALSE)
      {
         bcm_stack_attach_fn (bcm_st_cur_db, 0, 0);
      }
#endif
      usl_card_remove(usp.unit,usp.slot);
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
#ifdef INCLUDE_L3
  int rv;
#endif
  L7_uchar8 mac[6];

  usp.unit = unit;
  usp.slot = slot;

  if(IS_SLOT_TYPE_PHYSICAL(&usp, dapi_g))
  {
    /* Set the flow control state since it is a global */
    memcpy (mac, ((BROAD_SYSTEM_t*)dapi_g->system->hapiSystem)->bridgeMacAddr.addr, sizeof(mac));
    usl_bcmx_flow_control_set(unit,dapi_g->system->flowControlEnable,mac);
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
 /* Install the Mcast filter for MRPs if IGMP enabled */
  if (dapi_g->system->igmpEnable == L7_TRUE)
  {
    hapiBroadConfigL3McastFilter(L7_TRUE);
  }
#endif

#ifdef INCLUDE_L3
  /* If ip multicast is not enabled, then start with IPMC disabled.
   * SDK enables IPMC by default
   */
  if (dapi_g->system->mcastEnable == L7_TRUE)
  {
    rv = bcmx_ipmc_enable(L7_TRUE);
  }
  else
  {
    rv = bcmx_ipmc_enable(L7_FALSE);
  }
  if (L7_BCMX_OK(rv) != L7_TRUE)
        return L7_FAILURE;
#endif

  /* Send the IGMP filter if igmp snooping is configured */
  if (dapi_g->system->igmpSnoopingEnable)
    hapiBroadConfigIgmpFilter(L7_TRUE, 0, HAPI_BROAD_INGRESS_MED_PRIORITY_COS, L7_FALSE,&usp,dapi_g);

#ifndef FEAT_METRO_CPE_V1_0
  /* Set the MLD filter if either l3 mld or mld snooping is configured */
  if (dapi_g->system->mldSnoopingEnable || dapi_g->system->mldEnable)
    hapiBroadConfigMldFilter(L7_TRUE,dapi_g);
#endif 

  hapiBroadSwitchConfigDos(dapi_g);

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
    LOG_ERROR(0);
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
  DAPI_CARD_MGMT_CMD_t       *dapiCmd = (DAPI_CARD_MGMT_CMD_t*)data;
  L7_RC_t                     result = L7_SUCCESS;

#ifdef L7_STACKING_PACKAGE
  BROAD_PORT_t               *hapiPortPtr;
  L7_uint32                   nextUnit, nextSlot;
  unitMgrStatus_t             unitStatus;  
  L7_BOOL                     cacheInit = L7_TRUE;


  result = unitMgrUnitStatusGet(usp->unit, &unitStatus);
  if ((result != L7_SUCCESS) || (unitStatus != L7_UNITMGR_UNIT_OK) ||
      (usl_unit_sync_check(usp->unit) == L7_FALSE))
  {
    return L7_SUCCESS;
  }


  /* Card Plug-in to start, prepare the cache to store the card configuration */
  if (dapiCmd->cardCmd == CARD_CMD_PLUGIN_START) 
  {
    (void)hapiBroadVlanCritSecEnter();
    (void)dapiCardConfigWriteLockTake(usp->unit, usp->slot);

    /* Stop the stack card plugin timer if it is running as a new card is to be plugged-in */
    if (hapiBroadStackCardPluginsFinishTimer != L7_NULL)
    {
      osapiTimerFree(hapiBroadStackCardPluginsFinishTimer);
      hapiBroadStackCardPluginsFinishTimer = L7_NULL;
    }

    /* Clear the cache */
    (void)l7_usl_port_cache_clear();

    /* Make an entry in the cache for all the ports in the card */
    for (usp->port=0; usp->port < dapi_g->unit[usp->unit]->slot[usp->slot]->numOfPortsInSlot; usp->port++)
    {
      if (isValidUsp (usp, dapi_g) != L7_TRUE)
      {
        continue;
      }
          
      hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
      result = l7_usl_port_cache_entry_setup(usp->unit, usp->slot, hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port);
      if (result != L7_SUCCESS) 
      {
        cacheInit = L7_FALSE;
        /* Clear the cache */
       (void)l7_usl_port_cache_clear();
        break;
      }
    }

    if (cacheInit == L7_TRUE)
    {
    /* Set the cachePortConfig flag so that all the port configuration will be cached */
    dapi_g->unit[usp->unit]->slot[usp->slot]->cachePortConfig = L7_TRUE;
    }

    (void)dapiCardConfigWriteLockGive(usp->unit, usp->slot);
    (void)hapiBroadVlanCritSecExit();

    /*
    ** Replay configurations that are system based in our application
    ** but are configured on a per port basis in the technology
    ** Namely: Flow Control, Broadcast Storm, Route Forwarding,
    **         Multicast Route Forwarding
    */
    if (hapiBroadApplyConfig(usp->unit,usp->slot,dapi_g) == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "\n%s %d: In %s call to 'hapiBroadApplyConfig'\n",
                     __FILE__, __LINE__, __FUNCTION__);
    }
  }
  else if (dapiCmd->cardCmd == CARD_CMD_PLUGIN_FINISH) 
  {
    if (dapi_g->unit[usp->unit]->slot[usp->slot]->cachePortConfig == L7_TRUE)
    {
      (void)hapiBroadVlanCritSecEnter();
      (void)dapiCardConfigWriteLockTake(usp->unit, usp->slot);

      /* Commit the cached configuration */
      l7_usl_port_cache_commit(usp->unit, usp->slot);
      (void)l7_usl_port_cache_clear();
      dapi_g->unit[usp->unit]->slot[usp->slot]->cachePortConfig = L7_FALSE;

      /* Mark that the port on this card are not enabled yet */
      dapi_g->unit[usp->unit]->slot[usp->slot]->pendingAdminModeConfig = L7_TRUE;

      (void)dapiCardConfigWriteLockGive(usp->unit, usp->slot);
      (void)hapiBroadVlanCritSecExit();
    }

    /* Check if there are more cards to be plugged-in */
    if (cmgrCardPendingPluginCheck(usp->unit, usp->slot, &nextUnit, &nextSlot) == L7_FALSE)
    {
      hapiBroadStackCardPluginsPostProcess(1, dapi_g);
    }
    else
    {
      /* Start the timer */
      if (hapiBroadStackCardPluginsFinishTimer == L7_NULL)
      {
        osapiTimerAdd((void*)hapiBroadStackCardPluginsFinishTimerProcess, (L7_uint32)dapi_g, 0,
                       HAPI_BROAD_STACK_CARD_PLUGINS_FINISH_TIMER, &hapiBroadStackCardPluginsFinishTimer);
      }
    }
  }

#else /* Stand-alone */

  if (dapiCmd->cardCmd == CARD_CMD_PLUGIN_START) 
  {
    /*
    ** Replay configurations that are system based in our application
    ** but are configured on a per port basis in the technology
    ** Namely: Flow Control, Broadcast Storm, Route Forwarding,
    **         Multicast Route Forwarding
    */
    if (hapiBroadApplyConfig(usp->unit,usp->slot,dapi_g) == L7_FAILURE)
    {
      result = L7_FAILURE;
      SYSAPI_PRINTF( SYSAPI_LOGGING_DAPI_ERROR,
                     "\n%s %d: In %s call to 'hapiBroadApplyConfig'\n",
                     __FILE__, __LINE__, __FUNCTION__);
    }
  }
#endif

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
  L7_RC_t                  result = L7_SUCCESS;

#ifdef L7_STACKING_PACKAGE

  DAPI_CARD_MGMT_CMD_t    *dapiCmd = (DAPI_CARD_MGMT_CMD_t*)data;
  BROAD_PORT_t            *hapiPortPtr;
  unitMgrStatus_t          unitStatus;  
  L7_BOOL                  cacheInit = L7_TRUE;

  result = unitMgrUnitStatusGet(usp->unit, &unitStatus);
  if ((result != L7_SUCCESS) || (unitStatus != L7_UNITMGR_UNIT_OK) ||
      (usl_unit_sync_check(usp->unit) == L7_FALSE))
  {
    return L7_SUCCESS;
  }

  /* Stop the card plugin timer if it is running as a card is being un-plugged 
  ** before the system init is complete. Take the safe approach here and do the post 
  ** plugin processing for the cards seen so far.
  */
  if (hapiBroadStackCardPluginsFinishTimer != L7_NULL)
  {
    osapiTimerFree(hapiBroadStackCardPluginsFinishTimer);
    hapiBroadStackCardPluginsFinishTimer = L7_NULL;

    hapiBroadStackCardPluginsPostProcess(1, dapi_g);
  }

  (void)hapiBroadVlanCritSecEnter();
  (void)dapiCardConfigWriteLockTake(usp->unit, usp->slot);

  /* Un-Plugin to start, prepare the cache to store the card configuration */
  if (dapiCmd->cardCmd == CARD_CMD_UNPLUG_START) 
  {
    
    /* Disable all the ports in the card */
    hapiBroadSystemCardPortsAdminModeSet(usp->unit, usp->slot, L7_TRUE, L7_FALSE, dapi_g);

    (void)l7_usl_port_cache_clear();

    for (usp->port=0; usp->port < dapi_g->unit[usp->unit]->slot[usp->slot]->numOfPortsInSlot; usp->port++)
    {
      if (isValidUsp (usp, dapi_g) != L7_TRUE)
      {
        continue;
      }
          
      hapiPortPtr = HAPI_PORT_GET(usp, dapi_g);
      result = l7_usl_port_cache_entry_setup(usp->unit, usp->slot, hapiPortPtr->bcm_unit, hapiPortPtr->bcm_port);
      if (result != L7_SUCCESS) 
      {
        cacheInit = L7_FALSE;
        /* Clear the cache */
       (void)l7_usl_port_cache_clear();
        break;
      }
    }

    if (cacheInit == L7_TRUE)
    {
      /* Set the cachePortConfig flag so that all the port configuration will be cached */
      dapi_g->unit[usp->unit]->slot[usp->slot]->cachePortConfig = L7_TRUE;
    }
  }
  else if (dapiCmd->cardCmd == CARD_CMD_UNPLUG_FINISH) 
  {
    if (dapi_g->unit[usp->unit]->slot[usp->slot]->cachePortConfig == L7_TRUE)
    {
    /* Commit the cached configuration */
    l7_usl_port_cache_commit(usp->unit, usp->slot);
    (void)l7_usl_port_cache_clear();
     dapi_g->unit[usp->unit]->slot[usp->slot]->cachePortConfig = L7_FALSE;
  }
  }

  (void)dapiCardConfigWriteLockGive(usp->unit, usp->slot);
  (void)hapiBroadVlanCritSecExit();

#else /* Stand-alone */

#endif

  return result;
}


