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




//P T P / 1588***********************************************************************************
//ANDL/HAPI LAYER********************************************************************************
typedef struct {
    ptin_dtl_search_ptp_t e;

    BROAD_POLICY_t policyId;
} __attribute__((packed)) ptin_hapi_search_ptp_t;




//#include <my_types.h>
#define N_SEARCH_PTP    128
#include <table.h>




typedef_TABLE_TEMPLATE(ptin_hapi_search_ptp_t, N_SEARCH_PTP, T_SEARCH_PTP_TABLE)



T_SEARCH_PTP_TABLE search_PTP_table;













void ptin_hapi_ptp_table_init(void) {
    PT_LOG_INFO(LOG_CTX_HAPI, "init_table()=%d",
                init_table(&search_PTP_table, sizeof(ptin_hapi_search_ptp_t), N_SEARCH_PTP)
                );
}











L7_RC_t ptin_hapi_ptp_entry_add(ptin_dapi_port_t *dapiPort, ptin_dtl_search_ptp_t *entry)
{
#if !(PTIN_BOARD == PTIN_BOARD_OLT1T0)
 return L7_FAILURE;
#else
  //L7_int      /*entry, free_entry,*/ rule, free_rule, max_rules;
  ptin_hapi_search_ptp_t tbl_entry, *p;
  //L7_uchar8 exact_mask[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
  BROAD_POLICY_t      policyId = BROAD_POLICY_INVALID;
  BROAD_POLICY_RULE_t ruleId;
  T_index _1st_free, i;
  L7_RC_t rc = L7_SUCCESS;
  bcm_port_t    bcm_port;
  bcmx_lport_t  lport;




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


    if ((rc=hapi_ptin_bcmPort_get(entry->key.prt, &bcm_port)) != L7_SUCCESS) {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error getting bcm_port %u", bcm_port);
        break;
        //hapiBroadPolicyDelete(policyId);
        //return L7_FAILURE;
    }
    lport = bcmx_unit_port_to_lport(0, bcm_port);
    if ((rc=hapiBroadPolicyApplyToIface(policyId, lport)) != L7_SUCCESS) {
      PT_LOG_ERR(LOG_CTX_HAPI, "Error applying to lport %u", lport);
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
#if !(PTIN_BOARD == PTIN_BOARD_OLT1T0)
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
#if !(PTIN_BOARD == PTIN_BOARD_OLT1T0)
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
#define VIDpcpu PTIN_VLAN_FPGA2CPU
#define VIDprt(i)       (PTIN_VLAN_FPGA2PORT_MIN+(i))      /*0<=i<16*/
//#include <ptin_xlate_api.h>
extern L7_RC_t ptin_xlate_egress_set( L7_uint port, L7_uint16 outer_vlan, L7_uint op, L7_uint16 newOuterVlanId);

void ptin_ptp_flows_init(void) {
#if (PTIN_BOARD == PTIN_BOARD_OLT1T0)
L7_uint i;

    //Remove VID(cpu) @ CPU port's egress
    ptin_xlate_egress_set(PTIN_PORT_CPU, VIDpcpu, PTIN_XLATE_ACTION_DELETE, -1);

    //Remove VID(po) (=VID(pi)) @ PO/PI port's egress
    for (i = 0; i < PTIN_SYSTEM_N_UPLINK_INTERF; i++) {
        ptin_xlate_egress_set(i, VIDprt(i), PTIN_XLATE_ACTION_DELETE, -1);
    }//for
#endif
}//ptin_ptp_flows_init






















//extern int search_oam_ptp_add_ptp_entry_pair(T_SEARCH_OAM_PTP_TABLE *t_table, u16 vid_prt, u16 vid, u16 vid_os);
//extern int search_oam_ptp_del_ptp_entry_pair(T_SEARCH_OAM_PTP_TABLE *t_table, u16 vid_prt, u16 vid, u16 vid_os);
extern inline L7_RC_t ptin_intf_port2intIfNum(L7_uint32 ptin_port, L7_uint32 *intIfNum);
extern L7_RC_t ptin_xlate_ingress_set( L7_uint port, L7_uint16 outer_vlan, L7_uint op, L7_uint16 newOuterVlanId);
extern L7_RC_t ptin_xlate_ingress_clear( L7_uint port, L7_uint16 outer_vlan, L7_uint16 inner_vlan);
extern L7_RC_t dtlPtinGeneric(L7_uint32 intIfNum, L7_uint16 msgId, DAPI_CMD_GET_SET_t operation, L7_uint32 dataSize, void *data);

L7_RC_t ptin_ptp_fpga_entry(ptin_dtl_search_ptp_t *e, DAPI_CMD_GET_SET_t operation) {
#if !(PTIN_BOARD == PTIN_BOARD_OLT1T0)
 return L7_FAILURE;
#else
L7_RC_t rc;
L7_uint32 intIfNum;

    if (ptin_intf_port2intIfNum(e->key.prt, &intIfNum)!=L7_SUCCESS) {PT_LOG_ERR(LOG_CTX_MISC, "Non existent port"); return L7_FAILURE;}

    //PTIN_PORT_CPU
    switch (operation) {
    default: return L7_FAILURE;
    case DAPI_CMD_SET:      ptin_xlate_ingress_set(PTIN_PORT_CPU, e->vid_os, PTIN_XLATE_ACTION_ADD, VIDpcpu); break;
    case DAPI_CMD_CLEAR:    ptin_xlate_ingress_clear(PTIN_PORT_CPU, e->vid_os, 0); break;
    }


    //0..PTIN_SYSTEM_N_UPLINK_INTERF
    //ptin_hapi_ptp_entry_add() / ptin_hapi_ptp_entry_del() through DTL layer (tunneled down to HAPI layer)
    //PT_LOG_INFO(LOG_CTX_MISC, "",
    //         intIfNum, operation, param1, param2, sizeof(ptin_dtl_example_t));

    e->vid_prt = VIDprt(e->key.prt);
    rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_PTP_FPGA, operation, sizeof(ptin_dtl_search_ptp_t), (void *) e);

    if (L7_SUCCESS!=rc && DAPI_CMD_SET==operation) ptin_xlate_ingress_clear(PTIN_PORT_CPU, e->vid_os, 0);
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








L7_RC_t ptin_hapi_oam_entry_add(ptin_dapi_port_t *dapiPort, ptin_dtl_search_oam_t *entry) {
//Filter MEP traffic to FPGA     (VCAP[+ICAP?])
//Mutual exclusive with hapiBroadConfigCcmFilter     (trapping; ICAP)

#if !(PTIN_BOARD == PTIN_BOARD_OLT1T0)
 return L7_FAILURE;
#else

ptin_hapi_search_oam_t tbl_entry, *p;
L7_uchar8 exact_mask[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
BROAD_POLICY_t      policyId = BROAD_POLICY_INVALID;
BROAD_POLICY_RULE_t ruleId;
T_index _1st_free, i;
L7_RC_t rc = L7_SUCCESS;
bcm_port_t    bcm_port;
bcmx_lport_t  lport;




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


      if ((rc=hapi_ptin_bcmPort_get(entry->key.prt, &bcm_port)) != L7_SUCCESS) {
          PT_LOG_ERR(LOG_CTX_HAPI, "Error getting bcm_port %u", bcm_port);
          break;
          //hapiBroadPolicyDelete(policyId);
          //return L7_FAILURE;
      }
      lport = bcmx_unit_port_to_lport(0, bcm_port);
      if ((rc=hapiBroadPolicyApplyToIface(policyId, lport)) != L7_SUCCESS) {
        PT_LOG_ERR(LOG_CTX_HAPI, "Error applying to lport %u", lport);
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
#if !(PTIN_BOARD == PTIN_BOARD_OLT1T0)
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
#if !(PTIN_BOARD == PTIN_BOARD_OLT1T0)
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
#if !(PTIN_BOARD == PTIN_BOARD_OLT1T0)
 return L7_FAILURE;
#else

L7_RC_t rc;
L7_uint32 intIfNum;

    if (ptin_intf_port2intIfNum(e->key.prt, &intIfNum)!=L7_SUCCESS) {PT_LOG_ERR(LOG_CTX_MISC, "Non existent port"); return L7_FAILURE;}

    //0..PTIN_SYSTEM_N_UPLINK_INTERF
    //ptin_hapi_oam_entry_add() / ptin_hapi_oam_entry_del() through DTL layer (tunneled down to HAPI layer)
    //PT_LOG_INFO(LOG_CTX_MISC, "",
    //         intIfNum, operation, param1, param2, sizeof(ptin_dtl_example_t));

    rc = dtlPtinGeneric(intIfNum, PTIN_DTL_MSG_OAM_FPGA, operation, sizeof(ptin_dtl_search_oam_t), (void *) e);

    return L7_SUCCESS;
#endif
}//ptin_oam_fpga_entry

