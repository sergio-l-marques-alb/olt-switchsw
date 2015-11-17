/*
 * $Id: cint_internal_ports.c,v 1.7 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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
 * Cint QOS Setup example code
 *
 * copy to /usr/local/sbin location, run bcm.user
 * Run script:
 *
   cd ../../../src/examples/dpp/
   cint cint_port_tpid.c
   cint cint_tm_fap_config2.c
   cint cint_pp_config2.c
   cint cint_internal_ports.c
   cint
   tm_config2_setup(1, 0, 0);
   tm_config2_setup(9, 1, 0);
   pp_config2_run(0, 1, 9, 1);
   int_ports_dump(0);
   recycle_port_destination_set(0, 9, 1);

   tm_config2_setup sets up hierarchy for destination port 1, no recycling
   tm_config2_setup sets up hierarcy for port 10 for recycling
   pp_config2_run sets up traditional bridge for vlan 3 port 0 mac 0x11 and recycling port mac is 0x22
   recycle_port_destination_set() sets the destination of the recycling port packets to port 1 for the recycling port 10.
 */
bcm_gport_t erp_gport;
bcm_gport_t olp_gport;

int
int_ports_dump(int unit)
{
    int rv;
    int flags;
    int int_port_count;
    int port_i;
    
    flags = BCM_PORT_INTERNAL_EGRESS_REPLICATION;

    rv = bcm_port_internal_get(unit, flags, 1, &erp_gport, &int_port_count);

    if (rv != BCM_E_NONE) {
	printf("error in bcm_port_internal_get()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
    }

    printf("ERP gport(0x%08x) port_cnt(%d)\n", erp_gport, int_port_count);

    flags = BCM_PORT_INTERNAL_OLP;
    rv = bcm_port_internal_get(unit, flags, 1, &olp_gport, &int_port_count);

    if (rv != BCM_E_NONE) {
	printf("error in bcm_port_internal_get()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
    }

    printf("OLP gport(0x%08x) port_cnt(%d)\n", olp_gport, int_port_count);

    return rv;
}

int
recycle_port_destination_set(int unit, int rcy_port, int dest_port)
{
    bcm_port_dest_info_t dest_info;
    int rv;
    int rcy_gport;
    int modid;

    rv = bcm_stk_my_modid_get(unit, &modid);
    if (rv != BCM_E_NONE) {
	    printf("error($rv) in bcm_stk_my_modid_get() for unit(%d)\n", rv);
	    return rv;
    }

    BCM_GPORT_MODPORT_SET(rcy_gport, modid, rcy_port+1);

    rv = bcm_port_force_dest_get(unit, 
		                		 rcy_gport, 
				                 &dest_info);
    
    if (rv != BCM_E_NONE) {
	    printf("error($rv) in bcm_port_force_dest_get() for gport(0x%08x)\n", 
	           rcy_gport);
	    return rv;
    }
	
    dest_info.flags = 0;
    dest_info.dp = 0;
    dest_info.priority = 0;
    
    /* dest_info.gport = tm_config2_get_flow_handle(dest_port); */
    dest_info.gport = (tm_config2_get_sysport_handle(dest_port));
    
    rv = bcm_port_force_dest_set(unit, 
				 rcy_gport, 
				 &dest_info);
    
    if (rv != BCM_E_NONE) {
        printf("error($rv) in bcm_port_force_dest_set() for gport(0x%08x)\n", rcy_gport);
        return rv;
    }
    printf("dest_info: flags(%d) pri(%d) gport(0x%08x)\n",
	   dest_info.flags, dest_info.priority, dest_info.gport);
}
