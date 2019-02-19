/*
 * $Id: pkt.c,v 1.15 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 */

#include <shared/alloc.h>
#include <sal/core/libc.h>

#include <soc/cm.h>
#include <soc/drv.h>
#include <shared/bsl.h>

#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/rx.h>
#include <bcm/pkt.h>
/*#include <bcm_int/pkt.h>*/
#include <bcm_int/control.h>

#define UNIT_VALID(unit) \
{ \
  if (!BCM_UNIT_VALID(unit)) { return BCM_E_UNIT; } \
}



char*
  bcm_pkt_dnx_type_to_string(
    bcm_pkt_dnx_type_t dnx_hdr_type
  )
{
  char* str = NULL;
  switch(dnx_hdr_type)
  {
    case bcmPktDnxTypePtch1:
        str = "ptch1";
        break;
    case bcmPktDnxTypePtch2:
        str = "ptch2";
        break;
    case bcmPktDnxTypeItmh:
        str = "itmh";
        break;
    case bcmPktDnxTypeFtmh:
        str = "ftmh";
        break;
    case bcmPktDnxTypePph:
        str = "pph";
        break;
    case bcmPktDnxTypeOtsh:
        str = "otsh";
        break;
    case bcmPktDnxTypeOtmh:
        str = "otmh";
        break;
    case bcmPktDnxTypeRaw:
        str = "raw";
        break;
    default:
        str = " Unknown";
  }
  return str;
}
void bcm_pkt_dnx_ptch1_dump(
   bcm_pkt_dnx_ptch1_t  *ptch1
   )
{
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %u %s %u %s %u\n\r" ),
             "src_gport:" ,ptch1->src_gport, 
             "opaque_attr:", ptch1->opaque_attr, 
             "is_port_header_type:", ptch1->is_port_header_type
             ));

}
void bcm_pkt_dnx_ptch2_dump(
   bcm_pkt_dnx_ptch2_t  *ptch2
   )
{
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %u %s %u %s %u\n\r" ),
             "src_local_port:" ,ptch2->src_local_port, 
             "opaque_attr:", ptch2->opaque_attr, 
             "is_port_header_type:", ptch2->is_port_header_type
             ));

}
char*
  bcm_pkt_dnx_itmh_dest_type_to_string(
     bcm_pkt_dnx_itmh_dest_type_t dest_type
  )
{
  char* str = NULL;
  switch(dest_type)
  {
    case bcmPktDnxItmhDestTypeMulticast:
        str = "Multicast";
        break;
    case bcmPktDnxItmhDestTypeFlow:
        str = "Flow";
        break;
    case bcmPktDnxItmhDestTypeIngressShapingFlow:
        str = "IngressShapingFlow";
        break;
    case bcmPktDnxItmhDestTypeVport:
        str = "Vport";
        break;
    case bcmPktDnxItmhDestTypeSystemPort:
        str = "SystemPort";
        break;
    default:
        str = " Unknown";
  }
  return str;
}

void bcm_pkt_dnx_itmh_dest_dump(
                        bcm_pkt_dnx_itmh_dest_t *itmh_dest
                        )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %s %s %s %u %s %u %s %u\n\r" ),
             "dest_type:" ,bcm_pkt_dnx_itmh_dest_type_to_string(itmh_dest->dest_type), 
             "dest_extension_type:", bcm_pkt_dnx_itmh_dest_type_to_string(itmh_dest->dest_extension_type), 
             "destination:", itmh_dest->destination,
             "multicast_id:", itmh_dest->multicast_id,
             "destination_ext:", itmh_dest->destination_ext
             ));


}
void bcm_pkt_dnx_itmh_dump(
   bcm_pkt_dnx_itmh_t  *itmh
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %u %s %u %s %u %s %u\n\r" ),
             "inbound_mirror_disable:" ,itmh->inbound_mirror_disable, 
             "snoop_cmnd:", itmh->snoop_cmnd, 
             "prio:", itmh->prio,
             "color:", itmh->color
             ));



    bcm_pkt_dnx_itmh_dest_dump(&(itmh->dest));


}
char*
 bcm_pkt_dnx_ftmh_action_type_to_string(
      bcm_pkt_dnx_ftmh_action_type_t action_type
  )
{
  char* str = NULL;
  switch(action_type)
  {
    case bcmPktDnxFtmhActionTypeForward:
        str = "Forward";
        break;
    case bcmPktDnxFtmhActionTypeSnoop:
        str = "Snoop";
        break;
    case bcmPktDnxFtmhActionTypeInboundMirror:
        str = "InboundMirror";
        break;
    case bcmPktDnxFtmhActionTypeOutboundMirror:
        str = "OutboundMirror";
        break;
    default:
        str = "Unknown";
  }
  return str;
}


void bcm_pkt_dnx_ftmh_lb_extension_dump(
   bcm_pkt_dnx_ftmh_lb_extension_t  *lb_extension
   )
{
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %u\n\r" ),
             "lb_extension:" ,
             "lb_key:", lb_extension->lb_key
             ));



}

void bcm_pkt_dnx_ftmh_dest_extension_dump(
   bcm_pkt_dnx_ftmh_dest_extension_t  *dest_extension
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %u\n\r" ),
             "dest_extension:" ,
             "dst_sysport:", dest_extension->dst_sysport
             ));


}
void bcm_pkt_dnx_ftmh_stack_extension_dump(
   bcm_pkt_dnx_ftmh_stack_extension_t  *stack_extension
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("\t\t %s %s %u\n\r" ),
             "stack_extension:" ,
             "stack_route_history_bmp:", stack_extension->stack_route_history_bmp
             ));

}

void bcm_pkt_dnx_ftmh_dump(
   bcm_pkt_dnx_ftmh_t  *ftmh
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %u %s %u %s %u %s %u\n\r \
                      %s %u %s %s %s %u %s %u\n\r \
                      %s %u %s %u %s %u\n\r" ), 
             "packet_size:" ,ftmh->packet_size, 
             "prio:", ftmh->prio, 
             "src_sysport:", ftmh->src_sysport,
             "dst_port:", ftmh->dst_port,
             "ftmh_dp:" ,ftmh->ftmh_dp,
             "action_type:", bcm_pkt_dnx_ftmh_action_type_to_string(ftmh->action_type),
             "out_mirror_disable:", ftmh->out_mirror_disable,
             "is_mc_traffic:" ,ftmh->is_mc_traffic,
             "multicast_id:", ftmh->multicast_id,
             "out_vport:", ftmh->out_vport,
             "cni:", ftmh->cni));

    if (ftmh->lb_ext.valid) {
        bcm_pkt_dnx_ftmh_lb_extension_dump(&(ftmh->lb_ext));
    }
    if (ftmh->dest_ext.valid ) {
        bcm_pkt_dnx_ftmh_dest_extension_dump(&(ftmh->dest_ext));
    }
    if (ftmh->stack_ext.valid ) {
        bcm_pkt_dnx_ftmh_stack_extension_dump(&(ftmh->stack_ext));
    }
}
void bcm_pkt_dnx_pph_eei_extension_dump(
   bcm_pkt_dnx_pph_eei_extension_t  *ext
   )
{
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s  %s %u %s %u %s %u %s %u\n\r"),
                    "pph_eei_extension",
                    "is_mim:",ext->is_mim,
                    "i_sid:",ext->i_sid,
                    "command:", ext->command,
                    "data:",ext->data));


}
void bcm_pkt_dnx_pph_learn_extension_dump(
   bcm_pkt_dnx_pph_learn_extension_t  *ext
   )
{

    /*LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("raw_data: %u\n\r"),ext->raw_data));*/ 
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s  %s %u %s %u %s %u\n\r"),
                    "pph_learn_extension",
                    "phy_gport:",ext->phy_gport,
                    "encap_id_valid:",ext->is_encap_id_valid,
                    "encap_id:", ext->encap_id));
    if (ext->eei.valid) {
        bcm_pkt_dnx_pph_eei_extension_dump(&(ext->eei));
    }


}
 char*
 bcm_pkt_dnx_pph_fhei_type_to_string(
      bcm_pkt_dnx_pph_fhei_type_t fhei_type
  )
{
  char* str = NULL;
  switch(fhei_type)
  {
    case bcmPktDnxPphFheiTypeBridge:
        str = "Bridge";
        break;
    case bcmPktDnxPphFheiTypeTrap:
        str = "Trap";
        break;
    case bcmPktDnxPphFheiTypeIp:
        str = "Ip";
        break;
    case bcmPktDnxPphFheiTypeMpls:
        str = "Mpls";
        break;
  case bcmPktDnxPphFheiTypeTrill:
      str = "Trill";
      break;
    default:
        str = "Unknown";
        break;

  }
  return str;
}
void bcm_pkt_dnx_pph_fhei_bridge_extension_dump(
   bcm_pkt_dnx_pph_fhei_bridge_extension_t  *ext
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %u %s %u %s %u %s %u %s %u %s %u %s %u\n\r"),
                    "pph_fhei_bridge_extension",
                    "ive_cmd:",ext->ive_cmd,
                    "pcp1:",ext->pcp1,
                    "dei1:",ext->dei1,
                    "vlan1:", ext->vlan1,
                    "pcp2:",ext->pcp2,
                    "dei2:",ext->dei2,
                    "vlan2:",ext->vlan2));


}
void bcm_pkt_dnx_pph_fhei_trap_extension_dump(
   bcm_pkt_dnx_pph_fhei_trap_extension_t  *ext
   )
{
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %u %s %u\n\r"),
                    "pph_fhei_trap_extension",
                    "trap_qualifier:",ext->trap_qualifier,
                    "trap_id:",ext->trap_id));
}

void bcm_pkt_dnx_pph_fhei_ip_extension_dump(
   bcm_pkt_dnx_pph_fhei_ip_extension_t  *ext
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("\t\t %s %s %u %s %u %s %u\n\r"),
                    "pph_fhei_ip_extension:",
                    "ive_cmd:",ext->ive_cmd,
                    "pri:",ext->pri,
                    "in_ttl:", ext->in_ttl));


}

void bcm_pkt_dnx_pph_fhei_mpls_extension_dump(
   bcm_pkt_dnx_pph_fhei_mpls_extension_t  *ext
   )
{
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %u %s %u %s %u %s %u %s %u %s %u %s %u %s %u\n\r"),
                    "pph_fhei_mpls_extension:",
                    "tpid_profile:",ext->tpid_profile,
                    "label:",ext->label,
                    "upper_layer_protocol:",ext->upper_layer_protocol,
                    "is_pipe_model:", ext->is_pipe_model,
                    "cw:", ext->cw,
                    "label_cmd:", ext->label_cmd,
                    "in_exp:", ext->in_exp,
                    "in_ttl:", ext->in_ttl
                   ));

}
void bcm_pkt_dnx_pph_fhei_trill_extension_dump(
   bcm_pkt_dnx_pph_fhei_trill_extension_t  *ext
   )
{


    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %u\n\r"),
                    "pph_fhei_trill_extension:",
                    "in_ttl:", ext->in_ttl
                   ));

}
void bcm_pkt_dnx_pph_fhei_extension_dump(
   bcm_pkt_dnx_pph_fhei_extension_t  *ext
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %s\n\r"),
                    "pph_fhei_extension:",
                    "fhei_type:",bcm_pkt_dnx_pph_fhei_type_to_string(ext->fhei_type)
                   ));

    switch (ext->fhei_type) {
    case bcmPktDnxPphFheiTypeBridge:
        bcm_pkt_dnx_pph_fhei_bridge_extension_dump(&(ext->bridge));
        break;
    case bcmPktDnxPphFheiTypeTrap:
        bcm_pkt_dnx_pph_fhei_trap_extension_dump(&(ext->trap));
        break;
    case bcmPktDnxPphFheiTypeIp:
        bcm_pkt_dnx_pph_fhei_ip_extension_dump(&(ext->ip));
        break;
    case bcmPktDnxPphFheiTypeMpls:
        bcm_pkt_dnx_pph_fhei_mpls_extension_dump(&(ext->mpls));
        break;
    case bcmPktDnxPphFheiTypeTrill:
      bcm_pkt_dnx_pph_fhei_trill_extension_dump(&(ext->trill));
      break;
    default:
        break;
    }


}
void bcm_pkt_dnx_pph_dump(
   bcm_pkt_dnx_pph_t  *pph
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %u %s %u %s %u %s %u %s %u %s %u\n\r \
                      %s %u %s %u %s %u %s %u %s %u\n\r"),
                    "fhei_size:",pph->fhei_size,
                    "forwarding_type:",pph->forwarding_type,
                    "forwarding_header_offset:",pph->forwarding_header_offset,
                    "bypass_filter:", pph->bypass_filter,
                    "snoop_cmnd:", pph->snoop_cmnd,
                    "vport_orientation:", pph->vport_orientation,
                    "unknown_address:", pph->unknown_address,
                    "learn_allowed:", pph->learn_allowed,
                    "vswitch:", pph->vswitch,
                    "vrf:", pph->vrf,
                    "in_vport:", pph->in_vport
                   ));
    bcm_pkt_dnx_pph_fhei_extension_dump(&(pph->fhei));
    if (pph->eei.valid) {
        bcm_pkt_dnx_pph_eei_extension_dump(&(pph->eei));
    }
    if (pph->learn.valid) {
        bcm_pkt_dnx_pph_learn_extension_dump(&(pph->learn));
    }


}

 char*
 bcm_pkt_dnx_otsh_type_to_string(
      bcm_pkt_dnx_otsh_type_t otsh_type
  )
{
  char* str = NULL;
  switch(otsh_type)
  {
    case bcmPktDnxOtshTypeOam:
        str = "Oam";
        break;
    case bcmPktDnxOtshTypeL588v2:
        str = "L588v2";
        break;
    case bcmPktDnxOtshTypeLatency:
        str = "Latency";
        break;
    default:
        str = "Unknown";
  }
  return str;
}

 char*
 bcm_pkt_dnx_otsh_oam_subtype_to_string(
     bcm_pkt_dnx_otsh_oam_subtype_t otsh_subtype
  )
{
  char* str = NULL;
  switch(otsh_subtype)
  {
    case bcmPktDnxOtshOamSubtypeNone:
        str = "None";
        break;
    case bcmPktDnxOtshOamSubtypeLm:
        str = "Lm";
        break;
    case bcmPktDnxOtshOamSubtypeDm1588:
        str = "Dm1588";
        break;
    case bcmPktDnxOtshOamSubtypeDmNtp:
        str = "DmNtp";
        break;
    case bcmPktDnxOtshOamSubtypeOamDefault:
        str = "Default";
        break;
    case bcmPktDnxOtshOamSubtypeLoopback:
        str = "Loopback";
        break;
    case bcmPktDnxOtshOamSubtypeEcn:
        str = "Ecn";
        break;
    default:
        str = "Unknown";
  }
  return str;
}
void bcm_pkt_dnx_otsh_dump(
   bcm_pkt_dnx_otsh_t  *otsh
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %s %s %s %u %s %u %s %u %s %u %s %u\n\r"),
                    "otsh_type:",bcm_pkt_dnx_otsh_type_to_string(otsh->otsh_type),
                    "oam_sub_type:",bcm_pkt_dnx_otsh_oam_subtype_to_string(otsh->oam_sub_type),
                    "oam_up_mep:",otsh->oam_up_mep,
                    "tp_cmd:", otsh->tp_cmd,
                    "ts_encap:", otsh->ts_encap,
                    "latency_flow_ID:", otsh->latency_flow_ID,
                    "offset:", otsh->offset
                   ));
    /* LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("oam_ts_data: %u\n\r"),otsh->oam_ts_data));*/ 

}

void bcm_pkt_dnx_otmh_src_sysport_extension_dump(
   bcm_pkt_dnx_otmh_src_sysport_extension_t  *ext
   )
{
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %u\n\r"),
                    "otmh_src_sysport_extension:",
                    "src_sysport:", ext->src_sysport
                   ));

}
void bcm_pkt_dnx_otmh_vport_extension_dump(
   bcm_pkt_dnx_otmh_vport_extension_t  *ext
   )
{

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %u\n\r"),
                    "otmh_vport_extension:",
                    "out_vport", ext->out_vport
                   ));

}
void bcm_pkt_dnx_otmh_dump(
   bcm_pkt_dnx_otmh_t  *otmh
   )
{
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%s %s %s %u %s %u %s %u %s %u\n\r"),
                    "action_type:",bcm_pkt_dnx_ftmh_action_type_to_string(otmh->action_type),
                    "ftmh_dp:",otmh->ftmh_dp,
                    "is_mc_traffic:",otmh->is_mc_traffic,
                    "prio:", otmh->prio,
                    "dst_port:", otmh->dst_port
                   ));
    if (otmh->src_sysport_ext.valid) {
        bcm_pkt_dnx_otmh_src_sysport_extension_dump(&otmh->src_sysport_ext);
    }

    if (otmh->out_vport_ext.valid) {
        bcm_pkt_dnx_otmh_vport_extension_dump(&otmh->out_vport_ext);
    }

}


void bcm_pkt_dnx_raw_dump(
    bcm_pkt_dnx_raw_t *pkt
   )
{  
    int i;
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("data len: %u\n\r"),pkt->len));

    for (i = 0; i < pkt->len; i++) {
        LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("%u "),pkt->data[i]));

    }
    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("\n\r")));

}
void bcm_pkt_dnx_dump(
    bcm_pkt_t *pkt
   )
{
    int i;
    bcm_pkt_dnx_ptch1_t *ptch1;
    bcm_pkt_dnx_t       *dnx_pkt;

    LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("dnx_header_count: %u\n\r"),pkt->dnx_header_count));

    for (i = 0; i < pkt->dnx_header_count; i++) {
        dnx_pkt = (bcm_pkt_dnx_t *)&(pkt->dnx_header_stack[i]);
 
        LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("===Header : %u======\n\r"), i));

        LOG_VERBOSE(BSL_LS_BCM_PKT,(BSL_META("type: %s \n\r"), bcm_pkt_dnx_type_to_string(pkt->dnx_header_stack[i].type)));

        switch (pkt->dnx_header_stack[i].type) {
        case bcmPktDnxTypePtch1:
                ptch1 = (bcm_pkt_dnx_ptch1_t *)&(dnx_pkt->ptch1);
                bcm_pkt_dnx_ptch1_dump(ptch1);
                break;
            case bcmPktDnxTypePtch2:
                bcm_pkt_dnx_ptch2_dump(&(dnx_pkt->ptch2));
                break;        
            case bcmPktDnxTypeItmh:
                bcm_pkt_dnx_itmh_dump((bcm_pkt_dnx_itmh_t *)&(pkt->dnx_header_stack[i].itmh));
                break;
            case bcmPktDnxTypeFtmh:
                bcm_pkt_dnx_ftmh_dump((bcm_pkt_dnx_ftmh_t *)&(pkt->dnx_header_stack[i].ftmh));
                break;
            case bcmPktDnxTypePph:
                bcm_pkt_dnx_pph_dump((bcm_pkt_dnx_pph_t *)&(pkt->dnx_header_stack[i].pph));
                break;
            case bcmPktDnxTypeOtsh:
                bcm_pkt_dnx_otsh_dump(&(pkt->dnx_header_stack[i].otsh));
                break;
            case bcmPktDnxTypeOtmh:
                bcm_pkt_dnx_otmh_dump(&(pkt->dnx_header_stack[i].otmh));
                break;
            default:
                bcm_pkt_dnx_raw_dump(&(pkt->dnx_header_stack[i].raw));
                break;
        }

    }

}





