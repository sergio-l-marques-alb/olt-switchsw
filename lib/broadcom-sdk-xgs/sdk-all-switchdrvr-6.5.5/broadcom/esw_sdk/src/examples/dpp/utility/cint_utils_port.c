/* $Id: cint_utils_port.c,v 1.10 2013/02/03 10:59:10 	Mark Exp $
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
 * This file provides port basic functionality
 */


/* Set VLAN domain */
int port__vlan_domain__set(int unit, bcm_port_t port, int vlan_domain){

    return bcm_port_class_set(unit, port, bcmPortClassId, vlan_domain);
}

int port__tpid__set(int unit, bcm_port_t port, uint16 outer_tpid, uint16 inner_tpid)
{
    int rv;

    /* Set outer tpid */
    rv = bcm_port_tpid_set(unit, port, outer_tpid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_set, port=%d, \n",  port);
        return rv;
    }

    if(inner_tpid != 0)
    {
        /* Set inner TPID on port */
        rv = bcm_port_inner_tpid_set(unit, port, inner_tpid);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_port_inner_tpid_set, port=%d, \n",  port);
            return rv;
        }
    }

    return rv;
}

int port__basic_tpid_class__set(int unit, bcm_port_t port)
{
    bcm_port_tpid_class_t port_tpid_class;
    int rv;

    bcm_port_tpid_class_t_init(&port_tpid_class);
    sal_memset(&port_tpid_class, 0, sizeof (port_tpid_class));

    port_tpid_class.port  = port;
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = 2;  /* Set tag format to '2' for all TPID types  */
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    bcm_port_tpid_class_t_init(&port_tpid_class);
    sal_memset(&port_tpid_class, 0, sizeof (port_tpid_class));

    port_tpid_class.port  = port;
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 1;  /* Set tag format to '1' for all TPID types with one TPID  */
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }
	
    bcm_port_tpid_class_t_init(&port_tpid_class);
    sal_memset(&port_tpid_class, 0, sizeof (port_tpid_class));

    port_tpid_class.port  = port;
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = 1;  /* Set tag format to '1' for all TPID types with one TPID  */
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }


    bcm_port_tpid_class_t_init(&port_tpid_class);
    sal_memset(&port_tpid_class, 0, sizeof (port_tpid_class));

    port_tpid_class.port  = port;
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    return rv;
}



/* Set port TPID */
int port__default_tpid__set(int unit, bcm_port_t port){
    int rv;

    port_tpid_init(port, 1, 1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set with port %d\n", port);
        print rv;
    }
    return rv;
}


/* Reset the TPIDs for a port
 * Deletes any previously defined TPIDs for the port and sets the user supplied TPIDs.
 *
 * INPUT: 
 *   port: Physical port or a PWE gport
 *   outer_tpid: Outer TPID value
 *   inner_tpid: Inner TPID value
 */
int port__tpids__set(int unit,
                     bcm_port_t port,
                     uint32 outer_tpid,
                     uint32 inner_tpid)
{
    int rv;
     
    /* Remove possible old TPIDs from the port */
    rv = bcm_port_tpid_delete_all(unit, port);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_delete_all, port - %d, rv - %d\n", port, rv);
        return rv;
    }
    
    /* Set the outer TPID of the port */
    rv = bcm_port_tpid_set(unit, port, outer_tpid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_set, port - %d, outer_tpid - %d, rv - %d\n", port, outer_tpid, rv);
        return rv;
    }
    
    /* Set the inner TPID of the port */
    rv = bcm_port_inner_tpid_set(unit, port, inner_tpid);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_set, port - %d, inner_tpid - %d, rv - %d\n", port, inner_tpid, rv);
        return rv;
    }

    return BCM_E_NONE;
}


/* Classify a Ports combination of TPIDs to a specific tag format.
 * Applicable only in AVT mode. 
 *  
 * INPUT: 
 *   port: Physical port or a PWE gport
 *   tag_format: Tag format.
 *   outer_tpid: Outer TPID value
 *   inner_tpid: Inner TPID value
 */
int port__tag_classification__set(int unit,
                                  bcm_port_t port,
                                  bcm_port_tag_format_class_t tag_format,
                                  uint32 outer_tpid,
                                  uint32 inner_tpid)
{
    int rv;
    bcm_port_tpid_class_t tpid_class;

    bcm_port_tpid_class_t_init(&tpid_class);

    tpid_class.flags = 0;   /* Both for Ingress and Egress */
    tpid_class.port = port;
    tpid_class.tag_format_class_id = tag_format;
    tpid_class.tpid1 = outer_tpid;
    tpid_class.tpid2 = inner_tpid;
    rv = bcm_port_tpid_class_set(unit, &tpid_class);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_port_tpid_class_set, port - %d, rv - %d\n", port, rv);
        return rv;
    }

    return BCM_E_NONE;
}


int cint_ire_nif_shaper_drops_set(int unit)
{
    int rv = BCM_E_NONE;
    int is_jer = 0, is_qmx = 0;
    bcm_info_t info;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_info_get\n", rv);
        return rv;
    }

    if(info.device == 0x8675 || info.device == 0x8680) {
        is_jer = 1;
    }

    if(info.device == 0x8375) {
        is_qmx = 1;
    }

    /* artificially create pressure in the NIF buffers */
    if (is_jer || is_qmx) {
        bshell(unit, "m IRE_NETWORK_INTERFACE_SHAPER NIF_N_SHAPER_TIMER_CYCLES=1 NIF_N_SHAPER_GRANT_SIZE=2 NIF_N_SHAPER_MAX_BURST=0x101");
    }
    else {
        bshell(unit, "m IRE_NETWORK_INTERFACE_SHAPER NIF_SHAPER_TIMER_CYCLES=1 NIF_SHAPER_GRANT_SIZE=2 NIF_SHAPER_MAX_BURST=0x101");
    }

    return BCM_E_NONE;
}

int cint_nif_priority_config(int unit, int in_port,
                             int lp_pcp, int low_priority,
                             int hp_pcp, int high_priority, uint32 tpid, uint32 header)
{

    int rv = BCM_E_NONE;
    uint32 flags = 0;
    int lp_key, hp_key;
    int cosq_map;
    bcm_info_t info;

    rv = bcm_info_get(unit, &info);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_info_get\n", rv);
        return rv;
    }

    /* Configure the NIF ports priority drop*/

    /* Header type legend
    0 - VLAN 
    1 - ITMH 
    2 - HIGIG 
    3 - IP 
    4 - MPLS  
    */ 

    /*Depending on the header we chose also and the map type 
    bcmCosqIngressPortDropTmTcDpPriorityTable = 0, 
    bcmCosqIngressPortDropIpDscpToPriorityTable = 1, 
    bcmCosqIngressPortDropEthPcpDeiToPriorityTable = 2, 
    bcmCosqIngressPortDropMplsExpToPriorityTable = 3 
    }*/

    /* Choose the right key constructor macro for the specified header type and the right COSQ table*/
    if (header == 0) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(lp_pcp,0);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_ETH_MAP_KEY(hp_pcp,0);
        cosq_map = 2;
    } else if (header == 1 || header == 2) {
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY(lp_pcp,0);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_TM_MAP_KEY(hp_pcp,0);
        cosq_map = 0;
    } else if (info.device == 0x8680 && header == 3) {
        /*Check if device is Jericho Plus and header type IP*/
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY(lp_pcp);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_IP_MAP_KEY(hp_pcp);
        cosq_map = 1;
    } else if (info.device == 0x8680 && header == 4) {
        /*Check if device is Jericho Plus and header type MPLS*/
        lp_key = BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY(lp_pcp);
        hp_key = BCM_COSQ_INGRESS_PORT_DROP_MPLS_MAP_KEY(hp_pcp);
        cosq_map = 3;
    } else {
        printf("ERROR: Incorrect value for header type %d", header);
        return BCM_E_PARAM;
    }

    /*Map the created lp_key to low priority*/
    rv = bcm_cosq_ingress_port_drop_map_set(unit,in_port,flags,cosq_map,lp_key,low_priority);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_map_set\n", rv);
        return rv;
    }

    /*Map the created hp_key to high priority*/
    rv = bcm_cosq_ingress_port_drop_map_set(unit,in_port,flags,cosq_map,hp_key,high_priority);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_map_set\n", rv);
        return rv;
    }

    /*Set TPID for the port
    rv = bcm_cosq_control_set(unit,in_port,-1,bcmCosqControlIngressPortDropTpid1,tpid);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_control_set\n", rv);
        return rv;
    }
*/
    /*Set 1 byte as a threshold level for the FIFO for packets mapped to low_priority*/
    rv = bcm_cosq_ingress_port_drop_threshold_set(unit,in_port,flags,low_priority,1);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_threshold_set\n", rv);
        return rv;
    }

    /*Return traffic back to the IXIA to get stream statistics*/
    rv = bcm_port_force_forward_set(unit,in_port,in_port,1);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_port_force_forward_set\n", rv);
        return rv;
    }

    /*Enable the NIF PRD feature for the given port!
      If device is JER+ enable hard mode!*/
    rv = bcm_cosq_ingress_port_drop_enable_set(unit, in_port, flags, 1);  
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_cosq_ingress_port_drop_enable_set\n", rv);
        return rv;
    }

    printf("cint_bcm_NIF_priority_config: PASS\n\n");
    return BCM_E_NONE;

}

uint64 cint_port_dropped_packets_get (int unit, int port) {
    int rv = BCM_E_NONE;
    uint64 dropped_cnt;

    rv = bcm_stat_get(unit,port,snmpEtherStatsDropEvents,&dropped_cnt); 
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) bcm_stat_get \n", rv);
        return rv;
    }

    printf("Dropped packets number %d \n", dropped_cnt);

    return dropped_cnt;
}


