/*
 * $Id: tunnel.h,v 1.5 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * File:        tunnel.h
 * Purpose:     tunnel internal definitions to the BCM library.
 */

#ifndef   _BCM_INT_DPP_TUNNEL_H_
#define   _BCM_INT_DPP_TUNNEL_H_


/* tunnel subtype.
   For example, for a tunnel type bcmTunnelTypeL2gre, subtype l2gre_is_erspan indicate we're building an IP tunnel for ERPSAN
                                                      subtype none indicate we're building an L2gre tunnel  */
typedef enum _bcm_tunnel_subtype_e {
    _bcm_tunnel_subtype_none = 0,
    _bcm_tunnel_subtype_l2gre_is_erspan = 1 /* IP_Tunnel.size will include erspan */

} _bcm_tunnel_subtype_t;



int _bcm_ip_tunnel_sw_init(int unit);
int _bcm_ip_tunnel_sw_cleanup(int unit);

int bcm_petra_tunnel_initiator_data_set(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel,
    int vsi_param,
    int ll_eep_param,
    int *tunnel_eep
    ); /* needed only in case intf is of type EEP */

int
_bcm_petra_tunnel_initiator_set(
    int unit, 
    bcm_l3_intf_t *intf, 
    bcm_tunnel_initiator_t *tunnel
    ); 

int 
_bcm_petra_tunnel_terminator_add(
    int unit, 
    bcm_tunnel_terminator_t *info) ;

int
_bcm_dpp_in_lif_tunnel_term_get(int unit, bcm_tunnel_terminator_t *tunnel_term, int lif);

/* see _bcm_petra_tunnel_ipv4_type_to_ppd comments */
int
_bcm_petra_tunnel_ipv4_type_from_ppd(int unit, 
                                     SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *ipv4_encap_info, 
                                     bcm_tunnel_type_t *tunnel_type, 
                                     _bcm_tunnel_subtype_t *tunnel_subtype);

/* 
 * resolve the ppd tunnel type to use. 
 *  
 * For Jericho A0 and below, tunnel type is bcmTunnelType.
 * For Jericho B0 and above, tunnel type is composed of bcmTunnelType and tunnel_subtype. 
 * In most case there are no tunnel_subtype. 
 * Ex:  
 * For VxLAN:  bcmTunnelType: bcmTunnelTypeVxlan and tunnel_subtype: _bcm_tunnel_subtype_none
 * For Erspan: bcmTunnelType: bcmTunnelTypeL2Gre and tunnel_subtype: _bcm_tunnel_subtype_l2gre_is_erspan. 
 *             tunnel_subtype indicates that the IP_Tunnel.size should include erspan. 
 */   
int 
_bcm_petra_tunnel_ipv4_type_to_ppd(int unit, 
                                   bcm_tunnel_type_t tunnel_type, 
                                   _bcm_tunnel_subtype_t tunnel_subtype, 
                                   SOC_PPC_EG_ENCAP_IPV4_ENCAP_INFO *ipv4_encap_info, 
                                   uint8 is_replace); 

#endif /* _BCM_INT_DPP_TUNNEL_H_ */
