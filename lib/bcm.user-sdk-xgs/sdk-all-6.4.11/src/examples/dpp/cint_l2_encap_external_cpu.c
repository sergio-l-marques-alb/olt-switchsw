/*
 * $Id: cint_l2_encap_external_cpu.c,v 1.2 Broadcom SDK $
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
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$SDK/src/examples/dpp/cint_mact.c
 * cint $SDK/src/examples/dpp/cint cint_l2_encap_external_cpu.c
 * 
 * execution:
 * BCM>cint
 * cint>l2_external_cpu_uc_forwarding(unit);
 * cint>exit;
 * for uc send
 * BCM> tx 1 DATA=0x0000000000eb0000000000138100000108990123456789098765432123456789c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 PSRC=13
 * the added l2 header will be 23456789abcd0a1b2c3d4e5f810050018999+system headers
 * 
 * for uc ingress trap
 * execution:
 * BCM>cint
 * cint>l2_external_cpu_uc_trap_set(unit);
 * cint>exit;
 * for uc send (trap for same DA and SA)
 * BCM> tx 1 DATA=0x0000000000eb00x0000000000eb8100000108990123456789098765432123456789c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 PSRC=13
 * the added l2 header will be 23456789abcd0a1b2c3d4e5f810050018999+system headers
 *
 * 
 * for mc send
 * BCM>cint
 * cint>l2_external_cpu_mc_forwarding(unit);
 * cint>exit;
 * BCM> tx 1 DATA=0xff00000000eb0000000000138100000108990123456789098765432123456789c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 PSRC=13
 * the added l2 header will be 23456789abcd0a1b2c3d4e5f810050018999+system headers
 */


int l2_external_cpu_uc_forwarding(int unit) {

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

    printf("tunnel id:%x\n",l2_encap_tunnel.tunnel_id);

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
	l2_addr.port=1;
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.encap_id = tunnel_info.l3a_intf_id; /*outlif (not a gport)*/

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
             printf("Error, l2_addr_add (%d)\n",idx);
             return rv;
        }
	
	return rv;

}

int l2_external_cpu_uc_trap_set(int unit) {

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

    printf("tunnel ids  global:%x  local:%x\n",l2_encap_tunnel.tunnel_id,tunnel_info.l3a_intf_id);

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

    int pcp=3;
    int dei=0;
    encap_config.tos=pcp<<1|dei;

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
	l2_addr.port=201;/*the trap should override this destination*/
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

    bcm_rx_trap_config_t config;
    int flags=0;
    int trap_id=0;

	sal_memset(&config, 0, sizeof(config));

	/*for port dest change*/
	config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_ENCAP_ID;

	config.trap_strength = 7; /*FOR USER DEFINE ONLY !!! */


	config.dest_port=13;
    config.encap_id=tunnel_info.l3a_intf_id;

	config.snoop_cmnd = 0; 
	config.snoop_strength = 0; 


	rv = bcm_rx_trap_type_create(unit,flags,bcmRxTrapSaEqualsDa,&trap_id);
	if (rv != BCM_E_NONE) {
		printf("Error, in trap create, trap id $trap_id \n");
		return rv;
	}
	

	rv = bcm_rx_trap_set(unit,trap_id,&config);
	if (rv != BCM_E_NONE) {
		printf("Error, in trap set \n");
		return rv;
	}
	
	return rv;

}

int l2_external_cpu_mc_forwarding(int unit) {

	bcm_error_t rv;
	bcm_port_encap_config_t encap_config;
	bcm_tunnel_initiator_t l2_encap_tunnel;
    bcm_l3_intf_t tunnel_info;
    uint8 fwd_mac[6] = {0xff,0x00,0x00,0x00,0x00,0xeb}; 

	bcm_tunnel_initiator_t_init(&l2_encap_tunnel);
    bcm_l3_intf_t_init(&tunnel_info);
	/* span info */
	l2_encap_tunnel.type       = 33;/*bcmTunnelTypeL2EncapExternalCpu;*/


	rv = bcm_tunnel_initiator_create(unit, &tunnel_info , &l2_encap_tunnel);
	if(rv != BCM_E_NONE) {
	 printf("Error, create tunnel initiator \n");
	 return rv;
	}

    printf("tunnel id:%x\n",l2_encap_tunnel.tunnel_id);

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

	bcm_multicast_t mc_group = 111;
	int flags;
	flags =  BCM_MULTICAST_INGRESS_GROUP | BCM_MULTICAST_WITH_ID ;
	rv = bcm_multicast_destroy(unit, mc_group);
	if (rv != BCM_E_NONE) {
        printf("Error, in mc destroy,  mc_group $mc_group \n");
        return rv;
    }
    rv = bcm_multicast_create(unit, flags, &mc_group);
    if (rv != BCM_E_NONE) {
        printf("Error, in mc create, flags $flags mc_group $mc_group \n");
        return rv;
    }

	printf("mc group created %d, \n",mc_group);

    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr,fwd_mac,1);
	
	l2_addr.vid=1;
	l2_addr.port=1;
	l2_addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    l2_addr.l2mc_group = mc_group;
    l2_addr.encap_id = tunnel_info.l3a_intf_id; /*outlif (not a gport)*/

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    }

    bcm_gport_t gport;
    BCM_GPORT_LOCAL_SET(gport, l2_addr.port); 
    rv = bcm_multicast_ingress_add(unit, mc_group, gport, tunnel_info.l3a_intf_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in mc ingress add, mc_group $mc_group dest_gport $dest_gport \n");
        return rv;
    }

	
	return rv;

}

int l2_external_cpu_sanitiy_check(int unit) {

	bcm_error_t rv;
	bcm_port_encap_config_t encap_config;
	bcm_tunnel_initiator_t l2_encap_tunnel;
    bcm_l3_intf_t tunnel_info;

	bcm_tunnel_initiator_t_init(&l2_encap_tunnel);
    bcm_l3_intf_t_init(&tunnel_info);
	l2_encap_tunnel.type       = bcmTunnelTypeL2EncapExternalCpu;


	rv = bcm_tunnel_initiator_create(unit, &tunnel_info , &l2_encap_tunnel);
	if(rv != BCM_E_NONE) {
	 printf("Error, create tunnel initiator \n");
	 return rv;
	}


    rv = bcm_tunnel_initiator_clear(unit, &tunnel_info);
	if(rv != BCM_E_NONE) {
	 printf("Error, create tunnel initiator \n");
	 return rv;
	}

    bcm_tunnel_initiator_t_init(&l2_encap_tunnel);
    bcm_l3_intf_t_init(&tunnel_info);
	/* span info */
	l2_encap_tunnel.type       = 33;/*bcmTunnelTypeL2EncapExternalCpu;*/

    l2_encap_tunnel.tunnel_id=0x0f000;
    l2_encap_tunnel.flags|=BCM_TUNNEL_WITH_ID;

	rv = bcm_tunnel_initiator_create(unit, &tunnel_info , &l2_encap_tunnel);
	if(rv != BCM_E_NONE) {
	 printf("Error, create tunnel initiator \n");
	 return rv;
	}

    rv = bcm_tunnel_initiator_clear(unit, &tunnel_info);
	if(rv != BCM_E_NONE) {
	 printf("Error, create tunnel initiator \n");
	 return rv;
	}

    return rv;
}


int l2_external_cpu_uc_run_all(int unit) {

	int rv=BCM_E_NONE;

    rv=l2_external_cpu_sanitiy_check(unit);
	if (rv != BCM_E_NONE) {
        printf("Error, l2_external_cpu_sanitiy_check \n");
        print rv;
        return rv;
    }

	rv = l2_external_cpu_uc_trap_set(unit);
	if (rv != BCM_E_NONE) {
        printf("Error, l2_external_cpu_uc_trap_set \n");
        print rv;
        return rv;
    }
	
	return rv;

}

