/* $Id: cint_mpls_lif_counter.c, 2015/09/21 huilizha Exp $
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
* 
* File: cint_mpls_lif_counter.c
* Purpose: An example of the mpls lif counter.

* There are totally 9 statistics modes:
    ----------------------------------------------------------------------------------------------------------------------------------
    |stat_mode           | counting pkts                                                                                             |
    ----------------------------------------------------------------------------------------------------------------------------------
    |all                 | total pkts                                                                                                |                   
    |fwd                 | enqueued pkts                                                                                             |
    |drop                | discarded pkts                                                                                            |
    |fwd_drop            | enqueued and discarded pkts                                                                               |
    |simple_color        | green pkts, not green pkts                                                                                |
    |simple_color_fwd    | enqueued green pkts, enqueued nongreen pkts                                                               |
    |simple_color_drop   | discarded green pkts, discarded nongreen pkts                                                             |
    |green_not_green     | enqueued green pkts, discarded green pkts, enqueued nongreen pkts, discarded nongreen pkts                |
    |full_color          | enqueued green pkts, discarded green pkts, enqueued yellow pkts, discarded yellow pkts, discarded red pkts|
    ----------------------------------------------------------------------------------------------------------------------------------

* SOC property:    
    ---------------------------------------------------------------------------------------------------------
    |        init soc	                               |                 term soc                           |
    ---------------------------------------------------------------------------------------------------------
    |counter_engine_source_0.0=EGRESS_OUT_LIF_0_0_0    |      counter_engine_source_0.0=INGRESS_IN_LIF_0_0_0|
    |counter_engine_source_1.0=EGRESS_OUT_LIF_0_0_1    |      counter_engine_source_1.0=INGRESS_IN_LIF_0_0_1|
    |counter_engine_source_2.0=EGRESS_OUT_LIF_0_0_2    |      counter_engine_source_2.0=INGRESS_IN_LIF_0_0_2|
    |counter_engine_source_3.0=EGRESS_OUT_LIF_0_0_3    |      counter_engine_source_3.0=INGRESS_IN_LIF_0_0_3|
    |counter_engine_statistics_0.0=$mode               |      counter_engine_statistics_0.0=$mode           |
    |counter_engine_statistics_1.0=$mode               |      counter_engine_statistics_1.0=$mode           |
    |counter_engine_statistics_2.0=$mode               |      counter_engine_statistics_2.0=$mode           |
    |counter_engine_statistics_3.0=$mode	       |      counter_engine_statistics_3.0=$mode           |
    ---------------------------------------------------------------------------------------------------------

* mpls service: 
  Initiator:
    *  Default Example, Set MPLS LSR to Outgoing Tunnels B (1001) ,A (1000)
    *  Send Packet:
    *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}
    *  VID = 200 
    *  MPLS label 6000
    *  Expected Packet:
    *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44}
    *  SA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33}
    *  MPLS label 9000, MPLS label 1001, MPLS label 1000
    *   Sending packet from in_port: 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+    |
    *   |    | DA |SA||TIPD1 |Prio|VID||   MPLS   || Data |    |
    *   |    |0:33|  ||0x8100|    |200||Label:6000||      |    | 
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+    |
    *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+   |
    *   |   | Figure 1: Sending Packet from in_port   |   | 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
    *
    * * The packet will be received in out_port with following header 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+--+    |
    *   |    |DA    | SA ||   MPLS   ||MPLS      ||MPLS      || Data |    |
    *   |    |0:44  |0:33||Label:1000||Label:1001||Label:9000||      |    | 
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+--+    |
    *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
    *   |   |         Figure 2: Packets Received on out_port          |   | 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
  Terminator:
    *  Send Packet:
    *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11}
    *  VID = 100 
    *  MPLS label 4096 MPLS label 5000
    *  Expected Packet:
    *  DA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x22}
    *  SA = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11}
    *  MPLS label 8000
    *   Sending packet from in_port: 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+--+    |
    *   |    |DA    | SA ||   MPLS   ||MPLS      || Data |    |
    *   |    |0:22  |0:11||Label:4096||Label:5000||           |      |    | 
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+--+    |
    *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+
    *   |   |         Figure 3: Sending Packet from in_port           |   | 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
    
    * * The packet will be received in out_port with following header 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+    |
    *   |    | DA | SA ||   MPLS   || Data |    |
    *   |    |0:22|0:11||Label:8000||      |    | 
    *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+    |
    *   |   +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+   |
    *   |   | Figure 4: Packets Received on out_port       |   | 
    *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

* Calling sequence:
  --------------------------------------------------------
  |                 The example of fwd                   |
  --------------------------------------------------------
    cd ../../../../
    cint src/examples/dpp/utility/cint_utils_global.c
    cint src/examples/dpp/utility/cint_utils_l3.c
    cint src/examples/dpp/utility/cint_utils_l2.c
    cint src/examples/dpp/utility/cint_utils_mpls.c
    cint src/examples/dpp/cint_qos.c
    cint src/examples/dpp/cint_multi_device_utils.c
    cint src/examples/dpp/cint_mpls_lsr.c
    cint src/examples/dpp/cint_mpls_tunnel_initiator.c
    cint src/examples/dpp/cint_mpls_lif_counter.c
    cint
    print mpls_lif_counter_set(unit, fwd, init);
    ******** send the packet in Figure1 *******
    print mpls_lif_counter_get(unit, fwd, init);
  --------------------------------------------------------
  |         The example of simple_color_drop             |
  --------------------------------------------------------
    ./bcm.user
    cd ../../../../
    cint src/examples/dpp/utility/cint_utils_global.c
    cint src/examples/dpp/utility/cint_utils_l3.c
    cint src/examples/dpp/utility/cint_utils_l2.c
    cint src/examples/dpp/utility/cint_utils_mpls.c
    cint src/examples/dpp/cint_qos.c
    cint src/examples/dpp/cint_multi_device_utils.c
    cint src/examples/dpp/cint_mpls_lsr.c
    cint src/examples/dpp/cint_mpls_tunnel_initiator.c
    cint src/examples/dpp/cint_mpls_lif_counter.c
    cint
    print mpls_lif_counter_set(unit, simple_color_drop, term);
    ******** send the packet in Figure3 *******
    print mpls_lif_counter_get(unit, simple_color_drop, term);
    
*/

int unit = 0;
int units_ids[1] = {0};
int nof_units = 1;
bcm_port_t in_port = 13;
bcm_port_t out_port = 14;

/* the inlif gport and outlif gport */
bcm_gport_t inlif_gport;
bcm_gport_t outlif_gport;

/* start_lif, end_lif, lif_counting_profile */
int start_lif;
int end_lif;
int lif_counting_profile;

/* all kinds of stat_mode */
typedef enum stat_format_t {
    fwd,
    drop,
    fwd_drop,
    simple_color,
    simple_color_fwd,
    simple_color_drop,
    green_not_green,
    full_color,
    all
};

/* mpls initiator or terminator */
typedef enum mpls_init_or_term_t {
    init,
    term
};

/* set the counting source and lif range of lif counting profile */
int lif_counter_set(int unit, stat_format_t stat_format, mpls_init_or_term_t init_or_term) 
{
	bcm_stat_lif_counting_t lif_counting;
	int lif_stack_level;
	int priority;
    int count_source;
    int rv;
    
    switch (init_or_term) {
        case init:
            count_source = bcmStatCounterSourceEgressTransmitOutlif;
            lif_stack_level = bcmStatCounterLifStackId0;
            start_lif = 8*1024;
            end_lif = 9*1024-1;
            lif_counting_profile = 0;
            break;
        case term:
            count_source = bcmStatCounterSourceIngressInlif;
            lif_stack_level = bcmStatCounterLifStackId1;
            start_lif = 32*1024;
            end_lif = 64*1024;
            lif_counting_profile = 2;
            break;
        default:
            return BCM_E_PARAM;
    }
    
    /* lif counting source. */
    lif_counting.source.type = count_source;
	lif_counting.source.command_id = 0;
    
    /* a range of lif IDs. */
    lif_counting.range.start = start_lif;
    lif_counting.range.end = end_lif;
    
    /* mapping a lif-couting-profile to a counting-source and lif-counting-range. */
	rv = bcm_stat_lif_counting_profile_set(unit, 0, lif_counting_profile, &lif_counting);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stat_lif_counting_profile_set $rv\n");
        return rv;
    }

	priority = 1; /* we are couting the second LIF on the stack - set it to a higher priority */
	rv = bcm_stat_lif_counting_stack_level_priority_set(unit, 0, &lif_counting.source, lif_stack_level, priority);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stat_lif_counting_stack_level_priority_set $rv\n");
        return rv;
    }
    
    return rv;
}

/* set up the mpls service and get the inlif_gport or outlif_gport */
int mpls_tunnel_set(int unit, mpls_init_or_term_t init_or_term) 
{
    bcm_gport_t in_modport, in_sysport;
    bcm_gport_t out_modport, out_sysport;
    int term_label = 0x1000;
    int pipe_mode_exp_set = 0;
    int exp_set = 2;
    int rv;
    
    switch (init_or_term) {
    case init:
        outlif_counting_profile = lif_counting_profile;
        rv = mpls_tunnel_initiator_run_with_defaults_aux(unit, -1, in_port, out_port, pipe_mode_exp_set, exp_set);
        if (rv != BCM_E_NONE) {
            printf("Error, mpls_tunnel_initiator_run_with_defaults_aux $rv\n");
            return rv;
        }
    
        outlif_gport = outlif_to_count;
        printf("The tunnel outlif_gport is %d\n",outlif_gport);
        
        break;
    case term:
        rv = bcm_port_gport_get(unit, in_port, &in_modport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_gport_get $rv\n");
            return rv;
        }
    
        rv = bcm_port_gport_get(unit, out_port, &out_modport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_gport_get $rv\n");
            return rv;
        }
    
        rv = bcm_stk_gport_sysport_get(unit, in_modport, &in_sysport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_stk_gport_sysport_get $rv\n");
            return rv;
        }
    
        rv = bcm_stk_gport_sysport_get(unit, out_modport, &out_sysport);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_stk_gport_sysport_get $rv\n");
            return rv;
        }
    
        rv = mpls_lsr_run_with_defaults_multi_device(units_ids, nof_units, in_sysport, out_sysport);
        if (rv != BCM_E_NONE) {
            printf("Error, mpls_lsr_run_with_defaults_multi_device $rv\n");
            return rv;
        }
		
        inlif_counting_profile = lif_counting_profile;
        rv = mpls_add_term_entry(unit, term_label, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, mpls_add_term_entry $rv\n");
            return rv;
        }
    
        inlif_gport = ingress_tunnel_id;
        printf("The tunnel inlif_gport is %d\n",inlif_gport);

        break;
    default:
        return BCM_E_PARAM;
    }
    
    return rv;
}

int mpls_lif_counter_set(int unit, stat_format_t stat_format, mpls_init_or_term_t init_or_term)   
{
    int rv;
    bcm_gport_t local_gport = 0;   
    
    rv = lif_counter_set(unit, stat_format, init_or_term);
    if (rv != BCM_E_NONE) {
        printf("Error, lif_counter_set $rv\n");
        return rv;
    }
  
    if (stat_format == drop || stat_format == fwd_drop) {
        /* Shape related */ 
        BCM_GPORT_LOCAL_SET(local_gport, out_port);
        rv = bcm_cosq_gport_bandwidth_set(unit, local_gport, 0, 0, 1000000, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_bandwidth_set $rv\n");
            return rv;
        }
    } else if (stat_format == simple_color || stat_format == simple_color_fwd) {
        /* qos related */
        rv = qos_map_mpls_ingress_profile(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, qos_map_mpls_ingress_profile $rv\n");
            return rv;
        }
    } else if (stat_format == simple_color_drop || stat_format == green_not_green || stat_format == full_color) {
        /* Shape related */ 
        BCM_GPORT_LOCAL_SET(local_gport, out_port);
        rv = bcm_cosq_gport_bandwidth_set(unit, local_gport, 0, 0, 1000000, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_gport_bandwidth_set $rv\n");
            return rv;
        }
            
        /* qos related */
        rv = qos_map_mpls_ingress_profile(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, qos_map_mpls_ingress_profile $rv\n");
            return rv;
        }
    }
    
    rv = mpls_tunnel_set(unit, init_or_term);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_tunnel_set $rv\n");
        return rv;
    }
    
    return rv;
}

int mpls_lif_counter_get(int unit, stat_format_t stat_format, mpls_init_or_term_t init_or_term)  
{
    int counter_proc;
	int stat_id = 0;
    int local_lif;
	uint32 numOfpackets = 0;
    int set_size = 0; /* FWD : see search SOC_TMC_CNT_MODE_STATISTICS_FWD_NO_COLOR in _bcm_dpp_counter_mode_config */
    int index;
    int stats[12]= {bcmFieldStatAcceptedPackets, bcmFieldStatDroppedPackets, 
                                bcmFieldStatGreenPackets, bcmFieldStatNotGreenPackets, 
                                bcmFieldStatAcceptedGreenPackets, bcmFieldStatAcceptedNotGreenPackets,
                                bcmFieldStatDroppedGreenPackets, bcmFieldStatDroppedNotGreenPackets,
                                bcmFieldStatAcceptedYellowPackets, bcmFieldStatDroppedYellowPackets, bcmFieldStatDroppedRedPackets,
                                bcmFieldStatPackets};
    int rv;

    if (stat_format == fwd || stat_format == drop || stat_format == all) {
        set_size = 1;
    } else if (stat_format == fwd_drop || stat_format == simple_color || stat_format == simple_color_fwd || stat_format == simple_color_drop) {
        set_size = 2;
    } else if (stat_format == green_not_green) {
        set_size = 4;
    } else if (stat_format == full_color) {
        set_size = 5;
    }
    
    /* Get local Encap id per Gport */
	switch (init_or_term) 
    {
        case init:	
            rv = bcm_port_encap_local_get(unit, outlif_gport, BCM_PORT_ENCAP_LOCAL_EGRESS, &local_lif);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_encap_local_get $rv\n");
                return rv;
            }
            
            counter_proc = (local_lif - start_lif)*set_size / SOC_DPP_DEFS_GET_COUNTERS_PER_COUNTER_PROCESSOR(unit);   
            break;
        case term:
            rv = bcm_port_encap_local_get(unit, inlif_gport, BCM_PORT_ENCAP_LOCAL_INGRESS, &local_lif);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_port_encap_local_get $rv\n");
                return rv;
            }
            
            counter_proc = (local_lif - start_lif)*set_size / SOC_DPP_DEFS_GET_COUNTERS_PER_COUNTER_PROCESSOR(unit);   
            break;
        default:
            return BCM_E_PARAM;
    }
    
     
    BCM_FIELD_STAT_ID_SET(stat_id, counter_proc, local_lif);
    
    printf("The local_lif is %d\n",local_lif);     
    printf("The counter_proc is %d\n",counter_proc); 
    printf("The stat_id is %d\n",stat_id); 
    
    /* get 32-bit counter value for specific statistic type */
    switch (stat_format) {
    case fwd:
        rv = bcm_field_stat_get32(unit, stat_id, stats[0], &numOfpackets);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_stat_get32 $rv\n");
            return rv;
        }
        printf("%d\n",numOfpackets); 
        break;
    case drop:
        rv = bcm_field_stat_get32(unit, stat_id, stats[1], &numOfpackets);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_stat_get32 $rv\n");
            return rv;
        }
        printf("%d\n",numOfpackets);  
        break;
    case fwd_drop:
        for (index = 0; index < 2; index ++) {
            rv = bcm_field_stat_get32(unit, stat_id, stats[index], &numOfpackets);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_field_stat_get32 $rv\n");
                return rv;
            }
            printf("%d\n",numOfpackets); 
        }
        break;
    case simple_color:
        for (index = 2; index < 4; index ++) {
            rv = bcm_field_stat_get32(unit, stat_id, stats[index], &numOfpackets);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_field_stat_get32 $rv\n");
                return rv;
            }
            printf("%d\n",numOfpackets); 
        }
        break;
    case simple_color_fwd:
        for (index = 4; index < 6; index ++) {
            rv = bcm_field_stat_get32(unit, stat_id, stats[index], &numOfpackets);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_field_stat_get32 $rv\n");
                return rv;
            }
            printf("%d\n",numOfpackets); 
        }
        break;
    case simple_color_drop:
        for (index = 6; index < 8; index ++) {
            rv = bcm_field_stat_get32(unit, stat_id, stats[index], &numOfpackets);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_field_stat_get32 $rv\n");
                return rv;
            }
            printf("%d\n",numOfpackets); 
        }
        break;
    case green_not_green:
        for (index = 4; index < 8; index ++) {
            rv = bcm_field_stat_get32(unit, stat_id, stats[index], &numOfpackets);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_field_stat_get32 $rv\n");
                return rv;
            }
            printf("%d\n",numOfpackets); 
        }
        break;
    case full_color:
        for (index = 6; index < 11; index ++) {
            rv = bcm_field_stat_get32(unit, stat_id, stats[index], &numOfpackets);
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_field_stat_get32 $rv\n");
                return rv;
            }
            printf("%d\n",numOfpackets); 
        }
        break;
    case all:
        rv = bcm_field_stat_get32(unit, stat_id, stats[11], &numOfpackets);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_field_stat_get32 $rv\n");
            return rv;
        }
        printf("%d\n",numOfpackets); 
        break;
    default:
        return BCM_E_PARAM;
    }
        
    return rv;
}
