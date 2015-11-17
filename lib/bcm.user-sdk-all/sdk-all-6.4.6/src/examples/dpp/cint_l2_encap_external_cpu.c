/*
 * $Id: cint_l2_encap_external_cpu.c,v 1.2 Broadcom SDK $
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
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$SDK/src/examples/dpp/cint_mact.c
 * cint $SDK/src/examples/dpp/cint cint_l2_encap_external_cpu.c
 * 
 * execution:
 * BCM>cint
 * cint>l2_external_cpu_run_all(unit);
 * cint>exit;
 * BCM> tx 1 DATA=0x0000000000ab0000000000138100000108990123456789098765432123456789c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 PSRC=13
 */


int l2_external_cpu_run_all(int unit) {

	bcm_error_t rv;
	bcm_port_encap_config_t encap_config;
	bcm_tunnel_initiator_t l2_encap_tunnel;
    bcm_l3_intf_t tunnel_info;
    uint8 fwd_mac[6] = {0x00,0x00,0x00,0x00,0x00,0xeb}; 

	bcm_tunnel_initiator_t_init(&l2_encap_tunnel);
    bcm_l3_intf_t_init(&tunnel_info);
	/* span info */
	l2_encap_tunnel.type       = 33;/*bcmTunnelTypeL2EncapExternalCpu;*/


	rv = bcm_tunnel_initiator_create(unit, &tunnel_info , &l2_encap_tunnel);
	if(rv != BCM_E_NONE) {
	 printf("Error, create tunnel initiator \n");
	 return rv;
	}

    /*printf("tunnel id:%x\n",l2_encap_tunnel.tunnel_id);*/

	encap_config.encap = BCM_PORT_ENCAP_IEEE;
	/* example L2 header */
	encap_config.dst_mac[0]=0x23;
	encap_config.dst_mac[1]=0x45;
	encap_config.dst_mac[2]=0x67;
	encap_config.dst_mac[3]=0x89;
	encap_config.dst_mac[4]=0xAB;
	encap_config.dst_mac[5]=0xCD;

    encap_config.src_mac[0]=0x0A;
	encap_config.src_mac[1]=0x1B;
	encap_config.src_mac[2]=0x2C;
	encap_config.src_mac[3]=0x3D;
	encap_config.src_mac[4]=0x4E;
	encap_config.src_mac[5]=0x5F;
    

	encap_config.tpid=0x8100;
	encap_config.vlan=1;
	encap_config.oui_ethertype=0x8999;

	rv = bcm_port_encap_config_set(unit,l2_encap_tunnel.tunnel_id,&encap_config);
	if (rv != BCM_E_NONE) {
		return rv;
	}

    rv = bcm_port_encap_config_get(unit,l2_encap_tunnel.tunnel_id,&encap_config);
	if (rv != BCM_E_NONE) {
		return rv;
	}

    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr,fwd_mac,1);
	
	l2_addr.vid=1;
	l2_addr.port=15;
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.encap_id = tunnel_info.l3a_intf_id; /*outlif (not a gport)*/

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    }
        if (rv != BCM_E_NONE) {
             printf("Error, l2_addr_add (%d)\n",idx);
             return rv;
        }
	
	return rv;

}

