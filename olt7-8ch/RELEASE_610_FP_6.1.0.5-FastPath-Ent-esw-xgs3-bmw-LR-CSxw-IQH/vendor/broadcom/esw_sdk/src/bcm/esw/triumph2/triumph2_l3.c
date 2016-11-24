/*
 * $Id: triumph2_l3.c,v 1.1 2011/04/18 17:11:02 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 *
 * File:        l3.c
 * Purpose:     Triumph2 L3 function implementations
 */


#include <soc/defs.h>

#include <assert.h>

#include <sal/core/libc.h>
#if defined(BCM_TRIUMPH2_SUPPORT)  && defined(INCLUDE_L3)

#include <shared/util.h>
#include <soc/mem.h>
#include <soc/cm.h>
#include <soc/drv.h>
#include <soc/register.h>
#include <soc/memory.h>
#include <soc/l3x.h>
#include <soc/lpm.h>
#include <soc/tnl_term.h>

#include <bcm/l3.h>
#include <bcm/tunnel.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/stack.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw_dispatch.h>
/*
 * Function:
 *      _bcm_tr2_l3_tnl_term_add
 * Purpose:
 *      Add tunnel termination entry to the hw.
 * Parameters:
 *      unit     - (IN)SOC unit number.
 *      tnl_info - (IN)Tunnel terminator parameters. 
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_l3_tnl_term_add(int unit, uint32 *entry_ptr, bcm_tunnel_terminator_t *tnl_info)
{
    bcm_module_t mod_in, mod_out, my_mod;
    bcm_port_t port_in, port_out;
    _bcm_l3_ingress_intf_t iif;
    int tunnel, wlan;
    int rv;

    /* Program remote port */
    if ((tnl_info->type == bcmTunnelTypeWlanWtpToAc) || 
        (tnl_info->type == bcmTunnelTypeWlanAcToAc) ||
        (tnl_info->type == bcmTunnelTypeWlanWtpToAc6) ||
        (tnl_info->type == bcmTunnelTypeWlanAcToAc6)) {
        wlan = 1;
    } else {
        wlan = 0;
    }
    if (wlan) {
        if (tnl_info->flags & BCM_TUNNEL_TERM_WLAN_REMOTE_TERMINATE) {
            if (!BCM_GPORT_IS_MODPORT(tnl_info->remote_port)) {
                return BCM_E_PARAM;
            }
            mod_in = BCM_GPORT_MODPORT_MODID_GET(tnl_info->remote_port);
            port_in = BCM_GPORT_MODPORT_PORT_GET(tnl_info->remote_port); 
            BCM_IF_ERROR_RETURN
                (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, mod_in, 
                                        port_in, &mod_out, &port_out));
            if (!SOC_MODID_ADDRESSABLE(unit, mod_out)) {
                return (BCM_E_BADID);
            }
            if (!SOC_PORT_ADDRESSABLE(unit, port_out)) {
                return (BCM_E_PORT);
            }
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, REMOTE_TERM_GPPf,
                                       (mod_out << 6) | port_out);
        } else {
            /* Send to the local loopback port */
            rv = bcm_esw_stk_my_modid_get(unit, &my_mod);
            BCM_IF_ERROR_RETURN(rv);
            port_in = 54;
            mod_in = my_mod;
            rv = bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET, mod_in, 
                                        port_in, &mod_out, &port_out);
            BCM_IF_ERROR_RETURN(rv);

            soc_L3_TUNNELm_field32_set(unit, entry_ptr, REMOTE_TERM_GPPf,
                                       (mod_out << 6) | port_out);
        }
        /* Program tunnel id */
        if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) {
            if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
                return BCM_E_PARAM;
            }
            tunnel = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, TUNNEL_IDf, tunnel);
        }
    }
    if ((tnl_info->type == bcmTunnelTypeAutoMulticast) ||
        (tnl_info->type == bcmTunnelTypeAutoMulticast6)) {
        /* Program L3_IIFm */
        if(SOC_MEM_FIELD_VALID(unit, L3_IIFm, IPMC_L3_IIFf)) {
            sal_memset(&iif, 0, sizeof(_bcm_l3_ingress_intf_t));
            iif.intf_id = tnl_info->vlan;

            rv = _bcm_tr_l3_ingress_interface_get(unit, &iif);
            BCM_IF_ERROR_RETURN(rv);
            iif.vrf = tnl_info->vrf;
            rv = _bcm_tr_l3_ingress_interface_set(unit, &iif);
            BCM_IF_ERROR_RETURN(rv);
        }

        /* Program tunnel id */
        if (tnl_info->flags & BCM_TUNNEL_TERM_TUNNEL_WITH_ID) {
            if (!BCM_GPORT_IS_TUNNEL(tnl_info->tunnel_id)) {
                return BCM_E_PARAM;
            }
            tunnel = BCM_GPORT_TUNNEL_ID_GET(tnl_info->tunnel_id);
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, TUNNEL_IDf, tunnel);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr2_l3_tnl_term_entry_init
 * Purpose:
 *      Initialize soc tunnel terminator entry key portion.
 * Parameters:
 *      unit     - (IN)  BCM device number. 
 *      tnl_info - (IN)  BCM buffer with tunnel info.
 *      entry    - (OUT) SOC buffer with key filled in.  
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_l3_tnl_term_entry_init(int unit, bcm_tunnel_terminator_t *tnl_info,
                                soc_tunnel_term_t *entry)
{
    int       idx;                /* Entry iteration index.     */
    int       idx_max;            /* Entry widht.               */
    uint32    *entry_ptr;         /* Filled entry pointer.      */
    _bcm_tnl_term_type_t tnl_type;/* Tunnel type.               */
    int       rv;                 /* Operation return status.   */

    /* Input parameters check. */
    if ((NULL == tnl_info) || (NULL == entry)) {
        return (BCM_E_PARAM);
    }

    /* Get tunnel type & sub_type. */
    BCM_IF_ERROR_RETURN
        (_bcm_xgs3_l3_set_tnl_term_type(unit, tnl_info, &tnl_type));

    /* Reset destination structure. */
    sal_memset(entry, 0, sizeof(soc_tunnel_term_t));

    /* Set Destination/Source pair. */
    entry_ptr = (uint32 *)&entry->entry_arr[0];
    if (tnl_type.tnl_outer_hdr_ipv6) {
        /* Apply mask on source address. */
        rv = bcm_xgs3_l3_mask6_apply(tnl_info->sip6_mask, tnl_info->sip6);
        BCM_IF_ERROR_RETURN(rv);

        /* SIP [0-63] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[0], IP_ADDRf,
                             tnl_info->sip6, SOC_MEM_IP6_LOWER_ONLY);
        /* SIP [64-127] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[1], IP_ADDRf,
                             tnl_info->sip6, SOC_MEM_IP6_UPPER_ONLY);
        /* DIP [0-63] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[2], IP_ADDRf,
                             tnl_info->dip6, SOC_MEM_IP6_LOWER_ONLY);
        /* DIP [64-127] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[3], IP_ADDRf,
                             tnl_info->dip6, SOC_MEM_IP6_UPPER_ONLY);

        /* SIP MASK [0-63] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[0], IP_ADDR_MASKf,
                             tnl_info->sip6_mask, SOC_MEM_IP6_LOWER_ONLY);
        /* SIP MASK [64-127] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[1], IP_ADDR_MASKf,
                             tnl_info->sip6_mask, SOC_MEM_IP6_UPPER_ONLY);
        /* DIP MASK [0-63] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[2], IP_ADDR_MASKf,
                             tnl_info->dip6_mask, SOC_MEM_IP6_LOWER_ONLY);
        /* DIP MASK [64-127] */
        soc_mem_ip6_addr_set(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[3], IP_ADDR_MASKf,
                             tnl_info->dip6_mask, SOC_MEM_IP6_UPPER_ONLY);
    }  else {
        tnl_info->sip &= tnl_info->sip_mask;

        /* Set destination ip. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, DIPf, tnl_info->dip);

        /* Set source ip. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, SIPf, tnl_info->sip);

        /* Set destination subnet mask. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, DIP_MASKf,
                                   tnl_info->dip_mask);

        /* Set source subnet mask. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, SIP_MASKf,
                                   tnl_info->sip_mask);
    }

    /* Resolve number of entries hw entry occupies. */
    idx_max = (tnl_type.tnl_outer_hdr_ipv6) ? SOC_TNL_TERM_IPV6_ENTRY_WIDTH : \
              SOC_TNL_TERM_IPV4_ENTRY_WIDTH;  
    
    for (idx = 0; idx < idx_max; idx++) {
        entry_ptr = (uint32 *)&entry->entry_arr[idx];

        /* Set valid bit. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, VALIDf, 1);

        /* Set tunnel subtype. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, SUB_TUNNEL_TYPEf,
                                   tnl_type.tnl_sub_type);

        /* Set tunnel type. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, TUNNEL_TYPEf,
                                   tnl_type.tnl_auto);

        /* Set entry mode. */
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, MODEf,
                                   tnl_type.tnl_outer_hdr_ipv6);
        soc_L3_TUNNELm_field32_set(unit, entry_ptr, MODE_MASKf, 1);

        if (0 == idx) {
            /* Set the PROTOCOL field */
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, PROTOCOLf,
                                       tnl_type.tnl_protocol);
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, PROTOCOL_MASKf, 0xff);
        }
        
        if ((tnl_info->type == bcmTunnelTypeWlanWtpToAc) ||
            (tnl_info->type == bcmTunnelTypeWlanAcToAc) ||
            (tnl_info->type == bcmTunnelTypeWlanWtpToAc6) ||
            (tnl_info->type == bcmTunnelTypeWlanAcToAc6)) { 

            /* Set the L4 ports - WLAN/AMT tunnels */
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, L4_DEST_PORTf,
                                       tnl_info->udp_dst_port);

            soc_L3_TUNNELm_field32_set(unit, entry_ptr,
                                       L4_DEST_PORT_MASKf, 0xffff);

            soc_L3_TUNNELm_field32_set(unit, entry_ptr, L4_SRC_PORTf,
                                       tnl_info->udp_src_port);

            soc_L3_TUNNELm_field32_set(unit, entry_ptr,
                                       L4_SRC_PORT_MASKf, 0xffff);

            /* Set UDP tunnel type. */
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, UDP_TUNNEL_TYPEf,
                                       tnl_type.tnl_udp_type);
            /* Ignore UDP checksum */
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, IGNORE_UDP_CHECKSUMf,
   				       0x1);
        } else if (tnl_info->type == bcmTunnelTypeAutoMulticast) {

		/* Set UDP tunnel type. */
		soc_L3_TUNNELm_field32_set(unit, entry_ptr, UDP_TUNNEL_TYPEf,
					   tnl_type.tnl_udp_type);

		soc_L3_TUNNELm_field32_set(unit, entry_ptr, IGNORE_UDP_CHECKSUMf,
					   0x1);
		soc_L3_TUNNELm_field32_set(unit, entry_ptr, CTRL_PKTS_TO_CPUf,
                       0x1);
        } else if (tnl_info->type == bcmTunnelTypeAutoMulticast6) {

		/* Set UDP tunnel type. */
              soc_L3_TUNNELm_field32_set(unit, entry_ptr, UDP_TUNNEL_TYPEf,
						   tnl_type.tnl_udp_type);	
              soc_L3_TUNNELm_field32_set(unit, entry_ptr, IGNORE_UDP_CHECKSUMf,
						   0x1);
              soc_L3_TUNNELm_field32_set(unit, entry_ptr, CTRL_PKTS_TO_CPUf,
						   0x1);
        }

        /* Save vlan id for ipmc lookup.*/
        if((tnl_info->vlan) && SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, L3_IIFf)) {                 
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, L3_IIFf, tnl_info->vlan);
        }

        /* Set GRE payload */
        if (tnl_type.tnl_gre) {
            /* GRE IPv6 payload is allowed. */
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, GRE_PAYLOAD_IPV6f,
                                       tnl_type.tnl_gre_v6_payload);

            /* GRE IPv6 payload is allowed. */
            soc_L3_TUNNELm_field32_set(unit, entry_ptr, GRE_PAYLOAD_IPV4f,
                                       tnl_type.tnl_gre_v4_payload);
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_tr2_l3_tnl_term_entry_parse
 * Purpose:
 *      Parse tunnel terminator entry portion.
 * Parameters:
 *      unit     - (IN)  BCM device number. 
 *      entry    - (IN)  SOC buffer with tunne information.  
 *      tnl_info - (OUT) BCM buffer with tunnel info.
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_tr2_l3_tnl_term_entry_parse(int unit, soc_tunnel_term_t *entry,
                                  bcm_tunnel_terminator_t *tnl_info)
{
    _bcm_tnl_term_type_t tnl_type;     /* Tunnel type information.   */
    uint32 *entry_ptr;                 /* Filled entry pointer.      */
    int tunnel_id;                     /* Tunnel ID */
    int remote_port;                   /* Remote port */
    bcm_module_t mod, mod_out, my_mod; /* Module IDs */
    bcm_port_t port, port_out;         /* Physical ports */

    /* Input parameters check. */
    if ((NULL == tnl_info) || (NULL == entry)) {
        return (BCM_E_PARAM);
    }

    /* Reset destination structure. */
    sal_memset(tnl_info, 0, sizeof(bcm_tunnel_terminator_t));
    sal_memset(&tnl_type, 0, sizeof(_bcm_tnl_term_type_t));

    entry_ptr = (uint32 *)&entry->entry_arr[0];

    /* Get valid bit. */
    if (!soc_L3_TUNNELm_field32_get(unit, entry_ptr, VALIDf)) {
        return (BCM_E_NOT_FOUND);
    }

    if (SOC_MEM_FIELD_VALID(unit, L3_TUNNELm, MODEf)) {
        tnl_type.tnl_outer_hdr_ipv6 =
            soc_L3_TUNNELm_field32_get(unit, entry_ptr, MODEf);
    }
     
    /* Get Destination/Source pair. */
    if (tnl_type.tnl_outer_hdr_ipv6) {
        /* SIP [0-63] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[0], IP_ADDRf,
                             tnl_info->sip6, SOC_MEM_IP6_LOWER_ONLY);
        /* SIP [64-127] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[1], IP_ADDRf,
                             tnl_info->sip6, SOC_MEM_IP6_UPPER_ONLY);
        /* DIP [0-63] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[2], IP_ADDRf,
                             tnl_info->dip6, SOC_MEM_IP6_LOWER_ONLY);
        /* DIP [64-127] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[3], IP_ADDRf,
                             tnl_info->dip6, SOC_MEM_IP6_UPPER_ONLY);

        /* SIP MASK [0-63] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[0], IP_ADDR_MASKf,
                             tnl_info->sip6_mask, SOC_MEM_IP6_LOWER_ONLY);
        /* SIP MASK [64-127] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[1], IP_ADDR_MASKf,
                             tnl_info->sip6_mask, SOC_MEM_IP6_UPPER_ONLY);
        /* DIP MASK [0-63] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[2], IP_ADDR_MASKf,
                             tnl_info->dip6_mask, SOC_MEM_IP6_LOWER_ONLY);
        /* DIP MASK [64-127] */
        soc_mem_ip6_addr_get(unit, L3_TUNNELm, 
                             (uint32 *)&entry->entry_arr[3], IP_ADDR_MASKf,
                             tnl_info->dip6_mask, SOC_MEM_IP6_UPPER_ONLY);

    }  else {
        /* Get destination ip. */
        tnl_info->dip = soc_L3_TUNNELm_field32_get(unit, entry_ptr, DIPf);

        /* Get source ip. */
        tnl_info->sip = soc_L3_TUNNELm_field32_get(unit, entry_ptr, SIPf);

        /* Destination subnet mask. */
        tnl_info->dip_mask = BCM_XGS3_L3_IP4_FULL_MASK;

        /* Source subnet mask. */
        tnl_info->sip_mask = soc_L3_TUNNELm_field32_get(unit, entry_ptr, 
                                                        SIP_MASKf);
    }

    /* Get tunnel subtype. */
    tnl_type.tnl_sub_type = 
        soc_L3_TUNNELm_field32_get(unit, entry_ptr, SUB_TUNNEL_TYPEf);

    /* Get UDP tunnel type. */
    tnl_type.tnl_udp_type = 
        soc_L3_TUNNELm_field32_get(unit, entry_ptr, UDP_TUNNEL_TYPEf);

    /* Get tunnel type. */
    tnl_type.tnl_auto = 
        soc_L3_TUNNELm_field32_get(unit, entry_ptr, TUNNEL_TYPEf);

    /* Copy DSCP from outer header flag. */
    if (soc_L3_TUNNELm_field32_get(unit, entry_ptr, USE_OUTER_HDR_DSCPf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_USE_OUTER_DSCP;
    }
    /* Copy TTL from outer header flag. */
    if (soc_L3_TUNNELm_field32_get(unit, entry_ptr, USE_OUTER_HDR_TTLf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_USE_OUTER_TTL;
    }
    /* Keep inner header DSCP flag. */
    if (soc_L3_TUNNELm_field32_get(unit, entry_ptr,
                                   DONOT_CHANGE_INNER_HDR_DSCPf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_KEEP_INNER_DSCP;
    }

    soc_mem_pbmp_field_get(unit, L3_TUNNELm, entry_ptr, ALLOWED_PORT_BITMAPf,
                           &tnl_info->pbmp);

    /* Tunnel or IPMC lookup vlan id */
    tnl_info->vlan = soc_L3_TUNNELm_field32_get(unit, entry_ptr, IINTFf);

    /*  Get trust dscp per tunnel */ 
    if (soc_L3_TUNNELm_field32_get(unit, entry_ptr, USE_OUTER_HDR_DSCPf)) {
        tnl_info->flags |= BCM_TUNNEL_TERM_DSCP_TRUST;
    }

    /* Get the protocol field and make some decisions */
    tnl_type.tnl_protocol = soc_L3_TUNNELm_field32_get(unit, entry_ptr, 
                                                       PROTOCOLf);
    switch (tnl_type.tnl_protocol) {
        case 0x2F:
            tnl_type.tnl_gre = 1;
            break;
        case 0x67:
            tnl_type.tnl_pim_sm = 1;
        default:
            break;
    }
    /* Get gre IPv4 payload allowed. */
    tnl_type.tnl_gre_v4_payload = 
        soc_L3_TUNNELm_field32_get(unit, entry_ptr, GRE_PAYLOAD_IPV4f); 

    /* Get gre IPv6 payload allowed. */
    tnl_type.tnl_gre_v6_payload = 
        soc_L3_TUNNELm_field32_get(unit, entry_ptr, GRE_PAYLOAD_IPV6f);

    /* Get the L4 data */
    tnl_info->udp_src_port = soc_L3_TUNNELm_field32_get(unit, entry_ptr, 
                                                        L4_SRC_PORTf);
    tnl_info->udp_dst_port = soc_L3_TUNNELm_field32_get(unit, entry_ptr, 
                                                        L4_DEST_PORTf);
    /* Get the tunnel ID */
    tunnel_id = soc_L3_TUNNELm_field32_get(unit, entry_ptr, TUNNEL_IDf);
    if (tunnel_id) {
        BCM_GPORT_TUNNEL_ID_SET(tnl_info->tunnel_id, tunnel_id);
        tnl_info->flags |= BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    }

    /* Get the remote port member */
    remote_port = soc_L3_TUNNELm_field32_get(unit, entry_ptr, REMOTE_TERM_GPPf);
    mod = (remote_port >> 6) & 0x7F;
    port = remote_port & 0x3F;
    BCM_IF_ERROR_RETURN(bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET,
                        mod, port, &mod_out, &port_out));
    BCM_GPORT_MODPORT_SET(tnl_info->remote_port, mod_out, port_out);
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_mod));
    if (mod != my_mod) {
        tnl_info->flags |= BCM_TUNNEL_TERM_WLAN_REMOTE_TERMINATE;
    } 
                                
    /* Get tunnel type, sub_type and protocol. */
    BCM_IF_ERROR_RETURN
        (_bcm_xgs3_l3_get_tnl_term_type(unit, tnl_info, &tnl_type));

    return (BCM_E_NONE);
}

#else /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */
int bcm_esw_triumph2_l3_not_empty;
#endif /* BCM_TRIUMPH2_SUPPORT && INCLUDE_L3 */

