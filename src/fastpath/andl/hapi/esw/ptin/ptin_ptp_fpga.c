/*
* Description: 
*
*
* rcosta@ptinovacao.pt
*
* Copyright: See COPYING file that comes with this distribution
*
*/
//http://wiki.ptin.corppt.com/pages/viewpage.action?pageId=730693695
//(Ata de 2015-11-27 - OLT1T0 - Y.1731 (CCMs) & IEEE1588)

#include "ptin_globaldefs.h"
#include "logger.h"
#include "ptin_hapi.h"
#include "l7_common.h"
#include "ptin_utils.h"

#include "ptin_ptp_fpga.h"

#include "broad_policy.h"
#include "broad_group_bcm.h"




//C O M M O N************************************************************************************
//DTL/APP LAYER**********************************************************************************
//#include <my_types.h>
#define N_FPGA_VIDPRTS  16
#include <table.h>

#if (PTIN_BOARD_IS_STANDALONE)
 #if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
  #define VIDpcpu PTIN_VLAN_FPGA2CPU
  #define EVCpcpu PTIN_EVC_FPGA2CPU
 #endif
 #define VIDprt(prt)     (PTIN_VLAN_FPGA2PORT_MIN+(prt))      /*0<=prt<PTIN_SYSTEM_N_INTERF*/
 #define EVCprt(prt)     (PTIN_EVC_FPGA2PORTS_MIN+(prt))
#endif

//#include <ptin_xlate_api.h>
extern L7_RC_t ptin_xlate_egress_set( L7_uint port, L7_uint16 outer_vlan, L7_uint op, L7_uint16 newOuterVlanId);
//#include <ptin_evc.h>
extern L7_RC_t ptin_evc_create(ptin_HwEthMef10Evc_t *evcConf);
extern L7_RC_t ptin_evc_delete(L7_uint32 evc_ext_id);

void ptin_flows_fpga_init(void) {
#if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
  L7_RC_t rc;
  ptin_HwEthMef10Evc_t evcConf;

  /* Create CPU-FPGA circuit */
  memset(&evcConf, 0x00, sizeof(evcConf));
  evcConf.index         = EVCpcpu;
  evcConf.flags         = PTIN_EVC_MASK_MACLEARNING;
  evcConf.mc_flood      = PTIN_EVC_MC_FLOOD_ALL;
  evcConf.internal_vlan = PTIN_VLAN_FPGA2CPU;
  evcConf.n_intf        = 2;
  /* Root port */
  evcConf.intf[0].intf.format = PTIN_INTF_FORMAT_PORT;
  evcConf.intf[0].intf.value.ptin_port = PTIN_PORT_CPU;
  evcConf.intf[0].mef_type    = PTIN_EVC_INTF_ROOT;
  evcConf.intf[0].vid         = 0;
  evcConf.intf[0].action_outer= PTIN_XLATE_ACTION_NONE;
  evcConf.intf[0].action_inner= PTIN_XLATE_ACTION_NONE;
  /* Leaf ports */
  evcConf.intf[1].intf.format = PTIN_INTF_FORMAT_PORT;
  evcConf.intf[1].intf.value.ptin_port = PTIN_PORT_FPGA;
  evcConf.intf[1].mef_type    = PTIN_EVC_INTF_LEAF;
  evcConf.intf[1].vid         = 0;
  evcConf.intf[1].action_outer= PTIN_XLATE_ACTION_NONE;
  evcConf.intf[1].action_inner= PTIN_XLATE_ACTION_NONE;
  /* Create circuit */
  rc = ptin_evc_create(&evcConf);
  if (rc != L7_SUCCESS) {
    PT_LOG_ERR(LOG_CTX_API, "Error creating EVC# %u connecting CPU-FPGA", EVCpcpu);
    return;
  }

  //Remove VID(cpu) @ CPU port's egress
  rc = ptin_xlate_egress_set(PTIN_PORT_CPU, VIDpcpu, PTIN_XLATE_ACTION_DELETE, -1);
  if (rc != L7_SUCCESS) {
    PT_LOG_ERR(LOG_CTX_API, "Error defining pop xlate action for EVC# %u (CPU-FPGA): rc=%d", EVCpcpu, rc);
    return;
  }
#endif
}//ptin_flows_fpga_init

















#if (PTIN_BOARD_IS_STANDALONE)
typedef
struct {
 unsigned long  prt,
                vid_prt;
} T_PRT_VID;
typedef_TABLE_TEMPLATE(T_PRT_VID, N_FPGA_VIDPRTS, T_PRT_VID_TABLE)

static T_PRT_VID_TABLE prt_vid_table;




static unsigned long ptin_flow_fpga_entry(unsigned char init0_add1_del2_prt2VIDprt3, unsigned long prt) {
//init0 - returns 0
//add1 - returns 0 iff OK (adds to DB)
//del2 - returns 0 iff OK (dels from DB)
unsigned long vid_prt=VIDprt(prt);

static unsigned char _1st_time=1;
unsigned long i;
L7_RC_t rc, rc2;
T_PRT_VID ent, *p;
ptin_HwEthMef10Evc_t evcConf;


 if (_1st_time || 0==init0_add1_del2_prt2VIDprt3) {
     _1st_time=0;
     PT_LOG_INFO(LOG_CTX_API, "init_table()=%d",
                init_table(&prt_vid_table, sizeof(T_PRT_VID), N_FPGA_VIDPRTS)
                );
     if (0==init0_add1_del2_prt2VIDprt3) return 0;
 }//if (_1st_time || 0==init0_add1_del2_prt2VIDprt3)


 if (prt>=PTIN_SYSTEM_N_INTERF) return -1;


 switch (init0_add1_del2_prt2VIDprt3) {
 //case 0:
 default: return -2;
 case 1:
     {
        ent.prt=prt;
        ent.vid_prt=vid_prt;
        i=add_entry(&prt_vid_table, &ent, sizeof(ent), sizeof(ent.prt), N_FPGA_VIDPRTS, 0, 1, 1);
        if (i>=N_FPGA_VIDPRTS) return -3;
        if (1==index2n_used(&prt_vid_table, i, N_FPGA_VIDPRTS)) {//1st entity using this port
            memset(&evcConf, 0x00, sizeof(evcConf)); 
            evcConf.index         = EVCprt(prt);
            evcConf.flags         = PTIN_EVC_MASK_MACLEARNING;
            evcConf.mc_flood      = PTIN_EVC_MC_FLOOD_ALL;
            evcConf.internal_vlan = VIDprt(prt);
            evcConf.n_intf        = 2;
            /* Root port */
            evcConf.intf[0].intf.format = PTIN_INTF_FORMAT_PORT;
            evcConf.intf[0].intf.value.ptin_port = prt;
            evcConf.intf[0].mef_type    = PTIN_EVC_INTF_ROOT;
            evcConf.intf[0].vid         = 0;
            evcConf.intf[0].action_outer= PTIN_XLATE_ACTION_NONE;
            evcConf.intf[0].action_inner= PTIN_XLATE_ACTION_NONE;
            /* Leaf ports */
            evcConf.intf[1].intf.format = PTIN_INTF_FORMAT_PORT;
            evcConf.intf[1].intf.value.ptin_port = PTIN_PORT_FPGA;
            evcConf.intf[1].mef_type    = PTIN_EVC_INTF_LEAF;
            evcConf.intf[1].vid         = 0;
            evcConf.intf[1].action_outer= PTIN_XLATE_ACTION_NONE;
            evcConf.intf[1].action_inner= PTIN_XLATE_ACTION_NONE;
            /* Create circuit */
            rc = ptin_evc_create(&evcConf);
            if (rc != L7_SUCCESS) {
              PT_LOG_ERR(LOG_CTX_API, "Error creating EVC# %u for connecting Port %u to FPGA", EVCprt(prt), prt);
              //return rc;
            }

            //Remove VID(po) (=VID(pi)) @ PO/PI port's egress
            rc2 = ptin_xlate_egress_set(prt, VIDprt(prt), PTIN_XLATE_ACTION_DELETE, -1);
            if (rc2 != L7_SUCCESS) {
              PT_LOG_ERR(LOG_CTX_API, "Error defining pop xlate action for EVC# %u (CPU-FPGA): rc=%d", EVCprt(prt), rc);
              //return rc2;
            }
            if (rc!=L7_SUCCESS || rc2!=L7_SUCCESS) {
                //del_entry(&prt_vid_table, NULL, sizeof(ent), sizeof(ent.prt), N_FPGA_VIDPRTS, i);
                ////del_entry(&prt_vid_table, &ent, sizeof(ent), sizeof(ent.prt), N_FPGA_VIDPRTS, i);
                return -4;
            }
        }//if (1==index2n_used(&prt_vid_table, i, N_FPGA_VIDPRTS))      //1st entity using this port
     }
     break;
 case 2:
     {
        ent.prt=prt;
        i=find_entry(&prt_vid_table, &ent, sizeof(ent), sizeof(ent.prt), N_FPGA_VIDPRTS, 0, NULL);
        //i=del_entry(&prt_vid_table, &ent, sizeof(ent), sizeof(ent.prt), N_FPGA_VIDPRTS, 0);
        if (i>=N_FPGA_VIDPRTS) {
            PT_LOG_ERR(LOG_CTX_API, "del_entry(prt=%lu)=%lu", ent.prt, i);
            return -5;
        }
        p= (T_PRT_VID *) pointer2table_index(&prt_vid_table, i, N_FPGA_VIDPRTS, sizeof(ent));
        vid_prt=p->vid_prt;
        del_entry(&prt_vid_table, NULL, sizeof(ent), sizeof(ent.prt), N_FPGA_VIDPRTS, i);
        if (0==index2n_used(&prt_vid_table, i, N_FPGA_VIDPRTS)) {   //last entity leaving
            rc = ptin_evc_delete(EVCprt(prt));
            if (rc != L7_SUCCESS) {
              PT_LOG_ERR(LOG_CTX_API, "Error deleting EVC# %u for connecting Port %u to FPGA", EVCprt(prt), prt);
              return -6;
            }
        }//if (0==index2n_used(&prt_vid_table, i, N_FPGA_VIDPRTS))  //last entity leaving
     }
     break;
 case 3:
     ent.prt=prt;
     i=find_entry(&prt_vid_table, &ent, sizeof(ent), sizeof(ent.prt), N_FPGA_VIDPRTS, 0, NULL);
     if (i>=N_FPGA_VIDPRTS) return -7;
     p= (T_PRT_VID *) pointer2table_index(&prt_vid_table, i, N_FPGA_VIDPRTS, sizeof(ent));
     return p->vid_prt;
     //break;
 case 10:
     printf("Dumping PRT_VID_TABLE (size %u)...\n\r", N_FPGA_VIDPRTS);
     for (i=0; i<N_FPGA_VIDPRTS; i++) {
         if (entry_is_empty(&prt_vid_table, i, N_FPGA_VIDPRTS, sizeof(*p))) continue;
         p = (T_PRT_VID *) pointer2table_index(&prt_vid_table, i, N_FPGA_VIDPRTS, sizeof(*p));
         printf("%lu:\tprt=%lu\tvid_prt=%lu\n\r", i, p->prt, p->vid_prt);
     }
     break;
 }//switch

 return 0;
}//ptin_flow_fpga_entry




L7_RC_t ptin_ptp_oam_prtvid_dump(void) {
 ptin_flow_fpga_entry(10, 0);
 return L7_SUCCESS;
}
#endif








































//P T P / 1588***********************************************************************************
//ANDL/HAPI LAYER********************************************************************************
typedef struct {
    ptin_dtl_search_ptp_t e;

    BROAD_POLICY_t policyId;
} __attribute__((packed)) ptin_hapi_search_ptp_t;




#define N_SEARCH_PTP    128




typedef_TABLE_TEMPLATE(ptin_hapi_search_ptp_t, N_SEARCH_PTP, T_SEARCH_PTP_TABLE)



T_SEARCH_PTP_TABLE search_PTP_table;













void ptin_hapi_ptp_table_init(void) {
    PT_LOG_INFO(LOG_CTX_HAPI, "init_table()=%d",
                init_table(&search_PTP_table, sizeof(ptin_hapi_search_ptp_t), N_SEARCH_PTP)
                );
}











L7_RC_t ptin_hapi_ptp_entry_add(ptin_dapi_port_t *dapiPort, ptin_dtl_search_ptp_t *entry)
{
#if (!PTIN_BOARD_IS_STANDALONE)
 return L7_FAILURE;
#else
  //L7_int      /*entry, free_entry,*/ rule, free_rule, max_rules;
  ptin_hapi_search_ptp_t tbl_entry, *p;
  //L7_uchar8 exact_mask[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  BROAD_POLICY_t      policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t ruleId;
  T_index _1st_free, i;
  L7_RC_t rc = L7_SUCCESS;
  L7_uint32    bcm_unit, bcm_port;
  bcm_gport_t  gport;
  extern DAPI_t *dapi_g;

  switch (entry->encap) {
  default:
      PT_LOG_ERR(LOG_CTX_HAPI, "encap=%d", entry->encap);
      return L7_ERROR;
  //case TS_ENCAP_ETH_PTP:
  //    PT_LOG_TRACE(LOG_CTX_HAPI, "prt=%u, VID=%u, VID_PRT=%u, encap=%d, MAC=%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
  //                 entry->key.prt, entry->key.vid, entry->vid_prt, entry->encap,
  //                 entry->ntw.DMAC[0], entry->ntw.DMAC[1], entry->ntw.DMAC[2], entry->ntw.DMAC[3], entry->ntw.DMAC[4], entry->ntw.DMAC[5]);
  //    break;
  case TS_ENCAP_ETH_IPv4_PTP:
      PT_LOG_TRACE(LOG_CTX_HAPI, "prt=%u, VID=%u, VID_PRT=%u, encap=%d, dIP=%u.%u.%u.%u",
                   entry->key.prt, entry->key.vid, entry->vid_prt, entry->encap,
                   entry->ntw.dIP[0], entry->ntw.dIP[1], entry->ntw.dIP[2], entry->ntw.dIP[3]);
      break;
  //case TS_ENCAP_ETH_IPv6_PTP:
  //    PT_LOG_TRACE(LOG_CTX_HAPI, "prt=%u, VID=%u, VID_PRT=%u, encap=%d, dIP=%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x",
  //                 entry->key.prt, entry->key.vid, entry->vid_prt, entry->encap,
  //                 entry->ntw.dIP[0], entry->ntw.dIP[1], entry->ntw.dIP[2], entry->ntw.dIP[3],
  //                 entry->ntw.dIP[4], entry->ntw.dIP[5], entry->ntw.dIP[6], entry->ntw.dIP[7],
  //                 entry->ntw.dIP[8], entry->ntw.dIP[9], entry->ntw.dIP[10], entry->ntw.dIP[11],
  //                 entry->ntw.dIP[12], entry->ntw.dIP[13], entry->ntw.dIP[14], entry->ntw.dIP[15]);
  //    break;
  }//switch (entry->encap)




  tbl_entry.e = *entry; //tbl_entry.e.key = entry->key;
  i = find_entry(&search_PTP_table, &tbl_entry, sizeof(tbl_entry), sizeof(entry->key), N_SEARCH_PTP, -1, &_1st_free);
  PT_LOG_TRACE(LOG_CTX_HAPI, "i=%u\t_1st_free=%u\t(N_SEARCH_PTP=%u)", i, _1st_free, N_SEARCH_PTP);

  if (i<N_SEARCH_PTP) { //Entry found
      p = (ptin_hapi_search_ptp_t*) pointer2table_index(&search_PTP_table, i, N_SEARCH_PTP, sizeof(tbl_entry));
      if (!memcmp(entry, &p->e, sizeof(p->e))) return L7_SUCCESS;  //Nothing to do; same entry

      //Delete entry
      PT_LOG_TRACE(LOG_CTX_HAPI, "Deleting entry...");
      rc = hapiBroadPolicyDelete(p->policyId);
      if (rc != L7_SUCCESS) {PT_LOG_ERR(LOG_CTX_HAPI, "Cannot hapiBroadPolicyDelete()");}
      del_entry(&search_PTP_table, NULL, sizeof(tbl_entry), sizeof(entry->key), N_SEARCH_PTP, i);
  }
  else
  if (_1st_free>=N_SEARCH_PTP) return L7_TABLE_IS_FULL; //No entries left
  else
  i=_1st_free;          //Entry not found; using 1st free



  // Configure rule
  do
  {
    rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_IPSG);
    if (rc != L7_SUCCESS) {PT_LOG_ERR(LOG_CTX_HAPI, "Cannot create policy"); break;}


    //VCAP
    rc = hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_LOOKUP);
    if (rc != L7_SUCCESS) {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error creating a lookup policy");
      hapiBroadPolicyCreateCancel();
      break;
    }


    // Create rule
    rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
    if (rc != L7_SUCCESS) {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error adding rule");
      hapiBroadPolicyCreateCancel();
      break;
    }


    //Qualifiers
    //rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_INPORTS, (L7_uchar8 *)&pbm, (L7_uchar8 *)&pbm_mask);
    //if (rc != L7_SUCCESS) {
    //  PT_LOG_ERR(LOG_CTX_HAPI, "Error adding INPORTS qualifier");
    //  hapiBroadPolicyCreateCancel();
    //  break;
    //}
    //PT_LOG_TRACE(LOG_CTX_HAPI,"INPORTS qualifier added");

    {
     L7_uint16   vlan_mask = 0xfff;

        rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&entry->key.vid, (L7_uint8 *) &vlan_mask);
        if (rc != L7_SUCCESS) {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error adding OVID qualifier (%u)", entry->key.vid);
          hapiBroadPolicyCreateCancel();
          break;
        }
        PT_LOG_TRACE(LOG_CTX_HAPI,"OVID %u/0x%x qualifier added", entry->key.vid, vlan_mask);
    }


#if 1
    //Not only PTPoIP but also ICMPoIP and ARP must go through
    //Impossible to filter PTPoIP, ICMPoIP and ARP in a single rule (would imply 2x2x...; economy => skip it)
    //hopefully the OVID (above) and this will exclude most other protocols' interception/black holing
    {
     L7_ushort16 ethtype, msk;

        switch (entry->encap) {
        case TS_ENCAP_ETH_IPv4_PTP:
            msk = (L7_ushort16) ~(L7_ETYPE_IP ^ L7_ETYPE_ARP);
            ethtype = L7_ETYPE_IP & L7_ETYPE_ARP & msk;
            break;
        case TS_ENCAP_ETH_IPv6_PTP:
            msk = (L7_ushort16) ~(L7_ETYPE_IPV6 ^ L7_ETYPE_ARP);
            ethtype = L7_ETYPE_IPV6 & L7_ETYPE_ARP & msk;
            break;
        }//switch
            
        rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *) &ethtype, (L7_uchar8 *) &msk);
        if (rc != L7_SUCCESS) {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error adding ETHTYPE qualifier (0x%04x/0x%04x)", ethtype, msk);
          hapiBroadPolicyCreateCancel();
          break;
        }
        PT_LOG_TRACE(LOG_CTX_HAPI,"ETHTYPE 0x%04x/0x%04x qualifier added", ethtype, msk);
    }


#else   //PTPoIP or XoIP for a certain dIP
    {
     L7_uchar8 ipv = BROAD_IP_TYPE_IPV4;

        rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IP_TYPE, &ipv, exact_mask);
        if (rc != L7_SUCCESS) {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error adding IP_TYPE qualifier (%u)", ipv);
          hapiBroadPolicyCreateCancel();
          break;
        }
        PT_LOG_TRACE(LOG_CTX_HAPI,"IP_TYPE %u qualifier added", ipv);
    }

    {
     unsigned long ip4;
        if (TS_ENCAP_ETH_IPv6_PTP==entry->encap) rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IP6_DST, entry->ntw.dIP, exact_mask);
        else {
            ip4 = entry->ntw.dIP[0]; ip4<<=8;
            ip4|= entry->ntw.dIP[1]; ip4<<=8;
            ip4|= entry->ntw.dIP[2]; ip4<<=8;
            ip4|= entry->ntw.dIP[3];
            rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DIP, (L7_uchar8 *) &ip4, exact_mask);
        }
        if (rc != L7_SUCCESS) {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error adding DIP qualifier");
          hapiBroadPolicyCreateCancel();
          break;
        }
        PT_LOG_TRACE(LOG_CTX_HAPI,"DIP qualifier added");
    }


    //ICMP is to pass => cannot use this qualifier maintaining a single rule; economy => skip it
    //{
    // L7_uchar8 udp_proto = 0x11;
    //
    //    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_PROTO, &udp_proto, exact_mask);
    //    if (rc != L7_SUCCESS) {
    //      PT_LOG_ERR(LOG_CTX_HAPI, "Error adding UDP PROTO qualifier (%u)", udp_proto);
    //      hapiBroadPolicyCreateCancel();
    //      break;
    //    }
    //    PT_LOG_TRACE(LOG_CTX_HAPI,"UDP PROTO %u qualifier added", udp_proto);
    //}

    //{
    // L7_ushort16 dport;
    //
    //    //Using port implies adding both 319 and 320 (2x rules); economy => skip it
    //    dport = UDP_PORT_EVENT_PTP;//UDP_PORT_GENERAL_PTP;
    //    rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_DPORT, (L7_uchar8*)&dport, exact_mask);
    //    if (rc != L7_SUCCESS) {
    //      PT_LOG_ERR(LOG_CTX_HAPI, "Error adding DPORT qualifier (%u)", dport);
    //      hapiBroadPolicyCreateCancel();
    //      break;
    //    }
    //    PT_LOG_TRACE(LOG_CTX_HAPI,"DPORT %u qualifier added", dport);
    //}
#endif

    //Actions
    if (hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_ADD_OUTER_VID, entry->vid_prt, 0, 0) != L7_SUCCESS)// ||
        //hapiBroadPolicyRuleExceedActionAdd(ruleId, BROAD_ACTION_ADD_OUTER_VID, entry->int_priority, 0, 0) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error adding ADD_OUTER_VID action");
      hapiBroadPolicyCreateCancel();
      break;
    }
    PT_LOG_TRACE(LOG_CTX_HAPI,"ADD_OUTER_VID action added");


    // Apply policy
    if ((rc=hapiBroadPolicyCommit(&policyId)) != L7_SUCCESS) {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error commiting trap policy");
      hapiBroadPolicyCreateCancel();
      break;
    }

    /* Get bcm data from physical USP port */
    if (hapi_ptin_get_bcmdata_from_uspport(entry->key.prt, dapi_g,
                                           &bcm_unit, &bcm_unit, L7_NULLPT) != L7_SUCCESS)
    {
        PT_LOG_ERR(LOG_CTX_HAPI, "Can't get bcm data from usp_port %U", entry->key.prt);
        return L7_FAILURE;
    }

    if (bcmy_lut_unit_port_to_gport_get(bcm_unit, bcm_port, &gport) != BCMY_E_NONE)
    {
      printf("Error with unit %d, port %d", bcm_unit, bcm_port);
      return L7_FAILURE;
    }
    if ((rc=hapiBroadPolicyApplyToIface(policyId, gport)) != L7_SUCCESS) {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error applying to gport %u", gport);
      break;
      //hapiBroadPolicyDelete(policyId);
      //return L7_FAILURE;
    }

    PT_LOG_TRACE(LOG_CTX_HAPI, "Trap policy commited successfully (policyId=%u)", policyId);
  } while (0);

  /* Have occurred any error? */
  if (rc != L7_SUCCESS) {
    /* Delete rule */
    if (policyId != BROAD_POLICY_INVALID) {
      PT_LOG_TRACE(LOG_CTX_HAPI, "Removing policyId %u", policyId);
      if (L7_SUCCESS != hapiBroadPolicyDelete(policyId)) {PT_LOG_ERR(LOG_CTX_HAPI, "Cannot hapiBroadPolicyDelete()");}
    }
    PT_LOG_ERR(LOG_CTX_HAPI, "Error while creating VCAP rule");
    return rc;
  }

  //PT_LOG_TRACE(LOG_CTX_HAPI,"VCAP rule configured");

  // Save entry
  tbl_entry.policyId = policyId;
  i = add_entry(&search_PTP_table, &tbl_entry, sizeof(tbl_entry), sizeof(entry->key), N_SEARCH_PTP, i, 0, 0);
  //wr_entry(&search_PTP_table, &tbl_entry, sizeof(tbl_entry), N_SEARCH_PTP, i);

  PT_LOG_TRACE(LOG_CTX_HAPI,"add_entry()=%u\t(N_SEARCH_PTP=%u)", i, N_SEARCH_PTP);
  //PT_LOG_TRACE(LOG_CTX_HAPI,"Operation finished successfully");

  return L7_SUCCESS;
#endif
}//ptin_hapi_ptp_entry_add





















L7_RC_t ptin_hapi_ptp_entry_del(ptin_dapi_port_t *dapiPort, ptin_dtl_search_ptp_t *entry) {
#if (!PTIN_BOARD_IS_STANDALONE)
 return L7_FAILURE;
#else

ptin_hapi_search_ptp_t tbl_entry, *p;
T_index i;
L7_RC_t rc = L7_SUCCESS;

    tbl_entry.e = *entry; //tbl_entry.e.key = entry->key;
    i = find_entry(&search_PTP_table, &tbl_entry, sizeof(tbl_entry), sizeof(entry->key), N_SEARCH_PTP, -1, NULL);
    if (i>=N_SEARCH_PTP) return L7_FAILURE; //not found

    p = (ptin_hapi_search_ptp_t*) pointer2table_index(&search_PTP_table, i, N_SEARCH_PTP, sizeof(tbl_entry));

    //Delete entry
    PT_LOG_TRACE(LOG_CTX_HAPI, "Deleting entry %u (prt=%u, vid=%u, vid_os=%u, policyId=%u)...",
                 i, p->e.key.prt, p->e.key.vid, p->e.vid_os, p->policyId);
    rc = hapiBroadPolicyDelete(p->policyId);
    if (rc != L7_SUCCESS) {PT_LOG_ERR(LOG_CTX_HAPI, "Cannot hapiBroadPolicyDelete()");}

    return N_SEARCH_PTP > del_entry(&search_PTP_table, NULL, sizeof(tbl_entry), sizeof(entry->key), N_SEARCH_PTP, i)
           &&
           rc == L7_SUCCESS ? L7_SUCCESS: L7_FAILURE;
#endif
}//ptin_hapi_ptp_entry_del















L7_RC_t ptin_hapi_ptp_dump(void) {
#if (!PTIN_BOARD_IS_STANDALONE)
 return L7_FAILURE;
#else

T_index i;
ptin_hapi_search_ptp_t  *p;
ptin_dtl_search_ptp_t   *entry;


 printf("Dumping SEARCH_PTP_TABLE (size %u)...\n\r", N_SEARCH_PTP);
 for (i=0; i<N_SEARCH_PTP; i++) {
     if (entry_is_empty(&search_PTP_table, i, N_SEARCH_PTP, sizeof(*p))) continue;

     p = (ptin_hapi_search_ptp_t*) pointer2table_index(&search_PTP_table, i, N_SEARCH_PTP, sizeof(*p));
     entry = &p->e;
     printf("%lu:\tkey(prt,vid)=(%u,%u)\tvid_prt=%u\tvid_os=%u\t", i, entry->key.prt, entry->key.vid, entry->vid_prt, entry->vid_os);

     switch (entry->encap) {
     default:
         printf("errored encap=%d", entry->encap);
         break;
     case TS_ENCAP_ETH_PTP:
         printf("encap=%d, MAC=%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
                entry->encap,
                entry->ntw.DMAC[0], entry->ntw.DMAC[1], entry->ntw.DMAC[2], entry->ntw.DMAC[3], entry->ntw.DMAC[4], entry->ntw.DMAC[5]);
         break;
     case TS_ENCAP_ETH_IPv4_PTP:
         printf("encap=%d, dIP=%u.%u.%u.%u",
                entry->encap,
                entry->ntw.dIP[0], entry->ntw.dIP[1], entry->ntw.dIP[2], entry->ntw.dIP[3]);
         break;
     case TS_ENCAP_ETH_IPv6_PTP:
         printf("encap=%d, dIP=%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x",
                entry->encap,
                entry->ntw.dIP[0], entry->ntw.dIP[1], entry->ntw.dIP[2], entry->ntw.dIP[3],
                entry->ntw.dIP[4], entry->ntw.dIP[5], entry->ntw.dIP[6], entry->ntw.dIP[7],
                entry->ntw.dIP[8], entry->ntw.dIP[9], entry->ntw.dIP[10], entry->ntw.dIP[11],
                entry->ntw.dIP[12], entry->ntw.dIP[13], entry->ntw.dIP[14], entry->ntw.dIP[15]);
         break;
     }//switch (entry->encap)

     printf("\tpolicyId=%u\n\r", p->policyId);
 }//for

  return L7_SUCCESS;
#endif
}//ptin_hapi_ptp_dump




























//DTL/APP LAYER**********************************************************************************
//extern int search_oam_ptp_add_ptp_entry_pair(T_SEARCH_OAM_PTP_TABLE *t_table, u16 vid_prt, u16 vid, u16 vid_os);
//extern int search_oam_ptp_del_ptp_entry_pair(T_SEARCH_OAM_PTP_TABLE *t_table, u16 vid_prt, u16 vid, u16 vid_os);
extern L7_RC_t ptin_intf_port2intIfNum(L7_uint32 ptin_port, L7_uint32 *intIfNum);
extern L7_RC_t ptin_xlate_ingress_set( L7_uint port, L7_uint16 outer_vlan, L7_uint op, L7_uint16 newOuterVlanId);
extern L7_RC_t ptin_xlate_ingress_clear( L7_uint port, L7_uint16 outer_vlan, L7_uint16 inner_vlan);
extern L7_RC_t dtlPtinGeneric(L7_uint32 intIfNum, L7_uint16 msgId, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data);

L7_RC_t ptin_ptp_fpga_entry(ptin_dtl_search_ptp_t *e, DAPI_CMD_GET_SET_t operation) {
#if (!PTIN_BOARD_IS_STANDALONE)
    return L7_FAILURE;
#else
    L7_RC_t rc=L7_SUCCESS;
    unsigned long r=0;
    L7_uint32 intIfNum;
    nimUSP_t  usp;

    /* FIXME TC16SXG: Convert ptin_port to DDUSP.port */
    if (ptin_intf_port2intIfNum(e->key.prt, &intIfNum)!=L7_SUCCESS) {
      PT_LOG_ERR(LOG_CTX_MISC, "Non existent port");
      return L7_ERROR;
    }
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MISC, "Non existent port");
      return L7_FAILURE;
    }
    e->key.prt = usp.port-1;

    //PTIN_PORT_CPU
    switch (operation) {
    default: return L7_ERROR;
    case DAPI_CMD_SET:
        r = ptin_flow_fpga_entry(1, e->key.prt);    //if (r) return L7_TABLE_IS_FULL;
    #if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
        ptin_xlate_ingress_set(PTIN_PORT_CPU, e->vid_os, PTIN_XLATE_ACTION_ADD, VIDpcpu);
    #endif
        break;
    case DAPI_CMD_CLEAR:
        ptin_flow_fpga_entry(2, e->key.prt);
    #if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
        ptin_xlate_ingress_clear(PTIN_PORT_CPU, e->vid_os, 0);
    #endif
        break;
    }

    //0..PTIN_SYSTEM_N_UPLINK_INTERF
    //ptin_hapi_ptp_entry_add() / ptin_hapi_ptp_entry_del() through DTL layer (tunneled down to HAPI layer)
    //PT_LOG_INFO(LOG_CTX_MISC, "",
    //         intIfNum, operation, param1, param2, sizeof(ptin_dtl_example_t));

    if (!r) {
        e->vid_prt = VIDprt(e->key.prt);
        rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_PTP_FPGA, operation, sizeof(ptin_dtl_search_ptp_t), (void *) e);
    }

    if ((r || L7_SUCCESS!=rc) && DAPI_CMD_SET==operation) {
        ptin_flow_fpga_entry(2, e->key.prt);
    #if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
        ptin_xlate_ingress_clear(PTIN_PORT_CPU, e->vid_os, 0);
    #endif
        if (r) {
            if (-3==r) return L7_TABLE_IS_FULL;
            return L7_FAILURE;
        }
        return rc;
    }
    return L7_SUCCESS;
#endif
}//ptin_ptp_fpga_entry






























//O A M******************************************************************************************
typedef struct {
    ptin_dtl_search_oam_t e;

    BROAD_POLICY_t policyId;
} __attribute__((packed)) ptin_hapi_search_oam_t;




#define N_SEARCH_OAM    N_SEARCH_PTP
//#define N_SEARCH_OAM    128
//#include <table.h>




typedef_TABLE_TEMPLATE(ptin_hapi_search_oam_t, N_SEARCH_OAM, T_SEARCH_OAM_TABLE)



T_SEARCH_OAM_TABLE search_OAM_table;













//ANDL/HAPI LAYER********************************************************************************
void ptin_hapi_oam_table_init(void) {
    PT_LOG_INFO(LOG_CTX_HAPI, "init_table()=%d",
                init_table(&search_OAM_table, sizeof(ptin_hapi_search_oam_t), N_SEARCH_OAM)
                );
}








L7_RC_t ptin_hapi_oam_entry_add(ptin_dapi_port_t *dapiPort, ptin_dtl_search_oam_t *entry)
{
#if (!PTIN_BOARD_IS_STANDALONE)
    return L7_FAILURE;
#else
//Filter MEP traffic to FPGA     (VCAP[+ICAP?])
//Mutual exclusive with hapiBroadConfigCcmFilter     (trapping; ICAP)
    ptin_hapi_search_oam_t tbl_entry, *p;
    L7_uchar8 exact_mask[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
    BROAD_POLICY_t      policyId = BROAD_POLICY_INVALID;
    BROAD_POLICY_RULE_t ruleId;
    T_index _1st_free, i;
    L7_RC_t rc = L7_SUCCESS;
    L7_uint32    bcm_port, bcm_unit;
    bcm_gport_t  gport;
    extern DAPI_t *dapi_g;

    tbl_entry.e = *entry; //tbl_entry.e.key = entry->key;
    i = find_entry(&search_OAM_table, &tbl_entry, sizeof(tbl_entry), sizeof(entry->key), N_SEARCH_OAM, -1, &_1st_free);
    PT_LOG_TRACE(LOG_CTX_HAPI, "i=%u\t_1st_free=%u\t(N_SEARCH_OAM=%u)", i, _1st_free, N_SEARCH_OAM);

    if (i<N_SEARCH_OAM) { //Entry found
        p = (ptin_hapi_search_oam_t*) pointer2table_index(&search_OAM_table, i, N_SEARCH_OAM, sizeof(tbl_entry));
        if (!memcmp(entry, &p->e, sizeof(p->e))) return L7_SUCCESS;  //Nothing to do; same entry

        //Delete entry
        PT_LOG_TRACE(LOG_CTX_HAPI, "Deleting entry...");
        rc = hapiBroadPolicyDelete(p->policyId);
        if (rc != L7_SUCCESS) {PT_LOG_ERR(LOG_CTX_HAPI, "Cannot hapiBroadPolicyDelete()");}
        del_entry(&search_OAM_table, NULL, sizeof(tbl_entry), sizeof(entry->key), N_SEARCH_OAM, i);
    }
    else
    if (_1st_free>=N_SEARCH_OAM) return L7_TABLE_IS_FULL; //No entries left
    else
    i=_1st_free;          //Entry not found; using 1st free



    // Configure rule
    do
    {
      rc = hapiBroadPolicyCreate(BROAD_POLICY_TYPE_IPSG);
      if (rc != L7_SUCCESS) {PT_LOG_ERR(LOG_CTX_HAPI, "Cannot create policy"); break;}


      //VCAP
      rc = hapiBroadPolicyStageSet(BROAD_POLICY_STAGE_LOOKUP);
      if (rc != L7_SUCCESS) {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error creating a lookup policy");
        hapiBroadPolicyCreateCancel();
        break;
      }


      // Create rule
      rc = hapiBroadPolicyPriorityRuleAdd(&ruleId, BROAD_POLICY_RULE_PRIORITY_DEFAULT);
      if (rc != L7_SUCCESS) {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error adding rule");
        hapiBroadPolicyCreateCancel();
        break;
      }


      //Qualifiers
      {
          rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_OVID, (L7_uchar8 *)&entry->key.vid, exact_mask);
          if (rc != L7_SUCCESS) {
            PT_LOG_ERR(LOG_CTX_HAPI, "Error adding OVID qualifier (%u)", entry->key.vid);
            hapiBroadPolicyCreateCancel();
            break;
          }
          PT_LOG_TRACE(LOG_CTX_HAPI,"OVID %u qualifier added", entry->key.vid);
      }


      {
       L7_ushort16 ethtype;

          ethtype=L7_ETYPE_CFM;
          rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_ETHTYPE, (L7_uchar8 *) &ethtype, exact_mask);
          if (rc != L7_SUCCESS) {
            PT_LOG_ERR(LOG_CTX_HAPI, "Error adding ETHTYPE qualifier 0x%04x", ethtype);
            hapiBroadPolicyCreateCancel();
            break;
          }
          PT_LOG_TRACE(LOG_CTX_HAPI,"ETHTYPE 0x%04x qualifier added", ethtype);
      }


      {//OAM ETH's MEL 3 bits (Check SDK's _bcm_tr3_oam_fp_create())
       bcm_ip6_t mdl_data, mdl_mask;

       memset(&mdl_data, 0, sizeof(bcm_ip6_t));
       mdl_data[0] = entry->lvl << 5;
       memset(&mdl_mask, 0, sizeof(bcm_ip6_t));
       mdl_mask[0] = 0xE0;

       rc = hapiBroadPolicyRuleQualifierAdd(ruleId, BROAD_FIELD_IP6_DST, mdl_data, mdl_mask);
       if (rc != L7_SUCCESS) {
         PT_LOG_ERR(LOG_CTX_HAPI, "Error adding BROAD_FIELD_IP6_DST qualifier 0x%2x/0x%2x", mdl_data[0], mdl_mask[0]);
         hapiBroadPolicyCreateCancel();
         break;
       }
       PT_LOG_TRACE(LOG_CTX_HAPI,"BROAD_FIELD_IP6_DST 0x%2x/0x%2x qualifier added", mdl_data[0], mdl_mask[0]);
      }


      //Actions
      if (hapiBroadPolicyRuleActionAdd(ruleId, BROAD_ACTION_ADD_OUTER_VID, VIDprt(entry->key.prt), 0, 0) != L7_SUCCESS)// ||
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error adding ADD_OUTER_VID (%u) action", VIDprt(entry->key.prt));
        hapiBroadPolicyCreateCancel();
        break;
      }
      PT_LOG_TRACE(LOG_CTX_HAPI,"ADD_OUTER_VID %u action added", VIDprt(entry->key.prt));


      // Apply policy
      if ((rc=hapiBroadPolicyCommit(&policyId)) != L7_SUCCESS) {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error commiting trap policy");
        hapiBroadPolicyCreateCancel();
        break;
      }

      /* Get bcm data from physical USP port */
      if (hapi_ptin_get_bcmdata_from_uspport(entry->key.prt, dapi_g,
                                             &bcm_unit, &bcm_unit, L7_NULLPT) != L7_SUCCESS)
      {
        PT_LOG_ERR(LOG_CTX_HAPI, "Can't get bcm data from usp_port %U", entry->key.prt);
        return L7_FAILURE;
      }

      if (bcmy_lut_unit_port_to_gport_get(bcm_unit, bcm_port, &gport) != BCMY_E_NONE)
      {
        printf("Error with unit %d, port %d", bcm_unit, bcm_port);
        return L7_FAILURE;
      }
      if ((rc=hapiBroadPolicyApplyToIface(policyId, gport)) != L7_SUCCESS) {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error applying to gport %u", gport);
        break;
        //hapiBroadPolicyDelete(policyId);
        //return L7_FAILURE;
      }

      PT_LOG_TRACE(LOG_CTX_HAPI, "Trap policy commited successfully (policyId=%u)", policyId);
    } while (0);

    /* Have occurred any error? */
    if (rc != L7_SUCCESS) {
      /* Delete rule */
      if (policyId != BROAD_POLICY_INVALID) {
        PT_LOG_TRACE(LOG_CTX_HAPI, "Removing policyId %u", policyId);
        if (L7_SUCCESS != hapiBroadPolicyDelete(policyId)) {PT_LOG_ERR(LOG_CTX_HAPI, "Cannot hapiBroadPolicyDelete()");}
      }
      PT_LOG_ERR(LOG_CTX_HAPI, "Error while creating VCAP rule");
      return rc;
    }

    //PT_LOG_TRACE(LOG_CTX_HAPI,"VCAP rule configured");

    // Save entry
    tbl_entry.policyId = policyId;
    i = add_entry(&search_OAM_table, &tbl_entry, sizeof(tbl_entry), sizeof(entry->key), N_SEARCH_OAM, i, 0, 0);
    //wr_entry(&search_OAM_table, &tbl_entry, sizeof(tbl_entry), N_SEARCH_OAM, i);

    PT_LOG_TRACE(LOG_CTX_HAPI,"add_entry()=%u\t(N_SEARCH_OAM=%u)", i, N_SEARCH_OAM);

    return L7_SUCCESS;
#endif
}//ptin_hapi_oam_entry_add








L7_RC_t ptin_hapi_oam_entry_del(ptin_dapi_port_t *dapiPort, ptin_dtl_search_oam_t *entry) {
//Filter MEP traffic to FPGA     (VCAP[+ICAP?])
//Mutual exclusive with hapiBroadConfigCcmFilter     (trapping; ICAP)
#if (!PTIN_BOARD_IS_STANDALONE)
 return L7_FAILURE;
#else

ptin_hapi_search_oam_t tbl_entry, *p;
T_index i;
L7_RC_t rc = L7_SUCCESS;

    tbl_entry.e = *entry; //tbl_entry.e.key = entry->key;
    i = find_entry(&search_OAM_table, &tbl_entry, sizeof(tbl_entry), sizeof(entry->key), N_SEARCH_OAM, -1, NULL);
    if (i>=N_SEARCH_OAM) return L7_FAILURE; //not found

    p = (ptin_hapi_search_oam_t*) pointer2table_index(&search_OAM_table, i, N_SEARCH_OAM, sizeof(tbl_entry));

    //Delete entry
    PT_LOG_TRACE(LOG_CTX_HAPI, "Deleting entry %u (prt=%u, vid=%u, oam_level=%u, policyId=%u)...",
                 i, p->e.key.prt, p->e.key.vid, p->e.lvl, p->policyId);
    rc = hapiBroadPolicyDelete(p->policyId);
    if (rc != L7_SUCCESS) {PT_LOG_ERR(LOG_CTX_HAPI, "Cannot hapiBroadPolicyDelete()");}

    return N_SEARCH_OAM > del_entry(&search_OAM_table, NULL, sizeof(tbl_entry), sizeof(entry->key), N_SEARCH_OAM, i)
           &&
           rc == L7_SUCCESS ? L7_SUCCESS: L7_FAILURE;
#endif
}//ptin_hapi_oam_entry_del








L7_RC_t ptin_hapi_oam_dump(void) {
#if (!PTIN_BOARD_IS_STANDALONE)
 return L7_FAILURE;
#else

T_index i;
ptin_hapi_search_oam_t  *p;
ptin_dtl_search_oam_t   *entry;


 printf("Dumping SEARCH_OAM_TABLE (size %u)...\n\r", N_SEARCH_OAM);
 for (i=0; i<N_SEARCH_OAM; i++) {
     if (entry_is_empty(&search_OAM_table, i, N_SEARCH_OAM, sizeof(*p))) continue;

     p = (ptin_hapi_search_oam_t*) pointer2table_index(&search_OAM_table, i, N_SEARCH_OAM, sizeof(*p));
     entry = &p->e;
     printf("%lu:\tkey(prt,vid)=(%u,%u)\tvid_prt=%u\toam_level=%u\t", i, entry->key.prt, entry->key.vid, VIDprt(entry->key.prt), entry->lvl);

     printf("\tpolicyId=%u\n\r", p->policyId);
 }//for

  return L7_SUCCESS;
#endif
}//ptin_hapi_oam_dump


//DTL/APP LAYER**********************************************************************************
L7_RC_t ptin_oam_fpga_entry(ptin_dtl_search_oam_t *e, DAPI_CMD_GET_SET_t operation) {
#if (!PTIN_BOARD_IS_STANDALONE)
    return L7_FAILURE;
#else
    L7_RC_t rc=L7_SUCCESS;
    unsigned long r=0;
    L7_uint32 intIfNum;
    nimUSP_t  usp;

    /* FIXME TC16SXG: Convert ptin_port to DDUSP.port */
    if (ptin_intf_port2intIfNum(e->key.prt, &intIfNum)!=L7_SUCCESS)
    {
        PT_LOG_ERR(LOG_CTX_MISC, "Non existent port");
        return L7_FAILURE;
    }
    if (nimGetUnitSlotPort(intIfNum, &usp) != L7_SUCCESS)
    {
      PT_LOG_ERR(LOG_CTX_MISC, "Non existent port");
      return L7_FAILURE;
    }
    e->key.prt = usp.port-1;

    switch (operation) {
    default: return L7_ERROR;
    case DAPI_CMD_SET:
        if (ptin_flow_fpga_entry(1, e->key.prt)>=N_FPGA_VIDPRTS) return L7_TABLE_IS_FULL;
        break;
    case DAPI_CMD_CLEAR:
        ptin_flow_fpga_entry(2, e->key.prt);
        break;
    }


    //0..PTIN_SYSTEM_N_UPLINK_INTERF
    //ptin_hapi_oam_entry_add() / ptin_hapi_oam_entry_del() through DTL layer (tunneled down to HAPI layer)
    //PT_LOG_INFO(LOG_CTX_MISC, "",
    //         intIfNum, operation, param1, param2, sizeof(ptin_dtl_example_t));

    if (!r) {
        rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_OAM_FPGA, operation, sizeof(ptin_dtl_search_oam_t), (void *) e);
    }

    if ((r || L7_SUCCESS!=rc) && DAPI_CMD_SET==operation) {
        ptin_flow_fpga_entry(2, e->key.prt);
        if (r) {
            if (-3==r) return L7_TABLE_IS_FULL;
            return L7_FAILURE;
        }
    }
    return L7_SUCCESS;
#endif
}//ptin_oam_fpga_entry

